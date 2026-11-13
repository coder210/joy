/******************************************************************************
 * src/joy_script.h —— script 域声明头（joy_script.c 的配套头）
 * 职责：msgpack / 插件模块
 * 方向：本头是该域声明的归属地，由 include/joy/japi.h 聚合包含；
 *       本头只依赖基础类型头，不再 include 聚合门面 joy.h，避免循环包含。
 * 生成链：本头是 gen_bindings.js 的 DECL_SOURCES 之一，改动声明后必须跑 --scan。
 ******************************************************************************/
#ifndef JOY_SCRIPT_IMPL_H
#define JOY_SCRIPT_IMPL_H

#include <joy/jtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef JOY_USE_PLUGINS  /* 见 JoySources.cmake 的可选适配层；源码在 jmodule.c */
        JOY_API joy_modules_p joy_script_module_create(const char* path);
        JOY_API void joy_script_module_destroy(joy_modules_p modules);
        JOY_API bool joy_script_module_query(joy_modules_p modules, const char* name, joy_module_p* mod);
        JOY_API void joy_script_module_register(joy_modules_p modules, const char* name,
                joy_module_create_t create,
                joy_module_init_t init,
                joy_module_event_t event_handler,
                joy_module_update_t update,
                joy_module_release_t release);
        JOY_API void joy_script_module_require(joy_modules_p modules, const char* name);
        JOY_API bool joy_script_modules_retire(joy_modules_p modules, const char* name, joy_module_p* mod);
        JOY_API void* joy_script_module_instance_create(joy_module_p mod);
        JOY_API void joy_script_module_instance_init(joy_module_p mod, void* inst, void* ctx, const char* param);
        JOY_API void joy_script_module_instance_event(joy_module_p mod, void* inst, void* event);
        JOY_API void joy_script_module_instance_update(joy_module_p mod, void* inst, float delta_time);
        JOY_API void joy_script_module_instance_release(joy_module_p mod, void* inst);
#endif /* JOY_USE_PLUGINS */

        JOY_API void joy_script_msgpack_packer_init(joy_msgpack_packer_t* pk, void* data, joy_msgpack_write_fn cb);
        JOY_API int  joy_script_msgpack_pack_int(joy_msgpack_packer_t* pk, int d);
        JOY_API int  joy_script_msgpack_pack_int64(joy_msgpack_packer_t* pk, int64_t d);
        JOY_API int  joy_script_msgpack_pack_uint(joy_msgpack_packer_t* pk, unsigned int d);
        JOY_API int  joy_script_msgpack_pack_uint64(joy_msgpack_packer_t* pk, uint64_t d);
        JOY_API int  joy_script_msgpack_pack_nil(joy_msgpack_packer_t* pk);
        JOY_API int  joy_script_msgpack_pack_array(joy_msgpack_packer_t* pk, size_t n);
        JOY_API int  joy_script_msgpack_pack_str(joy_msgpack_packer_t* pk, size_t l);
        JOY_API int  joy_script_msgpack_pack_str_body(joy_msgpack_packer_t* pk, const void* b, size_t l);
        JOY_API void joy_script_msgpack_unpacked_init(joy_msgpack_unpacked_t* result);
        JOY_API void joy_script_msgpack_unpacked_destroy(joy_msgpack_unpacked_t* result);
        /* 返回值: >0 成功, 0 需要更多数据, <0 错误 */
        JOY_API int  joy_script_msgpack_unpack_next(joy_msgpack_unpacked_t* result, const char* data, size_t len, size_t* off);

        /* ---- joy_msgpack_object_t 不透明访问器（替代直接读结构体字段） ---- */
        JOY_API const joy_msgpack_object_t* joy_script_msgpack_unpacked_data(const joy_msgpack_unpacked_t* u);
        JOY_API joy_msgpack_object_type_t joy_script_msgpack_object_type(const joy_msgpack_object_t* o);
        JOY_API uint32_t             joy_script_msgpack_object_array_size(const joy_msgpack_object_t* o);
        JOY_API const joy_msgpack_object_t* joy_script_msgpack_object_array_at(const joy_msgpack_object_t* o, uint32_t i);
        JOY_API uint32_t             joy_script_msgpack_object_str_size(const joy_msgpack_object_t* o);
        JOY_API const char*          joy_script_msgpack_object_str_ptr(const joy_msgpack_object_t* o);
        JOY_API uint64_t             joy_script_msgpack_object_u64(const joy_msgpack_object_t* o);
        JOY_API int64_t              joy_script_msgpack_object_i64(const joy_msgpack_object_t* o);
        JOY_API bool                 joy_script_msgpack_object_boolean(const joy_msgpack_object_t* o);
        JOY_API double               joy_script_msgpack_object_f64(const joy_msgpack_object_t* o);

#ifdef __cplusplus
}
#endif

#endif /* JOY_SCRIPT_IMPL_H */
