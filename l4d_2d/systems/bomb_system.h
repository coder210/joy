#ifndef BOMB_SYSTEM_H
#define BOMB_SYSTEM_H

#include "../components/bomb_component.h"
#include "../components/health_component.h"
#include "../components/player_component.h"
#include "../components/enemy_component.h"
#include "../components/logic_position_component.h"
#include "../components/logic_rect_component.h"
#include "../components/action_state_component.h"
#include "./map_def.h"
#include "../context.h"
#include <vector>

struct Context;

// 定点数向下取整转 int（与 bullet_system 一致）
static inline int bomb_fp_floor_to_int(fp_t x)
{
    return (int)(x >> FP_FRACTIONAL_PLACES);
}

// 炸弹碰撞盒是否与不可通行格重叠（EPS 收缩后查覆盖格，纯定点数、帧同步确定性）。
static inline bool bomb_box_blocked(fp_t x0, fp_t y0, fp_t w, fp_t h)
{
    const fp_t x1 = fp_add(x0, w);
    const fp_t y1 = fp_add(y0, h);
    const fp_t EPS = fp_from_float(0.001f);
    int gx0 = bomb_fp_floor_to_int(x0);
    int gx1 = bomb_fp_floor_to_int(fp_sub(x1, EPS));
    int gy0 = bomb_fp_floor_to_int(y0);
    int gy1 = bomb_fp_floor_to_int(fp_sub(y1, EPS));
    for (int gy = gy0; gy <= gy1; gy++)
        for (int gx = gx0; gx <= gx1; gx++)
            if (map_is_collidable(map_tile_at(gx, gy)))
                return true;
    return false;
}

// 炸弹推进系统：飞行（重力 + 地图碰撞）→ 引信倒计时 → 爆炸（范围伤害）→ 销毁
// 两端确定性运行：逻辑只依赖相同输入与相同物理步进，无需网络同步（实体由 get_entity_id 本地生成）。
static void bomb_system(entt::registry& registry, Context* ctx, float /*dt*/)
{
    auto bomb_view = registry.view<BombComponent, LogicPositionComponent, LogicRectComponent>();
    auto enemy_view = registry.view<EnemyComponent, LogicPositionComponent, LogicRectComponent, HealthComponent>();

    const fp_t dt = fp_from_float(game_runtime(ctx).FIXED_TIMESTEP);
    const fp_t g  = fp_from_float(BOMB_GRAVITY);
    std::vector<entt::entity> to_destroy;

    for (auto e : bomb_view) {
        auto& bomb = bomb_view.get<BombComponent>(e);
        auto& pos  = bomb_view.get<LogicPositionComponent>(e);
        auto& rect = bomb_view.get<LogicRectComponent>(e);

        if (bomb.state == BOMB_ST_OFF) {
            // ---- 飞行阶段：重力 + 逐轴地图碰撞 ----
            bomb.vy = fp_add(bomb.vy, fp_mul(g, dt));
            bool landed = false;

            // X 轴
            fp_t nx = fp_add(pos.x, fp_mul(bomb.vx, dt));
            if (bomb_box_blocked(nx, pos.y, rect.width, rect.height)) { nx = pos.x; bomb.vx = 0; }
            pos.x = nx;

            // Y 轴
            fp_t ny = fp_add(pos.y, fp_mul(bomb.vy, dt));
            if (bomb_box_blocked(pos.x, ny, rect.width, rect.height)) {
                if (bomb.vy > 0) landed = true;   // 向下撞到地面
                ny = pos.y; bomb.vy = 0;
            }
            pos.y = ny;

            bomb.flight_ticks--;
            // 落地或飞行超时 → 进入引信态
            if (landed || bomb.flight_ticks <= 0) {
                bomb.state = BOMB_ST_ON;
                bomb.fuse_ticks = BOMB_FUSE_TICKS;
                bomb.vx = 0; bomb.vy = 0;
            }
        }
        else if (bomb.state == BOMB_ST_ON) {
            // ---- 引信阶段：静止等待引爆 ----
            bomb.fuse_ticks--;
            if (bomb.fuse_ticks <= 0) {
                bomb.state = BOMB_ST_EXPLODING;
                bomb.explode_ticks = BOMB_EXPLODE_TICKS;

                // 范围伤害：对半径内敌人一次性结算（跳过已死亡）。无友伤（与子弹一致）。
                const fp_t bcx = fp_add(pos.x, fp_from_float(BOMB_HALF));
                const fp_t bcy = fp_add(pos.y, fp_from_float(BOMB_HALF));
                const fp_t R = fp_from_float(BOMB_RADIUS);
                const fp_t R2 = fp_mul(R, R);
                const int dmg = (int)BOMB_DAMAGE;
                for (auto en : enemy_view) {
                    auto& h = enemy_view.get<HealthComponent>(en);
                    if (h.dead) continue;
                    auto& ep = enemy_view.get<LogicPositionComponent>(en);
                    auto& er = enemy_view.get<LogicRectComponent>(en);
                    fp_t ecx = fp_add(ep.x, fp_mul(er.width,  fp_from_float(0.5f)));
                    fp_t ecy = fp_add(ep.y, fp_mul(er.height, fp_from_float(0.5f)));
                    fp_t dx = fp_sub(ecx, bcx);
                    fp_t dy = fp_sub(ecy, bcy);
                    if (fp_add(fp_mul(dx, dx), fp_mul(dy, dy)) <= R2) {
                        h.hp -= dmg; if (h.hp < 0) h.hp = 0;
                    }
                }
            }
        }
        else { // BOMB_ST_EXPLODING
            bomb.explode_ticks--;
            if (bomb.explode_ticks <= 0) to_destroy.push_back(e);
        }
    }

    for (auto e : to_destroy) registry.destroy(e);
}

#endif
