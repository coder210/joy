#include <map>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <joy/joy.h>
#include "game_scene.h"
#include "../game_mode.h"
#include "../protocol.h"
#include "../passes/interpolate_pass.h"
#include "../passes/drawing_entity_pass.h"
#include "../systems/input_consume_join_system.h"
#include "../passes/player_status_hud_pass.h"
#include "../passes/frame_display_pass.h"
#include "../systems/input_consume_leave_system.h"
#include "../systems/input_consume_input_system.h"
#include "../systems/movement_system.h"
#include "../systems/cleanup_system.h"
#include "../systems/collision_system.h"
#include "../layers/debug_layer.h"
#include "../asset_manager.h"
#include "../network/network_utils.h"
#include "../passes/camera_follow_pass.h"
#include "../systems/ai_system.h"
#include "../systems/melee_attack_system.h"
#include "../systems/player_shoot_system.h"
#include "../systems/player_ray_system.h"
#include "../systems/bullet_system.h"
#include "../systems/player_death_system.h"
#include "../systems/pathfinding_system.h"

#include "../passes/drawing_map_pass.h"
#include "../passes/drawing_collision_debug_pass.h"
#include "../systems/state_checksum.h"
#include "../systems/map_spawn_system.h"
#include "../passes/fog_pass_2d.h"
#include "../passes/drawing_minimap_pass.h"
#include "victory_scene.h"

static GameModeInterface* g_mode = NULL;

static void on_load(scene_p s)
{
    game_scene_p self = (game_scene_p)scene_get_userdata(s);
    Context* ctx = self->ctx;
    log_info("[game scene] start... mode=%d", (int)ctx->mode);

    // 选择模式接口
    switch (ctx->mode) {
    case MODE_SINGLE: g_mode = &g_single_mode; break;
    case MODE_CLIENT: g_mode = &g_client_mode; break;
    case MODE_HOST:   g_mode = &g_host_mode;   break;
    case MODE_SERVER: g_mode = &g_server_mode; break;
    }

    debug_layer_p debug_layer = create_debug_layer();
    scene_add_root_node(self->scene, debug_layer_get_node(debug_layer));

    // 注册全局单例
    ctx->ecs.ctx().emplace<NetComponent>();
    auto& gs = ctx->ecs.ctx().emplace<GlobalState>();
    gs.level = ctx->selected_level;   // 应用选关页选定的固定地图关卡
    map_load_level(gs.level);         // 装入当前关卡 64×64 ASCII 网格（墙+出生）

    // 注册公共系统
    add_system(ctx, OnStart, "map_spawn", map_spawn_system);
    add_system(ctx, PreUpdate, "input_consume_join", input_consume_join_system);
    add_system(ctx, PreUpdate, "input_consume_leave", input_consume_leave_system);
    add_system(ctx, PreUpdate, "input_consume_input", input_consume_input_system);
    add_system(ctx, OnUpdate, "ai_system", ai_system);
    add_system(ctx, OnUpdate, "melee_attack", melee_attack_system);
    add_system(ctx, OnUpdate, "player_shoot", player_shoot_system);
    add_system(ctx, OnUpdate, "player_ray", player_ray_system);
    add_system(ctx, OnUpdate, "bullet", bullet_system);
    add_system(ctx, OnUpdate, "player_death", player_death_system);
    add_system(ctx, OnUpdate, "pathfinding", pathfinding_system);
    add_system(ctx, OnUpdate, "movement", movement_system);
    add_system(ctx, OnUpdate, "collision", collision_system);
    add_system(ctx, OnUpdate, "cleanup", cleanup_system);

    // 执行 OnStart phase（生成地图）
    run_phase(ctx, OnStart, 0);

    // 模式特有初始化
    g_mode->init(self);
}

static void on_handle_event(scene_p s, const void* ev)
{
    game_scene_p self = (game_scene_p)scene_get_userdata(s);
    if (g_mode) g_mode->handle_event(self, ev);
}

static void on_update(scene_p s, float dt)
{
    static uint32_t s_diag_t0 = 0;
    static int     s_diag_frames = 0, s_diag_ticks = 0;
    static float   s_diag_dtsum = 0;
    game_scene_p self = (game_scene_p)scene_get_userdata(s);
    Context* ctx = self->ctx;

    // 网络层更新（模式特有）
    g_mode->network_update(self);

    // 固定步长逻辑 Tick（20Hz）
    ctx->accumulator += dt;
    auto& gs = ctx->ecs.ctx().get<GlobalState>();
    // ---- 临时诊断：每秒打点 dt 累计 / 渲染帧数 / 实际逻辑 tick 数 ----
    if (get_ticks() - s_diag_t0 >= 1000) {
        log_info("[diag] framerate=%d dt_sum=%.4f ticks=%d logic=%d",
            s_diag_frames, s_diag_dtsum, s_diag_ticks, gs.logic_fps);
        s_diag_frames = 0; s_diag_dtsum = 0; s_diag_ticks = 0; s_diag_t0 = get_ticks();
    }
    s_diag_frames++; s_diag_dtsum += dt;
    while (ctx->accumulator >= ctx->FIXED_TIMESTEP) {
        ctx->accumulator -= ctx->FIXED_TIMESTEP;
        s_diag_ticks++;

        // 模式特有 tick：帧号分配、输入收集、命令打包/转发、输入发送
        g_mode->tick(self);

        // 消费一条命令帧（模拟前先写 NetComponent）
        bool got_command = false;
        if (!ctx->command_queue.empty()) {
            std::string command_data = ctx->command_queue.front();
            ctx->command_queue.pop();

            int frame_id, jn = 0, ln = 0, in = 0;
            S2CPlayerJoin joins[32];
            S2CPlayerLeave leaves[32];
            S2CPlayerInput inputs[64];
            char server_checksum[64] = { 0 };
            bool ok = unpack_command(command_data.c_str(), (int)command_data.size(),
                &frame_id, joins, 32, &jn, leaves, 32, &ln, inputs, 64, &in,
                server_checksum, sizeof(server_checksum));
            if (ok) {
                // 客户端跳过 <= server_frameid 的帧
                if (ctx->mode == MODE_CLIENT && frame_id <= ctx->server_frameid) {
                    // 跳过，不模拟
                } else {
                    got_command = true;
                    auto& cmd = ctx->ecs.ctx().get<NetComponent>();
                    cmd.frame_id = frame_id;
                    cmd.num_joins = jn;  memcpy(cmd.joins, joins, jn * sizeof(S2CPlayerJoin));
                    cmd.num_leaves = ln; memcpy(cmd.leaves, leaves, ln * sizeof(S2CPlayerLeave));
                    cmd.num_inputs = in; memcpy(cmd.inputs, inputs, in * sizeof(S2CPlayerInput));

                    // 帧同步校验（客户端/单机）
                    if (ctx->mode == MODE_CLIENT || ctx->mode == MODE_SINGLE) {
                        std::vector<ChecksumEntry> entries;
                        auto pv = ctx->ecs.view<IdComponent, LogicPositionComponent>();
                        for (auto e : pv) {
                            auto& idc = pv.get<IdComponent>(e);
                            auto& lpc = pv.get<LogicPositionComponent>(e);
                            entries.push_back({ idc.id, lpc.x, lpc.y });
                        }
                        char local_checksum[64] = { 0 };
                        compute_full_md5(entries, local_checksum, sizeof(local_checksum));

                        std::string input_finger;
                        for (int k = 0; k < in; k++) {
                            char b[64];
                            snprintf(b, sizeof(b), "%d:%d:%d:%d:%d;",
                                inputs[k].conv, inputs[k].dir,
                                (int)inputs[k].attack, (int)inputs[k].ranged,
                                (int)inputs[k].ray);
                            input_finger += b;
                        }

                        int mismatched = (strcmp(local_checksum, server_checksum) != 0) ? 1 : 0;
                        ctx->checksum_records.push_back(
                            { frame_id, "", input_finger,
                              checksum_entries_string(entries) });
                        auto& rec = ctx->checksum_records.back();
                        memcpy(rec.checksum, local_checksum, sizeof(rec.checksum));
                        if (mismatched) {
                            printf("[CHECKSUM] frame=%d MISMATCH server=%s local=%s inputs=%s entities=%s\n",
                                frame_id, server_checksum, local_checksum,
                                input_finger.c_str(), checksum_entries_string(entries).c_str());
                            fflush(stdout);
                        }
                    }
                }
            }
        }

        // 模拟推进
        if (ctx->mode != MODE_CLIENT || got_command) {
            run_phase(ctx, PreUpdate, ctx->FIXED_TIMESTEP);
            run_phase(ctx, OnUpdate, ctx->FIXED_TIMESTEP);
            run_phase(ctx, PostUpdate, ctx->FIXED_TIMESTEP);
        }
    }

    // 逻辑帧率测量
    // 客户端（MODE_CLIENT）本端 gs.next_frame_id 不递增（由服务器权威帧号 NetComponent.frame_id 驱动），
    // 直接用它度量会恒为 0；因此客户端改用 NetComponent.frame_id，其余模式仍用本端 next_frame_id。
    uint32_t now_ms = get_ticks();
    int cur_logic_frame = (ctx->mode == MODE_CLIENT)
        ? ctx->ecs.ctx().get<NetComponent>().frame_id
        : gs.next_frame_id;
    if (self->fps_measure_time == 0) {
        self->fps_measure_time  = now_ms;
        self->fps_measure_frame = cur_logic_frame;
    } else if (now_ms - self->fps_measure_time >= 1000) {
        int elapsed_frames = cur_logic_frame - self->fps_measure_frame;
        if (elapsed_frames < 0) elapsed_frames = 0;
        gs.logic_fps = (int)(elapsed_frames * 1000 / (now_ms - self->fps_measure_time));
        self->fps_measure_time  = now_ms;
        self->fps_measure_frame = cur_logic_frame;
    }

    // 视觉相关 pass 用实时 dt 每帧更新
    interpolate_pass(ctx->ecs, ctx, dt);
    camera_follow_pass(ctx->ecs, ctx, dt);

    // 视觉时间推进
    ctx->visual_time += dt;

    // ---- 通关判定（与 3D game_scene 业务逻辑对齐）----
    // 关卡开始后，检测全部敌人被肃清则延时切入胜利场景（非服务端）。
    // 优先取权威字段 level_cleared；兜底用本地实体状态推导（帧同步实体同步，跨端确定性一致）。
    if (!self->game_started) {
        auto en0 = ctx->ecs.view<EnemyComponent>();
        if (en0.begin() != en0.end()) self->game_started = true;
    }
    if (!self->victory_started && ctx->mode != MODE_SERVER &&
        ctx->ecs.ctx().contains<GlobalState>()) {
        auto& gsv = ctx->ecs.ctx().get<GlobalState>();
        bool cleared = gsv.level_cleared;
        if (!cleared && self->game_started) {
            cleared = true;
            auto ev2 = ctx->ecs.view<EnemyComponent, HealthComponent>();
            for (auto e : ev2) {
                if (!ev2.get<HealthComponent>(e).dead) { cleared = false; break; }
            }
        }
        if (cleared) {
            gsv.level_cleared = true;
            self->victory_delay += dt;
            if (self->victory_delay >= 1.2f) {
                self->victory_started = true;
                ctx->pending_scene = victory_scene_get_scene(victory_scene_create(ctx));
                push_user_event(VICTORY_EVENT, VICTORY_EVENT_TRIGGERED);
            }
        }
    }
}

static void on_render(scene_p s)
{
    game_scene_p self = (game_scene_p)scene_get_userdata(s);
    Context* ctx = self->ctx;

    // 几何网格背景 + 地形装饰
    draw_grid_background(ctx);
    draw_map_pass(ctx);

    // ECS 实体几何渲染
    auto draw_view = ctx->ecs.view<IdComponent, LogicRectComponent, TransformComponent>();
    for (auto e : draw_view) {
        auto& id = draw_view.get<IdComponent>(e);
        auto& rect = draw_view.get<LogicRectComponent>(e);
        auto& t = draw_view.get<TransformComponent>(e);
        drawing_entity_pass(ctx, e, id, rect, t);
    }

    // 2D 战争迷雾：覆盖地图与实体，仅暴露视野附近，墙后与远端被墨雾遮蔽
    fog_pass_2d(ctx);

    // 玩家碰撞盒（淡墨调试可视化）
    drawing_collision_debug_pass(ctx);

    // 模式特有渲染（碰撞调试、HUD 等）
    if (g_mode) g_mode->render(self);

    // 玩家状态 HUD（本地有玩家的模式显示）
    if (ctx->mode != MODE_SERVER) {
        player_status_hud_pass(ctx->ecs, ctx->local_conv, ctx->r2d,
            AssetManager::GetInstance()->GetSimheiFont28());
    }

    // 当前逻辑帧号 + 实测逻辑帧率 + 敌人数量 + 全局实体 id
    int frame_id = (ctx->mode == MODE_SERVER || ctx->mode == MODE_HOST)
        ? ctx->ecs.ctx().get<GlobalState>().next_frame_id - 1
        : ctx->ecs.ctx().get<NetComponent>().frame_id;
    int logic_fps = ctx->ecs.ctx().get<GlobalState>().logic_fps;
    int enemy_count = (int)ctx->ecs.view<EnemyComponent>().size();
    int entity_id = ctx->ecs.ctx().get<GlobalState>().next_entity_id;
    frame_display_pass(ctx->r2d, AssetManager::GetInstance()->GetSimheiFont28(),
        frame_id, logic_fps, 8.0f, 60.0f, enemy_count, entity_id);

    // 水墨风小地图（右上角宣纸底）
    drawing_minimap_pass(ctx);
}

static void on_destroy(scene_p s)
{
    game_scene_p self = (game_scene_p)scene_get_userdata(s);
    Context* ctx = self->ctx;

    // 网络清理
    if (ctx->netclient) {
        netclient_destroy(ctx->netclient);
        ctx->netclient = NULL;
    }
    if (ctx->net_server) {
        netserver_destroy(ctx->net_server);
        ctx->net_server = NULL;
    }
    AssetManager::GetInstance()->tex_cache_clear();

    // 模式特有清理（音频等）
    if (g_mode) g_mode->destroy(self);

    // 退出时统一落盘帧同步校验记录
    if (!ctx->checksum_records.empty()) {
        const char* log_name = (ctx->mode == MODE_SERVER || ctx->mode == MODE_HOST)
            ? "checksum_log_server.txt" : "checksum_log.txt";
        FILE* fp = fopen(log_name, "w");
        if (fp) {
            fprintf(fp, "frame\tchecksum\tinputs\tentities\n");
            for (auto& r : ctx->checksum_records) {
                fprintf(fp, "%d\t%s\t%s\t%s\n",
                    r.frame_id, r.checksum,
                    r.inputs.c_str(), r.entities.c_str());
            }
            fclose(fp);
        }
    }
    delete self;
}

game_scene_p game_scene_create(Context* ctx)
{
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

scene_p game_scene_get_scene(game_scene_p s)
{
    return s ? s->scene : NULL;
}