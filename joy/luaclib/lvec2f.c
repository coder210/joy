#include "../external/lua/lauxlib.h"
#include "../jmath.h"
#include "luaclib.h"

static int l_vec2f_negate(lua_State* L)
{
        vec2f_t a, r;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec2f_negate(a);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2f_dot(lua_State* L)
{
        vec2f_t a, b; fp_t r;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec2f_dot(a, b);
        lua_pushinteger(L, r);
        return 1;
}

static int l_vec2f_add(lua_State* L)
{
        vec2f_t a, b, r;
        luaL_checktype(L, 1, LUA_TTABLE); luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec2f_add(a, b);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2f_sub(lua_State* L)
{
        vec2f_t a, b, r;
        luaL_checktype(L, 1, LUA_TTABLE); luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec2f_sub(a, b);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2f_scale(lua_State* L)
{
        vec2f_t a, r; fp_t s;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        s = luaL_checkinteger(L, -1);
        r = vec2f_scale(a, s);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2f_absx(lua_State* L)
{
        vec2f_t a, r;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec2f_abs(a);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2f_length_squared(lua_State* L)
{
        vec2f_t a; fp_t r;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec2f_length_squared(a);
        lua_pushinteger(L, r);
        return 1;
}

static int l_vec2f_length(lua_State* L)
{
        vec2f_t a; fp_t r;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec2f_length(a);
        lua_pushinteger(L, r);
        return 1;
}

static int l_vec2f_normalize(lua_State* L)
{
        vec2f_t a, r;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec2f_normalize(a);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2f_normal(lua_State* L)
{
        vec2f_t a, r;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec2f_normal(a);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2f_rotate(lua_State* L)
{
        vec2f_t a, r; fp_t angle;
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        angle = luaL_checkinteger(L, -1);
        r = vec2f_rotate(a, angle);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2f_lerp(lua_State* L)
{
        vec2f_t a, b, r; fp_t t;
        luaL_checktype(L, 1, LUA_TTABLE); luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        t = luaL_checkinteger(L, -1);
        r = vec2f_lerp(a, b, t);
        lua_newtable(L);
        lua_pushinteger(L, r.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, r.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_vec2f_distance(lua_State* L)
{
        vec2f_t a, b; fp_t r;
        luaL_checktype(L, 1, LUA_TTABLE); luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        r = vec2f_distance(a, b);
        lua_pushinteger(L, r);
        return 1;
}

int luaopen_vec2f(lua_State *L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                {"negate", l_vec2f_negate},
                {"dot", l_vec2f_dot},
                {"add", l_vec2f_add},
                {"sub", l_vec2f_sub},
                {"scale", l_vec2f_scale},
                {"abs", l_vec2f_absx},
                {"length_squared", l_vec2f_length_squared},
                {"length", l_vec2f_length},
                {"normalize", l_vec2f_normalize},
                {"normal", l_vec2f_normal},
                {"rotate", l_vec2f_rotate},
                {"lerp", l_vec2f_lerp},
                {"distance", l_vec2f_distance},
                {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
