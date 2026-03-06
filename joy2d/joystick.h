/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: joystick.h
Description: Ò¡¸Ë
Author: ydlc
Version: 1.0
Date: 2026.1.7
History:
*************************************************/
#ifndef JOYSTICK_H
#define JOYSTICK_H
#include <SDL3/SDL.h>
#include <stdbool.h>
#include "config.h"

typedef struct joystick joystick_t, *joystick_p;

#ifdef __cplusplus
extern "C" {
#endif

	JOY_API joystick_p joystick_create(SDL_Renderer* renderer, float x, float y, float radius);
	JOY_API void joystick_destroy(joystick_p joystick);
	JOY_API void joystick_draw(joystick_p joystick);
	JOY_API void joystick_handle_event(joystick_p joystick, SDL_Event* event);
	JOY_API void joystick_reset(joystick_p joystick);
	JOY_API void joystick_set_position(joystick_p joystick, float x, float y);
	JOY_API SDL_FPoint joystick_get_direction(joystick_p joystick);
	JOY_API float joystick_get_magnitude(joystick_p joystick);

#ifdef __cplusplus
}
#endif

#endif // JOYSTICK_H