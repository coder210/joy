#ifndef DRAWING_ATTACK_RAY_EFFECT_SYSTEM_H
#define DRAWING_ATTACK_RAY_EFFECT_SYSTEM_H
#include "../context.h"
#include "../../adventure_shared/components/attack_ray_effect_component.h"


static void 
drawing_attack_ray_effect_system(Context* ctx, 
        AttackRayEffectComponent& effect)
{
        jrect_t line;
        line.x = (effect.x - ctx->camera_x) * PIXELS_PER_METER;
        line.y = (effect.y - ctx->camera_y) * PIXELS_PER_METER;
        line.w = effect.w * PIXELS_PER_METER;
        line.h = effect.h * PIXELS_PER_METER;
        r2d_set_color(ctx->r2d, 255, 255, 255, 255);
        shape_draw_rectangle(ctx->r2d, "fill", line);
}


#endif
