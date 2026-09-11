#ifndef DRAWING_ATTACK_RAY_EFFECT_PASS_H
#define DRAWING_ATTACK_RAY_EFFECT_PASS_H
#include "entt.hpp"
#include "../components/attack_ray_effect_component.h"
#include "../components/global_state_component.h"
#include "../protocol.h"


struct Context;

static void drawing_attack_ray_effect_pass(entt::registry& registry, Context* ctx, AttackRayEffectComponent& effect)
{
        /*auto& cam = ctx->ecs.ctx().get<GlobalState>().camera_position;
        jrect_t line;
        line.x = (effect.x - cam.x) * SERVER_PPM;
        line.y = (effect.y - cam.y) * SERVER_PPM;
        line.w = effect.w * SERVER_PPM;
        line.h = effect.h * SERVER_PPM;
        r2d_set_color(ctx->r2d, 40, 40, 55, 210);
        shape_draw_rectangle(ctx->r2d, "fill", line);*/
}

#endif