#include <string>
#include <map>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <joy/joy.h>
#include "../../adventure_shared/protocol.h"
#include "../layers/debug_layer.h"
#include "../systems/effect_lifecycle_system.h"
#include "../systems/lerp_system.h"
#include "../systems/animator_update_system.h"
#include "../systems/animator_render_system.h"
#include "../systems/drawing_attack_ray_effect_system.h"
#include "../systems/drawing_entity_system.h"
#include "../systems/startup_system.h"
#include "../systems/input_consume_system.h"
#include "../context.h"
#include "../asset_manager.h"
#include "game_scene.h"



struct game_scene {
        scene_p scene;
        simple_fps_counter_p sample_fps;
        Context* ctx;
};

// ========== 动画定义辅助 ==========
static flecs::entity create_warrior_anim_def(flecs::world& world, const char* tex_path) {
        auto def_entity = world.entity();
        AnimatorDef def;

        const int FPS = 6;
        const float DUR = 0.15f;
        const int FW = 192, FH = 192;

        {
                AnimStateDef s; s.name = "idle"; s.loop = true;
                for (int i = 0; i < FPS; i++)
                        s.clips.push_back({ tex_path, { (float)(i * FW), 0, (float)FW, (float)FH }, DUR });
                def.states.push_back(s);
        }
        {
                AnimStateDef s; s.name = "walk"; s.loop = true;
                for (int i = 0; i < FPS; i++)
                        s.clips.push_back({ tex_path, { (float)(i * FW), (float)(1 * FH), (float)FW, (float)FH }, DUR });
                def.states.push_back(s);
        }

        const char* atk_names[] = { "attack_d","attack_l","attack_r","attack_u" };
        int atk_rows[] = { 4,2,2,6 };
        for (int d = 0; d < 4; d++) {
                AnimStateDef s; s.name = atk_names[d]; s.loop = false;
                for (int i = 0; i < FPS; i++)
                        s.clips.push_back({ tex_path, { (float)(i * FW), (float)(atk_rows[d] * FH), (float)FW, (float)FH }, DUR });
                def.states.push_back(s);
        }

        {
                AnimTransitionDef t; t.from_state = 0; t.to_state = 1;
                t.conditions.push_back({ "speed",AnimCondMode::GREATER,0.5f }); t.exit_time = 0.05f; def.transitions.push_back(t);
        }
        {
                AnimTransitionDef t; t.from_state = 1; t.to_state = 0;
                t.conditions.push_back({ "speed",AnimCondMode::LESS,0.5f }); t.exit_time = 0.05f; def.transitions.push_back(t);
        }
        for (int d = 0; d < 4; d++) {
                AnimTransitionDef t; t.from_state = -1; t.to_state = 2 + d;
                t.conditions.push_back({ "attack",AnimCondMode::TRIGGER,0 });
                t.conditions.push_back({ "dir",AnimCondMode::EQUALS,(float)d }); def.transitions.push_back(t);
        }
        for (int d = 0; d < 4; d++) {
                AnimTransitionDef t; t.from_state = 2 + d; t.to_state = 0; t.exit_time = 0.6f; def.transitions.push_back(t);
        }

        def.default_state = 0;
        def_entity.set<AnimatorDef>(def);
        return def_entity;
}



static void on_load(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        self->sample_fps = simple_fps_create();
        Context* ctx = self->ctx;
        ctx->net_server = netserver_create(NET_SERVER_WEBSOCKET, "192.168.1.17", 30000);

        debug_layer_p debug_layer = create_debug_layer();
        scene_add_root_node(self->scene, debug_layer_get_node(debug_layer));

        ctx->ecs.set_ctx(ctx);

        ctx->ecs.system<AttackRayEffectComponent>().each(EffectLifecycleSystem);
        ctx->ecs.system<Animator, AnimParams, PlayerActionComponent>().each(animator_update_system);
        auto net_entity = ctx->ecs.entity().add<NetComponent>();
        log_info("NetComponent count: %d", ctx->ecs.filter<NetComponent>().count());


        ctx->ecs.system("Startup")
                .kind(flecs::OnStart)
                .iter(startup_system);

        // 消费
        ctx->ecs.system<NetComponent>()
            .kind(flecs::PreUpdate)
            .rate(0.05)
            .iter(input_consume_system);
       
      /*  ctx->ecs.system<NetComponent>()
                .kind(flecs::OnUpdate)
                .rate(0.05)
                .iter(collect_command_system);
        ctx->ecs.system<NetComponent>()
                .kind(flecs::OnUpdate)
                .rate(0.05)
                .iter(handle_command_system);*/

        //ctx->ecs.system<ConnectionComponent>()
        //    .kind(flecs::OnUpdate)
        //    .rate(0.05)
        //    .each(notify_command_system);

        ctx->drawing_entity_query = ctx->ecs.query<IdComponent, LogicRectComponent, TransformComponent>();
        ctx->drawing_attack_rayeffect_query = ctx->ecs.query<AttackRayEffectComponent>();
        ctx->body_query = ctx->ecs.query<IdComponent, LogicRectComponent, LogicPositionComponent>();
        ctx->player_query = ctx->ecs.query<PlayerComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();
        ctx->enemy_query = ctx->ecs.query<EnemyComponent, AIComponent, IdComponent, LogicRectComponent, LogicPositionComponent>();

        // 预构建渲染 query，避免每帧重复 filter_builder().build()
        ctx->render_player_query = ctx->ecs.query<TransformComponent, LogicRectComponent,
                Animator, AnimParams, PlayerActionComponent, PlayerComponent>();
        ctx->render_enemy_query = ctx->ecs.query<TransformComponent, LogicRectComponent,
                Animator, AnimParams, PlayerActionComponent, EnemyComponent>();

        // 位置同步 query（所有同时拥有逻辑位置和渲染位置的实体）
        ctx->position_sync_query = ctx->ecs.query<LogicPositionComponent, TransformComponent>();

       

        S2CEntity entities[512]; int ec = 0;
        ctx->body_query.each([&](flecs::entity e, IdComponent& id,
                LogicRectComponent& r, LogicPositionComponent& pos) {
                        if (ec >= 512) return;
                        auto& ent = entities[ec++]; memset(&ent, 0, sizeof(ent));
                        ent.id = id.id;
                        if (e.has<PlayerComponent>()) { ent.type = S2C_TYPE_PLAYER; ent.player_conv = e.get_mut<PlayerComponent>()->conv; }
                        else if (e.has<EnemyComponent>()) { ent.type = S2C_TYPE_ENEMY; }
                        else { ent.type = S2C_TYPE_NORMAL; }
                        ent.hp = id.hp; ent.width = r.width; ent.height = r.height;
                        ent.position_x = pos.x; ent.position_y = pos.y;
                });
        ctx->world_snapshots[ctx->global_frame_id] = pack_world(ctx->global_entity_id, entities, ec);
}

static void server_on_keypressed(const char* key, int scancode, bool repeat, bool ctrl, bool shift, bool alt, void* arg)
{
        (void)arg;
        if (strcmp(key, "q") == 0 || strcmp(key, "Q") == 0) {
                push_quit_event();
        }
}

static void on_handle_event(scene_p s, const void* ev)
{
        jevent_callbacks_t cbs = {};
        cbs.on_keypressed = server_on_keypressed;
        dispatch_event(cbs, ev, NULL);
}


static void handle_cmd_loading(Context* ctx, int conv)
{
        log_info("C2S_CMD_LOADING");
        auto it = ctx->sessions.find(conv);
        if (it == ctx->sessions.end()) {
            // 如果 Session 不存在，先创建一个（正常情况应该在 CONNECTED 时已创建）
            ctx->sessions[conv] = { conv, flecs::entity::null(), 10, 0, false };
            it = ctx->sessions.find(conv);
        }
        // 标记该玩家需要初始加载（帧末由 ReplicationSystem 发送快照）
        it->second.is_loaded = false;   // false 表示尚未发送初始数据
        it->second.health = 10;

        /*flecs::entity entity = ctx->ecs.entity().add<ConnectionComponent>();
        auto conn = entity.get_mut<ConnectionComponent>();
        conn->conv = conv;
        conn->frameid = std::max(ctx->global_frame_id - 1, 1);
        conn->health = 10;

        auto it = ctx->world_snapshots.find(conn->frameid);
        if (it == ctx->world_snapshots.end()) {
                log_info("没有world");
                return;
        }

        S2CEntity entities[512];
        int ec = 0, eid = 0;
        ec = unpack_world(it->second.c_str(), (int)it->second.size(), &eid, entities, 512);
        auto data = pack_s2c_loading(conn->conv, conn->frameid, eid, entities, ec);
        netserver_send(ctx->net_server, conn->conv, data.c_str(), (int)data.size());*/
}

static void handle_cmd_player_join(Context* ctx, int conv, int64_t x, int64_t y)
{
        S2CPlayerJoin j = { conv, x, y };
        ctx->player_joins.push_back(j);
}

static void handle_cmd_player_leave(Context* ctx, int conv)
{
        S2CPlayerLeave l = { conv };
        ctx->player_leaves.push_back(l);
}

static void handle_cmd_player_input(Context* ctx, int conv, uint32_t seq, int dir, int attack)
{
    S2CPlayerInput i = { conv, seq, dir, (int8_t)attack };
    ctx->player_inputs.push_back(i);
}

static void handle_cmd_heartbeat(Context* ctx, int conv)
{
        log_info("%d: C2S_CMD_HEARTBEAT", conv);
        uint64_t ms_ticks;
        utils_get_current_time(&ms_ticks);
        auto it = ctx->sessions.find(conv);
        if (it != ctx->sessions.end()) {
            it->second.health = 10;
            it->second.last_heartbeat = ms_ticks;
        }
        else {
            log_debug("Heartbeat from unknown conv: %d", conv);
        }
}


static void network_receive(Context* ctx)
{
        net_message_t msg;
        while (netserver_poll_message(ctx->net_server, &msg)) {
                if (msg.type == NET_TYPE_CONNECTED) {
                        log_info("connected=%d", msg.conv);
                }
                else if (msg.type == NET_TYPE_DISCONNECTED) {
                        log_info("disconnected=%d", msg.conv);
                        S2CPlayerLeave l = { msg.conv };
                        ctx->player_leaves.push_back(l);
                }
                else if (msg.type == NET_TYPE_MESSAGE) {
                        int cmd, input_dir = 0, input_attack = 0; int64_t join_x = 0, join_y = 0;
                        if (unpack_c2s((const char*)msg.data, (int)msg.len, &cmd, &join_x, &join_y, &input_dir, &input_attack)) {
                                switch (cmd) {
                                case CMD_LOADING:
                                        handle_cmd_loading(ctx, msg.conv);
                                        break;
                                case CMD_PLAYER_JOIN:
                                        handle_cmd_player_join(ctx, msg.conv, join_x, join_y);
                                        break;
                                case CMD_PLAYER_LEAVE:
                                        handle_cmd_player_leave(ctx, msg.conv);
                                        break;
                                case CMD_PLAYER_INPUT:
                                        handle_cmd_player_input(ctx, msg.conv, input_dir, input_attack);
                                        break;
                                case CMD_PLAYER_HEART:
                                        handle_cmd_heartbeat(ctx, msg.conv);
                                        break;
                                }
                        }
                        else {
                                log_info("Failed to parse message from conv=%d", msg.conv);
                        }
                }
                if (msg.type == NET_TYPE_MESSAGE && msg.data) {
                        netmessage_free_data(&msg);
                }
        }
}


static void on_update(scene_p s, float dt)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        Context* ctx = self->ctx;

        simple_fps_update(self->sample_fps);
        netserver_update(ctx->net_server);
        network_receive(ctx);
        ctx->ecs.progress(dt);

        // 位置同步（插值平滑，避免 20Hz 逻辑更新导致的跳跃）
        ctx->position_sync_query.each([&](LogicPositionComponent& pos, TransformComponent& t) {
                float target_x = fp_to_float(pos.x);
                float target_y = fp_to_float(pos.y);
                float alpha = dt * 10.0f;
                if (alpha > 1.0f) alpha = 1.0f;
                t.position.x = ft_lerp(t.position.x, target_x, alpha);
                t.position.y = ft_lerp(t.position.y, target_y, alpha);
                });
       

        // ---------- 摄像机跟随第一个玩家 ----------
        ctx->player_query.each([&](PlayerComponent&, IdComponent&,
                LogicRectComponent&, LogicPositionComponent& pos) {
                        float target_cam_x = fp_to_float(pos.x) - 6.4f;
                        float target_cam_y = fp_to_float(pos.y) - 4.8f;
                        float lerp = dt * 5.0f;
                        if (lerp > 1.0f) lerp = 1.0f;
                        ctx->camera.x += (target_cam_x - ctx->camera.x) * lerp;
                        ctx->camera.y += (target_cam_y - ctx->camera.y) * lerp;
                        return; // 只跟第一个玩家
                });
}

static void on_render(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        auto* ctx = self->ctx;

        // ---- tilemap 背景 ----
        if (ctx->tilemap) {
                float cx = ctx->camera.x * (float)SERVER_PPM;
                float cy = ctx->camera.y * (float)SERVER_PPM;
                jtilemap_render_all(ctx->tilemap_render, ctx->tilemap, cx, cy, get_ticks());
        }

        // ---- ECS 实体（碰撞体） ----
        //self->drawing_entity_query.each(DrawingEntitySystem);
        //self->drawing_attack_rayeffect_query.each(DrawingAttackRayEffectSystem);

        //// ---- ECS 精灵渲染（玩家） ----
        //self->render_player_query.each([](flecs::iter& it, size_t i, TransformComponent& t, LogicRectComponent& r,
        //        Animator& anim, AnimParams& params, PlayerActionComponent& act, PlayerComponent&) {
        //                animator_render_system(it.entity(i), t, r, anim, act);
        //        });

        //// ---- ECS 精灵渲染（敌人） ----
        //self->render_enemy_query.each([](flecs::iter& it, size_t i, TransformComponent& t, LogicRectComponent& r,
        //        Animator& anim, AnimParams& params, PlayerActionComponent& act, EnemyComponent&) {
        //                animator_render_system(it.entity(i), t, r, anim, act);
        //        });

        //// ---- 血条渲染（所有有血量实体头顶） ----
        //{
        //        auto* ctx = self->ctx;
        //        self->world.filter_builder<IdComponent, TransformComponent>()
        //                .build().each([ctx](flecs::iter& it, size_t i, IdComponent& id, TransformComponent& t) {
        //                if (id.hp <= 0) return;
        //                flecs::entity e = it.entity(i);
        //                if (!e.has<PlayerComponent>() && !e.has<EnemyComponent>()) return;
        //                float sx = (t.position_x - ctx->camera_x) * SERVER_PPM;
        //                float sy = (t.position_y - ctx->camera_y) * SERVER_PPM;
        //                const float SPRITE_OFF = 81.0f;
        //                float bar_w = 48.0f, bar_h = 8.0f;
        //                float bar_x = sx - bar_w / 2.0f, bar_y = sy - SPRITE_OFF - bar_h - 4.0f;
        //                jrect_t bg = { bar_x - 1, bar_y - 1, bar_w + 2, bar_h + 2 };

        //                r2d_set_color(ctx->r2d, 0, 0, 0, 255);
        //                shape_draw_rectangle(ctx->r2d, "fill", bg);
        //                jrect_t base = { bar_x, bar_y, bar_w, bar_h };
        //                r2d_set_color(ctx->r2d, 40, 40, 40, 255);
        //                shape_draw_rectangle(ctx->r2d, "fill", base);
        //                float ratio = (float)id.hp / 10.0f;
        //                if (ratio > 1.0f) ratio = 1.0f;
        //                if (ratio < 0) ratio = 0;
        //                jrect_t fill = { bar_x, bar_y, bar_w * ratio, bar_h };
        //                if (ratio > 0.6f)      r2d_set_color(ctx->r2d, 60, 210, 60, 255);
        //                else if (ratio > 0.3f) r2d_set_color(ctx->r2d, 210, 210, 60, 255);
        //                else                   r2d_set_color(ctx->r2d, 220, 60, 60, 255);
        //                shape_draw_rectangle(ctx->r2d, "fill", fill);
        //                        });
        //}
}

static void on_destroy(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        Context* ctx = self->ctx;
        simple_fps_destory(self->sample_fps);
        netserver_destroy(ctx->net_server);
        AssetManager::GetInstance()->tex_cache_clear();
        if (ctx->tilemap_render) {
            jtilemap_render_destroy(ctx->tilemap_render);
        }
        if (ctx->tilemap) { 
            jtilemap_destroy(ctx->tilemap);
            ctx->tilemap = NULL;
        }
}

game_scene_p game_scene_create(Context* ctx)
{
        game_scene_p self = new game_scene();
        self->scene = scene_create("game_server");
        self->ctx = ctx;
        // 初始化摄像机
        ctx->camera = { 0.0f,0.0f };
        ctx->ecs.component<IdComponent>();
        ctx->ecs.component<LogicRectComponent>();
        ctx->ecs.component<LogicPositionComponent>();
        ctx->ecs.component<LogicVelocityComponent>();
        ctx->ecs.component<TransformComponent>();
        ctx->ecs.component<PlayerComponent>();
        ctx->ecs.component<AttackRayEffectComponent>();
        ctx->ecs.component<PlayerActionComponent>();
        ctx->ecs.component<AnimatorDef>();
        ctx->ecs.component<Animator>();
        ctx->ecs.component<AnimParams>();
        ctx->ecs.component<AIComponent>();
        ctx->ecs.component<EnemyComponent>();
        ctx->ecs.component<NetComponent>();

        scene_set_userdata(self->scene, self);
        scene_set_load_callback(self->scene, on_load);
        scene_set_event_callback(self->scene, on_handle_event);
        scene_set_update_callback(self->scene, on_update);
        scene_set_render_callback(self->scene, on_render);
        scene_set_destroy_callback(self->scene, on_destroy);

        return self;
}

scene_p game_scene_get_scene(game_scene_p g)
{
        return g ? g->scene : NULL;
}
