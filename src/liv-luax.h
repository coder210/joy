/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-luax.h
Description: lua���
Author: ydlc
Version: 1.0
Date: 2021.3.22
History:
*************************************************/
#ifndef LIV_LUAX_H
#define LIV_LUAX_H
#include <stdbool.h>
#include "liv.h"

void* liv_luax_create(void);
int liv_luax_init(void* inst, void *app_ptr, const char* param);
int liv_luax_event_handler(void* inst, void* event);
int liv_luax_update(void* inst, float delta_time);
void liv_luax_release(void* inst);


#endif // !LIV_LUAX_H
