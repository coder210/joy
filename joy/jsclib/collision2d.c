#include <assert.h>
#include <stdlib.h>
#include "jsclib.h"
#include "../jcollision.h"

static circle_t read_circle(js_State *J, int idx)
{
        circle_t c;
        js_getproperty(J, idx, "x"); c.center.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "y"); c.center.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "r"); c.radius = js_tonumber(J, -1); js_pop(J, 1);
        return c;
}

static rectangle_t read_rect(js_State *J, int idx)
{
        rectangle_t r;
        js_getproperty(J, idx, "x"); r.x = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "y"); r.y = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "w"); r.width = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "h"); r.height = js_tonumber(J, -1); js_pop(J, 1);
        return r;
}

static polygon_t read_poly(js_State *J, int idx)
{
        polygon_t p;
        p.num_vertices = js_getlength(J, idx);
        p.vertices = (vec2_t*)malloc(sizeof(vec2_t) * p.num_vertices);
        assert(p.vertices);
        for (int i = 0; i < p.num_vertices; i++) {
                js_getindex(J, idx, i);
                js_getproperty(J, -1, "x"); p.vertices[i].x = js_tonumber(J, -1); js_pop(J, 1);
                js_getproperty(J, -1, "y"); p.vertices[i].y = js_tonumber(J, -1); js_pop(J, 1);
                js_pop(J, 1);
        }
        return p;
}

static void free_poly(polygon_t p) { if (p.vertices) free(p.vertices); }

static void push_contact(js_State *J, contact2d_t *c)
{
        js_newobject(J);
        js_newobject(J); js_pushnumber(J, c->sp.x); js_setproperty(J, -2, "x");
                          js_pushnumber(J, c->sp.y); js_setproperty(J, -2, "y"); js_setproperty(J, -2, "sp");
        js_newobject(J); js_pushnumber(J, c->ep.x); js_setproperty(J, -2, "x");
                          js_pushnumber(J, c->ep.y); js_setproperty(J, -2, "y"); js_setproperty(J, -2, "ep");
        js_newobject(J); js_pushnumber(J, c->normal.x); js_setproperty(J, -2, "x");
                          js_pushnumber(J, c->normal.y); js_setproperty(J, -2, "y"); js_setproperty(J, -2, "normal");
        js_pushnumber(J, c->depth); js_setproperty(J, -2, "depth");
}

static void jc2d_circles(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        circle_t a = read_circle(J, 1), b = read_circle(J, 2);
        js_pushboolean(J, collision2d_check_circles(a, b));
}

static void jc2d_rectangles(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        rectangle_t a = read_rect(J, 1), b = read_rect(J, 2);
        js_pushboolean(J, collision2d_check_rectangles(a, b));
}

static void jc2d_circlesx(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        circle_t a = read_circle(J, 1), b = read_circle(J, 2);
        contact2d_t contact;
        int ok = collision2d_get_circles(a, b, &contact);
        js_pushboolean(J, ok != 0);
        if (ok) push_contact(J, &contact); else js_pushundefined(J);
}

static void jc2d_rectanglesx(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 3));
        rectangle_t a = read_rect(J, 1);
        ft_t a_angle = js_tonumber(J, 2);
        rectangle_t b = read_rect(J, 3);
        ft_t b_angle = js_tonumber(J, 4);
        contact2d_t contact;
        int ok = collision2d_get_rectangles(a, a_angle, b, b_angle, &contact);
        js_pushboolean(J, ok != 0);
        if (ok) push_contact(J, &contact); else js_pushundefined(J);
}

static void jc2d_polygons(js_State *J)
{
        assert(js_isobject(J, 1)); assert(js_isobject(J, 2));
        polygon_t a = read_poly(J, 1), b = read_poly(J, 2);
        contact2d_t contact;
        int ok = collision2d_get_polygons(a, b, &contact);
        free_poly(a); free_poly(b);
        js_pushboolean(J, ok != 0);
        if (ok) push_contact(J, &contact); else js_pushundefined(J);
}

void jsopen_collision2d(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jc2d_circles, "circles", 2);         js_setproperty(J, -2, "circles");
        js_newcfunction(J, jc2d_rectangles, "rectangles", 2);   js_setproperty(J, -2, "rectangles");
        js_newcfunction(J, jc2d_circlesx, "circlesx", 2);       js_setproperty(J, -2, "circlesx");
        js_newcfunction(J, jc2d_rectanglesx, "rectanglesx", 4); js_setproperty(J, -2, "rectanglesx");
        js_newcfunction(J, jc2d_polygons, "polygons", 2);       js_setproperty(J, -2, "polygons");
        js_setglobal(J, "collision2d");
}
