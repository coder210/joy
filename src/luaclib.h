/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: luaclib.h
Description: lua脚本的接口
Author: ydlc
Version: 1.0
Date: 2021.3.18
History:
*************************************************/
#ifndef LIVNET_LUACLIB_H
#define LIVNET_LUACLIB_H
#include "./external/lua/lualib.h"
#include "./external/lua_cjson.h"


int luaopen_window(lua_State* L);
int luaopen_sdl(lua_State *L);
int luaopen_graphics(lua_State* L);
int luaopen_audio(lua_State* L);
int luaopen_net(lua_State* L);
int luaopen_utils(lua_State *L);
int luaopen_mathx(lua_State *L);
int luaopen_vec2(lua_State *L);
int luaopen_vec3(lua_State *L);
int luaopen_collision2d(lua_State *L);
int luaopen_collision3d(lua_State *L);
int luaopen_packagex(lua_State *L);
int luaopen_ui(lua_State* L);
int luaopen_lockstep(lua_State* L);
int luaopen_c2s(lua_State* L);
int luaopen_s2c(lua_State* L);
int luaopen_ecs(lua_State* L);
int luaopen_rigidbody(lua_State* L);
int luaopen_world2df(lua_State* L);
int luaopen_server(lua_State* L);
int luaopen_client(lua_State* L);
int luaopen_timer(lua_State* L);
int luaopen_render_world(lua_State* L);
int luaopen_keyboard(lua_State* L);
int luaopen_animation(lua_State* L);
int luaopen_joystick(lua_State* L);
int luaopen_profiler(lua_State* L);

#endif // !LIVNET_LUACLIB_H
