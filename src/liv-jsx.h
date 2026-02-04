/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-jsx.h
Description: js�ű���� 
Author: ydlc
Version: 1.0
Date: 2025.7.30
History:
*************************************************/
#ifndef LIV_JSX_H
#define LIV_JSX_H
#include <stdbool.h>
#include "liv.h"

void* liv_jsx_create(void);
int liv_jsx_init(void* inst, void *app_ptr, const char* param);
int liv_jsx_event_handler(void* inst, void* event);
int liv_jsx_update(void* inst, float delta_time);
void liv_jsx_release(void* inst);

#endif // !LIV_JSX_H
