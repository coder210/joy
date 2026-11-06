/*
 * JavaScript 后端关闭时的空桩：调用方 jsx_* 调用仍可编译、可链接，
 * 创建必然失败、调用必然返回空，调用方据此跳过脚本逻辑即可。
 * 模块启用时 ENABLED_JS_SCRIPT 被定义，本文件内容为空，由 jsx.c 提供实现。
 */
#include <stdbool.h>
#include <stddef.h>

#include "joy_js.h"

#ifndef ENABLED_JS_SCRIPT
jsx_p jsx_create(void) { return NULL; }
void  jsx_release(jsx_p jsx) { (void)jsx; }
bool  jsx_dofile(jsx_p jsx, const char* filename) { (void)jsx; (void)filename; return false; }
bool  jsx_reload(jsx_p jsx, const char* filename) { (void)jsx; (void)filename; return false; }
void  jsx_dispatch_event(jsx_p jsx, const void* event) { (void)jsx; (void)event; }
bool  jsx_call_joy(jsx_p jsx, const char* func, int argc, float dt) {
        (void)jsx; (void)func; (void)argc; (void)dt; return false;
}
void jsx_set_watch(jsx_p jsx, const char* filename) { (void)jsx; (void)filename; }
bool jsx_poll_reload(jsx_p jsx) { (void)jsx; return false; }
const char* jsx_get_watch(jsx_p jsx) { (void)jsx; return NULL; }
void jsx_set_args(jsx_p jsx, int argc, char* argv[]) { (void)jsx; (void)argc; (void)argv; }

void jsopen_api(void* duk_context) { (void)duk_context; }
const char* jsopen_api_selftest(void) { return "js/disabled"; }
#endif /* ENABLED_JS_SCRIPT */
