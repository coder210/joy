#pragma once
#include "../flecs.h"
#include "../components/animator_component.h"
#include "../components/animator_def.h"
#include "../components/player_action_component.h"

// 检查单个条件
static inline bool check_anim_cond(const AnimCondDef& cond, const AnimParams& params) {
    switch (cond.mode) {
    case AnimCondMode::GREATER:
        if (cond.param_name == "speed") return params.speed > cond.threshold;
        if (cond.param_name == "dir")   return (float)params.dir > cond.threshold;
        break;
    case AnimCondMode::LESS:
        if (cond.param_name == "speed") return params.speed < cond.threshold;
        if (cond.param_name == "dir")   return (float)params.dir < cond.threshold;
        break;
    case AnimCondMode::EQUALS:
        if (cond.param_name == "speed") return fabsf(params.speed - cond.threshold) < 0.001f;
        if (cond.param_name == "dir")   return (float)params.dir == cond.threshold;
        break;
    case AnimCondMode::NOT_EQUALS:
        if (cond.param_name == "speed") return fabsf(params.speed - cond.threshold) >= 0.001f;
        if (cond.param_name == "dir")   return (float)params.dir != cond.threshold;
        break;
    case AnimCondMode::IF_TRUE:
        break;
    case AnimCondMode::IF_FALSE:
        break;
    case AnimCondMode::TRIGGER:
        return params.attack; // trigger 已设置
    }
    return false;
}

// 检查一组条件（全部满足才通过）
static inline bool check_cond_list(const std::vector<AnimCondDef>& conditions, const AnimParams& params) {
    for (auto& c : conditions)
        if (!check_anim_cond(c, params)) return false;
    return true;
}

// 动画状态机更新系统
static void animator_update_system(flecs::entity e, Animator& anim, AnimParams& params, PlayerActionComponent& act) {
    // 获取定义
    flecs::entity def_entity = e.world().get_alive(anim.def);
    if (!def_entity.is_valid()) return;
    const AnimatorDef* def = def_entity.get<AnimatorDef>();
    if (!def || def->states.empty()) return;

    float dt = (float)e.world().delta_time();

    // --- 1. 检查过渡 ---
    bool transitioned = false;
    // 先检查 Any State 过渡（from_state == -1）
    for (auto& t : def->transitions) {
        if (t.from_state != -1) continue;
        if (anim.state_elapsed < t.exit_time) continue; // exit time 未到
        if (!check_cond_list(t.conditions, params)) continue;
        // 过渡触发
        anim.current_state = t.to_state;
        anim.current_frame = 0;
        anim.frame_elapsed = 0;
        anim.state_elapsed = 0;
        anim.just_entered = true;
        transitioned = true;
        break;
    }
    if (!transitioned) {
        // 再检查从当前状态出发的过渡
        for (auto& t : def->transitions) {
            if (t.from_state != anim.current_state) continue;
            if (anim.state_elapsed < t.exit_time) continue;
            if (!check_cond_list(t.conditions, params)) continue;
            anim.current_state = t.to_state;
            anim.current_frame = 0;
            anim.frame_elapsed = 0;
            anim.state_elapsed = 0;
            anim.just_entered = true;
            transitioned = true;
            break;
        }
    }

    // --- 2. 消耗 trigger 参数 ---
    if (params.attack) params.attack = false;

    // --- 3. 帧推进 ---
    if (!transitioned) {
        anim.just_entered = false;
        anim.state_elapsed += dt;
    }

    const auto& state = def->states[anim.current_state];
    if (state.clips.empty()) return;

    anim.frame_elapsed += dt * state.speed;
    float clip_dur = state.clips[anim.current_frame].duration;

    while (anim.frame_elapsed >= clip_dur && clip_dur > 0) {
        anim.frame_elapsed -= clip_dur;
        if (anim.current_frame + 1 < (int)state.clips.size()) {
            anim.current_frame++;
            clip_dur = state.clips[anim.current_frame].duration;
        } else {
            if (state.loop) {
                anim.current_frame = 0;
                clip_dur = state.clips[0].duration;
            } else {
                anim.current_frame = (int)state.clips.size() - 1;
                anim.frame_elapsed = 0;
                break;
            }
        }
    }

    // --- 4. 同步回 PlayerActionComponent ---
    // 如果动画机已离开攻击状态，但 act.state 仍卡在 ATTACK，则复位
    if (act.state == PlayerActionComponent::ATTACK &&
        state.name.rfind("attack", 0) != 0) {
        act.state = PlayerActionComponent::IDLE;
    }
}
