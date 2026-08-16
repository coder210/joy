#ifndef INPUT_CONSUME_JOIN_SYSTEM_H
#define INPUT_CONSUME_JOIN_SYSTEM_H

#include "../components/global_state_component.h"
#include "../components/action_state_component.h"
#include "../components/health_component.h"
#include "../components/spawn_point_component.h"

struct Context;

static void input_consume_join_system(entt::registry& registry, Context* ctx, float /*dt*/) {
	auto& net = registry.ctx().get<NetComponent>();
	if (net.frame_id < 0) return;

	auto& gs = registry.ctx().get<GlobalState>();

	for (int i = 0; i < net.num_joins; i++) {
		log_info("creating player %d", net.num_joins);
		auto& join = net.joins[i];

		// 该 conv 玩家可能已由加载快照创建（client 端）：不重复创建，仅刷新出生点
		bool exists = false;
		auto pview = registry.view<PlayerComponent>();
		for (auto pe : pview) {
			if (registry.get<PlayerComponent>(pe).conv == join.conv) {
				// 已有实体：仅刷新出生点（重连/重新加入场景）
				if (registry.all_of<HealthComponent>(pe)) {
					auto& sp = registry.get<SpawnPointComponent>(pe);
					sp.x = join.position_x;
					sp.y = join.position_y;
				}
				exists = true;
				break;
			}
		}
		if (exists) continue;

		entt::entity player = registry.create();
		registry.emplace<IdComponent>(player, get_entity_id(ctx));
		registry.emplace<PlayerComponent>(player, join.conv);
		registry.emplace<LogicPositionComponent>(player, join.position_x, join.position_y);
		registry.emplace<LogicRectComponent>(player, fp_from_float(0.8f), fp_from_float(0.8f));
		// 碰撞盒为 0.8x0.8（原 1x1 的 80%），position 取碰撞盒中心（渲染以中心为原点）
		registry.emplace<TransformComponent>(player, fp_to_float(join.position_x) + 0.4f, fp_to_float(join.position_y) + 0.4f, 0.0f, 1.0f, 1.0f);

		// 挂载动作状态组件（几何渲染的朝向 / 攻击表现依赖它）
		registry.emplace<ActionStateComponent>(player);

		// 挂载血量组件（远程敌人子弹命中扣血）
		registry.emplace<HealthComponent>(player, PLAYER_MAX_HP, PLAYER_MAX_HP);
		// 出生点 = join 位置（重生传送目标）
		registry.emplace<SpawnPointComponent>(player, join.position_x, join.position_y);

		// 首次加入的玩家作为摄像机跟随目标
		if (gs.camera_follow_conv < 0) {
			gs.camera_follow_conv = join.conv;
		}
	}
	net.num_joins = 0;
}

#endif
