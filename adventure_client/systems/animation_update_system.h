#pragma once
#include "../components/sprite_sheet_component.h"
#include "../components/player_action_component.h"

static void animation_update_system(PlayerActionComponent& act,
    SpriteSheetComponent& ss, AnimationFrameComponent& af, float dt)
{
        if (act.state == PlayerActionComponent::ATTACK) {
                act.timer += dt;
                if (act.timer >= 0.6f) { act.state = PlayerActionComponent::IDLE; act.timer = 0; }
        }

        int target_row = ss.idle_row;
        if (act.state == PlayerActionComponent::WALK) target_row = ss.walk_row;
        else if (act.state == PlayerActionComponent::ATTACK) target_row = ss.atk_rows[act.dir];

        if (target_row != af.cur_row) { af.cur_row = target_row; af.cur_frame = 0; af.timer = 0; }

        af.timer += dt;
        if (af.timer >= ss.frame_duration) {
                af.timer -= ss.frame_duration;
                af.cur_frame = (af.cur_frame + 1) % ss.frame_count;
        }
}
