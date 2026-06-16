#include "luaclib.h"
#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../jtilemap.h"
#include "../jtilemap_render.h"

static int l_tilemap_load_file(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);
    jtilemap_p tm = jtilemap_load_file(filename);
    if (!tm) { lua_pushnil(L); return 1; }
    lua_pushlightuserdata(L, tm);
    return 1;
}

static int l_tilemap_destroy(lua_State* L)
{
    jtilemap_p p = (jtilemap_p)lua_touserdata(L, 1);
    if (p) jtilemap_destroy(p);
    return 0;
}

static int l_tilemap_get_width(lua_State* L)
{
    jtilemap_p p = (jtilemap_p)lua_touserdata(L, 1);
    lua_pushinteger(L, p ? p->width : 0);
    return 1;
}

static int l_tilemap_get_height(lua_State* L)
{
    jtilemap_p p = (jtilemap_p)lua_touserdata(L, 1);
    lua_pushinteger(L, p ? p->height : 0);
    return 1;
}

static int l_tilemap_get_tilewidth(lua_State* L)
{
    jtilemap_p p = (jtilemap_p)lua_touserdata(L, 1);
    lua_pushinteger(L, p ? p->tilewidth : 0);
    return 1;
}

static int l_tilemap_get_tileheight(lua_State* L)
{
    jtilemap_p p = (jtilemap_p)lua_touserdata(L, 1);
    lua_pushinteger(L, p ? p->tileheight : 0);
    return 1;
}

/* ---- rendering (可重入实例) ---- */
/* Lua 侧保存 render 实例指针，用法：
 *   local trender = joy.tilemap.render_create(ren, base_path)
 *   joy.tilemap.render_all(trender, tm, cx, cy, ms)
 *   joy.tilemap.render_destroy(trender)
 */
static int l_tilemap_render_create(lua_State* L)
{
    jr2d_p ren = (jr2d_p)lua_touserdata(L, 1);
    if (!ren) { lua_pushnil(L); return 1; }
    jtilemap_render_p r = jtilemap_render_create(ren, luaL_optstring(L, 2, NULL), NULL);
    lua_pushlightuserdata(L, r);
    return 1;
}

static int l_tilemap_render_create_base(lua_State* L)
{
    jr2d_p ren = (jr2d_p)lua_touserdata(L, 1);
    if (!ren) { lua_pushnil(L); return 1; }
    jtilemap_render_p r = jtilemap_render_create(ren, luaL_checkstring(L, 2), NULL);
    lua_pushlightuserdata(L, r);
    return 1;
}

static int l_tilemap_render_all(lua_State* L)
{
    jtilemap_render_p r = (jtilemap_render_p)lua_touserdata(L, 1);
    jtilemap_p tm = (jtilemap_p)lua_touserdata(L, 2);
    if (!r || !tm) return 0;
    float cam_x = (float)luaL_checknumber(L, 3);
    float cam_y = (float)luaL_checknumber(L, 4);
    uint32_t time_ms = (uint32_t)luaL_optinteger(L, 5, 0);
    jtilemap_render_all(r, tm, cam_x, cam_y, time_ms);
    return 0;
}

static int l_tilemap_render_layer(lua_State* L)
{
    jtilemap_render_p r = (jtilemap_render_p)lua_touserdata(L, 1);
    jtilemap_p tm = (jtilemap_p)lua_touserdata(L, 2);
    if (!r || !tm) return 0;
    const char* name = luaL_checkstring(L, 3);
    float cam_x = (float)luaL_checknumber(L, 4);
    float cam_y = (float)luaL_checknumber(L, 5);
    uint32_t time_ms = (uint32_t)luaL_optinteger(L, 6, 0);
    lua_pushboolean(L, jtilemap_render_layer(r, tm, name, cam_x, cam_y, time_ms));
    return 1;
}

static int l_tilemap_render_destroy(lua_State* L)
{
    jtilemap_render_p r = (jtilemap_render_p)lua_touserdata(L, 1);
    if (r) jtilemap_render_destroy(r);
    return 0;
}

int luaopen_tilemap(lua_State* L)
{
    luaL_checkversion(L);
    luaL_Reg lib[] = {
        {"load_file",        l_tilemap_load_file},
        {"destroy",          l_tilemap_destroy},
        {"get_width",        l_tilemap_get_width},
        {"get_height",       l_tilemap_get_height},
        {"get_tilewidth",    l_tilemap_get_tilewidth},
        {"get_tileheight",   l_tilemap_get_tileheight},
        {"render_create",        l_tilemap_render_create},
        {"render_create_base",   l_tilemap_render_create_base},
        {"render_all",           l_tilemap_render_all},
        {"render_layer",         l_tilemap_render_layer},
        {"render_destroy",       l_tilemap_render_destroy},
        {NULL, NULL}
    };
    luaL_newlib(L, lib);
    return 1;
}
