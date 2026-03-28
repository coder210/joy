/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jsx.h
Description: js�ű���� 
Author: ydlc
Version: 1.0
Date: 2025.7.30
History:
*************************************************/
#ifndef JSX_H
#define JSX_H
#include <stdbool.h>
#include "jconfig.h"

typedef struct jsx jsx_t, *jsx_p;

#ifdef __cplusplus
extern "C" {
#endif
	JOY_API jsx_p jsx_create(void);
	JOY_API bool jsx_dofile(jsx_p jsx, const char* filename);
	JOY_API void jsx_release(jsx_p jsx);
#ifdef __cplusplus
}
#endif

#endif // !JSX_H
