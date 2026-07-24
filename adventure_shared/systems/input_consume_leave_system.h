#ifndef INPUT_CONSUME_LEAVE_SYSTEM_H
#define INPUT_CONSUME_LEAVE_SYSTEM_H
#include "../adventure_server/context.h"

static void input_consume_leave_system(entt::registry& registry, Context* ctx, float dt) {
        (void)dt;
        auto& net = registry.ctx().get<NetComponent>();
        if (net.frame_id < 0) return;

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
        net.num_leaves = 0;
}

#endif
