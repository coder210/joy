#ifndef CREATING_BLOCK_SYSTEM_H
#define CREATING_BLOCK_SYSTEM_H
#include "../flecs.h"
#include <joy/joy.h>
#include "../../adventure_shared/protocol.h"
#include "../components/id_component.h"
#include "../components/logic_rect_component.h"
#include "../components/logic_position_component.h"
#include "../components/transform_component.h"
#include "../context.h"


static void creating_block_system(flecs::iter& it)
{
        log_info("[creating_block] system: %s", it.system().name());
        Context* ctx = (Context*)it.world().get_ctx();

        ctx->ecs.entity()
               .add<LocalOnlyComponent>()
               .set<IdComponent>({ 10000 })
               .set<LogicRectComponent>({ fp_from_float(0.5f), fp_from_float(0.5f) })
               .set<LogicPositionComponent>({ fp_from_float(5), fp_from_float(5) })
               .set<TransformComponent>({ 5,5,0,1,1 });
}

#endif
