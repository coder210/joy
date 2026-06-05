#include <assert.h>
#include <stdlib.h>
#include "jsclib.h"
#include "../jcollision.h"

static circlef_t read_circlef(duk_context* ctx, int idx)
{
	circlef_t c = { {0,0}, 0 };
	duk_get_prop_string(ctx, idx, "x"); c.center.x = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, idx, "y"); c.center.y = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, idx, "r"); c.radius = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
	return c;
}

static rectanglef_t read_rectf(duk_context* ctx, int idx)
{
	rectanglef_t r = {0,0,0,0};
	duk_get_prop_string(ctx, idx, "x"); r.x = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, idx, "y"); r.y = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, idx, "w"); r.width = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, idx, "h"); r.height = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
	return r;
}

static duk_ret_t jc2df_circles(duk_context* ctx)
{
	circlef_t a = read_circlef(ctx, 1), b = read_circlef(ctx, 2);
	duk_push_boolean(ctx, collision2df_check_circles(a, b));
	return 1;
}

static duk_ret_t jc2df_rectangles(duk_context* ctx)
{
	rectanglef_t a = read_rectf(ctx, 1), b = read_rectf(ctx, 2);
	duk_push_boolean(ctx, collision2df_check_rectangles(a, b));
	return 1;
}

static void push_contact2df(duk_context* ctx, contact2df_t* c)
{
	duk_push_object(ctx);
	duk_push_object(ctx); duk_push_number(ctx, (duk_double_t)c->sp.x); duk_put_prop_string(ctx, -2, "x");
	                      duk_push_number(ctx, (duk_double_t)c->sp.y); duk_put_prop_string(ctx, -2, "y"); duk_put_prop_string(ctx, -2, "sp");
	duk_push_object(ctx); duk_push_number(ctx, (duk_double_t)c->ep.x); duk_put_prop_string(ctx, -2, "x");
	                      duk_push_number(ctx, (duk_double_t)c->ep.y); duk_put_prop_string(ctx, -2, "y"); duk_put_prop_string(ctx, -2, "ep");
	duk_push_number(ctx, (duk_double_t)c->depth); duk_put_prop_string(ctx, -2, "depth");
}

static duk_ret_t jc2df_get_ray_rectangle(duk_context* ctx)
{
	ray2df_t ray;
	duk_get_prop_string(ctx, 1, "x"); ray.origin.x = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, 1, "y"); ray.origin.y = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, 2, "x"); ray.direction.x = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, 2, "y"); ray.direction.y = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
	rectanglef_t rect = read_rectf(ctx, 3);
	ray2d_collisionf_t result = collision2df_get_ray_rectangle(ray, rect);
	duk_push_boolean(ctx, result.hit);
	if (result.hit) {
		duk_push_object(ctx);
		duk_push_number(ctx, (duk_double_t)result.point.x); duk_put_prop_string(ctx, -2, "x");
		duk_push_number(ctx, (duk_double_t)result.point.y); duk_put_prop_string(ctx, -2, "y");
		duk_push_number(ctx, (duk_double_t)result.distance); duk_put_prop_string(ctx, -2, "distance");
	} else {
		duk_push_undefined(ctx);
	}
	return 2;
}

static duk_ret_t jc2df_polygons(duk_context* ctx)
{
	duk_push_boolean(ctx, 0);
	return 1;
}

void jsopen_collision2df(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jc2df_circles, 2);           duk_put_prop_string(ctx, -2, "circles");
	duk_push_c_function(ctx, jc2df_rectangles, 2);        duk_put_prop_string(ctx, -2, "rectangles");
	duk_push_c_function(ctx, jc2df_get_ray_rectangle, 3); duk_put_prop_string(ctx, -2, "get_ray_rectangle");
	duk_push_c_function(ctx, jc2df_polygons, 2);          duk_put_prop_string(ctx, -2, "polygons");
	duk_put_global_string(ctx, "collision2df");
}