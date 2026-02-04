#include <SDL3/SDL.h>
#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"
#include "liv-log.h"
#include "liv-proto.h"
#include "luaclib.h"

static int l_c2s_serialize_ready(lua_State* L)
{
        c2s_t c2s;
        int len;
        char buf[LIVNET_MAX_BUFFER];
        c2s.cmd = C2S_CMD_READY;
        c2s_serialize(&c2s, buf, &len);
        lua_pushlstring(L, buf, len);
        return 1;
}

static int l_c2s_serialize_loading(lua_State* L)
{
        c2s_t c2s;
        int len;
        char buf[LIVNET_MAX_BUFFER];
        c2s.cmd = C2S_CMD_LOADING;
        c2s_serialize(&c2s, buf, &len);
        lua_pushlstring(L, buf, len);
        return 1;
}

static int l_c2s_serialize_heartbeat(lua_State* L)
{
        c2s_t c2s;
        int len;
        char buf[LIVNET_MAX_BUFFER];
        c2s.cmd = C2S_CMD_HEARTBEAT;
        c2s_serialize(&c2s, buf, &len);
        lua_pushlstring(L, buf, len);
        return 1;
}

static int l_c2s_serialize_player_join(lua_State* L)
{
        c2s_t c2s;
        int len;
        char buf[LIVNET_MAX_BUFFER];
        c2s.cmd = C2S_CMD_PLAYER_JOIN;
        c2s.player_join.position_x = (int64_t)luaL_checkinteger(L, 1);
        c2s.player_join.position_y = (int64_t)luaL_checkinteger(L, 2);
        c2s_serialize(&c2s, buf, &len);
        lua_pushlstring(L, buf, len);
        return 1;
}

static int l_c2s_serialize_player_leave(lua_State* L)
{
        c2s_t c2s;
        int len;
        char buf[LIVNET_MAX_BUFFER];
        c2s.cmd = C2S_CMD_PLAYER_LEAVE;
        c2s.player_leave = (int64_t)luaL_checkinteger(L, 1);
        c2s_serialize(&c2s, buf, &len);
        lua_pushlstring(L, buf, len);
        return 1;
}

static int l_c2s_serialize_player_input(lua_State* L)
{
        c2s_t c2s;
        int len;
        char buf[LIVNET_MAX_BUFFER];
        c2s.cmd = C2S_CMD_PLAYER_INPUT;
        c2s.player_input.keycode = (int16_t)luaL_checkinteger(L, 1);
        c2s_serialize(&c2s, buf, &len);
        lua_pushlstring(L, buf, len);
        return 1;
}

static int l_c2s_deserialize(lua_State* L)
{
        c2s_t c2s;
        size_t len;
        const char* buf;

        buf = luaL_checklstring(L, 1, &len);
        c2s_deserialize(&c2s, buf, len);

        lua_newtable(L);
        lua_pushinteger(L, c2s.cmd);
        lua_setfield(L, -2, "cmd");
        if (c2s.cmd == C2S_CMD_PLAYER_JOIN) {
                lua_pushinteger(L, c2s.player_join.position_x);
                lua_setfield(L, -2, "position_x");
                lua_pushinteger(L, c2s.player_join.position_y);
                lua_setfield(L, -2, "position_y");
        }
        else if (c2s.cmd == C2S_CMD_PLAYER_LEAVE) {
                lua_pushinteger(L, c2s.player_leave);
                lua_setfield(L, -2, "conv");
        }
        else if (c2s.cmd == C2S_CMD_PLAYER_INPUT) {
                lua_pushinteger(L, c2s.player_input.keycode);
                lua_setfield(L, -2, "keycode");
        }
        return 1;
}


int luaopen_c2s(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                {"serialize_ready", l_c2s_serialize_ready},
                {"serialize_loading", l_c2s_serialize_loading},
                {"serialize_heartbeat", l_c2s_serialize_heartbeat},
                {"serialize_player_join", l_c2s_serialize_player_join},
                {"serialize_player_leave", l_c2s_serialize_player_leave},
                {"serialize_player_input", l_c2s_serialize_player_input},
                {"deserialize", l_c2s_deserialize},
                {NULL, NULL},
        };
        luaL_newlib(L, l);
        return 1;
}