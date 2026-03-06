#include <stdlib.h>
#include <assert.h>
#include "mujs/mujs.h"
#include "jsx.h"
#include "joy2d/utils.h"
#include "joy2d/log.h"
#include "jsclib.h"

struct jsx {
        js_State* j;
        void* userdata;
};

//
//static void jrequire(js_State* J)
//{
//        jsx_p jsx;
//        char filename[JOY_MAX_PATH] = { 0 };
//        const char *name, *root_path;
//        char *data;
//        size_t data_size;
//
//        jsx = (jsx_p)js_getcontext(J);
//        name = js_tostring(J, 1);
//        root_path = joy_getenv(jsx->app, "root_path");
//        SDL_strlcat(filename, root_path, JOY_MAX_PATH);
//        SDL_strlcat(filename, name, JOY_MAX_PATH);
//        data = SDL_LoadFile(filename, &data_size);
//        if (data) {
//                data[data_size] = 0;
//                if (js_dostring(J, data)) {
//                        /* failure */
//                        SDL_Log("dostring %s fail.", filename);
//                }
//                SDL_free(data);
//        }
//        js_pushundefined(J);
//}
//
//
//static void jcore_quit(js_State* J)
//{
//        jsx_p jsx;
//        jsx = (jsx_p)js_toint64(J, 1);
//        joy_quit(jsx->app);
//        js_pushundefined(J);
//}
//
//static void jcore_log(js_State* J)
//{
//        jsx_p jsx;
//        const char* str;
//        jsx = (jsx_p)js_toint64(J, 1);
//        str = js_tostring(J, 2);
//        log_info("jsx:%s", str);
//        js_pushundefined(J);
//}
//
//static void jcore_debug(js_State* J)
//{
//        jsx_p jsx;
//        const char* str;
//        jsx = (jsx_p)js_toint64(J, 1);
//        str = js_tostring(J, 2);
//        log_debug("jsx:%s", str);
//        js_pushundefined(J);
//}
//
//static void jcore_error(js_State* J)
//{
//        jsx_p jsx;
//        const char* str;
//        jsx = (jsx_p)js_toint64(J, 1);
//        str = js_tostring(J, 2);
//        log_error("jsx:%s", str);
//        js_pushundefined(J);
//}
//
//static void jcore_get_env(js_State* J)
//{
//        jsx_p jsx;
//        const char* name, *value;
//        jsx = (jsx_p)js_toint64(J, 1);
//        name = js_tostring(J, 2);
//        value = joy_getenv(jsx->app, name);
//        js_pushstring(J, value);
//}
//
//static void jcore_set_env(js_State* J)
//{
//        jsx_p jsx;
//        const char* name, *value;
//        jsx = (jsx_p)js_toint64(J, 1);
//        name = js_tostring(J, 2);
//        value = js_tostring(J, 3);
//        joy_setenv(jsx->app, name, value);
//        js_pushundefined(J);
//}
//
//static void jsopen_core(js_State* J)
//{
//        js_newcfunction(J, jrequire, "require", 1);
//        js_setglobal(J, "require");
//        js_newobject(J);
//        js_newcfunction(J, jcore_quit, "quit", 1);
//        js_setproperty(J, -2, "quit");
//        js_newcfunction(J, jcore_log, "log", 1);
//        js_setproperty(J, -2, "log");
//        js_newcfunction(J, jcore_debug, "debug", 1);
//        js_setproperty(J, -2, "debug");
//        js_newcfunction(J, jcore_error, "error", 1);
//        js_setproperty(J, -2, "error");
//        js_newcfunction(J, jcore_get_env, "get_env", 1);
//        js_setproperty(J, -2, "get_env");
//        js_newcfunction(J, jcore_set_env, "set_env", 1);
//        js_setproperty(J, -2, "set_env");
//        js_setglobal(J, "core");
//}

jsx_p jsx_create(void)
{
        jsx_p jsx;
        jsx = (jsx_p)malloc(sizeof(jsx_t));
        assert(jsx);
        jsx->j = js_newstate(NULL, jsx, JS_STRICT);
        assert(jsx->j);
        jsopen_base(jsx->j);
        jsopen_fp(jsx->j);
        jsopen_fp(jsx->j);
        jsopen_vec2f(jsx->j);
        jsopen_vec2(jsx->j);
        js_setcontext(jsx->j, jsx);
        return jsx;
}

bool jsx_dofile(jsx_p jsx, const char* filename)
{
        size_t data_size;
        char* data;
        data = utils_read_file(filename, &data_size);
        if (data) {
                if (js_dostring(jsx->j, data)) {
                        free(data);
                        log_info("%s", js_tostring(jsx->j, -1));
                        return true;
                }
                else {
                        free(data);
                        return false;
                }
        }
        return false;
}


void jsx_release(jsx_p jsx)
{
        js_freestate(jsx->j);
        free(jsx);
}
