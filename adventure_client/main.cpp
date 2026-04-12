#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <joy2d/jsys.h>
#include <joy2d/jcore.h>
#include <joy2d/jmath.h>
#include <joy2d/jutils.h>
#include <joy2d/jtext.h>
#include <joy2d/jcollision.h>
#include <joy2d/jshapes.h>
#include <string>
#include "Systems.h"
#include "Context.h"


const int PIXELS_PER_METER = 50;


static flecs::world world;



static const fp_t MOVE_SPEED = fp_from_float(0.5f);

// 输入掩码
const int INPUT_UP = 1 << 0;
const int INPUT_DOWN = 1 << 1;
const int INPUT_LEFT = 1 << 2;
const int INPUT_RIGHT = 1 << 3;
const int INPUT_ATTACK = 1 << 4;

static void Attack(LogicPositionComponent* p,
        LogicRectComponent& currRect,
        IdComponent& currId)
{
        auto ctx = world.get_mut<Context>();
        ray2df_t ray;
        ray.origin.x = fp_add(p->x, fp_mul(currRect.width, fp_half()));
        ray.origin.y = p->y;
        ray.direction.x = fp_from_float(.0f);
        ray.direction.y = fp_from_float(-1.0f);

        // ===================== 核心修改：初始化最近目标存储变量 =====================
        // 记录最近的碰撞结果（初始化为未命中）
        ray2d_collisionf_t nearest_hit = { 0 };
        // 记录最近的目标Id组件（空指针）
        IdComponent* nearest_id = nullptr;
        // 记录最近距离（初始化为极大值，确保第一个有效碰撞会替换它）
        float nearest_dist = 999999.0f;

        // 遍历所有碰撞体（不再中途return，必须遍历完所有目标）
        ctx->body_query.each([&](IdComponent& id,
                LogicRectComponent& r, LogicPositionComponent& pos) {
                        // 跳过自身
                        if (currId.id == id.id) return;

                        rectanglef_t rect = { 0 };
                        rect.x = pos.x;
                        rect.y = pos.y;
                        rect.width = r.width;
                        rect.height = r.height;
                        ray2d_collisionf_t result = collision2df_get_ray_rectangle(ray, rect);

                        // ===================== 修改：仅收集碰撞，不立即处理 =====================
                        if (result.hit) {
                                // 计算当前碰撞点与射线起点的距离（垂直射线，直接算Y轴差值即可）
                                float current_dist = fp_to_float(fp_sub(ray.origin.y, result.point.y));

                                // 如果当前目标更近，更新最近目标信息
                                if (current_dist < nearest_dist) {
                                        nearest_dist = current_dist;
                                        nearest_hit = result;
                                        nearest_id = &id; // 记录目标指针
                                }
                        }
                });

        // ===================== 修改：遍历完成后，仅处理【最近的目标】 =====================
        if (nearest_id != nullptr) {
                // 仅对最近目标减血
                nearest_id->hp--;
                // 绘制最近目标的攻击射线特效
                SDL_FRect line;
                float end_x = fp_to_float(ray.origin.x);
                float end_y = fp_to_float(ray.origin.y);
                float start_x = fp_to_float(nearest_hit.point.x);
                float start_y = fp_to_float(nearest_hit.point.y);
                line.w = 0.05f;
                line.h = (end_y - start_y);
                line.x = start_x - line.w * 0.5f;
                line.y = start_y;
                world.entity().set<AttackRayEffectComponent>({ line.x, line.y, line.w, line.h, 0.1f });
        }
}

static void ApplyInput(LogicPositionComponent* p, LogicRectComponent& currRect,
        IdComponent& currId, int conv, int sequence, int input)
{
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
                Attack(p, currRect, currId);
        }
}


static void HandleLoading(adventure::S2C* s2c)
{
        auto ctx = world.get_mut<Context>();
        ctx->ready = true;
        ctx->server_frameid = s2c->map().frame_id();
        ctx->server_entity_id = s2c->map().global_entity_id();
        for (auto& entity : s2c->map().entities())
        {
                auto e = world.entity()
                        .set<IdComponent>({ entity.id(), entity.hp() })
                        .set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
                        .set<LogicPositionComponent>({ entity.position_x(), entity.position_y() })
                        .set<TransformComponent>({ fp_to_float(entity.position_x()), fp_to_float(entity.position_y()), 0, 0, 0, 0 });

                if (entity.type() == adventure::S2C_TYPE_PLAYER)
                {
                        e.set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) });
                        e.set<PlayerComponent>({ entity.player_conv()});
                }
        }

        // ========== 修复：不用new，避免内存泄漏 ==========
        adventure::C2S c2s;
        c2s.set_cmd(adventure::CMD_PLAYER_JOIN);
        auto* join = c2s.mutable_player_join();
        join->set_position_x(fp_from_float(1.2f));
        join->set_position_y(fp_from_float(2.3f));
        std::string data = c2s.SerializeAsString();
        kcpclient_send(ctx->kcpclient, data.c_str(), data.size());
}

static void HandleCommand(adventure::S2C& s2c)
{
        // 创建玩家
        auto ctx = world.get_mut<Context>();
        ctx->server_frameid = s2c.command().frame_id();
        for (auto& player_join : s2c.command().player_joins()) {
                log_info("%d:CMD_PLAYER_JOIN", player_join.conv());
                fp_t x = player_join.position_x();
                fp_t y = player_join.position_y();

                world.entity()
                        .set<IdComponent>({ ctx->server_entity_id++, 10 })
                        .set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
                        .set<LogicPositionComponent>({ x, y })
                        .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
                        .set<TransformComponent>({ fp_to_float(x), fp_to_float(y), 0, 0, 0, 0 })
                        .set<PlayerComponent>({ player_join.conv() });
        }

        // 玩家离开
        world.defer_begin();
        for (auto& player_leave : s2c.command().player_leaves()) {
                //log_info("%d:CMD_PLAYER_LEAVE", player_leave.conv());
                ctx->player_query.each([&](flecs::entity entity,
                        PlayerComponent& p, IdComponent& id,
                        LogicRectComponent& r, LogicPositionComponent& pos) {
                                if (p.conv == player_leave.conv()) {
                                        entity.destruct();
                                        //log_info("Entity removed for conv %d", p.conv);
                                }
                        });
        }
        world.defer_end();   // 此时才真正执行删除

        // 应用输入
        //log_info("Applying inputs for frame %d", s2c.command().frame_id());
        for (auto& input : s2c.command().player_inputs()) {
                //log_info("input from conv %d: seq=%d, keycode=%d", input.conv(), input.sequence(), input.keycode());
                ctx->player_query.each([&](PlayerComponent& p, IdComponent& id,
                        LogicRectComponent& r, LogicPositionComponent& pos) {
                                if (p.conv != input.conv()) return;
                                //log_info("Apply input from conv %d: seq=%d, keycode=%d", input.conv(), input.sequence(), input.keycode());
                                ApplyInput(&pos, r, id, input.conv(), input.sequence(), input.keycode());
                        });
        }
}

static void OnMessage(net_message_p msg, void*)
{
        auto ctx = world.get_mut<Context>();
        if (msg->type == NET_TYPE_CONNECTED) {
                adventure::C2S c2s;
                c2s.set_cmd(adventure::CMD_LOADING);
                std::string d = c2s.SerializeAsString();
                kcpclient_send(ctx->kcpclient, d.c_str(), d.size());
        }
        else if (msg->type == NET_TYPE_MESSAGE) {
                adventure::S2C s2c;
                if (s2c.ParseFromArray(msg->data, msg->len)) {
                        if (s2c.cmd() == adventure::S2C_CMD_LOADING) {
                                HandleLoading(&s2c);
                        }
                        else if (s2c.cmd() == adventure::S2C_CMD_COMMAND) {
                                HandleCommand(s2c);
                        }
                }
        }

        if (msg->data) SDL_free(msg->data);
}

static void SendLocalInputToServer()
{
        auto ctx = world.get_mut<Context>();
        if (!ctx->ready) return;
        int cur = 0;
        if (ctx->upPressed) { cur |= INPUT_UP; ctx->upPressed = false; }
        if (ctx->downPressed) { cur |= INPUT_DOWN; ctx->downPressed = false; }
        if (ctx->leftPressed) { cur |= INPUT_LEFT; ctx->leftPressed = false; }
        if (ctx->rightPressed) { cur |= INPUT_RIGHT; ctx->rightPressed = false; }
        if (ctx->attackPressed) { cur |= INPUT_ATTACK; ctx->attackPressed = false; }

        adventure::C2S c2s;
        c2s.set_cmd(adventure::CMD_PLAYER_INPUT);
        auto* pi = c2s.mutable_player_input();
        pi->set_sequence(ctx->globalSequence++);
        pi->set_keycode(cur);

        std::string data = c2s.SerializeAsString();
        kcpclient_send(ctx->kcpclient, data.c_str(), data.size());
}


static void SendHeartbeat(float dt)
{
        auto ctx = world.get_mut<Context>();
        ctx->heartbeat_timer += dt;
        if (ctx->heartbeat_timer >= 1.f)
        {
                ctx->heartbeat_timer = 0.f;
                adventure::C2S c2s;
                c2s.set_cmd(adventure::CMD_PLAYER_HEART);
                std::string d = c2s.SerializeAsString();
                kcpclient_send(ctx->kcpclient, d.c_str(), d.size());
        }
}

static void FixedLogicUpdate(float dt)
{
        auto ctx = world.get_mut<Context>();
        net_message_t msg;
        if (kcpclient_poll_message(ctx->kcpclient, &msg)) {
                OnMessage(&msg, NULL);
        }
        SendLocalInputToServer();
        SendHeartbeat(dt);
}


SDL_AppResult SDL_AppInit(void**, int, char**)
{
        sys_init_netenv();
        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("SDL init failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        world.component<Context>();
        world.component<ConnectionComponent>();
        world.component<LogicRectComponent>();
        world.component<LogicPositionComponent>();
        world.component<LogicVelocityComponent>();
        world.component<TransformComponent>();
        world.component<PlayerComponent>();
        world.set<Context>({});
        auto ctx = world.get_mut<Context>();

        SDL_CreateWindowAndRenderer("client", 640, 480, 0, &ctx->window, &ctx->renderer);
        SDL_SetRenderLogicalPresentation(ctx->renderer, 640, 480, SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_STRETCH);
        ctx->kcpclient = kcpclient_create("192.168.1.33", 10000);
        //ctx->kcpclient = kcpclient_create("8.148.188.213", 10000);
        //kcpclient_set_callback(kcpclient, OnMessage, nullptr);

        world.system<LogicPositionComponent, TransformComponent>().each(LerpSystem);
        world.system<IdComponent, LogicRectComponent, TransformComponent>().each(RendererSystem);
        world.system<AttackRayEffectComponent>().each(RendererAttackRayEffectSystem);

        ctx->player_query = world.query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
        ctx->body_query = world.query<IdComponent, LogicRectComponent, LogicPositionComponent>();

        ctx->resources = new Resources(ctx->renderer);
        ctx->debugLayer = new DebugLayer(ctx->resources);
        ctx->mobileInputLayer = new MobileInputLayer(ctx->resources, ctx->renderer);

        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void*, SDL_Event* e)
{
        auto ctx = world.get_mut<Context>();
        if (e->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
        if (e->type == SDL_EVENT_KEY_DOWN || e->type == SDL_EVENT_KEY_UP)
        {
                bool d = (e->type == SDL_EVENT_KEY_DOWN);
                switch (e->key.key)
                {
                case SDLK_W: ctx->upPressed = d; break;
                case SDLK_S: ctx->downPressed = d; break;
                case SDLK_A: ctx->leftPressed = d; break;
                case SDLK_D: ctx->rightPressed = d; break;
                case SDLK_J: ctx->attackPressed = d; break;
                case SDLK_Q: return SDL_APP_SUCCESS;
                }
        }
        else if (e->type == MOBILE_INPUT_EVENT) {
                switch (e->user.code)
                {
                case MOBILE_INPUT_UP: ctx->upPressed = true; break;
                case MOBILE_INPUT_DOWN: ctx->downPressed = true; break;
                case MOBILE_INPUT_LEFT: ctx->leftPressed = true; break;
                case MOBILE_INPUT_RIGHT: ctx->rightPressed = true; break;
                case MOBILE_INPUT_ATTACK: ctx->attackPressed = true; break;
                default:
                        ctx->upPressed = ctx->downPressed = false;
                        ctx->leftPressed = ctx->rightPressed = false;
                        break;
                }
        }

        ctx->mobileInputLayer->ListenEvent(e);

        return ctx->running ? SDL_APP_CONTINUE : SDL_APP_SUCCESS;
}

SDL_AppResult SDL_AppIterate(void*)
{
        auto ctx = world.get_mut<Context>();
        char buff[JOY_MAX_PATH];
        Uint64 now = SDL_GetTicks();
        float delta = (now - ctx->lastTime) / 1000.0f;
        ctx->lastTime = now;
        ctx->accumulator += delta;

        kcpclient_update(ctx->kcpclient);
        SDL_SetRenderDrawColor(ctx->renderer, 100, 100, 100, 255);
        SDL_RenderClear(ctx->renderer);

        if (ctx->accumulator >= ctx->FIXED_TIMESTEP) {
                FixedLogicUpdate(ctx->FIXED_TIMESTEP);
                ctx->accumulator -= ctx->FIXED_TIMESTEP;
        }

        world.progress(delta);

        ctx->debugLayer->Update(ctx->server_frameid, 0);
        ctx->debugLayer->Draw(ctx->renderer);
        ctx->mobileInputLayer->Update();
        ctx->mobileInputLayer->Draw();

        SDL_RenderPresent(ctx->renderer);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void*, SDL_AppResult)
{
        auto ctx = world.get_mut<Context>();
        delete ctx->resources;
        delete ctx->debugLayer;
        delete ctx->mobileInputLayer;
        kcpclient_destroy(ctx->kcpclient);
        SDL_DestroyRenderer(ctx->renderer);
        SDL_DestroyWindow(ctx->window);
        sys_release_netenv();
        google::protobuf::ShutdownProtobufLibrary();
}