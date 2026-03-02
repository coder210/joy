/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: log.h
Description: »’÷æ
Author: ydlc
Version: 1.0
Date: 2021.2.30
History:
*************************************************/
#ifndef CORE_LOG_H
#define CORE_LOG_H

int log_info(const char* format, ...);
int log_debug(const char* format, ...);
int log_error(const char* format, ...);

#endif // !CORE_LOG_H