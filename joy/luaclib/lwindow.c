#include "luaclib.h"
#include "../jcore.h"
#include "../external/lua/lauxlib.h"
#include <SDL3/SDL.h>


static int l_create(lua_State* L) {
	const char* title = luaL_checkstring(L, 1);
	int w = (int)luaL_checkinteger(L, 2);
	int h = (int)luaL_checkinteger(L, 3);
	jwindow_p win = create_window(title, w, h);
	lua_pushlightuserdata(L, win);
	return 1;
}

static int l_destroy(lua_State* L) {
	jwindow_p win = (jwindow_p)lua_touserdata(L, 1);
	if (win) { destroy_window(win); }
	return 0;
}

static int l_get_size(lua_State* L) {
	jwindow_p win = (jwindow_p)lua_touserdata(L, 1);
	SDL_Window* raw = (SDL_Window*)get_raw_window(win);
	int w, h;
	lua_pushboolean(L, SDL_GetWindowSize(raw, &w, &h));
	lua_pushinteger(L, w);
	lua_pushinteger(L, h);
	return 3;
}

static int l_set_icon(lua_State* L) {
	jwindow_p win = (jwindow_p)lua_touserdata(L, 1);
	SDL_Window* raw = (SDL_Window*)get_raw_window(win);
	const char* filepath = luaL_checkstring(L, 2);
	SDL_Surface* icon = SDL_LoadBMP(filepath);
	if (icon) {
		Uint32 key = SDL_MapSurfaceRGB(icon, 255, 255, 255);
		SDL_SetSurfaceColorKey(icon, true, key);
		SDL_SetWindowIcon(raw, icon);
		SDL_DestroySurface(icon);
		lua_pushboolean(L, true);
	}
	else {
		lua_pushboolean(L, false);
	}
	return 1;
}

static int l_set_fullscreen(lua_State* L) {
	jwindow_p win = (jwindow_p)lua_touserdata(L, 1);
	SDL_Window* raw = (SDL_Window*)get_raw_window(win);
	lua_pushboolean(L, SDL_SetWindowFullscreen(raw, SDL_WINDOW_FULLSCREEN));
	return 1;
}

static int l_should_close(lua_State* L) {
	SDL_Event* event = (SDL_Event*)lua_touserdata(L, 1);
	lua_pushboolean(L, event->type == SDL_EVENT_QUIT);
	return 1;
}

int luaopen_window(lua_State* L) {
	luaL_checkversion(L);
	luaL_Reg l[] = {
	    {"create",        l_create},
	    {"destroy",       l_destroy},
	    {"get_size",      l_get_size},
	    {"set_icon",      l_set_icon},
	    {"set_fullscreen", l_set_fullscreen},
	    {"should_close",  l_should_close},
	    {NULL, NULL}
	};
	luaL_newlib(L, l);
	return 1;
}
