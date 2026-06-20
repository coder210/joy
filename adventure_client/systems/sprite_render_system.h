#pragma once
#include "../flecs.h"
#include "../context.h"
#include "../asset_manager.h"
#include "../components/sprite_sheet_component.h"
#include "../components/player_action_component.h"
#include "../components/transform_component.h"
#include "../components/logic_rect_component.h"

static void sprite_render_system(flecs::entity e,
    TransformComponent& t, LogicRectComponent& r,
    SpriteSheetComponent& ss, AnimationFrameComponent& af,
    PlayerActionComponent& act)
{
    struct context* ctx = (struct context*)e.world().get_ctx();
    float scr_x = (t.position_x - ctx->camera_x) * PIXELS_PER_METER;
    float scr_y = (t.position_y - ctx->camera_y) * PIXELS_PER_METER;

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

    jrect_t dst = { scr_x - off_x, scr_y - off_y, (float)ss.frame_w, (float)ss.frame_h };
    //jsdl_render_texture_rotated(ctx->r2d, tex, &src, &dst, 0, nullptr, flip);
    image_draw_ex(tex, &src, { scr_x - off_x, scr_y - off_y }, 0, { 1, 1 }, { 0, 0 });
}
