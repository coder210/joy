#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H
#include "../components/player_state_input.h"
#include "../components/logic_position_component.h"
#include "../components/action_state_component.h"
#include "../components/animator_component.h"
#include <joy/jmath.h>
#include <cmath>

struct Context;

static const fp_t MOVE_SPEED = fp_from_float(5.0f);

static void movement_system(entt::registry& registry, Context* ctx, float /*dt*/) {
	auto view = registry.view<PlayerStateInput, LogicPositionComponent, ActionStateComponent>();
	fp_t step = fp_mul(MOVE_SPEED, fp_from_float(ctx->FIXED_TIMESTEP));
	for (auto e : view) {
		auto& psi = view.get<PlayerStateInput>(e);
		auto& p   = view.get<LogicPositionComponent>(e);
		auto& act = view.get<ActionStateComponent>(e);
		if (psi.move_bits == 0) { continue; }

		// 俯视角：XZ 平面移动（W=-Z, S=+Z, A=-X, D=+X）
		fp_t dx = fp_zero();
		fp_t dz = fp_zero();
		if (psi.move_bits & INPUT_UP)    dz = fp_sub(dz, step);
		if (psi.move_bits & INPUT_DOWN)  dz = fp_add(dz, step);
		if (psi.move_bits & INPUT_LEFT)  dx = fp_sub(dx, step);
		if (psi.move_bits & INPUT_RIGHT) dx = fp_add(dx, step);

		p.x = fp_add(p.x, dx);
		p.z = fp_add(p.z, dz);

		// 朝向来自鼠标（量化角度 → 弧度）
		act.yaw = (float)psi.yaw_quantized / 32767.0f * 3.14159265f;

		if (registry.all_of<AnimParamsComponent>(e))
			registry.get<AnimParamsComponent>(e).speed = 1.0f;
	}
}

#endif
