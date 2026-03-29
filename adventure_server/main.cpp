#define SDL_MAIN_USE_CALLBACKS 1
#include "flecs.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <joy2d/jsys.h>
#include <joy2d/jnetwork.h>
#include <joy2d/jcore.h>
#include <joy2d/jmath.h>
#include <joy2d/jutils.h>
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




// 位掩码定义（每个方向占用一个独立位）
const int INPUT_UP = 1 << 0;   // 1
const int INPUT_DOWN = 1 << 1;   // 2
const int INPUT_LEFT = 1 << 2;   // 4
const int INPUT_RIGHT = 1 << 3;   // 8


// 方向键状态
static bool upPressed = false;
static bool downPressed = false;
static bool leftPressed = false;
static bool rightPressed = false;
static const fp_t MOVE_SPEED = fp_from_float(0.5f);

static Uint64 lastTime = 0;
static float accumulator = 0.0f;
static const float FIXED_TIMESTEP = 1.0f / 60.0f;   // 60Hz固定步长

struct NetworkSingleton {
        int g_id = 1;
        int g_frameid = 1;
        std::vector<adventure::S2CPlayerJoin> player_joins;
        std::vector<adventure::S2CPlayerLeave> player_leaves;
        std::vector<adventure::S2CPlayerInput> player_inputs;
        std::map<int, std::string> commands;
        std::queue<std::string> command_queue;
        std::map<int, std::vector<adventure::S2CEntity>> worlds;
};

static void handle_cmd_loading(int conv, adventure::C2S* c2s)
{
        log_info("C2S_CMD_LOADING");
        auto ns = world.get_mut<NetworkSingleton>();
        flecs::entity entity = world.entity().add<ConnectionComponent>();
        auto conn = entity.get_mut<ConnectionComponent>();
        conn->conv = conv;
        conn->frameid = ns->g_frameid;
        conn->health = 10;
        adventure::S2C s2c;
        s2c.set_cmd(adventure::S2C_CMD_LOADING);
        adventure::S2CMap* map = new adventure::S2CMap();
        map->set_frame_id(conn->frameid);
        map->set_conv(conn->conv);
        if (ns->worlds.find(conn->frameid) != ns->worlds.end()) {
                auto world = ns->worlds[conn->frameid];
                for (int i = 0; i < world.size(); i++) {
                        auto entity = map->add_entities();
                        entity->set_id(world.at(i).id());
                        entity->set_position_x(world.at(i).position_x());
                        entity->set_position_y(world.at(i).position_y());
                        entity->set_hp(world.at(i).hp());
                        entity->set_type(world.at(i).type());
                }
        }
        s2c.set_allocated_map(map);
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
        adventure::S2CPlayerInput palyer_input;
        palyer_input.set_conv(conv);
        palyer_input.set_keycode(c2s->player_input().keycode());
        palyer_input.set_sequence(c2s->player_input().sequence());
        world.get_mut<NetworkSingleton>()->player_inputs.push_back(palyer_input);
}

static void handle_cmd_heartbeat(int conv, adventure::C2S* c2s)
{
        log_info("%d: C2S_CMD_HEARTBEAT", conv);
        world.each([&](flecs::entity e, ConnectionComponent& conn) {
                if (conn.conv == conv) {
                        e.get_mut<ConnectionComponent>()->health = 10;
                        return false;
                }
                return true;
                });
}


static void ApplyInput(LogicVelocityComponent* v, int sequence, int input)
{
        if (INPUT_UP == input) {
                v->y = fp_sub(v->y, MOVE_SPEED);
        }
        else if (INPUT_DOWN == input) {
                v->y = fp_add(v->y, MOVE_SPEED);
        }
        else if (INPUT_LEFT == input) {
                v->x = fp_sub(v->x, MOVE_SPEED);
        }
        else if (INPUT_RIGHT == input) {
                v->x = fp_add(v->x, MOVE_SPEED);
        }
}

static void OnMessage(net_message_p msg, void* userdata)
{
        if (msg->type == NET_TYPE_CONNECTED) {
                log_info("connected=%d", msg->conv);
        }
        else if (msg->type == NET_TYPE_DISCONNECTED) {
                log_info("disconnected=%d", msg->conv);
        }
        else if (msg->type == NET_TYPE_MESSAGE) {
                //log_info("msg=%s", msg->data);
                //std::string data(msg->data, msg->len);
                //std::cout << "Received message: " << data << std::endl;
                adventure::C2S c2s;
                if (c2s.ParseFromArray(msg->data, msg->len)) {
                        if (c2s.cmd() == adventure::CMD_LOADING) {
                                handle_cmd_loading(msg->conv, &c2s);
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
        SDL_free(msg->data);
}

static void CollectCommandSystem(flecs::iter it)
{
        auto ns = it.world().get_mut<NetworkSingleton>();
        adventure::S2C s2c;
        s2c.set_cmd(adventure::S2C_CMD_COMMAND);
        adventure::S2CCommand* command = new adventure::S2CCommand();
        command->set_frame_id(ns->g_frameid++);
        if (ns->player_joins.size() > 0) {
                for (int i = 0; i < ns->player_joins.size(); i++) {
                        auto player_join = command->add_player_joins();
                        *player_join = ns->player_joins[i];
                }
        }
        if (ns->player_inputs.size() > 0) {
                for (int i = 0; i < ns->player_inputs.size(); i++) {
                        auto player_input = command->add_player_inputs();
                        *player_input = ns->player_inputs[i];
                }
        }
        s2c.set_allocated_command(command);
        auto command_data = s2c.SerializeAsString();
        ns->commands.insert({ s2c.command().frame_id() , command_data });
        ns->command_queue.push(command_data);

        ns->player_joins.clear();
        ns->player_leaves.clear();
        ns->player_inputs.clear();

        std::vector<adventure::S2CEntity> world;
        for (auto i : it) {
                flecs::entity e = it.entity(i);
                if (e.has<IdComponent>() && e.has<LogicVelocityComponent>()
                        && e.has<LogicPositionComponent>()) {
                        auto idComponent = e.get_mut<IdComponent>();
                        auto logicVelocity = e.get_mut<LogicVelocityComponent>();
                        auto logicPosition = e.get_mut<LogicPositionComponent>();
                        adventure::S2CEntity entity;
                        entity.set_id(idComponent->id);
                        entity.set_type(0);
                        entity.set_hp(0);
                        entity.set_position_x(logicPosition->x);
                        entity.set_position_y(logicPosition->y);
                        world.push_back(entity);
                }
        }
        ns->worlds.insert({ ns->g_frameid , world });
}

static void HandleCommandSystem(flecs::iter it)
{
        auto ns = it.world().get_mut<NetworkSingleton>();
        if (ns->commands.empty()) {
                return;
        }

        std::string command_data = ns->command_queue.front();
        adventure::S2C s2c;
        if (!s2c.ParseFromString(command_data)) {
                log_info("Failed to parse command");
                ns->command_queue.pop();
                return;
        }

        for (int i = 0; i < s2c.command().player_joins().size(); i++) {
                auto player_join = s2c.command().player_joins().at(i);
                log_info("%d:CMD_PLAYER_JOIN", player_join.conv());
                fp_t position_x = player_join.position_x();
                fp_t position_y = player_join.position_y();
                int conv = player_join.conv();
                world.entity()
                        .set<IdComponent>({ ns->g_id++ })
                        .set<LogicPositionComponent>({ position_x, position_y })
                        .set<LogicVelocityComponent>({ fp_from_float(0.0f), fp_from_float(0.0f) })
                        .set<PositionComponent>({ fp_to_float(position_x), fp_to_float(position_y) })
                        .add<PlayerComponent>();

        }

        for (int i = 0; i < s2c.command().player_leaves().size(); i++) {
                auto player_leave = s2c.command().player_leaves().at(i);

        }

        for (int i = 0; i < s2c.command().player_inputs().size(); i++) {
                auto player_input = s2c.command().player_inputs().at(i);
                world.query<PlayerComponent, LogicVelocityComponent>()
                        .each([&](flecs::entity e, PlayerComponent& player, LogicVelocityComponent& v) {
                        if (e.has<PlayerComponent>() && e.has<LogicVelocityComponent>()) {
                                ApplyInput(&v, player_input.sequence(), player_input.keycode());
                        }
                                });
        }

        ns->command_queue.pop();
}

static void NotifySystem(flecs::entity e, ConnectionComponent& conn)
{
        auto ns = e.world().get_mut<NetworkSingleton>();
        auto iter = ns->commands.find(conn.frameid);
        if (iter != ns->commands.end()) {
                conn.frameid++;
                kcpserver_send(kcpserver, conn.conv, iter->second.c_str(), iter->second.size());
        }
}

static void StartupSystem(flecs::iter& it,
        LogicPositionComponent* pos,
        LogicVelocityComponent* vel)
{
        NetworkSingleton* ns = it.world().get_mut<NetworkSingleton>();
        std::vector<adventure::S2CEntity> world;
        int64_t count = it.count();
        for (auto i : it) {
                flecs::entity e = it.entity(i);
                if (e.has<LogicVelocityComponent>() && e.has<LogicPositionComponent>()) {
                        LogicVelocityComponent* logicVelocity = e.get_mut<LogicVelocityComponent>();
                        LogicPositionComponent* logicPosition = e.get_mut<LogicPositionComponent>();
                        adventure::S2CEntity entity;
                        entity.set_type(0);
                        entity.set_hp(0);
                        entity.set_position_x(logicVelocity->x);
                        entity.set_position_y(logicPosition->y);
                        world.push_back(entity);
                }
        }
        ns->worlds.insert({ ns->g_frameid , world });
}

static void MoveSystem(LogicPositionComponent& p, LogicVelocityComponent& v)
{
        p.x = fp_add(p.x, v.x);
        p.y = fp_add(p.y, v.y);
        v.x = 0;
        v.y = 0;
}

static void LerpSystem(LogicPositionComponent& lp, PositionComponent& p)
{
        p.x = fp_to_float(lp.x) * PIXELS_PER_METER;
        p.y = fp_to_float(lp.y) * PIXELS_PER_METER;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
        sys_init_netenv();
        SDL_SetAppMetadata("Adventure server", "1.0", "com.example.adventure-server");

        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        if (!SDL_CreateWindowAndRenderer("adventure/server", 640, 480, 0, &window, &renderer)) {
                SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        log_info("server start");
        kcpserver = kcpserver_create("192.168.1.33", 10000);
        kcpserver_set_callback(kcpserver, OnMessage, kcpserver);

        // 注册组件
        world.component<NetworkSingleton>();
        world.component<ConnectionComponent>();
        world.component<LogicPositionComponent>()
                .member<fp_t>("x").member<fp_t>("y");
        world.component<LogicVelocityComponent>()
                .member<fp_t>("x").member<fp_t>("y");
        world.component<PositionComponent>()
                .member<ft_t>("x").member<ft_t>("y");
        world.component<PlayerComponent>();

        /* 收集command,并执行 20 frames per 1 meter */
        world.set<NetworkSingleton>({});



        /* A:Collect Frames */
        world.system<ConnectionComponent>()
                .interval(0.05f)
                .iter(CollectCommandSystem);

        /* B: Handle Command */
        world.system<ConnectionComponent>()
                .interval(0.05f)
                .iter(HandleCommandSystem);

        /* C */
        world.system<ConnectionComponent>()
                .interval(0.05f)
                .each(NotifySystem);

        // 移动系统：每帧将速度加到位置
        world.system<LogicPositionComponent, LogicVelocityComponent>()
                .each(MoveSystem);

        world.system<LogicPositionComponent, PositionComponent>()
                .interval(0.02f)
                .each(LerpSystem);

        world.entity()
                .set<IdComponent>({ 1000 })
                .set<LogicPositionComponent>({ fp_from_float(1.0f), fp_from_float(1.0f) })
                .set<LogicVelocityComponent>({ fp_from_float(0.0f), fp_from_float(0.0f) })
                .set<PositionComponent>({ 1.0f, 1.0f });

        world.entity()
                .set<IdComponent>({ 1001 })
                .set<LogicPositionComponent>({ fp_from_float(2.0f), fp_from_float(2.0f) })
                .set<LogicVelocityComponent>({ fp_from_float(0.0f), fp_from_float(0.0f) })
                .set<PositionComponent>({ 2.0f, 2.0f });

        world.system<LogicPositionComponent, LogicVelocityComponent>("Startup")
                .kind(flecs::OnStart)
                .iter(StartupSystem);

        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
        if (event->type == SDL_EVENT_QUIT) {
                return SDL_APP_SUCCESS;
        }

        if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
                bool isDown = (event->type == SDL_EVENT_KEY_DOWN);
                SDL_Keycode key = event->key.key;

                switch (key) {
                case SDLK_W: upPressed = isDown; break;    // W -> 上
                case SDLK_S: downPressed = isDown; break;  // S -> 下
                case SDLK_A: leftPressed = isDown; break;  // A -> 左
                case SDLK_D: rightPressed = isDown; break; // D -> 右
                case SDLK_Q: if (isDown) return SDL_APP_SUCCESS; break; // Q退出
                default: break;
                }

                float vx = 0.0f, vy = 0.0f;
                if (rightPressed) vx += MOVE_SPEED;
                if (leftPressed)  vx -= MOVE_SPEED;
                if (downPressed)  vy += MOVE_SPEED;
                if (upPressed)    vy -= MOVE_SPEED;
        }

        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
        Uint64 currentTime = SDL_GetPerformanceCounter();
        if (lastTime == 0) {
                lastTime = currentTime;
        }
        float deltaTime = (float)((currentTime - lastTime) / (double)SDL_GetPerformanceFrequency());
        lastTime = currentTime;

        accumulator += deltaTime;
        while (accumulator >= FIXED_TIMESTEP) {
                world.progress(FIXED_TIMESTEP);
                accumulator -= FIXED_TIMESTEP;
        }

        kcpserver_update(kcpserver);

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        world.query<IdComponent, PositionComponent>()
                .each([=](IdComponent& id, PositionComponent& p) {
                SDL_FRect rect = { p.x, p.y, 30.0f, 30.0f };
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &rect);
                        });
        SDL_RenderPresent(renderer);

        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        kcpserver_destroy(kcpserver);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        sys_release_netenv();
}