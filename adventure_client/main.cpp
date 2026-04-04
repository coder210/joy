#define SDL_MAIN_USE_CALLBACKS 1
#include "flecs.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <joy2d/jnetwork.h>
#include <joy2d/jcore.h>
#include <joy2d/jsys.h>
#include <joy2d/jmath.h>
#include <joy2d/jutils.h>
#include <joy2d/jui.h>
#include <joy2d/jcollision.h>
#include "Component.h"
#include "adventure.pb.h"
#include <iostream>
#include <map>
#include <list>
#include <string>
#include "DebugLayer.h"
#include "MobileInputLayer.h"


const int PIXELS_PER_METER = 50;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static kcpclient_p kcpclient = NULL;
static flecs::world world;

static std::map<int, int> server_inputs;
static bool ready = false;
static int server_frameid = 0;
static int server_entity_id = 0;

static Resources* resources = NULL;
static DebugLayer* debugLayer = NULL;
static MobileInputLayer* mobileInputLayer = NULL;

// 按键状态
static bool upPressed = false;
static bool downPressed = false;
static bool leftPressed = false;
static bool rightPressed = false;
static bool attackPressed = false;
static const fp_t MOVE_SPEED = fp_from_float(0.5f);
static int globalSequence = 0;

// 输入掩码
const int INPUT_UP = 1 << 0;
const int INPUT_DOWN = 1 << 1;
const int INPUT_LEFT = 1 << 2;
const int INPUT_RIGHT = 1 << 3;
const int INPUT_ATTACK = 1 << 4;

// 固定逻辑步
static Uint64 lastTime = SDL_GetTicks();
static float accumulator = 0.0f;
static const float FIXED_TIMESTEP = 1.0f / 16.0f;

static flecs::query<IdComponent, LogicRectComponent, TransformComponent> render_query;
static flecs::query<LogicPositionComponent, TransformComponent> sync_pos_query;
static flecs::query<IdComponent, LogicRectComponent, LogicPositionComponent> body_query;
static flecs::query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent> player_query;


static void ApplyInput(LogicPositionComponent* p, LogicRectComponent& currRect,
        IdComponent& currId, int conv, int sequence, int input)
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
        if (input & INPUT_ATTACK) {
                log_info("attack");
                ray2df_t ray;
                ray.origin.x = fp_add(p->x, fp_mul(currRect.width, fp_half()));
                ray.origin.y = p->y;
                ray.direction.x = fp_from_float(0);
                ray.direction.y = fp_from_float(-10.0f);
                body_query.each([&](IdComponent& id, 
                        LogicRectComponent& r, LogicPositionComponent& pos) {
                        if (currId.id == id.id) return;
                        rectanglef_t rect = {0};
                        rect.x = pos.x;
                        rect.y = pos.y;
                        rect.width = r.width;
                        rect.height = r.height;
                        ray2d_collisionf_t result = collision2df_get_ray_rectangle(ray, rect);
                        if (result.hit) {
                                log_info("Entity %d hit at distance %.2f", id.id, fp_to_float(result.distance));
                                return;
                        }
                });
        }

        //p.x = fp_add(p.x, v.x);
        //p.y = fp_add(p.y, v.y);
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
                        .set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
                        .set<LogicPositionComponent>({ entity.position_x(), entity.position_y() })
                        .set<TransformComponent>({ fp_to_float(entity.position_x()), fp_to_float(entity.position_y()), 0, 0, 0, 0 });

                if (entity.type() == adventure::S2C_TYPE_PLAYER)
                {
                        e.set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) });
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
        server_frameid = s2c.command().frame_id();
        for (auto& player_join : s2c.command().player_joins()) {
                log_info("%d:CMD_PLAYER_JOIN", player_join.conv());
                fp_t x = player_join.position_x();
                fp_t y = player_join.position_y();

                world.entity()
                        .set<IdComponent>({ server_entity_id++ })
                        .set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
                        .set<LogicPositionComponent>({ x, y })
                        .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
                        .set<TransformComponent>({ fp_to_float(x), fp_to_float(y), 0, 0, 0, 0 })
                        .set<PlayerComponent>({ player_join.conv() });
        }

        // 玩家离开
        for (auto& player_leave : s2c.command().player_leaves()) {
                log_info("%d:CMD_PLAYER_LEAVE", player_leave.conv());
                player_query.each([&](flecs::entity& entity,
                        PlayerComponent& p, IdComponent&id,
                        LogicRectComponent& r, LogicPositionComponent& pos) {
                                if (p.conv == player_leave.conv()) {
                                        entity.destruct();
                                        log_info("Entity removed for conv %d", p.conv);
                                }
                        });
        }

        // 应用输入
        //log_info("Applying inputs for frame %d", s2c.command().frame_id());
        for (auto& input : s2c.command().player_inputs()) {
                //log_info("input from conv %d: seq=%d, keycode=%d", input.conv(), input.sequence(), input.keycode());
                player_query.each([&](PlayerComponent& p, IdComponent& id,
                        LogicRectComponent& r, LogicPositionComponent& pos) {
                        if (p.conv != input.conv()) return;
                        //log_info("Apply input from conv %d: seq=%d, keycode=%d", input.conv(), input.sequence(), input.keycode());
                        ApplyInput(&pos, r, id, input.conv(), input.sequence(), input.keycode());
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
                        if (s2c.cmd() == adventure::S2C_CMD_LOADING) {
                                HandleCmdLoading(&s2c);
                        }
                        else if (s2c.cmd() == adventure::S2C_CMD_COMMAND) {
                                HandleCmdCommand(s2c);
                        }
                }
        }

        // 安全释放
        if (msg->data) SDL_free(msg->data);
}

// ========== 发包：彻底无内存泄漏版 ==========
static void SendLocalInputToServer()
{
        if (!ready) return;
        int cur = 0;
        if (upPressed) { cur |= INPUT_UP; upPressed = false; }
        if (downPressed) { cur |= INPUT_DOWN; downPressed = false; }
        if (leftPressed) { cur |= INPUT_LEFT; leftPressed = false; }
        if (rightPressed) {
                cur |= INPUT_RIGHT;
                rightPressed = false;
        }
        if (attackPressed) {
                cur |= INPUT_ATTACK;
                attackPressed = false;
        }


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
        net_message_t msg;
        for (int i = 0; i < 10; i++) {
                if (kcpclient_poll_message(kcpclient, &msg)) {
                        OnMessage(&msg, NULL);
                }
                else {
                        break;
                }
        }

        SendLocalInputToServer();
        SendHeartbeat(dt);
}


SDL_AppResult SDL_AppInit(void**, int, char**)
{
        sys_init_netenv();
        SDL_Init(SDL_INIT_VIDEO);
        SDL_CreateWindowAndRenderer("client", 640, 480, 0, &window, &renderer);

        kcpclient = kcpclient_create("192.168.1.33", 10000);
        //kcpclient_set_callback(kcpclient, OnMessage, nullptr);

        world.component<IdComponent>();
        world.component<ConnectionComponent>();
        world.component<LogicRectComponent>();
        world.component<LogicPositionComponent>();
        world.component<LogicVelocityComponent>();
        world.component<TransformComponent>();
        world.component<PlayerComponent>();

        render_query = world.query<IdComponent, LogicRectComponent, TransformComponent>();
        sync_pos_query = world.query<LogicPositionComponent, TransformComponent>();
        player_query = world.query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
        body_query = world.query<IdComponent, LogicRectComponent, LogicPositionComponent>();

        resources = new Resources(renderer);
        debugLayer = new DebugLayer(resources);
        mobileInputLayer = new MobileInputLayer(resources, renderer);

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
                case SDLK_J: attackPressed = d; break;
                case SDLK_Q: return SDL_APP_SUCCESS;
                }
        }
        else if (e->type == MOBILE_INPUT_EVENT) {
                switch (e->user.code)
                {
                case MOBILE_INPUT_UP: upPressed = true; break;
                case MOBILE_INPUT_DOWN: downPressed = true; break;
                case MOBILE_INPUT_LEFT: leftPressed = true; break;
                case MOBILE_INPUT_RIGHT: rightPressed = true; break;
                case MOBILE_INPUT_ATTACK: attackPressed = true; break;
                default:
                        upPressed = downPressed = leftPressed = rightPressed = false;
                        break;
                }
        }

        mobileInputLayer->ListenEvent(e);

        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void*)
{
        char buff[JOY_MAX_PATH];
        Uint64 now = SDL_GetTicks();
        float delta = (now - lastTime) / 1000.0f;
        lastTime = now;
        accumulator += delta;

        kcpclient_update(kcpclient);
        while (accumulator >= FIXED_TIMESTEP) {
                //utils_current_datetime("%H:%M:%S", buff, JOY_MAX_PATH);
                //log_info(buff);
                FixedLogicUpdate(FIXED_TIMESTEP);
                accumulator -= FIXED_TIMESTEP;
        }

        world.progress(delta);

        // 渲染同步放这里（你之前改对的位置）
        sync_pos_query.each([=](LogicPositionComponent& lp, TransformComponent& t) {
                float target_position_x = fp_to_float(lp.x);
                float target_position_y = fp_to_float(lp.y);
                // 正确的固定步插值 alpha（0~1）
                float alpha = accumulator / FIXED_TIMESTEP;
                t.position_x = ft_lerp(t.position_x, target_position_x, alpha);
                t.position_y = ft_lerp(t.position_y, target_position_y, alpha);
                /*t.position_x = target_position_x;
                t.position_y = target_position_y;*/
                });

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        render_query.each([&](IdComponent& id,
                LogicRectComponent& rect, TransformComponent& t) {
                        SDL_FRect r = { 0 };
                        r.x = t.position_x * PIXELS_PER_METER;
                        r.y = t.position_y * PIXELS_PER_METER;
                        r.w = fp_to_float(rect.width) * PIXELS_PER_METER;
                        r.h = fp_to_float(rect.height) * PIXELS_PER_METER;
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        SDL_RenderFillRect(renderer, &r);
                });

        debugLayer->Update(server_frameid, 0);
        debugLayer->Draw(renderer);
        mobileInputLayer->Update();
        mobileInputLayer->Draw();

        SDL_RenderPresent(renderer);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void*, SDL_AppResult)
{
        delete resources;
        delete debugLayer;
        delete mobileInputLayer;

        kcpclient_destroy(kcpclient);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        sys_release_netenv();
}