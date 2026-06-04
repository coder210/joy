#include "jsclib.h"
#include <SDL3/SDL.h>

static void jsdl_set_app_metadata(js_State *J)
{
        const char *name = js_tostring(J, 1);
        const char *version = js_tostring(J, 2);
        const char *identifier = js_tostring(J, 3);
        js_pushboolean(J, SDL_SetAppMetadata(name, version, identifier));
}

static void jsdl_get_version(js_State *J)
{
        js_pushnumber(J, SDL_GetVersion());
}

static void jsdl_log(js_State *J)
{
        SDL_Log("%s", js_tostring(J, 1));
}

static void jsdl_error(js_State *J)
{
        SDL_LogError(SDL_LOG_PRIORITY_ERROR, "%s", js_tostring(J, 1));
}

static void jsdl_delay(js_State *J)
{
        SDL_Delay((Uint32)js_tonumber(J, 1));
}

static void jsdl_get_ticks(js_State *J)
{
        js_pushnumber(J, (double)SDL_GetTicks());
}

static void jsdl_get_mouse_state(js_State *J)
{
        float x, y;
        SDL_GetMouseState(&x, &y);
        js_pushnumber(J, x);
        js_pushnumber(J, y);
}

static void jsdl_load_file(js_State *J)
{
        const char *file = js_tostring(J, 1);
        size_t data_size;
        char *data = (char*)SDL_LoadFile(file, &data_size);
        if (data) {
                js_pushboolean(J, true);
                js_pushlstring(J, data, data_size);
                SDL_free(data);
        } else {
                js_pushboolean(J, false);
                js_pushundefined(J);
        }
}

static void jsdl_get_current_display_mode(js_State *J)
{
        int index = (int)js_tonumber(J, 1);
        const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(index);
        if (mode) {
                js_newobject(J);
                js_pushnumber(J, mode->w);              js_setproperty(J, -2, "w");
                js_pushnumber(J, mode->h);              js_setproperty(J, -2, "h");
                js_pushnumber(J, (int)mode->format);     js_setproperty(J, -2, "format");
                js_pushnumber(J, mode->refresh_rate);   js_setproperty(J, -2, "refresh_rate");
        } else {
                js_pushundefined(J);
        }
}

void jsopen_sdl(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jsdl_set_app_metadata, "set_app_metadata", 3);   js_setproperty(J, -2, "set_app_metadata");
        js_newcfunction(J, jsdl_get_version, "get_version", 0);             js_setproperty(J, -2, "get_version");
        js_newcfunction(J, jsdl_log, "log", 1);                             js_setproperty(J, -2, "log");
        js_newcfunction(J, jsdl_error, "error", 1);                         js_setproperty(J, -2, "error");
        js_newcfunction(J, jsdl_delay, "delay", 1);                         js_setproperty(J, -2, "delay");
        js_newcfunction(J, jsdl_get_ticks, "get_ticks", 0);                 js_setproperty(J, -2, "get_ticks");
        js_newcfunction(J, jsdl_get_mouse_state, "get_mouse_state", 0);     js_setproperty(J, -2, "get_mouse_state");
        js_newcfunction(J, jsdl_load_file, "load_file", 1);                 js_setproperty(J, -2, "load_file");
        js_newcfunction(J, jsdl_get_current_display_mode, "get_current_display_mode", 1); js_setproperty(J, -2, "get_current_display_mode");
        js_setglobal(J, "sdl");
}
