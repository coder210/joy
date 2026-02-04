#include <assert.h>
#include <stdlib.h>
#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"
#include "liv-collision.h"
#include "luaclib.h"


//static int lcircles(lua_State *L)
//{
//        bool result;
//        luaL_checktype(L, 1, LUA_TTABLE);
//        luaL_checktype(L, 2, LUA_TTABLE);
//        circle a, b;
//        lua_getfield(L, 1, "x");
//        a.center.x = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 1, "y");
//        a.center.y = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 1, "r");
//        a.radius = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//
//        lua_getfield(L, 2, "x");
//        b.center.x = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 2, "y");
//        b.center.y = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 2, "r");
//        b.radius = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//
//        result = check_collision_circles(a, b);
//        lua_pushboolean(L, result);
//        return 1;
//}
//
//static int lrectangles(lua_State *L)
//{
//        bool result;
//        luaL_checktype(L, 1, LUA_TTABLE);
//        luaL_checktype(L, 2, LUA_TTABLE);
//        rectangle a, b;
//        lua_getfield(L, 1, "x");
//        a.x = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 1, "y");
//        a.y = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 1, "w");
//        a.width = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 1, "h");
//        a.height = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//
//        lua_getfield(L, 2, "x");
//        b.x = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 2, "y");
//        b.y = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 2, "w");
//        b.width = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 2, "h");
//        b.height = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        result = check_collision_rectangles(a, b);
//        lua_pushboolean(L, result);
//        return 1;
//}
//
//static int lcirclesx(lua_State *L)
//{
//        int result;
//        contact2d contact;
//        luaL_checktype(L, 1, LUA_TTABLE);
//        luaL_checktype(L, 2, LUA_TTABLE);
//        circle a, b;
//        lua_getfield(L, 1, "x");
//        a.center.x = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 1, "y");
//        a.center.y = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 1, "r");
//        a.radius = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//
//        lua_getfield(L, 2, "x");
//        b.center.x = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 2, "y");
//        b.center.y = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 2, "r");
//        b.radius = (float)luaL_checknumber(L, -1);
//        lua_pop(L, 1);
//
//        result = get_collision_circles(a, b, &contact);
//        lua_pushinteger(L, result);
//
//        if (result){
//                lua_newtable(L);
//                lua_newtable(L);
//                lua_pushnumber(L, contact.sp.x);
//                lua_setfield(L, -2, "x");
//                lua_pushnumber(L, contact.sp.y);
//                lua_setfield(L, -2, "y");
//                lua_setfield(L, -2, "sp");
//
//                lua_newtable(L);
//                lua_pushnumber(L, contact.ep.x);
//                lua_setfield(L, -2, "x");
//                lua_pushnumber(L, contact.ep.y);
//                lua_setfield(L, -2, "y");
//                lua_setfield(L, -2, "ep");
//
//                lua_newtable(L);
//                lua_pushnumber(L, contact.normal.x);
//                lua_setfield(L, -2, "x");
//                lua_pushnumber(L, contact.normal.y);
//                lua_setfield(L, -2, "y");
//                lua_setfield(L, -2, "normal");
//
//                lua_pushnumber(L, contact.depth);
//                lua_setfield(L, -2, "depth");
//        }
//        else{
//                lua_pushnil(L);
//        }
//
//        return 2;
//}
//
//static int lrectanglesx(lua_State *L)
//{
//        int result;
//        contact2d contact;
//
//        luaL_checktype(L, 1, LUA_TTABLE);
//        luaL_checktype(L, 3, LUA_TTABLE);
//        
//        rectangle a, b;
//        ft a_angle, b_angle;
//
//        lua_getfield(L, 1, "x");
//        a.x = FTFF((float)luaL_checknumber(L, -1));
//        lua_pop(L, 1);
//        lua_getfield(L, 1, "y");
//        a.y = FTFF((float)luaL_checknumber(L, -1));
//        lua_pop(L, 1);
//        lua_getfield(L, 1, "w");
//        a.width = FTFF((float)luaL_checknumber(L, -1));
//        lua_pop(L, 1);
//        lua_getfield(L, 1, "h");
//        a.height = FTFF((float)luaL_checknumber(L, -1));
//        lua_pop(L, 1);
//
//        a_angle = FTFF((float)luaL_checknumber(L, 2));
//
//        lua_getfield(L, 3, "x");
//        b.x = FTFF((float)luaL_checknumber(L, -1));
//        lua_pop(L, 1);
//        lua_getfield(L, 3, "y");
//        b.y = FTFF((float)luaL_checknumber(L, -1));
//        lua_pop(L, 1);
//        lua_getfield(L, 3, "w");
//        b.width = FTFF((float)luaL_checknumber(L, -1));
//        lua_pop(L, 1);
//        lua_getfield(L, 3, "h");
//        b.height = FTFF((float)luaL_checknumber(L, -1));
//        lua_pop(L, 1);
//
//        b_angle = FTFF((float)luaL_checknumber(L, 4));
//
//        result = get_collision_rectangles(a, a_angle, b, b_angle, &contact);
//        
//        lua_pushinteger(L, result);
//        if (result){
//                lua_newtable(L);
//
//                lua_newtable(L);
//                lua_pushnumber(L, contact.sp.x);
//                lua_setfield(L, -2, "x");
//                lua_pushnumber(L, contact.sp.y);
//                lua_setfield(L, -2, "y");
//                lua_setfield(L, -2, "sp");
//        
//                lua_newtable(L);
//                lua_pushnumber(L, contact.ep.x);
//                lua_setfield(L, -2, "x");
//                lua_pushnumber(L, contact.ep.y);
//                lua_setfield(L, -2, "y");
//                lua_setfield(L, -2, "ep");
//        
//                lua_newtable(L);
//                lua_pushnumber(L, contact.normal.x);
//                lua_setfield(L, -2, "x");
//                lua_pushnumber(L, contact.normal.y);
//                lua_setfield(L, -2, "y");
//                lua_setfield(L, -2, "normal");
//        
//                lua_pushnumber(L, contact.depth);
//                lua_setfield(L, -2, "depth");  
//        }
//        else{
//                lua_pushnil(L);
//        }
//
//        return 2;
//}
//
//
//static int lpolygons(lua_State *L)
//{
//        int result;
//        contact2d contact;
//        luaL_checktype(L, 1, LUA_TTABLE);
//        luaL_checktype(L, 2, LUA_TTABLE);
//        polygon a, b;
//        a.num_vertices = (int)luaL_len(L, 1);
//        b.num_vertices = (int)luaL_len(L, 2);
//
//	a.vertices = (vec2*)malloc(sizeof(vec2) * a.num_vertices);
//	assert(a.vertices);
//	b.vertices = (vec2*)malloc(sizeof(vec2) * b.num_vertices);
//	assert(b.vertices);
//
//	for (int i = 0; i < a.num_vertices; i++) {
//		lua_geti(L, 1, i + 1);
//		lua_getfield(L, -1, "x");
//                ft t = (float)luaL_checknumber(L, -1);
//		a.vertices[i].x = (float)luaL_checknumber(L, -1);
//		lua_getfield(L, -2, "y");
//		a.vertices[i].y = (float)luaL_checknumber(L, -1);
//		lua_pop(L, 3);
//	}
//
//        for (int i = 0; i < b.num_vertices; i++) {
//		lua_geti(L, 2, i + 1);
//		lua_getfield(L, -1, "x");
//		b.vertices[i].x = (float)luaL_checknumber(L, -1);
//		lua_getfield(L, -2, "y");
//		b.vertices[i].y = (float)luaL_checknumber(L, -1);
//		lua_pop(L, 3);
//	}
//
//        result = get_collision_polygons(a, b, &contact);
//
//        free(a.vertices);
//        free(b.vertices);
//        
//        lua_pushinteger(L, result);
//
//        if (result){
//                lua_newtable(L);
//
//                lua_newtable(L);
//                lua_pushnumber(L, contact.sp.x);
//                lua_setfield(L, -2, "x");
//                lua_pushnumber(L, contact.sp.y);
//                lua_setfield(L, -2, "y");
//                lua_setfield(L, -2, "sp");
//        
//                lua_newtable(L);
//                lua_pushnumber(L, contact.ep.x);
//                lua_setfield(L, -2, "x");
//                lua_pushnumber(L, contact.ep.y);
//                lua_setfield(L, -2, "y");
//                lua_setfield(L, -2, "ep");
//        
//                lua_newtable(L);
//                lua_pushnumber(L, contact.normal.x);
//                lua_setfield(L, -2, "x");
//                lua_pushnumber(L, contact.normal.y);
//                lua_setfield(L, -2, "y");
//                lua_setfield(L, -2, "normal");
//        
//                lua_pushnumber(L, contact.depth);
//                lua_setfield(L, -2, "depth");
//        }
//        else{
//                lua_pushnil(L);
//        }
//        
//        return 2;
//}


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
        lua_pushinteger(L, result);

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
        else{
                lua_pushnil(L);
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
        
        lua_pushinteger(L, result);
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
        else{
                lua_pushnil(L);
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
                printf("<%f,%f>\n", FPTF(a.vertices[i].x), FPTF(a.vertices[i].y));
	}

        for (int i = 0; i < b.num_vertices; i++) {
		lua_geti(L, 2, i + 1);
		lua_getfield(L, -1, "x");
		b.vertices[i].x = luaL_checkinteger(L, -1);
		lua_getfield(L, -2, "y");
		b.vertices[i].y = luaL_checkinteger(L, -1);
		lua_pop(L, 3);
                printf("<%f,%f>\n", FPTF(b.vertices[i].x), FPTF(b.vertices[i].y));
	}

        result = collision2df_get_polygons(a, b, &contact);

        free(a.vertices);
        free(b.vertices);
        
        lua_pushinteger(L, result);

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
        else{
                lua_pushnil(L);
        }
        
        return 2;
}

int luaopen_collision2d(lua_State *L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
	/* {"circles", lcircles},
	{"rectangles", lrectangles},
	{"circlesx", lcirclesx},
	{"polygons", lpolygons},*/

        {"get_ray_rectanglef", l_collision2df_get_ray_rectangle},
        {"get_ray_rectanglexf", l_collision2df_get_ray_rectanglex},
        {"circlesf", l_collision2df_circles},
        {"rectanglesf", l_collision2df_rectangles},
        {"circlesxf", l_collision2df_circlesx},
        {"rectanglesxf", l_collision2df_rectanglesx},
        {"polygonsf", l_collision2df_polygons},
        {NULL, NULL}
        };
        luaL_newlib(L, l);

        return 1;
}
