#ifndef DRAWING_MAP_PASS_H
#define DRAWING_MAP_PASS_H

#include <math.h>
#include <joy/joy.h>
#include "../components/global_state_component.h"
#include "../systems/map_def.h"
#include "../protocol.h"

struct Context;

static void draw_map_pass(Context* ctx)
{
    auto& gs = ctx->ecs.ctx().get<GlobalState>();
    float cx = gs.camera_position.x * SERVER_PPM;
    float cy = gs.camera_position.y * SERVER_PPM;

    const float VP_W = 480.0f, VP_H = 320.0f;

    int x0 = (int)floorf(cx / SERVER_PPM) - 1;
    int y0 = (int)floorf(cy / SERVER_PPM) - 1;
    int x1 = (int)ceilf((cx + VP_W) / SERVER_PPM) + 1;
    int y1 = (int)ceilf((cy + VP_H) / SERVER_PPM) + 1;

    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            if (x < 0 || y < 0 || x >= MAP_W || y >= MAP_H) continue;
            char c = map_tile_at(x, y);
            int type = map_tile_type(c);
            if (type == MAP_TILE_EMPTY) continue;

            float px = x * SERVER_PPM - cx;
            float py = y * SERVER_PPM - cy;
            vec2_t center = { px + SERVER_PPM * 0.5f, py + SERVER_PPM * 0.5f };

            switch (type) {
            case MAP_TILE_WALL: {
                /* 白底 + 墨色描边（图纸风，对齐 3D 墙体） */
                r2d_set_color(ctx->r2d, 250, 248, 244, 255);
                jrect_t r = { px, py, SERVER_PPM, SERVER_PPM };
                shape_draw_rectangle(ctx->r2d, "fill", r);
                r2d_set_color(ctx->r2d, 40, 40, 45, 255);
                float mx = px + SERVER_PPM * 0.5f;
                float my = py + SERVER_PPM * 0.5f;
                shape_draw_line(ctx->r2d, mx, py, mx, py + SERVER_PPM);
                shape_draw_line(ctx->r2d, px, my, px + SERVER_PPM, my);
                jrect_t b = { px + 1.0f, py + 1.0f, SERVER_PPM - 2.0f, SERVER_PPM - 2.0f };
                shape_draw_rectangle(ctx->r2d, "line", b);
                break;
            }
            case MAP_TILE_TREE: {
                /* 墨色树：淡墨圈 + 实心墨点 */
                r2d_set_color(ctx->r2d, 90, 90, 98, 255);
                shape_draw_circle(ctx->r2d, "line", center, SERVER_PPM * 0.36f, 16);
                r2d_set_color(ctx->r2d, 90, 90, 98, 255);
                shape_draw_circle(ctx->r2d, "fill", center, SERVER_PPM * 0.12f, 12);
                break;
            }
            case MAP_TILE_ROCK: {
                r2d_set_color(ctx->r2d, 168, 166, 160, 255);
                shape_draw_circle(ctx->r2d, "fill", center, SERVER_PPM * 0.32f, 12);
                r2d_set_color(ctx->r2d, 40, 40, 45, 255);
                shape_draw_circle(ctx->r2d, "line", center, SERVER_PPM * 0.32f, 12);
                break;
            }
            case MAP_TILE_WATER: {
                /* 水：淡宣纸底 + 淡墨波纹（去蓝，改图纸灰） */
                r2d_set_color(ctx->r2d, 228, 228, 232, 255);
                jrect_t wr = { px, py, SERVER_PPM, SERVER_PPM };
                shape_draw_rectangle(ctx->r2d, "fill", wr);
                r2d_set_color(ctx->r2d, 150, 152, 162, 255);
                float w = SERVER_PPM * 0.66f;
                float hw = w * 0.5f;
                for (int k = -1; k <= 1; k++) {
                    float yy = center.y + k * SERVER_PPM * 0.18f;
                    shape_draw_line(ctx->r2d, center.x - hw, yy, center.x + hw, yy);
                }
                break;
            }
            case MAP_TILE_GRASS:
                r2d_set_color(ctx->r2d, 150, 150, 158, 255);
                {
                    float s = 5.0f;
                    jrect_t grass = { center.x - s, center.y - s, s * 2, s * 2 };
                    shape_draw_rectangle(ctx->r2d, "fill", grass);
                }
                break;
            case MAP_TILE_FLOWER:
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
            case MAP_TILE_PATH: {
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