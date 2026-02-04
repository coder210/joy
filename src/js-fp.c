#include "jsclib.h"
#include "SDL3/SDL.h"
#include "liv-math.h"

static void jfp_from_float(js_State*J)
{
        float f;
        fp_t a;
        f = (float)js_tonumber(J, 1);
        a = fp_from_float(f);
        js_pushnumber(J, a);
}

static void jfp_to_float(js_State *J)
{
        fp_t a;
        a = (fp_t)js_tonumber(J, 1);
        float f = fp_to_float(a);
        js_pushnumber(J, f);
}

static void jfp_negate(js_State *J)
{
        fp_t a, r;
        a = (fp_t)js_tonumber(J, 1);
        r = -a;
        js_pushnumber(J, r);
}

static void jfp_zero(js_State *J)
{
        js_pushnumber(J, fp_zero());
}

static void jfp_half(js_State *J)
{
        js_pushnumber(J, fp_half());
        
}

static void jfp_one(js_State *J)
{
        js_pushnumber(J, fp_one());
        
}

static void jfp_pi(js_State *J)
{
        js_pushnumber(J, fp_pi());
        
}

static void jfp_min_value(js_State *J)
{
        js_pushnumber(J, fp_min_value());
        
}

static void jfp_max_value(js_State *J)
{
        js_pushnumber(J, fp_max_value());
        
}

static void jfp_add(js_State *J)
{
        fp_t a, b, r;
        a = (fp_t)js_tonumber(J, 1);
        b = (fp_t)js_tonumber(J, 2);
        r = fp_add(a, b);
        js_pushnumber(J, r);
}

static void jfp_sub(js_State *J)
{
        fp_t a, b, r;
        a = (fp_t)js_tonumber(J, 1);
        b = (fp_t)js_tonumber(J, 2);
        r = fp_sub(a, b);
        js_pushnumber(J, r);
}

static void jfp_mul(js_State *J)
{
        fp_t a, b, r;
        a = (fp_t)js_tonumber(J, 1);
        b = (fp_t)js_tonumber(J, 2);
        r = fp_mul(a, b);
        js_pushnumber(J, r);
}

static void jfp_divx(js_State *J)
{
        fp_t a, b, r;
        a = (fp_t)js_tonumber(J, 1);
        b = (fp_t)js_tonumber(J, 2);
        r = fp_div(a, b);
        js_pushnumber(J, r);
}

static void jfp_sign(js_State *J)
{
        fp_t a;
        int r;
        a = (fp_t)js_tonumber(J, 1);
        r = fp_sign(a);
        js_pushnumber(J, r);
}

static void jfp_max(js_State *J)
{
        fp_t a, b, r;
        a = (fp_t)js_tonumber(J, 1);
        b = (fp_t)js_tonumber(J, 2);
        r = fp_max(a, b);
        js_pushnumber(J, r);
}

static void jfp_min(js_State *J)
{
        fp_t a, b, r;
        a = (fp_t)js_tonumber(J, 1);
        b = (fp_t)js_tonumber(J, 2);
        r = fp_min(a, b);
        js_pushnumber(J, r);
}

static void jfp_absx(js_State *J)
{
        fp_t a, r;
        a = (fp_t)js_tonumber(J, 1);
        r = fp_abs(a);
        js_pushnumber(J, r);
}

static void jfp_pow2(js_State *J)
{
        fp_t a, r;
        a = (fp_t)js_tonumber(J, 1);
        r = fp_pow2(a);
        js_pushnumber(J, r);
}

static void jfp_clamp(js_State *J)
{
        fp_t value, min, max, r;
        value = (fp_t)js_tonumber(J, 1);
        min = (fp_t)js_tonumber(J, 2);
        max = (fp_t)js_tonumber(J, 3);
        r = fp_clamp(value, min, max);
        js_pushnumber(J, r);
}

static void jfp_sqrt(js_State *J)
{
        fp_t a, r;
        a = (fp_t)js_tonumber(J, 1);
        r = fp_sqrt(a);
        js_pushnumber(J, r);
}

static void jfp_sin(js_State *J)
{
        fp_t a, r;
        a = (fp_t)js_tonumber(J, 1);
        r = fp_sin(a);
        js_pushnumber(J, r);
}

static void jfp_cos(js_State *J)
{
        fp_t a, r;
        a = (fp_t)js_tonumber(J, 1);
        r = fp_cos(a);
        js_pushnumber(J, r);
}

static void jfp_lerp(js_State *J)
{
        fp_t a, b, t, r;
        a = (fp_t)js_tonumber(J, 1);
        b = (fp_t)js_tonumber(J, 2);
        t = (fp_t)js_tonumber(J, 3);
        r = fp_lerp(a, b, t);
        js_pushnumber(J, r);
}

void jsopen_fp(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jfp_from_float, "from_float", 1);
        js_setproperty(J, -2, "from_float");
        js_newcfunction(J, jfp_to_float, "to_float", 1);
        js_setproperty(J, -2, "to_float");
        js_newcfunction(J, jfp_negate, "negate", 1);
        js_setproperty(J, -2, "negate");
        js_newcfunction(J, jfp_zero, "zero", 1);
        js_setproperty(J, -2, "zero");
        js_newcfunction(J, jfp_half, "half", 1);
        js_setproperty(J, -2, "half");
        js_newcfunction(J, jfp_one, "one", 1);
        js_setproperty(J, -2, "one");
        js_newcfunction(J, jfp_pi, "pi", 1);
        js_setproperty(J, -2, "pi");
        js_newcfunction(J, jfp_min_value, "min_value", 1);
        js_setproperty(J, -2, "min_value");
        js_newcfunction(J, jfp_max_value, "max_value", 1);
        js_setproperty(J, -2, "max_value");
        js_newcfunction(J, jfp_add, "add", 1);
        js_setproperty(J, -2, "add");
        js_newcfunction(J, jfp_sub, "sub", 1);
        js_setproperty(J, -2, "sub");
        js_newcfunction(J, jfp_mul, "mul", 1);
        js_setproperty(J, -2, "mul");
        js_newcfunction(J, jfp_divx, "div", 1);
        js_setproperty(J, -2, "div");
        js_newcfunction(J, jfp_sign, "sign", 1);
        js_setproperty(J, -2, "sign");
        js_newcfunction(J, jfp_max, "max", 1);
        js_setproperty(J, -2, "max");
        js_newcfunction(J, jfp_min, "min", 1);
        js_setproperty(J, -2, "min");
        js_newcfunction(J, jfp_absx, "abs", 1);
        js_setproperty(J, -2, "abs");
        js_newcfunction(J, jfp_pow2, "pow2", 1);
        js_setproperty(J, -2, "pow2");
        js_newcfunction(J, jfp_clamp, "clamp", 1);
        js_setproperty(J, -2, "clamp");
        js_newcfunction(J, jfp_sqrt, "sqrt", 1);
        js_setproperty(J, -2, "sqrt");
        js_newcfunction(J, jfp_sin, "sin", 1);
        js_setproperty(J, -2, "sin");
        js_newcfunction(J, jfp_cos, "cos", 1);
        js_setproperty(J, -2, "cos");
        js_newcfunction(J, jfp_lerp, "lerp", 1);
        js_setproperty(J, -2, "lerp");
        js_setglobal(J, "fp");
}
