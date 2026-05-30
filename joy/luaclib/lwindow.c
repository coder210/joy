#include "luaclib.h"
#include "SDL3/SDL.h"
#include "../external/lua/lauxlib.h"

#define WINDOW_MT  "SDL_Window*"
#define CUR_WIN_KEY "joy.window.cur_win"

static SDL_Window* cur_win(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, CUR_WIN_KEY);
    SDL_Window** win = (SDL_Window**)lua_touserdata(L, -1);
    if (!win || !*win) {
        lua_pop(L, 1);
        luaL_error(L, "no current window");
        return NULL;
    }
    lua_pop(L, 1);
    return *win;
}

static int l_gc(lua_State* L) {
    SDL_Window** win = (SDL_Window**)lua_touserdata(L, 1);
    if (win && *win) { SDL_DestroyWindow(*win); *win = NULL; }
    return 0;
}

static int l_create(lua_State* L) {
    const char* title = luaL_checkstring(L, 1);
    int w = (int)luaL_checkinteger(L, 2);
    int h = (int)luaL_checkinteger(L, 3);
    int flags = (int)luaL_checkinteger(L, 4);
    SDL_Window** win = (SDL_Window**)lua_newuserdata(L, sizeof(SDL_Window*));
    *win = SDL_CreateWindow(title, w, h, flags);
    luaL_getmetatable(L, WINDOW_MT);
    lua_setmetatable(L, -2);
    lua_pushvalue(L, -1);
    lua_setfield(L, LUA_REGISTRYINDEX, CUR_WIN_KEY);
    return 1;
}

static int l_select(lua_State* L) {
    SDL_Window** win = (SDL_Window**)luaL_checkudata(L, 1, WINDOW_MT);
    lua_pushvalue(L, 1);
    lua_setfield(L, LUA_REGISTRYINDEX, CUR_WIN_KEY);
    return 0;
}

static int l_destroy(lua_State* L) {
    SDL_Window** win = (SDL_Window**)lua_touserdata(L, 1);
    if (!win || !*win) { win = (SDL_Window**)lua_touserdata(L, -1); }
    // 不传参数则销毁当前窗口
    SDL_Window** target = (SDL_Window**)luaL_checkudata(L, 1, WINDOW_MT);
    if (target && *target) { SDL_DestroyWindow(*target); *target = NULL; }
    return 0;
}

/* 以下函数不再需要传 win，使用当前窗口 */
static int l_get_size(lua_State* L) {
    SDL_Window* win = cur_win(L);
    int w, h;
    lua_pushboolean(L, SDL_GetWindowSize(win, &w, &h));
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 3;
}

static int l_set_icon(lua_State* L) {
    SDL_Window* win = cur_win(L);
    const char* filepath = luaL_checkstring(L, 1);
    SDL_Surface* icon = SDL_LoadBMP(filepath);
    if (icon) {
        Uint32 key = SDL_MapSurfaceRGB(icon, 255, 255, 255);
        SDL_SetSurfaceColorKey(icon, true, key);
        SDL_SetWindowIcon(win, icon);
        SDL_DestroySurface(icon);
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }
    return 1;
}

static int l_set_fullscreen(lua_State* L) {
    lua_pushboolean(L, SDL_SetWindowFullscreen(cur_win(L), SDL_WINDOW_FULLSCREEN));
    return 1;
}

static int l_should_close(lua_State* L) {
    SDL_Event* event = (SDL_Event*)lua_touserdata(L, 1);
    lua_pushboolean(L, event->type == SDL_EVENT_QUIT);
    return 1;
}

int luaopen_window(lua_State* L) {
    luaL_newmetatable(L, WINDOW_MT);
    lua_pushcfunction(L, l_gc);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);

    luaL_checkversion(L);
    luaL_Reg l[] = {
        {"create",        l_create},
        {"select",        l_select},
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
