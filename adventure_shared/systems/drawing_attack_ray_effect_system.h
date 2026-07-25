#ifndef DRAWING_ATTACK_RAY_EFFECT_SYSTEM_H
#define DRAWING_ATTACK_RAY_EFFECT_SYSTEM_H
#include "../components/attack_ray_effect_component.h"
#include "../components/global_state_component.h"
#include "../protocol.h"

struct Context;

static void 
drawing_attack_ray_effect_system(Context* ctx, 
        AttackRayEffectComponent& effect)
{
        auto& cam = ctx->ecs.ctx().get<GlobalState>().camera_position;
        jrect_t line;
        line.x = (effect.x - cam.x) * PIXELS_PER_METER;
        line.y = (effect.y - cam.y) * PIXELS_PER_METER;
        line.w = effect.w * PIXELS_PER_METER;
        line.h = effect.h * PIXELS_PER_METER;
        r2d_set_color(ctx->r2d, 255, 255, 255, 255);
        shape_draw_rectangle(ctx->r2d, "fill", line);
}


#endif
