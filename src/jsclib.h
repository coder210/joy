#ifndef JSCLIB_H
#define JSCLIB_H
#include "./external/mujs/mujs.h"

void jsopen_base(js_State* J);
void jsopen_fp(js_State* J);
void jsopen_vec2f(js_State* J);
void jsopen_vec2(js_State* J);
void jsopen_lockstep(js_State* J);

#endif // !JSCLIB_H
