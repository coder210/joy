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

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct luax luax_t, *luax_p;
	JOY_API luax_p luax_create(void);
	JOY_API bool luax_dofile(luax_p luax, const char* filename);
	JOY_API void luax_release(luax_p luax);

	/* --- 热重载支持 --- */
	JOY_API void luax_init_state(luax_p luax);            // 创建初始 state 表
	JOY_API bool luax_reload(luax_p luax, const char* filename); // 加载/重载脚本，保留 state
	JOY_API bool luax_call_joy(luax_p luax, const char* func, int argc, float dt); // 调用 joy.func(dt)
#ifdef __cplusplus
}
#endif

#endif // !JLUAX_H
