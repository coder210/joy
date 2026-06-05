#include <assert.h>
#include <stdlib.h>
#include "jsclib.h"
#include "../jcollision.h"

static circle_t read_circle(duk_context* ctx, int idx) { circle_t c={{0,0},0}; duk_get_prop_string(ctx,idx,"x"); c.center.x=(int)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,idx,"y"); c.center.y=(int)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,idx,"r"); c.radius=(int)duk_to_number(ctx,-1); duk_pop(ctx); return c; }
static rectangle_t read_rect(duk_context* ctx, int idx) { rectangle_t r={0,0,0,0}; duk_get_prop_string(ctx,idx,"x"); r.x=(int)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,idx,"y"); r.y=(int)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,idx,"w"); r.width=(int)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,idx,"h"); r.height=(int)duk_to_number(ctx,-1); duk_pop(ctx); return r; }
static polygon_t read_poly(duk_context* ctx, int idx) { polygon_t p; p.num_vertices=(int)duk_get_length(ctx,idx); p.vertices=(vec2_t*)malloc(sizeof(vec2_t)*p.num_vertices); assert(p.vertices); for(int i=0;i<p.num_vertices;i++){ duk_get_prop_index(ctx,idx,(duk_uarridx_t)i); duk_get_prop_string(ctx,-1,"x"); p.vertices[i].x=(int)duk_to_number(ctx,-1); duk_pop(ctx); duk_get_prop_string(ctx,-1,"y"); p.vertices[i].y=(int)duk_to_number(ctx,-1); duk_pop(ctx); duk_pop(ctx); } return p; }
static void free_poly(polygon_t p) { if(p.vertices) free(p.vertices); }
static void push_contact(duk_context* ctx, contact2d_t* c) { duk_push_object(ctx); duk_push_object(ctx); duk_push_number(ctx,(duk_double_t)c->sp.x); duk_put_prop_string(ctx,-2,"x"); duk_push_number(ctx,(duk_double_t)c->sp.y); duk_put_prop_string(ctx,-2,"y"); duk_put_prop_string(ctx,-2,"sp"); duk_push_object(ctx); duk_push_number(ctx,(duk_double_t)c->ep.x); duk_put_prop_string(ctx,-2,"x"); duk_push_number(ctx,(duk_double_t)c->ep.y); duk_put_prop_string(ctx,-2,"y"); duk_put_prop_string(ctx,-2,"ep"); duk_push_number(ctx,(duk_double_t)c->depth); duk_put_prop_string(ctx,-2,"depth"); }

static duk_ret_t jc2d_circles(duk_context* ctx) { circle_t a=read_circle(ctx,0),b=read_circle(ctx,1); duk_push_boolean(ctx,collision2d_check_circles(a,b)); return 1; }
static duk_ret_t jc2d_rectangles(duk_context* ctx) { rectangle_t a=read_rect(ctx,0),b=read_rect(ctx,1); duk_push_boolean(ctx,collision2d_check_rectangles(a,b)); return 1; }

static void push_result(duk_context* ctx, int ok, contact2d_t* c) {
	duk_push_object(ctx);
	duk_push_boolean(ctx, ok);
	duk_put_prop_string(ctx, -2, "ok");
	if (ok) { push_contact(ctx, c); duk_put_prop_string(ctx, -2, "contact"); }
}

static duk_ret_t jc2d_circlesx(duk_context* ctx) { circle_t a=read_circle(ctx,0),b=read_circle(ctx,1); contact2d_t contact; int ok=collision2d_get_circles(a,b,&contact); push_result(ctx, ok, &contact); return 1; }
static duk_ret_t jc2d_rectanglesx(duk_context* ctx) { rectangle_t a=read_rect(ctx,0); int a_angle=(int)duk_to_number(ctx,1); rectangle_t b=read_rect(ctx,2); int b_angle=(int)duk_to_number(ctx,3); contact2d_t contact; int ok=collision2d_get_rectangles(a,a_angle,b,b_angle,&contact); push_result(ctx, ok, &contact); return 1; }
static duk_ret_t jc2d_polygons(duk_context* ctx) { polygon_t a=read_poly(ctx,0),b=read_poly(ctx,1); int ok=collision2d_check_polygons(a,b); free_poly(a); free_poly(b); duk_push_boolean(ctx,ok); return 1; }
static duk_ret_t jc2d_polygonsx(duk_context* ctx) { polygon_t a=read_poly(ctx,0),b=read_poly(ctx,1); contact2d_t contact; int ok=collision2d_get_polygons(a,b,&contact); free_poly(a); free_poly(b); push_result(ctx, ok, &contact); return 1; }

void jsopen_collision2d(duk_context* ctx) {
	duk_push_object(ctx);
	duk_push_c_function(ctx,jc2d_circles,2); duk_put_prop_string(ctx,-2,"circles");
	duk_push_c_function(ctx,jc2d_rectangles,2); duk_put_prop_string(ctx,-2,"rectangles");
	duk_push_c_function(ctx,jc2d_circlesx,2); duk_put_prop_string(ctx,-2,"circlesx");
	duk_push_c_function(ctx,jc2d_rectanglesx,4); duk_put_prop_string(ctx,-2,"rectanglesx");
	duk_push_c_function(ctx,jc2d_polygons,2); duk_put_prop_string(ctx,-2,"polygons");
	duk_push_c_function(ctx,jc2d_polygonsx,2); duk_put_prop_string(ctx,-2,"polygonsx");
	duk_put_global_string(ctx, "collision2d");
}
