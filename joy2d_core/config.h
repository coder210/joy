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

#ifdef JOY_WIN
#	define JOY_API __declspec(dllexport)
#else
#	define JOY_API  
#endif 

#define JOY_INLINE static inline
#define JOY_MAX_BUFFER 1024
#define JOY_MIN_BUFFER 128
#define JOY_MAX_PATH 512
#define JOY_MAX_IP 127 


#endif // !CORE_CONIFG_H
