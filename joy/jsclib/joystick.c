#include <assert.h>
#include "jsclib.h"
#include "../jui.h"

static duk_ret_t jjoy_create(duk_context* ctx)
{
	SDL_Renderer* ren = (SDL_Renderer*)duk_get_pointer(ctx, 1);
	float x = (float)duk_to_number(ctx, 2), y = (float)duk_to_number(ctx, 3), radius = (float)duk_to_number(ctx, 4);
	duk_push_pointer(ctx, joystick_create(ren, x, y, radius));
	return 1;
}

static duk_ret_t jjoy_destroy(duk_context* ctx)
{
	joystick_p joy = (joystick_p)duk_get_pointer(ctx, 1);
	if (joy) joystick_destroy(joy);
	return 0;
}

static duk_ret_t jjoy_draw(duk_context* ctx)
{
	joystick_p joy = (joystick_p)duk_get_pointer(ctx, 1);
	if (joy) joystick_draw(joy);
	return 0;
}

static duk_ret_t jjoy_is_dragging(duk_context* ctx)
{
	joystick_p joy = (joystick_p)duk_get_pointer(ctx, 1);
	duk_push_boolean(ctx, joy && joystick_is_dragging(joy));
	return 1;
}

static duk_ret_t jjoy_touch_down(duk_context* ctx)
{
	joystick_p joy = (joystick_p)duk_get_pointer(ctx, 1);
	if (joy) joystick_touch_down(joy, (float)duk_to_number(ctx, 2), (float)duk_to_number(ctx, 3));
	return 0;
}

static duk_ret_t jjoy_touch_move(duk_context* ctx)
{
	joystick_p joy = (joystick_p)duk_get_pointer(ctx, 1);
	if (joy) joystick_touch_move(joy, (float)duk_to_number(ctx, 2), (float)duk_to_number(ctx, 3));
	return 0;
}

static duk_ret_t jjoy_touch_up(duk_context* ctx)
{
	joystick_p joy = (joystick_p)duk_get_pointer(ctx, 1);
	if (joy) joystick_touch_up(joy);
	return 0;
}

static duk_ret_t jjoy_reset(duk_context* ctx)
{
	joystick_p joy = (joystick_p)duk_get_pointer(ctx, 1);
	if (joy) joystick_reset(joy);
	return 0;
}

static duk_ret_t jjoy_set_position(duk_context* ctx)
{
	joystick_p joy = (joystick_p)duk_get_pointer(ctx, 1);
	if (joy) joystick_set_position(joy, (float)duk_to_number(ctx, 2), (float)duk_to_number(ctx, 3));
	return 0;
}

static duk_ret_t jjoy_get_direction(duk_context* ctx)
{
	joystick_p joy = (joystick_p)duk_get_pointer(ctx, 1);
	SDL_FPoint pt = joystick_get_direction(joy);
	duk_push_number(ctx, (duk_double_t)pt.x);
	duk_push_number(ctx, (duk_double_t)pt.y);
	return 2;
}

static duk_ret_t jjoy_get_magnitude(duk_context* ctx)
{
	joystick_p joy = (joystick_p)duk_get_pointer(ctx, 1);
	duk_push_number(ctx, (duk_double_t)joystick_get_magnitude(joy));
	return 1;
}

void jsopen_joystick(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jjoy_create, 4);          duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, jjoy_destroy, 1);         duk_put_prop_string(ctx, -2, "destroy");
	duk_push_c_function(ctx, jjoy_draw, 1);             duk_put_prop_string(ctx, -2, "draw");
	duk_push_c_function(ctx, jjoy_is_dragging, 1);     duk_put_prop_string(ctx, -2, "is_dragging");
	duk_push_c_function(ctx, jjoy_touch_down, 3);      duk_put_prop_string(ctx, -2, "touch_down");
	duk_push_c_function(ctx, jjoy_touch_move, 3);      duk_put_prop_string(ctx, -2, "touch_move");
	duk_push_c_function(ctx, jjoy_touch_up, 1);        duk_put_prop_string(ctx, -2, "touch_up");
	duk_push_c_function(ctx, jjoy_reset, 1);           duk_put_prop_string(ctx, -2, "reset");
	duk_push_c_function(ctx, jjoy_set_position, 3);    duk_put_prop_string(ctx, -2, "set_position");
	duk_push_c_function(ctx, jjoy_get_direction, 1);   duk_put_prop_string(ctx, -2, "get_direction");
	duk_push_c_function(ctx, jjoy_get_magnitude, 1);   duk_put_prop_string(ctx, -2, "get_magnitude");
	duk_put_global_string(ctx, "joystick");
}