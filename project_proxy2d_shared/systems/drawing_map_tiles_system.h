#ifndef DRAWING_MAP_TILES_SYSTEM_H
#define DRAWING_MAP_TILES_SYSTEM_H
#include <math.h>
#include <joy/joy.h>
#include "../components/global_state_component.h"
#include "map_def.h"
#include "../protocol.h"

struct Context;

// ============================================================
// 地图瓦片几何渲染（纯视觉，不参与帧同步逻辑）
//  - 按 MAP_LAYOUT 绘制地形装饰，风格统一为《几何决斗》极简几何：
//      白底画布 + 黑/灰几何形状（线框/圆环/几何点阵），无写实绿色植被
//  - 1 格 = 1m = SERVER_PPM 像素，随相机滚动
//  - 只绘制视口（480x320 逻辑像素）内的格子
//  - 越界格子（空气墙外）不绘制任何地形，保持白底，与地图内部一致
//  - 与 draw_grid_background 配合使用（先网格背景，后地形）
// ============================================================
static void draw_map_tiles(Context* ctx)
{
	auto& gs = ctx->ecs.ctx().get<GlobalState>();
	float cx = gs.camera_position.x * SERVER_PPM;
	float cy = gs.camera_position.y * SERVER_PPM;

	const float VP_W = 480.0f, VP_H = 320.0f;

	// 可视格子范围（向外扩 1 格，保证边缘完整）
	int x0 = (int)floorf(cx / SERVER_PPM) - 1;
	int y0 = (int)floorf(cy / SERVER_PPM) - 1;
	int x1 = (int)ceilf((cx + VP_W) / SERVER_PPM) + 1;
	int y1 = (int)ceilf((cy + VP_H) / SERVER_PPM) + 1;

	for (int y = y0; y <= y1; y++) {
		for (int x = x0; x <= x1; x++) {
			// 越界格子（空气墙外）保持白底，不绘制地形装饰
			if (x < 0 || y < 0 || x >= MAP_W || y >= MAP_H) continue;
			char c = map_tile_at(x, y);
			int type = map_tile_type(c);
			if (type == MAP_TILE_EMPTY || type == MAP_TILE_WALL) continue;

			float px = x * SERVER_PPM - cx;   // 格子左上角（屏幕坐标）
			float py = y * SERVER_PPM - cy;
			vec2_t center = { px + SERVER_PPM * 0.5f, py + SERVER_PPM * 0.5f };

			switch (type) {
			case MAP_TILE_WALL: {   // '#' 边界墙：几何砖格（深灰填充 + 浅色十字分割）
				r2d_set_color(ctx->r2d, 60, 62, 68, 255);
				jrect_t r = { px, py, SERVER_PPM, SERVER_PPM };
				shape_draw_rectangle(ctx->r2d, "fill", r);
				r2d_set_color(ctx->r2d, 228, 228, 232, 255);
				float mx = px + SERVER_PPM * 0.5f;
				float my = py + SERVER_PPM * 0.5f;
				shape_draw_line(ctx->r2d, mx, py, mx, py + SERVER_PPM);
				shape_draw_line(ctx->r2d, px, my, px + SERVER_PPM, my);
				break;
			}
			case MAP_TILE_TREE: {   // 'T' 树：黑色圆环（几何感，无绿色）
				r2d_set_color(ctx->r2d, 45, 45, 50, 255);
				shape_draw_circle(ctx->r2d, "line", center, SERVER_PPM * 0.36f, 16);
				r2d_set_color(ctx->r2d, 45, 45, 50, 255);
				shape_draw_circle(ctx->r2d, "fill", center, SERVER_PPM * 0.12f, 12);
				break;
			}
			case MAP_TILE_ROCK: {   // 'R' 石头：深灰实心圆 + 白色描边
				r2d_set_color(ctx->r2d, 80, 80, 88, 255);
				shape_draw_circle(ctx->r2d, "fill", center, SERVER_PPM * 0.32f, 12);
				r2d_set_color(ctx->r2d, 240, 240, 244, 255);
				shape_draw_circle(ctx->r2d, "line", center, SERVER_PPM * 0.32f, 12);
				break;
			}
			case MAP_TILE_WATER: {  // '~' 水：浅蓝几何波纹（三条横线）
				r2d_set_color(ctx->r2d, 96, 148, 214, 255);
				float w = SERVER_PPM * 0.66f;
				float hw = w * 0.5f;
				for (int k = -1; k <= 1; k++) {
					float yy = center.y + k * SERVER_PPM * 0.18f;
					shape_draw_line(ctx->r2d, center.x - hw, yy, center.x + hw, yy);
				}
				break;
			}
			case MAP_TILE_GRASS:    // 'g' 草丛：几何小方块点阵
				r2d_set_color(ctx->r2d, 150, 150, 158, 255);
				{
					float s = 5.0f;
					jrect_t grass = { center.x - s, center.y - s, s * 2, s * 2 };
					shape_draw_rectangle(ctx->r2d, "fill", grass);
				}
				break;
			case MAP_TILE_FLOWER:   // 'f' 花：几何菱形点
				r2d_set_color(ctx->r2d, 205, 205, 212, 255);
				{
					float s = 4.0f;
					vec2_t pts[4] = {
						{ center.x,         center.y - s * 1.5f },
						{ center.x + s,     center.y },
						{ center.x,         center.y + s * 1.5f },
						{ center.x - s,     center.y },
					};
					jcolor_t flower = { 205, 205, 212, 255 };
					shape_draw_polygon(ctx->r2d, "fill", pts, 4, flower);
				}
				break;
			case MAP_TILE_PATH: {   // 'p' 小路：浅灰线框矩形
				r2d_set_color(ctx->r2d, 176, 176, 184, 255);
				jrect_t r = { px + 8.0f, py + 8.0f, SERVER_PPM - 16.0f, SERVER_PPM - 16.0f };
				shape_draw_rectangle(ctx->r2d, "line", r);
				break;
			}
			default:
				break;
			}
		}
	}
}

#endif
