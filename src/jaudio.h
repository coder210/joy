/******************************************************************************
 * src/jaudio.h —— audio 域声明头（jaudio.c 的配套头）
 * 职责：音频
 * 方向：本头是该域声明的归属地，由 include/joy/japi.h 聚合包含；
 *       本头只依赖基础类型头，不再 include 聚合门面 joy.h，避免循环包含。
 * 生成链：本头是 gen_bindings.js 的 DECL_SOURCES 之一，改动声明后必须跑 --scan。
 ******************************************************************************/
#ifndef JOY_AUDIO_IMPL_H
#define JOY_AUDIO_IMPL_H

#include <joy/jtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

        JOY_API uint32_t joy_audio_open_device();
        JOY_API void joy_audio_close_device(uint32_t device_id);
        JOY_API bool joy_audio_create_wav(const char* filepath, uint32_t device_id);
        JOY_API bool joy_audio_create_mp3(const char* filepath, uint32_t device_id);
        JOY_API bool joy_audio_create_flac(const char* filepath, uint32_t device_id);
        JOY_API bool joy_audio_create_ogg(const char* filepath, uint32_t device_id);
        JOY_API joy_audio_shot_p joy_audio_shot_create(const char* wav_path, uint32_t device_id);
        JOY_API joy_audio_shot_p joy_audio_shot_create_mp3(const char* path, uint32_t device_id);
        JOY_API joy_audio_shot_p joy_audio_shot_create_flac(const char* path, uint32_t device_id);
        JOY_API joy_audio_shot_p joy_audio_shot_create_ogg(const char* path, uint32_t device_id);
        JOY_API void joy_audio_shot_play(joy_audio_shot_p shot);
        JOY_API void joy_audio_shot_set_gain(joy_audio_shot_p shot, float gain);
        JOY_API void joy_audio_shot_destroy(joy_audio_shot_p shot);
        JOY_API joy_audio_bgm_p joy_audio_bgm_create(const char* wav_path, uint32_t device_id);
        JOY_API joy_audio_bgm_p joy_audio_bgm_create_mp3(const char* path, uint32_t device_id);
        JOY_API joy_audio_bgm_p joy_audio_bgm_create_flac(const char* path, uint32_t device_id);
        JOY_API joy_audio_bgm_p joy_audio_bgm_create_ogg(const char* path, uint32_t device_id);
        JOY_API void joy_audio_bgm_stop(joy_audio_bgm_p bgm);
        JOY_API void joy_audio_bgm_set_gain(joy_audio_bgm_p bgm, float gain);
        JOY_API void joy_audio_bgm_destroy(joy_audio_bgm_p bgm);

#ifdef __cplusplus
}
#endif

#endif /* JOY_AUDIO_IMPL_H */
