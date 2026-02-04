#include "luaclib.h"
#include "SDL3/SDL.h"
#include "external/lua/lapi.h"
#include "external/lua/lauxlib.h"


static struct {
        const char* name;
        SDL_Scancode scancode;
} scancodes[] = {
    {"A", SDL_SCANCODE_A},
    {"Q", SDL_SCANCODE_Q},
    {"ESCAPE", SDL_SCANCODE_ESCAPE},
    {"SPACE", SDL_SCANCODE_SPACE},
    {"RIGHT", SDL_SCANCODE_RIGHT},
    {"UP", SDL_SCANCODE_UP},
    {"LEFT", SDL_SCANCODE_LEFT},
    {"DOWN", SDL_SCANCODE_DOWN},
};

static const int scancodes_count = sizeof(scancodes) / sizeof(scancodes[0]);

static SDL_Scancode 
find_scancode(const char* name)
{
        for (int i = 0; i < scancodes_count; i++) {
                if (SDL_strcmp(scancodes[i].name, name) == 0) {
                        return scancodes[i].scancode;
                }
        }
        return SDL_SCANCODE_UNKNOWN;
}

static int l_keyboard_is_keydown(lua_State* L)
{
        SDL_Event* event = (SDL_Event*)lua_touserdata(L, 1);
        lua_pushboolean(L, event->type == SDL_EVENT_KEY_DOWN);
        return 1;
}

static int l_keyboard_is_down(lua_State* L)
{
        const char *key;
        SDL_Event* event;
        event = (SDL_Event*)lua_touserdata(L, 1);
        key = luaL_checkstring(L, 2);
        if (event->type == SDL_EVENT_KEY_DOWN) {
                if (event->key.scancode == find_scancode(key)) {
                        lua_pushboolean(L, true);
                }
                else {
                        lua_pushboolean(L, false);
                }
        }
        else {
                lua_pushboolean(L, false);
        }
        return 1;
}

static int l_keyboard_is_keyup(lua_State* L)
{
        SDL_Event* event = (SDL_Event*)lua_touserdata(L, 1);
        lua_pushboolean(L, event->type == SDL_EVENT_KEY_UP);
        return 1;
}

static int l_keyboard_is_up(lua_State* L)
{
        const char* key;
        SDL_Event* event;
        event = (SDL_Event*)lua_touserdata(L, 1);
        key = luaL_checkstring(L, 2);
        if (event->type == SDL_EVENT_KEY_UP) {
                if (event->key.scancode == find_scancode(key)) {
                        lua_pushboolean(L, true);
                }
                else {
                        lua_pushboolean(L, false);
                }
        }
        else {
                lua_pushboolean(L, false);
        }
        return 1;

}

int luaopen_keyboard(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
            {"is_keydown", l_keyboard_is_keydown},
            {"is_down", l_keyboard_is_down},
            {"is_keyup", l_keyboard_is_keyup},
            {"is_up", l_keyboard_is_up},
            {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}