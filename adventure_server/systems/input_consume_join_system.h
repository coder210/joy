#ifndef INPUT_CONSUME_JOIN_SYSTEM_H
#define INPUT_CONSUME_JOIN_SYSTEM_H
#include "../context.h"

static void input_consume_join_system(entt::registry& registry, Context* ctx, float dt) {
        (void)dt;
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
        net.num_joins = 0;
}

#endif
