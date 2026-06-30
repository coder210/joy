#ifndef INPUT_CONSUME_SYSTEM_H
#define INPUT_CONSUME_SYSTEM_H
#include "../flecs.h"
#include "../context.h"

static const uint8_t DIR_TO_BITS[4] = { 0b0001, 0b0010, 0b0100, 0b1000 };

// 系统实现
static void input_consume_system(flecs::iter& it) {
    Context* ctx = (Context*)it.world().get_ctx();

    // ---- 1. 处理 JOIN ----
    for (auto& join : ctx->player_joins) {
        flecs::entity player = ctx->ecs.entity();
        player.set<PlayerComponent>({ join.conv });
        player.set<LogicPositionComponent>({ join.position_x, join.position_y });
        player.set<LogicRectComponent>({ 32, 32 }); // 根据需要设置
        player.set<TransformComponent>({ fp_to_float(join.position_x), fp_to_float(join.position_y) });
        // 添加一个“初始同步”标记，让 ReplicationSystem 知道要发全量数据
        player.add<InitialSyncTag>();

        // 关联到 Session
        auto it_sess = ctx->sessions.find(join.conv);
        if (it_sess != ctx->sessions.end()) {
            it_sess->second.player_entity = player;
            it_sess->second.is_loaded = true;
        }
    }
    ctx->player_joins.clear();

    // ---- 2. 处理 LEAVE ----
    for (auto& leave : ctx->player_leaves) {
        auto it_sess = ctx->sessions.find(leave.conv);
        if (it_sess != ctx->sessions.end()) {
            if (it_sess->second.player_entity) {
                it_sess->second.player_entity.destruct();
            }
            ctx->sessions.erase(it_sess);
        }
    }
    ctx->player_leaves.clear();

    // ---- 3. 处理 INPUT（最关键） ----
    // 由于 KCP 已保证有序，无需 sort，直接遍历即可
    for (auto& input : ctx->player_inputs) {
        auto it_sess = ctx->sessions.find(input.conv);
        if (it_sess == ctx->sessions.end() || !it_sess->second.player_entity) continue;

        auto& sess = it_sess->second;
        // 【去重】丢弃已处理的序列号
        if (input.client_seq <= sess.last_processed_seq) {
            continue;
        }
        sess.last_processed_seq = input.client_seq;

        flecs::entity player = sess.player_entity;

        // 状态类：移动（覆盖）
        auto* state = player.get_mut<PlayerStateInput>();
        if (!state) {
            player.set<PlayerStateInput>({ 0, 0, 0 });
            state = player.get_mut<PlayerStateInput>();
        }
        state->move_bits = DIR_TO_BITS[input.dir % 4]; // 转换

        // 事件类：攻击（追加）
        if (input.attack) {
            auto* events = player.get_mut<PlayerEventInput>();
            if (!events) {
                player.set<PlayerEventInput>({});
                events = player.get_mut<PlayerEventInput>();
            }
            events->pending_actions.push_back({ input.client_seq, 0 });
        }
    }
    ctx->player_inputs.clear();
}




#endif
