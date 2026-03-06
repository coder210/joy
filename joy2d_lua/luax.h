/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: luax.h
Description: lua
Author: ydlc
Version: 1.0
Date: 2021.3.22
History:
*************************************************/
#ifndef LUAX_H
#define LUAX_H
#include "joy2d/config.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct luax luax_t, *luax_p;
	JOY_API luax_p luax_create(void);
	JOY_API bool luax_dofile(luax_p luax, const char* filename);
	JOY_API void luax_release(luax_p luax);
#ifdef __cplusplus
}
#endif

#endif // !LUAX_H
