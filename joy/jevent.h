/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jevent.h
Description: 事件的封装
Author: ydlc
Version: 1.0
Date: 2026.6.1
History:
*************************************************/
#ifndef JEVENT_H
#define JEVENT_H
#include <stdint.h>
#include <stdbool.h>
#include "jconfig.h"
#include <SDL3/SDL_events.h>

typedef void (*event_keypressed_fn)(const char* key, int scancode, bool repeat, bool ctrl, bool shift, bool alt, void* arg);
typedef void (*event_keyreleased_fn)(const char* key, int scancode, void* arg);
typedef void (*event_mousepressed_fn)(float x, float y, int button, void* arg);
typedef void (*event_mousereleased_fn)(float x, float y, int button, void* arg);
typedef void (*event_mousemoved_fn)(float x, float y, float dx, float dy, void* arg);
typedef void (*event_textinput_fn)(const char* text, void* arg);
typedef void (*event_resize_fn)(int w, int h, void* arg);

typedef struct event_callbacks {
	event_keypressed_fn on_keypressed;
	event_keyreleased_fn on_keyreleased;
	event_mousepressed_fn on_mousepressed;
	event_mousereleased_fn on_mousereleased;
	event_mousemoved_fn on_mousemoved;
	event_textinput_fn on_textinput;
	event_resize_fn on_resize;
} event_callbacks_t;


#ifdef __cplusplus
extern "C" {
#endif
	JOY_API void dispatch_event(event_callbacks_t cbs, const SDL_Event* event, void* arg);
#ifdef __cplusplus
}
#endif

#endif // !JEVENT_H