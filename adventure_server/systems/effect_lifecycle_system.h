#ifndef EFFECT_LIFECYCLE_SYSTEM_H
#define EFFECT_LIFECYCLE_SYSTEM_H
#include "../flecs.h"
#include "../components/attack_ray_effect_component.h"
#include "../app_context.h"

static void EffectLifecycleSystem(flecs::entity e, AttackRayEffectComponent& effect)
{
	Context* ctx = (Context*)e.world().get_ctx();
	effect.lifetime -= ctx->FIXED_TIMESTEP;
	if (effect.lifetime <= 0.0f) {
		e.destruct();
	}
}

#endif
