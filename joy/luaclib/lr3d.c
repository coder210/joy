#include "luaclib.h"
#include "../external/lua/lauxlib.h"
#include "../joy.h"

/* ---- helpers ---- */

static vec3_t read_vec3(lua_State* L, int idx)
{
	vec3_t v = { 0, 0, 0 };
	lua_getfield(L, idx, "x"); v.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
	lua_getfield(L, idx, "y"); v.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
	lua_getfield(L, idx, "z"); v.z = (float)lua_tonumber(L, -1); lua_pop(L, 1);
	return v;
}

static jcolor_t read_color(lua_State* L, int idx)
{
	jcolor_t c = { 255, 255, 255, 255 };
	lua_getfield(L, idx, "r"); c.r = (uint8_t)lua_tointeger(L, -1); lua_pop(L, 1);
	lua_getfield(L, idx, "g"); c.g = (uint8_t)lua_tointeger(L, -1); lua_pop(L, 1);
	lua_getfield(L, idx, "b"); c.b = (uint8_t)lua_tointeger(L, -1); lua_pop(L, 1);
	lua_getfield(L, idx, "a"); if (!lua_isnil(L, -1)) c.a = (uint8_t)lua_tointeger(L, -1); lua_pop(L, 1);
	return c;
}

/* ---- API ---- */

static int l_create(lua_State* L)
{
	jwindow_p win = (jwindow_p)lua_touserdata(L, 1);
	const char* shader_dir = luaL_checkstring(L, 2);
	jr3d_p r = r3d_create(win, shader_dir);
	if (!r) luaL_error(L, "render3d.create failed");
	lua_pushlightuserdata(L, r);
	return 1;
}

static int l_destroy(lua_State* L)
{
	jr3d_p r = (jr3d_p)lua_touserdata(L, 1);
	if (r) r3d_destroy(r);
	return 0;
}

static int l_begin(lua_State* L)
{
	jr3d_p r = (jr3d_p)lua_touserdata(L, 1);
	lua_pushboolean(L, r ? r3d_begin(r) : 0);
	return 1;
}

static int l_end(lua_State* L)
{
	jr3d_p r = (jr3d_p)lua_touserdata(L, 1);
	if (r) r3d_end(r);
	return 0;
}

static int l_camera_lookat(lua_State* L)
{
	jr3d_p r = (jr3d_p)lua_touserdata(L, 1);
	vec3_t eye    = read_vec3(L, 2);
	vec3_t target = read_vec3(L, 3);
	vec3_t up     = read_vec3(L, 4);
	float  fovy   = (float)luaL_checknumber(L, 5);
	r3d_camera_lookat(r, eye, target, up, fovy);
	return 0;
}

static int l_camera_fps(lua_State* L)
{
	jr3d_p r = (jr3d_p)lua_touserdata(L, 1);
	vec3_t pos = read_vec3(L, 2);
	float yaw  = (float)luaL_checknumber(L, 3);
	float pitch = (float)luaL_checknumber(L, 4);
	float fovy = (float)luaL_checknumber(L, 5);
	r3d_camera_fps(r, pos, yaw, pitch, fovy);
	return 0;
}

static int l_mesh_cube(lua_State* L)
{
	jr3d_p r = (jr3d_p)lua_touserdata(L, 1);
	float w = (float)luaL_checknumber(L, 2);
	float h = (float)luaL_checknumber(L, 3);
	float d = (float)luaL_checknumber(L, 4);
	lua_pushlightuserdata(L, r3d_mesh_cube(r, w, h, d));
	return 1;
}

static int l_mesh_plane(lua_State* L)
{
	jr3d_p r = (jr3d_p)lua_touserdata(L, 1);
	float w = (float)luaL_checknumber(L, 2);
	float d = (float)luaL_checknumber(L, 3);
	int   segs = (int)luaL_checkinteger(L, 4);
	lua_pushlightuserdata(L, r3d_mesh_plane(r, w, d, segs));
	return 1;
}

static int l_mesh_sphere(lua_State* L)
{
	jr3d_p r = (jr3d_p)lua_touserdata(L, 1);
	float  radius = (float)luaL_checknumber(L, 2);
	int    slices = (int)luaL_checkinteger(L, 3);
	int    stacks = (int)luaL_checkinteger(L, 4);
	lua_pushlightuserdata(L, r3d_mesh_sphere(r, radius, slices, stacks));
	return 1;
}

static int l_mesh_destroy(lua_State* L)
{
	jr3d_p r = (jr3d_p)lua_touserdata(L, 1);
	render_mesh_p m = (render_mesh_p)lua_touserdata(L, 2);
	if (r && m) r3d_mesh_destroy(r, m);
	return 0;
}

static int l_draw_mesh(lua_State* L)
{
	jr3d_p r = (jr3d_p)lua_touserdata(L, 1);
	render_mesh_p m = (render_mesh_p)lua_touserdata(L, 2);
	vec3_t pos   = read_vec3(L, 3);
	vec3_t scale = read_vec3(L, 4);
	jcolor_t c  = read_color(L, 5);
	r3d_draw_mesh(r, m, pos, scale, c);
	return 0;
}

static int l_mesh_ramp(lua_State* L)
{
	jr3d_p r = (jr3d_p)lua_touserdata(L, 1);
	float w = (float)luaL_checknumber(L, 2);
	float l = (float)luaL_checknumber(L, 3);
	float h = (float)luaL_checknumber(L, 4);
	lua_pushlightuserdata(L, r3d_mesh_ramp(r, w, l, h));
	return 1;
}

static int l_gen_checkerboard(lua_State* L)
{
	jr3d_p r = (jr3d_p)lua_touserdata(L, 1);
	int tile_size = (int)luaL_checkinteger(L, 2);
	int tile_cnt  = (int)luaL_checkinteger(L, 3);
	void* tex = r3d_gen_checkerboard(r, tile_size, tile_cnt);
	lua_pushlightuserdata(L, tex);
	return 1;
}

static int l_draw_mesh_tex(lua_State* L)
{
	jr3d_p r = (jr3d_p)lua_touserdata(L, 1);
	render_mesh_p m = (render_mesh_p)lua_touserdata(L, 2);
	vec3_t pos   = read_vec3(L, 3);
	vec3_t scale = read_vec3(L, 4);
	void* tex    = lua_touserdata(L, 5);
	r3d_draw_mesh_tex(r, m, pos, scale, tex);
	return 0;
}

static int l_draw_grid(lua_State* L)
{
	jr3d_p r = (jr3d_p)lua_touserdata(L, 1);
	int slices    = (int)luaL_checkinteger(L, 2);
	float spacing = (float)luaL_checknumber(L, 3);
	float lw      = (float)luaL_optnumber(L, 4, spacing * 0.01f);
	r3d_draw_grid(r, slices, spacing, lw);
	return 0;
}

/* ---- 模块注册 ---- */

int luaopen_render3d(lua_State* L)
{
	luaL_Reg funcs[] = {
		{"create",        l_create},
		{"destroy",       l_destroy},
		{"begin",         l_begin},
		{"end_frame",     l_end},
		{"camera_lookat", l_camera_lookat},
		{"camera_fps",    l_camera_fps},
		{"mesh_cube",     l_mesh_cube},
		{"mesh_sphere",   l_mesh_sphere},
		{"mesh_plane",    l_mesh_plane},
		{"mesh_ramp",     l_mesh_ramp},
		{"gen_checkerboard", l_gen_checkerboard},
		{"draw_mesh_tex", l_draw_mesh_tex},
		{"mesh_destroy",  l_mesh_destroy},
		{"draw_mesh",     l_draw_mesh},
		{"draw_grid",     l_draw_grid},
		{NULL, NULL}
	};
	luaL_newlib(L, funcs);
	return 1;
}
