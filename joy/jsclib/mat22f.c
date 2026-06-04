#include <assert.h>
#include "jsclib.h"
#include "../jmath.h"

static vec2f_t read_vec2f(js_State *J, int idx)
{
        vec2f_t v;
        js_getproperty(J, idx, "x"); v.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "y"); v.y = js_toint64(J, -1); js_pop(J, 1);
        return v;
}

static void push_vec2f(js_State *J, vec2f_t v)
{
        js_newobject(J);
        js_pushint64(J, v.x); js_setproperty(J, -2, "x");
        js_pushint64(J, v.y); js_setproperty(J, -2, "y");
}

static mat22f_t read_mat22f(js_State *J, int idx)
{
        mat22f_t m;
        js_getproperty(J, idx, "col1"); m.col1 = read_vec2f(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "col2"); m.col2 = read_vec2f(J, -1); js_pop(J, 1);
        return m;
}

static void push_mat22f(js_State *J, mat22f_t m)
{
        js_newobject(J);
        push_vec2f(J, m.col1); js_setproperty(J, -2, "col1");
        push_vec2f(J, m.col2); js_setproperty(J, -2, "col2");
}

static void jmat22f_add(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        mat22f_t a = read_mat22f(J, 1), b = read_mat22f(J, 2);
        push_mat22f(J, mat22f_add(a, b));
}

static void jmat22f_mul(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        mat22f_t a = read_mat22f(J, 1), b = read_mat22f(J, 2);
        push_mat22f(J, mat22f_mul(a, b));
}

static void jmat22f_mul_vec(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        mat22f_t m = read_mat22f(J, 1);
        vec2f_t v = read_vec2f(J, 2);
        push_vec2f(J, mat22f_mul_vec2f(m, v));
}

static void jmat22f_transpose(js_State *J)
{
        assert(js_isobject(J, 1));
        push_mat22f(J, mat22f_transpose(read_mat22f(J, 1)));
}

static void jmat22f_invert(js_State *J)
{
        assert(js_isobject(J, 1));
        push_mat22f(J, mat22f_invert(read_mat22f(J, 1)));
}

static void jmat22f_rotate(js_State *J)
{
        fp_t angle = js_toint64(J, 1);
        push_mat22f(J, mat22f_rotate(angle));
}

static void jmat22f_abs(js_State *J)
{
        assert(js_isobject(J, 1));
        push_mat22f(J, mat22f_abs(read_mat22f(J, 1)));
}

void jsopen_mat22f(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jmat22f_add, "add", 2);           js_setproperty(J, -2, "add");
        js_newcfunction(J, jmat22f_mul, "mul", 2);           js_setproperty(J, -2, "mul");
        js_newcfunction(J, jmat22f_mul_vec, "mul_vec", 2);   js_setproperty(J, -2, "mul_vec");
        js_newcfunction(J, jmat22f_transpose, "transpose", 1); js_setproperty(J, -2, "transpose");
        js_newcfunction(J, jmat22f_invert, "invert", 1);     js_setproperty(J, -2, "invert");
        js_newcfunction(J, jmat22f_rotate, "rotate", 1);     js_setproperty(J, -2, "rotate");
        js_newcfunction(J, jmat22f_abs, "abs", 1);           js_setproperty(J, -2, "abs");
        js_setglobal(J, "mat22f");
}
