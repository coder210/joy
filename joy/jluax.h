/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jluax.h
Description: lua
Author: ydlc
Version: 1.0
Date: 2021.3.22
History:
*************************************************/
#ifndef JLUAX_H
#define JLUAX_H
#include "jconfig.h"
#include "jcore.h"
#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct luax luax_t, *luax_p;

	/* --- 基础 --- */
	JOY_API luax_p luax_create(void);
	JOY_API bool luax_dofile(luax_p luax, const char* filename);
	JOY_API void luax_release(luax_p luax);
	void* luax_get_state(luax_p luax);

	/* --- state 持久化 --- */
	JOY_API void luax_init_state(luax_p luax);
	JOY_API bool luax_reload(luax_p luax, const char* filename);
	JOY_API bool luax_call_joy(luax_p luax, const char* func, int argc, float dt);

	/* --- 事件派发 --- */
	JOY_API void luax_dispatch_event(luax_p luax, const SDL_Event* event);

	/* --- 文件监控 --- */
	JOY_API void luax_set_watch(luax_p luax, const char* filename);
	JOY_API bool luax_poll_reload(luax_p luax);
	JOY_API const char* luax_get_watch(luax_p luax);

	/* --- 命令行参数 --- */
	JOY_API void luax_set_args(luax_p luax, int argc, char* argv[]);

#ifdef __cplusplus
}
#endif

#endif // !JLUAX_H
