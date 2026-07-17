#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H
#include "../context.h"
#include <joy/jmath.h>

static const fp_t MOVE_SPEED = fp_from_float(5.0f);

static void movement_system(Context* ctx, entt::registry& registry, entt::entity e, PlayerStateInput& psi, LogicPositionComponent& p)
{
        if (psi.move_bits == 0) { registry.remove<PlayerStateInput>(e); return; }

        fp_t step = fp_mul(MOVE_SPEED, fp_from_float(ctx->FIXED_TIMESTEP));
        if (psi.move_bits & 0b0001)
                p.y = fp_sub(p.y, step);
        if (psi.move_bits & 0b0010) 
                p.y = fp_add(p.y, step);
        if (psi.move_bits & 0b0100) 
                p.x = fp_sub(p.x, step);
        if (psi.move_bits & 0b1000) 
                p.x = fp_add(p.x, step);

        int id = registry.get<IdComponent>(e).id;
        log_info("id:%d, x=%f, y=%f", id, fp_to_float(p.x), fp_to_float(p.y));

        registry.remove<PlayerStateInput>(e);
}

#endif
