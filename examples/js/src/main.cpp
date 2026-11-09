/* ============================================================
 * joy 示例 · JavaScript(duktape) 版宿主
 *
 * 宿主几乎是空的：窗口、渲染器、游戏状态、绘制全在 src/main.js 里
 *（源码在 src/，构建时铺进运行期资源根的 scripts/main.js；资源根在桌面
 *  是 exe 同目录、Android 是 APK 的 assets 根，路径都不带 assets/ 前缀）。
 * C++ 只负责 SDL 生命周期 + 把事件/帧推给脚本。
 *
 * 契约（jsx.c 提供，见 modules/js/joy_js.h）：
 *   jsx_create()          建 duktape heap，并把 r2d / window / keyboard /
 *                         console 等模块注册为**全局对象**（不是 joy.xxx，
 *                         这点与 Lua 版不同）
 *   jsx_dofile()          加载脚本
 *   jsx_call_joy(f)       调用脚本里的 joy.<f>(...)
 *   jsx_dispatch_event()  把 SDL_Event 转成 joy.keypressed / mousepressed 等回调
 *   jsx_get_context()     取 duk_context，用来读写脚本全局变量
 *
 * 退出：脚本把全局 quit 置 true（可选 exit_code），宿主每帧读完即退出。
 *
 * 参数：--smoke N   无窗口冒烟，固定 1/60 步长跑 N 帧
 * ============================================================ */
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <joy/joy.h>

#include "joy_js.h"
#include "duktape/duktape.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

static jsx_p        g_js = NULL;
static duk_context* g_ctx = NULL;
static int          g_smoke_frames = 0;
static int          g_frame = 0;
static Uint64       g_last_tick_ns = 0;
static int          g_exit_code = 0;

static int find_arg(int argc, char* argv[], const char* name)
{
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], name) == 0) return i;
    }
    return -1;
}

static bool global_bool(const char* name)
{
    if (!g_ctx) return false;
    duk_get_global_string(g_ctx, name);
    bool v = duk_get_boolean(g_ctx, -1) != 0;
    duk_pop(g_ctx);
    return v;
}

static int global_int(const char* name)
{
    if (!g_ctx) return 0;
    duk_get_global_string(g_ctx, name);
    int v = (int)duk_get_number(g_ctx, -1);
    duk_pop(g_ctx);
    return v;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    (void)appstate;

    int i = find_arg(argc, argv, "--smoke");
    if (i >= 0 && i + 1 < argc) g_smoke_frames = atoi(argv[i + 1]);

    if (g_smoke_frames > 0) {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    } else {
        SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60");
    }

    g_js = jsx_create();
    if (!g_js) {
        fprintf(stderr, "[host] jsx_create failed\n");
        return SDL_APP_FAILURE;
    }
    g_ctx = (duk_context*)jsx_get_context(g_js);
    if (!g_ctx) {
        fprintf(stderr, "[host] jsx_get_context failed\n");
        return SDL_APP_FAILURE;
    }

    if (!jsx_dofile(g_js, "scripts/main.js")) {
        fprintf(stderr, "[host] load scripts/main.js failed\n");
        return SDL_APP_FAILURE;
    }

    /* 冒烟帧数透传给脚本（与 Lua 版同一个全局名） */
    duk_push_int(g_ctx, g_smoke_frames);
    duk_put_global_string(g_ctx, "SMOKE_FRAMES");

    if (!jsx_call_joy(g_js, "init", 0, 0.0f)) {
        fprintf(stderr, "[host] joy.init() failed\n");
        return SDL_APP_FAILURE;
    }

    printf("[host] init 480x320 smoke=%d\n", g_smoke_frames);
    fflush(stdout);
    g_last_tick_ns = SDL_GetTicksNS();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
    (void)appstate;
    if (e->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
    jsx_dispatch_event(g_js, e);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    (void)appstate;

    float dt;
    if (g_smoke_frames > 0) {
        dt = 1.0f / 60.0f;
    } else {
        Uint64 now = SDL_GetTicksNS();
        dt = (float)((double)(now - g_last_tick_ns) / (double)SDL_NS_PER_SECOND);
        g_last_tick_ns = now;
        if (dt < 0.0f) dt = 0.0f;
        if (dt > 0.1f) dt = 0.1f;
    }

    jsx_call_joy(g_js, "frame_begin", 0, dt);
    jsx_call_joy(g_js, "update", 1, dt);
    jsx_call_joy(g_js, "draw", 0, dt);
    jsx_call_joy(g_js, "frame_end", 0, dt);

    g_frame++;

    if (global_bool("quit")) {
        g_exit_code = global_int("exit_code");
        printf("[host] smoke finished: frames=%d exit=%d\n", g_frame, g_exit_code);
        fflush(stdout);
        return g_exit_code == 0 ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_smoke_frames > 0 && g_frame >= g_smoke_frames) {
        printf("[host] smoke finished: frames=%d exit=%d\n", g_frame, g_exit_code);
        fflush(stdout);
        return g_exit_code == 0 ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    (void)appstate;
    (void)result;

    if (!g_js) return;
    jsx_call_joy(g_js, "shutdown", 0, 0.0f);
    jsx_release(g_js);
    g_js = NULL;
    g_ctx = NULL;
}
