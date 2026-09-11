#ifndef INPUT_CONSUME_INPUT_SYSTEM_H
#define INPUT_CONSUME_INPUT_SYSTEM_H

#include "../components/action_state_component.h"
#include "../components/health_component.h"
#include "../protocol.h"

struct Context;

static void input_consume_input_system(entt::registry& registry, Context* ctx, float /*dt*/) {
	auto& net = registry.ctx().get<NetComponent>();
	if (net.frame_id < 0) return;

	auto view = registry.view<PlayerComponent>();
	for (int i = 0; i < net.num_inputs; i++) {
		auto& input = net.inputs[i];

		entt::entity player = entt::null;
		for (auto e : view) {
			if (registry.get<PlayerComponent>(e).conv == input.conv) {
				player = e;
				break;
			}
		}
		if (player == entt::null) continue;

		// 死亡期间不消费输入：尸体不转向、不出招、不留移动位。
		// 帧同步确定性：dead 由两端同一 tick 置位，故此处判定两端一致
		if (registry.all_of<HealthComponent>(player) &&
		    registry.get<HealthComponent>(player).dead) continue;

		PlayerStateInput state_input;
		state_input.move_bits = input.dir;

		// log_info("Raw dir: %d\n", input.dir);

		if (registry.all_of<PlayerStateInput>(player)) {
			auto& state = registry.get<PlayerStateInput>(player);
			state.move_bits = state_input.move_bits;
		}
		else {
			registry.emplace<PlayerStateInput>(player, state_input);
		}

		auto push_action = [&](int type) {
			PlayerEventInput::Action a;
			a.client_seq = input.client_seq;
			a.type       = (uint8_t)type;
			if (registry.all_of<PlayerEventInput>(player)) {
				auto& ev = registry.get<PlayerEventInput>(player);
				ev.pending_actions.push_back(a);
			} else {
				PlayerEventInput ev;
				ev.pending_actions.push_back(a);
				registry.emplace<PlayerEventInput>(player, ev);
			}
		};
		// 攻/射/线：均为"完整点击"离散触发。射改单发（对齐 3D），线为大招 hitscan。
		if (input.attack) push_action(PlayerEventInput::ATTACK_MELEE);
		if (input.ranged) push_action(PlayerEventInput::ATTACK_BULLET);
		if (input.ray)    push_action(PlayerEventInput::ATTACK_RAY);

		// 更新动作状态：attack_timer 驱动攻击持续（替代动画状态机复位），
		// 其余由移动位掩码决定 WALK/IDLE 与朝向
		if (registry.all_of<ActionStateComponent>(player)) {
			auto& act = registry.get<ActionStateComponent>(player);
			if (act.attack_timer > 0) act.attack_timer -= 1;

			int  dir_idx = input_to_dir(input.dir);
			bool moving  = has_dir(input.dir);

			if (input.attack) {
				act.state        = ActionStateComponent::ATTACK;
				act.attack_timer = ActionStateComponent::ATTACK_DURATION;
				if (moving) act.dir = dir_idx;
			} else {
				act.state = act.attack_timer > 0 ? ActionStateComponent::ATTACK
				                                 : (moving ? ActionStateComponent::WALK
				                                           : ActionStateComponent::IDLE);
				if (moving) act.dir = dir_idx;
			}
		}
	}
	net.num_inputs = 0;
}

#endif
