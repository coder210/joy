#ifndef ANIMATOR_UPDATE_SYSTEM_H
#define ANIMATOR_UPDATE_SYSTEM_H
#include "../context.h"

static bool check_anim_cond(float a, AnimCondMode mode, float b) {
    switch (mode) {
    case AnimCondMode::GREATER:     return a > b;
    case AnimCondMode::LESS:        return a < b;
    case AnimCondMode::EQUALS:      return a >= b - 0.001f && a <= b + 0.001f;
    case AnimCondMode::NOT_EQUALS:  return a < b - 0.001f || a > b + 0.001f;
    case AnimCondMode::IF_TRUE:     return a != 0;
    case AnimCondMode::IF_FALSE:    return a == 0;
    case AnimCondMode::TRIGGER:     return a != 0;
    default: return false;
    }
}

static bool check_cond_list(const std::vector<AnimCondDef>& conds, AnimParams& params) {
    if (conds.empty()) return true;
    for (auto& c : conds) {
        float val = 0;
        if (c.param_name == "speed")   val = params.speed;
        else if (c.param_name == "dir")     val = (float)params.dir;
        else if (c.param_name == "attack")  val = params.attack ? 1.0f : 0.0f;
        if (!check_anim_cond(val, c.mode, c.threshold))
            return false;
    }
    return true;
}

static void animator_update_system(entt::registry& registry, Context* ctx, float dt) {
    (void)ctx;

    if (!registry.ctx().contains<AnimatorDef>()) return;
    auto& def = registry.ctx().get<AnimatorDef>();

    auto view = registry.view<Animator, AnimParams, PlayerActionComponent>();
    for (auto entity : view) {
        auto& anim = view.get<Animator>(entity);
        auto& params = view.get<AnimParams>(entity);
        auto& act = view.get<PlayerActionComponent>(entity);

        // 方向同步
        anim.flip_h = (params.dir == 1);

        bool transitioned = false;

        // === 第一轮：Any-State 转换（from_state == -1）===
        for (auto& t : def.transitions) {
            if (t.from_state != -1) continue;
            if (anim.state_elapsed < t.exit_time) continue;
            if (!check_cond_list(t.conditions, params)) continue;
            // 防止重复触发攻击：如果当前已在攻击状态，不重触发
            if (anim.current_state >= 2 && anim.current_state <= 5) continue;
            anim.current_state = t.to_state;
            anim.current_frame = 0;
            anim.frame_elapsed = 0;
            anim.state_elapsed = 0;
            anim.just_entered = true;
            transitioned = true;
            break;
        }

        // === 第二轮：当前状态专用转换 ===
        if (!transitioned) {
            for (auto& t : def.transitions) {
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

        // === 消耗 attack trigger ===
        if (params.attack) params.attack = false;

        // === 推进帧 ===
        if (!transitioned) {
            anim.just_entered = false;
            anim.state_elapsed += dt;
        }

        if (anim.current_state >= 0 && anim.current_state < (int)def.states.size()) {
            auto& state = def.states[anim.current_state];
            anim.frame_elapsed += dt * state.speed;
            if (!state.clips.empty()) {
                float frame_dur = state.clips[anim.current_frame].duration;
                while (anim.frame_elapsed >= frame_dur) {
                    anim.frame_elapsed -= frame_dur;
                    if (anim.current_frame + 1 < (int)state.clips.size()) {
                        anim.current_frame++;
                    } else {
                        if (state.loop) {
                            anim.current_frame = 0;
                        }
                        // 非循环状态停留在最后一帧
                        break;
                    }
                    frame_dur = state.clips[anim.current_frame].duration;
                }
            }
        }

        // === 同步 act.state 与动画状态 ===
        if (act.state == PlayerActionComponent::ATTACK) {
            auto& cur_state = def.states[anim.current_state];
            if (cur_state.name.rfind("attack", 0) != 0) {
                act.state = PlayerActionComponent::IDLE;
            }
        }
    }
}

#endif
