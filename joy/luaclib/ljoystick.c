#include <stdbool.h>
#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../jui.h"
#include "luaclib.h"

static int l_joystick_create(lua_State* L)
{
        float x, y, radius;
        SDL_Renderer* renderer;
        joystick_p joystick;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        x = (float)luaL_checknumber(L, 2);
        y = (float)luaL_checknumber(L, 3);
        radius = (float)luaL_checknumber(L, 4);
        joystick = joystick_create(renderer, x, y, radius);
        lua_pushlightuserdata(L, joystick);
        return 1;
}

static int l_joystick_destroy(lua_State* L)
{
        joystick_p joystick;
        SDL_assert(lua_islightuserdata(L, 1));
        joystick = (joystick_p)lua_touserdata(L, 1);
        joystick_destroy(joystick);
        return 0;
}

static int l_joystick_draw(lua_State* L)
{
        joystick_p joystick;
        SDL_assert(lua_islightuserdata(L, 1));
        joystick = (joystick_p)lua_touserdata(L, 1);
        joystick_draw(joystick);
        return 0;
}

static int l_joystick_is_dragging(lua_State* L)
{
        joystick_p j = (joystick_p)lua_touserdata(L, 1);
        lua_pushboolean(L, joystick_is_dragging(j));
        return 1;
}

static int l_joystick_touch_down(lua_State* L)
{
        joystick_p j = (joystick_p)lua_touserdata(L, 1);
        float x = (float)luaL_checknumber(L, 2);
        float y = (float)luaL_checknumber(L, 3);
        joystick_touch_down(j, x, y);
        return 0;
}

static int l_joystick_touch_move(lua_State* L)
{
        joystick_p j = (joystick_p)lua_touserdata(L, 1);
        float x = (float)luaL_checknumber(L, 2);
        float y = (float)luaL_checknumber(L, 3);
        joystick_touch_move(j, x, y);
        return 0;
}

static int l_joystick_touch_up(lua_State* L)
{
        joystick_p j = (joystick_p)lua_touserdata(L, 1);
        joystick_touch_up(j);
        return 0;
}

static int l_joystick_reset(lua_State* L)
{
        joystick_p joystick;
        SDL_assert(lua_islightuserdata(L, 1));
        joystick = (joystick_p)lua_touserdata(L, 1);
        joystick_reset(joystick);
        return 0;
}

static int l_joystick_set_position(lua_State* L)
{
        float x, y;
        joystick_p joystick;
        SDL_assert(lua_islightuserdata(L, 1));
        joystick = (joystick_p)lua_touserdata(L, 1);
        x = (float)luaL_checknumber(L, 2);
        y = (float)luaL_checknumber(L, 3);
        joystick_set_position(joystick, x, y);
        return 0;
}

static int l_joystick_get_direction(lua_State* L)
{
        SDL_FPoint point;
        joystick_p joystick;
        SDL_assert(lua_islightuserdata(L, 1));
        joystick = (joystick_p)lua_touserdata(L, 1);
        point = joystick_get_direction(joystick);
        lua_newtable(L);
        lua_pushnumber(L, point.x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, point.y); lua_setfield(L, -2, "y");
        return 1;
}

static int l_joystick_get_magnitude(lua_State* L)
{
        float magnitude;
        joystick_p joystick;
        SDL_assert(lua_islightuserdata(L, 1));
        joystick = (joystick_p)lua_touserdata(L, 1);
        magnitude = joystick_get_magnitude(joystick);
        lua_pushnumber(L, magnitude);
        return 1;
}

int luaopen_joystick(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
            {"create", l_joystick_create},
            {"destroy", l_joystick_destroy},
            {"draw", l_joystick_draw},
            {"is_dragging", l_joystick_is_dragging},
            {"touch_down", l_joystick_touch_down},
            {"touch_move", l_joystick_touch_move},
            {"touch_up", l_joystick_touch_up},
            {"reset", l_joystick_reset},
            {"set_position", l_joystick_set_position},
            {"get_direction", l_joystick_get_direction},
            {"get_magnitude", l_joystick_get_magnitude},
            {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}