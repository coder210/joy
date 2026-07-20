#ifndef INPUT_CONSUME_SYSTEM_H
#define INPUT_CONSUME_SYSTEM_H
#include "../context.h"

static const uint8_t DIR_TO_BITS[4] = { 
        0b1000,  // 索引 0 -> 右
        0b0010,  // 索引 1 -> 下
        0b0100,  // 索引 2 -> 左
        0b0001   // 索引 3 -> 上
};

// 系统实现
static void input_consume_system(entt::registry& registry, Context* ctx, float dt) {
        (void)dt;
        // 从 NetComponent 读取当前 tick 的命令数据
        auto& net = registry.ctx().get<NetComponent>();
        if (net.frame_id < 0) return;

        // ---- 1. 处理 JOIN ----
        for (int i = 0; i < net.num_joins; i++) {
                auto& join = net.joins[i];
                entt::entity player = registry.create();
                registry.emplace<IdComponent>(player, get_entity_id(ctx));
                registry.emplace<PlayerComponent>(player, join.conv);
                registry.emplace<LogicPositionComponent>(player, join.position_x, join.position_y);
                registry.emplace<LogicRectComponent>(player, fp_from_float(1), fp_from_float(1));
                registry.emplace<TransformComponent>(player, fp_to_float(join.position_x), fp_to_float(join.position_y), 0.0f, 1.0f, 1.0f);
                // 挂载网络连接数据
                ConnectionComponent conn;
                conn.conv = join.conv;
                registry.emplace<ConnectionComponent>(player, conn);

                // 挂载动画组件（如果 AnimatorDefComponent 已注册）
                if (registry.ctx().contains<AnimatorDefComponent>()) {
                        registry.emplace<AnimatorComponent>(player);
                        registry.emplace<AnimParamsComponent>(player);
                        registry.emplace<PlayerActionComponent>(player);
                }

                // 关联到 Session
                auto it_sess = ctx->sessions.find(join.conv);
                if (it_sess != ctx->sessions.end()) {
                        it_sess->second.player_entity = player;
                        it_sess->second.is_loaded = true;
                }
        }

        // ---- 2. 处理 LEAVE ----
        for (int i = 0; i < net.num_leaves; i++) {
                auto& leave = net.leaves[i];
                auto it_sess = ctx->sessions.find(leave.conv);
                if (it_sess != ctx->sessions.end()) {
                        if (it_sess->second.player_entity != entt::null) {
                                registry.destroy(it_sess->second.player_entity);
                        }
                        ctx->sessions.erase(it_sess);
                }
        }

        // ---- 3. 处理 INPUT（最关键） ----
        for (int i = 0; i < net.num_inputs; i++) {
                auto& input = net.inputs[i];
                auto it_sess = ctx->sessions.find(input.conv);
                if (it_sess == ctx->sessions.end() || it_sess->second.player_entity == entt::null)
                        continue;

                entt::entity player = it_sess->second.player_entity;
                auto& conn = registry.get_or_emplace<ConnectionComponent>(player);

                // 序列号去重
                if (input.client_seq <= conn.last_processed_seq) {
                        //continue;
                }
                conn.last_processed_seq = input.client_seq;

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

                // 更新动画参数：将原始方向位掩码翻译为 anim_params_t.dir / speed / attack
                if (registry.all_of<anim_params_t, PlayerActionComponent>(player)) {
                        auto& params = registry.get<anim_params_t>(player);
                        auto& act    = registry.get<PlayerActionComponent>(player);

                        int  dir_idx = input_to_dir(input.dir);
                        bool moving  = has_dir(input.dir);

                        if (input.attack) {
                                act.state     = PlayerActionComponent::ATTACK;
                                params.attack = true;
                                params.dir    = dir_idx;
                        } else {
                                act.state  = moving ? PlayerActionComponent::WALK
                                                    : PlayerActionComponent::IDLE;
                                params.speed = moving ? 1.0f : 0.0f;
                                params.dir   = dir_idx;
                        }
                }
        }

        net.num_joins = 0;
        net.num_leaves = 0;
        net.num_inputs = 0;
}

#endif
