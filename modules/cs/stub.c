/*
 * C# 后端关闭时的空桩：调用方 csx_* / csopen_* 调用仍可编译、可链接。
 * csx_create() 返回 NULL，宿主据此直接退出，不会真正走到这些函数。
 * 模块启用时 ENABLED_CS_SCRIPT 被定义，本文件内容为空，由 jcsx.c 提供实现。
 */
#include <stdbool.h>
#include <stddef.h>

#include "joy_csharp.h"

#ifndef ENABLED_CS_SCRIPT
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

/* 胶水层同理：未启用时不注册任何 internal call（声明见 joy_csharp.h） */
void csopen_generated(void) {}
void csopen_draw(void) {}
void csopen_input(void) {}
void csopen_host(void) {}

/* 输入桥 / 宿主状态同理：宿主只通过 joy_csharp.h 调用，关闭模块时也要能链接 */
void csx_input_key(int scancode, bool down) { (void)scancode; (void)down; }
void csx_input_mouse_move(float x, float y) { (void)x; (void)y; }
void csx_input_mouse_button(int button, bool down) { (void)button; (void)down; }
void csx_input_wheel(float dy) { (void)dy; }
void csx_input_end_frame(void) {}
void csx_host_request_quit(void) {}
bool csx_host_quit_requested(void) { return false; }
void csx_host_set_exit_code(int code) { (void)code; }
int  csx_host_exit_code(void) { return 0; }
#endif /* ENABLED_CS_SCRIPT */
