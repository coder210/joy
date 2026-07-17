#ifndef CREATING_BLOCK_SYSTEM_H
#define CREATING_BLOCK_SYSTEM_H
#include <joy/joy.h>
#include "../../adventure_shared/protocol.h"
#include "../components/id_component.h"
#include "../components/logic_rect_component.h"
#include "../components/logic_position_component.h"
#include "../components/transform_component.h"
#include "../components/local_only_component.h"
#include "../context.h"

static void creating_block_system(entt::registry& registry, Context* ctx)
{
        log_info("[creating_block] system");
        entt::entity e = registry.create();
        registry.emplace<LocalOnlyComponent>(e);
        registry.emplace<IdComponent>(e, 10000);
        registry.emplace<LogicRectComponent>(e, fp_from_float(0.5f), fp_from_float(0.5f));
        registry.emplace<LogicPositionComponent>(e, fp_from_float(5), fp_from_float(5));
        registry.emplace<TransformComponent>(e, 5.0f, 5.0f, 0.0f, 1.0f, 1.0f);
}

#endif
