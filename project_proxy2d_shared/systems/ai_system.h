#ifndef AI_SYSTEM_H
#define AI_SYSTEM_H

#include "../components/ai_component.h"
#include "../components/enemy_component.h"
#include "../components/player_component.h"
#include "../components/logic_position_component.h"
#include "../components/logic_rect_component.h"
#include "../components/transform_component.h"
#include "../components/id_component.h"
#include "../components/bullet_component.h"
#include "../components/action_state_component.h"
#include "../components/player_event_input.h"
#include "../components/pathfinding_component.h"
#include "../components/health_component.h"
#include <cmath>

struct Context;

// 远程敌人发射子弹：帧同步下两端用 get_entity_id 分配相同 ID，无需网络同步。
static void spawn_enemy_bullet(entt::registry& registry, Context* ctx,
                               fp_t ex, fp_t ey, fp_t px, fp_t py,
                               const AIComponent& ai)
{
    // 从敌人中心指向玩家中心（LogicPosition 均为碰撞盒左上角，0.8 盒中心 = +0.4）
    float exf = fp_to_float(ex) + 0.4f;
    float eyf = fp_to_float(ey) + 0.4f;
    float pxf = fp_to_float(px) + 0.4f;
    float pyf = fp_to_float(py) + 0.4f;
    float dx = pxf - exf;
    float dy = pyf - eyf;
    float len = sqrtf(dx * dx + dy * dy);
    if (len < 0.001f) len = 1.0f;
    float speed = fp_to_float(ai.bullet_speed);
    float vx = dx / len * speed;
    float vy = dy / len * speed;

    // 出生点：敌人中心（子弹碰撞盒左上角 = 中心 - 半尺寸）
    float cxf = exf - BULLET_HALF;
    float cyf = eyf - BULLET_HALF;

    entt::entity b = registry.create();
    registry.emplace<IdComponent>(b, get_entity_id(ctx));
    registry.emplace<BulletComponent>(b,
        fp_from_float(vx), fp_from_float(vy), ai.bullet_damage, 0, BULLET_LIFE);
    registry.emplace<LogicPositionComponent>(b, fp_from_float(cxf), fp_from_float(cyf));
    registry.emplace<LogicRectComponent>(b, fp_from_float(BULLET_SIZE), fp_from_float(BULLET_SIZE));
    registry.emplace<TransformComponent>(b,
        cxf + BULLET_HALF, cyf + BULLET_HALF, 0.0f, BULLET_SIZE, BULLET_SIZE);
}

static void ai_system(entt::registry& registry, Context* ctx, float /*dt*/) {
    fp_t step = fp_mul(fp_from_float(2.5f), fp_from_float(ctx->FIXED_TIMESTEP));

    auto view = registry.view<IdComponent, EnemyComponent, AIComponent, LogicPositionComponent, ActionStateComponent>();
    auto player_view = registry.view<PlayerComponent, LogicPositionComponent, HealthComponent>();

    if (!ctx->is_server) {
        int i = 10;
    }

    for (auto e : view) {
        auto& id =  view.get<IdComponent>(e);
        auto& ai  = view.get<AIComponent>(e);
        auto& pos = view.get<LogicPositionComponent>(e);
        auto& act = view.get<ActionStateComponent>(e);

        // 死亡敌人不行动（不追击/不攻击/不巡逻），等待 player_death_system 重生
        if (registry.all_of<HealthComponent>(e) && registry.get<HealthComponent>(e).dead) {
            if (registry.all_of<PathComponent>(e)) registry.remove<PathComponent>(e);
            continue;
        }

        // 攻击计时递减（替代动画状态机的攻击复位），每逻辑 tick 减 1
        if (act.attack_timer > 0) act.attack_timer -= 1;

        // 寻找最近玩家
        bool found_player = false;
        fp_t nearest_dist_sq = fp_mul(ai.detect_radius, ai.detect_radius);
        fp_t nearest_px = fp_zero(), nearest_py = fp_zero();

        for (auto p : player_view) {
            // 死亡玩家不作为敌人攻击/追击目标（不鞭尸）
            if (player_view.get<HealthComponent>(p).dead) continue;
            auto& ppos = player_view.get<LogicPositionComponent>(p);
            fp_t dx = fp_sub(ppos.x, pos.x);
            fp_t dy = fp_sub(ppos.y, pos.y);
            fp_t dist_sq = fp_add(fp_mul(dx, dx), fp_mul(dy, dy));
            if (dist_sq < nearest_dist_sq) {
                nearest_dist_sq = dist_sq;
                found_player = true;
                nearest_px = ppos.x;
                nearest_py = ppos.y;
            }
        }

        // 攻击半径判断
        fp_t attack_dist_sq = fp_mul(ai.attack_radius, ai.attack_radius);
        bool in_attack_range = found_player && (nearest_dist_sq < attack_dist_sq);

        if (in_attack_range) {
            // 状态 2：攻击
            ai.state = 2;
            // 首次进入攻击范围时设置攻击计时器
            if (act.attack_timer == 0) {
                    act.attack_timer = ActionStateComponent::ATTACK_DURATION;
            }
            act.state = ActionStateComponent::ATTACK;

            // 远程敌人：射击冷却与子弹生成（近战敌人跳过）
            if (ai.ranged) {
                if (ai.shoot_cooldown > 0) {
                    ai.shoot_cooldown--;
                } else {
                    ai.shoot_cooldown = ai.shoot_interval;
                    spawn_enemy_bullet(registry, ctx, pos.x, pos.y,
                                       nearest_px, nearest_py, ai);
                }
            }

            if (!registry.all_of<PlayerEventInput>(e)) {
                registry.emplace<PlayerEventInput>(e);
            }
            auto& events = registry.get<PlayerEventInput>(e);
            events.pending_actions.push_back({ 0, 0 });

            fp_t dx = fp_sub(nearest_px, pos.x);
            fp_t dy = fp_sub(nearest_py, pos.y);
            act.dir = (fp_abs(dx) > fp_abs(dy))
                ? (dx > fp_zero() ? 2 : 1)
                : (dy > fp_zero() ? 0 : 3);
        }
        else if (found_player) {
            // 状态 1：追击（实际移动由 pathfinding_system 的 A* 完成）
            ai.state = 1;
            // 攻击计时未结束前保持攻击表现，结束后回到 WALK
            act.state = act.attack_timer > 0 ? ActionStateComponent::ATTACK
                                             : ActionStateComponent::WALK;

            // 记录最近玩家所在格子作为寻路目标，标记需要重算
            if (!registry.all_of<PathComponent>(e)) {
                registry.emplace<PathComponent>(e);
            }
            auto& pc = registry.get<PathComponent>(e);
            pc.target_gx = (int)(fp_to_float(nearest_px));
            pc.target_gy = (int)(fp_to_float(nearest_py));
            pc.dirty = true;

            fp_t dx = fp_sub(nearest_px, pos.x);
            fp_t dy = fp_sub(nearest_py, pos.y);
            act.dir = (fp_abs(dx) > fp_abs(dy))
                ? (dx > fp_zero() ? 2 : 1)
                : (dy > fp_zero() ? 0 : 3);

            ai.timer = 0;
        }
        else {
            // 状态 0：巡逻
            ai.state = 0;
            // 攻击计时未结束前保持攻击表现，结束后回到 WALK
            act.state = act.attack_timer > 0 ? ActionStateComponent::ATTACK
                                             : ActionStateComponent::WALK;

            // 巡逻目标更新逻辑（仅当 timer 归零时生成新目标）
            ai.timer--;
            if (ai.timer <= 0) {
                int seed = 0;
                fp_t rx = fp_from_float((fp_to_float(ai.patrol_radius) * ((seed % 7) - 3) / 3.0f));
                fp_t ry = fp_from_float((fp_to_float(ai.patrol_radius) * (((seed * 13) % 9) - 4) / 4.0f));
                ai.target_x = fp_add(ai.patrol_center_x, rx);
                ai.target_y = fp_add(ai.patrol_center_y, ry);
                ai.timer = 120;
            }

            fp_t dx = fp_sub(ai.target_x, pos.x);
            fp_t dy = fp_sub(ai.target_y, pos.y);
            // 距离阈值：小于0.2个单位则认为到达
            if (fp_abs(dx) > fp_from_float(0.2f) || fp_abs(dy) > fp_from_float(0.2f)) {
                    // 确保有 PathComponent
                    if (!registry.all_of<PathComponent>(e)) {
                            registry.emplace<PathComponent>(e);
                    }
                    auto& pc = registry.get<PathComponent>(e);
                    // 将目标点转换为网格坐标（假设格子大小为 TILE_SIZE）
                    constexpr float TILE_SIZE = 1.0f; // 请根据实际情况定义
                    pc.target_gx = (int)(fp_to_float(ai.target_x) / TILE_SIZE);
                    pc.target_gy = (int)(fp_to_float(ai.target_y) / TILE_SIZE);
                    pc.dirty = true;
            }
            else {
                    // 到达目标，清除路径组件（可选）
                    if (registry.all_of<PathComponent>(e)) {
                            registry.remove<PathComponent>(e);
                    }
            }

            act.dir = (fp_abs(dx) > fp_abs(dy))
                ? (dx > fp_zero() ? 2 : 1)
                : (dy > fp_zero() ? 0 : 3);
        }
    }
}

#endif