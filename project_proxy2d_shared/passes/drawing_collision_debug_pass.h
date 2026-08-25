#ifndef DRAWING_COLLISION_DEBUG_PASS_H
#define DRAWING_COLLISION_DEBUG_PASS_H

#include "../components/logic_position_component.h"
#include "../components/logic_rect_component.h"
#include "../components/global_state_component.h"
#include "../protocol.h"

struct Context;

static void drawing_collision_debug_pass(Context* ctx)
{
    auto& cam = ctx->ecs.ctx().get<GlobalState>().camera_position;
    auto view = ctx->ecs.view<LogicRectComponent, LogicPositionComponent>();
    r2d_set_color(ctx->r2d, 255, 40, 40, 255);
    for (auto e : view) {
        auto& pos  = view.get<LogicPositionComponent>(e);
        auto& rect = view.get<LogicRectComponent>(e);
        float w = fp_to_float(rect.width);
        float h = fp_to_float(rect.height);
        if (w <= 0.0f || h <= 0.0f) continue;
        jrect_t r;
        r.x = (fp_to_float(pos.x) - cam.x) * SERVER_PPM;
        r.y = (fp_to_float(pos.y) - cam.y) * SERVER_PPM;
        r.w = w * SERVER_PPM;
        r.h = h * SERVER_PPM;
        shape_draw_rectangle(ctx->r2d, "line", r);
    }
}

#endif