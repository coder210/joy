#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../jmath.h"
#include "luaclib.h"

static int l_fp_from_float(lua_State* L)
{
        float f;
        fp_t a;
        f = (float)luaL_checknumber(L, 1);
        a = fp_from_float(f);
        lua_pushinteger(L, a);
        return 1;
}

static int l_fp_to_float(lua_State* L)
{
        fp_t a;
        a = luaL_checkinteger(L, 1);
        float f = fp_to_float(a);
        lua_pushnumber(L, f);
        return 1;
}

static int l_fp_negate(lua_State* L)
{
        fp_t a, r;
        a = luaL_checkinteger(L, 1);
        r = -a;
        lua_pushinteger(L, r);
        return 1;
}

static int l_fp_zero(lua_State* L)
{
        lua_pushinteger(L, fp_zero());
        return 1;
}

static int l_fp_half(lua_State* L)
{
        lua_pushinteger(L, fp_half());
        return 1;
}

static int l_fp_one(lua_State* L)
{
        lua_pushinteger(L, fp_one());
        return 1;
}

static int l_fp_pi(lua_State* L)
{
        lua_pushinteger(L, fp_pi());
        return 1;
}

static int l_fp_min_value(lua_State* L)
{
        lua_pushinteger(L, fp_min_value());
        return 1;
}

static int l_fp_max_value(lua_State* L)
{
        lua_pushinteger(L, fp_max_value());
        return 1;
}

static int l_fp_add(lua_State* L)
{
        fp_t a, b, r;
        a = luaL_checkinteger(L, 1);
        b = luaL_checkinteger(L, 2);
        r = fp_add(a, b);
        lua_pushinteger(L, r);
        return 1;
}

static int l_fp_sub(lua_State* L)
{
        fp_t a, b, r;
        a = luaL_checkinteger(L, 1);
        b = luaL_checkinteger(L, 2);
        r = fp_sub(a, b);
        lua_pushinteger(L, r);
        return 1;
}

static int l_fp_mul(lua_State* L)
{
        fp_t a, b, r;
        a = luaL_checkinteger(L, 1);
        b = luaL_checkinteger(L, 2);
        r = fp_mul(a, b);
        lua_pushinteger(L, r);
        return 1;
}

static int l_fp_divx(lua_State* L)
{
        fp_t a, b, r;
        a = luaL_checkinteger(L, 1);
        b = luaL_checkinteger(L, 2);
        r = fp_div(a, b);
        lua_pushinteger(L, r);
        return 1;
}

static int l_fp_sign(lua_State* L)
{
        fp_t a;
        int r;
        a = luaL_checkinteger(L, 1);
        r = fp_sign(a);
        lua_pushinteger(L, r);
        return 1;
}

static int l_fp_max(lua_State* L)
{
        fp_t a, b, r;
        a = luaL_checkinteger(L, 1);
        b = luaL_checkinteger(L, 2);
        r = fp_max(a, b);
        lua_pushinteger(L, r);
        return 1;
}

static int l_fp_min(lua_State* L)
{
        fp_t a, b, r;
        a = luaL_checkinteger(L, 1);
        b = luaL_checkinteger(L, 2);
        r = fp_min(a, b);
        lua_pushinteger(L, r);
        return 1;
}

static int l_fp_absx(lua_State* L)
{
        fp_t a, r;
        a = luaL_checkinteger(L, 1);
        r = fp_abs(a);
        lua_pushinteger(L, r);
        return 1;
}

static int l_fp_pow2(lua_State* L)
{
        fp_t a, r;
        a = luaL_checkinteger(L, 1);
        r = fp_pow2(a);
        lua_pushinteger(L, r);
        return 1;
}

static int l_fp_clamp(lua_State* L)
{
        fp_t value, min, max, r;
        value = luaL_checkinteger(L, 1);
        min = luaL_checkinteger(L, 2);
        max = luaL_checkinteger(L, 3);
        r = fp_clamp(value, min, max);
        lua_pushinteger(L, r);
        return 1;
}

static int l_fp_sqrt(lua_State* L)
{
        fp_t a, r;
        a = luaL_checkinteger(L, 1);
        r = fp_sqrt(a);
        lua_pushinteger(L, r);
        return 1;
}

static int l_fp_sin(lua_State* L)
{
        fp_t a, r;
        a = luaL_checkinteger(L, 1);
        r = fp_sin(a);
        lua_pushinteger(L, r);
        return 1;
}

static int l_fp_cos(lua_State* L)
{
        fp_t a, r;
        a = luaL_checkinteger(L, 1);
        r = fp_cos(a);
        lua_pushinteger(L, r);
        return 1;
}

static int l_fp_lerp(lua_State* L)
{
        fp_t a, b, t, r;
        a = luaL_checkinteger(L, 1);
        b = luaL_checkinteger(L, 2);
        t = luaL_checkinteger(L, 3);
        r = fp_lerp(a, b, t);
        lua_pushinteger(L, r);
        return 1;
}

int luaopen_fp(lua_State *L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
        {"from_float", l_fp_from_float},
        {"to_float", l_fp_to_float},
        {"negate", l_fp_negate},
        {"zero", l_fp_zero},
        {"half", l_fp_half},
        {"one", l_fp_one},
        {"pi", l_fp_pi},
        {"min_value", l_fp_min_value},
        {"max_value", l_fp_max_value},
        {"add", l_fp_add},
        {"sub", l_fp_sub},
        {"mul", l_fp_mul},
        {"div", l_fp_divx},
        {"sign", l_fp_sign},
        {"max", l_fp_max},
        {"min", l_fp_min},
        {"abs", l_fp_absx},
        {"pow2", l_fp_pow2},
        {"clamp", l_fp_clamp},
        {"sqrt", l_fp_sqrt},
        {"sin", l_fp_sin},
        {"cos", l_fp_cos},
        {"lerp", l_fp_lerp},
        {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
