#ifndef SPAWN_ENEMY_SYSTEM_H
#define SPAWN_ENEMY_SYSTEM_H

#include "../context.h"
#include "../../project_proxy2d_shared/components/enemy_component.h"
#include "../../project_proxy2d_shared/components/ai_component.h"
#include "../../project_proxy2d_shared/components/logic_position_component.h"
#include "../../project_proxy2d_shared/components/logic_rect_component.h"
#include "../../project_proxy2d_shared/components/transform_component.h"
#include "../../project_proxy2d_shared/components/action_state_component.h"
#include "../../project_proxy2d_shared/components/animator_component.h"
#include "../../project_proxy2d_shared/systems/spawn_def.h"

// 在 OnStart 阶段运行，创建初始敌人实体
static void spawn_enemy_system(entt::registry& registry, Context* ctx, float /*dt*/) {
    (void)ctx;

    for (int i = 0; i < ENEMY_SPAWN_COUNT; i++) {
        auto& s = ENEMY_SPAWNS[i];
        entt::entity e = registry.create();
        registry.emplace<IdComponent>(e, get_entity_id(ctx));
        registry.emplace<EnemyComponent>(e);
        registry.emplace<SpawnIdxComponent>(e, i);
        registry.emplace<LogicPositionComponent>(e, fp_from_float(s.x), fp_from_float(s.y));
        registry.emplace<LogicRectComponent>(e, fp_from_float(1), fp_from_float(1));
        // 碰撞盒为 1x1，position 取碰撞盒中心（渲染以中心为原点）
        registry.emplace<TransformComponent>(e, s.x + 0.5f, s.y + 0.5f, 0.0f, 1.0f, 1.0f);

        registry.emplace<ActionStateComponent>(e);
        registry.emplace<AIComponent>(e,
            fp_from_float(s.x), fp_from_float(s.y),  // patrol_center
            fp_from_float(s.patrol_radius),
            fp_from_float(s.detect_radius),
            fp_from_float(s.attack_radius)
        );

        // 挂载动画组件（如果 TorchBlueAnimDefComponent 已注册）
        if (registry.ctx().contains<TorchBlueAnimDefComponent>()) {
            registry.emplace<AnimatorComponent>(e);
            registry.emplace<AnimParamsComponent>(e);
        }
    }
}

#endif