#include <assert.h>
#include "jsclib.h"
#include "../jmath.h"

static void jvec3_negate(js_State *J)
{
        vec3_t a, r;
        assert(js_isobject(J, 1));
        js_getproperty(J, 1, "x"); a.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_tonumber(J, -1); js_pop(J, 1);
        r = vec3_negate(a);
        js_newobject(J);
        js_pushnumber(J, r.x); js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y); js_setproperty(J, -2, "y");
        js_pushnumber(J, r.z); js_setproperty(J, -2, "z");
}

static void jvec3_add(js_State *J)
{
        vec3_t a, b, r;
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        js_getproperty(J, 1, "x"); a.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "x"); b.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "y"); b.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "z"); b.z = js_tonumber(J, -1); js_pop(J, 1);
        r = vec3_add(a, b);
        js_newobject(J);
        js_pushnumber(J, r.x); js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y); js_setproperty(J, -2, "y");
        js_pushnumber(J, r.z); js_setproperty(J, -2, "z");
}

static void jvec3_sub(js_State *J)
{
        vec3_t a, b, r;
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        js_getproperty(J, 1, "x"); a.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "x"); b.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "y"); b.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "z"); b.z = js_tonumber(J, -1); js_pop(J, 1);
        r = vec3_sub(a, b);
        js_newobject(J);
        js_pushnumber(J, r.x); js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y); js_setproperty(J, -2, "y");
        js_pushnumber(J, r.z); js_setproperty(J, -2, "z");
}

static void jvec3_scale(js_State *J)
{
        vec3_t a, r; ft_t s;
        assert(js_isobject(J, 1));
        js_getproperty(J, 1, "x"); a.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_tonumber(J, -1); js_pop(J, 1);
        s = js_tonumber(J, 2);
        r = vec3_scale(a, s);
        js_newobject(J);
        js_pushnumber(J, r.x); js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y); js_setproperty(J, -2, "y");
        js_pushnumber(J, r.z); js_setproperty(J, -2, "z");
}

static void jvec3_dot(js_State *J)
{
        vec3_t a, b;
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        js_getproperty(J, 1, "x"); a.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "x"); b.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "y"); b.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "z"); b.z = js_tonumber(J, -1); js_pop(J, 1);
        js_pushnumber(J, vec3_dot(a, b));
}

static void jvec3_cross(js_State *J)
{
        vec3_t a, b, r;
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        js_getproperty(J, 1, "x"); a.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "x"); b.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "y"); b.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "z"); b.z = js_tonumber(J, -1); js_pop(J, 1);
        r = vec3_cross(a, b);
        js_newobject(J);
        js_pushnumber(J, r.x); js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y); js_setproperty(J, -2, "y");
        js_pushnumber(J, r.z); js_setproperty(J, -2, "z");
}

static void jvec3_length_squared(js_State *J)
{
        vec3_t a;
        assert(js_isobject(J, 1));
        js_getproperty(J, 1, "x"); a.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_tonumber(J, -1); js_pop(J, 1);
        js_pushnumber(J, vec3_square_length(a));
}

static void jvec3_length(js_State *J)
{
        vec3_t a;
        assert(js_isobject(J, 1));
        js_getproperty(J, 1, "x"); a.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_tonumber(J, -1); js_pop(J, 1);
        js_pushnumber(J, vec3_length(a));
}

static void jvec3_normalize(js_State *J)
{
        vec3_t a, r;
        assert(js_isobject(J, 1));
        js_getproperty(J, 1, "x"); a.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_tonumber(J, -1); js_pop(J, 1);
        r = vec3_normalize(a);
        js_newobject(J);
        js_pushnumber(J, r.x); js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y); js_setproperty(J, -2, "y");
        js_pushnumber(J, r.z); js_setproperty(J, -2, "z");
}

static void jvec3_lerp(js_State *J)
{
        vec3_t a, b, r; ft_t t;
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        js_getproperty(J, 1, "x"); a.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "x"); b.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "y"); b.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "z"); b.z = js_tonumber(J, -1); js_pop(J, 1);
        t = js_tonumber(J, 3);
        r = vec3_lerp(a, b, t);
        js_newobject(J);
        js_pushnumber(J, r.x); js_setproperty(J, -2, "x");
        js_pushnumber(J, r.y); js_setproperty(J, -2, "y");
        js_pushnumber(J, r.z); js_setproperty(J, -2, "z");
}

static void jvec3_distance(js_State *J)
{
        vec3_t a, b;
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        js_getproperty(J, 1, "x"); a.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "y"); a.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 1, "z"); a.z = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "x"); b.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "y"); b.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, 2, "z"); b.z = js_tonumber(J, -1); js_pop(J, 1);
        js_pushnumber(J, vec3_distance(a, b));
}

void jsopen_vec3(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jvec3_negate, "negate", 1);        js_setproperty(J, -2, "negate");
        js_newcfunction(J, jvec3_add, "add", 2);              js_setproperty(J, -2, "add");
        js_newcfunction(J, jvec3_sub, "sub", 2);              js_setproperty(J, -2, "sub");
        js_newcfunction(J, jvec3_scale, "scale", 2);          js_setproperty(J, -2, "scale");
        js_newcfunction(J, jvec3_dot, "dot", 2);              js_setproperty(J, -2, "dot");
        js_newcfunction(J, jvec3_cross, "cross", 2);          js_setproperty(J, -2, "cross");
        js_newcfunction(J, jvec3_length_squared, "length_squared", 1); js_setproperty(J, -2, "length_squared");
        js_newcfunction(J, jvec3_length, "length", 1);        js_setproperty(J, -2, "length");
        js_newcfunction(J, jvec3_normalize, "normalize", 1);  js_setproperty(J, -2, "normalize");
        js_newcfunction(J, jvec3_lerp, "lerp", 3);            js_setproperty(J, -2, "lerp");
        js_newcfunction(J, jvec3_distance, "distance", 2);    js_setproperty(J, -2, "distance");
        js_setglobal(J, "vec3");
}
