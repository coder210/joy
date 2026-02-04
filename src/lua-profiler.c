#include <stdbool.h>
#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"
#include "liv-profiler.h"
#include "luaclib.h"

static int l_simple_fps_create(lua_State* L)
{
	simple_fps_counter_p fps;
	fps = simple_fps_create();
	lua_pushlightuserdata(L, fps);
	return 1;
}

static int l_simple_fps_destory(lua_State* L)
{
	simple_fps_counter_p fps;
	fps = (simple_fps_counter_p)lua_touserdata(L, 1);
	simple_fps_destory(fps);
	return 0;
}

static int l_simple_fps_update(lua_State* L)
{
	int counter;
	simple_fps_counter_p fps;
	fps = (simple_fps_counter_p)lua_touserdata(L, 1);
	counter = simple_fps_update(fps);
	lua_pushinteger(L, counter);
	return 1;
}

int luaopen_profiler(lua_State* L)
{
	luaL_checkversion(L);
	lua_newtable(L);
	luaL_Reg simple_fps_lib[] = {
		{"create", l_simple_fps_create},
		{"destory", l_simple_fps_destory},
		{"update", l_simple_fps_update},
		{NULL, NULL}
	};
	luaL_newlib(L, simple_fps_lib);
	lua_setfield(L, -2, "simple_fps");
	return 1;
}