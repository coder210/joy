/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: utils.h
Description: ͨ�÷���
Author: ydlc
Version: 1.0
Date: 2021.3.20
History:
*************************************************/
#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>
#include <stdint.h>
#include "config.h"

typedef struct delay {
	uint64_t time;
	short timeout;
} delay_t;

typedef struct random_num random_num_t, * random_num_p;

#ifdef __cplusplus
extern "C" {
#endif

	JOY_API char* utils_strdup(const char* str, size_t* out_sz);
	JOY_API bool utils_get_current_time(uint64_t* ms_ticks);
	JOY_API void utils_current_datetime(const char* fmt, char* buf, size_t len);
	JOY_API char* utils_read_file(const char* filename, size_t* sz);
	JOY_API bool utils_check_file(const char* filename);
	JOY_API void utils_append_file(const char* filename, const char* data);
	JOY_API char* utils_read_script(const char* file, size_t* data_size);
	JOY_API short utils_bit2short(uint8_t* buf);
	JOY_API void  utils_short2bit(uint8_t* buf, short value);
	/* c# = BitConverter.FromUInt32 */
	JOY_API int utils_bit2int(uint8_t* buf);
	/* c# = BitConverter.ToUInt32 */
	JOY_API void utils_int2bit(uint8_t* buf, int value);
	JOY_API bool utils_wait_delay(delay_t* delay, uint64_t current_time);
	JOY_API short utils_md5x(const char* input, size_t len, char* output);

#ifdef __cplusplus
}
#endif


#endif // !UTILS_H
