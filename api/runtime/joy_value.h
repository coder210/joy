/* ============================================================
 * joy 共享绑定运行层：统一值容器
 *
 * 三后端（lua / js / cs）的 VM 值先转成 joy_value_t，thunk 只看这个值，
 * 不知道背后是哪个脚本引擎。P1 首步只覆盖 L0/L1 需要的标量 / 字符串 / 句柄。
 * ============================================================ */
#ifndef JOY_API_VALUE_H
#define JOY_API_VALUE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
        JOY_V_NULL = 0,
        JOY_V_BOOL,
        JOY_V_I32,
        JOY_V_U32,
        JOY_V_I64,
        JOY_V_U64,
        JOY_V_F64,
        JOY_V_STR,
        JOY_V_HANDLE
} joy_value_kind;

typedef struct joy_value {
        joy_value_kind kind;
        union {
                int         b;
                int32_t     i32;
                uint32_t    u32;
                int64_t     i64;
                uint64_t    u64;
                double      f64;
                const char* str;
                uint64_t    handle;
        } as;
} joy_value_t;

typedef enum {
        JOY_OK = 0,
        JOY_ERR_ARGC,
        JOY_ERR_TYPE,
        JOY_ERR_HANDLE,
        JOY_ERR_CREATE,
        JOY_ERR_CAPACITY
} joy_status_t;

joy_value_t joy_v_null(void);
joy_value_t joy_v_bool(int v);
joy_value_t joy_v_i32(int32_t v);
joy_value_t joy_v_u32(uint32_t v);
joy_value_t joy_v_i64(int64_t v);
joy_value_t joy_v_u64(uint64_t v);
joy_value_t joy_v_f64(double v);
joy_value_t joy_v_str(const char* v);
joy_value_t joy_v_handle(uint64_t v);

/* 统一错误：返回 NULL 值，同时把 code / 出错位置写进状态。 */
joy_value_t joy_v_err(joy_status_t* st, joy_status_t code, const char* where);
const char* joy_status_name(joy_status_t s);
const char* joy_last_error_where(void);

#ifdef __cplusplus
}
#endif
#endif /* JOY_API_VALUE_H */
