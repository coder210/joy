#pragma once
#include "../context.h"
#include "../asset_manager.h"
#include "../../adventure_shared/protocol.h"
#include "../components/animator_component.h"
#include "../components/animator_def.h"
#include "../components/transform_component.h"
#include "../components/logic_rect_component.h"
#include "../components/player_action_component.h"

static void animator_render_system(Context* ctx, entt::registry& registry,
    TransformComponent& t, LogicRectComponent& r,
    Animator& anim, PlayerActionComponent& act)
{
    if (anim.def == entt::null) return;
    if (!registry.valid((entt::entity)anim.def)) return;
    const AnimatorDef* def = registry.try_get<AnimatorDef>((entt::entity)anim.def);
    if (!def) return;
    if (anim.current_state < 0 || anim.current_state >= (int)def->states.size()) return;

    const auto& state = def->states[anim.current_state];
    if (anim.current_frame < 0 || anim.current_frame >= (int)state.clips.size()) return;
    const auto& clip = state.clips[anim.current_frame];

    image_p tex = AssetManager::GetInstance()->tex_cache_get(ctx->r2d, clip.path.c_str());
    if (!tex) return;

    auto& cam = ctx->ecs.get<CameraComponent>(ctx->singleton_camera).position;
    float scr_x = (t.position.x - cam.x) * SERVER_PPM;
    float scr_y = (t.position.y - cam.y) * SERVER_PPM;

    int frame_w = clip.src_rect.w;
    int frame_h = clip.src_rect.h;
    float body_w = fp_to_float(r.width) * SERVER_PPM;
    float body_h = fp_to_float(r.height) * SERVER_PPM;
    float off_x = (frame_w - body_w) / 2.0f;
    float off_y = (frame_h - body_h) / 2.0f;

    float draw_x = scr_x - off_x;
    float draw_y = scr_y - off_y;
    float scale_x = (anim.flip_h) ? -1.0f : 1.0f;
    float cx = draw_x + frame_w * 0.5f;
    float cy = draw_y + frame_h * 0.5f;

    image_draw_ex(tex, &clip.src_rect, { cx, cy }, 0, { scale_x, 1 }, { frame_w * 0.5f, frame_h * 0.5f });
}
