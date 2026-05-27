#pragma once
#include "../flecs.h"
#include "../components/sprite_sheet_component.h"
#include "../components/player_action_component.h"

static void animation_update_system(flecs::entity e,
    PlayerActionComponent& act, SpriteSheetComponent& ss, AnimationFrameComponent& af)
{
        float dt = e.world().delta_time();

        // 攻击计时（timer 从 0 累加到 0.6s 后回到 IDLE）
        if (act.state == PlayerActionComponent::ATTACK) {
                act.timer += dt;
                if (act.timer >= 0.6f) { act.state = PlayerActionComponent::IDLE; act.timer = 0; }
        }

        // 根据当前状态选动画行
        int target_row = ss.idle_row;
        if (act.state == PlayerActionComponent::WALK) target_row = ss.walk_row;
        else if (act.state == PlayerActionComponent::ATTACK) target_row = ss.atk_rows[act.dir];

        // 行切换时重置帧
        if (target_row != af.cur_row) { af.cur_row = target_row; af.cur_frame = 0; af.timer = 0; }

        // 帧推进
        af.timer += dt;
        if (af.timer >= ss.frame_duration) {
                af.timer -= ss.frame_duration;
                af.cur_frame = (af.cur_frame + 1) % ss.frame_count;
        }
}
