#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H
#include "../components/player_state_input.h"
#include "../components/logic_position_component.h"
#include "../components/logic_rect_component.h"
#include "../components/health_component.h"
#include "map_def.h"
#include <joy/jmath.h>

struct Context;

static const fp_t MOVE_SPEED = fp_from_float(5.0f);

// 定点数向下取整转 int（算术右移即 floor，负向负无穷），与 collision_system 一致。
static inline int movement_fp_floor_to_int(fp_t x)
{
    return (int)(x >> FP_FRACTIONAL_PLACES);
}

// 判断 AABB(x0,y0,w,h) 是否触碰不可通行格（含越界视为墙）。
// 纯定点整数查 MAP_LAYOUT，帧同步确定性；与 collision_system 的网格覆盖规则保持一致。
static inline bool movement_box_blocked(fp_t x0, fp_t y0, fp_t w, fp_t h)
{
    const fp_t x1 = fp_add(x0, w);
    const fp_t y1 = fp_add(y0, h);
    const fp_t EPS = fp_from_float(0.001f);

    int gx0 = movement_fp_floor_to_int(x0);
    int gx1 = movement_fp_floor_to_int(fp_sub(x1, EPS));
    int gy0 = movement_fp_floor_to_int(y0);
    int gy1 = movement_fp_floor_to_int(fp_sub(y1, EPS));

    for (int gy = gy0; gy <= gy1; gy++) {
        for (int gx = gx0; gx <= gx1; gx++) {
            if (gx < 0 || gy < 0 || gx >= MAP_W || gy >= MAP_H)
                return true;                     // 越界视为墙
            if (map_is_collidable(map_tile_at(gx, gy)))
                return true;
        }
    }
    return false;
}

// 撞墙时把移动体 AABB 贴齐到墙格边界，消除"碰撞盒与墙之间仍留空隙"的视觉误差。
// 纯定点/整数运算，帧同步确定性。
// dir<0 表示向负轴移动（上/左），dir>0 表示向正轴移动（下/右）。
// 前缘（负轴：pos；正轴：pos+size）撞到墙格 g 后，贴齐到该墙格的近侧边界。
static inline fp_t movement_snap_axis(fp_t pos, fp_t size, fp_t cand, int dir)
{
    const fp_t EPS = fp_from_float(0.001f);
    if (dir < 0) {
        // 上/左：前缘 = pos，贴齐到墙格 g 的下/右边界 g+1
        int g = movement_fp_floor_to_int(cand);
        return fp_from_int(g + 1);
    } else {
        // 下/右：后缘 = pos+size，贴齐到墙格 g 的上/左边界 g
        int g = movement_fp_floor_to_int(fp_sub(fp_add(cand, size), EPS));
        return fp_sub(fp_from_int(g), size);
    }
}

// 玩家平移：逐轴移动 + 撞墙即停，避免产生渗透后被碰撞系统推回造成顶墙抖动。
// 先沿 Y（上/下）移动，再沿 X（左/右），对角移动时被挡的轴停下、未挡的轴继续滑动。
static void movement_system(entt::registry& registry, Context* ctx, float /*dt*/) {
    auto view = registry.view<PlayerStateInput, LogicPositionComponent>();
    fp_t step = fp_mul(MOVE_SPEED, fp_from_float(ctx->FIXED_TIMESTEP));

    for (auto e : view) {
        if (registry.all_of<HealthComponent>(e) && registry.get<HealthComponent>(e).dead) { continue; }
        auto& psi = view.get<PlayerStateInput>(e);
        auto& p   = view.get<LogicPositionComponent>(e);
        if (psi.move_bits == 0) { continue; }

        // 无碰撞盒（异常实体）时退化为直接位移，保持原有行为
        if (!registry.all_of<LogicRectComponent>(e)) {
            if (psi.move_bits & 0b0001) p.y = fp_sub(p.y, step);
            if (psi.move_bits & 0b0010) p.y = fp_add(p.y, step);
            if (psi.move_bits & 0b0100) p.x = fp_sub(p.x, step);
            if (psi.move_bits & 0b1000) p.x = fp_add(p.x, step);
            continue;
        }

        const fp_t w = registry.get<LogicRectComponent>(e).width;
        const fp_t h = registry.get<LogicRectComponent>(e).height;

        // Y 轴
        if ((psi.move_bits & 0b0001) || (psi.move_bits & 0b0010)) {
            fp_t cand = p.y;
            int dy = 0;
            if (psi.move_bits & 0b0001) { cand = fp_sub(cand, step); dy = -1; }   // 上
            if (psi.move_bits & 0b0010) { cand = fp_add(cand, step); dy = +1; }   // 下
            if (!movement_box_blocked(p.x, cand, w, h)) p.y = cand;        // 无障碍则正常移动
            else p.y = movement_snap_axis(p.y, h, cand, dy);               // 撞墙则贴齐墙格边界
        }
        // X 轴
        if ((psi.move_bits & 0b0100) || (psi.move_bits & 0b1000)) {
            fp_t cand = p.x;
            int dx = 0;
            if (psi.move_bits & 0b0100) { cand = fp_sub(cand, step); dx = -1; }   // 左
            if (psi.move_bits & 0b1000) { cand = fp_add(cand, step); dx = +1; }   // 右
            if (!movement_box_blocked(cand, p.y, w, h)) p.x = cand;        // 无障碍则正常移动
            else p.x = movement_snap_axis(p.x, w, cand, dx);               // 撞墙则贴齐墙格边界
        }
    }
}

#endif