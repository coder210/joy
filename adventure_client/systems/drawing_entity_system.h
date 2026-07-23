#ifndef DRAWING_ENTITY_SYSTEM_H
#define DRAWING_ENTITY_SYSTEM_H
#include "../context.h"
#include "../../adventure_shared/components/id_component.h"
#include "../../adventure_shared/components/player_component.h"
#include "../../adventure_shared/components/logic_rect_component.h"
#include "../../adventure_shared/components/transform_component.h"

static void drawing_entity_system(Context* ctx, entt::entity e,
        IdComponent& id,
        LogicRectComponent& rect, 
        TransformComponent& t)
{
        jrect_t body = { 0 };
        body.x = (t.position.x - ctx->camera_x) * PIXELS_PER_METER;
        body.y = (t.position.y - ctx->camera_y) * PIXELS_PER_METER;
        body.w = fp_to_float(rect.width) * PIXELS_PER_METER;
        body.h = fp_to_float(rect.height) * PIXELS_PER_METER;
        if (ctx->ecs.all_of<PlayerComponent>(e)) {
                r2d_set_color(ctx->r2d, 140, 195, 196, 255);
                shape_draw_rectangle(ctx->r2d, "fill", body);
                return;
        }
        else {
                r2d_set_color(ctx->r2d, 140, 195, 196, 255);
                shape_draw_rectangle(ctx->r2d, "fill", body);
        }

        r2d_set_color(ctx->r2d, 0, 0, 0, 255);
}


#endif
