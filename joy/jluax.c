/************************************************
Copyright: 2021-2022, lanchong.xyz/Ltd.
File name: luax.c
Description:
Author: ydlc
Version: 1.0
Date: 2021.12.14
History:
*************************************************/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <SDL3/SDL.h>
#include "external/lua/lapi.h"
#include "external/lua/lualib.h"
#include "external/lua/lauxlib.h"
#include "external/lua_cjson/lua_cjson.h"
#include "jcore.h"
#include "jevent.h"
#include "jutils.h"
#include "luaclib/luaclib.h"
#include "jluax.h"

struct luax {
        lua_State* L;
        void* userdata;
        char* watch_file;
        SDL_Time last_mtime;

        event_callbacks_t cbs;   /* C 层事件回调集合 */
};


//static int l_core_traceback(lua_State* L)
//{
//        const char* msg = lua_tostring(L, 1);
//        if (msg) {
//                luaL_traceback(L, L, msg, 1);
//                lua_remove(L, 1);
//                const char* trace = lua_tostring(L, -1);
//                log_error("c:%s", trace);
//        }
//        else {
//                lua_pushliteral(L, "(no error message)");
//        }
//        return 1;
//}
//
//static int l_core_quit(lua_State* L)
//{
//        luax_t* luax;
//        luax = (luax_t*)lua_touserdata(L, 1);
//        joy_quit(luax->app);
//        return 0;
//}
//
//static int l_core_log(lua_State* L)
//{
//        luax_t* luax;
//        const char* str;
//        luax = lua_touserdata(L, 1);
//        str = luaL_checkstring(L, 2);
//        log_info("luax:%s", str);
//        return 1;
//}
//
//static int l_core_debug(lua_State* L)
//{
//        luax_t* luax;
//        const char* str;
//        luax = lua_touserdata(L, 1);
//        str = luaL_checkstring(L, 2);
//        log_debug("luax:%s", str);
//        return 1;
//}
//
//static int l_core_error(lua_State* L)
//{
//        luax_t* luax;
//        const char* str;
//        luax = lua_touserdata(L, 1);
//        str = luaL_checkstring(L, 2);
//        log_error("luax:%s", str);
//        return 1;
//}
//
//static int l_core_get_env(lua_State* L)
//{
//        luax_t* luax;
//        const char* name, * value;
//        luax = (luax_t*)lua_touserdata(L, 1);
//        name = luaL_checkstring(L, 2);
//        value = joy_getenv(luax->app, name);
//        lua_pushstring(L, value);
//        return 1;
//}
//
//static int l_core_set_env(lua_State* L)
//{
//        luax_t* luax;
//        const char* name, * value;
//        luax = (luax_t*)lua_touserdata(L, 1);
//        name = luaL_checkstring(L, 2);
//        value = luaL_checkstring(L, 3);
//        joy_setenv(luax->app, name, value);
//        return 0;
//}
//
//static int luaopen_core(lua_State* L)
//{
//        luaL_checkversion(L);
//        luaL_Reg l[] = {
//        {"quit", l_core_quit},
//        {"log", l_core_log},
//        {"debug", l_core_debug},
//        {"error", l_core_error},
//        {"get_env", l_core_get_env},
//        {"set_env", l_core_set_env},
//        {NULL, NULL},
//        };
//        luaL_newlib(L, l);
//        return 1;
//}

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
                log_error("joy.%s: %s", func, lua_tostring(L, -1));
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

        /* 设置默认回调，自动转发到 Lua */
        luax->cbs.on_mousepressed   = default_mousepressed;
        luax->cbs.on_mousereleased  = default_mousereleased;
        luax->cbs.on_mousemoved     = default_mousemoved;
        luax->cbs.on_keypressed     = default_keypressed;
        luax->cbs.on_keyreleased    = default_keyreleased;
        luax->cbs.on_textinput      = default_textinput;
        luax->cbs.on_resize         = default_resize;

        // 创建 joy 表，所有模块注册在 joy.* 下
        lua_newtable(luax->L);

#define JOY_MOD(name, open) \
        luaL_requiref(luax->L, name, open, 0); \
        lua_setfield(luax->L, -2, name)

        JOY_MOD("window", luaopen_window);
        JOY_MOD("graphics", luaopen_graphics);
        JOY_MOD("sdl", luaopen_sdl);
        JOY_MOD("audio", luaopen_audio);
        JOY_MOD("keyboard", luaopen_keyboard);
        JOY_MOD("net", luaopen_net);
        JOY_MOD("utils", luaopen_utils);
        JOY_MOD("fp", luaopen_fp);
        JOY_MOD("ft", luaopen_ft);
        JOY_MOD("vec2f", luaopen_vec2f);
        JOY_MOD("vec2", luaopen_vec2);
        JOY_MOD("vec3f", luaopen_vec3f);
        JOY_MOD("vec3", luaopen_vec3);
        JOY_MOD("collision2df", luaopen_collision2df);
        JOY_MOD("collision2d", luaopen_collision2d);
        JOY_MOD("collision3d", luaopen_collision3d);
        JOY_MOD("cjson", luaopen_cjson_safe);
        JOY_MOD("packagex", luaopen_packagex);
        JOY_MOD("ui", luaopen_ui);
        JOY_MOD("c2s", luaopen_c2s);
        JOY_MOD("s2c", luaopen_s2c);
        JOY_MOD("ecs", luaopen_ecs);
        JOY_MOD("mat22f", luaopen_mat22f);
        JOY_MOD("mat22", luaopen_mat22);
        JOY_MOD("world2df", luaopen_world2df);
        JOY_MOD("server", luaopen_server);
        JOY_MOD("client", luaopen_client);
        JOY_MOD("timer", luaopen_timer);
        JOY_MOD("animation", luaopen_animation);
        JOY_MOD("joystick", luaopen_joystick);
        JOY_MOD("event", luaopen_event);
        JOY_MOD("thread", luaopen_thread);

#undef JOY_MOD
        lua_setglobal(luax->L, "joy");
        return luax;
}

bool luax_dofile(luax_p luax, const char* filename)
{
        char chunkname[JOY_MAX_PATH] = { 0 };
        strcat(chunkname, "@");
        strcat(chunkname, filename);
        size_t data_sz;
        char* data = utils_read_file(filename, &data_sz);
        if (data) {
                if (luaL_loadbuffer(luax->L, data, data_sz, chunkname) != LUA_OK) {
                        const char* error_msg = lua_tostring(luax->L, -1);
                        log_info("%s", error_msg);
                        return false;
                }
                if (lua_pcall(luax->L, 0, LUA_MULTRET, 0) != LUA_OK) {
                        const char* error_msg = lua_tostring(luax->L, -1);
                        log_info("%s", error_msg);
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

void luax_dispatch_event(luax_p luax, const SDL_Event* event)
{
        dispatch_event(luax->cbs, event, luax);
}

#define STATE_KEY ((void*)0x1)

void luax_init_state(luax_p luax)
{
        lua_State* L = luax->L;
        // state 表（如果已存在则不覆盖）
        lua_getglobal(L, "state");
        if (lua_isnil(L, -1)) {
                lua_pop(L, 1);
                lua_newtable(L);
                lua_setglobal(L, "state");
        } else {
                lua_pop(L, 1);
        }
        // 立即持久化
        lua_getglobal(L, "state");
        lua_pushlightuserdata(L, STATE_KEY);
        lua_pushvalue(L, -2);
        lua_settable(L, LUA_REGISTRYINDEX);
        lua_pop(L, 1);
}

void luax_save_state(luax_p luax)
{
        lua_State* L = luax->L;
        lua_getglobal(L, "state");
        lua_pushlightuserdata(L, STATE_KEY);
        lua_pushvalue(L, -2);
        lua_settable(L, LUA_REGISTRYINDEX);
        lua_pop(L, 1);
}

static void luax_restore_state(luax_p luax)
{
        lua_State* L = luax->L;
        lua_pushlightuserdata(L, STATE_KEY);
        lua_gettable(L, LUA_REGISTRYINDEX);
        if (!lua_isnil(L, -1)) {
                lua_setglobal(L, "state");
        } else {
                lua_pop(L, 1);
                luax_init_state(luax);
        }
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
                log_error("joy.%s error: %s", func, lua_tostring(L, -1));
                lua_pop(L, 1);
                lua_pop(L, 1);
                return false;
        }
        lua_pop(L, 1);
        return true;
}

bool luax_reload(luax_p luax, const char* filename)
{
        // 备份当前 state → 执行脚本 → 恢复 state
        luax_save_state(luax);
        bool ok = luax_dofile(luax, filename);
        luax_restore_state(luax);
        return ok;
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
        log_info("[CHANGED] %s", luax->watch_file);
        if (!luax_reload(luax, luax->watch_file))
                return false;
        luax_call_joy(luax, "hotfix", 0, 0);
        return true;
}

const char* luax_get_watch(luax_p luax)
{
        return luax->watch_file;
}

