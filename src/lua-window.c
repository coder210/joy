#include "luaclib.h"
#include "SDL3/SDL.h"
#include "external/lua/lapi.h"
#include "external/lua/lauxlib.h"


static int l_window_create(lua_State* L)
{
        const char* title;
        int w, h, flags;
        SDL_Window* window;
        title = luaL_checkstring(L, 1);
        w = (int)luaL_checkinteger(L, 2);
        h = (int)luaL_checkinteger(L, 3);
        flags = (int)luaL_checkinteger(L, 4);
        window = SDL_CreateWindow(title, w, h, flags);
        lua_pushlightuserdata(L, window);
        return 1;
}

static int l_window_destroy(lua_State* L)
{
        SDL_Window* window;
        SDL_assert(lua_islightuserdata(L, 1));
        window = (SDL_Window*)lua_touserdata(L, 1);
        SDL_DestroyWindow(window);
        return 0;
}

static int l_window_get_size(lua_State* L)
{
        bool ok;
        int w, h;
        SDL_Window* window;
        window = (SDL_Window*)lua_touserdata(L, 1);
        ok = SDL_GetWindowSize(window, &w, &h);
        lua_pushboolean(L, ok);
        lua_pushinteger(L, w);
        lua_pushinteger(L, h);
        return 3;
}

static int l_window_set_icon(lua_State* L)
{
        SDL_Window* window;
        const char* filepath;
        SDL_Surface* icon;
        Uint32 key;
        SDL_assert(lua_islightuserdata(L, 1));
        window = (SDL_Window*)lua_touserdata(L, 1);
        filepath = luaL_checkstring(L, 2);
        icon = SDL_LoadBMP(filepath);
        if (icon) {
                key = SDL_MapSurfaceRGB(icon, 255, 255, 255);
                SDL_SetSurfaceColorKey(icon, true, key);
                SDL_SetWindowIcon(window, icon);
                SDL_DestroySurface(icon);
                lua_pushboolean(L, true);
        }
        else {
                lua_pushboolean(L, false);
        }
        return 1;
}

static int l_window_set_fullscreen(lua_State* L)
{
        SDL_Window* window;
        SDL_assert(lua_islightuserdata(L, 1));
        window = (SDL_Window*)lua_touserdata(L, 1);
        bool r = SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        lua_pushboolean(L, r);
        return 1;
}

static int l_window_create_with_renderer(lua_State* L)
{
        const char* title;
        int w, h;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_WindowFlags flags;
        bool r;
        title = luaL_checkstring(L, 1);
        w = (int)luaL_checkinteger(L, 2);
        h = (int)luaL_checkinteger(L, 3);
        if (lua_isnil(L, 4)) {
                flags = SDL_WINDOW_RESIZABLE;
        }
        else {
                flags = (int)luaL_checkinteger(L, 4);
        }
        r = SDL_CreateWindowAndRenderer(title, w, h, flags, &window, &renderer);
        lua_pushboolean(L, r);
        lua_pushlightuserdata(L, window);
        lua_pushlightuserdata(L, renderer);
        return 3;
}

static int l_window_should_close(lua_State* L)
{
        SDL_Event* event;
        event = (SDL_Event*)lua_touserdata(L, 1);
        lua_pushboolean(L, event->type == SDL_EVENT_QUIT);
        return 1;
}

int luaopen_window(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                {"create", l_window_create},
                {"destroy", l_window_destroy},
                {"get_size", l_window_get_size},
                {"set_icon", l_window_set_icon},
                {"set_fullscreen", l_window_set_fullscreen},
                {"create_with_renderer", l_window_create_with_renderer},
                {"should_close", l_window_should_close},
                {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
