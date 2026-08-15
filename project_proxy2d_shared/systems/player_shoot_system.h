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
    const float speed  = 9.0f;
    const fp_t  damage = fp_from_float(25.0f);

    // 玩家碰撞盒 1x1，中心 = 左上角 + 0.5
    float cx = fp_to_float(px) + 0.5f;
    float cy = fp_to_float(py) + 0.5f;
    float vx = 0.0f, vy = 0.0f;
    switch (dir) {
        case 0:  vy =  speed; break; // 下
        case 1:  vx = -speed; break; // 左
        case 2:  vx =  speed; break; // 右
        default: vy = -speed; break; // 上
    }

    // 出生点：从玩家中心沿朝向推进 0.4m，避免子弹出生即与玩家/墙体重叠
    cx += vx * 0.04f;
    cy += vy * 0.04f;

    entt::entity b = registry.create();
    registry.emplace<IdComponent>(b, get_entity_id(ctx));
    // owner_conv = 玩家 conv + 1（0 保留给敌人子弹，bullet_system 按 owner==0 区分阵营）
    registry.emplace<BulletComponent>(b,
        fp_from_float(vx), fp_from_float(vy), damage, conv + 1, BULLET_LIFE);
    registry.emplace<LogicPositionComponent>(b, fp_from_float(cx - BULLET_HALF), fp_from_float(cy - BULLET_HALF));
    registry.emplace<LogicRectComponent>(b, fp_from_float(BULLET_SIZE), fp_from_float(BULLET_SIZE));
    registry.emplace<TransformComponent>(b,
        cx, cy, 0.0f, BULLET_SIZE, BULLET_SIZE);
}

static void player_shoot_system(entt::registry& registry, Context* ctx, float /*dt*/)
{
    auto view = registry.view<PlayerComponent, PlayerStateInput, LogicPositionComponent, ActionStateComponent>();
    for (auto e : view) {
        auto& act = view.get<ActionStateComponent>(e);
        // 冷却递减（无条件，无论是否按住）
        if (act.ranged_cooldown > 0.0f) act.ranged_cooldown -= ctx->FIXED_TIMESTEP;

        auto& psi = view.get<PlayerStateInput>(e);
        if (!(psi.move_bits & INPUT_RANGED)) continue;

        // 死亡玩家不能射击
        if (registry.all_of<HealthComponent>(e) && registry.get<HealthComponent>(e).dead) continue;
        if (act.ranged_cooldown > 0.0f) continue;

        act.ranged_cooldown = ActionStateComponent::RANGED_COOLDOWN;
        auto& pos = view.get<LogicPositionComponent>(e);
        auto& pc  = view.get<PlayerComponent>(e);
        spawn_player_bullet(registry, ctx, pos.x, pos.y, pc.conv, act.dir);
    }
}

#endif
