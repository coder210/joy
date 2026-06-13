#include "luaclib.h"
#include "SDL3/SDL.h"
#include "../external/lua/lauxlib.h"


static int l_create(lua_State* L) {
        const char* title = luaL_checkstring(L, 1);
        int w = (int)luaL_checkinteger(L, 2);
        int h = (int)luaL_checkinteger(L, 3);
        int flags = (int)luaL_checkinteger(L, 4);
        SDL_Window* win = SDL_CreateWindow(title, w, h, flags);
        lua_pushlightuserdata(L, win);
        return 1;
}

static int l_destroy(lua_State* L) {
        SDL_Window* win = lua_touserdata(L, 1);
        if (win) { SDL_DestroyWindow(win); }
        return 0;
}

static int l_get_size(lua_State* L) {
        SDL_Window* win = lua_touserdata(L, 1);
        int w, h;
        lua_pushboolean(L, SDL_GetWindowSize(win, &w, &h));
        lua_pushinteger(L, w);
        lua_pushinteger(L, h);
        return 3;
}

static int l_set_icon(lua_State* L) {
        SDL_Window* win = lua_touserdata(L, 1);
        const char* filepath = luaL_checkstring(L, 2);
        SDL_Surface* icon = SDL_LoadBMP(filepath);
        if (icon) {
                Uint32 key = SDL_MapSurfaceRGB(icon, 255, 255, 255);
                SDL_SetSurfaceColorKey(icon, true, key);
                SDL_SetWindowIcon(win, icon);
                SDL_DestroySurface(icon);
                lua_pushboolean(L, true);
        }
        else {
                lua_pushboolean(L, false);
        }
        return 1;
}

static int l_set_fullscreen(lua_State* L) {
        SDL_Window* win = lua_touserdata(L, 1);
        lua_pushboolean(L, SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN));
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
