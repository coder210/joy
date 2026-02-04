#include "luaclib.h"
#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"
#include "external/klib/khash.h"
#include <SDL3/SDL.h>
#include "liv-log.h"
#include "liv-math.h"
#include "liv-proto.h"

typedef enum {
        KEYCODE_NONE = 0x0,
        KEYCODE_UP = 0x1,
        KEYCODE_DOWN = 0x2,
        KEYCODE_LEFT = 0x3,
        KEYCODE_RIGHT = 0x4,
        KEYCODE_ENTER = 0xD,
        KEYCODE_ESC = 0x1B,
        KEYCODE_SPACE = 0x20,
}keycode_k;


typedef struct connection {
        int health;
        int frameid;
        int conv;
        bool ready;
        int pointer;
        char* arr;
        int keycode;
}connection_t, * connection_p;

typedef struct frame {
        char* data;
        int len;
}frame_t, * frame_p;

KHASH_INIT(kconn, int, connection_p, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(kcommand, int, frame_t, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(kworld, int, frame_t, 1, kh_int_hash_func, kh_int_hash_equal)

typedef struct lockstep_server {
        int frameid;
        khash_t(kconn)* conns;
        khash_t(kcommand)* commands;
        khash_t(kworld)* worlds;
        s2c_command_t command;
}lockstep_server_t, * lockstep_server_p;

static int l_server_create(lua_State* L)
{
        lockstep_server_p ctx;
        ctx = (lockstep_server_p)SDL_malloc(sizeof(lockstep_server_t));
        ctx->frameid = 1;
        ctx->conns = kh_init(kconn);
        ctx->commands = kh_init(kcommand);
        ctx->worlds = kh_init(kworld);
        kv_init(ctx->command.player_joins);
        kv_init(ctx->command.player_leaves);
        kv_init(ctx->command.player_inputs);
        kv_init(ctx->command.creating_emenies);
        lua_pushlightuserdata(L, ctx);
        return 1;
}

static int l_server_destroy(lua_State* L)
{
        lockstep_server_p ctx;
        ctx = lua_touserdata(L, 1);
        frame_t command, world;
        khint_t k;
        for (k = kh_begin(ctx->commands); k != kh_end(ctx->commands); k++) {
                if (kh_exist(ctx->commands, k)) {
                        command = kh_val(ctx->commands, k);
                        SDL_free(command.data);
                }
        }
        for (k = kh_begin(ctx->worlds); k != kh_end(ctx->worlds); k++) {
                if (kh_exist(ctx->worlds, k)) {
                        world = kh_val(ctx->worlds, k);
                        SDL_free(world.data);
                }
        }
        kh_destroy(kcommand, ctx->commands);
        kh_destroy(kworld, ctx->worlds);
        kh_destroy(kconn, ctx->conns);
        SDL_free(ctx);
        return 0;
}

static int
handle_cmd_ready(lockstep_server_p ctx, int conv, char* data)
{
        khint_t k;
        connection_p conn;
        frame_t world;
        s2c_t s2c;
        int ret, len;
        const int size_of_fragment = 256;

        liv_log_info("cmd ready %d", conv);
        k = kh_get(kconn, ctx->conns, conv);
        if (k != kh_end(ctx->conns)) {
                liv_log_info("cmd ready %d conn is exists.", conv);
                return 0;
        }

        conn = (connection_p)SDL_malloc(sizeof(connection_t));
        conn->health = 10;
        conn->conv = conv;
        conn->pointer = 0;
        conn->keycode = KEYCODE_NONE;
        k = kh_put(kconn, ctx->conns, conv, &ret);
        kh_val(ctx->conns, k) = conn;

        if (ctx->frameid == 1) {
                conn->ready = true;
                conn->frameid = ctx->frameid;
                s2c.cmd = S2C_CMD_LOADING;
                s2c.loading.frame_id = conn->frameid;
                s2c.loading.conv = conn->conv;
                s2c.loading.ok = conn->ready;
                s2c.loading.data_len = 0;
                s2c_serialize(&s2c, data, &len);
                return len;
        }
        else {
                conn->ready = false;
                conn->frameid = ctx->frameid - 1;
                k = kh_get(kworld, ctx->worlds, conn->frameid);
                if (k == kh_end(ctx->worlds)) {
                        liv_log_error("not frameid=%d", conn->frameid);
                        return 0;
                }
                world = kh_val(ctx->worlds, k);
                if (world.len > size_of_fragment) {
                        SDL_memcpy(s2c.loading.data, world.data, size_of_fragment);
                        s2c.loading.data_len = size_of_fragment;
                        conn->pointer = size_of_fragment;
                }
                else {
                        SDL_memcpy(s2c.loading.data, world.data, world.len);
                        s2c.loading.data_len = world.len;
                        conn->pointer = world.len;
                        conn->ready = true;
                }

                s2c.cmd = S2C_CMD_LOADING;
                s2c.loading.frame_id = conn->frameid;
                s2c.loading.conv = conn->conv;
                s2c.loading.ok = conn->ready;
                s2c_serialize(&s2c, data, &len);
                data[len] = 0;
                return len;
        }
}

static int
handle_cmd_heartbeat(lockstep_server_p ctx, int conv)
{
        khint_t k;
        connection_p conn;
        k = kh_get(kconn, ctx->conns, conv);
        if (k == kh_end(ctx->conns)) {
                liv_log_debug("conn %d is not exists.", conv);
                return 0;
        }
        conn = kh_val(ctx->conns, k);
        conn->health = 10;
        liv_log_debug("conn %d heatbaeat", conv);
        return 0;
}

static int
handle_cmd_loading(lockstep_server_p ctx, int conv, char* data)
{
        khint_t k;
        connection_p conn;
        frame_t world;
        s2c_t s2c;
        int frameid, ret, len;
        const int size_of_fragment = 256;

        liv_log_info("cmd loading %d", conv);
        k = kh_get(kconn, ctx->conns, conv);
        if (k == kh_end(ctx->conns)) {
                liv_log_debug("conn %d is not exists.", conv);
                return 0;
        }

        conn = kh_val(ctx->conns, k);
        if (conn->ready) {
                liv_log_debug("conn %d is ready.", conv);
                return 0;
        }

        k = kh_get(kworld, ctx->worlds, conn->frameid);
        if (k == kh_end(ctx->worlds)) {
                liv_log_error("%s: frameid=%d", "not current world", conn->frameid);
                return 0;
        }

        world = kh_val(ctx->worlds, k);
        if (world.len > conn->pointer + size_of_fragment) {
                s2c.loading.data_len = size_of_fragment;
        }
        else {
                s2c.loading.data_len = world.len - conn->pointer;
                conn->ready = true;
        }

        SDL_memcpy(s2c.loading.data, world.data + conn->pointer, size_of_fragment);
        conn->pointer += s2c.loading.data_len;

        s2c.cmd = S2C_CMD_LOADING;
        s2c.loading.conv = conn->conv;
        s2c.loading.ok = conn->ready;
        s2c_serialize(&s2c, data, &len);
        return len;
}

static int
handle_cmd_player_join(lockstep_server_p ctx, int conv, vec2f_t position)
{
        s2c_player_join_t player_join;
        player_join.conv = conv;
        player_join.position_x = position.x;
        player_join.position_y = position.y;
        kv_push(s2c_player_join_t, ctx->command.player_joins, player_join);
        return 0;
}

static int
handle_cmd_player_leave(lockstep_server_p ctx, int conv)
{
        kv_push(s2c_player_leave_t, ctx->command.player_leaves, conv);
        return 0;
}

static int
handle_cmd_player_input(lockstep_server_p ctx, int conv, int keycode)
{
        /* 判断是否已经有输入 */
        /*s2c_player_input_p found_player_input;
        s2c_player_input_t player_input;
        for (int i = 0; i < kv_size(ctx->command.player_inputs); i++) {
                found_player_input = &kv_A(ctx->command.player_inputs, i);
                 if (found_player_input->conv == conv) {
                         found_player_input->keycode = keycode;
                         return;
                 }
        }

        player_input.conv = conv;
        player_input.keycode = keycode;
        kv_push(s2c_player_input_t, ctx->command.player_inputs, player_input);*/

        khint_t k;
        connection_p conn;
        k = kh_get(kconn, ctx->conns, conv);
        if (k != kh_end(ctx->conns)) {
                conn = kh_val(ctx->conns, k);
                conn->keycode = keycode;
        }
        return 0;
}

static int
server_create_enemy(lockstep_server_p ctx, fp_t w, fp_t h,
        fp_t linear_velocity_x, fp_t linear_velocity_y,
        fp_t position_x, fp_t position_y)
{
        if (ctx->frameid <= 1) {
                return 0;
        }
        s2c_creating_emeny_t creating_emeny;
        creating_emeny.width = w;
        creating_emeny.height = h;
        creating_emeny.linear_velocity_x = linear_velocity_x;
        creating_emeny.linear_velocity_y = linear_velocity_y;
        creating_emeny.position_x = position_x;
        creating_emeny.position_y = position_y;
        kv_push(s2c_creating_emeny_t, ctx->command.creating_emenies, creating_emeny);
        return 0;
}

static int l_server_handle_cmd_ready(lua_State* L)
{
        char data[LIVNET_MAX_BUFFER];
        lockstep_server_p ctx;
        int conv, len;
        ctx = lua_touserdata(L, 1);
        conv = luaL_checkinteger(L, 2);
        len = handle_cmd_ready(ctx, conv, data);
        lua_pushlstring(L, data, len);
        return 1;
}

static int l_server_handle_cmd_loading(lua_State* L)
{
        char data[LIVNET_MAX_BUFFER];
        lockstep_server_p ctx;
        int conv, len;
        ctx = (lockstep_server_p)lua_touserdata(L, 1);
        conv = luaL_checkinteger(L, 2);
        len = handle_cmd_loading(ctx, conv, data);
        lua_pushlstring(L, data, len);
        return 1;
}

static int l_server_handle_cmd_heartbeat(lua_State* L)
{
        lockstep_server_p ctx;
        int conv;
        ctx = lua_touserdata(L, 1);
        conv = luaL_checkinteger(L, 2);
        handle_cmd_heartbeat(ctx, conv);
        return 0;
}

static int l_server_handle_player_join(lua_State* L)
{
        lockstep_server_p ctx;
        vec2f_t position;
        int conv;
        ctx = (lockstep_server_p)lua_touserdata(L, 1);
        conv = luaL_checkinteger(L, 2);
        position.x = luaL_checkinteger(L, 3);
        position.y = luaL_checkinteger(L, 4);
        handle_cmd_player_join(ctx, conv, position);
        return 0;
}

static int l_server_handle_player_leave(lua_State* L)
{
        lockstep_server_p ctx;
        int conv;
        ctx = (lockstep_server_p)lua_touserdata(L, 1);
        conv = luaL_checkinteger(L, 2);
        handle_cmd_player_leave(ctx, conv);
        return 0;
}

static int l_server_handle_player_input(lua_State* L)
{
        lockstep_server_p ctx;
        int conv, keycode;
        ctx = (lockstep_server_p)lua_touserdata(L, 1);
        conv = luaL_checkinteger(L, 2);
        keycode = luaL_checkinteger(L, 3);
        handle_cmd_player_input(ctx, conv, keycode);
        return 0;
}

static int l_server_create_enemy(lua_State* L)
{
        lockstep_server_p ctx;
        fp_t w, h, position_x, position_y, liner_velocity_x, liner_velocity_y;
        ctx = (lockstep_server_p)lua_touserdata(L, 1);
        w = luaL_checkinteger(L, 2);
        h = luaL_checkinteger(L, 3);
        liner_velocity_x = luaL_checkinteger(L, 4);
        liner_velocity_y = luaL_checkinteger(L, 5);
        position_x = luaL_checkinteger(L, 6);
        position_y = luaL_checkinteger(L, 7);
        server_create_enemy(ctx, w, h, liner_velocity_x, liner_velocity_y, position_x, position_y);
        return 0;
}


static int l_server_has_command(lua_State* L)
{
        lockstep_server_p ctx;
        int count;
        ctx = (lockstep_server_p)lua_touserdata(L, 1);
        count = kv_size(ctx->command.player_inputs);
        count += kv_size(ctx->command.player_joins);
        count += kv_size(ctx->command.player_leaves);
        count += kv_size(ctx->command.creating_emenies);
        lua_pushboolean(L, count > 0);
        return 1;
}

static int l_server_collect_command(lua_State* L)
{
        frame_t command;
        khint_t k;
        char data[LIVNET_MAX_BUFFER];
        int len, ret, count;
        size_t world_payload_size, world_checksum_size;
        lockstep_server_p ctx;
        connection_p conn;
        s2c_t s2c;
        s2c_player_input_t player_input;
        frame_t new_world;
        const char* world_payload;
        const char* world_checksum;

        ctx = (lockstep_server_p)lua_touserdata(L, 1);
        world_payload = luaL_checklstring(L, 2, &world_payload_size);
        world_checksum = luaL_checklstring(L, 3, &world_checksum_size);

        /* 补帧 */
        for (k = kh_begin(ctx->conns); k != kh_end(ctx->conns); k++) {
                if (kh_exist(ctx->conns, k)) {
                        conn = kh_val(ctx->conns, k);
                        if (conn->ready) {
                                player_input.conv = conn->conv;
                                player_input.keycode = conn->keycode;
                                kv_push(s2c_player_input_t, ctx->command.player_inputs, player_input);
                                conn->keycode = KEYCODE_NONE;
                        }
                }
        }

        count = kv_size(ctx->command.player_inputs);
        count += kv_size(ctx->command.player_joins);
        count += kv_size(ctx->command.player_leaves);
        count += kv_size(ctx->command.creating_emenies);
        if (count > 0) {
                s2c.cmd = S2C_CMD_COMMAND;
                ctx->command.checksum_len = world_checksum_size;
                SDL_memcpy(ctx->command.checksum, world_checksum, world_checksum_size);

                s2c.command = ctx->command;
                s2c.command.frame_id = ctx->frameid++;
                s2c_serialize(&s2c, data, &len);
                k = kh_get(kcommand, ctx->commands, s2c.command.frame_id);
                if (k == kh_end(ctx->commands)) {
                        k = kh_put(kcommand, ctx->commands, s2c.command.frame_id, &ret);
                        if (ret != 0) {
                                command.len = len;
                                command.data = (char*)SDL_malloc(sizeof(char) * len);
                                SDL_memcpy(command.data, data, len);
                                kh_val(ctx->commands, k) = command;
                        }
                }
                ctx->command.player_joins.n = 0;
                ctx->command.player_leaves.n = 0;
                ctx->command.player_inputs.n = 0;
                ctx->command.creating_emenies.n = 0;

                k = kh_get(kworld, ctx->worlds, s2c.command.frame_id);
                if (k == kh_end(ctx->worlds)) {
                        new_world.len = world_payload_size;
                        new_world.data = (char*)SDL_malloc(sizeof(char) * new_world.len);
                        SDL_memcpy(new_world.data, world_payload, new_world.len);
                        k = kh_put(kworld, ctx->worlds, s2c.command.frame_id, &ret);
                        kh_val(ctx->worlds, k) = new_world;
                }

                lua_pushlstring(L, data, len);
        }
        else {
                s2c.cmd = S2C_CMD_COMMAND;
                ctx->command.checksum_len = world_checksum_size;
                SDL_memcpy(ctx->command.checksum, world_checksum, world_checksum_size);

                s2c.command = ctx->command;
                s2c.command.frame_id = ctx->frameid++;
                s2c_serialize(&s2c, data, &len);
                k = kh_get(kcommand, ctx->commands, s2c.command.frame_id);
                if (k == kh_end(ctx->commands)) {
                        k = kh_put(kcommand, ctx->commands, s2c.command.frame_id, &ret);
                        if (ret != 0) {
                                command.len = len;
                                command.data = (char*)SDL_malloc(sizeof(char) * len);
                                SDL_memcpy(command.data, data, len);
                                kh_val(ctx->commands, k) = command;
                        }
                }
                ctx->command.player_joins.n = 0;
                ctx->command.player_leaves.n = 0;
                ctx->command.player_inputs.n = 0;
                ctx->command.creating_emenies.n = 0;

                k = kh_get(kworld, ctx->worlds, s2c.command.frame_id);
                if (k == kh_end(ctx->worlds)) {
                        new_world.len = world_payload_size;
                        new_world.data = (char*)SDL_malloc(sizeof(char) * new_world.len);
                        SDL_memcpy(new_world.data, world_payload, new_world.len);
                        k = kh_put(kworld, ctx->worlds, s2c.command.frame_id, &ret);
                        kh_val(ctx->worlds, k) = new_world;
                }

                lua_pushlstring(L, data, len);
        }

        return 1;
}

static int l_server_connections_count(lua_State* L)
{
        int num;
        lockstep_server_p ctx;
        ctx = (lockstep_server_p)lua_touserdata(L, 1);
        num = kh_size(ctx->conns);
        lua_pushinteger(L, num);
        return 1;
}

static int l_server_remove_connection(lua_State* L)
{
        khint_t k;
        lockstep_server_p ctx;
        int conv;
        ctx = (lockstep_server_p)lua_touserdata(L, 1);
        conv = luaL_checkinteger(L, 2);
        k = kh_get(kconn, ctx->conns, conv);
        if (k != kh_end(ctx->conns)) {
                kh_del(kconn, ctx->conns, conv);
                lua_pushboolean(L, true);
        }
        else {
                lua_pushboolean(L, false);
        }

        return 1;
}

static int l_server_sync(lua_State* L)
{
        khint_t k, p;
        connection_p conn;
        frame_t frame;
        lockstep_server_p ctx;
        ctx = (lockstep_server_p)lua_touserdata(L, 1);
        luaL_checktype(L, 2, LUA_TFUNCTION);
        lua_pushvalue(L, 2);
        int callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);

        lua_pushinteger(L, callback_ref);

        for (k = kh_begin(ctx->conns); k != kh_end(ctx->conns); k++) {
                if (kh_exist(ctx->conns, k)) {
                        conn = kh_val(ctx->conns, k);
                        if (conn->ready) {
                                p = kh_get(kcommand, ctx->commands, conn->frameid);
                                if (p != kh_end(ctx->commands)) {
                                        frame = kh_val(ctx->commands, p);
                                        lua_rawgeti(L, LUA_REGISTRYINDEX, callback_ref);
                                        lua_pushinteger(L, conn->conv);
                                        lua_pushlstring(L, frame.data, frame.len);
                                        if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
                                                liv_log_error(lua_tostring(L, -1));
                                                lua_pop(L, 1);  // 弹出错误信息
                                        }
                                        else {
                                                conn->frameid++;
                                        }
                                }
                        }
                }
        }

        luaL_unref(L, LUA_REGISTRYINDEX, callback_ref);
        return 0;
}

static int l_server_get_world(lua_State* L)
{
        lockstep_server_p ctx;
        int frameid;
        khint_t k;
        frame_t world;
        ctx = (lockstep_server_p)lua_touserdata(L, 1);
        frameid = luaL_checkinteger(L, 2);
        k = kh_get(kworld, ctx->worlds, frameid);
        if (k != kh_end(ctx->worlds)) {
                world = kh_val(ctx->worlds, k);
                lua_pushlstring(L, world.data, world.len);
        }
        else {
                lua_pushnil(L);
        }
        return 1;
}

static int l_server_get_frameid(lua_State* L)
{
        lockstep_server_p ctx;
        ctx = (lockstep_server_p)lua_touserdata(L, 1);
        lua_pushinteger(L, ctx->frameid);
        return 1;
}

int luaopen_server(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
            {"create", l_server_create},
            {"destroy", l_server_destroy},
            {"handle_cmd_ready", l_server_handle_cmd_ready},
            {"handle_cmd_loading", l_server_handle_cmd_loading},
            {"handle_cmd_heartbeat", l_server_handle_cmd_heartbeat},
            {"handle_player_join", l_server_handle_player_join},
            {"handle_player_leave", l_server_handle_player_leave},
            {"handle_player_input", l_server_handle_player_input},
            {"create_enemy", l_server_create_enemy},
            {"has_command", l_server_has_command},
            {"collect_command", l_server_collect_command},
            {"sync", l_server_sync},
            {"remove_connection", l_server_remove_connection},
            {"connections_count", l_server_connections_count},
            {"get_frame_id", l_server_get_frameid},
            {NULL, NULL},
        };
        luaL_newlib(L, l);
        return 1;
}

