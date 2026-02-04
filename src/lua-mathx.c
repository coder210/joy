#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"
#include "liv-math.h"
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


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////



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


int luaopen_mathx(lua_State *L)
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


        {"from_float", l_fp_from_float},
        {"to_float", l_fp_to_float},
        {"negatef", l_fp_negate},
        {"zerof", l_fp_zero},
        {"halff", l_fp_half},
        {"onef", l_fp_one},
        {"pif", l_fp_pi},
        {"min_valuef", l_fp_min_value},
        {"max_valuef", l_fp_max_value},
        {"addf", l_fp_add},
        {"subf", l_fp_sub},
        {"mulf", l_fp_mul},
        {"divf", l_fp_divx},
        {"signf", l_fp_sign},
        {"maxf", l_fp_max},
        {"minf", l_fp_min},
        {"absf", l_fp_absx},
        {"pow2f", l_fp_pow2},
        {"clampf", l_fp_clamp},
        {"sqrtf", l_fp_sqrt},
        {"sinf", l_fp_sin},
        {"cosf", l_fp_cos},
        {"lerpf", l_fp_lerp},
        {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
