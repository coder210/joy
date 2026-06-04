#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../jmath.h"
#include "luaclib.h"

static void read_vec2(lua_State* L, int idx, vec2_t* v)
{
        lua_getfield(L, idx, "x"); v->x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, idx, "y"); v->y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
}

static void push_vec2(lua_State* L, vec2_t v)
{
        lua_newtable(L);
        lua_pushnumber(L, v.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, v.y); lua_setfield(L, -2, "y");
}

static void read_mat22(lua_State* L, int idx, mat22_t* m)
{
        lua_getfield(L, idx, "col1"); read_vec2(L, -1, &m->col1); lua_pop(L, 1);
        lua_getfield(L, idx, "col2"); read_vec2(L, -1, &m->col2); lua_pop(L, 1);
}

static void push_mat22(lua_State* L, mat22_t m)
{
        lua_newtable(L);
        push_vec2(L, m.col1); lua_setfield(L, -2, "col1");
        push_vec2(L, m.col2); lua_setfield(L, -2, "col2");
}

static int l_mat22_add(lua_State* L)
{
        mat22_t a, b;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        read_mat22(L, 1, &a); read_mat22(L, 2, &b);
        push_mat22(L, mat22_add(a, b));
        return 1;
}

static int l_mat22_mul(lua_State* L)
{
        mat22_t a, b;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        read_mat22(L, 1, &a); read_mat22(L, 2, &b);
        push_mat22(L, mat22_mul(a, b));
        return 1;
}

static int l_mat22_mul_vec(lua_State* L)
{
        mat22_t m; vec2_t v;
        assert(lua_istable(L, 1)); assert(lua_istable(L, 2));
        read_mat22(L, 1, &m); read_vec2(L, 2, &v);
        push_vec2(L, mat22_mul_vec2(m, v));
        return 1;
}

static int l_mat22_transpose(lua_State* L)
{
        mat22_t a;
        assert(lua_istable(L, 1));
        read_mat22(L, 1, &a);
        push_mat22(L, mat22_transpose(a));
        return 1;
}

static int l_mat22_invert(lua_State* L)
{
        mat22_t a;
        assert(lua_istable(L, 1));
        read_mat22(L, 1, &a);
        push_mat22(L, mat22_invert(a));
        return 1;
}

static int l_mat22_rotate(lua_State* L)
{
        ft_t angle = (ft_t)luaL_checknumber(L, 1);
        push_mat22(L, mat22_rotate(angle));
        return 1;
}

static int l_mat22_absx(lua_State* L)
{
        mat22_t a;
        assert(lua_istable(L, 1));
        read_mat22(L, 1, &a);
        push_mat22(L, mat22_abs(a));
        return 1;
}

int luaopen_mat22(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                {"add", l_mat22_add},
                {"mul", l_mat22_mul},
                {"mul_vec", l_mat22_mul_vec},
                {"transpose", l_mat22_transpose},
                {"invert", l_mat22_invert},
                {"rotate", l_mat22_rotate},
                {"abs", l_mat22_absx},
                {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
