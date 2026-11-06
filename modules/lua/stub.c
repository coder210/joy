/*
 * Lua 后端关闭时的空桩：调用方 luax_* 调用仍可编译、可链接，
 * 创建必然失败、调用必然返回空，调用方据此跳过脚本逻辑即可。
 * 模块启用时 ENABLED_LUA_SCRIPT 被定义，本文件内容为空，由 jluax.c 提供实现。
 */
#include <stdbool.h>
#include <stddef.h>

#include "joy_lua.h"

#ifndef ENABLED_LUA_SCRIPT
luax_p luax_create(void) { return NULL; }
void   luax_release(luax_p luax) { (void)luax; }
bool   luax_dofile(luax_p luax, const char* filename) { (void)luax; (void)filename; return false; }
bool   luax_reload(luax_p luax, const char* filename) { (void)luax; (void)filename; return false; }
void*  luax_get_state(luax_p luax) { (void)luax; return NULL; }
bool   luax_call_joy(luax_p luax, const char* func, int argc, float dt) {
        (void)luax; (void)func; (void)argc; (void)dt; return false;
}
void luax_dispatch_event(luax_p luax, const void* event) { (void)luax; (void)event; }
void luax_set_watch(luax_p luax, const char* filename) { (void)luax; (void)filename; }
bool luax_poll_reload(luax_p luax) { (void)luax; return false; }
const char* luax_get_watch(luax_p luax) { (void)luax; return NULL; }
void luax_set_args(luax_p luax, int argc, char* argv[]) { (void)luax; (void)argc; (void)argv; }

void luax_open_api(void* lua_state) { (void)lua_state; }
const char* luax_api_selftest(void) { return "lua/disabled"; }
#endif /* ENABLED_LUA_SCRIPT */
