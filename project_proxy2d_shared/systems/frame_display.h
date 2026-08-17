#pragma once
#include <stdio.h>
#include <string.h>
#include <joy/joy.h>

// ============================================================
// 当前逻辑帧号显示（纯视觉 HUD，不影响帧同步/ECS 逻辑）
//   - 在指定坐标绘制当前逻辑帧号（帧同步 lockstep 的帧计数）
//   - 需在 r2d_begin/r2d_end 之间调用
// ============================================================
static void frame_display_draw(jr2d_p r2d, font_p font, int frame_id, float x, float y)
{
        char text[32];
        snprintf(text, sizeof(text), "Frame: %d", frame_id);
        jcolor_t c = { 60, 60, 70, 235 };   // 深灰，适配白底背景
        jtext_printx(r2d, font, text, (int)strlen(text), c, x, y);
}
