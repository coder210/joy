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
//  - 按 MAP_LAYOUT 绘制地形装饰：墙/树/石头/水/草丛/花/小路
//  - 1 格 = 1m = SERVER_PPM 像素，随相机滚动
//  - 只绘制视口（480x320 逻辑像素）内的格子
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
			char c = map_tile_at(x, y);
			int type = map_tile_type(c);
			if (type == MAP_TILE_EMPTY) continue;

			float px = x * SERVER_PPM - cx;   // 格子左上角（屏幕坐标）
			float py = y * SERVER_PPM - cy;
			vec2_t center = { px + SERVER_PPM * 0.5f, py + SERVER_PPM * 0.5f };

			switch (type) {
			case MAP_TILE_WALL: {   // '#' 树篱墙
				r2d_set_color(ctx->r2d, 76, 120, 60, 255);
				jrect_t r = { px, py, SERVER_PPM, SERVER_PPM };
				shape_draw_rectangle(ctx->r2d, "fill", r);
				break;
			}
			case MAP_TILE_TREE: {   // 'T' 树：绿色树冠 + 棕色树干
				r2d_set_color(ctx->r2d, 96, 160, 72, 255);
				shape_draw_circle(ctx->r2d, "fill", center, SERVER_PPM * 0.38f, 16);
				r2d_set_color(ctx->r2d, 130, 90, 55, 255);
				jrect_t trunk = { center.x - 5.0f, center.y + 4.0f, 10.0f, 14.0f };
				shape_draw_rectangle(ctx->r2d, "fill", trunk);
				break;
			}
			case MAP_TILE_ROCK: {   // 'R' 石头：灰色圆 + 深色描边
				r2d_set_color(ctx->r2d, 156, 156, 162, 255);
				shape_draw_circle(ctx->r2d, "fill", center, SERVER_PPM * 0.34f, 12);
				r2d_set_color(ctx->r2d, 120, 120, 128, 255);
				shape_draw_circle(ctx->r2d, "line", center, SERVER_PPM * 0.34f, 12);
				break;
			}
			case MAP_TILE_WATER: {  // '~' 水：半透明浅蓝
				r2d_set_color(ctx->r2d, 110, 170, 230, 200);
				jrect_t r = { px, py, SERVER_PPM, SERVER_PPM };
				shape_draw_rectangle(ctx->r2d, "fill", r);
				break;
			}
			case MAP_TILE_GRASS:    // 'g' 草丛：浅绿圆点
				r2d_set_color(ctx->r2d, 170, 220, 130, 210);
				shape_draw_circle(ctx->r2d, "fill", center, 7.0f, 10);
				break;
			case MAP_TILE_FLOWER:   // 'f' 花：粉红圆点
				r2d_set_color(ctx->r2d, 235, 150, 180, 255);
				shape_draw_circle(ctx->r2d, "fill", center, 5.0f, 8);
				break;
			case MAP_TILE_PATH: {   // 'p' 小路：米黄圆角感矩形
				r2d_set_color(ctx->r2d, 218, 198, 152, 220);
				jrect_t r = { px + 6.0f, py + 6.0f, SERVER_PPM - 12.0f, SERVER_PPM - 12.0f };
				shape_draw_rectangle(ctx->r2d, "fill", r);
				break;
			}
			default:
				break;
			}
		}
	}
}

#endif
