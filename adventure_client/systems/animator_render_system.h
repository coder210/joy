// animator_render_system.h
#pragma once

#include <unordered_map>
#include "../context.h"
#include "../../adventure_shared/components/player_action_component.h"

// 缓存图片（按路径）
static image_p get_or_load_image(jr2d_p r2d, std::unordered_map<std::string, image_p>& cache, const std::string& path) {
    auto it = cache.find(path);
    if (it != cache.end()) return it->second;
    image_p img = image_create(r2d, path.c_str());
    cache[path] = img;
    return img;
}

inline void animator_render_system(Context* ctx, entt::registry& registry,
    const TransformComponent& t, const LogicRectComponent& rect,
    anim_t& anim, PlayerActionComponent& act) {

    auto& def = registry.ctx().get<anim_def_t>();
    anim_clip_t clip;
    if (!anim_get_current_clip(&anim, &def, &clip)) return;

    // 屏幕坐标（世界坐标 - 相机偏移）* PPM
    float scr_x = (t.position.x - ctx->camera_x) * (float)PIXELS_PER_METER;
    float scr_y = (t.position.y - ctx->camera_y) * (float)PIXELS_PER_METER;

    static std::unordered_map<std::string, image_p> image_cache;
    image_p tex = get_or_load_image(ctx->r2d, image_cache, clip.path);
    if (!tex) return;

    image_draw_ex(tex, &clip.src_rect,
        { scr_x, scr_y }, 0,
        { act.dir < 0 ? -1.0f : 1.0f, 1 },
        { clip.src_rect.w * 0.5f, clip.src_rect.h * 0.5f });
}
