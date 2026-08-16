#ifndef SPAWN_ENEMY_SYSTEM_H
#define SPAWN_ENEMY_SYSTEM_H

#include "../context.h"
#include "../../project_proxy2d_shared/components/enemy_component.h"
#include "../../project_proxy2d_shared/components/ai_component.h"
#include "../../project_proxy2d_shared/components/logic_position_component.h"
#include "../../project_proxy2d_shared/components/logic_rect_component.h"
#include "../../project_proxy2d_shared/components/transform_component.h"
#include "../../project_proxy2d_shared/components/action_state_component.h"
#include "../../project_proxy2d_shared/components/health_component.h"
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
        registry.emplace<LogicRectComponent>(e, fp_from_float(0.8f), fp_from_float(0.8f));
        // 碰撞盒为 0.8x0.8（原 1x1 的 80%），position 取碰撞盒中心（渲染以中心为原点）
        registry.emplace<TransformComponent>(e, s.x + 0.4f, s.y + 0.4f, 0.0f, 1.0f, 1.0f);

        registry.emplace<ActionStateComponent>(e);
        // 血量组件：玩家远程子弹命中扣血；死亡后重生回出生点
        registry.emplace<HealthComponent>(e, ENEMY_MAX_HP, ENEMY_MAX_HP,
            false, 0.0f, fp_from_float(s.x), fp_from_float(s.y));
        registry.emplace<AIComponent>(e,
            fp_from_float(s.x), fp_from_float(s.y),  // patrol_center
            fp_from_float(s.patrol_radius),
            fp_from_float(s.detect_radius),
            fp_from_float(s.attack_radius),
            s.ranged,                                   // 是否远程敌人
            fp_from_float(s.bullet_speed),              // 子弹速度
            fp_from_float(s.bullet_damage),             // 子弹伤害
            (int)(s.bullet_interval * 20.0f + 0.5f),    // 射击间隔（tick）
            0                                           // 射击冷却
        );
    }
}

#endif