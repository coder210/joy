#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"

/* ===== c2s (空模块) ===== */
int luaopen_c2s(lua_State* L)
{
    luaL_checkversion(L);
    luaL_Reg l[] = {
        {NULL, NULL},
    };
    luaL_newlib(L, l);
    return 1;
}

/* ===== s2c (空模块) ===== */
int luaopen_s2c(lua_State* L)
{
    luaL_checkversion(L);
    luaL_Reg l[] = {
        {NULL, NULL},
    };
    luaL_newlib(L, l);
    return 1;
}

/* ===== client (空模块) ===== */
int luaopen_client(lua_State* L)
{
    luaL_checkversion(L);
    luaL_Reg l[] = {
        {NULL, NULL},
    };
    luaL_newlib(L, l);
    return 1;
}

/* ===== server (空模块) ===== */
int luaopen_server(lua_State* L)
{
    luaL_checkversion(L);
    luaL_Reg l[] = {
        {NULL, NULL},
    };
    luaL_newlib(L, l);
    return 1;
}
