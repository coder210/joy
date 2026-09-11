#include <cstring>
#include <cstdio>
#include <joy/joy.h>
#include "game_mode.h"
#include "scenes/game_scene.h"
#include "context.h"
#include "protocol.h"
#include "layers/gameplay_controls_layer.h"
#include "network/network_utils.h"
#include "systems/spawn_enemy_system.h"
#include "systems/snapshot_system.h"
#include "passes/drawing_collision_debug_pass.h"

// ============================================================
// 键盘事件回调（服务端/Host）
// ============================================================
static void host_on_keypressed(const char* key, int scancode, bool repeat, bool ctrl, bool shift, bool alt, void* arg)
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
    else if (strcmp(key, "e") == 0 || strcmp(key, "E") == 0) { self->ctx->bullet_triggered = true; return; }
    else if (strcmp(key, "l") == 0 || strcmp(key, "L") == 0) { self->ctx->ray_triggered = true; return; }
    if (key_mask) {
        self->ctx->current_input_mask |= key_mask;
        if (key_mask & INPUT_DIR_MASK)
            self->ctx->local_dir = input_to_dir(key_mask);
    }
}

static void host_on_keyreleased(const char* key, int scancode, void* arg)
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
    else if (strcmp(key, "e") == 0 || strcmp(key, "E") == 0) { self->ctx->bullet_triggered = true; return; }
    else if (strcmp(key, "l") == 0 || strcmp(key, "L") == 0) { self->ctx->ray_triggered = true; return; }
    if (key_mask) self->ctx->current_input_mask &= ~key_mask;
}

static void server_on_keypressed(const char* key, int scancode, bool repeat, bool ctrl, bool shift, bool alt, void* arg)
{
    (void)arg;
    if (strcmp(key, "q") == 0 || strcmp(key, "Q") == 0) {
        push_quit_event();
    }
}

// ============================================================
// Host 模式 (MODE_HOST)
// ============================================================
static void host_init(game_scene_p self)
{
    Context* ctx = self->ctx;
    // 权威端系统
    add_system(ctx, OnStart, "spawn_enemy", spawn_enemy_system);
    add_system(ctx, PostUpdate, "snapshot", snapshot_system);
    auto& gs = ctx->ecs.ctx().get<GlobalState>();
    gs.random_seed = (int)(get_ticks() ^ ((uintptr_t)&gs & 0xFFFF));
    gs.random = lcg_random_create((uint64_t)(uint32_t)gs.random_seed);

    // 网络服务端
    ctx->net_server = netserver_create(NET_SERVER_WEBSOCKET, ctx->settings.server_ip, ctx->settings.server_port);

    // 注入本地玩家
    inject_local_join(ctx);

    // 操作层
    gameplay_controls_layer_p gcl = create_gameplay_controls_layer(ctx);
    scene_add_root_node(self->scene, gameplay_controls_layer_get_node(gcl));

    // 音频
    ctx->audio_dev = audio_open_device();
    ctx->step_sound = audio_shot_create("l4d_2d/sounds/grass-step.wav", ctx->audio_dev);
    audio_shot_set_gain(ctx->step_sound, ctx->settings.sfx_volume);
    ctx->bgm = audio_bgm_create("l4d_2d/sounds/bgm.wav", ctx->audio_dev);
    audio_bgm_set_gain(ctx->bgm, ctx->settings.bgm_volume);
}

static void host_handle_event(game_scene_p self, const void* ev)
{
    Context* ctx = self->ctx;
    // 操作层事件
    if (is_user_event(ev, GAMEPLAY_CONTROL_EVENT)) {
        int new_mask = ctx->current_input_mask;
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
            ctx->attack_triggered = true;
            break;
        case GAMEPLAY_CONTROL_RANGED:
            ctx->bullet_triggered = true;
            break;
        case GAMEPLAY_CONTROL_RAY:
            ctx->ray_triggered = true;
            break;
        default:
            new_mask &= ~(INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT);
            break;
        }
        if (new_mask != ctx->current_input_mask) {
            ctx->current_input_mask = new_mask;
        }
        return;
    }
    // 键盘事件
    jevent_callbacks_t cbs = {};
    cbs.on_keypressed = host_on_keypressed;
    cbs.on_keyreleased = host_on_keyreleased;
    dispatch_event(cbs, ev, self);
}

static void host_network_update(game_scene_p self)
{
    Context* ctx = self->ctx;
    if (ctx->net_server) {
        netserver_update(ctx->net_server);
        server_network_receive(ctx);
    }
}

static void host_tick(game_scene_p self)
{
    Context* ctx = self->ctx;
    auto& gs = ctx->ecs.ctx().get<GlobalState>();
    int frame_id = gs.next_frame_id++;
    collect_local_input(ctx);
    collect_input(ctx, frame_id);
    input_forwarding(ctx);
}

static void host_render(game_scene_p self)
{
    // 碰撞调试红框
    drawing_collision_debug_pass(self->ctx);
}

static void host_destroy(game_scene_p self)
{
    Context* ctx = self->ctx;
    if (ctx->step_sound) { audio_shot_destroy(ctx->step_sound); ctx->step_sound = NULL; }
    if (ctx->bgm) { audio_bgm_destroy(ctx->bgm); ctx->bgm = NULL; }
    if (ctx->audio_dev) { audio_close_device(ctx->audio_dev); ctx->audio_dev = 0; }
}

// ============================================================
// 服务端模式 (MODE_SERVER)
// ============================================================
static void server_init(game_scene_p self)
{
    Context* ctx = self->ctx;
    // 权威端系统
    add_system(ctx, OnStart, "spawn_enemy", spawn_enemy_system);
    add_system(ctx, PostUpdate, "snapshot", snapshot_system);
    auto& gs = ctx->ecs.ctx().get<GlobalState>();
    gs.random_seed = (int)(get_ticks() ^ ((uintptr_t)&gs & 0xFFFF));
    gs.random = lcg_random_create((uint64_t)(uint32_t)gs.random_seed);

    // 网络服务端
    ctx->net_server = netserver_create(NET_SERVER_WEBSOCKET, ctx->settings.server_ip, ctx->settings.server_port);
}

static void server_handle_event(game_scene_p self, const void* ev)
{
    // 服务端只响应退出键
    jevent_callbacks_t cbs = {};
    cbs.on_keypressed = server_on_keypressed;
    dispatch_event(cbs, ev, NULL);
}

static void server_network_update(game_scene_p self)
{
    Context* ctx = self->ctx;
    if (ctx->net_server) {
        netserver_update(ctx->net_server);
        server_network_receive(ctx);
    }
}

static void server_tick(game_scene_p self)
{
    Context* ctx = self->ctx;
    auto& gs = ctx->ecs.ctx().get<GlobalState>();
    int frame_id = gs.next_frame_id++;
    collect_input(ctx, frame_id);
    input_forwarding(ctx);
}

static void server_render(game_scene_p self)
{
    // 碰撞调试红框
    drawing_collision_debug_pass(self->ctx);
}

static void server_destroy(game_scene_p self)
{
    // 服务端无音频需要清理
}

// ============================================================
// 接口导出
// ============================================================
GameModeInterface g_host_mode = {
    host_init,
    host_handle_event,
    host_network_update,
    host_tick,
    host_render,
    host_destroy,
};

GameModeInterface g_server_mode = {
    server_init,
    server_handle_event,
    server_network_update,
    server_tick,
    server_render,
    server_destroy,
};