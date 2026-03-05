/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: profiler.h
Description: 监控
Author: ydlc
Version: 1.0
Date: 2026.1.9
History:
*************************************************/
#ifndef PROFILER_H
#define PROFILER_H
#include <SDL3/SDL.h>

typedef struct simple_fps_counter simple_fps_counter_t, *simple_fps_counter_p;

simple_fps_counter_p simple_fps_create();
void simple_fps_destory(simple_fps_counter_p fps);
int simple_fps_update(simple_fps_counter_p fps);

#endif // !CORE_PROFILER_H