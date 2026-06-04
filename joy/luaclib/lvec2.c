#include "../external/lua/lauxlib.h"
#include "../jmath.h"
#include "luaclib.h"

static int l_vec2_negate(lua_State *L)
{
        vec2_t a, r;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec2_negate(a);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2_dot(lua_State *L)
{
        vec2_t a, b; ft_t r;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec2_dot(a, b);
        lua_pushnumber(L, r);
        return 1;
}

static int l_vec2_add(lua_State *L)
{
        vec2_t a, b, r;
        luaL_checktype(L, 1, LUA_TTABLE); luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec2_add(a, b);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2_sub(lua_State *L)
{
        vec2_t a, b, r;
        luaL_checktype(L, 1, LUA_TTABLE); luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec2_sub(a, b);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2_scale(lua_State *L)
{
        vec2_t a, r; ft_t s;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        s = (float)luaL_checknumber(L, -1);
        r = vec2_scale(a, s);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2_absx(lua_State *L)
{
        vec2_t a, r;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec2_abs(a);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2_length_squared(lua_State *L)
{
        vec2_t a; ft_t r;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec2_length_squared(a);
        lua_pushnumber(L, r);
        return 1;
}

static int l_vec2_length(lua_State *L)
{
        vec2_t a; ft_t r;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec2_length(a);
        lua_pushnumber(L, r);
        return 1;
}

static int l_vec2_normalize(lua_State *L)
{
        vec2_t a, r;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec2_normalize(a);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2_normal(lua_State *L)
{
        vec2_t a, r;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec2_normal(a);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2_rotate(lua_State *L)
{
        vec2_t a, r; ft_t angle;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        angle = (float)luaL_checknumber(L, -1);
        r = vec2_rotate(a, angle);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2_lerp(lua_State *L)
{
        vec2_t a, b, r; ft_t t;
        luaL_checktype(L, 1, LUA_TTABLE); luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        t = (float)luaL_checknumber(L, -1);
        r = vec2_lerp(a, b, t);
        lua_newtable(L);
        lua_pushnumber(L, r.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2_distance(lua_State *L)
{
        vec2_t a, b; ft_t r;
        luaL_checktype(L, 1, LUA_TTABLE); luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = (float)luaL_checknumber(L, -1); lua_pop(L, 1);
        r = vec2_distance(a, b);
        lua_pushnumber(L, r);
        return 1;
}

int luaopen_vec2(lua_State *L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                {"negate", l_vec2_negate},
                {"dot", l_vec2_dot},
                {"add", l_vec2_add},
                {"sub", l_vec2_sub},
                {"scale", l_vec2_scale},
                {"abs", l_vec2_absx},
                {"length_squared", l_vec2_length_squared},
                {"length", l_vec2_length},
                {"normalize", l_vec2_normalize},
                {"normal", l_vec2_normal},
                {"rotate", l_vec2_rotate},
                {"lerp", l_vec2_lerp},
                {"distance", l_vec2_distance},
                {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
