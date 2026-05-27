#pragma once
#include "../flecs.h"
#include "../context.h"
#include "../components/sprite_sheet_component.h"
#include "../components/player_action_component.h"
#include "../components/transform_component.h"
#include "../components/logic_rect_component.h"

// 纹理缓存（定义在 game_scene.cpp）
SDL_Texture* tex_cache_get(SDL_Renderer* r, const char* path);
void tex_cache_clear();

static void sprite_render_system(flecs::entity e,
    TransformComponent& t, LogicRectComponent& r,
    SpriteSheetComponent& ss, AnimationFrameComponent& af,
    PlayerActionComponent& act)
{
    struct context* ctx = (struct context*)e.world().get_ctx();
    float scr_x = (t.position_x - ctx->camera_x) * PIXELS_PER_METER;
    float scr_y = (t.position_y - ctx->camera_y) * PIXELS_PER_METER;

    SDL_Texture* tex = tex_cache_get(ctx->renderer, ss.path);
    if (!tex) return;

    SDL_FlipMode flip = (act.dir == 1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    SDL_FRect src = {
        (float)(af.cur_frame * ss.frame_w),
        (float)(af.cur_row * ss.frame_h),
        (float)ss.frame_w, (float)ss.frame_h
    };

    float body_w = fp_to_float(r.width) * PIXELS_PER_METER;
    float body_h = fp_to_float(r.height) * PIXELS_PER_METER;
    float off_x = (ss.frame_w - body_w) / 2.0f;
    float off_y = (ss.frame_h - body_h) / 2.0f;

    SDL_FRect dst = { scr_x - off_x, scr_y - off_y, (float)ss.frame_w, (float)ss.frame_h };
    SDL_RenderTextureRotated(ctx->renderer, tex, &src, &dst, 0, nullptr, flip);
}
