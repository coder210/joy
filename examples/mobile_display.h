/* ============================================================
 * 四份示例共用的移动端显示设置
 *
 * 做法对齐 joy_topdown_template/main.cpp 与 logical_presentation.h：
 * 引擎的 joy_core_create_window / joy_r2d_create 保持原样，屏幕适配全部放在应用层。
 *
 * 1) 横屏：SDL 在 SDL_CreateWindow 时用 SDL_HINT_ORIENTATIONS 调 Android 的
 *    setRequestedOrientation()。joy_core_create_window 带 SDL_WINDOW_RESIZABLE，
 *    不设 hint 时 Java 侧按 FULL_USER 处理（跟随系统/用户锁定），会覆盖
 *    AndroidManifest 的 screenOrientation，默认竖屏的模拟器上横版游戏
 *    只能占很窄一条。指定两个横屏方向 → USER_LANDSCAPE。
 *    必须在 joy_core_create_window() 之前调用。
 *
 * 2) 逻辑呈现：joy_r2d_create() 会把窗口实际尺寸（Android 上是屏幕物理尺寸）
 *    当成逻辑分辨率并用 STRETCH，于是按 480x320 绘制的画面只落在屏幕一角。
 *    四份示例在建好 renderer 后各自调用 R2dSetLogicalPresentationMode，
 *    覆盖成固定 480x320 + LETTERBOX（等比缩放居中、不拉伸），即模板
 *    logical_presentation.h 里的 presentation_set_design()。
 *
 * 模板游戏场景用 joy_r2d_get_logical_presentation() 做自适应布局，所以它能用
 * presentation_set_adaptive() 铺满且无黑边；本仓四份示例把 480x320 写死在
 * 玩法逻辑里，只能用 design 版本（横屏下左右留少量黑边，比例不变）。
 * ============================================================ */
#ifndef JOY_EXAMPLES_MOBILE_DISPLAY_H
#define JOY_EXAMPLES_MOBILE_DISPLAY_H

#include <SDL3/SDL_hints.h>

/* 与 include/joy/defs.h 的 JOY_LOGICAL_PRESENTATION_LETTERBOX 同值。
 * 脚本层（lua / js / csharp）拿不到 C 头文件常量，统一用这个数值。 */
#define JOY_EXAMPLE_PRESENTATION_LETTERBOX 2

static inline void joy_examples_setup_mobile_display(void)
{
#if defined(__ANDROID__)
        SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
#endif
}

#endif /* JOY_EXAMPLES_MOBILE_DISPLAY_H */
