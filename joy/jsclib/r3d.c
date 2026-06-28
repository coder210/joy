#include "jsclib.h"
#include "../joy.h"

#define R3D_PTR(n)  ((jr3d_p)duk_get_pointer(ctx, n))
#define MSH_PTR(n)  ((render_mesh_p)duk_get_pointer(ctx, n))
#define NUM(n)      ((float)duk_to_number(ctx, n))
#define INT(n)      ((int)duk_to_number(ctx, n))
#define STR(n)      duk_to_string(ctx, n)

/* ---- helpers ---- */

static vec3_t read_vec3(duk_context* ctx, int idx)
{
	vec3_t v = { 0, 0, 0 };
	duk_get_prop_string(ctx, idx, "x"); v.x = (float)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, idx, "y"); v.y = (float)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, idx, "z"); v.z = (float)duk_to_number(ctx, -1); duk_pop(ctx);
	return v;
}

static jcolor_t read_color(duk_context* ctx, int idx)
{
	jcolor_t c = { 255, 255, 255, 255 };
	duk_get_prop_string(ctx, idx, "r"); c.r = (uint8_t)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, idx, "g"); c.g = (uint8_t)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, idx, "b"); c.b = (uint8_t)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, idx, "a"); if (duk_is_number(ctx, -1)) c.a = (uint8_t)duk_to_number(ctx, -1); duk_pop(ctx);
	return c;
}

/* ---- API ---- */

static duk_ret_t jr3d_create(duk_context* ctx)
{
	jwindow_p win = (jwindow_p)duk_get_pointer(ctx, 0);
	const char* dir = duk_to_string(ctx, 1);
	duk_push_pointer(ctx, r3d_create(win, dir));
	return 1;
}

static duk_ret_t jr3d_destroy(duk_context* ctx)
{
	r3d_destroy(R3D_PTR(0));
	duk_push_pointer(ctx, NULL);
	return 1;
}

static duk_ret_t jr3d_begin(duk_context* ctx)
{
	duk_push_boolean(ctx, r3d_begin(R3D_PTR(0)));
	return 1;
}

static duk_ret_t jr3d_end(duk_context* ctx)
{
	r3d_end(R3D_PTR(0));
	return 0;
}

static duk_ret_t jr3d_camera_lookat(duk_context* ctx)
{
	jr3d_p r = R3D_PTR(0);
	vec3_t eye    = read_vec3(ctx, 1);
	vec3_t target = read_vec3(ctx, 2);
	vec3_t up     = read_vec3(ctx, 3);
	float fovy    = NUM(4);
	r3d_camera_lookat(r, eye, target, up, fovy);
	return 0;
}

static duk_ret_t jr3d_camera_fps(duk_context* ctx)
{
	jr3d_p r = R3D_PTR(0);
	vec3_t pos   = read_vec3(ctx, 1);
	float  yaw   = NUM(2);
	float  pitch = NUM(3);
	float  fovy  = NUM(4);
	r3d_camera_fps(r, pos, yaw, pitch, fovy);
	return 0;
}

static duk_ret_t jr3d_mesh_cube(duk_context* ctx)
{
	jr3d_p r = R3D_PTR(0);
	float w = NUM(1), h = NUM(2), d = NUM(3);
	duk_push_pointer(ctx, r3d_mesh_cube(r, w, h, d));
	return 1;
}

static duk_ret_t jr3d_mesh_plane(duk_context* ctx)
{
	jr3d_p r = R3D_PTR(0);
	float w = NUM(1), d = NUM(2);
	int segs = INT(3);
	duk_push_pointer(ctx, r3d_mesh_plane(r, w, d, segs));
	return 1;
}

static duk_ret_t jr3d_mesh_sphere(duk_context* ctx)
{
	jr3d_p r = R3D_PTR(0);
	float radius = NUM(1);
	int slices = INT(2), stacks = INT(3);
	duk_push_pointer(ctx, r3d_mesh_sphere(r, radius, slices, stacks));
	return 1;
}

static duk_ret_t jr3d_mesh_destroy(duk_context* ctx)
{
	r3d_mesh_destroy(R3D_PTR(0), MSH_PTR(1));
	return 0;
}

static duk_ret_t jr3d_draw_mesh(duk_context* ctx)
{
	jr3d_p r = R3D_PTR(0);
	render_mesh_p m = MSH_PTR(1);
	vec3_t pos   = read_vec3(ctx, 2);
	vec3_t scale = read_vec3(ctx, 3);
	jcolor_t c  = read_color(ctx, 4);
	r3d_draw_mesh(r, m, pos, scale, c);
	return 0;
}

static duk_ret_t jr3d_mesh_ramp(duk_context* ctx)
{
	jr3d_p r = R3D_PTR(0);
	float w = NUM(1), l = NUM(2), h = NUM(3);
	duk_push_pointer(ctx, r3d_mesh_ramp(r, w, l, h));
	return 1;
}

static duk_ret_t jr3d_gen_checkerboard(duk_context* ctx)
{
	jr3d_p r = R3D_PTR(0);
	int ts = INT(1), tc = INT(2);
	duk_push_pointer(ctx, r3d_gen_checkerboard(r, ts, tc));
	return 1;
}

static duk_ret_t jr3d_draw_mesh_tex(duk_context* ctx)
{
	jr3d_p r = R3D_PTR(0);
	render_mesh_p m = MSH_PTR(1);
	vec3_t pos   = read_vec3(ctx, 2);
	vec3_t scale = read_vec3(ctx, 3);
	void* tex    = duk_get_pointer(ctx, 4);
	r3d_draw_mesh_tex(r, m, pos, scale, tex);
	return 0;
}

static duk_ret_t jr3d_draw_grid(duk_context* ctx)
{
	float lw = duk_is_number(ctx, 3) ? NUM(3) : NUM(2) * 0.01f;
	r3d_draw_grid(R3D_PTR(0), INT(1), NUM(2), lw);
	return 0;
}

/* ---- 模块注册 ---- */

void jsopen_render3d(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jr3d_create, 2);       duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, jr3d_destroy, 1);      duk_put_prop_string(ctx, -2, "destroy");
	duk_push_c_function(ctx, jr3d_begin, 1);         duk_put_prop_string(ctx, -2, "begin");
	duk_push_c_function(ctx, jr3d_end, 1);           duk_put_prop_string(ctx, -2, "end_frame");
	duk_push_c_function(ctx, jr3d_camera_lookat, 5); duk_put_prop_string(ctx, -2, "camera_lookat");
	duk_push_c_function(ctx, jr3d_camera_fps, 5);    duk_put_prop_string(ctx, -2, "camera_fps");
	duk_push_c_function(ctx, jr3d_mesh_cube, 4);     duk_put_prop_string(ctx, -2, "mesh_cube");
	duk_push_c_function(ctx, jr3d_mesh_sphere, 4);   duk_put_prop_string(ctx, -2, "mesh_sphere");
	duk_push_c_function(ctx, jr3d_mesh_plane, 4);    duk_put_prop_string(ctx, -2, "mesh_plane");
	duk_push_c_function(ctx, jr3d_mesh_ramp, 4);     duk_put_prop_string(ctx, -2, "mesh_ramp");
	duk_push_c_function(ctx, jr3d_gen_checkerboard, 3); duk_put_prop_string(ctx, -2, "gen_checkerboard");
	duk_push_c_function(ctx, jr3d_draw_mesh_tex, 5); duk_put_prop_string(ctx, -2, "draw_mesh_tex");
	duk_push_c_function(ctx, jr3d_mesh_destroy, 2);  duk_put_prop_string(ctx, -2, "mesh_destroy");
	duk_push_c_function(ctx, jr3d_draw_mesh, 5);     duk_put_prop_string(ctx, -2, "draw_mesh");
	duk_push_c_function(ctx, jr3d_draw_grid, 3);     duk_put_prop_string(ctx, -2, "draw_grid");
	duk_put_global_string(ctx, "render3d");
}
