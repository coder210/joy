#ifndef ANIMATOR_UPDATE_SYSTEM_H
#define ANIMATOR_UPDATE_SYSTEM_H
#include "../context.h"
#include <joy/anim.h>

static void animator_update_system(entt::registry& registry, Context* ctx, float dt) {
    (void)ctx;

    if (!registry.ctx().contains<anim_def_t>()) return;
    auto& def = registry.ctx().get<anim_def_t>();

    auto view = registry.view<anim_t, anim_params_t, PlayerActionComponent>();
    for (auto entity : view) {
        auto& a = view.get<anim_t>(entity);
        auto& params = view.get<anim_params_t>(entity);
        auto& act = view.get<PlayerActionComponent>(entity);

        // 这帧没有收到输入 → 空闲
        if (!registry.all_of<PlayerStateInput>(entity))
            params.speed = 0.0f;

        // 方向同步
        a.flip_h = (params.dir == 1);

        // 核心状态机更新（无 entt 依赖）
        anim_update(&a, &def, &params, dt);

        // 攻击是一次性触发，消费后立即复位
        // 让攻击动画自然播放 0.9s 后通过 s_atk[d] → idle 过渡回到空闲
        params.attack = false;

        // === ECS 专有逻辑：同步 act.state ===
        if (act.state == PlayerActionComponent::ATTACK) {
            if (strncmp(def.states[a.current_state].name, "attack", 6) != 0) {
                act.state = PlayerActionComponent::IDLE;
            }
        }
    }
}

#endif
