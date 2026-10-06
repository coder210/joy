#ifndef DRAW_BOMB_PASS_H
#define DRAW_BOMB_PASS_H
#include <joy/joy.h>
#include <string>
#include "../components/global_state_component.h"
#include "../components/bomb_component.h"
#include "../components/transform_component.h"
#include "../asset_manager.h"
#include "render_math.h"

struct Context;

// 渲染所有炸弹实体（objects/bomb_sheet 序列帧：off/on/explosion）。
// 纯渲染层：仅推进 BombComponent.anim_time（dt 驱动，不进快照、不参与帧同步），
// 绝不回写任何逻辑组件。
static void draw_bomb_pass(Context* ctx, float dt)
{
    auto& gs = ctx->ecs.ctx().get<GlobalState>();
    const float z = (gs.view_zoom > 0.0f) ? gs.view_zoom : 1.0f;

    // 懒加载三张精灵表（路径稳定，整局不变）
    static image_p g_off = NULL, g_on = NULL, g_exp = NULL;
    auto* am = AssetManager::GetInstance();
    if (am && !g_off) g_off = am->GetImage(ctx->r2d, "l4d_2d/sprites/objects/bomb_sheet/off.png");
    if (am && !g_on)  g_on  = am->GetImage(ctx->r2d, "l4d_2d/sprites/objects/bomb_sheet/on.png");
    if (am && !g_exp) g_exp = am->GetImage(ctx->r2d, "l4d_2d/sprites/objects/bomb_sheet/explosion.png");

    const float TILE = 108.0f;             // 正方形帧边长（与打包一致）
    const float kFPS = 12.0f;              // 与角色动画一致
    const float world_size = 0.7f;         // 炸弹世界尺寸（m）
    const float sizePx = meters_to_pixels(world_size) * z;
    const float scl = sizePx / TILE;

    auto view = ctx->ecs.view<BombComponent, TransformComponent>();
    for (auto e : view) {
        if (!ctx->ecs.valid(e)) continue;
        auto& bomb = view.get<BombComponent>(e);
        auto& t = view.get<TransformComponent>(e);

        image_p img = NULL;
        int frames = 1;
        int frame = 0;
        if (bomb.state == BOMB_ST_OFF)       { img = g_off; frames = 1; }
        else if (bomb.state == BOMB_ST_ON)  { img = g_on;  frames = 10; }
        else                                { img = g_exp; frames = 9; }
        if (!img) continue;

        // 渲染帧推进（仅表现）
        bomb.anim_time += dt;
        int f = (int)(bomb.anim_time * kFPS);
        if (frames > 1) {
            if (bomb.state == BOMB_ST_EXPLODING) frame = (f < frames) ? f : (frames - 1); // 爆炸播到末帧停住
            else frame = f % frames;                                              // 引信循环
        }

        vec2_t c;
        c.x = meters_to_pixels(t.position.x - gs.camera_position.x) * z;
        c.y = meters_to_pixels(t.position.y - gs.camera_position.y) * z;

        jrect_t src = { (float)frame * TILE, 0.0f, TILE, TILE };
        vec2_t pos = c;
        vec2_t scale = { scl, scl };
        vec2_t origin = { TILE * 0.5f, TILE * 0.5f };
        image_draw_ex(img, &src, pos, 0.0f, scale, origin);
    }
}

#endif
