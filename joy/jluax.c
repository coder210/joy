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
#include "jutils.h"
#include "luaclib/luaclib.h"
#include "jluax.h"

struct luax {
        lua_State* L;
        void* userdata;
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

luax_p luax_create(void)
{
        luax_p luax;
        luax = (luax_p)malloc(sizeof(luax_t));
        assert(luax);
        luax->L = luaL_newstate();
        assert(luax->L);
        luaL_openlibs(luax->L);
        lua_pushlightuserdata(luax->L, luax);
        lua_setfield(luax->L, LUA_REGISTRYINDEX, "__this");

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
        JOY_MOD("mathx", luaopen_mathx);
        JOY_MOD("vec2", luaopen_vec2);
        JOY_MOD("vec3", luaopen_vec3);
        JOY_MOD("collision2d", luaopen_collision2d);
        JOY_MOD("collision3d", luaopen_collision3d);
        JOY_MOD("cjson", luaopen_cjson_safe);
        JOY_MOD("packagex", luaopen_packagex);
        JOY_MOD("ui", luaopen_ui);
        JOY_MOD("c2s", luaopen_c2s);
        JOY_MOD("s2c", luaopen_s2c);
        JOY_MOD("ecs", luaopen_ecs);
        JOY_MOD("rigidbody", luaopen_rigidbody);
        JOY_MOD("world2df", luaopen_world2df);
        JOY_MOD("server", luaopen_server);
        JOY_MOD("client", luaopen_client);
        JOY_MOD("timer", luaopen_timer);
        JOY_MOD("animation", luaopen_animation);
        JOY_MOD("joystick", luaopen_joystick);

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
        lua_close(luax->L);
        free(luax);
}

lua_State* luax_get_state(luax_p luax)
{
        return luax->L;
}

/* =================================================================
 * 事件分发
 * ================================================================= */

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

/* 按键名映射 */
static const char* key_name(SDL_Keycode k)
{
        static char buf[2];
        switch (k) {
        case SDLK_RETURN:    return "return";
        case SDLK_ESCAPE:    return "escape";
        case SDLK_BACKSPACE: return "backspace";
        case SDLK_TAB:       return "tab";
        case SDLK_SPACE:     return "space";
        case SDLK_DELETE:    return "delete";
        case SDLK_UP:        return "up";
        case SDLK_DOWN:      return "down";
        case SDLK_LEFT:      return "left";
        case SDLK_RIGHT:     return "right";
        case SDLK_HOME:      return "home";
        case SDLK_END:       return "end";
        case SDLK_PAGEUP:    return "pageup";
        case SDLK_PAGEDOWN:  return "pagedown";
        case SDLK_INSERT:    return "insert";
        case SDLK_LSHIFT:    return "lshift";
        case SDLK_RSHIFT:    return "rshift";
        case SDLK_LCTRL:     return "lctrl";
        case SDLK_RCTRL:     return "rctrl";
        case SDLK_LALT:      return "lalt";
        case SDLK_RALT:      return "ralt";
        default:
                if (k >= SDLK_A && k <= SDLK_Z) { buf[0] = (char)('a' + (k - SDLK_A)); buf[1] = 0; return buf; }
                if (k >= SDLK_0 && k <= SDLK_9) { buf[0] = (char)('0' + (k - SDLK_0)); buf[1] = 0; return buf; }
                return NULL;
        }
}

void luax_dispatch_event(luax_p luax, const SDL_Event* event)
{
        lua_State* L = luax->L;

        switch (event->type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
                lua_pushnumber(L, event->button.x);
                lua_pushnumber(L, event->button.y);
                lua_pushinteger(L, event->button.button);
                call_joy_n(L, "mousepressed", 3);
                break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
                lua_pushnumber(L, event->button.x);
                lua_pushnumber(L, event->button.y);
                lua_pushinteger(L, event->button.button);
                call_joy_n(L, "mousereleased", 3);
                break;

        case SDL_EVENT_MOUSE_MOTION:
                lua_pushnumber(L, event->motion.x);
                lua_pushnumber(L, event->motion.y);
                lua_pushnumber(L, event->motion.xrel);
                lua_pushnumber(L, event->motion.yrel);
                call_joy_n(L, "mousemoved", 4);
                break;

        case SDL_EVENT_KEY_DOWN: {
                const char* name = key_name(event->key.key);
                if (!name) break;
                lua_pushstring(L, name);
                lua_pushinteger(L, event->key.scancode);
                lua_pushboolean(L, event->key.repeat);
                call_joy_n(L, "keypressed", 3);
                break;
        }

        case SDL_EVENT_KEY_UP: {
                const char* name = key_name(event->key.key);
                if (!name) break;
                lua_pushstring(L, name);
                lua_pushinteger(L, event->key.scancode);
                call_joy_n(L, "keyreleased", 2);
                break;
        }

        case SDL_EVENT_WINDOW_RESIZED:
                lua_pushinteger(L, event->window.data1);
                lua_pushinteger(L, event->window.data2);
                call_joy_n(L, "resize", 2);
                break;
        }
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

//static void 
//_append_luapath(lua_State* L, const char* new_path)
//{
//        const char* current_path;
//        char combined_path[JOY_MAX_PATH];
//
//        lua_getglobal(L, "packagex");
//        lua_getfield(L, -1, "path");
//        current_path = lua_tostring(L, -1);
//        SDL_strlcpy(combined_path, current_path, JOY_MAX_PATH);
//        SDL_strlcat(combined_path, ";", JOY_MAX_PATH);
//        SDL_strlcat(combined_path, new_path, JOY_MAX_PATH);
//        lua_pushstring(L, combined_path);
//        lua_setfield(L, -3, "path");
//        lua_pop(L, 2);
//}
//
//static char*
//_read_file(const char* filename, uint32_t* out_sz)
//{
//        size_t data_size;
//        char* data, * decrypt_data;
//        const char* key;
//
//        key = "com.livnet.liwei";
//        data = SDL_LoadFile(filename, &data_size);
//        if (!data) {
//                *out_sz = 0;
//                return data;
//        }
//
//        if (SDL_strstr(filename, ".luas")) {
//                decrypt_data = utils_des_decrypt(key, data, data_size, &data_size);
//                if (data) {
//                        decrypt_data[data_size] = 0;
//                        *out_sz = data_size;
//                        SDL_free(data);
//                }
//                return decrypt_data;
//        }
//        else {
//                *out_sz = data_size;
//                data[*out_sz] = 0;
//                return data;
//        }
//}
//
//
//static int ldofilex(lua_State* L)
//{
//        const char* filename, * error_msg;
//        char chunkname[JOY_MAX_PATH] = { 0 };
//        char* data;
//        uint32_t data_size;
//
//        filename = luaL_checkstring(L, 1);
//        SDL_strlcat(chunkname, "@", JOY_MAX_PATH);
//        SDL_strlcat(chunkname, filename, JOY_MAX_PATH);
//
//        data = _read_file(filename, &data_size);
//        if (!data) {
//                error_msg = "filename is not exists.";
//                goto failure;
//        }
//        if (luaL_loadbuffer(L, data, data_size, chunkname) != LUA_OK) {
//                error_msg = lua_tostring(L, -1);
//                SDL_free(data);
//                goto failure;
//        }
//        if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
//                error_msg = lua_tostring(L, -1);
//                SDL_free(data);
//                goto failure;
//        }
//        return 1;
//failure:
//        luaL_error(L, "%s", error_msg);
//        return 0;
//}

