#ifndef ANIMATOR_RENDER_SYSTEM_H
#define ANIMATOR_RENDER_SYSTEM_H
#include "../context.h"
#include <joy/anim.h>

static image_p get_or_load_image(jr2d_p r2d, const char* path) {
    static std::map<std::string, image_p> cache;
    auto it = cache.find(path);
    if (it != cache.end()) return it->second;
    image_p img = image_create(r2d, path);
    if (img) cache[path] = img;
    return img;
}

static void animator_render_system(Context* ctx, entt::registry& registry,
    TransformComponent& t, LogicRectComponent& r,
    anim_t& anim, PlayerActionComponent& act)
{
    (void)registry;
    (void)act;

    if (!ctx->ecs.ctx().contains<anim_def_t>()) return;
    auto& def = ctx->ecs.ctx().get<anim_def_t>();
    if (def.num_states <= 0) return;

    anim_clip_t clip;
    if (!anim_get_current_clip(&anim, &def, &clip)) return;

    image_p img = get_or_load_image(ctx->r2d, clip.path);
    if (!img) {
        jrect_t dr = {
            t.position.x - clip.src_rect.w / 2,
            t.position.y - clip.src_rect.h / 2,
            (float)clip.src_rect.w, (float)clip.src_rect.h
        };
        r2d_set_color(ctx->r2d, 0, 255, 0, 180);
        shape_draw_rectangle(ctx->r2d, "fill", dr);
        return;
    }

    float cx = t.position.x;
    float cy = t.position.y;
    vec2_t pos = { cx, cy };
    vec2_t scale = { anim.flip_h ? -1.0f : 1.0f, 1.0f };
    vec2_t origin = { clip.src_rect.w / 2, clip.src_rect.h / 2 };

    image_draw_ex(img, &clip.src_rect, pos, 0.0f, scale, origin);

    // 绘制碰撞框（调试）
    jrect_t debug_rect = {
        cx - r.width / 2 * 100, cy - r.height / 2 * 100,
        r.width * 100, r.height * 100
    };
    r2d_set_color(ctx->r2d, 255, 0, 0, 100);
    shape_draw_rectangle(ctx->r2d, "line", debug_rect);
}

#endif // ANIMATOR_RENDER_SYSTEM_H
