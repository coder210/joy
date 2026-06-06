#include "luaclib.h"
#include "../external/lua/lauxlib.h"
#include "../jgjk.h"
#include <string.h>
#include <stdlib.h>

#define COLLIDER_MT "gjk3d_collider_t*"

static int l_gjk3d_new_collider(lua_State* L)
{
	gjk3d_collider_t* c = (gjk3d_collider_t*)lua_newuserdata(L, sizeof(gjk3d_collider_t));
	memset(c, 0, sizeof(*c));
	luaL_getmetatable(L, COLLIDER_MT);
	lua_setmetatable(L, -2);
	return 1;
}

static int l_gjk3d_init_box(lua_State* L)
{
	gjk3d_collider_t* c = (gjk3d_collider_t*)luaL_checkudata(L, 1, COLLIDER_MT);
	lua_getfield(L, 2, "x"); c->position.x = (fp_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
	lua_getfield(L, 2, "y"); c->position.y = (fp_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
	lua_getfield(L, 2, "z"); c->position.z = (fp_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
	lua_getfield(L, 3, "x"); c->half_size.x = (fp_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
	lua_getfield(L, 3, "y"); c->half_size.y = (fp_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
	lua_getfield(L, 3, "z"); c->half_size.z = (fp_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
	c->type = GJK3D_CTYPE_BOX;
	return 0;
}

static int l_gjk3d_init_sphere(lua_State* L)
{
	gjk3d_collider_t* c = (gjk3d_collider_t*)luaL_checkudata(L, 1, COLLIDER_MT);
	lua_getfield(L, 2, "x"); c->position.x = (fp_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
	lua_getfield(L, 2, "y"); c->position.y = (fp_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
	lua_getfield(L, 2, "z"); c->position.z = (fp_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
	c->radius = (fp_t)luaL_checkinteger(L, 3);
	c->type = GJK3D_CTYPE_SPHERE;
	return 0;
}

static int l_gjk3d_collide(lua_State* L)
{
	gjk3d_collider_t* a = (gjk3d_collider_t*)luaL_checkudata(L, 1, COLLIDER_MT);
	gjk3d_collider_t* b = (gjk3d_collider_t*)luaL_checkudata(L, 2, COLLIDER_MT);
	vec3f_t dir;
	lua_getfield(L, 3, "x"); dir.x = (fp_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
	lua_getfield(L, 3, "y"); dir.y = (fp_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
	lua_getfield(L, 3, "z"); dir.z = (fp_t)luaL_checkinteger(L, -1); lua_pop(L, 1);

	gjk3d_contact_t contact;
	int hit = gjk3d_collide(a, b, dir, &contact);

	lua_newtable(L);
	lua_pushboolean(L, hit != 0); lua_setfield(L, -2, "hit");
	if (hit) {
		lua_pushinteger(L, contact.depth); lua_setfield(L, -2, "depth");
		lua_newtable(L);
		lua_pushinteger(L, contact.normal.x); lua_setfield(L, -2, "x");
		lua_pushinteger(L, contact.normal.y); lua_setfield(L, -2, "y");
		lua_pushinteger(L, contact.normal.z); lua_setfield(L, -2, "z");
		lua_setfield(L, -2, "normal");
	}
	return 1;
}

static int l_gjk3d_gc(lua_State* L)
{
	/* 无额外资源需要释放，clear即可 */
	gjk3d_collider_t* c = (gjk3d_collider_t*)lua_touserdata(L, 1);
	if (c) memset(c, 0, sizeof(*c));
	return 0;
}

static const struct luaL_Reg gjk3d_funcs[] = {
	{"new_collider", l_gjk3d_new_collider},
	{"init_box",     l_gjk3d_init_box},
	{"init_sphere",  l_gjk3d_init_sphere},
	{"collide",      l_gjk3d_collide},
	{NULL, NULL}
};

int luaopen_gjk3d(lua_State* L)
{
	luaL_newmetatable(L, COLLIDER_MT);
	lua_pushcfunction(L, l_gjk3d_gc); lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);

	luaL_newlib(L, gjk3d_funcs);
	return 1;
}
