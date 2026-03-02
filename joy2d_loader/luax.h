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
#include <stdbool.h>
#include "joy.h"

void* luax_create(void);
int luax_init(void* inst, void *app_ptr, const char* param);
int luax_event_handler(void* inst, void* event);
int luax_update(void* inst, float delta_time);
void luax_release(void* inst);


#endif // !LUAX_H
