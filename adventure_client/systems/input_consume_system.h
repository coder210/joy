#ifndef INPUT_CONSUME_SYSTEM_H
#define INPUT_CONSUME_SYSTEM_H
#include "../context.h"

static const uint8_t DIR_TO_BITS[4] = { 0b0001, 0b0010, 0b0100, 0b1000 };

static void input_consume_system(entt::registry& registry, Context* ctx, float /*dt*/) {
    auto& net = registry.get<NetComponent>(ctx->singleton_net);
    if (net.frame_id < 0) return;

    // ---- 1. 处理 JOIN ----
    for (int i = 0; i < net.num_joins; i++) {
        auto& join = net.joins[i];
        entt::entity player = registry.create();
        registry.emplace<IdComponent>(player, get_entity_id(ctx));
        registry.emplace<PlayerComponent>(player, join.conv);
        registry.emplace<LogicPositionComponent>(player, join.position_x, join.position_y);
        registry.emplace<LogicRectComponent>(player, fp_from_float(1), fp_from_float(1));
        registry.emplace<TransformComponent>(player, fp_to_float(join.position_x), fp_to_float(join.position_y), 0.0f, 0.0f);

        auto it_sess = ctx->sessions.find(join.conv);
        if (it_sess == ctx->sessions.end()) {
                ctx->sessions.insert({ join.conv, player });
        }
    }

    // ---- 2. 处理 LEAVE ----
    for (int i = 0; i < net.num_leaves; i++) {
        auto& leave = net.leaves[i];
        auto it_sess = ctx->sessions.find(leave.conv);
        if (it_sess != ctx->sessions.end()) {
            if (it_sess->second != entt::null) {
                registry.destroy(it_sess->second);
            }
            ctx->sessions.erase(it_sess);
        }
    }

    // ---- 3. 处理 INPUT ----
    for (int i = 0; i < net.num_inputs; i++) {
        auto& input = net.inputs[i];
        auto it_sess = ctx->sessions.find(input.conv);
        if (it_sess == ctx->sessions.end() || it_sess->second == entt::null)
            continue;

        entt::entity player = it_sess->second;

        PlayerStateInput state_input;
        state_input.move_bits = input.dir;

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
    }

    net.num_joins = 0;
    net.num_leaves = 0;
    net.num_inputs = 0;
}

#endif
