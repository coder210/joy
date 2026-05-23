#ifndef JLIGHT_H
#define JLIGHT_H
#include "jconfig.h"
#include <stdint.h>
#include <stdbool.h>
#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

#define JLIGHT_MAX 128
#define JLIGHT_OCCLUDER_MAX 512

/* ---------- 光源 ---------- */
typedef struct jlight {
    float x, y;          /* 位置(像素坐标) */
    float radius;        /* 光照半径 */
    float r, g, b;       /* 颜色(0-255) */
    float intensity;     /* 强度 0~1 */
} jlight_t;

/* ---------- 遮挡线段(墙壁/柱子等) ---------- */
typedef struct jlight_occluder {
    float x1, y1, x2, y2;
} jlight_occluder_t;

/* ---------- 灯光系统 ---------- */
typedef struct jlight_sys {
    int width, height;
    SDL_Texture* lightmap;
    SDL_Texture* gradient;
    jlight_t lights[JLIGHT_MAX];
    bool active[JLIGHT_MAX];
    int count;
    Uint8 ambient_r, ambient_g, ambient_b;
    jlight_occluder_t occluders[JLIGHT_OCCLUDER_MAX];
    int occluder_count;
} jlight_sys_t, *jlight_sys_p;

/* 创建/销毁 */
JOY_API jlight_sys_p jlight_sys_create(int width, int height);
JOY_API void jlight_sys_destroy(jlight_sys_p sys);
JOY_API void jlight_sys_resize(jlight_sys_p sys, int width, int height);

/* 管理光源 */
JOY_API int  jlight_add(jlight_sys_p sys, jlight_t light);
JOY_API void jlight_remove(jlight_sys_p sys, int id);
JOY_API void jlight_set(jlight_sys_p sys, int id, jlight_t light);
JOY_API void jlight_clear(jlight_sys_p sys);

/* 管理遮挡物 */
JOY_API int jlight_add_occluder(jlight_sys_p sys, float x1, float y1, float x2, float y2);
JOY_API void jlight_clear_occluders(jlight_sys_p sys);

/* 环境光 */
JOY_API void jlight_set_ambient(jlight_sys_p sys, Uint8 r, Uint8 g, Uint8 b);

/* 渲染: 场景绘制完成后, SDL_RenderPresent 前调用 */
JOY_API void jlight_render(jlight_sys_p sys, SDL_Renderer* renderer);

#ifdef __cplusplus
}
#endif

#endif // JLIGHT_H
