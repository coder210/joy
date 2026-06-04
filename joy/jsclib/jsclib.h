#ifndef JSCLIB_H
#define JSCLIB_H
#include "../external/mujs/mujs.h"

void jsopen_base(js_State* J);
void jsopen_fp(js_State* J);
void jsopen_ft(js_State* J);
void jsopen_vec2f(js_State* J);
void jsopen_vec2(js_State* J);
void jsopen_vec3f(js_State* J);
void jsopen_vec3(js_State* J);
void jsopen_mat22f(js_State* J);
void jsopen_mat22(js_State* J);
void jsopen_collision2d(js_State* J);
void jsopen_collision2df(js_State* J);
void jsopen_collision3d(js_State* J);
void jsopen_utils(js_State* J);
void jsopen_event(js_State* J);
void jsopen_sdl(js_State* J);
void jsopen_keyboard(js_State* J);
void jsopen_audio(js_State* J);
void jsopen_animation(js_State* J);

#endif // !JSCLIB_H
