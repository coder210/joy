/* ============================================================
 * 宿主 <-> 游戏 DLL 的唯一契约（纯 C POD，跨 DLL 边界安全）
 *
 * 方向约定：
 *   - 宿主 → DLL：JoyApi（引擎 API 大函数指针表）+ JoyInput（输入快照）
 *   - DLL → 宿主：JoyGameHooks（帧驱动回调）+ JoyFrameResult（退出请求）
 *
 * DLL 只导出一个入口 joy_game_get_hooks()，不导出任何游戏符号；宿主用
 * SDL_LoadFunction 取它，因此不需要 .lib / __declspec(dllimport)，DLL 也不
 * 反向链接宿主、不链接 joy::all（避免把引擎源码再编一份、出现两套句柄表）。
 * ============================================================ */
#ifndef JOY_GAME_HOOKS_H
#define JOY_GAME_HOOKS_H

#include <stdint.h>
#include <stdbool.h>
#include "generated/joy_api_vtable.h"

#define JOY_GAME_ABI 1u

/* 按键数组按 SDL scancode 索引；宿主负责把 SDL_Event 翻译成这份纯 C 快照，
 * DLL 因此完全不需要 include SDL 头（游戏侧只用 SDL3/SDL_scancode.h 的枚举值）。 */
typedef struct JoyInput {
        bool  keys[512];
        float mouse_x, mouse_y;
        bool  mouse_left;
} JoyInput;

typedef struct JoyFrameResult {
        bool quit;
        int  exit_code;
        int  frame;
} JoyFrameResult;

typedef struct JoyGameHooks {
        uint32_t abi_version;
        uint32_t size;
        void*    game;                  /* DLL 私有状态，由宿主原样回传 */

        /* 创建窗口/渲染器、加载资源、进入初始场景；false = 初始化失败 */
        bool (*init)(void* game, const JoyApi* api, int smoke_frames);
        /* 每帧驱动；out->quit=true 表示请求结束进程 */
        void (*on_frame)(void* game, double dt, const JoyInput* input,
                         JoyFrameResult* out);
        /* 释放全部 joy 句柄；宿主随后卸载 DLL */
        void (*shutdown)(void* game);
} JoyGameHooks;

#if defined(_WIN32)
#  define JOY_GAME_EXPORT __declspec(dllexport)
#else
#  define JOY_GAME_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* 唯一导出入口：返回静态 hooks 表。 */
JOY_GAME_EXPORT const JoyGameHooks* joy_game_get_hooks(void);
#ifdef __cplusplus
}
#endif

#endif /* JOY_GAME_HOOKS_H */
