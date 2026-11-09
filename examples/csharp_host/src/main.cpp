/* ============================================================
 * joy 示例 · C# 版宿主
 *
 * C++ 侧只做窗口 / 主循环 / 事件泵 / 渲染帧边界，游戏逻辑全在
 * JoyGame.dll（game/ 玩法 + runtime/ 骨架 + test/ 冒烟脚本）。
 *
 * 每帧：r2d_begin → C# Host.OnFrame(dt) → r2d_end
 * 输入：SDL 事件写进 cs 模块的输入桥，C# 通过 Joy.Input 轮询。
 *
 * 与 cpp / lua / js 三个示例功能完全相同（见 ../README.md）。
 *
 * 参数：
 *   --smoke N        无窗口冒烟：dummy 驱动 + 固定 1/60 步长，跑 N 帧后退出
 *   --assembly PATH  指定 C# 程序集（默认 JoyGame.dll，相对运行期资源根）
 * ============================================================ */
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_loadso.h>
#include <joy/joy.h>
#include "joy_csharp.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(_WIN32)
#include <direct.h>
#else
#include <unistd.h>
#endif

static const int APP_W = 480;
static const int APP_H = 320;

/* 宿主不持有窗口与渲染器：它们由 C# 侧用生成链路创建（Joy.Api.Bindings）。
 * 生成链路用句柄表管理对象，宿主直接创建的裸指针进不了那张表。 */
static csx_p        g_csx = NULL;
static csx_method_p g_on_init = NULL;
static csx_method_p g_on_frame = NULL;
static csx_method_p g_on_shutdown = NULL;

static int    g_smoke_frames = 0;
static int    g_frame = 0;
static Uint64 g_last_tick_ns = 0;
static const char* g_assembly = "JoyGame.dll";

static int find_arg(int argc, char* argv[], const char* name)
{
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], name) == 0) return i;
    }
    return -1;
}

/* 把工作目录切到 exe 目录：资源根就是 exe 同目录，JoyGame.dll / fonts/ 等
 * 都按不带 assets/ 前缀的相对路径加载 */
static void chdir_to_exe_dir(void)
{
    const char* base = SDL_GetBasePath();
    if (!base) return;
#if defined(_WIN32)
    _chdir(base);
#else
    chdir(base);
#endif
}

/* BCL 与绑定程序集放在 lib/：csx_setup_mono_dirs() 只认系统 Mono 路径
 * 或 MONO_LIB_DIR，所以在 csx_create() 之前显式指过去。 */
#if defined(__ANDROID__)
/* APK 的 assets/ 是压缩数据，不是文件系统目录，而 mono_set_dirs() 要真实路径
 * （csx_setup_mono_dirs 用 stat() 校验）。首次启动把 lib/ 下的 DLL 解到
 * SDL_GetPrefPath() 指向的内部存储，再把 MONO_LIB_DIR 指过去。
 * 清单与 ../CMakeLists.txt 部署到 lib/ 的文件保持一致。 */
static const char* const kClrLibs[] = {
    "System.Private.CoreLib.dll", "netstandard.dll",
    "System.Runtime.dll", "System.Collections.dll",
    "Joy.Bindings.dll", NULL
};

static bool copy_clr_lib(const char* dir, const char* name)
{
    char src[512], dst[512];
    SDL_snprintf(src, sizeof(src), "lib/%s", name);
    /* SDL_GetPrefPath() 在 Android 返回带尾部 '/' 的内部存储路径 */
    SDL_snprintf(dst, sizeof(dst), "%s%s", dir, name);

    size_t size = 0;
    void* data = SDL_LoadFile(src, &size);
    if (!data) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "[cs] read asset %s failed: %s", src, SDL_GetError());
        return false;
    }
    SDL_IOStream* io = SDL_IOFromFile(dst, "wb");
    if (!io) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "[cs] write %s failed: %s", dst, SDL_GetError());
        SDL_free(data);
        return false;
    }
    bool ok = SDL_WriteIO(io, data, size) == size;
    if (!ok) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "[cs] write %s incomplete: %s", dst, SDL_GetError());
    }
    SDL_CloseIO(io);
    SDL_free(data);
    return ok;
}
#endif

static bool setup_clr_dirs(void)
{
#if defined(__ANDROID__)
    char* pref = SDL_GetPrefPath("joy", "example_csharp");
    if (!pref) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "[cs] SDL_GetPrefPath failed: %s", SDL_GetError());
        return false;
    }
    for (int i = 0; kClrLibs[i]; ++i) {
        /* 少一个 BCL，mono_jit_init() 就会因为找不到 CoreLib 直接 abort，
         * 只打日志继续跑没有任何意义，必须立刻终止启动。 */
        if (!copy_clr_lib(pref, kClrLibs[i])) {
            SDL_free(pref);
            return false;
        }
    }
    /* MONO_LIB_DIR 是 jcsx.c 自己读的变量；MonoVM 认的是 MONO_PATH，
     * .NET 8 还要求 mono_set_assemblies_path()（见 jcsx.c）。两条路径
     * 都指向同一个解包目录，缺一条都可能让 C# APK 启动闪退。 */
    setenv("MONO_PATH", pref, 1);
    setenv("MONO_LIB_DIR", pref, 1);
    SDL_Log("[cs] CLR dirs: %s", pref);
    SDL_free(pref);
#else
    const char* base = SDL_GetBasePath();
    if (!base) return true;
    char lib[512];
#  if defined(_WIN32)
    snprintf(lib, sizeof(lib), "%slib", base);
    _putenv_s("MONO_LIB_DIR", lib);
#  else
    snprintf(lib, sizeof(lib), "%slib", base);
    setenv("MONO_LIB_DIR", lib, 1);

    /* Linux/macOS 的 runtime pack 把 CoreLib 的 P/Invoke 目标拆成独立 shim，
     * 而 libcoreclr.so/.dylib 没有 DT_RUNPATH，dlopen 不会自动搜 exe 目录。
     * 在 csx_create() 之前按绝对路径预加载，后续同名加载直接复用；
     * 清单与 runtimes/<platform>/ 入库的 libSystem.* 一致，缺失的忽略。 */
    static const char* const kClrShims[] = {
        "libSystem.Native.so", "libSystem.Native.dylib",
        "libSystem.Globalization.Native.so", "libSystem.Globalization.Native.dylib",
        "libSystem.IO.Compression.Native.so", "libSystem.IO.Compression.Native.dylib",
        "libSystem.Net.Security.Native.so", "libSystem.Net.Security.Native.dylib",
        "libSystem.Security.Cryptography.Native.OpenSsl.so",
        "libSystem.Security.Cryptography.Native.OpenSsl.dylib",
        "libSystem.Security.Cryptography.Native.Apple.dylib",
        NULL
    };
    for (int i = 0; kClrShims[i]; ++i) {
        char path[512];
        snprintf(path, sizeof(path), "%s%s", base, kClrShims[i]);
        if (!SDL_LoadObject(path)) SDL_ClearError();
    }
#  endif
#endif
    return true;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    (void)appstate;

    chdir_to_exe_dir();
    if (!setup_clr_dirs()) return SDL_APP_FAILURE;

    int i = find_arg(argc, argv, "--smoke");
    if (i >= 0 && i + 1 < argc) g_smoke_frames = atoi(argv[i + 1]);
    i = find_arg(argc, argv, "--assembly");
    if (i >= 0 && i + 1 < argc) g_assembly = argv[i + 1];

    if (g_smoke_frames > 0) {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    } else {
        SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60");
    }

    /* stderr 在 Android 上不稳定，全部走 SDL_Log → logcat，闪退时能定位到
     * 是 CLR 初始化、程序集加载还是方法查找哪一步失败。 */
    SDL_Log("[cs] creating MonoVM, assembly=%s", g_assembly);
    g_csx = csx_create();
    if (!g_csx) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "[host] csx_create failed (CLR runtime unavailable?)");
        return SDL_APP_FAILURE;
    }
    if (!csx_load_assembly(g_csx, g_assembly)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "[host] load assembly failed: %s", g_assembly);
        return SDL_APP_FAILURE;
    }

    g_on_init = csx_find_method(g_csx, "JoyGame", "Host", "OnInit", 3);
    g_on_frame = csx_find_method(g_csx, "JoyGame", "Host", "OnFrame", 1);
    g_on_shutdown = csx_find_method(g_csx, "JoyGame", "Host", "OnShutdown", 0);
    if (!g_on_init || !g_on_frame) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "[host] JoyGame.Host.OnInit/OnFrame not found");
        return SDL_APP_FAILURE;
    }
    SDL_Log("[cs] MonoVM ready");

    {
        int w = APP_W;
        int h = APP_H;
        int smoke = g_smoke_frames;
        void* args[3] = { &w, &h, &smoke };
        csx_invoke_method(g_csx, g_on_init, args);
    }

    g_last_tick_ns = SDL_GetTicksNS();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
    (void)appstate;
    switch (e->type) {
    case SDL_EVENT_KEY_DOWN:
        csx_input_key((int)e->key.scancode, true);
        break;
    case SDL_EVENT_KEY_UP:
        csx_input_key((int)e->key.scancode, false);
        break;
    case SDL_EVENT_MOUSE_MOTION:
        /* 宿主没有渲染器做逻辑坐标换算，直接透传窗口坐标 */
        csx_input_mouse_move(e->motion.x, e->motion.y);
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        csx_input_mouse_button((int)e->button.button, true);
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        csx_input_mouse_button((int)e->button.button, false);
        break;
    case SDL_EVENT_MOUSE_WHEEL:
        csx_input_wheel(e->wheel.y);
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

    {
        void* args[1] = { &dt };
        csx_invoke_method(g_csx, g_on_frame, args);
    }
    csx_input_end_frame();

    g_frame++;
    if (g_smoke_frames > 0 && g_frame >= g_smoke_frames) {
        printf("[host] smoke finished: frames=%d exit=%d\n", g_frame, csx_host_exit_code());
        fflush(stdout);
        return csx_host_exit_code() == 0 ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (csx_host_quit_requested()) {
        return csx_host_exit_code() == 0 ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    (void)appstate;
    (void)result;

    if (g_csx && g_on_shutdown) csx_invoke_method(g_csx, g_on_shutdown, NULL);
    if (g_on_init) csx_method_free(g_on_init);
    if (g_on_frame) csx_method_free(g_on_frame);
    if (g_on_shutdown) csx_method_free(g_on_shutdown);
    if (g_csx) csx_release(g_csx);
}
