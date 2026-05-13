#ifndef LERP_SYSTEM_H
#define LERP_SYSTEM_H
#include "../flecs.h"
#include "../components/logic_position_component.h"
#include "../components/transform_component.h"
#include "../app_context.h"

static void LerpSystem(flecs::entity e,
	LogicPositionComponent& lp, TransformComponent& t)
{
	Context* ctx = (Context*)e.world().get_ctx();
	float target_position_x = fp_to_float(lp.x);
	float target_position_y = fp_to_float(lp.y);
	float alpha = e.world().delta_time() * 10;
	alpha = std::clamp(alpha, 0.0f, 1.0f);
	t.position_x = ft_lerp(t.position_x, target_position_x, alpha);
	t.position_y = ft_lerp(t.position_y, target_position_y, alpha);
}

#endif
