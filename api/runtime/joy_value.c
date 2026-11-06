#include "joy_value.h"

#include <string.h>

static char s_err_where[128];

static joy_value_t make(joy_value_kind k)
{
        joy_value_t v;
        v.kind = k;
        v.as.u64 = 0;
        return v;
}

joy_value_t joy_v_null(void)    { return make(JOY_V_NULL); }
joy_value_t joy_v_bool(int v)   { joy_value_t r = make(JOY_V_BOOL);  r.as.b = v ? 1 : 0; return r; }
joy_value_t joy_v_i32(int32_t v){ joy_value_t r = make(JOY_V_I32);   r.as.i32 = v; return r; }
joy_value_t joy_v_u32(uint32_t v){ joy_value_t r = make(JOY_V_U32);  r.as.u32 = v; return r; }
joy_value_t joy_v_i64(int64_t v){ joy_value_t r = make(JOY_V_I64);   r.as.i64 = v; return r; }
joy_value_t joy_v_u64(uint64_t v){ joy_value_t r = make(JOY_V_U64);  r.as.u64 = v; return r; }
joy_value_t joy_v_f64(double v) { joy_value_t r = make(JOY_V_F64);   r.as.f64 = v; return r; }
joy_value_t joy_v_str(const char* v){ joy_value_t r = make(JOY_V_STR); r.as.str = v; return r; }
joy_value_t joy_v_handle(uint64_t v){ joy_value_t r = make(JOY_V_HANDLE); r.as.handle = v; return r; }

joy_value_t joy_v_err(joy_status_t* st, joy_status_t code, const char* where)
{
        if (st) *st = code;
        if (where) {
                strncpy(s_err_where, where, sizeof(s_err_where) - 1);
                s_err_where[sizeof(s_err_where) - 1] = '\0';
        } else {
                s_err_where[0] = '\0';
        }
        return make(JOY_V_NULL);
}

const char* joy_last_error_where(void) { return s_err_where; }

const char* joy_status_name(joy_status_t s)
{
        switch (s) {
        case JOY_OK:           return "OK";
        case JOY_ERR_ARGC:     return "ARGC";
        case JOY_ERR_TYPE:     return "TYPE";
        case JOY_ERR_HANDLE:   return "HANDLE";
        case JOY_ERR_CREATE:   return "CREATE";
        case JOY_ERR_CAPACITY: return "CAPACITY";
        default:               return "UNKNOWN";
        }
}
