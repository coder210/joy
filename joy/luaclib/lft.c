#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../jmath.h"
#include "luaclib.h"

static int l_ft_negate(lua_State *L)
{
        ft_t a, r;
        a = (ft_t)luaL_checknumber(L, 1);
        r = -a;
        lua_pushnumber(L, r);
        return 1;
}

static int l_ft_zero(lua_State *L)
{
        lua_pushnumber(L, ft_zero());
        return 1;
}

static int l_ft_half(lua_State *L)
{
        lua_pushnumber(L, ft_half());
        return 1;
}

static int l_ft_one(lua_State *L)
{
        lua_pushnumber(L, ft_one());
        return 1;
}

static int l_ft_pi(lua_State *L)
{
        lua_pushnumber(L, ft_pi());
        return 1;
}

static int l_ft_min_value(lua_State *L)
{
        lua_pushnumber(L, ft_min_value());
        return 1;
}

static int l_ft_max_value(lua_State *L)
{
        lua_pushnumber(L, ft_max_value());
        return 1;
}

static int l_ft_add(lua_State *L)
{
        ft_t a, b, r;
        a = (ft_t)luaL_checknumber(L, 1);
        b = (ft_t)luaL_checknumber(L, 2);
        r = ft_add(a, b);
        lua_pushnumber(L, r);
        return 1;
}

static int l_ft_sub(lua_State *L)
{
        ft_t a, b, r;
        a = (ft_t)luaL_checknumber(L, 1);
        b = (ft_t)luaL_checknumber(L, 2);
        r = ft_sub(a, b);
        lua_pushnumber(L, r);
        return 1;
}

static int l_ft_mul(lua_State *L)
{
        ft_t a, b, r;
        a = (ft_t)luaL_checknumber(L, 1);
        b = (ft_t)luaL_checknumber(L, 2);
        r = ft_mul(a, b);
        lua_pushnumber(L, r);
        return 1;
}

static int l_ft_divx(lua_State *L)
{
        ft_t a, b, r;
        a = (ft_t)luaL_checknumber(L, 1);
        b = (ft_t)luaL_checknumber(L, 2);
        r = ft_div(a, b);
        lua_pushnumber(L, r);
        return 1;
}

static int l_ft_sign(lua_State *L)
{
        ft_t a;
        int r;
        a = (ft_t)luaL_checknumber(L, 1);
        r = ft_sign(a);
        lua_pushinteger(L, r);
        return 1;
}

static int l_ft_max(lua_State *L)
{
        ft_t a, b, r;
        a = (ft_t)luaL_checknumber(L, 1);
        b = (ft_t)luaL_checknumber(L, 2);
        r = ft_max(a, b);
        lua_pushnumber(L, r);
        return 1;
}

static int l_ft_min(lua_State *L)
{
        ft_t a, b, r;
        a = (ft_t)luaL_checknumber(L, 1);
        b = (ft_t)luaL_checknumber(L, 2);
        r = ft_min(a, b);
        lua_pushnumber(L, r);
        return 1;
}

static int l_ft_absx(lua_State *L)
{
        ft_t a, r;
        a = (ft_t)luaL_checknumber(L, 1);
        r = ft_abs(a);
        lua_pushnumber(L, r);
        return 1;
}

static int l_ft_pow2(lua_State *L)
{
        ft_t a, r;
        a = (ft_t)luaL_checknumber(L, 1);
        r = ft_pow2(a);
        lua_pushnumber(L, r);
        return 1;
}

static int l_ft_clamp(lua_State *L)
{
        ft_t value, min, max, r;
        value = (ft_t)luaL_checknumber(L, 1);
        min = (ft_t)luaL_checknumber(L, 2);
        max = (ft_t)luaL_checknumber(L, 3);
        r = ft_clamp(value, min, max);
        lua_pushnumber(L, r);
        return 1;
}

static int l_ft_sqrt(lua_State *L)
{
        ft_t a, r;
        a = (ft_t)luaL_checknumber(L, 1);
        r = ft_sqrt(a);
        lua_pushnumber(L, r);
        return 1;
}

static int l_ft_sin(lua_State *L)
{
        ft_t a, r;
        a = (ft_t)luaL_checknumber(L, 1);
        r = ft_sin(a);
        lua_pushnumber(L, r);
        return 1;
}

static int l_ft_cos(lua_State *L)
{
        ft_t a, r;
        a = (ft_t)luaL_checknumber(L, 1);
        r = ft_cos(a);
        lua_pushnumber(L, r);
        return 1;
}

static int l_ft_lerp(lua_State *L)
{
        ft_t a, b, t, r;
        a = (ft_t)luaL_checknumber(L, 1);
        b = (ft_t)luaL_checknumber(L, 2);
        t = (ft_t)luaL_checknumber(L, 3);
        r = ft_lerp(a, b, t);
        lua_pushnumber(L, r);
        return 1;
}

int luaopen_ft(lua_State *L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
        {"negate", l_ft_negate},
        {"zero", l_ft_zero},
        {"half", l_ft_half},
        {"one", l_ft_one},
        {"pi", l_ft_pi},
        {"min_value", l_ft_min_value},
        {"max_value", l_ft_max_value},
        {"add", l_ft_add},
        {"sub", l_ft_sub},
        {"mul", l_ft_mul},
        {"div", l_ft_divx},
        {"sign", l_ft_sign},
        {"max", l_ft_max},
        {"min", l_ft_min},
        {"abs", l_ft_absx},
        {"pow2", l_ft_pow2},
        {"clamp", l_ft_clamp},
        {"sqrt", l_ft_sqrt},
        {"sin", l_ft_sin},
        {"cos", l_ft_cos},
        {"lerp", l_ft_lerp},
        {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
