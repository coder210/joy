#ifndef EFFECT_LIFECYCLE_SYSTEM_H
#define EFFECT_LIFECYCLE_SYSTEM_H
#include "../flecs.h"
#include "../components/attack_ray_effect_component.h"

void EffectLifecycleSystem(flecs::entity e, AttackRayEffectComponent& effect);

#endif
