#include "../joy.h"
#include "jsclib.h"

/* ==================== 辅助：根据扩展名派发 ==================== */

static bool create_by_ext(const char* path, uint32_t dev)
{
	const char* ext = strrchr(path, '.');
	if (!ext) return false;
	if (SDL_strcasecmp(ext, ".mp3") == 0)  return audio_create_mp3(path, dev);
	if (SDL_strcasecmp(ext, ".flac") == 0) return audio_create_flac(path, dev);
	if (SDL_strcasecmp(ext, ".ogg") == 0)  return audio_create_ogg(path, dev);
	return audio_create_wav(path, dev);
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

static duk_ret_t jaudio_open_device(duk_context* ctx)
{
	duk_push_number(ctx, (duk_double_t)audio_open_device());
	return 1;
}

static duk_ret_t jaudio_close_device(duk_context* ctx)
{
	audio_close_device((uint32_t)duk_to_number(ctx, 0));
	return 0;
}

/* ==================== 简易播放（统一派发） ==================== */

static duk_ret_t jaudio_create(duk_context* ctx)
{
	const char* path = duk_to_string(ctx, 0);
	uint32_t dev = (uint32_t)duk_to_number(ctx, 1);
	duk_push_boolean(ctx, create_by_ext(path, dev));
	return 1;
}

static duk_ret_t jaudio_create_wav(duk_context* ctx)
{
	const char* filepath = duk_to_string(ctx, 0);
	uint32_t device_id = (uint32_t)duk_to_number(ctx, 1);
	duk_push_boolean(ctx, audio_create_wav(filepath, device_id));
	return 1;
}

static duk_ret_t jaudio_create_mp3(duk_context* ctx)
{
	const char* path = duk_to_string(ctx, 0);
	uint32_t dev = (uint32_t)duk_to_number(ctx, 1);
	duk_push_boolean(ctx, audio_create_mp3(path, dev));
	return 1;
}

static duk_ret_t jaudio_create_flac(duk_context* ctx)
{
	const char* path = duk_to_string(ctx, 0);
	uint32_t dev = (uint32_t)duk_to_number(ctx, 1);
	duk_push_boolean(ctx, audio_create_flac(path, dev));
	return 1;
}

static duk_ret_t jaudio_create_ogg(duk_context* ctx)
{
	const char* path = duk_to_string(ctx, 0);
	uint32_t dev = (uint32_t)duk_to_number(ctx, 1);
	duk_push_boolean(ctx, audio_create_ogg(path, dev));
	return 1;
}

/* ==================== 一次性音效（shot） ==================== */

static duk_ret_t jaudio_shot_create(duk_context* ctx)
{
	const char* path = duk_to_string(ctx, 0);
	uint32_t dev = (uint32_t)duk_to_number(ctx, 1);
	duk_push_pointer(ctx, shot_by_ext(path, dev));
	return 1;
}

static duk_ret_t jaudio_shot_play(duk_context* ctx)
{
	audio_shot_play((audio_shot_p)duk_get_pointer(ctx, 0));
	return 0;
}

static duk_ret_t jaudio_shot_set_gain(duk_context* ctx)
{
	audio_shot_p s = (audio_shot_p)duk_get_pointer(ctx, 0);
	float gain = (float)duk_to_number(ctx, 1);
	audio_shot_set_gain(s, gain);
	return 0;
}

static duk_ret_t jaudio_shot_destroy(duk_context* ctx)
{
	audio_shot_destroy((audio_shot_p)duk_get_pointer(ctx, 0));
	return 0;
}

/* ==================== 背景音乐（bgm） ==================== */

static duk_ret_t jaudio_bgm_create(duk_context* ctx)
{
	const char* path = duk_to_string(ctx, 0);
	uint32_t dev = (uint32_t)duk_to_number(ctx, 1);
	duk_push_pointer(ctx, bgm_by_ext(path, dev));
	return 1;
}

static duk_ret_t jaudio_bgm_stop(duk_context* ctx)
{
	audio_bgm_stop((audio_bgm_p)duk_get_pointer(ctx, 0));
	return 0;
}

static duk_ret_t jaudio_bgm_set_gain(duk_context* ctx)
{
	audio_bgm_p bgm = (audio_bgm_p)duk_get_pointer(ctx, 0);
	float gain = (float)duk_to_number(ctx, 1);
	audio_bgm_set_gain(bgm, gain);
	return 0;
}

static duk_ret_t jaudio_bgm_destroy(duk_context* ctx)
{
	audio_bgm_destroy((audio_bgm_p)duk_get_pointer(ctx, 0));
	return 0;
}

/* ==================== 模块注册 ==================== */

void jsopen_audio(duk_context* ctx)
{
	duk_push_object(ctx);

	/* 设备 */
	duk_push_c_function(ctx, jaudio_open_device, 0);  duk_put_prop_string(ctx, -2, "open_device");
	duk_push_c_function(ctx, jaudio_close_device, 1); duk_put_prop_string(ctx, -2, "close_device");

	/* 简易播放 */
	duk_push_c_function(ctx, jaudio_create, 2);       duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, jaudio_create_wav, 2);   duk_put_prop_string(ctx, -2, "create_wav");
	duk_push_c_function(ctx, jaudio_create_mp3, 2);   duk_put_prop_string(ctx, -2, "create_mp3");
	duk_push_c_function(ctx, jaudio_create_flac, 2);  duk_put_prop_string(ctx, -2, "create_flac");
	duk_push_c_function(ctx, jaudio_create_ogg, 2);   duk_put_prop_string(ctx, -2, "create_ogg");

	/* 一次性音效 */
	duk_push_c_function(ctx, jaudio_shot_create, 2);  duk_put_prop_string(ctx, -2, "create_shot");
	duk_push_c_function(ctx, jaudio_shot_play, 1);    duk_put_prop_string(ctx, -2, "shot_play");
	duk_push_c_function(ctx, jaudio_shot_set_gain, 2);duk_put_prop_string(ctx, -2, "shot_set_gain");
	duk_push_c_function(ctx, jaudio_shot_destroy, 1); duk_put_prop_string(ctx, -2, "shot_destroy");

	/* 背景音乐 */
	duk_push_c_function(ctx, jaudio_bgm_create, 2);   duk_put_prop_string(ctx, -2, "create_bgm");
	duk_push_c_function(ctx, jaudio_bgm_stop, 1);     duk_put_prop_string(ctx, -2, "bgm_stop");
	duk_push_c_function(ctx, jaudio_bgm_set_gain, 2); duk_put_prop_string(ctx, -2, "bgm_set_gain");
	duk_push_c_function(ctx, jaudio_bgm_destroy, 1);  duk_put_prop_string(ctx, -2, "bgm_destroy");

	duk_put_global_string(ctx, "audio");
}
