#ifndef LERP_SYSTEM_H
#define LERP_SYSTEM_H
#include <algorithm>
#include "../flecs.h"
#include "../components/logic_position_component.h"
#include "../components/transform_component.h"
#include "../context.h"

static void LerpSystem(flecs::entity e,
	LogicPositionComponent& lp, TransformComponent& t)
{
	Context* ctx = (Context*)e.world().get_ctx();
	vec2_t target = {
		fp_to_float(lp.x),
		fp_to_float(lp.y)
	};
	float alpha = e.world().delta_time() * 10;
	alpha = std::clamp(alpha, 0.0f, 1.0f);
	t.position.x = ft_lerp(t.position.x, target.x, alpha);
	t.position.y = ft_lerp(t.position.y, target.y, alpha);
}

#endif
