#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../jmath.h"
#include "luaclib.h"

static void read_vec2f(lua_State* L, int idx, vec2f_t* v)
{
        lua_getfield(L, idx, "x"); v->x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_getfield(L, idx, "y"); v->y = luaL_checkinteger(L, -1); lua_pop(L, 1);
}

static void push_vec2f(lua_State* L, vec2f_t v)
{
        lua_newtable(L);
        lua_pushinteger(L, v.x); lua_setfield(L, -2, "x");
        lua_pushinteger(L, v.y); lua_setfield(L, -2, "y");
}

static void read_mat22f(lua_State* L, int idx, mat22f_t* m)
{
        lua_getfield(L, idx, "col1"); read_vec2f(L, -1, &m->col1); lua_pop(L, 1);
        lua_getfield(L, idx, "col2"); read_vec2f(L, -1, &m->col2); lua_pop(L, 1);
}

static void push_mat22f(lua_State* L, mat22f_t m)
{
        lua_newtable(L);
        push_vec2f(L, m.col1); lua_setfield(L, -2, "col1");
        push_vec2f(L, m.col2); lua_setfield(L, -2, "col2");
}

static int l_mat22f_add(lua_State* L)
{
        mat22f_t a, b;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        read_mat22f(L, 1, &a); read_mat22f(L, 2, &b);
        push_mat22f(L, mat22f_add(a, b));
        return 1;
}

static int l_mat22f_mul(lua_State* L)
{
        mat22f_t a, b;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        read_mat22f(L, 1, &a); read_mat22f(L, 2, &b);
        push_mat22f(L, mat22f_mul(a, b));
        return 1;
}

static int l_mat22f_mul_vec(lua_State* L)
{
        mat22f_t m; vec2f_t v;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        read_mat22f(L, 1, &m); read_vec2f(L, 2, &v);
        push_vec2f(L, mat22f_mul_vec2f(m, v));
        return 1;
}

static int l_mat22f_transpose(lua_State* L)
{
        mat22f_t a;
        assert(lua_istable(L, 1));
        read_mat22f(L, 1, &a);
        push_mat22f(L, mat22f_transpose(a));
        return 1;
}

static int l_mat22f_invert(lua_State* L)
{
        mat22f_t a;
        assert(lua_istable(L, 1));
        read_mat22f(L, 1, &a);
        push_mat22f(L, mat22f_invert(a));
        return 1;
}

static int l_mat22f_rotate(lua_State* L)
{
        fp_t angle = luaL_checkinteger(L, 1);
        push_mat22f(L, mat22f_rotate(angle));
        return 1;
}

static int l_mat22f_absx(lua_State* L)
{
        mat22f_t a;
        assert(lua_istable(L, 1));
        read_mat22f(L, 1, &a);
        push_mat22f(L, mat22f_abs(a));
        return 1;
}

int luaopen_mat22f(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                {"add", l_mat22f_add},
                {"mul", l_mat22f_mul},
                {"mul_vec", l_mat22f_mul_vec},
                {"transpose", l_mat22f_transpose},
                {"invert", l_mat22f_invert},
                {"rotate", l_mat22f_rotate},
                {"abs", l_mat22f_absx},
                {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
