#include "jsclib.h"
#include "../jmath.h"

static duk_ret_t jv3f_new(duk_context* ctx) { vec3f_t* v=(vec3f_t*)malloc(sizeof(vec3f_t)); v->x=(fp_t)duk_to_number(ctx,1); v->y=(fp_t)duk_to_number(ctx,2); v->z=(fp_t)duk_to_number(ctx,3); duk_push_pointer(ctx,v); return 1; }
static duk_ret_t jv3f_destroy(duk_context* ctx) { free(duk_get_pointer(ctx,1)); return 0; }
static duk_ret_t jv3f_get_x(duk_context* ctx) { vec3f_t* v=(vec3f_t*)duk_get_pointer(ctx,1); duk_push_number(ctx,(duk_double_t)v->x); return 1; }
static duk_ret_t jv3f_get_y(duk_context* ctx) { vec3f_t* v=(vec3f_t*)duk_get_pointer(ctx,1); duk_push_number(ctx,(duk_double_t)v->y); return 1; }
static duk_ret_t jv3f_get_z(duk_context* ctx) { vec3f_t* v=(vec3f_t*)duk_get_pointer(ctx,1); duk_push_number(ctx,(duk_double_t)v->z); return 1; }
static duk_ret_t jv3f_set_x(duk_context* ctx) { vec3f_t* v=(vec3f_t*)duk_get_pointer(ctx,1); v->x=(fp_t)duk_to_number(ctx,2); return 0; }
static duk_ret_t jv3f_set_y(duk_context* ctx) { vec3f_t* v=(vec3f_t*)duk_get_pointer(ctx,1); v->y=(fp_t)duk_to_number(ctx,2); return 0; }
static duk_ret_t jv3f_set_z(duk_context* ctx) { vec3f_t* v=(vec3f_t*)duk_get_pointer(ctx,1); v->z=(fp_t)duk_to_number(ctx,2); return 0; }

void jsopen_vec3f(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jv3f_new,3); duk_put_prop_string(ctx,-2,"new");
	duk_push_c_function(ctx, jv3f_destroy,1); duk_put_prop_string(ctx,-2,"destroy");
	duk_push_c_function(ctx, jv3f_get_x,1); duk_put_prop_string(ctx,-2,"get_x");
	duk_push_c_function(ctx, jv3f_get_y,1); duk_put_prop_string(ctx,-2,"get_y");
	duk_push_c_function(ctx, jv3f_get_z,1); duk_put_prop_string(ctx,-2,"get_z");
	duk_push_c_function(ctx, jv3f_set_x,2); duk_put_prop_string(ctx,-2,"set_x");
	duk_push_c_function(ctx, jv3f_set_y,2); duk_put_prop_string(ctx,-2,"set_y");
	duk_push_c_function(ctx, jv3f_set_z,2); duk_put_prop_string(ctx,-2,"set_z");
	duk_put_global_string(ctx, "vec3f");
}