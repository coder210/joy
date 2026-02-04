#ifndef LIV_MSGPACK_H
#define LIV_MSGPACK_H
#include <SDL3/SDL.h>
#include <stdint.h>

static int pack_int8(char* buf, int8_t value, int offset)
{
	int type_size = sizeof(int8_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}
static int pack_int16(char* buf, int16_t value, int offset)
{
	int type_size = sizeof(int16_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}
static int pack_int32(char* buf, int32_t value, int offset)
{
	int type_size = sizeof(int32_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}
static int pack_uint32(char* buf, uint32_t value, int offset)
{
	int type_size = sizeof(uint32_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}
static int pack_int64(char* buf, int64_t value, int offset)
{
	int type_size = sizeof(int64_t);
	SDL_memcpy(buf + offset, &value, type_size);
	return offset + type_size;
}

static int unpack_int8(const char* buf, int8_t* value, int offset)
{
	int type_size = sizeof(int8_t);
	*value = *((int8_t*)(buf + offset));
	return offset + type_size;
}
static int unpack_int16(const char* buf, int16_t* value, int offset)
{
	int type_size = sizeof(int16_t);
	*value = *((int16_t*)(buf + offset));
	return offset + type_size;
}
static int unpack_int32(const char* buf, int32_t* value, int offset)
{
	int type_size = sizeof(int32_t);
	*value = *((int32_t*)(buf + offset));
	return offset + type_size;
}
static int unpack_uint32(const char* buf, uint32_t* value, int offset)
{
	int type_size = sizeof(uint32_t);
	*value = *((uint32_t*)(buf + offset));
	return offset + type_size;
}
static int unpack_int64(const char* buf, int64_t* value, int offset)
{
	int type_size = sizeof(int64_t);
	*value = *((int64_t*)(buf + offset));
	return offset + type_size;
}
static int
pack_string(char* buf, const char* value, int type_size, int offset)
{
	memcpy(buf + offset, value, type_size);
	return offset + type_size;
}
static int
unpack_string(const char* buf, char* value, int type_size, int offset)
{
	memcpy(value, buf + offset, type_size);
	return offset + type_size;
}

#endif // !LIV_MSGPACK_H