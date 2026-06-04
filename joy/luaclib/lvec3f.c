#include "../external/lua/lauxlib.h"
#include "../jmath.h"
#include "luaclib.h"

static int l_vec3f_negate(lua_State *L)
{
        vec3f_t a, r;
        assert(lua_istable(L, 1));
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec3f_negate(a);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        lua_pushinteger(L, r.z); lua_setfield(L, -2, "z");
        return 1;
}

static int l_vec3f_add(lua_State *L)
{
        vec3f_t a, b, r;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "z"); b.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec3f_add(a, b);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        lua_pushinteger(L, r.z); lua_setfield(L, -2, "z");
        return 1;
}

static int l_vec3f_sub(lua_State *L)
{
        vec3f_t a, b, r;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "z"); b.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec3f_sub(a, b);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        lua_pushinteger(L, r.z); lua_setfield(L, -2, "z");
        return 1;
}

static int l_vec3f_scale(lua_State *L)
{
        vec3f_t a, r; fp_t s;
        assert(lua_istable(L, 1));
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        s = luaL_checkinteger(L, -1);
        r = vec3f_scale(a, s);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        lua_pushinteger(L, r.z); lua_setfield(L, -2, "z");
        return 1;
}

static int l_vec3f_dot(lua_State *L)
{
        vec3f_t a, b; fp_t r;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "z"); b.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec3f_dot(a, b);
        lua_pushinteger(L, r);
        return 1;
}

static int l_vec3f_cross(lua_State *L)
{
        vec3f_t a, b, r;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "z"); b.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec3f_cross(a, b);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        lua_pushinteger(L, r.z); lua_setfield(L, -2, "z");
        return 1;
}

static int l_vec3f_length_squared(lua_State *L)
{
        vec3f_t a; fp_t r;
        assert(lua_istable(L, 1));
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec3f_square_length(a);
        lua_pushinteger(L, r);
        return 1;
}

static int l_vec3f_length(lua_State *L)
{
        vec3f_t a; fp_t r;
        assert(lua_istable(L, 1));
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec3f_length(a);
        lua_pushinteger(L, r);
        return 1;
}

static int l_vec3f_normalize(lua_State *L)
{
        vec3f_t a, r;
        assert(lua_istable(L, 1));
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec3f_normalize(a);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        lua_pushinteger(L, r.z); lua_setfield(L, -2, "z");
        return 1;
}

static int l_vec3f_lerp(lua_State *L)
{
        vec3f_t a, b, r; fp_t t;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "z"); b.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        t = luaL_checkinteger(L, -1);
        r = vec3f_lerp(a, b, t);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        lua_pushinteger(L, r.z); lua_setfield(L, -2, "z");
        return 1;
}

static int l_vec3f_distance(lua_State *L)
{
        vec3f_t a, b; fp_t r;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "z"); b.z = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec3f_distance(a, b);
        lua_pushinteger(L, r);
        return 1;
}

int luaopen_vec3f(lua_State *L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                {"negate", l_vec3f_negate},
                {"add", l_vec3f_add},
                {"sub", l_vec3f_sub},
                {"scale", l_vec3f_scale},
                {"dot", l_vec3f_dot},
                {"cross", l_vec3f_cross},
                {"length_squared", l_vec3f_length_squared},
                {"length", l_vec3f_length},
                {"normalize", l_vec3f_normalize},
                {"lerp", l_vec3f_lerp},
                {"distance", l_vec3f_distance},
                {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
