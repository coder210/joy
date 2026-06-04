#include <assert.h>
#include "jsclib.h"
#include "../jmath.h"

static void jvec3f_negate(js_State *J)
{
        vec3f_t a, r;
        assert(js_isobject(J, 1));
        js_getproperty(J, 1, "x"); a.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_toint64(J, -1); js_pop(J, 1);
        r = vec3f_negate(a);
        js_newobject(J);
        js_pushint64(J, r.x); js_setproperty(J, -2, "x");
        js_pushint64(J, r.y); js_setproperty(J, -2, "y");
        js_pushint64(J, r.z); js_setproperty(J, -2, "z");
}

static void jvec3f_add(js_State *J)
{
        vec3f_t a, b, r;
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        js_getproperty(J, 1, "x"); a.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "x"); b.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "y"); b.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "z"); b.z = js_toint64(J, -1); js_pop(J, 1);
        r = vec3f_add(a, b);
        js_newobject(J);
        js_pushint64(J, r.x); js_setproperty(J, -2, "x");
        js_pushint64(J, r.y); js_setproperty(J, -2, "y");
        js_pushint64(J, r.z); js_setproperty(J, -2, "z");
}

static void jvec3f_sub(js_State *J)
{
        vec3f_t a, b, r;
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        js_getproperty(J, 1, "x"); a.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "x"); b.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "y"); b.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "z"); b.z = js_toint64(J, -1); js_pop(J, 1);
        r = vec3f_sub(a, b);
        js_newobject(J);
        js_pushint64(J, r.x); js_setproperty(J, -2, "x");
        js_pushint64(J, r.y); js_setproperty(J, -2, "y");
        js_pushint64(J, r.z); js_setproperty(J, -2, "z");
}

static void jvec3f_scale(js_State *J)
{
        vec3f_t a, r; fp_t s;
        assert(js_isobject(J, 1));
        js_getproperty(J, 1, "x"); a.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_toint64(J, -1); js_pop(J, 1);
        s = js_toint64(J, 2);
        r = vec3f_scale(a, s);
        js_newobject(J);
        js_pushint64(J, r.x); js_setproperty(J, -2, "x");
        js_pushint64(J, r.y); js_setproperty(J, -2, "y");
        js_pushint64(J, r.z); js_setproperty(J, -2, "z");
}

static void jvec3f_dot(js_State *J)
{
        vec3f_t a, b;
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        js_getproperty(J, 1, "x"); a.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "x"); b.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "y"); b.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "z"); b.z = js_toint64(J, -1); js_pop(J, 1);
        js_pushint64(J, vec3f_dot(a, b));
}

static void jvec3f_cross(js_State *J)
{
        vec3f_t a, b, r;
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        js_getproperty(J, 1, "x"); a.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "x"); b.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "y"); b.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "z"); b.z = js_toint64(J, -1); js_pop(J, 1);
        r = vec3f_cross(a, b);
        js_newobject(J);
        js_pushint64(J, r.x); js_setproperty(J, -2, "x");
        js_pushint64(J, r.y); js_setproperty(J, -2, "y");
        js_pushint64(J, r.z); js_setproperty(J, -2, "z");
}

static void jvec3f_length_squared(js_State *J)
{
        vec3f_t a;
        assert(js_isobject(J, 1));
        js_getproperty(J, 1, "x"); a.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_toint64(J, -1); js_pop(J, 1);
        js_pushint64(J, vec3f_square_length(a));
}

static void jvec3f_length(js_State *J)
{
        vec3f_t a;
        assert(js_isobject(J, 1));
        js_getproperty(J, 1, "x"); a.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_toint64(J, -1); js_pop(J, 1);
        js_pushint64(J, vec3f_length(a));
}

static void jvec3f_normalize(js_State *J)
{
        vec3f_t a, r;
        assert(js_isobject(J, 1));
        js_getproperty(J, 1, "x"); a.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_toint64(J, -1); js_pop(J, 1);
        r = vec3f_normalize(a);
        js_newobject(J);
        js_pushint64(J, r.x); js_setproperty(J, -2, "x");
        js_pushint64(J, r.y); js_setproperty(J, -2, "y");
        js_pushint64(J, r.z); js_setproperty(J, -2, "z");
}

static void jvec3f_lerp(js_State *J)
{
        vec3f_t a, b, r; fp_t t;
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        js_getproperty(J, 1, "x"); a.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "x"); b.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "y"); b.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "z"); b.z = js_toint64(J, -1); js_pop(J, 1);
        t = js_toint64(J, 3);
        r = vec3f_lerp(a, b, t);
        js_newobject(J);
        js_pushint64(J, r.x); js_setproperty(J, -2, "x");
        js_pushint64(J, r.y); js_setproperty(J, -2, "y");
        js_pushint64(J, r.z); js_setproperty(J, -2, "z");
}

static void jvec3f_distance(js_State *J)
{
        vec3f_t a, b;
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        js_getproperty(J, 1, "x"); a.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "x"); b.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "y"); b.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "z"); b.z = js_toint64(J, -1); js_pop(J, 1);
        js_pushint64(J, vec3f_distance(a, b));
}

void jsopen_vec3f(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jvec3f_negate, "negate", 1);        js_setproperty(J, -2, "negate");
        js_newcfunction(J, jvec3f_add, "add", 2);              js_setproperty(J, -2, "add");
        js_newcfunction(J, jvec3f_sub, "sub", 2);              js_setproperty(J, -2, "sub");
        js_newcfunction(J, jvec3f_scale, "scale", 2);          js_setproperty(J, -2, "scale");
        js_newcfunction(J, jvec3f_dot, "dot", 2);              js_setproperty(J, -2, "dot");
        js_newcfunction(J, jvec3f_cross, "cross", 2);          js_setproperty(J, -2, "cross");
        js_newcfunction(J, jvec3f_length_squared, "length_squared", 1); js_setproperty(J, -2, "length_squared");
        js_newcfunction(J, jvec3f_length, "length", 1);        js_setproperty(J, -2, "length");
        js_newcfunction(J, jvec3f_normalize, "normalize", 1);  js_setproperty(J, -2, "normalize");
        js_newcfunction(J, jvec3f_lerp, "lerp", 3);            js_setproperty(J, -2, "lerp");
        js_newcfunction(J, jvec3f_distance, "distance", 2);    js_setproperty(J, -2, "distance");
        js_setglobal(J, "vec3f");
}
