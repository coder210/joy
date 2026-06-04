/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jsx.h
Description: js脚本引擎
Author: ydlc
Version: 1.0
Date: 2025.7.30
History:
*************************************************/
#ifndef JSX_H
#define JSX_H
#include <stdbool.h>
#include <SDL3/SDL.h>
#include "jconfig.h"

typedef struct jsx jsx_t, *jsx_p;

#ifdef __cplusplus
extern "C" {
#endif
	/* --- 基础 --- */
	JOY_API jsx_p jsx_create(void);
	JOY_API bool jsx_dofile(jsx_p jsx, const char* filename);
	JOY_API void jsx_release(jsx_p jsx);

	/* --- 事件派发 --- */
	JOY_API void jsx_dispatch_event(jsx_p jsx, const SDL_Event* event);

	/* --- 脚本调用 --- */
	JOY_API bool jsx_call_joy(jsx_p jsx, const char* func, int argc, float dt);

	/* --- state 持久化 --- */
	JOY_API void jsx_init_state(jsx_p jsx);
	JOY_API bool jsx_reload(jsx_p jsx, const char* filename);

	/* --- 文件监控 --- */
	JOY_API void jsx_set_watch(jsx_p jsx, const char* filename);
	JOY_API bool jsx_poll_reload(jsx_p jsx);
	JOY_API const char* jsx_get_watch(jsx_p jsx);

	/* --- 命令行参数 --- */
	JOY_API void jsx_set_args(jsx_p jsx, int argc, char* argv[]);

#ifdef __cplusplus
}
#endif

#endif // !JSX_H
