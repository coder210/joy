#include "./external/lua/lapi.h"
#include "./external/lua/lauxlib.h"
#include "liv-math.h"
#include "luaclib.h"


static int l_vec3f_negate(lua_State *L)
{
        vec3f_t a, r;
        assert(lua_istable(L, 1));

        lua_getfield(L, 1, "x");
        a.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "y");
        a.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "z");
        a.z = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        r = vec3f_negate(a);

        lua_newtable(L);
        lua_pushinteger(L, r.x);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y);
        lua_setfield(L, -2, "y");
        lua_pushinteger(L, r.z);
        lua_setfield(L, -2, "z");
        return 1;
}


static int l_vec3f_add(lua_State *L)
{
        vec3f_t a, b, r;

        assert(lua_istable(L, 1));
        assert(lua_istable(L, 2));

        lua_getfield(L, 1, "x");
        a.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "y");
        a.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "z");
        a.z = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 1, "x");
        b.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "y");
        b.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "z");
        b.z = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        r = vec3f_add(a, b);

        lua_newtable(L);
        lua_pushinteger(L, r.x);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y);
        lua_setfield(L, -2, "y");
        lua_pushinteger(L, r.z);
        lua_setfield(L, -2, "z");
        return 1;
}


int luaopen_vec3(lua_State *L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                {"negatef", l_vec3f_negate},
                {"addf", l_vec3f_add},
                {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
