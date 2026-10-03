#ifndef DRAWING_COLLISION_DEBUG_PASS_H
#define DRAWING_COLLISION_DEBUG_PASS_H

#include "../components/logic_position_component.h"
#include "../components/logic_rect_component.h"
#include "../components/player_component.h"
#include "../components/local_only_component.h"
#include "../components/global_state_component.h"
#include "../protocol.h"

struct Context;

// 碰撞盒调试可视化：绘制玩家 AABB（淡墨灰）与墙 AABB（更淡暖灰），均为低透明度，
// 标注实体真实占位（LogicPosition 左上角 + LogicRect 宽高），避免喧宾夺主。
static void drawing_collision_debug_pass(Context* ctx)
{
    auto& cam = ctx->ecs.ctx().get<GlobalState>().camera_position;

    // 玩家碰撞盒：红色线框
    auto pview = ctx->ecs.view<PlayerComponent, LogicRectComponent, LogicPositionComponent>();
    r2d_set_color(ctx->r2d, 220, 45, 45, 180);
    for (auto e : pview) {
        auto& pos  = pview.get<LogicPositionComponent>(e);
        auto& rect = pview.get<LogicRectComponent>(e);
        float w = fp_to_float(rect.width);
        float h = fp_to_float(rect.height);
        if (w <= 0.0f || h <= 0.0f) continue;
        jrect_t r;
        r.x = (fp_to_float(pos.x) - cam.x) * PIXELS_PER_METER;
        r.y = (fp_to_float(pos.y) - cam.y) * PIXELS_PER_METER;
        r.w = w * PIXELS_PER_METER;
        r.h = h * PIXELS_PER_METER;
        shape_draw_rectangle(ctx->r2d, "line", r);
    }

    // 墙碰撞盒：绿色线框（墙由 map_spawn_system 生成为 LocalOnly 实体，1×1 米）
    auto wview = ctx->ecs.view<LocalOnlyComponent, LogicRectComponent, LogicPositionComponent>();
    r2d_set_color(ctx->r2d, 50, 200, 95, 150);
    for (auto e : wview) {
        auto& pos  = wview.get<LogicPositionComponent>(e);
        auto& rect = wview.get<LogicRectComponent>(e);
        float w = fp_to_float(rect.width);
        float h = fp_to_float(rect.height);
        if (w <= 0.0f || h <= 0.0f) continue;
        jrect_t r;
        r.x = (fp_to_float(pos.x) - cam.x) * PIXELS_PER_METER;
        r.y = (fp_to_float(pos.y) - cam.y) * PIXELS_PER_METER;
        r.w = w * PIXELS_PER_METER;
        r.h = h * PIXELS_PER_METER;
        shape_draw_rectangle(ctx->r2d, "line", r);
    }
}

#endif