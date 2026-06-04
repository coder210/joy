#include "jsclib.h"
#include <SDL3/SDL.h>

static struct {
        const char *name;
        SDL_Scancode scancode;
} scancodes[] = {
    {"A",       SDL_SCANCODE_A},
    {"Q",       SDL_SCANCODE_Q},
    {"ESCAPE",  SDL_SCANCODE_ESCAPE},
    {"SPACE",   SDL_SCANCODE_SPACE},
    {"RIGHT",   SDL_SCANCODE_RIGHT},
    {"UP",      SDL_SCANCODE_UP},
    {"LEFT",    SDL_SCANCODE_LEFT},
    {"DOWN",    SDL_SCANCODE_DOWN},
};

static int scancodes_count = sizeof(scancodes) / sizeof(scancodes[0]);

static SDL_Scancode find_scancode(const char *name)
{
        for (int i = 0; i < scancodes_count; i++) {
                if (SDL_strcmp(scancodes[i].name, name) == 0)
                        return scancodes[i].scancode;
        }
        return SDL_SCANCODE_UNKNOWN;
}

static void jkeyboard_is_down(js_State *J)
{
        const char *name = js_tostring(J, 1);
        const bool *state = SDL_GetKeyboardState(NULL);
        SDL_Scancode sc = find_scancode(name);
        js_pushboolean(J, sc != SDL_SCANCODE_UNKNOWN && state[sc]);
}

static void jkeyboard_is_up(js_State *J)
{
        const char *name = js_tostring(J, 1);
        const bool *state = SDL_GetKeyboardState(NULL);
        SDL_Scancode sc = find_scancode(name);
        js_pushboolean(J, sc == SDL_SCANCODE_UNKNOWN || !state[sc]);
}

void jsopen_keyboard(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jkeyboard_is_down, "is_down", 1); js_setproperty(J, -2, "is_down");
        js_newcfunction(J, jkeyboard_is_up, "is_up", 1);     js_setproperty(J, -2, "is_up");
        js_setglobal(J, "keyboard");
}
