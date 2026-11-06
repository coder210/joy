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
        const char* name;         /* C API 名（snake_case），供 joy_api_call 查表 */
        const char* script_name;  /* 脚本层方法名（三端统一 PascalCase），由 naming.js 生成 */
        joy_thunk_fn fn;
        int argc;
        /* 三端共用的类型契约：vm.c 按 arg_kinds[j] 把第 j 个脚本值转成 joy_value，
         * 再交给 fn；ret_kind 决定返回值怎么翻译回脚本。后端不需要各解析一遍 IDL。 */
        const joy_value_kind* arg_kinds;
        joy_value_kind ret_kind;
} joy_thunk_entry;

const joy_thunk_entry* joy_thunks_table(int* count);

/* 按 C API 名查注册表并调用。三个后端都只走这个入口，
 * 不必引用生成物里的 static thunk 函数。找不到名字时返回 NULL 值并写 st。 */
joy_value_t joy_api_call(const char* name, joy_value_t* args, int argc, joy_status_t* st);

#ifdef __cplusplus
}
#endif
#endif /* JOY_API_THUNK_H */
