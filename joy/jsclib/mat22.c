#include "jsclib.h"
#include "../jmath.h"

static duk_ret_t jm22_new(duk_context* ctx) { mat22_t* m=(mat22_t*)malloc(sizeof(mat22_t)); m->col1.x=(ft_t)duk_to_number(ctx,1); m->col2.x=(ft_t)duk_to_number(ctx,2); m->col1.y=(ft_t)duk_to_number(ctx,3); m->col2.y=(ft_t)duk_to_number(ctx,4); duk_push_pointer(ctx,m); return 1; }
static duk_ret_t jm22_destroy(duk_context* ctx) { free(duk_get_pointer(ctx,1)); return 0; }

void jsopen_mat22(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jm22_new,4); duk_put_prop_string(ctx,-2,"new");
	duk_push_c_function(ctx, jm22_destroy,1); duk_put_prop_string(ctx,-2,"destroy");
	duk_put_global_string(ctx, "mat22");
}