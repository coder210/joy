/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-csx.h
Description: c#�ű����
Author: ydlc
Version: 1.0
Date: 2021.3.28
History:
*************************************************/
#ifndef LIV_CSX_H
#define LIV_CSX_H
#include <stdbool.h>
#include "liv.h"

void* liv_csx_create(void);
int liv_csx_init(void* inst, void *app_ptr, const char* param);
int liv_csx_event_handler(void* inst, void* event);
int liv_csx_update(void* inst, float delta_time);
void liv_csx_release(void* inst);

#endif // !LIV_CSX_H
