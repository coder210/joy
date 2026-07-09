#ifndef DRAWING_ATTACK_RAY_EFFECT_SYSTEM_H
#define DRAWING_ATTACK_RAY_EFFECT_SYSTEM_H
#include "../flecs.h"
#include "../context.h"
#include "../components/attack_ray_effect_component.h"


static void 
drawing_attack_ray_effect_system(flecs::entity e, 
        AttackRayEffectComponent& effect)
{
        Context* ctx = (Context* )e.world().get_ctx();
        jrect_t line;
        line.x = (effect.x - ctx->camera_x) * PIXELS_PER_METER;
        line.y = (effect.y - ctx->camera_y) * PIXELS_PER_METER;
        line.w = effect.w * PIXELS_PER_METER;
        line.h = effect.h * PIXELS_PER_METER;
        r2d_set_color(ctx->r2d, 255, 255, 255, 255);
        shape_draw_rectangle(ctx->r2d, "fill", line);
}


#endif