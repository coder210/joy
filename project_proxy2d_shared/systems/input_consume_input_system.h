#ifndef INPUT_CONSUME_INPUT_SYSTEM_H
#define INPUT_CONSUME_INPUT_SYSTEM_H

#include "../components/action_state_component.h"
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

		PlayerStateInput state_input;
		state_input.move_bits = input.dir | (input.ranged ? INPUT_RANGED : 0);

		log_info("Raw dir: %d\n", input.dir);

		if (registry.all_of<PlayerStateInput>(player)) {
			auto& state = registry.get<PlayerStateInput>(player);
			state.move_bits = state_input.move_bits;
		}
		else {
			registry.emplace<PlayerStateInput>(player, state_input);
		}

		if (input.attack) {
			if (registry.all_of<PlayerEventInput>(player)) {
				auto& events = registry.get<PlayerEventInput>(player);
				events.pending_actions.push_back({ input.client_seq, (uint8_t)input.attack });
			}
			else {
				PlayerEventInput events;
				events.pending_actions.push_back({ input.client_seq, (uint8_t)input.attack });
				registry.emplace<PlayerEventInput>(player, events);
			}
		}

		// 更新动作状态：attack_timer 驱动攻击持续（替代动画状态机复位），
		// 其余由移动位掩码决定 WALK/IDLE 与朝向
		if (registry.all_of<ActionStateComponent>(player)) {
			auto& act = registry.get<ActionStateComponent>(player);
			if (act.attack_timer > 0.0f) act.attack_timer -= ctx->FIXED_TIMESTEP;

			int  dir_idx = input_to_dir(input.dir);
			bool moving  = has_dir(input.dir);

			if (input.attack) {
				act.state        = ActionStateComponent::ATTACK;
				act.attack_timer = ActionStateComponent::ATTACK_DURATION;
				if (moving) act.dir = dir_idx;
			} else {
				act.state = act.attack_timer > 0.0f ? ActionStateComponent::ATTACK
				                                    : (moving ? ActionStateComponent::WALK
				                                              : ActionStateComponent::IDLE);
				if (moving) act.dir = dir_idx;
			}
		}
	}
	net.num_inputs = 0;
}

#endif
