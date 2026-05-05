/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jcore.h
Description: 窗口创建,日志打印等
Author: ydlc
Version: 1.0
Date: 2021.3.28
History:
*************************************************/
#ifndef CORE_H
#define CORE_H
#include <stdint.h>
#include "jconfig.h"

typedef struct env env_t, * env_p;
typedef struct simple_fps_counter simple_fps_counter_t, * simple_fps_counter_p;
typedef struct game_timer {
	int64_t last_time;          // 上一帧结束时的纳秒时间戳
	int64_t frame_start_time;   // 当前帧开始时的纳秒时间戳
	double delta_time;         // 未缩放的帧间隔时间（秒）
	double time_scale;         // 时间缩放因子
	int target_fps;            // 目标 FPS（0 表示不限制）
	double target_frame_time;  // 目标每帧时间（秒）
} game_timer_t, * game_timer_p;


#ifdef __cplusplus
extern "C" {
#endif
	JOY_API int log_info(const char* format, ...);
	JOY_API int log_debug(const char* format, ...);
	JOY_API int log_error(const char* format, ...);
	JOY_API void log_title(int type);

	JOY_API env_p env_create(void);
	JOY_API const char* env_get(env_p env, const char* key);
	JOY_API void env_set(env_p env, const char* key, const char* value);
	JOY_API void env_destroy(env_p env);

	JOY_API simple_fps_counter_p simple_fps_create();
	JOY_API void simple_fps_destory(simple_fps_counter_p fps);
	JOY_API int simple_fps_update(simple_fps_counter_p fps);


	JOY_API void game_timer_init(game_timer_p clock);
	JOY_API void game_timer_update(game_timer_p clock);
	JOY_API float game_timer_get_delta_time(const game_timer_p clock);
	JOY_API float game_timer_get_unscaled_delta_time(const game_timer_p clock);
	JOY_API void game_timer_set_time_scale(game_timer_p clock, float scale);
	JOY_API float game_timer_get_time_scale(const game_timer_p clock);
	JOY_API void game_timer_set_target_fps(game_timer_p clock, int fps);
	JOY_API int game_timer_get_target_fps(const game_timer_p clock);

#ifdef __cplusplus
}
#endif


#endif // !CORE_H