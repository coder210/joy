#define SDL_MAIN_USE_CALLBACKS 1
#include "flecs.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <joy2d/jsys.h>
#include <joy2d/jnetwork.h>
#include <joy2d/jcore.h>
#include <joy2d/jmath.h>
#include <joy2d/jutils.h>
#include <joy2d/jtext.h>
#include "Component.h"
#include <map>
#include <queue>
#include <list>
#include <string>
#include "adventure.pb.h"

const int PIXELS_PER_METER = 50;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static kcpserver_p kcpserver = NULL;
static flecs::world world;

static font_p simhei_font = NULL;
static text_texture_p fps_texture = NULL;

// 位掩码定义
const int INPUT_UP = 1 << 0;
const int INPUT_DOWN = 1 << 1;
const int INPUT_LEFT = 1 << 2;
const int INPUT_RIGHT = 1 << 3;

static bool upPressed = false;
static bool downPressed = false;
static bool leftPressed = false;
static bool rightPressed = false;
static const fp_t MOVE_SPEED = fp_from_float(0.5f);

// ###########################
// 帧同步核心：固定时间步
// ###########################
static Uint64 lastTime = SDL_GetTicks();
static float accumulator = 0.0f;
static const float FIXED_TIMESTEP = 1.0f / 16.0f;

// 【新增】帧缓存最大保留数量（防止无限暴涨）
const int MAX_FRAME_HISTORY = 60;

struct NetworkSingleton {
        int g_id = 1;
        int g_frameid = 1;

        std::vector<adventure::S2CPlayerJoin> player_joins;
        std::vector<adventure::S2CPlayerLeave> player_leaves;
        std::vector<adventure::S2CPlayerInput> player_inputs;

        std::map<int, std::string> commands;
        std::queue<std::string> command_queue;
        std::map<int, std::string> worlds;
};


static flecs::query<IdComponent, TransformComponent> render_query;
static flecs::query<IdComponent, LogicPositionComponent, LogicVelocityComponent> body_query;
static flecs::query<ConnectionComponent> connection_query;
static flecs::query<PlayerComponent, LogicPositionComponent> player_query;
static flecs::query<LogicPositionComponent, TransformComponent> sync_pos_query;

static int GenId(NetworkSingleton* ns)
{
        return ns->g_id++;
}

static void HandleCmdLoading(int conv, adventure::C2S* c2s)
{
        log_info("C2S_CMD_LOADING");
        auto ns = world.get_mut<NetworkSingleton>();
        flecs::entity entity = world.entity().add<ConnectionComponent>();
        auto conn = entity.get_mut<ConnectionComponent>();
        conn->conv = conv;
        conn->frameid = ns->g_frameid - 1;
        conn->health = 10;

        adventure::S2C s2c;
        adventure::S2CMap* map = s2c.mutable_map();
        s2c.set_cmd(adventure::S2C_CMD_LOADING);
        auto it = ns->worlds.find(conn->frameid);
        if (it == ns->worlds.end()) {
                SDL_Log("没有world");
                return;
        }
        if (!map->ParseFromArray(it->second.c_str(), it->second.length())) {
                SDL_Log("解析失败");
                return;
        }
        auto data = s2c.SerializeAsString();
        kcpserver_send(kcpserver, conn->conv, data.c_str(), data.length());
}

static void handle_cmd_player_join(int conv, adventure::C2S* c2s)
{
        adventure::S2CPlayerJoin player_join;
        player_join.set_conv(conv);
        player_join.set_position_x(c2s->player_join().position_x());
        player_join.set_position_y(c2s->player_join().position_y());
        world.get_mut<NetworkSingleton>()->player_joins.push_back(player_join);
}

static void handle_cmd_player_leave(int conv, adventure::C2S* c2s)
{
        adventure::S2CPlayerLeave player_leave;
        player_leave.set_conv(conv);
        world.get_mut<NetworkSingleton>()->player_leaves.push_back(player_leave);
}

static void handle_cmd_player_input(int conv, adventure::C2S* c2s)
{
        adventure::S2CPlayerInput player_input;
        player_input.set_conv(conv);
        player_input.set_keycode(c2s->player_input().keycode());
        player_input.set_sequence(c2s->player_input().sequence());
        world.get_mut<NetworkSingleton>()->player_inputs.push_back(player_input);
}

static void handle_cmd_heartbeat(int conv, adventure::C2S* c2s)
{
        log_info("%d: C2S_CMD_HEARTBEAT", conv);
        connection_query.each([&](ConnectionComponent& conn) {
                if (conn.conv == conv) {
                        conn.health = 10;
                        return false;
                }
                return true;
                });
}


static void ApplyInput(LogicPositionComponent* p, int conv, int sequence, int input)
{
        // 必须先清空！！！
        //v->x = fp_from_float(0);
        //v->y = fp_from_float(0);

        // 用 & 判断按键，支持多键同时按
        if (input & INPUT_UP) {
                p->y = fp_sub(p->y, MOVE_SPEED);
        }
        if (input & INPUT_DOWN) {
                p->y = fp_add(p->y, MOVE_SPEED);
        }
        if (input & INPUT_LEFT) {
                p->x = fp_sub(p->x, MOVE_SPEED);
        }
        if (input & INPUT_RIGHT) {
                p->x = fp_add(p->x, MOVE_SPEED);
        }

        //p.x = fp_add(p.x, v.x);
        //p.y = fp_add(p.y, v.y);
}

static void OnMessage(net_message_p msg, void* userdata)
{
        // ========== 修复：无论走不走分支，msg->data 都必须释放 ==========
        void* msg_data = msg->data;
        size_t msg_len = msg->len;

        if (msg->type == NET_TYPE_CONNECTED) {
                log_info("connected=%d", msg->conv);
        }
        else if (msg->type == NET_TYPE_DISCONNECTED) {
                log_info("disconnected=%d", msg->conv);
        }
        else if (msg->type == NET_TYPE_MESSAGE) {
                adventure::C2S c2s;
                if (c2s.ParseFromArray(msg_data, msg_len)) {
                        if (c2s.cmd() == adventure::CMD_LOADING) {
                                HandleCmdLoading(msg->conv, &c2s);
                        }
                        else if (c2s.cmd() == adventure::CMD_PLAYER_JOIN) {
                                handle_cmd_player_join(msg->conv, &c2s);
                        }
                        else if (c2s.cmd() == adventure::CMD_PLAYER_LEAVE) {
                                handle_cmd_player_leave(msg->conv, &c2s);
                        }
                        else if (c2s.cmd() == adventure::CMD_PLAYER_INPUT) {
                                handle_cmd_player_input(msg->conv, &c2s);
                        }
                        else if (c2s.cmd() == adventure::CMD_PLAYER_HEART) {
                                handle_cmd_heartbeat(msg->conv, &c2s);
                        }
                }
        }

        // ========== 统一释放：最关键的泄漏点！客户端一进就疯狂泄漏 ==========
        if (msg_data) {
                SDL_free(msg_data);
        }
}

// ###########################################################################
// 【帧同步】收集命令（每固定帧执行一次，非定时器）
// ###########################################################################
static void CollectCommandSystem(flecs::world& world)
{
        auto ns = world.get_mut<NetworkSingleton>();
        adventure::S2C s2c;
        s2c.set_cmd(adventure::S2C_CMD_COMMAND);

        adventure::S2CCommand* command = s2c.mutable_command();
        command->set_frame_id(ns->g_frameid++);

        for (auto& pj : ns->player_joins) {
                *command->add_player_joins() = pj;
        }
        for (auto& pi : ns->player_inputs) {
                *command->add_player_inputs() = pi;
        }

        auto command_data = s2c.SerializeAsString();
        ns->commands[command->frame_id()] = command_data;
        ns->command_queue.push(command_data);

        ns->player_joins.clear();
        ns->player_leaves.clear();
        ns->player_inputs.clear();

        // 收集世界实体状态
        adventure::S2CMap map;
        map.set_frame_id(command->frame_id());
        map.set_global_entity_id(ns->g_id);
        body_query.each([&](flecs::entity e, IdComponent& id,
                LogicPositionComponent& pos, LogicVelocityComponent& vel) {
                        adventure::S2CEntity* ent = map.add_entities();
                        ent->set_id(id.id);
                        if (e.has<PlayerComponent>()) {
                                ent->set_type(adventure::S2C_TYPE_PLAYER);
                                ent->set_player_conv(e.get_mut<PlayerComponent>()->conv);
                        }
                        else {
                                ent->set_type(adventure::S2C_TYPE_NORMAL);
                        }
                        ent->set_hp(0);
                        ent->set_position_x(pos.x);
                        ent->set_position_y(pos.y);
                });

        ns->worlds[map.frame_id()] = map.SerializeAsString();
}

// ###########################################################################
// 【帧同步】执行命令
// ###########################################################################
static void HandleCommandSystem(flecs::world& world)
{
        auto ns = world.get_mut<NetworkSingleton>();
        if (ns->command_queue.empty()) return;

        std::string command_data = ns->command_queue.front();
        adventure::S2C s2c;

        if (!s2c.ParseFromString(command_data)) {
                log_info("Failed to parse command");
                ns->command_queue.pop();
                return;
        }

        // 创建玩家
        for (auto& player_join : s2c.command().player_joins()) {
                log_info("%d:CMD_PLAYER_JOIN", player_join.conv());
                fp_t x = player_join.position_x();
                fp_t y = player_join.position_y();

                world.entity()
                        .set<IdComponent>({ GenId(ns) })
                        .set<LogicPositionComponent>({ x, y })
                        .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
                        .set<TransformComponent>({ fp_to_float(x), fp_to_float(y), 0, 0, 0, 0 })
                        .set<PlayerComponent>({ player_join.conv() });
        }

        // 玩家离开
        for (auto& player_leave : s2c.command().player_leaves()) {}

        // 应用输入
        for (auto& input : s2c.command().player_inputs()) {
                player_query.each([&](PlayerComponent& p, LogicPositionComponent& pos) {
                        if (p.conv != input.conv()) return;
                        ApplyInput(&pos, input.conv(), input.sequence(), input.keycode());
                        });
        }

        ns->command_queue.pop();
}

static int get_target_frameid(int curr_frameid, int context_frameid)
{
        int diff = context_frameid - curr_frameid;

        if (diff >= 50)      return curr_frameid + 9;
        if (diff >= 30)      return curr_frameid;
        if (diff >= 10)      return curr_frameid + 4;
        if (diff > 2)        return curr_frameid + 2;

        return curr_frameid;
}

// ###########################################################################
// 【帧同步】通知客户端
// ###########################################################################
static void NotifySystem(flecs::world& world)
{
        auto ns = world.get_mut<NetworkSingleton>();
        connection_query.each([&](flecs::entity e, ConnectionComponent& conn) {
                int taget_frameid = get_target_frameid(conn.frameid, ns->g_frameid);
                for (int i = conn.frameid; i < taget_frameid; i++) {
                        auto it = ns->commands.find(i);
                        kcpserver_send(kcpserver, conn.conv, it->second.c_str(), it->second.size());
                }
                conn.frameid = taget_frameid;
                });
}

static void StartupSystem(flecs::world& world)
{
        auto ns = world.get_mut<NetworkSingleton>();
        adventure::S2CMap map;
        map.set_frame_id(ns->g_frameid);
        map.set_global_entity_id(ns->g_id);
        body_query.each([&](flecs::entity e, IdComponent& id,
                LogicPositionComponent& pos, LogicVelocityComponent& vel) {
                        adventure::S2CEntity* ent = map.add_entities();
                        ent->set_id(id.id);
                        if (e.has<PlayerComponent>()) {
                                ent->set_type(adventure::S2C_TYPE_PLAYER);
                                ent->set_player_conv(e.get_mut<PlayerComponent>()->conv);
                        }
                        else {
                                ent->set_type(adventure::S2C_TYPE_NORMAL);
                        }
                        ent->set_hp(0);
                        ent->set_position_x(pos.x);
                        ent->set_position_y(pos.y);
                });

        ns->worlds[map.frame_id()] = map.SerializeAsString();

        simhei_font = font_create(renderer, "adventure_server_fonts/simhei.ttf", 24);
        const char* fps_str = "fps:";
        fps_texture = text_createx(simhei_font, fps_str, SDL_strlen(fps_str), { 255,255,255,255 });
}

// ###########################
// Flecs 系统：移动
// ###########################
static void MoveSystem(LogicPositionComponent& p, LogicVelocityComponent& v)
{
        p.x = fp_add(p.x, v.x);
        p.y = fp_add(p.y, v.y);
        v.x = fp_from_float(0);
        v.y = fp_from_float(0);
}

// ###########################
// Flecs 系统：渲染插值
// ###########################
static void LerpSystem(LogicPositionComponent& lp, TransformComponent& t)
{
        t.position_x = fp_to_float(lp.x);
        t.position_y = fp_to_float(lp.y);
}

// ###########################################################################
// 【帧同步核心】固定逻辑帧更新
// ###########################################################################
static void FixedLogicUpdate(flecs::world& world)
{
        net_message_t msg;
        while (kcpserver_poll_message(kcpserver, &msg)) {
                OnMessage(&msg, NULL);
                //SDL_free(msg.data);
        }
        CollectCommandSystem(world);
        HandleCommandSystem(world);
        NotifySystem(world);
}


SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
        sys_init_netenv();
        SDL_SetAppMetadata("Adventure server", "1.0", "com.example.adventure-server");

        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("SDL init failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        if (!SDL_CreateWindowAndRenderer("adventure/server", 640, 480, 0, &window, &renderer)) {
                SDL_Log("Window failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        log_info("server start");
        kcpserver = kcpserver_create("192.168.1.33", 10000);
        //kcpserver_set_callback(kcpserver, OnMessage, kcpserver);

        world.component<NetworkSingleton>();
        world.component<ConnectionComponent>();
        world.component<LogicPositionComponent>();
        world.component<LogicVelocityComponent>();
        world.component<TransformComponent>();
        world.component<PlayerComponent>();
        world.set<NetworkSingleton>({});

        auto ns = world.get_mut<NetworkSingleton>();

        //world.system<LogicPositionComponent, LogicVelocityComponent>().each(MoveSystem);
        sync_pos_query = world.query<LogicPositionComponent, TransformComponent>();

        world.entity()
                .set<IdComponent>({ GenId(ns) })
                .set<LogicPositionComponent>({ fp_from_float(1), fp_from_float(1) })
                .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
                .set<TransformComponent>({ 1,1 });

        world.entity()
                .set<IdComponent>({ GenId(ns) })
                .set<LogicPositionComponent>({ fp_from_float(2), fp_from_float(2) })
                .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
                .set<TransformComponent>({ 2,2 });
        render_query = world.query<IdComponent, TransformComponent>();
        body_query = world.query<IdComponent, LogicPositionComponent, LogicVelocityComponent>();
        connection_query = world.query<ConnectionComponent>();
        player_query = world.query<PlayerComponent, LogicPositionComponent>();

        StartupSystem(world);
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
        if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;

        if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
                bool isDown = event->type == SDL_EVENT_KEY_DOWN;
                switch (event->key.key) {
                case SDLK_W: upPressed = isDown; break;
                case SDLK_S: downPressed = isDown; break;
                case SDLK_A: leftPressed = isDown; break;
                case SDLK_D: rightPressed = isDown; break;
                case SDLK_Q: return SDL_APP_SUCCESS;
                default: break;
                }
        }
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
        char buff[JOY_MAX_PATH];
        Uint64 now = SDL_GetTicks();
        float delta = (now - lastTime) / 1000.0f;
        lastTime = now;
        accumulator += delta;
        kcpserver_update(kcpserver);
        while (accumulator >= FIXED_TIMESTEP) {
                //utils_current_datetime("%H:%M:%S", buff, JOY_MAX_PATH);
                //log_info(buff);
                FixedLogicUpdate(world);
                accumulator -= FIXED_TIMESTEP;
        }

        world.progress(delta);


        // 渲染同步放这里（你之前改对的位置）
        sync_pos_query.each([=](LogicPositionComponent& lp, TransformComponent& t) {
                float target_position_x = fp_to_float(lp.x);
                float target_position_y = fp_to_float(lp.y);
                // 正确的固定步插值 alpha（0~1）
               /* float alpha = accumulator / FIXED_TIMESTEP;
                t.position_x = ft_lerp(t.position_x, target_position_x, alpha);
                t.position_y = ft_lerp(t.position_y, target_position_y, alpha);*/
                t.position_x = target_position_x;
                t.position_y = target_position_y;
                });


        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);
        render_query.each([&](IdComponent& id, TransformComponent& t) {
                SDL_FRect r = { t.position_x * PIXELS_PER_METER, t.position_y * PIXELS_PER_METER, 30,30 };
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &r);
                });

        auto ns = world.get_mut<NetworkSingleton>();

        char content[JOY_MAX_PATH] = { 0 };
        sprintf(content, "fps:%d", ns->g_frameid);
        text_updatex(fps_texture, simhei_font, content, SDL_strlen(content), { 255,255,255,255 });
        text_print(renderer, fps_texture, 10, 10);

        SDL_RenderPresent(renderer);

        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        font_destroy(simhei_font);
        text_destroy(fps_texture);

        kcpserver_destroy(kcpserver);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        sys_release_netenv();

        // ========== 修复：protobuf 库资源释放 ==========
        google::protobuf::ShutdownProtobufLibrary();
}