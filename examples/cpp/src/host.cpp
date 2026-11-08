/* ============================================================
 * joy 示例 · C++ 薄宿主（native plugin 版）
 *
 * 这里没有游戏逻辑：窗口、场景、物理、绘制全在 assets/cpp/JoyGameCpp.dll 里。
 * 宿主只做五件事：
 *   1. SDL 主回调（SDL_AppInit / Event / Iterate / Quit）；
 *   2. SDL_LoadLibrary 加载游戏 DLL，取唯一入口 joy_game_get_hooks；
 *   3. joy_api_vtable_init() 填好 JoyApi 大函数指针表并注入 DLL；
 *   4. 把 SDL 事件翻译成纯 C 的 JoyInput，每帧调 hooks->on_frame(dt)；
 *   5. 冒烟模式解析 --smoke N、固定 1/60 步长、按 DLL 回报的 exit_code 退出。
 *
 * 与 lua/js/csharp 三个示例的宿主职责完全对齐，区别只在「逻辑外置」的载体：
 * 它们是脚本/托管程序集，这里是原生 DLL。
 *
 * 注意：桌面可以从 assets/ 直接 SDL_LoadLibrary；Android 不能从 APK assets
 * 加载 .so（必须由系统从 lib/<abi>/ 解压），移动端需要 JOY_GAME_STATIC 的
 * 编译期 fallback，本次试点只覆盖桌面。
 * ============================================================ */
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include "joy_game_hooks.h"
#include "generated/joy_api_vtable.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

/* 与 CMake 默认 SHARED 库产物名一致，避免把生成器表达式塞进编译宏 */
#ifndef JOY_GAME_DLL_NAME
#  if defined(_WIN32)
#    define JOY_GAME_DLL_NAME "JoyGameCpp.dll"
#  elif defined(__APPLE__)
#    define JOY_GAME_DLL_NAME "libJoyGameCpp.dylib"
#  else
#    define JOY_GAME_DLL_NAME "libJoyGameCpp.so"
#  endif
#endif

typedef const JoyGameHooks* (*joy_game_get_hooks_fn)(void);

static SDL_SharedObject*     g_lib   = NULL;
static const JoyGameHooks*   g_hooks = NULL;
static JoyApi                g_api;        /* 宿主拥有，DLL 只持有 const 指针 */
static JoyInput              g_input;
static int                   g_smoke_frames = 0;
static bool                  g_smoke = false;

static int find_arg(int argc, char* argv[], const char* name)
{
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], name) == 0) return i;
    }
    return -1;
}

static bool load_game_dll(void)
{
    char path[1024];
    const char* base = SDL_GetBasePath();
    if (!base) return false;

#if defined(_WIN32)
    SDL_snprintf(path, sizeof(path), "%sassets\\cpp\\%s", base, JOY_GAME_DLL_NAME);
#else
    SDL_snprintf(path, sizeof(path), "%sassets/cpp/%s", base, JOY_GAME_DLL_NAME);
#endif
    /* 不要 SDL_free(base)：SDL3 的 SDL_GetBasePath() 返回内部 CachedBasePath
     * 静态缓存，释放它会在进程退出时造成堆损坏（0xC0000374）。 */

    g_lib = SDL_LoadObject(path);
    if (!g_lib) {
        fprintf(stderr, "[host] SDL_LoadObject(\"%s\") failed: %s\n",
                path, SDL_GetError());
        return false;
    }

    joy_game_get_hooks_fn get_hooks =
        reinterpret_cast<joy_game_get_hooks_fn>(
            SDL_LoadFunction(g_lib, "joy_game_get_hooks"));
    if (!get_hooks) {
        fprintf(stderr, "[host] joy_game_get_hooks not found: %s\n", SDL_GetError());
        return false;
    }

    g_hooks = get_hooks();
    if (!g_hooks || g_hooks->abi_version != JOY_GAME_ABI
        || g_hooks->size < sizeof(JoyGameHooks) || !g_hooks->init
        || !g_hooks->on_frame || !g_hooks->shutdown) {
        fprintf(stderr, "[host] incompatible JoyGameHooks (abi/size)\n");
        return false;
    }
    return true;
}

/* ---------------- SDL 入口 ---------------- */

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    (void)appstate;

    int i = find_arg(argc, argv, "--smoke");
    if (i >= 0 && i + 1 < argc) g_smoke_frames = atoi(argv[i + 1]);
    g_smoke = g_smoke_frames > 0;

    /* hint 必须在 DLL 创建窗口之前设置 */
    if (g_smoke) {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    } else {
        SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60");
    }

    if (!load_game_dll()) return SDL_APP_FAILURE;

    /* 宿主是唯一链接 joy::all 的一方：表在这里填好，DLL 只拿指针 */
    if (joy_api_vtable_init(&g_api, NULL) != 0) {
        fprintf(stderr, "[host] joy_api_vtable_init failed\n");
        return SDL_APP_FAILURE;
    }
    const char* err = joy_api_vtable_selftest();
    if (err) {
        fprintf(stderr, "[host] vtable selftest failed: %s\n", err);
        return SDL_APP_FAILURE;
    }

    memset(&g_input, 0, sizeof(g_input));
    if (!g_hooks->init(g_hooks->game, &g_api, g_smoke_frames)) {
        return SDL_APP_FAILURE;
    }

    printf("[host] game DLL = %s (JoyApi abi=%u size=%u bytes)\n",
           JOY_GAME_DLL_NAME, JOY_API_VTABLE_ABI, (unsigned)sizeof(JoyApi));
    fflush(stdout);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
    (void)appstate;

    switch (e->type) {
    case SDL_EVENT_KEY_DOWN:
        if (e->key.scancode < 512) g_input.keys[e->key.scancode] = true;
        break;
    case SDL_EVENT_KEY_UP:
        if (e->key.scancode < 512) g_input.keys[e->key.scancode] = false;
        break;
    case SDL_EVENT_MOUSE_MOTION:
        g_input.mouse_x = e->motion.x;
        g_input.mouse_y = e->motion.y;
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (e->button.button == 1) g_input.mouse_left = true;
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (e->button.button == 1) g_input.mouse_left = false;
        break;
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    (void)appstate;
    if (!g_hooks) return SDL_APP_SUCCESS;

    double dt;
    if (g_smoke) {
        dt = 1.0 / 60.0;
    } else {
        static Uint64 last_ns = 0;
        Uint64 now = SDL_GetTicksNS();
        if (last_ns == 0) last_ns = now;
        dt = (double)(now - last_ns) / (double)SDL_NS_PER_SECOND;
        last_ns = now;
        if (dt < 0) dt = 0;
        if (dt > 0.05) dt = 0.05;   /* 与 C# GameApp.Tick 的上限一致 */
    }

    JoyFrameResult r;
    memset(&r, 0, sizeof(r));
    g_hooks->on_frame(g_hooks->game, dt, &g_input, &r);

    if (r.quit) {
        if (g_smoke) {
            printf("[host] smoke finished: frames=%d exit=%d\n", r.frame, r.exit_code);
            fflush(stdout);
        }
        return r.exit_code == 0 ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    (void)appstate;
    (void)result;

    /* 先让 DLL 释放全部 joy 句柄，再卸载 DLL；顺序反了会留下悬空调用 */
    if (g_hooks && g_hooks->shutdown) g_hooks->shutdown(g_hooks->game);
    g_hooks = NULL;
    if (g_lib) {
        SDL_UnloadObject(g_lib);
        g_lib = NULL;
    }
}
