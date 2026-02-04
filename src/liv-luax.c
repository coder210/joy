/************************************************
Copyright: 2021-2022, lanchong.xyz/Ltd.
File name: mod-luax.c
Description:
Author: ydlc
Version: 1.0
Date: 2021.12.14
History:
*************************************************/
#include "./external/lua/lapi.h"
#include "./external/lua/lualib.h"
#include "./external/lua/lauxlib.h"
#include "./external/lua_cjson.h"
#include <SDL3/SDL.h>
#include "liv-luax.h"
#include "liv-log.h"
#include "liv-utils.h"
#include "luaclib.h"
#include "external/lsproto.h"

typedef struct luax {
        lua_State* L;
        app_p app;
}luax_t, * luax_p;


static int l_core_traceback(lua_State* L)
{
        const char* msg = lua_tostring(L, 1);
        if (msg) {
                luaL_traceback(L, L, msg, 1);
                lua_remove(L, 1);
                const char* trace = lua_tostring(L, -1);
                liv_log_error("c:%s", trace);
        }
        else {
                lua_pushliteral(L, "(no error message)");
        }
        return 1;
}

static int l_core_quit(lua_State* L)
{
        luax_t* luax;
        luax = (luax_t*)lua_touserdata(L, 1);
        liv_quit(luax->app);
        return 0;
}

static int l_core_log(lua_State* L)
{
        luax_t* luax;
        const char* str;
        luax = lua_touserdata(L, 1);
        str = luaL_checkstring(L, 2);
        liv_log_info("luax:%s", str);
        return 1;
}

static int l_core_debug(lua_State* L)
{
        luax_t* luax;
        const char* str;
        luax = lua_touserdata(L, 1);
        str = luaL_checkstring(L, 2);
        liv_log_debug("luax:%s", str);
        return 1;
}

static int l_core_error(lua_State* L)
{
        luax_t* luax;
        const char* str;
        luax = lua_touserdata(L, 1);
        str = luaL_checkstring(L, 2);
        liv_log_error("luax:%s", str);
        return 1;
}

static int l_core_get_env(lua_State* L)
{
        luax_t* luax;
        const char* name, * value;
        luax = (luax_t*)lua_touserdata(L, 1);
        name = luaL_checkstring(L, 2);
        value = liv_get(luax->app, name);
        lua_pushstring(L, value);
        return 1;
}

static int l_core_set_env(lua_State* L)
{
        luax_t* luax;
        const char* name, * value;
        luax = (luax_t*)lua_touserdata(L, 1);
        name = luaL_checkstring(L, 2);
        value = luaL_checkstring(L, 3);
        liv_set(luax->app, name, value);
        return 0;
}

static int luaopen_core(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
        {"quit", l_core_quit},
        {"log", l_core_log},
        {"debug", l_core_debug},
        {"error", l_core_error},
        {"get_env", l_core_get_env},
        {"set_env", l_core_set_env},
        {NULL, NULL},
        };
        luaL_newlib(L, l);
        return 1;
}

void* liv_luax_create(void)
{
        luax_p luax;
        luax = (luax_p)SDL_malloc(sizeof(luax_t));
        SDL_assert(luax);
        luax->L = luaL_newstate();
        return luax;
}

static void 
_append_luapath(lua_State* L, const char* new_path)
{
        const char* current_path;
        char combined_path[LIVNET_MAX_PATH];

        lua_getglobal(L, "packagex");
        lua_getfield(L, -1, "path");
        current_path = lua_tostring(L, -1);
        SDL_strlcpy(combined_path, current_path, LIVNET_MAX_PATH);
        SDL_strlcat(combined_path, ";", LIVNET_MAX_PATH);
        SDL_strlcat(combined_path, new_path, LIVNET_MAX_PATH);
        lua_pushstring(L, combined_path);
        lua_setfield(L, -3, "path");
        lua_pop(L, 2);
}

static char*
_read_file(const char* filename, uint32_t* out_sz)
{
        size_t data_size;
        char* data, * decrypt_data;
        const char* key;

        key = "com.livnet.liwei";
        data = SDL_LoadFile(filename, &data_size);
        if (!data) {
                *out_sz = 0;
                return data;
        }

        if (SDL_strstr(filename, ".luas")) {
                decrypt_data = utils_des_decrypt(key, data, data_size, &data_size);
                if (data) {
                        decrypt_data[data_size] = 0;
                        *out_sz = data_size;
                        SDL_free(data);
                }
                return decrypt_data;
        }
        else {
                *out_sz = data_size;
                data[*out_sz] = 0;
                return data;
        }
}


static int ldofilex(lua_State* L)
{
        const char* filename, * error_msg;
        char chunkname[LIVNET_MAX_PATH] = { 0 };
        char* data;
        uint32_t data_size;

        filename = luaL_checkstring(L, 1);
        SDL_strlcat(chunkname, "@", LIVNET_MAX_PATH);
        SDL_strlcat(chunkname, filename, LIVNET_MAX_PATH);

        data = _read_file(filename, &data_size);
        if (!data) {
                error_msg = "filename is not exists.";
                goto failure;
        }
        if (luaL_loadbuffer(L, data, data_size, chunkname) != LUA_OK) {
                error_msg = lua_tostring(L, -1);
                SDL_free(data);
                goto failure;
        }
        if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
                error_msg = lua_tostring(L, -1);
                SDL_free(data);
                goto failure;
        }
        return 1;
failure:
        luaL_error(L, "%s", error_msg);
        return 0;
}
int liv_luax_init(void* inst, void* app_ptr, const char* param)
{
        const char* luapath;
        luax_p luax;
        char filename[LIVNET_MAX_PATH], chunkname[LIVNET_MAX_PATH];
        uint32_t data_len;
        char* data;

        luax = (luax_p)inst;
        luax->app = (app_p)app_ptr;

        luapath = liv_get(luax->app, "luapath");

        luaL_openlibs(luax->L);
        lua_pushlightuserdata(luax->L, luax);
        lua_setfield(luax->L, LUA_REGISTRYINDEX, "__this");
        lua_pushcfunction(luax->L, l_core_traceback);
        luaL_requiref(luax->L, "window", luaopen_window, 1);
        luaL_requiref(luax->L, "sdl", luaopen_sdl, 1);
        luaL_requiref(luax->L, "audio", luaopen_audio, 1);
        luaL_requiref(luax->L, "graphics", luaopen_graphics, 1);
        luaL_requiref(luax->L, "keyboard", luaopen_keyboard, 1);
        luaL_requiref(luax->L, "net", luaopen_net, 1);
        luaL_requiref(luax->L, "utils", luaopen_utils, 1);
        luaL_requiref(luax->L, "mathx", luaopen_mathx, 1);
        luaL_requiref(luax->L, "vec2", luaopen_vec2, 1);
        luaL_requiref(luax->L, "vec3", luaopen_vec3, 1);
        luaL_requiref(luax->L, "collision2d", luaopen_collision2d, 1);
        luaL_requiref(luax->L, "collision3d", luaopen_collision3d, 1);
        luaL_requiref(luax->L, "cjson", luaopen_cjson_safe, 1);
        luaL_requiref(luax->L, "sproto_core", luaopen_sproto_core, 1);
        luaL_requiref(luax->L, "packagex", luaopen_packagex, 1);
        luaL_requiref(luax->L, "core", luaopen_core, 1);
        luaL_requiref(luax->L, "ui", luaopen_ui, 1);
        luaL_requiref(luax->L, "lockstep", luaopen_lockstep, 1);
        luaL_requiref(luax->L, "c2s", luaopen_c2s, 1);
        luaL_requiref(luax->L, "s2c", luaopen_s2c, 1);
        luaL_requiref(luax->L, "ecs", luaopen_ecs, 1);
        luaL_requiref(luax->L, "rigidbody", luaopen_rigidbody, 1);
        luaL_requiref(luax->L, "world2df", luaopen_world2df, 1);
        luaL_requiref(luax->L, "server", luaopen_server, 1);
        luaL_requiref(luax->L, "client", luaopen_client, 1);
        luaL_requiref(luax->L, "timer", luaopen_timer, 1);
        luaL_requiref(luax->L, "render_world", luaopen_render_world, 1);
        luaL_requiref(luax->L, "animation", luaopen_animation, 1);
        luaL_requiref(luax->L, "joystick", luaopen_joystick, 1);
        luaL_requiref(luax->L, "profiler", luaopen_profiler, 1);

        _append_luapath(luax->L, luapath);

        SDL_memset(filename, 0, LIVNET_MAX_PATH);
        SDL_memset(chunkname, 0, LIVNET_MAX_PATH);

        SDL_strlcat(filename, param, LIVNET_MAX_PATH);
        SDL_strlcat(chunkname, "@", LIVNET_MAX_PATH);
        SDL_strlcat(chunkname, filename, LIVNET_MAX_PATH);
        data = _read_file(filename, &data_len);
        if (data) {
                if (luaL_loadbuffer(luax->L, data, data_len, chunkname) != LUA_OK) {
                        const char* error_msg = lua_tostring(luax->L, -1);
                        SDL_Log("%s", error_msg);
                }
                if (lua_pcall(luax->L, 0, LUA_MULTRET, 1) != LUA_OK) {
                        const char* error_msg = lua_tostring(luax->L, -1);
                        SDL_Log("%s", error_msg);
                }
                else {
                }
                SDL_free(data);
        }

        lua_getfield(luax->L, -1, "start");
        if (lua_isfunction(luax->L, -1)) {
                lua_pushlightuserdata(luax->L, luax);
                lua_pushcfunction(luax->L, l_core_traceback);
                lua_insert(luax->L, -3);
                if (lua_pcall(luax->L, 1, LUA_MULTRET, -3) != LUA_OK) {
                        liv_log_error("start:%s", lua_tostring(luax->L, -1));
                }
                lua_pop(luax->L, 1);
        }

        return 0;
}

int liv_luax_event_handler(void* inst, void* event)
{
        luax_p luax;
        luax = (luax_p)inst;
        int top = lua_gettop(luax->L);
        lua_getfield(luax->L, -1, "event");
        if (lua_isfunction(luax->L, -1)) {
                lua_pushlightuserdata(luax->L, event);
                lua_pushcfunction(luax->L, l_core_traceback);
                lua_insert(luax->L, -3);
                if (lua_pcall(luax->L, 1, LUA_MULTRET, -3) != LUA_OK) {
                        SDL_Log("event:%s", lua_tostring(luax->L, -1));
                }
                lua_pop(luax->L, 1);
        }
        top = lua_gettop(luax->L);
        return 0;
}

int liv_luax_update(void* inst, float delta_time)
{
        luax_p luax;
        luax = (luax_p)inst;
        lua_getfield(luax->L, -1, "update");
        if (lua_isfunction(luax->L, -1)) {
                lua_pushcfunction(luax->L, l_core_traceback);
                lua_insert(luax->L, -2);
                lua_pushnumber(luax->L, delta_time);
                if (lua_pcall(luax->L, 1, LUA_MULTRET, -3) != LUA_OK) {
                        SDL_Log("update:%s", lua_tostring(luax->L, -1));
                }
                lua_pop(luax->L, 1);
        }
        return 0;
}

void liv_luax_release(void* inst)
{
        luax_p luax;
        luax = (luax_p)inst;
        lua_getfield(luax->L, -1, "destroy");
        if (lua_isfunction(luax->L, -1)) {
                lua_pushcfunction(luax->L, l_core_traceback);
                lua_insert(luax->L, -2);
                if (lua_pcall(luax->L, 0, LUA_MULTRET, -2) != LUA_OK) {
                        SDL_Log("destroy:%s", lua_tostring(luax->L, -1));
                }
                lua_pop(luax->L, 1);
        }
        lua_close(luax->L);
        SDL_free(luax);
}

