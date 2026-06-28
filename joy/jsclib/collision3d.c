#include "../joy.h"
#include "jsclib.h"

static vec3f_t read_vec3f(duk_context* ctx, int idx) { vec3f_t v; duk_get_prop_string(ctx,idx,"x"); v.x=(fp_t)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,idx,"y"); v.y=(fp_t)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,idx,"z"); v.z=(fp_t)duk_to_number(ctx,-1); duk_pop(ctx); return v; }
/* ray_quad_int: 返回 {hit:bool, distance:n} */
static duk_ret_t jc3d_ray_quad_int(duk_context* ctx) { ray3df_t ray; ray.position=read_vec3f(ctx,0); ray.direction=read_vec3f(ctx,1); vec3f_t v0=read_vec3f(ctx,2),v1=read_vec3f(ctx,3),v2=read_vec3f(ctx,4),v3=read_vec3f(ctx,5); ray3d_collisionf_t result=collision3df_get_ray_quad(ray,v0,v1,v2,v3); duk_push_object(ctx); duk_push_boolean(ctx,result.hit); duk_put_prop_string(ctx,-2,"hit"); duk_push_number(ctx,(duk_double_t)result.distance); duk_put_prop_string(ctx,-2,"distance"); return 1; }
void jsopen_collision3d(duk_context* ctx) { duk_push_object(ctx); duk_push_c_function(ctx,jc3d_ray_quad_int,6); duk_put_prop_string(ctx,-2,"ray_quad_int"); duk_put_global_string(ctx,"collision3d"); }
