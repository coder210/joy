#include "jmsgpack.h"
#include "jconfig.h"
#include <msgpack/pack.h>
#include <msgpack/unpack.h>
#include <msgpack/object.h>
#include <msgpack/zone.h>

/* ===== Packer ===== */

void jmsgpack_packer_init(jmsgpack_packer_t* pk, void* data, jmsgpack_write_fn cb) {
    msgpack_packer p;
    msgpack_packer_init(&p, data, (msgpack_packer_write)cb);
    pk->data = p.data;
    pk->callback = p.callback;
}

int jmsgpack_pack_int(jmsgpack_packer_t* pk, int d) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_int(&p, d);
}

int jmsgpack_pack_int64(jmsgpack_packer_t* pk, int64_t d) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_int64(&p, d);
}

int jmsgpack_pack_uint(jmsgpack_packer_t* pk, unsigned int d) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_unsigned_int(&p, d);
}

int jmsgpack_pack_uint64(jmsgpack_packer_t* pk, uint64_t d) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_uint64(&p, d);
}

int jmsgpack_pack_nil(jmsgpack_packer_t* pk) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_nil(&p);
}

int jmsgpack_pack_array(jmsgpack_packer_t* pk, size_t n) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_array(&p, n);
}

int jmsgpack_pack_str(jmsgpack_packer_t* pk, size_t l) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_str(&p, l);
}

int jmsgpack_pack_str_body(jmsgpack_packer_t* pk, const void* b, size_t l) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_str_body(&p, b, l);
}

/* ===== Unpacker ===== */

void jmsgpack_unpacked_init(jmsgpack_unpacked_t* result) {
    result->zone = NULL;
    memset(&result->data, 0, sizeof(result->data));
}

void jmsgpack_unpacked_destroy(jmsgpack_unpacked_t* result) {
    if (result->zone) {
        msgpack_zone_free((msgpack_zone*)result->zone);
        result->zone = NULL;
    }
}

int jmsgpack_unpack_next(jmsgpack_unpacked_t* result, const char* data, size_t len, size_t* off) {
    msgpack_unpacked inner;
    msgpack_unpacked_init(&inner);
    int ret = msgpack_unpack_next(&inner, data, len, off);
    if (ret >= 0) {
        /* 转换 inner → result */
        result->zone = (jmsgpack_zone_t*)inner.zone;
        inner.zone = NULL;  /* 转移所有权 */
        /* 转换 msgpack_object → jmsgpack_object */
        const msgpack_object* src = &inner.data;
        jmsgpack_object_t* dst = &result->data;
        dst->type = (jmsgpack_object_type_t)src->type;
        switch (src->type) {
        case MSGPACK_OBJECT_NIL: break;
        case MSGPACK_OBJECT_BOOLEAN: dst->via.boolean = src->via.boolean; break;
        case MSGPACK_OBJECT_POSITIVE_INTEGER: dst->via.u64 = src->via.u64; break;
        case MSGPACK_OBJECT_NEGATIVE_INTEGER: dst->via.i64 = src->via.i64; break;
        case MSGPACK_OBJECT_FLOAT64: dst->via.f64 = src->via.f64; break;
        case MSGPACK_OBJECT_STR:
            dst->via.str.size = src->via.str.size;
            dst->via.str.ptr = src->via.str.ptr;
            break;
        case MSGPACK_OBJECT_ARRAY:
            dst->via.array.size = src->via.array.size;
            dst->via.array.ptr = (const jmsgpack_object_t*)src->via.array.ptr;
            break;
        default: break;
        }
        msgpack_unpacked_destroy(&inner);
    }
    return ret;
}
