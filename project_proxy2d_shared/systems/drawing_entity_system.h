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
// 几何代理战斗渲染系统（纯视觉，只读组件，不参与帧同步逻辑）
//   - 地形：树/石头/水/草丛/小路（draw_map_tiles，基于 MAP_LAYOUT）
//   - 玩家：亮蓝圆 + 白描边 + 朝向指示线
//   - 敌人：橙红三角（顶点指向朝向）
//   - 其他：青灰菱形（地图碰撞地形实体除外）
//   - 攻击中（ActionState==ATTACK）：沿朝向画一段攻击射线
// ============================================================

// 世界坐标网格背景（呼应几何/坐标感，跟随相机滚动）
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

// 地形装饰绘制：遍历相机可视范围内的 MAP_LAYOUT 格子并绘制对应图形
static void draw_map_tiles(Context* ctx)
{
        auto& cam = ctx->ecs.ctx().get<GlobalState>().camera_position;
        const float VIEW_W = 480.0f / SERVER_PPM;   // 可见世界宽度(米)
        const float VIEW_H = 320.0f / SERVER_PPM;   // 可见世界高度(米)
        const float PI = 3.14159265f;

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
                        vec2_t center = { cx, cy };

                        switch (map_tile_type(c)) {
                        case MAP_TILE_WALL: {
                                // 树篱墙
                                vec2_t pts[4] = { {cx-s,cy-s},{cx+s,cy-s},{cx+s,cy+s},{cx-s,cy+s} };
                                jcolor_t col = { 46, 68, 51, 255 };
                                shape_draw_polygon(ctx->r2d, "fill", pts, 4, col);
                                jcolor_t lc  = { 60, 92, 62, 255 };
                                shape_draw_polygon(ctx->r2d, "line", pts, 4, lc);
                                break;
                        }
                        case MAP_TILE_TREE: {
                                // 树干
                                vec2_t trunk[4] = {
                                        {cx-s*0.2f, cy+s*0.25f}, {cx+s*0.2f, cy+s*0.25f},
                                        {cx+s*0.2f, cy+s*0.6f},  {cx-s*0.2f, cy+s*0.6f} };
                                jcolor_t tcol = { 101, 67, 40, 255 };
                                shape_draw_polygon(ctx->r2d, "fill", trunk, 4, tcol);
                                // 树冠（两层绿，制造立体感）
                                r2d_set_color(ctx->r2d, 42, 108, 58, 255);
                                shape_draw_circle(ctx->r2d, "fill", center, s*0.85f, 24);
                                r2d_set_color(ctx->r2d, 72, 142, 82, 255);
                                shape_draw_circle(ctx->r2d, "fill", center, s*0.55f, 24);
                                break;
                        }
                        case MAP_TILE_ROCK: {
                                // 石头（六边形）
                                vec2_t pts[6];
                                for (int i = 0; i < 6; i++) {
                                        float a = PI * (float)i / 3.0f + PI * 0.5f;
                                        pts[i] = { cx + cosf(a)*s*0.8f, cy + sinf(a)*s*0.8f };
                                }
                                jcolor_t col  = { 132, 132, 148, 255 };
                                shape_draw_polygon(ctx->r2d, "fill", pts, 6, col);
                                jcolor_t lcol = { 166, 166, 178, 255 };
                                shape_draw_polygon(ctx->r2d, "line", pts, 6, lcol);
                                break;
                        }
                        case MAP_TILE_WATER: {
                                // 水面 + 波纹
                                vec2_t pts[4] = { {cx-s,cy-s},{cx+s,cy-s},{cx+s,cy+s},{cx-s,cy+s} };
                                jcolor_t col = { 42, 104, 178, 255 };
                                shape_draw_polygon(ctx->r2d, "fill", pts, 4, col);
                                r2d_set_color(ctx->r2d, 130, 195, 238, 200);
                                shape_draw_line(ctx->r2d, cx-s*0.6f, cy-s*0.3f, cx, cy-s*0.3f);
                                shape_draw_line(ctx->r2d, cx-s*0.4f, cy+s*0.2f, cx+s*0.5f, cy+s*0.2f);
                                break;
                        }
                        case MAP_TILE_GRASS: {
                                // 草丛：三根短线
                                r2d_set_color(ctx->r2d, 96, 168, 96, 255);
                                shape_draw_line(ctx->r2d, cx-s*0.55f, cy+s*0.45f, cx-s*0.65f, cy-s*0.30f);
                                shape_draw_line(ctx->r2d, cx,          cy+s*0.50f, cx,          cy-s*0.40f);
                                shape_draw_line(ctx->r2d, cx+s*0.55f, cy+s*0.45f, cx+s*0.65f, cy-s*0.30f);
                                break;
                        }
                        case MAP_TILE_FLOWER: {
                                // 花：粉瓣 + 黄芯
                                r2d_set_color(ctx->r2d, 232, 150, 190, 255);
                                shape_draw_circle(ctx->r2d, "fill", center, s*0.35f, 12);
                                r2d_set_color(ctx->r2d, 255, 220, 90, 255);
                                shape_draw_circle(ctx->r2d, "fill", center, s*0.14f, 8);
                                break;
                        }
                        case MAP_TILE_PATH: {
                                // 小路：浅棕块
                                vec2_t pts[4] = {
                                        {cx-s*0.45f, cy-s*0.45f}, {cx+s*0.45f, cy-s*0.45f},
                                        {cx+s*0.45f, cy+s*0.45f}, {cx-s*0.45f, cy+s*0.45f} };
                                jcolor_t col = { 158, 130, 96, 255 };
                                shape_draw_polygon(ctx->r2d, "fill", pts, 4, col);
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
        vec2_t center = { cx, cy };
        float radius  = (w < h ? w : h) * 0.5f;

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
                // ---- 玩家：亮蓝圆 + 白描边 + 朝向线 ----
                r2d_set_color(ctx->r2d, 90, 170, 255, 255);
                shape_draw_circle(ctx->r2d, "fill", center, radius, 32);
                r2d_set_color(ctx->r2d, 255, 255, 255, 90);
                shape_draw_circle(ctx->r2d, "line", center, radius, 32);
                r2d_set_color(ctx->r2d, 255, 255, 255, 230);
                shape_draw_line(ctx->r2d, cx, cy,
                        cx + cosf(dir_rad) * radius, cy + sinf(dir_rad) * radius);
        }
        else if (ctx->ecs.all_of<EnemyComponent>(e)) {
                // ---- 敌人：橙红三角（顶点指向朝向）----
                vec2_t pts[3];
                float R = (w < h ? w : h) * 0.5f;
                for (int i = 0; i < 3; i++) {
                        float a = dir_rad + (float)i * 2.09439510239f;
                        pts[i] = { cx + cosf(a) * R, cy + sinf(a) * R };
                }
                jcolor_t col = { 230, 95, 75, 255 };
                shape_draw_polygon(ctx->r2d, "fill", pts, 3, col);
                r2d_set_color(ctx->r2d, 255, 255, 255, 80);
                shape_draw_polygon(ctx->r2d, "line", pts, 3, col);
        }
        else {
                // ---- 地图碰撞地形实体由 draw_map_tiles 负责绘制，跳过菱形 ----
                char tile = map_tile_at((int)floorf(t.position.x), (int)floorf(t.position.y));
                if (map_is_collidable(tile)) return;

                // ---- 其他：青灰菱形 ----
                vec2_t pts[4];
                pts[0] = { cx,            cy - h * 0.5f };
                pts[1] = { cx + w * 0.5f, cy };
                pts[2] = { cx,            cy + h * 0.5f };
                pts[3] = { cx - w * 0.5f, cy };
                jcolor_t col = { 140, 195, 196, 255 };
                shape_draw_polygon(ctx->r2d, "fill", pts, 4, col);
        }

        // ---- 攻击指示：ATTACK 时沿朝向画一段射线（纯视觉反馈）----
        if (act_state == ActionStateComponent::ATTACK) {
                float len = radius * 2.2f;
                r2d_set_color(ctx->r2d, 255, 255, 255, 200);
                shape_draw_line(ctx->r2d,
                        cx + cosf(dir_rad) * radius * 0.6f, cy + sinf(dir_rad) * radius * 0.6f,
                        cx + cosf(dir_rad) * len,           cy + sinf(dir_rad) * len);
        }
}
