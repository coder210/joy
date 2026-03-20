#include "utils.h"
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
#include "cont.h"
#include "config.h"
#include "sys.h"


#define MT_N 624
#define MT_M 397

struct random_num {
	int index;
	uint32_t mt[MT_N];
};


char* 
utils_strdup(const char* str, size_t* out_sz)
{
	size_t sz = (int)SDL_strlen(str);
	char* ret = (char*)SDL_malloc(sz + 1);
	if (ret)
		memcpy(ret, str, sz + 1);
	*out_sz = sz;
	return ret;
}

bool
utils_get_current_time(uint64_t *ms_ticks)
{
	bool resval;
	SDL_Time ticks;
	resval = SDL_GetCurrentTime(&ticks);
	*ms_ticks = ticks / 1000000;
	return resval;
}

void 
utils_current_datetime(const char* fmt, char* buf, size_t len)
{
	time_t t;
	time(&t);
	struct tm* now = (struct tm*)localtime(&t);
	strftime(buf, len, fmt, now);
}

char* 
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

bool 
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

void 
utils_append_file(const char* filename, const char* data)
{
	FILE* fp = fopen(filename, "a+");
	if (fp != NULL) {
		fprintf(fp, "%s\n", data);
		fclose(fp);
	}
}

short 
utils_bit2short(uint8_t* buf)
{
	short r = (short)buf[0] << 8 | (short)buf[1];
	return r;
}

void 
utils_short2bit(uint8_t* buf, short value)
{
	buf[0] = (value >> 8) & 0xff;
	buf[1] = value & 0xff;
}

/*
* c# = BitConverter.FromUInt32
*/
int 
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
void 
utils_int2bit(uint8_t* buf, int value)
{
	buf[3] = ((0xff000000 & value) >> 24);
	buf[2] = ((0xff0000 & value) >> 16);
	buf[1] = ((0xff00 & value) >> 8);
	buf[0] = (0xff & value);
}

bool
utils_wait_delay(delay_t* delay, uint64_t current_time)
{
	if (current_time - delay->time >= delay->timeout) {
		delay->time = current_time;
		return true;
	}
	return false;
}

short 
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

short 
utils_md5(const char* input, size_t len, char* output)
{
	md5(input, len, output);
	return 16;
}

char* 
utils_b64_encode(const char* input, size_t len)
{
	char* result = b64_encode(input, len);
	return result;
}

unsigned char* 
utils_b64_decode(const char* input, size_t len, size_t* out_len)
{
	unsigned char* result = b64_decode_ex(input, len, out_len);
	return result;
}

unsigned char* 
utils_aes_encrypt256(const char* input, size_t* len,
	const char* key, const char* iv)
{
	size_t input_len;
	unsigned char* output = NULL;

	//input_len = (int)SDL_strlen(input);
	//output = aes_encrypt256x(input, input_len, len, key, iv);

	return output;
}

/*
* key len is equal 32
*/
unsigned char*
utils_aes_decrypt256(const char* input, size_t* out_len,
	const char* key, const char* iv)
{
	unsigned char* output = NULL;
	//size_t input_len;

	//input_len = (int)SDL_strlen(input);
	//output = aes_decrypt256x(input, input_len, out_len, key, iv);

	return output;
}

/*
* key len is equal 16
*/
unsigned char* 
utils_aes_encrypt128(const char* input, size_t* out_len,
	const char* key, const char* iv)
{
	unsigned char* output = NULL;
	int input_len;

	//input_len = (int)SDL_strlen(input);
	//output = aes_encrypt128x(input, input_len, out_len, key, iv);
	return output;
}

unsigned char*
utils_aes_decrypt128(const char* input, size_t* out_len,
	const char* key, const char* iv)
{
	unsigned char* output = NULL;
	int input_len;

	input_len = (int)SDL_strlen(input);
	//output = aes_decrypt128x(input, input_len, out_len, key, iv);

	return output;
}


char* 
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

char*
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

char*
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

random_num_p
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

void
utils_random_destroy(random_num_p rnum)
{
	SDL_free(rnum);
}

uint32_t
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

unsigned long sdbm_hash(const char* str, int size)
{
	unsigned long hash = 0;
	int c;
	while ((c = *str++)) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}
	return hash % size;
}

unsigned long fnv1a_hash(const char* str, int size)
{
	unsigned long hash = 2166136261u;  // FNV_offset_basis
	int c;
	while ((c = *str++)) {
		hash ^= c;
		hash *= 16777619u;  // FNV_prime
	}
	return hash % size;
}


unsigned long jenkins_hash(const char* str, int size)
{
	unsigned long hash = 0;
	int c;
	while ((c = *str++)) {
		hash += c;
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash % size;
}

unsigned long murmur3_hash(const char* str)
{
	unsigned long hash = 0;
	const unsigned int seed = 0x9747b28c;
	const unsigned int c1 = 0xcc9e2d51;
	const unsigned int c2 = 0x1b873593;

	const unsigned char* data = (const unsigned char*)str;
	int len = strlen(str);

	while (len >= 4) {
		unsigned int k = *(unsigned int*)data;

		k *= c1;
		k = (k << 15) | (k >> 17);
		k *= c2;

		hash ^= k;
		hash = (hash << 13) | (hash >> 19);
		hash = hash * 5 + 0xe6546b64;

		data += 4;
		len -= 4;
	}

	unsigned int k = 0;
	switch (len) {
	case 3: k ^= data[2] << 16;
	case 2: k ^= data[1] << 8;
	case 1: k ^= data[0];
		k *= c1;
		k = (k << 15) | (k >> 17);
		k *= c2;
		hash ^= k;
	}

	hash ^= strlen(str);
	hash ^= hash >> 16;
	hash *= 0x85ebca6b;
	hash ^= hash >> 13;
	hash *= 0xc2b2ae35;
	hash ^= hash >> 16;

	return hash;
}

unsigned long kr_hash(const char* str, int size)
{
	unsigned long hash = 0;
	int c;
	while ((c = *str++)) {
		hash = c + hash * 31;
	}
	return hash % size;
}


#define MAXUNICODE 0x10FFFFu
#define iscont(c) (((c) & 0xC0) == 0x80)

/* 各长度字符的最小合法码点（防止过短表示） */
static const uint32_t limits[] = {
    ~(uint32_t)0, 0x80, 0x800, 0x10000u, 0x200000u, 0x4000000u
};

size_t utf8_decode(const char* s, size_t len, int32_t* code)
{
	if (len == 0) return 0;

	unsigned char c = (unsigned char)s[0];

	/* ASCII */
	if (c < 0x80) {
		*code = c;
		return 1;
	}

	int nbytes;
	uint32_t min;
	uint32_t res;

	/* 根据首字节判断字节数 */
	if (c < 0xC2) {
		return 0;
	}
	else if (c < 0xE0) {        /* 2字节: 110xxxxx */
		nbytes = 2;
		min = 0x80;
		res = c & 0x1F;
	}
	else if (c < 0xF0) {        /* 3字节: 1110xxxx */
		nbytes = 3;
		min = 0x800;
		res = c & 0x0F;
	}
	else if (c < 0xF5) {        /* 4字节: 11110xxx (F4 是能表示 0x10FFFF 的最大值) */
		nbytes = 4;
		min = 0x10000;
		res = c & 0x07;
	}
	else {
		return 0;                  /* 0xF5~0xFF 非法 */
	}

	if (len < (size_t)nbytes)
		return 0;

	/* 处理后续字节 */
	for (int i = 1; i < nbytes; i++) {
		unsigned char cc = (unsigned char)s[i];
		if (!iscont(cc))
			return 0;
		res = (res << 6) | (cc & 0x3F);
	}

	/* 过短检查 */
	if (res < min)
		return 0;

	/* 最大值及代理项检查（严格模式） */
	if (res > MAXUNICODE)
		return 0;
	if (0xD800 <= res && res <= 0xDFFF)
		return 0;

	*code = (int32_t)res;
	return nbytes;
}

int utf8_next(const char* s, size_t len, size_t* pos, int32_t* code)
{
	if (*pos >= len) return 0;
	size_t consumed = utf8_decode(s + *pos, len - *pos, code);
	if (consumed == 0) return 0;
	*pos += consumed;
	return 1;
}

size_t utf8_decode_all(const char* s, size_t len, int32_t* out_codes, size_t out_capacity)
{
	size_t count = 0;
	size_t pos = 0;

	/* 第一次遍历：计算所需码点个数，同时验证字符串有效性 */
	while (pos < len) {
		int32_t cp;  /* 临时存储，无需实际值 */
		size_t consumed = utf8_decode(s + pos, len - pos, &cp);
		if (consumed == 0) {
			return 0;  /* 无效 UTF-8 序列 */
		}
		count++;
		pos += consumed;
	}

	/* 如果 out_codes 为 NULL，则只返回所需长度（用于缓冲区预分配） */
	if (out_codes == NULL) {
		return count;
	}

	/* 检查缓冲区容量是否足够 */
	if (out_capacity < count) {
		return 0;  /* 缓冲区太小，失败 */
	}

	/* 第二次遍历：填充数组 */
	pos = 0;
	size_t idx = 0;
	while (pos < len) {
		int32_t cp;
		size_t consumed = utf8_decode(s + pos, len - pos, &cp);
		/* 此时不会失败，因为第一次已验证过 */
		out_codes[idx++] = cp;
		pos += consumed;
	}

	return count;
}


int pack_int8(char* buf, int8_t value, int offset)
{
	int type_size = sizeof(int8_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}
int pack_int16(char* buf, int16_t value, int offset)
{
	int type_size = sizeof(int16_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}
int pack_int32(char* buf, int32_t value, int offset)
{
	int type_size = sizeof(int32_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}
int pack_uint32(char* buf, uint32_t value, int offset)
{
	int type_size = sizeof(uint32_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}
int pack_int64(char* buf, int64_t value, int offset)
{
	int type_size = sizeof(int64_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}

int unpack_int8(const char* buf, int8_t* value, int offset)
{
	int type_size = sizeof(int8_t);
	*value = *((int8_t*)(buf + offset));
	return offset + type_size;
}
int unpack_int16(const char* buf, int16_t* value, int offset)
{
	int type_size = sizeof(int16_t);
	*value = *((int16_t*)(buf + offset));
	return offset + type_size;
}
int unpack_int32(const char* buf, int32_t* value, int offset)
{
	int type_size = sizeof(int32_t);
	*value = *((int32_t*)(buf + offset));
	return offset + type_size;
}
int unpack_uint32(const char* buf, uint32_t* value, int offset)
{
	int type_size = sizeof(uint32_t);
	*value = *((uint32_t*)(buf + offset));
	return offset + type_size;
}
int unpack_int64(const char* buf, int64_t* value, int offset)
{
	int type_size = sizeof(int64_t);
	*value = *((int64_t*)(buf + offset));
	return offset + type_size;
}
int pack_string(char* buf, const char* value, int type_size, int offset)
{
	memcpy(buf + offset, value, type_size);
	return offset + type_size;
}
int unpack_string(const char* buf, char* value, int type_size, int offset)
{
	memcpy(value, buf + offset, type_size);
	return offset + type_size;
}



struct timer {
	uint64_t last_time;        // 上次触发的时间点（SDL_GetTicks）
	int interval;              // 触发间隔（毫秒）
	bool active;               // 是否激活
	timer_callback callback;   // 回调函数
	void* userdata;            // 用户数据
};

timer_p timerx_create(int interval, timer_callback cb, void* userdata)
{
	timer_p timer = (timer_p)malloc(sizeof(struct timer));
	if (!timer) return NULL;

	timer->last_time = SDL_GetTicks();
	timer->interval = interval;
	timer->active = true;
	timer->callback = cb;
	timer->userdata = userdata;

	return timer;
}

void timerx_destroy(timer_p timer)
{
	if (timer) {
		free(timer);
	}
}

void timerx_trigger(timer_p timer)
{
	if (!timer || !timer->active || !timer->callback) {
		return;
	}

	uint64_t current_time = SDL_GetTicks();
	uint64_t elapsed = current_time - timer->last_time;

	if (elapsed >= (uint64_t)timer->interval) {
		int actual_interval = (int)elapsed;

		// 调用回调
		timer->callback(actual_interval, timer->interval, timer->userdata);

		// 更新最后触发时间（避免累积漂移）
		timer->last_time += timer->interval;

		// 如果落后太多，快速追赶（防止回调执行时间过长导致连续触发）
		while (current_time - timer->last_time >= (uint64_t)timer->interval) {
			timer->last_time += timer->interval;
		}
	}
}

void timerx_set_interval(timer_p timer, int interval)
{
	if (timer) {
		timer->interval = interval;
	}
}

void timerx_set_active(timer_p timer, bool active)
{
	if (timer) {
		timer->active = active;
		if (active) {
			timer->last_time = SDL_GetTicks(); // 重新激活时重置计时
		}
	}
}

void timerx_reset(timer_p timer)
{
	if (timer) {
		timer->last_time = SDL_GetTicks();
	}
}

uint64_t timerx_get_elapsed(timer_p timer)
{
	if (!timer) return 0;
	return SDL_GetTicks() - timer->last_time;
}
