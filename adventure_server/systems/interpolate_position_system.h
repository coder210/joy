#ifndef INTERPOLATE_POSITION_SYSTEM_H
#define INTERPOLATE_POSITION_SYSTEM_H
#include "../context.h"
#include "../components/logic_position_component.h"
#include "../components/transform_component.h"

static void interpolate_position_system(Context* ctx,
    IdComponent& id,
    LogicPositionComponent& lp, TransformComponent& t, float dt)
{
    float target_position_x = fp_to_float(lp.x);
    float target_position_y = fp_to_float(lp.y);
    float smooth_time = 0.1f;
    float alpha = dt * 10;
    t.position.x = ft_lerp(t.position.x, target_position_x, alpha);
    t.position.y = ft_lerp(t.position.y, target_position_y, alpha);
}

#endif
