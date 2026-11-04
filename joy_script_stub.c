/*
 * 脚本后端桩实现（lua / js / csharp 共用）
 *
 * 三个脚本后端都是 joy 的可选模块（modules/lua、modules/js、modules/csharp）。
 * 模块关闭时其源码完全不参与编译，本文件保证：
 *   - 调用方代码里的 luax_* / jsx_* / csx_* 调用依然能编译、能链接；
 *   - 创建必然失败、调用必然返回空，调用方据此跳过脚本逻辑即可。
 *
 * 模块启用后由模块的 CMakeLists 以 PUBLIC 方式定义 ENABLED_XXX_SCRIPT，
 * 对应桩实现被屏蔽，由模块提供真实实现。
 */
#include <stdbool.h>
#include <stddef.h>

#include "modules/lua/joy_lua.h"
#include "modules/js/joy_js.h"
#include "modules/csharp/joy_csharp.h"

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
#endif /* ENABLED_LUA_SCRIPT */

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
#endif /* ENABLED_JS_SCRIPT */

#ifndef ENABLED_CSHARP_SCRIPT
csx_p csx_create(void) { return NULL; }
void  csx_release(csx_p csx) { (void)csx; }
bool  csx_load_assembly(csx_p csx, const char* filename) { (void)csx; (void)filename; return false; }
MonoClass* csx_find_class(csx_p csx, const char* namespace_name, const char* class_name) {
        (void)csx; (void)namespace_name; (void)class_name; return NULL;
}
MonoObject* csx_invoke_static_method(csx_p csx, const char* namespace_name,
        const char* class_name, const char* method_name, void** args) {
        (void)csx; (void)namespace_name; (void)class_name; (void)method_name; (void)args;
        return NULL;
}
void csx_register_internal_call(const char* name, const void* method) {
        (void)name; (void)method;
}
void csx_thread_attach(csx_p csx) { (void)csx; }

csx_method_p csx_find_method(csx_p csx, const char* namespace_name, const char* class_name,
                             const char* method_name, int param_count) {
        (void)csx; (void)namespace_name; (void)class_name; (void)method_name; (void)param_count;
        return NULL;
}
MonoObject* csx_invoke_method(csx_p csx, csx_method_p method, void** args) {
        (void)csx; (void)method; (void)args;
        return NULL;
}
void csx_method_free(csx_method_p method) { (void)method; }

/* 胶水层同理：未启用时不注册任何 internal call（实现位于 modules/csharp/csclib） */
void csopen_sdl(void) {}
void csopen_kcp(void) {}
void csopen_math(void) {}
void csopen_collision(void) {}
void csopen_sys(void) {}
void csopen_utils(void) {}
void csopen_generated(void) {}
void csopen_draw(void) {}
void csopen_input(void) {}
void csopen_host(void) {}
#endif /* ENABLED_CSHARP_SCRIPT */
