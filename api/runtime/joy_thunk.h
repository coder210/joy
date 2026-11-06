/* ============================================================
 * joy 共享绑定运行层：thunk 签名与注册表
 *
 * 每个开放的 joy API 对应一个纯 C thunk：joy_value[] 进、joy_value 出。
 * 三个脚本后端将来只需把本语言的值转成 joy_value 再查这张表。
 * ============================================================ */
#ifndef JOY_API_THUNK_H
#define JOY_API_THUNK_H

#include "joy_value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef joy_value_t (*joy_thunk_fn)(joy_value_t* args, int argc, joy_status_t* st);

typedef struct joy_thunk_entry {
        const char* domain;
        const char* name;
        joy_thunk_fn fn;
        int argc;
} joy_thunk_entry;

const joy_thunk_entry* joy_thunks_table(int* count);

#ifdef __cplusplus
}
#endif
#endif /* JOY_API_THUNK_H */
