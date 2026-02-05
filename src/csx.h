/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: csx.h
Description: c#
Author: ydlc
Version: 1.0
Date: 2021.3.28
History:
*************************************************/
#ifndef CSX_H
#define CSX_H
#include <stdbool.h>
#include "joy.h"

void* csx_create(void);
int csx_init(void* inst, void *app_ptr, const char* param);
int csx_event_handler(void* inst, void* event);
int csx_update(void* inst, float delta_time);
void csx_release(void* inst);

#endif // !CSX_H
