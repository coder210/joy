#include <assert.h>
#include <stdlib.h>
#include "jsclib.h"
#include "../jcollision.h"

static circlef_t read_circlef(duk_context* ctx, int idx) { circlef_t c={{0,0},0}; duk_get_prop_string(ctx,idx,"x"); c.center.x=(fp_t)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,idx,"y"); c.center.y=(fp_t)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,idx,"r"); c.radius=(fp_t)duk_to_number(ctx,-1); duk_pop(ctx); return c; }
static rectanglef_t read_rectf(duk_context* ctx, int idx) { rectanglef_t r={0,0,0,0}; duk_get_prop_string(ctx,idx,"x"); r.x=(fp_t)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,idx,"y"); r.y=(fp_t)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,idx,"w"); r.width=(fp_t)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,idx,"h"); r.height=(fp_t)duk_to_number(ctx,-1); duk_pop(ctx); return r; }

static polygonf_t read_polyf(duk_context* ctx, int idx) {
	int n = (int)duk_get_length(ctx, idx);
	polygonf_t p;
	p.num_vertices = n;
	p.vertices = (vec2f_t*)malloc(sizeof(vec2f_t) * n);
	assert(p.vertices);
	for (int i = 0; i < n; i++) {
		duk_get_prop_index(ctx, idx, (duk_uarridx_t)i);
		duk_get_prop_string(ctx, -1, "x"); p.vertices[i].x = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
		duk_get_prop_string(ctx, -1, "y"); p.vertices[i].y = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
		duk_pop(ctx);
	}
	return p;
}
static void free_polyf(polygonf_t p) { if (p.vertices) free(p.vertices); }

static duk_ret_t jc2df_circles(duk_context* ctx) { circlef_t a=read_circlef(ctx,0),b=read_circlef(ctx,1); duk_push_boolean(ctx,collision2df_check_circles(a,b)); return 1; }
static duk_ret_t jc2df_rectangles(duk_context* ctx) { rectanglef_t a=read_rectf(ctx,0),b=read_rectf(ctx,1); duk_push_boolean(ctx,collision2df_check_rectangles(a,b)); return 1; }

/* 推入 contact 对象 {sp:{x,y}, ep:{x,y}, normal:{x,y}, depth:n} */
static void push_contact2df_obj(duk_context* ctx, contact2df_t* c) {
	duk_push_object(ctx);
	duk_push_object(ctx);
	duk_push_number(ctx,(duk_double_t)c->sp.x); duk_put_prop_string(ctx,-2,"x");
	duk_push_number(ctx,(duk_double_t)c->sp.y); duk_put_prop_string(ctx,-2,"y");
	duk_put_prop_string(ctx,-2,"sp");
	duk_push_object(ctx);
	duk_push_number(ctx,(duk_double_t)c->ep.x); duk_put_prop_string(ctx,-2,"x");
	duk_push_number(ctx,(duk_double_t)c->ep.y); duk_put_prop_string(ctx,-2,"y");
	duk_put_prop_string(ctx,-2,"ep");
	duk_push_object(ctx);
	duk_push_number(ctx,(duk_double_t)c->normal.x); duk_put_prop_string(ctx,-2,"x");
	duk_push_number(ctx,(duk_double_t)c->normal.y); duk_put_prop_string(ctx,-2,"y");
	duk_put_prop_string(ctx,-2,"normal");
	duk_push_number(ctx,(duk_double_t)c->depth);
	duk_put_prop_string(ctx,-2,"depth");
}

/* 推入结果对象 {ok:bool, contact:{...}} */
static void push_result(duk_context* ctx, int ok, contact2df_t* c) {
	duk_push_object(ctx);
	duk_push_boolean(ctx, ok);
	duk_put_prop_string(ctx, -2, "ok");
	if (ok) {
		push_contact2df_obj(ctx, c);
		duk_put_prop_string(ctx, -2, "contact");
	}
}

static duk_ret_t jc2df_circlesx(duk_context* ctx) {
	circlef_t a=read_circlef(ctx,0),b=read_circlef(ctx,1);
	contact2df_t contact;
	int ok = collision2df_get_circles(a,b,&contact);
	push_result(ctx, ok, &contact);
	return 1;
}

static duk_ret_t jc2df_rectanglesx(duk_context* ctx) {
	rectanglef_t a=read_rectf(ctx,0);
	fp_t a_angle=(fp_t)duk_to_number(ctx,1);
	rectanglef_t b=read_rectf(ctx,2);
	fp_t b_angle=(fp_t)duk_to_number(ctx,3);
	contact2df_t contact;
	int ok = collision2df_get_rectangles(a,a_angle,b,b_angle,&contact);
	push_result(ctx, ok, &contact);
	return 1;
}

static duk_ret_t jc2df_polygons(duk_context* ctx) {
	polygonf_t a=read_polyf(ctx,0),b=read_polyf(ctx,1);
	contact2df_t contact;
	int ok = collision2df_get_polygons(a,b,&contact);
	free_polyf(a); free_polyf(b);
	push_result(ctx, ok, &contact);
	return 1;
}

/* get_ray_rectangle: 返回 {hit:bool, point:{x,y}, distance:n} */
static duk_ret_t jc2df_get_ray_rectangle(duk_context* ctx) {
	ray2df_t ray; duk_get_prop_string(ctx,0,"x"); ray.origin.x=(fp_t)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,0,"y"); ray.origin.y=(fp_t)duk_to_number(ctx,-1); duk_pop(ctx);
	duk_get_prop_string(ctx,1,"x"); ray.direction.x=(fp_t)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,1,"y"); ray.direction.y=(fp_t)duk_to_number(ctx,-1); duk_pop(ctx);
	rectanglef_t rect=read_rectf(ctx,2); ray2d_collisionf_t result=collision2df_get_ray_rectangle(ray,rect);
	duk_push_object(ctx);
	duk_push_boolean(ctx,result.hit); duk_put_prop_string(ctx,-2,"hit");
	if(result.hit){ duk_push_object(ctx); duk_push_number(ctx,(duk_double_t)result.point.x); duk_put_prop_string(ctx,-2,"x"); duk_push_number(ctx,(duk_double_t)result.point.y); duk_put_prop_string(ctx,-2,"y"); duk_put_prop_string(ctx,-2,"point"); }
	duk_push_number(ctx,(duk_double_t)result.distance); duk_put_prop_string(ctx,-2,"distance");
	return 1;
}

void jsopen_collision2df(duk_context* ctx) {
	duk_push_object(ctx);
	duk_push_c_function(ctx,jc2df_circles,2); duk_put_prop_string(ctx,-2,"circles");
	duk_push_c_function(ctx,jc2df_rectangles,2); duk_put_prop_string(ctx,-2,"rectangles");
	duk_push_c_function(ctx,jc2df_circlesx,2); duk_put_prop_string(ctx,-2,"circlesx");
	duk_push_c_function(ctx,jc2df_rectanglesx,4); duk_put_prop_string(ctx,-2,"rectanglesx");
	duk_push_c_function(ctx,jc2df_polygons,2); duk_put_prop_string(ctx,-2,"polygons");
	duk_push_c_function(ctx,jc2df_get_ray_rectangle,3); duk_put_prop_string(ctx,-2,"get_ray_rectangle");
	duk_put_global_string(ctx,"collision2df");
}
