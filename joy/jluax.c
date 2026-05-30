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
        luaL_requiref(luax->L, "window", luaopen_window, 1);
        luaL_requiref(luax->L, "sdl", luaopen_sdl, 1);
        luaL_requiref(luax->L, "audio", luaopen_audio, 1);
        //luaL_requiref(luax->L, "graphics", luaopen_graphics, 1);
        luaL_requiref(luax->L, "keyboard", luaopen_keyboard, 1);
        luaL_requiref(luax->L, "net", luaopen_net, 1);
        luaL_requiref(luax->L, "utils", luaopen_utils, 1);
        luaL_requiref(luax->L, "mathx", luaopen_mathx, 1);
        luaL_requiref(luax->L, "vec2", luaopen_vec2, 1);
        luaL_requiref(luax->L, "vec3", luaopen_vec3, 1);
        luaL_requiref(luax->L, "collision2d", luaopen_collision2d, 1);
        luaL_requiref(luax->L, "collision3d", luaopen_collision3d, 1);
        luaL_requiref(luax->L, "cjson", luaopen_cjson_safe, 1);
        luaL_requiref(luax->L, "packagex", luaopen_packagex, 1);
        luaL_requiref(luax->L, "ui", luaopen_ui, 1);
        luaL_requiref(luax->L, "c2s", luaopen_c2s, 1);
        luaL_requiref(luax->L, "s2c", luaopen_s2c, 1);
        luaL_requiref(luax->L, "ecs", luaopen_ecs, 1);
        luaL_requiref(luax->L, "rigidbody", luaopen_rigidbody, 1);
        luaL_requiref(luax->L, "world2df", luaopen_world2df, 1);
        luaL_requiref(luax->L, "server", luaopen_server, 1);
        luaL_requiref(luax->L, "client", luaopen_client, 1);
        luaL_requiref(luax->L, "timer", luaopen_timer, 1);
        luaL_requiref(luax->L, "animation", luaopen_animation, 1);
        luaL_requiref(luax->L, "joystick", luaopen_joystick, 1);
        //luaL_requiref(luax->L, "profiler", luaopen_profiler, 1);
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

#define STATE_KEY ((void*)0x1)

void luax_init_state(luax_p luax)
{
        lua_State* L = luax->L;
        lua_newtable(L);
        lua_setglobal(L, "state");
        // 立即持久化
        lua_getglobal(L, "state");
        lua_pushlightuserdata(L, STATE_KEY);
        lua_pushvalue(L, -2);
        lua_settable(L, LUA_REGISTRYINDEX);
        lua_pop(L, 1);

        // 创建 joy 空表，脚本只需 function joy.load() {} 即可
        lua_newtable(L);
        lua_setglobal(L, "joy");
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
        // 1. 备份当前 state
        luax_save_state(luax);

        // 2. 执行脚本（复用 dofile 逻辑）
        bool ok = luax_dofile(luax, filename);

        // 3. 恢复 state（即使失败也恢复）
        luax_restore_state(luax);

        // 4. 热重载后调用 joy.hotfix()
        if (ok)
                luax_call_joy(luax, "hotfix", 0, 0);

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

