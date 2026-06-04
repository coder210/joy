#include "../external/lua/lauxlib.h"
#include "../jmath.h"
#include "luaclib.h"

static int l_vec3_negate(lua_State *L)
{
        vec3_t a, r;
        assert(lua_istable(L, 1));
        lua_getfield(L, 1, "x"); a.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec3_negate(a);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        lua_pushnumber(L, r.z); lua_setfield(L, -2, "z");
        return 1;
}

static int l_vec3_add(lua_State *L)
{
        vec3_t a, b, r;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        lua_getfield(L, 1, "x"); a.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "z"); b.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec3_add(a, b);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        lua_pushnumber(L, r.z); lua_setfield(L, -2, "z");
        return 1;
}

static int l_vec3_sub(lua_State *L)
{
        vec3_t a, b, r;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        lua_getfield(L, 1, "x"); a.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "z"); b.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec3_sub(a, b);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        lua_pushnumber(L, r.z); lua_setfield(L, -2, "z");
        return 1;
}

static int l_vec3_scale(lua_State *L)
{
        vec3_t a, r; ft_t s;
        assert(lua_istable(L, 1));
        lua_getfield(L, 1, "x"); a.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        s = (ft_t)luaL_checknumber(L, -1);
        r = vec3_scale(a, s);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        lua_pushnumber(L, r.z); lua_setfield(L, -2, "z");
        return 1;
}

static int l_vec3_dot(lua_State *L)
{
        vec3_t a, b; ft_t r;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        lua_getfield(L, 1, "x"); a.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "z"); b.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec3_dot(a, b);
        lua_pushnumber(L, r);
        return 1;
}

static int l_vec3_cross(lua_State *L)
{
        vec3_t a, b, r;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        lua_getfield(L, 1, "x"); a.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "z"); b.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec3_cross(a, b);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        lua_pushnumber(L, r.z); lua_setfield(L, -2, "z");
        return 1;
}

static int l_vec3_length_squared(lua_State *L)
{
        vec3_t a; ft_t r;
        assert(lua_istable(L, 1));
        lua_getfield(L, 1, "x"); a.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec3_square_length(a);
        lua_pushnumber(L, r);
        return 1;
}

static int l_vec3_length(lua_State *L)
{
        vec3_t a; ft_t r;
        assert(lua_istable(L, 1));
        lua_getfield(L, 1, "x"); a.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec3_length(a);
        lua_pushnumber(L, r);
        return 1;
}

static int l_vec3_normalize(lua_State *L)
{
        vec3_t a, r;
        assert(lua_istable(L, 1));
        lua_getfield(L, 1, "x"); a.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec3_normalize(a);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        lua_pushnumber(L, r.z); lua_setfield(L, -2, "z");
        return 1;
}

static int l_vec3_lerp(lua_State *L)
{
        vec3_t a, b, r; ft_t t;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        lua_getfield(L, 1, "x"); a.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "z"); b.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        t = (ft_t)luaL_checknumber(L, -1);
        r = vec3_lerp(a, b, t);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        lua_pushnumber(L, r.z); lua_setfield(L, -2, "z");
        return 1;
}

static int l_vec3_distance(lua_State *L)
{
        vec3_t a, b; ft_t r;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        lua_getfield(L, 1, "x"); a.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "z"); a.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "z"); b.z = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec3_distance(a, b);
        lua_pushnumber(L, r);
        return 1;
}

int luaopen_vec3(lua_State *L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                {"negate", l_vec3_negate},
                {"add", l_vec3_add},
                {"sub", l_vec3_sub},
                {"scale", l_vec3_scale},
                {"dot", l_vec3_dot},
                {"cross", l_vec3_cross},
                {"length_squared", l_vec3_length_squared},
                {"length", l_vec3_length},
                {"normalize", l_vec3_normalize},
                {"lerp", l_vec3_lerp},
                {"distance", l_vec3_distance},
                {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
