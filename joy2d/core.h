/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: core.h
Description: 窗口创建,日志打印等
Author: ydlc
Version: 1.0
Date: 2021.3.28
History:
*************************************************/
#ifndef CORE_H
#define CORE_H
#include "config.h"

typedef struct env env_t, * env_p;
typedef struct simple_fps_counter simple_fps_counter_t, * simple_fps_counter_p;

#ifdef __cplusplus
extern "C" {
#endif
	JOY_API int log_info(const char* format, ...);
	JOY_API int log_debug(const char* format, ...);
	JOY_API int log_error(const char* format, ...);

	JOY_API env_p env_create(void);
	JOY_API const char* env_get(env_p env, const char* key);
	JOY_API void env_set(env_p env, const char* key, const char* value);
	JOY_API void env_destroy(env_p env);

	JOY_API simple_fps_counter_p simple_fps_create();
	JOY_API void simple_fps_destory(simple_fps_counter_p fps);
	JOY_API int simple_fps_update(simple_fps_counter_p fps);
#ifdef __cplusplus
}
#endif


#endif // !CORE_H