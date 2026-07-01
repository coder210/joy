#ifndef INPUT_FORWARDING_SYSTEM_H
#define INPUT_FORWARDING_SYSTEM_H
#include "../flecs.h"
#include "../context.h"

static void input_forwarding_system(flecs::iter& it) {
    Context* ctx = (Context*)it.world().get_ctx();

    // 1. 转发 JOIN 指令
    for (auto& join : ctx->player_joins) {
        for (auto& kv : ctx->sessions) {
            if (kv.first == join.conv) continue; // 不发给自己
            auto data = pack_s2c_player_join(join); // 序列化
            ctx->send_queue.push_back({ kv.first, data });
        }
    }

    // 2. 转发 LEAVE 指令
    for (auto& leave : ctx->player_leaves) {
        for (auto& kv : ctx->sessions) {
            if (kv.first == leave.conv) continue;
            auto data = pack_s2c_player_leave(leave);
            ctx->send_queue.push_back({ kv.first, data });
        }
    }

    // 3. 转发 INPUT 指令（批量打包，提高效率）
    if (!ctx->player_inputs.empty()) {
        // 按目标 conv 分组
        std::unordered_map<int, std::vector<S2CPlayerInput>> grouped;
        for (auto& input : ctx->player_inputs) {
            for (auto& kv : ctx->sessions) {
                if (kv.first == input.conv) continue;
                grouped[kv.first].push_back(input);
            }
        }
        // 批量发送
        for (auto& kv : grouped) {
            auto batch = pack_batch_input(kv.second); // 需要实现批量打包
            ctx->send_queue.push_back({ kv.first, batch });
        }
    }
}


#endif
