#include "./external/mujs/mujs.h"
#include "liv-jsx.h"
#include "liv-utils.h"
#include "liv-log.h"
#include "jsclib.h"
#include <SDL3/SDL.h>

typedef struct jsx {
        js_State* j;
        app_p app;
}jsx_t, *jsx_p;

static void jrequire(js_State* J)
{
        jsx_p jsx;
        char filename[LIVNET_MAX_PATH] = { 0 };
        const char *name, *root_path;
        char *data;
        size_t data_size;

        jsx = (jsx_p)js_getcontext(J);
        name = js_tostring(J, 1);
        root_path = liv_get(jsx->app, "root_path");
        SDL_strlcat(filename, root_path, LIVNET_MAX_PATH);
        SDL_strlcat(filename, name, LIVNET_MAX_PATH);
        data = SDL_LoadFile(filename, &data_size);
        if (data) {
                data[data_size] = 0;
                if (js_dostring(J, data)) {
                        /* failure */
                        SDL_Log("dostring %s fail.", filename);
                }
                SDL_free(data);
        }
        js_pushundefined(J);
}


static void jcore_quit(js_State* J)
{
        jsx_p jsx;
        jsx = (jsx_p)js_toint64(J, 1);
        liv_quit(jsx->app);
        js_pushundefined(J);
}

static void jcore_log(js_State* J)
{
        jsx_p jsx;
        const char* str;
        jsx = (jsx_p)js_toint64(J, 1);
        str = js_tostring(J, 2);
        liv_log_info("jsx:%s", str);
        js_pushundefined(J);
}

static void jcore_debug(js_State* J)
{
        jsx_p jsx;
        const char* str;
        jsx = (jsx_p)js_toint64(J, 1);
        str = js_tostring(J, 2);
        liv_log_debug("jsx:%s", str);
        js_pushundefined(J);
}

static void jcore_error(js_State* J)
{
        jsx_p jsx;
        const char* str;
        jsx = (jsx_p)js_toint64(J, 1);
        str = js_tostring(J, 2);
        liv_log_error("jsx:%s", str);
        js_pushundefined(J);
}

static void jcore_get_env(js_State* J)
{
        jsx_p jsx;
        const char* name, *value;
        jsx = (jsx_p)js_toint64(J, 1);
        name = js_tostring(J, 2);
        value = liv_get(jsx->app, name);
        js_pushstring(J, value);
}

static void jcore_set_env(js_State* J)
{
        jsx_p jsx;
        const char* name, *value;
        jsx = (jsx_p)js_toint64(J, 1);
        name = js_tostring(J, 2);
        value = js_tostring(J, 3);
        liv_set(jsx->app, name, value);
        js_pushundefined(J);
}

static void jsopen_core(js_State* J)
{
        js_newcfunction(J, jrequire, "require", 1);
        js_setglobal(J, "require");
        js_newobject(J);
        js_newcfunction(J, jcore_quit, "quit", 1);
        js_setproperty(J, -2, "quit");
        js_newcfunction(J, jcore_log, "log", 1);
        js_setproperty(J, -2, "log");
        js_newcfunction(J, jcore_debug, "debug", 1);
        js_setproperty(J, -2, "debug");
        js_newcfunction(J, jcore_error, "error", 1);
        js_setproperty(J, -2, "error");
        js_newcfunction(J, jcore_get_env, "get_env", 1);
        js_setproperty(J, -2, "get_env");
        js_newcfunction(J, jcore_set_env, "set_env", 1);
        js_setproperty(J, -2, "set_env");
        js_setglobal(J, "core");
}

void* liv_jsx_create(void)
{
        jsx_p jsx;
        jsx = (jsx_p)SDL_malloc(sizeof(jsx_t));
        SDL_assert(jsx);
        jsx->j = js_newstate(NULL, jsx, JS_STRICT);
        SDL_assert(jsx->j);
        return jsx;
}

int liv_jsx_init(void* inst, void *app_ptr, const char* param)
{
        const char* root_path;
        char filename[LIVNET_MAX_PATH] = { 0 };
        size_t data_size;
        char* data;
        jsx_p jsx;

        jsx = (jsx_p)inst;
        jsx->app = (app_p)app_ptr;
        jsopen_core(jsx->j);
        jsopen_base(jsx->j);
        jsopen_fp(jsx->j);
        jsopen_vec2f(jsx->j);
        jsopen_vec2(jsx->j);
        jsopen_lockstep(jsx->j);

        js_setcontext(jsx->j, jsx);
        root_path = liv_get(jsx->app, "root_path");
        SDL_strlcat(filename, root_path, LIVNET_MAX_PATH);
        SDL_strlcat(filename, param, LIVNET_MAX_PATH);
        data = SDL_LoadFile(filename, &data_size);
        if (data) {
                if (js_dostring(jsx->j, data)) {
                        SDL_free(data);
                        SDL_Log("%s", js_tostring(jsx->j, -1));
                        goto failure;
                }
                else {
                        SDL_free(data);
                }
        }

        if (!js_try(jsx->j)) {
                js_getglobal(jsx->j, "app");
                if (!js_isundefined(jsx->j, -1)) {
                        if (!js_iscallable(jsx->j, -1)) {
                                js_getproperty(jsx->j, -1, "start");
                                js_copy(jsx->j, -2);
                                js_pushnumber(jsx->j, (uint64_t)jsx);
                                js_call(jsx->j, 1);

                                /* pop return value */
                                js_pop(jsx->j, 1);

                        }
                }
                js_pop(jsx->j, 1);
        }
        else {
                SDL_Log("Error calling start: %s", js_tostring(jsx->j, -1));
                js_pop(jsx->j, 1);

        }
        js_endtry(jsx->j);

failure:
        return 0;
}

int liv_jsx_event_handler(void* inst, void* event)
{
        jsx_p jsx;

        jsx = (jsx_p)inst;
        if (!js_try(jsx->j)) {
                js_getglobal(jsx->j, "app");
                if (!js_isundefined(jsx->j, -1)) {
                        if (!js_iscallable(jsx->j, -1)) {
                                js_getproperty(jsx->j, -1, "event");
                                js_copy(jsx->j, -2);
                                js_pushnumber(jsx->j, (uint64_t)event);
                                js_call(jsx->j, 1);

                                /* pop return value */
                                js_pop(jsx->j, 1);
                        }
                }
                js_pop(jsx->j, 1);
        }
        else {
                SDL_Log("Error calling event: %s", js_tostring(jsx->j, -1));
                js_pop(jsx->j, 1);

        }
        js_endtry(jsx->j);
        
        return 0;
}

int liv_jsx_update(void* inst, float delta_time)
{
        jsx_p jsx;

        jsx = (jsx_p)inst;
        
        int k = js_gettop(jsx->j);
        if (!js_try(jsx->j)) {
                js_getglobal(jsx->j, "app");
                if (!js_isundefined(jsx->j, -1)) {
                        if (!js_iscallable(jsx->j, -1)) {
                                js_getproperty(jsx->j, -1, "update");
                                js_copy(jsx->j, -2);
                                js_pushnumber(jsx->j, delta_time);
                                js_call(jsx->j, 1);
                                js_pop(jsx->j, 1);
                        }
                }
                js_pop(jsx->j, 1);
        }
        else {
                SDL_Log("Error calling update: %s", js_tostring(jsx->j, -1));
                js_pop(jsx->j, 1);

        }
        js_endtry(jsx->j);
        k = js_gettop(jsx->j);

        return 0;
}

void liv_jsx_release(void* inst)
{
        jsx_p jsx;
        jsx = (jsx_p)inst;

        if (!js_try(jsx->j)) {
                js_getglobal(jsx->j, "app");
                if (!js_isundefined(jsx->j, -1)) {
                        if (!js_iscallable(jsx->j, -1)) {
                                js_getproperty(jsx->j, -1, "destroy");
                                js_copy(jsx->j, -2);
                                js_call(jsx->j, 0);

                                /* pop return value */
                                js_pop(jsx->j, 1);
                        }
                }
                js_pop(jsx->j, 1);
        }
        else {
                SDL_Log("Error calling release: %s", js_tostring(jsx->j, -1));
                js_pop(jsx->j, 1);

        }
        js_endtry(jsx->j);


        js_freestate(jsx->j);
        SDL_free(jsx);
}
