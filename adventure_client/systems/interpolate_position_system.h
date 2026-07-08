#ifndef INTERPOLATE_POSITION_SYSTEM_H
#define INTERPOLATE_POSITION_SYSTEM_H
#include "../flecs.h"
#include "../context.h"
#include "../components/logic_position_component.h"
#include "../components/transform_component.h"

static void interpolate_position_system(flecs::entity e,
        LogicPositionComponent& lp, TransformComponent& t)
{
        context* ctx = (context*)e.world().get_ctx();
        float target_position_x = fp_to_float(lp.x);
        float target_position_y = fp_to_float(lp.y);
        float smooth_time = 0.1f;
        float alpha = e.world().delta_time() * 10;
        t.position.x = ft_lerp(t.position.x, target_position_x, alpha);
        t.position.y = ft_lerp(t.position.y, target_position_y, alpha);
}

#endif