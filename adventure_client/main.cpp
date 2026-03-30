#define SDL_MAIN_USE_CALLBACKS 1
#include "flecs.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <joy2d/jnetwork.h>
#include <joy2d/jcore.h>
#include <joy2d/jsys.h>
#include <joy2d/jmath.h>
#include <joy2d/jutils.h>
#include "Component.h"
#include "adventure.pb.h"
#include <iostream>
#include <map>
#include <list>
#include <string>
#include <cstring>

const int PIXELS_PER_METER = 50;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static kcpclient_p kcpclient = NULL;
static flecs::world world;

static std::map<int, int> local_inputs;
static std::map<int, int> server_inputs;

static bool ready = false;
static int server_frameid = 0;
static std::string loaded_world_data;

// 方向键状态
static bool upPressed = false;
static bool downPressed = false;
static bool leftPressed = false;
static bool rightPressed = false;
static const fp_t MOVE_SPEED = fp_from_float(0.5f);

static int lastSentInput = 0;
static int globalSequence = 0;

// 输入位掩码
const int INPUT_UP = 1 << 0;
const int INPUT_DOWN = 1 << 1;
const int INPUT_LEFT = 1 << 2;
const int INPUT_RIGHT = 1 << 3;

// ###########################
// 帧同步核心：固定时间步
// ###########################
static Uint64 lastTime = 0;
static float accumulator = 0.0f;
static const float FIXED_TIMESTEP = 1.0f / 16.0f;

struct NetworkSingleton {};


static void ApplyInput(LogicVelocityComponent* v, int conv, int sequence, int input)
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


static void handle_cmd_loading(adventure::S2C* s2c)
{
        ready = true;
        for (auto& entity : s2c->map().entities()) {
                auto ecs_entity = world.entity()
                        .set<LogicPositionComponent>({ entity.position_x(), entity.position_y() })
                        .set<PositionComponent>({ fp_to_float(entity.position_x()), fp_to_float(entity.position_y()) });
                if (entity.type() == adventure::S2C_TYPE_NORMAL) {

                }
                else if (entity.type() == adventure::S2C_TYPE_PLAYER) {
                        ecs_entity.set<PlayerComponent>({ entity.player_conv() });
                }
        }

        // 发送玩家加入
        adventure::C2S c2s;
        c2s.set_cmd(adventure::CMD_PLAYER_JOIN);
        auto playerjoin = new adventure::C2SPlayerJoin();
        playerjoin->set_position_x(fp_from_float(1.2f));
        playerjoin->set_position_y(fp_from_float(2.3f));
        c2s.set_allocated_player_join(playerjoin);
        auto data = c2s.SerializeAsString();
        kcpclient_send(kcpclient, data.c_str(), data.length());
}

static void handle_cmd_command(adventure::S2C* s2c)
{
        // 玩家加入
        for (auto& player_join : s2c->command().player_joins()) {
                float px = fp_to_float(player_join.position_x());
                float py = fp_to_float(player_join.position_y());
                log_info("CMD_PLAYER_JOIN");

                world.entity()
                        .set<IdComponent>({ 0 })
                        .set<LogicPositionComponent>({ player_join.position_x(), player_join.position_y() })
                        .set<LogicVelocityComponent>({ fp_from_float(0.0f), fp_from_float(0.0f) })
                        .set<PositionComponent>({ px, py })
                        .set<PlayerComponent>({ player_join.conv()});
        }

        // 玩家离开
        for (auto& player_leave : s2c->command().player_leaves()) {
                world.query<PlayerComponent>().each([&](flecs::entity e, PlayerComponent& player) {
                        if (e.get<ConnectionComponent>()->conv == player_leave.conv()) {
                                e.destruct();
                        }
                        });
        }

        // 应用服务器输入（帧同步核心）
        for (auto& input : s2c->command().player_inputs()) {
                world.query<PlayerComponent, LogicVelocityComponent>()
                        .each([&](flecs::entity e, PlayerComponent& p, LogicVelocityComponent& v) {
                        if (p.conv != input.conv()) return;
                        ApplyInput(&v, input.conv(), input.sequence(), input.keycode());
                                });
        }
}

static void msg_callback(net_message_p msg, void* userdata)
{
        if (msg->type == NET_TYPE_CONNECTED) {
                log_info("connected=%d", msg->conv);
                adventure::C2S c2s;
                c2s.set_cmd(adventure::CMD_LOADING);
                auto data = c2s.SerializeAsString();
                kcpclient_send(kcpclient, data.c_str(), data.length());
        }
        else if (msg->type == NET_TYPE_DISCONNECTED) {
                log_info("disconnected=%d", msg->conv);
        }
        else if (msg->type == NET_TYPE_MESSAGE) {
                adventure::S2C s2c;
                if (s2c.ParseFromArray(msg->data, msg->len)) {
                        if (s2c.cmd() == adventure::S2C_CMD_LOADING) {
                                handle_cmd_loading(&s2c);
                        }
                        else if (s2c.cmd() == adventure::S2C_CMD_COMMAND) {
                                handle_cmd_command(&s2c);
                        }
                }
        }
        SDL_free(msg->data);
}

// ###########################################################################
// 【帧同步】发送本地输入到服务器
// ###########################################################################
static void SendLocalInputToServer(flecs::world& world)
{
        for (auto& [seq, input] : local_inputs) {
                adventure::C2S c2s;
                c2s.set_cmd(adventure::CMD_PLAYER_INPUT);
                auto playerInput = new adventure::C2SPlayerInput();
                playerInput->set_sequence(seq);
                playerInput->set_keycode(input);
                c2s.set_allocated_player_input(playerInput);
                std::string data = c2s.SerializeAsString();
                kcpclient_send(kcpclient, data.c_str(), data.size());
        }
        local_inputs.clear();
}

// ###########################################################################
// 【帧同步】发送心跳（每1秒）
// ###########################################################################
static float heartbeat_timer = 0.0f;
static void SendHeartbeatIfNeeded(flecs::world& world, float dt)
{
        heartbeat_timer += dt;
        if (heartbeat_timer >= 1.0f) {
                heartbeat_timer = 0.0f;
                adventure::C2S c2s;
                c2s.set_cmd(adventure::CMD_PLAYER_HEART);
                auto data = c2s.SerializeAsString();
                kcpclient_send(kcpclient, data.c_str(), data.size());
        }
}

// ###########################################################################
// 移动系统
// ###########################################################################
static void MoveSystem(LogicPositionComponent& p, LogicVelocityComponent& v)
{
        p.x = fp_add(p.x, v.x);
        p.y = fp_add(p.y, v.y);
        v.x = fp_from_float(0);
        v.y = fp_from_float(0);
}

// ###########################################################################
// 逻辑坐标 → 渲染坐标
// ###########################################################################
static void LerpRenderSystem(LogicPositionComponent& lp, PositionComponent& p)
{
        p.x = fp_to_float(lp.x) * PIXELS_PER_METER;
        p.y = fp_to_float(lp.y) * PIXELS_PER_METER;
}

// ###########################################################################
// 【帧同步核心】固定逻辑帧更新
// ###########################################################################
static void FixedLogicUpdate(flecs::world& world, float dt)
{
        // 1. 发送本地输入到服务器
        SendLocalInputToServer(world);

        // 2. 心跳
        SendHeartbeatIfNeeded(world, dt);

        // 3. 执行移动
        world.progress(FIXED_TIMESTEP);

        // 4. 同步渲染位置
        world.query<LogicPositionComponent, PositionComponent>().each(LerpRenderSystem);
}

// ###########################################################################
// 初始化
// ###########################################################################
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
        sys_init_netenv();
        SDL_SetAppMetadata("Adventure client", "1.0", "com.adventure.client");

        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("SDL init fail: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        if (!SDL_CreateWindowAndRenderer("adventure/client", 640, 480, 0, &window, &renderer)) {
                SDL_Log("Window fail: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        log_info("client start");
        kcpclient = kcpclient_create("192.168.1.33", 10000);
        kcpclient_set_callback(kcpclient, msg_callback, kcpclient);

        // 注册组件
        world.component<IdComponent>();
        world.component<ConnectionComponent>();
        world.component<LogicPositionComponent>();
        world.component<LogicVelocityComponent>();
        world.component<PositionComponent>();
        world.component<PlayerComponent>();

        // 只注册纯逻辑系统
        world.system<LogicPositionComponent, LogicVelocityComponent>().each(MoveSystem);

        return SDL_APP_CONTINUE;
}

// ###########################################################################
// 输入事件
// ###########################################################################
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
        if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;

        if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
                bool isDown = (event->type == SDL_EVENT_KEY_DOWN);
                switch (event->key.key) {
                case SDLK_W: upPressed = isDown; break;
                case SDLK_S: downPressed = isDown; break;
                case SDLK_A: leftPressed = isDown; break;
                case SDLK_D: rightPressed = isDown; break;
                case SDLK_Q: return SDL_APP_SUCCESS;
                default: break;
                }

                int currentInput = 0;
                if (upPressed) currentInput |= INPUT_UP;
                if (downPressed) currentInput |= INPUT_DOWN;
                if (leftPressed) currentInput |= INPUT_LEFT;
                if (rightPressed) currentInput |= INPUT_RIGHT;

                if (currentInput != lastSentInput) {
                        lastSentInput = currentInput;
                        local_inputs[globalSequence++] = currentInput;
                }
        }
        return SDL_APP_CONTINUE;
}

// ###########################################################################
// 主循环
// ###########################################################################
SDL_AppResult SDL_AppIterate(void* appstate)
{
        Uint64 now = SDL_GetPerformanceCounter();
        if (lastTime == 0) lastTime = now;
        float delta = (now - lastTime) / (double)SDL_GetPerformanceFrequency();
        lastTime = now;

        accumulator += delta;

        // ###########################
        // 帧同步固定逻辑循环
        // ###########################
        while (accumulator >= FIXED_TIMESTEP) {
                FixedLogicUpdate(world, FIXED_TIMESTEP);
                accumulator -= FIXED_TIMESTEP;
        }

        // 网络更新
        kcpclient_update(kcpclient);

        // 渲染
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        world.query<PositionComponent>().each([&](PositionComponent& p) {
                SDL_FRect rect = { p.x, p.y, 30.0f, 30.0f };
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &rect);
                });

        SDL_RenderPresent(renderer);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        kcpclient_destroy(kcpclient);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        sys_release_netenv();
}