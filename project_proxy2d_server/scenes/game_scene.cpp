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
        ctx->ecs.ctx().emplace<GlobalState>();
        // 注册 system
        add_system(ctx, OnStart, "map_spawn", map_spawn_system);
        //add_system(ctx, OnStart, "spawn_enemy", spawn_enemy_system);
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
        ctx->accumulator += dt;
        while (ctx->accumulator >= ctx->FIXED_TIMESTEP) {
                ctx->accumulator -= ctx->FIXED_TIMESTEP;
                collect_input(ctx);
                input_forwarding(ctx);
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

        // ---- 当前逻辑帧号（帧同步调试信息）----
        int frame_id = ctx->ecs.ctx().get<GlobalState>().next_frame_id - 1;
        frame_display_draw(ctx->r2d, AssetManager::GetInstance()->GetSimheiFont24(),
                frame_id, 8.0f, 60.0f);
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
