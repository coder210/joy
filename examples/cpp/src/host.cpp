/* ============================================================
 * joy 示例 · C++ 薄宿主（native plugin 模板）
 *
 * 这里没有游戏逻辑：窗口、场景、物理、绘制全在 game.cpp 里。
 * 宿主只做五件事：
 *   1. SDL 主回调（SDL_AppInit / Event / Iterate / Quit）；
 *   2. 取唯一入口 joy_game_get_hooks；
 *   3. joy_api_vtable_init() 填好 JoyApi 大函数指针表并注入游戏侧；
 *   4. 把 SDL 事件翻译成纯 C 的 JoyInput，每帧调 hooks->on_frame(dt)；
 *   5. 冒烟模式解析 --smoke N、固定 1/60 步长、按回报的 exit_code 退出。
 *
 * 与 lua/js/csharp 三个示例的宿主职责完全对齐，区别只在「逻辑外置」的载体：
 * 它们是脚本/托管程序集，这里是原生代码。
 *
 * 两种形态由 JOY_GAME_STATIC 决定（见 ../CMakeLists.txt）：
 *   动态 — 桌面用 SDL_LoadObject 从 cpp/ 加载游戏 DLL；
 *          Android 由系统从 lib/<abi>/ 解压 .so，宿主用裸库名让 linker 查找。
 *   静态 — iOS / wasm 把 game.cpp 编进本 exe，链接期解析 joy_game_get_hooks，
 *          避开代码签名 / Emscripten 动态链接配置。
 * game.cpp 不感知自己属于哪种形态。
 * ============================================================ */
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include <joy/joy.h>
#include "joy_game_hooks.h"
#include "generated/joy_api_vtable.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../../mobile_display.h"

/* 与 CMake 默认 SHARED 库产物名一致，避免把生成器表达式塞进编译宏。
 * Android 上这个名字也是交给系统 linker 搜索的裸库名。 */
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

#ifdef JOY_GAME_STATIC
/* 静态形态：game.cpp 已编进本 exe，入口由链接器解析，不经过 SDL_LoadObject。 */
extern "C" const JoyGameHooks* joy_game_get_hooks(void);
#endif

static SDL_SharedObject*     g_lib   = NULL;   /* 静态形态恒为 NULL */
static const JoyGameHooks*   g_hooks = NULL;
static JoyApi                g_api;        /* 宿主拥有，游戏侧只持有 const 指针 */
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

/* 两种形态共用：契约头三字段 + 三个回调必须齐全，前后兼容靠 abi/size。 */
static bool validate_hooks(const JoyGameHooks* hooks)
{
    if (!hooks || hooks->abi_version != JOY_GAME_ABI
        || hooks->size < sizeof(JoyGameHooks) || !hooks->init
        || !hooks->on_frame || !hooks->shutdown) {
        fprintf(stderr, "[host] incompatible JoyGameHooks (abi/size)\n");
        return false;
    }
    return true;
}

static bool load_game(void)
{
#ifdef JOY_GAME_STATIC
    /* iOS / wasm：运行时加载动态库需要构建期签名或 Emscripten
     * MAIN_MODULE/SIDE_MODULE 配置，模板默认把游戏代码编进宿主。 */
    g_hooks = joy_game_get_hooks();
    return validate_hooks(g_hooks);
#else
#  if defined(__ANDROID__)
    /* APK 只有 lib/<abi>/*.so 会被系统解压并加入 linker 搜索路径；
     * assets/ 是压缩数据，dlopen 打不开，用裸库名让 linker 查找。 */
    const char* path = JOY_GAME_DLL_NAME;
#  elif defined(_WIN32)
    const char* path = "cpp\\" JOY_GAME_DLL_NAME;
#  else
    const char* path = "cpp/" JOY_GAME_DLL_NAME;
#  endif
    /* 相对运行期资源根：SDL_AppInit 已把 CWD 对齐到 exe 目录。
     * 不用 SDL_GetBasePath() 拼绝对基址 —— 它在 Android 返回 "./"、
     * iOS 返回 bundle 路径，桌面之外的语义都不一致。 */

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
    return validate_hooks(g_hooks);
#endif
}

/* ---------------- SDL 入口 ---------------- */

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    (void)appstate;

    joy_examples_setup_mobile_display();  /* 必须在游戏 DLL 建窗口之前 */
    /* 资源（sprites/、fonts/）与游戏 DLL（cpp/）都是相对路径，先把 CWD
     * 对齐到 exe 目录，否则从 IDE / 仓库根启动会静默退化成纯色块。 */
    joy_examples_align_working_dir(argc > 0 ? argv[0] : NULL);
    int i = find_arg(argc, argv, "--smoke");
    if (i >= 0 && i + 1 < argc) g_smoke_frames = atoi(argv[i + 1]);
    g_smoke = g_smoke_frames > 0;

    /* hint 必须在游戏侧创建窗口之前设置 */
    if (g_smoke) {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    } else {
        SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60");
    }

    if (!load_game()) return SDL_APP_FAILURE;

    /* 宿主是唯一链接 joy::all 的一方：表在这里填好，游戏侧只拿 const 指针 */
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

#ifdef JOY_GAME_STATIC
    printf("[host] game = built-in (JoyApi abi=%u size=%u bytes)\n",
           JOY_API_VTABLE_ABI, (unsigned)sizeof(JoyApi));
#else
    printf("[host] game DLL = %s (JoyApi abi=%u size=%u bytes)\n",
           JOY_GAME_DLL_NAME, JOY_API_VTABLE_ABI, (unsigned)sizeof(JoyApi));
#endif
    fflush(stdout);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
    (void)appstate;

    /* 引擎层状态快照是唯一状态源。g_input 不再由宿主维护，而是在每帧
     * on_frame 前从 joy_core_input_* getter 重建出的 Native DLL ABI 快照。 */
    switch (e->type) {
    case SDL_EVENT_KEY_DOWN:
        joy_core_input_feed_key((int)e->key.scancode, true);
        break;
    case SDL_EVENT_KEY_UP:
        joy_core_input_feed_key((int)e->key.scancode, false);
        break;
    case SDL_EVENT_MOUSE_MOTION:
        joy_core_input_feed_mouse_move(e->motion.x, e->motion.y);
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        joy_core_input_feed_mouse_move(e->button.x, e->button.y);
        joy_core_input_feed_mouse_button((int)e->button.button, true);
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        joy_core_input_feed_mouse_move(e->button.x, e->button.y);
        joy_core_input_feed_mouse_button((int)e->button.button, false);
        break;
    case SDL_EVENT_MOUSE_WHEEL:
        joy_core_input_feed_wheel(e->wheel.y);
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

    /* 从引擎层唯一状态源重建 Native DLL 的输入快照 */
    for (int i = 0; i < 512; i++)
        g_input.keys[i] = joy_core_input_is_key_down(i);
    g_input.mouse_x = joy_core_input_get_mouse_x();
    g_input.mouse_y = joy_core_input_get_mouse_y();
    g_input.mouse_left = joy_core_input_is_mouse_button_down(1);

    JoyFrameResult r;
    memset(&r, 0, sizeof(r));
    g_hooks->on_frame(g_hooks->game, dt, &g_input, &r);
    joy_core_input_end_frame();   /* 清掉本帧滚轮增量与按键边沿 */

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

    /* 先让游戏侧释放全部 joy 句柄，再卸载 DLL（静态形态 g_lib 为 NULL，
     * 自然跳过卸载）；顺序反了会留下悬空调用 */
    if (g_hooks && g_hooks->shutdown) g_hooks->shutdown(g_hooks->game);
    g_hooks = NULL;
    if (g_lib) {
        SDL_UnloadObject(g_lib);
        g_lib = NULL;
    }
}
