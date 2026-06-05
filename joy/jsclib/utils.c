#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL3/SDL.h>
#include "jsclib.h"
#include "../jutils.h"

static duk_ret_t jutils_now(duk_context* ctx)
{ duk_push_number(ctx, (duk_double_t)SDL_GetTicks()); return 1; }

static duk_ret_t jutils_milliseconds(duk_context* ctx)
{ duk_push_number(ctx, (duk_double_t)SDL_GetTicks()); return 1; }

static duk_ret_t jutils_rand(duk_context* ctx)
{ float min=(float)duk_to_number(ctx,0),max=(float)duk_to_number(ctx,1); duk_push_number(ctx,(duk_double_t)(min+((float)rand()/(float)RAND_MAX)*(max-min))); return 1; }

static duk_ret_t jutils_rand_int(duk_context* ctx)
{ int min=(int)duk_to_number(ctx,0),max=(int)duk_to_number(ctx,1); duk_push_number(ctx,(duk_double_t)(min+rand()%(max-min+1))); return 1; }

static duk_ret_t jutils_clamp(duk_context* ctx)
{ float v=(float)duk_to_number(ctx,0),min=(float)duk_to_number(ctx,1),max=(float)duk_to_number(ctx,2); duk_push_number(ctx,(duk_double_t)(v<min?min:v>max?max:v)); return 1; }

static duk_ret_t jutils_lerp(duk_context* ctx)
{ float a=(float)duk_to_number(ctx,0),b=(float)duk_to_number(ctx,1),t=(float)duk_to_number(ctx,2); duk_push_number(ctx,(duk_double_t)(a+(b-a)*t)); return 1; }

static duk_ret_t jutils_read_file(duk_context* ctx)
{
	const char* path = duk_to_string(ctx, 0);
	size_t sz;
	char* data = utils_read_file(path, &sz);
	if (data) { duk_push_string(ctx, data); SDL_free(data); }
	else { duk_push_null(ctx); }
	return 1;
}

void jsopen_utils(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jutils_now, 0);          duk_put_prop_string(ctx, -2, "now");
	duk_push_c_function(ctx, jutils_milliseconds, 0); duk_put_prop_string(ctx, -2, "milliseconds");
	duk_push_c_function(ctx, jutils_rand, 2);         duk_put_prop_string(ctx, -2, "rand");
	duk_push_c_function(ctx, jutils_rand_int, 2);     duk_put_prop_string(ctx, -2, "rand_int");
	duk_push_c_function(ctx, jutils_clamp, 3);        duk_put_prop_string(ctx, -2, "clamp");
	duk_push_c_function(ctx, jutils_lerp, 3);         duk_put_prop_string(ctx, -2, "lerp");
	duk_push_c_function(ctx, jutils_read_file, 1);    duk_put_prop_string(ctx, -2, "read_file");
	duk_put_global_string(ctx, "utils");
}