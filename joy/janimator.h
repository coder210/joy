#ifndef JANIMATOR_H
#define JANIMATOR_H

#include "SDL3/SDL.h"
#include "jconfig.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========== Types ========== */
typedef struct janimator janimator_t, *janimator_p;

/* Parameter types */
typedef enum {
    JANIM_PARAM_FLOAT,
    JANIM_PARAM_INT,
    JANIM_PARAM_BOOL,
    JANIM_PARAM_TRIGGER,
} janim_param_type_t;

/* Condition modes */
typedef enum {
    JANIM_COND_GREATER,     /* param > threshold */
    JANIM_COND_LESS,        /* param < threshold */
    JANIM_COND_EQUALS,      /* param == threshold (float with epsilon) */
    JANIM_COND_NOT_EQUALS,  /* param != threshold */
    JANIM_COND_IF_TRUE,     /* bool param == true */
    JANIM_COND_IF_FALSE,    /* bool param == false */
    JANIM_COND_TRIGGER,     /* trigger was set */
} janim_cond_mode_t;

/* State enter/exit callback */
typedef void (*janim_state_cb)(const char* state_name, void* userdata);

/* ========== Lifecycle ========== */
JOY_API janimator_p janimator_create(jr2d_p renderer);
JOY_API void janimator_destroy(janimator_p anim);

/* ========== Parameters ========== */
JOY_API void janimator_set_float(janimator_p anim, const char* name, float v);
JOY_API void janimator_set_int(janimator_p anim, const char* name, int v);
JOY_API void janimator_set_bool(janimator_p anim, const char* name, bool v);
JOY_API void janimator_set_trigger(janimator_p anim, const char* name);
JOY_API float janimator_get_float(janimator_p anim, const char* name);
JOY_API int janimator_get_int(janimator_p anim, const char* name);
JOY_API bool janimator_get_bool(janimator_p anim, const char* name);

/* ========== States ========== */
JOY_API void janimator_add_state(janimator_p anim, const char* name);
JOY_API void janimator_set_default_state(janimator_p anim, const char* name);

/* Add a clip (frame) to a state from an image file */
JOY_API void janimator_state_add_clip(janimator_p anim, const char* state_name,
    const char* image_path, float duration, SDL_FRect src_rect);

/* Add a solid color clip (no image file needed) */
JOY_API void janimator_state_add_color(janimator_p anim, const char* state_name,
    float duration, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

JOY_API void janimator_state_set_loop(janimator_p anim, const char* state_name, bool loop);
JOY_API void janimator_state_set_speed(janimator_p anim, const char* state_name, float speed);

/* ========== Blend Tree (1D) ========== */
JOY_API void janimator_state_set_blend(janimator_p anim, const char* state_name,
    const char* param_name);
JOY_API void janimator_state_add_motion(janimator_p anim, const char* state_name,
    const char* image_path, float duration, SDL_FRect src_rect, float threshold);

/* ========== Transitions ========== */
/* Returns transition index (for adding conditions) */
JOY_API int janimator_add_transition(janimator_p anim, const char* from, const char* to);
/* Add a transition from Any State (index = -1) */
JOY_API int janimator_add_any_transition(janimator_p anim, const char* to);
/* Add condition to a transition by index */
JOY_API void janimator_transition_add_cond(janimator_p anim, int trans_idx,
    const char* param_name, int mode, float threshold);
/* Set exit time: minimum seconds the source state must play before transition (0 = immediate) */
JOY_API void janimator_transition_set_exit_time(janimator_p anim, int trans_idx, float seconds);

/* ========== Update / Draw ========== */
JOY_API void janimator_update(janimator_p anim, float dt);
JOY_API void janimator_draw(janimator_p anim, float x, float y);
JOY_API void janimator_set_flip_h(janimator_p anim, bool flip);
JOY_API const char* janimator_current_state(janimator_p anim);

/* ========== Callbacks ========== */
JOY_API void janimator_set_on_enter(janimator_p anim, janim_state_cb cb, void* userdata);
JOY_API void janimator_set_on_exit(janimator_p anim, janim_state_cb cb, void* userdata);

#ifdef __cplusplus
}
#endif

#endif /* JANIMATOR_H */
