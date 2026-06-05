#include "jsclib.h"
#include "../jutils.h"

static duk_ret_t jtimer_create(duk_context* ctx)
{
	int interval = (int)duk_to_number(ctx, 1);
	duk_push_pointer(ctx, timerx_create(interval, NULL, NULL));
	return 1;
}

static duk_ret_t jtimer_destroy(duk_context* ctx)
{
	timer_p t = (timer_p)duk_get_pointer(ctx, 1);
	if (t) timerx_destroy(t);
	return 0;
}

static duk_ret_t jtimer_trigger(duk_context* ctx)
{
	timer_p t = (timer_p)duk_get_pointer(ctx, 1);
	if (t) timerx_trigger(t);
	return 0;
}

static duk_ret_t jtimer_get_elapsed(duk_context* ctx)
{
	timer_p t = (timer_p)duk_get_pointer(ctx, 1);
	duk_push_number(ctx, (duk_double_t)(t ? timerx_get_elapsed(t) : 0));
	return 1;
}

static duk_ret_t jtimer_reset(duk_context* ctx)
{
	timer_p t = (timer_p)duk_get_pointer(ctx, 1);
	if (t) timerx_reset(t);
	return 0;
}

void jsopen_timer(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jtimer_create, 1);    duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, jtimer_destroy, 1);   duk_put_prop_string(ctx, -2, "destroy");
	duk_push_c_function(ctx, jtimer_trigger, 1);   duk_put_prop_string(ctx, -2, "trigger");
	duk_push_c_function(ctx, jtimer_get_elapsed, 1); duk_put_prop_string(ctx, -2, "get_elapsed");
	duk_push_c_function(ctx, jtimer_reset, 1);     duk_put_prop_string(ctx, -2, "reset");
	duk_put_global_string(ctx, "timer");
}