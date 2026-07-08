#ifndef INPUT_CONSUME_SYSTEM_H
#define INPUT_CONSUME_SYSTEM_H
#include "../flecs.h"
#include "../context.h"

static const uint8_t DIR_TO_BITS[4] = { 0b0001, 0b0010, 0b0100, 0b1000 };

// 系统实现
static void input_consume_system(flecs::iter& it) {
    Context* ctx = (Context*)it.world().get_ctx();

    // 从 NetComponent 读取当前 tick 的命令数据
    auto* net = it.world().get_mut<NetComponent>();
    if (!net || net->frame_id < 0) return;

    

    // ---- 1. 处理 JOIN ----
    for (int i = 0; i < net->num_joins; i++) {
        auto& join = net->joins[i];
        flecs::entity player = ctx->ecs.entity();
        player.set<IdComponent>({ get_entity_id(ctx) });
        player.set<PlayerComponent>({ join.conv });
        player.set<LogicPositionComponent>({ join.position_x, join.position_y });
        player.set<LogicRectComponent>({ fp_from_float(1), fp_from_float(1) });
        player.set<TransformComponent>({ fp_to_float(join.position_x), fp_to_float(join.position_y) });

        // 关联到 Session
        auto it_sess = ctx->sessions.find(join.conv);
        if (it_sess != ctx->sessions.end()) {
            it_sess->second.player_entity = player;
            it_sess->second.is_loaded = true;
        }
    }

    // ---- 2. 处理 LEAVE ----
    for (int i = 0; i < net->num_leaves; i++) {
        auto& leave = net->leaves[i];
        auto it_sess = ctx->sessions.find(leave.conv);
        if (it_sess != ctx->sessions.end()) {
            if (it_sess->second.player_entity) {
                it_sess->second.player_entity.destruct();
            }
            ctx->sessions.erase(it_sess);
        }
    }

    // ---- 3. 处理 INPUT（最关键） ----
    for (int i = 0; i < net->num_inputs; i++) {
        auto& input = net->inputs[i];
        auto it_sess = ctx->sessions.find(input.conv);
        if (it_sess == ctx->sessions.end() || !it_sess->second.player_entity) 
            continue;

        auto& sess = it_sess->second;
        if (input.client_seq <= sess.last_processed_seq) {
            //continue;
        }
        sess.last_processed_seq = input.client_seq;

        flecs::entity player = sess.player_entity;

        auto* state = player.get_mut<PlayerStateInput>();
        if (!state) {
            player.set<PlayerStateInput>({ 0, 0, 0 });
            state = player.get_mut<PlayerStateInput>();
        }
        state->move_bits = DIR_TO_BITS[input.dir % 4];

        if (input.attack) {
            auto* events = player.get_mut<PlayerEventInput>();
            if (!events) {
                player.set<PlayerEventInput>({});
                events = player.get_mut<PlayerEventInput>();
            }
            events->pending_actions.push_back({ input.client_seq, (uint8_t)input.attack });
        }
    }

    net->num_joins = 0;
    net->num_leaves = 0;
    net->num_inputs = 0;
}




#endif
