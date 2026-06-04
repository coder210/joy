#include <assert.h>
#include <stdlib.h>
#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../jcollision.h"
#include "luaclib.h"

static int l_collision2d_circles(lua_State *L)
{
        circle_t a, b;
        luaL_checktype(L, 1, LUA_TTABLE);
        luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.center.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.center.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "r"); a.radius = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.center.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.center.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "r"); b.radius = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_pushboolean(L, collision2d_check_circles(a, b));
        return 1;
}

static int l_collision2d_rectangles(lua_State *L)
{
        rectangle_t a, b;
        luaL_checktype(L, 1, LUA_TTABLE);
        luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "w"); a.width = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "h"); a.height = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "w"); b.width = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "h"); b.height = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_pushboolean(L, collision2d_check_rectangles(a, b));
        return 1;
}

static int l_collision2d_circlesx(lua_State *L)
{
        contact2d_t contact;
        circle_t a, b;
        luaL_checktype(L, 1, LUA_TTABLE);
        luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.center.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.center.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "r"); a.radius = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "x"); b.center.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); b.center.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "r"); b.radius = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        int result = collision2d_get_circles(a, b, &contact);
        lua_pushboolean(L, result != 0);
        if (result) {
                lua_newtable(L);
                lua_newtable(L);
                lua_pushnumber(L, contact.sp.x); lua_setfield(L, -2, "x");
                lua_pushnumber(L, contact.sp.y); lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "sp");
                lua_newtable(L);
                lua_pushnumber(L, contact.ep.x); lua_setfield(L, -2, "x");
                lua_pushnumber(L, contact.ep.y); lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "ep");
                lua_newtable(L);
                lua_pushnumber(L, contact.normal.x); lua_setfield(L, -2, "x");
                lua_pushnumber(L, contact.normal.y); lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "normal");
                lua_pushnumber(L, contact.depth);
                lua_setfield(L, -2, "depth");
        }
        return 2;
}

static int l_collision2d_rectanglesx(lua_State *L)
{
        contact2d_t contact;
        rectangle_t a, b;
        ft_t a_angle, b_angle;
        luaL_checktype(L, 1, LUA_TTABLE);
        luaL_checktype(L, 3, LUA_TTABLE);
        lua_getfield(L, 1, "x"); a.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "y"); a.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "w"); a.width = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 1, "h"); a.height = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        a_angle = (ft_t)luaL_checknumber(L, 2);
        lua_getfield(L, 3, "x"); b.x = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 3, "y"); b.y = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 3, "w"); b.width = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 3, "h"); b.height = (ft_t)luaL_checknumber(L, -1); lua_pop(L, 1);
        b_angle = (ft_t)luaL_checknumber(L, 4);
        int result = collision2d_get_rectangles(a, a_angle, b, b_angle, &contact);
        lua_pushboolean(L, result != 0);
        if (result) {
                lua_newtable(L);
                lua_newtable(L);
                lua_pushnumber(L, contact.sp.x); lua_setfield(L, -2, "x");
                lua_pushnumber(L, contact.sp.y); lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "sp");
                lua_newtable(L);
                lua_pushnumber(L, contact.ep.x); lua_setfield(L, -2, "x");
                lua_pushnumber(L, contact.ep.y); lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "ep");
                lua_newtable(L);
                lua_pushnumber(L, contact.normal.x); lua_setfield(L, -2, "x");
                lua_pushnumber(L, contact.normal.y); lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "normal");
                lua_pushnumber(L, contact.depth);
                lua_setfield(L, -2, "depth");
        }
        return 2;
}

static int l_collision2d_polygons(lua_State *L)
{
        contact2d_t contact;
        polygon_t a, b;
        luaL_checktype(L, 1, LUA_TTABLE);
        luaL_checktype(L, 2, LUA_TTABLE);
        a.num_vertices = (int)luaL_len(L, 1);
        b.num_vertices = (int)luaL_len(L, 2);
        a.vertices = (vec2_t*)malloc(sizeof(vec2_t) * a.num_vertices);
        assert(a.vertices);
        b.vertices = (vec2_t*)malloc(sizeof(vec2_t) * b.num_vertices);
        assert(b.vertices);
        for (int i = 0; i < a.num_vertices; i++) {
                lua_geti(L, 1, i + 1);
                lua_getfield(L, -1, "x"); a.vertices[i].x = (ft_t)luaL_checknumber(L, -1);
                lua_getfield(L, -2, "y"); a.vertices[i].y = (ft_t)luaL_checknumber(L, -1);
                lua_pop(L, 3);
        }
        for (int i = 0; i < b.num_vertices; i++) {
                lua_geti(L, 2, i + 1);
                lua_getfield(L, -1, "x"); b.vertices[i].x = (ft_t)luaL_checknumber(L, -1);
                lua_getfield(L, -2, "y"); b.vertices[i].y = (ft_t)luaL_checknumber(L, -1);
                lua_pop(L, 3);
        }
        int result = collision2d_get_polygons(a, b, &contact);
        free(a.vertices); free(b.vertices);
        lua_pushboolean(L, result != 0);
        if (result) {
                lua_newtable(L);
                lua_newtable(L);
                lua_pushnumber(L, contact.sp.x); lua_setfield(L, -2, "x");
                lua_pushnumber(L, contact.sp.y); lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "sp");
                lua_newtable(L);
                lua_pushnumber(L, contact.ep.x); lua_setfield(L, -2, "x");
                lua_pushnumber(L, contact.ep.y); lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "ep");
                lua_newtable(L);
                lua_pushnumber(L, contact.normal.x); lua_setfield(L, -2, "x");
                lua_pushnumber(L, contact.normal.y); lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "normal");
                lua_pushnumber(L, contact.depth);
                lua_setfield(L, -2, "depth");
        }
        return 2;
}

int luaopen_collision2d(lua_State *L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
        {"circles", l_collision2d_circles},
        {"rectangles", l_collision2d_rectangles},
        {"circlesx", l_collision2d_circlesx},
        {"rectanglesx", l_collision2d_rectanglesx},
        {"polygons", l_collision2d_polygons},
        {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
