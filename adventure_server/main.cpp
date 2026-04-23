#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <joy2d/jsys.h>
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

// 位掩码定义
const int INPUT_UP = 1 << 0;
const int INPUT_DOWN = 1 << 1;
const int INPUT_LEFT = 1 << 2;
const int INPUT_RIGHT = 1 << 3;
const int INPUT_ATTACK = 1 << 4;

static const fp_t MOVE_SPEED = fp_from_float(5.0f);



static int GenId(Context* ns)
{
        return ns->g_id++;
}

static void HandleLoading(int conv, adventure::C2S* c2s)
{
        log_info("C2S_CMD_LOADING");
        auto ctx = world.get_mut<Context>();
        flecs::entity entity = world.entity().add<ConnectionComponent>();
        auto conn = entity.get_mut<ConnectionComponent>();
        conn->conv = conv;
        conn->frameid = std::max(ctx->g_frameid - 1, 1);
        conn->health = 10;

        adventure::S2C s2c;
        adventure::S2CMap* map = s2c.mutable_map();
        map->set_frame_id(conn->frameid);
        s2c.set_cmd(adventure::S2C_CMD_LOADING);
        auto it = ctx->worlds.find(conn->frameid);
        if (it == ctx->worlds.end()) {
                log_info("没有world");
                return;
        }
        if (!map->ParseFromArray(it->second.c_str(), it->second.length())) {
                log_info("解析失败");
                return;
        }
        auto data = s2c.SerializeAsString();
        kcpserver_send(ctx->kcpserver, conn->conv, data.c_str(), data.length());
}

static void AddPlayerJoin(int conv, adventure::C2S* c2s)
{
        adventure::S2CPlayerJoin player_join;
        player_join.set_conv(conv);
        player_join.set_position_x(c2s->player_join().position_x());
        player_join.set_position_y(c2s->player_join().position_y());
        world.get_mut<Context>()->player_joins.push_back(player_join);
}

static void AddPlayerLeave(int conv, adventure::C2S* c2s)
{
        adventure::S2CPlayerLeave player_leave;
        player_leave.set_conv(conv);
        world.get_mut<Context>()->player_leaves.push_back(player_leave);
}

static void AddPlayerInput(int conv, adventure::C2S* c2s)
{
        adventure::S2CPlayerInput player_input;
        player_input.set_conv(conv);
        player_input.set_keycode(c2s->player_input().keycode());
        player_input.set_sequence(c2s->player_input().sequence());
        world.get_mut<Context>()->player_inputs.push_back(player_input);
}

static void HandleHeartbeat(int conv, adventure::C2S* c2s)
{
        log_info("%d: C2S_CMD_HEARTBEAT", conv);
        auto ctx = world.get_mut<Context>();
        ctx->connection_query.each([&](ConnectionComponent& conn) {
                if (conn.conv == conv) {
                        conn.health = 10;
                        return false;
                }
                return true;
                });
}
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
                if (nearest_id->hp <= 0) {
                }

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
        // 获取逻辑步长（可以从 ctx 传入，或者使用全局常量）
        auto ctx = world.get_mut<Context>();
        fp_t delta = fp_from_float(ctx->FIXED_TIMESTEP);  // 1/60 秒
        fp_t step = fp_mul(MOVE_SPEED, delta);            // 速度 × 时间

        if (input & INPUT_UP) {
                p->y = fp_sub(p->y, step);
        }
        if (input & INPUT_DOWN) {
                p->y = fp_add(p->y, step);
        }
        if (input & INPUT_LEFT) {
                p->x = fp_sub(p->x, step);
        }
        if (input & INPUT_RIGHT) {
                p->x = fp_add(p->x, step);
        }
        if (input & INPUT_ATTACK) {
                Attack(p, currRect, currId);
        }
}


static void OnMessage(net_message_p msg, void* userdata)
{
        void* msg_data = msg->data;
        size_t msg_len = msg->len;

        if (msg->type == NET_TYPE_CONNECTED) {
                log_info("connected=%d", msg->conv);
        }
        else if (msg->type == NET_TYPE_DISCONNECTED) {
                log_info("disconnected=%d", msg->conv);
                auto ctx = world.get_mut<Context>();
                adventure::S2CPlayerLeave playerLeave;
                playerLeave.set_conv(msg->conv);
                ctx->player_leaves.push_back(playerLeave);
        }
        else if (msg->type == NET_TYPE_MESSAGE) {
                adventure::C2S c2s;
                if (c2s.ParseFromArray(msg_data, msg_len)) {
                        if (c2s.cmd() == adventure::CMD_LOADING) {
                                HandleLoading(msg->conv, &c2s);
                        }
                        else if (c2s.cmd() == adventure::CMD_PLAYER_JOIN) {
                                AddPlayerJoin(msg->conv, &c2s);
                        }
                        else if (c2s.cmd() == adventure::CMD_PLAYER_LEAVE) {
                                AddPlayerLeave(msg->conv, &c2s);
                        }
                        else if (c2s.cmd() == adventure::CMD_PLAYER_INPUT) {
                                AddPlayerInput(msg->conv, &c2s);
                        }
                        else if (c2s.cmd() == adventure::CMD_PLAYER_HEART) {
                                HandleHeartbeat(msg->conv, &c2s);
                        }
                }
        }
        if (msg_data) {
                SDL_free(msg_data);
        }
}

// ###########################################################################
// 【帧同步】收集命令（每固定帧执行一次，非定时器）
// ###########################################################################
static void CollectCommandSystem(flecs::world& world)
{
        auto ctx = world.get_mut<Context>();
        adventure::S2C s2c;
        s2c.set_cmd(adventure::S2C_CMD_COMMAND);

        adventure::S2CCommand* command = s2c.mutable_command();
        command->set_frame_id(ctx->g_frameid++);

        for (auto& pj : ctx->player_joins) {
                *command->add_player_joins() = pj;
        }
        for (auto& pl : ctx->player_leaves) {
                *command->add_player_leaves() = pl;
        }
        for (auto& pi : ctx->player_inputs) {
                *command->add_player_inputs() = pi;
        }

        auto command_data = s2c.SerializeAsString();
        ctx->commands[command->frame_id()] = command_data;
        ctx->command_queue.push(command_data);

        ctx->player_joins.clear();
        ctx->player_leaves.clear();
        ctx->player_inputs.clear();

        // 收集世界实体状态
        adventure::S2CMap map;
        map.set_frame_id(command->frame_id());
        map.set_global_entity_id(ctx->g_id);
        ctx->body_query.each([&](flecs::entity e, IdComponent& id,
                LogicRectComponent& r, LogicPositionComponent& pos) {
                        adventure::S2CEntity* ent = map.add_entities();
                        ent->set_id(id.id);
                        if (e.has<PlayerComponent>()) {
                                ent->set_type(adventure::S2C_TYPE_PLAYER);
                                ent->set_player_conv(e.get_mut<PlayerComponent>()->conv);
                        }
                        else {
                                ent->set_type(adventure::S2C_TYPE_NORMAL);
                        }
                        ent->set_hp(id.hp);
                        ent->set_position_x(pos.x);
                        ent->set_position_y(pos.y);
                });

        ctx->worlds[map.frame_id()] = map.SerializeAsString();

        // ========== 新增：限制 worlds 和 commands 最多保留 1000 帧 ==========
        const size_t MAX_FRAMES = 1000;
        while (ctx->commands.size() > MAX_FRAMES) {
                ctx->commands.erase(ctx->commands.begin());
        }
        while (ctx->worlds.size() > MAX_FRAMES) {
                ctx->worlds.erase(ctx->worlds.begin());
        }
}


// ###########################################################################
// 【帧同步】执行命令
// ###########################################################################
static void HandleCommandSystem(flecs::world& world)
{
        auto ctx = world.get_mut<Context>();
        if (ctx->command_queue.empty()) return;

        std::string command_data = ctx->command_queue.front();
        adventure::S2C s2c;

        if (!s2c.ParseFromString(command_data)) {
                log_info("Failed to parse command");
                ctx->command_queue.pop();
                return;
        }

        // 创建玩家
        for (auto& player_join : s2c.command().player_joins()) {
                log_info("%d:CMD_PLAYER_JOIN", player_join.conv());
                fp_t x = player_join.position_x();
                fp_t y = player_join.position_y();

                world.entity()
                        .set<IdComponent>({ GenId(ctx), 10 })
                        .set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(.6f) })
                        .set<LogicPositionComponent>({ x, y })
                        .set<LogicVelocityComponent>({ fp_from_float(0), fp_from_float(0) })
                        .set<TransformComponent>({ fp_to_float(x), fp_to_float(y), 0, 0, 0, 0 })
                        .set<PlayerComponent>({ player_join.conv() });
        }

        // 玩家离开
        world.defer_begin();
        for (auto& player_leave : s2c.command().player_leaves()) {
                int conv = player_leave.conv();
                ctx->player_query.each([&](flecs::entity e, PlayerComponent& p, IdComponent& id,
                        LogicRectComponent& r, LogicPositionComponent& pos) {
                                if (p.conv == conv) {
                                        e.destruct();
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

        ctx->command_queue.pop();
}

static int GetTargetFrameId(int curr_frameid, int context_frameid)
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
        auto ctx = world.get_mut<Context>();
        ctx->connection_query.each([&](flecs::entity e, ConnectionComponent& conn) {
                int taget_frameid = GetTargetFrameId(conn.frameid, ctx->g_frameid);
                for (int i = conn.frameid; i < taget_frameid; i++) {
                        auto it = ctx->commands.find(i);
                        if (it != ctx->commands.end()) {
                                kcpserver_send(ctx->kcpserver, conn.conv, it->second.c_str(), it->second.size());
                        }
                        else {
                                // 可选：记录日志，或跳过丢失的帧（客户端可能已断开或过于落后）
                                log_error("Frame %d not found in commands, skip sending.", i);
                        }
                }
                conn.frameid = taget_frameid;
                });
}


// ###########################################################################
// 【帧同步核心】固定逻辑帧更新
// ###########################################################################
static void FixedUpdate(float dt)
{
        auto ctx = world.get_mut<Context>();
        net_message_t msg;
        if (kcpserver_poll_message(ctx->kcpserver, &msg)) {
                OnMessage(&msg, NULL);
        }
}


SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
        sys_init_netenv();
        SDL_SetAppMetadata("Adventure server", "1.0", "com.example.adventure-server");

        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("SDL init failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        log_info("server start");

        world.component<Context>();
        world.component<ConnectionComponent>();
        world.component<LogicRectComponent>();
        world.component<LogicPositionComponent>();
        world.component<LogicVelocityComponent>();
        world.component<TransformComponent>();
        world.component<PlayerComponent>();
        world.set<Context>({});
        auto ctx = world.get_mut<Context>();
        game_timer_init(&ctx->game_timer);
        game_timer_set_target_fps(&ctx->game_timer, 60);
        game_timer_set_time_scale(&ctx->game_timer, 1.0f);
        ctx->sample_fps = simple_fps_create();

        if (!SDL_CreateWindowAndRenderer("adventure/server", 640, 480, 0, &ctx->window, &ctx->renderer)) {
                SDL_Log("Window failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }
        SDL_SetRenderLogicalPresentation(ctx->renderer, 640, 480, SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_STRETCH);
        //ctx->kcpserver = kcpserver_create("192.168.1.33", 10000);
        ctx->kcpserver = kcpserver_create("192.168.2.49", 10000);
        //ctx->kcpserver = kcpserver_create("172.24.9.215", 10000);

        world.entity()
                .set<IdComponent>({ GenId(ctx), 100 })
                .set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(0.6f) })
                .set<LogicPositionComponent>({ fp_from_float(1), fp_from_float(1) })
                .set<TransformComponent>({ 1,1 });
        world.entity()
                .set<IdComponent>({ GenId(ctx), 100 })
                .set<LogicRectComponent>({ fp_from_float(.6f), fp_from_float(0.6f) })
                .set<LogicPositionComponent>({ fp_from_float(4), fp_from_float(4) })
                .set<TransformComponent>({ 2,2 });

        world.system<LogicPositionComponent, TransformComponent>().each(LerpSystem);
        world.system<AttackRayEffectComponent>().each(EffectLifecycleSystem);

        ctx->renderer_query = world.query<IdComponent, LogicRectComponent, TransformComponent>();
        ctx->renderer_attack_rayeffect_query = world.query<AttackRayEffectComponent>();
        //world.system<IdComponent, LogicRectComponent, TransformComponent>().each(RendererSystem);
        //world.system<AttackRayEffectComponent>().each(RendererAttackRayEffectSystem);

        StartupSystem(world);
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
        auto ctx = world.get_mut<Context>();
        if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;

        if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
                bool isDown = event->type == SDL_EVENT_KEY_DOWN;
                switch (event->key.key) {
                case SDLK_W: ctx->upPressed = isDown; break;
                case SDLK_S: ctx->downPressed = isDown; break;
                case SDLK_A: ctx->leftPressed = isDown; break;
                case SDLK_D: ctx->rightPressed = isDown; break;
                case SDLK_Q: return SDL_APP_SUCCESS;
                default: break;
                }
        }
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
        auto ctx = world.get_mut<Context>();
        game_timer_update(&ctx->game_timer);
        float dt = game_timer_get_delta_time(&ctx->game_timer);
        ctx->accumulator += dt;
        kcpserver_update(ctx->kcpserver);
        int fps = simple_fps_update(ctx->sample_fps);

        // ---------- 固定步长物理更新（60Hz） ----------
        if (ctx->accumulator >= ctx->FIXED_TIMESTEP) {
                FixedUpdate(ctx->FIXED_TIMESTEP);
                ctx->accumulator -= ctx->FIXED_TIMESTEP;
        }
        world.progress(dt);
        // ---------- 独立的输入发送定时器（15Hz） ----------
        ctx->serverTickTimer += dt;
        if (ctx->serverTickTimer >= ctx->SERVER_TICK_INTERVAL) {
                ctx->serverTickTimer -= ctx->SERVER_TICK_INTERVAL;
                CollectCommandSystem(world);
                HandleCommandSystem(world);
                NotifySystem(world);
        }

        SDL_SetRenderDrawColor(ctx->renderer, 100, 100, 100, 255);
        SDL_RenderClear(ctx->renderer);

        ctx->renderer_query.each(RendererSystem);
        ctx->renderer_attack_rayeffect_query.each(RendererAttackRayEffectSystem);

        ctx->debugLayer->Update(ctx->g_frameid, fps);
        ctx->debugLayer->Draw(ctx->renderer);

        SDL_RenderPresent(ctx->renderer);

        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        auto ctx = world.get_mut<Context>();
        delete ctx->resources;
        delete ctx->debugLayer;
        simple_fps_destory(ctx->sample_fps);
        kcpserver_destroy(ctx->kcpserver);
        SDL_DestroyWindow(ctx->window);
        SDL_DestroyRenderer(ctx->renderer);
        sys_release_netenv();
        google::protobuf::ShutdownProtobufLibrary();
}