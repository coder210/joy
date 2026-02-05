/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-jsx.h
Description: js�ű���� 
Author: ydlc
Version: 1.0
Date: 2025.7.30
History:
*************************************************/
#ifndef JOY_JSX_H
#define JOY_JSX_H
#include <stdbool.h>
#include "joy.h"

void* jsx_create(void);
int jsx_init(void* inst, void *app_ptr, const char* param);
int jsx_event_handler(void* inst, void* event);
int jsx_update(void* inst, float delta_time);
void jsx_release(void* inst);

#endif // !JOY_JSX_H
