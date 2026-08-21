#include <string>
#include <map>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <joy/joy.h>
#include "../../project_proxy2d_shared/protocol.h"
#include "../context.h"
#include "../layers/debug_layer.h"
#include "../../project_proxy2d_shared/systems/interpolate_position_system.h"
#include "../../project_proxy2d_shared/systems/drawing_entity_system.h"
#include "../../project_proxy2d_shared/systems/drawing_map_tiles_system.h"
#include "../systems/spawn_enemy_system.h"
#include "../project_proxy2d_shared/systems/map_spawn_system.h"
#include "../project_proxy2d_shared/systems/drawing_collision_debug_system.h"
#include "../../project_proxy2d_shared/systems/input_consume_join_system.h"
#include "../project_proxy2d_shared/systems/input_consume_leave_system.h"
#include "../project_proxy2d_shared/systems/input_consume_input_system.h"
#include "../systems/snapshot_system.h"
#include "../../project_proxy2d_shared/systems/movement_system.h"
#include "../../project_proxy2d_shared/systems/cleanup_system.h"
#include "../../project_proxy2d_shared/systems/collision_system.h"
#include "../../project_proxy2d_shared/systems/camera_follow_system.h"
#include "../../project_proxy2d_shared/systems/ai_system.h"
#include "../../project_proxy2d_shared/systems/melee_attack_system.h"
#include "../../project_proxy2d_shared/systems/player_shoot_system.h"
#include "../../project_proxy2d_shared/systems/bullet_system.h"
#include "../../project_proxy2d_shared/systems/player_death_system.h"
#include "../../project_proxy2d_shared/systems/pathfinding_system.h"
#include "../../project_proxy2d_shared/systems/frame_display.h"

#include "../network/network_utils.h"
#include "../asset_manager.h"
#include "game_scene.h"

struct game_scene {
        scene_p scene;
        simple_fps_counter_p sample_fps;
        Context* ctx;
        // 逻辑帧率测量
        uint32_t fps_measure_time = 0;   // 上次统计时刻（ms）
        int      fps_measure_frame = 0;  // 上次统计时的 next_frame_id
};

static void on_load(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        self->sample_fps = simple_fps_create();
        Context* ctx = self->ctx;
        ctx->net_server = netserver_create(NET_SERVER_WEBSOCKET, "192.168.2.28", 30000);
        //ctx->net_server = netserver_create(NET_SERVER_WEBSOCKET, "192.168.1.22", 30000);

        debug_layer_p debug_layer = create_debug_layer();
        scene_add_root_node(self->scene, debug_layer_get_node(debug_layer));

        // 注册全局单例（通过 registry.ctx() 访问）
        ctx->ecs.ctx().emplace<NetComponent>();
        auto& gs = ctx->ecs.ctx().emplace<GlobalState>();
        // 服务端生成确定性随机种子并创建随机序列；seed 通过 loading 协议下发给客户端，
        // 两端用同一 seed 创建同一序列，保证 AI/巡逻等行为帧同步一致。
        gs.random_seed = (int)(get_ticks() ^ ((uintptr_t)&gs & 0xFFFF));
        gs.random = utils_random_create((uint32_t)gs.random_seed);
        // 注册 system
        add_system(ctx, OnStart, "map_spawn", map_spawn_system);
        // 服务端生成初始敌人（带 SpawnIdx）；客户端不本地 spawn，开局敌人通过首帧快照
        // (handle_loading) 下发，中途加入者也据此按 SpawnIdx 重建当前活着的敌人集合。
        add_system(ctx, OnStart, "spawn_enemy", spawn_enemy_system);
        add_system(ctx, PreUpdate, "input_consume_join", input_consume_join_system);
        add_system(ctx, PreUpdate, "input_consume_leave", input_consume_leave_system);
        add_system(ctx, PreUpdate, "input_consume_input", input_consume_input_system);
        add_system(ctx, OnUpdate, "ai_system", ai_system);
        add_system(ctx, OnUpdate, "melee_attack", melee_attack_system);
        add_system(ctx, OnUpdate, "player_shoot", player_shoot_system);
        add_system(ctx, OnUpdate, "bullet", bullet_system);
        add_system(ctx, OnUpdate, "player_death", player_death_system);
        add_system(ctx, OnUpdate, "pathfinding", pathfinding_system);
        add_system(ctx, OnUpdate, "movement", movement_system);
        add_system(ctx, OnUpdate, "collision", collision_system);
        add_system(ctx, OnUpdate, "cleanup", cleanup_system);
        add_system(ctx, PostUpdate, "snapshot", snapshot_system);

        // 执行 OnStart 阶段的 system（只执行一次）
        run_phase(ctx, OnStart, 0);

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


static void on_update(scene_p s, float dt)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        Context* ctx = self->ctx;

        simple_fps_update(self->sample_fps);
        netserver_update(ctx->net_server);
        network_receive(ctx);

        // === 固定步长逻辑 Tick（20Hz）===
        // 注意：next_frame_id 的递增由 collect_input() 内部负责（每 tick 一次），
        // 此处不再重复递增，否则帧序号会翻倍（之前 LogicFPS≈39 即此 bug 所致）。
        ctx->accumulator += dt;
        auto& gs = ctx->ecs.ctx().get<GlobalState>();
        while (ctx->accumulator >= ctx->FIXED_TIMESTEP) {
                ctx->accumulator -= ctx->FIXED_TIMESTEP;
                collect_input(ctx);
                input_forwarding(ctx);
        }

        // ---- 逻辑帧率测量（每秒统计一次实际跑了几个逻辑 tick）----
        uint32_t now_ms = get_ticks();
        if (self->fps_measure_time == 0) {
                self->fps_measure_time  = now_ms;
                self->fps_measure_frame = gs.next_frame_id;
        } else if (now_ms - self->fps_measure_time >= 1000) {
                int elapsed_frames = gs.next_frame_id - self->fps_measure_frame;
                gs.logic_fps = (int)(elapsed_frames * 1000 / (now_ms - self->fps_measure_time));
                self->fps_measure_time  = now_ms;
                self->fps_measure_frame = gs.next_frame_id;
        }

        // 从队列取出命令，解析后写到 ECS 组件上
        if (!ctx->command_queue.empty()) {
                std::string command_data = ctx->command_queue.front();
                ctx->command_queue.pop();

                int frame_id, jn = 0, ln = 0, in = 0;
                S2CPlayerJoin joins[32];
                S2CPlayerLeave leaves[32];
                S2CPlayerInput inputs[64];
                bool ok = unpack_command(command_data.c_str(), (int)command_data.size(),
                        &frame_id, joins, 32, &jn, leaves, 32, &ln, inputs, 64, &in);
                if (ok) {
                        auto& cmd = ctx->ecs.ctx().get<NetComponent>();
                        cmd.frame_id = ok ? frame_id : -1;
                        cmd.num_joins = jn;
                        cmd.num_leaves = ln;
                        cmd.num_inputs = in;
                        memcpy(cmd.leaves, leaves, ln * sizeof(S2CPlayerLeave));
                        memcpy(cmd.joins, joins, jn * sizeof(S2CPlayerJoin));
                        if (cmd.num_inputs > 0) {
                            memcpy(cmd.inputs, inputs, in * sizeof(S2CPlayerInput));
                        }
                }

                // 自动调度逻辑 tick
                run_phase(ctx, PreUpdate, ctx->FIXED_TIMESTEP);
                run_phase(ctx, OnUpdate, ctx->FIXED_TIMESTEP);
                run_phase(ctx, PostUpdate, ctx->FIXED_TIMESTEP);
        }


        interpolate_position_system(ctx->ecs, ctx, dt);
        camera_follow_system(ctx->ecs, ctx, dt);

        // 视觉时间推进（仅渲染用，驱动"浑天仪式"轨道环自旋/变形，不参与帧同步）
        ctx->visual_time += dt;
}

static void on_render(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        auto* ctx = self->ctx;

        // ---- 几何网格背景 + 地形装饰（纯视觉）----
        draw_grid_background(ctx);
        draw_map_tiles(ctx);

        // ---- ECS 实体几何渲染（纯视觉，不参与帧同步逻辑）----
        auto draw_view = ctx->ecs.view<IdComponent, LogicRectComponent, TransformComponent>();
        for (auto e : draw_view) {
                auto& id = draw_view.get<IdComponent>(e);
                auto& rect = draw_view.get<LogicRectComponent>(e);
                auto& t = draw_view.get<TransformComponent>(e);
                drawing_entity_system(ctx, e, id, rect, t);
        }

        // ---- 真实碰撞体红框调试（与 collision_system 的 AABB 完全一致）----
        drawing_collision_debug_system(ctx);

        // ---- 当前逻辑帧号 + 实测逻辑帧率（帧同步调试信息）----
        int frame_id = ctx->ecs.ctx().get<GlobalState>().next_frame_id - 1;
        int logic_fps = ctx->ecs.ctx().get<GlobalState>().logic_fps;
        frame_display_draw(ctx->r2d, AssetManager::GetInstance()->GetSimheiFont28(),
                frame_id, logic_fps, 8.0f, 60.0f);
}

static void on_destroy(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        Context* ctx = self->ctx;
        simple_fps_destory(self->sample_fps);
        netserver_destroy(ctx->net_server);
        AssetManager::GetInstance()->tex_cache_clear();
}

game_scene_p game_scene_create(Context* ctx)
{
        game_scene_p self = new game_scene();
        self->scene = scene_create("game_server");
        self->ctx = ctx;
        // entt 不需要显式注册组件，使用即注册

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
