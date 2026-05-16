#ifndef EFFECT_LIFECYCLE_SYSTEM_H
#define EFFECT_LIFECYCLE_SYSTEM_H
#include "../flecs.h"
#include "../context.h"
#include "../components/attack_ray_effect_component.h"

static void effect_lifecycle_system(flecs::entity e, AttackRayEffectComponent& effect)
{
        struct context* ctx = (struct context*)e.world().get_ctx();
        effect.lifetime -= ctx->FIXED_TIMESTEP;  // ʹù̶ͬ
        if (effect.lifetime <= 0.0f) {
                e.destruct();
        }
}


#endif