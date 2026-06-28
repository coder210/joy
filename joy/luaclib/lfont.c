#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../joy.h"
#include "luaclib.h"

/* =================================================================
 * Font
 * ================================================================= */
static int lf_create(lua_State* L) {
        const char* font_path = luaL_checkstring(L, 1);
        font_p f = font_create(font_path, (int)luaL_checkinteger(L, 2));
        lua_pushlightuserdata(L, f);
        return 1;
}

static int lf_destroy(lua_State* L) {
        font_p f = (font_p)lua_touserdata(L, 1);
        if (f) font_destroy(f);
        return 0;
}

int luaopen_font(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
            {"create",   lf_create},
            {"destroy",  lf_destroy},
            {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
