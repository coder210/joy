#ifndef ANIMATOR_RENDER_SYSTEM_H
#define ANIMATOR_RENDER_SYSTEM_H
#include "../context.h"
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>

static SDL_Texture* get_or_load_texture(SDL_Renderer* renderer, const std::string& path) {
    static std::map<std::string, SDL_Texture*> cache;

    auto it = cache.find(path);
    if (it != cache.end()) return it->second;

    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) return nullptr;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (tex) cache[path] = tex;
    return tex;
}

static void animator_render_system(Context* ctx, entt::registry& registry,
    TransformComponent& t, LogicRectComponent& r,
    Animator& anim, PlayerActionComponent& act)
{
    (void)registry;
    (void)act;

    if (!ctx->ecs.ctx().contains<AnimatorDef>()) return;
    auto& def = ctx->ecs.ctx().get<AnimatorDef>();
    if (def.states.empty()) return;

    int cs = anim.current_state;
    if (cs < 0 || cs >= (int)def.states.size()) cs = def.default_state;
    auto& state = def.states[cs];

    int cf = anim.current_frame;
    if (cf < 0 || cf >= (int)state.clips.size()) cf = 0;
    auto& clip = state.clips[cf];

    SDL_Texture* tex = get_or_load_texture(ctx->renderer, clip.path);
    if (!tex) {
        // fallback: 绘制与原方块等大的纯色框
        float px = t.position.x - clip.src_rect.w / 2;
        float py = t.position.y - clip.src_rect.h / 2;
        SDL_FRect dr = { px, py, (float)clip.src_rect.w, (float)clip.src_rect.h };
        SDL_SetRenderDrawColor(ctx->renderer, 0, 255, 0, 180);
        SDL_RenderFillRect(ctx->renderer, &dr);
        return;
    }

    float scale_x = (anim.flip_h) ? -1.0f : 1.0f;
    SDL_FRect src = {
        (float)clip.src_rect.x, (float)clip.src_rect.y,
        (float)clip.src_rect.w, (float)clip.src_rect.h
    };
    float cx = t.position.x;
    float cy = t.position.y;
    float w = (float)clip.src_rect.w * ctx->global_scale;
    float h = (float)clip.src_rect.h * ctx->global_scale;
    SDL_FRect dst = { cx - w / 2, cy - h / 2, w, h };

    SDL_RenderTextureRotated(ctx->renderer, tex, &src, &dst, 0, nullptr,
        anim.flip_h ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

    // 绘制碰撞框（调试）
    SDL_FRect debug_rect = {
        cx - r.width / 2 * 100, cy - r.height / 2 * 100,
        r.width * 100, r.height * 100
    };
    SDL_SetRenderDrawColor(ctx->renderer, 255, 0, 0, 100);
    SDL_RenderRect(ctx->renderer, &debug_rect);
}

#endif
