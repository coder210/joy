/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: audio.h
Description: 声音相关
Author: ydlc
Version: 1.0
Date: 2021.3.28
History:
*************************************************/
#ifndef JAUDIO_H
#define JAUDIO_H
#include "jconfig.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

	JOY_API uint32_t audio_open_device();
	JOY_API void audio_close_device(uint32_t device_id);

	/* ---------- 简易播放（创建即播放，循环） ---------- */
	JOY_API bool audio_create_wav(const char* filepath, uint32_t device_id);
	JOY_API bool audio_create_mp3(const char* filepath, uint32_t device_id);
	JOY_API bool audio_create_flac(const char* filepath, uint32_t device_id);
	JOY_API bool audio_create_ogg(const char* filepath, uint32_t device_id);

	/* ---------- 一次性音效（脚步声/攻击声等） ---------- */
	typedef struct audio_shot audio_shot_t, * audio_shot_p;

	JOY_API audio_shot_p audio_shot_create(const char* wav_path, uint32_t device_id);
	JOY_API audio_shot_p audio_shot_create_mp3(const char* path, uint32_t device_id);
	JOY_API audio_shot_p audio_shot_create_flac(const char* path, uint32_t device_id);
	JOY_API audio_shot_p audio_shot_create_ogg(const char* path, uint32_t device_id);

	JOY_API void audio_shot_play(audio_shot_p shot);
	JOY_API void audio_shot_set_gain(audio_shot_p shot, float gain);
	JOY_API void audio_shot_destroy(audio_shot_p shot);

	/* ---------- 背景音乐（循环播放，可控停/音量） ---------- */
	typedef struct audio_bgm audio_bgm_t, * audio_bgm_p;

	JOY_API audio_bgm_p audio_bgm_create(const char* wav_path, uint32_t device_id);
	JOY_API audio_bgm_p audio_bgm_create_mp3(const char* path, uint32_t device_id);
	JOY_API audio_bgm_p audio_bgm_create_flac(const char* path, uint32_t device_id);
	JOY_API audio_bgm_p audio_bgm_create_ogg(const char* path, uint32_t device_id);

	JOY_API void audio_bgm_stop(audio_bgm_p bgm);
	JOY_API void audio_bgm_set_gain(audio_bgm_p bgm, float gain);
	JOY_API void audio_bgm_destroy(audio_bgm_p bgm);

#ifdef __cplusplus
}
#endif

#endif // !JAUDIO_H
