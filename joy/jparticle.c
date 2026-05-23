/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jparticle.c
Description: 2D 粒子系统实现
Author: ydlc
Version: 1.0
Date: 2024.7.15
History:
*************************************************/
#include "jparticle.h"
#include "external/klib/kvec.h"
#include <stdlib.h>
#include <math.h>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ======================== 内部粒子结构 ======================== */
typedef struct {
    float x, y;
    float vx, vy;
    float life;       /* 剩余生命 */
    float max_life;   /* 初始生命(用于插值) */
    float size;
    float r, g, b, a;
} particle_inner_t;

/* 发射器 */
struct jparticle_t {
    jparticle_cfg_t cfg;
    float emit_acc;         /* 持续发射累积时间 */
    kvec_t(particle_inner_t) particles;
};

/* ======================== 工具函数 ======================== */
static float randf(float min, float max) {
    return min + (float)rand() / (float)RAND_MAX * (max - min);
}

static float deg2rad(float deg) {
    return deg * (float)M_PI / 180.0f;
}

/* ======================== 默认配置 ======================== */
jparticle_cfg_t jparticle_cfg_default(void) {
    jparticle_cfg_t cfg;
    cfg.x = 0; cfg.y = 0;
    cfg.spawn_radius = 0;
    cfg.direction = -90;   /* 默认向上 */
    cfg.spread = 30;
    cfg.speed_min = 50;  cfg.speed_max = 100;
    cfg.lifetime_min = 0.5f; cfg.lifetime_max = 1.5f;
    cfg.size_start = 4; cfg.size_end = 1;
    cfg.r = 255; cfg.g = 255; cfg.b = 255; cfg.a = 255;
    cfg.fadeout = 0.8f;
    cfg.burst = 20;
    cfg.rate = 0;
    cfg.gravity = false;
    cfg.gravity_y = 50.0f;
    return cfg;
}

/* ======================== 创建/销毁 ======================== */
jparticle_p jparticle_create(jparticle_cfg_t cfg) {
    jparticle_p p = (jparticle_p)SDL_malloc(sizeof(jparticle_t));
    if (!p) return NULL;
    p->cfg = cfg;
    p->emit_acc = 0;
    kv_init(p->particles);
    return p;
}

void jparticle_destroy(jparticle_p p) {
    if (!p) return;
    kv_destroy(p->particles);
    SDL_free(p);
}

/* ======================== 控制 ======================== */
void jparticle_set_position(jparticle_p p, float x, float y) {
    if (!p) return;
    p->cfg.x = x;
    p->cfg.y = y;
}

void jparticle_clear(jparticle_p p) {
    if (!p) return;
    p->particles.n = 0;
    p->emit_acc = 0;
}

/* ======================== 发射粒子 ======================== */
void jparticle_emit(jparticle_p p) {
    if (!p) return;
    jparticle_cfg_t* c = &p->cfg;
    int n = c->burst > 0 ? c->burst : 1;
    for (int i = 0; i < n; i++) {
        particle_inner_t pt = {0};
        /* 位置 */
        float angle = randf(0, (float)M_PI * 2);
        float r = randf(0, c->spawn_radius);
        pt.x = c->x + cosf(angle) * r;
        pt.y = c->y + sinf(angle) * r;
        /* 速度方向 */
        float dir = deg2rad(c->direction + randf(-c->spread, c->spread));
        float speed = randf(c->speed_min, c->speed_max);
        pt.vx = cosf(dir) * speed;
        pt.vy = sinf(dir) * speed;
        /* 生命周期 */
        pt.max_life = randf(c->lifetime_min, c->lifetime_max);
        pt.life = pt.max_life;
        /* 大小 */
        pt.size = randf(c->size_start, c->size_end > c->size_start
                        ? c->size_end : c->size_start);
        /* 颜色 */
        pt.r = c->r;
        pt.g = c->g;
        pt.b = c->b;
        pt.a = c->a;
        kv_push(particle_inner_t, p->particles, pt);
    }
}

/* ======================== 更新 ======================== */
void jparticle_update(jparticle_p p, float dt) {
    if (!p) return;
    jparticle_cfg_t* c = &p->cfg;

    /* 持续发射 */
    if (c->rate > 0) {
        p->emit_acc += dt;
        float interval = 1.0f / c->rate;
        while (p->emit_acc >= interval) {
            p->emit_acc -= interval;
            jparticle_emit(p);
        }
    }

    /* 更新每个粒子 */
    kvec_t(particle_inner_t)* parts = &p->particles;
    size_t w = 0;
    for (size_t i = 0; i < parts->n; i++) {
        particle_inner_t* pt = &kv_A(*parts, i);
        pt->life -= dt;
        if (pt->life <= 0) continue; /* 死亡 */

        /* 移动 */
        pt->x += pt->vx * dt;
        pt->y += pt->vy * dt;

        /* 重力 */
        if (c->gravity) pt->vy += c->gravity_y * dt;

        /* 淡出 alpha */
        float life_ratio = pt->life / pt->max_life;
        if (life_ratio < c->fadeout) {
            pt->a = c->a * (life_ratio / c->fadeout);
        }

        /* 缩小 */
        float t = 1.0f - life_ratio;
        pt->size = c->size_start + (c->size_end - c->size_start) * t;

        /* 保留 */
        if (w != i) kv_A(*parts, w) = kv_A(*parts, i);
        w++;
    }
    parts->n = w;
}

/* ======================== 渲染 ======================== */
void jparticle_render(jparticle_p p, SDL_Renderer* renderer) {
    if (!p || !renderer) return;
    kvec_t(particle_inner_t)* parts = &p->particles;
    for (size_t i = 0; i < parts->n; i++) {
        particle_inner_t* pt = &kv_A(*parts, i);
        float half = pt->size * 0.5f;
        SDL_FRect r = { pt->x - half, pt->y - half, pt->size, pt->size };
        SDL_SetRenderDrawColor(renderer,
            (Uint8)pt->r, (Uint8)pt->g, (Uint8)pt->b, (Uint8)pt->a);
        SDL_RenderFillRect(renderer, &r);
    }
}

/* ======================== 查询 ======================== */
int jparticle_count(jparticle_p p) {
    return p ? (int)p->particles.n : 0;
}

bool jparticle_is_active(jparticle_p p) {
    return p && p->particles.n > 0;
}
