#ifndef MSGPACK_H
#define MSGPACK_H
#include <stdint.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif
	JOY_API int pack_int8(char* buf, int8_t value, int offset);
	JOY_API int pack_int16(char* buf, int16_t value, int offset);
	JOY_API int pack_int32(char* buf, int32_t value, int offset);
	JOY_API int pack_uint32(char* buf, uint32_t value, int offset);
	JOY_API int pack_int64(char* buf, int64_t value, int offset);
	JOY_API int unpack_int8(const char* buf, int8_t* value, int offset);
	JOY_API int unpack_int16(const char* buf, int16_t* value, int offset);
	JOY_API int unpack_int32(const char* buf, int32_t* value, int offset);
	JOY_API int unpack_uint32(const char* buf, uint32_t* value, int offset);
	JOY_API int unpack_int64(const char* buf, int64_t* value, int offset);
	JOY_API int pack_string(char* buf, const char* value, int type_size, int offset);
	JOY_API int unpack_string(const char* buf, char* value, int type_size, int offset);
#ifdef __cplusplus
}
#endif

#endif // !MSGPACK_H