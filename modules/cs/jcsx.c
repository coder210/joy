/* jcsx.c —— joy 的 C# 运行时宿主（CoreCLR / Godot 模式）。
 *
 * 与旧版（mono_jit_init + mono_add_internal_call）的本质差异：
 *   - 运行时：桌面用 hostfxr 启动真 CoreCLR；Android 用 libmonosgen 的
 *     coreclr_initialize（Godot fallback 路径）。全平台统一 coreclr_* 宿主 API。
 *   - native→managed：入口是 [UnmanagedCallersOnly] 静态方法，取函数指针直接调，
 *     不再用 mono_runtime_invoke 反射。
 *   - managed→native：C 侧维护 name→fn 函数指针表，只把一个 lookup(name) 函数
 *     指针交给 C#（Joy.NativeFuncs.Initialize），C# 按名字查表转成 delegate* 调用。
 *     不再用 mono_add_internal_call / MonoString / MonoArray。
 *
 * ICorDebug 诊断通道由真 CoreCLR 自动激活（VS 断点可用）；宿主侧只需
 * DOTNET_EnableDiagnostics=1（见 clr_host.cpp 的 VS 调试环境）。
 */
#ifdef ENABLED_CS_SCRIPT
#include "joy_csharp.h"
#include <joy/joy.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ---- 动态库加载（跨平台）---- */
#if defined(_WIN32)
#  include <windows.h>
#  include <direct.h>
#else
#  include <unistd.h>
#endif
#if defined(_WIN32)
#  define CSX_DL_LOAD(p)      LoadLibraryA(p)
#  define CSX_DL_SYM(h, n)    GetProcAddress((HMODULE)(h), n)
#  define CSX_DL_CLOSE(h)     FreeLibrary((HMODULE)(h))
#  define CSX_CDECL           __cdecl
#else
#  include <dlfcn.h>
#  define CSX_DL_LOAD(p)      dlopen((p), RTLD_NOW | RTLD_GLOBAL)
#  define CSX_DL_SYM(h, n)    dlsym((h), n)
#  define CSX_DL_CLOSE(h)     dlclose(h)
#  define CSX_CDECL
#endif

/* ---- hostfxr / coreclr 最小类型声明（避免依赖 SDK 头文件路径）----
 * 调用约定（MSVC x64 统一，x86 关键）：
 *   hostfxr_*         -> HOSTFXR_CALLTYPE = __cdecl
 *   load_asm_get_fp   -> CORECLR_DELEGATE_CALLTYPE = __stdcall
 *   coreclr_*         -> __cdecl
 *   UCO 入口函数       -> 由 [UnmanagedCallersOnly(CallConvCdecl)] 指定 = __cdecl
 */
#if defined(_WIN32)
typedef unsigned short csx_char_t;
#else
typedef char csx_char_t;
#endif
#define CSX_UCO_METHOD        ((const csx_char_t*)-1)

/* 组件模式（框架依赖专用）：hostfxr_initialize_for_runtime_config */
typedef int (__cdecl *csx_init_for_config_fn)(
    const csx_char_t* config_path, const void* params, void** out_ctx);
/* 命令行模式（Godot 导出版用这个，支持自包含）：hostfxr_initialize_for_dotnet_command_line。
 * argv[0] 必须是主程序集 DLL 路径（同目录需有 .runtimeconfig.json / .deps.json）。
 * 它等价于 `dotnet <argv[0]> <argv[1]...>`，但只初始化运行时、不执行 Main。 */
typedef int (__cdecl *csx_init_for_cmdline_fn)(
    int argc, const csx_char_t** argv, const void* params, void** out_ctx);
typedef int (__cdecl *csx_get_delegate_fn)(
    void* ctx, int type, void** out_delegate);
typedef int (__cdecl *csx_close_fn)(void* ctx);
#if defined(_WIN32)
typedef int (__stdcall *csx_load_asm_get_fp_fn)(
    const csx_char_t* asm_path, const csx_char_t* type_name,
    const csx_char_t* method_name, const csx_char_t* delegate_type_name,
    void* reserved, void** out_delegate);
#else
typedef int (*csx_load_asm_get_fp_fn)(
    const csx_char_t* asm_path, const csx_char_t* type_name,
    const csx_char_t* method_name, const csx_char_t* delegate_type_name,
    void* reserved, void** out_delegate);
#endif

/* hdt_load_assembly_and_get_function_pointer = 5（hostfxr.h 枚举顺序：
 * com_activation=0, load_in_memory_assembly=1, winrt_activation=2,
 * com_register=3, com_unregister=4, load_assembly_and_get_function_pointer=5）。 */
#define CSX_HDT_LOAD_ASM_GET_FP 5

/* coreclr_initialize / coreclr_create_delegate（Android fallback，__cdecl）*/
#if defined(_WIN32)
#  define CSX_CC __cdecl
#else
#  define CSX_CC
#endif
typedef int (CSX_CC *csx_coreclr_init_fn)(
    const char* exe_path, const char* friendly_name, int prop_count,
    const char** keys, const char** values, void** out_host, unsigned int* out_domain);
typedef int (CSX_CC *csx_coreclr_create_delegate_fn)(
    void* host, unsigned int domain, const char* asm_name,
    const char* type_name, const char* method_name, void** out_delegate);
typedef int (CSX_CC *csx_coreclr_shutdown_fn)(void* host, unsigned int domain);

/* ---- internal call 函数指针表（name -> fn）---- */
#define CSX_MAX_ICALLS 1024
typedef struct { const char* name; const void* fn; } csx_icall_t;
static csx_icall_t s_icalls[CSX_MAX_ICALLS];
static int s_icall_count = 0;

void csx_register_internal_call(const char* name, const void* method)
{
        if (!name || !method) return;
        if (s_icall_count >= CSX_MAX_ICALLS) {
                joy_core_log_error("internal call table full (%d)", CSX_MAX_ICALLS);
                return;
        }
        s_icalls[s_icall_count].name = name;
        s_icalls[s_icall_count].fn = method;
        s_icall_count++;
}

/* 交给 C# 的查表函数：const char* name -> void* fn（Cdecl）。*/
static void* CSX_CDECL csx_lookup_internal_call(const char* name)
{
        if (!name) return NULL;
        for (int i = 0; i < s_icall_count; ++i) {
                if (strcmp(s_icalls[i].name, name) == 0)
                        return (void*)s_icalls[i].fn;
        }
        joy_core_log_error("internal call not found: %s", name);
        return NULL;
}

/* ---- 宿主状态 ---- */
struct joy_csx {
        void* host_handle;        /* coreclr host handle（Android）*/
        unsigned int domain_id;   /* coreclr domain id（Android）*/
        void* fxr_ctx;            /* hostfxr context（桌面）*/
        void* runtime_lib;        /* hostfxr / coreclr 动态库句柄 */
        csx_load_asm_get_fp_fn load_asm_get_fp;  /* load_assembly_and_get_function_pointer */
        csx_coreclr_create_delegate_fn create_delegate; /* coreclr_create_delegate（Android）*/
        csx_coreclr_shutdown_fn shutdown;              /* coreclr_shutdown（Android）*/
        csx_close_fn fxr_close;                        /* hostfxr_close（桌面）*/
        bool android;
        char asm_dir[1024];       /* 程序集所在目录（绝对路径，用于拼接 dll）*/
};

/* 缓存的入口函数指针。UCO 方法的函数指针就是可直接调用的地址。 */
struct joy_csx_method {
        void* fn;
        int param_count;
};

/* ---- 工具：取 exe 所在目录（CWD 已被 clr_host 对齐到 exe 目录）---- */
static void csx_get_cwd(char* buf, size_t n)
{
#if defined(_WIN32)
        if (!_getcwd(buf, (int)n)) buf[0] = '\0';
#else
        if (!getcwd(buf, n)) buf[0] = '\0';
#endif
}

/* ---- 桌面：用 hostfxr 启动真 CoreCLR，拿 load_assembly_and_get_function_pointer ---- */
#if !defined(__ANDROID__)
static bool csx_init_desktop(csx_p csx)
{
        /* 两种部署模式（按优先级探测）：
         *  1) 自包含（self-contained）：exe 目录带 hostfxr.dll + coreclr.dll + BCL。
         *     用 exe 目录的 hostfxr，从同目录加载运行时，不需要系统 DOTNET_ROOT。
         *  2) 框架依赖（framework-dependent）：exe 目录无 hostfxr，用 CMake 探测到的
         *     系统 hostfxr 绝对路径 + DOTNET_ROOT，framework 解析到系统 .NET。
         * 环境变量 JOY_HOSTFXR_PATH / JOY_RUNTIMECONFIG 可覆盖，便于排障。 */
        const char* env_fxr = getenv("JOY_HOSTFXR_PATH");
        char local_fxr[1200];
        csx_get_cwd(local_fxr, sizeof(local_fxr));
        if (local_fxr[0]) {
                size_t n = strlen(local_fxr);
                if (local_fxr[n - 1] != '/' && local_fxr[n - 1] != '\\') strcat(local_fxr, "/");
        }
#if defined(_WIN32)
        strcat(local_fxr, "hostfxr.dll");
#elif defined(__APPLE__)
        strcat(local_fxr, "libhostfxr.dylib");
#else
        strcat(local_fxr, "libhostfxr.so");
#endif
        bool self_contained = false;
        FILE* probe = fopen(local_fxr, "rb");
        if (probe) { fclose(probe); self_contained = true; }

        const char* fxr_path = (env_fxr && *env_fxr) ? env_fxr
                             : (self_contained ? local_fxr :
#if defined(JOY_HOSTFXR_PATH_MACRO)
                               JOY_HOSTFXR_PATH_MACRO
#else
                               "hostfxr.dll"
#endif
                               );
        /* 仅框架依赖模式才需要系统 DOTNET_ROOT；自包含模式设了反而干扰。 */
        if (!self_contained && !(env_fxr && *env_fxr)) {
#if defined(JOY_DOTNET_ROOT)
                if (!getenv("DOTNET_ROOT") || !*getenv("DOTNET_ROOT")) {
#  if defined(_WIN32)
                        _putenv_s("DOTNET_ROOT", JOY_DOTNET_ROOT);
#  else
                        setenv("DOTNET_ROOT", JOY_DOTNET_ROOT, 1);
#  endif
                }
#endif
        }
        const char* runtimeconfig = getenv("JOY_RUNTIMECONFIG");
        if (!runtimeconfig || !*runtimeconfig) runtimeconfig = "JoyGame.runtimeconfig.json";
        joy_core_log_info("hostfxr mode: %s, path=%s", self_contained ? "self-contained" : "framework-dependent", fxr_path);

        void* lib = CSX_DL_LOAD(fxr_path);
        if (!lib) {
                joy_core_log_error("load hostfxr failed: %s", fxr_path);
                return false;
        }
        csx_init_for_cmdline_fn init_for_cmdline =
                (csx_init_for_cmdline_fn)CSX_DL_SYM(lib, "hostfxr_initialize_for_dotnet_command_line");
        csx_init_for_config_fn init_for_config =
                (csx_init_for_config_fn)CSX_DL_SYM(lib, "hostfxr_initialize_for_runtime_config");
        csx_get_delegate_fn get_delegate =
                (csx_get_delegate_fn)CSX_DL_SYM(lib, "hostfxr_get_runtime_delegate");
        csx_close_fn close_fn =
                (csx_close_fn)CSX_DL_SYM(lib, "hostfxr_close");
        if (!get_delegate || !close_fn || (!init_for_cmdline && !init_for_config)) {
                joy_core_log_error("hostfxr symbols missing");
                CSX_DL_CLOSE(lib);
                return false;
        }

        /* Windows 下 hostfxr 需要 UTF-16 字符串。 */
#if defined(_WIN32)
        #define CSX_WSTR(dst, src, cap) \
                wchar_t dst[cap]; MultiByteToWideChar(CP_ACP, 0, (src), -1, dst, (cap))
#else
        #define CSX_WSTR(dst, src, cap) const char* dst = (src)
#endif

        void* ctx = NULL;
        int rc = -1;
        const char* mode = "component";

        if (init_for_cmdline) {
                /* 命令行模式（Godot 导出版的做法）：argv[0] = 主程序集 DLL 完整路径。
                 * 需要 exe 目录有 JoyGame.runtimeconfig.json（自包含版无 framework 节点）。
                 * 只初始化运行时，不执行 Main——UCO 入口由 load_asm_get_fp 单独取。 */
                char main_asm[1200];
                csx_get_cwd(main_asm, sizeof(main_asm));
                if (main_asm[0]) {
                        size_t n = strlen(main_asm);
                        if (main_asm[n - 1] != '/' && main_asm[n - 1] != '\\') strcat(main_asm, "/");
                }
                strcat(main_asm, "JoyGame.dll");
                CSX_WSTR(wasm, main_asm, 1200);
                const csx_char_t* argv[1] = { (const csx_char_t*)wasm };
                rc = init_for_cmdline(1, argv, NULL, &ctx);
                if (rc == 0 && ctx) {
                        mode = "command-line";
                        joy_core_log_info("hostfxr command-line mode OK (argv[0]=%s)", main_asm);
                } else {
                        joy_core_log_error("hostfxr_initialize_for_dotnet_command_line failed: 0x%x (fallback to component)", rc);
                        ctx = NULL;
                }
        }

        /* 回退：组件模式（框架依赖，旧行为）。命令行模式失败时才走这里。 */
        if ((!ctx || rc != 0) && init_for_config) {
                CSX_WSTR(wcfg, runtimeconfig, 1024);
                rc = init_for_config((const csx_char_t*)wcfg, NULL, &ctx);
                if (rc == 0 && ctx) mode = "component";
        }
        if (rc < 0 || !ctx) {
                joy_core_log_error("hostfxr init failed: 0x%x", rc);
                CSX_DL_CLOSE(lib);
                return false;
        }

        void* delegate = NULL;
        rc = get_delegate(ctx, CSX_HDT_LOAD_ASM_GET_FP, &delegate);
        if (rc < 0 || !delegate) {
                joy_core_log_error("hostfxr_get_runtime_delegate failed: 0x%x", rc);
                close_fn(ctx);
                CSX_DL_CLOSE(lib);
                return false;
        }
        csx->runtime_lib = lib;
        csx->fxr_ctx = ctx;
        csx->fxr_close = close_fn;
        csx->load_asm_get_fp = (csx_load_asm_get_fp_fn)delegate;
        csx->android = false;
        joy_core_log_info("CoreCLR initialized via hostfxr (%s mode)", mode);
        return true;
#undef CSX_WSTR
}
#endif

/* ---- Android：libmonosgen 的 coreclr_initialize + coreclr_create_delegate ---- */
#if defined(__ANDROID__)
static bool csx_init_android(csx_p csx)
{
        void* lib = CSX_DL_LOAD("libmonosgen-2.0.so");
        if (!lib) {
                joy_core_log_error("load libmonosgen failed");
                return false;
        }
        csx_coreclr_init_fn coreclr_init =
                (csx_coreclr_init_fn)CSX_DL_SYM(lib, "coreclr_initialize");
        csx_coreclr_create_delegate_fn create_delegate =
                (csx_coreclr_create_delegate_fn)CSX_DL_SYM(lib, "coreclr_create_delegate");
        csx_coreclr_shutdown_fn shutdown_fn =
                (csx_coreclr_shutdown_fn)CSX_DL_SYM(lib, "coreclr_shutdown");
        if (!coreclr_init || !create_delegate || !shutdown_fn) {
                joy_core_log_error("coreclr symbols missing on Android libmonosgen");
                CSX_DL_CLOSE(lib);
                return false;
        }

        /* TPA：解包目录下的 BCL + JoyGame.dll + Joy.Bindings.dll。
         * clr_host.cpp 已把它们解到 SDL_GetPrefPath 并把路径放进 JOY_CLR_LIB_DIR。 */
        const char* libdir = getenv("JOY_CLR_LIB_DIR");
        if (!libdir || !*libdir) libdir = ".";
        char tpa[4096];
        snprintf(tpa, sizeof(tpa),
                 "%s/System.Private.CoreLib.dll:%s/System.Runtime.dll:%s/netstandard.dll:"
                 "%s/System.Collections.dll:%s/Joy.Bindings.dll:%s/JoyGame.dll",
                 libdir, libdir, libdir, libdir, libdir, libdir);
        const char* keys[] = { "TRUSTED_PLATFORM_ASSEMBLIES", "APP_PATHS", "NATIVE_DLL_SEARCH_DIRECTORIES" };
        const char* vals[] = { tpa, libdir, libdir };
        void* host = NULL; unsigned int domain = 0;
        int rc = coreclr_init("joy", "joy", 3, keys, vals, &host, &domain);
        if (rc < 0 || !host) {
                joy_core_log_error("coreclr_initialize failed: 0x%x", rc);
                CSX_DL_CLOSE(lib);
                return false;
        }
        csx->runtime_lib = lib;
        csx->host_handle = host;
        csx->domain_id = domain;
        csx->create_delegate = create_delegate;
        csx->shutdown = shutdown_fn;
        csx->android = true;
        snprintf(csx->asm_dir, sizeof(csx->asm_dir), "%s", libdir);
        joy_core_log_info("CoreCLR initialized via libmonosgen (Android)");
        return true;
}
#endif

/* ---- 生命周期 ---- */
csx_p csx_create(void)
{
        csx_p csx = (csx_p)calloc(1, sizeof(csx_t));
        if (!csx) return NULL;

        /* 函数指针表由各 csopen_* 填充（与 CLR 无关），必须在初始化 Runtime 前完成。 */
        csopen_input();
        csopen_host();
        csopen_api();
        csopen_borrow();
        csopen_r3d();
        csopen_r3d_plus();
        csopen_crypto();
        csopen_collision();

#if defined(__ANDROID__)
        bool ok = csx_init_android(csx);
#else
        bool ok = csx_init_desktop(csx);
#endif
        if (!ok) {
                free(csx);
                return NULL;
        }
        return csx;
}

bool csx_load_assembly(csx_p csx, const char* filename)
{
        if (!csx) return false;
        (void)filename; /* 入口在 csx_find_method 里按固定程序集名加载，这里只用 filename 建路径 */

        /* 取绑定初始化入口并调用。必须从 JoyGame.dll 取 GameApplication.Bootstrap，
         * 让它内部的 NativeFuncs.Initialize 与 Initialize/Update/Shutdown 落在同一个
         * AssemblyLoadContext 实例上——否则绑定填在 A、业务调用落在 B（函数指针全 NULL）。 */
        void* initialize_fn = NULL;
#if !defined(__ANDROID__)
        if (csx->load_asm_get_fp) {
                char bindings_path[1200];
                csx_get_cwd(bindings_path, sizeof(bindings_path));
                if (bindings_path[0]) {
                        size_t n = strlen(bindings_path);
                        if (bindings_path[n - 1] != '/' && bindings_path[n - 1] != '\\')
                                strcat(bindings_path, "/");
                }
                strcat(bindings_path, "JoyGame.dll");
#if defined(_WIN32)
                wchar_t wpath[1200], wtype[256], wmethod[128];
                MultiByteToWideChar(CP_ACP, 0, bindings_path, -1, wpath, 1200);
                MultiByteToWideChar(CP_ACP, 0, "JoyGame.GameApplication, JoyGame", -1, wtype, 256);
                MultiByteToWideChar(CP_ACP, 0, "Bootstrap", -1, wmethod, 128);
                int rc = csx->load_asm_get_fp((csx_char_t*)wpath, (csx_char_t*)wtype,
                                             (csx_char_t*)wmethod, CSX_UCO_METHOD, NULL, &initialize_fn);
                if (rc < 0 || !initialize_fn) {
                        joy_core_log_error("load GameApplication.Bootstrap from %s failed: 0x%x", bindings_path, rc);
                        return false;
                }
#else
                int rc = csx->load_asm_get_fp((csx_char_t*)bindings_path,
                                             (csx_char_t*)"JoyGame.GameApplication, JoyGame",
                                             (csx_char_t*)"Bootstrap", CSX_UCO_METHOD, NULL, &initialize_fn);
                if (rc < 0 || !initialize_fn) {
                        joy_core_log_error("load GameApplication.Bootstrap from %s failed: 0x%x", bindings_path, rc);
                        return false;
                }
#endif
        }
#else
        if (csx->create_delegate) {
                int rc = csx->create_delegate(csx->host_handle, csx->domain_id,
                                              "JoyGame", "JoyGame.GameApplication", "Bootstrap", &initialize_fn);
                if (rc < 0 || !initialize_fn) {
                        joy_core_log_error("create_delegate GameApplication.Bootstrap failed: 0x%x", rc);
                        return false;
                }
        }
#endif
        if (!initialize_fn) {
                joy_core_log_error("no way to load NativeFuncs.Initialize");
                return false;
        }

        /* UCO 签名：void Initialize(IntPtr lookupFn)，Cdecl。 */
        typedef void (CSX_CDECL *init_bindings_fn)(void*);
        ((init_bindings_fn)initialize_fn)((void*)&csx_lookup_internal_call);
        joy_core_log_info("Managed bindings initialized, %d native calls registered", s_icall_count);
        return true;
}

MonoClass* csx_find_class(csx_p csx, const char* namespace_name, const char* class_name)
{
        /* CoreCLR 模式不再按名字反射类；此 API 保留仅为 ABI 兼容，返回 NULL。 */
        (void)csx; (void)namespace_name; (void)class_name;
        return NULL;
}

MonoObject* csx_invoke_static_method(csx_p csx, const char* namespace_name,
        const char* class_name, const char* method_name, void** args)
{
        (void)csx; (void)namespace_name; (void)class_name; (void)method_name; (void)args;
        joy_core_log_error("csx_invoke_static_method not supported in CoreCLR mode");
        return NULL;
}

static void* csx_load_entry(csx_p csx, const char* asm_file, const char* type_name, const char* method_name)
{
        void* fn = NULL;
        if (csx->android) {
                int rc = csx->create_delegate(csx->host_handle, csx->domain_id,
                                              "JoyGame", type_name, method_name, &fn);
                if (rc < 0 || !fn)
                        joy_core_log_error("create_delegate %s failed: 0x%x", method_name, rc);
                return fn;
        }
#if defined(_WIN32)
        char asm_path[1200];
        csx_get_cwd(asm_path, sizeof(asm_path));
        if (asm_path[0]) {
                size_t n = strlen(asm_path);
                if (asm_path[n - 1] != '/' && asm_path[n - 1] != '\\') strcat(asm_path, "/");
        }
        strcat(asm_path, asm_file);
        wchar_t wpath[1200], wtype[256], wmethod[128];
        MultiByteToWideChar(CP_ACP, 0, asm_path, -1, wpath, 1200);
        MultiByteToWideChar(CP_ACP, 0, type_name, -1, wtype, 256);
        MultiByteToWideChar(CP_ACP, 0, method_name, -1, wmethod, 128);
        int rc = csx->load_asm_get_fp((csx_char_t*)wpath, (csx_char_t*)wtype,
                                     (csx_char_t*)wmethod, CSX_UCO_METHOD, NULL, &fn);
        if (rc < 0 || !fn) joy_core_log_error("load entry %s failed: 0x%x", method_name, rc);
#else
        int rc = csx->load_asm_get_fp((csx_char_t*)asm_file, (csx_char_t*)type_name,
                                     (csx_char_t*)method_name, CSX_UCO_METHOD, NULL, &fn);
        if (rc < 0 || !fn) joy_core_log_error("load entry %s failed: 0x%x", method_name, rc);
#endif
        return fn;
}

csx_method_p csx_find_method(csx_p csx, const char* namespace_name, const char* class_name,
                             const char* method_name, int param_count)
{
        if (!csx) return NULL;
        (void)namespace_name;
        /* load_assembly_and_get_function_pointer 要求 assembly-qualified 类型名：
         * "Namespace.Type, AssemblyName"（与 spike 的 "ManagedSmoke.Entry, ManagedSmoke" 一致）。
         * class_name 形如 "GameApplication"，namespace_name 形如 "JoyGame"。 */
        char type_name[256];
        snprintf(type_name, sizeof(type_name), "%s.%s, %s",
                 namespace_name, class_name, namespace_name);
        void* fn = csx_load_entry(csx, "JoyGame.dll", type_name, method_name);
        if (!fn) return NULL;
        csx_method_p m = (csx_method_p)malloc(sizeof(csx_method_t));
        if (!m) return NULL;
        m->fn = fn;
        m->param_count = param_count;
        return m;
}

MonoObject* csx_invoke_method(csx_p csx, csx_method_p method, void** args)
{
        (void)csx;
        if (!method || !method->fn) return NULL;
        switch (method->param_count) {
        case 0:
                ((void (CSX_CDECL *)(void))method->fn)();
                break;
        case 1:
                /* 唯一 1 参入口：Update(float dt) */
                if (args && args[0])
                        ((void (CSX_CDECL *)(float))method->fn)(*(float*)args[0]);
                break;
        case 3:
                /* 唯一 3 参入口：Initialize(int,int,int) */
                if (args && args[0] && args[1] && args[2])
                        ((void (CSX_CDECL *)(int, int, int))method->fn)(
                            *(int*)args[0], *(int*)args[1], *(int*)args[2]);
                break;
        default:
                joy_core_log_error("csx_invoke_method: unsupported param_count %d", method->param_count);
                break;
        }
        return NULL; /* 入口均返回 void */
}

void csx_method_free(csx_method_p method) { free(method); }

void csx_thread_attach(csx_p csx) { (void)csx; /* CoreCLR 宿主管理的线程无需手动 attach */ }

void csx_release(csx_p csx)
{
        if (!csx) return;
        if (csx->fxr_ctx && csx->fxr_close) csx->fxr_close(csx->fxr_ctx);
        if (csx->android && csx->shutdown) csx->shutdown(csx->host_handle, csx->domain_id);
        if (csx->runtime_lib) CSX_DL_CLOSE(csx->runtime_lib);
        free(csx);
}

#endif // ENABLED_CS_SCRIPT

/*
 * 未启用本模块时的桩实现已统一移到 modules/cs/stub.c
 * （模块关闭时本文件不参与编译，桩必须由核心侧提供）。
 */
