#include "effect_lifecycle_system.h"
#include "../Context.h"

void EffectLifecycleSystem(flecs::entity e, AttackRayEffectComponent& effect)
{
	auto ctx = e.world().get_mut<Context>();
	effect.lifetime -= ctx->FIXED_TIMESTEP;
	if (effect.lifetime <= 0.0f) {
		e.destruct();
	}
}
