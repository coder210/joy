#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H
#include "../components/player_state_input.h"
#include "../components/logic_position_component.h"
#include "../components/logic_rect_component.h"
#include "../components/health_component.h"
#include "../components/player_component.h"
#include "../components/logic_velocity_component.h"   // 横版重力/跳跃垂直速度
#include "character_catalog.h"
#include "map_def.h"
#include <joy/jmath.h>

struct Context;

// 移速兜底：异常实体（无 PlayerComponent）时使用。
// 正常玩家移速取自角色 stats.speed（characters.json），见 movement_system 内。
static const fp_t MOVE_SPEED_FALLBACK = fp_from_float(2.4f);

// ---- 横版（side-view）重力/跳跃参数 ----
// 均为"米 / 米每秒 / 米每平方秒"的浮点来源，进入逻辑后转定点参与帧同步。
// 跳跃高度 ≈ v² / (2g) ≈ 15²/(2·34) ≈ 3.3 格，可按手感调整。
static constexpr float SIDE_GRAVITY    = 34.0f;   // 重力加速度
static constexpr float SIDE_JUMP_SPEED = 15.0f;   // 跳跃初速（向上）
static constexpr float SIDE_MAX_FALL   = 24.0f;   // 下落终端速度（防高速穿透平台）

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

// 横版（side-view）玩家移动：
//  - X 轴：左/右位移 + 撞墙贴齐（沿用旧逻辑，bit2=左 bit3=右）
//  - Y 轴：重力积分 + 跳跃 + 竖直碰撞（落到平台顶 / 撞到天花板）
//  - 跳跃键 = move_bits 的 bit0（W / 方向键上 / 触屏上），仅地面可跳
//  - on_ground 由碰撞实时推导（不另存标志），全程定点运算，帧同步确定
static void movement_system(entt::registry& registry, Context* ctx, float /*dt*/) {
    auto view = registry.view<PlayerStateInput, LogicPositionComponent>();
    const fp_t tick     = fp_from_float(game_runtime(ctx).FIXED_TIMESTEP);
    const fp_t gravity  = fp_from_float(SIDE_GRAVITY);
    const fp_t jump_spd = fp_from_float(SIDE_JUMP_SPEED);
    const fp_t max_fall = fp_from_float(SIDE_MAX_FALL);
    const fp_t EPS      = fp_from_float(0.001f);

    for (auto e : view) {
        if (registry.all_of<HealthComponent>(e) && registry.get<HealthComponent>(e).dead) { continue; }
        auto& psi = view.get<PlayerStateInput>(e);
        auto& p   = view.get<LogicPositionComponent>(e);

        // 横版：bit0=上=跳，bit2=左，bit3=右（bit1=下 预留）；不再自由上下移动
        const uint8_t bits = psi.move_bits;

        // 移速按角色取（stats.speed，米/秒）。
        // 注意：不要把 PlayerComponent 加进 view —— 改变 view 组成会改变 entt 迭代顺序，
        // 帧同步下遍历顺序必须严格一致，故在循环内按需取。
        fp_t speed = MOVE_SPEED_FALLBACK;
        if (registry.all_of<PlayerComponent>(e))
            speed = g_characterCatalog.get(registry.get<PlayerComponent>(e).char_id).move_speed;
        const fp_t step = fp_mul(speed, tick);

        // 无碰撞盒（异常实体）时退化为左右平移
        if (!registry.all_of<LogicRectComponent>(e)) {
            if (bits & 0x4) p.x = fp_sub(p.x, step);
            if (bits & 0x8) p.x = fp_add(p.x, step);
            continue;
        }

        const fp_t w = registry.get<LogicRectComponent>(e).width;
        const fp_t h = registry.get<LogicRectComponent>(e).height;

        // ---- X 轴：左右 ----
        if ((bits & 0x4) || (bits & 0x8)) {
            fp_t cand = p.x;
            int dx = 0;
            if (bits & 0x4) { cand = fp_sub(cand, step); dx = -1; }   // 左
            if (bits & 0x8) { cand = fp_add(cand, step); dx = +1; }   // 右
            if (!movement_box_blocked(cand, p.y, w, h)) p.x = cand;        // 无障碍则正常移动
            else p.x = movement_snap_axis(p.x, w, cand, dx);               // 撞墙则贴齐墙格边界
        }

        // ---- Y 轴：重力 + 跳跃（仅对挂有速度组件的实体）----
        if (!registry.all_of<LogicVelocityComponent>(e)) continue;
        fp_t vy = registry.get<LogicVelocityComponent>(e).y;

        // 是否站在地面：脚下 1mm 处被阻挡即为着地
        const bool grounded = movement_box_blocked(p.x, fp_add(p.y, EPS), w, h);
        if (grounded && vy > fp_zero()) vy = fp_zero();      // 落地即清除下落速度

        // 跳跃：上键，且仅在地面
        if ((bits & 0x1) && grounded) vy = fp_sub(fp_zero(), jump_spd);

        // 重力积分 + 终端速度
        vy = fp_add(vy, fp_mul(gravity, tick));
        if (vy > max_fall) vy = max_fall;

        // 竖直位移 + 碰撞
        const fp_t move = fp_mul(vy, tick);
        if (move != fp_zero()) {
            const fp_t cand = fp_add(p.y, move);
            if (!movement_box_blocked(p.x, cand, w, h)) {
                p.y = cand;
            } else {
                const int dir = (move > fp_zero()) ? +1 : -1;
                p.y = movement_snap_axis(p.y, h, cand, dir);
                vy = fp_zero();      // 落地 / 撞顶：速度归零
            }
        }
        registry.get<LogicVelocityComponent>(e).y = vy;
    }
}

#endif