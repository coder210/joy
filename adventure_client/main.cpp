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

static std::map<int, int> server_inputs;
static bool ready = false;
static int server_frameid = 0;

// 按键状态
static bool upPressed = false;
static bool downPressed = false;
static bool leftPressed = false;
static bool rightPressed = false;
static const fp_t MOVE_SPEED = fp_from_float(0.5f);
static int globalSequence = 0;

// 输入掩码
const int INPUT_UP = 1 << 0;
const int INPUT_DOWN = 1 << 1;
const int INPUT_LEFT = 1 << 2;
const int INPUT_RIGHT = 1 << 3;

// 固定逻辑步
static Uint64 lastTime = 0;
static float accumulator = 0.0f;
static const float FIXED_TIMESTEP = 1.0f / 16.0f;

static void ApplyInput(LogicVelocityComponent* v, int input)
{
        if (input & INPUT_UP)    v->y = fp_sub(v->y, MOVE_SPEED);
        if (input & INPUT_DOWN)  v->y = fp_add(v->y, MOVE_SPEED);
        if (input & INPUT_LEFT)  v->x = fp_sub(v->x, MOVE_SPEED);
        if (input & INPUT_RIGHT) v->x = fp_add(v->x, MOVE_SPEED);
}

static void handle_cmd_loading(adventure::S2C* s2c)
{
        ready = true;
        for (auto& entity : s2c->map().entities())
        {
                auto e = world.entity()
                        .set<LogicPositionComponent>({ entity.position_x(), entity.position_y() })
                        .set<PositionComponent>({ fp_to_float(entity.position_x()), fp_to_float(entity.position_y()) });

                if (entity.type() == adventure::S2C_TYPE_PLAYER)
                {
                        e.set<PlayerComponent>({ entity.player_conv() });
                }
        }

        // ========== 修复：不用new，避免内存泄漏 ==========
        adventure::C2S c2s;
        c2s.set_cmd(adventure::CMD_PLAYER_JOIN);
        auto* join = c2s.mutable_player_join();
        join->set_position_x(fp_from_float(1.2f));
        join->set_position_y(fp_from_float(2.3f));

        std::string data = c2s.SerializeAsString();
        kcpclient_send(kcpclient, data.c_str(), data.size());
}

static void handle_cmd_command(adventure::S2C* s2c)
{
        // 玩家加入
        for (auto& j : s2c->command().player_joins())
        {
                world.entity()
                        .set<IdComponent>({ 0 })
                        .set<LogicPositionComponent>({ j.position_x(), j.position_y() })
                        .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
                        .set<PositionComponent>({ fp_to_float(j.position_x()), fp_to_float(j.position_y()) })
                        .set<PlayerComponent>({ j.conv() });
        }

        // 玩家离开
        for (auto& l : s2c->command().player_leaves())
        {
                world.query<PlayerComponent>().each([&](flecs::entity e, PlayerComponent& p) {
                        if (e.has<ConnectionComponent>() && e.get<ConnectionComponent>()->conv == l.conv())
                                e.destruct();
                        });
        }

        // 应用输入
        for (auto& in : s2c->command().player_inputs())
        {
                world.query<PlayerComponent, LogicVelocityComponent>()
                        .each([&](flecs::entity, PlayerComponent& p, LogicVelocityComponent& v) {
                        if (p.conv == in.conv()) ApplyInput(&v, in.keycode());
                                });
        }
}

static void msg_callback(net_message_p msg, void*)
{
        if (msg->type == NET_TYPE_CONNECTED)
        {
                adventure::C2S c2s;
                c2s.set_cmd(adventure::CMD_LOADING);
                std::string d = c2s.SerializeAsString();
                kcpclient_send(kcpclient, d.c_str(), d.size());
        }
        else if (msg->type == NET_TYPE_MESSAGE)
        {
                adventure::S2C s2c;
                if (s2c.ParseFromArray(msg->data, msg->len))
                {
                        if (s2c.cmd() == adventure::S2C_CMD_LOADING) handle_cmd_loading(&s2c);
                        if (s2c.cmd() == adventure::S2C_CMD_COMMAND) handle_cmd_command(&s2c);
                }
        }

        // 安全释放
        if (msg->data) SDL_free(msg->data);
}

// ========== 发包：彻底无内存泄漏版 ==========
static void SendLocalInputToServer()
{
        int cur = 0;
        if (upPressed) cur |= INPUT_UP;
        if (downPressed) cur |= INPUT_DOWN;
        if (leftPressed) cur |= INPUT_LEFT;
        if (rightPressed) cur |= INPUT_RIGHT;

        adventure::C2S c2s;
        c2s.set_cmd(adventure::CMD_PLAYER_INPUT);
        auto* pi = c2s.mutable_player_input();
        pi->set_sequence(globalSequence++);
        pi->set_keycode(cur);

        std::string data = c2s.SerializeAsString();
        kcpclient_send(kcpclient, data.c_str(), data.size());
}

static float heartbeat_timer = 0.f;
static void SendHeartbeat(float dt)
{
        heartbeat_timer += dt;
        if (heartbeat_timer >= 1.f)
        {
                heartbeat_timer = 0.f;
                adventure::C2S c2s;
                c2s.set_cmd(adventure::CMD_PLAYER_HEART);
                std::string d = c2s.SerializeAsString();
                kcpclient_send(kcpclient, d.c_str(), d.size());
        }
}

// ECS系统
static void MoveSys(LogicPositionComponent& p, LogicVelocityComponent& v)
{
        p.x = fp_add(p.x, v.x);
        p.y = fp_add(p.y, v.y);
        v.x = v.y = fp_from_float(0);
}

static void SyncRenderPos(LogicPositionComponent& lp, PositionComponent& rp)
{
        rp.x = fp_to_float(lp.x) * PIXELS_PER_METER;
        rp.y = fp_to_float(lp.y) * PIXELS_PER_METER;
}

static void FixedLogicUpdate(float dt)
{
        SendLocalInputToServer();
        SendHeartbeat(dt);
        world.progress(dt);
}

SDL_AppResult SDL_AppInit(void**, int, char**)
{
        sys_init_netenv();
        SDL_Init(SDL_INIT_VIDEO);
        SDL_CreateWindowAndRenderer("client", 640, 480, 0, &window, &renderer);

        kcpclient = kcpclient_create("192.168.2.37", 10000);
        kcpclient_set_callback(kcpclient, msg_callback, nullptr);

        world.component<IdComponent>();
        world.component<ConnectionComponent>();
        world.component<LogicPositionComponent>();
        world.component<LogicVelocityComponent>();
        world.component<PositionComponent>();
        world.component<PlayerComponent>();

        world.system<LogicPositionComponent, LogicVelocityComponent>().each(MoveSys);
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void*, SDL_Event* e)
{
        if (e->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
        if (e->type == SDL_EVENT_KEY_DOWN || e->type == SDL_EVENT_KEY_UP)
        {
                bool d = (e->type == SDL_EVENT_KEY_DOWN);
                switch (e->key.key)
                {
                case SDLK_W: upPressed = d; break;
                case SDLK_S: downPressed = d; break;
                case SDLK_A: leftPressed = d; break;
                case SDLK_D: rightPressed = d; break;
                case SDLK_Q: return SDL_APP_SUCCESS;
                }
        }
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void*)
{
        Uint64 now = SDL_GetPerformanceCounter();
        float delta = (now - lastTime) / (double)SDL_GetPerformanceFrequency();
        lastTime = now;
        accumulator += delta;

        while (accumulator >= FIXED_TIMESTEP)
        {
                FixedLogicUpdate(FIXED_TIMESTEP);
                accumulator -= FIXED_TIMESTEP;
        }

        // 渲染同步放这里（你之前改对的位置）
        world.query<LogicPositionComponent, PositionComponent>().each(SyncRenderPos);

        kcpclient_update(kcpclient);

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        world.query<PositionComponent>().each([&](PositionComponent& p) {
                SDL_FRect r{ p.x,p.y,30,30 };
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &r);
                });

        SDL_RenderPresent(renderer);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void*, SDL_AppResult)
{
        kcpclient_destroy(kcpclient);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        sys_release_netenv();
}