#include <assert.h>
#include "jsclib.h"
#include "../jui.h"

#define JOYSTICK_TAG "joystick"

static void joystick_finalize(js_State *J, void *p)
{
        (void)J; if (p) joystick_destroy((joystick_p)p);
}

static void jjoy_create(js_State *J)
{
        SDL_Renderer *ren = (SDL_Renderer*)js_touserdata(J, 1, "SDL_Renderer*");
        float x = js_tonumber(J, 2), y = js_tonumber(J, 3), radius = js_tonumber(J, 4);
        joystick_p joy = joystick_create(ren, x, y, radius);
        js_newuserdata(J, JOYSTICK_TAG, joy, joystick_finalize);
}

static void jjoy_destroy(js_State *J)
{
        joystick_p joy = js_touserdata(J, 1, JOYSTICK_TAG);
        if (joy) { joystick_destroy(joy); }
        js_pushnull(J);
}

static void jjoy_draw(js_State *J)
{
        joystick_p joy = js_touserdata(J, 1, JOYSTICK_TAG);
        if (joy) joystick_draw(joy);
}

static void jjoy_is_dragging(js_State *J)
{
        joystick_p joy = js_touserdata(J, 1, JOYSTICK_TAG);
        js_pushboolean(J, joy && joystick_is_dragging(joy));
}

static void jjoy_touch_down(js_State *J)
{
        joystick_p joy = js_touserdata(J, 1, JOYSTICK_TAG);
        if (joy) joystick_touch_down(joy, js_tonumber(J, 2), js_tonumber(J, 3));
}

static void jjoy_touch_move(js_State *J)
{
        joystick_p joy = js_touserdata(J, 1, JOYSTICK_TAG);
        if (joy) joystick_touch_move(joy, js_tonumber(J, 2), js_tonumber(J, 3));
}

static void jjoy_touch_up(js_State *J)
{
        joystick_p joy = js_touserdata(J, 1, JOYSTICK_TAG);
        if (joy) joystick_touch_up(joy);
}

static void jjoy_reset(js_State *J)
{
        joystick_p joy = js_touserdata(J, 1, JOYSTICK_TAG);
        if (joy) joystick_reset(joy);
}

static void jjoy_set_position(js_State *J)
{
        joystick_p joy = js_touserdata(J, 1, JOYSTICK_TAG);
        if (joy) joystick_set_position(joy, js_tonumber(J, 2), js_tonumber(J, 3));
}

static void jjoy_get_direction(js_State *J)
{
        joystick_p joy = js_touserdata(J, 1, JOYSTICK_TAG);
        SDL_FPoint pt = joystick_get_direction(joy);
        js_pushnumber(J, pt.x);
        js_pushnumber(J, pt.y);
}

static void jjoy_get_magnitude(js_State *J)
{
        joystick_p joy = js_touserdata(J, 1, JOYSTICK_TAG);
        js_pushnumber(J, joystick_get_magnitude(joy));
}

void jsopen_joystick(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jjoy_create, "create", 4);          js_setproperty(J, -2, "create");
        js_newcfunction(J, jjoy_destroy, "destroy", 1);        js_setproperty(J, -2, "destroy");
        js_newcfunction(J, jjoy_draw, "draw", 1);              js_setproperty(J, -2, "draw");
        js_newcfunction(J, jjoy_is_dragging, "is_dragging", 1); js_setproperty(J, -2, "is_dragging");
        js_newcfunction(J, jjoy_touch_down, "touch_down", 3);  js_setproperty(J, -2, "touch_down");
        js_newcfunction(J, jjoy_touch_move, "touch_move", 3);  js_setproperty(J, -2, "touch_move");
        js_newcfunction(J, jjoy_touch_up, "touch_up", 1);      js_setproperty(J, -2, "touch_up");
        js_newcfunction(J, jjoy_reset, "reset", 1);            js_setproperty(J, -2, "reset");
        js_newcfunction(J, jjoy_set_position, "set_position", 3); js_setproperty(J, -2, "set_position");
        js_newcfunction(J, jjoy_get_direction, "get_direction", 1); js_setproperty(J, -2, "get_direction");
        js_newcfunction(J, jjoy_get_magnitude, "get_magnitude", 1); js_setproperty(J, -2, "get_magnitude");
        js_setglobal(J, "joystick");
}
