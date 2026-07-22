#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H
#include "../context.h"
#include <joy/jmath.h>

static const fp_t MOVE_SPEED = fp_from_float(5.0f);

static void movement_system(entt::registry& registry, Context* ctx, float /*dt*/)
{
        auto view = registry.view<PlayerStateInput, LogicPositionComponent>();
        fp_t step = fp_mul(MOVE_SPEED, fp_from_float(ctx->FIXED_TIMESTEP));

        // 收集要移除的 entity（不能在遍历中移除）
        std::vector<entt::entity> to_remove;
        for (auto e : view) {
                auto& psi = view.get<PlayerStateInput>(e);
                auto& p = view.get<LogicPositionComponent>(e);

                if (psi.move_bits == 0) {
                        to_remove.push_back(e);
                        continue;
                }

                if (psi.move_bits & 0b0001)
                        p.y = fp_sub(p.y, step);
                if (psi.move_bits & 0b0010)
                        p.y = fp_add(p.y, step);
                if (psi.move_bits & 0b0100)
                        p.x = fp_sub(p.x, step);
                if (psi.move_bits & 0b1000)
                        p.x = fp_add(p.x, step);

                int id = registry.get<IdComponent>(e).id;
                log_info("id:%d, x=%f, y=%f", id, fp_to_float(p.x), fp_to_float(p.y));

                to_remove.push_back(e);
        }

        for (auto e : to_remove) {
                registry.remove<PlayerStateInput>(e);
        }
}

#endif
