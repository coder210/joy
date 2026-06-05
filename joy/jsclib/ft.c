#include "jsclib.h"
#include "../jmath.h"

static duk_ret_t jft_new(duk_context* ctx)
{
	ft_t* v = (ft_t*)malloc(2 * sizeof(ft_t));
	v[0] = (ft_t)duk_to_number(ctx, 0);
	v[1] = (ft_t)duk_to_number(ctx, 1);
	duk_push_pointer(ctx, v);
	return 1;
}
static duk_ret_t jft_destroy(duk_context* ctx) { free(duk_get_pointer(ctx, 0)); return 0; }
static duk_ret_t jft_get_x(duk_context* ctx) { ft_t* v = (ft_t*)duk_get_pointer(ctx, 0); duk_push_number(ctx, (duk_double_t)v[0]); return 1; }
static duk_ret_t jft_get_y(duk_context* ctx) { ft_t* v = (ft_t*)duk_get_pointer(ctx, 0); duk_push_number(ctx, (duk_double_t)v[1]); return 1; }
static duk_ret_t jft_set_x(duk_context* ctx) { ft_t* v = (ft_t*)duk_get_pointer(ctx, 0); v[0] = (ft_t)duk_to_number(ctx, 1); return 0; }
static duk_ret_t jft_set_y(duk_context* ctx) { ft_t* v = (ft_t*)duk_get_pointer(ctx, 0); v[1] = (ft_t)duk_to_number(ctx, 1); return 0; }

void jsopen_ft(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jft_new,2); duk_put_prop_string(ctx,-2,"new");
	duk_push_c_function(ctx, jft_destroy,1); duk_put_prop_string(ctx,-2,"destroy");
	duk_push_c_function(ctx, jft_get_x,1); duk_put_prop_string(ctx,-2,"get_x");
	duk_push_c_function(ctx, jft_get_y,1); duk_put_prop_string(ctx,-2,"get_y");
	duk_push_c_function(ctx, jft_set_x,2); duk_put_prop_string(ctx,-2,"set_x");
	duk_push_c_function(ctx, jft_set_y,2); duk_put_prop_string(ctx,-2,"set_y");
	duk_put_global_string(ctx, "ft");
}