#ifndef INTERPOLATE_POSITION_SYSTEM_H
#define INTERPOLATE_POSITION_SYSTEM_H
#include "../context.h"
#include "../../adventure_shared/components/logic_position_component.h"
#include "../../adventure_shared/components/transform_component.h"

static void interpolate_position_system(entt::registry& registry, Context* /*ctx*/, float dt)
{
        auto view = registry.view<IdComponent, LogicPositionComponent, TransformComponent>();
        for (auto e : view) {
                auto& lp = view.get<LogicPositionComponent>(e);
                auto& t = view.get<TransformComponent>(e);

                float target_position_x = fp_to_float(lp.x);
                float target_position_y = fp_to_float(lp.y);
                float alpha = dt * 10;
                t.position.x = ft_lerp(t.position.x, target_position_x, alpha);
                t.position.y = ft_lerp(t.position.y, target_position_y, alpha);
        }
}

#endif
