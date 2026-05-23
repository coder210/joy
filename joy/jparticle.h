/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jparticle.h
Description: 2D 粒子系统
Author: ydlc
Version: 1.0
Date: 2024.7.15
History:
*************************************************/
#ifndef JPARTICLE_H
#define JPARTICLE_H
#include "jconfig.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- 粒子发射器配置 ---------- */
typedef struct jparticle_cfg {
    float x, y;                /* 发射位置 */
    float spawn_radius;        /* 生成半径(粒子在此范围内随机生成) */
    float direction;           /* 基础方向(度) */
    float spread;              /* 扩散角(度, 0=无扩散, 360=全方向) */
    float speed_min, speed_max;
    float lifetime_min, lifetime_max;
    float size_start, size_end;/* 起始/结束大小(线性插值) */
    float r, g, b, a;         /* 颜色 */
    float fadeout;             /* 淡出比例 0~1(0.8=最后20%生命周期淡出) */
    int   burst;               /* 每次 emit 产生的粒子数 */
    float rate;                /* 持续发射: 每秒粒子数(<=0 只爆发不持续) */
    bool  gravity;             /* 是否受重力影响 */
    float gravity_y;           /* 重力值(正数向下, 如 50.0f) */
} jparticle_cfg_t;

/* 默认配置 */
JOY_API jparticle_cfg_t jparticle_cfg_default(void);

/* ---------- 发射器(不透明句柄) ---------- */
typedef struct jparticle_t jparticle_t, *jparticle_p;

/* 创建/销毁 */
JOY_API jparticle_p jparticle_create(jparticle_cfg_t cfg);
JOY_API void jparticle_destroy(jparticle_p p);

/* 控制 */
JOY_API void jparticle_set_position(jparticle_p p, float x, float y);
JOY_API void jparticle_emit(jparticle_p p);            /* 爆发一次 */
JOY_API void jparticle_clear(jparticle_p p);            /* 清除所有粒子 */

/* 每帧调用 */
JOY_API void jparticle_update(jparticle_p p, float dt);
JOY_API void jparticle_render(jparticle_p p, SDL_Renderer* renderer);

/* 查询 */
JOY_API int  jparticle_count(jparticle_p p);
JOY_API bool jparticle_is_active(jparticle_p p);

#ifdef __cplusplus
}
#endif

#endif // JPARTICLE_H
