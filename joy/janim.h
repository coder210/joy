/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: janim.h
Description: 帧动画状态机（纯 C，单文件，宏生成多实例类型）
  数据导向设计，定义/运行时/参数三层分离，
  适合 ECS 或自定义管理。

  klib 风格：用 JANIM_DEFINE(name) 生成独立具名类型，
  解决 ECS 中多个动画定义注册为组件时类型重名的问题：

    JANIM_DEFINE(warrior);    // 生成 janim_warrior_def_t + janim_warrior_* 函数
    JANIM_DEFINE(goblin);     // 生成 janim_goblin_def_t  + janim_goblin_*  函数

  用法示例:
    janim_warrior_def_t def = {0};
    int s = janim_warrior_add_state(&def, "walk", true);
    janim_warrior_add_clip(&def, s, "tex.png", 0.15f, (jrect_t){0,0,192,192});
    ...
    anim_t a; anim_init(&a);
    anim_params_t p = { .speed = 1.0f, .dir = 0 };
    janim_warrior_update(&a, &def, &p, 0.016f);

    anim_clip_t clip;
    janim_warrior_get_current_clip(&a, &def, &clip);
    image_draw_ex(img, &clip.src_rect, pos, 0, scale, origin);

  共享底层：未宏化的 anim_def_t / anim_* 函数仍然可用，
  适合不关心类型唯一性的直接使用场景。

Author: ydlc
Version: 2.0
Date: 2026.7.20
History:
  2026.8.14: v2.0 增加 JANIM_DEFINE(name) 宏生成多实例具名类型（klib 风格）
*************************************************/
#ifndef JOY_JANIM_H
#define JOY_JANIM_H

#include <stdbool.h>
#include <string.h>
#include "joy.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 常量 ==================== */
#define ANIM_MAX_STATES       16
#define ANIM_MAX_TRANSITIONS  32
#define ANIM_MAX_CLIPS        16
#define ANIM_MAX_CONDITIONS   8
#define ANIM_NAME_LEN         32
#define ANIM_PATH_LEN         256

/* ==================== 条件模式 ==================== */
typedef enum {
    ANIM_COND_GREATER,
    ANIM_COND_LESS,
    ANIM_COND_EQUALS,
    ANIM_COND_NOT_EQUALS,
    ANIM_COND_IF_TRUE,
    ANIM_COND_IF_FALSE,
    ANIM_COND_TRIGGER,
} anim_cond_mode_t;

/* ==================== 定义层（共享成员类型） ==================== */

/** 过渡条件 */
typedef struct anim_cond {
    char param_name[ANIM_NAME_LEN];
    anim_cond_mode_t mode;
    float threshold;
} anim_cond_t;

/** 状态过渡 */
typedef struct anim_transition {
    int from_state;   /* -1 = Any State */
    int to_state;
    anim_cond_t conditions[ANIM_MAX_CONDITIONS];
    int num_conditions;
    float exit_time;  /* 切出前最短停留时间（秒） */
} anim_transition_t;

/** 单帧定义 */
typedef struct anim_clip_def {
    char path[ANIM_PATH_LEN];
    jrect_t src_rect;
    float duration;
} anim_clip_def_t;

/** 动画状态 */
typedef struct anim_state {
    char name[ANIM_NAME_LEN];
    anim_clip_def_t clips[ANIM_MAX_CLIPS];
    int num_clips;
    bool loop;
    float speed;
} anim_state_t;

/** 完整动画定义（共享布局类型，多实体共享）。
    若需作为 ECS 组件注册多个不同类型，请用 JANIM_DEFINE(name) 生成具名包装类型。 */
typedef struct anim_def {
    anim_state_t states[ANIM_MAX_STATES];
    int num_states;
    anim_transition_t transitions[ANIM_MAX_TRANSITIONS];
    int num_transitions;
    int default_state;
} anim_def_t;

/* ==================== 运行时层（每个实体一份，全局共享） ==================== */

/** 动画运行时状态 */
typedef struct anim {
    int current_state;
    int current_frame;
    float frame_elapsed;
    float state_elapsed;
    bool just_entered;
    bool flip_h;
} anim_t;

/** 动画输入参数（由 gameplay 写入，每实体一份） */
typedef struct anim_params {
    float speed;
    int dir;
    bool attack;  /* trigger，消费后复位 */
} anim_params_t;

/* ==================== 输出（给渲染用） ==================== */
typedef struct anim_clip {
    const char* path;
    jrect_t src_rect;
    float duration;
} anim_clip_t;

/* ==================== 共享 API 声明（实现见文件末尾） ==================== */

int anim_add_state(anim_def_t* def, const char* name, bool loop);
void anim_add_clip(anim_def_t* def, int state_idx, const char* img_path, float duration, jrect_t src_rect);
void anim_set_default_state(anim_def_t* def, int idx);
int anim_add_transition(anim_def_t* def, int from_state, int to_state);
void anim_transition_add_cond(anim_def_t* def, int trans_idx, const char* param_name, int mode, float threshold);
void anim_transition_set_exit_time(anim_def_t* def, int trans_idx, float seconds);

void anim_init(anim_t* a);
bool anim_update(anim_t* a, const anim_def_t* def, const anim_params_t* params, float dt);
bool anim_get_current_clip(const anim_t* a, const anim_def_t* def, anim_clip_t* out_clip);

/* ==================== JANIM_DEFINE：生成具名定义类型 ==================== */
/* klib 风格宏。每个 name 生成一套独立类型与转发函数：
 *   类型     janim_##name##_def_t   （包裹 anim_def_t layout，布局统一）
 *   构建     janim_##name##_add_state / _add_clip / _set_default_state /
 *            _add_transition / _transition_add_cond / _transition_set_exit_time
 *   运行     janim_##name##_update / _get_current_clip
 * 转发只做 &def->layout 转换，共享实现只写一遍（见文件末尾）。
 */
#define JANIM_DEFINE(name)                                                                  \
    typedef struct janim_##name##_def {                                                     \
        anim_def_t layout;                                                                  \
    } janim_##name##_def_t;                                                                 \
                                                                                            \
    static inline int janim_##name##_add_state(                                             \
        janim_##name##_def_t* def, const char* n, bool loop)                                \
    { return anim_add_state(&def->layout, n, loop); }                                       \
                                                                                            \
    static inline void janim_##name##_add_clip(                                             \
        janim_##name##_def_t* def, int state_idx, const char* img, float dur,               \
        jrect_t rect)                                                                       \
    { anim_add_clip(&def->layout, state_idx, img, dur, rect); }                             \
                                                                                            \
    static inline void janim_##name##_set_default_state(                                    \
        janim_##name##_def_t* def, int idx)                                                 \
    { anim_set_default_state(&def->layout, idx); }                                          \
                                                                                            \
    static inline int janim_##name##_add_transition(                                        \
        janim_##name##_def_t* def, int from, int to)                                        \
    { return anim_add_transition(&def->layout, from, to); }                                 \
                                                                                            \
    static inline void janim_##name##_transition_add_cond(                                  \
        janim_##name##_def_t* def, int trans_idx, const char* p, int mode, float th)        \
    { anim_transition_add_cond(&def->layout, trans_idx, p, mode, th); }                     \
                                                                                            \
    static inline void janim_##name##_transition_set_exit_time(                             \
        janim_##name##_def_t* def, int trans_idx, float secs)                               \
    { anim_transition_set_exit_time(&def->layout, trans_idx, secs); }                       \
                                                                                            \
    static inline bool janim_##name##_update(                                               \
        anim_t* a, const janim_##name##_def_t* def, const anim_params_t* params, float dt)  \
    { return anim_update(a, &def->layout, params, dt); }                                    \
                                                                                            \
    static inline bool janim_##name##_get_current_clip(                                     \
        const anim_t* a, const janim_##name##_def_t* def, anim_clip_t* out)                 \
    { return anim_get_current_clip(a, &def->layout, out); }

#ifdef __cplusplus
}
#endif

/* ==================== 实现 ==================== */

// --- 定义构建 ---

static int anim_add_state(anim_def_t* def, const char* name, bool loop) {
    if (!def || def->num_states >= ANIM_MAX_STATES) return -1;
    int idx = def->num_states;
    anim_state_t* s = &def->states[idx];
    memset(s, 0, sizeof(*s));
    strncpy(s->name, name, ANIM_NAME_LEN - 1);
    s->loop = loop;
    s->speed = 1.0f;
    def->num_states++;
    return idx;
}

static void anim_add_clip(anim_def_t* def, int state_idx, const char* img_path, float duration, jrect_t src_rect) {
    if (!def || state_idx < 0 || state_idx >= def->num_states) return;
    anim_state_t* s = &def->states[state_idx];
    if (s->num_clips >= ANIM_MAX_CLIPS) return;
    anim_clip_def_t* c = &s->clips[s->num_clips];
    memset(c, 0, sizeof(*c));
    strncpy(c->path, img_path, ANIM_PATH_LEN - 1);
    c->duration = duration;
    c->src_rect = src_rect;
    s->num_clips++;
}

static void anim_set_default_state(anim_def_t* def, int idx) {
    if (def && idx >= 0 && idx < def->num_states)
        def->default_state = idx;
}

static int anim_add_transition(anim_def_t* def, int from_state, int to_state) {
    if (!def || def->num_transitions >= ANIM_MAX_TRANSITIONS) return -1;
    int idx = def->num_transitions;
    anim_transition_t* t = &def->transitions[idx];
    memset(t, 0, sizeof(*t));
    t->from_state = from_state;
    t->to_state = to_state;
    def->num_transitions++;
    return idx;
}

static void anim_transition_add_cond(anim_def_t* def, int trans_idx, const char* param_name, int mode, float threshold) {
    if (!def || trans_idx < 0 || trans_idx >= def->num_transitions) return;
    anim_transition_t* t = &def->transitions[trans_idx];
    if (t->num_conditions >= ANIM_MAX_CONDITIONS) return;
    anim_cond_t* c = &t->conditions[t->num_conditions];
    memset(c, 0, sizeof(*c));
    strncpy(c->param_name, param_name, ANIM_NAME_LEN - 1);
    c->mode = (anim_cond_mode_t)mode;
    c->threshold = threshold;
    t->num_conditions++;
}

static void anim_transition_set_exit_time(anim_def_t* def, int trans_idx, float seconds) {
    if (!def || trans_idx < 0 || trans_idx >= def->num_transitions) return;
    def->transitions[trans_idx].exit_time = seconds;
}

// --- 运行时 ---

static void anim_init(anim_t* a) {
    if (!a) return;
    memset(a, 0, sizeof(*a));
    a->just_entered = true;
}

// 内部：单条件检查
static bool check_anim_cond(float val, anim_cond_mode_t mode, float threshold) {
    switch (mode) {
    case ANIM_COND_GREATER:     return val > threshold;
    case ANIM_COND_LESS:        return val < threshold;
    case ANIM_COND_EQUALS:      return val >= threshold - 0.001f && val <= threshold + 0.001f;
    case ANIM_COND_NOT_EQUALS:  return val < threshold - 0.001f || val > threshold + 0.001f;
    case ANIM_COND_IF_TRUE:     return val != 0;
    case ANIM_COND_IF_FALSE:    return val == 0;
    case ANIM_COND_TRIGGER:     return val != 0;
    default: return false;
    }
}

// 内部：条件列表检查
static bool check_cond_list(const anim_cond_t* conds, int num, const anim_params_t* params) {
    if (num == 0) return true;
    for (int i = 0; i < num; i++) {
        const anim_cond_t* c = &conds[i];
        float val = 0.0f;
        if      (strcmp(c->param_name, "speed")   == 0) val = params->speed;
        else if (strcmp(c->param_name, "dir")     == 0) val = (float)params->dir;
        else if (strcmp(c->param_name, "attack")  == 0) val = params->attack ? 1.0f : 0.0f;
        if (!check_anim_cond(val, c->mode, c->threshold))
            return false;
    }
    return true;
}

static bool anim_update(anim_t* a, const anim_def_t* def, const anim_params_t* params, float dt) {
    if (!a || !def || def->num_states <= 0) return false;

    bool transitioned = false;

    // 第一轮：Any-State 过渡（from_state == -1）
    for (int i = 0; i < def->num_transitions; i++) {
        const anim_transition_t* t = &def->transitions[i];
        if (t->from_state != -1) continue;
        if (a->state_elapsed < t->exit_time) continue;
        if (!check_cond_list(t->conditions, t->num_conditions, params)) continue;
        // 防止重复触发攻击：已在攻击状态（index >= 2）则不重触发
        if (a->current_state >= 2 && a->current_state <= 5) continue;
        a->current_state = t->to_state;
        a->current_frame = 0;
        a->frame_elapsed = 0;
        a->state_elapsed = 0;
        a->just_entered = true;
        transitioned = true;
        break;
    }

    // 第二轮：当前状态专有过渡
    if (!transitioned) {
        for (int i = 0; i < def->num_transitions; i++) {
            const anim_transition_t* t = &def->transitions[i];
            if (t->from_state != a->current_state) continue;
            if (a->state_elapsed < t->exit_time) continue;
            if (!check_cond_list(t->conditions, t->num_conditions, params)) continue;
            a->current_state = t->to_state;
            a->current_frame = 0;
            a->frame_elapsed = 0;
            a->state_elapsed = 0;
            a->just_entered = true;
            transitioned = true;
            break;
        }
    }

    // 推进时间
    if (!transitioned) {
        a->just_entered = false;
        a->state_elapsed += dt;
    }

    // 推进帧
    if (a->current_state >= 0 && a->current_state < def->num_states) {
        const anim_state_t* state = &def->states[a->current_state];
        a->frame_elapsed += dt * state->speed;
        if (state->num_clips > 0) {
            float frame_dur = state->clips[a->current_frame].duration;
            while (a->frame_elapsed >= frame_dur) {
                a->frame_elapsed -= frame_dur;
                if (a->current_frame + 1 < state->num_clips) {
                    a->current_frame++;
                } else {
                    if (state->loop)
                        a->current_frame = 0;
                    // 非循环状态停留在最后一帧
                    break;
                }
                frame_dur = state->clips[a->current_frame].duration;
            }
        }
    }

    return transitioned;
}

static bool anim_get_current_clip(const anim_t* a, const anim_def_t* def, anim_clip_t* out_clip) {
    if (!a || !def || !out_clip || def->num_states <= 0) return false;

    int cs = a->current_state;
    if (cs < 0 || cs >= def->num_states) cs = def->default_state;
    if (cs < 0 || cs >= def->num_states) return false;

    const anim_state_t* state = &def->states[cs];
    int cf = a->current_frame;
    if (cf < 0 || cf >= state->num_clips) cf = 0;
    if (cf < 0 || cf >= state->num_clips) return false;

    const anim_clip_def_t* clip = &state->clips[cf];
    out_clip->path = clip->path;
    out_clip->src_rect = clip->src_rect;
    out_clip->duration = clip->duration;
    return true;
}

#endif /* JOY_JANIM_H */
