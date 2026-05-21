/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: audio.h
Description: …˘“Ùœ‡πÿ
Author: ydlc
Version: 1.0
Date: 2021.3.28
History:
*************************************************/
#ifndef AUDIO_H
#define AUDIO_H
#include "jconfig.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

	JOY_API uint32_t audio_open_device();
	JOY_API void audio_close_device(uint32_t device_id);
	JOY_API bool audio_create_wav(const char* filepath, uint32_t device_id);
	//JOY_API bool audio_create_ogg(const char* filepath, uint32_t device_id);

#ifdef __cplusplus
}
#endif

#endif // !CORE_AUDIO_H
