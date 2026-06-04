#include <assert.h>
#include "jsclib.h"
#include "../jmath.h"

static vec2_t read_vec2(js_State *J, int idx)
{
        vec2_t v;
        js_getproperty(J, idx, "x"); v.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "y"); v.y = js_tonumber(J, -1); js_pop(J, 1);
        return v;
}

static void push_vec2(js_State *J, vec2_t v)
{
        js_newobject(J);
        js_pushnumber(J, v.x); js_setproperty(J, -2, "x");
        js_pushnumber(J, v.y); js_setproperty(J, -2, "y");
}

static mat22_t read_mat22(js_State *J, int idx)
{
        mat22_t m;
        js_getproperty(J, idx, "col1"); m.col1 = read_vec2(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "col2"); m.col2 = read_vec2(J, -1); js_pop(J, 1);
        return m;
}

static void push_mat22(js_State *J, mat22_t m)
{
        js_newobject(J);
        push_vec2(J, m.col1); js_setproperty(J, -2, "col1");
        push_vec2(J, m.col2); js_setproperty(J, -2, "col2");
}

static void jmat22_add(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        mat22_t a = read_mat22(J, 1), b = read_mat22(J, 2);
        push_mat22(J, mat22_add(a, b));
}

static void jmat22_mul(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        mat22_t a = read_mat22(J, 1), b = read_mat22(J, 2);
        push_mat22(J, mat22_mul(a, b));
}

static void jmat22_mul_vec(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        mat22_t m = read_mat22(J, 1);
        vec2_t v = read_vec2(J, 2);
        push_vec2(J, mat22_mul_vec2(m, v));
}

static void jmat22_transpose(js_State *J)
{
        assert(js_isobject(J, 1));
        push_mat22(J, mat22_transpose(read_mat22(J, 1)));
}

static void jmat22_invert(js_State *J)
{
        assert(js_isobject(J, 1));
        push_mat22(J, mat22_invert(read_mat22(J, 1)));
}

static void jmat22_rotate(js_State *J)
{
        ft_t angle = js_tonumber(J, 1);
        push_mat22(J, mat22_rotate(angle));
}

static void jmat22_abs(js_State *J)
{
        assert(js_isobject(J, 1));
        push_mat22(J, mat22_abs(read_mat22(J, 1)));
}

void jsopen_mat22(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jmat22_add, "add", 2);           js_setproperty(J, -2, "add");
        js_newcfunction(J, jmat22_mul, "mul", 2);           js_setproperty(J, -2, "mul");
        js_newcfunction(J, jmat22_mul_vec, "mul_vec", 2);   js_setproperty(J, -2, "mul_vec");
        js_newcfunction(J, jmat22_transpose, "transpose", 1); js_setproperty(J, -2, "transpose");
        js_newcfunction(J, jmat22_invert, "invert", 1);     js_setproperty(J, -2, "invert");
        js_newcfunction(J, jmat22_rotate, "rotate", 1);     js_setproperty(J, -2, "rotate");
        js_newcfunction(J, jmat22_abs, "abs", 1);           js_setproperty(J, -2, "abs");
        js_setglobal(J, "mat22");
}
