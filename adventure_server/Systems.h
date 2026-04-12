#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "Component.h"
#include "flecs.h"

void StartupSystem(flecs::world& world);
void LerpSystem(flecs::entity entity,
        LogicPositionComponent& lp, TransformComponent& t);
void EffectLifecycleSystem(flecs::entity e, AttackRayEffectComponent& effect);

void RendererSystem(flecs::entity entity,
        IdComponent& id, LogicRectComponent& rect, TransformComponent& t);
void RendererAttackRayEffectSystem(flecs::entity e, AttackRayEffectComponent& effect);

#endif