#pragma once
#include "../flecs.h"
#include "../context.h"
#include "../asset_manager.h"
#include "../components/animator_component.h"
#include "../components/animator_def.h"
#include "../components/transform_component.h"
#include "../components/logic_rect_component.h"
#include "../components/player_action_component.h"

// 动画渲染系统
static void animator_render_system(flecs::entity e,
    TransformComponent& t, LogicRectComponent& r,
    Animator& anim, PlayerActionComponent& act)
{
    struct context* ctx = (struct context*)e.world().get_ctx();

    // 获取定义
    flecs::entity def_entity = e.world().get_alive(anim.def);
    if (!def_entity.is_valid()) return;
    const AnimatorDef* def = def_entity.get<AnimatorDef>();
    if (!def) return;
    if (anim.current_state < 0 || anim.current_state >= (int)def->states.size()) return;

    const auto& state = def->states[anim.current_state];
    if (anim.current_frame < 0 || anim.current_frame >= (int)state.clips.size()) return;
    const auto& clip = state.clips[anim.current_frame];

    // 加载纹理
    image_p tex = AssetManager::GetInstance()->tex_cache_get(ctx->r2d, clip.path.c_str());
    if (!tex) return;

    // 屏幕坐标
    float scr_x = (t.position.x - ctx->camera_x) * PIXELS_PER_METER;
    float scr_y = (t.position.y - ctx->camera_y) * PIXELS_PER_METER;

    // 精灵对齐偏移
    int frame_w = clip.src_rect.w;
    int frame_h = clip.src_rect.h;
    float body_w = fp_to_float(r.width) * PIXELS_PER_METER;
    float body_h = fp_to_float(r.height) * PIXELS_PER_METER;
    float off_x = (frame_w - body_w) / 2.0f;
    float off_y = (frame_h - body_h) / 2.0f;

    float draw_x = scr_x - off_x;
    float draw_y = scr_y - off_y;
    float scale_x = (anim.flip_h) ? -1.0f : 1.0f;
    float cx = draw_x + frame_w * 0.5f;
    float cy = draw_y + frame_h * 0.5f;

    image_draw_ex(tex, &clip.src_rect, { cx, cy }, 0, { scale_x, 1 }, { frame_w * 0.5f, frame_h * 0.5f });
}
