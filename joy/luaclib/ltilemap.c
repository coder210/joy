#include "luaclib.h"
#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../jtilemap.h"
#include "../jtilemap_render.h"

#define TILEMAP_MT "jtilemap_t*"

static int l_tilemap_load_file(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);
    jtilemap_p tm = jtilemap_load_file(filename);
    if (!tm) { lua_pushnil(L); return 1; }
    jtilemap_p* p = (jtilemap_p*)lua_newuserdata(L, sizeof(jtilemap_p));
    *p = tm;
    luaL_getmetatable(L, TILEMAP_MT);
    lua_setmetatable(L, -2);
    return 1;
}

static int l_tilemap_destroy(lua_State* L)
{
    jtilemap_p* p = (jtilemap_p*)luaL_checkudata(L, 1, TILEMAP_MT);
    if (p && *p) { jtilemap_destroy(*p); *p = NULL; }
    return 0;
}

static int l_tilemap_gc(lua_State* L)
{
    jtilemap_p* p = (jtilemap_p*)luaL_checkudata(L, 1, TILEMAP_MT);
    if (p && *p) jtilemap_destroy(*p);
    return 0;
}

static int l_tilemap_get_width(lua_State* L)
{
    jtilemap_p* p = (jtilemap_p*)luaL_checkudata(L, 1, TILEMAP_MT);
    lua_pushinteger(L, p && *p ? (*p)->width : 0);
    return 1;
}

static int l_tilemap_get_height(lua_State* L)
{
    jtilemap_p* p = (jtilemap_p*)luaL_checkudata(L, 1, TILEMAP_MT);
    lua_pushinteger(L, p && *p ? (*p)->height : 0);
    return 1;
}

static int l_tilemap_get_tilewidth(lua_State* L)
{
    jtilemap_p* p = (jtilemap_p*)luaL_checkudata(L, 1, TILEMAP_MT);
    lua_pushinteger(L, p && *p ? (*p)->tilewidth : 0);
    return 1;
}

static int l_tilemap_get_tileheight(lua_State* L)
{
    jtilemap_p* p = (jtilemap_p*)luaL_checkudata(L, 1, TILEMAP_MT);
    lua_pushinteger(L, p && *p ? (*p)->tileheight : 0);
    return 1;
}

/* ---- rendering ---- */
static int l_tilemap_render_init(lua_State* L)
{
    SDL_Renderer** rp = (SDL_Renderer**)lua_touserdata(L, 1);
    if (!rp || !*rp) { lua_pushboolean(L, 0); return 1; }
    jtilemap_render_init(*rp, luaL_optstring(L, 2, NULL), NULL);
    lua_pushboolean(L, 1);
    return 1;
}

static int l_tilemap_render_init_base(lua_State* L)
{
    SDL_Renderer** rp = (SDL_Renderer**)lua_touserdata(L, 1);
    if (!rp || !*rp) { lua_pushboolean(L, 0); return 1; }
    jtilemap_render_init(*rp, luaL_checkstring(L, 2), NULL);
    lua_pushboolean(L, 1);
    return 1;
}

static int l_tilemap_render_all(lua_State* L)
{
    jtilemap_p* p = (jtilemap_p*)luaL_checkudata(L, 1, TILEMAP_MT);
    if (!p || !*p) return 0;
    float cam_x = (float)luaL_checknumber(L, 2);
    float cam_y = (float)luaL_checknumber(L, 3);
    uint32_t time_ms = (uint32_t)luaL_optinteger(L, 4, 0);
    jtilemap_render_all(*p, cam_x, cam_y, time_ms);
    return 0;
}

static int l_tilemap_render_destroy(lua_State* L)
{
    jtilemap_render_destroy();
    return 0;
}

int luaopen_tilemap(lua_State* L)
{
    luaL_checkversion(L);

    luaL_newmetatable(L, TILEMAP_MT);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_tilemap_gc); lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);

    luaL_Reg lib[] = {
        {"load_file",     l_tilemap_load_file},
        {"destroy",       l_tilemap_destroy},
        {"get_width",     l_tilemap_get_width},
        {"get_height",    l_tilemap_get_height},
        {"get_tilewidth", l_tilemap_get_tilewidth},
        {"get_tileheight",l_tilemap_get_tileheight},
        {"render_init",      l_tilemap_render_init},
        {"render_init_base", l_tilemap_render_init_base},
        {"render_all",       l_tilemap_render_all},
        {"render_destroy",   l_tilemap_render_destroy},
        {NULL, NULL}
    };
    luaL_newlib(L, lib);
    return 1;
}
