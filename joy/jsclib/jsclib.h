#ifndef JSCLIB_H
#define JSCLIB_H
#include <SDL3/SDL.h>
#include "../external/duktape/duktape.h"

void jsopen_base(duk_context* J);
void jsopen_fp(duk_context* J);
void jsopen_ft(duk_context* J);
void jsopen_vec2f(duk_context* J);
void jsopen_vec2(duk_context* J);
void jsopen_vec3f(duk_context* J);
void jsopen_vec3(duk_context* J);
void jsopen_mat22f(duk_context* J);
void jsopen_mat22(duk_context* J);
void jsopen_collision2d(duk_context* J);
void jsopen_collision2df(duk_context* J);
void jsopen_collision3d(duk_context* J);
void jsopen_utils(duk_context* J);
void jsopen_event(duk_context* J);
void jsopen_sdl(duk_context* J);
void jsopen_keyboard(duk_context* J);
void jsopen_audio(duk_context* J);
void jsopen_animation(duk_context* J);
void jsopen_network(duk_context* J);
void jsopen_timer(duk_context* J);
void jsopen_joystick(duk_context* J);
void jsopen_ui(duk_context* J);
void jsopen_graphics(duk_context* J);
void jsopen_window(duk_context* J);
void jsopen_render3d(duk_context* J);
void jsopen_gjk3d(duk_context* J);

#endif // !JSCLIB_H
