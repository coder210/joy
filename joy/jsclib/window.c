#include "jsclib.h"
#include <stdlib.h>
#include <SDL3/SDL.h>

static duk_ret_t jwindow_create(duk_context* ctx)
{
	const char* title = duk_to_string(ctx, 0);
	int w = (int)duk_to_number(ctx, 1);
	int h = (int)duk_to_number(ctx, 2);
	int flags = (int)duk_to_number(ctx, 3);
	if (duk_is_undefined(ctx, 3)) flags = 0;
	SDL_Window* win = SDL_CreateWindow(title, w, h, flags);
	if (!win) { duk_push_undefined(ctx); return 1; }
	duk_push_pointer(ctx, win);
	return 1;
}

static duk_ret_t jwindow_destroy(duk_context* ctx)
{
	SDL_Window* win = (SDL_Window*)duk_get_pointer(ctx, 0);
	if (win) SDL_DestroyWindow(win);
	return 0;
}

void jsopen_window(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jwindow_create, 4);
	duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, jwindow_destroy, 1);
	duk_put_prop_string(ctx, -2, "destroy");
	duk_put_global_string(ctx, "window");
}