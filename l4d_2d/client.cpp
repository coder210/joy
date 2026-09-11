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

// ============================================================
// 键盘事件回调（客户端/Host/单机）
// ============================================================
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
    else if (strcmp(key, "e") == 0 || strcmp(key, "E") == 0) { self->ctx->bullet_triggered = true; return; }
    else if (strcmp(key, "l") == 0 || strcmp(key, "L") == 0) { self->ctx->ray_triggered = true; return; }
    if (key_mask) {
        self->ctx->current_input_mask |= key_mask;
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
    else if (strcmp(key, "e") == 0 || strcmp(key, "E") == 0) { self->ctx->bullet_triggered = true; return; }
    else if (strcmp(key, "l") == 0 || strcmp(key, "L") == 0) { self->ctx->ray_triggered = true; return; }
    if (key_mask) self->ctx->current_input_mask &= ~key_mask;
}

// ============================================================
// 单机模式 (MODE_SINGLE)
// ============================================================
static void single_init(game_scene_p self)
{
    Context* ctx = self->ctx;
    // 权威端系统
    add_system(ctx, OnStart, "spawn_enemy", spawn_enemy_system);
    add_system(ctx, PostUpdate, "snapshot", snapshot_system);
    auto& gs = ctx->ecs.ctx().get<GlobalState>();
    gs.random_seed = (int)(get_ticks() ^ ((uintptr_t)&gs & 0xFFFF));
    gs.random = lcg_random_create((uint64_t)(uint32_t)gs.random_seed);

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

static void single_handle_event(game_scene_p self, const void* ev)
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
    cbs.on_keypressed = client_on_keypressed;
    cbs.on_keyreleased = client_on_keyreleased;
    dispatch_event(cbs, ev, self);
}

static void single_network_update(game_scene_p self)
{
    // 单机无网络
}

static void single_tick(game_scene_p self)
{
    Context* ctx = self->ctx;
    auto& gs = ctx->ecs.ctx().get<GlobalState>();
    int frame_id = gs.next_frame_id++;
    collect_local_input(ctx);
    collect_input(ctx, frame_id);
}

static void single_render(game_scene_p self)
{
    // 单机无额外渲染（HUD 由 game_scene.cpp 统一处理）
}

static void single_destroy(game_scene_p self)
{
    Context* ctx = self->ctx;
    if (ctx->step_sound) { audio_shot_destroy(ctx->step_sound); ctx->step_sound = NULL; }
    if (ctx->bgm) { audio_bgm_destroy(ctx->bgm); ctx->bgm = NULL; }
    if (ctx->audio_dev) { audio_close_device(ctx->audio_dev); ctx->audio_dev = 0; }
}

// ============================================================
// 客户端模式 (MODE_CLIENT)
// ============================================================
static void client_init(game_scene_p self)
{
    Context* ctx = self->ctx;
    // 网络客户端
    ctx->netclient = netclient_create(NET_CLIENT_WEBSOCKET, ctx->settings.server_ip, ctx->settings.server_port);

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

static void client_handle_event(game_scene_p self, const void* ev)
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
    cbs.on_keypressed = client_on_keypressed;
    cbs.on_keyreleased = client_on_keyreleased;
    dispatch_event(cbs, ev, self);
}

static void client_network_update(game_scene_p self)
{
    Context* ctx = self->ctx;
    if (ctx->netclient) {
        netclient_update(ctx->netclient);
        client_network_receive(ctx);
    }
}

static void client_tick(game_scene_p self)
{
    Context* ctx = self->ctx;
    send_input(ctx);
}

static void client_render(game_scene_p self)
{
    // 客户端无额外渲染
}

static void client_destroy(game_scene_p self)
{
    Context* ctx = self->ctx;
    if (ctx->step_sound) { audio_shot_destroy(ctx->step_sound); ctx->step_sound = NULL; }
    if (ctx->bgm) { audio_bgm_destroy(ctx->bgm); ctx->bgm = NULL; }
    if (ctx->audio_dev) { audio_close_device(ctx->audio_dev); ctx->audio_dev = 0; }
}

// ============================================================
// 接口导出
// ============================================================
GameModeInterface g_single_mode = {
    single_init,
    single_handle_event,
    single_network_update,
    single_tick,
    single_render,
    single_destroy,
};

GameModeInterface g_client_mode = {
    client_init,
    client_handle_event,
    client_network_update,
    client_tick,
    client_render,
    client_destroy,
};