/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-joystick.h
Description: Ò¡¸Ë
Author: ydlc
Version: 1.0
Date: 2026.1.7
History:
*************************************************/
#ifndef LIV_JOYSTICK_H
#define LIV_JOYSTICK_H

#include <SDL3/SDL.h>
#include <stdbool.h>


typedef struct joystick joystick_t, *joystick_p;

joystick_p joystick_create(SDL_Renderer* renderer, float x, float y, float radius);
void joystick_destroy(joystick_p joystick);
void joystick_draw(joystick_p joystick);
void joystick_handle_event(joystick_p joystick, SDL_Event* event);
void joystick_reset(joystick_p joystick);
void joystick_set_position(joystick_p joystick, float x, float y);
SDL_FPoint joystick_get_direction(joystick_p joystick);
float joystick_get_magnitude(joystick_p joystick);

#endif // LIV_JOYSTICK_H