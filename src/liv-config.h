#ifndef LIV_CONIFG_H
#define LIV_CONIFG_H

#ifdef __cplusplus
#       define LIVNET_DECL extern "C"
#else
#       define LIVNET_DECL 
#endif


#ifdef LIVNET_BUILD_AS_DLL
#	ifdef LIVNET_WIN
#		define LIVNET_API __declspec(dllexport)
#	else
#		define LIVNET_API  
#	endif 
#else
#       define LIVNET_API 
#endif


#if defined(_WIN32) || defined(_WIN64)
#       define LIVNET_WIN
#elif defined(__linux__)
#       define LIVNET_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
#       define LIVNET_MACOS
#elif defined(__FreeBSD__) && defined(__OpenBSD__)
#       define LIVNET_UNIX
#elif defined(__EMSCRIPTEN__)
#       define LIVNET_EMSCRIPTEN
#else
#       define LIVNET_UNKNOW_OS
#endif


#define LIVNET_INLINE static inline
#define LIVNET_MAX_BUFFER 1024
#define LIVNET_MIN_BUFFER 128
#define LIVNET_MAX_PATH 512
#define LIVNET_MAX_IP 127 


#endif // !LIV_CONIFG_H
