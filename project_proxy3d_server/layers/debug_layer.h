#ifndef DEBUG_LAYER_H
#define DEBUG_LAYER_H

#include <joy/joy.h>

// 调试层：与 3D 实体渲染分离，负责屏幕 HUD（如 FPS）绘制。
// 字体渲染依赖 r3d 的 2D HUD 管线，必须在 r3d_begin / r3d_end 之间调用。
struct Context;

// 每帧调用（需在 r3d_begin 之后、r3d_end 之前），在窗口左上角绘制 FPS。
// quad 为单位 quad（r3d_mesh_quad），字体首次调用时自动初始化。
void debug_layer_render_hud(Context* ctx, render_mesh_p quad);

#endif
