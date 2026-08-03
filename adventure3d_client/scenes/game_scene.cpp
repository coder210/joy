#include <map>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <joy/joy.h>
#include "game_scene.h"
#include "../../adventure3d_shared/protocol.h"
#include "../adventure3d_shared/systems/interpolate_position_system.h"
#include "../systems/render3d_system.h"
#include "../adventure3d_shared/systems/input_consume_join_system.h"
#include "../adventure3d_shared/systems/input_consume_leave_system.h"
#include "../adventure3d_shared/systems/input_consume_input_system.h"
#include "../adventure3d_shared/systems/movement_system.h"
#include "../adventure3d_shared/systems/cleanup_system.h"
#include "../adventure3d_shared/systems/collision_system.h"
#include "../network/network_utils.h"
#include "../../adventure3d_shared/systems/ai_system.h"

// 玩家状态机（使用 ECS ActionStateComponent）

struct game_scene {
        scene_p scene;
        Context* ctx;
        float step_timer = 0;
};

static void on_load(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        log_info("[game scene] start...");

        self->ctx->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "192.168.2.40", 30000);
        //self->ctx->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "192.168.1.7", 30000);

        // entt 不需要显式注册组件，也不需要 set_ctx
        // 创建 NetComponent 作为 singleton（既作为实体组件，也注册到 registry 上下文）
        self->ctx->singleton_net = self->ctx->ecs.create();
        self->ctx->ecs.emplace<NetComponent>(self->ctx->singleton_net);
        self->ctx->ecs.ctx().emplace<NetComponent>();
        self->ctx->ecs.ctx().emplace<GlobalState>();

        // r3d is created once in SDL_AppInit; only meshes are created here
        // 全部使用 lit 网格 + 棋盘格地板纹理，与 joy_editor/main8888.cpp 一致
        self->ctx->mesh_player = r3d_mesh_cube_lit(self->ctx->r3d, 0.6f, 1.2f, 0.6f);
        self->ctx->mesh_enemy  = r3d_mesh_cube_lit(self->ctx->r3d, 0.5f, 1.0f, 0.5f);
        self->ctx->mesh_prop   = r3d_mesh_cube_lit(self->ctx->r3d, 1.0f, 1.0f, 1.0f);
        self->ctx->mesh_floor  = r3d_mesh_plane_lit(self->ctx->r3d, 20.0f, 20.0f, 10);
        self->ctx->floor_tex   = r3d_gen_checkerboard(self->ctx->r3d, 32, 10);
        self->ctx->mesh_sky    = r3d_mesh_cube(self->ctx->r3d, 200.0f, 200.0f, 200.0f);  // 天空盒

        // 注册 system
        add_system(self->ctx, PreUpdate, "input_consume_join", input_consume_join_system);
        add_system(self->ctx, PreUpdate, "input_consume_leave", input_consume_leave_system);
        add_system(self->ctx, PreUpdate, "input_consume_input", input_consume_input_system);
        add_system(self->ctx, OnUpdate, "ai_system", ai_system);
        add_system(self->ctx, OnUpdate,  "movement",           movement_system);
        add_system(self->ctx, OnUpdate,  "collision",          collision_system);
        add_system(self->ctx, OnUpdate,  "cleanup",            cleanup_system);

        // 执行 OnStart phase
        run_phase(self->ctx, OnStart, 0);

        // ---- 音频 ----
        self->ctx->audio_dev = audio_open_device();
        self->ctx->step_sound = audio_shot_create("adventure_client/sounds/grass-step.wav", self->ctx->audio_dev);
        audio_shot_set_gain(self->ctx->step_sound, 1.0f);
        self->ctx->bgm = audio_bgm_create("adventure_client/sounds/bgm.wav", self->ctx->audio_dev);
        audio_bgm_set_gain(self->ctx->bgm, 0.5f);

}

static void client_on_keypressed(const char* key, int scancode, bool repeat, bool ctrl, bool shift, bool alt, void* arg)
{
        game_scene_p self = (game_scene_p)arg;
        int key_mask = 0;
        if (strcmp(key, "w") == 0) key_mask = INPUT_UP;
        else if (strcmp(key, "W") == 0) key_mask = INPUT_UP;
        else if (strcmp(key, "s") == 0) key_mask = INPUT_DOWN;
        else if (strcmp(key, "S") == 0) key_mask = INPUT_DOWN;
        else if (strcmp(key, "a") == 0) key_mask = INPUT_LEFT;
        else if (strcmp(key, "A") == 0) key_mask = INPUT_LEFT;
        else if (strcmp(key, "d") == 0) key_mask = INPUT_RIGHT;
        else if (strcmp(key, "D") == 0) key_mask = INPUT_RIGHT;
        else if (strcmp(key, "space") == 0) key_mask = INPUT_JUMP;
        if (key_mask) {
                self->ctx->current_input_mask |= key_mask;
                if (key_mask == INPUT_ATTACK) {

                }
                        //self->ctx->attack_triggered = true;
                if (key_mask & INPUT_DIR_MASK)
                        self->ctx->local_dir = input_to_dir(key_mask);
        }
}

static void client_on_keyreleased(const char* key, int scancode, void* arg)
{
        game_scene_p self = (game_scene_p)arg;
        int key_mask = 0;
        if (strcmp(key, "w") == 0) key_mask = INPUT_UP;
        else if (strcmp(key, "W") == 0) key_mask = INPUT_UP;
        else if (strcmp(key, "s") == 0) key_mask = INPUT_DOWN;
        else if (strcmp(key, "S") == 0) key_mask = INPUT_DOWN;
        else if (strcmp(key, "a") == 0) key_mask = INPUT_LEFT;
        else if (strcmp(key, "A") == 0) key_mask = INPUT_LEFT;
        else if (strcmp(key, "d") == 0) key_mask = INPUT_RIGHT;
        else if (strcmp(key, "D") == 0) key_mask = INPUT_RIGHT;
        else if (strcmp(key, "space") == 0) key_mask = INPUT_JUMP;
        if (key_mask) self->ctx->current_input_mask &= ~key_mask;
}

static void client_on_mousemoved(float x, float y, float /*dx*/, float /*dy*/, void* arg)
{
        game_scene_p self = (game_scene_p)arg;
        // 逻辑分辨率 480x320，中心点 (240, 160)
        float cx = 240.0f;
        float cy = 160.0f;
        self->ctx->local_yaw = atan2f(x - cx, -(y - cy));
}

static void on_handle_event(scene_p s, const void* ev)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);

        jevent_callbacks_t cbs = {};
        cbs.on_keypressed = client_on_keypressed;
        cbs.on_keyreleased = client_on_keyreleased;
        cbs.on_mousemoved = client_on_mousemoved;
        dispatch_event(cbs, ev, self);
}

static void on_update(scene_p s, float dt) {
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        if (self->ctx->netclient) {
                netclient_update(self->ctx->netclient);
                network_receive(self->ctx);
        }

        // === 固定步长逻辑 Tick（20Hz）===
        self->ctx->accumulator += dt;
        while (self->ctx->accumulator >= self->ctx->FIXED_TIMESTEP) {
            self->ctx->accumulator -= self->ctx->FIXED_TIMESTEP;
            send_input(self->ctx);
        }

        // 从队列取出命令，解析后写到 ECS 组件上
        if (!self->ctx->command_queue.empty()) {
                std::string command_data = self->ctx->command_queue.front();
                self->ctx->command_queue.pop();

                int frame_id, jn = 0, ln = 0, in = 0;
                S2CPlayerJoin joins[32];
                S2CPlayerLeave leaves[32];
                S2CPlayerInput inputs[64];
                bool ok = unpack_command(command_data.c_str(), (int)command_data.size(),
                        &frame_id, joins, 32, &jn, leaves, 32, &ln, inputs, 64, &in);
                if (ok) {
                        auto& cmd = self->ctx->ecs.ctx().get<NetComponent>();
                        cmd.frame_id = ok ? frame_id : -1;
                        cmd.num_joins = jn;  memcpy(cmd.joins, joins, jn * sizeof(S2CPlayerJoin));
                        cmd.num_leaves = ln; memcpy(cmd.leaves, leaves, ln * sizeof(S2CPlayerLeave));
                        cmd.num_inputs = in; memcpy(cmd.inputs, inputs, in * sizeof(S2CPlayerInput));
                }

                // 统一 system 调度（PreUpdate → OnUpdate → PostUpdate）
                run_phase(self->ctx, PreUpdate, self->ctx->FIXED_TIMESTEP);
                run_phase(self->ctx, OnUpdate, self->ctx->FIXED_TIMESTEP);
                run_phase(self->ctx, PostUpdate, self->ctx->FIXED_TIMESTEP);
        }

        // 视觉相关 system 用实时 dt 每帧更新（不放在固定步长内）
        interpolate_position_system(self->ctx->ecs, self->ctx, dt);
}

static void on_render(scene_p s) {
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        render3d_system(self->ctx, 0.0f);
}

static void on_destroy(scene_p s) {
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        if (self->ctx->netclient) {
                netclient_destroy(self->ctx->netclient);
                self->ctx->netclient = NULL;
        }
        audio_shot_destroy(self->ctx->step_sound);
        audio_bgm_destroy(self->ctx->bgm);
        if (self->ctx->audio_dev) audio_close_device(self->ctx->audio_dev);
        if (self->ctx->mesh_floor) r3d_mesh_destroy(self->ctx->r3d, self->ctx->mesh_floor);
        if (self->ctx->mesh_sky) r3d_mesh_destroy(self->ctx->r3d, self->ctx->mesh_sky);
        if (self->ctx->mesh_enemy) r3d_mesh_destroy(self->ctx->r3d, self->ctx->mesh_enemy);
        if (self->ctx->mesh_player) r3d_mesh_destroy(self->ctx->r3d, self->ctx->mesh_player);
        if (self->ctx->mesh_prop) r3d_mesh_destroy(self->ctx->r3d, self->ctx->mesh_prop);
        if (self->ctx->random) utils_random_destroy(self->ctx->random);
        delete self;
}

game_scene_p game_scene_create(Context* ctx) {
        game_scene_p self = new game_scene();
        self->scene = scene_create("game");
        self->ctx = ctx;
        scene_set_userdata(self->scene, self);
        scene_set_load_callback(self->scene, on_load);
        scene_set_event_callback(self->scene, on_handle_event);
        scene_set_update_callback(self->scene, on_update);
        scene_set_render_callback(self->scene, on_render);
        scene_set_destroy_callback(self->scene, on_destroy);
        return self;
}

scene_p game_scene_get_scene(game_scene_p s) {
        return s ? s->scene : NULL;
}
