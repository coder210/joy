#include "debug_layer.h"
#include "../context.h"
#include <cstdio>

// ---- FPS 统计与 HUD 绘制 ----
// 与 ECS 实体渲染分离的独立调试层，字体/quad 首次调用时一次性初始化。

void debug_layer_render_hud(Context* ctx, render_mesh_p quad)
{
	if (!ctx || !ctx->r3d) return;

	static bool  g_font_ok = false;
	static bool  g_font_tried = false;
	static render_mesh_p g_quad = NULL;
	static uint64_t g_fps_last = 0;
	static int   g_fps_count = 0;
	static int   g_fps = 0;

	if (!g_font_tried) {
		g_font_tried = true;
		// 优先黑体（含中文），回退 arial（ASCII）；路径为资源目录下的 fonts
		if (!r3d_font_init(ctx->r3d, "project_proxy3d_server/fonts/simhei.ttf", 48) &&
			!r3d_font_init(ctx->r3d, "project_proxy3d_server/fonts/arial.ttf", 48)) {
			log_info("debug_layer: font init failed (FPS HUD disabled)");
		} else {
			g_font_ok = true;
		}
	}

	// FPS 统计：每累计 ~0.5s 更新一次
	{
		uint64_t now = get_ticks();
		g_fps_count++;
		if (g_fps_last == 0) g_fps_last = now;
		if (now - g_fps_last >= 500) {
			g_fps = (int)(g_fps_count * 1000 / (now - g_fps_last));
			g_fps_count = 0;
			g_fps_last = now;
		}
	}

	// 用传入的 quad（若为 NULL 则取自身持有的）
	render_mesh_p q = quad ? quad : g_quad;

	if (g_font_ok && q) {
		int winW = 0, winH = 0;
		r3d_get_window_size(ctx->r3d, &winW, &winH);
		char fpsTxt[64];
		snprintf(fpsTxt, sizeof(fpsTxt), "FPS: %d", g_fps);
		r3d_font_draw_2d(ctx->r3d, q, fpsTxt, 10.0f, 10.0f, 24.0f,
						 (float)(winW > 0 ? winW : 760), (float)(winH > 0 ? winH : 480));
	}
}
