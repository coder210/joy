#ifndef PLAYER_DEATH_SYSTEM_H
#define PLAYER_DEATH_SYSTEM_H
#include "../components/health_component.h"
#include "../components/logic_position_component.h"
#include "../components/logic_rect_component.h"
#include "../components/player_component.h"
#include "../components/enemy_component.h"
#include "../components/transform_component.h"

struct Context;

// 玩家死亡 / 重生（帧同步确定性：两端同一 tick 判定与传送，无需网络同步）
// 注册在 bullet_system 之后：子弹扣血 → 死亡判定 → 重生计时
static void player_death_system(entt::registry& registry, Context* ctx, float /*dt*/)
{
    auto view = registry.view<PlayerComponent, HealthComponent, LogicPositionComponent>();
    for (auto e : view) {
        auto& h   = view.get<HealthComponent>(e);
        auto& pos = view.get<LogicPositionComponent>(e);

        // 死亡判定：HP 归零（bullet_system 已把 HP 钳制在 0）
        if (!h.dead && h.hp <= 0.0f) {
            h.dead = true;
            h.respawn_timer = PLAYER_RESPAWN_TIME;
        }

        // 重生计时与传送回出生点
        if (h.dead) {
            h.respawn_timer -= ctx->FIXED_TIMESTEP;
            if (h.respawn_timer <= 0.0f) {
                h.dead = false;
                h.hp = h.max_hp;
                pos.x = h.spawn_x;
                pos.y = h.spawn_y;
                // 同步渲染坐标（碰撞盒中心 = 左上角 + 半宽高），避免插值缓慢漂移
                if (registry.all_of<TransformComponent>(e)) {
                    auto& t    = registry.get<TransformComponent>(e);
                    auto& rect = registry.get<LogicRectComponent>(e);
                    t.position.x = fp_to_float(h.spawn_x) + fp_to_float(rect.width) * 0.5f;
                    t.position.y = fp_to_float(h.spawn_y) + fp_to_float(rect.height) * 0.5f;
                }
            }
        }
    }

    // 敌人死亡 / 重生：玩家远程子弹击杀后 3 秒回出生点满血复活
    auto eview = registry.view<EnemyComponent, HealthComponent, LogicPositionComponent>();
    for (auto e : eview) {
        auto& h   = eview.get<HealthComponent>(e);
        auto& pos = eview.get<LogicPositionComponent>(e);

        if (!h.dead && h.hp <= 0.0f) {
            h.dead = true;
            h.respawn_timer = PLAYER_RESPAWN_TIME;
        }

        if (h.dead) {
            h.respawn_timer -= ctx->FIXED_TIMESTEP;
            if (h.respawn_timer <= 0.0f) {
                h.dead = false;
                h.hp = h.max_hp;
                pos.x = h.spawn_x;
                pos.y = h.spawn_y;
                if (registry.all_of<TransformComponent>(e)) {
                    auto& t    = registry.get<TransformComponent>(e);
                    auto& rect = registry.get<LogicRectComponent>(e);
                    t.position.x = fp_to_float(h.spawn_x) + fp_to_float(rect.width) * 0.5f;
                    t.position.y = fp_to_float(h.spawn_y) + fp_to_float(rect.height) * 0.5f;
                }
            }
        }
    }
}

#endif
