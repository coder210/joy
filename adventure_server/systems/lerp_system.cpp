#include "lerp_system.h"
#include "../Context.h"

void LerpSystem(flecs::entity e,
	LogicPositionComponent& lp, TransformComponent& t)
{
	auto ctx = e.world().get_mut<Context>();
	float target_position_x = fp_to_float(lp.x);
	float target_position_y = fp_to_float(lp.y);
	float alpha = e.world().delta_time() * 10;
	alpha = std::clamp(alpha, 0.0f, 1.0f);
	t.position_x = ft_lerp(t.position_x, target_position_x, alpha);
	t.position_y = ft_lerp(t.position_y, target_position_y, alpha);
}
