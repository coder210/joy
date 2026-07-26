#ifndef INPUT_CONSUME_JOIN_SYSTEM_H
#define INPUT_CONSUME_JOIN_SYSTEM_H

struct Context;

static void input_consume_join_system(entt::registry& registry, Context* ctx, float /*dt*/) {
	auto& net = registry.ctx().get<NetComponent>();
	if (net.frame_id < 0) return;

	for (int i = 0; i < net.num_joins; i++) {
		auto& join = net.joins[i];
		entt::entity player = registry.create();
		registry.emplace<IdComponent>(player, get_entity_id(ctx));
		registry.emplace<PlayerComponent>(player, join.conv);
		registry.emplace<LogicPositionComponent>(player, join.position_x, join.position_y);
		registry.emplace<LogicRectComponent>(player, fp_from_float(1), fp_from_float(1));
		registry.emplace<TransformComponent>(player, fp_to_float(join.position_x), fp_to_float(join.position_y), 0.0f, 1.0f, 1.0f);

		// 挂载动画组件（如果 AnimatorDefComponent 已注册）
		if (registry.ctx().contains<AnimatorDefComponent>()) {
			registry.emplace<AnimatorComponent>(player);
			registry.emplace<AnimParamsComponent>(player);
			registry.emplace<PlayerActionComponent>(player);
		}
	}
	net.num_joins = 0;
}

#endif
