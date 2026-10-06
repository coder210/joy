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
#include "../components/global_state_component.h"
#include "../components/logic_velocity_component.h"
#include "movement_system.h"     // 复用横版碰撞：着地判定 movement_box_blocked（基于 joy 的 collision2df_check_rectangles）+ 移动扫掠 movement_sweep_x / movement_sweep_y
#include <cmath>

struct Context;

// 定点数向零截断转 int（等价于 (int)(x / 2^32) 的 C 语义），纯整数运算，帧同步确定性。
static inline int fp_trunc_to_int(fp_t x)
{
    int64_t q = x >> FP_FRACTIONAL_PLACES;   // 负数时偏小（floor 方向）
    if (x < 0 && (x & (FP_ONE - 1)) != 0) q += 1;  // 向零修正
    return (int)q;
}

// 远程敌人发射子弹：帧同步下两端用 get_entity_id 分配相同 ID，无需网络同步。
static void spawn_enemy_bullet(entt::registry& registry, Context* ctx,
                               fp_t ex, fp_t ey, fp_t px, fp_t py,
                               const AIComponent& ai)
{
    // 敌人/玩家碰撞盒 0.8x0.8，中心 = 左上角 + 0.4。全程纯定点数，帧同步确定性。
    fp_t exf = fp_add(ex, fp_from_float(0.4f));
    fp_t eyf = fp_add(ey, fp_from_float(0.4f));
    fp_t pxf = fp_add(px, fp_from_float(0.4f));
    fp_t pyf = fp_add(py, fp_from_float(0.4f));
    // 从敌人中心指向玩家中心的方向向量
    fp_t dx = fp_sub(pxf, exf);
    fp_t dy = fp_sub(pyf, eyf);
    // 归一化方向并乘以速度（fp_sqrt/fp_div 均为纯整数实现，两端结果一致）
    fp_t len_sq = fp_add(fp_mul(dx, dx), fp_mul(dy, dy));
    fp_t len = fp_sqrt(len_sq);
    if (len < fp_from_float(0.001f)) len = fp_one();
    fp_t vx = fp_div(fp_mul(dx, ai.bullet_speed), len);
    fp_t vy = fp_div(fp_mul(dy, ai.bullet_speed), len);

    // 出生点：敌人中心（子弹碰撞盒左上角 = 中心 - 半尺寸）
    fp_t cxf = fp_sub(exf, fp_from_float(BULLET_HALF));
    fp_t cyf = fp_sub(eyf, fp_from_float(BULLET_HALF));

    entt::entity b = registry.create();
    registry.emplace<IdComponent>(b, get_entity_id(ctx));
    registry.emplace<BulletComponent>(b,
        vx, vy, ai.bullet_damage, 0, BULLET_LIFE_TICKS);
    registry.emplace<LogicPositionComponent>(b, cxf, cyf);
    registry.emplace<LogicRectComponent>(b, fp_from_float(BULLET_SIZE), fp_from_float(BULLET_SIZE));
    vec2_t tpos  = { fp_to_float(cxf), fp_to_float(cyf) };
    vec2_t tscale = { BULLET_SIZE, BULLET_SIZE };
    registry.emplace<TransformComponent>(b, tpos, 0.0f, tscale);
}

// 横版（side-view）敌人 AI：
//  - 受重力、落到地面/平台（与玩家共用 movement_box_blocked / movement_sweep_x / movement_sweep_y）
//  - 追击：只做水平（左右）逼近玩家；进入攻击范围则攻击
//  - 巡逻：围绕出生点水平游走（定时用 LCG 选新目标 x，帧同步确定）
//  - 不再使用 A* 寻路（横版不需要，也与重力垂直移动冲突）
static void ai_system(entt::registry& registry, Context* ctx, float /*dt*/) {
    const fp_t tick     = fp_from_float(game_runtime(ctx).FIXED_TIMESTEP);
    const fp_t step     = fp_mul(fp_from_float(2.5f), tick);
    const fp_t gravity  = fp_from_float(SIDE_GRAVITY);
    const fp_t max_fall = fp_from_float(SIDE_MAX_FALL);
    const fp_t EPS      = fp_from_float(0.001f);

    auto view = registry.view<IdComponent, EnemyComponent, AIComponent, LogicPositionComponent, ActionStateComponent>();
    auto player_view = registry.view<PlayerComponent, LogicPositionComponent, HealthComponent>();

    for (auto e : view) {
        auto& id  = view.get<IdComponent>(e);
        auto& ai  = view.get<AIComponent>(e);
        auto& pos = view.get<LogicPositionComponent>(e);
        auto& act = view.get<ActionStateComponent>(e);
        (void)id;

        // 死亡敌人不行动（等待 player_death_system 重生）
        if (registry.all_of<HealthComponent>(e) && registry.get<HealthComponent>(e).dead) continue;

        // 攻击计时递减（替代动画状态机的攻击复位），每逻辑 tick 减 1
        if (act.attack_timer > 0) act.attack_timer -= 1;

        // 寻找最近玩家（检测/攻击沿用欧氏距离；横版朝向只看水平）
        bool found_player = false;
        fp_t nearest_dist_sq = fp_mul(ai.detect_radius, ai.detect_radius);
        fp_t nearest_px = fp_zero();

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
            }
        }

        // 攻击半径判断
        const bool in_attack_range =
            found_player && (nearest_dist_sq < fp_mul(ai.attack_radius, ai.attack_radius));

        // 水平移动意图（横版：只左右）
        fp_t vx = fp_zero();

        if (in_attack_range) {
            // 状态 2：攻击
            ai.state = 2;
            if (act.attack_timer == 0) act.attack_timer = ActionStateComponent::ATTACK_DURATION;
            act.state = ActionStateComponent::ATTACK;

            // 远程敌人：射击冷却（子弹生成暂关，与原型一致）
            if (ai.ranged) {
                if (ai.shoot_cooldown > 0) ai.shoot_cooldown--;
                else ai.shoot_cooldown = ai.shoot_interval;
            }

            if (!registry.all_of<PlayerEventInput>(e)) registry.emplace<PlayerEventInput>(e);
            registry.get<PlayerEventInput>(e).pending_actions.push_back({ 0, 0 });

            act.dir = (nearest_px > pos.x) ? 2 : 1;   // 面向玩家（左右）
        }
        else if (found_player) {
            // 状态 1：追击（横版：水平逼近，直接在本系统移动）
            ai.state = 1;
            ai.timer = 0;
            act.state = act.attack_timer > 0 ? ActionStateComponent::ATTACK
                                             : ActionStateComponent::WALK;
            fp_t dx = fp_sub(nearest_px, pos.x);
            if (dx > fp_from_float(0.1f))       { vx = step;                    act.dir = 2; }
            else if (dx < fp_from_float(-0.1f)) { vx = fp_sub(fp_zero(), step); act.dir = 1; }
        }
        else {
            // 状态 0：巡逻（横版：围绕出生点水平游走）
            ai.state = 0;
            act.state = act.attack_timer > 0 ? ActionStateComponent::ATTACK
                                             : ActionStateComponent::WALK;
            ai.timer--;
            if (ai.timer <= 0) {
                // 纯定点 LCG → 巡逻目标 x（帧同步确定，两端同 seed 同序列 → 行为一致）
                auto& gsref = ctx->ecs.ctx().get<GlobalState>();
                uint32_t r1 = lcg_random_next(gsref.random);
                fp_t fx = fp_sub(fp_mul(fp_from_fraction(r1), fp_from_float(2.0f)), fp_one());
                ai.target_x = fp_add(ai.patrol_center_x, fp_mul(ai.patrol_radius, fx));
                ai.target_y = ai.patrol_center_y;
                ai.timer = 120;
            }
            fp_t dx = fp_sub(ai.target_x, pos.x);
            if (dx > fp_from_float(0.2f))       { vx = step;                    act.dir = 2; }
            else if (dx < fp_from_float(-0.2f)) { vx = fp_sub(fp_zero(), step); act.dir = 1; }
        }

        // ---- 应用移动：水平（带碰撞）+ 重力（竖直带碰撞）----
        if (!registry.all_of<LogicRectComponent>(e)) { pos.x = fp_add(pos.x, vx); continue; }

        const fp_t w = registry.get<LogicRectComponent>(e).width;
        const fp_t h = registry.get<LogicRectComponent>(e).height;

        if (vx != fp_zero()) {
            movement_sweep_x(pos.x, pos.y, vx, w, h);        // 扫掠推进，撞墙则贴齐墙格边界
        }

        if (registry.all_of<LogicVelocityComponent>(e)) {
            fp_t vy = registry.get<LogicVelocityComponent>(e).y;
            // 是否站在地面：脚下 1mm 处被阻挡即为着地
            const bool grounded = movement_box_blocked(pos.x, fp_add(pos.y, EPS), w, h);
            if (grounded && vy > fp_zero()) vy = fp_zero();
            vy = fp_add(vy, fp_mul(gravity, tick));
            if (vy > max_fall) vy = max_fall;
            // 竖直位移 + 碰撞（扫掠推进，落地 / 撞顶时内部速度归零）
            movement_sweep_y(pos.x, pos.y, vy, tick, w, h);
            registry.get<LogicVelocityComponent>(e).y = vy;
        }
    }
}

#endif
