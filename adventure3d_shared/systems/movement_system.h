#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H
#include "../components/player_state_input.h"
#include "../components/logic_position_component.h"
#include "../components/player_physics_component.h"
#include "../components/action_state_component.h"
#include "../protocol.h"
#include <joy/jmath.h>
#include <cmath>

struct Context;

static const fp_t MOVE_SPEED = fp_from_float(5.0f);
static const fp_t GRAVITY    = fp_from_float(9.8f);
static const fp_t JUMP_SPEED = fp_from_float(4.0f);
static const fp_t GROUND_Y   = fp_from_float(0.6f);  // 玩家中心高度（半高 0.6，脚底贴地）
static const fp_t WALL_LIMIT = fp_from_float(9.5f);  // 空气墙边界

static void movement_system(entt::registry& registry, Context* ctx, float /*dt*/) {
	fp_t step = fp_mul(MOVE_SPEED, fp_from_float(ctx->FIXED_TIMESTEP));
	auto view = registry.view<PlayerStateInput, LogicPositionComponent>();
	for (auto e : view) {
		auto& psi = view.get<PlayerStateInput>(e);
		auto& p   = view.get<LogicPositionComponent>(e);

		// 朝向：来自鼠标（量化角度 → 弧度）
		float yaw = (float)psi.yaw_quantized / 32767.0f * 3.14159265f;
		if (registry.all_of<ActionStateComponent>(e))
			registry.get<ActionStateComponent>(e).yaw = yaw;

		// ---- 相对相机方向的 XZ 移动（W=相机前方，D=相机右侧） ----
		float fdx = 0.0f, fdz = 0.0f;
		float sy = sinf(yaw), cy = cosf(yaw);
		if (psi.move_bits & INPUT_UP)    { fdx += sy; fdz += cy; }
		if (psi.move_bits & INPUT_DOWN)  { fdx -= sy; fdz -= cy; }
		if (psi.move_bits & INPUT_LEFT)  { fdx -= cy; fdz += sy; }
		if (psi.move_bits & INPUT_RIGHT) { fdx += cy; fdz -= sy; }
		float len = sqrtf(fdx * fdx + fdz * fdz);
		if (len > 1e-4f) {
			fdx /= len; fdz /= len;
			p.x = fp_add(p.x, fp_from_float(fdx * fp_to_float(step)));
			p.z = fp_add(p.z, fp_from_float(fdz * fp_to_float(step)));
		}

		// ---- 垂直物理（重力/跳跃/落地） ----
		if (registry.all_of<PlayerPhysicsComponent>(e)) {
			auto& phy = registry.get<PlayerPhysicsComponent>(e);
			if (phy.jump_requested && phy.on_ground) {
				phy.vertical_velocity = JUMP_SPEED;
				phy.on_ground = false;
			}
			phy.jump_requested = false;
			phy.vertical_velocity = fp_sub(phy.vertical_velocity,
				fp_mul(GRAVITY, fp_from_float(ctx->FIXED_TIMESTEP)));
			p.y = fp_add(p.y, fp_mul(phy.vertical_velocity, fp_from_float(ctx->FIXED_TIMESTEP)));
			if (fp_to_float(p.y) <= fp_to_float(GROUND_Y)) {
				p.y = GROUND_Y;
				phy.vertical_velocity = fp_zero();
				phy.on_ground = true;
			}
		}

		// ---- 空气墙 ----
		fp_t wall_neg = fp_from_float(-9.5f);
		p.x = fp_clamp(p.x, wall_neg, WALL_LIMIT);
		p.z = fp_clamp(p.z, wall_neg, WALL_LIMIT);
	}
}

#endif
