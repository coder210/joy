#include <msgpack/pack.h>
#include <msgpack/unpack.h>
#include <msgpack/object.h>
#include <msgpack/zone.h>
#include "joy.h"

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

/* jmsgpack_object_t 在内部就是 msgpack_object（对上层为不透明句柄）。
 * 所有字段访问都收敛到这里，任何平台/版本的布局差异只影响本文件，
 * 上层代码不再依赖结构体尺寸，杜绝 wasm32 下镜像错位之类的静默 bug。 */
#define JMP_OBJ(o) ((const msgpack_object*)(const void*)(o))

jmsgpack_object_type_t jmsgpack_object_type(const jmsgpack_object_t* o) {
    return o ? (jmsgpack_object_type_t)JMP_OBJ(o)->type : JMSGPACK_OBJECT_NIL;
}
uint32_t jmsgpack_object_array_size(const jmsgpack_object_t* o) {
    if (!o || JMP_OBJ(o)->type != MSGPACK_OBJECT_ARRAY) return 0;
    return JMP_OBJ(o)->via.array.size;
}
const jmsgpack_object_t* jmsgpack_object_array_at(const jmsgpack_object_t* o, uint32_t i) {
    if (!o || JMP_OBJ(o)->type != MSGPACK_OBJECT_ARRAY) return NULL;
    if (i >= JMP_OBJ(o)->via.array.size) return NULL;
    return (const jmsgpack_object_t*)(const void*)&JMP_OBJ(o)->via.array.ptr[i];
}
uint32_t jmsgpack_object_str_size(const jmsgpack_object_t* o) {
    if (!o || JMP_OBJ(o)->type != MSGPACK_OBJECT_STR) return 0;
    return JMP_OBJ(o)->via.str.size;
}
const char* jmsgpack_object_str_ptr(const jmsgpack_object_t* o) {
    if (!o || JMP_OBJ(o)->type != MSGPACK_OBJECT_STR) return NULL;
    return JMP_OBJ(o)->via.str.ptr;
}
uint64_t jmsgpack_object_u64(const jmsgpack_object_t* o) { return o ? JMP_OBJ(o)->via.u64 : 0; }
int64_t  jmsgpack_object_i64(const jmsgpack_object_t* o) { return o ? JMP_OBJ(o)->via.i64 : 0; }
bool     jmsgpack_object_boolean(const jmsgpack_object_t* o) { return o ? JMP_OBJ(o)->via.boolean : false; }
double   jmsgpack_object_f64(const jmsgpack_object_t* o) { return o ? JMP_OBJ(o)->via.f64 : 0.0; }

void jmsgpack_unpacked_init(jmsgpack_unpacked_t* result) {
    result->zone = NULL;
    result->data = NULL;
}

void jmsgpack_unpacked_destroy(jmsgpack_unpacked_t* result) {
    if (result->zone) {
        msgpack_zone_free((msgpack_zone*)result->zone);
        result->zone = NULL;
    }
    result->data = NULL;
}

int jmsgpack_unpack_next(jmsgpack_unpacked_t* result, const char* data, size_t len, size_t* off) {
    msgpack_unpacked inner;
    msgpack_unpacked_init(&inner);
    int ret = msgpack_unpack_next(&inner, data, len, off);
    if (ret >= 0) {
        /* 把根对象复制进 zone，使其地址在整个解包生命周期内稳定（inner.data 在栈上）。
         * 之后所有访问器都直接以 msgpack_object* 读 zone 内存，不再有镜像结构体。 */
        msgpack_object* root = (msgpack_object*)msgpack_zone_malloc(inner.zone, sizeof(msgpack_object));
        if (!root) {
            msgpack_unpacked_destroy(&inner);
            return MSGPACK_UNPACK_NOMEM_ERROR;
        }
        *root = inner.data;
        result->zone = (jmsgpack_zone_t*)inner.zone;
        inner.zone = NULL;  /* 转移所有权 */
        result->data = (const jmsgpack_object_t*)(const void*)root;
        msgpack_unpacked_destroy(&inner);
    }
    return ret;
}

const jmsgpack_object_t* jmsgpack_unpacked_data(const jmsgpack_unpacked_t* u) {
    return u ? u->data : NULL;
}
