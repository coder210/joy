#include "luaclib.h"
#include "SDL3/SDL.h"
#include "../external/lua/lauxlib.h"

#define WINDOW_MT  "SDL_Window*"

static SDL_Window* check_win(lua_State* L, int idx) {
    SDL_Window** win = (SDL_Window**)luaL_checkudata(L, idx, WINDOW_MT);
    if (!win || !*win) luaL_error(L, "invalid window");
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
    return 1;
}

static int l_select(lua_State* L) {
    check_win(L, 1);  /* 验证窗口有效 */
    return 0;
}

static int l_destroy(lua_State* L) {
    SDL_Window** win = (SDL_Window**)luaL_checkudata(L, 1, WINDOW_MT);
    if (win && *win) { SDL_DestroyWindow(*win); *win = NULL; }
    return 0;
}

static int l_get_size(lua_State* L) {
    SDL_Window* win = check_win(L, 1);
    int w, h;
    lua_pushboolean(L, SDL_GetWindowSize(win, &w, &h));
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 3;
}

static int l_set_icon(lua_State* L) {
    SDL_Window* win = check_win(L, 1);
    const char* filepath = luaL_checkstring(L, 2);
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
    lua_pushboolean(L, SDL_SetWindowFullscreen(check_win(L, 1), SDL_WINDOW_FULLSCREEN));
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
