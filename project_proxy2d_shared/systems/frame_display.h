#pragma once
#include <stdio.h>
#include <string.h>
#include <joy/joy.h>

// ============================================================
// 当前逻辑帧号显示（纯视觉 HUD，不影响帧同步/ECS 逻辑）
//   - 在指定坐标绘制当前逻辑帧号（帧同步 lockstep 的帧计数）
//   - 可选敌人数量（压力测试：观察大量敌人时逻辑 FPS 是否仍保持 20Hz）
//   - 需在 r2d_begin/r2d_end 之间调用
// ============================================================
static void frame_display_draw(jr2d_p r2d, font_p font, int frame_id, int logic_fps,
        float x, float y, int enemy_count = -1)
{
        char text[64];
        if (enemy_count >= 0)
            snprintf(text, sizeof(text), "Frame: %d  LogicFPS: %d  Enemies: %d",
                     frame_id, logic_fps, enemy_count);
        else
            snprintf(text, sizeof(text), "Frame: %d  LogicFPS: %d", frame_id, logic_fps);
        jcolor_t c = { 60, 60, 70, 235 };   // 深灰，适配白底背景
        // 超采样：28px 字体在 scale=0.5 下缩到 14px 显示，文字边缘更锐利。
        // 因 scale 会同时缩放坐标，故传入坐标需翻倍（x*2, y*2）以保持在原位置。
        r2d_set_scale(r2d, 0.5f, 0.5f);
        jtext_printx(r2d, font, text, (int)strlen(text), c, x * 2.0f, y * 2.0f);
        r2d_set_scale(r2d, 1.0f, 1.0f);   // 恢复，避免影响后续绘制
}
