#include "luaclib.h"
#include "SDL3/SDL.h"
#include "../external/lua/lauxlib.h"

static int l_quit(lua_State* L) {
    (void)L;
    SDL_Event quit;
    SDL_zero(quit);
    quit.type = SDL_EVENT_QUIT;
    SDL_PushEvent(&quit);
    return 0;
}

int luaopen_event(lua_State* L) {
    luaL_checkversion(L);
    luaL_Reg l[] = {
        {"quit", l_quit},
        {NULL, NULL}
    };
    luaL_newlib(L, l);
    return 1;
}
