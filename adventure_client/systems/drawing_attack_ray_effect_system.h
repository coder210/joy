#ifndef DRAWING_ATTACK_RAY_EFFECT_SYSTEM_H
#define DRAWING_ATTACK_RAY_EFFECT_SYSTEM_H
#include "../flecs.h"
#include <joy/jshapes.h>
#include "../context.h"
#include "../components/attack_ray_effect_component.h"


static void 
drawing_attack_ray_effect_system(flecs::entity e, 
        AttackRayEffectComponent& effect)
{
        struct context* ctx = (struct context*)e.world().get_ctx();
        SDL_FRect line;
        line.x = (effect.x - ctx->camera_x) * PIXELS_PER_METER;
        line.y = (effect.y - ctx->camera_y) * PIXELS_PER_METER;
        line.w = effect.w * PIXELS_PER_METER;
        line.h = effect.h * PIXELS_PER_METER;
        SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255);
        shape_draw_rectangle(ctx->renderer, "fill", line);
}


#endif