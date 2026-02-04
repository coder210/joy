#include <assert.h>
#include <stdlib.h>
#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"
#include "liv-collision.h"
#include "luaclib.h"

static int l_ray_quad_int(lua_State *L)
{
        ray3d_collisionf_t result;
        ray3df_t a;
        vec3f_t p1, p2, p3, p4;

        luaL_checktype(L, 1, LUA_TTABLE);
        luaL_checktype(L, 2, LUA_TTABLE);
        luaL_checktype(L, 3, LUA_TTABLE);

        lua_getfield(L, 1, "x");
        a.position.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "y");
        a.position.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "z");
        a.position.z = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "x");
        a.direction.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "y");
        a.direction.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "z");
        a.direction.z = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 3, "x");
        p1.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "y");
        p1.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "z");
        p1.z = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 4, "x");
        p2.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 4, "y");
        p2.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 4, "z");
        p2.z = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 5, "x");
        p3.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 5, "y");
        p3.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 5, "z");
        p3.z = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 6, "x");
        p4.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 6, "y");
        p4.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 6, "z");
        p4.z = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        
        result = collision3df_get_ray_quad(a, p1, p2, p3, p4);

        lua_newtable(L);        
        lua_pushboolean(L, result.hit);
        lua_setfield(L, -2, "hit");
        lua_pushinteger(L, result.distance);
        lua_setfield(L, -2, "distance");
        lua_newtable(L);
        lua_pushinteger(L, result.normal.x);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, result.normal.y);
        lua_setfield(L, -2, "y");
        lua_pushinteger(L, result.normal.z);
        lua_setfield(L, -2, "z");
        lua_setfield(L, -2, "normal");

        lua_newtable(L);
        lua_pushinteger(L, result.point.x);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, result.point.y);
        lua_setfield(L, -2, "y");
        lua_pushinteger(L, result.point.z);
        lua_setfield(L, -2, "z");
        lua_setfield(L, -2, "point");

        return 1;
}


int luaopen_collision3d(lua_State *L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
        {"ray_quad_int", l_ray_quad_int},
        {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
