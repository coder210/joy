#include <SDL3/SDL.h>
#include "jsclib.h"

static duk_ret_t jsdl_get_num_video_drivers(duk_context* ctx)
{
	duk_push_number(ctx, (duk_double_t)SDL_GetNumVideoDrivers());
	return 1;
}

static duk_ret_t jsdl_get_current_video_driver(duk_context* ctx)
{
	duk_push_string(ctx, SDL_GetCurrentVideoDriver());
	return 1;
}

void jsopen_sdl(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jsdl_get_num_video_drivers, 0);    duk_put_prop_string(ctx, -2, "get_num_video_drivers");
	duk_push_c_function(ctx, jsdl_get_current_video_driver, 0); duk_put_prop_string(ctx, -2, "get_current_video_driver");
	duk_put_global_string(ctx, "sdl");
}