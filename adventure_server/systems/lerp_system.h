#ifndef LERP_SYSTEM_H
#define LERP_SYSTEM_H
#include "../flecs.h"
#include "../components/logic_position_component.h"
#include "../components/transform_component.h"

void LerpSystem(flecs::entity entity,
	LogicPositionComponent& lp, TransformComponent& t);

#endif
