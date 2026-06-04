#include <assert.h>
#include <stdlib.h>
#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../jcollision.h"
#include "luaclib.h"

static int l_collision2df_get_ray_rectangle(lua_State *L)
{
        ray2d_collisionf_t result;
        ray2df_t ray;
        rectanglef_t rect;

        luaL_checktype(L, 1, LUA_TTABLE);
        luaL_checktype(L, 2, LUA_TTABLE);
        luaL_checktype(L, 3, LUA_TTABLE);

        lua_getfield(L, 1, "x");
        ray.origin.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "y");
        ray.origin.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "x");
        ray.direction.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "y");
        ray.direction.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 3, "x");
        rect.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "y");
        rect.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "w");
        rect.width = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "h");
        rect.height = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        result = collision2df_get_ray_rectangle(ray, rect);
        
        lua_newtable(L);
        lua_pushboolean(L, result.hit);
        lua_setfield(L, -2, "hit");
        lua_pushinteger(L, result.distance);
        lua_setfield(L, -2, "distance");

        lua_newtable(L);
        lua_pushinteger(L, result.normal.x);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, result.normal.y);
        lua_setfield(L, -2, "y");
        lua_setfield(L, -2, "normal");
        
        lua_newtable(L);
        lua_pushinteger(L, result.point.x);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, result.point.y);
        lua_setfield(L, -2, "y");
        lua_setfield(L, -2, "point");

        return 1;
}

static int l_collision2df_get_ray_rectanglex(lua_State *L)
{
        ray2d_collisionf_t result;
        ray2df_t ray;
        rectanglef_t rect;
        fp_t angle;

        luaL_checktype(L, 1, LUA_TTABLE);
        luaL_checktype(L, 2, LUA_TTABLE);
        luaL_checktype(L, 3, LUA_TTABLE);

        lua_getfield(L, 1, "x");
        ray.origin.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "y");
        ray.origin.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "x");
        ray.direction.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "y");
        ray.direction.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 3, "x");
        rect.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "y");
        rect.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "w");
        rect.width = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "h");
        rect.height = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        angle = luaL_checkinteger(L, 4);

        result = collision2df_get_ray_rectanglex(ray, rect, angle);
        
        lua_newtable(L);
        lua_pushboolean(L, result.hit);
        lua_setfield(L, -2, "hit");
        lua_pushinteger(L, result.distance);
        lua_setfield(L, -2, "distance");

        lua_newtable(L);
        lua_pushinteger(L, result.normal.x);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, result.normal.y);
        lua_setfield(L, -2, "y");
        lua_setfield(L, -2, "normal");
        
        lua_newtable(L);
        lua_pushinteger(L, result.point.x);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, result.point.y);
        lua_setfield(L, -2, "y");
        lua_setfield(L, -2, "point");

        return 1;
}

static int l_collision2df_circles(lua_State *L)
{
        bool result;
        circlef_t a, b;

        luaL_checktype(L, 1, LUA_TTABLE);
        luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 1, "x");
        a.center.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "y");
        a.center.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "r");
        a.radius = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "x");
        b.center.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "y");
        b.center.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "r");
        b.radius = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        result = collision2df_check_circles(a, b);

        lua_pushboolean(L, result);
        return 1;
}

static int l_collision2df_rectangles(lua_State *L)
{
        bool result;
        rectanglef_t a, b;

        luaL_checktype(L, 1, LUA_TTABLE);
        luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 1, "x");
        a.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "y");
        a.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "w");
        a.width = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "h");
        a.height = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "x");
        b.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "y");
        b.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "w");
        b.width = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "h");
        b.height = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        result = collision2df_check_rectangles(a, b);
        lua_pushboolean(L, result);
        return 1;
}

static int l_collision2df_circlesx(lua_State *L)
{
        int result;
        contact2df_t contact;
        circlef_t a, b;

        luaL_checktype(L, 1, LUA_TTABLE);
        luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 1, "x");
        a.center.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "y");
        a.center.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "r");
        a.radius = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "x");
        b.center.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "y");
        b.center.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "r");
        b.radius = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        result = collision2df_get_circles(a, b, &contact);
        lua_pushboolean(L, result != 0);

        if (result){
                lua_newtable(L);
                lua_newtable(L);
                lua_pushinteger(L, contact.sp.x);
                lua_setfield(L, -2, "x");
                lua_pushinteger(L, contact.sp.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "sp");

                lua_newtable(L);
                lua_pushinteger(L, contact.ep.x);
                lua_setfield(L, -2, "x");
                lua_pushinteger(L, contact.ep.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "ep");

                lua_newtable(L);
                lua_pushinteger(L, contact.normal.x);
                lua_setfield(L, -2, "x");
                lua_pushinteger(L, contact.normal.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "normal");

                lua_pushinteger(L, contact.depth);
                lua_setfield(L, -2, "depth");
        }

        return 2;
}

static int l_collision2df_rectanglesx(lua_State *L)
{
        int result;
        contact2df_t contact;
        rectanglef_t a, b;
        fp_t a_angle, b_angle;

        luaL_checktype(L, 1, LUA_TTABLE);
        luaL_checktype(L, 3, LUA_TTABLE);

        lua_getfield(L, 1, "x");
        a.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "y");
        a.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "w");
        a.width = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 1, "h");
        a.height = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        a_angle = luaL_checkinteger(L, 2);

        lua_getfield(L, 3, "x");
        b.x = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "y");
        b.y = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "w");
        b.width = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "h");
        b.height = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        b_angle = luaL_checkinteger(L, 4);

        result = collision2df_get_rectangles(a, a_angle, b, b_angle, &contact);
        
        lua_pushboolean(L, result != 0);
        if (result){
                lua_newtable(L);

                lua_newtable(L);
                lua_pushinteger(L, contact.sp.x);
                lua_setfield(L, -2, "x");
                lua_pushinteger(L, contact.sp.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "sp");
        
                lua_newtable(L);
                lua_pushinteger(L, contact.ep.x);
                lua_setfield(L, -2, "x");
                lua_pushinteger(L, contact.ep.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "ep");
        
                lua_newtable(L);
                lua_pushinteger(L, contact.normal.x);
                lua_setfield(L, -2, "x");
                lua_pushinteger(L, contact.normal.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "normal");
        
                lua_pushinteger(L, contact.depth);
                lua_setfield(L, -2, "depth");  
        }

        return 2;
}

static int l_collision2df_polygons(lua_State *L)
{
        int result;
        contact2df_t contact;
        polygonf_t a, b;

        luaL_checktype(L, 1, LUA_TTABLE);
        luaL_checktype(L, 2, LUA_TTABLE);
        a.num_vertices = (int)luaL_len(L, 1);
        b.num_vertices = (int)luaL_len(L, 2);

	a.vertices = (vec2f_t*)malloc(sizeof(vec2f_t) * a.num_vertices);
	assert(a.vertices);
	b.vertices = (vec2f_t*)malloc(sizeof(vec2f_t) * b.num_vertices);
	assert(b.vertices);

        for (int i = 0; i < a.num_vertices; i++) {
		lua_geti(L, 1, i + 1);
		lua_getfield(L, -1, "x");
		a.vertices[i].x = luaL_checkinteger(L, -1);
		lua_getfield(L, -2, "y");
		a.vertices[i].y = luaL_checkinteger(L, -1);
		lua_pop(L, 3);
	}

        for (int i = 0; i < b.num_vertices; i++) {
		lua_geti(L, 2, i + 1);
		lua_getfield(L, -1, "x");
		b.vertices[i].x = luaL_checkinteger(L, -1);
		lua_getfield(L, -2, "y");
		b.vertices[i].y = luaL_checkinteger(L, -1);
		lua_pop(L, 3);
	}

        result = collision2df_get_polygons(a, b, &contact);

        free(a.vertices);
        free(b.vertices);
        
        lua_pushboolean(L, result != 0);

        if (result){
                lua_newtable(L);

                lua_newtable(L);
                lua_pushinteger(L, contact.sp.x);
                lua_setfield(L, -2, "x");
                lua_pushinteger(L, contact.sp.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "sp");
        
                lua_newtable(L);
                lua_pushinteger(L, contact.ep.x);
                lua_setfield(L, -2, "x");
                lua_pushinteger(L, contact.ep.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "ep");
        
                lua_newtable(L);
                lua_pushinteger(L, contact.normal.x);
                lua_setfield(L, -2, "x");
                lua_pushinteger(L, contact.normal.y);
                lua_setfield(L, -2, "y");
                lua_setfield(L, -2, "normal");
        
                lua_pushinteger(L, contact.depth);
                lua_setfield(L, -2, "depth");
        }
        
        return 2;
}

int luaopen_collision2df(lua_State *L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
        {"get_ray_rectangle", l_collision2df_get_ray_rectangle},
        {"get_ray_rectanglex", l_collision2df_get_ray_rectanglex},
        {"circles", l_collision2df_circles},
        {"rectangles", l_collision2df_rectangles},
        {"circlesx", l_collision2df_circlesx},
        {"rectanglesx", l_collision2df_rectanglesx},
        {"polygons", l_collision2df_polygons},
        {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
