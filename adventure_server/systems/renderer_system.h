#ifndef RENDERER_SYSTEM_H
#define RENDERER_SYSTEM_H
#include "../flecs.h"
#include "../components/id_component.h"
#include "../components/logic_rect_component.h"
#include "../components/transform_component.h"

void RendererSystem(flecs::entity entity,
	IdComponent& id, LogicRectComponent& rect, TransformComponent& t);

#endif
