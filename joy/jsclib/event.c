#include "jsclib.h"
#include "../jcore.h"

static duk_ret_t jevent_quit(duk_context* ctx)
{
	push_quit_event();
	return 0;
}

void jsopen_event(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jevent_quit, 0);
	duk_put_prop_string(ctx, -2, "quit");
	duk_put_global_string(ctx, "event");
}