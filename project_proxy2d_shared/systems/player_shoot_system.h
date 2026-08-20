#ifndef PLAYER_SHOOT_SYSTEM_H
#define PLAYER_SHOOT_SYSTEM_H

#include "../components/action_state_component.h"
#include "../components/bullet_component.h"
#include "../components/health_component.h"
#include "../components/id_component.h"
#include "../components/logic_position_component.h"
#include "../components/logic_rect_component.h"
#include "../components/player_component.h"
#include "../components/player_state_input.h"
#include "../components/transform_component.h"
#include "../protocol.h"

struct Context;

/* 玩家远程射击：按住远程攻击键（键盘 E / 屏上 E 按钮，INPUT_RANGED）沿当前朝向持续发射子弹，
 * 每 RANGED_COOLDOWN 秒一发。帧同步确定性：两端同一 tick 用 get_entity_id 生成相同 ID 的子弹实体。 */
static void spawn_player_bullet(entt::registry& registry, Context* ctx,
                                fp_t px, fp_t py, int conv, int dir)
{
    // 玩家子弹参数（几何 m，速度 m/s，伤害直接作用于敌人 HP）
    const fp_t speed  = fp_from_float(9.0f);
    const fp_t  damage = fp_from_float(25.0f);

    // 玩家碰撞盒 0.8x0.8，中心 = 左上角 + 0.4
    fp_t cx = fp_add(px, fp_from_float(0.4f));
    fp_t cy = fp_add(py, fp_from_float(0.4f));
    fp_t vx = 0, vy = 0;
    switch (dir) {
        case 0:  vy =  speed; break; // 下
        case 1:  vx = -speed; break; // 左
        case 2:  vx =  speed; break; // 右
        default: vy = -speed; break; // 上
    }

    // 出生点：从玩家中心沿朝向推进 0.4m，避免子弹出生即与玩家/墙体重叠
    cx = fp_add(cx, fp_mul(vx, fp_from_float(0.04f)));
    cy = fp_add(cy, fp_mul(vy, fp_from_float(0.04f)));

    vec2_t position = { fp_to_float(cx), fp_to_float(cy) };
    vec2_t scale = { BULLET_SIZE, BULLET_SIZE };

    entt::entity b = registry.create();
    registry.emplace<IdComponent>(b, get_entity_id(ctx));
    // owner_conv = 玩家 conv + 1（0 保留给敌人子弹，bullet_system 按 owner==0 区分阵营）
    registry.emplace<BulletComponent>(b,
        vx, vy, damage, conv + 1, BULLET_LIFE_TICKS);
    registry.emplace<LogicPositionComponent>(b, fp_sub(cx,  fp_from_float(BULLET_HALF)), fp_sub(cy, fp_from_float(BULLET_HALF)));
    registry.emplace<LogicRectComponent>(b, fp_from_float(BULLET_SIZE), fp_from_float(BULLET_SIZE));
    registry.emplace<TransformComponent>(b, position, 0.0f, scale);
}

static void player_shoot_system(entt::registry& registry, Context* ctx, float /*dt*/)
{
    auto view = registry.view<PlayerComponent, PlayerStateInput, LogicPositionComponent, ActionStateComponent>();
    for (auto e : view) {
        auto& act = view.get<ActionStateComponent>(e);
        // 冷却递减（无条件，无论是否按住），每逻辑 tick 减 1
        if (act.ranged_cooldown > 0) act.ranged_cooldown -= 1;

        auto& psi = view.get<PlayerStateInput>(e);
        if (!(psi.move_bits & INPUT_RANGED)) continue;

        // 死亡玩家不能射击
        if (registry.all_of<HealthComponent>(e) && registry.get<HealthComponent>(e).dead) continue;
        if (act.ranged_cooldown > 0) continue;

        act.ranged_cooldown = ActionStateComponent::RANGED_COOLDOWN;
        auto& pos = view.get<LogicPositionComponent>(e);
        auto& pc  = view.get<PlayerComponent>(e);
        spawn_player_bullet(registry, ctx, pos.x, pos.y, pc.conv, act.dir);
    }
}

#endif
