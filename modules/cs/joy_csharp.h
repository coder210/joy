#ifndef JOY_CSHARP_H
#define JOY_CSHARP_H

/*
 * joy_csharp —— joy 的可选 C# 脚本后端对外唯一入口。
 *
 * 该模块不构建独立的库 / DLL：选中 MODULES cs 后，joy_add.cmake 会把
 * modules/cs 下的源码（含 bindings/ 胶水）直接编入调用方 target。
 * 宿主只需要 #include 本头，不必（也不应）包含 bindings/ 里的实现细节。
 *
 * 未启用模块时，stub.c 依据 ENABLED_CS_SCRIPT 提供空实现，
 * 下述函数依然可编译、可链接（创建必然失败）。
 *
 * 依赖：joy 公共 API <joy/joy.h>；Mono 开发包仅模块内部源码需要。
 */

#include <stdbool.h>

/*
 * Mono 句柄在此只作不透明前置声明：使用方无需安装 Mono 开发包也能包含本头。
 */
typedef struct _MonoClass MonoClass;
typedef struct _MonoObject MonoObject;
typedef struct _MonoMethod MonoMethod;
typedef struct csx_method csx_method_t, * csx_method_p;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct csx csx_t, * csx_p;

/* 创建 / 销毁运行时句柄 */
csx_p csx_create(void);
void  csx_release(csx_p csx);

/* 从 .dll(程序集) 加载 C# 代码 */
bool csx_load_assembly(csx_p csx, const char* filename);

/* 反射查找类 / 调用静态方法 / 注册内部调用(C# 调用 C 的入口) */
MonoClass*  csx_find_class(csx_p csx, const char* namespace_name, const char* class_name);
MonoObject* csx_invoke_static_method(csx_p csx, const char* namespace_name,
                                     const char* class_name, const char* method_name,
                                     void** args);
void csx_register_internal_call(const char* name, const void* method);

/* ---- 方法句柄缓存 ----
 * 帧回调这类高频调用先 csx_find_method 查一次，之后 csx_invoke_method 直接调；
 * param_count 传 -1 表示忽略参数个数。 */
csx_method_p csx_find_method(csx_p csx, const char* namespace_name, const char* class_name,
                             const char* method_name, int param_count);
MonoObject* csx_invoke_method(csx_p csx, csx_method_p method, void** args);
void csx_method_free(csx_method_p method);

/* 工作线程调用 managed 代码前确保已 attach（幂等） */
void csx_thread_attach(csx_p csx);

/* ---- internal call 注册入口 ----
 * 由 csx_create() 内部调用；未启用模块时 stub.c 提供空实现。
 * 新增注册分组时在此登记声明，不要在别处另立对外头文件。
 * host 组（输入桥 / 宿主服务）不属于脚本胶水，始终参与构建。 */
void csopen_input(void);
void csopen_host(void);
void csopen_api(void);            /* IDL 驱动的共享 thunk 薄适配 */
void csopen_borrow(void);         /* 宿主自建对象 → 脚本层借用句柄 */

/* ---- 宿主 → 胶水层的输入桥 ----
 * 宿主把 SDL 事件写进来，C# 侧通过 Joy.Input 轮询，因此 C# 不需要接触 SDL_Event。 */
void csx_input_key(int scancode, bool down);
void csx_input_mouse_move(float x, float y);
void csx_input_mouse_button(int button, bool down);
void csx_input_wheel(float dy);
void csx_input_end_frame(void);       /* 每帧末尾清一次性状态 */

/* ---- 宿主状态 ---- */
void csx_host_request_quit(void);     /* C# 请求退出主循环 */
bool csx_host_quit_requested(void);
void csx_host_set_exit_code(int code);/* 冒烟失败等场景设置进程退出码 */
int  csx_host_exit_code(void);

#ifdef __cplusplus
}
#endif
#endif /* JOY_CSHARP_H */
