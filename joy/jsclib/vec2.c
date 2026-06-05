#include "jsclib.h"
#include "../jmath.h"

static duk_ret_t jv2_new(duk_context* ctx) { vec2_t* v=(vec2_t*)malloc(sizeof(vec2_t)); v->x=(ft_t)duk_to_number(ctx,0); v->y=(ft_t)duk_to_number(ctx,1); duk_push_pointer(ctx,v); return 1; }
static duk_ret_t jv2_destroy(duk_context* ctx) { free(duk_get_pointer(ctx,0)); return 0; }
static duk_ret_t jv2_get_x(duk_context* ctx) { vec2_t* v=(vec2_t*)duk_get_pointer(ctx,0); duk_push_number(ctx,(duk_double_t)v->x); return 1; }
static duk_ret_t jv2_get_y(duk_context* ctx) { vec2_t* v=(vec2_t*)duk_get_pointer(ctx,0); duk_push_number(ctx,(duk_double_t)v->y); return 1; }
static duk_ret_t jv2_set_x(duk_context* ctx) { vec2_t* v=(vec2_t*)duk_get_pointer(ctx,0); v->x=(ft_t)duk_to_number(ctx,1); return 0; }
static duk_ret_t jv2_set_y(duk_context* ctx) { vec2_t* v=(vec2_t*)duk_get_pointer(ctx,0); v->y=(ft_t)duk_to_number(ctx,1); return 0; }

void jsopen_vec2(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jv2_new,2); duk_put_prop_string(ctx,-2,"new");
	duk_push_c_function(ctx, jv2_destroy,1); duk_put_prop_string(ctx,-2,"destroy");
	duk_push_c_function(ctx, jv2_get_x,1); duk_put_prop_string(ctx,-2,"get_x");
	duk_push_c_function(ctx, jv2_get_y,1); duk_put_prop_string(ctx,-2,"get_y");
	duk_push_c_function(ctx, jv2_set_x,2); duk_put_prop_string(ctx,-2,"set_x");
	duk_push_c_function(ctx, jv2_set_y,2); duk_put_prop_string(ctx,-2,"set_y");
	duk_put_global_string(ctx, "vec2");
}