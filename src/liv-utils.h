/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-utils.h
Description: 通用方法
Author: ydlc
Version: 1.0
Date: 2021.3.20
History:
*************************************************/
#ifndef LIVNET_UTILS_H
#define LIVNET_UTILS_H
#include <time.h>
#include "external/md5.h"
#include "external/base64.h"
#include "external/aes_util.h"
#include "external/des.h"
#include "external/kcp.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liv-cont.h"
#include "liv-config.h"
#include "liv-sys.h"
#include <stdint.h>
#include <string.h>


#define MT_N 624
#define MT_M 397


typedef struct delay {
	uint64_t time;
	short timeout;
}delay_t;


typedef struct random_num {
	int index;
	uint32_t mt[MT_N];
}random_num_t, *random_num_p;


static char* 
utils_strdup(const char* str, size_t* out_sz)
{
	size_t sz = (int)SDL_strlen(str);
	char* ret = (char*)SDL_malloc(sz + 1);
	if (ret)
		memcpy(ret, str, sz + 1);
	*out_sz = sz;
	return ret;
}

static bool
utils_get_current_time(uint64_t *ms_ticks)
{
	bool resval;
	SDL_Time ticks;
	resval = SDL_GetCurrentTime(&ticks);
	*ms_ticks = ticks / 1000000;
	return resval;
}

static void 
utils_current_datetime(const char* fmt, char* buf, size_t len)
{
	time_t t;
	time(&t);
	struct tm* now = (struct tm*)localtime(&t);
	strftime(buf, len, fmt, now);
}

static char* 
utils_read_file(const char* filename, size_t* sz)
{
	FILE* fp;
	char* buf;
	long total_size;

	fp = fopen(filename, "rb");
	if (!fp) {
		*sz = 0;
		return NULL;
	}
	fseek(fp, 0, SEEK_END);
	total_size = ftell(fp);
	rewind(fp);
	buf = (char*)SDL_malloc((total_size + 1) * sizeof(char));
	if (buf)
		*sz = fread(buf, 1, total_size, fp);
	else
		*sz = 0;
	fclose(fp);
	return buf;
}

static bool 
utils_check_file(const char* filename)
{
	FILE* file = fopen(filename, "r");
	if (file) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

static void 
utils_append_file(const char* filename, const char* data)
{
	FILE* fp = fopen(filename, "a+");
	if (fp != NULL) {
		fprintf(fp, "%s\n", data);
		fclose(fp);
	}
}

static short 
utils_bit2short(uint8_t* buf)
{
	short r = (short)buf[0] << 8 | (short)buf[1];
	return r;
}

static void 
utils_short2bit(uint8_t* buf, short value)
{
	buf[0] = (value >> 8) & 0xff;
	buf[1] = value & 0xff;
}

/*
* c# = BitConverter.FromUInt32
*/
static int 
utils_bit2int(uint8_t* buf)
{
	int r = buf[0];
	r |= (int)buf[1] << 8;
	r |= (int)buf[2] << 16;
	r |= (int)buf[3] << 24;
	return r;
}

/*
* c# = BitConverter.ToUInt32
*/
static void 
utils_int2bit(uint8_t* buf, int value)
{
	buf[3] = ((0xff000000 & value) >> 24);
	buf[2] = ((0xff0000 & value) >> 16);
	buf[1] = ((0xff00 & value) >> 8);
	buf[0] = (0xff & value);
}

static bool
utils_wait_delay(delay_t* delay, uint64_t current_time)
{
	if (current_time - delay->time >= delay->timeout) {
		delay->time = current_time;
		return true;
	}
	return false;
}

static short 
utils_md5x(const char* input, size_t len, char* output)
{
	char value[16] = { 0 };
	md5(input, len, value);
	for (int i = 0; i < 16; i++) {
		char buf[4] = { 0 };
		sprintf(buf, "%02x", (unsigned char)value[i]);
		strcat(output, buf);
	}
	return 32;
}

static short 
utils_md5(const char* input, size_t len, char* output)
{
	md5(input, len, output);
	return 16;
}

static char* 
utils_b64_encode(const char* input, size_t len)
{
	char* result = b64_encode(input, len);
	return result;
}

static char* 
utils_b64_decode(const char* input, size_t len, size_t* out_len)
{
	char* result = b64_decode_ex(input, len, out_len);
	return result;
}

static unsigned char* 
utils_aes_encrypt256(const char* input, size_t* len,
	const char* key, const char* iv)
{
	size_t input_len;
	unsigned char* output;

	input_len = (int)SDL_strlen(input);
	output = aes_encrypt256x(input, input_len, len, key, iv);

	return output;
}

/*
* key len is equal 32
*/
static unsigned char*
utils_aes_decrypt256(const char* input, size_t* out_len,
	const char* key, const char* iv)
{
	unsigned char* output;
	size_t input_len;

	input_len = (int)SDL_strlen(input);
	output = aes_decrypt256x(input, input_len, out_len, key, iv);

	return output;
}

/*
* key len is equal 16
*/
static unsigned char* 
utils_aes_encrypt128(const char* input, size_t* out_len,
	const char* key, const char* iv)
{
	unsigned char* output;
	int input_len;

	input_len = (int)SDL_strlen(input);
	output = aes_encrypt128x(input, input_len, out_len, key, iv);
	return output;
}

static unsigned char*
utils_aes_decrypt128(const char* input, size_t* out_len,
	const char* key, const char* iv)
{
	unsigned char* output;
	int input_len;

	input_len = (int)SDL_strlen(input);
	output = aes_decrypt128x(input, input_len, out_len, key, iv);

	return output;
}


static char* 
utils_des_crypt(const char* key, const char* text,
	size_t textsz, size_t* out_len)
{
	uint32_t SK[32];
	uint8_t* buf;
	const uint8_t* utext;
	size_t keysz;
	int chunksz, bytes, i, j;
	uint8_t tail[8];

	keysz = (int)SDL_strlen(key);
	des_main_ks(SK, key);
	utext = (const uint8_t*)text;
	chunksz = (textsz + 8) & ~7;
	buf = SDL_malloc(sizeof(uint8_t) * chunksz);
	for (i = 0; i < (int)textsz - 7; i += 8) {
		des_crypt(SK, utext + i, buf + i);
	}
	bytes = textsz - i;
	for (j = 0; j < 8; j++) {
		if (j < bytes) {
			tail[j] = utext[i + j];
		}
		else if (j == bytes) {
			tail[j] = 0x80;
		}
		else {
			tail[j] = 0;
		}
	}
	des_crypt(SK, tail, buf + i);
	*out_len = chunksz;
	return buf;
}

static char*
utils_des_decrypt(const char* key, const char* text, size_t textsz, size_t* out_len)
{
	uint32_t ESK[32];
	uint32_t SK[32];
	int i, padding;
	size_t keysz;
	const uint8_t* utext;
	uint8_t* buf;

	padding = 1;
	keysz = (int)SDL_strlen(key);
	des_main_ks(ESK, key);
	utext = (const uint8_t*)text;

	for (i = 0; i < 32; i += 2) {
		SK[i] = ESK[30 - i];
		SK[i + 1] = ESK[31 - i];
	}

	if ((textsz & 7) || textsz == 0) {
		return NULL;
	}

	buf = SDL_malloc(sizeof(uint8_t) * textsz);
	for (i = 0; i < textsz; i += 8) {
		des_crypt(SK, utext + i, buf + i);
	}

	for (i = textsz - 1; i >= textsz - 8; i--) {
		if (buf[i] == 0) {
			padding++;
		}
		else if (buf[i] == 0x80) {
			break;
		}
		else {
			return NULL;
		}
	}
	if (padding > 8) {
		return NULL;
	}
	*out_len = textsz - padding;
	return buf;
}

static char*
utils_read_script(const char* file, size_t* data_size)
{
	size_t raw_data_size;
	char* raw_data, * data;
	const char* key;

	key = "com.livnet.liwei";
	raw_data = SDL_LoadFile(file, &raw_data_size);
	if (raw_data) {
		data = utils_des_decrypt(key, raw_data, raw_data_size, data_size);
		if (data) {
			data[*data_size] = 0;
			SDL_free(raw_data);
		}
		else {
			data = raw_data;
			*data_size = raw_data_size;
		}
	}
	else {
		data = NULL;
		*data_size = 0;
	}

	return data;
}

static random_num_p
utils_random_create(uint32_t seed)
{
	random_num_p rnum;
	rnum = (random_num_p)SDL_malloc(sizeof(random_num_t));
	SDL_assert(rnum);
	rnum->mt[0] = seed;
	rnum->index = 0;
	for (int i = 1; i < MT_N; i++) {
		rnum->mt[i] = (1812433253 * (rnum->mt[i - 1]
			^ (rnum->mt[i - 1] >> 30)) + i);
	}
	return rnum;
}

static void
utils_random_destroy(random_num_p rnum)
{
	SDL_free(rnum);
}

static uint32_t
utils_random_next(random_num_p rnum)
{
	const uint32_t matrix_a = 0x9908b0df;
	const uint32_t upper_mask = 0x80000000;
	const uint32_t lower_mask = 0x7fffffff;
	if (rnum->index == 0) {
		for (int i = 0; i < MT_N; i++) {
			uint32_t y = (rnum->mt[i] & upper_mask)
				| (rnum->mt[(i + 1) % MT_N] & lower_mask);
			rnum->mt[i] = rnum->mt[(i + MT_M) % MT_N] ^ (y >> 1);
			if (y % 2 != 0)
				rnum->mt[i] ^= matrix_a;
		}
	}
	uint32_t y = rnum->mt[rnum->index];
	y ^= y >> 11;
	y ^= (y << 7) & 0x9d2c5680;
	y ^= (y << 15) & 0xefc60000;
	y ^= y >> 18;
	rnum->index = (rnum->index + 1) % MT_N;
	return y;
}


#endif // !LIVNET_UTILS_H
