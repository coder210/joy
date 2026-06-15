#include "jsclib.h"
#include "../janimator.h"

#define PTR(n) duk_get_pointer(ctx, n)
#define STR(n) duk_to_string(ctx, n)
#define NUM(n) ((float)duk_to_number(ctx, n))
#define INT(n) ((int)duk_to_number(ctx, n))
#define BOOL(n) duk_to_boolean(ctx, n)

static duk_ret_t j_janim_create(duk_context* ctx)
{
    jr2d_p ren = (jr2d_p)PTR(0);
    if (!ren) { duk_error(ctx, DUK_ERR_ERROR, "animator.create: invalid renderer"); return 0; }
    duk_push_pointer(ctx, janimator_create(ren));
    return 1;
}

static duk_ret_t j_janim_destroy(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim) janimator_destroy(anim);
    return 0;
}

static duk_ret_t j_janim_set_float(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim) janimator_set_float(anim, STR(1), NUM(2));
    return 0;
}

static duk_ret_t j_janim_set_int(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim) janimator_set_int(anim, STR(1), INT(2));
    return 0;
}

static duk_ret_t j_janim_set_bool(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim) janimator_set_bool(anim, STR(1), BOOL(2));
    return 0;
}

static duk_ret_t j_janim_set_trigger(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim) janimator_set_trigger(anim, STR(1));
    return 0;
}

static duk_ret_t j_janim_get_float(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    duk_push_number(ctx, anim ? janimator_get_float(anim, STR(1)) : 0);
    return 1;
}

static duk_ret_t j_janim_get_int(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    duk_push_number(ctx, anim ? janimator_get_int(anim, STR(1)) : 0);
    return 1;
}

static duk_ret_t j_janim_get_bool(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    duk_push_boolean(ctx, anim ? janimator_get_bool(anim, STR(1)) : 0);
    return 1;
}

static duk_ret_t j_janim_add_state(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim) janimator_add_state(anim, STR(1));
    return 0;
}

static duk_ret_t j_janim_set_default_state(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim) janimator_set_default_state(anim, STR(1));
    return 0;
}

static duk_ret_t j_janim_state_add_clip(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (!anim) return 0;
    jrect_t src;
    src.x = NUM(4); src.y = NUM(5); src.w = NUM(6); src.h = NUM(7);
    janimator_state_add_clip(anim, STR(1), STR(2), NUM(3), src);
    return 0;
}

static duk_ret_t j_janim_state_add_color(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (!anim) return 0;
    janimator_state_add_color(anim, STR(1), NUM(2), INT(3), INT(4),
        INT(5), INT(6), INT(7), INT(8));
    return 0;
}

static duk_ret_t j_janim_state_set_loop(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim) janimator_state_set_loop(anim, STR(1), BOOL(2));
    return 0;
}

static duk_ret_t j_janim_state_set_speed(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim) janimator_state_set_speed(anim, STR(1), NUM(2));
    return 0;
}

static duk_ret_t j_janim_state_set_blend(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim) janimator_state_set_blend(anim, STR(1), STR(2));
    return 0;
}

static duk_ret_t j_janim_state_add_motion(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (!anim) return 0;
    jrect_t src;
    src.x = NUM(4); src.y = NUM(5); src.w = NUM(6); src.h = NUM(7);
    janimator_state_add_motion(anim, STR(1), STR(2), NUM(3), src, NUM(8));
    return 0;
}

static duk_ret_t j_janim_add_transition(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (!anim) { duk_push_int(ctx, -1); return 1; }
    duk_push_int(ctx, janimator_add_transition(anim, STR(1), STR(2)));
    return 1;
}

static duk_ret_t j_janim_add_any_transition(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (!anim) { duk_push_int(ctx, -1); return 1; }
    duk_push_int(ctx, janimator_add_any_transition(anim, STR(1)));
    return 1;
}

static duk_ret_t j_janim_transition_add_cond(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim)     janimator_transition_add_cond(anim, INT(1), STR(2), INT(3), NUM(4));
    return 0;
}

static duk_ret_t j_janim_transition_set_exit_time(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim) janimator_transition_set_exit_time(anim, INT(1), NUM(2));
    return 0;
}

static duk_ret_t j_janim_update(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim) janimator_update(anim, NUM(1));
    return 0;
}

static duk_ret_t j_janim_set_flip_h(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim) janimator_set_flip_h(anim, BOOL(1));
    return 0;
}

static duk_ret_t j_janim_draw(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    if (anim) janimator_draw(anim, NUM(1), NUM(2));
    return 0;
}

static duk_ret_t j_janim_current_state(duk_context* ctx)
{
    janimator_p anim = (janimator_p)PTR(0);
    duk_push_string(ctx, anim ? janimator_current_state(anim) : "");
    return 1;
}

void jsopen_janimator(duk_context* ctx)
{
    duk_push_object(ctx);
    duk_push_c_function(ctx, j_janim_create, 1); duk_put_prop_string(ctx, -2, "create");
    duk_push_c_function(ctx, j_janim_destroy, 1); duk_put_prop_string(ctx, -2, "destroy");
    duk_push_c_function(ctx, j_janim_set_float, 3); duk_put_prop_string(ctx, -2, "set_float");
    duk_push_c_function(ctx, j_janim_set_int, 3); duk_put_prop_string(ctx, -2, "set_int");
    duk_push_c_function(ctx, j_janim_set_bool, 3); duk_put_prop_string(ctx, -2, "set_bool");
    duk_push_c_function(ctx, j_janim_set_trigger, 2); duk_put_prop_string(ctx, -2, "set_trigger");
    duk_push_c_function(ctx, j_janim_get_float, 2); duk_put_prop_string(ctx, -2, "get_float");
    duk_push_c_function(ctx, j_janim_get_int, 2); duk_put_prop_string(ctx, -2, "get_int");
    duk_push_c_function(ctx, j_janim_get_bool, 2); duk_put_prop_string(ctx, -2, "get_bool");
    duk_push_c_function(ctx, j_janim_add_state, 2); duk_put_prop_string(ctx, -2, "add_state");
    duk_push_c_function(ctx, j_janim_set_default_state, 2); duk_put_prop_string(ctx, -2, "set_default_state");
    duk_push_c_function(ctx, j_janim_state_add_clip, 8); duk_put_prop_string(ctx, -2, "state_add_clip");
    duk_push_c_function(ctx, j_janim_state_add_color, 9); duk_put_prop_string(ctx, -2, "state_add_color");
    duk_push_c_function(ctx, j_janim_state_set_loop, 3); duk_put_prop_string(ctx, -2, "state_set_loop");
    duk_push_c_function(ctx, j_janim_state_set_speed, 3); duk_put_prop_string(ctx, -2, "state_set_speed");
    duk_push_c_function(ctx, j_janim_state_set_blend, 3); duk_put_prop_string(ctx, -2, "state_set_blend");
    duk_push_c_function(ctx, j_janim_state_add_motion, 9); duk_put_prop_string(ctx, -2, "state_add_motion");
    duk_push_c_function(ctx, j_janim_add_transition, 3); duk_put_prop_string(ctx, -2, "add_transition");
    duk_push_c_function(ctx, j_janim_add_any_transition, 2); duk_put_prop_string(ctx, -2, "add_any_transition");
    duk_push_c_function(ctx, j_janim_transition_add_cond, 5); duk_put_prop_string(ctx, -2, "transition_add_cond");
    duk_push_c_function(ctx, j_janim_transition_set_exit_time, 3); duk_put_prop_string(ctx, -2, "transition_set_exit_time");
    duk_push_c_function(ctx, j_janim_update, 2); duk_put_prop_string(ctx, -2, "update");
    duk_push_c_function(ctx, j_janim_draw, 3); duk_put_prop_string(ctx, -2, "draw");
    duk_push_c_function(ctx, j_janim_set_flip_h, 2); duk_put_prop_string(ctx, -2, "set_flip_h");
    duk_push_c_function(ctx, j_janim_current_state, 1); duk_put_prop_string(ctx, -2, "current_state");
    duk_put_global_string(ctx, "animator");
}
