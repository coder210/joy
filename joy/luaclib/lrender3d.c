#include "luaclib.h"
#include <SDL3/SDL.h>
#include "../external/lua/lauxlib.h"
#include "../jrender.h"

#define R3D_MT  "render3d_t*"
#define MESH_MT "render_mesh_t*"

/* ---- helpers ---- */

static render3d_p check_r3d(lua_State* L, int idx)
{
	render3d_p* r = (render3d_p*)luaL_checkudata(L, idx, R3D_MT);
	if (!r || !*r) luaL_error(L, "invalid render3d");
	return *r;
}

static render_mesh_p check_mesh(lua_State* L, int idx)
{
	render_mesh_p* m = (render_mesh_p*)luaL_checkudata(L, idx, MESH_MT);
	if (!m || !*m) luaL_error(L, "invalid render_mesh");
	return *m;
}

static vec3_t read_vec3(lua_State* L, int idx)
{
	vec3_t v = { 0, 0, 0 };
	lua_getfield(L, idx, "x"); v.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
	lua_getfield(L, idx, "y"); v.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
	lua_getfield(L, idx, "z"); v.z = (float)lua_tonumber(L, -1); lua_pop(L, 1);
	return v;
}

static SDL_Color read_color(lua_State* L, int idx)
{
	SDL_Color c = { 255, 255, 255, 255 };
	lua_getfield(L, idx, "r"); c.r = (Uint8)lua_tointeger(L, -1); lua_pop(L, 1);
	lua_getfield(L, idx, "g"); c.g = (Uint8)lua_tointeger(L, -1); lua_pop(L, 1);
	lua_getfield(L, idx, "b"); c.b = (Uint8)lua_tointeger(L, -1); lua_pop(L, 1);
	lua_getfield(L, idx, "a"); if (!lua_isnil(L, -1)) c.a = (Uint8)lua_tointeger(L, -1); lua_pop(L, 1);
	return c;
}

static int push_r3d(lua_State* L, render3d_p r)
{
	render3d_p* p = (render3d_p*)lua_newuserdata(L, sizeof(render3d_p));
	*p = r;
	luaL_getmetatable(L, R3D_MT);
	lua_setmetatable(L, -2);
	return 1;
}

static int push_mesh(lua_State* L, render_mesh_p m)
{
	render_mesh_p* p = (render_mesh_p*)lua_newuserdata(L, sizeof(render_mesh_p));
	*p = m;
	luaL_getmetatable(L, MESH_MT);
	lua_setmetatable(L, -2);
	return 1;
}

/* ---- ___gc ---- */

static int l_r3d_gc(lua_State* L)
{
	render3d_p* r = (render3d_p*)lua_touserdata(L, 1);
	if (r && *r) { render3d_destroy(*r); *r = NULL; }
	return 0;
}

static int l_mesh_gc(lua_State* L)
{
	/* 无法在 gc 中获取 render3d_p，所以 mesh gc 只释放 buffer */
	/* 用户应在 destroy render3d 前手动 mesh_destroy */
	render_mesh_p* m = (render_mesh_p*)lua_touserdata(L, 1);
	if (m && *m) { SDL_free(*m); *m = NULL; }
	return 0;
}

/* ---- API ---- */

static int l_create(lua_State* L)
{
	SDL_Window** win = (SDL_Window**)luaL_checkudata(L, 1, "SDL_Window*");
	const char* shader_dir = luaL_checkstring(L, 2);
	render3d_p r = render3d_create(*win, shader_dir);
	if (!r) luaL_error(L, "render3d.create failed");
	return push_r3d(L, r);
}

static int l_destroy(lua_State* L)
{
	render3d_p r = check_r3d(L, 1);
	render3d_destroy(r);
	render3d_p* p = (render3d_p*)lua_touserdata(L, 1);
	*p = NULL;
	return 0;
}

static int l_begin(lua_State* L)
{
	lua_pushboolean(L, render3d_begin(check_r3d(L, 1)));
	return 1;
}

static int l_end(lua_State* L)
{
	render3d_end(check_r3d(L, 1));
	return 0;
}

static int l_camera_lookat(lua_State* L)
{
	render3d_p r = check_r3d(L, 1);
	vec3_t eye    = read_vec3(L, 2);
	vec3_t target = read_vec3(L, 3);
	vec3_t up     = read_vec3(L, 4);
	float  fovy   = (float)luaL_checknumber(L, 5);
	render3d_camera_lookat(r, eye, target, up, fovy);
	return 0;
}

static int l_camera_fps(lua_State* L)
{
	render3d_p r = check_r3d(L, 1);
	vec3_t pos = read_vec3(L, 2);
	float yaw  = (float)luaL_checknumber(L, 3);
	float pitch = (float)luaL_checknumber(L, 4);
	float fovy = (float)luaL_checknumber(L, 5);
	render3d_camera_fps(r, pos, yaw, pitch, fovy);
	return 0;
}

static int l_mesh_cube(lua_State* L)
{
	render3d_p r = check_r3d(L, 1);
	float w = (float)luaL_checknumber(L, 2);
	float h = (float)luaL_checknumber(L, 3);
	float d = (float)luaL_checknumber(L, 4);
	return push_mesh(L, render3d_mesh_cube(r, w, h, d));
}

static int l_mesh_sphere(lua_State* L)
{
	render3d_p r = check_r3d(L, 1);
	float  radius = (float)luaL_checknumber(L, 2);
	int    slices = (int)luaL_checkinteger(L, 3);
	int    stacks = (int)luaL_checkinteger(L, 4);
	return push_mesh(L, render3d_mesh_sphere(r, radius, slices, stacks));
}

static int l_mesh_destroy(lua_State* L)
{
	render3d_p r = check_r3d(L, 1);
	render_mesh_p m = check_mesh(L, 2);
	render3d_mesh_destroy(r, m);
	render_mesh_p* p = (render_mesh_p*)lua_touserdata(L, 2);
	*p = NULL;
	return 0;
}

static int l_draw_mesh(lua_State* L)
{
	render3d_p r = check_r3d(L, 1);
	render_mesh_p m = check_mesh(L, 2);
	vec3_t pos   = read_vec3(L, 3);
	vec3_t scale = read_vec3(L, 4);
	SDL_Color c  = read_color(L, 5);
	render3d_draw_mesh(r, m, pos, scale, c);
	return 0;
}

static int l_draw_grid(lua_State* L)
{
	render3d_p r = check_r3d(L, 1);
	int slices  = (int)luaL_checkinteger(L, 2);
	float spacing = (float)luaL_checknumber(L, 3);
	render3d_draw_grid(r, slices, spacing);
	return 0;
}

/* ---- 模块注册 ---- */

int luaopen_render3d(lua_State* L)
{
	/* 注册 metatable */
	luaL_newmetatable(L, R3D_MT);
	lua_pushcfunction(L, l_r3d_gc); lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);

	luaL_newmetatable(L, MESH_MT);
	lua_pushcfunction(L, l_mesh_gc); lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);

	luaL_Reg funcs[] = {
		{"create",        l_create},
		{"destroy",       l_destroy},
		{"begin",         l_begin},
		{"end_frame",     l_end},
		{"camera_lookat", l_camera_lookat},
		{"camera_fps",    l_camera_fps},
		{"mesh_cube",     l_mesh_cube},
		{"mesh_sphere",   l_mesh_sphere},
		{"mesh_destroy",  l_mesh_destroy},
		{"draw_mesh",     l_draw_mesh},
		{"draw_grid",     l_draw_grid},
		{NULL, NULL}
	};
	luaL_newlib(L, funcs);
	return 1;
}
