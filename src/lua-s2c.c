#include <SDL3/SDL.h>
#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"
#include "liv-log.h"
#include "liv-proto.h"
#include "luaclib.h"

static int l_s2c_serialize_loading(lua_State* L)
{
        s2c_t s2c;
        const char* data;
        char buf[LIVNET_MAX_BUFFER];
        size_t sz;
        int len;

        s2c.cmd = S2C_CMD_LOADING;
        s2c.loading.frame_id = (int32_t)luaL_checkinteger(L, 1);
        s2c.loading.conv = (int32_t)luaL_checkinteger(L, 2);
        data = luaL_checklstring(L, 3, &sz);
        SDL_memcpy(s2c.loading.data, data, sz);
        s2c.loading.data_len = sz;
        s2c.loading.ok = lua_toboolean(L, 4);
        s2c_serialize(&s2c, buf, &len);
        lua_pushlstring(L, buf, len);
        return 1;
}

static int l_s2c_serialize_command(lua_State* L)
{
        s2c_t s2c;
        char buf[LIVNET_MAX_BUFFER];
        char* data;
        size_t sz;
        int len, table_len;
        s2c_player_input_t player_input;
        s2c_player_join_t player_join;
        s2c_player_leave_t player_leave;

        SDL_assert(lua_istable(L, 2));
        SDL_assert(lua_istable(L, 3));
        SDL_assert(lua_istable(L, 4));

        s2c.cmd = S2C_CMD_COMMAND;
        s2c.command.frame_id = (int32_t)luaL_checkinteger(L, 1);
        table_len = (int)luaL_len(L, 2);
        for (int i = 0; i < table_len; i++) {
                lua_rawgeti(L, 2, i);
                lua_getfield(L, -1, "conv");
                player_join.conv = (int)lua_tointeger(L, -1);
                lua_pop(L, 1);
                lua_getfield(L, -1, "position_x");
                player_join.position_x = lua_tointeger(L, -1);
                lua_pop(L, 1);
                lua_getfield(L, -1, "position_y");
                player_join.position_x = lua_tointeger(L, -1);
                lua_pop(L, 1);
                kv_push(s2c_player_join_t, s2c.command.player_joins, player_join);
        }

        table_len = (int)luaL_len(L, 3);
        for (int i = 0; i < table_len; i++) {
                lua_rawgeti(L, 3, i);
                lua_getfield(L, -1, "conv");
                player_leave = (int)lua_tointeger(L, -1);
                lua_pop(L, 1);
                kv_push(s2c_player_leave_t, s2c.command.player_leaves, player_leave);
        }

        table_len = (int)luaL_len(L, 4);
        for (int i = 0; i < table_len; i++) {
                lua_rawgeti(L, 4, i);
                lua_getfield(L, -1, "conv");
                player_input.conv = (int)lua_tointeger(L, -1);
                lua_pop(L, 1);
                lua_getfield(L, -1, "keycode");
                player_input.keycode = (int16_t)lua_tointeger(L, -1);
                lua_pop(L, 1);
                kv_push(s2c_player_input_t, s2c.command.player_inputs, player_input);
        }

        s2c_serialize(&s2c, buf, &len);
        lua_pushlstring(L, buf, len);
        return 1;
}

static int l_s2c_deserialize(lua_State* L)
{
        s2c_t s2c;
        s2c_player_join_t player_join;
        s2c_player_input_t player_input;
        s2c_creating_emeny_t creating_emeny;
        size_t len;
        const char* buf;
        int player_leave;

        buf = luaL_checklstring(L, 1, &len);
        s2c_deserialize(&s2c, buf, len);

        lua_newtable(L);
        lua_pushinteger(L, s2c.cmd);
        lua_setfield(L, -2, "cmd");
        if (s2c.cmd == S2C_CMD_LOADING) {
                lua_pushinteger(L, s2c.loading.frame_id);
                lua_setfield(L, -2, "frame_id");
                lua_pushinteger(L, s2c.loading.conv);
                lua_setfield(L, -2, "conv");
                lua_pushlstring(L, s2c.loading.data, s2c.loading.data_len);
                lua_setfield(L, -2, "data");
                lua_pushboolean(L, s2c.loading.ok);
                lua_setfield(L, -2, "ok");
        }
        else if (s2c.cmd == S2C_CMD_COMMAND) {
                lua_pushinteger(L, s2c.command.frame_id);
                lua_setfield(L, -2, "frame_id");

                lua_newtable(L);
                for (int i = 0; i < kv_size(s2c.command.player_joins); i++) {
                        player_join = kv_A(s2c.command.player_joins, i);
                        lua_newtable(L);
                        lua_pushinteger(L, player_join.conv);
                        lua_setfield(L, -2, "conv");
                        lua_pushinteger(L, player_join.position_x);
                        lua_setfield(L, -2, "position_x");
                        lua_pushinteger(L, player_join.position_y);
                        lua_setfield(L, -2, "position_y");
                        lua_rawseti(L, -2, i + 1);
                }
                lua_setfield(L, -2, "player_joins");

                lua_newtable(L);
                for (int i = 0; i < kv_size(s2c.command.player_leaves); i++) {
                        player_leave = kv_A(s2c.command.player_leaves, i);
                        lua_newtable(L);
                        lua_pushinteger(L, player_leave);
                        lua_setfield(L, -2, "conv");
                        lua_rawseti(L, -2, i + 1);
                        //lua_pushinteger(L, player_leave);
                        //lua_rawseti(L, -2, i + 1);
                }
                lua_setfield(L, -2, "player_leaves");

                lua_newtable(L);
                for (int i = 0; i < kv_size(s2c.command.player_inputs); i++) {
                        player_input = kv_A(s2c.command.player_inputs, i);
                        lua_newtable(L);
                        lua_pushinteger(L, player_input.conv);
                        lua_setfield(L, -2, "conv");
                        lua_pushinteger(L, player_input.keycode);
                        lua_setfield(L, -2, "keycode");
                        lua_rawseti(L, -2, i + 1);
                }
                lua_setfield(L, -2, "player_inputs");

                lua_newtable(L);
                for (int i = 0; i < kv_size(s2c.command.creating_emenies); i++) {
                        creating_emeny = kv_A(s2c.command.creating_emenies, i);
                        lua_newtable(L);
                        lua_pushinteger(L, creating_emeny.width);
                        lua_setfield(L, -2, "width");
                        lua_pushinteger(L, creating_emeny.height);
                        lua_setfield(L, -2, "height");
                        lua_pushinteger(L, creating_emeny.linear_velocity_x);
                        lua_setfield(L, -2, "linear_velocity_x");
                        lua_pushinteger(L, creating_emeny.linear_velocity_y);
                        lua_setfield(L, -2, "linear_velocity_y");
                        lua_pushinteger(L, creating_emeny.position_x);
                        lua_setfield(L, -2, "position_x");
                        lua_pushinteger(L, creating_emeny.position_y);
                        lua_setfield(L, -2, "position_y");
                        lua_rawseti(L, -2, i + 1);
                }
                lua_setfield(L, -2, "creating_emenies");

                lua_pushlstring(L, s2c.command.checksum,   s2c.command.checksum_len);
                lua_setfield(L, -2, "checksum");
        }
        return 1;
}

static int l_s2c_cmd_is_loading(lua_State* L)
{
        s2c_cmd_k cmd;
        cmd = (s2c_cmd_k)luaL_checkinteger(L, 1);
        lua_pushboolean(L, cmd == S2C_CMD_LOADING);
        return 1;
}

static int l_s2c_cmd_is_command(lua_State* L)
{
        s2c_cmd_k cmd;
        cmd = (s2c_cmd_k)luaL_checkinteger(L, 1);
        lua_pushboolean(L, cmd == S2C_CMD_COMMAND);
        return 1;
}

int luaopen_s2c(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                {"serialize_loading", l_s2c_serialize_loading},
                {"serialize_command", l_s2c_serialize_command},
                {"deserialize", l_s2c_deserialize},
                {"is_loading", l_s2c_cmd_is_loading},
                {"is_command", l_s2c_cmd_is_command},
                {NULL, NULL},
        };
        luaL_newlib(L, l);
        return 1;
}