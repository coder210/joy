/******************************************************************************
 * src/jcodec.h —— codec 域声明头（jcodec.c 的配套头）
 * 职责：msgpack 打包 / 解包封装（序列化编解码）
 * 方向：本头是该域声明的归属地，由 include/joy/japi.h 聚合包含；
 *       本头只依赖基础类型头，不再 include 聚合门面 joy.h，避免循环包含。
 * 生成链：本头是 gen_bindings.js 的 DECL_SOURCES 之一，改动声明后必须跑 --scan。
 ******************************************************************************/
#ifndef JOY_CODEC_IMPL_H
#define JOY_CODEC_IMPL_H

#include <joy/jtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

        JOY_API void joy_codec_msgpack_packer_init(joy_msgpack_packer_t* pk, void* data, joy_msgpack_write_fn cb);
        JOY_API int  joy_codec_msgpack_pack_int(joy_msgpack_packer_t* pk, int d);
        JOY_API int  joy_codec_msgpack_pack_int64(joy_msgpack_packer_t* pk, int64_t d);
        JOY_API int  joy_codec_msgpack_pack_uint(joy_msgpack_packer_t* pk, unsigned int d);
        JOY_API int  joy_codec_msgpack_pack_uint64(joy_msgpack_packer_t* pk, uint64_t d);
        JOY_API int  joy_codec_msgpack_pack_nil(joy_msgpack_packer_t* pk);
        JOY_API int  joy_codec_msgpack_pack_array(joy_msgpack_packer_t* pk, size_t n);
        JOY_API int  joy_codec_msgpack_pack_str(joy_msgpack_packer_t* pk, size_t l);
        JOY_API int  joy_codec_msgpack_pack_str_body(joy_msgpack_packer_t* pk, const void* b, size_t l);
        JOY_API void joy_codec_msgpack_unpacked_init(joy_msgpack_unpacked_t* result);
        JOY_API void joy_codec_msgpack_unpacked_destroy(joy_msgpack_unpacked_t* result);
        /* 返回值: >0 成功, 0 需要更多数据, <0 错误 */
        JOY_API int  joy_codec_msgpack_unpack_next(joy_msgpack_unpacked_t* result, const char* data, size_t len, size_t* off);

        /* ---- joy_msgpack_object_t 不透明访问器（替代直接读结构体字段） ---- */
        JOY_API const joy_msgpack_object_t* joy_codec_msgpack_unpacked_data(const joy_msgpack_unpacked_t* u);
        JOY_API joy_msgpack_object_type_t joy_codec_msgpack_object_type(const joy_msgpack_object_t* o);
        JOY_API uint32_t             joy_codec_msgpack_object_array_size(const joy_msgpack_object_t* o);
        JOY_API const joy_msgpack_object_t* joy_codec_msgpack_object_array_at(const joy_msgpack_object_t* o, uint32_t i);
        JOY_API uint32_t             joy_codec_msgpack_object_str_size(const joy_msgpack_object_t* o);
        JOY_API const char*          joy_codec_msgpack_object_str_ptr(const joy_msgpack_object_t* o);
        JOY_API uint64_t             joy_codec_msgpack_object_u64(const joy_msgpack_object_t* o);
        JOY_API int64_t              joy_codec_msgpack_object_i64(const joy_msgpack_object_t* o);
        JOY_API bool                 joy_codec_msgpack_object_boolean(const joy_msgpack_object_t* o);
        JOY_API double               joy_codec_msgpack_object_f64(const joy_msgpack_object_t* o);

#ifdef __cplusplus
}
#endif

#endif /* JOY_CODEC_IMPL_H */
