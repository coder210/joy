#ifndef CONIFG_H
#define CONIFG_H

#if defined(_WIN32) || defined(_WIN64)
#       define JOY_WIN
#elif defined(__linux__)
#       define JOY_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
#       define JOY_MACOS
#elif defined(__FreeBSD__) && defined(__OpenBSD__)
#       define JOY_UNIX
#elif defined(__EMSCRIPTEN__)
#       define JOY_EMSCRIPTEN
#else
#       define JOY_UNKNOW_OS
#endif


#ifdef __cplusplus
#       define JOY_DECL extern "C"
#else
#       define JOY_DECL 
#endif


#if defined(JOY_STATIC)
// 静态库：不需要任何导入导出声明
#       define JOY_API
#elif defined(JOY_WIN) || defined(__CYGWIN__)
// Windows 或 Cygwin：使用 __declspec(dllexport/import)
#       define JOY_API __declspec(dllexport)
#elif defined(__GNUC__) && (__GNUC__ >= 4) || defined(__clang__)
// GCC >= 4 或 Clang：使用可见性属性
#       define JOY_API __attribute__((visibility("default")))
#else
// 其他编译器：无特殊声明
#       define JOY_API
#endif

#define JOY_INLINE static inline
#define JOY_MAX_BUFFER 1024
#define JOY_MIN_BUFFER 128
#define JOY_MAX_PATH 512
#define JOY_MAX_IP 127 


#endif // !CONIFG_H
