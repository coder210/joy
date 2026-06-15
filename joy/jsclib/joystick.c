#include <assert.h>
#include "jsclib.h"
#include "../jui.h"
#define PTR(n) duk_get_pointer(ctx,n)
#define NUM(n) ((float)duk_to_number(ctx,n))

static duk_ret_t jjoystick_create(duk_context* ctx)
{
	jr2d_p r2d = (jr2d_p)PTR(0);
	duk_push_pointer(ctx, joystick_create(ren, NUM(1), NUM(2), NUM(3)));
	return 1;
}

static duk_ret_t jjoystick_destroy(duk_context* ctx)
{
	joystick_p joy = (joystick_p)PTR(0);
	if (joy) joystick_destroy(joy);
	return 0;
}

static duk_ret_t jjoystick_draw(duk_context* ctx)
{
	joystick_p joy = (joystick_p)PTR(0);
	if (joy)
		joystick_draw(joy);
	return 0;
}

static duk_ret_t jjoystick_is_dragging(duk_context* ctx)
{
	joystick_p joy = (joystick_p)PTR(0);
	duk_push_boolean(ctx, joy && joystick_is_dragging(joy));
	return 1;
}

static duk_ret_t jjoystick_touch_down(duk_context* ctx)
{
	joystick_p joy = (joystick_p)PTR(0);
	if (joy)
		joystick_touch_down(joy, NUM(1), NUM(2));
	return 0;
}

static duk_ret_t jjoystick_touch_move(duk_context* ctx)
{
	joystick_p joy = (joystick_p)PTR(0);
	if (joy)
		joystick_touch_move(joy, NUM(1), NUM(2));
	return 0;
}

static duk_ret_t jjoystick_touch_up(duk_context* ctx)
{
	joystick_p joy = (joystick_p)PTR(0);
	if (joy) joystick_touch_up(joy);
	return 0;
}

static duk_ret_t jjoystick_reset(duk_context* ctx)
{
	joystick_p joy = (joystick_p)PTR(0);
	if (joy) joystick_reset(joy);
	return 0;
}

static duk_ret_t jjoystick_set_position(duk_context* ctx)
{ 
	joystick_p joy = (joystick_p)PTR(0);
	if (joy) 
		joystick_set_position(joy, NUM(1), NUM(2));
	return 0;
}

/* get_direction: 返回 {x:n, y:n} */
static duk_ret_t jjoystick_get_direction(duk_context* ctx) 
{ 
	joystick_p joy = (joystick_p)PTR(0);
	vec2_t pt = joystick_get_direction(joy);
	duk_push_object(ctx);
	duk_push_number(ctx, (duk_double_t)pt.x);
	duk_put_prop_string(ctx, -2, "x");
	duk_push_number(ctx, (duk_double_t)pt.y);
	duk_put_prop_string(ctx, -2, "y");
	return 1;
}

static duk_ret_t jjoystick_get_magnitude(duk_context* ctx)
{ 
	joystick_p joy = (joystick_p)PTR(0);
	duk_push_number(ctx, (duk_double_t)joystick_get_magnitude(joy));
	return 1;
}

void jsopen_joystick(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jjoystick_create, 4); duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, jjoystick_destroy, 1); duk_put_prop_string(ctx, -2, "destroy");
	duk_push_c_function(ctx, jjoystick_draw, 1); duk_put_prop_string(ctx, -2, "draw");
	duk_push_c_function(ctx, jjoystick_is_dragging, 1); duk_put_prop_string(ctx, -2, "is_dragging");
	duk_push_c_function(ctx, jjoystick_touch_down, 3); duk_put_prop_string(ctx, -2, "touch_down");
	duk_push_c_function(ctx, jjoystick_touch_move, 3); duk_put_prop_string(ctx, -2, "touch_move");
	duk_push_c_function(ctx, jjoystick_touch_up, 1); duk_put_prop_string(ctx, -2, "touch_up");
	duk_push_c_function(ctx, jjoystick_reset, 1); duk_put_prop_string(ctx, -2, "reset");
	duk_push_c_function(ctx, jjoystick_set_position, 3); duk_put_prop_string(ctx, -2, "set_position");
	duk_push_c_function(ctx, jjoystick_get_direction, 1); duk_put_prop_string(ctx, -2, "get_direction");
	duk_push_c_function(ctx, jjoystick_get_magnitude, 1); duk_put_prop_string(ctx, -2, "get_magnitude");
	duk_put_global_string(ctx, "joystick");
}
