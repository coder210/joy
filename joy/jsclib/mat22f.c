#include "jsclib.h"
#include "../jmath.h"

static duk_ret_t jm22f_new(duk_context* ctx) { mat22f_t* m=(mat22f_t*)malloc(sizeof(mat22f_t)); m->col1.x=(fp_t)duk_to_number(ctx,1); m->col2.x=(fp_t)duk_to_number(ctx,2); m->col1.y=(fp_t)duk_to_number(ctx,3); m->col2.y=(fp_t)duk_to_number(ctx,4); duk_push_pointer(ctx,m); return 1; }
static duk_ret_t jm22f_destroy(duk_context* ctx) { free(duk_get_pointer(ctx,1)); return 0; }

void jsopen_mat22f(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jm22f_new,4); duk_put_prop_string(ctx,-2,"new");
	duk_push_c_function(ctx, jm22f_destroy,1); duk_put_prop_string(ctx,-2,"destroy");
	duk_put_global_string(ctx, "mat22f");
}