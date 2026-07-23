#ifndef EFFECT_LIFECYCLE_SYSTEM_H
#define EFFECT_LIFECYCLE_SYSTEM_H
#include "../context.h"
#include "../../adventure_shared/components/attack_ray_effect_component.h"

static void effect_lifecycle_system(Context* ctx, entt::registry& registry, entt::entity e, AttackRayEffectComponent& effect, float dt)
{
        effect.lifetime -= dt;
        if (effect.lifetime <= 0.0f) {
                registry.destroy(e);
        }
}


#endif
