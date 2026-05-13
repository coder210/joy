#ifndef RENDERER_ATTACK_RAY_EFFECT_SYSTEM_H
#define RENDERER_ATTACK_RAY_EFFECT_SYSTEM_H
#include "../flecs.h"
#include "../components/attack_ray_effect_component.h"

void RendererAttackRayEffectSystem(flecs::entity e, AttackRayEffectComponent& effect);

#endif
