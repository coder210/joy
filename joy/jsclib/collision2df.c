#include <assert.h>
#include <stdlib.h>
#include "jsclib.h"
#include "../jcollision.h"

static circlef_t read_circlef(js_State *J, int idx)
{
        circlef_t c;
        js_getproperty(J, idx, "x"); c.center.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "y"); c.center.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "r"); c.radius = js_toint64(J, -1); js_pop(J, 1);
        return c;
}

static rectanglef_t read_rectf(js_State *J, int idx)
{
        rectanglef_t r;
        js_getproperty(J, idx, "x"); r.x = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "y"); r.y = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "w"); r.width = js_toint64(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "h"); r.height = js_toint64(J, -1); js_pop(J, 1);
        return r;
}

static polygonf_t read_polyf(js_State *J, int idx)
{
        polygonf_t p;
        p.num_vertices = js_getlength(J, idx);
        p.vertices = (vec2f_t*)malloc(sizeof(vec2f_t) * p.num_vertices);
        assert(p.vertices);
        for (int i = 0; i < p.num_vertices; i++) {
                js_getindex(J, idx, i);
                js_getproperty(J, -1, "x"); p.vertices[i].x = js_toint64(J, -1); js_pop(J, 1);
                js_getproperty(J, -1, "y"); p.vertices[i].y = js_toint64(J, -1); js_pop(J, 1);
                js_pop(J, 1);
        }
        return p;
}

static void freep(polygonf_t p) { if (p.vertices) free(p.vertices); }

static void push_contactf(js_State *J, contact2df_t *c)
{
        js_newobject(J);
        js_newobject(J); js_pushint64(J, c->sp.x); js_setproperty(J, -2, "x");
                          js_pushint64(J, c->sp.y); js_setproperty(J, -2, "y"); js_setproperty(J, -2, "sp");
        js_newobject(J); js_pushint64(J, c->ep.x); js_setproperty(J, -2, "x");
                          js_pushint64(J, c->ep.y); js_setproperty(J, -2, "y"); js_setproperty(J, -2, "ep");
        js_newobject(J); js_pushint64(J, c->normal.x); js_setproperty(J, -2, "x");
                          js_pushint64(J, c->normal.y); js_setproperty(J, -2, "y"); js_setproperty(J, -2, "normal");
        js_pushint64(J, c->depth); js_setproperty(J, -2, "depth");
}

static void jc2df_circles(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        circlef_t a = read_circlef(J, 1), b = read_circlef(J, 2);
        js_pushboolean(J, collision2df_check_circles(a, b));
}

static void jc2df_rectangles(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        rectanglef_t a = read_rectf(J, 1), b = read_rectf(J, 2);
        js_pushboolean(J, collision2df_check_rectangles(a, b));
}

static void jc2df_circlesx(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        circlef_t a = read_circlef(J, 1), b = read_circlef(J, 2);
        contact2df_t contact;
        int ok = collision2df_get_circles(a, b, &contact);
        js_pushboolean(J, ok != 0);
        if (ok) push_contactf(J, &contact); else js_pushundefined(J);
}

static void jc2df_rectanglesx(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 3));
        rectanglef_t a = read_rectf(J, 1);
        fp_t a_angle = js_toint64(J, 2);
        rectanglef_t b = read_rectf(J, 3);
        fp_t b_angle = js_toint64(J, 4);
        contact2df_t contact;
        int ok = collision2df_get_rectangles(a, a_angle, b, b_angle, &contact);
        js_pushboolean(J, ok != 0);
        if (ok) push_contactf(J, &contact); else js_pushundefined(J);
}

static void jc2df_polygons(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        polygonf_t a = read_polyf(J, 1), b = read_polyf(J, 2);
        contact2df_t contact;
        int ok = collision2df_get_polygons(a, b, &contact);
        freep(a); freep(b);
        js_pushboolean(J, ok != 0);
        if (ok) push_contactf(J, &contact); else js_pushundefined(J);
}

void jsopen_collision2df(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jc2df_circles, "circles", 2);         js_setproperty(J, -2, "circles");
        js_newcfunction(J, jc2df_rectangles, "rectangles", 2);   js_setproperty(J, -2, "rectangles");
        js_newcfunction(J, jc2df_circlesx, "circlesx", 2);       js_setproperty(J, -2, "circlesx");
        js_newcfunction(J, jc2df_rectanglesx, "rectanglesx", 4); js_setproperty(J, -2, "rectanglesx");
        js_newcfunction(J, jc2df_polygons, "polygons", 2);       js_setproperty(J, -2, "polygons");
        js_setglobal(J, "collision2df");
}
