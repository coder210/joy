/* ============================================================
 * 自动生成，勿手改：node api/gen_bindings.js --emit
 * cs 后端薄适配：Mono 值 → joy_value[] → 共享 thunk（不含业务逻辑）。
 * 只有 joy.idl 里 status:on 的 L0/L1 条目会出现在这里。
 * ============================================================ */
#include <stdint.h>
#include <stdio.h>
#include <joy/joy.h>
#include <joy/api/runtime/joy_value.h>
#include <joy/api/runtime/joy_obj.h>
#include <joy/api/runtime/joy_thunk.h>
#include "joy_csharp.h"

static uint64_t icall_LcgRandomCreate(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_u64((uint64_t)p0);
        r = joy_api_call("lcg_random_create", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "lcg_random_create", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static uint32_t icall_LcgRandomNext(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("lcg_random_next", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "lcg_random_next", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint32_t)r.as.u32;
}

static uint64_t icall_GetTicks(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("get_ticks", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "get_ticks", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static int32_t icall_JtextCacheGetCount(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("jtext_cache_get_count", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "jtext_cache_get_count", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

/* 句柄对象被 GC 时由 C# finalizer 调这里；计数仅供自检。 */
static void icall_release(uint64_t h) { joy_obj_release((uint64_t)h); }
static uint32_t icall_live_count(void) { return joy_obj_live_count(); }
static uint32_t icall_destroy_count(void) { return joy_obj_destroy_count(); }

void csopen_api(void)
{
        csx_register_internal_call("Joy.Api.Native::LcgRandomCreate", (const void*)icall_LcgRandomCreate);
        csx_register_internal_call("Joy.Api.Native::LcgRandomNext", (const void*)icall_LcgRandomNext);
        csx_register_internal_call("Joy.Api.Native::GetTicks", (const void*)icall_GetTicks);
        csx_register_internal_call("Joy.Api.Native::JtextCacheGetCount", (const void*)icall_JtextCacheGetCount);
        csx_register_internal_call("Joy.Api.Native::Release", (const void*)icall_release);
        csx_register_internal_call("Joy.Api.Native::DebugLiveCount", (const void*)icall_live_count);
        csx_register_internal_call("Joy.Api.Native::DebugDestroyCount", (const void*)icall_destroy_count);
}
