#ifndef BULLET_SYSTEM_H
#define BULLET_SYSTEM_H

#include "../components/bullet_component.h"
#include "../components/health_component.h"
#include "../components/player_component.h"
#include "../components/enemy_component.h"
#include "../components/logic_position_component.h"
#include "../components/logic_rect_component.h"
#include "./map_def.h"
#include <cmath>
#include <vector>

struct Context;

// 子弹推进系统：移动 / 地图碰撞 / 玩家命中 / 生命周期
// 两端确定性运行：逻辑只依赖相同输入，无需网络同步。
static void bullet_system(entt::registry& registry, Context* ctx, float /*dt*/)
{
    auto bullet_view = registry.view<BulletComponent, LogicPositionComponent>();
    auto player_view = registry.view<PlayerComponent, LogicPositionComponent,
                                     LogicRectComponent, HealthComponent>();
    auto enemy_view = registry.view<EnemyComponent, LogicPositionComponent,
                                    LogicRectComponent, HealthComponent>();

    std::vector<entt::entity> to_destroy;

    for (auto e : bullet_view) {
        auto& b   = bullet_view.get<BulletComponent>(e);
        auto& pos = bullet_view.get<LogicPositionComponent>(e);

        // 1) 移动（LogicPosition 为碰撞盒左上角，速度 * 固定步长）
        pos.x = fp_add(pos.x, fp_mul(b.vx, fp_from_float(ctx->FIXED_TIMESTEP)));
        pos.y = fp_add(pos.y, fp_mul(b.vy, fp_from_float(ctx->FIXED_TIMESTEP)));

        // 2) 生命周期
        b.life -= ctx->FIXED_TIMESTEP;
        if (b.life <= 0.0f) { to_destroy.push_back(e); continue; }

        // 3) 地图碰撞：检测子弹中心所在格
        float cx = fp_to_float(pos.x) + BULLET_HALF;
        float cy = fp_to_float(pos.y) + BULLET_HALF;
        if (map_is_collidable(map_tile_at((int)floorf(cx), (int)floorf(cy)))) {
            to_destroy.push_back(e);
            continue;
        }

        // 4) 命中判定：敌人子弹（owner=0）命中玩家；玩家子弹（owner>0）命中敌人。无友伤。
        //    AABB 相交（子弹盒 vs 目标盒，均为左上角 + 宽高）
        float bx = fp_to_float(pos.x);
        float by = fp_to_float(pos.y);
        if (b.owner_conv == 0) {
            for (auto p : player_view) {
                auto& ppos  = player_view.get<LogicPositionComponent>(p);
                auto& prect = player_view.get<LogicRectComponent>(p);
                float px = fp_to_float(ppos.x);
                float py = fp_to_float(ppos.y);
                float pw = fp_to_float(prect.width);
                float ph = fp_to_float(prect.height);
                if (bx < px + pw && bx + BULLET_SIZE > px &&
                    by < py + ph && by + BULLET_SIZE > py) {
                    auto& h = player_view.get<HealthComponent>(p);
                    if (h.dead) continue;   // 已死亡玩家不结算子弹伤害
                    h.hp -= (int)fp_to_float(b.damage);
                    if (h.hp < 0) h.hp = 0;
                    to_destroy.push_back(e);
                    break;
                }
            }
        } else {
            // 玩家子弹 → 敌人（跳过已死亡敌人：尸体不受二次伤害）
            for (auto en : enemy_view) {
                auto& epos  = enemy_view.get<LogicPositionComponent>(en);
                auto& erect = enemy_view.get<LogicRectComponent>(en);
                float ex = fp_to_float(epos.x);
                float ey = fp_to_float(epos.y);
                float ew = fp_to_float(erect.width);
                float eh = fp_to_float(erect.height);
                if (bx < ex + ew && bx + BULLET_SIZE > ex &&
                    by < ey + eh && by + BULLET_SIZE > ey) {
                    auto& h = enemy_view.get<HealthComponent>(en);
                    if (h.dead) continue;   // 已死亡敌人不结算子弹伤害
                    h.hp -= (int)fp_to_float(b.damage);
                    if (h.hp < 0) h.hp = 0;
                    to_destroy.push_back(e);
                    break;
                }
            }
        }
    }

    for (auto e : to_destroy) registry.destroy(e);
}

#endif
