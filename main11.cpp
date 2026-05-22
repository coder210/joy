// main11.cpp — Tilemap 测试（jtilemap_render 渲染封装）
#include <cstdio>
#include <cstdlib>
#include "SDL3/SDL.h"
#include <joy/jtilemap.h>
#include <joy/jtilemap_render.h>
#include <joy/jcore.h>

static const int WINDOW_W = 1024;
static const int WINDOW_H = 760;

static SDL_Window*   gWindow  = nullptr;
static SDL_Renderer* gRenderer = nullptr;
static jtilemap_p    gTilemap = nullptr;
static bool          gRunning = true;

static struct { float x, y; } gCamera = { -100.0f, -100.0f };
static game_timer_t         gTimer;
static simple_fps_counter_p gFps = nullptr;

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

    // 初始化渲染模块（默认路径映射匹配当前项目资产目录结构）
    jtilemap_render_init(gRenderer, NULL, NULL);

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
        if (sc == SDL_SCANCODE_UP || sc == SDL_SCANCODE_W)       gCamera.y -= 10;
        else if (sc == SDL_SCANCODE_RIGHT || sc == SDL_SCANCODE_D) gCamera.x += 10;
        else if (sc == SDL_SCANCODE_DOWN || sc == SDL_SCANCODE_S)  gCamera.y += 10;
        else if (sc == SDL_SCANCODE_LEFT || sc == SDL_SCANCODE_A)  gCamera.x -= 10;
    }
    return true;
}

static bool app_iterate(void)
{
    game_timer_update(&gTimer); simple_fps_update(gFps);
    uint32_t ms = SDL_GetTicks();

    SDL_SetRenderDrawColor(gRenderer, 71, 171, 169, 255);
    SDL_RenderClear(gRenderer);

    // 一行渲染所有层
    jtilemap_render_all(gTilemap, gCamera.x, gCamera.y, ms);

    { char t[128]; snprintf(t, sizeof(t), "tilemap - FPS:%d", simple_fps_value(gFps)); SDL_SetWindowTitle(gWindow, t); }
    SDL_RenderPresent(gRenderer);
    return true;
}

static void app_quit(void)
{
    jtilemap_render_destroy();
    if (gTilemap) { jtilemap_destroy(gTilemap); gTilemap = nullptr; }
    if (gFps) { simple_fps_destory(gFps); gFps = nullptr; }
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
