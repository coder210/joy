/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-log.h
Description: »’÷æ
Author: ydlc
Version: 1.0
Date: 2021.2.30
History:
*************************************************/
#ifndef LIV_LOG_H
#define LIV_LOG_H

int liv_log_info(const char* format, ...);
int liv_log_debug(const char* format, ...);
int liv_log_error(const char* format, ...);

#endif // !LIV_LOG_H