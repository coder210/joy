#include <assert.h>
#include "jsclib.h"
#include "../jcollision.h"

static vec3f_t read_vec3f(js_State *J, int idx)
{
        vec3f_t v;
        js_getproperty(J, idx, "x"); v.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "y"); v.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "z"); v.z = js_toint64(J, -1); js_pop(J, 1);
        return v;
}

static void push_result(js_State *J, ray3d_collisionf_t r)
{
        js_newobject(J);
        js_pushboolean(J, r.hit);            js_setproperty(J, -2, "hit");
        js_pushint64(J, r.distance);         js_setproperty(J, -2, "distance");
        js_newobject(J); js_pushint64(J, r.normal.x); js_setproperty(J, -2, "x");
                          js_pushint64(J, r.normal.y); js_setproperty(J, -2, "y");
                          js_pushint64(J, r.normal.z); js_setproperty(J, -2, "z");
                          js_setproperty(J, -2, "normal");
        js_newobject(J); js_pushint64(J, r.point.x); js_setproperty(J, -2, "x");
                          js_pushint64(J, r.point.y); js_setproperty(J, -2, "y");
                          js_pushint64(J, r.point.z); js_setproperty(J, -2, "z");
                          js_setproperty(J, -2, "point");
}

static void jc3d_ray_quad_int(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        assert(js_isobject(J, 3)); assert(js_isobject(J, 4));
        assert(js_isobject(J, 5)); assert(js_isobject(J, 6));
        ray3df_t ray;
        ray.position  = read_vec3f(J, 1);
        ray.direction = read_vec3f(J, 2);
        vec3f_t p1 = read_vec3f(J, 3), p2 = read_vec3f(J, 4);
        vec3f_t p3 = read_vec3f(J, 5), p4 = read_vec3f(J, 6);
        push_result(J, collision3df_get_ray_quad(ray, p1, p2, p3, p4));
}

void jsopen_collision3d(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jc3d_ray_quad_int, "ray_quad_int", 6);
        js_setproperty(J, -2, "ray_quad_int");
        js_setglobal(J, "collision3d");
}
