#include <string>
#include <map>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <joy/joy.h>
#include "../../adventure3d_shared/protocol.h"
#include "../context.h"
#include "../../adventure3d_shared/systems/effect_lifecycle_system.h"
#include "../../adventure3d_shared/systems/interpolate_position_system.h"
#include "../adventure3d_shared/components/static_mesh_component.h"
#include "../../adventure3d_shared/systems/input_consume_join_system.h"
#include "../../adventure3d_shared/systems/input_consume_leave_system.h"
#include "../../adventure3d_shared/systems/input_consume_input_system.h"
#include "../../adventure3d_shared/systems/movement_system.h"
#include "../../adventure3d_shared/systems/cleanup_system.h"
#include "../../adventure3d_shared/systems/collision_system.h"
#include "../../adventure3d_shared/systems/ai_system.h"
#include "../../adventure3d_shared/systems/spawn_obstacles_system.h"
#include "../systems/snapshot_system.h"
#include "../systems/spawn_enemy_system.h"

#include "../network/network_utils.h"
#include "../systems/render3d_system.h"
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
        ctx->net_server = netserver_create(NET_SERVER_WEBSOCKET, "192.168.2.40", 30000);
        //ctx->net_server = netserver_create(NET_SERVER_WEBSOCKET, "192.168.1.7", 30000);

        // 初始化 3D 渲染器和网格资源（lit 网格 + 棋盘格地板，与 main8888.cpp 一致）
        ctx->r3d = r3d_create(ctx->window, "adventure3d_server/shaders");
        ctx->mesh_floor  = r3d_mesh_plane_lit(ctx->r3d, 20.0f, 20.0f, 10);
        ctx->mesh_player = r3d_mesh_cube_lit(ctx->r3d, 0.6f, 1.2f, 0.6f);
        ctx->mesh_enemy  = r3d_mesh_cube_lit(ctx->r3d, 0.5f, 1.0f, 0.5f);
        ctx->floor_tex   = r3d_gen_checkerboard(ctx->r3d, 32, 10);
        ctx->mesh_sky    = r3d_mesh_cube(ctx->r3d, 200.0f, 200.0f, 200.0f);  // 天空盒

        // 注册全局单例（通过 registry.ctx() 访问）
        ctx->ecs.ctx().emplace<NetComponent>();
        auto& gs = ctx->ecs.ctx().emplace<GlobalState>();
        gs.random_seed = (int)time(nullptr);  // 每次启动随机种子，loading 时同步给客户端
        ctx->random = utils_random_create((uint32_t)gs.random_seed);

        // 注册 system
        add_system(ctx, OnStart, "spawn_obstacles", spawn_obstacles);
        add_system(ctx, OnStart, "spawn_enemy", spawn_enemy_system);
        add_system(ctx, PreUpdate, "input_consume_join", input_consume_join_system);
        add_system(ctx, PreUpdate, "input_consume_leave", input_consume_leave_system);
        add_system(ctx, PreUpdate, "input_consume_input", input_consume_input_system);
        add_system(ctx, OnUpdate, "ai_system", ai_system);
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

}

static void on_render(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        auto* ctx = self->ctx;

        // 3D rendering: render3d_system manages r3d_begin/r3d_end internally
        render3d_system(ctx, 0);
}

static void on_destroy(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        Context* ctx = self->ctx;
        simple_fps_destory(self->sample_fps);
        // 销毁静态碰撞体 mesh
        if (ctx->r3d) {
                auto sm_view = ctx->ecs.view<StaticMeshComponent>();
                for (auto e : sm_view) {
                        if (sm_view.get<StaticMeshComponent>(e).mesh)
                                r3d_mesh_destroy(ctx->r3d, sm_view.get<StaticMeshComponent>(e).mesh);
                }
        }
        if (ctx->mesh_floor) r3d_mesh_destroy(ctx->r3d, ctx->mesh_floor);
        if (ctx->mesh_sky) r3d_mesh_destroy(ctx->r3d, ctx->mesh_sky);
        if (ctx->mesh_player) r3d_mesh_destroy(ctx->r3d, ctx->mesh_player);
        if (ctx->mesh_enemy) r3d_mesh_destroy(ctx->r3d, ctx->mesh_enemy);
        if (ctx->r3d) {
                r3d_destroy(ctx->r3d);
                ctx->r3d = NULL;
        }
        if (ctx->random) utils_random_destroy(ctx->random);
        netserver_destroy(ctx->net_server);
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
