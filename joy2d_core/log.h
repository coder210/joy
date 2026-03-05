/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: log.h
Description: »’÷æ
Author: ydlc
Version: 1.0
Date: 2021.2.30
History:
*************************************************/
#ifndef LOG_H
#define LOG_H
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

	JOY_API int log_info(const char* format, ...);
	JOY_API int log_debug(const char* format, ...);
	JOY_API int log_error(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif // !LOG_H