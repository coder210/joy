#ifndef INPUT_CONSUME_JOIN_SYSTEM_H
#define INPUT_CONSUME_JOIN_SYSTEM_H

#include "../components/global_state_component.h"
#include "../components/collider_component.h"
#include "../components/player_physics_component.h"
#include "../components/player_state_input.h"

struct Context;

static void input_consume_join_system(entt::registry& registry, Context* ctx, float /*dt*/) {
	auto& net = registry.ctx().get<NetComponent>();
	if (net.frame_id < 0) return;

	auto& gs = registry.ctx().get<GlobalState>();

	for (int i = 0; i < net.num_joins; i++) {
		log_info("creating player %d", net.num_joins);
		auto& join = net.joins[i];
		entt::entity player = registry.create();
		registry.emplace<IdComponent>(player, get_entity_id(ctx));
		registry.emplace<PlayerComponent>(player, join.conv);
		registry.emplace<LogicPositionComponent>(player, join.position_x, join.position_y);
		registry.emplace<LogicRectComponent>(player, fp_from_float(1), fp_from_float(1));
		registry.emplace<TransformComponent>(player, fp_to_float(join.position_x), fp_to_float(join.position_y), 0.0f, 1.0f, 1.0f);

		// 玩家输入/碰撞/物理组件（无动画时也可移动/跳跃）
		registry.emplace<PlayerStateInput>(player);
		registry.emplace<ColliderComponent>(player);
		registry.emplace<PlayerPhysicsComponent>(player);

		// 挂载状态组件（承载 yaw/朝向数据，3D 相机跟随需要）
		registry.emplace<ActionStateComponent>(player);

		// 首次加入的玩家作为摄像机跟随目标
		if (gs.camera_follow_conv < 0) {
			gs.camera_follow_conv = join.conv;
		}
	}
	net.num_joins = 0;
}

#endif
