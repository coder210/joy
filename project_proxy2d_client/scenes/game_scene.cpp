#include <map>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <joy/joy.h>
#include "game_scene.h"
#include "../../project_proxy2d_shared/protocol.h"
#include "../project_proxy2d_shared/systems/interpolate_position_system.h"
#include "../project_proxy2d_shared/systems/drawing_entity_system.h"
#include "../project_proxy2d_shared/systems/input_consume_join_system.h"
#include "../project_proxy2d_shared/systems/player_status_display.h"
#include "../project_proxy2d_shared/systems/frame_display.h"
#include "../project_proxy2d_shared/systems/input_consume_leave_system.h"
#include "../project_proxy2d_shared/systems/input_consume_input_system.h"
#include "../project_proxy2d_shared/systems/movement_system.h"
#include "../project_proxy2d_shared/systems/cleanup_system.h"
#include "../project_proxy2d_shared/systems/collision_system.h"
#include "../layers/debug_layer.h"
#include "../layers/gameplay_controls_layer.h"
#include "../asset_manager.h"
#include "../network/network_utils.h"
#include "../../project_proxy2d_shared/systems/camera_follow_system.h"
#include "../../project_proxy2d_shared/systems/ai_system.h"
#include "../../project_proxy2d_shared/systems/melee_attack_system.h"
#include "../../project_proxy2d_shared/systems/player_shoot_system.h"
#include "../../project_proxy2d_shared/systems/bullet_system.h"
#include "../../project_proxy2d_shared/systems/player_death_system.h"
#include "../../project_proxy2d_shared/systems/pathfinding_system.h"

#include "../../project_proxy2d_shared/systems/drawing_map_tiles_system.h"
#include "../../project_proxy2d_shared/systems/state_checksum.h"
#include "../../project_proxy2d_shared/systems/map_spawn_system.h"

struct game_scene {
        scene_p scene;
        Context* ctx;
        float step_timer = 0;
        // 逻辑帧率测量
        uint32_t fps_measure_time = 0;   // 上次统计时刻（ms）
        int      fps_measure_frame = 0;  // 上次统计时的 next_frame_id
};

static void on_load(scene_p s)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        log_info("[game scene] start...");

        debug_layer_p debug_layer = create_debug_layer();
        gameplay_controls_layer_p gameplay_controls_layer = create_gameplay_controls_layer(self->ctx);
        scene_add_root_node(self->scene, debug_layer_get_node(debug_layer));
        scene_add_root_node(self->scene, gameplay_controls_layer_get_node(gameplay_controls_layer));
        self->ctx->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "192.168.2.28", 30000);
        //self->ctx->netclient = netclient_create(NET_CLIENT_WEBSOCKET, "192.168.1.22", 30000);

        // entt 不需要显式注册组件，也不需要 set_ctx
        // 创建 NetComponent 作为 singleton（既作为实体组件，也注册到 registry 上下文）
        self->ctx->singleton_net = self->ctx->ecs.create();
        self->ctx->ecs.emplace<NetComponent>(self->ctx->singleton_net);
        self->ctx->ecs.ctx().emplace<NetComponent>();
        self->ctx->ecs.ctx().emplace<GlobalState>();

        // 注册 system
        add_system(self->ctx, PreUpdate, "input_consume_join", input_consume_join_system);
        add_system(self->ctx, PreUpdate, "input_consume_leave", input_consume_leave_system);
        add_system(self->ctx, PreUpdate, "input_consume_input", input_consume_input_system);
        add_system(self->ctx, OnUpdate, "ai_system", ai_system);
        add_system(self->ctx, OnUpdate, "melee_attack", melee_attack_system);
        add_system(self->ctx, OnUpdate, "player_shoot", player_shoot_system);
        add_system(self->ctx, OnUpdate, "bullet", bullet_system);
        add_system(self->ctx, OnUpdate, "player_death", player_death_system);
        add_system(self->ctx, OnUpdate, "pathfinding", pathfinding_system);
        add_system(self->ctx, OnUpdate,  "movement",           movement_system);
        add_system(self->ctx, OnUpdate,  "collision",          collision_system);
        add_system(self->ctx, OnUpdate,  "cleanup",            cleanup_system);

        // 本地生成地图碰撞体（不再从服务端网络同步）
        add_system(self->ctx, OnStart, "map_spawn", map_spawn_system);

        // 执行 OnStart phase
        run_phase(self->ctx, OnStart, 0);

        // ---- 音频 ----
        self->ctx->audio_dev = audio_open_device();
        self->ctx->step_sound = audio_shot_create("project_proxy2d_client/sounds/grass-step.wav", self->ctx->audio_dev);
        audio_shot_set_gain(self->ctx->step_sound, 1.0f);
        self->ctx->bgm = audio_bgm_create("project_proxy2d_client/sounds/bgm.wav", self->ctx->audio_dev);
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
        else if (strcmp(key, "space") == 0) key_mask = INPUT_ATTACK;
        else if (strcmp(key, "e") == 0) key_mask = INPUT_RANGED;
        else if (strcmp(key, "E") == 0) key_mask = INPUT_RANGED;
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
        else if (strcmp(key, "space") == 0) key_mask = INPUT_ATTACK;
        else if (strcmp(key, "e") == 0) key_mask = INPUT_RANGED;
        else if (strcmp(key, "E") == 0) key_mask = INPUT_RANGED;
        if (key_mask) self->ctx->current_input_mask &= ~key_mask;
}

static void on_handle_event(scene_p s, const void* ev)
{
        game_scene_p self = (game_scene_p)scene_get_userdata(s);

        // 自定义触屏/UI 事件
        if (is_user_event(ev, GAMEPLAY_CONTROL_EVENT)) {
                int new_mask = self->ctx->current_input_mask;
                switch (get_event_code(ev)) {
                case GAMEPLAY_CONTROL_UP:    new_mask |= INPUT_UP; break;
                case GAMEPLAY_CONTROL_DOWN:  new_mask |= INPUT_DOWN; break;
                case GAMEPLAY_CONTROL_LEFT:  new_mask |= INPUT_LEFT; break;
                case GAMEPLAY_CONTROL_RIGHT: new_mask |= INPUT_RIGHT; break;
                case GAMEPLAY_CONTROL_RELEASE_UP:    new_mask &= ~INPUT_UP; break;
                case GAMEPLAY_CONTROL_RELEASE_DOWN:  new_mask &= ~INPUT_DOWN; break;
                case GAMEPLAY_CONTROL_RELEASE_LEFT:  new_mask &= ~INPUT_LEFT; break;
                case GAMEPLAY_CONTROL_RELEASE_RIGHT: new_mask &= ~INPUT_RIGHT; break;
                case GAMEPLAY_CONTROL_ATTACK:
                        self->ctx->attack_triggered = true;
                        break;
                case GAMEPLAY_CONTROL_RANGED:
                        new_mask |= INPUT_RANGED;
                        break;
                case GAMEPLAY_CONTROL_RELEASE_RANGED:
                        new_mask &= ~INPUT_RANGED;
                        break;
                default:
                        new_mask &= ~(INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT);
                        break;
                }
                if (new_mask != self->ctx->current_input_mask) {
                        self->ctx->current_input_mask = new_mask;
                }
                return;
        }

        jevent_callbacks_t cbs = {};
        cbs.on_keypressed = client_on_keypressed;
        cbs.on_keyreleased = client_on_keyreleased;
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
        auto& gs = self->ctx->ecs.ctx().get<GlobalState>();
        while (self->ctx->accumulator >= self->ctx->FIXED_TIMESTEP) {
            self->ctx->accumulator -= self->ctx->FIXED_TIMESTEP;
            send_input(self->ctx);
            gs.next_frame_id++;   // 每跑一个逻辑 tick，帧序号 +1
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
        if (!self->ctx->command_queue.empty()) {
                std::string command_data = self->ctx->command_queue.front();
                self->ctx->command_queue.pop();

                int frame_id, jn = 0, ln = 0, in = 0;
                S2CPlayerJoin joins[32];
                S2CPlayerLeave leaves[32];
                S2CPlayerInput inputs[64];
                char server_checksum[64] = { 0 };
                bool ok = unpack_command(command_data.c_str(), (int)command_data.size(),
                        &frame_id, joins, 32, &jn, leaves, 32, &ln, inputs, 64, &in,
                        server_checksum, sizeof(server_checksum));
                if (ok) {
                        auto& cmd = self->ctx->ecs.ctx().get<NetComponent>();
                        cmd.frame_id = ok ? frame_id : -1;
                        cmd.num_joins = jn;  memcpy(cmd.joins, joins, jn * sizeof(S2CPlayerJoin));
                        cmd.num_leaves = ln; memcpy(cmd.leaves, leaves, ln * sizeof(S2CPlayerLeave));
                        cmd.num_inputs = in; memcpy(cmd.inputs, inputs, in * sizeof(S2CPlayerInput));

                        // 帧同步校验：模拟本帧前，用与服务端相同公式计算本地玩家状态 MD5 并对比
                        std::vector<ChecksumEntry> entries;
                        auto pv = self->ctx->ecs.view<IdComponent, PlayerComponent, LogicPositionComponent>();
                        for (auto e : pv) {
                                auto& idc = pv.get<IdComponent>(e);
                                auto& lpc = pv.get<LogicPositionComponent>(e);
                                entries.push_back({ idc.id, lpc.x, lpc.y });
                        }
                        char local_checksum[64] = { 0 };
                        compute_players_md5(entries, local_checksum, sizeof(local_checksum));
                        if (strcmp(local_checksum, server_checksum) == 0) {
                                log_info("[CHECKSUM] frame=%d OK %s", frame_id, local_checksum);
                        } else {
                                log_error("[CHECKSUM] frame=%d MISMATCH server=%s local=%s",
                                        frame_id, server_checksum, local_checksum);
                        }
                }

                // 统一 system 调度（PreUpdate → OnUpdate → PostUpdate）
                run_phase(self->ctx, PreUpdate, self->ctx->FIXED_TIMESTEP);
                run_phase(self->ctx, OnUpdate, self->ctx->FIXED_TIMESTEP);
                run_phase(self->ctx, PostUpdate, self->ctx->FIXED_TIMESTEP);
        }

        // 视觉相关 system 用实时 dt 每帧更新（不放在固定步长内）
        interpolate_position_system(self->ctx->ecs, self->ctx, dt);
        camera_follow_system(self->ctx->ecs, self->ctx, dt);

        // 视觉时间推进（仅渲染用，驱动"浑天仪式"轨道环自旋/变形，不参与帧同步）
        self->ctx->visual_time += dt;
}

static void on_render(scene_p s) {
        game_scene_p self = (game_scene_p)scene_get_userdata(s);

        // ---- 几何网格背景 + 地形装饰（纯视觉，自绘白底+浅灰网格）----
        draw_grid_background(self->ctx);
        draw_map_tiles(self->ctx);

        // ---- ECS 实体几何渲染（纯视觉，不参与帧同步逻辑）----
        auto draw_view = self->ctx->ecs.view<IdComponent, LogicRectComponent, TransformComponent>();
        for (auto e : draw_view) {
                auto& id = draw_view.get<IdComponent>(e);
                auto& rect = draw_view.get<LogicRectComponent>(e);
                auto& t = draw_view.get<TransformComponent>(e);
                drawing_entity_system(self->ctx, e, id, rect, t);
        }

        // ---- 玩家状态 HUD（仅游戏场景显示）----
        player_status_display_draw(self->ctx->ecs, self->ctx->local_conv, self->ctx->r2d,
                AssetManager::GetInstance()->GetSimheiFont24());

        // ---- 当前逻辑帧号 + 实测逻辑帧率（帧同步调试信息）----
        int frame_id = self->ctx->ecs.ctx().get<NetComponent>().frame_id;
        int logic_fps = self->ctx->ecs.ctx().get<GlobalState>().logic_fps;
        frame_display_draw(self->ctx->r2d, AssetManager::GetInstance()->GetSimheiFont12(),
                frame_id, logic_fps, 8.0f, 60.0f);
}

static void on_destroy(scene_p s) {
        game_scene_p self = (game_scene_p)scene_get_userdata(s);
        if (self->ctx->netclient) {
                netclient_destroy(self->ctx->netclient);
                self->ctx->netclient = NULL;
        }
        AssetManager::GetInstance()->tex_cache_clear();
        audio_shot_destroy(self->ctx->step_sound);
        audio_bgm_destroy(self->ctx->bgm);
        if (self->ctx->audio_dev) audio_close_device(self->ctx->audio_dev);
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
