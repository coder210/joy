#include "jsclib.h"
#include <msgpack.h>
#include <stdlib.h>
#include <string.h>

/* ===== 递归 pack：将 Duktape 值编码为 msgpack ===== */
static int pack_one(duk_context* ctx, msgpack_packer* pk, int idx) {
    int t = duk_get_type(ctx, idx);
    switch (t) {
    case DUK_TYPE_NULL:
    case DUK_TYPE_UNDEFINED:
        return msgpack_pack_nil(pk);
    case DUK_TYPE_BOOLEAN:
        return duk_get_boolean(ctx, idx) ? msgpack_pack_true(pk) : msgpack_pack_false(pk);
    case DUK_TYPE_NUMBER: {
        double v = duk_get_number(ctx, idx);
        if (v == (double)(int64_t)v) {
            int64_t iv = (int64_t)v;
            if (iv >= 0)
                return msgpack_pack_unsigned_long_long(pk, (unsigned long long)iv);
            else
                return msgpack_pack_long_long(pk, (long long)iv);
        }
        return msgpack_pack_double(pk, v);
    }
    case DUK_TYPE_STRING: {
        size_t len;
        const char* s = duk_get_lstring(ctx, idx, &len);
        int ret = msgpack_pack_str(pk, len);
        if (ret != 0) return ret;
        return msgpack_pack_str_body(pk, s, len);
    }
    case DUK_TYPE_OBJECT: {
        if (duk_is_array(ctx, idx)) {
            /* array */
            duk_size_t n = (duk_size_t)duk_get_length(ctx, idx);
            int ret = msgpack_pack_array(pk, n);
            if (ret != 0) return ret;
            for (duk_size_t i = 0; i < n; i++) {
                duk_get_prop_index(ctx, idx, (duk_uarridx_t)i);
                ret = pack_one(ctx, pk, -1);
                duk_pop(ctx);
                if (ret != 0) return ret;
            }
        } else {
            /* map */
            /* 先数属性个数 */
            duk_enum(ctx, idx, DUK_ENUM_OWN_PROPERTIES_ONLY);
            size_t count = 0;
            while (duk_next(ctx, -1, 0)) {
                count++;
                duk_pop(ctx);
            }
            duk_pop(ctx);

            int ret = msgpack_pack_map(pk, count);
            if (ret != 0) return ret;

            duk_enum(ctx, idx, DUK_ENUM_OWN_PROPERTIES_ONLY);
            while (duk_next(ctx, -1, 1)) {
                /* key at -2, val at -1 */
                ret = pack_one(ctx, pk, -2);
                if (ret == 0) ret = pack_one(ctx, pk, -1);
                duk_pop(ctx);
                duk_pop(ctx);
                if (ret != 0) { duk_pop(ctx); return ret; }
            }
            duk_pop(ctx);
        }
        return 0;
    }
    case DUK_TYPE_BUFFER:
    case DUK_TYPE_POINTER:
    default: {
        /* 不支持的类型 pack 为 nil */
        return msgpack_pack_nil(pk);
    }
    }
}

static duk_ret_t js_msgpack_pack(duk_context* ctx) {
    int nargs = duk_get_top(ctx);

    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);
    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    for (int i = 0; i < nargs; i++) {
        if (pack_one(ctx, &pk, i) != 0) {
            msgpack_sbuffer_destroy(&sbuf);
            return DUK_RET_TYPE_ERROR;
        }
    }

    duk_push_lstring(ctx, sbuf.data, sbuf.size);
    msgpack_sbuffer_destroy(&sbuf);
    return 1;
}

/* ===== 递归 unpack：将 msgpack_object 转为 Duktape 值 ===== */
static void push_object(duk_context* ctx, msgpack_object obj);

static void push_object(duk_context* ctx, msgpack_object obj) {
    switch (obj.type) {
    case MSGPACK_OBJECT_NIL:
        duk_push_null(ctx);
        return;
    case MSGPACK_OBJECT_BOOLEAN:
        duk_push_boolean(ctx, obj.via.boolean);
        return;
    case MSGPACK_OBJECT_POSITIVE_INTEGER:
        duk_push_number(ctx, (double)obj.via.u64);
        return;
    case MSGPACK_OBJECT_NEGATIVE_INTEGER:
        duk_push_number(ctx, (double)obj.via.i64);
        return;
    case MSGPACK_OBJECT_FLOAT32:
    case MSGPACK_OBJECT_FLOAT64:
        duk_push_number(ctx, obj.via.f64);
        return;
    case MSGPACK_OBJECT_STR:
        duk_push_lstring(ctx, obj.via.str.ptr, obj.via.str.size);
        return;
    case MSGPACK_OBJECT_BIN:
        duk_push_lstring(ctx, obj.via.bin.ptr, obj.via.bin.size);
        return;
    case MSGPACK_OBJECT_ARRAY: {
        duk_idx_t arr = duk_push_array(ctx);
        for (uint32_t i = 0; i < obj.via.array.size; i++) {
            push_object(ctx, obj.via.array.ptr[i]);
            duk_put_prop_index(ctx, arr, (duk_uarridx_t)i);
        }
        return;
    }
    case MSGPACK_OBJECT_MAP: {
        duk_idx_t map = duk_push_object(ctx);
        for (uint32_t i = 0; i < obj.via.map.size; i++) {
            push_object(ctx, obj.via.map.ptr[i].key);
            push_object(ctx, obj.via.map.ptr[i].val);
            duk_put_prop(ctx, map);
        }
        return;
    }
    case MSGPACK_OBJECT_EXT:
        duk_push_lstring(ctx, obj.via.ext.ptr, obj.via.ext.size);
        return;
    default:
        duk_push_null(ctx);
        return;
    }
}

static duk_ret_t js_msgpack_unpack(duk_context* ctx) {
    if (!duk_is_string(ctx, 0)) {
        duk_push_null(ctx);
        return 1;
    }
    size_t len;
    const char* data = duk_get_lstring(ctx, 0, &len);
    if (!data || len == 0) {
        duk_push_null(ctx);
        return 1;
    }

    size_t off = 0;
    int count = 0;

    /* 先解包所有值到临时数组中 */
    msgpack_object objects[256];
    msgpack_zone* zones[256] = {NULL};
    int max = 256;

    while (off < len && count < max) {
        msgpack_unpacked result;
        msgpack_unpacked_init(&result);
        msgpack_unpack_return ret = msgpack_unpack_next(&result, data, len, &off);

        if (ret == MSGPACK_UNPACK_SUCCESS || ret == MSGPACK_UNPACK_EXTRA_BYTES) {
            objects[count] = result.data;
            zones[count] = result.zone;
            count++;
        } else {
            msgpack_unpacked_destroy(&result);
            break;
        }
    }

    if (count == 0) {
        duk_push_null(ctx);
        return 1;
    }

    if (count == 1) {
        push_object(ctx, objects[0]);
        if (zones[0]) msgpack_zone_free(zones[0]);
        return 1;
    }

    /* 多值返回数组 */
    duk_idx_t arr = duk_push_array(ctx);
    for (int i = 0; i < count; i++) {
        push_object(ctx, objects[i]);
        duk_put_prop_index(ctx, arr, (duk_uarridx_t)i);
        if (zones[i]) msgpack_zone_free(zones[i]);
    }
    return 1;
}

void jsopen_msgpack(duk_context* ctx) {
    duk_push_object(ctx);

    duk_push_c_function(ctx, js_msgpack_pack, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "pack");

    duk_push_c_function(ctx, js_msgpack_unpack, 1);
    duk_put_prop_string(ctx, -2, "unpack");

    duk_put_global_string(ctx, "msgpack");
}
