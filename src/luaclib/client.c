#include <SDL3/SDL.h>
#include "luaclib.h"
#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../external/klib/khash.h"
#include "../core/log.h"
#include "../core/proto.h"
#include "../core/mathx.h"
#include "../core/physics.h"

typedef struct player_input {
	int conv;
	int keycode;
}player_input_t, * player_input_p;

typedef struct frame {
	char* data;
	int len;
}frame_t, * frame_p;

KHASH_INIT(kworld, int, frame_t, 1, kh_int_hash_func, kh_int_hash_equal)

/*
key: conv,
*/
KHASH_INIT(kplayerinput, int, player_input_t, 1, kh_int_hash_func, kh_int_hash_equal)


/*
key:frameid
value: server_buffer
*/
KHASH_INIT(ksbuffer, int, khash_t(kplayerinput)*, 1, kh_int_hash_func, kh_int_hash_equal)


typedef struct lockstep_client {
	bool ready;
	char buf[JOY_MAX_BUFFER * 1024 * 2];
	int saving_cursor; /* 保存的字节数 */
	int local_frameid;
	int server_frameid;
	khash_t(kworld)* worlds;  /* 服务器的帧world */
	khash_t(kplayerinput)* local_buffers; /* 本地预测输入 */
	//khash_t(ksbuffer)* server_buffers; /* 服务端的输入 */
}lockstep_client_t, * lockstep_client_p;

static int l_client_create(lua_State* L)
{
	lockstep_client_p client;
	client = (lockstep_client_p)SDL_malloc(sizeof(lockstep_client_t));
	SDL_assert(client);
	client->ready = false;
	client->saving_cursor = 0;
	client->local_frameid = 0;
	client->server_frameid = 0;
	client->worlds = kh_init(kworld);
	client->local_buffers = kh_init(kplayerinput);
	//client->server_buffers = kh_init(ksbuffer);
	lua_pushlightuserdata(L, client);
	return 1;
}

static int l_client_destroy(lua_State* L)
{
	lockstep_client_p client;
	client = lua_touserdata(L, 1);
	kh_destroy(kworld, client->worlds);
	kh_destroy(kplayerinput, client->local_buffers);
	//kh_destroy(ksbuffer, client->server_buffers);
	SDL_free(client);
	return 0;
}

static int l_client_cmd_loading1(lua_State* L)
{
	lockstep_client_p client;
	khint_t k;
	frame_t frame;
	int conv, frame_id, ret;
	size_t len;
	const char* in_data;
	bool ok;
	world2df_p world2d;

	client = (lockstep_client_p)lua_touserdata(L, 1);
	world2d = (world2df_p)lua_touserdata(L, 2);
	ok = lua_toboolean(L, 3);
	in_data = luaL_tolstring(L, 4, &len);
	frame_id = luaL_checkinteger(L, 5);
	if (!client->ready) {
		client->ready = ok;
		if (len > 0) {
			/* 保存数据 */
			SDL_memcpy(client->buf + client->saving_cursor, in_data, len);
			client->saving_cursor += len;
			/* 初始化世界 */
			world2df_deserialize(world2d, client->buf, client->saving_cursor);

			/* 备份帧 */
		        /*
			frame.len = len;
			frame.data = (char*)SDL_malloc(sizeof(char) * frame.len);
			SDL_memcpy(frame.data, client->buf, frame.len);
			k = kh_put(ksbuffer, client->server_buffers, client->server_frameid, &ret);
			kh_val(client->server_buffers, k) = frame;*/

			/*{
				char output[JOY_MAX_BUFFER] = { 0 };
				int len = world2df_checksum(world2d, output);
				output[len] = 0;
				log_debug("output222222:%s\n", output);
			}*/
		}
	}

	if (client->ready) {
		client->server_frameid = frame_id;
		client->local_frameid = frame_id;
	}
	lua_pushboolean(L, client->ready);
	return 1;
}

static int l_client_create_player_join(lua_State* L)
{
	c2s_t c2s;
	char data[JOY_MAX_BUFFER];
	int len;
	c2s.cmd = C2S_CMD_PLAYER_JOIN;
	c2s.player_join.position_x = fp_from_float(luaL_checknumber(L, 1));
	c2s.player_join.position_y = fp_from_float(luaL_checknumber(L, 2));
	c2s_serialize(&c2s, data, &len);
	lua_pushlstring(L, data, len);
	return 1;
}

static int l_client_cmd_loading2(lua_State* L)
{
	lockstep_client_p client;
	int conv, data_len;
	size_t len;
	char out_data[JOY_MAX_BUFFER];
	const char* in_data;
	bool ok;
	int out_data_len;
	c2s_t c2s;

	client = (lockstep_client_p)lua_touserdata(L, 1);
	ok = lua_toboolean(L, 2);
	in_data = luaL_tolstring(L, 3, &len);
	if (!client->ready) {
		/* 保存数据 */
		SDL_memcpy(client->buf + client->saving_cursor, in_data, len);
		client->saving_cursor += len;
		/* 再次发送数据加载请求 */
		c2s.cmd = C2S_CMD_LOADING;
		c2s_serialize(&c2s, out_data, &out_data_len);
		lua_pushlstring(L, out_data, out_data_len);
	}
	else {
		lua_pushnil(L);
	}

	return 1;
}

static int l_client_cmd_playerjoin(lua_State* L)
{
	lockstep_client_p client;
	int conv;
	vec2f_t position;
	client = (lockstep_client_p)lua_touserdata(L, 1);
	conv = luaL_checkinteger(L, 2);
	position.x = luaL_checkinteger(L, 3);
	position.y = luaL_checkinteger(L, 4);
	lua_pushnil(L);
	return 1;
}

static int l_client_predict(lua_State* L)
{
	lockstep_client_p client;
	world2df_p world;
	client = (lockstep_client_p)lua_touserdata(L, 1);
	world = (world2df_p)lua_touserdata(L, 2);
        int start_frameid = client->server_frameid + 1;
	if (client->local_frameid > start_frameid) {
		for (int i = start_frameid; i < client->local_frameid; i++) {
			if (kh_exist(client->local_buffers, i)) {
				player_input_t player_input = kh_val(client->local_buffers, i);
				world2df_move_rigidbody(world, player_input.conv, player_input.keycode);
			}
		}
	}
	else {

	}
	return 0;
}


static int l_client_apply_input(lua_State* L)
{
	khint_t k;
	frame_t frame;
	lockstep_client_p client;
	world2df_p world;
	player_input_t player_input;
	s2c_t s2c;
	int ret;

	client = (lockstep_client_p)lua_touserdata(L, 1);
	world = (world2df_p)lua_touserdata(L, 2);

	if (client->local_frameid > client->server_frameid) {

	}
	else {

	}

	client->local_frameid++;

	/* 当前客户端 */
	player_input.conv = (int16_t)luaL_checkinteger(L, 3);
	player_input.keycode = (int16_t)luaL_checkinteger(L, 4);

	k = kh_put(kplayerinput, client->local_buffers, client->local_frameid, &ret);
	kh_val(client->local_buffers, k) = player_input;

	/* 先执行 */
	world2df_move_rigidbody(world, player_input.conv, player_input.keycode);

	return 0;
}

static int l_client_rollback(lua_State* L)
{
	lockstep_client_p client;
	world2df_p main_world;

	client = (lockstep_client_p)lua_touserdata(L, 1);
	main_world = (world2df_p)lua_touserdata(L, 2);

	khint_t k = kh_get(kworld, client->worlds, client->server_frameid);
	if (k != kh_end(client->worlds)) {
		frame_t world_packet = kh_val(client->worlds, k);
		world2df_deserialize(main_world, world_packet.data, world_packet.len);
		lua_pushboolean(L, true);
	}
	else {
		lua_pushboolean(L, false);
	}
	return 1;
}

static int l_get_local_inputs(lua_State* L)
{
	lockstep_client_p client = (lockstep_client_p)
	luaL_checkudata(L, 1, "lockstep_client");
	int frame_id = (int)luaL_checkinteger(L, 2);
	if (client->local_buffers == NULL) {
		lua_newtable(L);
		return 1;
	}
	lua_newtable(L);
	khint_t k = kh_get(kplayerinput, client->local_buffers, frame_id);
	if (k != kh_end(client->local_buffers)) {
		player_input_t player_input = kh_val(client->local_buffers, k);
		lua_newtable(L);
		lua_pushinteger(L, player_input.conv);
		lua_setfield(L, -2, "conv");
		lua_pushinteger(L, player_input.keycode);
		lua_setfield(L, -2, "keycode");
		lua_rawseti(L, -2, 1);
	}

	return 1;
}

static int l_client_add_command(lua_State* L)
{
	/*khint_t k;
	int ret, len;
	lockstep_client_p client;
	player_input_t player_input;
	khash_t(kplayerinput)* server_player_inputs;

	server_player_inputs = kh_init(kplayerinput);
	client = (lockstep_client_p)lua_touserdata(L, 1);
	len = luaL_len(L, 2);
	for (int i = 1; i <= len; i++) {
		lua_rawgeti(L, 2, i);
		if (lua_istable(L, -1)) {
			lua_getfield(L, -1, "conv");
			player_input.conv = (int)lua_tointeger(L, -1);
			lua_pop(L, 1);

			lua_getfield(L, -1, "keycode");
			player_input.keycode = (int)lua_tointeger(L, -1);
			lua_pop(L, 1);

			k = kh_put(kplayerinput, server_player_inputs, player_input.conv, &ret);
			kh_val(server_player_inputs, k) = player_input;
		}

		lua_pop(L, 1);
	}

	k = kh_put(ksbuffer, client->server_buffers, client->server_frameid, &ret);
	kh_val(client->server_buffers, k) = server_player_inputs;*/

	return 0;
}

static int l_client_add_world(lua_State* L)
{
	khint_t k;
	int ret;
	size_t size;
	frame_t frame;
	const char* data;
	lockstep_client_p client;
	client = (lockstep_client_p)lua_touserdata(L, 1);
	client->server_frameid = luaL_checkinteger(L, 2);
	data = luaL_checklstring(L, 3, &size);
	frame.len = size;
	frame.data = (char*)SDL_malloc(sizeof(char) * frame.len);
	SDL_memcpy(frame.data, data, frame.len);

	k = kh_put(kworld, client->worlds, client->server_frameid, &ret);
	kh_val(client->worlds, k) = frame;

	return 0;
}

static int l_client_get_frameid(lua_State* L)
{
	lockstep_client_p client;
	client = (lockstep_client_p)lua_touserdata(L, 1);
	lua_pushinteger(L, client->server_frameid);
	lua_pushinteger(L, client->local_frameid);
	return 2;
}

static int l_client_set_server_frameid(lua_State* L)
{
	lockstep_client_p client;
	client = (lockstep_client_p)lua_touserdata(L, 1);
	client->server_frameid = luaL_checkinteger(L, 2);
	return 0;
}

static int l_client_is_ready(lua_State *L)
{
	lockstep_client_p client;
	client = (lockstep_client_p)lua_touserdata(L, 1);
	lua_pushboolean(L, client->ready);
	return 1;
}

int luaopen_client(lua_State* L)
{
	luaL_checkversion(L);
	luaL_Reg l[] = {
	    {"create", l_client_create},
	    {"destroy", l_client_destroy},
	    {"cmd_loading1", l_client_cmd_loading1},
	    {"cmd_loading2", l_client_cmd_loading2},
	    {"cmd_playerjoin", l_client_cmd_playerjoin},
	    {"create_player_join", l_client_create_player_join},
	    {"predict", l_client_predict },
	    {"rollback", l_client_rollback },
	    {"apply_input", l_client_apply_input },
	    {"get_local_inputs", l_get_local_inputs },
	    {"add_command", l_client_add_command },
	    {"add_world", l_client_add_world },
	    {"get_frame_id", l_client_get_frameid },
	    {"set_server_frameid", l_client_set_server_frameid },
	    {"is_ready", l_client_is_ready },
	    {NULL, NULL},
	};
	luaL_newlib(L, l);
	return 1;
}


