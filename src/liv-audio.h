/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-audio.h
Description: …˘“Ùœ‡πÿ
Author: ydlc
Version: 1.0
Date: 2021.3.28
History:
*************************************************/
#ifndef LIV_AUDIO_H
#define LIV_AUDIO_H
#include <stdint.h>
#include <stdbool.h>

uint32_t audio_open_device();
void audio_close_device(uint32_t device_id);
bool audio_create_wav(const char* filepath, uint32_t device_id);
bool audio_create_ogg(const char* filepath, uint32_t device_id);

#endif // !LIV_AUDIO_H
