/* ============================================================
 * joy 示例 · Lua 版宿主
 *
 * 宿主几乎是空的：窗口、渲染器、游戏状态、绘制全在 src/main.lua 里
 *（源码在 src/，构建时铺进运行期资源根的 scripts/main.lua；资源根在桌面
 *  是 exe 同目录、Android 是 APK 的 assets 根，路径都不带 assets/ 前缀）。
 * C++ 只负责 SDL 生命周期 + 把事件/帧推给脚本。
 *
 * 契约（jluax.c 提供，见 modules/lua/joy_lua.h）：
 *   luax_create()        建 Lua VM，并在内部注册好 joy.window / joy.r2d /
 *                        joy.keyboard / joy.font / joy.timer / joy.ui 等模块表
 *   luax_dofile()        加载脚本
 *   luax_call_joy(f)     调用脚本里的 joy.<f>(...)
 *   luax_dispatch_event() 把 SDL_Event 转成 joy.keypressed / mousepressed 等回调
 *
 * 退出：脚本把全局 quit 置 true（可选 exit_code），宿主每帧读完即退出。
 * 用全局变量而不是返回值，是因为 luax_call_joy 吃掉了 lua_pcall 的返回栈。
 *
 * 参数：--smoke N   无窗口冒烟，固定 1/60 步长跑 N 帧
 * ============================================================ */
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <joy/joy.h>

#include "joy_lua.h"
/* external/lua 的 lua.h 没有 extern "C" 保护，C++ 直接包含会按修饰名链接，
 * 报一串 LNK2019（lua_settop / lua_getglobal ...）。 */
extern "C" {
#include "lua/lua.h"
}

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../../mobile_display.h"

static luax_p  g_lua = NULL;
static lua_State* g_L = NULL;
static int     g_smoke_frames = 0;
static int     g_frame = 0;
static Uint64  g_last_tick_ns = 0;
static int     g_exit_code = 0;

static int find_arg(int argc, char* argv[], const char* name)
{
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], name) == 0) return i;
    }
    return -1;
}

static bool lua_global_bool(const char* name)
{
    if (!g_L) return false;
    lua_getglobal(g_L, name);
    bool v = lua_toboolean(g_L, -1) != 0;
    lua_pop(g_L, 1);
    return v;
}

static int lua_global_int(const char* name)
{
    if (!g_L) return 0;
    lua_getglobal(g_L, name);
    int v = (int)lua_tointeger(g_L, -1);
    lua_pop(g_L, 1);
    return v;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    (void)appstate;

    joy_examples_setup_mobile_display();  /* 必须在脚本建窗口之前 */
    int i = find_arg(argc, argv, "--smoke");
    if (i >= 0 && i + 1 < argc) g_smoke_frames = atoi(argv[i + 1]);

    /* dummy / 软件渲染的 hint 必须早于脚本建窗口（joy.init 里 joy_core_create_window） */
    if (g_smoke_frames > 0) {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    } else {
        SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60");
    }

    g_lua = luax_create();
    if (!g_lua) {
        fprintf(stderr, "[host] luax_create failed\n");
        return SDL_APP_FAILURE;
    }
    g_L = (lua_State*)luax_get_state(g_lua);

    luax_set_args(g_lua, argc, argv);
    if (!luax_dofile(g_lua, "scripts/main.lua")) {
        fprintf(stderr, "[host] load scripts/main.lua failed\n");
        return SDL_APP_FAILURE;
    }

    /* 冒烟帧数透传给脚本，让 Play 场景用与 cpp/csharp 相同的初始布局 */
    g_exit_code = 0;
    lua_pushinteger(g_L, g_smoke_frames);
    lua_setglobal(g_L, "SMOKE_FRAMES");

    if (!luax_call_joy(g_lua, "init", 0, 0.0f)) {
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
    luax_dispatch_event(g_lua, e);
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

    luax_call_joy(g_lua, "frame_begin", 0, dt);   /* 冒烟脚本注入模拟按键 */
    luax_call_joy(g_lua, "update", 1, dt);
    luax_call_joy(g_lua, "draw", 0, dt);
    luax_call_joy(g_lua, "frame_end", 0, dt);     /* 冒烟观察 + 断言 */

    g_frame++;

    if (lua_global_bool("quit")) {
        g_exit_code = lua_global_int("exit_code");
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

    if (!g_lua) return;
    luax_call_joy(g_lua, "shutdown", 0, 0.0f);
    luax_release(g_lua);
    g_lua = NULL;
    g_L = NULL;
}
