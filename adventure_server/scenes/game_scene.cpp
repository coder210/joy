#include <string>
#include <map>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <joy/joy.h>
#include "../../adventure_shared/protocol.h"
#include "../context.h"
#include "../layers/debug_layer.h"
#include "../../adventure_shared/systems/effect_lifecycle_system.h"
#include "../../adventure_shared/systems/interpolate_position_system.h"
#include "../../adventure_shared/systems/animator_update_system.h"
#include "../../adventure_shared/systems/animator_render_system.h"
#include "../../adventure_shared/systems/drawing_attack_ray_effect_system.h"
#include "../../adventure_shared/systems/drawing_entity_system.h"
#include "../systems/creating_block_system.h"
#include "../systems/spawn_enemy_system.h"
#include "../systems/init_tilemap_system.h"
#include "../../adventure_shared/systems/input_consume_join_system.h"
#include "../adventure_shared/systems/input_consume_leave_system.h"
#include "../adventure_shared/systems/input_consume_input_system.h"
#include "../factory/animator_def_factory.h"
#include "../systems/snapshot_system.h"
#include "../../adventure_shared/systems/movement_system.h"
#include "../../adventure_shared/systems/cleanup_system.h"
#include "../../adventure_shared/systems/collision_system.h"
#include "../../adventure_shared/systems/camera_follow_system.h"
#include "../../adventure_shared/systems/ai_system.h"

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
        //ctx->net_server = netserver_create(NET_SERVER_WEBSOCKET, "192.168.2.44", 30000);
        ctx->net_server = netserver_create(NET_SERVER_WEBSOCKET, "192.168.1.7", 30000);

        debug_layer_p debug_layer = create_debug_layer();
        scene_add_root_node(self->scene, debug_layer_get_node(debug_layer));

        // 注册全局单例（通过 registry.ctx() 访问）
        ctx->ecs.ctx().emplace<NetComponent>();
        ctx->ecs.ctx().emplace<GlobalState>();
        create_warrior_anim_def(ctx->ecs, "adventure_server/textures/knights/troops/warrior/warrior_blue.png");
        create_goblin_anim_def(ctx->ecs, "adventure_server/textures/goblins/troops/torch/torch_blue.png");

        // 注册 system
        //add_system(ctx, OnStart, "creating_block", creating_block_system);
        add_system(ctx, OnStart, "spawn_enemy", spawn_enemy_system);
        add_system(ctx, OnStart, "init_tilemap", init_tilemap_system);
        add_system(ctx, PreUpdate, "input_consume_join", input_consume_join_system);
        add_system(ctx, PreUpdate, "input_consume_leave", input_consume_leave_system);
        add_system(ctx, PreUpdate, "input_consume_input", input_consume_input_system);
        //add_system(ctx, OnUpdate, "ai_system", ai_system);
        add_system(ctx, OnUpdate, "movement", movement_system);
        add_system(ctx, OnUpdate, "collision", collision_system);
        add_system(ctx, OnUpdate, "animator_update", animator_update_system);
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

}

static void on_render(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        auto* ctx = self->ctx;

        // ---- tilemap 背景 ----
        if (ctx->tilemap) {
                auto& cam = ctx->ecs.ctx().get<GlobalState>().camera_position;
                float cx = cam.x * (float)SERVER_PPM;
                float cy = cam.y * (float)SERVER_PPM;
                jtilemap_render_all(ctx->tilemap_render, ctx->tilemap, cx, cy, get_ticks());
        }

        // ---- ECS 实体渲染（有动画的用动画，没有的用方块）----
        auto draw_view = ctx->ecs.view<IdComponent, LogicRectComponent, TransformComponent>();
        for (auto e : draw_view) {
                auto& rect = draw_view.get<LogicRectComponent>(e);
                auto& t = draw_view.get<TransformComponent>(e);
                if (ctx->ecs.all_of<anim_t, anim_params_t, ActionStateComponent>(e)) {
                        auto& anim = ctx->ecs.get<AnimatorComponent>(e);
                        auto& act = ctx->ecs.get<ActionStateComponent>(e);
                        animator_render_system(ctx, ctx->ecs, t, rect, anim, act, e);
                } else {
                        auto& id = draw_view.get<IdComponent>(e);
                        drawing_entity_system(ctx, e, id, rect, t);
                }
        }
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
