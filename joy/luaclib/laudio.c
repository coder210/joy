#include "../external/lua/lapi.h"
#include "../external/lua/lauxlib.h"
#include "../joy.h"
#include "luaclib.h"

/* ==================== 辅助：根据扩展名派发 ==================== */

static bool create_audio_by_ext(const char* path, uint32_t dev)
{
	const char* ext = strrchr(path, '.');
	if (!ext) return false;
	if (SDL_strcasecmp(ext, ".mp3") == 0)  return audio_create_mp3(path, dev);
	if (SDL_strcasecmp(ext, ".flac") == 0) return audio_create_flac(path, dev);
	if (SDL_strcasecmp(ext, ".ogg") == 0)  return audio_create_ogg(path, dev);
	return audio_create_wav(path, dev); /* 默认 WAV */
}

static audio_shot_p shot_by_ext(const char* path, uint32_t dev)
{
	const char* ext = strrchr(path, '.');
	if (!ext) return NULL;
	if (SDL_strcasecmp(ext, ".mp3") == 0)  return audio_shot_create_mp3(path, dev);
	if (SDL_strcasecmp(ext, ".flac") == 0) return audio_shot_create_flac(path, dev);
	if (SDL_strcasecmp(ext, ".ogg") == 0)  return audio_shot_create_ogg(path, dev);
	return audio_shot_create(path, dev);
}

static audio_bgm_p bgm_by_ext(const char* path, uint32_t dev)
{
	const char* ext = strrchr(path, '.');
	if (!ext) return NULL;
	if (SDL_strcasecmp(ext, ".mp3") == 0)  return audio_bgm_create_mp3(path, dev);
	if (SDL_strcasecmp(ext, ".flac") == 0) return audio_bgm_create_flac(path, dev);
	if (SDL_strcasecmp(ext, ".ogg") == 0)  return audio_bgm_create_ogg(path, dev);
	return audio_bgm_create(path, dev);
}

/* ==================== 设备管理 ==================== */

static int l_audio_open_device(lua_State* L)
{
	uint32_t device_id;
	device_id = audio_open_device();
	lua_pushinteger(L, device_id);
	return 1;
}

static int l_audio_close_device(lua_State* L)
{
	uint32_t device_id;
	device_id = luaL_checkinteger(L, 1);
	audio_close_device(device_id);
	return 0;
}

/* ==================== 简易播放（统一派发） ==================== */

static int l_audio_create(lua_State* L)
{
	const char* filepath = luaL_checkstring(L, 1);
	uint32_t device_id = luaL_checkinteger(L, 2);
	lua_pushboolean(L, create_audio_by_ext(filepath, device_id));
	return 1;
}

static int l_audio_create_wav(lua_State* L)
{
	const char* filepath = luaL_checkstring(L, 1);
	uint32_t device_id = luaL_checkinteger(L, 2);
	lua_pushboolean(L, audio_create_wav(filepath, device_id));
	return 1;
}

static int l_audio_create_mp3(lua_State* L)
{
	const char* filepath = luaL_checkstring(L, 1);
	uint32_t device_id = luaL_checkinteger(L, 2);
	lua_pushboolean(L, audio_create_mp3(filepath, device_id));
	return 1;
}

static int l_audio_create_flac(lua_State* L)
{
	const char* filepath = luaL_checkstring(L, 1);
	uint32_t device_id = luaL_checkinteger(L, 2);
	lua_pushboolean(L, audio_create_flac(filepath, device_id));
	return 1;
}

static int l_audio_create_ogg(lua_State* L)
{
	const char* filepath = luaL_checkstring(L, 1);
	uint32_t device_id = luaL_checkinteger(L, 2);
	lua_pushboolean(L, audio_create_ogg(filepath, device_id));
	return 1;
}

/* ==================== 一次性音效（shot） ==================== */

static int l_audio_shot_create(lua_State* L)
{
	const char* path = luaL_checkstring(L, 1);
	uint32_t dev = luaL_checkinteger(L, 2);
	audio_shot_p s = shot_by_ext(path, dev);
	lua_pushlightuserdata(L, s);
	return 1;
}

static int l_audio_shot_play(lua_State* L)
{
	audio_shot_p s = (audio_shot_p)lua_touserdata(L, 1);
	audio_shot_play(s);
	return 0;
}

static int l_audio_shot_set_gain(lua_State* L)
{
	audio_shot_p s = (audio_shot_p)lua_touserdata(L, 1);
	float gain = (float)luaL_checknumber(L, 2);
	audio_shot_set_gain(s, gain);
	return 0;
}

static int l_audio_shot_destroy(lua_State* L)
{
	audio_shot_p s = (audio_shot_p)lua_touserdata(L, 1);
	audio_shot_destroy(s);
	return 0;
}

/* ==================== 背景音乐（bgm） ==================== */

static int l_audio_bgm_create(lua_State* L)
{
	const char* path = luaL_checkstring(L, 1);
	uint32_t dev = (uint32_t)luaL_optinteger(L, 2, 0);
	audio_bgm_p bgm = bgm_by_ext(path, dev);
	lua_pushlightuserdata(L, bgm);
	return 1;
}

static int l_audio_bgm_stop(lua_State* L)
{
	audio_bgm_p bgm = (audio_bgm_p)lua_touserdata(L, 1);
	audio_bgm_stop(bgm);
	return 0;
}

static int l_audio_bgm_set_gain(lua_State* L)
{
	audio_bgm_p bgm = (audio_bgm_p)lua_touserdata(L, 1);
	float gain = (float)luaL_checknumber(L, 2);
	audio_bgm_set_gain(bgm, gain);
	return 0;
}

static int l_audio_bgm_destroy(lua_State* L)
{
	audio_bgm_p bgm = (audio_bgm_p)lua_touserdata(L, 1);
	audio_bgm_destroy(bgm);
	return 0;
}

/* ==================== 模块注册 ==================== */

int luaopen_audio(lua_State *L)
{
	luaL_checkversion(L);
	luaL_Reg l[] = {
		/* 设备 */
		{"open_device",   l_audio_open_device},
		{"close_device",  l_audio_close_device},
		/* 简易播放 */
		{"create",        l_audio_create},
		{"create_wav",    l_audio_create_wav},
		{"create_mp3",    l_audio_create_mp3},
		{"create_flac",   l_audio_create_flac},
		{"create_ogg",    l_audio_create_ogg},
		/* 一次性音效 */
		{"create_shot",   l_audio_shot_create},
		{"shot_play",     l_audio_shot_play},
		{"shot_set_gain", l_audio_shot_set_gain},
		{"shot_destroy",  l_audio_shot_destroy},
		/* 背景音乐 */
		{"create_bgm",    l_audio_bgm_create},
		{"bgm_stop",      l_audio_bgm_stop},
		{"bgm_set_gain",  l_audio_bgm_set_gain},
		{"bgm_destroy",   l_audio_bgm_destroy},
		{NULL, NULL}
	};
	luaL_newlib(L, l);
	return 1;
}
