#pragma once
#include <math.h>
#include "../components/global_state_component.h"
#include "../components/id_component.h"
#include "../components/player_component.h"
#include "../components/enemy_component.h"
#include "../components/logic_rect_component.h"
#include "../components/transform_component.h"
#include "../components/action_state_component.h"
#include "../protocol.h"
#include "map_def.h"

struct Context;

// ============================================================
// 纯矩形渲染系统（纯视觉，只读组件，不参与帧同步逻辑）
//   - 地形：矩形块
//   - 玩家：亮蓝矩形 + 白描边 + 朝向指示线
//   - 敌人：橙红矩形
//   - 其他：青灰矩形
//   - 攻击中（ActionState==ATTACK）：沿朝向画一段攻击射线
// ============================================================

// 世界坐标网格背景（跟随相机滚动）
static void draw_grid_background(Context* ctx)
{
        auto& cam = ctx->ecs.ctx().get<GlobalState>().camera_position;
        const float W = 480.0f;
        const float H = 320.0f;
        const float CELL = 40.0f;                       // 网格间距(px)
        float ox = -fmodf(cam.x * SERVER_PPM, CELL);    // 网格随相机偏移
        float oy = -fmodf(cam.y * SERVER_PPM, CELL);
        r2d_set_color(ctx->r2d, 42, 54, 74, 255);
        for (float x = ox; x < W; x += CELL)
                shape_draw_line(ctx->r2d, x, 0.0f, x, H);
        for (float y = oy; y < H; y += CELL)
                shape_draw_line(ctx->r2d, 0.0f, y, W, y);
}

// 地图矩形绘制工具：以中心点 + 半宽半高画填充矩形
static void draw_fill_rect(Context* ctx, float cx, float cy, float hw, float hh, jcolor_t col)
{
        vec2_t pts[4] = {
                { cx - hw, cy - hh },
                { cx + hw, cy - hh },
                { cx + hw, cy + hh },
                { cx - hw, cy + hh }
        };
        shape_draw_polygon(ctx->r2d, "fill", pts, 4, col);
}

static void draw_fill_rect_line(Context* ctx, float cx, float cy, float hw, float hh, jcolor_t fill, jcolor_t line)
{
        vec2_t pts[4] = {
                { cx - hw, cy - hh },
                { cx + hw, cy - hh },
                { cx + hw, cy + hh },
                { cx - hw, cy + hh }
        };
        shape_draw_polygon(ctx->r2d, "fill", pts, 4, fill);
        shape_draw_polygon(ctx->r2d, "line", pts, 4, line);
}

// 地形装饰绘制：遍历相机可视范围内的 MAP_LAYOUT 格子并绘制对应矩形
static void draw_map_tiles(Context* ctx)
{
        auto& cam = ctx->ecs.ctx().get<GlobalState>().camera_position;
        const float VIEW_W = 480.0f / SERVER_PPM;   // 可见世界宽度(米)
        const float VIEW_H = 320.0f / SERVER_PPM;   // 可见世界高度(米)

        int x0 = (int)floorf(cam.x - VIEW_W * 0.5f - 1.0f);
        int x1 = (int)ceilf (cam.x + VIEW_W * 0.5f + 1.0f);
        int y0 = (int)floorf(cam.y - VIEW_H * 0.5f - 1.0f);
        int y1 = (int)ceilf (cam.y + VIEW_H * 0.5f + 1.0f);

        for (int ty = y0; ty <= y1; ty++) {
                for (int tx = x0; tx <= x1; tx++) {
                        char c = map_tile_at(tx, ty);
                        if (c == '.') continue;

                        float cx = ((float)tx + 0.5f - cam.x) * SERVER_PPM;
                        float cy = ((float)ty + 0.5f - cam.y) * SERVER_PPM;
                        float s  = SERVER_PPM * 0.5f;   // 半格(px)

                        switch (map_tile_type(c)) {
                        case MAP_TILE_WALL: {
                                // 树篱墙：深绿矩形
                                jcolor_t col  = { 46, 68, 51, 255 };
                                jcolor_t lcol = { 60, 92, 62, 255 };
                                draw_fill_rect_line(ctx, cx, cy, s, s, col, lcol);
                                break;
                        }
                        case MAP_TILE_TREE: {
                                // 树干：棕色矩形
                                jcolor_t tcol = { 101, 67, 40, 255 };
                                draw_fill_rect(ctx, cx, cy + s * 0.15f, s * 0.25f, s * 0.2f, tcol);
                                // 树冠：两层绿色矩形叠出立体感
                                jcolor_t col1 = { 42, 108, 58, 255 };
                                draw_fill_rect(ctx, cx, cy - s * 0.15f, s * 0.75f, s * 0.65f, col1);
                                jcolor_t col2 = { 72, 142, 82, 255 };
                                draw_fill_rect(ctx, cx, cy - s * 0.2f, s * 0.45f, s * 0.4f, col2);
                                break;
                        }
                        case MAP_TILE_ROCK: {
                                // 石头：灰色矩形
                                jcolor_t col  = { 132, 132, 148, 255 };
                                jcolor_t lcol = { 166, 166, 178, 255 };
                                draw_fill_rect_line(ctx, cx, cy, s * 0.75f, s * 0.6f, col, lcol);
                                break;
                        }
                        case MAP_TILE_WATER: {
                                // 水面：蓝色矩形
                                jcolor_t col = { 42, 104, 178, 255 };
                                draw_fill_rect(ctx, cx, cy, s, s, col);
                                // 波纹线（保留两条短线示意水面）
                                r2d_set_color(ctx->r2d, 130, 195, 238, 200);
                                shape_draw_line(ctx->r2d, cx - s * 0.6f, cy - s * 0.3f, cx, cy - s * 0.3f);
                                shape_draw_line(ctx->r2d, cx - s * 0.4f, cy + s * 0.2f, cx + s * 0.5f, cy + s * 0.2f);
                                break;
                        }
                        case MAP_TILE_GRASS: {
                                // 草丛：嫩绿小矩形
                                jcolor_t col = { 96, 168, 96, 255 };
                                draw_fill_rect(ctx, cx, cy, s * 0.35f, s * 0.25f, col);
                                break;
                        }
                        case MAP_TILE_FLOWER: {
                                // 花：粉色矩形（花瓣）+ 黄色小矩形（花蕊）
                                jcolor_t pcol = { 232, 150, 190, 255 };
                                draw_fill_rect(ctx, cx, cy, s * 0.35f, s * 0.35f, pcol);
                                jcolor_t ycol = { 255, 220, 90, 255 };
                                draw_fill_rect(ctx, cx, cy, s * 0.14f, s * 0.14f, ycol);
                                break;
                        }
                        case MAP_TILE_PATH: {
                                // 小路：浅棕矩形
                                jcolor_t col = { 158, 130, 96, 255 };
                                draw_fill_rect(ctx, cx, cy, s * 0.45f, s * 0.45f, col);
                                break;
                        }
                        default: break;
                        }
                }
        }
}

static void drawing_entity_system(Context* ctx, entt::entity e,
        IdComponent& id,
        LogicRectComponent& rect,
        TransformComponent& t)
{
        (void)id;
        auto& cam = ctx->ecs.ctx().get<GlobalState>().camera_position;
        float cx = (t.position.x - cam.x) * SERVER_PPM;
        float cy = (t.position.y - cam.y) * SERVER_PPM;
        float w  = fp_to_float(rect.width)  * SERVER_PPM;
        float h  = fp_to_float(rect.height) * SERVER_PPM;
        float hw = w * 0.5f;
        float hh = h * 0.5f;

        // 朝向角：优先 ActionStateComponent.dir（0=down 1=left 2=right 3=up），
        // 否则用 TransformComponent.rotation
        float dir_rad = t.rotation;
        int   act_state = -1;
        if (ctx->ecs.all_of<ActionStateComponent>(e)) {
                auto& act = ctx->ecs.get<ActionStateComponent>(e);
                act_state = (int)act.state;
                switch (act.dir) {
                case 0: dir_rad = 3.14159265f * 0.5f; break;        // down
                case 1: dir_rad = 3.14159265f;        break;        // left
                case 2: dir_rad = 0.0f;               break;        // right
                case 3: dir_rad = -3.14159265f * 0.5f; break;       // up
                default: break;
                }
        }

        if (ctx->ecs.all_of<PlayerComponent>(e)) {
                // ---- 玩家：亮蓝矩形 + 白描边 + 朝向线 ----
                jcolor_t fill = { 90, 170, 255, 255 };
                jcolor_t line = { 255, 255, 255, 90 };
                draw_fill_rect_line(ctx, cx, cy, hw, hh, fill, line);
                // 朝向指示线
                r2d_set_color(ctx->r2d, 255, 255, 255, 230);
                shape_draw_line(ctx->r2d, cx, cy,
                        cx + cosf(dir_rad) * hw, cy + sinf(dir_rad) * hh);
        }
        else if (ctx->ecs.all_of<EnemyComponent>(e)) {
                // ---- 敌人：橙红矩形 ----
                jcolor_t fill = { 230, 95, 75, 255 };
                jcolor_t line = { 255, 255, 255, 80 };
                draw_fill_rect_line(ctx, cx, cy, hw, hh, fill, line);
        }
        else {
                // ---- 地图碰撞地形实体由 draw_map_tiles 负责绘制，跳过 ----
                char tile = map_tile_at((int)floorf(t.position.x), (int)floorf(t.position.y));
                if (map_is_collidable(tile)) return;

                // ---- 其他：青灰矩形 ----
                jcolor_t fill = { 140, 195, 196, 255 };
                draw_fill_rect(ctx, cx, cy, hw, hh, fill);
        }

        // ---- 攻击指示：ATTACK 时沿朝向画一段射线（纯视觉反馈）----
        if (act_state == ActionStateComponent::ATTACK) {
                float len = (hw < hh ? hw : hh) * 2.2f;
                float r = (hw < hh ? hw : hh) * 0.6f;
                r2d_set_color(ctx->r2d, 255, 255, 255, 200);
                shape_draw_line(ctx->r2d,
                        cx + cosf(dir_rad) * r, cy + sinf(dir_rad) * r,
                        cx + cosf(dir_rad) * len, cy + sinf(dir_rad) * len);
        }
}