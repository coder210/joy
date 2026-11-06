/*
 * JavaScript(duktape) 后端薄适配（IDL 驱动）。
 *
 * 遍历 joy_thunks_table()，把每条开放 API 注册成全局
 * `Joy.Api.Bindings.<PascalName>`，与 cs / lua 端同名、同参数顺序、同句柄语义。
 *
 * 唯一和 JS 相关的只有三件事：
 *   1. joy_value <-> duktape 值转换（按注册表里的 arg_kinds / ret_kind）
 *   2. 句柄用 8 字节 buffer + finalizer，脚本侧看不到 uint64
 *   3. 错误用 duk_error 抛出
 *
 * 新增 API 不需要改这个文件：重新生成 thunks_generated.c 即可。
 */
#include "duktape/duktape.h"
#include "joy_js.h"
#include <joy/joy.h>
#include <joy/api/runtime/joy_value.h>
#include <joy/api/runtime/joy_obj.h>
#include <joy/api/runtime/joy_thunk.h>
#include <stdio.h>
#include <string.h>

#define JSAPI_MAX_ARGS 16
#define JSAPI_IDX_KEY  "\xFF" "joyi"
/* 句柄拆成两个 32 位整数放在普通对象上：plain buffer 不是 object，
 * duk_set_finalizer 不接受，而且也不该让脚本读到二进制内容。 */
#define JSAPI_HANDLE_LO "\xFF" "joylo"
#define JSAPI_HANDLE_HI "\xFF" "joyhi"

static duk_ret_t jsapi_handle_finalizer(duk_context* ctx)
{
        duk_uint_t lo, hi;
        uint64_t h;

        duk_get_prop_string(ctx, 0, JSAPI_HANDLE_LO);
        lo = duk_get_uint(ctx, -1);
        duk_pop(ctx);
        duk_get_prop_string(ctx, 0, JSAPI_HANDLE_HI);
        hi = duk_get_uint(ctx, -1);
        duk_pop(ctx);

        h = ((uint64_t)hi << 32) | (uint64_t)lo;
        if (h) joy_obj_release(h);
        return 0;
}

static void jsapi_push_handle(duk_context* ctx, uint64_t h)
{
        duk_push_object(ctx);
        duk_push_uint(ctx, (duk_uint_t)(h & 0xFFFFFFFFu));
        duk_put_prop_string(ctx, -2, JSAPI_HANDLE_LO);
        duk_push_uint(ctx, (duk_uint_t)(h >> 32));
        duk_put_prop_string(ctx, -2, JSAPI_HANDLE_HI);
        duk_push_c_function(ctx, jsapi_handle_finalizer, 1);
        duk_set_finalizer(ctx, -2);
}

/* 隐藏属性里存 joy_thunks_table() 的下标 */
static duk_ret_t jsapi_dispatch(duk_context* ctx)
{
        int count = 0;
        int i;
        int n;
        int idx;
        const joy_thunk_entry* tab;
        const joy_thunk_entry* e;
        joy_value_t args[JSAPI_MAX_ARGS];
        joy_value_t r;
        joy_status_t st;

        duk_push_current_function(ctx);
        duk_get_prop_string(ctx, -1, JSAPI_IDX_KEY);
        idx = (int)duk_get_int(ctx, -1);
        duk_pop_2(ctx);

        tab = joy_thunks_table(&count);
        if (idx < 0 || idx >= count) {
                return duk_error(ctx, DUK_ERR_ERROR, "Joy.Api.Bindings: bad index");
        }
        e = &tab[idx];

        n = duk_get_top(ctx);
        if (n != e->argc) {
                return duk_error(ctx, DUK_ERR_TYPE_ERROR,
                        "Joy.Api.Bindings.%s expects %d argument(s), got %d",
                        e->script_name, e->argc, n);
        }
        if (n > JSAPI_MAX_ARGS) {
                return duk_error(ctx, DUK_ERR_RANGE_ERROR,
                        "Joy.Api.Bindings.%s: too many arguments", e->script_name);
        }

        for (i = 0; i < n; i++) {
                switch (e->arg_kinds[i]) {
                case JOY_V_BOOL:   args[i] = joy_v_bool(duk_to_boolean(ctx, i)); break;
                case JOY_V_I32:    args[i] = joy_v_i32((int32_t)duk_to_int(ctx, i)); break;
                case JOY_V_U32:    args[i] = joy_v_u32((uint32_t)duk_to_uint(ctx, i)); break;
                case JOY_V_I64:    args[i] = joy_v_i64((int64_t)duk_to_number(ctx, i)); break;
                case JOY_V_U64:    args[i] = joy_v_u64((uint64_t)duk_to_number(ctx, i)); break;
                case JOY_V_F64:    args[i] = joy_v_f64(duk_to_number(ctx, i)); break;
                case JOY_V_STR:    args[i] = joy_v_str(duk_to_string(ctx, i)); break;
                case JOY_V_HANDLE: {
                        duk_uint_t lo, hi;
                        if (!duk_is_object(ctx, i) || !duk_has_prop_string(ctx, i, JSAPI_HANDLE_LO)) {
                                return duk_error(ctx, DUK_ERR_TYPE_ERROR,
                                        "Joy.Api.Bindings.%s: argument #%d is not a joy handle",
                                        e->script_name, i + 1);
                        }
                        duk_get_prop_string(ctx, i, JSAPI_HANDLE_LO);
                        lo = duk_get_uint(ctx, -1);
                        duk_pop(ctx);
                        duk_get_prop_string(ctx, i, JSAPI_HANDLE_HI);
                        hi = duk_get_uint(ctx, -1);
                        duk_pop(ctx);
                        args[i] = joy_v_handle(((uint64_t)hi << 32) | (uint64_t)lo);
                        break;
                }
                default:
                        return duk_error(ctx, DUK_ERR_TYPE_ERROR,
                                "Joy.Api.Bindings.%s: unsupported argument #%d",
                                e->script_name, i + 1);
                }
        }

        st = JOY_OK;
        r = e->fn(args, n, &st);
        if (st != JOY_OK) {
                return duk_error(ctx, DUK_ERR_ERROR, "Joy.Api.Bindings.%s failed: %s (%s)",
                        e->script_name, joy_status_name(st),
                        joy_last_error_where() ? joy_last_error_where() : "?");
        }

        switch (e->ret_kind) {
        case JOY_V_NULL:   return 0;
        case JOY_V_BOOL:   duk_push_boolean(ctx, r.as.b != 0); return 1;
        case JOY_V_I32:    duk_push_int(ctx, (duk_int_t)r.as.i32); return 1;
        case JOY_V_U32:    duk_push_uint(ctx, (duk_uint_t)r.as.u32); return 1;
        case JOY_V_I64:    duk_push_number(ctx, (duk_double_t)r.as.i64); return 1;
        case JOY_V_U64:    duk_push_number(ctx, (duk_double_t)r.as.u64); return 1;
        case JOY_V_F64:    duk_push_number(ctx, r.as.f64); return 1;
        case JOY_V_STR:    duk_push_string(ctx, r.as.str ? r.as.str : ""); return 1;
        case JOY_V_HANDLE: jsapi_push_handle(ctx, r.as.handle); return 1;
        default:           duk_push_undefined(ctx); return 1;
        }
}

void jsopen_api(void* ctxv)
{
        duk_context* ctx = (duk_context*)ctxv;
        int count = 0;
        int i;
        const joy_thunk_entry* tab;

        if (!ctx) return;
        tab = joy_thunks_table(&count);

        duk_push_global_object(ctx);
        duk_push_object(ctx);  /* Joy */
        duk_push_object(ctx);  /* Joy.Api */
        duk_push_object(ctx);  /* Joy.Api.Bindings */
        for (i = 0; i < count; i++) {
                duk_push_c_function(ctx, jsapi_dispatch, DUK_VARARGS);
                duk_push_int(ctx, i);
                duk_put_prop_string(ctx, -2, JSAPI_IDX_KEY);
                duk_put_prop_string(ctx, -2, tab[i].script_name);
        }
        duk_put_prop_string(ctx, -2, "Bindings");
        duk_put_prop_string(ctx, -2, "Api");
        duk_put_prop_string(ctx, -2, "Joy");
        duk_pop(ctx);
}

const char* jsopen_api_selftest(void)
{
        static char errbuf[256];
        uint32_t live0 = joy_obj_live_count();
        uint32_t dead0 = joy_obj_destroy_count();
        duk_context* ctx = duk_create_heap_default();
        if (!ctx) return "js/newheap";

        jsopen_api(ctx);
        if (duk_peval_string(ctx,
                "var t = Joy.Api.Bindings.GetTicks();\n"
                "if (typeof t !== 'number' || t < 0) throw new Error('ticks');\n"
                "var c = Joy.Api.Bindings.JtextCacheGetCount();\n"
                "if (typeof c !== 'number') throw new Error('cache');\n"
                "var r = Joy.Api.Bindings.LcgRandomCreate(12345);\n"
                "var a = Joy.Api.Bindings.LcgRandomNext(r);\n"
                "var b = Joy.Api.Bindings.LcgRandomNext(r);\n"
                "if (a === b) throw new Error('lcg');\n"
                "var threw = false;\n"
                "try { Joy.Api.Bindings.LcgRandomNext(1); } catch (e) { threw = true; }\n"
                "if (!threw) throw new Error('type guard');\n"
                "r = null;\n") != 0) {
                snprintf(errbuf, sizeof(errbuf), "js/%s", duk_safe_to_string(ctx, -1));
                duk_destroy_heap(ctx);
                return errbuf;
        }

        duk_pop(ctx);
        duk_gc(ctx, 0);
        duk_destroy_heap(ctx);
        if (joy_obj_live_count() != live0) return "js/handle-leak";
        if (joy_obj_destroy_count() != dead0 + 1) return "js/handle-not-released";
        return NULL;
}
