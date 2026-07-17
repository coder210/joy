#pragma once
#include "../context.h"
#include "../asset_manager.h"
#include "../../adventure_shared/protocol.h"
#include "../components/sprite_sheet_component.h"
#include "../components/player_action_component.h"
#include "../components/transform_component.h"
#include "../components/logic_rect_component.h"

static void sprite_render_system(Context* ctx,
    TransformComponent& t, LogicRectComponent& r,
    SpriteSheetComponent& ss, AnimationFrameComponent& af,
    PlayerActionComponent& act)
{
    float scr_x = (t.position.x - ctx->camera_x) * PIXELS_PER_METER;
    float scr_y = (t.position.y - ctx->camera_y) * PIXELS_PER_METER;

    image_p tex = AssetManager::GetInstance()->tex_cache_get(ctx->r2d, ss.path);
    if (!tex) return;

    jrect_t src = {
        (float)(af.cur_frame * ss.frame_w),
        (float)(af.cur_row * ss.frame_h),
        (float)ss.frame_w, (float)ss.frame_h
    };

    float body_w = fp_to_float(r.width) * PIXELS_PER_METER;
    float body_h = fp_to_float(r.height) * PIXELS_PER_METER;
    float off_x = (ss.frame_w - body_w) / 2.0f;
    float off_y = (ss.frame_h - body_h) / 2.0f;

    float draw_x = scr_x - off_x;
    float draw_y = scr_y - off_y;
    float scale_x = (act.dir == 1) ? -1.0f : 1.0f;
    float cx = draw_x + ss.frame_w * 0.5f;
    float cy = draw_y + ss.frame_h * 0.5f;
    image_draw_ex(tex, &src, { cx, cy }, 0, { scale_x, 1 }, { ss.frame_w * 0.5f, ss.frame_h * 0.5f });
}
