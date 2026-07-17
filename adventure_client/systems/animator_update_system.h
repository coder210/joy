#pragma once
#include "../components/animator_component.h"
#include "../components/animator_def.h"
#include "../components/player_action_component.h"

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
    case AnimCondMode::IF_TRUE: case AnimCondMode::IF_FALSE: break;
    case AnimCondMode::TRIGGER:
        return params.attack;
    }
    return false;
}

static inline bool check_cond_list(const std::vector<AnimCondDef>& conditions, const AnimParams& params) {
    for (auto& c : conditions)
        if (!check_anim_cond(c, params)) return false;
    return true;
}

static void animator_update_system(entt::registry& registry, entt::entity e,
    Animator& anim, AnimParams& params, PlayerActionComponent& act, float dt) {
    if (anim.def == entt::null) return;
    if (!registry.valid((entt::entity)anim.def)) return;
    const AnimatorDef* def = registry.try_get<AnimatorDef>((entt::entity)anim.def);
    if (!def || def->states.empty()) return;

    anim.flip_h = (params.dir == 1);

    bool transitioned = false;
    for (auto& t : def->transitions) {
        if (t.from_state != -1) continue;
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
    if (!transitioned) {
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

    if (params.attack) params.attack = false;

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

    if (act.state == PlayerActionComponent::ATTACK &&
        state.name.rfind("attack", 0) != 0) {
        act.state = PlayerActionComponent::IDLE;
    }
}
