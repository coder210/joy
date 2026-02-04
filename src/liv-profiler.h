/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-profiler.h
Description: 性能监控
Author: ydlc
Version: 1.0
Date: 2026.1.9
History:
*************************************************/
#ifndef LIV_PROFILER_H
#define LIV_PROFILER_H
#include <SDL3/SDL.h>

// 最简单的FPS计数器
typedef struct simple_fps_counter {
	int frame_count;
	int fps;
	Uint64 last_time;
	Uint64 current_time;
} simple_fps_counter_t, *simple_fps_counter_p;


simple_fps_counter_p simple_fps_create();
void simple_fps_destory(simple_fps_counter_p fps);
int simple_fps_update(simple_fps_counter_p fps);


#endif // !LIV_PROFILER_H