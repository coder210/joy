/* ============================================================
 * joy 共享绑定运行层：对象表 / 句柄 / 自动释放
 *
 * C 层创建的指针一律登记在这里，脚本层只拿到 joy_handle（uint64_t），
 * 永远不接触地址。引用计数归零时自动调用 IDL 里标注的析构函数。
 * 句柄带 generation，防止槽位复用后旧句柄误指向新对象。
 * ============================================================ */
#ifndef JOY_API_OBJ_H
#define JOY_API_OBJ_H

#include <stdint.h>
#include "joy_value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*joy_obj_dtor)(void*);

/* 返回 0 表示失败（表满 / ptr 为空）。tag 用 C 类型名，deref 时校验。 */
uint64_t joy_obj_register(void* ptr, const char* tag, joy_obj_dtor dtor, const char* api);

/* 句柄 → 指针。类型不符 / 已释放 / 句柄无效时返回 NULL 并写 st。 */
void* joy_obj_deref(uint64_t handle, const char* tag, joy_status_t* st);

void joy_obj_retain(uint64_t handle);
void joy_obj_release(uint64_t handle);

/* 自检与将来帧级兜底用。 */
uint32_t joy_obj_live_count(void);
uint32_t joy_obj_destroy_count(void);

#ifdef __cplusplus
}
#endif
#endif /* JOY_API_OBJ_H */
