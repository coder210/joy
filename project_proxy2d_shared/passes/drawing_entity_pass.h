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
    if (hc.dead) return;

    float ratio = (float)hc.hp / (float)hc.max_hp;
    if (ratio > 1.0f) ratio = 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;

    const float R    = ring_R;
    const int   segs = 24;
    const float a0   = -GD_PI * 0.5f;

    jcolor_t dim = is_player ? jcolor_t{ 60, 80, 150, 70 } : jcolor_t{ 150, 120, 50, 70 };
    r2d_set_color(ctx->r2d, dim.r, dim.g, dim.b, dim.a);
    for (int i = 0; i < segs; i++) {
        float a1 = a0 + (float)i * (2.0f * GD_PI / segs);
        float a2 = a0 + (float)(i + 1) * (2.0f * GD_PI / segs);
        vec2_t p1 = { c.x + cosf(a1) * R, c.y + sinf(a1) * R };
        vec2_t p2 = { c.x + cosf(a2) * R, c.y + sinf(a2) * R };
        shape_draw_line(ctx->r2d, p1.x, p1.y, p2.x, p2.y);
    }

    if (ratio <= 0.0f) return;

    jcolor_t col;
    if      (ratio > 0.50f) col = is_player ? jcolor_t{ 90, 140, 255, 255 } : jcolor_t{ 255, 200, 80, 255 };
    else if (ratio > 0.25f) col = jcolor_t{ 240, 200, 60, 255 };
    else                    col = jcolor_t{ 230, 60, 50, 255 };
    if (ratio < 0.25f) {
        float pulse = 0.55f + 0.45f * sinf(ctx->visual_time * 6.0f);
        col.a = (uint8_t)(255 * pulse);
    }
    r2d_set_color(ctx->r2d, col.r, col.g, col.b, col.a);

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
    float cx = gs.camera_position.x * SERVER_PPM;
    float cy = gs.camera_position.y * SERVER_PPM;

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
    c.x = (t.position.x - cam.x) * SERVER_PPM;
    c.y = (t.position.y - cam.y) * SERVER_PPM;

    if (is_bullet) {
        auto& bc = ctx->ecs.get<BulletComponent>(e);
        bool is_enemy_bullet = (bc.owner_conv == 0);
        jcolor_t glow, body, edge;
        if (is_enemy_bullet) {
            glow = jcolor_t{ 120, 80, 40, 80 };
            body = jcolor_t{ 90, 55, 30, 255 };
            edge = jcolor_t{ 255, 230, 200, 255 };
        } else {
            glow = jcolor_t{ 50, 80, 110, 80 };
            body = jcolor_t{ 35, 55, 80, 255 };
            edge = jcolor_t{ 210, 235, 255, 255 };
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

    float w = fp_to_float(rect.width);
    float h = fp_to_float(rect.height);
    float half = (w > h ? w : h) * 0.5f;
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
        r2d_set_color(ctx->r2d, is_player ? 90 : 255, is_player ? 140 : 200, is_player ? 255 : 80, 90);
        shape_draw_line(ctx->r2d, r1.x - 2.0f, r1.y - 2.0f, r2.x - 2.0f, r2.y - 2.0f);
        r2d_set_color(ctx->r2d, is_player ? 45 : 155, is_player ? 65 : 115, is_player ? 140 : 30, 230);
        shape_draw_line(ctx->r2d, r1.x, r1.y, r2.x, r2.y);
    }

    bool player_dead = is_player &&
        ctx->ecs.all_of<HealthComponent>(e) && ctx->ecs.get<HealthComponent>(e).dead;
    if (is_player) {
        jcolor_t fill   = player_dead ? jcolor_t{ 168, 170, 175, 200 } : jcolor_t{ 255, 255, 255, 255 };
        jcolor_t stroke = player_dead ? jcolor_t{ 92, 94, 100, 255 }   : jcolor_t{ 30, 30, 35, 255 };
        draw_rotated_square(ctx, c, half, rot, fill, stroke);
    } else {
        jcolor_t fill   = { 22, 22, 26, 255 };
        jcolor_t stroke = { 255, 255, 255, 255 };
        draw_rotated_square(ctx, c, half, rot, fill, stroke);
    }

    r2d_set_color(ctx->r2d, player_dead ? 120 : (is_player ? 90 : 255),
        player_dead ? 120 : (is_player ? 140 : 200),
        player_dead ? 125 : (is_player ? 255 : 80), player_dead ? 140 : 220);
    vec2_t tip = { c.x + cosf(dir_rad) * half * 0.8f, c.y + sinf(dir_rad) * half * 0.8f };
    shape_draw_line(ctx->r2d, c.x, c.y, tip.x, tip.y);

    if (ctx->ecs.all_of<HealthComponent>(e)) {
        auto& hc = ctx->ecs.get<HealthComponent>(e);
        float ring_R = fp_to_float(rect.width) * SERVER_PPM * 0.5f;
        draw_hp_ring(ctx, c, ring_R, hc, is_player);
    }
}

#endif