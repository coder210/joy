#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "janimator.h"
#include "jr2d.h"
#include "external/klib/kvec.h"

/* ========== Internal Types ========== */

#define JANIM_MAX_NAME 64

typedef struct {
    char param_name[JANIM_MAX_NAME];
    janim_cond_mode_t mode;
    float threshold;
} janim_condition_t;

typedef struct {
    int from_state;   /* -1 for Any State */
    int to_state;
    kvec_t(janim_condition_t) conditions;
    float exit_time;  /* min seconds source state must play before allowing transition */
} janim_transition_t;

typedef struct {
    image_p image;
    float duration;
    SDL_FRect src_rect;
    bool is_color;     /* true = solid color, no image */
    Uint8 color_r, color_g, color_b, color_a;
} janim_motion_t;

typedef struct {
    char name[JANIM_MAX_NAME];
    kvec_t(janim_motion_t) clips;      /* frame list */
    bool loop;
    float speed;

    /* blend tree */
    bool is_blended;
    char blend_param[JANIM_MAX_NAME];
    kvec_t(janim_motion_t) motions;
    kvec_t(float) thresholds;

    /* runtime */
    int frame;
    float elapsed;      /* time within current clip */
    float state_time;   /* total time since entering this state */
    bool just_entered;
} janim_state_t;

typedef struct {
    char name[JANIM_MAX_NAME];
    janim_param_type_t type;
    union {
        float f;
        int i;
        int b;    /* 0/1 for bool */
    } v;
    bool trigger_consumed;  /* trigger fires once then consumed */
} janim_param_t;

struct janimator {
    jr2d_p renderer;
    kvec_t(janim_state_t) states;
    kvec_t(janim_transition_t) transitions;
    kvec_t(janim_param_t) params;

    int default_state;
    int current_state;

    janim_state_cb on_enter;
    janim_state_cb on_exit;
    void* cb_userdata;
    bool flip_h;  /* horizontal flip */
};

/* ========== Helpers ========== */

static int find_state(janimator_p anim, const char* name)
{
    unsigned int i;
    for (i = 0; i < kv_size(anim->states); i++) {
        if (SDL_strcmp(kv_A(anim->states, i).name, name) == 0)
            return (int)i;
    }
    return -1;
}

static int find_param(janimator_p anim, const char* name)
{
    unsigned int i;
    for (i = 0; i < kv_size(anim->params); i++) {
        if (SDL_strcmp(kv_A(anim->params, i).name, name) == 0)
            return (int)i;
    }
    return -1;
}

static janim_param_t* get_or_create_param(janimator_p anim, const char* name,
    janim_param_type_t type)
{
    int idx = find_param(anim, name);
    if (idx >= 0) {
        janim_param_t* p = &kv_A(anim->params, idx);
        p->type = type;
        return p;
    }
    janim_param_t p;
    SDL_strlcpy(p.name, name, JANIM_MAX_NAME);
    p.type = type;
    p.v.f = 0.0f;
    p.trigger_consumed = true;  /* trigger starts unset */
    kv_push(janim_param_t, anim->params, p);
    return &kv_A(anim->params, kv_size(anim->params) - 1);
}

static void enter_state(janimator_p anim, int idx)
{
    if (anim->on_exit && anim->current_state >= 0 &&
        (unsigned)anim->current_state < kv_size(anim->states))
    {
        anim->on_exit(kv_A(anim->states, anim->current_state).name,
            anim->cb_userdata);
    }

    anim->current_state = idx;
    kv_A(anim->states, idx).frame = 0;
    kv_A(anim->states, idx).elapsed = 0.0f;
    kv_A(anim->states, idx).state_time = 0.0f;
    kv_A(anim->states, idx).just_entered = true;

    if (anim->on_enter) {
        anim->on_enter(kv_A(anim->states, idx).name, anim->cb_userdata);
    }
}

/* ========== Lifecycle ========== */

janimator_p janimator_create(jr2d_p renderer)
{
    janimator_p anim = (janimator_p)SDL_calloc(1, sizeof(janimator_t));
    if (!anim) return NULL;
    anim->renderer = renderer;
    kv_init(anim->states);
    kv_init(anim->transitions);
    kv_init(anim->params);
    anim->default_state = -1;
    anim->current_state = -1;
    anim->on_enter = NULL;
    anim->on_exit = NULL;
    anim->cb_userdata = NULL;
    anim->flip_h = false;
    return anim;
}

void janimator_destroy(janimator_p anim)
{
    unsigned int i, j;
    if (!anim) return;

    /* free state clips/motions */
    for (i = 0; i < kv_size(anim->states); i++) {
        janim_state_t* s = &kv_A(anim->states, i);
        for (j = 0; j < kv_size(s->clips); j++)
            if (kv_A(s->clips, j).image)
                image_destroy(kv_A(s->clips, j).image);
        kv_destroy(s->clips);
        for (j = 0; j < kv_size(s->motions); j++)
            if (kv_A(s->motions, j).image)
                image_destroy(kv_A(s->motions, j).image);
        kv_destroy(s->motions);
        kv_destroy(s->thresholds);
    }
    kv_destroy(anim->states);

    /* free transitions */
    for (i = 0; i < kv_size(anim->transitions); i++)
        kv_destroy(kv_A(anim->transitions, i).conditions);
    kv_destroy(anim->transitions);

    kv_destroy(anim->params);
    SDL_free(anim);
}

/* ========== Parameters ========== */

void janimator_set_float(janimator_p anim, const char* name, float v)
{
    janim_param_t* p = get_or_create_param(anim, name, JANIM_PARAM_FLOAT);
    p->v.f = v;
}

void janimator_set_int(janimator_p anim, const char* name, int v)
{
    janim_param_t* p = get_or_create_param(anim, name, JANIM_PARAM_INT);
    p->v.i = v;
}

void janimator_set_bool(janimator_p anim, const char* name, bool v)
{
    janim_param_t* p = get_or_create_param(anim, name, JANIM_PARAM_BOOL);
    p->v.b = v ? 1 : 0;
}

void janimator_set_trigger(janimator_p anim, const char* name)
{
    janim_param_t* p = get_or_create_param(anim, name, JANIM_PARAM_TRIGGER);
    p->trigger_consumed = false;
}

float janimator_get_float(janimator_p anim, const char* name)
{
    int idx = find_param(anim, name);
    if (idx < 0) return 0.0f;
    return kv_A(anim->params, idx).v.f;
}

int janimator_get_int(janimator_p anim, const char* name)
{
    int idx = find_param(anim, name);
    if (idx < 0) return 0;
    return kv_A(anim->params, idx).v.i;
}

bool janimator_get_bool(janimator_p anim, const char* name)
{
    int idx = find_param(anim, name);
    if (idx < 0) return false;
    return kv_A(anim->params, idx).v.b != 0;
}

/* ========== States ========== */

void janimator_add_state(janimator_p anim, const char* name)
{
    janim_state_t s;
    SDL_strlcpy(s.name, name, JANIM_MAX_NAME);
    kv_init(s.clips);
    s.loop = true;
    s.speed = 1.0f;
    s.is_blended = false;
    s.blend_param[0] = '\0';
    kv_init(s.motions);
    kv_init(s.thresholds);
    s.frame = 0;
    s.elapsed = 0.0f;
    s.state_time = 0.0f;
    s.just_entered = false;
    kv_push(janim_state_t, anim->states, s);

    /* first state becomes default */
    if (anim->default_state < 0) {
        anim->default_state = (int)(kv_size(anim->states) - 1);
        anim->current_state = (int)(kv_size(anim->states) - 1);
    }
}

void janimator_set_default_state(janimator_p anim, const char* name)
{
    int idx = find_state(anim, name);
    if (idx >= 0) {
        anim->default_state = idx;
        enter_state(anim, idx);
    }
}

void janimator_state_add_clip(janimator_p anim, const char* state_name,
    const char* image_path, float duration, SDL_FRect src_rect)
{
    int sidx = find_state(anim, state_name);
    if (sidx < 0) return;
    janim_state_t* s = &kv_A(anim->states, sidx);
    janim_motion_t m;
    m.image = image_create(anim->renderer, image_path);
    m.duration = duration;
    m.src_rect = src_rect;
    m.is_color = false;
    kv_push(janim_motion_t, s->clips, m);
}

void janimator_state_add_color(janimator_p anim, const char* state_name,
    float duration, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    int sidx = find_state(anim, state_name);
    if (sidx < 0) return;
    janim_state_t* s = &kv_A(anim->states, sidx);
    janim_motion_t m;
    m.image = NULL;
    m.duration = duration;
    m.src_rect = (SDL_FRect){ 0, 0, (float)w, (float)h };
    m.is_color = true;
    m.color_r = r; m.color_g = g; m.color_b = b; m.color_a = a;
    kv_push(janim_motion_t, s->clips, m);
}

void janimator_state_set_loop(janimator_p anim, const char* state_name, bool loop)
{
    int sidx = find_state(anim, state_name);
    if (sidx >= 0)
        kv_A(anim->states, sidx).loop = loop;
}

void janimator_state_set_speed(janimator_p anim, const char* state_name, float speed)
{
    int sidx = find_state(anim, state_name);
    if (sidx >= 0)
        kv_A(anim->states, sidx).speed = speed;
}

/* ========== Blend Tree ========== */

void janimator_state_set_blend(janimator_p anim, const char* state_name,
    const char* param_name)
{
    int sidx = find_state(anim, state_name);
    if (sidx < 0) return;
    janim_state_t* s = &kv_A(anim->states, sidx);
    s->is_blended = true;
    SDL_strlcpy(s->blend_param, param_name, JANIM_MAX_NAME);
}

void janimator_state_add_motion(janimator_p anim, const char* state_name,
    const char* image_path, float duration, SDL_FRect src_rect, float threshold)
{
    int sidx = find_state(anim, state_name);
    if (sidx < 0) return;
    janim_state_t* s = &kv_A(anim->states, sidx);
    janim_motion_t m;
    m.image = image_create(anim->renderer, image_path);
    m.duration = duration;
    m.src_rect = src_rect;
    m.is_color = false;
    kv_push(janim_motion_t, s->motions, m);
    kv_push(float, s->thresholds, threshold);
}

/* ========== Transitions ========== */

int janimator_add_transition(janimator_p anim, const char* from, const char* to)
{
    int from_idx = find_state(anim, from);
    int to_idx = find_state(anim, to);
    if (from_idx < 0 || to_idx < 0) return -1;
    janim_transition_t t;
    t.from_state = from_idx;
    t.to_state = to_idx;
    t.exit_time = 0.0f;
    kv_init(t.conditions);
    kv_push(janim_transition_t, anim->transitions, t);
    return (int)(kv_size(anim->transitions) - 1);
}

int janimator_add_any_transition(janimator_p anim, const char* to)
{
    int to_idx = find_state(anim, to);
    if (to_idx < 0) return -1;
    janim_transition_t t;
    t.from_state = -1;  /* Any State */
    t.to_state = to_idx;
    t.exit_time = 0.0f;
    kv_init(t.conditions);
    kv_push(janim_transition_t, anim->transitions, t);
    return (int)(kv_size(anim->transitions) - 1);
}

void janimator_transition_add_cond(janimator_p anim, int trans_idx,
    const char* param_name, int mode, float threshold)
{
    if (trans_idx < 0 || (unsigned)trans_idx >= kv_size(anim->transitions))
        return;
    janim_condition_t c;
    SDL_strlcpy(c.param_name, param_name, JANIM_MAX_NAME);
    c.mode = (janim_cond_mode_t)mode;
    c.threshold = threshold;
    kv_push(janim_condition_t, kv_A(anim->transitions, trans_idx).conditions, c);
}

void janimator_transition_set_exit_time(janimator_p anim, int trans_idx, float seconds)
{
    if (trans_idx < 0 || (unsigned)trans_idx >= kv_size(anim->transitions))
        return;
    kv_A(anim->transitions, trans_idx).exit_time = seconds;
}

/* ========== Transition checking ========== */

static bool check_condition(janimator_p anim, const janim_condition_t* c)
{
    int pidx = find_param(anim, c->param_name);
    if (pidx < 0) return false;
    janim_param_t* p = &kv_A(anim->params, pidx);

    switch (c->mode) {
    case JANIM_COND_GREATER:
        return (p->type == JANIM_PARAM_FLOAT) ? (p->v.f > c->threshold) :
               (p->type == JANIM_PARAM_INT)   ? (p->v.i > (int)c->threshold) :
               false;
    case JANIM_COND_LESS:
        return (p->type == JANIM_PARAM_FLOAT) ? (p->v.f < c->threshold) :
               (p->type == JANIM_PARAM_INT)   ? (p->v.i < (int)c->threshold) :
               false;
    case JANIM_COND_EQUALS: {
        float eps = 0.001f;
        return (p->type == JANIM_PARAM_FLOAT) ? (SDL_fabsf(p->v.f - c->threshold) < eps) :
               (p->type == JANIM_PARAM_INT)   ? (p->v.i == (int)c->threshold) :
               false;
    }
    case JANIM_COND_NOT_EQUALS: {
        float eps = 0.001f;
        return (p->type == JANIM_PARAM_FLOAT) ? (SDL_fabsf(p->v.f - c->threshold) >= eps) :
               (p->type == JANIM_PARAM_INT)   ? (p->v.i != (int)c->threshold) :
               false;
    }
    case JANIM_COND_IF_TRUE:
        return (p->type == JANIM_PARAM_BOOL) && (p->v.b != 0);
    case JANIM_COND_IF_FALSE:
        return (p->type == JANIM_PARAM_BOOL) && (p->v.b == 0);
    case JANIM_COND_TRIGGER: {
        /* don't consume here; check_transition will consume after all conditions pass */
        return (p->type == JANIM_PARAM_TRIGGER && !p->trigger_consumed);
    }
    default:
        return false;
    }
}

static bool check_transition(janimator_p anim, const janim_transition_t* t)
{
    unsigned int i;
    if (kv_size(t->conditions) == 0)
        return true;
    for (i = 0; i < kv_size(t->conditions); i++) {
        if (!check_condition(anim, &kv_A(t->conditions, i)))
            return false;
    }
    /* all conditions passed → now consume triggers */
    for (i = 0; i < kv_size(t->conditions); i++) {
        const janim_condition_t* c = &kv_A(t->conditions, i);
        if (c->mode == JANIM_COND_TRIGGER) {
            int pidx = find_param(anim, c->param_name);
            if (pidx >= 0)
                kv_A(anim->params, pidx).trigger_consumed = true;
        }
    }
    return true;
}

/* ========== Update ========== */

void janimator_update(janimator_p anim, float dt)
{
    unsigned int i;
    unsigned int nstates;
    int cur;
    janim_state_t* cs;

    if (!anim) return;
    nstates = kv_size(anim->states);
    if (nstates == 0) return;

    cur = anim->current_state;
    if (cur < 0 || (unsigned)cur >= nstates) {
        if (anim->default_state >= 0 && (unsigned)anim->default_state < nstates) {
            enter_state(anim, anim->default_state);
            cur = anim->current_state;
        } else {
            return;
        }
    }

    cs = &kv_A(anim->states, cur);
    cs->state_time += dt;
    cs->just_entered = false;

    /* Check Any State transitions first */
    for (i = 0; i < kv_size(anim->transitions); i++) {
        janim_transition_t* t = &kv_A(anim->transitions, i);
        if (t->from_state == -1 && t->to_state != cur) {
            if (check_transition(anim, t)) {
                enter_state(anim, t->to_state);
                cs = &kv_A(anim->states, anim->current_state);
                goto after_transition;
            }
        }
    }

    /* Normal transitions: only if exit_time has elapsed */
    for (i = 0; i < kv_size(anim->transitions); i++) {
        janim_transition_t* t = &kv_A(anim->transitions, i);
        if (t->from_state == cur && t->to_state != cur) {
            if (cs->state_time < t->exit_time) continue;  /* exit time not met */
            if (check_transition(anim, t)) {
                enter_state(anim, t->to_state);
                cs = &kv_A(anim->states, anim->current_state);
                goto after_transition;
            }
        }
    }

after_transition:
    if (cs->is_blended) {
        int pidx = find_param(anim, cs->blend_param);
        float val = 0.0f;
        if (pidx >= 0) {
            janim_param_t* p = &kv_A(anim->params, pidx);
            val = (p->type == JANIM_PARAM_FLOAT) ? p->v.f : (float)p->v.i;
        }
        unsigned int mi;
        int best = 0;
        for (mi = 0; mi < kv_size(cs->thresholds); mi++) {
            if (val >= kv_A(cs->thresholds, mi))
                best = (int)mi;
        }
        if ((unsigned)best >= kv_size(cs->motions))
            best = (int)kv_size(cs->motions) - 1;
        cs->frame = best;
    } else {
        if (kv_size(cs->clips) > 0) {
            float adjusted_dt = dt * cs->speed;
            cs->elapsed += adjusted_dt;
            janim_motion_t* clip = &kv_A(cs->clips, cs->frame);
            if (cs->elapsed >= clip->duration) {
                cs->elapsed -= clip->duration;
                cs->frame++;
                if ((unsigned)cs->frame >= kv_size(cs->clips)) {
                    if (cs->loop)
                        cs->frame = 0;
                    else
                        cs->frame = (int)kv_size(cs->clips) - 1;
                }
            }
        }
    }
}

/* ========== Draw ========== */

void janimator_draw(janimator_p anim, float x, float y)
{
    if (!anim) return;
    if (anim->current_state < 0 || (unsigned)anim->current_state >= kv_size(anim->states))
        return;
    janim_state_t* cs = &kv_A(anim->states, anim->current_state);

    janim_motion_t* m;
    if (cs->is_blended) {
        if ((unsigned)cs->frame >= kv_size(cs->motions)) return;
        m = &kv_A(cs->motions, cs->frame);
    } else {
        if ((unsigned)cs->frame >= kv_size(cs->clips)) return;
        m = &kv_A(cs->clips, cs->frame);
    }

    if (m->is_color) {
        SDL_FRect r = { x, y, m->src_rect.w, m->src_rect.h };
        SDL_SetRenderDrawColor(anim->renderer, m->color_r, m->color_g, m->color_b, m->color_a);
        SDL_RenderFillRect(anim->renderer, &r);
    } else if (m->image) {
        float sx = anim->flip_h ? -1.0f : 1.0f;
        image_draw_ex(m->image, &m->src_rect, (vec2_t){x, y}, 0.0f,
            (vec2_t){sx, 1.0f}, (vec2_t){0.0f, 0.0f});
    } else {
        SDL_FRect r = { x, y, m->src_rect.w, m->src_rect.h };
        SDL_SetRenderDrawColor(anim->renderer, 80, 80, 80, 255);
        SDL_RenderFillRect(anim->renderer, &r);
        SDL_SetRenderDrawColor(anim->renderer, 120, 120, 120, 255);
        SDL_RenderRect(anim->renderer, &r);
    }
}

void janimator_set_flip_h(janimator_p anim, bool flip)
{
    if (anim) anim->flip_h = flip;
}

/* ========== Query ========== */

const char* janimator_current_state(janimator_p anim)
{
    if (!anim || anim->current_state < 0 ||
        (unsigned)anim->current_state >= kv_size(anim->states))
        return "";
    return kv_A(anim->states, anim->current_state).name;
}

/* ========== Callbacks ========== */

void janimator_set_on_enter(janimator_p anim, janim_state_cb cb, void* userdata)
{
    anim->on_enter = cb;
    anim->cb_userdata = userdata;
}

void janimator_set_on_exit(janimator_p anim, janim_state_cb cb, void* userdata)
{
    anim->on_exit = cb;
    anim->cb_userdata = userdata;
}
