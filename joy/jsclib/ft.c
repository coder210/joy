#include "jsclib.h"
#include "../jmath.h"

static void jft_negate(js_State *J)
{
        ft_t a, r;
        a = (ft_t)js_tonumber(J, 1);
        r = -a;
        js_pushnumber(J, r);
}

static void jft_zero(js_State *J)
{
        js_pushnumber(J, ft_zero());
}

static void jft_half(js_State *J)
{
        js_pushnumber(J, ft_half());
}

static void jft_one(js_State *J)
{
        js_pushnumber(J, ft_one());
}

static void jft_pi(js_State *J)
{
        js_pushnumber(J, ft_pi());
}

static void jft_min_value(js_State *J)
{
        js_pushnumber(J, ft_min_value());
}

static void jft_max_value(js_State *J)
{
        js_pushnumber(J, ft_max_value());
}

static void jft_add(js_State *J)
{
        ft_t a, b, r;
        a = (ft_t)js_tonumber(J, 1);
        b = (ft_t)js_tonumber(J, 2);
        r = ft_add(a, b);
        js_pushnumber(J, r);
}

static void jft_sub(js_State *J)
{
        ft_t a, b, r;
        a = (ft_t)js_tonumber(J, 1);
        b = (ft_t)js_tonumber(J, 2);
        r = ft_sub(a, b);
        js_pushnumber(J, r);
}

static void jft_mul(js_State *J)
{
        ft_t a, b, r;
        a = (ft_t)js_tonumber(J, 1);
        b = (ft_t)js_tonumber(J, 2);
        r = ft_mul(a, b);
        js_pushnumber(J, r);
}

static void jft_div(js_State *J)
{
        ft_t a, b, r;
        a = (ft_t)js_tonumber(J, 1);
        b = (ft_t)js_tonumber(J, 2);
        r = ft_div(a, b);
        js_pushnumber(J, r);
}

static void jft_sign(js_State *J)
{
        ft_t a;
        a = (ft_t)js_tonumber(J, 1);
        js_pushnumber(J, ft_sign(a));
}

static void jft_max(js_State *J)
{
        ft_t a, b, r;
        a = (ft_t)js_tonumber(J, 1);
        b = (ft_t)js_tonumber(J, 2);
        r = ft_max(a, b);
        js_pushnumber(J, r);
}

static void jft_min(js_State *J)
{
        ft_t a, b, r;
        a = (ft_t)js_tonumber(J, 1);
        b = (ft_t)js_tonumber(J, 2);
        r = ft_min(a, b);
        js_pushnumber(J, r);
}

static void jft_abs(js_State *J)
{
        ft_t a, r;
        a = (ft_t)js_tonumber(J, 1);
        r = ft_abs(a);
        js_pushnumber(J, r);
}

static void jft_pow2(js_State *J)
{
        ft_t a, r;
        a = (ft_t)js_tonumber(J, 1);
        r = ft_pow2(a);
        js_pushnumber(J, r);
}

static void jft_clamp(js_State *J)
{
        ft_t value, min, max, r;
        value = (ft_t)js_tonumber(J, 1);
        min = (ft_t)js_tonumber(J, 2);
        max = (ft_t)js_tonumber(J, 3);
        r = ft_clamp(value, min, max);
        js_pushnumber(J, r);
}

static void jft_sqrt(js_State *J)
{
        ft_t a, r;
        a = (ft_t)js_tonumber(J, 1);
        r = ft_sqrt(a);
        js_pushnumber(J, r);
}

static void jft_sin(js_State *J)
{
        ft_t a, r;
        a = (ft_t)js_tonumber(J, 1);
        r = ft_sin(a);
        js_pushnumber(J, r);
}

static void jft_cos(js_State *J)
{
        ft_t a, r;
        a = (ft_t)js_tonumber(J, 1);
        r = ft_cos(a);
        js_pushnumber(J, r);
}

static void jft_lerp(js_State *J)
{
        ft_t a, b, t, r;
        a = (ft_t)js_tonumber(J, 1);
        b = (ft_t)js_tonumber(J, 2);
        t = (ft_t)js_tonumber(J, 3);
        r = ft_lerp(a, b, t);
        js_pushnumber(J, r);
}

void jsopen_ft(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jft_negate, "negate", 1); js_setproperty(J, -2, "negate");
        js_newcfunction(J, jft_zero, "zero", 0);     js_setproperty(J, -2, "zero");
        js_newcfunction(J, jft_half, "half", 0);     js_setproperty(J, -2, "half");
        js_newcfunction(J, jft_one, "one", 0);       js_setproperty(J, -2, "one");
        js_newcfunction(J, jft_pi, "pi", 0);         js_setproperty(J, -2, "pi");
        js_newcfunction(J, jft_min_value, "min_value", 0); js_setproperty(J, -2, "min_value");
        js_newcfunction(J, jft_max_value, "max_value", 0); js_setproperty(J, -2, "max_value");
        js_newcfunction(J, jft_add, "add", 2);       js_setproperty(J, -2, "add");
        js_newcfunction(J, jft_sub, "sub", 2);       js_setproperty(J, -2, "sub");
        js_newcfunction(J, jft_mul, "mul", 2);       js_setproperty(J, -2, "mul");
        js_newcfunction(J, jft_div, "div", 2);       js_setproperty(J, -2, "div");
        js_newcfunction(J, jft_sign, "sign", 1);     js_setproperty(J, -2, "sign");
        js_newcfunction(J, jft_max, "max", 2);       js_setproperty(J, -2, "max");
        js_newcfunction(J, jft_min, "min", 2);       js_setproperty(J, -2, "min");
        js_newcfunction(J, jft_abs, "abs", 1);       js_setproperty(J, -2, "abs");
        js_newcfunction(J, jft_pow2, "pow2", 1);     js_setproperty(J, -2, "pow2");
        js_newcfunction(J, jft_clamp, "clamp", 3);   js_setproperty(J, -2, "clamp");
        js_newcfunction(J, jft_sqrt, "sqrt", 1);     js_setproperty(J, -2, "sqrt");
        js_newcfunction(J, jft_sin, "sin", 1);       js_setproperty(J, -2, "sin");
        js_newcfunction(J, jft_cos, "cos", 1);       js_setproperty(J, -2, "cos");
        js_newcfunction(J, jft_lerp, "lerp", 3);     js_setproperty(J, -2, "lerp");
        js_setglobal(J, "ft");
}
