#include "jsclib.h"
#include "../jcore.h"
#include <stdlib.h>

static duk_ret_t jwindow_create(duk_context* ctx)
{
	const char* title = duk_to_string(ctx, 0);
	int w = (int)duk_to_number(ctx, 1);
	int h = (int)duk_to_number(ctx, 2);
	duk_push_pointer(ctx, create_window(title, w, h));
	return 1;
}

static duk_ret_t jwindow_destroy(duk_context* ctx)
{
	jwindow_p win = (jwindow_p)duk_get_pointer(ctx, 0);
	if (win) destroy_window(win);
	return 0;
}

void jsopen_window(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jwindow_create, 3);
	duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, jwindow_destroy, 1);
	duk_put_prop_string(ctx, -2, "destroy");
	duk_put_global_string(ctx, "window");
}
