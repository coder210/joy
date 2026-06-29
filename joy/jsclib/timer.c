#include "jsclib.h"
#include "../joy.h"

static duk_ret_t js_timer_create(duk_context* ctx)
{
	int interval = (int)duk_to_number(ctx, 0);
	duk_push_pointer(ctx, jtimer_create(interval, NULL, NULL));
	return 1;
}

static duk_ret_t js_timer_destroy(duk_context* ctx)
{
	jtimer_p t = (jtimer_p)duk_get_pointer(ctx, 0);
	if (t) jtimer_destroy(t);
	return 0;
}

static duk_ret_t js_timer_trigger(duk_context* ctx)
{
	jtimer_p t = (jtimer_p)duk_get_pointer(ctx, 0);
	if (t) jtimer_trigger(t);
	return 0;
}

static duk_ret_t js_timer_get_elapsed(duk_context* ctx)
{
	jtimer_p t = (jtimer_p)duk_get_pointer(ctx, 0);
	duk_push_number(ctx, (duk_double_t)(t ? jtimer_get_elapsed(t) : 0));
	return 1;
}

static duk_ret_t js_timer_reset(duk_context* ctx)
{
	jtimer_p t = (jtimer_p)duk_get_pointer(ctx, 0);
	if (t) jtimer_reset(t);
	return 0;
}

void jsopen_timer(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, js_timer_create, 1);    duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, js_timer_destroy, 1);   duk_put_prop_string(ctx, -2, "destroy");
	duk_push_c_function(ctx, js_timer_trigger, 1);   duk_put_prop_string(ctx, -2, "trigger");
	duk_push_c_function(ctx, js_timer_get_elapsed, 1); duk_put_prop_string(ctx, -2, "get_elapsed");
	duk_push_c_function(ctx, js_timer_reset, 1);     duk_put_prop_string(ctx, -2, "reset");
	duk_put_global_string(ctx, "timer");
}