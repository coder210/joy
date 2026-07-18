#ifndef DRAWING_ENTITY_SYSTEM_H
#define DRAWING_ENTITY_SYSTEM_H
#include "../context.h"
#include "../scenes/game_scene.h"
#include "../../adventure_shared/protocol.h"
#include "../components/id_component.h"
#include "../components/player_component.h"
#include "../components/logic_rect_component.h"
#include "../components/transform_component.h"

static void drawing_entity_system(Context* ctx, entt::entity e,
        IdComponent& id,
        LogicRectComponent& rect, 
        TransformComponent& t)
{
        jrect_t body_rect = { 0 };
        auto& cam = ctx->ecs.get<CameraComponent>(ctx->singleton_camera).position;
        body_rect.x = (t.position.x - cam.x) * SERVER_PPM;
        body_rect.y = (t.position.y - cam.y) * SERVER_PPM;
        body_rect.w = fp_to_float(rect.width) * SERVER_PPM;
        body_rect.h = fp_to_float(rect.height) * SERVER_PPM;
        if (ctx->ecs.all_of<PlayerComponent>(e)) {
                r2d_set_color(ctx->r2d, 140, 195, 196, 255);
                shape_draw_rectangle(ctx->r2d, "fill", body_rect);
                return;
        }
        else {
            r2d_set_color(ctx->r2d, 140, 195, 196, 255);
            shape_draw_rectangle(ctx->r2d, "fill", body_rect);
        }
}

#endif
