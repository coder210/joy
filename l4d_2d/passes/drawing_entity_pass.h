#ifndef DRAWING_ENTITY_PASS_H
#define DRAWING_ENTITY_PASS_H
#include <math.h>
#include <unordered_map>
#include <joy/joy.h>
#include "../components/global_state_component.h"
#include "../components/id_component.h"
#include "../components/player_component.h"
#include "../components/enemy_component.h"
#include "../components/bullet_component.h"
#include "../components/logic_rect_component.h"
#include "../components/transform_component.h"
#include "../components/action_state_component.h"
#include "../components/health_component.h"
#include "../protocol.h"

struct Context;

static const float GD_PI         = 3.14159265f;
static const float GD_GRID_STEP  = 50.0f;
static const float GD_RING_IN    = 36.0f;
static const float GD_RING_OUT   = 48.0f;
static const int   GD_RING_POINTS = 36;

static void draw_rotated_square(Context* ctx, vec2_t c, float half, float rot,
        jcolor_t fill, jcolor_t stroke)
{
    vec2_t pts[4];
    for (int i = 0; i < 4; i++) {
        float a = rot + i * (GD_PI * 0.5f);
        pts[i].x = c.x + cosf(a) * half;
        pts[i].y = c.y + sinf(a) * half;
    }
    shape_draw_polygon(ctx->r2d, "fill", pts, 4, fill);
    shape_draw_polygon(ctx->r2d, "line", pts, 4, stroke);
}

static void draw_hp_ring(Context* ctx, vec2_t c, float ring_R, const HealthComponent& hc,
        bool is_player)
{
    (void)is_player;
    if (hc.dead) return;

    float ratio = (float)hc.hp / (float)hc.max_hp;
    if (ratio > 1.0f) ratio = 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;

    const float R    = ring_R;
    const int   segs = 24;
    const float a0   = -GD_PI * 0.5f;

    // 背景环：淡墨（浅灰）空血轮廓，对齐 3D 水墨画风
    jcolor_t dim = jcolor_t{ 170, 170, 178, 200 };
    r2d_set_color(ctx->r2d, dim.r, dim.g, dim.b, dim.a);
    for (int i = 0; i < segs; i++) {
        float a1 = a0 + (float)i * (2.0f * GD_PI / segs);
        float a2 = a0 + (float)(i + 1) * (2.0f * GD_PI / segs);
        vec2_t p1 = { c.x + cosf(a1) * R, c.y + sinf(a1) * R };
        vec2_t p2 = { c.x + cosf(a2) * R, c.y + sinf(a2) * R };
        shape_draw_line(ctx->r2d, p1.x, p1.y, p2.x, p2.y);
    }

    if (ratio <= 0.0f) return;

    // 前景血量环：墨色为主，低血量改朱砂红警示（对齐 3D health_ring_pass）
    uint8_t cr, cg, cb;
    if (ratio > 0.3f) { cr = 40;  cg = 40;  cb = 45; }   // 墨色
    else              { cr = 200; cg = 60;  cb = 50; }   // 低血量朱砂红
    if (ratio <= 0.3f) {
        float pulse = 0.55f + 0.45f * sinf(ctx->visual_time * 6.0f);
        r2d_set_color(ctx->r2d, cr, cg, cb, (uint8_t)(255 * pulse));
    } else {
        r2d_set_color(ctx->r2d, cr, cg, cb, 255);
    }

    float sweep = ratio * (2.0f * GD_PI);
    int   n     = (int)ceilf(sweep / (2.0f * GD_PI) * segs);
    for (int i = 0; i < n; i++) {
        float a1 = a0 + (float)i * (2.0f * GD_PI / segs);
        float a2 = a0 + (float)(i + 1) * (2.0f * GD_PI / segs);
        if (a2 - a0 > sweep) a2 = a0 + sweep;
        vec2_t p1 = { c.x + cosf(a1) * R, c.y + sinf(a1) * R };
        vec2_t p2 = { c.x + cosf(a2) * R, c.y + sinf(a2) * R };
        shape_draw_line(ctx->r2d, p1.x, p1.y, p2.x, p2.y);
    }
}

static void draw_grid_background(Context* ctx)
{
    auto& gs = ctx->ecs.ctx().get<GlobalState>();
    float cx = gs.camera_position.x * PIXELS_PER_METER;
    float cy = gs.camera_position.y * PIXELS_PER_METER;

    r2d_set_color(ctx->r2d, 255, 255, 255, 255);
    jrect_t bg = { 0, 0, 480, 320 };
    shape_draw_rectangle(ctx->r2d, "fill", bg);

    r2d_set_color(ctx->r2d, 224, 224, 224, 76);
    float offx = fmodf(-cx, GD_GRID_STEP);
    if (offx > 0.0f) offx -= GD_GRID_STEP;
    for (float gx = offx; gx <= 480.0f; gx += GD_GRID_STEP)
        shape_draw_line(ctx->r2d, gx, 0.0f, gx, 320.0f);
    float offy = fmodf(-cy, GD_GRID_STEP);
    if (offy > 0.0f) offy -= GD_GRID_STEP;
    for (float gy = offy; gy <= 320.0f; gy += GD_GRID_STEP)
        shape_draw_line(ctx->r2d, 0.0f, gy, 320.0f, gy);
}

static void drawing_entity_pass(Context* ctx, entt::entity e,
        IdComponent& id, LogicRectComponent& rect, TransformComponent& t)
{
    (void)id;
    if (!ctx->ecs.valid(e)) return;

    bool is_player = ctx->ecs.all_of<PlayerComponent>(e);
    bool is_enemy  = ctx->ecs.all_of<EnemyComponent>(e);
    bool is_bullet = ctx->ecs.all_of<BulletComponent>(e);
    if (!is_player && !is_enemy && !is_bullet) return;

    auto& cam = ctx->ecs.ctx().get<GlobalState>().camera_position;
    vec2_t c;
    c.x = (t.position.x - cam.x) * PIXELS_PER_METER;
    c.y = (t.position.y - cam.y) * PIXELS_PER_METER;

    if (is_bullet) {
        auto& bc = ctx->ecs.get<BulletComponent>(e);
        bool is_enemy_bullet = (bc.owner_conv == 0);
        jcolor_t glow, body, edge;
        if (is_enemy_bullet) {
            /* 敌人子弹：朱砂红墨（对齐 3D 敌人子弹 235/60/50） */
            glow = jcolor_t{ 120, 40, 34, 80 };
            body = jcolor_t{ 235, 60, 50, 255 };
            edge = jcolor_t{ 200, 30, 25, 255 };
        } else {
            /* 玩家子弹：青绿水墨（对齐 3D 玩家子弹 60/220/230） */
            glow = jcolor_t{ 40, 90, 100, 80 };
            body = jcolor_t{ 60, 220, 230, 255 };
            edge = jcolor_t{ 30, 180, 200, 255 };
        }
        float bs = 7.0f;
        float bspin = ctx->visual_time * 6.0f;
        vec2_t pts[4];
        for (int i = 0; i < 4; i++) {
            float a = bspin + i * (GD_PI * 0.5f);
            pts[i].x = c.x + cosf(a) * bs;
            pts[i].y = c.y + sinf(a) * bs;
        }
        shape_draw_polygon(ctx->r2d, "fill", pts, 4, glow);
        for (int i = 0; i < 4; i++) {
            float a = bspin + i * (GD_PI * 0.5f);
            pts[i].x = c.x + cosf(a) * bs * 0.6f;
            pts[i].y = c.y + sinf(a) * bs * 0.6f;
        }
        shape_draw_polygon(ctx->r2d, "fill", pts, 4, body);
        shape_draw_polygon(ctx->r2d, "line", pts, 4, edge);
        return;
    }

    // 视觉角色半径：紧凑固定值(约 0.3m)，避免按碰撞盒全尺寸画出遮挡地图的巨大方块；
    // 真实碰撞范围由 drawing_collision_debug_pass 的红/绿线框单独标出。
    float half = 0.30f * PIXELS_PER_METER;
    if (half < 8.0f) half = 8.0f;

    float dir_rad = -GD_PI * 0.5f;
    bool is_attack = false;
    if (ctx->ecs.all_of<ActionStateComponent>(e)) {
        auto& ac = ctx->ecs.get<ActionStateComponent>(e);
        if (ac.state == ActionStateComponent::ATTACK) is_attack = true;
        switch (ac.dir) {
        case 0: dir_rad =  GD_PI * 0.5f; break;
        case 1: dir_rad =  GD_PI;        break;
        case 2: dir_rad =  0.0f;         break;
        case 3: dir_rad = -GD_PI * 0.5f; break;
        }
    }

    float speed_ratio = 5.0f;
    float spin = 0.7f + speed_ratio * 1.8f;
    float rot  = 0.785398f + ctx->visual_time * spin;

    if (is_attack) {
        float ray_len = half * 3.2f;
        vec2_t r1 = { c.x + cosf(dir_rad) * half * 0.9f, c.y + sinf(dir_rad) * half * 0.9f };
        vec2_t r2 = { c.x + cosf(dir_rad) * ray_len,     c.y + sinf(dir_rad) * ray_len };
        // 挥砍路径：玩家青石墨、敌人朱砂红墨（对齐 3D 子弹配色）
        int gl = is_player ? 40 : 120, gg = is_player ? 90 : 40, gb = is_player ? 100 : 34;
        r2d_set_color(ctx->r2d, gl, gg, gb, 90);
        shape_draw_line(ctx->r2d, r1.x - 2.0f, r1.y - 2.0f, r2.x - 2.0f, r2.y - 2.0f);
        int hl = is_player ? 60 : 235, hg = is_player ? 220 : 60, hb = is_player ? 230 : 50;
        r2d_set_color(ctx->r2d, hl, hg, hb, 230);
        shape_draw_line(ctx->r2d, r1.x, r1.y, r2.x, r2.y);
    }

    bool player_dead = is_player &&
        ctx->ecs.all_of<HealthComponent>(e) && ctx->ecs.get<HealthComponent>(e).dead;
    if (is_player) {
        /* 玩家：白底墨边（图纸风，对齐 3D 主角白底模型） */
        jcolor_t fill   = player_dead ? jcolor_t{ 168, 170, 175, 200 } : jcolor_t{ 255, 255, 255, 255 };
        jcolor_t stroke = player_dead ? jcolor_t{ 92, 94, 100, 255 }   : jcolor_t{ 30, 30, 35, 255 };
        draw_rotated_square(ctx, c, half, rot, fill, stroke);
    } else {
        /* 敌人：墨色填充 + 淡墨描边（对齐 3D 敌人墨色实体） */
        jcolor_t fill   = { 40, 40, 45, 255 };
        jcolor_t stroke = { 120, 120, 128, 255 };
        draw_rotated_square(ctx, c, half, rot, fill, stroke);
    }

    // 朝向指示：玩家青绿、敌人淡朱砂红
    r2d_set_color(ctx->r2d, is_player ? 60 : 200, is_player ? 220 : 60, is_player ? 230 : 50,
        player_dead ? 140 : 220);
    vec2_t tip = { c.x + cosf(dir_rad) * half * 0.8f, c.y + sinf(dir_rad) * half * 0.8f };
    shape_draw_line(ctx->r2d, c.x, c.y, tip.x, tip.y);

    if (ctx->ecs.all_of<HealthComponent>(e)) {
        auto& hc = ctx->ecs.get<HealthComponent>(e);
        float ring_R = fp_to_float(rect.width) * PIXELS_PER_METER * 0.25f;  // 玩家/敌人血环半径（减半）
        draw_hp_ring(ctx, c, ring_R, hc, is_player);
    }
}

#endif