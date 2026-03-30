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
static Uint64 lastTime = 0;
static float accumulator = 0.0f;
static const float FIXED_TIMESTEP = 1.0f / 16.0f;

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

static int GenId(NetworkSingleton *ns)
{
	return ns->g_id++;
}

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
        auto& world_entities = ns->worlds[conn->frameid];
        for (auto& e : world_entities) {
            auto entity = map->add_entities();
            entity->set_id(e.id());
            entity->set_position_x(e.position_x());
            entity->set_position_y(e.position_y());
            entity->set_hp(e.hp());
            entity->set_type(e.type());
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
    adventure::S2CPlayerInput player_input;
    player_input.set_conv(conv);
    player_input.set_keycode(c2s->player_input().keycode());
    player_input.set_sequence(c2s->player_input().sequence());
    world.get_mut<NetworkSingleton>()->player_inputs.push_back(player_input);
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

static void ApplyInput(LogicVelocityComponent* v, int conv, int sequence, int input)
{
    if (INPUT_UP == input) {
        v->y = fp_sub(v->y, MOVE_SPEED);
    } else if (INPUT_DOWN == input) {
        v->y = fp_add(v->y, MOVE_SPEED);
    } else if (INPUT_LEFT == input) {
        v->x = fp_sub(v->x, MOVE_SPEED);
    } else if (INPUT_RIGHT == input) {
        v->x = fp_add(v->x, MOVE_SPEED);
    }
}

static void OnMessage(net_message_p msg, void* userdata)
{
    if (msg->type == NET_TYPE_CONNECTED) {
        log_info("connected=%d", msg->conv);
    } else if (msg->type == NET_TYPE_DISCONNECTED) {
        log_info("disconnected=%d", msg->conv);
    } else if (msg->type == NET_TYPE_MESSAGE) {
        adventure::C2S c2s;
        if (c2s.ParseFromArray(msg->data, msg->len)) {
            if (c2s.cmd() == adventure::CMD_LOADING) {
                handle_cmd_loading(msg->conv, &c2s);
            } else if (c2s.cmd() == adventure::CMD_PLAYER_JOIN) {
                handle_cmd_player_join(msg->conv, &c2s);
            } else if (c2s.cmd() == adventure::CMD_PLAYER_LEAVE) {
                handle_cmd_player_leave(msg->conv, &c2s);
            } else if (c2s.cmd() == adventure::CMD_PLAYER_INPUT) {
                handle_cmd_player_input(msg->conv, &c2s);
            } else if (c2s.cmd() == adventure::CMD_PLAYER_HEART) {
                handle_cmd_heartbeat(msg->conv, &c2s);
            }
        }
    }
    SDL_free(msg->data);
}

// ###########################################################################
// 【帧同步】收集命令（每固定帧执行一次，非定时器）
// ###########################################################################
static void CollectCommandSystem(flecs::world& world)
{
    auto ns = world.get_mut<NetworkSingleton>();
    adventure::S2C s2c;
    s2c.set_cmd(adventure::S2C_CMD_COMMAND);

    adventure::S2CCommand* command = new adventure::S2CCommand();
    command->set_frame_id(ns->g_frameid++);

    for (auto& pj : ns->player_joins) {
        *command->add_player_joins() = pj;
    }
    for (auto& pi : ns->player_inputs) {
        *command->add_player_inputs() = pi;
    }

    s2c.set_allocated_command(command);
    auto command_data = s2c.SerializeAsString();
    ns->commands[command->frame_id()] = command_data;
    ns->command_queue.push(command_data);

    ns->player_joins.clear();
    ns->player_leaves.clear();
    ns->player_inputs.clear();

    // 收集世界实体状态
    std::vector<adventure::S2CEntity> world_entities;
    world.query<IdComponent, LogicPositionComponent, LogicVelocityComponent>()
        .each([&](flecs::entity e, IdComponent& id, LogicPositionComponent& pos, LogicVelocityComponent& vel) {
            adventure::S2CEntity ent;
            ent.set_id(id.id);
            if (e.has<PlayerComponent>()) {
                    ent.set_type(adventure::S2C_TYPE_PLAYER); // 玩家
                    ent.set_player_conv(e.get_mut<PlayerComponent>()->conv);
            }
            else {
                    ent.set_type(adventure::S2C_TYPE_NORMAL); // 其他实体
            }
            ent.set_hp(0);
            ent.set_position_x(pos.x);
            ent.set_position_y(pos.y);
            world_entities.push_back(ent);
        });

    ns->worlds[ns->g_frameid] = world_entities;
}

// ###########################################################################
// 【帧同步】执行命令（无 Flecs 锁，自由创建/修改实体）
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
            .set<IdComponent>({ GenId(ns)})
            .set<LogicPositionComponent>({ x, y })
            .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
            .set<PositionComponent>({ fp_to_float(x), fp_to_float(y) })
            .set<PlayerComponent>({ player_join.conv() });
    }

    // 玩家离开
    for (auto& player_leave : s2c.command().player_leaves()) {}

    // 应用输入
    for (auto& input : s2c.command().player_inputs()) {
        world.query<PlayerComponent, LogicVelocityComponent>()
            .each([&](flecs::entity e, PlayerComponent& p, LogicVelocityComponent& v) {
		if (p.conv != input.conv()) return;
                ApplyInput(&v, input.conv(), input.sequence(), input.keycode());
            });
    }

    ns->command_queue.pop();
}

// ###########################################################################
// 【帧同步】通知客户端
// ###########################################################################
static void NotifySystem(flecs::world& world)
{
    auto ns = world.get_mut<NetworkSingleton>();
    world.query<ConnectionComponent>()
        .each([&](flecs::entity e, ConnectionComponent& conn) {
            auto it = ns->commands.find(conn.frameid);
            if (it != ns->commands.end()) {
                conn.frameid++;
                kcpserver_send(kcpserver, conn.conv, it->second.c_str(), it->second.size());
            }
        });
}

static void StartupSystem(flecs::world& world)
{
    auto ns = world.get_mut<NetworkSingleton>();
    std::vector<adventure::S2CEntity> entities;

    world.query<IdComponent, LogicPositionComponent, LogicVelocityComponent>()
        .each([&](flecs::entity e, IdComponent& id, LogicPositionComponent& pos, LogicVelocityComponent& vel) {
            adventure::S2CEntity ent;
            ent.set_id(id.id);
            if (e.has<PlayerComponent>()) {
                    ent.set_type(adventure::S2C_TYPE_PLAYER); // 玩家
                    ent.set_player_conv(e.get_mut<PlayerComponent>()->conv);
            }
            else {
                    ent.set_type(adventure::S2C_TYPE_NORMAL); // 其他实体
            }
            ent.set_hp(0);
            ent.set_position_x(pos.x);
            ent.set_position_y(pos.y);
            entities.push_back(ent);
        });

    ns->worlds[ns->g_frameid] = entities;
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
static void LerpSystem(LogicPositionComponent& lp, PositionComponent& p)
{
    p.x = fp_to_float(lp.x) * PIXELS_PER_METER;
    p.y = fp_to_float(lp.y) * PIXELS_PER_METER;
}

// ###########################################################################
// 【帧同步核心】固定逻辑帧更新
// ###########################################################################
static void FixedLogicUpdate(flecs::world& world)
{
    // 1. 收集这一帧的所有网络命令
    CollectCommandSystem(world);

    // 2. 执行命令（创建/删除/修改实体 ✅ 无锁）
    HandleCommandSystem(world);

    // 3. 移动系统
    world.progress(FIXED_TIMESTEP);

    // 4. 推送给客户端
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
    kcpserver_set_callback(kcpserver, OnMessage, kcpserver);

    world.component<NetworkSingleton>();
    world.component<ConnectionComponent>();
    world.component<LogicPositionComponent>().member<fp_t>("x").member<fp_t>("y");
    world.component<LogicVelocityComponent>().member<fp_t>("x").member<fp_t>("y");
    world.component<PositionComponent>().member<ft_t>("x").member<ft_t>("y");
    world.component<PlayerComponent>();
    world.set<NetworkSingleton>({});

    auto ns = world.get_mut<NetworkSingleton>();

    // 唯一保留的系统：移动 + 插值
    world.system<LogicPositionComponent, LogicVelocityComponent>().each(MoveSystem);
    world.system<LogicPositionComponent, PositionComponent>().each(LerpSystem);

    // 测试实体
    world.entity()
        .set<IdComponent>({GenId(ns)})
        .set<LogicPositionComponent>({fp_from_float(1), fp_from_float(1)})
        .set<LogicVelocityComponent>({fp_from_float(0), fp_from_float(0)})
        .set<PositionComponent>({1,1});

    world.entity()
        .set<IdComponent>({ GenId(ns) })
        .set<LogicPositionComponent>({fp_from_float(2), fp_from_float(2)})
        .set<LogicVelocityComponent>({fp_from_float(0), fp_from_float(0)})
        .set<PositionComponent>({2,2});

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

// ###########################################################################
// 主循环：固定帧逻辑 + 自由渲染
// ###########################################################################
SDL_AppResult SDL_AppIterate(void* appstate)
{
    Uint64 now = SDL_GetPerformanceCounter();
    if (lastTime == 0) lastTime = now;
    float delta = (now - lastTime) / (double)SDL_GetPerformanceFrequency();
    lastTime = now;

    accumulator += delta;

    // ###########################
    // 帧同步核心循环
    // ###########################
    while (accumulator >= FIXED_TIMESTEP) {
        FixedLogicUpdate(world);  // 所有逻辑在这里！
        accumulator -= FIXED_TIMESTEP;
    }

    kcpserver_update(kcpserver);

    // 渲染
    SDL_SetRenderDrawColor(renderer, 100,100,100,255);
    SDL_RenderClear(renderer);
    world.query<IdComponent, PositionComponent>().each([&](IdComponent& id, PositionComponent& p) {
        SDL_FRect r = { p.x, p.y, 30,30 };
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderFillRect(renderer, &r);
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