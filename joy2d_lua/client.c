#include <SDL3/SDL.h>
#include "luaclib.h"
#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "joy2d/external/klib/khash.h"
#include "joy2d/external/klib/klist.h"
#include "joy2d/log.h"
//#include "joy2d/proto.h"
#include "joy2d/mathx.h"
#include "joy2d/physics.h"

typedef struct player_input {
	int conv;
	int keycode;
}player_input_t, * player_input_p;

typedef struct frame {
	char* data;
	int len;
}frame_t, * frame_p;

KHASH_INIT(kworld, int, frame_t, 1, kh_int_hash_func, kh_int_hash_equal)


#define __input_free(x)
KLIST_INIT(kinput_queue, player_input_t, __input_free)

typedef struct lockstep_client {
	bool ready;
	char buf[JOY_MAX_BUFFER * 1024 * 2];
	int saving_cursor; /* 保存的字节数 */
	int server_frameid;
	khash_t(kworld)* worlds;  /* 服务器的帧world */
	klist_t(kinput_queue)* local_buffers; /* 本地预测输入 */
}lockstep_client_t, * lockstep_client_p;

static int l_client_create(lua_State* L)
{
	lockstep_client_p client;
	client = (lockstep_client_p)SDL_malloc(sizeof(lockstep_client_t));
	SDL_assert(client);
	client->ready = false;
	client->saving_cursor = 0;
	client->server_frameid = 0;
	client->worlds = kh_init(kworld);
	client->local_buffers = kl_init(kinput_queue);
	lua_pushlightuserdata(L, client);
	return 1;
}

static int l_client_destroy(lua_State* L)
{
	lockstep_client_p client;
	client = lua_touserdata(L, 1);
	kh_destroy(kworld, client->worlds);
	kl_destroy(kinput_queue, client->local_buffers);
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
		}
	}

	if (client->ready) {
		client->server_frameid = frame_id;
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

static int l_client_pop_local_input(lua_State* L)
{
        lockstep_client_p client;
        client = (lockstep_client_p)lua_touserdata(L, 1);
	kl_shift(kinput_queue, client->local_buffers, 0);
        return 0;
}

static int l_client_predict(lua_State* L)
{
	lockstep_client_p client;
	world2df_p world;
	player_input_t player_input;
        kliter_t(kinput_queue)* it;
	client = (lockstep_client_p)lua_touserdata(L, 1);
	world = (world2df_p)lua_touserdata(L, 2);


	//SDL_Log("================predict========================");
	//for (it = kl_begin(client->local_buffers);
	//	it != kl_end(client->local_buffers);
	//	it = kl_next(it)) {
	//	player_input = kl_val(it);
	//	SDL_Log("keycode=%d,", player_input.keycode);
	//}

	////      local ok, conv = net.kcpclient.get_conv(app.kcpclient);
 //       //kl_shift(kinput_queue, client->local_buffers, 0);
	//SDL_Log("================predict2========================");

	for (it = kl_begin(client->local_buffers); 
		it != kl_end(client->local_buffers); 
		it = kl_next(it)) {
                player_input = kl_val(it);
		world2df_move_player(world, player_input.conv, player_input.keycode);
                //SDL_Log("keycode=%d,", player_input.keycode);
	}

	return 0;
}


static int l_client_apply_input(lua_State* L)
{
	//khint_t k;
	frame_t frame;
	lockstep_client_p client;
	world2df_p world;
	player_input_t player_input;
	int ret;

	client = (lockstep_client_p)lua_touserdata(L, 1);
	world = (world2df_p)lua_touserdata(L, 2);

	/* 当前客户端 */
	player_input.conv = (int16_t)luaL_checkinteger(L, 3);
	player_input.keycode = (int16_t)luaL_checkinteger(L, 4);

	/* 输入 */
	*kl_pushp(kinput_queue, client->local_buffers) = player_input;

	/* 先执行 */
	world2df_move_player(world, player_input.conv, player_input.keycode);

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
	return 1;
}

static int l_client_set_frameid(lua_State* L)
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
	    {"add_world", l_client_add_world },
	    {"get_frameid", l_client_get_frameid },
	    {"set_frameid", l_client_set_frameid },
	    {"is_ready", l_client_is_ready },
	    {"pop_local_input", l_client_pop_local_input },
	    {NULL, NULL},
	};
	luaL_newlib(L, l);
	return 1;
}


