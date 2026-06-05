#include <math.h>
#include "jsclib.h"
#include "../jmath.h"

static duk_ret_t jfp_from_float(duk_context* ctx)
{ duk_push_number(ctx, (duk_double_t)fp_from_float((float)duk_to_number(ctx,1))); return 1; }
static duk_ret_t jfp_to_float(duk_context* ctx)
{ duk_push_number(ctx, (duk_double_t)fp_to_float((fp_t)duk_to_number(ctx,1))); return 1; }
static duk_ret_t jfp_negate(duk_context* ctx)
{ duk_push_number(ctx, (duk_double_t)(-(fp_t)duk_to_number(ctx,1))); return 1; }
static duk_ret_t jfp_add(duk_context* ctx)
{ fp_t a=(fp_t)duk_to_number(ctx,1),b=(fp_t)duk_to_number(ctx,2); duk_push_number(ctx,(duk_double_t)fp_add(a,b)); return 1; }
static duk_ret_t jfp_sub(duk_context* ctx)
{ fp_t a=(fp_t)duk_to_number(ctx,1),b=(fp_t)duk_to_number(ctx,2); duk_push_number(ctx,(duk_double_t)fp_sub(a,b)); return 1; }
static duk_ret_t jfp_mul(duk_context* ctx)
{ fp_t a=(fp_t)duk_to_number(ctx,1),b=(fp_t)duk_to_number(ctx,2); duk_push_number(ctx,(duk_double_t)fp_mul(a,b)); return 1; }
static duk_ret_t jfp_divx(duk_context* ctx)
{ fp_t a=(fp_t)duk_to_number(ctx,1),b=(fp_t)duk_to_number(ctx,2); duk_push_number(ctx,(duk_double_t)fp_div(a,b)); return 1; }
static duk_ret_t jfp_sign(duk_context* ctx)
{ duk_push_number(ctx, (duk_double_t)(((fp_t)duk_to_number(ctx,1)) < 0 ? -1 : 1)); return 1; }
static duk_ret_t jfp_absx(duk_context* ctx)
{ duk_push_number(ctx, (duk_double_t)fp_abs((fp_t)duk_to_number(ctx,1))); return 1; }
static duk_ret_t jfp_floor(duk_context* ctx)
{ fp_t v=(fp_t)duk_to_number(ctx,1); duk_push_number(ctx,(duk_double_t)((v/FP_ONE)*FP_ONE)); return 1; }
static duk_ret_t jfp_sqrt(duk_context* ctx)
{ duk_push_number(ctx,(duk_double_t)fp_sqrt((fp_t)duk_to_number(ctx,1))); return 1; }
static duk_ret_t jfp_sin(duk_context* ctx)
{ duk_push_number(ctx,(duk_double_t)fp_sin((fp_t)duk_to_number(ctx,1))); return 1; }
static duk_ret_t jfp_cos(duk_context* ctx)
{ duk_push_number(ctx,(duk_double_t)fp_cos((fp_t)duk_to_number(ctx,1))); return 1; }
static duk_ret_t jfp_atan2(duk_context* ctx)
{ fp_t y=(fp_t)duk_to_number(ctx,1),x=(fp_t)duk_to_number(ctx,2); duk_push_number(ctx,(duk_double_t)fp_from_float(atan2f(fp_to_float(y),fp_to_float(x)))); return 1; }
static duk_ret_t jfp_max(duk_context* ctx)
{ fp_t a=(fp_t)duk_to_number(ctx,1),b=(fp_t)duk_to_number(ctx,2); duk_push_number(ctx,(duk_double_t)(a>b?a:b)); return 1; }
static duk_ret_t jfp_min(duk_context* ctx)
{ fp_t a=(fp_t)duk_to_number(ctx,1),b=(fp_t)duk_to_number(ctx,2); duk_push_number(ctx,(duk_double_t)(a<b?a:b)); return 1; }
static duk_ret_t jfp_zero(duk_context* ctx)
{ duk_push_number(ctx,(duk_double_t)fp_zero()); return 1; }
static duk_ret_t jfp_half(duk_context* ctx)
{ duk_push_number(ctx,(duk_double_t)fp_half()); return 1; }
static duk_ret_t jfp_one(duk_context* ctx)
{ duk_push_number(ctx,(duk_double_t)fp_one()); return 1; }
static duk_ret_t jfp_pi(duk_context* ctx)
{ duk_push_number(ctx,(duk_double_t)FP_PI); return 1; }
static duk_ret_t jfp_min_value(duk_context* ctx)
{ duk_push_number(ctx,(duk_double_t)FP_MIN_VALUE); return 1; }
static duk_ret_t jfp_max_value(duk_context* ctx)
{ duk_push_number(ctx,(duk_double_t)FP_MAX_VALUE); return 1; }

void jsopen_fp(duk_context* ctx)
{
	duk_push_object(ctx);
#define REG(n,a) duk_push_c_function(ctx,jfp_##n,a); duk_put_prop_string(ctx,-2,#n)
	REG(zero,0); REG(half,0); REG(one,0); REG(pi,0);
	REG(min_value,0); REG(max_value,0);
	REG(from_float,1); REG(to_float,1); REG(negate,1); REG(sign,1);
	REG(absx,1); REG(floor,1); REG(sqrt,1); REG(sin,1);
	REG(cos,1); REG(atan2,2); REG(add,2); REG(sub,2);
	REG(mul,2); REG(divx,2); REG(max,2); REG(min,2);
#undef REG
	duk_put_global_string(ctx, "fp");
}