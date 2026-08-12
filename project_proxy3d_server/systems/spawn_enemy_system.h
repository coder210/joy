#ifndef SPAWN_ENEMY_SYSTEM_H
#define SPAWN_ENEMY_SYSTEM_H

#include "../context.h"
#include "../../project_proxy3d_shared/components/enemy_component.h"
#include "../../project_proxy3d_shared/components/ai_component.h"
#include "../../project_proxy3d_shared/components/logic_position_component.h"
#include "../../project_proxy3d_shared/components/logic_rect_component.h"
#include "../../project_proxy3d_shared/components/transform_component.h"
#include "../../project_proxy3d_shared/components/action_state_component.h"
#include "../../project_proxy3d_shared/components/mesh_component.h"
#include "../../project_proxy3d_shared/systems/spawn_def.h"

// 在 OnStart 阶段运行，创建初始敌人实体
static void spawn_enemy_system(entt::registry& registry, Context* ctx, float /*dt*/) {

    for (int i = 0; i < ENEMY_SPAWN_COUNT; i++) {
        auto& s = ENEMY_SPAWNS[i];
        entt::entity e = registry.create();
        registry.emplace<IdComponent>(e, get_entity_id(ctx));
        registry.emplace<EnemyComponent>(e);
        registry.emplace<MeshComponent>(e, ctx->mesh_enemy, jcolor_t{ 255, 80, 80, 255 });
        registry.emplace<SpawnIdxComponent>(e, i);
        registry.emplace<LogicPositionComponent>(e, fp_from_float(s.x), fp_zero(), fp_from_float(s.z));
        registry.emplace<LogicRectComponent>(e, fp_from_float(1), fp_from_float(1));
        registry.emplace<ColliderComponent>(e);
        registry.emplace<TransformComponent>(e,
            vec3_t{ s.x, 0.0f, s.z },
            0.0f,
            vec3_t{ 1.0f, 1.0f, 1.0f });

        registry.emplace<ActionStateComponent>(e);
        registry.emplace<AIComponent>(e,
            fp_from_float(s.x), fp_from_float(s.z),  // patrol_center
            fp_from_float(s.patrol_radius),
            fp_from_float(s.detect_radius),
            fp_from_float(s.attack_radius)
        );
    }
}

#endif