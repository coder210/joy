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

    const float TILE = 108.0f;                 // 正方形帧边长（与打包一致）
    const float kFPS = 12.0f;                  // 与角色动画一致
    const float RENDER_PPM = 50.0f;            // 与玩家精灵同标尺（玩家用硬编码 50 PPM @z=1）；
                                              // 不沿用 meters_to_pixels（那是地图 PPM，会让炸弹缩到约 1/4）
    const float bomb_world    = 1.3f;          // 炸弹本体世界尺寸（m），明显大于碰撞盒、比玩家醒目
    const float explode_world = BOMB_RADIUS * 2.0f;  // 爆炸覆盖伤圈直径（2.5→5.0m），显著放大

    auto view = ctx->ecs.view<BombComponent, TransformComponent>();
    for (auto e : view) {
        if (!ctx->ecs.valid(e)) continue;
        auto& bomb = view.get<BombComponent>(e);
        auto& t = view.get<TransformComponent>(e);

        image_p img = NULL;
        int frames = 1;
        if (bomb.state == BOMB_ST_OFF)       { img = g_off; frames = 1; }
        else if (bomb.state == BOMB_ST_ON)  { img = g_on;  frames = 10; }
        else                                { img = g_exp; frames = 9; }
        if (!img) continue;

        // 尺寸：爆炸态按伤圈直径放大，其余按炸弹本体
        const float world_size = (bomb.state == BOMB_ST_EXPLODING) ? explode_world : bomb_world;
        const float sizePx = world_size * RENDER_PPM * z;   // 用玩家同标尺，避免被地图 PPM 缩小
        const float scl = sizePx / TILE;

        // 渲染帧推进（仅表现）
        bomb.anim_time += dt;
        int frame = 0;
        if (bomb.state == BOMB_ST_EXPLODING) {
            // 由 explode_ticks 推导进度（0→1）：避免 anim_time 跨 OFF/ON 累积导致爆炸冻结在末帧
            float prog = 1.0f - (float)bomb.explode_ticks / (float)BOMB_EXPLODE_TICKS;
            if (prog < 0.0f) prog = 0.0f; else if (prog > 1.0f) prog = 1.0f;
            frame = (int)(prog * (frames - 1) + 0.5f);
            if (frame > frames - 1) frame = frames - 1;
        } else if (frames > 1) {
            frame = ((int)(bomb.anim_time * kFPS)) % frames;   // 引信态循环闪烁
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
