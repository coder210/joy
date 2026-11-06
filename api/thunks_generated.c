/* ============================================================
 * 自动生成，勿手改：node api/gen_bindings.js --emit
 * 只有 joy.idl 里 status:on 的 L0/L1 条目会出现在这里。
 * ============================================================ */
#include <joy/joy.h>
#include "joy_api.h"
#include "runtime/joy_value.h"
#include "runtime/joy_obj.h"
#include "runtime/joy_thunk.h"

static void dtor_lcg_random_destroy(void* p) { lcg_random_destroy((lcg_num_p)p); }

static joy_value_t t_lcg_random_create(joy_value_t* a, int n, joy_status_t* st)
{
        uint64_t p0;
        lcg_num_p _r;
        uint64_t _h;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "lcg_random_create");
        if (a[0].kind != JOY_V_U64) return joy_v_err(st, JOY_ERR_TYPE, "lcg_random_create.arg0");
        p0 = (uint64_t)a[0].as.u64;
        _r = lcg_random_create(p0);
        if (!_r) return joy_v_err(st, JOY_ERR_CREATE, "lcg_random_create");
        _h = joy_obj_register((void*)_r, "lcg_num_p", dtor_lcg_random_destroy, "lcg_random_create");
        if (!_h) { lcg_random_destroy(_r); return joy_v_err(st, JOY_ERR_CAPACITY, "lcg_random_create"); }
        return joy_v_handle(_h);
}

static joy_value_t t_lcg_random_next(joy_value_t* a, int n, joy_status_t* st)
{
        lcg_num_p p0;
        uint32_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "lcg_random_next");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "lcg_random_next.arg0");
        p0 = (lcg_num_p)joy_obj_deref(a[0].as.handle, "lcg_num_p", st);
        if (!p0) return joy_v_err(st, *st, "lcg_random_next.arg0");
        _r = lcg_random_next(p0);
        return joy_v_u32((uint32_t)_r);
}

static joy_value_t t_get_ticks(joy_value_t* a, int n, joy_status_t* st)
{
        uint64_t _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "get_ticks");
        _r = get_ticks();
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_jtext_cache_get_count(joy_value_t* a, int n, joy_status_t* st)
{
        int _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jtext_cache_get_count");
        _r = jtext_cache_get_count();
        return joy_v_i32((int)_r);
}

static const joy_thunk_entry s_thunks[] = {
        { "lcg", "lcg_random_create", t_lcg_random_create, 1 },
        { "lcg", "lcg_random_next", t_lcg_random_next, 1 },
        { "ticks", "get_ticks", t_get_ticks, 0 },
        { "jtext", "jtext_cache_get_count", t_jtext_cache_get_count, 0 },
};

const joy_thunk_entry* joy_thunks_table(int* count)
{
        if (count) *count = (int)(sizeof(s_thunks) / sizeof(s_thunks[0]));
        return s_thunks;
}

const char* joy_api_selftest(void)
{
        joy_status_t st = JOY_OK;
        uint32_t live0 = joy_obj_live_count();
        uint32_t dead0 = joy_obj_destroy_count();
        joy_value_t r;
        joy_value_t args[4];

        /* L0：纯标量往返 */
        st = JOY_OK;
        r = t_get_ticks(NULL, 0, &st);
        if (st != JOY_OK || r.kind != JOY_V_U64) return "L0/get_ticks";
        st = JOY_OK;
        t_get_ticks(args, 1, &st);
        if (st != JOY_ERR_ARGC) return "argc-check";

        /* L1：create -> 句柄 -> 类型校验 -> 自动析构 */
        st = JOY_OK;
        args[0] = joy_v_u64((uint64_t)12345);
        r = t_lcg_random_create(args, 1, &st);
        if (st != JOY_OK || r.kind != JOY_V_HANDLE) return "L1/lcg_random_create";
        if (joy_obj_live_count() != live0 + 1) return "live-after-create";

        st = JOY_OK;
        {
                joy_value_t h = t_lcg_random_next(&r, 1, &st);
                if (st != JOY_OK || h.kind != JOY_V_U32) return "handle-arg";
        }

        st = JOY_OK;
        if (joy_obj_deref(r.as.handle, "__wrong_tag__", &st) != NULL || st == JOY_OK)
                return "type-tag-check";

        joy_obj_release(r.as.handle);
        if (joy_obj_destroy_count() != dead0 + 1) return "auto-destroy";
        if (joy_obj_live_count() != live0) return "live-after-release";
        st = JOY_OK;
        if (joy_obj_deref(r.as.handle, "lcg_num_p", &st) != NULL)
                return "stale-handle";

        return NULL;
}
