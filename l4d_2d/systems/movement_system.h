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
#include <cstdio>   // 临时诊断日志（move_debug.log）

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
// 复用 joy 的 collision2df_check_rectangles（定点数，帧同步确定性），与 collision_system 一致：
// 对覆盖到的每个格子，若该格越界或不可通行，则与其实体 AABB 做重叠判定。
// 因覆盖范围是 EPS 收缩后的严格内部，被覆盖的不可通行格必然与移动体严格重叠，
// 故判定结果与原来的"网格覆盖"完全等价。
static inline bool movement_box_blocked(fp_t x0, fp_t y0, fp_t w, fp_t h)
{
    const fp_t x1 = fp_add(x0, w);
    const fp_t y1 = fp_add(y0, h);
    const fp_t EPS = fp_from_float(0.001f);

    int gx0 = movement_fp_floor_to_int(x0);
    int gx1 = movement_fp_floor_to_int(fp_sub(x1, EPS));
    int gy0 = movement_fp_floor_to_int(y0);
    int gy1 = movement_fp_floor_to_int(fp_sub(y1, EPS));

    rectanglef_t mover_box = { x0, y0, w, h };
    for (int gy = gy0; gy <= gy1; gy++) {
        for (int gx = gx0; gx <= gx1; gx++) {
            if (gx >= 0 && gy >= 0 && gx < MAP_W && gy < MAP_H &&
                !map_is_collidable(map_tile_at(gx, gy))) continue;   // 越界或不可通行格才判定
            rectanglef_t tile_box = { fp_from_int(gx), fp_from_int(gy), fp_one(), fp_one() };
            if (collision2df_check_rectangles(mover_box, tile_box)) return true;
        }
    }
    return false;
}

// ============ 逐轴扫掠（swept AABB on tilemap）============
// 与"先推进到底、再按最小穿透方向推出"的根本区别：
// 扫掠先算出"沿运动方向会撞到哪一格"，只推进到接触点，全程**不产生穿透**，
// 因此不存在"从哪一侧顶出"的歧义，任意速度都不会穿墙，也没有可调参数。
// 格边界是整数且轴对齐，故扫掠退化为整数行/列扫描：无需 slab 除法，也无需 EPA。
//
// 覆盖格约定（与 movement_box_blocked 一致）：近端用 floor、远端用 floor(v - EPS)，
// 使"恰好贴边"不算阻挡；越界一律视为墙。
//
// 注：扫掠只处理"本次移动新跨越的格"。若实体先天陷在墙里（如出生点异常），
// 由紧随其后的 collision_system 做推出修正（game_scene.cpp 中 movement → collision）。

// 扫掠用：行 r 在列跨度 [cx0,cx1] 内是否存在不可通行格
static inline bool movement_row_blocked(int r, int cx0, int cx1)
{
    for (int c = cx0; c <= cx1; c++) {
        if (c < 0 || r < 0 || c >= MAP_W || r >= MAP_H) return true;   // 越界视为墙
        if (map_is_collidable(map_tile_at(c, r))) return true;
    }
    return false;
}

// 扫掠用：列 c 在行跨度 [ry0,ry1] 内是否存在不可通行格
static inline bool movement_col_blocked(int c, int ry0, int ry1)
{
    for (int r = ry0; r <= ry1; r++) {
        if (c < 0 || r < 0 || c >= MAP_W || r >= MAP_H) return true;   // 越界视为墙
        if (map_is_collidable(map_tile_at(c, r))) return true;
    }
    return false;
}

// 水平扫掠：沿 X 推进 dx（可正可负），撞墙则贴齐到墙格边界并返回 true。
// 纯定点整数运算，帧同步确定性。
static inline bool movement_sweep_x(fp_t& px, fp_t& py, fp_t dx, fp_t w, fp_t h)
{
    if (dx == fp_zero()) return false;
    const fp_t EPS = fp_from_float(0.001f);
    // X 扫掠期间行跨度保持不变
    const int ry0 = movement_fp_floor_to_int(py);
    const int ry1 = movement_fp_floor_to_int(fp_sub(fp_add(py, h), EPS));

    if (dx > fp_zero()) {                                   // 向右：右缘向右跨列
        const fp_t r_old = fp_add(px, w);
        const int c0 = movement_fp_floor_to_int(fp_sub(r_old, EPS)) + 1;
        const int c1 = movement_fp_floor_to_int(fp_sub(fp_add(r_old, dx), EPS));
        for (int c = c0; c <= c1; c++) {                    // 由近及远，首个阻挡即最先接触
            if (movement_col_blocked(c, ry0, ry1)) {
                px = fp_sub(fp_from_int(c), w);             // 贴到该列左边界
                return true;
            }
        }
    } else {                                                // 向左：左缘向左跨列
        const int c0 = movement_fp_floor_to_int(px) - 1;
        const int c1 = movement_fp_floor_to_int(fp_add(px, dx));
        for (int c = c0; c >= c1; c--) {                    // 由近及远
            if (movement_col_blocked(c, ry0, ry1)) {
                px = fp_from_int(c + 1);                    // 贴到该列右边界
                return true;
            }
        }
    }
    px = fp_add(px, dx);                                    // 无阻挡，完整推进
    return false;
}

// 竖直扫掠：沿 Y 推进 vy*tick（y 轴向下为正，故 >0 下落、<0 上升）。
// 撞到则贴齐并把 vy 归零，返回 true。纯定点整数运算，帧同步确定性。
static inline bool movement_sweep_y(fp_t& px, fp_t& py, fp_t& vy, fp_t tick, fp_t w, fp_t h)
{
    const fp_t dy = fp_mul(vy, tick);
    if (dy == fp_zero()) return false;
    const fp_t EPS = fp_from_float(0.001f);
    // Y 扫掠期间列跨度保持不变
    const int cx0 = movement_fp_floor_to_int(px);
    const int cx1 = movement_fp_floor_to_int(fp_sub(fp_add(px, w), EPS));

    if (dy > fp_zero()) {                                   // 下落：底边向下跨行
        const fp_t b_old = fp_add(py, h);
        const int r0 = movement_fp_floor_to_int(fp_sub(b_old, EPS)) + 1;
        const int r1 = movement_fp_floor_to_int(fp_sub(fp_add(b_old, dy), EPS));
        for (int r = r0; r <= r1; r++) {                    // 自上而下，首个阻挡即最先接触
            if (movement_row_blocked(r, cx0, cx1)) {
                py = fp_sub(fp_from_int(r), h);             // 贴到该行顶边（落地）
                vy = fp_zero();
                return true;
            }
        }
    } else {                                                // 上升：顶边向上跨行
        const int r0 = movement_fp_floor_to_int(py) - 1;
        const int r1 = movement_fp_floor_to_int(fp_add(py, dy));
        for (int r = r0; r >= r1; r--) {                    // 自下而上，首个阻挡即最先接触
            if (movement_row_blocked(r, cx0, cx1)) {
                py = fp_from_int(r + 1);                    // 贴到该行底边（撞顶）
                vy = fp_zero();
                return true;
            }
        }
    }
    py = fp_add(py, dy);                                    // 无阻挡，完整推进
    return false;
}

// 横版（side-view）玩家移动：
//  - X 轴：左/右位移 + 撞墙贴齐（沿用旧逻辑，bit2=左 bit3=右）
//  - Y 轴：重力积分 + 跳跃 + 竖直碰撞（落到平台顶 / 撞到天花板）
//  - 跳跃键 = move_bits 的 bit0（W / 方向键上 / 触屏上），仅地面可跳
//  - on_ground 由碰撞实时推导（不另存标志），全程定点运算，帧同步确定
static void movement_system(entt::registry& registry, Context* ctx, float /*dt*/) {
    // 视图用 PlayerComponent+LogicPosition（二者随玩家创建即存在）：避免"玩家还没收到输入、
    // 尚未挂 PlayerStateInput 时不被模拟（不受重力）"的问题。move_bits 缺省按 0 处理。
    auto view = registry.view<PlayerComponent, LogicPositionComponent>();
    const fp_t tick     = fp_from_float(game_runtime(ctx).FIXED_TIMESTEP);
    const fp_t gravity  = fp_from_float(SIDE_GRAVITY);
    const fp_t jump_spd = fp_from_float(SIDE_JUMP_SPEED);
    const fp_t max_fall = fp_from_float(SIDE_MAX_FALL);
    const fp_t EPS      = fp_from_float(0.001f);

    for (auto e : view) {
        if (registry.all_of<HealthComponent>(e) && registry.get<HealthComponent>(e).dead) { continue; }
        auto& p = view.get<LogicPositionComponent>(e);

        // 横版：bit0=上=跳，bit2=左，bit3=右（bit1=下 预留）；不再自由上下移动
        const uint8_t bits = registry.all_of<PlayerStateInput>(e)
                           ? registry.get<PlayerStateInput>(e).move_bits : 0;

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

        // ---- X 轴：左右（扫掠推进，撞墙则贴齐墙格边界）----
        if ((bits & 0x4) || (bits & 0x8)) {
            fp_t dx = fp_zero();
            if (bits & 0x4) dx = fp_sub(dx, step);   // 左
            if (bits & 0x8) dx = fp_add(dx, step);   // 右
            movement_sweep_x(p.x, p.y, dx, w, h);
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

        // 竖直位移 + 碰撞（扫掠推进，落地 / 撞顶时内部速度归零）
        movement_sweep_y(p.x, p.y, vy, tick, w, h);
        registry.get<LogicVelocityComponent>(e).y = vy;
    }
}

#endif