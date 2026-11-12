/************************************************
Copyright: 2021-2022, lanchong.xyz/Ltd.
File name: luax.c
Description:
Author: ydlc
Version: 1.0
Date: 2021.12.14
History:
*************************************************/
#include <SDL3/SDL.h>
#include "lua/lapi.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
#include "lua_cjson/lua_cjson.h"
#include <joy/joy.h>
#include "joy_lua.h"

/* set_env / get_env 前向声明 */
static int l_set_env(lua_State* L);
static int l_get_env(lua_State* L);

struct joy_luax {
        lua_State* L;
        void* userdata;
        char* watch_file;
        SDL_Time last_mtime;
        lua_State* env;
        joy_event_callbacks_t cbs;   /* C 层事件回调集合 */
};


/* 查表并调用 joy.func(args...)，调用者已把 arg 推入栈 */
static bool call_joy_n(lua_State* L, const char* func, int nargs)
{
        lua_getglobal(L, "joy");
        if (lua_isnil(L, -1)) { lua_pop(L, 1); return false; }
        lua_getfield(L, -1, func);     /* [args..., joy, func] */
        if (lua_isnil(L, -1)) {
                lua_pop(L, 2);  /* pop nil + joy */
                /* 清除之前推入的 arg */
                lua_pop(L, nargs);
                return false;
        }

        /* 把 func 移到栈底，再弹出 joy，变成 [func, args...] */
        lua_rotate(L, 1, 1);           /* [func, args..., joy] */
        lua_pop(L, 1);                  /* [func, args...] */

        if (lua_pcall(L, nargs, 0, 0) != LUA_OK) {
                joy_core_log_error("joy.%s: %s", func, lua_tostring(L, -1));
                lua_pop(L, 1);
                return false;
        }
        return true;
}


static void default_mousepressed(float x, float y, int b, void* arg)
{
        luax_p luax = (luax_p)arg;
        lua_State* L = luax->L;
        lua_pushnumber(L, x); lua_pushnumber(L, y); lua_pushinteger(L, b);
        call_joy_n(L, "mousepressed", 3);
}
static void default_mousereleased(float x, float y, int b, void* arg)
{
        luax_p luax = (luax_p)arg;
        lua_State* L = luax->L;
        lua_pushnumber(L, x); lua_pushnumber(L, y); lua_pushinteger(L, b);
        call_joy_n(L, "mousereleased", 3);
}
static void default_mousemoved(float x, float y, float dx, float dy, void* arg)
{
        luax_p luax = (luax_p)arg;
        lua_State* L = luax->L;
        lua_pushnumber(L, x); lua_pushnumber(L, y); lua_pushnumber(L, dx); lua_pushnumber(L, dy);
        call_joy_n(L, "mousemoved", 4);
}
static void default_keypressed(const char* key, int sc, bool rep, bool ctrl, bool shift, bool alt, void* arg)
{
        luax_p luax = (luax_p)arg;
        lua_State* L = luax->L;
        lua_pushstring(L, key); lua_pushinteger(L, sc);
        lua_pushboolean(L, rep); lua_pushboolean(L, ctrl);
        lua_pushboolean(L, shift); lua_pushboolean(L, alt);
        call_joy_n(L, "keypressed", 6);
}
static void default_keyreleased(const char* key, int sc, void* arg)
{
        luax_p luax = (luax_p)arg;
        lua_State* L = luax->L;
        lua_pushstring(L, key); lua_pushinteger(L, sc);
        call_joy_n(L, "keyreleased", 2);
}
static void default_textinput(const char* text, void* arg)
{
        luax_p luax = (luax_p)arg;
        lua_State* L = luax->L;
        lua_pushstring(L, text);
        call_joy_n(L, "textinput", 1);
}
static void default_resize(int w, int h, void* arg)
{
        luax_p luax = (luax_p)arg;
        lua_State* L = luax->L;
        lua_pushinteger(L, w); lua_pushinteger(L, h);
        call_joy_n(L, "resize", 2);
}
static void default_process_event(const void* event, void* arg)
{
        luax_p luax = (luax_p)arg;
        lua_State* L = luax->L;
        lua_pushlightuserdata(L, (void*)event);
        call_joy_n(L, "process_event", 1);
}

luax_p luax_create(void)
{
        luax_p luax;
        luax = (luax_p)SDL_calloc(1, sizeof(luax_t));
        assert(luax);
        luax->L = luaL_newstate();
        assert(luax->L);
        luaL_openlibs(luax->L);
        lua_pushlightuserdata(luax->L, luax);
        lua_setfield(luax->L, LUA_REGISTRYINDEX, "__this");

        /* 创建独立的 env lua_State （热更新不重置）*/
        luax->env = luaL_newstate();
        assert(luax->env);
        luaL_openlibs(luax->env);  /* 提供基础库 */

        /* 设置默认回调，自动转发到 Lua */
        luax->cbs.on_mousepressed   = default_mousepressed;
        luax->cbs.on_mousereleased  = default_mousereleased;
        luax->cbs.on_mousemoved     = default_mousemoved;
        luax->cbs.on_keypressed     = default_keypressed;
        luax->cbs.on_keyreleased    = default_keyreleased;
        luax->cbs.on_textinput      = default_textinput;
        luax->cbs.on_resize         = default_resize;
        luax->cbs.on_process_event  = default_process_event;

        /* 创建全局 joy 表：旧手写模块表（joy.window / joy.r2d / joy.keyboard …）
         * 已随 modules/lua/bindings/legacy 一起移除，这里只留空表供脚本挂自己的
         * init / update / draw 回调与事件处理函数。 */
        lua_newtable(luax->L);
        lua_setglobal(luax->L, "joy");

        /* IDL 驱动的新绑定层：全局 Joy.Api.Bindings.*（与 cs / js 同名） */
        luax_open_api(luax->L);

        /* 注册 set_env / get_env 为全局函数 */
        lua_pushcfunction(luax->L, l_set_env);
        lua_setglobal(luax->L, "set_env");
        lua_pushcfunction(luax->L, l_get_env);
        lua_setglobal(luax->L, "get_env");
        return luax;
}

/* ===== set_env / get_env (跨 lua_State 持久化) ===== */
static int l_set_env(lua_State* L)
{
        lua_getfield(L, LUA_REGISTRYINDEX, "__this");
        luax_p luax = (luax_p)lua_touserdata(L, -1);
        lua_pop(L, 1);
        if (!luax || !luax->env) return 0;

        lua_State* E = luax->env;
        /* 用 lua_xmove 把 key, value 从主 L 复制到 env L */
        lua_pushvalue(L, 1);                /* key on main L */
        lua_pushvalue(L, 2);                /* value on main L */
        lua_xmove(L, E, 2);                 /* move 2 values: key, value → env stack */
        lua_settable(E, LUA_REGISTRYINDEX);  /* registry[key] = value on env L */
        return 0;
}

static int l_get_env(lua_State* L)
{
        lua_getfield(L, LUA_REGISTRYINDEX, "__this");
        luax_p luax = (luax_p)lua_touserdata(L, -1);
        lua_pop(L, 1);
        if (!luax || !luax->env) return 0;

        lua_State* E = luax->env;
        lua_pushvalue(L, 1);                /* key on main L */
        lua_xmove(L, E, 1);                 /* move key → env stack */
        lua_pushvalue(E, -1);               /* dup key for gettable */
        lua_gettable(E, LUA_REGISTRYINDEX);  /* registry[key] → value on env stack */
        lua_xmove(E, L, 1);                 /* move value back → main stack */
        lua_pop(E, 1);                      /* pop key from env */
        return 1;
}

bool luax_dofile(luax_p luax, const char* filename)
{
        char chunkname[JOY_MAX_PATH] = { 0 };
        strcat(chunkname, "@");
        strcat(chunkname, filename);
        size_t data_sz;
        char* data = joy_core_utils_read_file(filename, &data_sz);
        if (data) {
                if (luaL_loadbuffer(luax->L, data, data_sz, chunkname) != LUA_OK) {
                        const char* error_msg = lua_tostring(luax->L, -1);
                        joy_core_log_info("%s", error_msg);
                        return false;
                }
                if (lua_pcall(luax->L, 0, LUA_MULTRET, 0) != LUA_OK) {
                        const char* error_msg = lua_tostring(luax->L, -1);
                        joy_core_log_info("%s", error_msg);
                        SDL_free(data);
                        return false;
                }
                SDL_free(data);
                return true;
        }
        return false;
}

void luax_release(luax_p luax)
{
        if (luax->watch_file) SDL_free(luax->watch_file);
        lua_close(luax->L);
        free(luax);
}

void* luax_get_state(luax_p luax)
{
        return luax->L;
}

void luax_dispatch_event(luax_p luax, const void* event)
{
	joy_core_dispatch_event(luax->cbs, event, luax);
}

void luax_set_args(luax_p luax, int argc, char* argv[])
{
	lua_State* L = luax->L;
	lua_newtable(L);
	for (int i = 0; i < argc; i++) {
		lua_pushstring(L, argv[i]);
		lua_rawseti(L, -2, i);
	}
	lua_setglobal(L, "arg");
}

bool luax_call_joy(luax_p luax, const char* func, int argc, float dt)
{
        lua_State* L = luax->L;
        lua_getglobal(L, "joy");
        if (lua_isnil(L, -1)) { lua_pop(L, 1); return false; }
        lua_getfield(L, -1, func);
        if (lua_isnil(L, -1)) { lua_pop(L, 2); return false; }

        if (argc > 0)
                lua_pushnumber(L, dt);

        int ret = lua_pcall(L, argc, 0, 0);
        if (ret != LUA_OK) {
                joy_core_log_error("joy.%s error: %s", func, lua_tostring(L, -1));
                lua_pop(L, 1);
                lua_pop(L, 1);
                return false;
        }
        lua_pop(L, 1);
        return true;
}

bool luax_reload(luax_p luax, const char* filename)
{
        return luax_dofile(luax, filename);
}

/* ===== 文件监控 ===== */

void luax_set_watch(luax_p luax, const char* filename)
{
        if (luax->watch_file) SDL_free(luax->watch_file);
        luax->watch_file = filename ? SDL_strdup(filename) : NULL;
        if (filename) {
                SDL_PathInfo info;
                if (SDL_GetPathInfo(filename, &info))
                        luax->last_mtime = info.modify_time;
        }
}

bool luax_poll_reload(luax_p luax)
{
        if (!luax->watch_file) return false;
        SDL_PathInfo info;
        if (!SDL_GetPathInfo(luax->watch_file, &info))
                return false;
        if (info.modify_time == luax->last_mtime)
                return false;
        luax->last_mtime = info.modify_time;
        joy_core_log_info("[CHANGED] %s", luax->watch_file);
        if (!luax_reload(luax, luax->watch_file))
                return false;
        luax_call_joy(luax, "hotfix", 0, 0);
        return true;
}

const char* luax_get_watch(luax_p luax)
{
        return luax->watch_file;
}

