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
static int server_entity_id = 0;

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

static flecs::query<IdComponent, TransformComponent> render_query;
static flecs::query<LogicPositionComponent, TransformComponent> sync_pos_query;
static flecs::query<PlayerComponent, LogicVelocityComponent> player_query;


static void ApplyInput(LogicVelocityComponent* v, int conv, int sequence, int input)
{
        // 必须先清空！！！
        v->x = fp_from_float(0);
        v->y = fp_from_float(0);

        // 用 & 判断按键，支持多键同时按
        if (input & INPUT_UP) {
                v->y = fp_sub(v->y, MOVE_SPEED);
        }
        if (input & INPUT_DOWN) {
                v->y = fp_add(v->y, MOVE_SPEED);
        }
        if (input & INPUT_LEFT) {
                v->x = fp_sub(v->x, MOVE_SPEED);
        }
        if (input & INPUT_RIGHT) {
                v->x = fp_add(v->x, MOVE_SPEED);
        }
}


static void HandleCmdLoading(adventure::S2C* s2c)
{
        ready = true;
        server_frameid = s2c->map().frame_id();
        server_entity_id = s2c->map().global_entity_id();
        for (auto& entity : s2c->map().entities())
        {
                auto e = world.entity()
                        .set<IdComponent>({ entity.id() })
                        .set<LogicPositionComponent>({ entity.position_x(), entity.position_y() })
                        .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
                        .set<TransformComponent>({ fp_to_float(entity.position_x()), fp_to_float(entity.position_y()), 0, 0, 0, 0 });

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

static void HandleCmdCommand(adventure::S2C& s2c)
{
        // 创建玩家
        for (auto& player_join : s2c.command().player_joins()) {
                log_info("%d:CMD_PLAYER_JOIN", player_join.conv());
                fp_t x = player_join.position_x();
                fp_t y = player_join.position_y();

                world.entity()
                        .set<IdComponent>({ server_entity_id++ })
                        .set<LogicPositionComponent>({ x, y })
                        .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
                        .set<TransformComponent>({ fp_to_float(x), fp_to_float(y), 0, 0, 0, 0 })
                        .set<PlayerComponent>({ player_join.conv() });
        }

        // 玩家离开
        for (auto& player_leave : s2c.command().player_leaves()) {

        }

        // 应用输入
        log_info("Applying inputs for frame %d", s2c.command().frame_id());
        for (auto& input : s2c.command().player_inputs()) {
                log_info("input from conv %d: seq=%d, keycode=%d", input.conv(), input.sequence(), input.keycode());
                player_query.each([&](PlayerComponent& p, LogicVelocityComponent& v) {
                        if (p.conv != input.conv()) return;
                        log_info("Apply input from conv %d: seq=%d, keycode=%d", input.conv(), input.sequence(), input.keycode());
                        ApplyInput(&v, input.conv(), input.sequence(), input.keycode());
                });
        }
}

static void OnMessage(net_message_p msg, void*)
{
        if (msg->type == NET_TYPE_CONNECTED) {
                adventure::C2S c2s;
                c2s.set_cmd(adventure::CMD_LOADING);
                std::string d = c2s.SerializeAsString();
                kcpclient_send(kcpclient, d.c_str(), d.size());
        }
        else if (msg->type == NET_TYPE_MESSAGE) {
                adventure::S2C s2c;
                if (s2c.ParseFromArray(msg->data, msg->len)) {
                        if (s2c.cmd() == adventure::S2C_CMD_LOADING) 
                                HandleCmdLoading(&s2c);
                        if (s2c.cmd() == adventure::S2C_CMD_COMMAND)
                                HandleCmdCommand(s2c);
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
static void MoveSystem(LogicPositionComponent& p, LogicVelocityComponent& v)
{
        p.x = fp_add(p.x, v.x);
        p.y = fp_add(p.y, v.y);
        v.x = v.y = fp_from_float(0);
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

        kcpclient = kcpclient_create("192.168.1.33", 10000);
        kcpclient_set_callback(kcpclient, OnMessage, nullptr);

        world.component<IdComponent>();
        world.component<ConnectionComponent>();
        world.component<LogicPositionComponent>();
        world.component<LogicVelocityComponent>();
        world.component<TransformComponent>();
        world.component<PlayerComponent>();

        world.system<LogicPositionComponent, LogicVelocityComponent>().each(MoveSystem);
        render_query = world.query<IdComponent, TransformComponent>();
        sync_pos_query = world.query<LogicPositionComponent, TransformComponent>();
        player_query = world.query<PlayerComponent, LogicVelocityComponent>();

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
        kcpclient_update(kcpclient);

        Uint64 now = SDL_GetPerformanceCounter();
        if (lastTime == 0) lastTime = now;
        float delta = (now - lastTime) / (double)SDL_GetPerformanceFrequency();
        lastTime = now;
        accumulator += delta;
        while (accumulator >= FIXED_TIMESTEP){
                FixedLogicUpdate(FIXED_TIMESTEP);
                accumulator -= FIXED_TIMESTEP;
        }

        // 渲染同步放这里（你之前改对的位置）
        sync_pos_query.each([=](LogicPositionComponent& lp, TransformComponent& t) {
                float target_position_x = fp_to_float(lp.x);
                float target_position_y = fp_to_float(lp.y);
                // 正确的固定步插值 alpha（0~1）
                float alpha = accumulator / FIXED_TIMESTEP;
                t.position_x = ft_lerp(t.position_x, target_position_x, alpha);
                t.position_y = ft_lerp(t.position_y, target_position_y, alpha);
        });

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        render_query.each([&](IdComponent& id, TransformComponent& t) {
                SDL_FRect r = {
                        t.position_x * PIXELS_PER_METER,
                        t.position_y * PIXELS_PER_METER, 30, 30
                };
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