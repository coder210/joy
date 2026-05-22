// main11.cpp — Tilemap 测试 + 玩家碰撞 + janimation
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include "SDL3/SDL.h"
#include <joy/jtilemap.h>
#include <joy/jtilemap_render.h>
#include <joy/janimations.h>
#include <joy/jcore.h>

static const int WINDOW_W = 1024;
static const int WINDOW_H = 760;

static SDL_Window*   gWindow  = nullptr;
static SDL_Renderer* gRenderer = nullptr;
static jtilemap_p    gTilemap = nullptr;
static bool          gRunning = true;

static game_timer_t         gTimer;
static simple_fps_counter_p gFps = nullptr;

// ======================== 玩家 ========================
struct Player {
    float x, y;
    float vx, vy;
    float w, h;        // 碰撞体积
    float speed;
    float vis_w, vis_h; // 视觉尺寸（原生帧大小）
} gPlayer = { 300, 300, 0, 0, 36, 52, 200.0f, 192, 192 };

static sprite_animation_p gWarriorWalk[4] = {}; // 4 方向行走
static sprite_animation_p gWarriorIdle[4] = {}; // 4 方向待机
static int gPlayerDir = 0; // 0=下 1=左 2=右 3=上
static const char* gSpritePath = "joy2d_editor_textures/knights/troops/warrior/warrior_blue.png";

// ======================== 碰撞墙体（缓存 collision 层） ========================
struct Wall { float x, y, w, h; };
static std::vector<Wall> gWalls;

static void cache_walls(jtilemap_p tm)
{
    jtilemap_object_layer_p ol = jtilemap_find_object_layer(tm, "collision");
    if (!ol) { log_info("no collision layer found"); return; }
    for (size_t i = 0; i < kv_size(ol->objects); i++) {
        jtilemap_object_t* obj = &kv_A(ol->objects, i);
        if (obj->shape == JTILEMAP_SHAPE_RECT && obj->visible)
            gWalls.push_back({ obj->x, obj->y, obj->width, obj->height });
    }
    log_info("cached %zu collision walls", gWalls.size());
}

// AABB 碰撞检测
static bool overlap(float ax, float ay, float aw, float ah,
                     float bx, float by, float bw, float bh)
{
    return ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by;
}

// ======================== 初始化动画（warrior_blue） ========================
// warrio_blue.tsx: 行 0=idle, 行 1=run, 行 2-7=各种方向攻击
// 左右方向通过水平翻转实现（角色默认面朝右）
static void init_player_anim()
{
    float dur = 0.15f;
    const int FW = 192, FH = 192;

    // --- 待机（行 0，idle，6 帧） ---
    for (int d = 0; d < 4; d++)
        gWarriorIdle[d] = sprite_animation_create(gRenderer);
    for (int c = 0; c < 6; c++) {
        SDL_FRect src = { (float)(c * FW), 0, (float)FW, (float)FH };
        for (int d = 0; d < 4; d++)
            sprite_animation_add_clip(gWarriorIdle[d], gSpritePath, dur, src);
    }
    for (int d = 0; d < 4; d++)
        sprite_animation_set_scale(gWarriorIdle[d], 1.0f, 1.0f);

    // --- 跑步（行 1，run，6 帧） ---
    for (int d = 0; d < 4; d++)
        gWarriorWalk[d] = sprite_animation_create(gRenderer);
    for (int c = 0; c < 6; c++) {
        SDL_FRect src = { (float)(c * FW), (float)FH, (float)FW, (float)FH };
        for (int d = 0; d < 4; d++)
            sprite_animation_add_clip(gWarriorWalk[d], gSpritePath, dur, src);
    }
    for (int d = 0; d < 4; d++)
        sprite_animation_set_scale(gWarriorWalk[d], 1.0f, 1.0f);
}

// ======================== 碰撞解析（先 X 后 Y） ========================
static void resolve_collision(float& px, float& py)
{
    for (auto& w : gWalls) {
        if (!overlap(px, py, gPlayer.w, gPlayer.h, w.x, w.y, w.w, w.h))
            continue;

        // 计算四个方向的穿透深度
        float overlap_l = (px + gPlayer.w) - w.x;         // 从左边推出
        float overlap_r = (w.x + w.w) - px;               // 从右边推出
        float overlap_t = (py + gPlayer.h) - w.y;         // 从上面推出
        float overlap_b = (w.y + w.h) - py;               // 从下面推出

        // 找最小的穿透方向推出
        float min_x = overlap_l < overlap_r ? overlap_l : overlap_r;
        float min_y = overlap_t < overlap_b ? overlap_t : overlap_b;

        if (min_x < min_y) {
            px += (overlap_l < overlap_r) ? -overlap_l : overlap_r;
        } else {
            py += (overlap_t < overlap_b) ? -overlap_t : overlap_b;
        }
    }
}

// ======================== 回调 ========================
static bool app_init(void)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) { log_error("SDL_Init: %s", SDL_GetError()); return false; }
    if (!SDL_CreateWindowAndRenderer("tilemap", WINDOW_W, WINDOW_H, SDL_WINDOW_RESIZABLE, &gWindow, &gRenderer)) {
        log_error("SDL_Window: %s", SDL_GetError()); SDL_Quit(); return false;
    }

    { const char* base = SDL_GetBasePath();
      std::string path = (base ? base : "") + std::string("joy2d_editor_tilemap/map.lua");
      gTilemap = jtilemap_load_file(path.c_str()); }
    if (!gTilemap) { log_error("failed to load tilemap"); return false; }
    log_info("tilemap: %dx%d tilesets=%zu", gTilemap->width, gTilemap->height, kv_size(gTilemap->tilesets));

    jtilemap_render_init(gRenderer, NULL, NULL);

    // 缓存碰撞墙体
    cache_walls(gTilemap);

    // 创建玩家动画
    init_player_anim();

    game_timer_init(&gTimer); game_timer_set_target_fps(&gTimer, 60);
    gFps = simple_fps_create();
    return true;
}

static bool app_event(const SDL_Event* e)
{
    if (e->type == SDL_EVENT_QUIT) return false;
    if (e->type == SDL_EVENT_KEY_DOWN) {
        auto sc = e->key.scancode;
        if (sc == SDL_SCANCODE_ESCAPE || sc == SDL_SCANCODE_Q) return false;
        if (sc == SDL_SCANCODE_W)       gPlayer.vy = -1;
        else if (sc == SDL_SCANCODE_S)  gPlayer.vy = 1;
        else if (sc == SDL_SCANCODE_A)  gPlayer.vx = -1;
        else if (sc == SDL_SCANCODE_D)  gPlayer.vx = 1;
        else if (sc == SDL_SCANCODE_UP)       gPlayer.vy = -1;
        else if (sc == SDL_SCANCODE_DOWN)     gPlayer.vy = 1;
        else if (sc == SDL_SCANCODE_LEFT)     gPlayer.vx = -1;
        else if (sc == SDL_SCANCODE_RIGHT)    gPlayer.vx = 1;
    }
    if (e->type == SDL_EVENT_KEY_UP) {
        auto sc = e->key.scancode;
        if (sc == SDL_SCANCODE_W || sc == SDL_SCANCODE_UP)       gPlayer.vy = 0;
        else if (sc == SDL_SCANCODE_S || sc == SDL_SCANCODE_DOWN)  gPlayer.vy = 0;
        else if (sc == SDL_SCANCODE_A || sc == SDL_SCANCODE_LEFT)  gPlayer.vx = 0;
        else if (sc == SDL_SCANCODE_D || sc == SDL_SCANCODE_RIGHT) gPlayer.vx = 0;
    }
    return true;
}

static bool app_iterate(void)
{
    game_timer_update(&gTimer);
    simple_fps_update(gFps);
    float dt = game_timer_get_unscaled_delta_time(&gTimer);

    // ---- 玩家移动 + 碰撞 ----
    float nx = gPlayer.x + gPlayer.vx * gPlayer.speed * dt;
    float ny = gPlayer.y + gPlayer.vy * gPlayer.speed * dt;
    resolve_collision(nx, ny);
    gPlayer.x = nx; gPlayer.y = ny;

    // ---- 摄像机跟随玩家视觉中心 ----
    float cam_x = gPlayer.x + gPlayer.vis_w / 2.0f - WINDOW_W / 2.0f;
    float cam_y = gPlayer.y + gPlayer.vis_h / 2.0f - WINDOW_H / 2.0f;

    // 四个朝向都用 idle/run 动画，左右通过翻转实现
    if (gPlayer.vx < 0)            gPlayerDir = 1; // 左（翻转）
    else if (gPlayer.vx > 0)       gPlayerDir = 2; // 右（正常）
    else if (gPlayer.vy < 0)       gPlayerDir = 3; // 上
    else if (gPlayer.vy > 0)       gPlayerDir = 0; // 下

    float len2 = gPlayer.vx * gPlayer.vx + gPlayer.vy * gPlayer.vy;
    int idx = (len2 > 0) ? 0 : 1; // 0=walk, 1=idle
    sprite_animation_p arr[2] = { gWarriorWalk[0], gWarriorIdle[0] };
    sprite_animation_p cur = arr[idx];
    // 水平翻转（左方向用负 scale）
    float sx = (gPlayerDir == 1) ? -1.0f : 1.0f;
    sprite_animation_set_scale(cur, sx, 1.0f);

    if (len2 > 0) sprite_animation_update(cur, dt);
    else          sprite_animation_reset(cur);

    float screen_x = gPlayer.x - cam_x - (gPlayer.vis_w - gPlayer.w) / 2.0f;
    float screen_y = gPlayer.y - cam_y - (gPlayer.vis_h - gPlayer.h) / 2.0f;
    sprite_animation_set_position(cur, screen_x, screen_y);

    // ---- 渲染 ----
    SDL_SetRenderDrawColor(gRenderer, 71, 171, 169, 255);
    SDL_RenderClear(gRenderer);

    jtilemap_render_all(gTilemap, cam_x, cam_y, SDL_GetTicks());

    // 绘制玩家（始终用 idle/run，各方向共用）
    sprite_animation_p cur2 = (gPlayer.vx != 0 || gPlayer.vy != 0) ? gWarriorWalk[0] : gWarriorIdle[0];
    // 左方向负 scale 已在更新时设置
    sprite_animation_draw(cur2, nullptr);

    { char t[128]; snprintf(t, sizeof(t), "tilemap - FPS:%d pos:%.0f,%.0f",
                             simple_fps_value(gFps), gPlayer.x, gPlayer.y);
      SDL_SetWindowTitle(gWindow, t); }
    SDL_RenderPresent(gRenderer);
    return true;
}

static void app_quit(void)
{
    for (int i = 0; i < 4; i++) {
        if (gWarriorWalk[i]) sprite_animation_destroy(gWarriorWalk[i]);
        if (gWarriorIdle[i]) sprite_animation_destroy(gWarriorIdle[i]);
    }
    jtilemap_render_destroy();
    if (gTilemap) jtilemap_destroy(gTilemap);
    if (gFps) simple_fps_destory(gFps);
    if (gRenderer) SDL_DestroyRenderer(gRenderer);
    if (gWindow) SDL_DestroyWindow(gWindow);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    (void)argc; (void)argv;
    if (!app_init()) return -1;
    SDL_Event e;
    while (gRunning) {
        while (SDL_PollEvent(&e)) { if (!app_event(&e)) { gRunning = false; break; } }
        if (gRunning) app_iterate();
    }
    app_quit();
    return 0;
}
