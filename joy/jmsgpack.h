#ifndef JMSGPACK_H
#define JMSGPACK_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "jconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ===== 装箱回调类型 ===== */
typedef int (*jmsgpack_write_fn)(void* data, const char* buf, size_t len);

/* ===== 类型别名 ===== */
typedef struct jmsgpack_packer {
    void* data;
    jmsgpack_write_fn callback;
} jmsgpack_packer_t;

typedef struct jmsgpack_zone jmsgpack_zone_t;
typedef struct jmsgpack_object jmsgpack_object_t;

typedef enum {
    JMSGPACK_OBJECT_NIL               = 0x00,
    JMSGPACK_OBJECT_BOOLEAN           = 0x01,
    JMSGPACK_OBJECT_POSITIVE_INTEGER  = 0x02,
    JMSGPACK_OBJECT_NEGATIVE_INTEGER  = 0x03,
    JMSGPACK_OBJECT_FLOAT64           = 0x04,
    JMSGPACK_OBJECT_STR               = 0x05,
    JMSGPACK_OBJECT_ARRAY             = 0x06,
    JMSGPACK_OBJECT_MAP               = 0x07,
} jmsgpack_object_type_t;

typedef struct {
    uint32_t size;
    const jmsgpack_object_t* ptr;
} jmsgpack_object_array_t;

typedef struct {
    uint32_t size;
    const char* ptr;
} jmsgpack_object_str_t;

typedef struct jmsgpack_object {
    jmsgpack_object_type_t type;
    union {
        bool boolean;
        uint64_t u64;
        int64_t i64;
        double f64;
        jmsgpack_object_array_t array;
        jmsgpack_object_str_t str;
    } via;
} jmsgpack_object_t;

typedef struct {
    jmsgpack_zone_t* zone;
    jmsgpack_object_t data;
} jmsgpack_unpacked_t;

/* ===== Packer ===== */
JOY_API void jmsgpack_packer_init(jmsgpack_packer_t* pk, void* data, jmsgpack_write_fn cb);
JOY_API int  jmsgpack_pack_int(jmsgpack_packer_t* pk, int d);
JOY_API int  jmsgpack_pack_int64(jmsgpack_packer_t* pk, int64_t d);
JOY_API int  jmsgpack_pack_uint(jmsgpack_packer_t* pk, unsigned int d);
JOY_API int  jmsgpack_pack_uint64(jmsgpack_packer_t* pk, uint64_t d);
JOY_API int  jmsgpack_pack_nil(jmsgpack_packer_t* pk);
JOY_API int  jmsgpack_pack_array(jmsgpack_packer_t* pk, size_t n);
JOY_API int  jmsgpack_pack_str(jmsgpack_packer_t* pk, size_t l);
JOY_API int  jmsgpack_pack_str_body(jmsgpack_packer_t* pk, const void* b, size_t l);

/* ===== Unpacker ===== */
JOY_API void jmsgpack_unpacked_init(jmsgpack_unpacked_t* result);
JOY_API void jmsgpack_unpacked_destroy(jmsgpack_unpacked_t* result);
/* 返回值: >0 成功, 0 需要更多数据, <0 错误 */
JOY_API int  jmsgpack_unpack_next(jmsgpack_unpacked_t* result, const char* data, size_t len, size_t* off);

#ifdef __cplusplus
}
#endif

#endif /* JMSGPACK_H */
