/* ============================================================
 * 自动生成，勿手改：node api/tools/gen_bindings.js --emit
 * joy.idl 里 status:on 的 L0/L1，以及可展开的 L2 试点条目会出现在这里。
 * ============================================================ */
#include <joy/joy.h>
#include "joy_api.h"
#include "runtime/joy_value.h"
#include "runtime/joy_obj.h"
#include "runtime/joy_thunk.h"

static void dtor_joy_core_lcg_random_destroy(void* p) { joy_core_lcg_random_destroy((lcg_num_p)p); }
static void dtor_joy_core_destroy_window(void* p) { joy_core_destroy_window((jwindow_p)p); }
static void dtor_joy_r2d_font_destroy(void* p) { joy_r2d_font_destroy((font_p)p); }
static void dtor_joy_r2d_destroy(void* p) { joy_r2d_destroy((jr2d_p)p); }
static void dtor_joy_ui_image_destroy(void* p) { joy_ui_image_destroy((image_p)p); }

static joy_value_t t_joy_core_lcg_random_create(joy_value_t* a, int n, joy_status_t* st)
{
        uint64_t p0;
        lcg_num_p _r;
        uint64_t _h;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_lcg_random_create");
        if (a[0].kind != JOY_V_U64) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_lcg_random_create.arg0");
        p0 = (uint64_t)a[0].as.u64;
        _r = joy_core_lcg_random_create(p0);
        if (!_r) return joy_v_err(st, JOY_ERR_CREATE, "joy_core_lcg_random_create");
        _h = joy_obj_register((void*)_r, "lcg_num_p", dtor_joy_core_lcg_random_destroy, "joy_core_lcg_random_create");
        if (!_h) { joy_core_lcg_random_destroy(_r); return joy_v_err(st, JOY_ERR_CAPACITY, "joy_core_lcg_random_create"); }
        return joy_v_handle(_h);
}

static joy_value_t t_joy_core_lcg_random_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        lcg_num_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_lcg_random_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_lcg_random_destroy.arg0");
        p0 = (lcg_num_p)joy_obj_deref(a[0].as.handle, "lcg_num_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_core_lcg_random_destroy.arg0");
        joy_core_lcg_random_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_core_lcg_random_next(joy_value_t* a, int n, joy_status_t* st)
{
        lcg_num_p p0;
        uint32_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_lcg_random_next");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_lcg_random_next.arg0");
        p0 = (lcg_num_p)joy_obj_deref(a[0].as.handle, "lcg_num_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_core_lcg_random_next.arg0");
        _r = joy_core_lcg_random_next(p0);
        return joy_v_u32((uint32_t)_r);
}

static joy_value_t t_joy_core_lcg_random_set_state(joy_value_t* a, int n, joy_status_t* st)
{
        lcg_num_p p0;
        uint64_t p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_lcg_random_set_state");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_lcg_random_set_state.arg0");
        if (a[1].kind != JOY_V_U64) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_lcg_random_set_state.arg1");
        p0 = (lcg_num_p)joy_obj_deref(a[0].as.handle, "lcg_num_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_core_lcg_random_set_state.arg0");
        p1 = (uint64_t)a[1].as.u64;
        joy_core_lcg_random_set_state(p0, p1);
        return joy_v_null();
}

static joy_value_t t_jpm_config_add_spawn(joy_value_t* a, int n, joy_status_t* st)
{
        jpm_config_p p0;
        int p1;
        int p2;
        int p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jpm_config_add_spawn");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jpm_config_add_spawn.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jpm_config_add_spawn.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jpm_config_add_spawn.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jpm_config_add_spawn.arg3");
        p0 = (jpm_config_p)joy_obj_deref(a[0].as.handle, "jpm_config_p", st);
        if (!p0) return joy_v_err(st, *st, "jpm_config_add_spawn.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        p3 = (int)a[3].as.i32;
        jpm_config_add_spawn(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_jpm_wave_seed(joy_value_t* a, int n, joy_status_t* st)
{
        uint32_t p0;
        int p1;
        uint32_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jpm_wave_seed");
        if (a[0].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "jpm_wave_seed.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jpm_wave_seed.arg1");
        p0 = (uint32_t)a[0].as.u32;
        p1 = (int)a[1].as.i32;
        _r = jpm_wave_seed(p0, p1);
        return joy_v_u32((uint32_t)_r);
}

static joy_value_t t_joy_core_sdbm_hash(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        int p1;
        unsigned long _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_sdbm_hash");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_sdbm_hash.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_sdbm_hash.arg1");
        p0 = (const char*)a[0].as.str;
        p1 = (int)a[1].as.i32;
        _r = joy_core_sdbm_hash(p0, p1);
        return joy_v_u64((unsigned long)_r);
}

static joy_value_t t_joy_core_fnv1a_hash(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        int p1;
        unsigned long _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_fnv1a_hash");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_fnv1a_hash.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_fnv1a_hash.arg1");
        p0 = (const char*)a[0].as.str;
        p1 = (int)a[1].as.i32;
        _r = joy_core_fnv1a_hash(p0, p1);
        return joy_v_u64((unsigned long)_r);
}

static joy_value_t t_joy_core_jenkins_hash(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        int p1;
        unsigned long _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_jenkins_hash");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_jenkins_hash.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_jenkins_hash.arg1");
        p0 = (const char*)a[0].as.str;
        p1 = (int)a[1].as.i32;
        _r = joy_core_jenkins_hash(p0, p1);
        return joy_v_u64((unsigned long)_r);
}

static joy_value_t t_joy_core_murmur3_hash(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        unsigned long _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_murmur3_hash");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_murmur3_hash.arg0");
        p0 = (const char*)a[0].as.str;
        _r = joy_core_murmur3_hash(p0);
        return joy_v_u64((unsigned long)_r);
}

static joy_value_t t_joy_core_kr_hash(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        int p1;
        unsigned long _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_kr_hash");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_kr_hash.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_kr_hash.arg1");
        p0 = (const char*)a[0].as.str;
        p1 = (int)a[1].as.i32;
        _r = joy_core_kr_hash(p0, p1);
        return joy_v_u64((unsigned long)_r);
}

static joy_value_t t_joy_scene_jtimer_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jtimer_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_scene_jtimer_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_jtimer_destroy.arg0");
        p0 = (jtimer_p)joy_obj_deref(a[0].as.handle, "jtimer_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_scene_jtimer_destroy.arg0");
        joy_scene_jtimer_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_scene_jtimer_trigger(joy_value_t* a, int n, joy_status_t* st)
{
        jtimer_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_scene_jtimer_trigger");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_jtimer_trigger.arg0");
        p0 = (jtimer_p)joy_obj_deref(a[0].as.handle, "jtimer_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_scene_jtimer_trigger.arg0");
        joy_scene_jtimer_trigger(p0);
        return joy_v_null();
}

static joy_value_t t_joy_scene_jtimer_set_interval(joy_value_t* a, int n, joy_status_t* st)
{
        jtimer_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_scene_jtimer_set_interval");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_jtimer_set_interval.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_jtimer_set_interval.arg1");
        p0 = (jtimer_p)joy_obj_deref(a[0].as.handle, "jtimer_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_scene_jtimer_set_interval.arg0");
        p1 = (int)a[1].as.i32;
        joy_scene_jtimer_set_interval(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_scene_jtimer_set_active(joy_value_t* a, int n, joy_status_t* st)
{
        jtimer_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_scene_jtimer_set_active");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_jtimer_set_active.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_jtimer_set_active.arg1");
        p0 = (jtimer_p)joy_obj_deref(a[0].as.handle, "jtimer_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_scene_jtimer_set_active.arg0");
        p1 = (bool)a[1].as.b;
        joy_scene_jtimer_set_active(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_scene_jtimer_reset(joy_value_t* a, int n, joy_status_t* st)
{
        jtimer_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_scene_jtimer_reset");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_jtimer_reset.arg0");
        p0 = (jtimer_p)joy_obj_deref(a[0].as.handle, "jtimer_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_scene_jtimer_reset.arg0");
        joy_scene_jtimer_reset(p0);
        return joy_v_null();
}

static joy_value_t t_joy_scene_jtimer_get_elapsed(joy_value_t* a, int n, joy_status_t* st)
{
        jtimer_p p0;
        uint64_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_scene_jtimer_get_elapsed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_jtimer_get_elapsed.arg0");
        p0 = (jtimer_p)joy_obj_deref(a[0].as.handle, "jtimer_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_scene_jtimer_get_elapsed.arg0");
        _r = joy_scene_jtimer_get_elapsed(p0);
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_joy_core_init_env(joy_value_t* a, int n, joy_status_t* st)
{
        bool _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_init_env");
        _r = joy_core_init_env();
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_core_quit_env(joy_value_t* a, int n, joy_status_t* st)
{
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_quit_env");
        joy_core_quit_env();
        return joy_v_null();
}

static joy_value_t t_joy_core_get_ticks(joy_value_t* a, int n, joy_status_t* st)
{
        uint64_t _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_get_ticks");
        _r = joy_core_get_ticks();
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_joy_core_create_window(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        int p1;
        int p2;
        jwindow_p _r;
        uint64_t _h;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_create_window");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_create_window.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_create_window.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_create_window.arg2");
        p0 = (const char*)a[0].as.str;
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        _r = joy_core_create_window(p0, p1, p2);
        if (!_r) return joy_v_err(st, JOY_ERR_CREATE, "joy_core_create_window");
        _h = joy_obj_register((void*)_r, "jwindow_p", dtor_joy_core_destroy_window, "joy_core_create_window");
        if (!_h) { joy_core_destroy_window(_r); return joy_v_err(st, JOY_ERR_CAPACITY, "joy_core_create_window"); }
        return joy_v_handle(_h);
}

static joy_value_t t_joy_core_destroy_window(joy_value_t* a, int n, joy_status_t* st)
{
        jwindow_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_destroy_window");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_destroy_window.arg0");
        p0 = (jwindow_p)joy_obj_deref(a[0].as.handle, "jwindow_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_core_destroy_window.arg0");
        joy_core_destroy_window(p0);
        return joy_v_null();
}

static joy_value_t t_joy_core_window_set_fullscreen(joy_value_t* a, int n, joy_status_t* st)
{
        jwindow_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_window_set_fullscreen");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_window_set_fullscreen.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_window_set_fullscreen.arg1");
        p0 = (jwindow_p)joy_obj_deref(a[0].as.handle, "jwindow_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_core_window_set_fullscreen.arg0");
        p1 = (bool)a[1].as.b;
        joy_core_window_set_fullscreen(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_core_window_get_fullscreen(joy_value_t* a, int n, joy_status_t* st)
{
        jwindow_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_window_get_fullscreen");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_window_get_fullscreen.arg0");
        p0 = (jwindow_p)joy_obj_deref(a[0].as.handle, "jwindow_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_core_window_get_fullscreen.arg0");
        _r = joy_core_window_get_fullscreen(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_core_window_set_icon(joy_value_t* a, int n, joy_status_t* st)
{
        jwindow_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_window_set_icon");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_window_set_icon.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_window_set_icon.arg1");
        p0 = (jwindow_p)joy_obj_deref(a[0].as.handle, "jwindow_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_core_window_set_icon.arg0");
        p1 = (const char*)a[1].as.str;
        joy_core_window_set_icon(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_core_push_user_event(joy_value_t* a, int n, joy_status_t* st)
{
        int p0;
        int p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_push_user_event");
        if (a[0].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_push_user_event.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_push_user_event.arg1");
        p0 = (int)a[0].as.i32;
        p1 = (int)a[1].as.i32;
        _r = joy_core_push_user_event(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_core_push_quit_event(joy_value_t* a, int n, joy_status_t* st)
{
        bool _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_push_quit_event");
        _r = joy_core_push_quit_event();
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_core_log_title(joy_value_t* a, int n, joy_status_t* st)
{
        int p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_log_title");
        if (a[0].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_log_title.arg0");
        p0 = (int)a[0].as.i32;
        joy_core_log_title(p0);
        return joy_v_null();
}

static joy_value_t t_joy_core_env_set(joy_value_t* a, int n, joy_status_t* st)
{
        env_p p0;
        const char* p1;
        const char* p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_env_set");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_env_set.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_env_set.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_env_set.arg2");
        p0 = (env_p)joy_obj_deref(a[0].as.handle, "env_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_core_env_set.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (const char*)a[2].as.str;
        joy_core_env_set(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_core_env_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        env_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_env_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_env_destroy.arg0");
        p0 = (env_p)joy_obj_deref(a[0].as.handle, "env_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_core_env_destroy.arg0");
        joy_core_env_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_core_simple_fps_destory(joy_value_t* a, int n, joy_status_t* st)
{
        simple_fps_counter_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_simple_fps_destory");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_simple_fps_destory.arg0");
        p0 = (simple_fps_counter_p)joy_obj_deref(a[0].as.handle, "simple_fps_counter_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_core_simple_fps_destory.arg0");
        joy_core_simple_fps_destory(p0);
        return joy_v_null();
}

static joy_value_t t_joy_core_simple_fps_update(joy_value_t* a, int n, joy_status_t* st)
{
        simple_fps_counter_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_simple_fps_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_simple_fps_update.arg0");
        p0 = (simple_fps_counter_p)joy_obj_deref(a[0].as.handle, "simple_fps_counter_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_core_simple_fps_update.arg0");
        joy_core_simple_fps_update(p0);
        return joy_v_null();
}

static joy_value_t t_joy_core_simple_fps_value(joy_value_t* a, int n, joy_status_t* st)
{
        simple_fps_counter_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_simple_fps_value");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_simple_fps_value.arg0");
        p0 = (simple_fps_counter_p)joy_obj_deref(a[0].as.handle, "simple_fps_counter_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_core_simple_fps_value.arg0");
        _r = joy_core_simple_fps_value(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_scene_game_timer_init(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_scene_game_timer_init");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_game_timer_init.arg0");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_scene_game_timer_init.arg0");
        joy_scene_game_timer_init(p0);
        return joy_v_null();
}

static joy_value_t t_joy_scene_game_timer_update(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_scene_game_timer_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_game_timer_update.arg0");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_scene_game_timer_update.arg0");
        joy_scene_game_timer_update(p0);
        return joy_v_null();
}

static joy_value_t t_joy_scene_game_timer_get_delta_time(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_scene_game_timer_get_delta_time");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_game_timer_get_delta_time.arg0");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_scene_game_timer_get_delta_time.arg0");
        _r = joy_scene_game_timer_get_delta_time(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_joy_scene_game_timer_get_unscaled_delta_time(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_scene_game_timer_get_unscaled_delta_time");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_game_timer_get_unscaled_delta_time.arg0");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_scene_game_timer_get_unscaled_delta_time.arg0");
        _r = joy_scene_game_timer_get_unscaled_delta_time(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_joy_scene_game_timer_set_time_scale(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_scene_game_timer_set_time_scale");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_game_timer_set_time_scale.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_game_timer_set_time_scale.arg1");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_scene_game_timer_set_time_scale.arg0");
        p1 = (float)a[1].as.f64;
        joy_scene_game_timer_set_time_scale(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_scene_game_timer_get_time_scale(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_scene_game_timer_get_time_scale");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_game_timer_get_time_scale.arg0");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_scene_game_timer_get_time_scale.arg0");
        _r = joy_scene_game_timer_get_time_scale(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_joy_scene_game_timer_set_target_fps(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_scene_game_timer_set_target_fps");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_game_timer_set_target_fps.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_game_timer_set_target_fps.arg1");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_scene_game_timer_set_target_fps.arg0");
        p1 = (int)a[1].as.i32;
        joy_scene_game_timer_set_target_fps(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_scene_game_timer_get_target_fps(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_scene_game_timer_get_target_fps");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_scene_game_timer_get_target_fps.arg0");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_scene_game_timer_get_target_fps.arg0");
        _r = joy_scene_game_timer_get_target_fps(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_core_set_clipboard_text(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_core_set_clipboard_text");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_core_set_clipboard_text.arg0");
        p0 = (const char*)a[0].as.str;
        joy_core_set_clipboard_text(p0);
        return joy_v_null();
}

static joy_value_t t_sys_init_netenv(joy_value_t* a, int n, joy_status_t* st)
{
        bool _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "sys_init_netenv");
        _r = sys_init_netenv();
        return joy_v_bool((bool)_r);
}

static joy_value_t t_sys_release_netenv(joy_value_t* a, int n, joy_status_t* st)
{
        bool _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "sys_release_netenv");
        _r = sys_release_netenv();
        return joy_v_bool((bool)_r);
}

static joy_value_t t_sys_anyaddr(joy_value_t* a, int n, joy_status_t* st)
{
        int _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "sys_anyaddr");
        _r = sys_anyaddr();
        return joy_v_i32((int)_r);
}

static joy_value_t t_sys_closesocket(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "sys_closesocket");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "sys_closesocket.arg0");
        p0 = (int64_t)a[0].as.i64;
        _r = sys_closesocket(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_sys_set_sock_sndtimeo(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int p1;
        int _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "sys_set_sock_sndtimeo");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "sys_set_sock_sndtimeo.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "sys_set_sock_sndtimeo.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int)a[1].as.i32;
        _r = sys_set_sock_sndtimeo(p0, p1);
        return joy_v_i32((int)_r);
}

static joy_value_t t_sys_set_sock_rcvtimeo(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int p1;
        int _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "sys_set_sock_rcvtimeo");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "sys_set_sock_rcvtimeo.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "sys_set_sock_rcvtimeo.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int)a[1].as.i32;
        _r = sys_set_sock_rcvtimeo(p0, p1);
        return joy_v_i32((int)_r);
}

static joy_value_t t_sys_set_sock_accpettimeo(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int p1;
        int _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "sys_set_sock_accpettimeo");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "sys_set_sock_accpettimeo.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "sys_set_sock_accpettimeo.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int)a[1].as.i32;
        _r = sys_set_sock_accpettimeo(p0, p1);
        return joy_v_i32((int)_r);
}

static joy_value_t t_sys_set_sock_broadcast(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "sys_set_sock_broadcast");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "sys_set_sock_broadcast.arg0");
        p0 = (int64_t)a[0].as.i64;
        _r = sys_set_sock_broadcast(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_sys_set_sock_reuseaddr(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "sys_set_sock_reuseaddr");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "sys_set_sock_reuseaddr.arg0");
        p0 = (int64_t)a[0].as.i64;
        _r = sys_set_sock_reuseaddr(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_sys_tcp(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "sys_tcp");
        _r = sys_tcp();
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_sys_udp(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "sys_udp");
        _r = sys_udp();
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_sys_bind(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        const char* p1;
        int p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "sys_bind");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "sys_bind.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "sys_bind.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "sys_bind.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        _r = sys_bind(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_sys_connect(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        const char* p1;
        int p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "sys_connect");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "sys_connect.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "sys_connect.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "sys_connect.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        _r = sys_connect(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_sys_listen(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "sys_listen");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "sys_listen.arg0");
        p0 = (int64_t)a[0].as.i64;
        _r = sys_listen(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_sys_send(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        const char* p1;
        int p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "sys_send");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "sys_send.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "sys_send.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "sys_send.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        _r = sys_send(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_sys_sendto(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        const char* p1;
        int p2;
        const char* p3;
        int p4;
        int _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "sys_sendto");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "sys_sendto.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "sys_sendto.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "sys_sendto.arg2");
        if (a[3].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "sys_sendto.arg3");
        if (a[4].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "sys_sendto.arg4");
        p0 = (int64_t)a[0].as.i64;
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        p3 = (const char*)a[3].as.str;
        p4 = (int)a[4].as.i32;
        _r = sys_sendto(p0, p1, p2, p3, p4);
        return joy_v_i32((int)_r);
}

static joy_value_t t_sys_current_time(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "sys_current_time");
        _r = sys_current_time();
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_sys_sleep(joy_value_t* a, int n, joy_status_t* st)
{
        int p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "sys_sleep");
        if (a[0].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "sys_sleep.arg0");
        p0 = (int)a[0].as.i32;
        sys_sleep(p0);
        return joy_v_null();
}

static joy_value_t t_sys_install_crash_handlers(joy_value_t* a, int n, joy_status_t* st)
{
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "sys_install_crash_handlers");
        sys_install_crash_handlers();
        return joy_v_null();
}

static joy_value_t t_joy_net_iocp_server_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        iocp_server_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_iocp_server_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_iocp_server_destroy.arg0");
        p0 = (iocp_server_p)joy_obj_deref(a[0].as.handle, "iocp_server_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_iocp_server_destroy.arg0");
        joy_net_iocp_server_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_iocp_process_events(joy_value_t* a, int n, joy_status_t* st)
{
        iocp_server_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_iocp_process_events");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_iocp_process_events.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_iocp_process_events.arg1");
        p0 = (iocp_server_p)joy_obj_deref(a[0].as.handle, "iocp_server_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_iocp_process_events.arg0");
        p1 = (int)a[1].as.i32;
        joy_net_iocp_process_events(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_font_create(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        int p1;
        font_p _r;
        uint64_t _h;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_font_create");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_font_create.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_font_create.arg1");
        p0 = (const char*)a[0].as.str;
        p1 = (int)a[1].as.i32;
        _r = joy_r2d_font_create(p0, p1);
        if (!_r) return joy_v_err(st, JOY_ERR_CREATE, "joy_r2d_font_create");
        _h = joy_obj_register((void*)_r, "font_p", dtor_joy_r2d_font_destroy, "joy_r2d_font_create");
        if (!_h) { joy_r2d_font_destroy(_r); return joy_v_err(st, JOY_ERR_CAPACITY, "joy_r2d_font_create"); }
        return joy_v_handle(_h);
}

static joy_value_t t_joy_r2d_font_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        font_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_font_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_font_destroy.arg0");
        p0 = (font_p)joy_obj_deref(a[0].as.handle, "font_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_font_destroy.arg0");
        joy_r2d_font_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_font_get_scale(joy_value_t* a, int n, joy_status_t* st)
{
        font_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_font_get_scale");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_font_get_scale.arg0");
        p0 = (font_p)joy_obj_deref(a[0].as.handle, "font_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_font_get_scale.arg0");
        _r = joy_r2d_font_get_scale(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_joy_r2d_font_get_size(joy_value_t* a, int n, joy_status_t* st)
{
        font_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_font_get_size");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_font_get_size.arg0");
        p0 = (font_p)joy_obj_deref(a[0].as.handle, "font_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_font_get_size.arg0");
        _r = joy_r2d_font_get_size(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_r2d_font_get_ascent(joy_value_t* a, int n, joy_status_t* st)
{
        font_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_font_get_ascent");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_font_get_ascent.arg0");
        p0 = (font_p)joy_obj_deref(a[0].as.handle, "font_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_font_get_ascent.arg0");
        _r = joy_r2d_font_get_ascent(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_r2d_create(joy_value_t* a, int n, joy_status_t* st)
{
        jwindow_p p0;
        jr2d_p _r;
        uint64_t _h;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_create");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_create.arg0");
        p0 = (jwindow_p)joy_obj_deref(a[0].as.handle, "jwindow_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_create.arg0");
        _r = joy_r2d_create(p0);
        if (!_r) return joy_v_err(st, JOY_ERR_CREATE, "joy_r2d_create");
        _h = joy_obj_register((void*)_r, "jr2d_p", dtor_joy_r2d_destroy, "joy_r2d_create");
        if (!_h) { joy_r2d_destroy(_r); return joy_v_err(st, JOY_ERR_CAPACITY, "joy_r2d_create"); }
        return joy_v_handle(_h);
}

static joy_value_t t_joy_r2d_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_destroy.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_destroy.arg0");
        joy_r2d_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_set_color(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        uint8_t p1;
        uint8_t p2;
        uint8_t p3;
        uint8_t p4;
        bool _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_set_color");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_color.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_color.arg1");
        if (a[2].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_color.arg2");
        if (a[3].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_color.arg3");
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_color.arg4");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_set_color.arg0");
        p1 = (uint8_t)a[1].as.u32;
        p2 = (uint8_t)a[2].as.u32;
        p3 = (uint8_t)a[3].as.u32;
        p4 = (uint8_t)a[4].as.u32;
        _r = joy_r2d_set_color(p0, p1, p2, p3, p4);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r2d_begin(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_begin");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_begin.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_begin.arg0");
        joy_r2d_begin(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_end(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_end.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_end.arg0");
        joy_r2d_end(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_set_logical_presentation(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        int p1;
        int p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_set_logical_presentation");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_logical_presentation.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_logical_presentation.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_logical_presentation.arg2");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_set_logical_presentation.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        _r = joy_r2d_set_logical_presentation(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r2d_set_logical_presentation_mode(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        int p1;
        int p2;
        int p3;
        bool _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_set_logical_presentation_mode");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_logical_presentation_mode.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_logical_presentation_mode.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_logical_presentation_mode.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_logical_presentation_mode.arg3");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_set_logical_presentation_mode.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        p3 = (int)a[3].as.i32;
        _r = joy_r2d_set_logical_presentation_mode(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r2d_set_vsync(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        int p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_set_vsync");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_vsync.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_vsync.arg1");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_set_vsync.arg0");
        p1 = (int)a[1].as.i32;
        _r = joy_r2d_set_vsync(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r2d_set_scale(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_set_scale");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_scale.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_scale.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_scale.arg2");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_set_scale.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_r2d_set_scale(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r2d_set_colorf(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        float p1;
        float p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_set_colorf");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_colorf.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_colorf.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_colorf.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_colorf.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_colorf.arg4");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_set_colorf.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        joy_r2d_set_colorf(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_set_blend_mode(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        int p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_set_blend_mode");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_blend_mode.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_blend_mode.arg1");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_set_blend_mode.arg0");
        p1 = (int)a[1].as.i32;
        _r = joy_r2d_set_blend_mode(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r2d_draw_point(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_draw_point");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_draw_point.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_draw_point.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_draw_point.arg2");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_draw_point.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_r2d_draw_point(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r2d_set_clip_rect(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        jrect_t p1 = { 0 };
        bool _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_set_clip_rect");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_clip_rect.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_set_clip_rect.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_clip_rect.arg1");
        p1.x = (float)a[1].as.f64;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_clip_rect.arg2");
        p1.y = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_clip_rect.arg3");
        p1.w = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_set_clip_rect.arg4");
        p1.h = (float)a[4].as.f64;
        _r = joy_r2d_set_clip_rect(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r2d_clear_clip(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_clear_clip");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_clear_clip.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_clear_clip.arg0");
        joy_r2d_clear_clip(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_push_clip_rect(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        jrect_t p1 = { 0 };
        bool _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_push_clip_rect");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_push_clip_rect.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_push_clip_rect.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_push_clip_rect.arg1");
        p1.x = (float)a[1].as.f64;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_push_clip_rect.arg2");
        p1.y = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_push_clip_rect.arg3");
        p1.w = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_push_clip_rect.arg4");
        p1.h = (float)a[4].as.f64;
        _r = joy_r2d_push_clip_rect(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r2d_pop_clip_rect(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_pop_clip_rect");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_pop_clip_rect.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_pop_clip_rect.arg0");
        joy_r2d_pop_clip_rect(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_reset_clip_stack(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_reset_clip_stack");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_reset_clip_stack.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_reset_clip_stack.arg0");
        joy_r2d_reset_clip_stack(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_point_in_clip(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_point_in_clip");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_point_in_clip.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_point_in_clip.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_point_in_clip.arg2");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_point_in_clip.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_r2d_point_in_clip(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_shape_draw_line(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        float p1;
        float p2;
        float p3;
        float p4;
        bool _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "shape_draw_line");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_line.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_line.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_line.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_line.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_line.arg4");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "shape_draw_line.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        _r = shape_draw_line(p0, p1, p2, p3, p4);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_shape_draw_rectangle(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        const char* p1;
        jrect_t p2 = { 0 };
        bool _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "shape_draw_rectangle");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_rectangle.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "shape_draw_rectangle.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_rectangle.arg1");
        p1 = (const char*)a[1].as.str;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_rectangle.arg2");
        p2.x = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_rectangle.arg3");
        p2.y = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_rectangle.arg4");
        p2.w = (float)a[4].as.f64;
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_rectangle.arg5");
        p2.h = (float)a[5].as.f64;
        _r = shape_draw_rectangle(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_image_create(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        const char* p1;
        image_p _r;
        uint64_t _h;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_image_create");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_create.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_create.arg1");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_image_create.arg0");
        p1 = (const char*)a[1].as.str;
        _r = joy_ui_image_create(p0, p1);
        if (!_r) return joy_v_err(st, JOY_ERR_CREATE, "joy_ui_image_create");
        _h = joy_obj_register((void*)_r, "image_p", dtor_joy_ui_image_destroy, "joy_ui_image_create");
        if (!_h) { joy_ui_image_destroy(_r); return joy_v_err(st, JOY_ERR_CAPACITY, "joy_ui_image_create"); }
        return joy_v_handle(_h);
}

static joy_value_t t_joy_ui_image_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        image_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_image_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_destroy.arg0");
        p0 = (image_p)joy_obj_deref(a[0].as.handle, "image_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_image_destroy.arg0");
        joy_ui_image_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_image_set_alpha(joy_value_t* a, int n, joy_status_t* st)
{
        image_p p0;
        uint8_t p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_image_set_alpha");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_set_alpha.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_set_alpha.arg1");
        p0 = (image_p)joy_obj_deref(a[0].as.handle, "image_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_image_set_alpha.arg0");
        p1 = (uint8_t)a[1].as.u32;
        joy_ui_image_set_alpha(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_image_set_blend_mode(joy_value_t* a, int n, joy_status_t* st)
{
        image_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_image_set_blend_mode");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_set_blend_mode.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_set_blend_mode.arg1");
        p0 = (image_p)joy_obj_deref(a[0].as.handle, "image_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_image_set_blend_mode.arg0");
        p1 = (int)a[1].as.i32;
        joy_ui_image_set_blend_mode(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_image_set_scale_mode(joy_value_t* a, int n, joy_status_t* st)
{
        image_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_image_set_scale_mode");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_set_scale_mode.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_set_scale_mode.arg1");
        p0 = (image_p)joy_obj_deref(a[0].as.handle, "image_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_image_set_scale_mode.arg0");
        p1 = (int)a[1].as.i32;
        joy_ui_image_set_scale_mode(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_text_updatex(joy_value_t* a, int n, joy_status_t* st)
{
        jtext_p p0;
        font_p p1;
        const char* p2;
        int p3;
        jcolor_t p4 = { 0 };
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_text_updatex");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_updatex.arg0");
        p0 = (jtext_p)joy_obj_deref(a[0].as.handle, "jtext_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_text_updatex.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_updatex.arg1");
        p1 = (font_p)joy_obj_deref(a[1].as.handle, "font_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_ui_text_updatex.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_updatex.arg2");
        p2 = (const char*)a[2].as.str;
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_updatex.arg3");
        p3 = (int)a[3].as.i32;
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_updatex.arg4");
        p4.r = (uint8_t)a[4].as.u32;
        if (a[5].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_updatex.arg5");
        p4.g = (uint8_t)a[5].as.u32;
        if (a[6].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_updatex.arg6");
        p4.b = (uint8_t)a[6].as.u32;
        if (a[7].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_updatex.arg7");
        p4.a = (uint8_t)a[7].as.u32;
        joy_ui_text_updatex(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_ui_text_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jtext_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_text_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_destroy.arg0");
        p0 = (jtext_p)joy_obj_deref(a[0].as.handle, "jtext_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_text_destroy.arg0");
        joy_ui_text_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_jtext_print(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        jtext_p p1;
        float p2;
        float p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_jtext_print");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_print.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_print.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_print.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_print.arg3");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_jtext_print.arg0");
        p1 = (jtext_p)joy_obj_deref(a[1].as.handle, "jtext_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r2d_jtext_print.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        joy_r2d_jtext_print(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_jtext_printx(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        font_p p1;
        const char* p2;
        int p3;
        jcolor_t p4 = { 0 };
        float p5;
        float p6;
        if (n != 10) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_jtext_printx");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printx.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_jtext_printx.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printx.arg1");
        p1 = (font_p)joy_obj_deref(a[1].as.handle, "font_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r2d_jtext_printx.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printx.arg2");
        p2 = (const char*)a[2].as.str;
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printx.arg3");
        p3 = (int)a[3].as.i32;
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printx.arg4");
        p4.r = (uint8_t)a[4].as.u32;
        if (a[5].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printx.arg5");
        p4.g = (uint8_t)a[5].as.u32;
        if (a[6].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printx.arg6");
        p4.b = (uint8_t)a[6].as.u32;
        if (a[7].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printx.arg7");
        p4.a = (uint8_t)a[7].as.u32;
        if (a[8].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printx.arg8");
        p5 = (float)a[8].as.f64;
        if (a[9].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printx.arg9");
        p6 = (float)a[9].as.f64;
        joy_r2d_jtext_printx(p0, p1, p2, p3, p4, p5, p6);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_draw_point_f(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_draw_point_f");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_draw_point_f.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_draw_point_f.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_draw_point_f.arg2");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_draw_point_f.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_r2d_draw_point_f(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_shape_draw_rectangle_f(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        const char* p1;
        float p2;
        float p3;
        float p4;
        float p5;
        bool _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "shape_draw_rectangle_f");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_rectangle_f.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_rectangle_f.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_rectangle_f.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_rectangle_f.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_rectangle_f.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_rectangle_f.arg5");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "shape_draw_rectangle_f.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = shape_draw_rectangle_f(p0, p1, p2, p3, p4, p5);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_shape_draw_line_f(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        float p1;
        float p2;
        float p3;
        float p4;
        bool _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "shape_draw_line_f");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_line_f.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_line_f.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_line_f.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_line_f.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "shape_draw_line_f.arg4");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "shape_draw_line_f.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        _r = shape_draw_line_f(p0, p1, p2, p3, p4);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r2d_jtext_printf(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        font_p p1;
        const char* p2;
        int p3;
        jcolor_t p4 = { 0 };
        float p5;
        float p6;
        if (n != 10) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_jtext_printf");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_jtext_printf.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf.arg1");
        p1 = (font_p)joy_obj_deref(a[1].as.handle, "font_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r2d_jtext_printf.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf.arg2");
        p2 = (const char*)a[2].as.str;
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf.arg3");
        p3 = (int)a[3].as.i32;
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf.arg4");
        p4.r = (uint8_t)a[4].as.u32;
        if (a[5].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf.arg5");
        p4.g = (uint8_t)a[5].as.u32;
        if (a[6].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf.arg6");
        p4.b = (uint8_t)a[6].as.u32;
        if (a[7].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf.arg7");
        p4.a = (uint8_t)a[7].as.u32;
        if (a[8].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf.arg8");
        p5 = (float)a[8].as.f64;
        if (a[9].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf.arg9");
        p6 = (float)a[9].as.f64;
        joy_r2d_jtext_printf(p0, p1, p2, p3, p4, p5, p6);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_jtext_printf_scaled(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        font_p p1;
        const char* p2;
        int p3;
        jcolor_t p4 = { 0 };
        float p5;
        float p6;
        float p7;
        if (n != 11) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_jtext_printf_scaled");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf_scaled.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_jtext_printf_scaled.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf_scaled.arg1");
        p1 = (font_p)joy_obj_deref(a[1].as.handle, "font_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r2d_jtext_printf_scaled.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf_scaled.arg2");
        p2 = (const char*)a[2].as.str;
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf_scaled.arg3");
        p3 = (int)a[3].as.i32;
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf_scaled.arg4");
        p4.r = (uint8_t)a[4].as.u32;
        if (a[5].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf_scaled.arg5");
        p4.g = (uint8_t)a[5].as.u32;
        if (a[6].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf_scaled.arg6");
        p4.b = (uint8_t)a[6].as.u32;
        if (a[7].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf_scaled.arg7");
        p4.a = (uint8_t)a[7].as.u32;
        if (a[8].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf_scaled.arg8");
        p5 = (float)a[8].as.f64;
        if (a[9].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf_scaled.arg9");
        p6 = (float)a[9].as.f64;
        if (a[10].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_printf_scaled.arg10");
        p7 = (float)a[10].as.f64;
        joy_r2d_jtext_printf_scaled(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_null();
}

static joy_value_t t_joy_ui_text_get_width(joy_value_t* a, int n, joy_status_t* st)
{
        jtext_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_text_get_width");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_get_width.arg0");
        p0 = (jtext_p)joy_obj_deref(a[0].as.handle, "jtext_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_text_get_width.arg0");
        _r = joy_ui_text_get_width(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_ui_text_get_height(joy_value_t* a, int n, joy_status_t* st)
{
        jtext_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_text_get_height");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_get_height.arg0");
        p0 = (jtext_p)joy_obj_deref(a[0].as.handle, "jtext_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_text_get_height.arg0");
        _r = joy_ui_text_get_height(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_r2d_jtext_cache_clear(joy_value_t* a, int n, joy_status_t* st)
{
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_jtext_cache_clear");
        joy_r2d_jtext_cache_clear();
        return joy_v_null();
}

static joy_value_t t_joy_r2d_jtext_cache_set_limit(joy_value_t* a, int n, joy_status_t* st)
{
        int p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_jtext_cache_set_limit");
        if (a[0].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_cache_set_limit.arg0");
        p0 = (int)a[0].as.i32;
        joy_r2d_jtext_cache_set_limit(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_jtext_cache_get_count(joy_value_t* a, int n, joy_status_t* st)
{
        int _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_jtext_cache_get_count");
        _r = joy_r2d_jtext_cache_get_count();
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_r2d_jtext_cache_sweep(joy_value_t* a, int n, joy_status_t* st)
{
        int p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_jtext_cache_sweep");
        if (a[0].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_jtext_cache_sweep.arg0");
        p0 = (int)a[0].as.i32;
        joy_r2d_jtext_cache_sweep(p0);
        return joy_v_null();
}

static joy_value_t t_animation_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        animation_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "animation_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "animation_destroy.arg0");
        p0 = (animation_p)joy_obj_deref(a[0].as.handle, "animation_p", st);
        if (!p0) return joy_v_err(st, *st, "animation_destroy.arg0");
        animation_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_animation_reset(joy_value_t* a, int n, joy_status_t* st)
{
        animation_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "animation_reset");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "animation_reset.arg0");
        p0 = (animation_p)joy_obj_deref(a[0].as.handle, "animation_p", st);
        if (!p0) return joy_v_err(st, *st, "animation_reset.arg0");
        animation_reset(p0);
        return joy_v_null();
}

static joy_value_t t_animation_add_clip(joy_value_t* a, int n, joy_status_t* st)
{
        animation_p p0;
        const char* p1;
        float p2;
        jrect_t p3 = { 0 };
        if (n != 7) return joy_v_err(st, JOY_ERR_ARGC, "animation_add_clip");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "animation_add_clip.arg0");
        p0 = (animation_p)joy_obj_deref(a[0].as.handle, "animation_p", st);
        if (!p0) return joy_v_err(st, *st, "animation_add_clip.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "animation_add_clip.arg1");
        p1 = (const char*)a[1].as.str;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "animation_add_clip.arg2");
        p2 = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "animation_add_clip.arg3");
        p3.x = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "animation_add_clip.arg4");
        p3.y = (float)a[4].as.f64;
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "animation_add_clip.arg5");
        p3.w = (float)a[5].as.f64;
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "animation_add_clip.arg6");
        p3.h = (float)a[6].as.f64;
        animation_add_clip(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_animation_is_finished(joy_value_t* a, int n, joy_status_t* st)
{
        animation_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "animation_is_finished");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "animation_is_finished.arg0");
        p0 = (animation_p)joy_obj_deref(a[0].as.handle, "animation_p", st);
        if (!p0) return joy_v_err(st, *st, "animation_is_finished.arg0");
        _r = animation_is_finished(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_animation_set_position(joy_value_t* a, int n, joy_status_t* st)
{
        animation_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "animation_set_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "animation_set_position.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "animation_set_position.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "animation_set_position.arg2");
        p0 = (animation_p)joy_obj_deref(a[0].as.handle, "animation_p", st);
        if (!p0) return joy_v_err(st, *st, "animation_set_position.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        animation_set_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_animation_set_scale(joy_value_t* a, int n, joy_status_t* st)
{
        animation_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "animation_set_scale");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "animation_set_scale.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "animation_set_scale.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "animation_set_scale.arg2");
        p0 = (animation_p)joy_obj_deref(a[0].as.handle, "animation_p", st);
        if (!p0) return joy_v_err(st, *st, "animation_set_scale.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        animation_set_scale(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_animation_set_rotation(joy_value_t* a, int n, joy_status_t* st)
{
        animation_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "animation_set_rotation");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "animation_set_rotation.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "animation_set_rotation.arg1");
        p0 = (animation_p)joy_obj_deref(a[0].as.handle, "animation_p", st);
        if (!p0) return joy_v_err(st, *st, "animation_set_rotation.arg0");
        p1 = (float)a[1].as.f64;
        animation_set_rotation(p0, p1);
        return joy_v_null();
}

static joy_value_t t_animation_update(joy_value_t* a, int n, joy_status_t* st)
{
        animation_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "animation_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "animation_update.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "animation_update.arg1");
        p0 = (animation_p)joy_obj_deref(a[0].as.handle, "animation_p", st);
        if (!p0) return joy_v_err(st, *st, "animation_update.arg0");
        p1 = (float)a[1].as.f64;
        animation_update(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_destroy.arg0");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_destroy.arg0");
        joy_r3d_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_begin(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_begin");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_begin.arg0");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_begin.arg0");
        _r = joy_r3d_begin(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r3d_end(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_end.arg0");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_end.arg0");
        joy_r3d_end(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_get_draw_calls(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_get_draw_calls");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_get_draw_calls.arg0");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_get_draw_calls.arg0");
        _r = joy_r3d_get_draw_calls(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_r3d_set_clear_color(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        uint8_t p1;
        uint8_t p2;
        uint8_t p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_set_clear_color");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_set_clear_color.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_set_clear_color.arg1");
        if (a[2].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_set_clear_color.arg2");
        if (a[3].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_set_clear_color.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_set_clear_color.arg0");
        p1 = (uint8_t)a[1].as.u32;
        p2 = (uint8_t)a[2].as.u32;
        p3 = (uint8_t)a[3].as.u32;
        joy_r3d_set_clear_color(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_mesh_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_mesh_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_mesh_destroy.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_mesh_destroy.arg1");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_mesh_destroy.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_mesh_destroy.arg1");
        joy_r3d_mesh_destroy(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_font_init(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        const char* p1;
        int p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_font_init");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_init.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_init.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_init.arg2");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_font_init.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        _r = joy_r3d_font_init(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r3d_font_draw_2d(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        const char* p2;
        float p3;
        float p4;
        float p5;
        float p6;
        float p7;
        bool _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_font_draw_2d");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_draw_2d.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_draw_2d.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_draw_2d.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_draw_2d.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_draw_2d.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_draw_2d.arg5");
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_draw_2d.arg6");
        if (a[7].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_draw_2d.arg7");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_font_draw_2d.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_font_draw_2d.arg1");
        p2 = (const char*)a[2].as.str;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        p6 = (float)a[6].as.f64;
        p7 = (float)a[7].as.f64;
        _r = joy_r3d_font_draw_2d(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r3d_font_cache_set_limit(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_font_cache_set_limit");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_cache_set_limit.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_cache_set_limit.arg1");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_font_cache_set_limit.arg0");
        p1 = (int)a[1].as.i32;
        joy_r3d_font_cache_set_limit(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_font_cache_get_count(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_font_cache_get_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_cache_get_count.arg0");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_font_cache_get_count.arg0");
        _r = joy_r3d_font_cache_get_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_r3d_font_cache_clear(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_font_cache_clear");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_cache_clear.arg0");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_font_cache_clear.arg0");
        joy_r3d_font_cache_clear(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_font_cache_sweep(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_font_cache_sweep");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_cache_sweep.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_font_cache_sweep.arg1");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_font_cache_sweep.arg0");
        p1 = (int)a[1].as.i32;
        joy_r3d_font_cache_sweep(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_set_logical_presentation(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        int p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_set_logical_presentation");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_set_logical_presentation.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_set_logical_presentation.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_set_logical_presentation.arg2");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_set_logical_presentation.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        joy_r3d_set_logical_presentation(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_set_logical_presentation_mode(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        int p1;
        int p2;
        int p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_set_logical_presentation_mode");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_set_logical_presentation_mode.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_set_logical_presentation_mode.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_set_logical_presentation_mode.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_set_logical_presentation_mode.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_set_logical_presentation_mode.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        p3 = (int)a[3].as.i32;
        joy_r3d_set_logical_presentation_mode(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_button_draw(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_button_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_button_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_draw.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_draw.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_draw.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_draw.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_button_draw.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_button_draw.arg1");
        p2 = (jr3d_button_p)joy_obj_deref(a[2].as.handle, "jr3d_button_p", st);
        if (!p2) return joy_v_err(st, *st, "joy_r3d_button_draw.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        joy_r3d_button_draw(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_button_draw_circle(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_button_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_button_draw_circle");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_draw_circle.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_draw_circle.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_draw_circle.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_draw_circle.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_draw_circle.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_button_draw_circle.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_button_draw_circle.arg1");
        p2 = (jr3d_button_p)joy_obj_deref(a[2].as.handle, "jr3d_button_p", st);
        if (!p2) return joy_v_err(st, *st, "joy_r3d_button_draw_circle.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        joy_r3d_button_draw_circle(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_button_is_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_button_p p1;
        float p2;
        float p3;
        bool _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_button_is_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_is_hovered.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_is_hovered.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_is_hovered.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_is_hovered.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_button_is_hovered.arg0");
        p1 = (jr3d_button_p)joy_obj_deref(a[1].as.handle, "jr3d_button_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_button_is_hovered.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = joy_r3d_button_is_hovered(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r3d_button_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_button_p p1;
        bool p2;
        float p3;
        float p4;
        bool _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_button_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_is_pressed.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_is_pressed.arg1");
        if (a[2].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_is_pressed.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_is_pressed.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_is_pressed.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_button_is_pressed.arg0");
        p1 = (jr3d_button_p)joy_obj_deref(a[1].as.handle, "jr3d_button_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_button_is_pressed.arg1");
        p2 = (bool)a[2].as.b;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        _r = joy_r3d_button_is_pressed(p0, p1, p2, p3, p4);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r3d_button_release(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_button_p p1;
        float p2;
        float p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_button_release");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_release.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_release.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_release.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_release.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_button_release.arg0");
        p1 = (jr3d_button_p)joy_obj_deref(a[1].as.handle, "jr3d_button_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_button_release.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        joy_r3d_button_release(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_button_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_button_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_button_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_button_destroy.arg0");
        p0 = (jr3d_button_p)joy_obj_deref(a[0].as.handle, "jr3d_button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_button_destroy.arg0");
        joy_r3d_button_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_label_set_text(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_label_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_label_set_text");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_label_set_text.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_label_set_text.arg1");
        p0 = (jr3d_label_p)joy_obj_deref(a[0].as.handle, "jr3d_label_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_label_set_text.arg0");
        p1 = (const char*)a[1].as.str;
        joy_r3d_label_set_text(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_label_draw(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_label_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_label_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_label_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_label_draw.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_label_draw.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_label_draw.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_label_draw.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_label_draw.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_label_draw.arg1");
        p2 = (jr3d_label_p)joy_obj_deref(a[2].as.handle, "jr3d_label_p", st);
        if (!p2) return joy_v_err(st, *st, "joy_r3d_label_draw.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        joy_r3d_label_draw(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_label_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_label_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_label_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_label_destroy.arg0");
        p0 = (jr3d_label_p)joy_obj_deref(a[0].as.handle, "jr3d_label_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_label_destroy.arg0");
        joy_r3d_label_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_set_text(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_set_text");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_set_text.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_set_text.arg1");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_set_text.arg0");
        p1 = (const char*)a[1].as.str;
        joy_r3d_input_set_text(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_is_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_input_p p1;
        float p2;
        float p3;
        bool _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_is_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_is_hovered.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_is_hovered.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_is_hovered.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_is_hovered.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_is_hovered.arg0");
        p1 = (jr3d_input_p)joy_obj_deref(a[1].as.handle, "jr3d_input_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_input_is_hovered.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = joy_r3d_input_is_hovered(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r3d_input_focus(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_focus");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_focus.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_focus.arg0");
        joy_r3d_input_focus(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_defocus(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_defocus");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_defocus.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_defocus.arg0");
        joy_r3d_input_defocus(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_is_focused(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_is_focused");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_is_focused.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_is_focused.arg0");
        _r = joy_r3d_input_is_focused(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r3d_input_start_text_input(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_input_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_start_text_input");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_start_text_input.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_start_text_input.arg1");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_start_text_input.arg0");
        p1 = (jr3d_input_p)joy_obj_deref(a[1].as.handle, "jr3d_input_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_input_start_text_input.arg1");
        joy_r3d_input_start_text_input(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_stop_text_input(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_input_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_stop_text_input");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_stop_text_input.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_stop_text_input.arg1");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_stop_text_input.arg0");
        p1 = (jr3d_input_p)joy_obj_deref(a[1].as.handle, "jr3d_input_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_input_stop_text_input.arg1");
        joy_r3d_input_stop_text_input(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_insert(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_insert");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_insert.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_insert.arg1");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_insert.arg0");
        p1 = (const char*)a[1].as.str;
        joy_r3d_input_insert(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_backspace(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_backspace");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_backspace.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_backspace.arg0");
        joy_r3d_input_backspace(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_delete(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_delete");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_delete.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_delete.arg0");
        joy_r3d_input_delete(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_cursor_left(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_cursor_left");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_cursor_left.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_cursor_left.arg0");
        joy_r3d_input_cursor_left(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_cursor_right(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_cursor_right");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_cursor_right.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_cursor_right.arg0");
        joy_r3d_input_cursor_right(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_cursor_home(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_cursor_home");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_cursor_home.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_cursor_home.arg0");
        joy_r3d_input_cursor_home(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_cursor_end(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_cursor_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_cursor_end.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_cursor_end.arg0");
        joy_r3d_input_cursor_end(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_click(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_input_p p1;
        float p2;
        float p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_click");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_click.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_click.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_click.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_click.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_click.arg0");
        p1 = (jr3d_input_p)joy_obj_deref(a[1].as.handle, "jr3d_input_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_input_click.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        joy_r3d_input_click(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_submit(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_submit");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_submit.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_submit.arg0");
        joy_r3d_input_submit(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_draw(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_input_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_draw.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_draw.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_draw.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_draw.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_draw.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_input_draw.arg1");
        p2 = (jr3d_input_p)joy_obj_deref(a[2].as.handle, "jr3d_input_p", st);
        if (!p2) return joy_v_err(st, *st, "joy_r3d_input_draw.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        joy_r3d_input_draw(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_input_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_input_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_input_destroy.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_input_destroy.arg0");
        joy_r3d_input_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_combobox_input_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_combobox_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_combobox_input_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_destroy.arg0");
        p0 = (jr3d_combobox_input_p)joy_obj_deref(a[0].as.handle, "jr3d_combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_combobox_input_destroy.arg0");
        joy_r3d_combobox_input_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_combobox_input_add_item(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_combobox_input_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_combobox_input_add_item");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_add_item.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_add_item.arg1");
        p0 = (jr3d_combobox_input_p)joy_obj_deref(a[0].as.handle, "jr3d_combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_combobox_input_add_item.arg0");
        p1 = (const char*)a[1].as.str;
        joy_r3d_combobox_input_add_item(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_combobox_input_clear_items(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_combobox_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_combobox_input_clear_items");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_clear_items.arg0");
        p0 = (jr3d_combobox_input_p)joy_obj_deref(a[0].as.handle, "jr3d_combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_combobox_input_clear_items.arg0");
        joy_r3d_combobox_input_clear_items(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_combobox_input_set_text(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_combobox_input_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_combobox_input_set_text");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_set_text.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_set_text.arg1");
        p0 = (jr3d_combobox_input_p)joy_obj_deref(a[0].as.handle, "jr3d_combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_combobox_input_set_text.arg0");
        p1 = (const char*)a[1].as.str;
        joy_r3d_combobox_input_set_text(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_combobox_input_hit_item(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_combobox_input_p p0;
        float p1;
        float p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_combobox_input_hit_item");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_hit_item.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_hit_item.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_hit_item.arg2");
        p0 = (jr3d_combobox_input_p)joy_obj_deref(a[0].as.handle, "jr3d_combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_combobox_input_hit_item.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_r3d_combobox_input_hit_item(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_r3d_combobox_input_draw(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_combobox_input_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_combobox_input_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_draw.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_draw.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_draw.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_draw.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_combobox_input_draw.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_combobox_input_draw.arg1");
        p2 = (jr3d_combobox_input_p)joy_obj_deref(a[2].as.handle, "jr3d_combobox_input_p", st);
        if (!p2) return joy_v_err(st, *st, "joy_r3d_combobox_input_draw.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        joy_r3d_combobox_input_draw(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_combobox_input_draw_dropdown(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_combobox_input_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_combobox_input_draw_dropdown");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_draw_dropdown.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_draw_dropdown.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_draw_dropdown.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_draw_dropdown.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_combobox_input_draw_dropdown.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_combobox_input_draw_dropdown.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_combobox_input_draw_dropdown.arg1");
        p2 = (jr3d_combobox_input_p)joy_obj_deref(a[2].as.handle, "jr3d_combobox_input_p", st);
        if (!p2) return joy_v_err(st, *st, "joy_r3d_combobox_input_draw_dropdown.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        joy_r3d_combobox_input_draw_dropdown(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_slider_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_slider_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_slider_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_destroy.arg0");
        p0 = (jr3d_slider_p)joy_obj_deref(a[0].as.handle, "jr3d_slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_slider_destroy.arg0");
        joy_r3d_slider_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_slider_set_value(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_slider_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_slider_set_value");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_set_value.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_set_value.arg1");
        p0 = (jr3d_slider_p)joy_obj_deref(a[0].as.handle, "jr3d_slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_slider_set_value.arg0");
        p1 = (float)a[1].as.f64;
        joy_r3d_slider_set_value(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_slider_get_value(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_slider_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_slider_get_value");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_get_value.arg0");
        p0 = (jr3d_slider_p)joy_obj_deref(a[0].as.handle, "jr3d_slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_slider_get_value.arg0");
        _r = joy_r3d_slider_get_value(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_joy_r3d_slider_draw(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_slider_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_slider_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_draw.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_draw.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_draw.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_draw.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_slider_draw.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_slider_draw.arg1");
        p2 = (jr3d_slider_p)joy_obj_deref(a[2].as.handle, "jr3d_slider_p", st);
        if (!p2) return joy_v_err(st, *st, "joy_r3d_slider_draw.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        joy_r3d_slider_draw(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_slider_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_slider_p p1;
        float p2;
        float p3;
        bool _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_slider_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_is_pressed.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_is_pressed.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_is_pressed.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_slider_is_pressed.arg0");
        p1 = (jr3d_slider_p)joy_obj_deref(a[1].as.handle, "jr3d_slider_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_slider_is_pressed.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = joy_r3d_slider_is_pressed(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r3d_slider_set_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_slider_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_slider_set_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_set_pressed.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_set_pressed.arg1");
        p0 = (jr3d_slider_p)joy_obj_deref(a[0].as.handle, "jr3d_slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_slider_set_pressed.arg0");
        p1 = (bool)a[1].as.b;
        joy_r3d_slider_set_pressed(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_slider_drag_move(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_slider_p p1;
        float p2;
        float p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_slider_drag_move");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_drag_move.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_drag_move.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_drag_move.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_drag_move.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_slider_drag_move.arg0");
        p1 = (jr3d_slider_p)joy_obj_deref(a[1].as.handle, "jr3d_slider_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_slider_drag_move.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        joy_r3d_slider_drag_move(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_slider_is_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_slider_p p1;
        float p2;
        float p3;
        bool _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_slider_is_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_is_hovered.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_is_hovered.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_is_hovered.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_is_hovered.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_slider_is_hovered.arg0");
        p1 = (jr3d_slider_p)joy_obj_deref(a[1].as.handle, "jr3d_slider_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_slider_is_hovered.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = joy_r3d_slider_is_hovered(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r3d_slider_set_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_slider_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_slider_set_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_set_hovered.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_slider_set_hovered.arg1");
        p0 = (jr3d_slider_p)joy_obj_deref(a[0].as.handle, "jr3d_slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_slider_set_hovered.arg0");
        p1 = (bool)a[1].as.b;
        joy_r3d_slider_set_hovered(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_checkbox_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_checkbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_checkbox_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_destroy.arg0");
        p0 = (jr3d_checkbox_p)joy_obj_deref(a[0].as.handle, "jr3d_checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_checkbox_destroy.arg0");
        joy_r3d_checkbox_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_checkbox_toggle(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_checkbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_checkbox_toggle");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_toggle.arg0");
        p0 = (jr3d_checkbox_p)joy_obj_deref(a[0].as.handle, "jr3d_checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_checkbox_toggle.arg0");
        joy_r3d_checkbox_toggle(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_checkbox_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_checkbox_p p1;
        float p2;
        float p3;
        bool _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_checkbox_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_is_pressed.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_is_pressed.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_is_pressed.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_checkbox_is_pressed.arg0");
        p1 = (jr3d_checkbox_p)joy_obj_deref(a[1].as.handle, "jr3d_checkbox_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_checkbox_is_pressed.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = joy_r3d_checkbox_is_pressed(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r3d_checkbox_set_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_checkbox_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_checkbox_set_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_hovered.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_hovered.arg1");
        p0 = (jr3d_checkbox_p)joy_obj_deref(a[0].as.handle, "jr3d_checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_checkbox_set_hovered.arg0");
        p1 = (bool)a[1].as.b;
        joy_r3d_checkbox_set_hovered(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_checkbox_is_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_checkbox_p p1;
        float p2;
        float p3;
        bool _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_checkbox_is_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_is_hovered.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_is_hovered.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_is_hovered.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_is_hovered.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_checkbox_is_hovered.arg0");
        p1 = (jr3d_checkbox_p)joy_obj_deref(a[1].as.handle, "jr3d_checkbox_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_checkbox_is_hovered.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = joy_r3d_checkbox_is_hovered(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r3d_checkbox_set_colors(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_checkbox_p p0;
        jcolor_t p1 = { 0 };
        jcolor_t p2 = { 0 };
        jcolor_t p3 = { 0 };
        if (n != 13) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_checkbox_set_colors");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_colors.arg0");
        p0 = (jr3d_checkbox_p)joy_obj_deref(a[0].as.handle, "jr3d_checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_checkbox_set_colors.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_colors.arg1");
        p1.r = (uint8_t)a[1].as.u32;
        if (a[2].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_colors.arg2");
        p1.g = (uint8_t)a[2].as.u32;
        if (a[3].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_colors.arg3");
        p1.b = (uint8_t)a[3].as.u32;
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_colors.arg4");
        p1.a = (uint8_t)a[4].as.u32;
        if (a[5].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_colors.arg5");
        p2.r = (uint8_t)a[5].as.u32;
        if (a[6].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_colors.arg6");
        p2.g = (uint8_t)a[6].as.u32;
        if (a[7].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_colors.arg7");
        p2.b = (uint8_t)a[7].as.u32;
        if (a[8].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_colors.arg8");
        p2.a = (uint8_t)a[8].as.u32;
        if (a[9].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_colors.arg9");
        p3.r = (uint8_t)a[9].as.u32;
        if (a[10].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_colors.arg10");
        p3.g = (uint8_t)a[10].as.u32;
        if (a[11].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_colors.arg11");
        p3.b = (uint8_t)a[11].as.u32;
        if (a[12].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_set_colors.arg12");
        p3.a = (uint8_t)a[12].as.u32;
        joy_r3d_checkbox_set_colors(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_checkbox_draw(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_checkbox_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_checkbox_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_draw.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_draw.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_draw.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_checkbox_draw.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_checkbox_draw.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_checkbox_draw.arg1");
        p2 = (jr3d_checkbox_p)joy_obj_deref(a[2].as.handle, "jr3d_checkbox_p", st);
        if (!p2) return joy_v_err(st, *st, "joy_r3d_checkbox_draw.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        joy_r3d_checkbox_draw(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_draw_grid(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        int p1;
        float p2;
        float p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_draw_grid");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_draw_grid.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_draw_grid.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_draw_grid.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_draw_grid.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_draw_grid.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        joy_r3d_draw_grid(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_janimator_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "janimator_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_destroy.arg0");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_destroy.arg0");
        janimator_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_janimator_set_float(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "janimator_set_float");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_float.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_float.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_float.arg2");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_set_float.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (float)a[2].as.f64;
        janimator_set_float(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_janimator_set_int(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "janimator_set_int");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_int.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_int.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_int.arg2");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_set_int.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        janimator_set_int(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_janimator_set_bool(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        bool p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "janimator_set_bool");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_bool.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_bool.arg1");
        if (a[2].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_bool.arg2");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_set_bool.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (bool)a[2].as.b;
        janimator_set_bool(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_janimator_set_trigger(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "janimator_set_trigger");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_trigger.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_trigger.arg1");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_set_trigger.arg0");
        p1 = (const char*)a[1].as.str;
        janimator_set_trigger(p0, p1);
        return joy_v_null();
}

static joy_value_t t_janimator_get_float(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "janimator_get_float");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_get_float.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_get_float.arg1");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_get_float.arg0");
        p1 = (const char*)a[1].as.str;
        _r = janimator_get_float(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_janimator_get_int(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        int _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "janimator_get_int");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_get_int.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_get_int.arg1");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_get_int.arg0");
        p1 = (const char*)a[1].as.str;
        _r = janimator_get_int(p0, p1);
        return joy_v_i32((int)_r);
}

static joy_value_t t_janimator_get_bool(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "janimator_get_bool");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_get_bool.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_get_bool.arg1");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_get_bool.arg0");
        p1 = (const char*)a[1].as.str;
        _r = janimator_get_bool(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_janimator_add_state(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "janimator_add_state");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_add_state.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_add_state.arg1");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_add_state.arg0");
        p1 = (const char*)a[1].as.str;
        janimator_add_state(p0, p1);
        return joy_v_null();
}

static joy_value_t t_janimator_set_default_state(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "janimator_set_default_state");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_default_state.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_default_state.arg1");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_set_default_state.arg0");
        p1 = (const char*)a[1].as.str;
        janimator_set_default_state(p0, p1);
        return joy_v_null();
}

static joy_value_t t_janimator_state_add_clip(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        const char* p2;
        float p3;
        jrect_t p4 = { 0 };
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "janimator_state_add_clip");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_clip.arg0");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_state_add_clip.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_clip.arg1");
        p1 = (const char*)a[1].as.str;
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_clip.arg2");
        p2 = (const char*)a[2].as.str;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_clip.arg3");
        p3 = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_clip.arg4");
        p4.x = (float)a[4].as.f64;
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_clip.arg5");
        p4.y = (float)a[5].as.f64;
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_clip.arg6");
        p4.w = (float)a[6].as.f64;
        if (a[7].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_clip.arg7");
        p4.h = (float)a[7].as.f64;
        janimator_state_add_clip(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_janimator_state_add_color(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        float p2;
        int p3;
        int p4;
        uint8_t p5;
        uint8_t p6;
        uint8_t p7;
        uint8_t p8;
        if (n != 9) return joy_v_err(st, JOY_ERR_ARGC, "janimator_state_add_color");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_color.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_color.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_color.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_color.arg3");
        if (a[4].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_color.arg4");
        if (a[5].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_color.arg5");
        if (a[6].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_color.arg6");
        if (a[7].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_color.arg7");
        if (a[8].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_color.arg8");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_state_add_color.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (float)a[2].as.f64;
        p3 = (int)a[3].as.i32;
        p4 = (int)a[4].as.i32;
        p5 = (uint8_t)a[5].as.u32;
        p6 = (uint8_t)a[6].as.u32;
        p7 = (uint8_t)a[7].as.u32;
        p8 = (uint8_t)a[8].as.u32;
        janimator_state_add_color(p0, p1, p2, p3, p4, p5, p6, p7, p8);
        return joy_v_null();
}

static joy_value_t t_janimator_state_set_loop(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        bool p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "janimator_state_set_loop");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_set_loop.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_set_loop.arg1");
        if (a[2].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_set_loop.arg2");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_state_set_loop.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (bool)a[2].as.b;
        janimator_state_set_loop(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_janimator_state_set_speed(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "janimator_state_set_speed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_set_speed.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_set_speed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_set_speed.arg2");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_state_set_speed.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (float)a[2].as.f64;
        janimator_state_set_speed(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_janimator_state_set_blend(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        const char* p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "janimator_state_set_blend");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_set_blend.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_set_blend.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_set_blend.arg2");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_state_set_blend.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (const char*)a[2].as.str;
        janimator_state_set_blend(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_janimator_state_add_motion(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        const char* p2;
        float p3;
        jrect_t p4 = { 0 };
        float p5;
        if (n != 9) return joy_v_err(st, JOY_ERR_ARGC, "janimator_state_add_motion");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_motion.arg0");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_state_add_motion.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_motion.arg1");
        p1 = (const char*)a[1].as.str;
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_motion.arg2");
        p2 = (const char*)a[2].as.str;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_motion.arg3");
        p3 = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_motion.arg4");
        p4.x = (float)a[4].as.f64;
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_motion.arg5");
        p4.y = (float)a[5].as.f64;
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_motion.arg6");
        p4.w = (float)a[6].as.f64;
        if (a[7].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_motion.arg7");
        p4.h = (float)a[7].as.f64;
        if (a[8].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_state_add_motion.arg8");
        p5 = (float)a[8].as.f64;
        janimator_state_add_motion(p0, p1, p2, p3, p4, p5);
        return joy_v_null();
}

static joy_value_t t_janimator_add_transition(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        const char* p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "janimator_add_transition");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_add_transition.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_add_transition.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_add_transition.arg2");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_add_transition.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (const char*)a[2].as.str;
        _r = janimator_add_transition(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_janimator_add_any_transition(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        const char* p1;
        int _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "janimator_add_any_transition");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_add_any_transition.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_add_any_transition.arg1");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_add_any_transition.arg0");
        p1 = (const char*)a[1].as.str;
        _r = janimator_add_any_transition(p0, p1);
        return joy_v_i32((int)_r);
}

static joy_value_t t_janimator_transition_add_cond(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        int p1;
        const char* p2;
        int p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "janimator_transition_add_cond");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_transition_add_cond.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "janimator_transition_add_cond.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "janimator_transition_add_cond.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "janimator_transition_add_cond.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_transition_add_cond.arg4");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_transition_add_cond.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (const char*)a[2].as.str;
        p3 = (int)a[3].as.i32;
        p4 = (float)a[4].as.f64;
        janimator_transition_add_cond(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_janimator_transition_set_exit_time(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        int p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "janimator_transition_set_exit_time");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_transition_set_exit_time.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "janimator_transition_set_exit_time.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_transition_set_exit_time.arg2");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_transition_set_exit_time.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (float)a[2].as.f64;
        janimator_transition_set_exit_time(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_janimator_update(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "janimator_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_update.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_update.arg1");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_update.arg0");
        p1 = (float)a[1].as.f64;
        janimator_update(p0, p1);
        return joy_v_null();
}

static joy_value_t t_janimator_draw(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "janimator_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_draw.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_draw.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "janimator_draw.arg2");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_draw.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        janimator_draw(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_janimator_set_flip_h(joy_value_t* a, int n, joy_status_t* st)
{
        janimator_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "janimator_set_flip_h");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_flip_h.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "janimator_set_flip_h.arg1");
        p0 = (janimator_p)joy_obj_deref(a[0].as.handle, "janimator_p", st);
        if (!p0) return joy_v_err(st, *st, "janimator_set_flip_h.arg0");
        p1 = (bool)a[1].as.b;
        janimator_set_flip_h(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_label_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        label_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_label_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_label_destroy.arg0");
        p0 = (label_p)joy_obj_deref(a[0].as.handle, "label_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_label_destroy.arg0");
        joy_ui_label_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_label_draw(joy_value_t* a, int n, joy_status_t* st)
{
        label_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_label_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_label_draw.arg0");
        p0 = (label_p)joy_obj_deref(a[0].as.handle, "label_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_label_draw.arg0");
        joy_ui_label_draw(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_button_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_destroy.arg0");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_destroy.arg0");
        joy_ui_button_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_button_set_position(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_set_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_position.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_position.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_position.arg2");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_set_position.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_ui_button_set_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_button_set_size(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_set_size");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_size.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_size.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_size.arg2");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_set_size.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_ui_button_set_size(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_button_set_normal_color(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        jcolor_t p1 = { 0 };
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_set_normal_color");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_normal_color.arg0");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_set_normal_color.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_normal_color.arg1");
        p1.r = (uint8_t)a[1].as.u32;
        if (a[2].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_normal_color.arg2");
        p1.g = (uint8_t)a[2].as.u32;
        if (a[3].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_normal_color.arg3");
        p1.b = (uint8_t)a[3].as.u32;
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_normal_color.arg4");
        p1.a = (uint8_t)a[4].as.u32;
        joy_ui_button_set_normal_color(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_button_set_hover_color(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        jcolor_t p1 = { 0 };
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_set_hover_color");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_hover_color.arg0");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_set_hover_color.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_hover_color.arg1");
        p1.r = (uint8_t)a[1].as.u32;
        if (a[2].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_hover_color.arg2");
        p1.g = (uint8_t)a[2].as.u32;
        if (a[3].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_hover_color.arg3");
        p1.b = (uint8_t)a[3].as.u32;
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_hover_color.arg4");
        p1.a = (uint8_t)a[4].as.u32;
        joy_ui_button_set_hover_color(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_button_set_pressed_color(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        jcolor_t p1 = { 0 };
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_set_pressed_color");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_pressed_color.arg0");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_set_pressed_color.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_pressed_color.arg1");
        p1.r = (uint8_t)a[1].as.u32;
        if (a[2].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_pressed_color.arg2");
        p1.g = (uint8_t)a[2].as.u32;
        if (a[3].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_pressed_color.arg3");
        p1.b = (uint8_t)a[3].as.u32;
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_pressed_color.arg4");
        p1.a = (uint8_t)a[4].as.u32;
        joy_ui_button_set_pressed_color(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_button_set_image(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        image_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_set_image");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_image.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_image.arg1");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_set_image.arg0");
        p1 = (image_p)joy_obj_deref(a[1].as.handle, "image_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_ui_button_set_image.arg1");
        joy_ui_button_set_image(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_button_set_pressed_image(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        image_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_set_pressed_image");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_pressed_image.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_pressed_image.arg1");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_set_pressed_image.arg0");
        p1 = (image_p)joy_obj_deref(a[1].as.handle, "image_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_ui_button_set_pressed_image.arg1");
        joy_ui_button_set_pressed_image(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_button_set_textx(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        font_p p1;
        const char* p2;
        int p3;
        jcolor_t p4 = { 0 };
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_set_textx");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_textx.arg0");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_set_textx.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_textx.arg1");
        p1 = (font_p)joy_obj_deref(a[1].as.handle, "font_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_ui_button_set_textx.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_textx.arg2");
        p2 = (const char*)a[2].as.str;
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_textx.arg3");
        p3 = (int)a[3].as.i32;
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_textx.arg4");
        p4.r = (uint8_t)a[4].as.u32;
        if (a[5].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_textx.arg5");
        p4.g = (uint8_t)a[5].as.u32;
        if (a[6].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_textx.arg6");
        p4.b = (uint8_t)a[6].as.u32;
        if (a[7].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_textx.arg7");
        p4.a = (uint8_t)a[7].as.u32;
        joy_ui_button_set_textx(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_ui_button_is_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_is_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_is_hovered.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_is_hovered.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_is_hovered.arg2");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_is_hovered.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_button_is_hovered(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_button_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_is_pressed.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_is_pressed.arg2");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_is_pressed.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_button_is_pressed(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_button_set_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_set_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_hovered.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_hovered.arg1");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_set_hovered.arg0");
        p1 = (bool)a[1].as.b;
        joy_ui_button_set_hovered(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_button_set_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_set_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_pressed.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_pressed.arg1");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_set_pressed.arg0");
        p1 = (bool)a[1].as.b;
        joy_ui_button_set_pressed(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_button_get_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_get_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_get_pressed.arg0");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_get_pressed.arg0");
        _r = joy_ui_button_get_pressed(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_button_set_text_offset(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_set_text_offset");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_text_offset.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_text_offset.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_set_text_offset.arg2");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_set_text_offset.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_ui_button_set_text_offset(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_button_draw(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button_draw.arg0");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button_draw.arg0");
        joy_ui_button_draw(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_scrollview_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scrollview_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_destroy.arg0");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scrollview_destroy.arg0");
        joy_ui_scrollview_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_scrollview_set_view_px(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        float p1;
        float p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scrollview_set_view_px");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_set_view_px.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_set_view_px.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_set_view_px.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_set_view_px.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_set_view_px.arg4");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scrollview_set_view_px.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        joy_ui_scrollview_set_view_px(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_ui_scrollview_set_content(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scrollview_set_content");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_set_content.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_set_content.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_set_content.arg2");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scrollview_set_content.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_ui_scrollview_set_content(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_scrollview_set_offset(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scrollview_set_offset");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_set_offset.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_set_offset.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_set_offset.arg2");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scrollview_set_offset.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_ui_scrollview_set_offset(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_scrollview_clamp(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scrollview_clamp");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_clamp.arg0");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scrollview_clamp.arg0");
        joy_ui_scrollview_clamp(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_scrollview_get_offset_x(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scrollview_get_offset_x");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_get_offset_x.arg0");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scrollview_get_offset_x.arg0");
        _r = joy_ui_scrollview_get_offset_x(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_joy_ui_scrollview_get_offset_y(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scrollview_get_offset_y");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_get_offset_y.arg0");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scrollview_get_offset_y.arg0");
        _r = joy_ui_scrollview_get_offset_y(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_joy_ui_scrollview_set_show_bar(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scrollview_set_show_bar");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_set_show_bar.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_set_show_bar.arg1");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scrollview_set_show_bar.arg0");
        p1 = (bool)a[1].as.b;
        joy_ui_scrollview_set_show_bar(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_scrollview_dragging(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scrollview_dragging");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_dragging.arg0");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scrollview_dragging.arg0");
        _r = joy_ui_scrollview_dragging(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_scrollview_hit(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scrollview_hit");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_hit.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_hit.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_hit.arg2");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scrollview_hit.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_scrollview_hit(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_scrollview_begin(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scrollview_begin");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_begin.arg0");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scrollview_begin.arg0");
        joy_ui_scrollview_begin(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_scrollview_end(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scrollview_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollview_end.arg0");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scrollview_end.arg0");
        joy_ui_scrollview_end(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_combobox_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_destroy.arg0");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_destroy.arg0");
        joy_ui_combobox_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_combobox_add_item(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        font_p p1;
        const char* p2;
        int p3;
        jcolor_t p4 = { 0 };
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_add_item");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_add_item.arg0");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_add_item.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_add_item.arg1");
        p1 = (font_p)joy_obj_deref(a[1].as.handle, "font_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_ui_combobox_add_item.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_add_item.arg2");
        p2 = (const char*)a[2].as.str;
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_add_item.arg3");
        p3 = (int)a[3].as.i32;
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_add_item.arg4");
        p4.r = (uint8_t)a[4].as.u32;
        if (a[5].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_add_item.arg5");
        p4.g = (uint8_t)a[5].as.u32;
        if (a[6].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_add_item.arg6");
        p4.b = (uint8_t)a[6].as.u32;
        if (a[7].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_add_item.arg7");
        p4.a = (uint8_t)a[7].as.u32;
        joy_ui_combobox_add_item(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_ui_combobox_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_is_pressed.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_is_pressed.arg2");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_is_pressed.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_combobox_is_pressed(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_combobox_is_open(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_is_open");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_is_open.arg0");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_is_open.arg0");
        _r = joy_ui_combobox_is_open(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_combobox_set_open(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_set_open");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_set_open.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_set_open.arg1");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_set_open.arg0");
        p1 = (bool)a[1].as.b;
        joy_ui_combobox_set_open(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_combobox_hit_item(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        float p1;
        float p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_hit_item");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_hit_item.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_hit_item.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_hit_item.arg2");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_hit_item.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_combobox_hit_item(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_ui_combobox_set_selected(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_set_selected");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_set_selected.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_set_selected.arg1");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_set_selected.arg0");
        p1 = (int)a[1].as.i32;
        joy_ui_combobox_set_selected(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_combobox_get_selected(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_get_selected");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_get_selected.arg0");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_get_selected.arg0");
        _r = joy_ui_combobox_get_selected(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_ui_combobox_draw(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_draw.arg0");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_draw.arg0");
        joy_ui_combobox_draw(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_combobox_input_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_input_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_destroy.arg0");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_input_destroy.arg0");
        joy_ui_combobox_input_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_combobox_input_add_item(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_input_add_item");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_add_item.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_add_item.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_add_item.arg2");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_input_add_item.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        joy_ui_combobox_input_add_item(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_combobox_input_clear_items(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_input_clear_items");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_clear_items.arg0");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_input_clear_items.arg0");
        joy_ui_combobox_input_clear_items(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_combobox_input_set_text(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_input_set_text");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_set_text.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_set_text.arg1");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_input_set_text.arg0");
        p1 = (const char*)a[1].as.str;
        joy_ui_combobox_input_set_text(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_combobox_input_draw(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_input_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_draw.arg0");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_input_draw.arg0");
        joy_ui_combobox_input_draw(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_combobox_input_draw_dropdown(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_input_draw_dropdown");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_draw_dropdown.arg0");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_input_draw_dropdown.arg0");
        joy_ui_combobox_input_draw_dropdown(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_combobox_input_is_open(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_input_is_open");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_is_open.arg0");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_input_is_open.arg0");
        _r = joy_ui_combobox_input_is_open(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_combobox_input_set_popup_node(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        scene_node_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_input_set_popup_node");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_set_popup_node.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_set_popup_node.arg1");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_input_set_popup_node.arg0");
        p1 = (scene_node_p)joy_obj_deref(a[1].as.handle, "scene_node_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_ui_combobox_input_set_popup_node.arg1");
        joy_ui_combobox_input_set_popup_node(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_combobox_input_hit_item(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        float p1;
        float p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_input_hit_item");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_hit_item.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_hit_item.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_hit_item.arg2");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_input_hit_item.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_combobox_input_hit_item(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_ui_combobox_input_set_open(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_combobox_input_set_open");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_set_open.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_combobox_input_set_open.arg1");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_combobox_input_set_open.arg0");
        p1 = (bool)a[1].as.b;
        joy_ui_combobox_input_set_open(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_datagrid_setheaders(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_datagrid_setheaders");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_setheaders.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_setheaders.arg1");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_datagrid_setheaders.arg0");
        p1 = (const char*)a[1].as.str;
        joy_ui_datagrid_setheaders(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_datagrid_setheader(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        int p1;
        const char* p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_datagrid_setheader");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_setheader.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_setheader.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_setheader.arg2");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_datagrid_setheader.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (const char*)a[2].as.str;
        joy_ui_datagrid_setheader(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_datagrid_setcell(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        int p1;
        int p2;
        const char* p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_datagrid_setcell");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_setcell.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_setcell.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_setcell.arg2");
        if (a[3].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_setcell.arg3");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_datagrid_setcell.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        p3 = (const char*)a[3].as.str;
        joy_ui_datagrid_setcell(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_ui_datagrid_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_datagrid_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_destroy.arg0");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_datagrid_destroy.arg0");
        joy_ui_datagrid_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_datagrid_draw(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        jr2d_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_datagrid_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_draw.arg1");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_datagrid_draw.arg0");
        p1 = (jr2d_p)joy_obj_deref(a[1].as.handle, "jr2d_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_ui_datagrid_draw.arg1");
        joy_ui_datagrid_draw(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_datagrid_hit_row(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        float p1;
        float p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_datagrid_hit_row");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_hit_row.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_hit_row.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_hit_row.arg2");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_datagrid_hit_row.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_datagrid_hit_row(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_ui_datagrid_scroll(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_datagrid_scroll");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_scroll.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_scroll.arg1");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_datagrid_scroll.arg0");
        p1 = (int)a[1].as.i32;
        joy_ui_datagrid_scroll(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_datagrid_set_selected_row(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_datagrid_set_selected_row");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_set_selected_row.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_datagrid_set_selected_row.arg1");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_datagrid_set_selected_row.arg0");
        p1 = (int)a[1].as.i32;
        joy_ui_datagrid_set_selected_row(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_radio_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        radio_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_radio_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_radio_destroy.arg0");
        p0 = (radio_p)joy_obj_deref(a[0].as.handle, "radio_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_radio_destroy.arg0");
        joy_ui_radio_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_radio_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        radio_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_radio_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_radio_is_pressed.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_radio_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_radio_is_pressed.arg2");
        p0 = (radio_p)joy_obj_deref(a[0].as.handle, "radio_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_radio_is_pressed.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_radio_is_pressed(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_radio_set_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        radio_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_radio_set_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_radio_set_hovered.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_radio_set_hovered.arg1");
        p0 = (radio_p)joy_obj_deref(a[0].as.handle, "radio_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_radio_set_hovered.arg0");
        p1 = (bool)a[1].as.b;
        joy_ui_radio_set_hovered(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_checkbox_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        checkbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_checkbox_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_destroy.arg0");
        p0 = (checkbox_p)joy_obj_deref(a[0].as.handle, "checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_checkbox_destroy.arg0");
        joy_ui_checkbox_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_checkbox_set_colors(joy_value_t* a, int n, joy_status_t* st)
{
        checkbox_p p0;
        jcolor_t p1 = { 0 };
        jcolor_t p2 = { 0 };
        jcolor_t p3 = { 0 };
        if (n != 13) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_checkbox_set_colors");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_colors.arg0");
        p0 = (checkbox_p)joy_obj_deref(a[0].as.handle, "checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_checkbox_set_colors.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_colors.arg1");
        p1.r = (uint8_t)a[1].as.u32;
        if (a[2].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_colors.arg2");
        p1.g = (uint8_t)a[2].as.u32;
        if (a[3].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_colors.arg3");
        p1.b = (uint8_t)a[3].as.u32;
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_colors.arg4");
        p1.a = (uint8_t)a[4].as.u32;
        if (a[5].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_colors.arg5");
        p2.r = (uint8_t)a[5].as.u32;
        if (a[6].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_colors.arg6");
        p2.g = (uint8_t)a[6].as.u32;
        if (a[7].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_colors.arg7");
        p2.b = (uint8_t)a[7].as.u32;
        if (a[8].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_colors.arg8");
        p2.a = (uint8_t)a[8].as.u32;
        if (a[9].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_colors.arg9");
        p3.r = (uint8_t)a[9].as.u32;
        if (a[10].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_colors.arg10");
        p3.g = (uint8_t)a[10].as.u32;
        if (a[11].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_colors.arg11");
        p3.b = (uint8_t)a[11].as.u32;
        if (a[12].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_colors.arg12");
        p3.a = (uint8_t)a[12].as.u32;
        joy_ui_checkbox_set_colors(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_ui_checkbox_set_r2d(joy_value_t* a, int n, joy_status_t* st)
{
        checkbox_p p0;
        jr2d_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_checkbox_set_r2d");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_r2d.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_r2d.arg1");
        p0 = (checkbox_p)joy_obj_deref(a[0].as.handle, "checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_checkbox_set_r2d.arg0");
        p1 = (jr2d_p)joy_obj_deref(a[1].as.handle, "jr2d_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_ui_checkbox_set_r2d.arg1");
        joy_ui_checkbox_set_r2d(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_checkbox_set_position(joy_value_t* a, int n, joy_status_t* st)
{
        checkbox_p p0;
        int p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_checkbox_set_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_position.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_position.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_position.arg2");
        p0 = (checkbox_p)joy_obj_deref(a[0].as.handle, "checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_checkbox_set_position.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        joy_ui_checkbox_set_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_checkbox_set_text_color(joy_value_t* a, int n, joy_status_t* st)
{
        checkbox_p p0;
        jcolor_t p1 = { 0 };
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_checkbox_set_text_color");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_text_color.arg0");
        p0 = (checkbox_p)joy_obj_deref(a[0].as.handle, "checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_checkbox_set_text_color.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_text_color.arg1");
        p1.r = (uint8_t)a[1].as.u32;
        if (a[2].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_text_color.arg2");
        p1.g = (uint8_t)a[2].as.u32;
        if (a[3].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_text_color.arg3");
        p1.b = (uint8_t)a[3].as.u32;
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_text_color.arg4");
        p1.a = (uint8_t)a[4].as.u32;
        joy_ui_checkbox_set_text_color(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_checkbox_toggle(joy_value_t* a, int n, joy_status_t* st)
{
        checkbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_checkbox_toggle");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_toggle.arg0");
        p0 = (checkbox_p)joy_obj_deref(a[0].as.handle, "checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_checkbox_toggle.arg0");
        joy_ui_checkbox_toggle(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_checkbox_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        checkbox_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_checkbox_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_is_pressed.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_is_pressed.arg2");
        p0 = (checkbox_p)joy_obj_deref(a[0].as.handle, "checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_checkbox_is_pressed.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_checkbox_is_pressed(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_checkbox_set_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        checkbox_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_checkbox_set_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_hovered.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_checkbox_set_hovered.arg1");
        p0 = (checkbox_p)joy_obj_deref(a[0].as.handle, "checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_checkbox_set_hovered.arg0");
        p1 = (bool)a[1].as.b;
        joy_ui_checkbox_set_hovered(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_input_joystick_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_input_joystick_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_destroy.arg0");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_input_joystick_destroy.arg0");
        joy_input_joystick_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_input_joystick_set_textures(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        const char* p1;
        const char* p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_input_joystick_set_textures");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_set_textures.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_set_textures.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_set_textures.arg2");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_input_joystick_set_textures.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (const char*)a[2].as.str;
        joy_input_joystick_set_textures(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_input_joystick_set_vector(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_input_joystick_set_vector");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_set_vector.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_set_vector.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_set_vector.arg2");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_input_joystick_set_vector.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_input_joystick_set_vector(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_input_joystick_draw(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_input_joystick_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_draw.arg0");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_input_joystick_draw.arg0");
        joy_input_joystick_draw(p0);
        return joy_v_null();
}

static joy_value_t t_joy_input_joystick_reset(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_input_joystick_reset");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_reset.arg0");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_input_joystick_reset.arg0");
        joy_input_joystick_reset(p0);
        return joy_v_null();
}

static joy_value_t t_joy_input_joystick_set_position(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_input_joystick_set_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_set_position.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_set_position.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_set_position.arg2");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_input_joystick_set_position.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_input_joystick_set_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_input_joystick_set_radius(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_input_joystick_set_radius");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_set_radius.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_set_radius.arg1");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_input_joystick_set_radius.arg0");
        p1 = (float)a[1].as.f64;
        joy_input_joystick_set_radius(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_input_joystick_is_dragging(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_input_joystick_is_dragging");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_is_dragging.arg0");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_input_joystick_is_dragging.arg0");
        _r = joy_input_joystick_is_dragging(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_input_joystick_touch_down(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_input_joystick_touch_down");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_touch_down.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_touch_down.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_touch_down.arg2");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_input_joystick_touch_down.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_input_joystick_touch_down(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_input_joystick_touch_move(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_input_joystick_touch_move");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_touch_move.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_touch_move.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_touch_move.arg2");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_input_joystick_touch_move.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_input_joystick_touch_move(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_input_joystick_touch_up(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_input_joystick_touch_up");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_touch_up.arg0");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_input_joystick_touch_up.arg0");
        joy_input_joystick_touch_up(p0);
        return joy_v_null();
}

static joy_value_t t_joy_input_joystick_get_magnitude(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_input_joystick_get_magnitude");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_input_joystick_get_magnitude.arg0");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_input_joystick_get_magnitude.arg0");
        _r = joy_input_joystick_get_magnitude(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_joy_ui_inputbox_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_destroy.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_destroy.arg0");
        joy_ui_inputbox_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_set_text(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_set_text");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_set_text.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_set_text.arg1");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_set_text.arg0");
        p1 = (const char*)a[1].as.str;
        joy_ui_inputbox_set_text(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_draw(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_draw.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_draw.arg0");
        joy_ui_inputbox_draw(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_is_pressed.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_is_pressed.arg2");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_is_pressed.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_inputbox_is_pressed(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_inputbox_is_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_is_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_is_hovered.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_is_hovered.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_is_hovered.arg2");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_is_hovered.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_inputbox_is_hovered(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_inputbox_focus(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_focus");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_focus.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_focus.arg0");
        joy_ui_inputbox_focus(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_defocus(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_defocus");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_defocus.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_defocus.arg0");
        joy_ui_inputbox_defocus(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_blur(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_blur");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_blur.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_blur.arg0");
        joy_ui_inputbox_blur(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_insert(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_insert");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_insert.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_insert.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_insert.arg2");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_insert.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        joy_ui_inputbox_insert(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_backspace(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_backspace");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_backspace.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_backspace.arg0");
        joy_ui_inputbox_backspace(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_delete(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_delete");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_delete.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_delete.arg0");
        joy_ui_inputbox_delete(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_cursor_left(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_cursor_left");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_cursor_left.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_cursor_left.arg0");
        joy_ui_inputbox_cursor_left(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_cursor_right(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_cursor_right");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_cursor_right.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_cursor_right.arg0");
        joy_ui_inputbox_cursor_right(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_cursor_home(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_cursor_home");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_cursor_home.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_cursor_home.arg0");
        joy_ui_inputbox_cursor_home(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_cursor_end(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_cursor_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_cursor_end.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_cursor_end.arg0");
        joy_ui_inputbox_cursor_end(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_select_left(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_select_left");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_select_left.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_select_left.arg0");
        joy_ui_inputbox_select_left(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_select_right(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_select_right");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_select_right.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_select_right.arg0");
        joy_ui_inputbox_select_right(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_select_home(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_select_home");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_select_home.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_select_home.arg0");
        joy_ui_inputbox_select_home(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_select_end(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_select_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_select_end.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_select_end.arg0");
        joy_ui_inputbox_select_end(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_select_all(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_select_all");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_select_all.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_select_all.arg0");
        joy_ui_inputbox_select_all(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_paste(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_paste");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_paste.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_paste.arg0");
        joy_ui_inputbox_paste(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_copy(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_copy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_copy.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_copy.arg0");
        joy_ui_inputbox_copy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_cut(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_cut");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_cut.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_cut.arg0");
        joy_ui_inputbox_cut(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_inputbox_is_focused(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_inputbox_is_focused");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_inputbox_is_focused.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_inputbox_is_focused.arg0");
        _r = joy_ui_inputbox_is_focused(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_slider_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_slider_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_destroy.arg0");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_slider_destroy.arg0");
        joy_ui_slider_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_slider_set_value(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_slider_set_value");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_set_value.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_set_value.arg1");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_slider_set_value.arg0");
        p1 = (float)a[1].as.f64;
        joy_ui_slider_set_value(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_slider_get_value(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_slider_get_value");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_get_value.arg0");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_slider_get_value.arg0");
        _r = joy_ui_slider_get_value(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_joy_ui_slider_set_rect(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        float p1;
        float p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_slider_set_rect");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_set_rect.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_set_rect.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_set_rect.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_set_rect.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_set_rect.arg4");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_slider_set_rect.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        joy_ui_slider_set_rect(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_ui_slider_set_range(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_slider_set_range");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_set_range.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_set_range.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_set_range.arg2");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_slider_set_range.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_ui_slider_set_range(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_slider_draw(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_slider_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_draw.arg0");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_slider_draw.arg0");
        joy_ui_slider_draw(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_slider_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_slider_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_is_pressed.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_is_pressed.arg2");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_slider_is_pressed.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_slider_is_pressed(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_slider_set_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_slider_set_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_set_pressed.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_set_pressed.arg1");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_slider_set_pressed.arg0");
        p1 = (bool)a[1].as.b;
        joy_ui_slider_set_pressed(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_slider_drag_move(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_slider_drag_move");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_drag_move.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_drag_move.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_drag_move.arg2");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_slider_drag_move.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_ui_slider_drag_move(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_slider_is_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_slider_is_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_is_hovered.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_is_hovered.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_is_hovered.arg2");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_slider_is_hovered.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_slider_is_hovered(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_slider_set_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_slider_set_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_set_hovered.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_slider_set_hovered.arg1");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_slider_set_hovered.arg0");
        p1 = (bool)a[1].as.b;
        joy_ui_slider_set_hovered(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_popup_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_popup_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_destroy.arg0");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_popup_destroy.arg0");
        joy_ui_popup_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_popup_set_title(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_popup_set_title");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_set_title.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_set_title.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_set_title.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_popup_set_title.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        joy_ui_popup_set_title(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_popup_set_content(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_popup_set_content");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_set_content.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_set_content.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_set_content.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_popup_set_content.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        joy_ui_popup_set_content(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_popup_set_position(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_popup_set_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_set_position.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_set_position.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_set_position.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_popup_set_position.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_ui_popup_set_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_popup_set_visible(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_popup_set_visible");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_set_visible.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_set_visible.arg1");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_popup_set_visible.arg0");
        p1 = (bool)a[1].as.b;
        joy_ui_popup_set_visible(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_popup_set_closeable(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_popup_set_closeable");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_set_closeable.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_set_closeable.arg1");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_popup_set_closeable.arg0");
        p1 = (bool)a[1].as.b;
        joy_ui_popup_set_closeable(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_popup_draw(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        font_p p1;
        font_p p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_popup_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_draw.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_draw.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_popup_draw.arg0");
        p1 = (font_p)joy_obj_deref(a[1].as.handle, "font_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_ui_popup_draw.arg1");
        p2 = (font_p)joy_obj_deref(a[2].as.handle, "font_p", st);
        if (!p2) return joy_v_err(st, *st, "joy_ui_popup_draw.arg2");
        joy_ui_popup_draw(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_popup_is_close_clicked(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_popup_is_close_clicked");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_is_close_clicked.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_is_close_clicked.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_is_close_clicked.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_popup_is_close_clicked.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_popup_is_close_clicked(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_popup_is_title_clicked(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_popup_is_title_clicked");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_is_title_clicked.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_is_title_clicked.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_is_title_clicked.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_popup_is_title_clicked.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_popup_is_title_clicked(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_popup_drag_move(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_popup_drag_move");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_drag_move.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_drag_move.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_drag_move.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_popup_drag_move.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_ui_popup_drag_move(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_popup_is_inside(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_popup_is_inside");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_is_inside.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_is_inside.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_popup_is_inside.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_popup_is_inside.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = joy_ui_popup_is_inside(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_tooltip_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        tooltip_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_tooltip_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_tooltip_destroy.arg0");
        p0 = (tooltip_p)joy_obj_deref(a[0].as.handle, "tooltip_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_tooltip_destroy.arg0");
        joy_ui_tooltip_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_tooltip_set_text(joy_value_t* a, int n, joy_status_t* st)
{
        tooltip_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_tooltip_set_text");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_tooltip_set_text.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_tooltip_set_text.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_tooltip_set_text.arg2");
        p0 = (tooltip_p)joy_obj_deref(a[0].as.handle, "tooltip_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_tooltip_set_text.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        joy_ui_tooltip_set_text(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_tooltip_set_position(joy_value_t* a, int n, joy_status_t* st)
{
        tooltip_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_tooltip_set_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_tooltip_set_position.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_tooltip_set_position.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_tooltip_set_position.arg2");
        p0 = (tooltip_p)joy_obj_deref(a[0].as.handle, "tooltip_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_tooltip_set_position.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_ui_tooltip_set_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_tooltip_set_visible(joy_value_t* a, int n, joy_status_t* st)
{
        tooltip_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_tooltip_set_visible");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_tooltip_set_visible.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_tooltip_set_visible.arg1");
        p0 = (tooltip_p)joy_obj_deref(a[0].as.handle, "tooltip_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_tooltip_set_visible.arg0");
        p1 = (bool)a[1].as.b;
        joy_ui_tooltip_set_visible(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_tooltip_draw(joy_value_t* a, int n, joy_status_t* st)
{
        tooltip_p p0;
        font_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_tooltip_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_tooltip_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_tooltip_draw.arg1");
        p0 = (tooltip_p)joy_obj_deref(a[0].as.handle, "tooltip_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_tooltip_draw.arg0");
        p1 = (font_p)joy_obj_deref(a[1].as.handle, "font_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_ui_tooltip_draw.arg1");
        joy_ui_tooltip_draw(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_ctx_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_ctx_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_ctx_destroy.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_ctx_destroy.arg0");
        joy_ui_ctx_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_begin(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_begin");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_begin.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_begin.arg0");
        joy_ui_begin(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_end(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_end.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_end.arg0");
        joy_ui_end(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_input_motion(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_input_motion");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_input_motion.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_input_motion.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_input_motion.arg2");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_input_motion.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_ui_input_motion(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_input_down(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_input_down");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_input_down.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_input_down.arg1");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_input_down.arg0");
        p1 = (int)a[1].as.i32;
        joy_ui_input_down(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_input_up(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_input_up");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_input_up.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_input_up.arg1");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_input_up.arg0");
        p1 = (int)a[1].as.i32;
        joy_ui_input_up(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_input_wheel(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_input_wheel");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_input_wheel.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_input_wheel.arg1");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_input_wheel.arg0");
        p1 = (float)a[1].as.f64;
        joy_ui_input_wheel(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_input_key(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        bool p1;
        bool p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_input_key");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_input_key.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_input_key.arg1");
        if (a[2].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_input_key.arg2");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_input_key.arg0");
        p1 = (bool)a[1].as.b;
        p2 = (bool)a[2].as.b;
        joy_ui_input_key(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_is_down(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_is_down");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_is_down.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_is_down.arg0");
        _r = joy_ui_is_down(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_is_pressed.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_is_pressed.arg0");
        _r = joy_ui_is_pressed(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_is_released(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_is_released");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_is_released.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_is_released.arg0");
        _r = joy_ui_is_released(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_take_wheel(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_take_wheel");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_take_wheel.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_take_wheel.arg0");
        _r = joy_ui_take_wheel(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_joy_ui_is_shift(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_is_shift");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_is_shift.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_is_shift.arg0");
        _r = joy_ui_is_shift(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_is_ctrl(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_is_ctrl");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_is_ctrl.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_is_ctrl.arg0");
        _r = joy_ui_is_ctrl(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_push_clip(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        jrect_t p1 = { 0 };
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_push_clip");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_push_clip.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_push_clip.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_push_clip.arg1");
        p1.x = (float)a[1].as.f64;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_push_clip.arg2");
        p1.y = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_push_clip.arg3");
        p1.w = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_push_clip.arg4");
        p1.h = (float)a[4].as.f64;
        joy_ui_push_clip(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_pop_clip(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_pop_clip");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_pop_clip.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_pop_clip.arg0");
        joy_ui_pop_clip(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_reset_clip(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_reset_clip");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_reset_clip.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_reset_clip.arg0");
        joy_ui_reset_clip(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_hit(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        jrect_t p1 = { 0 };
        bool _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_hit");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_hit.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_hit.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_hit.arg1");
        p1.x = (float)a[1].as.f64;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_hit.arg2");
        p1.y = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_hit.arg3");
        p1.w = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_hit.arg4");
        p1.h = (float)a[4].as.f64;
        _r = joy_ui_hit(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_ui_fill_rect(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        jrect_t p1 = { 0 };
        jcolor_t p2 = { 0 };
        if (n != 9) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_fill_rect");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_fill_rect.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_fill_rect.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_fill_rect.arg1");
        p1.x = (float)a[1].as.f64;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_fill_rect.arg2");
        p1.y = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_fill_rect.arg3");
        p1.w = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_fill_rect.arg4");
        p1.h = (float)a[4].as.f64;
        if (a[5].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_fill_rect.arg5");
        p2.r = (uint8_t)a[5].as.u32;
        if (a[6].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_fill_rect.arg6");
        p2.g = (uint8_t)a[6].as.u32;
        if (a[7].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_fill_rect.arg7");
        p2.b = (uint8_t)a[7].as.u32;
        if (a[8].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_fill_rect.arg8");
        p2.a = (uint8_t)a[8].as.u32;
        joy_ui_fill_rect(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_stroke_rect(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        jrect_t p1 = { 0 };
        jcolor_t p2 = { 0 };
        if (n != 9) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_stroke_rect");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_stroke_rect.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_stroke_rect.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_stroke_rect.arg1");
        p1.x = (float)a[1].as.f64;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_stroke_rect.arg2");
        p1.y = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_stroke_rect.arg3");
        p1.w = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_stroke_rect.arg4");
        p1.h = (float)a[4].as.f64;
        if (a[5].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_stroke_rect.arg5");
        p2.r = (uint8_t)a[5].as.u32;
        if (a[6].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_stroke_rect.arg6");
        p2.g = (uint8_t)a[6].as.u32;
        if (a[7].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_stroke_rect.arg7");
        p2.b = (uint8_t)a[7].as.u32;
        if (a[8].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_stroke_rect.arg8");
        p2.a = (uint8_t)a[8].as.u32;
        joy_ui_stroke_rect(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_line(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        float p1;
        float p2;
        float p3;
        float p4;
        jcolor_t p5 = { 0 };
        if (n != 9) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_line");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_line.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_line.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_line.arg1");
        p1 = (float)a[1].as.f64;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_line.arg2");
        p2 = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_line.arg3");
        p3 = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_line.arg4");
        p4 = (float)a[4].as.f64;
        if (a[5].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_line.arg5");
        p5.r = (uint8_t)a[5].as.u32;
        if (a[6].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_line.arg6");
        p5.g = (uint8_t)a[6].as.u32;
        if (a[7].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_line.arg7");
        p5.b = (uint8_t)a[7].as.u32;
        if (a[8].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_line.arg8");
        p5.a = (uint8_t)a[8].as.u32;
        joy_ui_line(p0, p1, p2, p3, p4, p5);
        return joy_v_null();
}

static joy_value_t t_joy_ui_text(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        float p1;
        float p2;
        const char* p3;
        jcolor_t p4 = { 0 };
        int _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_text");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_text.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text.arg1");
        p1 = (float)a[1].as.f64;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text.arg2");
        p2 = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text.arg3");
        p3 = (const char*)a[3].as.str;
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text.arg4");
        p4.r = (uint8_t)a[4].as.u32;
        if (a[5].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text.arg5");
        p4.g = (uint8_t)a[5].as.u32;
        if (a[6].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text.arg6");
        p4.b = (uint8_t)a[6].as.u32;
        if (a[7].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text.arg7");
        p4.a = (uint8_t)a[7].as.u32;
        _r = joy_ui_text(p0, p1, p2, p3, p4);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_ui_text_centered(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        jrect_t p1 = { 0 };
        const char* p2;
        jcolor_t p3 = { 0 };
        int _r;
        if (n != 10) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_text_centered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_centered.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_text_centered.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_centered.arg1");
        p1.x = (float)a[1].as.f64;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_centered.arg2");
        p1.y = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_centered.arg3");
        p1.w = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_centered.arg4");
        p1.h = (float)a[4].as.f64;
        if (a[5].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_centered.arg5");
        p2 = (const char*)a[5].as.str;
        if (a[6].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_centered.arg6");
        p3.r = (uint8_t)a[6].as.u32;
        if (a[7].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_centered.arg7");
        p3.g = (uint8_t)a[7].as.u32;
        if (a[8].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_centered.arg8");
        p3.b = (uint8_t)a[8].as.u32;
        if (a[9].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_text_centered.arg9");
        p3.a = (uint8_t)a[9].as.u32;
        _r = joy_ui_text_centered(p0, p1, p2, p3);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_ui_panel_begin(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        jrect_t p1 = { 0 };
        const char* p2;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_panel_begin");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_panel_begin.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_panel_begin.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_panel_begin.arg1");
        p1.x = (float)a[1].as.f64;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_panel_begin.arg2");
        p1.y = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_panel_begin.arg3");
        p1.w = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_panel_begin.arg4");
        p1.h = (float)a[4].as.f64;
        if (a[5].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_panel_begin.arg5");
        p2 = (const char*)a[5].as.str;
        joy_ui_panel_begin(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_ui_panel_end(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_panel_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_panel_end.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_panel_end.arg0");
        joy_ui_panel_end(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_panel_frame(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        jrect_t p1 = { 0 };
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_panel_frame");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_panel_frame.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_panel_frame.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_panel_frame.arg1");
        p1.x = (float)a[1].as.f64;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_panel_frame.arg2");
        p1.y = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_panel_frame.arg3");
        p1.w = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_panel_frame.arg4");
        p1.h = (float)a[4].as.f64;
        joy_ui_panel_frame(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_ui_scroll_begin(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        int p1;
        jrect_t p2 = { 0 };
        float p3;
        float _r;
        if (n != 7) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scroll_begin");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scroll_begin.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scroll_begin.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scroll_begin.arg1");
        p1 = (int)a[1].as.i32;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scroll_begin.arg2");
        p2.x = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scroll_begin.arg3");
        p2.y = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scroll_begin.arg4");
        p2.w = (float)a[4].as.f64;
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scroll_begin.arg5");
        p2.h = (float)a[5].as.f64;
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scroll_begin.arg6");
        p3 = (float)a[6].as.f64;
        _r = joy_ui_scroll_begin(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_joy_ui_scroll_end(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scroll_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scroll_end.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scroll_end.arg0");
        joy_ui_scroll_end(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_scrollbar(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        jrect_t p1 = { 0 };
        float p2;
        float p3;
        if (n != 7) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scrollbar");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollbar.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scrollbar.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollbar.arg1");
        p1.x = (float)a[1].as.f64;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollbar.arg2");
        p1.y = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollbar.arg3");
        p1.w = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollbar.arg4");
        p1.h = (float)a[4].as.f64;
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollbar.arg5");
        p2 = (float)a[5].as.f64;
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scrollbar.arg6");
        p3 = (float)a[6].as.f64;
        joy_ui_scrollbar(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_ui_scroll_set_offset(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        int p1;
        float p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_scroll_set_offset");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scroll_set_offset.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scroll_set_offset.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scroll_set_offset.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scroll_set_offset.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_scroll_set_offset.arg4");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_scroll_set_offset.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        joy_ui_scroll_set_offset(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_joy_ui_button(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        int p1;
        jrect_t p2 = { 0 };
        const char* p3;
        bool _r;
        if (n != 7) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_button");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_button.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button.arg1");
        p1 = (int)a[1].as.i32;
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button.arg2");
        p2.x = (float)a[2].as.f64;
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button.arg3");
        p2.y = (float)a[3].as.f64;
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button.arg4");
        p2.w = (float)a[4].as.f64;
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button.arg5");
        p2.h = (float)a[5].as.f64;
        if (a[6].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_button.arg6");
        p3 = (const char*)a[6].as.str;
        _r = joy_ui_button(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_jtilemap_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jtilemap_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jtilemap_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_destroy.arg0");
        p0 = (jtilemap_p)joy_obj_deref(a[0].as.handle, "jtilemap_p", st);
        if (!p0) return joy_v_err(st, *st, "jtilemap_destroy.arg0");
        jtilemap_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_jtilemap_get_width(joy_value_t* a, int n, joy_status_t* st)
{
        jtilemap_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jtilemap_get_width");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_get_width.arg0");
        p0 = (jtilemap_p)joy_obj_deref(a[0].as.handle, "jtilemap_p", st);
        if (!p0) return joy_v_err(st, *st, "jtilemap_get_width.arg0");
        _r = jtilemap_get_width(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_jtilemap_get_height(joy_value_t* a, int n, joy_status_t* st)
{
        jtilemap_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jtilemap_get_height");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_get_height.arg0");
        p0 = (jtilemap_p)joy_obj_deref(a[0].as.handle, "jtilemap_p", st);
        if (!p0) return joy_v_err(st, *st, "jtilemap_get_height.arg0");
        _r = jtilemap_get_height(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_jtilemap_get_tilewidth(joy_value_t* a, int n, joy_status_t* st)
{
        jtilemap_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jtilemap_get_tilewidth");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_get_tilewidth.arg0");
        p0 = (jtilemap_p)joy_obj_deref(a[0].as.handle, "jtilemap_p", st);
        if (!p0) return joy_v_err(st, *st, "jtilemap_get_tilewidth.arg0");
        _r = jtilemap_get_tilewidth(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_jtilemap_get_tileheight(joy_value_t* a, int n, joy_status_t* st)
{
        jtilemap_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jtilemap_get_tileheight");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_get_tileheight.arg0");
        p0 = (jtilemap_p)joy_obj_deref(a[0].as.handle, "jtilemap_p", st);
        if (!p0) return joy_v_err(st, *st, "jtilemap_get_tileheight.arg0");
        _r = jtilemap_get_tileheight(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_jtilemap_get_anim_tileid(joy_value_t* a, int n, joy_status_t* st)
{
        jtilemap_p p0;
        int p1;
        uint32_t p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jtilemap_get_anim_tileid");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_get_anim_tileid.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_get_anim_tileid.arg1");
        if (a[2].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_get_anim_tileid.arg2");
        p0 = (jtilemap_p)joy_obj_deref(a[0].as.handle, "jtilemap_p", st);
        if (!p0) return joy_v_err(st, *st, "jtilemap_get_anim_tileid.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (uint32_t)a[2].as.u32;
        _r = jtilemap_get_anim_tileid(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_jtilemap_get_tile(joy_value_t* a, int n, joy_status_t* st)
{
        jtilemap_tile_layer_p p0;
        int p1;
        int p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jtilemap_get_tile");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_get_tile.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_get_tile.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_get_tile.arg2");
        p0 = (jtilemap_tile_layer_p)joy_obj_deref(a[0].as.handle, "jtilemap_tile_layer_p", st);
        if (!p0) return joy_v_err(st, *st, "jtilemap_get_tile.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        _r = jtilemap_get_tile(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_jtilemap_render_all(joy_value_t* a, int n, joy_status_t* st)
{
        jtilemap_render_p p0;
        jtilemap_p p1;
        float p2;
        float p3;
        uint32_t p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "jtilemap_render_all");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_all.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_all.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_all.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_all.arg3");
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_all.arg4");
        p0 = (jtilemap_render_p)joy_obj_deref(a[0].as.handle, "jtilemap_render_p", st);
        if (!p0) return joy_v_err(st, *st, "jtilemap_render_all.arg0");
        p1 = (jtilemap_p)joy_obj_deref(a[1].as.handle, "jtilemap_p", st);
        if (!p1) return joy_v_err(st, *st, "jtilemap_render_all.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (uint32_t)a[4].as.u32;
        jtilemap_render_all(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_jtilemap_render_tiles(joy_value_t* a, int n, joy_status_t* st)
{
        jtilemap_render_p p0;
        jtilemap_p p1;
        float p2;
        float p3;
        uint32_t p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "jtilemap_render_tiles");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_tiles.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_tiles.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_tiles.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_tiles.arg3");
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_tiles.arg4");
        p0 = (jtilemap_render_p)joy_obj_deref(a[0].as.handle, "jtilemap_render_p", st);
        if (!p0) return joy_v_err(st, *st, "jtilemap_render_tiles.arg0");
        p1 = (jtilemap_p)joy_obj_deref(a[1].as.handle, "jtilemap_p", st);
        if (!p1) return joy_v_err(st, *st, "jtilemap_render_tiles.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (uint32_t)a[4].as.u32;
        jtilemap_render_tiles(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_jtilemap_render_set_tile_px(joy_value_t* a, int n, joy_status_t* st)
{
        jtilemap_render_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jtilemap_render_set_tile_px");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_set_tile_px.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_set_tile_px.arg1");
        p0 = (jtilemap_render_p)joy_obj_deref(a[0].as.handle, "jtilemap_render_p", st);
        if (!p0) return joy_v_err(st, *st, "jtilemap_render_set_tile_px.arg0");
        p1 = (float)a[1].as.f64;
        jtilemap_render_set_tile_px(p0, p1);
        return joy_v_null();
}

static joy_value_t t_jtilemap_render_layer(joy_value_t* a, int n, joy_status_t* st)
{
        jtilemap_render_p p0;
        jtilemap_p p1;
        const char* p2;
        float p3;
        float p4;
        uint32_t p5;
        bool _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jtilemap_render_layer");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_layer.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_layer.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_layer.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_layer.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_layer.arg4");
        if (a[5].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_layer.arg5");
        p0 = (jtilemap_render_p)joy_obj_deref(a[0].as.handle, "jtilemap_render_p", st);
        if (!p0) return joy_v_err(st, *st, "jtilemap_render_layer.arg0");
        p1 = (jtilemap_p)joy_obj_deref(a[1].as.handle, "jtilemap_p", st);
        if (!p1) return joy_v_err(st, *st, "jtilemap_render_layer.arg1");
        p2 = (const char*)a[2].as.str;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (uint32_t)a[5].as.u32;
        _r = jtilemap_render_layer(p0, p1, p2, p3, p4, p5);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_jtilemap_render_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jtilemap_render_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jtilemap_render_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtilemap_render_destroy.arg0");
        p0 = (jtilemap_render_p)joy_obj_deref(a[0].as.handle, "jtilemap_render_p", st);
        if (!p0) return joy_v_err(st, *st, "jtilemap_render_destroy.arg0");
        jtilemap_render_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_kcpserver_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        kcpserver_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_kcpserver_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpserver_destroy.arg0");
        p0 = (kcpserver_p)joy_obj_deref(a[0].as.handle, "kcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_kcpserver_destroy.arg0");
        _r = joy_net_kcpserver_destroy(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_net_kcpserver_send(joy_value_t* a, int n, joy_status_t* st)
{
        kcpserver_p p0;
        int p1;
        const char* p2;
        int p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_kcpserver_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpserver_send.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpserver_send.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpserver_send.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpserver_send.arg3");
        p0 = (kcpserver_p)joy_obj_deref(a[0].as.handle, "kcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_kcpserver_send.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (const char*)a[2].as.str;
        p3 = (int)a[3].as.i32;
        joy_net_kcpserver_send(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_net_kcpserver_broadcast(joy_value_t* a, int n, joy_status_t* st)
{
        kcpserver_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_kcpserver_broadcast");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpserver_broadcast.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpserver_broadcast.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpserver_broadcast.arg2");
        p0 = (kcpserver_p)joy_obj_deref(a[0].as.handle, "kcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_kcpserver_broadcast.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        joy_net_kcpserver_broadcast(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_net_kcpserver_offline(joy_value_t* a, int n, joy_status_t* st)
{
        kcpserver_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_kcpserver_offline");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpserver_offline.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpserver_offline.arg1");
        p0 = (kcpserver_p)joy_obj_deref(a[0].as.handle, "kcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_kcpserver_offline.arg0");
        p1 = (int)a[1].as.i32;
        joy_net_kcpserver_offline(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_net_kcpserver_update(joy_value_t* a, int n, joy_status_t* st)
{
        kcpserver_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_kcpserver_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpserver_update.arg0");
        p0 = (kcpserver_p)joy_obj_deref(a[0].as.handle, "kcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_kcpserver_update.arg0");
        joy_net_kcpserver_update(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_kcpserver_connection_count(joy_value_t* a, int n, joy_status_t* st)
{
        kcpserver_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_kcpserver_connection_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpserver_connection_count.arg0");
        p0 = (kcpserver_p)joy_obj_deref(a[0].as.handle, "kcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_kcpserver_connection_count.arg0");
        _r = joy_net_kcpserver_connection_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_net_kcpserver_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        kcpserver_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_kcpserver_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpserver_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpserver_poll_message.arg1");
        p0 = (kcpserver_p)joy_obj_deref(a[0].as.handle, "kcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_kcpserver_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_net_kcpserver_poll_message.arg1");
        _r = joy_net_kcpserver_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_net_kcpclient_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        kcpclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_kcpclient_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpclient_destroy.arg0");
        p0 = (kcpclient_p)joy_obj_deref(a[0].as.handle, "kcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_kcpclient_destroy.arg0");
        joy_net_kcpclient_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_kcpclient_send(joy_value_t* a, int n, joy_status_t* st)
{
        kcpclient_p p0;
        const char* p1;
        int p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_kcpclient_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpclient_send.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpclient_send.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpclient_send.arg2");
        p0 = (kcpclient_p)joy_obj_deref(a[0].as.handle, "kcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_kcpclient_send.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        _r = joy_net_kcpclient_send(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_net_kcpclient_update(joy_value_t* a, int n, joy_status_t* st)
{
        kcpclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_kcpclient_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpclient_update.arg0");
        p0 = (kcpclient_p)joy_obj_deref(a[0].as.handle, "kcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_kcpclient_update.arg0");
        joy_net_kcpclient_update(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_kcpclient_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        kcpclient_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_kcpclient_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpclient_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_kcpclient_poll_message.arg1");
        p0 = (kcpclient_p)joy_obj_deref(a[0].as.handle, "kcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_kcpclient_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_net_kcpclient_poll_message.arg1");
        _r = joy_net_kcpclient_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_net_tcpserver_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        tcpserver_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_tcpserver_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpserver_destroy.arg0");
        p0 = (tcpserver_p)joy_obj_deref(a[0].as.handle, "tcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_tcpserver_destroy.arg0");
        _r = joy_net_tcpserver_destroy(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_net_tcpserver_send(joy_value_t* a, int n, joy_status_t* st)
{
        tcpserver_p p0;
        int p1;
        const char* p2;
        int p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_tcpserver_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpserver_send.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpserver_send.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpserver_send.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpserver_send.arg3");
        p0 = (tcpserver_p)joy_obj_deref(a[0].as.handle, "tcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_tcpserver_send.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (const char*)a[2].as.str;
        p3 = (int)a[3].as.i32;
        joy_net_tcpserver_send(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_net_tcpserver_broadcast(joy_value_t* a, int n, joy_status_t* st)
{
        tcpserver_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_tcpserver_broadcast");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpserver_broadcast.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpserver_broadcast.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpserver_broadcast.arg2");
        p0 = (tcpserver_p)joy_obj_deref(a[0].as.handle, "tcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_tcpserver_broadcast.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        joy_net_tcpserver_broadcast(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_net_tcpserver_offline(joy_value_t* a, int n, joy_status_t* st)
{
        tcpserver_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_tcpserver_offline");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpserver_offline.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpserver_offline.arg1");
        p0 = (tcpserver_p)joy_obj_deref(a[0].as.handle, "tcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_tcpserver_offline.arg0");
        p1 = (int)a[1].as.i32;
        joy_net_tcpserver_offline(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_net_tcpserver_update(joy_value_t* a, int n, joy_status_t* st)
{
        tcpserver_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_tcpserver_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpserver_update.arg0");
        p0 = (tcpserver_p)joy_obj_deref(a[0].as.handle, "tcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_tcpserver_update.arg0");
        joy_net_tcpserver_update(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_tcpserver_connection_count(joy_value_t* a, int n, joy_status_t* st)
{
        tcpserver_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_tcpserver_connection_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpserver_connection_count.arg0");
        p0 = (tcpserver_p)joy_obj_deref(a[0].as.handle, "tcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_tcpserver_connection_count.arg0");
        _r = joy_net_tcpserver_connection_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_net_tcpserver_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        tcpserver_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_tcpserver_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpserver_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpserver_poll_message.arg1");
        p0 = (tcpserver_p)joy_obj_deref(a[0].as.handle, "tcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_tcpserver_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_net_tcpserver_poll_message.arg1");
        _r = joy_net_tcpserver_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_net_tcpclient_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        tcpclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_tcpclient_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpclient_destroy.arg0");
        p0 = (tcpclient_p)joy_obj_deref(a[0].as.handle, "tcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_tcpclient_destroy.arg0");
        joy_net_tcpclient_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_tcpclient_send(joy_value_t* a, int n, joy_status_t* st)
{
        tcpclient_p p0;
        const char* p1;
        int p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_tcpclient_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpclient_send.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpclient_send.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpclient_send.arg2");
        p0 = (tcpclient_p)joy_obj_deref(a[0].as.handle, "tcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_tcpclient_send.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        _r = joy_net_tcpclient_send(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_net_tcpclient_update(joy_value_t* a, int n, joy_status_t* st)
{
        tcpclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_tcpclient_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpclient_update.arg0");
        p0 = (tcpclient_p)joy_obj_deref(a[0].as.handle, "tcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_tcpclient_update.arg0");
        joy_net_tcpclient_update(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_tcpclient_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        tcpclient_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_tcpclient_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpclient_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_tcpclient_poll_message.arg1");
        p0 = (tcpclient_p)joy_obj_deref(a[0].as.handle, "tcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_tcpclient_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_net_tcpclient_poll_message.arg1");
        _r = joy_net_tcpclient_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_net_wsnetserver_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetserver_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_wsnetserver_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetserver_destroy.arg0");
        p0 = (wsnetserver_p)joy_obj_deref(a[0].as.handle, "wsnetserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_wsnetserver_destroy.arg0");
        _r = joy_net_wsnetserver_destroy(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_net_wsnetserver_send(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetserver_p p0;
        int p1;
        const char* p2;
        int p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_wsnetserver_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetserver_send.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetserver_send.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetserver_send.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetserver_send.arg3");
        p0 = (wsnetserver_p)joy_obj_deref(a[0].as.handle, "wsnetserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_wsnetserver_send.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (const char*)a[2].as.str;
        p3 = (int)a[3].as.i32;
        joy_net_wsnetserver_send(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_net_wsnetserver_broadcast(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetserver_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_wsnetserver_broadcast");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetserver_broadcast.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetserver_broadcast.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetserver_broadcast.arg2");
        p0 = (wsnetserver_p)joy_obj_deref(a[0].as.handle, "wsnetserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_wsnetserver_broadcast.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        joy_net_wsnetserver_broadcast(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_net_wsnetserver_offline(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetserver_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_wsnetserver_offline");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetserver_offline.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetserver_offline.arg1");
        p0 = (wsnetserver_p)joy_obj_deref(a[0].as.handle, "wsnetserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_wsnetserver_offline.arg0");
        p1 = (int)a[1].as.i32;
        joy_net_wsnetserver_offline(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_net_wsnetserver_update(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetserver_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_wsnetserver_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetserver_update.arg0");
        p0 = (wsnetserver_p)joy_obj_deref(a[0].as.handle, "wsnetserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_wsnetserver_update.arg0");
        joy_net_wsnetserver_update(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_wsnetserver_connection_count(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetserver_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_wsnetserver_connection_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetserver_connection_count.arg0");
        p0 = (wsnetserver_p)joy_obj_deref(a[0].as.handle, "wsnetserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_wsnetserver_connection_count.arg0");
        _r = joy_net_wsnetserver_connection_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_net_wsnetserver_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetserver_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_wsnetserver_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetserver_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetserver_poll_message.arg1");
        p0 = (wsnetserver_p)joy_obj_deref(a[0].as.handle, "wsnetserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_wsnetserver_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_net_wsnetserver_poll_message.arg1");
        _r = joy_net_wsnetserver_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_net_wsnetclient_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_wsnetclient_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetclient_destroy.arg0");
        p0 = (wsnetclient_p)joy_obj_deref(a[0].as.handle, "wsnetclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_wsnetclient_destroy.arg0");
        joy_net_wsnetclient_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_wsnetclient_send(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetclient_p p0;
        const char* p1;
        int p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_wsnetclient_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetclient_send.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetclient_send.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetclient_send.arg2");
        p0 = (wsnetclient_p)joy_obj_deref(a[0].as.handle, "wsnetclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_wsnetclient_send.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        _r = joy_net_wsnetclient_send(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_net_wsnetclient_update(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_wsnetclient_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetclient_update.arg0");
        p0 = (wsnetclient_p)joy_obj_deref(a[0].as.handle, "wsnetclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_wsnetclient_update.arg0");
        joy_net_wsnetclient_update(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_wsnetclient_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetclient_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_wsnetclient_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetclient_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_wsnetclient_poll_message.arg1");
        p0 = (wsnetclient_p)joy_obj_deref(a[0].as.handle, "wsnetclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_wsnetclient_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_net_wsnetclient_poll_message.arg1");
        _r = joy_net_wsnetclient_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_net_netclient_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        netclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_netclient_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netclient_destroy.arg0");
        p0 = (netclient_p)joy_obj_deref(a[0].as.handle, "netclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_netclient_destroy.arg0");
        joy_net_netclient_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_netclient_send(joy_value_t* a, int n, joy_status_t* st)
{
        netclient_p p0;
        const char* p1;
        int p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_netclient_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netclient_send.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netclient_send.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netclient_send.arg2");
        p0 = (netclient_p)joy_obj_deref(a[0].as.handle, "netclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_netclient_send.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        _r = joy_net_netclient_send(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_net_netclient_update(joy_value_t* a, int n, joy_status_t* st)
{
        netclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_netclient_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netclient_update.arg0");
        p0 = (netclient_p)joy_obj_deref(a[0].as.handle, "netclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_netclient_update.arg0");
        joy_net_netclient_update(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_netclient_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        netclient_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_netclient_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netclient_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netclient_poll_message.arg1");
        p0 = (netclient_p)joy_obj_deref(a[0].as.handle, "netclient_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_netclient_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_net_netclient_poll_message.arg1");
        _r = joy_net_netclient_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_net_netserver_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        netserver_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_netserver_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netserver_destroy.arg0");
        p0 = (netserver_p)joy_obj_deref(a[0].as.handle, "netserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_netserver_destroy.arg0");
        joy_net_netserver_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_netserver_send(joy_value_t* a, int n, joy_status_t* st)
{
        netserver_p p0;
        int p1;
        const char* p2;
        int p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_netserver_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netserver_send.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netserver_send.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netserver_send.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netserver_send.arg3");
        p0 = (netserver_p)joy_obj_deref(a[0].as.handle, "netserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_netserver_send.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (const char*)a[2].as.str;
        p3 = (int)a[3].as.i32;
        joy_net_netserver_send(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_net_netserver_broadcast(joy_value_t* a, int n, joy_status_t* st)
{
        netserver_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_netserver_broadcast");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netserver_broadcast.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netserver_broadcast.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netserver_broadcast.arg2");
        p0 = (netserver_p)joy_obj_deref(a[0].as.handle, "netserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_netserver_broadcast.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        joy_net_netserver_broadcast(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_net_netserver_offline(joy_value_t* a, int n, joy_status_t* st)
{
        netserver_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_netserver_offline");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netserver_offline.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netserver_offline.arg1");
        p0 = (netserver_p)joy_obj_deref(a[0].as.handle, "netserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_netserver_offline.arg0");
        p1 = (int)a[1].as.i32;
        joy_net_netserver_offline(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_net_netserver_update(joy_value_t* a, int n, joy_status_t* st)
{
        netserver_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_netserver_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netserver_update.arg0");
        p0 = (netserver_p)joy_obj_deref(a[0].as.handle, "netserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_netserver_update.arg0");
        joy_net_netserver_update(p0);
        return joy_v_null();
}

static joy_value_t t_joy_net_netserver_connection_count(joy_value_t* a, int n, joy_status_t* st)
{
        netserver_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_netserver_connection_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netserver_connection_count.arg0");
        p0 = (netserver_p)joy_obj_deref(a[0].as.handle, "netserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_netserver_connection_count.arg0");
        _r = joy_net_netserver_connection_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_net_netserver_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        netserver_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_netserver_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netserver_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netserver_poll_message.arg1");
        p0 = (netserver_p)joy_obj_deref(a[0].as.handle, "netserver_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_netserver_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_net_netserver_poll_message.arg1");
        _r = joy_net_netserver_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_net_netmessage_free_data(joy_value_t* a, int n, joy_status_t* st)
{
        net_message_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_net_netmessage_free_data");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_net_netmessage_free_data.arg0");
        p0 = (net_message_p)joy_obj_deref(a[0].as.handle, "net_message_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_net_netmessage_free_data.arg0");
        joy_net_netmessage_free_data(p0);
        return joy_v_null();
}

static joy_value_t t_scene_node_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        scene_node_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scene_node_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_destroy.arg0");
        p0 = (scene_node_p)joy_obj_deref(a[0].as.handle, "scene_node_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_node_destroy.arg0");
        scene_node_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_scene_node_add_child(joy_value_t* a, int n, joy_status_t* st)
{
        scene_node_p p0;
        scene_node_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scene_node_add_child");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_add_child.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_add_child.arg1");
        p0 = (scene_node_p)joy_obj_deref(a[0].as.handle, "scene_node_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_node_add_child.arg0");
        p1 = (scene_node_p)joy_obj_deref(a[1].as.handle, "scene_node_p", st);
        if (!p1) return joy_v_err(st, *st, "scene_node_add_child.arg1");
        scene_node_add_child(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scene_node_remove_child(joy_value_t* a, int n, joy_status_t* st)
{
        scene_node_p p0;
        scene_node_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scene_node_remove_child");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_remove_child.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_remove_child.arg1");
        p0 = (scene_node_p)joy_obj_deref(a[0].as.handle, "scene_node_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_node_remove_child.arg0");
        p1 = (scene_node_p)joy_obj_deref(a[1].as.handle, "scene_node_p", st);
        if (!p1) return joy_v_err(st, *st, "scene_node_remove_child.arg1");
        scene_node_remove_child(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scene_node_get_child_count(joy_value_t* a, int n, joy_status_t* st)
{
        scene_node_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scene_node_get_child_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_get_child_count.arg0");
        p0 = (scene_node_p)joy_obj_deref(a[0].as.handle, "scene_node_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_node_get_child_count.arg0");
        _r = scene_node_get_child_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_scene_node_set_position(joy_value_t* a, int n, joy_status_t* st)
{
        scene_node_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "scene_node_set_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_position.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_position.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_position.arg2");
        p0 = (scene_node_p)joy_obj_deref(a[0].as.handle, "scene_node_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_node_set_position.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        scene_node_set_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_scene_node_set_world_position(joy_value_t* a, int n, joy_status_t* st)
{
        scene_node_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "scene_node_set_world_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_world_position.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_world_position.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_world_position.arg2");
        p0 = (scene_node_p)joy_obj_deref(a[0].as.handle, "scene_node_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_node_set_world_position.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        scene_node_set_world_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_scene_node_set_rotation(joy_value_t* a, int n, joy_status_t* st)
{
        scene_node_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scene_node_set_rotation");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_rotation.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_rotation.arg1");
        p0 = (scene_node_p)joy_obj_deref(a[0].as.handle, "scene_node_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_node_set_rotation.arg0");
        p1 = (float)a[1].as.f64;
        scene_node_set_rotation(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scene_node_get_rotation(joy_value_t* a, int n, joy_status_t* st)
{
        scene_node_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scene_node_get_rotation");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_get_rotation.arg0");
        p0 = (scene_node_p)joy_obj_deref(a[0].as.handle, "scene_node_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_node_get_rotation.arg0");
        _r = scene_node_get_rotation(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_scene_node_set_scale(joy_value_t* a, int n, joy_status_t* st)
{
        scene_node_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "scene_node_set_scale");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_scale.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_scale.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_scale.arg2");
        p0 = (scene_node_p)joy_obj_deref(a[0].as.handle, "scene_node_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_node_set_scale.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        scene_node_set_scale(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_scene_node_set_zorder(joy_value_t* a, int n, joy_status_t* st)
{
        scene_node_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scene_node_set_zorder");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_zorder.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_zorder.arg1");
        p0 = (scene_node_p)joy_obj_deref(a[0].as.handle, "scene_node_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_node_set_zorder.arg0");
        p1 = (int)a[1].as.i32;
        scene_node_set_zorder(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scene_node_get_zorder(joy_value_t* a, int n, joy_status_t* st)
{
        scene_node_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scene_node_get_zorder");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_get_zorder.arg0");
        p0 = (scene_node_p)joy_obj_deref(a[0].as.handle, "scene_node_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_node_get_zorder.arg0");
        _r = scene_node_get_zorder(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_scene_node_set_name(joy_value_t* a, int n, joy_status_t* st)
{
        scene_node_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scene_node_set_name");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_name.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_name.arg1");
        p0 = (scene_node_p)joy_obj_deref(a[0].as.handle, "scene_node_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_node_set_name.arg0");
        p1 = (const char*)a[1].as.str;
        scene_node_set_name(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scene_node_set_tag(joy_value_t* a, int n, joy_status_t* st)
{
        scene_node_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scene_node_set_tag");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_tag.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_set_tag.arg1");
        p0 = (scene_node_p)joy_obj_deref(a[0].as.handle, "scene_node_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_node_set_tag.arg0");
        p1 = (const char*)a[1].as.str;
        scene_node_set_tag(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scene_node_update(joy_value_t* a, int n, joy_status_t* st)
{
        scene_node_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scene_node_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_update.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scene_node_update.arg1");
        p0 = (scene_node_p)joy_obj_deref(a[0].as.handle, "scene_node_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_node_update.arg0");
        p1 = (float)a[1].as.f64;
        scene_node_update(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scene_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        scene_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scene_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_destroy.arg0");
        p0 = (scene_p)joy_obj_deref(a[0].as.handle, "scene_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_destroy.arg0");
        scene_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_scene_add_root_node(joy_value_t* a, int n, joy_status_t* st)
{
        scene_p p0;
        scene_node_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scene_add_root_node");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_add_root_node.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_add_root_node.arg1");
        p0 = (scene_p)joy_obj_deref(a[0].as.handle, "scene_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_add_root_node.arg0");
        p1 = (scene_node_p)joy_obj_deref(a[1].as.handle, "scene_node_p", st);
        if (!p1) return joy_v_err(st, *st, "scene_add_root_node.arg1");
        scene_add_root_node(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scene_remove_root_node(joy_value_t* a, int n, joy_status_t* st)
{
        scene_p p0;
        scene_node_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scene_remove_root_node");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_remove_root_node.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_remove_root_node.arg1");
        p0 = (scene_p)joy_obj_deref(a[0].as.handle, "scene_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_remove_root_node.arg0");
        p1 = (scene_node_p)joy_obj_deref(a[1].as.handle, "scene_node_p", st);
        if (!p1) return joy_v_err(st, *st, "scene_remove_root_node.arg1");
        scene_remove_root_node(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scene_get_root_count(joy_value_t* a, int n, joy_status_t* st)
{
        scene_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scene_get_root_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_get_root_count.arg0");
        p0 = (scene_p)joy_obj_deref(a[0].as.handle, "scene_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_get_root_count.arg0");
        _r = scene_get_root_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_scene_update(joy_value_t* a, int n, joy_status_t* st)
{
        scene_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scene_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_update.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scene_update.arg1");
        p0 = (scene_p)joy_obj_deref(a[0].as.handle, "scene_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_update.arg0");
        p1 = (float)a[1].as.f64;
        scene_update(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scene_set_popup_node(joy_value_t* a, int n, joy_status_t* st)
{
        scene_p p0;
        scene_node_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scene_set_popup_node");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_set_popup_node.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_set_popup_node.arg1");
        p0 = (scene_p)joy_obj_deref(a[0].as.handle, "scene_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_set_popup_node.arg0");
        p1 = (scene_node_p)joy_obj_deref(a[1].as.handle, "scene_node_p", st);
        if (!p1) return joy_v_err(st, *st, "scene_set_popup_node.arg1");
        scene_set_popup_node(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scene_manager_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        scene_manager_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scene_manager_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_manager_destroy.arg0");
        p0 = (scene_manager_p)joy_obj_deref(a[0].as.handle, "scene_manager_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_manager_destroy.arg0");
        scene_manager_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_scene_manager_push(joy_value_t* a, int n, joy_status_t* st)
{
        scene_manager_p p0;
        scene_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scene_manager_push");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_manager_push.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_manager_push.arg1");
        p0 = (scene_manager_p)joy_obj_deref(a[0].as.handle, "scene_manager_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_manager_push.arg0");
        p1 = (scene_p)joy_obj_deref(a[1].as.handle, "scene_p", st);
        if (!p1) return joy_v_err(st, *st, "scene_manager_push.arg1");
        scene_manager_push(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scene_manager_replace(joy_value_t* a, int n, joy_status_t* st)
{
        scene_manager_p p0;
        scene_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scene_manager_replace");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_manager_replace.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_manager_replace.arg1");
        p0 = (scene_manager_p)joy_obj_deref(a[0].as.handle, "scene_manager_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_manager_replace.arg0");
        p1 = (scene_p)joy_obj_deref(a[1].as.handle, "scene_p", st);
        if (!p1) return joy_v_err(st, *st, "scene_manager_replace.arg1");
        scene_manager_replace(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scene_manager_pop(joy_value_t* a, int n, joy_status_t* st)
{
        scene_manager_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scene_manager_pop");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_manager_pop.arg0");
        p0 = (scene_manager_p)joy_obj_deref(a[0].as.handle, "scene_manager_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_manager_pop.arg0");
        scene_manager_pop(p0);
        return joy_v_null();
}

static joy_value_t t_scene_manager_update(joy_value_t* a, int n, joy_status_t* st)
{
        scene_manager_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scene_manager_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_manager_update.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scene_manager_update.arg1");
        p0 = (scene_manager_p)joy_obj_deref(a[0].as.handle, "scene_manager_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_manager_update.arg0");
        p1 = (float)a[1].as.f64;
        scene_manager_update(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scene_manager_get_count(joy_value_t* a, int n, joy_status_t* st)
{
        scene_manager_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scene_manager_get_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scene_manager_get_count.arg0");
        p0 = (scene_manager_p)joy_obj_deref(a[0].as.handle, "scene_manager_p", st);
        if (!p0) return joy_v_err(st, *st, "scene_manager_get_count.arg0");
        _r = scene_manager_get_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_r2d_particle_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_particle_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_particle_destroy.arg0");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_particle_destroy.arg0");
        joy_r2d_particle_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_particle_set_position(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_particle_set_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_particle_set_position.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_particle_set_position.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_particle_set_position.arg2");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_particle_set_position.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joy_r2d_particle_set_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_particle_emit(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_particle_emit");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_particle_emit.arg0");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_particle_emit.arg0");
        joy_r2d_particle_emit(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_particle_clear(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_particle_clear");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_particle_clear.arg0");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_particle_clear.arg0");
        joy_r2d_particle_clear(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_particle_update(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_particle_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_particle_update.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_particle_update.arg1");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_particle_update.arg0");
        p1 = (float)a[1].as.f64;
        joy_r2d_particle_update(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_particle_render(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        jr2d_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_particle_render");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_particle_render.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_particle_render.arg1");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_particle_render.arg0");
        p1 = (jr2d_p)joy_obj_deref(a[1].as.handle, "jr2d_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r2d_particle_render.arg1");
        joy_r2d_particle_render(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r2d_particle_count(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_particle_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_particle_count.arg0");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_particle_count.arg0");
        _r = joy_r2d_particle_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_r2d_particle_is_active(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r2d_particle_is_active");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r2d_particle_is_active.arg0");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r2d_particle_is_active.arg0");
        _r = joy_r2d_particle_is_active(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joy_r3d_light_sys_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_light_sys_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_sys_destroy.arg0");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_light_sys_destroy.arg0");
        joy_r3d_light_sys_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_light_sys_resize(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        int p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_light_sys_resize");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_sys_resize.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_sys_resize.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_sys_resize.arg2");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_light_sys_resize.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        joy_r3d_light_sys_resize(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_light_remove(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_light_remove");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_remove.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_remove.arg1");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_light_remove.arg0");
        p1 = (int)a[1].as.i32;
        joy_r3d_light_remove(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_light_clear(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_light_clear");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_clear.arg0");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_light_clear.arg0");
        joy_r3d_light_clear(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_light_add_occluder(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        float p1;
        float p2;
        float p3;
        float p4;
        int _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_light_add_occluder");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_add_occluder.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_add_occluder.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_add_occluder.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_add_occluder.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_add_occluder.arg4");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_light_add_occluder.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        _r = joy_r3d_light_add_occluder(p0, p1, p2, p3, p4);
        return joy_v_i32((int)_r);
}

static joy_value_t t_joy_r3d_light_clear_occluders(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_light_clear_occluders");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_clear_occluders.arg0");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_light_clear_occluders.arg0");
        joy_r3d_light_clear_occluders(p0);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_light_set_ambient(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        uint8_t p1;
        uint8_t p2;
        uint8_t p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_light_set_ambient");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_set_ambient.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_set_ambient.arg1");
        if (a[2].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_set_ambient.arg2");
        if (a[3].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_set_ambient.arg3");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_light_set_ambient.arg0");
        p1 = (uint8_t)a[1].as.u32;
        p2 = (uint8_t)a[2].as.u32;
        p3 = (uint8_t)a[3].as.u32;
        joy_r3d_light_set_ambient(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_joy_r3d_light_render(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        jr2d_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joy_r3d_light_render");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_render.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_r3d_light_render.arg1");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_r3d_light_render.arg0");
        p1 = (jr2d_p)joy_obj_deref(a[1].as.handle, "jr2d_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_r3d_light_render.arg1");
        joy_r3d_light_render(p0, p1);
        return joy_v_null();
}

static joy_value_t t_jecs_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jecs_world_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jecs_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jecs_destroy.arg0");
        p0 = (jecs_world_p)joy_obj_deref(a[0].as.handle, "jecs_world_p", st);
        if (!p0) return joy_v_err(st, *st, "jecs_destroy.arg0");
        jecs_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_jecs_process(joy_value_t* a, int n, joy_status_t* st)
{
        jecs_world_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jecs_process");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jecs_process.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jecs_process.arg1");
        p0 = (jecs_world_p)joy_obj_deref(a[0].as.handle, "jecs_world_p", st);
        if (!p0) return joy_v_err(st, *st, "jecs_process.arg0");
        p1 = (float)a[1].as.f64;
        jecs_process(p0, p1);
        return joy_v_null();
}

static joy_value_t t_jecs_define(joy_value_t* a, int n, joy_status_t* st)
{
        jecs_world_p p0;
        const char* p1;
        size_t p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jecs_define");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jecs_define.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "jecs_define.arg1");
        if (a[2].kind != JOY_V_U64) return joy_v_err(st, JOY_ERR_TYPE, "jecs_define.arg2");
        p0 = (jecs_world_p)joy_obj_deref(a[0].as.handle, "jecs_world_p", st);
        if (!p0) return joy_v_err(st, *st, "jecs_define.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (size_t)a[2].as.u64;
        jecs_define(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_audio_open_device(joy_value_t* a, int n, joy_status_t* st)
{
        uint32_t _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "audio_open_device");
        _r = audio_open_device();
        return joy_v_u32((uint32_t)_r);
}

static joy_value_t t_audio_close_device(joy_value_t* a, int n, joy_status_t* st)
{
        uint32_t p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "audio_close_device");
        if (a[0].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "audio_close_device.arg0");
        p0 = (uint32_t)a[0].as.u32;
        audio_close_device(p0);
        return joy_v_null();
}

static joy_value_t t_audio_create_wav(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        uint32_t p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "audio_create_wav");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "audio_create_wav.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "audio_create_wav.arg1");
        p0 = (const char*)a[0].as.str;
        p1 = (uint32_t)a[1].as.u32;
        _r = audio_create_wav(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_audio_create_mp3(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        uint32_t p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "audio_create_mp3");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "audio_create_mp3.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "audio_create_mp3.arg1");
        p0 = (const char*)a[0].as.str;
        p1 = (uint32_t)a[1].as.u32;
        _r = audio_create_mp3(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_audio_create_flac(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        uint32_t p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "audio_create_flac");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "audio_create_flac.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "audio_create_flac.arg1");
        p0 = (const char*)a[0].as.str;
        p1 = (uint32_t)a[1].as.u32;
        _r = audio_create_flac(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_audio_create_ogg(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        uint32_t p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "audio_create_ogg");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "audio_create_ogg.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "audio_create_ogg.arg1");
        p0 = (const char*)a[0].as.str;
        p1 = (uint32_t)a[1].as.u32;
        _r = audio_create_ogg(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_audio_shot_play(joy_value_t* a, int n, joy_status_t* st)
{
        audio_shot_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "audio_shot_play");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "audio_shot_play.arg0");
        p0 = (audio_shot_p)joy_obj_deref(a[0].as.handle, "audio_shot_p", st);
        if (!p0) return joy_v_err(st, *st, "audio_shot_play.arg0");
        audio_shot_play(p0);
        return joy_v_null();
}

static joy_value_t t_audio_shot_set_gain(joy_value_t* a, int n, joy_status_t* st)
{
        audio_shot_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "audio_shot_set_gain");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "audio_shot_set_gain.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "audio_shot_set_gain.arg1");
        p0 = (audio_shot_p)joy_obj_deref(a[0].as.handle, "audio_shot_p", st);
        if (!p0) return joy_v_err(st, *st, "audio_shot_set_gain.arg0");
        p1 = (float)a[1].as.f64;
        audio_shot_set_gain(p0, p1);
        return joy_v_null();
}

static joy_value_t t_audio_shot_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        audio_shot_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "audio_shot_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "audio_shot_destroy.arg0");
        p0 = (audio_shot_p)joy_obj_deref(a[0].as.handle, "audio_shot_p", st);
        if (!p0) return joy_v_err(st, *st, "audio_shot_destroy.arg0");
        audio_shot_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_audio_bgm_stop(joy_value_t* a, int n, joy_status_t* st)
{
        audio_bgm_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "audio_bgm_stop");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "audio_bgm_stop.arg0");
        p0 = (audio_bgm_p)joy_obj_deref(a[0].as.handle, "audio_bgm_p", st);
        if (!p0) return joy_v_err(st, *st, "audio_bgm_stop.arg0");
        audio_bgm_stop(p0);
        return joy_v_null();
}

static joy_value_t t_audio_bgm_set_gain(joy_value_t* a, int n, joy_status_t* st)
{
        audio_bgm_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "audio_bgm_set_gain");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "audio_bgm_set_gain.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "audio_bgm_set_gain.arg1");
        p0 = (audio_bgm_p)joy_obj_deref(a[0].as.handle, "audio_bgm_p", st);
        if (!p0) return joy_v_err(st, *st, "audio_bgm_set_gain.arg0");
        p1 = (float)a[1].as.f64;
        audio_bgm_set_gain(p0, p1);
        return joy_v_null();
}

static joy_value_t t_audio_bgm_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        audio_bgm_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "audio_bgm_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "audio_bgm_destroy.arg0");
        p0 = (audio_bgm_p)joy_obj_deref(a[0].as.handle, "audio_bgm_p", st);
        if (!p0) return joy_v_err(st, *st, "audio_bgm_destroy.arg0");
        audio_bgm_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joy_ui_image_draw_rect(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        image_p p1;
        float p2;
        float p3;
        float p4;
        float p5;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "joy_ui_image_draw_rect");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_draw_rect.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_draw_rect.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_draw_rect.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_draw_rect.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_draw_rect.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joy_ui_image_draw_rect.arg5");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "joy_ui_image_draw_rect.arg0");
        p1 = (image_p)joy_obj_deref(a[1].as.handle, "image_p", st);
        if (!p1) return joy_v_err(st, *st, "joy_ui_image_draw_rect.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        joy_ui_image_draw_rect(p0, p1, p2, p3, p4, p5);
        return joy_v_null();
}

static joy_value_t t_jmath_fp_from_float(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        int64_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_from_float");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_from_float.arg0");
        p0 = (float)a[0].as.f64;
        _r = jmath_fp_from_float(p0);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_to_float(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_to_float");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_to_float.arg0");
        p0 = (int64_t)a[0].as.i64;
        _r = jmath_fp_to_float(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_fp_from_uint(joy_value_t* a, int n, joy_status_t* st)
{
        uint32_t p0;
        int64_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_from_uint");
        if (a[0].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_from_uint.arg0");
        p0 = (uint32_t)a[0].as.u32;
        _r = jmath_fp_from_uint(p0);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_from_int(joy_value_t* a, int n, joy_status_t* st)
{
        int32_t p0;
        int64_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_from_int");
        if (a[0].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_from_int.arg0");
        p0 = (int32_t)a[0].as.i32;
        _r = jmath_fp_from_int(p0);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_from_fraction(joy_value_t* a, int n, joy_status_t* st)
{
        uint32_t p0;
        int64_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_from_fraction");
        if (a[0].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_from_fraction.arg0");
        p0 = (uint32_t)a[0].as.u32;
        _r = jmath_fp_from_fraction(p0);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_min_value(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_min_value");
        _r = jmath_fp_min_value();
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_max_value(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_max_value");
        _r = jmath_fp_max_value();
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_add(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_add");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_add.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_add.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        _r = jmath_fp_add(p0, p1);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_sub(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_sub");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_sub.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_sub.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        _r = jmath_fp_sub(p0, p1);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_zero(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_zero");
        _r = jmath_fp_zero();
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_half(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_half");
        _r = jmath_fp_half();
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_one(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_one");
        _r = jmath_fp_one();
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_pi(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_pi");
        _r = jmath_fp_pi();
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_max(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_max");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_max.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_max.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        _r = jmath_fp_max(p0, p1);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_min(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_min");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_min.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_min.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        _r = jmath_fp_min(p0, p1);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_pow2(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_pow2");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_pow2.arg0");
        p0 = (int64_t)a[0].as.i64;
        _r = jmath_fp_pow2(p0);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_sign(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int32_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_sign");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_sign.arg0");
        p0 = (int64_t)a[0].as.i64;
        _r = jmath_fp_sign(p0);
        return joy_v_i32((int32_t)_r);
}

static joy_value_t t_jmath_fp_clamp(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_clamp");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_clamp.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_clamp.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_clamp.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_fp_clamp(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_fp_lerp(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_fp_lerp");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_lerp.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_lerp.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_fp_lerp.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_fp_lerp(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_negate_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_negate_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_negate_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_negate_x.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        _r = jmath_vec2f_negate_x(p0, p1);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_negate_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_negate_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_negate_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_negate_y.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        _r = jmath_vec2f_negate_y(p0, p1);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_dot(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_dot");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_dot.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_dot.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_dot.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_dot.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_vec2f_dot(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_cross(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_cross");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_vec2f_cross(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_cross2_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_cross2_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross2_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross2_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross2_x.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec2f_cross2_x(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_cross2_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_cross2_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross2_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross2_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross2_y.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec2f_cross2_y(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_cross3_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_cross3_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross3_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross3_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross3_x.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec2f_cross3_x(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_cross3_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_cross3_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross3_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross3_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_cross3_y.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec2f_cross3_y(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_add_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_add_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_add_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_add_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_add_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_add_x.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_vec2f_add_x(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_add_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_add_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_add_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_add_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_add_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_add_y.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_vec2f_add_y(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_sub_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_sub_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_sub_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_sub_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_sub_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_sub_x.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_vec2f_sub_x(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_sub_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_sub_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_sub_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_sub_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_sub_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_sub_y.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_vec2f_sub_y(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_scale_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_scale_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_scale_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_scale_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_scale_x.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec2f_scale_x(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_scale_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_scale_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_scale_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_scale_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_scale_y.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec2f_scale_y(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_abs_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_abs_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_abs_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_abs_x.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        _r = jmath_vec2f_abs_x(p0, p1);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_abs_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_abs_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_abs_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_abs_y.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        _r = jmath_vec2f_abs_y(p0, p1);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_length_squared(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_length_squared");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_length_squared.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_length_squared.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        _r = jmath_vec2f_length_squared(p0, p1);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_length(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_length");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_length.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_length.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        _r = jmath_vec2f_length(p0, p1);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_normalize_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_normalize_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_normalize_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_normalize_x.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        _r = jmath_vec2f_normalize_x(p0, p1);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_normalize_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_normalize_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_normalize_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_normalize_y.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        _r = jmath_vec2f_normalize_y(p0, p1);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_normal_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_normal_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_normal_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_normal_x.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        _r = jmath_vec2f_normal_x(p0, p1);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_normal_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_normal_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_normal_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_normal_y.arg1");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        _r = jmath_vec2f_normal_y(p0, p1);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_rotate_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_rotate_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_rotate_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_rotate_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_rotate_x.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec2f_rotate_x(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_rotate_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_rotate_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_rotate_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_rotate_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_rotate_y.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec2f_rotate_y(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_lerp_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_lerp_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_lerp_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_lerp_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_lerp_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_lerp_x.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_lerp_x.arg4");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        _r = jmath_vec2f_lerp_x(p0, p1, p2, p3, p4);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_lerp_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_lerp_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_lerp_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_lerp_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_lerp_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_lerp_y.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_lerp_y.arg4");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        _r = jmath_vec2f_lerp_y(p0, p1, p2, p3, p4);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec2f_distance(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2f_distance");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_distance.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_distance.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_distance.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2f_distance.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_vec2f_distance(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_negate_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_negate_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_negate_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_negate_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_negate_x.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec3f_negate_x(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_negate_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_negate_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_negate_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_negate_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_negate_y.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec3f_negate_y(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_negate_z(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_negate_z");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_negate_z.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_negate_z.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_negate_z.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec3f_negate_z(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_add_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_add_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_x.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_x.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_x.arg5");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        _r = jmath_vec3f_add_x(p0, p1, p2, p3, p4, p5);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_add_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_add_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_y.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_y.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_y.arg5");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        _r = jmath_vec3f_add_y(p0, p1, p2, p3, p4, p5);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_add_z(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_add_z");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_z.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_z.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_z.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_z.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_z.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_add_z.arg5");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        _r = jmath_vec3f_add_z(p0, p1, p2, p3, p4, p5);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_sub_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_sub_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_x.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_x.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_x.arg5");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        _r = jmath_vec3f_sub_x(p0, p1, p2, p3, p4, p5);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_sub_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_sub_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_y.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_y.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_y.arg5");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        _r = jmath_vec3f_sub_y(p0, p1, p2, p3, p4, p5);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_sub_z(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_sub_z");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_z.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_z.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_z.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_z.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_z.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_sub_z.arg5");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        _r = jmath_vec3f_sub_z(p0, p1, p2, p3, p4, p5);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_scale_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_scale_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_scale_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_scale_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_scale_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_scale_x.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_vec3f_scale_x(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_scale_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_scale_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_scale_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_scale_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_scale_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_scale_y.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_vec3f_scale_y(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_scale_z(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_scale_z");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_scale_z.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_scale_z.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_scale_z.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_scale_z.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_vec3f_scale_z(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_abs_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_abs_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_abs_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_abs_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_abs_x.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec3f_abs_x(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_abs_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_abs_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_abs_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_abs_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_abs_y.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec3f_abs_y(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_abs_z(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_abs_z");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_abs_z.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_abs_z.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_abs_z.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec3f_abs_z(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_dot(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_dot");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_dot.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_dot.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_dot.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_dot.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_dot.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_dot.arg5");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        _r = jmath_vec3f_dot(p0, p1, p2, p3, p4, p5);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_cross_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_cross_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_x.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_x.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_x.arg5");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        _r = jmath_vec3f_cross_x(p0, p1, p2, p3, p4, p5);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_cross_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_cross_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_y.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_y.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_y.arg5");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        _r = jmath_vec3f_cross_y(p0, p1, p2, p3, p4, p5);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_cross_z(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_cross_z");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_z.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_z.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_z.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_z.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_z.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_cross_z.arg5");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        _r = jmath_vec3f_cross_z(p0, p1, p2, p3, p4, p5);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_square_length(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_square_length");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_square_length.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_square_length.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_square_length.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec3f_square_length(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_length(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_length");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_length.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_length.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_length.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec3f_length(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_normalize_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_normalize_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_normalize_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_normalize_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_normalize_x.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec3f_normalize_x(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_normalize_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_normalize_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_normalize_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_normalize_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_normalize_y.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec3f_normalize_y(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_normalize_z(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_normalize_z");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_normalize_z.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_normalize_z.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_normalize_z.arg2");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        _r = jmath_vec3f_normalize_z(p0, p1, p2);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_lerp_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t p6;
        int64_t _r;
        if (n != 7) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_lerp_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_x.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_x.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_x.arg5");
        if (a[6].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_x.arg6");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        p6 = (int64_t)a[6].as.i64;
        _r = jmath_vec3f_lerp_x(p0, p1, p2, p3, p4, p5, p6);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_lerp_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t p6;
        int64_t _r;
        if (n != 7) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_lerp_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_y.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_y.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_y.arg5");
        if (a[6].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_y.arg6");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        p6 = (int64_t)a[6].as.i64;
        _r = jmath_vec3f_lerp_y(p0, p1, p2, p3, p4, p5, p6);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_lerp_z(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t p6;
        int64_t _r;
        if (n != 7) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_lerp_z");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_z.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_z.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_z.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_z.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_z.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_z.arg5");
        if (a[6].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_lerp_z.arg6");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        p6 = (int64_t)a[6].as.i64;
        _r = jmath_vec3f_lerp_z(p0, p1, p2, p3, p4, p5, p6);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_vec3f_distance(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3f_distance");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_distance.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_distance.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_distance.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_distance.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_distance.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3f_distance.arg5");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        _r = jmath_vec3f_distance(p0, p1, p2, p3, p4, p5);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_mul_vec2f_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_mul_vec2f_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_vec2f_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_vec2f_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_vec2f_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_vec2f_x.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_vec2f_x.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_vec2f_x.arg5");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        _r = jmath_mat22f_mul_vec2f_x(p0, p1, p2, p3, p4, p5);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_mul_vec2f_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_mul_vec2f_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_vec2f_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_vec2f_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_vec2f_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_vec2f_y.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_vec2f_y.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_vec2f_y.arg5");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        _r = jmath_mat22f_mul_vec2f_y(p0, p1, p2, p3, p4, p5);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_rotate_col1_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_rotate_col1_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_rotate_col1_x.arg0");
        p0 = (int64_t)a[0].as.i64;
        _r = jmath_mat22f_rotate_col1_x(p0);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_rotate_col1_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_rotate_col1_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_rotate_col1_y.arg0");
        p0 = (int64_t)a[0].as.i64;
        _r = jmath_mat22f_rotate_col1_y(p0);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_rotate_col2_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_rotate_col2_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_rotate_col2_x.arg0");
        p0 = (int64_t)a[0].as.i64;
        _r = jmath_mat22f_rotate_col2_x(p0);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_rotate_col2_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_rotate_col2_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_rotate_col2_y.arg0");
        p0 = (int64_t)a[0].as.i64;
        _r = jmath_mat22f_rotate_col2_y(p0);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_transpose_col1_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_transpose_col1_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col1_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col1_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col1_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col1_x.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_mat22f_transpose_col1_x(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_transpose_col1_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_transpose_col1_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col1_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col1_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col1_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col1_y.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_mat22f_transpose_col1_y(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_transpose_col2_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_transpose_col2_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col2_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col2_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col2_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col2_x.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_mat22f_transpose_col2_x(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_transpose_col2_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_transpose_col2_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col2_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col2_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col2_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_transpose_col2_y.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_mat22f_transpose_col2_y(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_invert_col1_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_invert_col1_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col1_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col1_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col1_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col1_x.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_mat22f_invert_col1_x(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_invert_col1_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_invert_col1_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col1_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col1_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col1_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col1_y.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_mat22f_invert_col1_y(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_invert_col2_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_invert_col2_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col2_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col2_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col2_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col2_x.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_mat22f_invert_col2_x(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_invert_col2_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_invert_col2_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col2_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col2_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col2_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_invert_col2_y.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_mat22f_invert_col2_y(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_add_col1_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t p6;
        int64_t p7;
        int64_t _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_add_col1_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_x.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_x.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_x.arg5");
        if (a[6].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_x.arg6");
        if (a[7].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_x.arg7");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        p6 = (int64_t)a[6].as.i64;
        p7 = (int64_t)a[7].as.i64;
        _r = jmath_mat22f_add_col1_x(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_add_col1_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t p6;
        int64_t p7;
        int64_t _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_add_col1_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_y.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_y.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_y.arg5");
        if (a[6].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_y.arg6");
        if (a[7].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col1_y.arg7");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        p6 = (int64_t)a[6].as.i64;
        p7 = (int64_t)a[7].as.i64;
        _r = jmath_mat22f_add_col1_y(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_add_col2_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t p6;
        int64_t p7;
        int64_t _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_add_col2_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_x.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_x.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_x.arg5");
        if (a[6].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_x.arg6");
        if (a[7].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_x.arg7");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        p6 = (int64_t)a[6].as.i64;
        p7 = (int64_t)a[7].as.i64;
        _r = jmath_mat22f_add_col2_x(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_add_col2_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t p6;
        int64_t p7;
        int64_t _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_add_col2_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_y.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_y.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_y.arg5");
        if (a[6].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_y.arg6");
        if (a[7].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_add_col2_y.arg7");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        p6 = (int64_t)a[6].as.i64;
        p7 = (int64_t)a[7].as.i64;
        _r = jmath_mat22f_add_col2_y(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_mul_col1_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t p6;
        int64_t p7;
        int64_t _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_mul_col1_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_x.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_x.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_x.arg5");
        if (a[6].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_x.arg6");
        if (a[7].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_x.arg7");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        p6 = (int64_t)a[6].as.i64;
        p7 = (int64_t)a[7].as.i64;
        _r = jmath_mat22f_mul_col1_x(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_mul_col1_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t p6;
        int64_t p7;
        int64_t _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_mul_col1_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_y.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_y.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_y.arg5");
        if (a[6].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_y.arg6");
        if (a[7].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col1_y.arg7");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        p6 = (int64_t)a[6].as.i64;
        p7 = (int64_t)a[7].as.i64;
        _r = jmath_mat22f_mul_col1_y(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_mul_col2_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t p6;
        int64_t p7;
        int64_t _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_mul_col2_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_x.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_x.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_x.arg5");
        if (a[6].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_x.arg6");
        if (a[7].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_x.arg7");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        p6 = (int64_t)a[6].as.i64;
        p7 = (int64_t)a[7].as.i64;
        _r = jmath_mat22f_mul_col2_x(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_mul_col2_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t p4;
        int64_t p5;
        int64_t p6;
        int64_t p7;
        int64_t _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_mul_col2_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_y.arg3");
        if (a[4].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_y.arg4");
        if (a[5].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_y.arg5");
        if (a[6].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_y.arg6");
        if (a[7].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_mul_col2_y.arg7");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        p4 = (int64_t)a[4].as.i64;
        p5 = (int64_t)a[5].as.i64;
        p6 = (int64_t)a[6].as.i64;
        p7 = (int64_t)a[7].as.i64;
        _r = jmath_mat22f_mul_col2_y(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_abs_col1_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_abs_col1_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col1_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col1_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col1_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col1_x.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_mat22f_abs_col1_x(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_abs_col1_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_abs_col1_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col1_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col1_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col1_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col1_y.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_mat22f_abs_col1_y(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_abs_col2_x(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_abs_col2_x");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col2_x.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col2_x.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col2_x.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col2_x.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_mat22f_abs_col2_x(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_mat22f_abs_col2_y(joy_value_t* a, int n, joy_status_t* st)
{
        int64_t p0;
        int64_t p1;
        int64_t p2;
        int64_t p3;
        int64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22f_abs_col2_y");
        if (a[0].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col2_y.arg0");
        if (a[1].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col2_y.arg1");
        if (a[2].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col2_y.arg2");
        if (a[3].kind != JOY_V_I64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22f_abs_col2_y.arg3");
        p0 = (int64_t)a[0].as.i64;
        p1 = (int64_t)a[1].as.i64;
        p2 = (int64_t)a[2].as.i64;
        p3 = (int64_t)a[3].as.i64;
        _r = jmath_mat22f_abs_col2_y(p0, p1, p2, p3);
        return joy_v_i64((int64_t)_r);
}

static joy_value_t t_jmath_ft_from_float(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_from_float");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_from_float.arg0");
        p0 = (float)a[0].as.f64;
        _r = jmath_ft_from_float(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_to_float(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_to_float");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_to_float.arg0");
        p0 = (float)a[0].as.f64;
        _r = jmath_ft_to_float(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_min_value(joy_value_t* a, int n, joy_status_t* st)
{
        float _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_min_value");
        _r = jmath_ft_min_value();
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_max_value(joy_value_t* a, int n, joy_status_t* st)
{
        float _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_max_value");
        _r = jmath_ft_max_value();
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_add(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_add");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_add.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_add.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_ft_add(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_sub(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_sub");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_sub.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_sub.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_ft_sub(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_mul(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_mul");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_mul.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_mul.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_ft_mul(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_div(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_div");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_div.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_div.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_ft_div(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_abs(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_abs");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_abs.arg0");
        p0 = (float)a[0].as.f64;
        _r = jmath_ft_abs(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_sqrt(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_sqrt");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_sqrt.arg0");
        p0 = (float)a[0].as.f64;
        _r = jmath_ft_sqrt(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_sin(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_sin");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_sin.arg0");
        p0 = (float)a[0].as.f64;
        _r = jmath_ft_sin(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_cos(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_cos");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_cos.arg0");
        p0 = (float)a[0].as.f64;
        _r = jmath_ft_cos(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_atan2(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_atan2");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_atan2.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_atan2.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_ft_atan2(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_zero(joy_value_t* a, int n, joy_status_t* st)
{
        float _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_zero");
        _r = jmath_ft_zero();
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_half(joy_value_t* a, int n, joy_status_t* st)
{
        float _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_half");
        _r = jmath_ft_half();
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_one(joy_value_t* a, int n, joy_status_t* st)
{
        float _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_one");
        _r = jmath_ft_one();
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_pi(joy_value_t* a, int n, joy_status_t* st)
{
        float _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_pi");
        _r = jmath_ft_pi();
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_max(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_max");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_max.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_max.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_ft_max(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_min(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_min");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_min.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_min.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_ft_min(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_pow2(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_pow2");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_pow2.arg0");
        p0 = (float)a[0].as.f64;
        _r = jmath_ft_pow2(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_sign(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        int32_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_sign");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_sign.arg0");
        p0 = (float)a[0].as.f64;
        _r = jmath_ft_sign(p0);
        return joy_v_i32((int32_t)_r);
}

static joy_value_t t_jmath_ft_clamp(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_clamp");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_clamp.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_clamp.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_clamp.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_ft_clamp(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_lerp(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_lerp");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_lerp.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_lerp.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_lerp.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_ft_lerp(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_negate_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_negate_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_negate_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_negate_x.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_vec2_negate_x(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_negate_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_negate_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_negate_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_negate_y.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_vec2_negate_y(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_negate(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        uint64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_negate");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_negate.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_negate.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_vec2_negate(p0, p1);
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_jmath_vec2_dot(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_dot");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_dot.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_dot.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_dot.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_dot.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec2_dot(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_cross(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_cross");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec2_cross(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_cross2_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_cross2_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross2_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross2_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross2_x.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec2_cross2_x(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_cross2_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_cross2_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross2_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross2_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross2_y.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec2_cross2_y(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_cross2(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        uint64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_cross2");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross2.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross2.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross2.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec2_cross2(p0, p1, p2);
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_jmath_vec2_cross3_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_cross3_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross3_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross3_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross3_x.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec2_cross3_x(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_cross3_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_cross3_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross3_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross3_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross3_y.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec2_cross3_y(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_cross3(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        uint64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_cross3");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross3.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross3.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_cross3.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec2_cross3(p0, p1, p2);
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_jmath_vec2_add_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_add_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_add_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_add_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_add_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_add_x.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec2_add_x(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_add_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_add_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_add_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_add_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_add_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_add_y.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec2_add_y(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_add(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        uint64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_add");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_add.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_add.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_add.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_add.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec2_add(p0, p1, p2, p3);
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_jmath_vec2_sub_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_sub_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_sub_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_sub_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_sub_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_sub_x.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec2_sub_x(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_sub_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_sub_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_sub_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_sub_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_sub_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_sub_y.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec2_sub_y(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_sub(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        uint64_t _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_sub");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_sub.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_sub.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_sub.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_sub.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec2_sub(p0, p1, p2, p3);
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_jmath_vec2_scale_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_scale_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_scale_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_scale_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_scale_x.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec2_scale_x(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_scale_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_scale_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_scale_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_scale_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_scale_y.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec2_scale_y(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_scale(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        uint64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_scale");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_scale.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_scale.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_scale.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec2_scale(p0, p1, p2);
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_jmath_vec2_abs_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_abs_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_abs_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_abs_x.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_vec2_abs_x(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_abs_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_abs_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_abs_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_abs_y.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_vec2_abs_y(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_abs(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        uint64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_abs");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_abs.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_abs.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_vec2_abs(p0, p1);
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_jmath_vec2_length_squared(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_length_squared");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_length_squared.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_length_squared.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_vec2_length_squared(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_length(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_length");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_length.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_length.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_vec2_length(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_normalize_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_normalize_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_normalize_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_normalize_x.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_vec2_normalize_x(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_normalize_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_normalize_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_normalize_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_normalize_y.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_vec2_normalize_y(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_normalize(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        uint64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_normalize");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_normalize.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_normalize.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_vec2_normalize(p0, p1);
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_jmath_vec2_normal_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_normal_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_normal_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_normal_x.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_vec2_normal_x(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_normal_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_normal_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_normal_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_normal_y.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_vec2_normal_y(p0, p1);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_normal(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        uint64_t _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_normal");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_normal.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_normal.arg1");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        _r = jmath_vec2_normal(p0, p1);
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_jmath_vec2_rotate_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_rotate_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_rotate_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_rotate_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_rotate_x.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec2_rotate_x(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_rotate_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_rotate_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_rotate_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_rotate_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_rotate_y.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec2_rotate_y(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_rotate(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        uint64_t _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_rotate");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_rotate.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_rotate.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_rotate.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec2_rotate(p0, p1, p2);
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_jmath_vec2_lerp_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_lerp_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp_x.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp_x.arg4");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        _r = jmath_vec2_lerp_x(p0, p1, p2, p3, p4);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_lerp_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_lerp_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp_y.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp_y.arg4");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        _r = jmath_vec2_lerp_y(p0, p1, p2, p3, p4);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec2_lerp(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        uint64_t _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_lerp");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_lerp.arg4");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        _r = jmath_vec2_lerp(p0, p1, p2, p3, p4);
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_jmath_vec2_distance(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec2_distance");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_distance.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_distance.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_distance.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec2_distance.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec2_distance(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_negate_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_negate_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_negate_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_negate_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_negate_x.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec3_negate_x(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_negate_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_negate_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_negate_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_negate_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_negate_y.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec3_negate_y(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_negate_z(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_negate_z");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_negate_z.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_negate_z.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_negate_z.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec3_negate_z(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_add_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_add_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_x.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_x.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_x.arg5");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = jmath_vec3_add_x(p0, p1, p2, p3, p4, p5);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_add_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_add_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_y.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_y.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_y.arg5");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = jmath_vec3_add_y(p0, p1, p2, p3, p4, p5);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_add_z(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_add_z");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_z.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_z.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_z.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_z.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_z.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_add_z.arg5");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = jmath_vec3_add_z(p0, p1, p2, p3, p4, p5);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_sub_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_sub_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_x.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_x.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_x.arg5");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = jmath_vec3_sub_x(p0, p1, p2, p3, p4, p5);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_sub_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_sub_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_y.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_y.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_y.arg5");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = jmath_vec3_sub_y(p0, p1, p2, p3, p4, p5);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_sub_z(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_sub_z");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_z.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_z.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_z.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_z.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_z.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_sub_z.arg5");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = jmath_vec3_sub_z(p0, p1, p2, p3, p4, p5);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_scale_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_scale_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_scale_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_scale_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_scale_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_scale_x.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec3_scale_x(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_scale_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_scale_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_scale_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_scale_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_scale_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_scale_y.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec3_scale_y(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_scale_z(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_scale_z");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_scale_z.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_scale_z.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_scale_z.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_scale_z.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec3_scale_z(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_abs_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_abs_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_abs_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_abs_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_abs_x.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec3_abs_x(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_abs_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_abs_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_abs_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_abs_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_abs_y.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec3_abs_y(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_abs_z(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_abs_z");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_abs_z.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_abs_z.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_abs_z.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec3_abs_z(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_dot(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_dot");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_dot.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_dot.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_dot.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_dot.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_dot.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_dot.arg5");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = jmath_vec3_dot(p0, p1, p2, p3, p4, p5);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_cross_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_cross_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_x.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_x.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_x.arg5");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = jmath_vec3_cross_x(p0, p1, p2, p3, p4, p5);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_cross_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_cross_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_y.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_y.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_y.arg5");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = jmath_vec3_cross_y(p0, p1, p2, p3, p4, p5);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_cross_z(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_cross_z");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_z.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_z.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_z.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_z.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_z.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_cross_z.arg5");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = jmath_vec3_cross_z(p0, p1, p2, p3, p4, p5);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_square_length(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_square_length");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_square_length.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_square_length.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_square_length.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec3_square_length(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_length(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_length");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_length.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_length.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_length.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec3_length(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_normalize_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_normalize_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_normalize_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_normalize_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_normalize_x.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec3_normalize_x(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_normalize_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_normalize_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_normalize_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_normalize_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_normalize_y.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec3_normalize_y(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_normalize_z(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_normalize_z");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_normalize_z.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_normalize_z.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_normalize_z.arg2");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = jmath_vec3_normalize_z(p0, p1, p2);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_lerp_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float p6;
        float _r;
        if (n != 7) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_lerp_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_x.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_x.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_x.arg5");
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_x.arg6");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        p6 = (float)a[6].as.f64;
        _r = jmath_vec3_lerp_x(p0, p1, p2, p3, p4, p5, p6);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_lerp_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float p6;
        float _r;
        if (n != 7) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_lerp_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_y.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_y.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_y.arg5");
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_y.arg6");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        p6 = (float)a[6].as.f64;
        _r = jmath_vec3_lerp_y(p0, p1, p2, p3, p4, p5, p6);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_lerp_z(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float p6;
        float _r;
        if (n != 7) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_lerp_z");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_z.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_z.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_z.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_z.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_z.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_z.arg5");
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_lerp_z.arg6");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        p6 = (float)a[6].as.f64;
        _r = jmath_vec3_lerp_z(p0, p1, p2, p3, p4, p5, p6);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_distance(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_distance");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_distance.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_distance.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_distance.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_distance.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_distance.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_distance.arg5");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = jmath_vec3_distance(p0, p1, p2, p3, p4, p5);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_mul_vec2_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_mul_vec2_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2_x.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2_x.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2_x.arg5");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = jmath_mat22_mul_vec2_x(p0, p1, p2, p3, p4, p5);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_mul_vec2_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_mul_vec2_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2_y.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2_y.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2_y.arg5");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = jmath_mat22_mul_vec2_y(p0, p1, p2, p3, p4, p5);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_mul_vec2(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        uint64_t _r;
        if (n != 6) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_mul_vec2");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_vec2.arg5");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        _r = jmath_mat22_mul_vec2(p0, p1, p2, p3, p4, p5);
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_jmath_mat22_rotate_col1_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_rotate_col1_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_rotate_col1_x.arg0");
        p0 = (float)a[0].as.f64;
        _r = jmath_mat22_rotate_col1_x(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_rotate_col1_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_rotate_col1_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_rotate_col1_y.arg0");
        p0 = (float)a[0].as.f64;
        _r = jmath_mat22_rotate_col1_y(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_rotate_col2_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_rotate_col2_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_rotate_col2_x.arg0");
        p0 = (float)a[0].as.f64;
        _r = jmath_mat22_rotate_col2_x(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_rotate_col2_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_rotate_col2_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_rotate_col2_y.arg0");
        p0 = (float)a[0].as.f64;
        _r = jmath_mat22_rotate_col2_y(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_transpose_col1_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_transpose_col1_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col1_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col1_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col1_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col1_x.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_mat22_transpose_col1_x(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_transpose_col1_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_transpose_col1_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col1_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col1_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col1_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col1_y.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_mat22_transpose_col1_y(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_transpose_col2_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_transpose_col2_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col2_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col2_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col2_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col2_x.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_mat22_transpose_col2_x(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_transpose_col2_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_transpose_col2_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col2_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col2_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col2_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_transpose_col2_y.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_mat22_transpose_col2_y(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_invert_col1_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_invert_col1_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col1_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col1_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col1_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col1_x.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_mat22_invert_col1_x(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_invert_col1_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_invert_col1_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col1_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col1_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col1_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col1_y.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_mat22_invert_col1_y(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_invert_col2_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_invert_col2_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col2_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col2_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col2_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col2_x.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_mat22_invert_col2_x(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_invert_col2_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_invert_col2_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col2_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col2_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col2_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_invert_col2_y.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_mat22_invert_col2_y(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_add_col1_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float p6;
        float p7;
        float _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_add_col1_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_x.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_x.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_x.arg5");
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_x.arg6");
        if (a[7].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_x.arg7");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        p6 = (float)a[6].as.f64;
        p7 = (float)a[7].as.f64;
        _r = jmath_mat22_add_col1_x(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_add_col1_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float p6;
        float p7;
        float _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_add_col1_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_y.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_y.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_y.arg5");
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_y.arg6");
        if (a[7].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col1_y.arg7");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        p6 = (float)a[6].as.f64;
        p7 = (float)a[7].as.f64;
        _r = jmath_mat22_add_col1_y(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_add_col2_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float p6;
        float p7;
        float _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_add_col2_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_x.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_x.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_x.arg5");
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_x.arg6");
        if (a[7].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_x.arg7");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        p6 = (float)a[6].as.f64;
        p7 = (float)a[7].as.f64;
        _r = jmath_mat22_add_col2_x(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_add_col2_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float p6;
        float p7;
        float _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_add_col2_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_y.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_y.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_y.arg5");
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_y.arg6");
        if (a[7].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_add_col2_y.arg7");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        p6 = (float)a[6].as.f64;
        p7 = (float)a[7].as.f64;
        _r = jmath_mat22_add_col2_y(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_mul_col1_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float p6;
        float p7;
        float _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_mul_col1_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_x.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_x.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_x.arg5");
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_x.arg6");
        if (a[7].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_x.arg7");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        p6 = (float)a[6].as.f64;
        p7 = (float)a[7].as.f64;
        _r = jmath_mat22_mul_col1_x(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_mul_col1_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float p6;
        float p7;
        float _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_mul_col1_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_y.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_y.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_y.arg5");
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_y.arg6");
        if (a[7].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col1_y.arg7");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        p6 = (float)a[6].as.f64;
        p7 = (float)a[7].as.f64;
        _r = jmath_mat22_mul_col1_y(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_mul_col2_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float p6;
        float p7;
        float _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_mul_col2_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_x.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_x.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_x.arg5");
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_x.arg6");
        if (a[7].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_x.arg7");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        p6 = (float)a[6].as.f64;
        p7 = (float)a[7].as.f64;
        _r = jmath_mat22_mul_col2_x(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_mul_col2_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float p4;
        float p5;
        float p6;
        float p7;
        float _r;
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_mul_col2_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_y.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_y.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_y.arg5");
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_y.arg6");
        if (a[7].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_mul_col2_y.arg7");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        p6 = (float)a[6].as.f64;
        p7 = (float)a[7].as.f64;
        _r = jmath_mat22_mul_col2_y(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_abs_col1_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_abs_col1_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col1_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col1_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col1_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col1_x.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_mat22_abs_col1_x(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_abs_col1_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_abs_col1_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col1_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col1_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col1_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col1_y.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_mat22_abs_col1_y(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_abs_col2_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_abs_col2_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col2_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col2_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col2_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col2_x.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_mat22_abs_col2_x(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_mat22_abs_col2_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_mat22_abs_col2_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col2_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col2_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col2_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_mat22_abs_col2_y.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_mat22_abs_col2_y(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_div_x(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_div_x");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_div_x.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_div_x.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_div_x.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_div_x.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec3_div_x(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_div_y(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_div_y");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_div_y.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_div_y.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_div_y.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_div_y.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec3_div_y(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_vec3_div_z(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float p1;
        float p2;
        float p3;
        float _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jmath_vec3_div_z");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_div_z.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_div_z.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_div_z.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_vec3_div_z.arg3");
        p0 = (float)a[0].as.f64;
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = jmath_vec3_div_z(p0, p1, p2, p3);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jmath_ft_radians(joy_value_t* a, int n, joy_status_t* st)
{
        float p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jmath_ft_radians");
        if (a[0].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jmath_ft_radians.arg0");
        p0 = (float)a[0].as.f64;
        _r = jmath_ft_radians(p0);
        return joy_v_f64((float)_r);
}

static const joy_value_kind k_joy_core_lcg_random_create[] = { JOY_V_U64 };
static const joy_value_kind k_joy_core_lcg_random_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_core_lcg_random_next[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_core_lcg_random_set_state[] = { JOY_V_HANDLE, JOY_V_U64 };
static const joy_value_kind k_jpm_config_add_spawn[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_jpm_wave_seed[] = { JOY_V_U32, JOY_V_I32 };
static const joy_value_kind k_joy_core_sdbm_hash[] = { JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_core_fnv1a_hash[] = { JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_core_jenkins_hash[] = { JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_core_murmur3_hash[] = { JOY_V_STR };
static const joy_value_kind k_joy_core_kr_hash[] = { JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_scene_jtimer_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_scene_jtimer_trigger[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_scene_jtimer_set_interval[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_scene_jtimer_set_active[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_scene_jtimer_reset[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_scene_jtimer_get_elapsed[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_core_create_window[] = { JOY_V_STR, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_joy_core_destroy_window[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_core_window_set_fullscreen[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_core_window_get_fullscreen[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_core_window_set_icon[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_joy_core_push_user_event[] = { JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_joy_core_log_title[] = { JOY_V_I32 };
static const joy_value_kind k_joy_core_env_set[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_STR };
static const joy_value_kind k_joy_core_env_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_core_simple_fps_destory[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_core_simple_fps_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_core_simple_fps_value[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_scene_game_timer_init[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_scene_game_timer_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_scene_game_timer_get_delta_time[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_scene_game_timer_get_unscaled_delta_time[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_scene_game_timer_set_time_scale[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_joy_scene_game_timer_get_time_scale[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_scene_game_timer_set_target_fps[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_scene_game_timer_get_target_fps[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_core_set_clipboard_text[] = { JOY_V_STR };
static const joy_value_kind k_sys_closesocket[] = { JOY_V_I64 };
static const joy_value_kind k_sys_set_sock_sndtimeo[] = { JOY_V_I64, JOY_V_I32 };
static const joy_value_kind k_sys_set_sock_rcvtimeo[] = { JOY_V_I64, JOY_V_I32 };
static const joy_value_kind k_sys_set_sock_accpettimeo[] = { JOY_V_I64, JOY_V_I32 };
static const joy_value_kind k_sys_set_sock_broadcast[] = { JOY_V_I64 };
static const joy_value_kind k_sys_set_sock_reuseaddr[] = { JOY_V_I64 };
static const joy_value_kind k_sys_bind[] = { JOY_V_I64, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_sys_connect[] = { JOY_V_I64, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_sys_listen[] = { JOY_V_I64 };
static const joy_value_kind k_sys_send[] = { JOY_V_I64, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_sys_sendto[] = { JOY_V_I64, JOY_V_STR, JOY_V_I32, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_sys_sleep[] = { JOY_V_I32 };
static const joy_value_kind k_joy_net_iocp_server_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_iocp_process_events[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_r2d_font_create[] = { JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_r2d_font_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_font_get_scale[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_font_get_size[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_font_get_ascent[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_create[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_set_color[] = { JOY_V_HANDLE, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_r2d_begin[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_set_logical_presentation[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_joy_r2d_set_logical_presentation_mode[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_joy_r2d_set_vsync[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_r2d_set_scale[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r2d_set_colorf[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r2d_set_blend_mode[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_r2d_draw_point[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r2d_set_clip_rect[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r2d_clear_clip[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_push_clip_rect[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r2d_pop_clip_rect[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_reset_clip_stack[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_point_in_clip[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_shape_draw_line[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_shape_draw_rectangle[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_image_create[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_joy_ui_image_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_image_set_alpha[] = { JOY_V_HANDLE, JOY_V_U32 };
static const joy_value_kind k_joy_ui_image_set_blend_mode[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_ui_image_set_scale_mode[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_ui_text_updatex[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_STR, JOY_V_I32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_ui_text_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_jtext_print[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r2d_jtext_printx[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_STR, JOY_V_I32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r2d_draw_point_f[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_shape_draw_rectangle_f[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_shape_draw_line_f[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r2d_jtext_printf[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_STR, JOY_V_I32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r2d_jtext_printf_scaled[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_STR, JOY_V_I32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_text_get_width[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_text_get_height[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_jtext_cache_set_limit[] = { JOY_V_I32 };
static const joy_value_kind k_joy_r2d_jtext_cache_sweep[] = { JOY_V_I32 };
static const joy_value_kind k_animation_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_animation_reset[] = { JOY_V_HANDLE };
static const joy_value_kind k_animation_add_clip[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_animation_is_finished[] = { JOY_V_HANDLE };
static const joy_value_kind k_animation_set_position[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_animation_set_scale[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_animation_set_rotation[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_animation_update[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_begin[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_get_draw_calls[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_set_clear_color[] = { JOY_V_HANDLE, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_r3d_mesh_destroy[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_font_init[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_r3d_font_draw_2d[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_STR, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_font_cache_set_limit[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_r3d_font_cache_get_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_font_cache_clear[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_font_cache_sweep[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_r3d_set_logical_presentation[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_joy_r3d_set_logical_presentation_mode[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_joy_r3d_button_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_button_draw_circle[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_button_is_hovered[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_button_is_pressed[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_BOOL, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_button_release[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_button_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_label_set_text[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_joy_r3d_label_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_label_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_input_set_text[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_joy_r3d_input_is_hovered[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_input_focus[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_input_defocus[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_input_is_focused[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_input_start_text_input[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_input_stop_text_input[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_input_insert[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_joy_r3d_input_backspace[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_input_delete[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_input_cursor_left[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_input_cursor_right[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_input_cursor_home[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_input_cursor_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_input_click[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_input_submit[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_input_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_input_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_combobox_input_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_combobox_input_add_item[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_joy_r3d_combobox_input_clear_items[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_combobox_input_set_text[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_joy_r3d_combobox_input_hit_item[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_combobox_input_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_combobox_input_draw_dropdown[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_slider_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_slider_set_value[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_slider_get_value[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_slider_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_slider_is_pressed[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_slider_set_pressed[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_r3d_slider_drag_move[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_slider_is_hovered[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_slider_set_hovered[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_r3d_checkbox_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_checkbox_toggle[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_checkbox_is_pressed[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_checkbox_set_hovered[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_r3d_checkbox_is_hovered[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_checkbox_set_colors[] = { JOY_V_HANDLE, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_r3d_checkbox_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_draw_grid[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_janimator_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_janimator_set_float[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_F64 };
static const joy_value_kind k_janimator_set_int[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_janimator_set_bool[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_BOOL };
static const joy_value_kind k_janimator_set_trigger[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_janimator_get_float[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_janimator_get_int[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_janimator_get_bool[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_janimator_add_state[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_janimator_set_default_state[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_janimator_state_add_clip[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_STR, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_janimator_state_add_color[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_F64, JOY_V_I32, JOY_V_I32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_janimator_state_set_loop[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_BOOL };
static const joy_value_kind k_janimator_state_set_speed[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_F64 };
static const joy_value_kind k_janimator_state_set_blend[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_STR };
static const joy_value_kind k_janimator_state_add_motion[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_STR, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_janimator_add_transition[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_STR };
static const joy_value_kind k_janimator_add_any_transition[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_janimator_transition_add_cond[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_STR, JOY_V_I32, JOY_V_F64 };
static const joy_value_kind k_janimator_transition_set_exit_time[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_F64 };
static const joy_value_kind k_janimator_update[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_janimator_draw[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_janimator_set_flip_h[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_ui_label_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_label_draw[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_button_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_button_set_position[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_button_set_size[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_button_set_normal_color[] = { JOY_V_HANDLE, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_ui_button_set_hover_color[] = { JOY_V_HANDLE, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_ui_button_set_pressed_color[] = { JOY_V_HANDLE, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_ui_button_set_image[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_button_set_pressed_image[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_button_set_textx[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_STR, JOY_V_I32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_ui_button_is_hovered[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_button_is_pressed[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_button_set_hovered[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_ui_button_set_pressed[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_ui_button_get_pressed[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_button_set_text_offset[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_button_draw[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_scrollview_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_scrollview_set_view_px[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_scrollview_set_content[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_scrollview_set_offset[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_scrollview_clamp[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_scrollview_get_offset_x[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_scrollview_get_offset_y[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_scrollview_set_show_bar[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_ui_scrollview_dragging[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_scrollview_hit[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_scrollview_begin[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_scrollview_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_combobox_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_combobox_add_item[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_STR, JOY_V_I32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_ui_combobox_is_pressed[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_combobox_is_open[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_combobox_set_open[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_ui_combobox_hit_item[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_combobox_set_selected[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_ui_combobox_get_selected[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_combobox_draw[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_combobox_input_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_combobox_input_add_item[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_ui_combobox_input_clear_items[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_combobox_input_set_text[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_joy_ui_combobox_input_draw[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_combobox_input_draw_dropdown[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_combobox_input_is_open[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_combobox_input_set_popup_node[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_combobox_input_hit_item[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_combobox_input_set_open[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_ui_datagrid_setheaders[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_joy_ui_datagrid_setheader[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_STR };
static const joy_value_kind k_joy_ui_datagrid_setcell[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32, JOY_V_STR };
static const joy_value_kind k_joy_ui_datagrid_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_datagrid_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_datagrid_hit_row[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_datagrid_scroll[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_ui_datagrid_set_selected_row[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_ui_radio_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_radio_is_pressed[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_radio_set_hovered[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_ui_checkbox_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_checkbox_set_colors[] = { JOY_V_HANDLE, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_ui_checkbox_set_r2d[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_checkbox_set_position[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_joy_ui_checkbox_set_text_color[] = { JOY_V_HANDLE, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_ui_checkbox_toggle[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_checkbox_is_pressed[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_checkbox_set_hovered[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_input_joystick_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_input_joystick_set_textures[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_STR };
static const joy_value_kind k_joy_input_joystick_set_vector[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_input_joystick_draw[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_input_joystick_reset[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_input_joystick_set_position[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_input_joystick_set_radius[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_joy_input_joystick_is_dragging[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_input_joystick_touch_down[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_input_joystick_touch_move[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_input_joystick_touch_up[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_input_joystick_get_magnitude[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_set_text[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_joy_ui_inputbox_draw[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_is_pressed[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_inputbox_is_hovered[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_inputbox_focus[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_defocus[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_blur[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_insert[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_ui_inputbox_backspace[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_delete[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_cursor_left[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_cursor_right[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_cursor_home[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_cursor_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_select_left[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_select_right[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_select_home[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_select_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_select_all[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_paste[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_copy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_cut[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_inputbox_is_focused[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_slider_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_slider_set_value[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_joy_ui_slider_get_value[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_slider_set_rect[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_slider_set_range[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_slider_draw[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_slider_is_pressed[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_slider_set_pressed[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_ui_slider_drag_move[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_slider_is_hovered[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_slider_set_hovered[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_ui_popup_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_popup_set_title[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_ui_popup_set_content[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_ui_popup_set_position[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_popup_set_visible[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_ui_popup_set_closeable[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_ui_popup_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_popup_is_close_clicked[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_popup_is_title_clicked[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_popup_drag_move[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_popup_is_inside[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_tooltip_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_tooltip_set_text[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_ui_tooltip_set_position[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_tooltip_set_visible[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joy_ui_tooltip_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_ctx_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_begin[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_input_motion[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_input_down[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_ui_input_up[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_ui_input_wheel[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_joy_ui_input_key[] = { JOY_V_HANDLE, JOY_V_BOOL, JOY_V_BOOL };
static const joy_value_kind k_joy_ui_is_down[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_is_pressed[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_is_released[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_take_wheel[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_is_shift[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_is_ctrl[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_push_clip[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_pop_clip[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_reset_clip[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_hit[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_fill_rect[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_ui_stroke_rect[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_ui_line[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_ui_text[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_STR, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_ui_text_centered[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_STR, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_ui_panel_begin[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_STR };
static const joy_value_kind k_joy_ui_panel_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_panel_frame[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_scroll_begin[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_scroll_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_scrollbar[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_scroll_set_offset[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_ui_button[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_STR };
static const joy_value_kind k_jtilemap_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_jtilemap_get_width[] = { JOY_V_HANDLE };
static const joy_value_kind k_jtilemap_get_height[] = { JOY_V_HANDLE };
static const joy_value_kind k_jtilemap_get_tilewidth[] = { JOY_V_HANDLE };
static const joy_value_kind k_jtilemap_get_tileheight[] = { JOY_V_HANDLE };
static const joy_value_kind k_jtilemap_get_anim_tileid[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_U32 };
static const joy_value_kind k_jtilemap_get_tile[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_jtilemap_render_all[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_U32 };
static const joy_value_kind k_jtilemap_render_tiles[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_U32 };
static const joy_value_kind k_jtilemap_render_set_tile_px[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_jtilemap_render_layer[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_STR, JOY_V_F64, JOY_V_F64, JOY_V_U32 };
static const joy_value_kind k_jtilemap_render_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_kcpserver_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_kcpserver_send[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_net_kcpserver_broadcast[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_net_kcpserver_offline[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_net_kcpserver_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_kcpserver_connection_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_kcpserver_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_net_kcpclient_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_kcpclient_send[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_net_kcpclient_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_kcpclient_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_net_tcpserver_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_tcpserver_send[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_net_tcpserver_broadcast[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_net_tcpserver_offline[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_net_tcpserver_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_tcpserver_connection_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_tcpserver_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_net_tcpclient_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_tcpclient_send[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_net_tcpclient_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_tcpclient_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_net_wsnetserver_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_wsnetserver_send[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_net_wsnetserver_broadcast[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_net_wsnetserver_offline[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_net_wsnetserver_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_wsnetserver_connection_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_wsnetserver_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_net_wsnetclient_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_wsnetclient_send[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_net_wsnetclient_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_wsnetclient_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_net_netclient_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_netclient_send[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_net_netclient_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_netclient_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_net_netserver_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_netserver_send[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_net_netserver_broadcast[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_joy_net_netserver_offline[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_net_netserver_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_netserver_connection_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_net_netserver_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_net_netmessage_free_data[] = { JOY_V_HANDLE };
static const joy_value_kind k_scene_node_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_scene_node_add_child[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_scene_node_remove_child[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_scene_node_get_child_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_scene_node_set_position[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_scene_node_set_world_position[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_scene_node_set_rotation[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_scene_node_get_rotation[] = { JOY_V_HANDLE };
static const joy_value_kind k_scene_node_set_scale[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_scene_node_set_zorder[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_scene_node_get_zorder[] = { JOY_V_HANDLE };
static const joy_value_kind k_scene_node_set_name[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_scene_node_set_tag[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_scene_node_update[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_scene_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_scene_add_root_node[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_scene_remove_root_node[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_scene_get_root_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_scene_update[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_scene_set_popup_node[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_scene_manager_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_scene_manager_push[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_scene_manager_replace[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_scene_manager_pop[] = { JOY_V_HANDLE };
static const joy_value_kind k_scene_manager_update[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_scene_manager_get_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_particle_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_particle_set_position[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r2d_particle_emit[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_particle_clear[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_particle_update[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_joy_r2d_particle_render[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_particle_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r2d_particle_is_active[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_light_sys_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_light_sys_resize[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_joy_r3d_light_remove[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_joy_r3d_light_clear[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_light_add_occluder[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joy_r3d_light_clear_occluders[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_r3d_light_set_ambient[] = { JOY_V_HANDLE, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_joy_r3d_light_render[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_jecs_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_jecs_process[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_jecs_define[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_U64 };
static const joy_value_kind k_audio_close_device[] = { JOY_V_U32 };
static const joy_value_kind k_audio_create_wav[] = { JOY_V_STR, JOY_V_U32 };
static const joy_value_kind k_audio_create_mp3[] = { JOY_V_STR, JOY_V_U32 };
static const joy_value_kind k_audio_create_flac[] = { JOY_V_STR, JOY_V_U32 };
static const joy_value_kind k_audio_create_ogg[] = { JOY_V_STR, JOY_V_U32 };
static const joy_value_kind k_audio_shot_play[] = { JOY_V_HANDLE };
static const joy_value_kind k_audio_shot_set_gain[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_audio_shot_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_audio_bgm_stop[] = { JOY_V_HANDLE };
static const joy_value_kind k_audio_bgm_set_gain[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_audio_bgm_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joy_ui_image_draw_rect[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_fp_from_float[] = { JOY_V_F64 };
static const joy_value_kind k_jmath_fp_to_float[] = { JOY_V_I64 };
static const joy_value_kind k_jmath_fp_from_uint[] = { JOY_V_U32 };
static const joy_value_kind k_jmath_fp_from_int[] = { JOY_V_I32 };
static const joy_value_kind k_jmath_fp_from_fraction[] = { JOY_V_U32 };
static const joy_value_kind k_jmath_fp_add[] = { JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_fp_sub[] = { JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_fp_max[] = { JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_fp_min[] = { JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_fp_pow2[] = { JOY_V_I64 };
static const joy_value_kind k_jmath_fp_sign[] = { JOY_V_I64 };
static const joy_value_kind k_jmath_fp_clamp[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_fp_lerp[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_negate_x[] = { JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_negate_y[] = { JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_dot[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_cross[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_cross2_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_cross2_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_cross3_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_cross3_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_add_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_add_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_sub_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_sub_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_scale_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_scale_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_abs_x[] = { JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_abs_y[] = { JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_length_squared[] = { JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_length[] = { JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_normalize_x[] = { JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_normalize_y[] = { JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_normal_x[] = { JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_normal_y[] = { JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_rotate_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_rotate_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_lerp_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_lerp_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec2f_distance[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_negate_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_negate_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_negate_z[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_add_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_add_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_add_z[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_sub_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_sub_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_sub_z[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_scale_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_scale_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_scale_z[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_abs_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_abs_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_abs_z[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_dot[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_cross_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_cross_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_cross_z[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_square_length[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_length[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_normalize_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_normalize_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_normalize_z[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_lerp_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_lerp_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_lerp_z[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_vec3f_distance[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_mul_vec2f_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_mul_vec2f_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_rotate_col1_x[] = { JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_rotate_col1_y[] = { JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_rotate_col2_x[] = { JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_rotate_col2_y[] = { JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_transpose_col1_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_transpose_col1_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_transpose_col2_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_transpose_col2_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_invert_col1_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_invert_col1_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_invert_col2_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_invert_col2_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_add_col1_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_add_col1_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_add_col2_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_add_col2_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_mul_col1_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_mul_col1_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_mul_col2_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_mul_col2_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_abs_col1_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_abs_col1_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_abs_col2_x[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_mat22f_abs_col2_y[] = { JOY_V_I64, JOY_V_I64, JOY_V_I64, JOY_V_I64 };
static const joy_value_kind k_jmath_ft_from_float[] = { JOY_V_F64 };
static const joy_value_kind k_jmath_ft_to_float[] = { JOY_V_F64 };
static const joy_value_kind k_jmath_ft_add[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_ft_sub[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_ft_mul[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_ft_div[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_ft_abs[] = { JOY_V_F64 };
static const joy_value_kind k_jmath_ft_sqrt[] = { JOY_V_F64 };
static const joy_value_kind k_jmath_ft_sin[] = { JOY_V_F64 };
static const joy_value_kind k_jmath_ft_cos[] = { JOY_V_F64 };
static const joy_value_kind k_jmath_ft_atan2[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_ft_max[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_ft_min[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_ft_pow2[] = { JOY_V_F64 };
static const joy_value_kind k_jmath_ft_sign[] = { JOY_V_F64 };
static const joy_value_kind k_jmath_ft_clamp[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_ft_lerp[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_negate_x[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_negate_y[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_negate[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_dot[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_cross[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_cross2_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_cross2_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_cross2[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_cross3_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_cross3_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_cross3[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_add_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_add_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_add[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_sub_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_sub_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_sub[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_scale_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_scale_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_scale[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_abs_x[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_abs_y[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_abs[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_length_squared[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_length[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_normalize_x[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_normalize_y[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_normalize[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_normal_x[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_normal_y[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_normal[] = { JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_rotate_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_rotate_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_rotate[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_lerp_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_lerp_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_lerp[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec2_distance[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_negate_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_negate_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_negate_z[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_add_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_add_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_add_z[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_sub_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_sub_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_sub_z[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_scale_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_scale_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_scale_z[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_abs_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_abs_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_abs_z[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_dot[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_cross_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_cross_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_cross_z[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_square_length[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_length[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_normalize_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_normalize_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_normalize_z[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_lerp_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_lerp_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_lerp_z[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_distance[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_mul_vec2_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_mul_vec2_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_mul_vec2[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_rotate_col1_x[] = { JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_rotate_col1_y[] = { JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_rotate_col2_x[] = { JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_rotate_col2_y[] = { JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_transpose_col1_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_transpose_col1_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_transpose_col2_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_transpose_col2_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_invert_col1_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_invert_col1_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_invert_col2_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_invert_col2_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_add_col1_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_add_col1_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_add_col2_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_add_col2_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_mul_col1_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_mul_col1_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_mul_col2_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_mul_col2_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_abs_col1_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_abs_col1_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_abs_col2_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_mat22_abs_col2_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_div_x[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_div_y[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_vec3_div_z[] = { JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jmath_ft_radians[] = { JOY_V_F64 };

static const joy_thunk_entry s_thunks[] = {
        { "core", "joy_core_lcg_random_create", "LcgRandomCreate", t_joy_core_lcg_random_create, 1, k_joy_core_lcg_random_create, JOY_V_HANDLE },
        { "core", "joy_core_lcg_random_destroy", "LcgRandomDestroy", t_joy_core_lcg_random_destroy, 1, k_joy_core_lcg_random_destroy, JOY_V_NULL },
        { "core", "joy_core_lcg_random_next", "LcgRandomNext", t_joy_core_lcg_random_next, 1, k_joy_core_lcg_random_next, JOY_V_U32 },
        { "core", "joy_core_lcg_random_set_state", "LcgRandomSetState", t_joy_core_lcg_random_set_state, 2, k_joy_core_lcg_random_set_state, JOY_V_NULL },
        { "jpm", "jpm_config_add_spawn", "JpmConfigAddSpawn", t_jpm_config_add_spawn, 4, k_jpm_config_add_spawn, JOY_V_NULL },
        { "jpm", "jpm_wave_seed", "JpmWaveSeed", t_jpm_wave_seed, 2, k_jpm_wave_seed, JOY_V_U32 },
        { "core", "joy_core_sdbm_hash", "SdbmHash", t_joy_core_sdbm_hash, 2, k_joy_core_sdbm_hash, JOY_V_U64 },
        { "core", "joy_core_fnv1a_hash", "Fnv1aHash", t_joy_core_fnv1a_hash, 2, k_joy_core_fnv1a_hash, JOY_V_U64 },
        { "core", "joy_core_jenkins_hash", "JenkinsHash", t_joy_core_jenkins_hash, 2, k_joy_core_jenkins_hash, JOY_V_U64 },
        { "core", "joy_core_murmur3_hash", "Murmur3Hash", t_joy_core_murmur3_hash, 1, k_joy_core_murmur3_hash, JOY_V_U64 },
        { "core", "joy_core_kr_hash", "KrHash", t_joy_core_kr_hash, 2, k_joy_core_kr_hash, JOY_V_U64 },
        { "scene", "joy_scene_jtimer_destroy", "JtimerDestroy", t_joy_scene_jtimer_destroy, 1, k_joy_scene_jtimer_destroy, JOY_V_NULL },
        { "scene", "joy_scene_jtimer_trigger", "JtimerTrigger", t_joy_scene_jtimer_trigger, 1, k_joy_scene_jtimer_trigger, JOY_V_NULL },
        { "scene", "joy_scene_jtimer_set_interval", "JtimerSetInterval", t_joy_scene_jtimer_set_interval, 2, k_joy_scene_jtimer_set_interval, JOY_V_NULL },
        { "scene", "joy_scene_jtimer_set_active", "JtimerSetActive", t_joy_scene_jtimer_set_active, 2, k_joy_scene_jtimer_set_active, JOY_V_NULL },
        { "scene", "joy_scene_jtimer_reset", "JtimerReset", t_joy_scene_jtimer_reset, 1, k_joy_scene_jtimer_reset, JOY_V_NULL },
        { "scene", "joy_scene_jtimer_get_elapsed", "JtimerGetElapsed", t_joy_scene_jtimer_get_elapsed, 1, k_joy_scene_jtimer_get_elapsed, JOY_V_U64 },
        { "core", "joy_core_init_env", "InitEnv", t_joy_core_init_env, 0, NULL, JOY_V_BOOL },
        { "core", "joy_core_quit_env", "QuitEnv", t_joy_core_quit_env, 0, NULL, JOY_V_NULL },
        { "core", "joy_core_get_ticks", "GetTicks", t_joy_core_get_ticks, 0, NULL, JOY_V_U64 },
        { "core", "joy_core_create_window", "CreateWindow", t_joy_core_create_window, 3, k_joy_core_create_window, JOY_V_HANDLE },
        { "core", "joy_core_destroy_window", "DestroyWindow", t_joy_core_destroy_window, 1, k_joy_core_destroy_window, JOY_V_NULL },
        { "core", "joy_core_window_set_fullscreen", "WindowSetFullscreen", t_joy_core_window_set_fullscreen, 2, k_joy_core_window_set_fullscreen, JOY_V_NULL },
        { "core", "joy_core_window_get_fullscreen", "WindowGetFullscreen", t_joy_core_window_get_fullscreen, 1, k_joy_core_window_get_fullscreen, JOY_V_BOOL },
        { "core", "joy_core_window_set_icon", "WindowSetIcon", t_joy_core_window_set_icon, 2, k_joy_core_window_set_icon, JOY_V_NULL },
        { "core", "joy_core_push_user_event", "PushUserEvent", t_joy_core_push_user_event, 2, k_joy_core_push_user_event, JOY_V_BOOL },
        { "core", "joy_core_push_quit_event", "PushQuitEvent", t_joy_core_push_quit_event, 0, NULL, JOY_V_BOOL },
        { "core", "joy_core_log_title", "LogTitle", t_joy_core_log_title, 1, k_joy_core_log_title, JOY_V_NULL },
        { "core", "joy_core_env_set", "EnvSet", t_joy_core_env_set, 3, k_joy_core_env_set, JOY_V_NULL },
        { "core", "joy_core_env_destroy", "EnvDestroy", t_joy_core_env_destroy, 1, k_joy_core_env_destroy, JOY_V_NULL },
        { "core", "joy_core_simple_fps_destory", "SimpleFpsDestory", t_joy_core_simple_fps_destory, 1, k_joy_core_simple_fps_destory, JOY_V_NULL },
        { "core", "joy_core_simple_fps_update", "SimpleFpsUpdate", t_joy_core_simple_fps_update, 1, k_joy_core_simple_fps_update, JOY_V_NULL },
        { "core", "joy_core_simple_fps_value", "SimpleFpsValue", t_joy_core_simple_fps_value, 1, k_joy_core_simple_fps_value, JOY_V_I32 },
        { "scene", "joy_scene_game_timer_init", "GameTimerInit", t_joy_scene_game_timer_init, 1, k_joy_scene_game_timer_init, JOY_V_NULL },
        { "scene", "joy_scene_game_timer_update", "GameTimerUpdate", t_joy_scene_game_timer_update, 1, k_joy_scene_game_timer_update, JOY_V_NULL },
        { "scene", "joy_scene_game_timer_get_delta_time", "GameTimerGetDeltaTime", t_joy_scene_game_timer_get_delta_time, 1, k_joy_scene_game_timer_get_delta_time, JOY_V_F64 },
        { "scene", "joy_scene_game_timer_get_unscaled_delta_time", "GameTimerGetUnscaledDeltaTime", t_joy_scene_game_timer_get_unscaled_delta_time, 1, k_joy_scene_game_timer_get_unscaled_delta_time, JOY_V_F64 },
        { "scene", "joy_scene_game_timer_set_time_scale", "GameTimerSetTimeScale", t_joy_scene_game_timer_set_time_scale, 2, k_joy_scene_game_timer_set_time_scale, JOY_V_NULL },
        { "scene", "joy_scene_game_timer_get_time_scale", "GameTimerGetTimeScale", t_joy_scene_game_timer_get_time_scale, 1, k_joy_scene_game_timer_get_time_scale, JOY_V_F64 },
        { "scene", "joy_scene_game_timer_set_target_fps", "GameTimerSetTargetFps", t_joy_scene_game_timer_set_target_fps, 2, k_joy_scene_game_timer_set_target_fps, JOY_V_NULL },
        { "scene", "joy_scene_game_timer_get_target_fps", "GameTimerGetTargetFps", t_joy_scene_game_timer_get_target_fps, 1, k_joy_scene_game_timer_get_target_fps, JOY_V_I32 },
        { "core", "joy_core_set_clipboard_text", "SetClipboardText", t_joy_core_set_clipboard_text, 1, k_joy_core_set_clipboard_text, JOY_V_NULL },
        { "sys", "sys_init_netenv", "SysInitNetenv", t_sys_init_netenv, 0, NULL, JOY_V_BOOL },
        { "sys", "sys_release_netenv", "SysReleaseNetenv", t_sys_release_netenv, 0, NULL, JOY_V_BOOL },
        { "sys", "sys_anyaddr", "SysAnyaddr", t_sys_anyaddr, 0, NULL, JOY_V_I32 },
        { "sys", "sys_closesocket", "SysClosesocket", t_sys_closesocket, 1, k_sys_closesocket, JOY_V_BOOL },
        { "sys", "sys_set_sock_sndtimeo", "SysSetSockSndtimeo", t_sys_set_sock_sndtimeo, 2, k_sys_set_sock_sndtimeo, JOY_V_I32 },
        { "sys", "sys_set_sock_rcvtimeo", "SysSetSockRcvtimeo", t_sys_set_sock_rcvtimeo, 2, k_sys_set_sock_rcvtimeo, JOY_V_I32 },
        { "sys", "sys_set_sock_accpettimeo", "SysSetSockAccpettimeo", t_sys_set_sock_accpettimeo, 2, k_sys_set_sock_accpettimeo, JOY_V_I32 },
        { "sys", "sys_set_sock_broadcast", "SysSetSockBroadcast", t_sys_set_sock_broadcast, 1, k_sys_set_sock_broadcast, JOY_V_I32 },
        { "sys", "sys_set_sock_reuseaddr", "SysSetSockReuseaddr", t_sys_set_sock_reuseaddr, 1, k_sys_set_sock_reuseaddr, JOY_V_I32 },
        { "sys", "sys_tcp", "SysTcp", t_sys_tcp, 0, NULL, JOY_V_I64 },
        { "sys", "sys_udp", "SysUdp", t_sys_udp, 0, NULL, JOY_V_I64 },
        { "sys", "sys_bind", "SysBind", t_sys_bind, 3, k_sys_bind, JOY_V_BOOL },
        { "sys", "sys_connect", "SysConnect", t_sys_connect, 3, k_sys_connect, JOY_V_BOOL },
        { "sys", "sys_listen", "SysListen", t_sys_listen, 1, k_sys_listen, JOY_V_BOOL },
        { "sys", "sys_send", "SysSend", t_sys_send, 3, k_sys_send, JOY_V_I32 },
        { "sys", "sys_sendto", "SysSendto", t_sys_sendto, 5, k_sys_sendto, JOY_V_I32 },
        { "sys", "sys_current_time", "SysCurrentTime", t_sys_current_time, 0, NULL, JOY_V_I64 },
        { "sys", "sys_sleep", "SysSleep", t_sys_sleep, 1, k_sys_sleep, JOY_V_NULL },
        { "sys", "sys_install_crash_handlers", "SysInstallCrashHandlers", t_sys_install_crash_handlers, 0, NULL, JOY_V_NULL },
        { "net", "joy_net_iocp_server_destroy", "IocpServerDestroy", t_joy_net_iocp_server_destroy, 1, k_joy_net_iocp_server_destroy, JOY_V_NULL },
        { "net", "joy_net_iocp_process_events", "IocpProcessEvents", t_joy_net_iocp_process_events, 2, k_joy_net_iocp_process_events, JOY_V_NULL },
        { "r2d", "joy_r2d_font_create", "FontCreate", t_joy_r2d_font_create, 2, k_joy_r2d_font_create, JOY_V_HANDLE },
        { "r2d", "joy_r2d_font_destroy", "FontDestroy", t_joy_r2d_font_destroy, 1, k_joy_r2d_font_destroy, JOY_V_NULL },
        { "r2d", "joy_r2d_font_get_scale", "FontGetScale", t_joy_r2d_font_get_scale, 1, k_joy_r2d_font_get_scale, JOY_V_F64 },
        { "r2d", "joy_r2d_font_get_size", "FontGetSize", t_joy_r2d_font_get_size, 1, k_joy_r2d_font_get_size, JOY_V_I32 },
        { "r2d", "joy_r2d_font_get_ascent", "FontGetAscent", t_joy_r2d_font_get_ascent, 1, k_joy_r2d_font_get_ascent, JOY_V_I32 },
        { "r2d", "joy_r2d_create", "R2dCreate", t_joy_r2d_create, 1, k_joy_r2d_create, JOY_V_HANDLE },
        { "r2d", "joy_r2d_destroy", "R2dDestroy", t_joy_r2d_destroy, 1, k_joy_r2d_destroy, JOY_V_NULL },
        { "r2d", "joy_r2d_set_color", "R2dSetColor", t_joy_r2d_set_color, 5, k_joy_r2d_set_color, JOY_V_BOOL },
        { "r2d", "joy_r2d_begin", "R2dBegin", t_joy_r2d_begin, 1, k_joy_r2d_begin, JOY_V_NULL },
        { "r2d", "joy_r2d_end", "R2dEnd", t_joy_r2d_end, 1, k_joy_r2d_end, JOY_V_NULL },
        { "r2d", "joy_r2d_set_logical_presentation", "R2dSetLogicalPresentation", t_joy_r2d_set_logical_presentation, 3, k_joy_r2d_set_logical_presentation, JOY_V_BOOL },
        { "r2d", "joy_r2d_set_logical_presentation_mode", "R2dSetLogicalPresentationMode", t_joy_r2d_set_logical_presentation_mode, 4, k_joy_r2d_set_logical_presentation_mode, JOY_V_BOOL },
        { "r2d", "joy_r2d_set_vsync", "R2dSetVsync", t_joy_r2d_set_vsync, 2, k_joy_r2d_set_vsync, JOY_V_BOOL },
        { "r2d", "joy_r2d_set_scale", "R2dSetScale", t_joy_r2d_set_scale, 3, k_joy_r2d_set_scale, JOY_V_BOOL },
        { "r2d", "joy_r2d_set_colorf", "R2dSetColorf", t_joy_r2d_set_colorf, 5, k_joy_r2d_set_colorf, JOY_V_NULL },
        { "r2d", "joy_r2d_set_blend_mode", "R2dSetBlendMode", t_joy_r2d_set_blend_mode, 2, k_joy_r2d_set_blend_mode, JOY_V_BOOL },
        { "r2d", "joy_r2d_draw_point", "R2dDrawPoint", t_joy_r2d_draw_point, 3, k_joy_r2d_draw_point, JOY_V_BOOL },
        { "r2d", "joy_r2d_set_clip_rect", "R2dSetClipRect", t_joy_r2d_set_clip_rect, 5, k_joy_r2d_set_clip_rect, JOY_V_BOOL },
        { "r2d", "joy_r2d_clear_clip", "R2dClearClip", t_joy_r2d_clear_clip, 1, k_joy_r2d_clear_clip, JOY_V_NULL },
        { "r2d", "joy_r2d_push_clip_rect", "R2dPushClipRect", t_joy_r2d_push_clip_rect, 5, k_joy_r2d_push_clip_rect, JOY_V_BOOL },
        { "r2d", "joy_r2d_pop_clip_rect", "R2dPopClipRect", t_joy_r2d_pop_clip_rect, 1, k_joy_r2d_pop_clip_rect, JOY_V_NULL },
        { "r2d", "joy_r2d_reset_clip_stack", "R2dResetClipStack", t_joy_r2d_reset_clip_stack, 1, k_joy_r2d_reset_clip_stack, JOY_V_NULL },
        { "r2d", "joy_r2d_point_in_clip", "R2dPointInClip", t_joy_r2d_point_in_clip, 3, k_joy_r2d_point_in_clip, JOY_V_BOOL },
        { "shape", "shape_draw_line", "ShapeDrawLine", t_shape_draw_line, 5, k_shape_draw_line, JOY_V_BOOL },
        { "shape", "shape_draw_rectangle", "ShapeDrawRectangle", t_shape_draw_rectangle, 6, k_shape_draw_rectangle, JOY_V_BOOL },
        { "ui", "joy_ui_image_create", "ImageCreate", t_joy_ui_image_create, 2, k_joy_ui_image_create, JOY_V_HANDLE },
        { "ui", "joy_ui_image_destroy", "ImageDestroy", t_joy_ui_image_destroy, 1, k_joy_ui_image_destroy, JOY_V_NULL },
        { "ui", "joy_ui_image_set_alpha", "ImageSetAlpha", t_joy_ui_image_set_alpha, 2, k_joy_ui_image_set_alpha, JOY_V_NULL },
        { "ui", "joy_ui_image_set_blend_mode", "ImageSetBlendMode", t_joy_ui_image_set_blend_mode, 2, k_joy_ui_image_set_blend_mode, JOY_V_NULL },
        { "ui", "joy_ui_image_set_scale_mode", "ImageSetScaleMode", t_joy_ui_image_set_scale_mode, 2, k_joy_ui_image_set_scale_mode, JOY_V_NULL },
        { "ui", "joy_ui_text_updatex", "TextUpdatex", t_joy_ui_text_updatex, 8, k_joy_ui_text_updatex, JOY_V_NULL },
        { "ui", "joy_ui_text_destroy", "TextDestroy", t_joy_ui_text_destroy, 1, k_joy_ui_text_destroy, JOY_V_NULL },
        { "r2d", "joy_r2d_jtext_print", "JtextPrint", t_joy_r2d_jtext_print, 4, k_joy_r2d_jtext_print, JOY_V_NULL },
        { "r2d", "joy_r2d_jtext_printx", "JtextPrintx", t_joy_r2d_jtext_printx, 10, k_joy_r2d_jtext_printx, JOY_V_NULL },
        { "r2d", "joy_r2d_draw_point_f", "R2dDrawPointF", t_joy_r2d_draw_point_f, 3, k_joy_r2d_draw_point_f, JOY_V_BOOL },
        { "shape", "shape_draw_rectangle_f", "ShapeDrawRectangleF", t_shape_draw_rectangle_f, 6, k_shape_draw_rectangle_f, JOY_V_BOOL },
        { "shape", "shape_draw_line_f", "ShapeDrawLineF", t_shape_draw_line_f, 5, k_shape_draw_line_f, JOY_V_BOOL },
        { "r2d", "joy_r2d_jtext_printf", "JtextPrintf", t_joy_r2d_jtext_printf, 10, k_joy_r2d_jtext_printf, JOY_V_NULL },
        { "r2d", "joy_r2d_jtext_printf_scaled", "JtextPrintfScaled", t_joy_r2d_jtext_printf_scaled, 11, k_joy_r2d_jtext_printf_scaled, JOY_V_NULL },
        { "ui", "joy_ui_text_get_width", "TextGetWidth", t_joy_ui_text_get_width, 1, k_joy_ui_text_get_width, JOY_V_I32 },
        { "ui", "joy_ui_text_get_height", "TextGetHeight", t_joy_ui_text_get_height, 1, k_joy_ui_text_get_height, JOY_V_I32 },
        { "r2d", "joy_r2d_jtext_cache_clear", "JtextCacheClear", t_joy_r2d_jtext_cache_clear, 0, NULL, JOY_V_NULL },
        { "r2d", "joy_r2d_jtext_cache_set_limit", "JtextCacheSetLimit", t_joy_r2d_jtext_cache_set_limit, 1, k_joy_r2d_jtext_cache_set_limit, JOY_V_NULL },
        { "r2d", "joy_r2d_jtext_cache_get_count", "JtextCacheGetCount", t_joy_r2d_jtext_cache_get_count, 0, NULL, JOY_V_I32 },
        { "r2d", "joy_r2d_jtext_cache_sweep", "JtextCacheSweep", t_joy_r2d_jtext_cache_sweep, 1, k_joy_r2d_jtext_cache_sweep, JOY_V_NULL },
        { "animation", "animation_destroy", "AnimationDestroy", t_animation_destroy, 1, k_animation_destroy, JOY_V_NULL },
        { "animation", "animation_reset", "AnimationReset", t_animation_reset, 1, k_animation_reset, JOY_V_NULL },
        { "animation", "animation_add_clip", "AnimationAddClip", t_animation_add_clip, 7, k_animation_add_clip, JOY_V_NULL },
        { "animation", "animation_is_finished", "AnimationIsFinished", t_animation_is_finished, 1, k_animation_is_finished, JOY_V_BOOL },
        { "animation", "animation_set_position", "AnimationSetPosition", t_animation_set_position, 3, k_animation_set_position, JOY_V_NULL },
        { "animation", "animation_set_scale", "AnimationSetScale", t_animation_set_scale, 3, k_animation_set_scale, JOY_V_NULL },
        { "animation", "animation_set_rotation", "AnimationSetRotation", t_animation_set_rotation, 2, k_animation_set_rotation, JOY_V_NULL },
        { "animation", "animation_update", "AnimationUpdate", t_animation_update, 2, k_animation_update, JOY_V_NULL },
        { "r3d", "joy_r3d_destroy", "R3dDestroy", t_joy_r3d_destroy, 1, k_joy_r3d_destroy, JOY_V_NULL },
        { "r3d", "joy_r3d_begin", "R3dBegin", t_joy_r3d_begin, 1, k_joy_r3d_begin, JOY_V_BOOL },
        { "r3d", "joy_r3d_end", "R3dEnd", t_joy_r3d_end, 1, k_joy_r3d_end, JOY_V_NULL },
        { "r3d", "joy_r3d_get_draw_calls", "R3dGetDrawCalls", t_joy_r3d_get_draw_calls, 1, k_joy_r3d_get_draw_calls, JOY_V_I32 },
        { "r3d", "joy_r3d_set_clear_color", "R3dSetClearColor", t_joy_r3d_set_clear_color, 4, k_joy_r3d_set_clear_color, JOY_V_NULL },
        { "r3d", "joy_r3d_mesh_destroy", "R3dMeshDestroy", t_joy_r3d_mesh_destroy, 2, k_joy_r3d_mesh_destroy, JOY_V_NULL },
        { "r3d", "joy_r3d_font_init", "R3dFontInit", t_joy_r3d_font_init, 3, k_joy_r3d_font_init, JOY_V_BOOL },
        { "r3d", "joy_r3d_font_draw_2d", "R3dFontDraw2d", t_joy_r3d_font_draw_2d, 8, k_joy_r3d_font_draw_2d, JOY_V_BOOL },
        { "r3d", "joy_r3d_font_cache_set_limit", "R3dFontCacheSetLimit", t_joy_r3d_font_cache_set_limit, 2, k_joy_r3d_font_cache_set_limit, JOY_V_NULL },
        { "r3d", "joy_r3d_font_cache_get_count", "R3dFontCacheGetCount", t_joy_r3d_font_cache_get_count, 1, k_joy_r3d_font_cache_get_count, JOY_V_I32 },
        { "r3d", "joy_r3d_font_cache_clear", "R3dFontCacheClear", t_joy_r3d_font_cache_clear, 1, k_joy_r3d_font_cache_clear, JOY_V_NULL },
        { "r3d", "joy_r3d_font_cache_sweep", "R3dFontCacheSweep", t_joy_r3d_font_cache_sweep, 2, k_joy_r3d_font_cache_sweep, JOY_V_NULL },
        { "r3d", "joy_r3d_set_logical_presentation", "R3dSetLogicalPresentation", t_joy_r3d_set_logical_presentation, 3, k_joy_r3d_set_logical_presentation, JOY_V_NULL },
        { "r3d", "joy_r3d_set_logical_presentation_mode", "R3dSetLogicalPresentationMode", t_joy_r3d_set_logical_presentation_mode, 4, k_joy_r3d_set_logical_presentation_mode, JOY_V_NULL },
        { "r3d", "joy_r3d_button_draw", "R3dButtonDraw", t_joy_r3d_button_draw, 5, k_joy_r3d_button_draw, JOY_V_NULL },
        { "r3d", "joy_r3d_button_draw_circle", "R3dButtonDrawCircle", t_joy_r3d_button_draw_circle, 5, k_joy_r3d_button_draw_circle, JOY_V_NULL },
        { "r3d", "joy_r3d_button_is_hovered", "R3dButtonIsHovered", t_joy_r3d_button_is_hovered, 4, k_joy_r3d_button_is_hovered, JOY_V_BOOL },
        { "r3d", "joy_r3d_button_is_pressed", "R3dButtonIsPressed", t_joy_r3d_button_is_pressed, 5, k_joy_r3d_button_is_pressed, JOY_V_BOOL },
        { "r3d", "joy_r3d_button_release", "R3dButtonRelease", t_joy_r3d_button_release, 4, k_joy_r3d_button_release, JOY_V_NULL },
        { "r3d", "joy_r3d_button_destroy", "R3dButtonDestroy", t_joy_r3d_button_destroy, 1, k_joy_r3d_button_destroy, JOY_V_NULL },
        { "r3d", "joy_r3d_label_set_text", "R3dLabelSetText", t_joy_r3d_label_set_text, 2, k_joy_r3d_label_set_text, JOY_V_NULL },
        { "r3d", "joy_r3d_label_draw", "R3dLabelDraw", t_joy_r3d_label_draw, 5, k_joy_r3d_label_draw, JOY_V_NULL },
        { "r3d", "joy_r3d_label_destroy", "R3dLabelDestroy", t_joy_r3d_label_destroy, 1, k_joy_r3d_label_destroy, JOY_V_NULL },
        { "r3d", "joy_r3d_input_set_text", "R3dInputSetText", t_joy_r3d_input_set_text, 2, k_joy_r3d_input_set_text, JOY_V_NULL },
        { "r3d", "joy_r3d_input_is_hovered", "R3dInputIsHovered", t_joy_r3d_input_is_hovered, 4, k_joy_r3d_input_is_hovered, JOY_V_BOOL },
        { "r3d", "joy_r3d_input_focus", "R3dInputFocus", t_joy_r3d_input_focus, 1, k_joy_r3d_input_focus, JOY_V_NULL },
        { "r3d", "joy_r3d_input_defocus", "R3dInputDefocus", t_joy_r3d_input_defocus, 1, k_joy_r3d_input_defocus, JOY_V_NULL },
        { "r3d", "joy_r3d_input_is_focused", "R3dInputIsFocused", t_joy_r3d_input_is_focused, 1, k_joy_r3d_input_is_focused, JOY_V_BOOL },
        { "r3d", "joy_r3d_input_start_text_input", "R3dInputStartTextInput", t_joy_r3d_input_start_text_input, 2, k_joy_r3d_input_start_text_input, JOY_V_NULL },
        { "r3d", "joy_r3d_input_stop_text_input", "R3dInputStopTextInput", t_joy_r3d_input_stop_text_input, 2, k_joy_r3d_input_stop_text_input, JOY_V_NULL },
        { "r3d", "joy_r3d_input_insert", "R3dInputInsert", t_joy_r3d_input_insert, 2, k_joy_r3d_input_insert, JOY_V_NULL },
        { "r3d", "joy_r3d_input_backspace", "R3dInputBackspace", t_joy_r3d_input_backspace, 1, k_joy_r3d_input_backspace, JOY_V_NULL },
        { "r3d", "joy_r3d_input_delete", "R3dInputDelete", t_joy_r3d_input_delete, 1, k_joy_r3d_input_delete, JOY_V_NULL },
        { "r3d", "joy_r3d_input_cursor_left", "R3dInputCursorLeft", t_joy_r3d_input_cursor_left, 1, k_joy_r3d_input_cursor_left, JOY_V_NULL },
        { "r3d", "joy_r3d_input_cursor_right", "R3dInputCursorRight", t_joy_r3d_input_cursor_right, 1, k_joy_r3d_input_cursor_right, JOY_V_NULL },
        { "r3d", "joy_r3d_input_cursor_home", "R3dInputCursorHome", t_joy_r3d_input_cursor_home, 1, k_joy_r3d_input_cursor_home, JOY_V_NULL },
        { "r3d", "joy_r3d_input_cursor_end", "R3dInputCursorEnd", t_joy_r3d_input_cursor_end, 1, k_joy_r3d_input_cursor_end, JOY_V_NULL },
        { "r3d", "joy_r3d_input_click", "R3dInputClick", t_joy_r3d_input_click, 4, k_joy_r3d_input_click, JOY_V_NULL },
        { "r3d", "joy_r3d_input_submit", "R3dInputSubmit", t_joy_r3d_input_submit, 1, k_joy_r3d_input_submit, JOY_V_NULL },
        { "r3d", "joy_r3d_input_draw", "R3dInputDraw", t_joy_r3d_input_draw, 5, k_joy_r3d_input_draw, JOY_V_NULL },
        { "r3d", "joy_r3d_input_destroy", "R3dInputDestroy", t_joy_r3d_input_destroy, 1, k_joy_r3d_input_destroy, JOY_V_NULL },
        { "r3d", "joy_r3d_combobox_input_destroy", "R3dComboboxInputDestroy", t_joy_r3d_combobox_input_destroy, 1, k_joy_r3d_combobox_input_destroy, JOY_V_NULL },
        { "r3d", "joy_r3d_combobox_input_add_item", "R3dComboboxInputAddItem", t_joy_r3d_combobox_input_add_item, 2, k_joy_r3d_combobox_input_add_item, JOY_V_NULL },
        { "r3d", "joy_r3d_combobox_input_clear_items", "R3dComboboxInputClearItems", t_joy_r3d_combobox_input_clear_items, 1, k_joy_r3d_combobox_input_clear_items, JOY_V_NULL },
        { "r3d", "joy_r3d_combobox_input_set_text", "R3dComboboxInputSetText", t_joy_r3d_combobox_input_set_text, 2, k_joy_r3d_combobox_input_set_text, JOY_V_NULL },
        { "r3d", "joy_r3d_combobox_input_hit_item", "R3dComboboxInputHitItem", t_joy_r3d_combobox_input_hit_item, 3, k_joy_r3d_combobox_input_hit_item, JOY_V_I32 },
        { "r3d", "joy_r3d_combobox_input_draw", "R3dComboboxInputDraw", t_joy_r3d_combobox_input_draw, 5, k_joy_r3d_combobox_input_draw, JOY_V_NULL },
        { "r3d", "joy_r3d_combobox_input_draw_dropdown", "R3dComboboxInputDrawDropdown", t_joy_r3d_combobox_input_draw_dropdown, 5, k_joy_r3d_combobox_input_draw_dropdown, JOY_V_NULL },
        { "r3d", "joy_r3d_slider_destroy", "R3dSliderDestroy", t_joy_r3d_slider_destroy, 1, k_joy_r3d_slider_destroy, JOY_V_NULL },
        { "r3d", "joy_r3d_slider_set_value", "R3dSliderSetValue", t_joy_r3d_slider_set_value, 2, k_joy_r3d_slider_set_value, JOY_V_NULL },
        { "r3d", "joy_r3d_slider_get_value", "R3dSliderGetValue", t_joy_r3d_slider_get_value, 1, k_joy_r3d_slider_get_value, JOY_V_F64 },
        { "r3d", "joy_r3d_slider_draw", "R3dSliderDraw", t_joy_r3d_slider_draw, 5, k_joy_r3d_slider_draw, JOY_V_NULL },
        { "r3d", "joy_r3d_slider_is_pressed", "R3dSliderIsPressed", t_joy_r3d_slider_is_pressed, 4, k_joy_r3d_slider_is_pressed, JOY_V_BOOL },
        { "r3d", "joy_r3d_slider_set_pressed", "R3dSliderSetPressed", t_joy_r3d_slider_set_pressed, 2, k_joy_r3d_slider_set_pressed, JOY_V_NULL },
        { "r3d", "joy_r3d_slider_drag_move", "R3dSliderDragMove", t_joy_r3d_slider_drag_move, 4, k_joy_r3d_slider_drag_move, JOY_V_NULL },
        { "r3d", "joy_r3d_slider_is_hovered", "R3dSliderIsHovered", t_joy_r3d_slider_is_hovered, 4, k_joy_r3d_slider_is_hovered, JOY_V_BOOL },
        { "r3d", "joy_r3d_slider_set_hovered", "R3dSliderSetHovered", t_joy_r3d_slider_set_hovered, 2, k_joy_r3d_slider_set_hovered, JOY_V_NULL },
        { "r3d", "joy_r3d_checkbox_destroy", "R3dCheckboxDestroy", t_joy_r3d_checkbox_destroy, 1, k_joy_r3d_checkbox_destroy, JOY_V_NULL },
        { "r3d", "joy_r3d_checkbox_toggle", "R3dCheckboxToggle", t_joy_r3d_checkbox_toggle, 1, k_joy_r3d_checkbox_toggle, JOY_V_NULL },
        { "r3d", "joy_r3d_checkbox_is_pressed", "R3dCheckboxIsPressed", t_joy_r3d_checkbox_is_pressed, 4, k_joy_r3d_checkbox_is_pressed, JOY_V_BOOL },
        { "r3d", "joy_r3d_checkbox_set_hovered", "R3dCheckboxSetHovered", t_joy_r3d_checkbox_set_hovered, 2, k_joy_r3d_checkbox_set_hovered, JOY_V_NULL },
        { "r3d", "joy_r3d_checkbox_is_hovered", "R3dCheckboxIsHovered", t_joy_r3d_checkbox_is_hovered, 4, k_joy_r3d_checkbox_is_hovered, JOY_V_BOOL },
        { "r3d", "joy_r3d_checkbox_set_colors", "R3dCheckboxSetColors", t_joy_r3d_checkbox_set_colors, 13, k_joy_r3d_checkbox_set_colors, JOY_V_NULL },
        { "r3d", "joy_r3d_checkbox_draw", "R3dCheckboxDraw", t_joy_r3d_checkbox_draw, 5, k_joy_r3d_checkbox_draw, JOY_V_NULL },
        { "r3d", "joy_r3d_draw_grid", "R3dDrawGrid", t_joy_r3d_draw_grid, 4, k_joy_r3d_draw_grid, JOY_V_NULL },
        { "janimator", "janimator_destroy", "JanimatorDestroy", t_janimator_destroy, 1, k_janimator_destroy, JOY_V_NULL },
        { "janimator", "janimator_set_float", "JanimatorSetFloat", t_janimator_set_float, 3, k_janimator_set_float, JOY_V_NULL },
        { "janimator", "janimator_set_int", "JanimatorSetInt", t_janimator_set_int, 3, k_janimator_set_int, JOY_V_NULL },
        { "janimator", "janimator_set_bool", "JanimatorSetBool", t_janimator_set_bool, 3, k_janimator_set_bool, JOY_V_NULL },
        { "janimator", "janimator_set_trigger", "JanimatorSetTrigger", t_janimator_set_trigger, 2, k_janimator_set_trigger, JOY_V_NULL },
        { "janimator", "janimator_get_float", "JanimatorGetFloat", t_janimator_get_float, 2, k_janimator_get_float, JOY_V_F64 },
        { "janimator", "janimator_get_int", "JanimatorGetInt", t_janimator_get_int, 2, k_janimator_get_int, JOY_V_I32 },
        { "janimator", "janimator_get_bool", "JanimatorGetBool", t_janimator_get_bool, 2, k_janimator_get_bool, JOY_V_BOOL },
        { "janimator", "janimator_add_state", "JanimatorAddState", t_janimator_add_state, 2, k_janimator_add_state, JOY_V_NULL },
        { "janimator", "janimator_set_default_state", "JanimatorSetDefaultState", t_janimator_set_default_state, 2, k_janimator_set_default_state, JOY_V_NULL },
        { "janimator", "janimator_state_add_clip", "JanimatorStateAddClip", t_janimator_state_add_clip, 8, k_janimator_state_add_clip, JOY_V_NULL },
        { "janimator", "janimator_state_add_color", "JanimatorStateAddColor", t_janimator_state_add_color, 9, k_janimator_state_add_color, JOY_V_NULL },
        { "janimator", "janimator_state_set_loop", "JanimatorStateSetLoop", t_janimator_state_set_loop, 3, k_janimator_state_set_loop, JOY_V_NULL },
        { "janimator", "janimator_state_set_speed", "JanimatorStateSetSpeed", t_janimator_state_set_speed, 3, k_janimator_state_set_speed, JOY_V_NULL },
        { "janimator", "janimator_state_set_blend", "JanimatorStateSetBlend", t_janimator_state_set_blend, 3, k_janimator_state_set_blend, JOY_V_NULL },
        { "janimator", "janimator_state_add_motion", "JanimatorStateAddMotion", t_janimator_state_add_motion, 9, k_janimator_state_add_motion, JOY_V_NULL },
        { "janimator", "janimator_add_transition", "JanimatorAddTransition", t_janimator_add_transition, 3, k_janimator_add_transition, JOY_V_I32 },
        { "janimator", "janimator_add_any_transition", "JanimatorAddAnyTransition", t_janimator_add_any_transition, 2, k_janimator_add_any_transition, JOY_V_I32 },
        { "janimator", "janimator_transition_add_cond", "JanimatorTransitionAddCond", t_janimator_transition_add_cond, 5, k_janimator_transition_add_cond, JOY_V_NULL },
        { "janimator", "janimator_transition_set_exit_time", "JanimatorTransitionSetExitTime", t_janimator_transition_set_exit_time, 3, k_janimator_transition_set_exit_time, JOY_V_NULL },
        { "janimator", "janimator_update", "JanimatorUpdate", t_janimator_update, 2, k_janimator_update, JOY_V_NULL },
        { "janimator", "janimator_draw", "JanimatorDraw", t_janimator_draw, 3, k_janimator_draw, JOY_V_NULL },
        { "janimator", "janimator_set_flip_h", "JanimatorSetFlipH", t_janimator_set_flip_h, 2, k_janimator_set_flip_h, JOY_V_NULL },
        { "ui", "joy_ui_label_destroy", "LabelDestroy", t_joy_ui_label_destroy, 1, k_joy_ui_label_destroy, JOY_V_NULL },
        { "ui", "joy_ui_label_draw", "LabelDraw", t_joy_ui_label_draw, 1, k_joy_ui_label_draw, JOY_V_NULL },
        { "ui", "joy_ui_button_destroy", "ButtonDestroy", t_joy_ui_button_destroy, 1, k_joy_ui_button_destroy, JOY_V_NULL },
        { "ui", "joy_ui_button_set_position", "ButtonSetPosition", t_joy_ui_button_set_position, 3, k_joy_ui_button_set_position, JOY_V_NULL },
        { "ui", "joy_ui_button_set_size", "ButtonSetSize", t_joy_ui_button_set_size, 3, k_joy_ui_button_set_size, JOY_V_NULL },
        { "ui", "joy_ui_button_set_normal_color", "ButtonSetNormalColor", t_joy_ui_button_set_normal_color, 5, k_joy_ui_button_set_normal_color, JOY_V_NULL },
        { "ui", "joy_ui_button_set_hover_color", "ButtonSetHoverColor", t_joy_ui_button_set_hover_color, 5, k_joy_ui_button_set_hover_color, JOY_V_NULL },
        { "ui", "joy_ui_button_set_pressed_color", "ButtonSetPressedColor", t_joy_ui_button_set_pressed_color, 5, k_joy_ui_button_set_pressed_color, JOY_V_NULL },
        { "ui", "joy_ui_button_set_image", "ButtonSetImage", t_joy_ui_button_set_image, 2, k_joy_ui_button_set_image, JOY_V_NULL },
        { "ui", "joy_ui_button_set_pressed_image", "ButtonSetPressedImage", t_joy_ui_button_set_pressed_image, 2, k_joy_ui_button_set_pressed_image, JOY_V_NULL },
        { "ui", "joy_ui_button_set_textx", "ButtonSetTextx", t_joy_ui_button_set_textx, 8, k_joy_ui_button_set_textx, JOY_V_NULL },
        { "ui", "joy_ui_button_is_hovered", "ButtonIsHovered", t_joy_ui_button_is_hovered, 3, k_joy_ui_button_is_hovered, JOY_V_BOOL },
        { "ui", "joy_ui_button_is_pressed", "ButtonIsPressed", t_joy_ui_button_is_pressed, 3, k_joy_ui_button_is_pressed, JOY_V_BOOL },
        { "ui", "joy_ui_button_set_hovered", "ButtonSetHovered", t_joy_ui_button_set_hovered, 2, k_joy_ui_button_set_hovered, JOY_V_NULL },
        { "ui", "joy_ui_button_set_pressed", "ButtonSetPressed", t_joy_ui_button_set_pressed, 2, k_joy_ui_button_set_pressed, JOY_V_NULL },
        { "ui", "joy_ui_button_get_pressed", "ButtonGetPressed", t_joy_ui_button_get_pressed, 1, k_joy_ui_button_get_pressed, JOY_V_BOOL },
        { "ui", "joy_ui_button_set_text_offset", "ButtonSetTextOffset", t_joy_ui_button_set_text_offset, 3, k_joy_ui_button_set_text_offset, JOY_V_NULL },
        { "ui", "joy_ui_button_draw", "ButtonDraw", t_joy_ui_button_draw, 1, k_joy_ui_button_draw, JOY_V_NULL },
        { "ui", "joy_ui_scrollview_destroy", "ScrollviewDestroy", t_joy_ui_scrollview_destroy, 1, k_joy_ui_scrollview_destroy, JOY_V_NULL },
        { "ui", "joy_ui_scrollview_set_view_px", "ScrollviewSetViewPx", t_joy_ui_scrollview_set_view_px, 5, k_joy_ui_scrollview_set_view_px, JOY_V_NULL },
        { "ui", "joy_ui_scrollview_set_content", "ScrollviewSetContent", t_joy_ui_scrollview_set_content, 3, k_joy_ui_scrollview_set_content, JOY_V_NULL },
        { "ui", "joy_ui_scrollview_set_offset", "ScrollviewSetOffset", t_joy_ui_scrollview_set_offset, 3, k_joy_ui_scrollview_set_offset, JOY_V_NULL },
        { "ui", "joy_ui_scrollview_clamp", "ScrollviewClamp", t_joy_ui_scrollview_clamp, 1, k_joy_ui_scrollview_clamp, JOY_V_NULL },
        { "ui", "joy_ui_scrollview_get_offset_x", "ScrollviewGetOffsetX", t_joy_ui_scrollview_get_offset_x, 1, k_joy_ui_scrollview_get_offset_x, JOY_V_F64 },
        { "ui", "joy_ui_scrollview_get_offset_y", "ScrollviewGetOffsetY", t_joy_ui_scrollview_get_offset_y, 1, k_joy_ui_scrollview_get_offset_y, JOY_V_F64 },
        { "ui", "joy_ui_scrollview_set_show_bar", "ScrollviewSetShowBar", t_joy_ui_scrollview_set_show_bar, 2, k_joy_ui_scrollview_set_show_bar, JOY_V_NULL },
        { "ui", "joy_ui_scrollview_dragging", "ScrollviewDragging", t_joy_ui_scrollview_dragging, 1, k_joy_ui_scrollview_dragging, JOY_V_BOOL },
        { "ui", "joy_ui_scrollview_hit", "ScrollviewHit", t_joy_ui_scrollview_hit, 3, k_joy_ui_scrollview_hit, JOY_V_BOOL },
        { "ui", "joy_ui_scrollview_begin", "ScrollviewBegin", t_joy_ui_scrollview_begin, 1, k_joy_ui_scrollview_begin, JOY_V_NULL },
        { "ui", "joy_ui_scrollview_end", "ScrollviewEnd", t_joy_ui_scrollview_end, 1, k_joy_ui_scrollview_end, JOY_V_NULL },
        { "ui", "joy_ui_combobox_destroy", "ComboboxDestroy", t_joy_ui_combobox_destroy, 1, k_joy_ui_combobox_destroy, JOY_V_NULL },
        { "ui", "joy_ui_combobox_add_item", "ComboboxAddItem", t_joy_ui_combobox_add_item, 8, k_joy_ui_combobox_add_item, JOY_V_NULL },
        { "ui", "joy_ui_combobox_is_pressed", "ComboboxIsPressed", t_joy_ui_combobox_is_pressed, 3, k_joy_ui_combobox_is_pressed, JOY_V_BOOL },
        { "ui", "joy_ui_combobox_is_open", "ComboboxIsOpen", t_joy_ui_combobox_is_open, 1, k_joy_ui_combobox_is_open, JOY_V_BOOL },
        { "ui", "joy_ui_combobox_set_open", "ComboboxSetOpen", t_joy_ui_combobox_set_open, 2, k_joy_ui_combobox_set_open, JOY_V_NULL },
        { "ui", "joy_ui_combobox_hit_item", "ComboboxHitItem", t_joy_ui_combobox_hit_item, 3, k_joy_ui_combobox_hit_item, JOY_V_I32 },
        { "ui", "joy_ui_combobox_set_selected", "ComboboxSetSelected", t_joy_ui_combobox_set_selected, 2, k_joy_ui_combobox_set_selected, JOY_V_NULL },
        { "ui", "joy_ui_combobox_get_selected", "ComboboxGetSelected", t_joy_ui_combobox_get_selected, 1, k_joy_ui_combobox_get_selected, JOY_V_I32 },
        { "ui", "joy_ui_combobox_draw", "ComboboxDraw", t_joy_ui_combobox_draw, 1, k_joy_ui_combobox_draw, JOY_V_NULL },
        { "ui", "joy_ui_combobox_input_destroy", "ComboboxInputDestroy", t_joy_ui_combobox_input_destroy, 1, k_joy_ui_combobox_input_destroy, JOY_V_NULL },
        { "ui", "joy_ui_combobox_input_add_item", "ComboboxInputAddItem", t_joy_ui_combobox_input_add_item, 3, k_joy_ui_combobox_input_add_item, JOY_V_NULL },
        { "ui", "joy_ui_combobox_input_clear_items", "ComboboxInputClearItems", t_joy_ui_combobox_input_clear_items, 1, k_joy_ui_combobox_input_clear_items, JOY_V_NULL },
        { "ui", "joy_ui_combobox_input_set_text", "ComboboxInputSetText", t_joy_ui_combobox_input_set_text, 2, k_joy_ui_combobox_input_set_text, JOY_V_NULL },
        { "ui", "joy_ui_combobox_input_draw", "ComboboxInputDraw", t_joy_ui_combobox_input_draw, 1, k_joy_ui_combobox_input_draw, JOY_V_NULL },
        { "ui", "joy_ui_combobox_input_draw_dropdown", "ComboboxInputDrawDropdown", t_joy_ui_combobox_input_draw_dropdown, 1, k_joy_ui_combobox_input_draw_dropdown, JOY_V_NULL },
        { "ui", "joy_ui_combobox_input_is_open", "ComboboxInputIsOpen", t_joy_ui_combobox_input_is_open, 1, k_joy_ui_combobox_input_is_open, JOY_V_BOOL },
        { "ui", "joy_ui_combobox_input_set_popup_node", "ComboboxInputSetPopupNode", t_joy_ui_combobox_input_set_popup_node, 2, k_joy_ui_combobox_input_set_popup_node, JOY_V_NULL },
        { "ui", "joy_ui_combobox_input_hit_item", "ComboboxInputHitItem", t_joy_ui_combobox_input_hit_item, 3, k_joy_ui_combobox_input_hit_item, JOY_V_I32 },
        { "ui", "joy_ui_combobox_input_set_open", "ComboboxInputSetOpen", t_joy_ui_combobox_input_set_open, 2, k_joy_ui_combobox_input_set_open, JOY_V_NULL },
        { "ui", "joy_ui_datagrid_setheaders", "DatagridSetheaders", t_joy_ui_datagrid_setheaders, 2, k_joy_ui_datagrid_setheaders, JOY_V_NULL },
        { "ui", "joy_ui_datagrid_setheader", "DatagridSetheader", t_joy_ui_datagrid_setheader, 3, k_joy_ui_datagrid_setheader, JOY_V_NULL },
        { "ui", "joy_ui_datagrid_setcell", "DatagridSetcell", t_joy_ui_datagrid_setcell, 4, k_joy_ui_datagrid_setcell, JOY_V_NULL },
        { "ui", "joy_ui_datagrid_destroy", "DatagridDestroy", t_joy_ui_datagrid_destroy, 1, k_joy_ui_datagrid_destroy, JOY_V_NULL },
        { "ui", "joy_ui_datagrid_draw", "DatagridDraw", t_joy_ui_datagrid_draw, 2, k_joy_ui_datagrid_draw, JOY_V_NULL },
        { "ui", "joy_ui_datagrid_hit_row", "DatagridHitRow", t_joy_ui_datagrid_hit_row, 3, k_joy_ui_datagrid_hit_row, JOY_V_I32 },
        { "ui", "joy_ui_datagrid_scroll", "DatagridScroll", t_joy_ui_datagrid_scroll, 2, k_joy_ui_datagrid_scroll, JOY_V_NULL },
        { "ui", "joy_ui_datagrid_set_selected_row", "DatagridSetSelectedRow", t_joy_ui_datagrid_set_selected_row, 2, k_joy_ui_datagrid_set_selected_row, JOY_V_NULL },
        { "ui", "joy_ui_radio_destroy", "RadioDestroy", t_joy_ui_radio_destroy, 1, k_joy_ui_radio_destroy, JOY_V_NULL },
        { "ui", "joy_ui_radio_is_pressed", "RadioIsPressed", t_joy_ui_radio_is_pressed, 3, k_joy_ui_radio_is_pressed, JOY_V_BOOL },
        { "ui", "joy_ui_radio_set_hovered", "RadioSetHovered", t_joy_ui_radio_set_hovered, 2, k_joy_ui_radio_set_hovered, JOY_V_NULL },
        { "ui", "joy_ui_checkbox_destroy", "CheckboxDestroy", t_joy_ui_checkbox_destroy, 1, k_joy_ui_checkbox_destroy, JOY_V_NULL },
        { "ui", "joy_ui_checkbox_set_colors", "CheckboxSetColors", t_joy_ui_checkbox_set_colors, 13, k_joy_ui_checkbox_set_colors, JOY_V_NULL },
        { "ui", "joy_ui_checkbox_set_r2d", "CheckboxSetR2d", t_joy_ui_checkbox_set_r2d, 2, k_joy_ui_checkbox_set_r2d, JOY_V_NULL },
        { "ui", "joy_ui_checkbox_set_position", "CheckboxSetPosition", t_joy_ui_checkbox_set_position, 3, k_joy_ui_checkbox_set_position, JOY_V_NULL },
        { "ui", "joy_ui_checkbox_set_text_color", "CheckboxSetTextColor", t_joy_ui_checkbox_set_text_color, 5, k_joy_ui_checkbox_set_text_color, JOY_V_NULL },
        { "ui", "joy_ui_checkbox_toggle", "CheckboxToggle", t_joy_ui_checkbox_toggle, 1, k_joy_ui_checkbox_toggle, JOY_V_NULL },
        { "ui", "joy_ui_checkbox_is_pressed", "CheckboxIsPressed", t_joy_ui_checkbox_is_pressed, 3, k_joy_ui_checkbox_is_pressed, JOY_V_BOOL },
        { "ui", "joy_ui_checkbox_set_hovered", "CheckboxSetHovered", t_joy_ui_checkbox_set_hovered, 2, k_joy_ui_checkbox_set_hovered, JOY_V_NULL },
        { "input", "joy_input_joystick_destroy", "JoystickDestroy", t_joy_input_joystick_destroy, 1, k_joy_input_joystick_destroy, JOY_V_NULL },
        { "input", "joy_input_joystick_set_textures", "JoystickSetTextures", t_joy_input_joystick_set_textures, 3, k_joy_input_joystick_set_textures, JOY_V_NULL },
        { "input", "joy_input_joystick_set_vector", "JoystickSetVector", t_joy_input_joystick_set_vector, 3, k_joy_input_joystick_set_vector, JOY_V_NULL },
        { "input", "joy_input_joystick_draw", "JoystickDraw", t_joy_input_joystick_draw, 1, k_joy_input_joystick_draw, JOY_V_NULL },
        { "input", "joy_input_joystick_reset", "JoystickReset", t_joy_input_joystick_reset, 1, k_joy_input_joystick_reset, JOY_V_NULL },
        { "input", "joy_input_joystick_set_position", "JoystickSetPosition", t_joy_input_joystick_set_position, 3, k_joy_input_joystick_set_position, JOY_V_NULL },
        { "input", "joy_input_joystick_set_radius", "JoystickSetRadius", t_joy_input_joystick_set_radius, 2, k_joy_input_joystick_set_radius, JOY_V_NULL },
        { "input", "joy_input_joystick_is_dragging", "JoystickIsDragging", t_joy_input_joystick_is_dragging, 1, k_joy_input_joystick_is_dragging, JOY_V_BOOL },
        { "input", "joy_input_joystick_touch_down", "JoystickTouchDown", t_joy_input_joystick_touch_down, 3, k_joy_input_joystick_touch_down, JOY_V_NULL },
        { "input", "joy_input_joystick_touch_move", "JoystickTouchMove", t_joy_input_joystick_touch_move, 3, k_joy_input_joystick_touch_move, JOY_V_NULL },
        { "input", "joy_input_joystick_touch_up", "JoystickTouchUp", t_joy_input_joystick_touch_up, 1, k_joy_input_joystick_touch_up, JOY_V_NULL },
        { "input", "joy_input_joystick_get_magnitude", "JoystickGetMagnitude", t_joy_input_joystick_get_magnitude, 1, k_joy_input_joystick_get_magnitude, JOY_V_F64 },
        { "ui", "joy_ui_inputbox_destroy", "InputboxDestroy", t_joy_ui_inputbox_destroy, 1, k_joy_ui_inputbox_destroy, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_set_text", "InputboxSetText", t_joy_ui_inputbox_set_text, 2, k_joy_ui_inputbox_set_text, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_draw", "InputboxDraw", t_joy_ui_inputbox_draw, 1, k_joy_ui_inputbox_draw, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_is_pressed", "InputboxIsPressed", t_joy_ui_inputbox_is_pressed, 3, k_joy_ui_inputbox_is_pressed, JOY_V_BOOL },
        { "ui", "joy_ui_inputbox_is_hovered", "InputboxIsHovered", t_joy_ui_inputbox_is_hovered, 3, k_joy_ui_inputbox_is_hovered, JOY_V_BOOL },
        { "ui", "joy_ui_inputbox_focus", "InputboxFocus", t_joy_ui_inputbox_focus, 1, k_joy_ui_inputbox_focus, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_defocus", "InputboxDefocus", t_joy_ui_inputbox_defocus, 1, k_joy_ui_inputbox_defocus, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_blur", "InputboxBlur", t_joy_ui_inputbox_blur, 1, k_joy_ui_inputbox_blur, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_insert", "InputboxInsert", t_joy_ui_inputbox_insert, 3, k_joy_ui_inputbox_insert, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_backspace", "InputboxBackspace", t_joy_ui_inputbox_backspace, 1, k_joy_ui_inputbox_backspace, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_delete", "InputboxDelete", t_joy_ui_inputbox_delete, 1, k_joy_ui_inputbox_delete, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_cursor_left", "InputboxCursorLeft", t_joy_ui_inputbox_cursor_left, 1, k_joy_ui_inputbox_cursor_left, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_cursor_right", "InputboxCursorRight", t_joy_ui_inputbox_cursor_right, 1, k_joy_ui_inputbox_cursor_right, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_cursor_home", "InputboxCursorHome", t_joy_ui_inputbox_cursor_home, 1, k_joy_ui_inputbox_cursor_home, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_cursor_end", "InputboxCursorEnd", t_joy_ui_inputbox_cursor_end, 1, k_joy_ui_inputbox_cursor_end, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_select_left", "InputboxSelectLeft", t_joy_ui_inputbox_select_left, 1, k_joy_ui_inputbox_select_left, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_select_right", "InputboxSelectRight", t_joy_ui_inputbox_select_right, 1, k_joy_ui_inputbox_select_right, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_select_home", "InputboxSelectHome", t_joy_ui_inputbox_select_home, 1, k_joy_ui_inputbox_select_home, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_select_end", "InputboxSelectEnd", t_joy_ui_inputbox_select_end, 1, k_joy_ui_inputbox_select_end, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_select_all", "InputboxSelectAll", t_joy_ui_inputbox_select_all, 1, k_joy_ui_inputbox_select_all, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_paste", "InputboxPaste", t_joy_ui_inputbox_paste, 1, k_joy_ui_inputbox_paste, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_copy", "InputboxCopy", t_joy_ui_inputbox_copy, 1, k_joy_ui_inputbox_copy, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_cut", "InputboxCut", t_joy_ui_inputbox_cut, 1, k_joy_ui_inputbox_cut, JOY_V_NULL },
        { "ui", "joy_ui_inputbox_is_focused", "InputboxIsFocused", t_joy_ui_inputbox_is_focused, 1, k_joy_ui_inputbox_is_focused, JOY_V_BOOL },
        { "ui", "joy_ui_slider_destroy", "SliderDestroy", t_joy_ui_slider_destroy, 1, k_joy_ui_slider_destroy, JOY_V_NULL },
        { "ui", "joy_ui_slider_set_value", "SliderSetValue", t_joy_ui_slider_set_value, 2, k_joy_ui_slider_set_value, JOY_V_NULL },
        { "ui", "joy_ui_slider_get_value", "SliderGetValue", t_joy_ui_slider_get_value, 1, k_joy_ui_slider_get_value, JOY_V_F64 },
        { "ui", "joy_ui_slider_set_rect", "SliderSetRect", t_joy_ui_slider_set_rect, 5, k_joy_ui_slider_set_rect, JOY_V_NULL },
        { "ui", "joy_ui_slider_set_range", "SliderSetRange", t_joy_ui_slider_set_range, 3, k_joy_ui_slider_set_range, JOY_V_NULL },
        { "ui", "joy_ui_slider_draw", "SliderDraw", t_joy_ui_slider_draw, 1, k_joy_ui_slider_draw, JOY_V_NULL },
        { "ui", "joy_ui_slider_is_pressed", "SliderIsPressed", t_joy_ui_slider_is_pressed, 3, k_joy_ui_slider_is_pressed, JOY_V_BOOL },
        { "ui", "joy_ui_slider_set_pressed", "SliderSetPressed", t_joy_ui_slider_set_pressed, 2, k_joy_ui_slider_set_pressed, JOY_V_NULL },
        { "ui", "joy_ui_slider_drag_move", "SliderDragMove", t_joy_ui_slider_drag_move, 3, k_joy_ui_slider_drag_move, JOY_V_NULL },
        { "ui", "joy_ui_slider_is_hovered", "SliderIsHovered", t_joy_ui_slider_is_hovered, 3, k_joy_ui_slider_is_hovered, JOY_V_BOOL },
        { "ui", "joy_ui_slider_set_hovered", "SliderSetHovered", t_joy_ui_slider_set_hovered, 2, k_joy_ui_slider_set_hovered, JOY_V_NULL },
        { "ui", "joy_ui_popup_destroy", "PopupDestroy", t_joy_ui_popup_destroy, 1, k_joy_ui_popup_destroy, JOY_V_NULL },
        { "ui", "joy_ui_popup_set_title", "PopupSetTitle", t_joy_ui_popup_set_title, 3, k_joy_ui_popup_set_title, JOY_V_NULL },
        { "ui", "joy_ui_popup_set_content", "PopupSetContent", t_joy_ui_popup_set_content, 3, k_joy_ui_popup_set_content, JOY_V_NULL },
        { "ui", "joy_ui_popup_set_position", "PopupSetPosition", t_joy_ui_popup_set_position, 3, k_joy_ui_popup_set_position, JOY_V_NULL },
        { "ui", "joy_ui_popup_set_visible", "PopupSetVisible", t_joy_ui_popup_set_visible, 2, k_joy_ui_popup_set_visible, JOY_V_NULL },
        { "ui", "joy_ui_popup_set_closeable", "PopupSetCloseable", t_joy_ui_popup_set_closeable, 2, k_joy_ui_popup_set_closeable, JOY_V_NULL },
        { "ui", "joy_ui_popup_draw", "PopupDraw", t_joy_ui_popup_draw, 3, k_joy_ui_popup_draw, JOY_V_NULL },
        { "ui", "joy_ui_popup_is_close_clicked", "PopupIsCloseClicked", t_joy_ui_popup_is_close_clicked, 3, k_joy_ui_popup_is_close_clicked, JOY_V_BOOL },
        { "ui", "joy_ui_popup_is_title_clicked", "PopupIsTitleClicked", t_joy_ui_popup_is_title_clicked, 3, k_joy_ui_popup_is_title_clicked, JOY_V_BOOL },
        { "ui", "joy_ui_popup_drag_move", "PopupDragMove", t_joy_ui_popup_drag_move, 3, k_joy_ui_popup_drag_move, JOY_V_NULL },
        { "ui", "joy_ui_popup_is_inside", "PopupIsInside", t_joy_ui_popup_is_inside, 3, k_joy_ui_popup_is_inside, JOY_V_BOOL },
        { "ui", "joy_ui_tooltip_destroy", "TooltipDestroy", t_joy_ui_tooltip_destroy, 1, k_joy_ui_tooltip_destroy, JOY_V_NULL },
        { "ui", "joy_ui_tooltip_set_text", "TooltipSetText", t_joy_ui_tooltip_set_text, 3, k_joy_ui_tooltip_set_text, JOY_V_NULL },
        { "ui", "joy_ui_tooltip_set_position", "TooltipSetPosition", t_joy_ui_tooltip_set_position, 3, k_joy_ui_tooltip_set_position, JOY_V_NULL },
        { "ui", "joy_ui_tooltip_set_visible", "TooltipSetVisible", t_joy_ui_tooltip_set_visible, 2, k_joy_ui_tooltip_set_visible, JOY_V_NULL },
        { "ui", "joy_ui_tooltip_draw", "TooltipDraw", t_joy_ui_tooltip_draw, 2, k_joy_ui_tooltip_draw, JOY_V_NULL },
        { "ui", "joy_ui_ctx_destroy", "JuiCtxDestroy", t_joy_ui_ctx_destroy, 1, k_joy_ui_ctx_destroy, JOY_V_NULL },
        { "ui", "joy_ui_begin", "JuiBegin", t_joy_ui_begin, 1, k_joy_ui_begin, JOY_V_NULL },
        { "ui", "joy_ui_end", "JuiEnd", t_joy_ui_end, 1, k_joy_ui_end, JOY_V_NULL },
        { "ui", "joy_ui_input_motion", "JuiInputMotion", t_joy_ui_input_motion, 3, k_joy_ui_input_motion, JOY_V_NULL },
        { "ui", "joy_ui_input_down", "JuiInputDown", t_joy_ui_input_down, 2, k_joy_ui_input_down, JOY_V_NULL },
        { "ui", "joy_ui_input_up", "JuiInputUp", t_joy_ui_input_up, 2, k_joy_ui_input_up, JOY_V_NULL },
        { "ui", "joy_ui_input_wheel", "JuiInputWheel", t_joy_ui_input_wheel, 2, k_joy_ui_input_wheel, JOY_V_NULL },
        { "ui", "joy_ui_input_key", "JuiInputKey", t_joy_ui_input_key, 3, k_joy_ui_input_key, JOY_V_NULL },
        { "ui", "joy_ui_is_down", "JuiIsDown", t_joy_ui_is_down, 1, k_joy_ui_is_down, JOY_V_BOOL },
        { "ui", "joy_ui_is_pressed", "JuiIsPressed", t_joy_ui_is_pressed, 1, k_joy_ui_is_pressed, JOY_V_BOOL },
        { "ui", "joy_ui_is_released", "JuiIsReleased", t_joy_ui_is_released, 1, k_joy_ui_is_released, JOY_V_BOOL },
        { "ui", "joy_ui_take_wheel", "JuiTakeWheel", t_joy_ui_take_wheel, 1, k_joy_ui_take_wheel, JOY_V_F64 },
        { "ui", "joy_ui_is_shift", "JuiIsShift", t_joy_ui_is_shift, 1, k_joy_ui_is_shift, JOY_V_BOOL },
        { "ui", "joy_ui_is_ctrl", "JuiIsCtrl", t_joy_ui_is_ctrl, 1, k_joy_ui_is_ctrl, JOY_V_BOOL },
        { "ui", "joy_ui_push_clip", "JuiPushClip", t_joy_ui_push_clip, 5, k_joy_ui_push_clip, JOY_V_NULL },
        { "ui", "joy_ui_pop_clip", "JuiPopClip", t_joy_ui_pop_clip, 1, k_joy_ui_pop_clip, JOY_V_NULL },
        { "ui", "joy_ui_reset_clip", "JuiResetClip", t_joy_ui_reset_clip, 1, k_joy_ui_reset_clip, JOY_V_NULL },
        { "ui", "joy_ui_hit", "JuiHit", t_joy_ui_hit, 5, k_joy_ui_hit, JOY_V_BOOL },
        { "ui", "joy_ui_fill_rect", "JuiFillRect", t_joy_ui_fill_rect, 9, k_joy_ui_fill_rect, JOY_V_NULL },
        { "ui", "joy_ui_stroke_rect", "JuiStrokeRect", t_joy_ui_stroke_rect, 9, k_joy_ui_stroke_rect, JOY_V_NULL },
        { "ui", "joy_ui_line", "JuiLine", t_joy_ui_line, 9, k_joy_ui_line, JOY_V_NULL },
        { "ui", "joy_ui_text", "JuiText", t_joy_ui_text, 8, k_joy_ui_text, JOY_V_I32 },
        { "ui", "joy_ui_text_centered", "JuiTextCentered", t_joy_ui_text_centered, 10, k_joy_ui_text_centered, JOY_V_I32 },
        { "ui", "joy_ui_panel_begin", "JuiPanelBegin", t_joy_ui_panel_begin, 6, k_joy_ui_panel_begin, JOY_V_NULL },
        { "ui", "joy_ui_panel_end", "JuiPanelEnd", t_joy_ui_panel_end, 1, k_joy_ui_panel_end, JOY_V_NULL },
        { "ui", "joy_ui_panel_frame", "JuiPanelFrame", t_joy_ui_panel_frame, 5, k_joy_ui_panel_frame, JOY_V_NULL },
        { "ui", "joy_ui_scroll_begin", "JuiScrollBegin", t_joy_ui_scroll_begin, 7, k_joy_ui_scroll_begin, JOY_V_F64 },
        { "ui", "joy_ui_scroll_end", "JuiScrollEnd", t_joy_ui_scroll_end, 1, k_joy_ui_scroll_end, JOY_V_NULL },
        { "ui", "joy_ui_scrollbar", "JuiScrollbar", t_joy_ui_scrollbar, 7, k_joy_ui_scrollbar, JOY_V_NULL },
        { "ui", "joy_ui_scroll_set_offset", "JuiScrollSetOffset", t_joy_ui_scroll_set_offset, 5, k_joy_ui_scroll_set_offset, JOY_V_NULL },
        { "ui", "joy_ui_button", "JuiButton", t_joy_ui_button, 7, k_joy_ui_button, JOY_V_BOOL },
        { "jtilemap", "jtilemap_destroy", "JtilemapDestroy", t_jtilemap_destroy, 1, k_jtilemap_destroy, JOY_V_NULL },
        { "jtilemap", "jtilemap_get_width", "JtilemapGetWidth", t_jtilemap_get_width, 1, k_jtilemap_get_width, JOY_V_I32 },
        { "jtilemap", "jtilemap_get_height", "JtilemapGetHeight", t_jtilemap_get_height, 1, k_jtilemap_get_height, JOY_V_I32 },
        { "jtilemap", "jtilemap_get_tilewidth", "JtilemapGetTilewidth", t_jtilemap_get_tilewidth, 1, k_jtilemap_get_tilewidth, JOY_V_I32 },
        { "jtilemap", "jtilemap_get_tileheight", "JtilemapGetTileheight", t_jtilemap_get_tileheight, 1, k_jtilemap_get_tileheight, JOY_V_I32 },
        { "jtilemap", "jtilemap_get_anim_tileid", "JtilemapGetAnimTileid", t_jtilemap_get_anim_tileid, 3, k_jtilemap_get_anim_tileid, JOY_V_I32 },
        { "jtilemap", "jtilemap_get_tile", "JtilemapGetTile", t_jtilemap_get_tile, 3, k_jtilemap_get_tile, JOY_V_I32 },
        { "jtilemap", "jtilemap_render_all", "JtilemapRenderAll", t_jtilemap_render_all, 5, k_jtilemap_render_all, JOY_V_NULL },
        { "jtilemap", "jtilemap_render_tiles", "JtilemapRenderTiles", t_jtilemap_render_tiles, 5, k_jtilemap_render_tiles, JOY_V_NULL },
        { "jtilemap", "jtilemap_render_set_tile_px", "JtilemapRenderSetTilePx", t_jtilemap_render_set_tile_px, 2, k_jtilemap_render_set_tile_px, JOY_V_NULL },
        { "jtilemap", "jtilemap_render_layer", "JtilemapRenderLayer", t_jtilemap_render_layer, 6, k_jtilemap_render_layer, JOY_V_BOOL },
        { "jtilemap", "jtilemap_render_destroy", "JtilemapRenderDestroy", t_jtilemap_render_destroy, 1, k_jtilemap_render_destroy, JOY_V_NULL },
        { "net", "joy_net_kcpserver_destroy", "KcpserverDestroy", t_joy_net_kcpserver_destroy, 1, k_joy_net_kcpserver_destroy, JOY_V_I32 },
        { "net", "joy_net_kcpserver_send", "KcpserverSend", t_joy_net_kcpserver_send, 4, k_joy_net_kcpserver_send, JOY_V_NULL },
        { "net", "joy_net_kcpserver_broadcast", "KcpserverBroadcast", t_joy_net_kcpserver_broadcast, 3, k_joy_net_kcpserver_broadcast, JOY_V_NULL },
        { "net", "joy_net_kcpserver_offline", "KcpserverOffline", t_joy_net_kcpserver_offline, 2, k_joy_net_kcpserver_offline, JOY_V_NULL },
        { "net", "joy_net_kcpserver_update", "KcpserverUpdate", t_joy_net_kcpserver_update, 1, k_joy_net_kcpserver_update, JOY_V_NULL },
        { "net", "joy_net_kcpserver_connection_count", "KcpserverConnectionCount", t_joy_net_kcpserver_connection_count, 1, k_joy_net_kcpserver_connection_count, JOY_V_I32 },
        { "net", "joy_net_kcpserver_poll_message", "KcpserverPollMessage", t_joy_net_kcpserver_poll_message, 2, k_joy_net_kcpserver_poll_message, JOY_V_BOOL },
        { "net", "joy_net_kcpclient_destroy", "KcpclientDestroy", t_joy_net_kcpclient_destroy, 1, k_joy_net_kcpclient_destroy, JOY_V_NULL },
        { "net", "joy_net_kcpclient_send", "KcpclientSend", t_joy_net_kcpclient_send, 3, k_joy_net_kcpclient_send, JOY_V_I32 },
        { "net", "joy_net_kcpclient_update", "KcpclientUpdate", t_joy_net_kcpclient_update, 1, k_joy_net_kcpclient_update, JOY_V_NULL },
        { "net", "joy_net_kcpclient_poll_message", "KcpclientPollMessage", t_joy_net_kcpclient_poll_message, 2, k_joy_net_kcpclient_poll_message, JOY_V_BOOL },
        { "net", "joy_net_tcpserver_destroy", "TcpserverDestroy", t_joy_net_tcpserver_destroy, 1, k_joy_net_tcpserver_destroy, JOY_V_I32 },
        { "net", "joy_net_tcpserver_send", "TcpserverSend", t_joy_net_tcpserver_send, 4, k_joy_net_tcpserver_send, JOY_V_NULL },
        { "net", "joy_net_tcpserver_broadcast", "TcpserverBroadcast", t_joy_net_tcpserver_broadcast, 3, k_joy_net_tcpserver_broadcast, JOY_V_NULL },
        { "net", "joy_net_tcpserver_offline", "TcpserverOffline", t_joy_net_tcpserver_offline, 2, k_joy_net_tcpserver_offline, JOY_V_NULL },
        { "net", "joy_net_tcpserver_update", "TcpserverUpdate", t_joy_net_tcpserver_update, 1, k_joy_net_tcpserver_update, JOY_V_NULL },
        { "net", "joy_net_tcpserver_connection_count", "TcpserverConnectionCount", t_joy_net_tcpserver_connection_count, 1, k_joy_net_tcpserver_connection_count, JOY_V_I32 },
        { "net", "joy_net_tcpserver_poll_message", "TcpserverPollMessage", t_joy_net_tcpserver_poll_message, 2, k_joy_net_tcpserver_poll_message, JOY_V_BOOL },
        { "net", "joy_net_tcpclient_destroy", "TcpclientDestroy", t_joy_net_tcpclient_destroy, 1, k_joy_net_tcpclient_destroy, JOY_V_NULL },
        { "net", "joy_net_tcpclient_send", "TcpclientSend", t_joy_net_tcpclient_send, 3, k_joy_net_tcpclient_send, JOY_V_I32 },
        { "net", "joy_net_tcpclient_update", "TcpclientUpdate", t_joy_net_tcpclient_update, 1, k_joy_net_tcpclient_update, JOY_V_NULL },
        { "net", "joy_net_tcpclient_poll_message", "TcpclientPollMessage", t_joy_net_tcpclient_poll_message, 2, k_joy_net_tcpclient_poll_message, JOY_V_BOOL },
        { "net", "joy_net_wsnetserver_destroy", "WsnetserverDestroy", t_joy_net_wsnetserver_destroy, 1, k_joy_net_wsnetserver_destroy, JOY_V_I32 },
        { "net", "joy_net_wsnetserver_send", "WsnetserverSend", t_joy_net_wsnetserver_send, 4, k_joy_net_wsnetserver_send, JOY_V_NULL },
        { "net", "joy_net_wsnetserver_broadcast", "WsnetserverBroadcast", t_joy_net_wsnetserver_broadcast, 3, k_joy_net_wsnetserver_broadcast, JOY_V_NULL },
        { "net", "joy_net_wsnetserver_offline", "WsnetserverOffline", t_joy_net_wsnetserver_offline, 2, k_joy_net_wsnetserver_offline, JOY_V_NULL },
        { "net", "joy_net_wsnetserver_update", "WsnetserverUpdate", t_joy_net_wsnetserver_update, 1, k_joy_net_wsnetserver_update, JOY_V_NULL },
        { "net", "joy_net_wsnetserver_connection_count", "WsnetserverConnectionCount", t_joy_net_wsnetserver_connection_count, 1, k_joy_net_wsnetserver_connection_count, JOY_V_I32 },
        { "net", "joy_net_wsnetserver_poll_message", "WsnetserverPollMessage", t_joy_net_wsnetserver_poll_message, 2, k_joy_net_wsnetserver_poll_message, JOY_V_BOOL },
        { "net", "joy_net_wsnetclient_destroy", "WsnetclientDestroy", t_joy_net_wsnetclient_destroy, 1, k_joy_net_wsnetclient_destroy, JOY_V_NULL },
        { "net", "joy_net_wsnetclient_send", "WsnetclientSend", t_joy_net_wsnetclient_send, 3, k_joy_net_wsnetclient_send, JOY_V_I32 },
        { "net", "joy_net_wsnetclient_update", "WsnetclientUpdate", t_joy_net_wsnetclient_update, 1, k_joy_net_wsnetclient_update, JOY_V_NULL },
        { "net", "joy_net_wsnetclient_poll_message", "WsnetclientPollMessage", t_joy_net_wsnetclient_poll_message, 2, k_joy_net_wsnetclient_poll_message, JOY_V_BOOL },
        { "net", "joy_net_netclient_destroy", "NetclientDestroy", t_joy_net_netclient_destroy, 1, k_joy_net_netclient_destroy, JOY_V_NULL },
        { "net", "joy_net_netclient_send", "NetclientSend", t_joy_net_netclient_send, 3, k_joy_net_netclient_send, JOY_V_I32 },
        { "net", "joy_net_netclient_update", "NetclientUpdate", t_joy_net_netclient_update, 1, k_joy_net_netclient_update, JOY_V_NULL },
        { "net", "joy_net_netclient_poll_message", "NetclientPollMessage", t_joy_net_netclient_poll_message, 2, k_joy_net_netclient_poll_message, JOY_V_BOOL },
        { "net", "joy_net_netserver_destroy", "NetserverDestroy", t_joy_net_netserver_destroy, 1, k_joy_net_netserver_destroy, JOY_V_NULL },
        { "net", "joy_net_netserver_send", "NetserverSend", t_joy_net_netserver_send, 4, k_joy_net_netserver_send, JOY_V_NULL },
        { "net", "joy_net_netserver_broadcast", "NetserverBroadcast", t_joy_net_netserver_broadcast, 3, k_joy_net_netserver_broadcast, JOY_V_NULL },
        { "net", "joy_net_netserver_offline", "NetserverOffline", t_joy_net_netserver_offline, 2, k_joy_net_netserver_offline, JOY_V_NULL },
        { "net", "joy_net_netserver_update", "NetserverUpdate", t_joy_net_netserver_update, 1, k_joy_net_netserver_update, JOY_V_NULL },
        { "net", "joy_net_netserver_connection_count", "NetserverConnectionCount", t_joy_net_netserver_connection_count, 1, k_joy_net_netserver_connection_count, JOY_V_I32 },
        { "net", "joy_net_netserver_poll_message", "NetserverPollMessage", t_joy_net_netserver_poll_message, 2, k_joy_net_netserver_poll_message, JOY_V_BOOL },
        { "net", "joy_net_netmessage_free_data", "NetmessageFreeData", t_joy_net_netmessage_free_data, 1, k_joy_net_netmessage_free_data, JOY_V_NULL },
        { "scene", "scene_node_destroy", "SceneNodeDestroy", t_scene_node_destroy, 1, k_scene_node_destroy, JOY_V_NULL },
        { "scene", "scene_node_add_child", "SceneNodeAddChild", t_scene_node_add_child, 2, k_scene_node_add_child, JOY_V_NULL },
        { "scene", "scene_node_remove_child", "SceneNodeRemoveChild", t_scene_node_remove_child, 2, k_scene_node_remove_child, JOY_V_NULL },
        { "scene", "scene_node_get_child_count", "SceneNodeGetChildCount", t_scene_node_get_child_count, 1, k_scene_node_get_child_count, JOY_V_I32 },
        { "scene", "scene_node_set_position", "SceneNodeSetPosition", t_scene_node_set_position, 3, k_scene_node_set_position, JOY_V_NULL },
        { "scene", "scene_node_set_world_position", "SceneNodeSetWorldPosition", t_scene_node_set_world_position, 3, k_scene_node_set_world_position, JOY_V_NULL },
        { "scene", "scene_node_set_rotation", "SceneNodeSetRotation", t_scene_node_set_rotation, 2, k_scene_node_set_rotation, JOY_V_NULL },
        { "scene", "scene_node_get_rotation", "SceneNodeGetRotation", t_scene_node_get_rotation, 1, k_scene_node_get_rotation, JOY_V_F64 },
        { "scene", "scene_node_set_scale", "SceneNodeSetScale", t_scene_node_set_scale, 3, k_scene_node_set_scale, JOY_V_NULL },
        { "scene", "scene_node_set_zorder", "SceneNodeSetZorder", t_scene_node_set_zorder, 2, k_scene_node_set_zorder, JOY_V_NULL },
        { "scene", "scene_node_get_zorder", "SceneNodeGetZorder", t_scene_node_get_zorder, 1, k_scene_node_get_zorder, JOY_V_I32 },
        { "scene", "scene_node_set_name", "SceneNodeSetName", t_scene_node_set_name, 2, k_scene_node_set_name, JOY_V_NULL },
        { "scene", "scene_node_set_tag", "SceneNodeSetTag", t_scene_node_set_tag, 2, k_scene_node_set_tag, JOY_V_NULL },
        { "scene", "scene_node_update", "SceneNodeUpdate", t_scene_node_update, 2, k_scene_node_update, JOY_V_NULL },
        { "scene", "scene_destroy", "SceneDestroy", t_scene_destroy, 1, k_scene_destroy, JOY_V_NULL },
        { "scene", "scene_add_root_node", "SceneAddRootNode", t_scene_add_root_node, 2, k_scene_add_root_node, JOY_V_NULL },
        { "scene", "scene_remove_root_node", "SceneRemoveRootNode", t_scene_remove_root_node, 2, k_scene_remove_root_node, JOY_V_NULL },
        { "scene", "scene_get_root_count", "SceneGetRootCount", t_scene_get_root_count, 1, k_scene_get_root_count, JOY_V_I32 },
        { "scene", "scene_update", "SceneUpdate", t_scene_update, 2, k_scene_update, JOY_V_NULL },
        { "scene", "scene_set_popup_node", "SceneSetPopupNode", t_scene_set_popup_node, 2, k_scene_set_popup_node, JOY_V_NULL },
        { "scene", "scene_manager_destroy", "SceneManagerDestroy", t_scene_manager_destroy, 1, k_scene_manager_destroy, JOY_V_NULL },
        { "scene", "scene_manager_push", "SceneManagerPush", t_scene_manager_push, 2, k_scene_manager_push, JOY_V_NULL },
        { "scene", "scene_manager_replace", "SceneManagerReplace", t_scene_manager_replace, 2, k_scene_manager_replace, JOY_V_NULL },
        { "scene", "scene_manager_pop", "SceneManagerPop", t_scene_manager_pop, 1, k_scene_manager_pop, JOY_V_NULL },
        { "scene", "scene_manager_update", "SceneManagerUpdate", t_scene_manager_update, 2, k_scene_manager_update, JOY_V_NULL },
        { "scene", "scene_manager_get_count", "SceneManagerGetCount", t_scene_manager_get_count, 1, k_scene_manager_get_count, JOY_V_I32 },
        { "r2d", "joy_r2d_particle_destroy", "JparticleDestroy", t_joy_r2d_particle_destroy, 1, k_joy_r2d_particle_destroy, JOY_V_NULL },
        { "r2d", "joy_r2d_particle_set_position", "JparticleSetPosition", t_joy_r2d_particle_set_position, 3, k_joy_r2d_particle_set_position, JOY_V_NULL },
        { "r2d", "joy_r2d_particle_emit", "JparticleEmit", t_joy_r2d_particle_emit, 1, k_joy_r2d_particle_emit, JOY_V_NULL },
        { "r2d", "joy_r2d_particle_clear", "JparticleClear", t_joy_r2d_particle_clear, 1, k_joy_r2d_particle_clear, JOY_V_NULL },
        { "r2d", "joy_r2d_particle_update", "JparticleUpdate", t_joy_r2d_particle_update, 2, k_joy_r2d_particle_update, JOY_V_NULL },
        { "r2d", "joy_r2d_particle_render", "JparticleRender", t_joy_r2d_particle_render, 2, k_joy_r2d_particle_render, JOY_V_NULL },
        { "r2d", "joy_r2d_particle_count", "JparticleCount", t_joy_r2d_particle_count, 1, k_joy_r2d_particle_count, JOY_V_I32 },
        { "r2d", "joy_r2d_particle_is_active", "JparticleIsActive", t_joy_r2d_particle_is_active, 1, k_joy_r2d_particle_is_active, JOY_V_BOOL },
        { "r3d", "joy_r3d_light_sys_destroy", "JlightSysDestroy", t_joy_r3d_light_sys_destroy, 1, k_joy_r3d_light_sys_destroy, JOY_V_NULL },
        { "r3d", "joy_r3d_light_sys_resize", "JlightSysResize", t_joy_r3d_light_sys_resize, 3, k_joy_r3d_light_sys_resize, JOY_V_NULL },
        { "r3d", "joy_r3d_light_remove", "JlightRemove", t_joy_r3d_light_remove, 2, k_joy_r3d_light_remove, JOY_V_NULL },
        { "r3d", "joy_r3d_light_clear", "JlightClear", t_joy_r3d_light_clear, 1, k_joy_r3d_light_clear, JOY_V_NULL },
        { "r3d", "joy_r3d_light_add_occluder", "JlightAddOccluder", t_joy_r3d_light_add_occluder, 5, k_joy_r3d_light_add_occluder, JOY_V_I32 },
        { "r3d", "joy_r3d_light_clear_occluders", "JlightClearOccluders", t_joy_r3d_light_clear_occluders, 1, k_joy_r3d_light_clear_occluders, JOY_V_NULL },
        { "r3d", "joy_r3d_light_set_ambient", "JlightSetAmbient", t_joy_r3d_light_set_ambient, 4, k_joy_r3d_light_set_ambient, JOY_V_NULL },
        { "r3d", "joy_r3d_light_render", "JlightRender", t_joy_r3d_light_render, 2, k_joy_r3d_light_render, JOY_V_NULL },
        { "jecs", "jecs_destroy", "JecsDestroy", t_jecs_destroy, 1, k_jecs_destroy, JOY_V_NULL },
        { "jecs", "jecs_process", "JecsProcess", t_jecs_process, 2, k_jecs_process, JOY_V_NULL },
        { "jecs", "jecs_define", "JecsDefine", t_jecs_define, 3, k_jecs_define, JOY_V_NULL },
        { "audio", "audio_open_device", "AudioOpenDevice", t_audio_open_device, 0, NULL, JOY_V_U32 },
        { "audio", "audio_close_device", "AudioCloseDevice", t_audio_close_device, 1, k_audio_close_device, JOY_V_NULL },
        { "audio", "audio_create_wav", "AudioCreateWav", t_audio_create_wav, 2, k_audio_create_wav, JOY_V_BOOL },
        { "audio", "audio_create_mp3", "AudioCreateMp3", t_audio_create_mp3, 2, k_audio_create_mp3, JOY_V_BOOL },
        { "audio", "audio_create_flac", "AudioCreateFlac", t_audio_create_flac, 2, k_audio_create_flac, JOY_V_BOOL },
        { "audio", "audio_create_ogg", "AudioCreateOgg", t_audio_create_ogg, 2, k_audio_create_ogg, JOY_V_BOOL },
        { "audio", "audio_shot_play", "AudioShotPlay", t_audio_shot_play, 1, k_audio_shot_play, JOY_V_NULL },
        { "audio", "audio_shot_set_gain", "AudioShotSetGain", t_audio_shot_set_gain, 2, k_audio_shot_set_gain, JOY_V_NULL },
        { "audio", "audio_shot_destroy", "AudioShotDestroy", t_audio_shot_destroy, 1, k_audio_shot_destroy, JOY_V_NULL },
        { "audio", "audio_bgm_stop", "AudioBgmStop", t_audio_bgm_stop, 1, k_audio_bgm_stop, JOY_V_NULL },
        { "audio", "audio_bgm_set_gain", "AudioBgmSetGain", t_audio_bgm_set_gain, 2, k_audio_bgm_set_gain, JOY_V_NULL },
        { "audio", "audio_bgm_destroy", "AudioBgmDestroy", t_audio_bgm_destroy, 1, k_audio_bgm_destroy, JOY_V_NULL },
        { "ui", "joy_ui_image_draw_rect", "ImageDrawRect", t_joy_ui_image_draw_rect, 6, k_joy_ui_image_draw_rect, JOY_V_NULL },
        { "jmath", "jmath_fp_from_float", "JmathFpFromFloat", t_jmath_fp_from_float, 1, k_jmath_fp_from_float, JOY_V_I64 },
        { "jmath", "jmath_fp_to_float", "JmathFpToFloat", t_jmath_fp_to_float, 1, k_jmath_fp_to_float, JOY_V_F64 },
        { "jmath", "jmath_fp_from_uint", "JmathFpFromUint", t_jmath_fp_from_uint, 1, k_jmath_fp_from_uint, JOY_V_I64 },
        { "jmath", "jmath_fp_from_int", "JmathFpFromInt", t_jmath_fp_from_int, 1, k_jmath_fp_from_int, JOY_V_I64 },
        { "jmath", "jmath_fp_from_fraction", "JmathFpFromFraction", t_jmath_fp_from_fraction, 1, k_jmath_fp_from_fraction, JOY_V_I64 },
        { "jmath", "jmath_fp_min_value", "JmathFpMinValue", t_jmath_fp_min_value, 0, NULL, JOY_V_I64 },
        { "jmath", "jmath_fp_max_value", "JmathFpMaxValue", t_jmath_fp_max_value, 0, NULL, JOY_V_I64 },
        { "jmath", "jmath_fp_add", "JmathFpAdd", t_jmath_fp_add, 2, k_jmath_fp_add, JOY_V_I64 },
        { "jmath", "jmath_fp_sub", "JmathFpSub", t_jmath_fp_sub, 2, k_jmath_fp_sub, JOY_V_I64 },
        { "jmath", "jmath_fp_zero", "JmathFpZero", t_jmath_fp_zero, 0, NULL, JOY_V_I64 },
        { "jmath", "jmath_fp_half", "JmathFpHalf", t_jmath_fp_half, 0, NULL, JOY_V_I64 },
        { "jmath", "jmath_fp_one", "JmathFpOne", t_jmath_fp_one, 0, NULL, JOY_V_I64 },
        { "jmath", "jmath_fp_pi", "JmathFpPi", t_jmath_fp_pi, 0, NULL, JOY_V_I64 },
        { "jmath", "jmath_fp_max", "JmathFpMax", t_jmath_fp_max, 2, k_jmath_fp_max, JOY_V_I64 },
        { "jmath", "jmath_fp_min", "JmathFpMin", t_jmath_fp_min, 2, k_jmath_fp_min, JOY_V_I64 },
        { "jmath", "jmath_fp_pow2", "JmathFpPow2", t_jmath_fp_pow2, 1, k_jmath_fp_pow2, JOY_V_I64 },
        { "jmath", "jmath_fp_sign", "JmathFpSign", t_jmath_fp_sign, 1, k_jmath_fp_sign, JOY_V_I32 },
        { "jmath", "jmath_fp_clamp", "JmathFpClamp", t_jmath_fp_clamp, 3, k_jmath_fp_clamp, JOY_V_I64 },
        { "jmath", "jmath_fp_lerp", "JmathFpLerp", t_jmath_fp_lerp, 3, k_jmath_fp_lerp, JOY_V_I64 },
        { "jmath", "jmath_vec2f_negate_x", "JmathVec2fNegateX", t_jmath_vec2f_negate_x, 2, k_jmath_vec2f_negate_x, JOY_V_I64 },
        { "jmath", "jmath_vec2f_negate_y", "JmathVec2fNegateY", t_jmath_vec2f_negate_y, 2, k_jmath_vec2f_negate_y, JOY_V_I64 },
        { "jmath", "jmath_vec2f_dot", "JmathVec2fDot", t_jmath_vec2f_dot, 4, k_jmath_vec2f_dot, JOY_V_I64 },
        { "jmath", "jmath_vec2f_cross", "JmathVec2fCross", t_jmath_vec2f_cross, 4, k_jmath_vec2f_cross, JOY_V_I64 },
        { "jmath", "jmath_vec2f_cross2_x", "JmathVec2fCross2X", t_jmath_vec2f_cross2_x, 3, k_jmath_vec2f_cross2_x, JOY_V_I64 },
        { "jmath", "jmath_vec2f_cross2_y", "JmathVec2fCross2Y", t_jmath_vec2f_cross2_y, 3, k_jmath_vec2f_cross2_y, JOY_V_I64 },
        { "jmath", "jmath_vec2f_cross3_x", "JmathVec2fCross3X", t_jmath_vec2f_cross3_x, 3, k_jmath_vec2f_cross3_x, JOY_V_I64 },
        { "jmath", "jmath_vec2f_cross3_y", "JmathVec2fCross3Y", t_jmath_vec2f_cross3_y, 3, k_jmath_vec2f_cross3_y, JOY_V_I64 },
        { "jmath", "jmath_vec2f_add_x", "JmathVec2fAddX", t_jmath_vec2f_add_x, 4, k_jmath_vec2f_add_x, JOY_V_I64 },
        { "jmath", "jmath_vec2f_add_y", "JmathVec2fAddY", t_jmath_vec2f_add_y, 4, k_jmath_vec2f_add_y, JOY_V_I64 },
        { "jmath", "jmath_vec2f_sub_x", "JmathVec2fSubX", t_jmath_vec2f_sub_x, 4, k_jmath_vec2f_sub_x, JOY_V_I64 },
        { "jmath", "jmath_vec2f_sub_y", "JmathVec2fSubY", t_jmath_vec2f_sub_y, 4, k_jmath_vec2f_sub_y, JOY_V_I64 },
        { "jmath", "jmath_vec2f_scale_x", "JmathVec2fScaleX", t_jmath_vec2f_scale_x, 3, k_jmath_vec2f_scale_x, JOY_V_I64 },
        { "jmath", "jmath_vec2f_scale_y", "JmathVec2fScaleY", t_jmath_vec2f_scale_y, 3, k_jmath_vec2f_scale_y, JOY_V_I64 },
        { "jmath", "jmath_vec2f_abs_x", "JmathVec2fAbsX", t_jmath_vec2f_abs_x, 2, k_jmath_vec2f_abs_x, JOY_V_I64 },
        { "jmath", "jmath_vec2f_abs_y", "JmathVec2fAbsY", t_jmath_vec2f_abs_y, 2, k_jmath_vec2f_abs_y, JOY_V_I64 },
        { "jmath", "jmath_vec2f_length_squared", "JmathVec2fLengthSquared", t_jmath_vec2f_length_squared, 2, k_jmath_vec2f_length_squared, JOY_V_I64 },
        { "jmath", "jmath_vec2f_length", "JmathVec2fLength", t_jmath_vec2f_length, 2, k_jmath_vec2f_length, JOY_V_I64 },
        { "jmath", "jmath_vec2f_normalize_x", "JmathVec2fNormalizeX", t_jmath_vec2f_normalize_x, 2, k_jmath_vec2f_normalize_x, JOY_V_I64 },
        { "jmath", "jmath_vec2f_normalize_y", "JmathVec2fNormalizeY", t_jmath_vec2f_normalize_y, 2, k_jmath_vec2f_normalize_y, JOY_V_I64 },
        { "jmath", "jmath_vec2f_normal_x", "JmathVec2fNormalX", t_jmath_vec2f_normal_x, 2, k_jmath_vec2f_normal_x, JOY_V_I64 },
        { "jmath", "jmath_vec2f_normal_y", "JmathVec2fNormalY", t_jmath_vec2f_normal_y, 2, k_jmath_vec2f_normal_y, JOY_V_I64 },
        { "jmath", "jmath_vec2f_rotate_x", "JmathVec2fRotateX", t_jmath_vec2f_rotate_x, 3, k_jmath_vec2f_rotate_x, JOY_V_I64 },
        { "jmath", "jmath_vec2f_rotate_y", "JmathVec2fRotateY", t_jmath_vec2f_rotate_y, 3, k_jmath_vec2f_rotate_y, JOY_V_I64 },
        { "jmath", "jmath_vec2f_lerp_x", "JmathVec2fLerpX", t_jmath_vec2f_lerp_x, 5, k_jmath_vec2f_lerp_x, JOY_V_I64 },
        { "jmath", "jmath_vec2f_lerp_y", "JmathVec2fLerpY", t_jmath_vec2f_lerp_y, 5, k_jmath_vec2f_lerp_y, JOY_V_I64 },
        { "jmath", "jmath_vec2f_distance", "JmathVec2fDistance", t_jmath_vec2f_distance, 4, k_jmath_vec2f_distance, JOY_V_I64 },
        { "jmath", "jmath_vec3f_negate_x", "JmathVec3fNegateX", t_jmath_vec3f_negate_x, 3, k_jmath_vec3f_negate_x, JOY_V_I64 },
        { "jmath", "jmath_vec3f_negate_y", "JmathVec3fNegateY", t_jmath_vec3f_negate_y, 3, k_jmath_vec3f_negate_y, JOY_V_I64 },
        { "jmath", "jmath_vec3f_negate_z", "JmathVec3fNegateZ", t_jmath_vec3f_negate_z, 3, k_jmath_vec3f_negate_z, JOY_V_I64 },
        { "jmath", "jmath_vec3f_add_x", "JmathVec3fAddX", t_jmath_vec3f_add_x, 6, k_jmath_vec3f_add_x, JOY_V_I64 },
        { "jmath", "jmath_vec3f_add_y", "JmathVec3fAddY", t_jmath_vec3f_add_y, 6, k_jmath_vec3f_add_y, JOY_V_I64 },
        { "jmath", "jmath_vec3f_add_z", "JmathVec3fAddZ", t_jmath_vec3f_add_z, 6, k_jmath_vec3f_add_z, JOY_V_I64 },
        { "jmath", "jmath_vec3f_sub_x", "JmathVec3fSubX", t_jmath_vec3f_sub_x, 6, k_jmath_vec3f_sub_x, JOY_V_I64 },
        { "jmath", "jmath_vec3f_sub_y", "JmathVec3fSubY", t_jmath_vec3f_sub_y, 6, k_jmath_vec3f_sub_y, JOY_V_I64 },
        { "jmath", "jmath_vec3f_sub_z", "JmathVec3fSubZ", t_jmath_vec3f_sub_z, 6, k_jmath_vec3f_sub_z, JOY_V_I64 },
        { "jmath", "jmath_vec3f_scale_x", "JmathVec3fScaleX", t_jmath_vec3f_scale_x, 4, k_jmath_vec3f_scale_x, JOY_V_I64 },
        { "jmath", "jmath_vec3f_scale_y", "JmathVec3fScaleY", t_jmath_vec3f_scale_y, 4, k_jmath_vec3f_scale_y, JOY_V_I64 },
        { "jmath", "jmath_vec3f_scale_z", "JmathVec3fScaleZ", t_jmath_vec3f_scale_z, 4, k_jmath_vec3f_scale_z, JOY_V_I64 },
        { "jmath", "jmath_vec3f_abs_x", "JmathVec3fAbsX", t_jmath_vec3f_abs_x, 3, k_jmath_vec3f_abs_x, JOY_V_I64 },
        { "jmath", "jmath_vec3f_abs_y", "JmathVec3fAbsY", t_jmath_vec3f_abs_y, 3, k_jmath_vec3f_abs_y, JOY_V_I64 },
        { "jmath", "jmath_vec3f_abs_z", "JmathVec3fAbsZ", t_jmath_vec3f_abs_z, 3, k_jmath_vec3f_abs_z, JOY_V_I64 },
        { "jmath", "jmath_vec3f_dot", "JmathVec3fDot", t_jmath_vec3f_dot, 6, k_jmath_vec3f_dot, JOY_V_I64 },
        { "jmath", "jmath_vec3f_cross_x", "JmathVec3fCrossX", t_jmath_vec3f_cross_x, 6, k_jmath_vec3f_cross_x, JOY_V_I64 },
        { "jmath", "jmath_vec3f_cross_y", "JmathVec3fCrossY", t_jmath_vec3f_cross_y, 6, k_jmath_vec3f_cross_y, JOY_V_I64 },
        { "jmath", "jmath_vec3f_cross_z", "JmathVec3fCrossZ", t_jmath_vec3f_cross_z, 6, k_jmath_vec3f_cross_z, JOY_V_I64 },
        { "jmath", "jmath_vec3f_square_length", "JmathVec3fSquareLength", t_jmath_vec3f_square_length, 3, k_jmath_vec3f_square_length, JOY_V_I64 },
        { "jmath", "jmath_vec3f_length", "JmathVec3fLength", t_jmath_vec3f_length, 3, k_jmath_vec3f_length, JOY_V_I64 },
        { "jmath", "jmath_vec3f_normalize_x", "JmathVec3fNormalizeX", t_jmath_vec3f_normalize_x, 3, k_jmath_vec3f_normalize_x, JOY_V_I64 },
        { "jmath", "jmath_vec3f_normalize_y", "JmathVec3fNormalizeY", t_jmath_vec3f_normalize_y, 3, k_jmath_vec3f_normalize_y, JOY_V_I64 },
        { "jmath", "jmath_vec3f_normalize_z", "JmathVec3fNormalizeZ", t_jmath_vec3f_normalize_z, 3, k_jmath_vec3f_normalize_z, JOY_V_I64 },
        { "jmath", "jmath_vec3f_lerp_x", "JmathVec3fLerpX", t_jmath_vec3f_lerp_x, 7, k_jmath_vec3f_lerp_x, JOY_V_I64 },
        { "jmath", "jmath_vec3f_lerp_y", "JmathVec3fLerpY", t_jmath_vec3f_lerp_y, 7, k_jmath_vec3f_lerp_y, JOY_V_I64 },
        { "jmath", "jmath_vec3f_lerp_z", "JmathVec3fLerpZ", t_jmath_vec3f_lerp_z, 7, k_jmath_vec3f_lerp_z, JOY_V_I64 },
        { "jmath", "jmath_vec3f_distance", "JmathVec3fDistance", t_jmath_vec3f_distance, 6, k_jmath_vec3f_distance, JOY_V_I64 },
        { "jmath", "jmath_mat22f_mul_vec2f_x", "JmathMat22fMulVec2fX", t_jmath_mat22f_mul_vec2f_x, 6, k_jmath_mat22f_mul_vec2f_x, JOY_V_I64 },
        { "jmath", "jmath_mat22f_mul_vec2f_y", "JmathMat22fMulVec2fY", t_jmath_mat22f_mul_vec2f_y, 6, k_jmath_mat22f_mul_vec2f_y, JOY_V_I64 },
        { "jmath", "jmath_mat22f_rotate_col1_x", "JmathMat22fRotateCol1X", t_jmath_mat22f_rotate_col1_x, 1, k_jmath_mat22f_rotate_col1_x, JOY_V_I64 },
        { "jmath", "jmath_mat22f_rotate_col1_y", "JmathMat22fRotateCol1Y", t_jmath_mat22f_rotate_col1_y, 1, k_jmath_mat22f_rotate_col1_y, JOY_V_I64 },
        { "jmath", "jmath_mat22f_rotate_col2_x", "JmathMat22fRotateCol2X", t_jmath_mat22f_rotate_col2_x, 1, k_jmath_mat22f_rotate_col2_x, JOY_V_I64 },
        { "jmath", "jmath_mat22f_rotate_col2_y", "JmathMat22fRotateCol2Y", t_jmath_mat22f_rotate_col2_y, 1, k_jmath_mat22f_rotate_col2_y, JOY_V_I64 },
        { "jmath", "jmath_mat22f_transpose_col1_x", "JmathMat22fTransposeCol1X", t_jmath_mat22f_transpose_col1_x, 4, k_jmath_mat22f_transpose_col1_x, JOY_V_I64 },
        { "jmath", "jmath_mat22f_transpose_col1_y", "JmathMat22fTransposeCol1Y", t_jmath_mat22f_transpose_col1_y, 4, k_jmath_mat22f_transpose_col1_y, JOY_V_I64 },
        { "jmath", "jmath_mat22f_transpose_col2_x", "JmathMat22fTransposeCol2X", t_jmath_mat22f_transpose_col2_x, 4, k_jmath_mat22f_transpose_col2_x, JOY_V_I64 },
        { "jmath", "jmath_mat22f_transpose_col2_y", "JmathMat22fTransposeCol2Y", t_jmath_mat22f_transpose_col2_y, 4, k_jmath_mat22f_transpose_col2_y, JOY_V_I64 },
        { "jmath", "jmath_mat22f_invert_col1_x", "JmathMat22fInvertCol1X", t_jmath_mat22f_invert_col1_x, 4, k_jmath_mat22f_invert_col1_x, JOY_V_I64 },
        { "jmath", "jmath_mat22f_invert_col1_y", "JmathMat22fInvertCol1Y", t_jmath_mat22f_invert_col1_y, 4, k_jmath_mat22f_invert_col1_y, JOY_V_I64 },
        { "jmath", "jmath_mat22f_invert_col2_x", "JmathMat22fInvertCol2X", t_jmath_mat22f_invert_col2_x, 4, k_jmath_mat22f_invert_col2_x, JOY_V_I64 },
        { "jmath", "jmath_mat22f_invert_col2_y", "JmathMat22fInvertCol2Y", t_jmath_mat22f_invert_col2_y, 4, k_jmath_mat22f_invert_col2_y, JOY_V_I64 },
        { "jmath", "jmath_mat22f_add_col1_x", "JmathMat22fAddCol1X", t_jmath_mat22f_add_col1_x, 8, k_jmath_mat22f_add_col1_x, JOY_V_I64 },
        { "jmath", "jmath_mat22f_add_col1_y", "JmathMat22fAddCol1Y", t_jmath_mat22f_add_col1_y, 8, k_jmath_mat22f_add_col1_y, JOY_V_I64 },
        { "jmath", "jmath_mat22f_add_col2_x", "JmathMat22fAddCol2X", t_jmath_mat22f_add_col2_x, 8, k_jmath_mat22f_add_col2_x, JOY_V_I64 },
        { "jmath", "jmath_mat22f_add_col2_y", "JmathMat22fAddCol2Y", t_jmath_mat22f_add_col2_y, 8, k_jmath_mat22f_add_col2_y, JOY_V_I64 },
        { "jmath", "jmath_mat22f_mul_col1_x", "JmathMat22fMulCol1X", t_jmath_mat22f_mul_col1_x, 8, k_jmath_mat22f_mul_col1_x, JOY_V_I64 },
        { "jmath", "jmath_mat22f_mul_col1_y", "JmathMat22fMulCol1Y", t_jmath_mat22f_mul_col1_y, 8, k_jmath_mat22f_mul_col1_y, JOY_V_I64 },
        { "jmath", "jmath_mat22f_mul_col2_x", "JmathMat22fMulCol2X", t_jmath_mat22f_mul_col2_x, 8, k_jmath_mat22f_mul_col2_x, JOY_V_I64 },
        { "jmath", "jmath_mat22f_mul_col2_y", "JmathMat22fMulCol2Y", t_jmath_mat22f_mul_col2_y, 8, k_jmath_mat22f_mul_col2_y, JOY_V_I64 },
        { "jmath", "jmath_mat22f_abs_col1_x", "JmathMat22fAbsCol1X", t_jmath_mat22f_abs_col1_x, 4, k_jmath_mat22f_abs_col1_x, JOY_V_I64 },
        { "jmath", "jmath_mat22f_abs_col1_y", "JmathMat22fAbsCol1Y", t_jmath_mat22f_abs_col1_y, 4, k_jmath_mat22f_abs_col1_y, JOY_V_I64 },
        { "jmath", "jmath_mat22f_abs_col2_x", "JmathMat22fAbsCol2X", t_jmath_mat22f_abs_col2_x, 4, k_jmath_mat22f_abs_col2_x, JOY_V_I64 },
        { "jmath", "jmath_mat22f_abs_col2_y", "JmathMat22fAbsCol2Y", t_jmath_mat22f_abs_col2_y, 4, k_jmath_mat22f_abs_col2_y, JOY_V_I64 },
        { "jmath", "jmath_ft_from_float", "JmathFtFromFloat", t_jmath_ft_from_float, 1, k_jmath_ft_from_float, JOY_V_F64 },
        { "jmath", "jmath_ft_to_float", "JmathFtToFloat", t_jmath_ft_to_float, 1, k_jmath_ft_to_float, JOY_V_F64 },
        { "jmath", "jmath_ft_min_value", "JmathFtMinValue", t_jmath_ft_min_value, 0, NULL, JOY_V_F64 },
        { "jmath", "jmath_ft_max_value", "JmathFtMaxValue", t_jmath_ft_max_value, 0, NULL, JOY_V_F64 },
        { "jmath", "jmath_ft_add", "JmathFtAdd", t_jmath_ft_add, 2, k_jmath_ft_add, JOY_V_F64 },
        { "jmath", "jmath_ft_sub", "JmathFtSub", t_jmath_ft_sub, 2, k_jmath_ft_sub, JOY_V_F64 },
        { "jmath", "jmath_ft_mul", "JmathFtMul", t_jmath_ft_mul, 2, k_jmath_ft_mul, JOY_V_F64 },
        { "jmath", "jmath_ft_div", "JmathFtDiv", t_jmath_ft_div, 2, k_jmath_ft_div, JOY_V_F64 },
        { "jmath", "jmath_ft_abs", "JmathFtAbs", t_jmath_ft_abs, 1, k_jmath_ft_abs, JOY_V_F64 },
        { "jmath", "jmath_ft_sqrt", "JmathFtSqrt", t_jmath_ft_sqrt, 1, k_jmath_ft_sqrt, JOY_V_F64 },
        { "jmath", "jmath_ft_sin", "JmathFtSin", t_jmath_ft_sin, 1, k_jmath_ft_sin, JOY_V_F64 },
        { "jmath", "jmath_ft_cos", "JmathFtCos", t_jmath_ft_cos, 1, k_jmath_ft_cos, JOY_V_F64 },
        { "jmath", "jmath_ft_atan2", "JmathFtAtan2", t_jmath_ft_atan2, 2, k_jmath_ft_atan2, JOY_V_F64 },
        { "jmath", "jmath_ft_zero", "JmathFtZero", t_jmath_ft_zero, 0, NULL, JOY_V_F64 },
        { "jmath", "jmath_ft_half", "JmathFtHalf", t_jmath_ft_half, 0, NULL, JOY_V_F64 },
        { "jmath", "jmath_ft_one", "JmathFtOne", t_jmath_ft_one, 0, NULL, JOY_V_F64 },
        { "jmath", "jmath_ft_pi", "JmathFtPi", t_jmath_ft_pi, 0, NULL, JOY_V_F64 },
        { "jmath", "jmath_ft_max", "JmathFtMax", t_jmath_ft_max, 2, k_jmath_ft_max, JOY_V_F64 },
        { "jmath", "jmath_ft_min", "JmathFtMin", t_jmath_ft_min, 2, k_jmath_ft_min, JOY_V_F64 },
        { "jmath", "jmath_ft_pow2", "JmathFtPow2", t_jmath_ft_pow2, 1, k_jmath_ft_pow2, JOY_V_F64 },
        { "jmath", "jmath_ft_sign", "JmathFtSign", t_jmath_ft_sign, 1, k_jmath_ft_sign, JOY_V_I32 },
        { "jmath", "jmath_ft_clamp", "JmathFtClamp", t_jmath_ft_clamp, 3, k_jmath_ft_clamp, JOY_V_F64 },
        { "jmath", "jmath_ft_lerp", "JmathFtLerp", t_jmath_ft_lerp, 3, k_jmath_ft_lerp, JOY_V_F64 },
        { "jmath", "jmath_vec2_negate_x", "JmathVec2NegateX", t_jmath_vec2_negate_x, 2, k_jmath_vec2_negate_x, JOY_V_F64 },
        { "jmath", "jmath_vec2_negate_y", "JmathVec2NegateY", t_jmath_vec2_negate_y, 2, k_jmath_vec2_negate_y, JOY_V_F64 },
        { "jmath", "jmath_vec2_negate", "JmathVec2Negate", t_jmath_vec2_negate, 2, k_jmath_vec2_negate, JOY_V_U64 },
        { "jmath", "jmath_vec2_dot", "JmathVec2Dot", t_jmath_vec2_dot, 4, k_jmath_vec2_dot, JOY_V_F64 },
        { "jmath", "jmath_vec2_cross", "JmathVec2Cross", t_jmath_vec2_cross, 4, k_jmath_vec2_cross, JOY_V_F64 },
        { "jmath", "jmath_vec2_cross2_x", "JmathVec2Cross2X", t_jmath_vec2_cross2_x, 3, k_jmath_vec2_cross2_x, JOY_V_F64 },
        { "jmath", "jmath_vec2_cross2_y", "JmathVec2Cross2Y", t_jmath_vec2_cross2_y, 3, k_jmath_vec2_cross2_y, JOY_V_F64 },
        { "jmath", "jmath_vec2_cross2", "JmathVec2Cross2", t_jmath_vec2_cross2, 3, k_jmath_vec2_cross2, JOY_V_U64 },
        { "jmath", "jmath_vec2_cross3_x", "JmathVec2Cross3X", t_jmath_vec2_cross3_x, 3, k_jmath_vec2_cross3_x, JOY_V_F64 },
        { "jmath", "jmath_vec2_cross3_y", "JmathVec2Cross3Y", t_jmath_vec2_cross3_y, 3, k_jmath_vec2_cross3_y, JOY_V_F64 },
        { "jmath", "jmath_vec2_cross3", "JmathVec2Cross3", t_jmath_vec2_cross3, 3, k_jmath_vec2_cross3, JOY_V_U64 },
        { "jmath", "jmath_vec2_add_x", "JmathVec2AddX", t_jmath_vec2_add_x, 4, k_jmath_vec2_add_x, JOY_V_F64 },
        { "jmath", "jmath_vec2_add_y", "JmathVec2AddY", t_jmath_vec2_add_y, 4, k_jmath_vec2_add_y, JOY_V_F64 },
        { "jmath", "jmath_vec2_add", "JmathVec2Add", t_jmath_vec2_add, 4, k_jmath_vec2_add, JOY_V_U64 },
        { "jmath", "jmath_vec2_sub_x", "JmathVec2SubX", t_jmath_vec2_sub_x, 4, k_jmath_vec2_sub_x, JOY_V_F64 },
        { "jmath", "jmath_vec2_sub_y", "JmathVec2SubY", t_jmath_vec2_sub_y, 4, k_jmath_vec2_sub_y, JOY_V_F64 },
        { "jmath", "jmath_vec2_sub", "JmathVec2Sub", t_jmath_vec2_sub, 4, k_jmath_vec2_sub, JOY_V_U64 },
        { "jmath", "jmath_vec2_scale_x", "JmathVec2ScaleX", t_jmath_vec2_scale_x, 3, k_jmath_vec2_scale_x, JOY_V_F64 },
        { "jmath", "jmath_vec2_scale_y", "JmathVec2ScaleY", t_jmath_vec2_scale_y, 3, k_jmath_vec2_scale_y, JOY_V_F64 },
        { "jmath", "jmath_vec2_scale", "JmathVec2Scale", t_jmath_vec2_scale, 3, k_jmath_vec2_scale, JOY_V_U64 },
        { "jmath", "jmath_vec2_abs_x", "JmathVec2AbsX", t_jmath_vec2_abs_x, 2, k_jmath_vec2_abs_x, JOY_V_F64 },
        { "jmath", "jmath_vec2_abs_y", "JmathVec2AbsY", t_jmath_vec2_abs_y, 2, k_jmath_vec2_abs_y, JOY_V_F64 },
        { "jmath", "jmath_vec2_abs", "JmathVec2Abs", t_jmath_vec2_abs, 2, k_jmath_vec2_abs, JOY_V_U64 },
        { "jmath", "jmath_vec2_length_squared", "JmathVec2LengthSquared", t_jmath_vec2_length_squared, 2, k_jmath_vec2_length_squared, JOY_V_F64 },
        { "jmath", "jmath_vec2_length", "JmathVec2Length", t_jmath_vec2_length, 2, k_jmath_vec2_length, JOY_V_F64 },
        { "jmath", "jmath_vec2_normalize_x", "JmathVec2NormalizeX", t_jmath_vec2_normalize_x, 2, k_jmath_vec2_normalize_x, JOY_V_F64 },
        { "jmath", "jmath_vec2_normalize_y", "JmathVec2NormalizeY", t_jmath_vec2_normalize_y, 2, k_jmath_vec2_normalize_y, JOY_V_F64 },
        { "jmath", "jmath_vec2_normalize", "JmathVec2Normalize", t_jmath_vec2_normalize, 2, k_jmath_vec2_normalize, JOY_V_U64 },
        { "jmath", "jmath_vec2_normal_x", "JmathVec2NormalX", t_jmath_vec2_normal_x, 2, k_jmath_vec2_normal_x, JOY_V_F64 },
        { "jmath", "jmath_vec2_normal_y", "JmathVec2NormalY", t_jmath_vec2_normal_y, 2, k_jmath_vec2_normal_y, JOY_V_F64 },
        { "jmath", "jmath_vec2_normal", "JmathVec2Normal", t_jmath_vec2_normal, 2, k_jmath_vec2_normal, JOY_V_U64 },
        { "jmath", "jmath_vec2_rotate_x", "JmathVec2RotateX", t_jmath_vec2_rotate_x, 3, k_jmath_vec2_rotate_x, JOY_V_F64 },
        { "jmath", "jmath_vec2_rotate_y", "JmathVec2RotateY", t_jmath_vec2_rotate_y, 3, k_jmath_vec2_rotate_y, JOY_V_F64 },
        { "jmath", "jmath_vec2_rotate", "JmathVec2Rotate", t_jmath_vec2_rotate, 3, k_jmath_vec2_rotate, JOY_V_U64 },
        { "jmath", "jmath_vec2_lerp_x", "JmathVec2LerpX", t_jmath_vec2_lerp_x, 5, k_jmath_vec2_lerp_x, JOY_V_F64 },
        { "jmath", "jmath_vec2_lerp_y", "JmathVec2LerpY", t_jmath_vec2_lerp_y, 5, k_jmath_vec2_lerp_y, JOY_V_F64 },
        { "jmath", "jmath_vec2_lerp", "JmathVec2Lerp", t_jmath_vec2_lerp, 5, k_jmath_vec2_lerp, JOY_V_U64 },
        { "jmath", "jmath_vec2_distance", "JmathVec2Distance", t_jmath_vec2_distance, 4, k_jmath_vec2_distance, JOY_V_F64 },
        { "jmath", "jmath_vec3_negate_x", "JmathVec3NegateX", t_jmath_vec3_negate_x, 3, k_jmath_vec3_negate_x, JOY_V_F64 },
        { "jmath", "jmath_vec3_negate_y", "JmathVec3NegateY", t_jmath_vec3_negate_y, 3, k_jmath_vec3_negate_y, JOY_V_F64 },
        { "jmath", "jmath_vec3_negate_z", "JmathVec3NegateZ", t_jmath_vec3_negate_z, 3, k_jmath_vec3_negate_z, JOY_V_F64 },
        { "jmath", "jmath_vec3_add_x", "JmathVec3AddX", t_jmath_vec3_add_x, 6, k_jmath_vec3_add_x, JOY_V_F64 },
        { "jmath", "jmath_vec3_add_y", "JmathVec3AddY", t_jmath_vec3_add_y, 6, k_jmath_vec3_add_y, JOY_V_F64 },
        { "jmath", "jmath_vec3_add_z", "JmathVec3AddZ", t_jmath_vec3_add_z, 6, k_jmath_vec3_add_z, JOY_V_F64 },
        { "jmath", "jmath_vec3_sub_x", "JmathVec3SubX", t_jmath_vec3_sub_x, 6, k_jmath_vec3_sub_x, JOY_V_F64 },
        { "jmath", "jmath_vec3_sub_y", "JmathVec3SubY", t_jmath_vec3_sub_y, 6, k_jmath_vec3_sub_y, JOY_V_F64 },
        { "jmath", "jmath_vec3_sub_z", "JmathVec3SubZ", t_jmath_vec3_sub_z, 6, k_jmath_vec3_sub_z, JOY_V_F64 },
        { "jmath", "jmath_vec3_scale_x", "JmathVec3ScaleX", t_jmath_vec3_scale_x, 4, k_jmath_vec3_scale_x, JOY_V_F64 },
        { "jmath", "jmath_vec3_scale_y", "JmathVec3ScaleY", t_jmath_vec3_scale_y, 4, k_jmath_vec3_scale_y, JOY_V_F64 },
        { "jmath", "jmath_vec3_scale_z", "JmathVec3ScaleZ", t_jmath_vec3_scale_z, 4, k_jmath_vec3_scale_z, JOY_V_F64 },
        { "jmath", "jmath_vec3_abs_x", "JmathVec3AbsX", t_jmath_vec3_abs_x, 3, k_jmath_vec3_abs_x, JOY_V_F64 },
        { "jmath", "jmath_vec3_abs_y", "JmathVec3AbsY", t_jmath_vec3_abs_y, 3, k_jmath_vec3_abs_y, JOY_V_F64 },
        { "jmath", "jmath_vec3_abs_z", "JmathVec3AbsZ", t_jmath_vec3_abs_z, 3, k_jmath_vec3_abs_z, JOY_V_F64 },
        { "jmath", "jmath_vec3_dot", "JmathVec3Dot", t_jmath_vec3_dot, 6, k_jmath_vec3_dot, JOY_V_F64 },
        { "jmath", "jmath_vec3_cross_x", "JmathVec3CrossX", t_jmath_vec3_cross_x, 6, k_jmath_vec3_cross_x, JOY_V_F64 },
        { "jmath", "jmath_vec3_cross_y", "JmathVec3CrossY", t_jmath_vec3_cross_y, 6, k_jmath_vec3_cross_y, JOY_V_F64 },
        { "jmath", "jmath_vec3_cross_z", "JmathVec3CrossZ", t_jmath_vec3_cross_z, 6, k_jmath_vec3_cross_z, JOY_V_F64 },
        { "jmath", "jmath_vec3_square_length", "JmathVec3SquareLength", t_jmath_vec3_square_length, 3, k_jmath_vec3_square_length, JOY_V_F64 },
        { "jmath", "jmath_vec3_length", "JmathVec3Length", t_jmath_vec3_length, 3, k_jmath_vec3_length, JOY_V_F64 },
        { "jmath", "jmath_vec3_normalize_x", "JmathVec3NormalizeX", t_jmath_vec3_normalize_x, 3, k_jmath_vec3_normalize_x, JOY_V_F64 },
        { "jmath", "jmath_vec3_normalize_y", "JmathVec3NormalizeY", t_jmath_vec3_normalize_y, 3, k_jmath_vec3_normalize_y, JOY_V_F64 },
        { "jmath", "jmath_vec3_normalize_z", "JmathVec3NormalizeZ", t_jmath_vec3_normalize_z, 3, k_jmath_vec3_normalize_z, JOY_V_F64 },
        { "jmath", "jmath_vec3_lerp_x", "JmathVec3LerpX", t_jmath_vec3_lerp_x, 7, k_jmath_vec3_lerp_x, JOY_V_F64 },
        { "jmath", "jmath_vec3_lerp_y", "JmathVec3LerpY", t_jmath_vec3_lerp_y, 7, k_jmath_vec3_lerp_y, JOY_V_F64 },
        { "jmath", "jmath_vec3_lerp_z", "JmathVec3LerpZ", t_jmath_vec3_lerp_z, 7, k_jmath_vec3_lerp_z, JOY_V_F64 },
        { "jmath", "jmath_vec3_distance", "JmathVec3Distance", t_jmath_vec3_distance, 6, k_jmath_vec3_distance, JOY_V_F64 },
        { "jmath", "jmath_mat22_mul_vec2_x", "JmathMat22MulVec2X", t_jmath_mat22_mul_vec2_x, 6, k_jmath_mat22_mul_vec2_x, JOY_V_F64 },
        { "jmath", "jmath_mat22_mul_vec2_y", "JmathMat22MulVec2Y", t_jmath_mat22_mul_vec2_y, 6, k_jmath_mat22_mul_vec2_y, JOY_V_F64 },
        { "jmath", "jmath_mat22_mul_vec2", "JmathMat22MulVec2", t_jmath_mat22_mul_vec2, 6, k_jmath_mat22_mul_vec2, JOY_V_U64 },
        { "jmath", "jmath_mat22_rotate_col1_x", "JmathMat22RotateCol1X", t_jmath_mat22_rotate_col1_x, 1, k_jmath_mat22_rotate_col1_x, JOY_V_F64 },
        { "jmath", "jmath_mat22_rotate_col1_y", "JmathMat22RotateCol1Y", t_jmath_mat22_rotate_col1_y, 1, k_jmath_mat22_rotate_col1_y, JOY_V_F64 },
        { "jmath", "jmath_mat22_rotate_col2_x", "JmathMat22RotateCol2X", t_jmath_mat22_rotate_col2_x, 1, k_jmath_mat22_rotate_col2_x, JOY_V_F64 },
        { "jmath", "jmath_mat22_rotate_col2_y", "JmathMat22RotateCol2Y", t_jmath_mat22_rotate_col2_y, 1, k_jmath_mat22_rotate_col2_y, JOY_V_F64 },
        { "jmath", "jmath_mat22_transpose_col1_x", "JmathMat22TransposeCol1X", t_jmath_mat22_transpose_col1_x, 4, k_jmath_mat22_transpose_col1_x, JOY_V_F64 },
        { "jmath", "jmath_mat22_transpose_col1_y", "JmathMat22TransposeCol1Y", t_jmath_mat22_transpose_col1_y, 4, k_jmath_mat22_transpose_col1_y, JOY_V_F64 },
        { "jmath", "jmath_mat22_transpose_col2_x", "JmathMat22TransposeCol2X", t_jmath_mat22_transpose_col2_x, 4, k_jmath_mat22_transpose_col2_x, JOY_V_F64 },
        { "jmath", "jmath_mat22_transpose_col2_y", "JmathMat22TransposeCol2Y", t_jmath_mat22_transpose_col2_y, 4, k_jmath_mat22_transpose_col2_y, JOY_V_F64 },
        { "jmath", "jmath_mat22_invert_col1_x", "JmathMat22InvertCol1X", t_jmath_mat22_invert_col1_x, 4, k_jmath_mat22_invert_col1_x, JOY_V_F64 },
        { "jmath", "jmath_mat22_invert_col1_y", "JmathMat22InvertCol1Y", t_jmath_mat22_invert_col1_y, 4, k_jmath_mat22_invert_col1_y, JOY_V_F64 },
        { "jmath", "jmath_mat22_invert_col2_x", "JmathMat22InvertCol2X", t_jmath_mat22_invert_col2_x, 4, k_jmath_mat22_invert_col2_x, JOY_V_F64 },
        { "jmath", "jmath_mat22_invert_col2_y", "JmathMat22InvertCol2Y", t_jmath_mat22_invert_col2_y, 4, k_jmath_mat22_invert_col2_y, JOY_V_F64 },
        { "jmath", "jmath_mat22_add_col1_x", "JmathMat22AddCol1X", t_jmath_mat22_add_col1_x, 8, k_jmath_mat22_add_col1_x, JOY_V_F64 },
        { "jmath", "jmath_mat22_add_col1_y", "JmathMat22AddCol1Y", t_jmath_mat22_add_col1_y, 8, k_jmath_mat22_add_col1_y, JOY_V_F64 },
        { "jmath", "jmath_mat22_add_col2_x", "JmathMat22AddCol2X", t_jmath_mat22_add_col2_x, 8, k_jmath_mat22_add_col2_x, JOY_V_F64 },
        { "jmath", "jmath_mat22_add_col2_y", "JmathMat22AddCol2Y", t_jmath_mat22_add_col2_y, 8, k_jmath_mat22_add_col2_y, JOY_V_F64 },
        { "jmath", "jmath_mat22_mul_col1_x", "JmathMat22MulCol1X", t_jmath_mat22_mul_col1_x, 8, k_jmath_mat22_mul_col1_x, JOY_V_F64 },
        { "jmath", "jmath_mat22_mul_col1_y", "JmathMat22MulCol1Y", t_jmath_mat22_mul_col1_y, 8, k_jmath_mat22_mul_col1_y, JOY_V_F64 },
        { "jmath", "jmath_mat22_mul_col2_x", "JmathMat22MulCol2X", t_jmath_mat22_mul_col2_x, 8, k_jmath_mat22_mul_col2_x, JOY_V_F64 },
        { "jmath", "jmath_mat22_mul_col2_y", "JmathMat22MulCol2Y", t_jmath_mat22_mul_col2_y, 8, k_jmath_mat22_mul_col2_y, JOY_V_F64 },
        { "jmath", "jmath_mat22_abs_col1_x", "JmathMat22AbsCol1X", t_jmath_mat22_abs_col1_x, 4, k_jmath_mat22_abs_col1_x, JOY_V_F64 },
        { "jmath", "jmath_mat22_abs_col1_y", "JmathMat22AbsCol1Y", t_jmath_mat22_abs_col1_y, 4, k_jmath_mat22_abs_col1_y, JOY_V_F64 },
        { "jmath", "jmath_mat22_abs_col2_x", "JmathMat22AbsCol2X", t_jmath_mat22_abs_col2_x, 4, k_jmath_mat22_abs_col2_x, JOY_V_F64 },
        { "jmath", "jmath_mat22_abs_col2_y", "JmathMat22AbsCol2Y", t_jmath_mat22_abs_col2_y, 4, k_jmath_mat22_abs_col2_y, JOY_V_F64 },
        { "jmath", "jmath_vec3_div_x", "JmathVec3DivX", t_jmath_vec3_div_x, 4, k_jmath_vec3_div_x, JOY_V_F64 },
        { "jmath", "jmath_vec3_div_y", "JmathVec3DivY", t_jmath_vec3_div_y, 4, k_jmath_vec3_div_y, JOY_V_F64 },
        { "jmath", "jmath_vec3_div_z", "JmathVec3DivZ", t_jmath_vec3_div_z, 4, k_jmath_vec3_div_z, JOY_V_F64 },
        { "jmath", "jmath_ft_radians", "JmathFtRadians", t_jmath_ft_radians, 1, k_jmath_ft_radians, JOY_V_F64 },
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
        joy_value_t args[13];
        int i;

        /* 每条开放的 API 都必须在注册表里且 argc 守卫生效。
         * 故意传错 argc：thunk 第一行就返回，不执行任何 joy 业务函数，
         * 因此批量开放有副作用 / 需要上下文的 API 也不会在这里被调用。 */
        st = JOY_OK;
        joy_api_call("joy_core_lcg_random_create", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_lcg_random_create";
        st = JOY_OK;
        joy_api_call("joy_core_lcg_random_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_lcg_random_destroy";
        st = JOY_OK;
        joy_api_call("joy_core_lcg_random_next", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_lcg_random_next";
        st = JOY_OK;
        joy_api_call("joy_core_lcg_random_set_state", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_lcg_random_set_state";
        st = JOY_OK;
        joy_api_call("jpm_config_add_spawn", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jpm_config_add_spawn";
        st = JOY_OK;
        joy_api_call("jpm_wave_seed", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jpm_wave_seed";
        st = JOY_OK;
        joy_api_call("joy_core_sdbm_hash", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_sdbm_hash";
        st = JOY_OK;
        joy_api_call("joy_core_fnv1a_hash", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_fnv1a_hash";
        st = JOY_OK;
        joy_api_call("joy_core_jenkins_hash", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_jenkins_hash";
        st = JOY_OK;
        joy_api_call("joy_core_murmur3_hash", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_murmur3_hash";
        st = JOY_OK;
        joy_api_call("joy_core_kr_hash", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_kr_hash";
        st = JOY_OK;
        joy_api_call("joy_scene_jtimer_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_scene_jtimer_destroy";
        st = JOY_OK;
        joy_api_call("joy_scene_jtimer_trigger", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_scene_jtimer_trigger";
        st = JOY_OK;
        joy_api_call("joy_scene_jtimer_set_interval", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_scene_jtimer_set_interval";
        st = JOY_OK;
        joy_api_call("joy_scene_jtimer_set_active", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_scene_jtimer_set_active";
        st = JOY_OK;
        joy_api_call("joy_scene_jtimer_reset", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_scene_jtimer_reset";
        st = JOY_OK;
        joy_api_call("joy_scene_jtimer_get_elapsed", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_scene_jtimer_get_elapsed";
        st = JOY_OK;
        joy_api_call("joy_core_init_env", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_init_env";
        st = JOY_OK;
        joy_api_call("joy_core_quit_env", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_quit_env";
        st = JOY_OK;
        joy_api_call("joy_core_get_ticks", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_get_ticks";
        st = JOY_OK;
        joy_api_call("joy_core_create_window", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_create_window";
        st = JOY_OK;
        joy_api_call("joy_core_destroy_window", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_destroy_window";
        st = JOY_OK;
        joy_api_call("joy_core_window_set_fullscreen", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_window_set_fullscreen";
        st = JOY_OK;
        joy_api_call("joy_core_window_get_fullscreen", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_window_get_fullscreen";
        st = JOY_OK;
        joy_api_call("joy_core_window_set_icon", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_window_set_icon";
        st = JOY_OK;
        joy_api_call("joy_core_push_user_event", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_push_user_event";
        st = JOY_OK;
        joy_api_call("joy_core_push_quit_event", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_push_quit_event";
        st = JOY_OK;
        joy_api_call("joy_core_log_title", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_log_title";
        st = JOY_OK;
        joy_api_call("joy_core_env_set", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_env_set";
        st = JOY_OK;
        joy_api_call("joy_core_env_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_env_destroy";
        st = JOY_OK;
        joy_api_call("joy_core_simple_fps_destory", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_simple_fps_destory";
        st = JOY_OK;
        joy_api_call("joy_core_simple_fps_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_simple_fps_update";
        st = JOY_OK;
        joy_api_call("joy_core_simple_fps_value", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_simple_fps_value";
        st = JOY_OK;
        joy_api_call("joy_scene_game_timer_init", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_scene_game_timer_init";
        st = JOY_OK;
        joy_api_call("joy_scene_game_timer_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_scene_game_timer_update";
        st = JOY_OK;
        joy_api_call("joy_scene_game_timer_get_delta_time", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_scene_game_timer_get_delta_time";
        st = JOY_OK;
        joy_api_call("joy_scene_game_timer_get_unscaled_delta_time", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_scene_game_timer_get_unscaled_delta_time";
        st = JOY_OK;
        joy_api_call("joy_scene_game_timer_set_time_scale", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_scene_game_timer_set_time_scale";
        st = JOY_OK;
        joy_api_call("joy_scene_game_timer_get_time_scale", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_scene_game_timer_get_time_scale";
        st = JOY_OK;
        joy_api_call("joy_scene_game_timer_set_target_fps", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_scene_game_timer_set_target_fps";
        st = JOY_OK;
        joy_api_call("joy_scene_game_timer_get_target_fps", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_scene_game_timer_get_target_fps";
        st = JOY_OK;
        joy_api_call("joy_core_set_clipboard_text", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_core_set_clipboard_text";
        st = JOY_OK;
        joy_api_call("sys_init_netenv", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_init_netenv";
        st = JOY_OK;
        joy_api_call("sys_release_netenv", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_release_netenv";
        st = JOY_OK;
        joy_api_call("sys_anyaddr", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_anyaddr";
        st = JOY_OK;
        joy_api_call("sys_closesocket", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_closesocket";
        st = JOY_OK;
        joy_api_call("sys_set_sock_sndtimeo", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_set_sock_sndtimeo";
        st = JOY_OK;
        joy_api_call("sys_set_sock_rcvtimeo", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_set_sock_rcvtimeo";
        st = JOY_OK;
        joy_api_call("sys_set_sock_accpettimeo", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_set_sock_accpettimeo";
        st = JOY_OK;
        joy_api_call("sys_set_sock_broadcast", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_set_sock_broadcast";
        st = JOY_OK;
        joy_api_call("sys_set_sock_reuseaddr", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_set_sock_reuseaddr";
        st = JOY_OK;
        joy_api_call("sys_tcp", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_tcp";
        st = JOY_OK;
        joy_api_call("sys_udp", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_udp";
        st = JOY_OK;
        joy_api_call("sys_bind", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_bind";
        st = JOY_OK;
        joy_api_call("sys_connect", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_connect";
        st = JOY_OK;
        joy_api_call("sys_listen", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_listen";
        st = JOY_OK;
        joy_api_call("sys_send", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_send";
        st = JOY_OK;
        joy_api_call("sys_sendto", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_sendto";
        st = JOY_OK;
        joy_api_call("sys_current_time", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_current_time";
        st = JOY_OK;
        joy_api_call("sys_sleep", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_sleep";
        st = JOY_OK;
        joy_api_call("sys_install_crash_handlers", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/sys_install_crash_handlers";
        st = JOY_OK;
        joy_api_call("joy_net_iocp_server_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_iocp_server_destroy";
        st = JOY_OK;
        joy_api_call("joy_net_iocp_process_events", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_iocp_process_events";
        st = JOY_OK;
        joy_api_call("joy_r2d_font_create", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_font_create";
        st = JOY_OK;
        joy_api_call("joy_r2d_font_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_font_destroy";
        st = JOY_OK;
        joy_api_call("joy_r2d_font_get_scale", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_font_get_scale";
        st = JOY_OK;
        joy_api_call("joy_r2d_font_get_size", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_font_get_size";
        st = JOY_OK;
        joy_api_call("joy_r2d_font_get_ascent", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_font_get_ascent";
        st = JOY_OK;
        joy_api_call("joy_r2d_create", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_create";
        st = JOY_OK;
        joy_api_call("joy_r2d_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_destroy";
        st = JOY_OK;
        joy_api_call("joy_r2d_set_color", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_set_color";
        st = JOY_OK;
        joy_api_call("joy_r2d_begin", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_begin";
        st = JOY_OK;
        joy_api_call("joy_r2d_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_end";
        st = JOY_OK;
        joy_api_call("joy_r2d_set_logical_presentation", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_set_logical_presentation";
        st = JOY_OK;
        joy_api_call("joy_r2d_set_logical_presentation_mode", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_set_logical_presentation_mode";
        st = JOY_OK;
        joy_api_call("joy_r2d_set_vsync", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_set_vsync";
        st = JOY_OK;
        joy_api_call("joy_r2d_set_scale", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_set_scale";
        st = JOY_OK;
        joy_api_call("joy_r2d_set_colorf", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_set_colorf";
        st = JOY_OK;
        joy_api_call("joy_r2d_set_blend_mode", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_set_blend_mode";
        st = JOY_OK;
        joy_api_call("joy_r2d_draw_point", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_draw_point";
        st = JOY_OK;
        joy_api_call("joy_r2d_set_clip_rect", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_set_clip_rect";
        st = JOY_OK;
        joy_api_call("joy_r2d_clear_clip", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_clear_clip";
        st = JOY_OK;
        joy_api_call("joy_r2d_push_clip_rect", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_push_clip_rect";
        st = JOY_OK;
        joy_api_call("joy_r2d_pop_clip_rect", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_pop_clip_rect";
        st = JOY_OK;
        joy_api_call("joy_r2d_reset_clip_stack", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_reset_clip_stack";
        st = JOY_OK;
        joy_api_call("joy_r2d_point_in_clip", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_point_in_clip";
        st = JOY_OK;
        joy_api_call("shape_draw_line", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/shape_draw_line";
        st = JOY_OK;
        joy_api_call("shape_draw_rectangle", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/shape_draw_rectangle";
        st = JOY_OK;
        joy_api_call("joy_ui_image_create", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_image_create";
        st = JOY_OK;
        joy_api_call("joy_ui_image_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_image_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_image_set_alpha", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_image_set_alpha";
        st = JOY_OK;
        joy_api_call("joy_ui_image_set_blend_mode", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_image_set_blend_mode";
        st = JOY_OK;
        joy_api_call("joy_ui_image_set_scale_mode", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_image_set_scale_mode";
        st = JOY_OK;
        joy_api_call("joy_ui_text_updatex", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_text_updatex";
        st = JOY_OK;
        joy_api_call("joy_ui_text_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_text_destroy";
        st = JOY_OK;
        joy_api_call("joy_r2d_jtext_print", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_jtext_print";
        st = JOY_OK;
        joy_api_call("joy_r2d_jtext_printx", NULL, 11, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_jtext_printx";
        st = JOY_OK;
        joy_api_call("joy_r2d_draw_point_f", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_draw_point_f";
        st = JOY_OK;
        joy_api_call("shape_draw_rectangle_f", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/shape_draw_rectangle_f";
        st = JOY_OK;
        joy_api_call("shape_draw_line_f", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/shape_draw_line_f";
        st = JOY_OK;
        joy_api_call("joy_r2d_jtext_printf", NULL, 11, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_jtext_printf";
        st = JOY_OK;
        joy_api_call("joy_r2d_jtext_printf_scaled", NULL, 12, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_jtext_printf_scaled";
        st = JOY_OK;
        joy_api_call("joy_ui_text_get_width", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_text_get_width";
        st = JOY_OK;
        joy_api_call("joy_ui_text_get_height", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_text_get_height";
        st = JOY_OK;
        joy_api_call("joy_r2d_jtext_cache_clear", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_jtext_cache_clear";
        st = JOY_OK;
        joy_api_call("joy_r2d_jtext_cache_set_limit", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_jtext_cache_set_limit";
        st = JOY_OK;
        joy_api_call("joy_r2d_jtext_cache_get_count", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_jtext_cache_get_count";
        st = JOY_OK;
        joy_api_call("joy_r2d_jtext_cache_sweep", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_jtext_cache_sweep";
        st = JOY_OK;
        joy_api_call("animation_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/animation_destroy";
        st = JOY_OK;
        joy_api_call("animation_reset", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/animation_reset";
        st = JOY_OK;
        joy_api_call("animation_add_clip", NULL, 8, &st);
        if (st != JOY_ERR_ARGC) return "registry/animation_add_clip";
        st = JOY_OK;
        joy_api_call("animation_is_finished", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/animation_is_finished";
        st = JOY_OK;
        joy_api_call("animation_set_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/animation_set_position";
        st = JOY_OK;
        joy_api_call("animation_set_scale", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/animation_set_scale";
        st = JOY_OK;
        joy_api_call("animation_set_rotation", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/animation_set_rotation";
        st = JOY_OK;
        joy_api_call("animation_update", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/animation_update";
        st = JOY_OK;
        joy_api_call("joy_r3d_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_destroy";
        st = JOY_OK;
        joy_api_call("joy_r3d_begin", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_begin";
        st = JOY_OK;
        joy_api_call("joy_r3d_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_end";
        st = JOY_OK;
        joy_api_call("joy_r3d_get_draw_calls", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_get_draw_calls";
        st = JOY_OK;
        joy_api_call("joy_r3d_set_clear_color", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_set_clear_color";
        st = JOY_OK;
        joy_api_call("joy_r3d_mesh_destroy", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_mesh_destroy";
        st = JOY_OK;
        joy_api_call("joy_r3d_font_init", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_font_init";
        st = JOY_OK;
        joy_api_call("joy_r3d_font_draw_2d", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_font_draw_2d";
        st = JOY_OK;
        joy_api_call("joy_r3d_font_cache_set_limit", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_font_cache_set_limit";
        st = JOY_OK;
        joy_api_call("joy_r3d_font_cache_get_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_font_cache_get_count";
        st = JOY_OK;
        joy_api_call("joy_r3d_font_cache_clear", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_font_cache_clear";
        st = JOY_OK;
        joy_api_call("joy_r3d_font_cache_sweep", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_font_cache_sweep";
        st = JOY_OK;
        joy_api_call("joy_r3d_set_logical_presentation", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_set_logical_presentation";
        st = JOY_OK;
        joy_api_call("joy_r3d_set_logical_presentation_mode", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_set_logical_presentation_mode";
        st = JOY_OK;
        joy_api_call("joy_r3d_button_draw", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_button_draw";
        st = JOY_OK;
        joy_api_call("joy_r3d_button_draw_circle", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_button_draw_circle";
        st = JOY_OK;
        joy_api_call("joy_r3d_button_is_hovered", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_button_is_hovered";
        st = JOY_OK;
        joy_api_call("joy_r3d_button_is_pressed", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_button_is_pressed";
        st = JOY_OK;
        joy_api_call("joy_r3d_button_release", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_button_release";
        st = JOY_OK;
        joy_api_call("joy_r3d_button_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_button_destroy";
        st = JOY_OK;
        joy_api_call("joy_r3d_label_set_text", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_label_set_text";
        st = JOY_OK;
        joy_api_call("joy_r3d_label_draw", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_label_draw";
        st = JOY_OK;
        joy_api_call("joy_r3d_label_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_label_destroy";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_set_text", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_set_text";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_is_hovered", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_is_hovered";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_focus", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_focus";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_defocus", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_defocus";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_is_focused", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_is_focused";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_start_text_input", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_start_text_input";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_stop_text_input", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_stop_text_input";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_insert", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_insert";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_backspace", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_backspace";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_delete", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_delete";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_cursor_left", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_cursor_left";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_cursor_right", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_cursor_right";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_cursor_home", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_cursor_home";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_cursor_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_cursor_end";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_click", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_click";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_submit", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_submit";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_draw", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_draw";
        st = JOY_OK;
        joy_api_call("joy_r3d_input_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_input_destroy";
        st = JOY_OK;
        joy_api_call("joy_r3d_combobox_input_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_combobox_input_destroy";
        st = JOY_OK;
        joy_api_call("joy_r3d_combobox_input_add_item", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_combobox_input_add_item";
        st = JOY_OK;
        joy_api_call("joy_r3d_combobox_input_clear_items", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_combobox_input_clear_items";
        st = JOY_OK;
        joy_api_call("joy_r3d_combobox_input_set_text", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_combobox_input_set_text";
        st = JOY_OK;
        joy_api_call("joy_r3d_combobox_input_hit_item", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_combobox_input_hit_item";
        st = JOY_OK;
        joy_api_call("joy_r3d_combobox_input_draw", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_combobox_input_draw";
        st = JOY_OK;
        joy_api_call("joy_r3d_combobox_input_draw_dropdown", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_combobox_input_draw_dropdown";
        st = JOY_OK;
        joy_api_call("joy_r3d_slider_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_slider_destroy";
        st = JOY_OK;
        joy_api_call("joy_r3d_slider_set_value", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_slider_set_value";
        st = JOY_OK;
        joy_api_call("joy_r3d_slider_get_value", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_slider_get_value";
        st = JOY_OK;
        joy_api_call("joy_r3d_slider_draw", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_slider_draw";
        st = JOY_OK;
        joy_api_call("joy_r3d_slider_is_pressed", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_slider_is_pressed";
        st = JOY_OK;
        joy_api_call("joy_r3d_slider_set_pressed", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_slider_set_pressed";
        st = JOY_OK;
        joy_api_call("joy_r3d_slider_drag_move", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_slider_drag_move";
        st = JOY_OK;
        joy_api_call("joy_r3d_slider_is_hovered", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_slider_is_hovered";
        st = JOY_OK;
        joy_api_call("joy_r3d_slider_set_hovered", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_slider_set_hovered";
        st = JOY_OK;
        joy_api_call("joy_r3d_checkbox_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_checkbox_destroy";
        st = JOY_OK;
        joy_api_call("joy_r3d_checkbox_toggle", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_checkbox_toggle";
        st = JOY_OK;
        joy_api_call("joy_r3d_checkbox_is_pressed", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_checkbox_is_pressed";
        st = JOY_OK;
        joy_api_call("joy_r3d_checkbox_set_hovered", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_checkbox_set_hovered";
        st = JOY_OK;
        joy_api_call("joy_r3d_checkbox_is_hovered", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_checkbox_is_hovered";
        st = JOY_OK;
        joy_api_call("joy_r3d_checkbox_set_colors", NULL, 14, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_checkbox_set_colors";
        st = JOY_OK;
        joy_api_call("joy_r3d_checkbox_draw", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_checkbox_draw";
        st = JOY_OK;
        joy_api_call("joy_r3d_draw_grid", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_draw_grid";
        st = JOY_OK;
        joy_api_call("janimator_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_destroy";
        st = JOY_OK;
        joy_api_call("janimator_set_float", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_set_float";
        st = JOY_OK;
        joy_api_call("janimator_set_int", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_set_int";
        st = JOY_OK;
        joy_api_call("janimator_set_bool", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_set_bool";
        st = JOY_OK;
        joy_api_call("janimator_set_trigger", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_set_trigger";
        st = JOY_OK;
        joy_api_call("janimator_get_float", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_get_float";
        st = JOY_OK;
        joy_api_call("janimator_get_int", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_get_int";
        st = JOY_OK;
        joy_api_call("janimator_get_bool", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_get_bool";
        st = JOY_OK;
        joy_api_call("janimator_add_state", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_add_state";
        st = JOY_OK;
        joy_api_call("janimator_set_default_state", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_set_default_state";
        st = JOY_OK;
        joy_api_call("janimator_state_add_clip", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_state_add_clip";
        st = JOY_OK;
        joy_api_call("janimator_state_add_color", NULL, 10, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_state_add_color";
        st = JOY_OK;
        joy_api_call("janimator_state_set_loop", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_state_set_loop";
        st = JOY_OK;
        joy_api_call("janimator_state_set_speed", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_state_set_speed";
        st = JOY_OK;
        joy_api_call("janimator_state_set_blend", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_state_set_blend";
        st = JOY_OK;
        joy_api_call("janimator_state_add_motion", NULL, 10, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_state_add_motion";
        st = JOY_OK;
        joy_api_call("janimator_add_transition", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_add_transition";
        st = JOY_OK;
        joy_api_call("janimator_add_any_transition", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_add_any_transition";
        st = JOY_OK;
        joy_api_call("janimator_transition_add_cond", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_transition_add_cond";
        st = JOY_OK;
        joy_api_call("janimator_transition_set_exit_time", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_transition_set_exit_time";
        st = JOY_OK;
        joy_api_call("janimator_update", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_update";
        st = JOY_OK;
        joy_api_call("janimator_draw", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_draw";
        st = JOY_OK;
        joy_api_call("janimator_set_flip_h", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/janimator_set_flip_h";
        st = JOY_OK;
        joy_api_call("joy_ui_label_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_label_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_label_draw", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_label_draw";
        st = JOY_OK;
        joy_api_call("joy_ui_button_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_set_position";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_size", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_set_size";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_normal_color", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_set_normal_color";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_hover_color", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_set_hover_color";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_pressed_color", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_set_pressed_color";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_image", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_set_image";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_pressed_image", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_set_pressed_image";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_textx", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_set_textx";
        st = JOY_OK;
        joy_api_call("joy_ui_button_is_hovered", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_is_hovered";
        st = JOY_OK;
        joy_api_call("joy_ui_button_is_pressed", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_is_pressed";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_hovered", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_set_hovered";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_pressed", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_set_pressed";
        st = JOY_OK;
        joy_api_call("joy_ui_button_get_pressed", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_get_pressed";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_text_offset", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_set_text_offset";
        st = JOY_OK;
        joy_api_call("joy_ui_button_draw", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button_draw";
        st = JOY_OK;
        joy_api_call("joy_ui_scrollview_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scrollview_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_scrollview_set_view_px", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scrollview_set_view_px";
        st = JOY_OK;
        joy_api_call("joy_ui_scrollview_set_content", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scrollview_set_content";
        st = JOY_OK;
        joy_api_call("joy_ui_scrollview_set_offset", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scrollview_set_offset";
        st = JOY_OK;
        joy_api_call("joy_ui_scrollview_clamp", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scrollview_clamp";
        st = JOY_OK;
        joy_api_call("joy_ui_scrollview_get_offset_x", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scrollview_get_offset_x";
        st = JOY_OK;
        joy_api_call("joy_ui_scrollview_get_offset_y", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scrollview_get_offset_y";
        st = JOY_OK;
        joy_api_call("joy_ui_scrollview_set_show_bar", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scrollview_set_show_bar";
        st = JOY_OK;
        joy_api_call("joy_ui_scrollview_dragging", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scrollview_dragging";
        st = JOY_OK;
        joy_api_call("joy_ui_scrollview_hit", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scrollview_hit";
        st = JOY_OK;
        joy_api_call("joy_ui_scrollview_begin", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scrollview_begin";
        st = JOY_OK;
        joy_api_call("joy_ui_scrollview_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scrollview_end";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_add_item", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_add_item";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_is_pressed", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_is_pressed";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_is_open", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_is_open";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_set_open", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_set_open";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_hit_item", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_hit_item";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_set_selected", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_set_selected";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_get_selected", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_get_selected";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_draw", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_draw";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_input_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_input_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_input_add_item", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_input_add_item";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_input_clear_items", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_input_clear_items";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_input_set_text", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_input_set_text";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_input_draw", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_input_draw";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_input_draw_dropdown", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_input_draw_dropdown";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_input_is_open", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_input_is_open";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_input_set_popup_node", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_input_set_popup_node";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_input_hit_item", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_input_hit_item";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_input_set_open", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_combobox_input_set_open";
        st = JOY_OK;
        joy_api_call("joy_ui_datagrid_setheaders", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_datagrid_setheaders";
        st = JOY_OK;
        joy_api_call("joy_ui_datagrid_setheader", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_datagrid_setheader";
        st = JOY_OK;
        joy_api_call("joy_ui_datagrid_setcell", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_datagrid_setcell";
        st = JOY_OK;
        joy_api_call("joy_ui_datagrid_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_datagrid_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_datagrid_draw", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_datagrid_draw";
        st = JOY_OK;
        joy_api_call("joy_ui_datagrid_hit_row", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_datagrid_hit_row";
        st = JOY_OK;
        joy_api_call("joy_ui_datagrid_scroll", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_datagrid_scroll";
        st = JOY_OK;
        joy_api_call("joy_ui_datagrid_set_selected_row", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_datagrid_set_selected_row";
        st = JOY_OK;
        joy_api_call("joy_ui_radio_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_radio_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_radio_is_pressed", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_radio_is_pressed";
        st = JOY_OK;
        joy_api_call("joy_ui_radio_set_hovered", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_radio_set_hovered";
        st = JOY_OK;
        joy_api_call("joy_ui_checkbox_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_checkbox_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_checkbox_set_colors", NULL, 14, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_checkbox_set_colors";
        st = JOY_OK;
        joy_api_call("joy_ui_checkbox_set_r2d", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_checkbox_set_r2d";
        st = JOY_OK;
        joy_api_call("joy_ui_checkbox_set_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_checkbox_set_position";
        st = JOY_OK;
        joy_api_call("joy_ui_checkbox_set_text_color", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_checkbox_set_text_color";
        st = JOY_OK;
        joy_api_call("joy_ui_checkbox_toggle", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_checkbox_toggle";
        st = JOY_OK;
        joy_api_call("joy_ui_checkbox_is_pressed", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_checkbox_is_pressed";
        st = JOY_OK;
        joy_api_call("joy_ui_checkbox_set_hovered", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_checkbox_set_hovered";
        st = JOY_OK;
        joy_api_call("joy_input_joystick_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_input_joystick_destroy";
        st = JOY_OK;
        joy_api_call("joy_input_joystick_set_textures", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_input_joystick_set_textures";
        st = JOY_OK;
        joy_api_call("joy_input_joystick_set_vector", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_input_joystick_set_vector";
        st = JOY_OK;
        joy_api_call("joy_input_joystick_draw", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_input_joystick_draw";
        st = JOY_OK;
        joy_api_call("joy_input_joystick_reset", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_input_joystick_reset";
        st = JOY_OK;
        joy_api_call("joy_input_joystick_set_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_input_joystick_set_position";
        st = JOY_OK;
        joy_api_call("joy_input_joystick_set_radius", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_input_joystick_set_radius";
        st = JOY_OK;
        joy_api_call("joy_input_joystick_is_dragging", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_input_joystick_is_dragging";
        st = JOY_OK;
        joy_api_call("joy_input_joystick_touch_down", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_input_joystick_touch_down";
        st = JOY_OK;
        joy_api_call("joy_input_joystick_touch_move", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_input_joystick_touch_move";
        st = JOY_OK;
        joy_api_call("joy_input_joystick_touch_up", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_input_joystick_touch_up";
        st = JOY_OK;
        joy_api_call("joy_input_joystick_get_magnitude", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_input_joystick_get_magnitude";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_set_text", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_set_text";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_draw", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_draw";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_is_pressed", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_is_pressed";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_is_hovered", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_is_hovered";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_focus", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_focus";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_defocus", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_defocus";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_blur", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_blur";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_insert", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_insert";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_backspace", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_backspace";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_delete", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_delete";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_cursor_left", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_cursor_left";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_cursor_right", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_cursor_right";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_cursor_home", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_cursor_home";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_cursor_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_cursor_end";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_select_left", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_select_left";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_select_right", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_select_right";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_select_home", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_select_home";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_select_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_select_end";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_select_all", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_select_all";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_paste", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_paste";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_copy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_copy";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_cut", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_cut";
        st = JOY_OK;
        joy_api_call("joy_ui_inputbox_is_focused", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_inputbox_is_focused";
        st = JOY_OK;
        joy_api_call("joy_ui_slider_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_slider_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_slider_set_value", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_slider_set_value";
        st = JOY_OK;
        joy_api_call("joy_ui_slider_get_value", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_slider_get_value";
        st = JOY_OK;
        joy_api_call("joy_ui_slider_set_rect", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_slider_set_rect";
        st = JOY_OK;
        joy_api_call("joy_ui_slider_set_range", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_slider_set_range";
        st = JOY_OK;
        joy_api_call("joy_ui_slider_draw", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_slider_draw";
        st = JOY_OK;
        joy_api_call("joy_ui_slider_is_pressed", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_slider_is_pressed";
        st = JOY_OK;
        joy_api_call("joy_ui_slider_set_pressed", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_slider_set_pressed";
        st = JOY_OK;
        joy_api_call("joy_ui_slider_drag_move", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_slider_drag_move";
        st = JOY_OK;
        joy_api_call("joy_ui_slider_is_hovered", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_slider_is_hovered";
        st = JOY_OK;
        joy_api_call("joy_ui_slider_set_hovered", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_slider_set_hovered";
        st = JOY_OK;
        joy_api_call("joy_ui_popup_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_popup_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_popup_set_title", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_popup_set_title";
        st = JOY_OK;
        joy_api_call("joy_ui_popup_set_content", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_popup_set_content";
        st = JOY_OK;
        joy_api_call("joy_ui_popup_set_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_popup_set_position";
        st = JOY_OK;
        joy_api_call("joy_ui_popup_set_visible", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_popup_set_visible";
        st = JOY_OK;
        joy_api_call("joy_ui_popup_set_closeable", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_popup_set_closeable";
        st = JOY_OK;
        joy_api_call("joy_ui_popup_draw", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_popup_draw";
        st = JOY_OK;
        joy_api_call("joy_ui_popup_is_close_clicked", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_popup_is_close_clicked";
        st = JOY_OK;
        joy_api_call("joy_ui_popup_is_title_clicked", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_popup_is_title_clicked";
        st = JOY_OK;
        joy_api_call("joy_ui_popup_drag_move", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_popup_drag_move";
        st = JOY_OK;
        joy_api_call("joy_ui_popup_is_inside", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_popup_is_inside";
        st = JOY_OK;
        joy_api_call("joy_ui_tooltip_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_tooltip_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_tooltip_set_text", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_tooltip_set_text";
        st = JOY_OK;
        joy_api_call("joy_ui_tooltip_set_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_tooltip_set_position";
        st = JOY_OK;
        joy_api_call("joy_ui_tooltip_set_visible", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_tooltip_set_visible";
        st = JOY_OK;
        joy_api_call("joy_ui_tooltip_draw", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_tooltip_draw";
        st = JOY_OK;
        joy_api_call("joy_ui_ctx_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_ctx_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_begin", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_begin";
        st = JOY_OK;
        joy_api_call("joy_ui_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_end";
        st = JOY_OK;
        joy_api_call("joy_ui_input_motion", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_input_motion";
        st = JOY_OK;
        joy_api_call("joy_ui_input_down", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_input_down";
        st = JOY_OK;
        joy_api_call("joy_ui_input_up", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_input_up";
        st = JOY_OK;
        joy_api_call("joy_ui_input_wheel", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_input_wheel";
        st = JOY_OK;
        joy_api_call("joy_ui_input_key", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_input_key";
        st = JOY_OK;
        joy_api_call("joy_ui_is_down", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_is_down";
        st = JOY_OK;
        joy_api_call("joy_ui_is_pressed", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_is_pressed";
        st = JOY_OK;
        joy_api_call("joy_ui_is_released", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_is_released";
        st = JOY_OK;
        joy_api_call("joy_ui_take_wheel", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_take_wheel";
        st = JOY_OK;
        joy_api_call("joy_ui_is_shift", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_is_shift";
        st = JOY_OK;
        joy_api_call("joy_ui_is_ctrl", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_is_ctrl";
        st = JOY_OK;
        joy_api_call("joy_ui_push_clip", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_push_clip";
        st = JOY_OK;
        joy_api_call("joy_ui_pop_clip", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_pop_clip";
        st = JOY_OK;
        joy_api_call("joy_ui_reset_clip", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_reset_clip";
        st = JOY_OK;
        joy_api_call("joy_ui_hit", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_hit";
        st = JOY_OK;
        joy_api_call("joy_ui_fill_rect", NULL, 10, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_fill_rect";
        st = JOY_OK;
        joy_api_call("joy_ui_stroke_rect", NULL, 10, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_stroke_rect";
        st = JOY_OK;
        joy_api_call("joy_ui_line", NULL, 10, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_line";
        st = JOY_OK;
        joy_api_call("joy_ui_text", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_text";
        st = JOY_OK;
        joy_api_call("joy_ui_text_centered", NULL, 11, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_text_centered";
        st = JOY_OK;
        joy_api_call("joy_ui_panel_begin", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_panel_begin";
        st = JOY_OK;
        joy_api_call("joy_ui_panel_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_panel_end";
        st = JOY_OK;
        joy_api_call("joy_ui_panel_frame", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_panel_frame";
        st = JOY_OK;
        joy_api_call("joy_ui_scroll_begin", NULL, 8, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scroll_begin";
        st = JOY_OK;
        joy_api_call("joy_ui_scroll_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scroll_end";
        st = JOY_OK;
        joy_api_call("joy_ui_scrollbar", NULL, 8, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scrollbar";
        st = JOY_OK;
        joy_api_call("joy_ui_scroll_set_offset", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_scroll_set_offset";
        st = JOY_OK;
        joy_api_call("joy_ui_button", NULL, 8, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_button";
        st = JOY_OK;
        joy_api_call("jtilemap_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtilemap_destroy";
        st = JOY_OK;
        joy_api_call("jtilemap_get_width", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtilemap_get_width";
        st = JOY_OK;
        joy_api_call("jtilemap_get_height", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtilemap_get_height";
        st = JOY_OK;
        joy_api_call("jtilemap_get_tilewidth", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtilemap_get_tilewidth";
        st = JOY_OK;
        joy_api_call("jtilemap_get_tileheight", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtilemap_get_tileheight";
        st = JOY_OK;
        joy_api_call("jtilemap_get_anim_tileid", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtilemap_get_anim_tileid";
        st = JOY_OK;
        joy_api_call("jtilemap_get_tile", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtilemap_get_tile";
        st = JOY_OK;
        joy_api_call("jtilemap_render_all", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtilemap_render_all";
        st = JOY_OK;
        joy_api_call("jtilemap_render_tiles", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtilemap_render_tiles";
        st = JOY_OK;
        joy_api_call("jtilemap_render_set_tile_px", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtilemap_render_set_tile_px";
        st = JOY_OK;
        joy_api_call("jtilemap_render_layer", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtilemap_render_layer";
        st = JOY_OK;
        joy_api_call("jtilemap_render_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtilemap_render_destroy";
        st = JOY_OK;
        joy_api_call("joy_net_kcpserver_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_kcpserver_destroy";
        st = JOY_OK;
        joy_api_call("joy_net_kcpserver_send", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_kcpserver_send";
        st = JOY_OK;
        joy_api_call("joy_net_kcpserver_broadcast", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_kcpserver_broadcast";
        st = JOY_OK;
        joy_api_call("joy_net_kcpserver_offline", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_kcpserver_offline";
        st = JOY_OK;
        joy_api_call("joy_net_kcpserver_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_kcpserver_update";
        st = JOY_OK;
        joy_api_call("joy_net_kcpserver_connection_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_kcpserver_connection_count";
        st = JOY_OK;
        joy_api_call("joy_net_kcpserver_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_kcpserver_poll_message";
        st = JOY_OK;
        joy_api_call("joy_net_kcpclient_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_kcpclient_destroy";
        st = JOY_OK;
        joy_api_call("joy_net_kcpclient_send", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_kcpclient_send";
        st = JOY_OK;
        joy_api_call("joy_net_kcpclient_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_kcpclient_update";
        st = JOY_OK;
        joy_api_call("joy_net_kcpclient_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_kcpclient_poll_message";
        st = JOY_OK;
        joy_api_call("joy_net_tcpserver_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_tcpserver_destroy";
        st = JOY_OK;
        joy_api_call("joy_net_tcpserver_send", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_tcpserver_send";
        st = JOY_OK;
        joy_api_call("joy_net_tcpserver_broadcast", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_tcpserver_broadcast";
        st = JOY_OK;
        joy_api_call("joy_net_tcpserver_offline", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_tcpserver_offline";
        st = JOY_OK;
        joy_api_call("joy_net_tcpserver_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_tcpserver_update";
        st = JOY_OK;
        joy_api_call("joy_net_tcpserver_connection_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_tcpserver_connection_count";
        st = JOY_OK;
        joy_api_call("joy_net_tcpserver_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_tcpserver_poll_message";
        st = JOY_OK;
        joy_api_call("joy_net_tcpclient_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_tcpclient_destroy";
        st = JOY_OK;
        joy_api_call("joy_net_tcpclient_send", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_tcpclient_send";
        st = JOY_OK;
        joy_api_call("joy_net_tcpclient_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_tcpclient_update";
        st = JOY_OK;
        joy_api_call("joy_net_tcpclient_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_tcpclient_poll_message";
        st = JOY_OK;
        joy_api_call("joy_net_wsnetserver_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_wsnetserver_destroy";
        st = JOY_OK;
        joy_api_call("joy_net_wsnetserver_send", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_wsnetserver_send";
        st = JOY_OK;
        joy_api_call("joy_net_wsnetserver_broadcast", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_wsnetserver_broadcast";
        st = JOY_OK;
        joy_api_call("joy_net_wsnetserver_offline", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_wsnetserver_offline";
        st = JOY_OK;
        joy_api_call("joy_net_wsnetserver_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_wsnetserver_update";
        st = JOY_OK;
        joy_api_call("joy_net_wsnetserver_connection_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_wsnetserver_connection_count";
        st = JOY_OK;
        joy_api_call("joy_net_wsnetserver_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_wsnetserver_poll_message";
        st = JOY_OK;
        joy_api_call("joy_net_wsnetclient_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_wsnetclient_destroy";
        st = JOY_OK;
        joy_api_call("joy_net_wsnetclient_send", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_wsnetclient_send";
        st = JOY_OK;
        joy_api_call("joy_net_wsnetclient_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_wsnetclient_update";
        st = JOY_OK;
        joy_api_call("joy_net_wsnetclient_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_wsnetclient_poll_message";
        st = JOY_OK;
        joy_api_call("joy_net_netclient_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_netclient_destroy";
        st = JOY_OK;
        joy_api_call("joy_net_netclient_send", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_netclient_send";
        st = JOY_OK;
        joy_api_call("joy_net_netclient_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_netclient_update";
        st = JOY_OK;
        joy_api_call("joy_net_netclient_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_netclient_poll_message";
        st = JOY_OK;
        joy_api_call("joy_net_netserver_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_netserver_destroy";
        st = JOY_OK;
        joy_api_call("joy_net_netserver_send", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_netserver_send";
        st = JOY_OK;
        joy_api_call("joy_net_netserver_broadcast", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_netserver_broadcast";
        st = JOY_OK;
        joy_api_call("joy_net_netserver_offline", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_netserver_offline";
        st = JOY_OK;
        joy_api_call("joy_net_netserver_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_netserver_update";
        st = JOY_OK;
        joy_api_call("joy_net_netserver_connection_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_netserver_connection_count";
        st = JOY_OK;
        joy_api_call("joy_net_netserver_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_netserver_poll_message";
        st = JOY_OK;
        joy_api_call("joy_net_netmessage_free_data", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_net_netmessage_free_data";
        st = JOY_OK;
        joy_api_call("scene_node_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_node_destroy";
        st = JOY_OK;
        joy_api_call("scene_node_add_child", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_node_add_child";
        st = JOY_OK;
        joy_api_call("scene_node_remove_child", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_node_remove_child";
        st = JOY_OK;
        joy_api_call("scene_node_get_child_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_node_get_child_count";
        st = JOY_OK;
        joy_api_call("scene_node_set_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_node_set_position";
        st = JOY_OK;
        joy_api_call("scene_node_set_world_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_node_set_world_position";
        st = JOY_OK;
        joy_api_call("scene_node_set_rotation", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_node_set_rotation";
        st = JOY_OK;
        joy_api_call("scene_node_get_rotation", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_node_get_rotation";
        st = JOY_OK;
        joy_api_call("scene_node_set_scale", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_node_set_scale";
        st = JOY_OK;
        joy_api_call("scene_node_set_zorder", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_node_set_zorder";
        st = JOY_OK;
        joy_api_call("scene_node_get_zorder", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_node_get_zorder";
        st = JOY_OK;
        joy_api_call("scene_node_set_name", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_node_set_name";
        st = JOY_OK;
        joy_api_call("scene_node_set_tag", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_node_set_tag";
        st = JOY_OK;
        joy_api_call("scene_node_update", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_node_update";
        st = JOY_OK;
        joy_api_call("scene_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_destroy";
        st = JOY_OK;
        joy_api_call("scene_add_root_node", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_add_root_node";
        st = JOY_OK;
        joy_api_call("scene_remove_root_node", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_remove_root_node";
        st = JOY_OK;
        joy_api_call("scene_get_root_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_get_root_count";
        st = JOY_OK;
        joy_api_call("scene_update", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_update";
        st = JOY_OK;
        joy_api_call("scene_set_popup_node", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_set_popup_node";
        st = JOY_OK;
        joy_api_call("scene_manager_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_manager_destroy";
        st = JOY_OK;
        joy_api_call("scene_manager_push", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_manager_push";
        st = JOY_OK;
        joy_api_call("scene_manager_replace", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_manager_replace";
        st = JOY_OK;
        joy_api_call("scene_manager_pop", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_manager_pop";
        st = JOY_OK;
        joy_api_call("scene_manager_update", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_manager_update";
        st = JOY_OK;
        joy_api_call("scene_manager_get_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scene_manager_get_count";
        st = JOY_OK;
        joy_api_call("joy_r2d_particle_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_particle_destroy";
        st = JOY_OK;
        joy_api_call("joy_r2d_particle_set_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_particle_set_position";
        st = JOY_OK;
        joy_api_call("joy_r2d_particle_emit", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_particle_emit";
        st = JOY_OK;
        joy_api_call("joy_r2d_particle_clear", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_particle_clear";
        st = JOY_OK;
        joy_api_call("joy_r2d_particle_update", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_particle_update";
        st = JOY_OK;
        joy_api_call("joy_r2d_particle_render", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_particle_render";
        st = JOY_OK;
        joy_api_call("joy_r2d_particle_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_particle_count";
        st = JOY_OK;
        joy_api_call("joy_r2d_particle_is_active", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r2d_particle_is_active";
        st = JOY_OK;
        joy_api_call("joy_r3d_light_sys_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_light_sys_destroy";
        st = JOY_OK;
        joy_api_call("joy_r3d_light_sys_resize", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_light_sys_resize";
        st = JOY_OK;
        joy_api_call("joy_r3d_light_remove", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_light_remove";
        st = JOY_OK;
        joy_api_call("joy_r3d_light_clear", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_light_clear";
        st = JOY_OK;
        joy_api_call("joy_r3d_light_add_occluder", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_light_add_occluder";
        st = JOY_OK;
        joy_api_call("joy_r3d_light_clear_occluders", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_light_clear_occluders";
        st = JOY_OK;
        joy_api_call("joy_r3d_light_set_ambient", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_light_set_ambient";
        st = JOY_OK;
        joy_api_call("joy_r3d_light_render", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_r3d_light_render";
        st = JOY_OK;
        joy_api_call("jecs_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jecs_destroy";
        st = JOY_OK;
        joy_api_call("jecs_process", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jecs_process";
        st = JOY_OK;
        joy_api_call("jecs_define", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jecs_define";
        st = JOY_OK;
        joy_api_call("audio_open_device", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/audio_open_device";
        st = JOY_OK;
        joy_api_call("audio_close_device", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/audio_close_device";
        st = JOY_OK;
        joy_api_call("audio_create_wav", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/audio_create_wav";
        st = JOY_OK;
        joy_api_call("audio_create_mp3", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/audio_create_mp3";
        st = JOY_OK;
        joy_api_call("audio_create_flac", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/audio_create_flac";
        st = JOY_OK;
        joy_api_call("audio_create_ogg", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/audio_create_ogg";
        st = JOY_OK;
        joy_api_call("audio_shot_play", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/audio_shot_play";
        st = JOY_OK;
        joy_api_call("audio_shot_set_gain", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/audio_shot_set_gain";
        st = JOY_OK;
        joy_api_call("audio_shot_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/audio_shot_destroy";
        st = JOY_OK;
        joy_api_call("audio_bgm_stop", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/audio_bgm_stop";
        st = JOY_OK;
        joy_api_call("audio_bgm_set_gain", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/audio_bgm_set_gain";
        st = JOY_OK;
        joy_api_call("audio_bgm_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/audio_bgm_destroy";
        st = JOY_OK;
        joy_api_call("joy_ui_image_draw_rect", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/joy_ui_image_draw_rect";
        st = JOY_OK;
        joy_api_call("jmath_fp_from_float", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_from_float";
        st = JOY_OK;
        joy_api_call("jmath_fp_to_float", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_to_float";
        st = JOY_OK;
        joy_api_call("jmath_fp_from_uint", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_from_uint";
        st = JOY_OK;
        joy_api_call("jmath_fp_from_int", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_from_int";
        st = JOY_OK;
        joy_api_call("jmath_fp_from_fraction", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_from_fraction";
        st = JOY_OK;
        joy_api_call("jmath_fp_min_value", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_min_value";
        st = JOY_OK;
        joy_api_call("jmath_fp_max_value", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_max_value";
        st = JOY_OK;
        joy_api_call("jmath_fp_add", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_add";
        st = JOY_OK;
        joy_api_call("jmath_fp_sub", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_sub";
        st = JOY_OK;
        joy_api_call("jmath_fp_zero", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_zero";
        st = JOY_OK;
        joy_api_call("jmath_fp_half", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_half";
        st = JOY_OK;
        joy_api_call("jmath_fp_one", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_one";
        st = JOY_OK;
        joy_api_call("jmath_fp_pi", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_pi";
        st = JOY_OK;
        joy_api_call("jmath_fp_max", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_max";
        st = JOY_OK;
        joy_api_call("jmath_fp_min", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_min";
        st = JOY_OK;
        joy_api_call("jmath_fp_pow2", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_pow2";
        st = JOY_OK;
        joy_api_call("jmath_fp_sign", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_sign";
        st = JOY_OK;
        joy_api_call("jmath_fp_clamp", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_clamp";
        st = JOY_OK;
        joy_api_call("jmath_fp_lerp", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_fp_lerp";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_negate_x", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_negate_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_negate_y", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_negate_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_dot", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_dot";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_cross", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_cross";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_cross2_x", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_cross2_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_cross2_y", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_cross2_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_cross3_x", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_cross3_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_cross3_y", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_cross3_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_add_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_add_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_add_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_add_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_sub_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_sub_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_sub_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_sub_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_scale_x", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_scale_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_scale_y", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_scale_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_abs_x", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_abs_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_abs_y", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_abs_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_length_squared", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_length_squared";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_length", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_length";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_normalize_x", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_normalize_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_normalize_y", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_normalize_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_normal_x", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_normal_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_normal_y", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_normal_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_rotate_x", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_rotate_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_rotate_y", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_rotate_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_lerp_x", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_lerp_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_lerp_y", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_lerp_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2f_distance", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2f_distance";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_negate_x", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_negate_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_negate_y", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_negate_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_negate_z", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_negate_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_add_x", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_add_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_add_y", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_add_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_add_z", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_add_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_sub_x", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_sub_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_sub_y", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_sub_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_sub_z", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_sub_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_scale_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_scale_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_scale_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_scale_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_scale_z", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_scale_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_abs_x", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_abs_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_abs_y", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_abs_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_abs_z", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_abs_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_dot", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_dot";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_cross_x", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_cross_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_cross_y", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_cross_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_cross_z", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_cross_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_square_length", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_square_length";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_length", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_length";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_normalize_x", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_normalize_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_normalize_y", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_normalize_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_normalize_z", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_normalize_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_lerp_x", NULL, 8, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_lerp_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_lerp_y", NULL, 8, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_lerp_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_lerp_z", NULL, 8, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_lerp_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3f_distance", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3f_distance";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_mul_vec2f_x", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_mul_vec2f_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_mul_vec2f_y", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_mul_vec2f_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_rotate_col1_x", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_rotate_col1_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_rotate_col1_y", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_rotate_col1_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_rotate_col2_x", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_rotate_col2_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_rotate_col2_y", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_rotate_col2_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_transpose_col1_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_transpose_col1_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_transpose_col1_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_transpose_col1_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_transpose_col2_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_transpose_col2_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_transpose_col2_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_transpose_col2_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_invert_col1_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_invert_col1_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_invert_col1_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_invert_col1_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_invert_col2_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_invert_col2_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_invert_col2_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_invert_col2_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_add_col1_x", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_add_col1_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_add_col1_y", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_add_col1_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_add_col2_x", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_add_col2_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_add_col2_y", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_add_col2_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_mul_col1_x", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_mul_col1_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_mul_col1_y", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_mul_col1_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_mul_col2_x", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_mul_col2_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_mul_col2_y", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_mul_col2_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_abs_col1_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_abs_col1_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_abs_col1_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_abs_col1_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_abs_col2_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_abs_col2_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22f_abs_col2_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22f_abs_col2_y";
        st = JOY_OK;
        joy_api_call("jmath_ft_from_float", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_from_float";
        st = JOY_OK;
        joy_api_call("jmath_ft_to_float", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_to_float";
        st = JOY_OK;
        joy_api_call("jmath_ft_min_value", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_min_value";
        st = JOY_OK;
        joy_api_call("jmath_ft_max_value", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_max_value";
        st = JOY_OK;
        joy_api_call("jmath_ft_add", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_add";
        st = JOY_OK;
        joy_api_call("jmath_ft_sub", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_sub";
        st = JOY_OK;
        joy_api_call("jmath_ft_mul", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_mul";
        st = JOY_OK;
        joy_api_call("jmath_ft_div", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_div";
        st = JOY_OK;
        joy_api_call("jmath_ft_abs", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_abs";
        st = JOY_OK;
        joy_api_call("jmath_ft_sqrt", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_sqrt";
        st = JOY_OK;
        joy_api_call("jmath_ft_sin", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_sin";
        st = JOY_OK;
        joy_api_call("jmath_ft_cos", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_cos";
        st = JOY_OK;
        joy_api_call("jmath_ft_atan2", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_atan2";
        st = JOY_OK;
        joy_api_call("jmath_ft_zero", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_zero";
        st = JOY_OK;
        joy_api_call("jmath_ft_half", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_half";
        st = JOY_OK;
        joy_api_call("jmath_ft_one", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_one";
        st = JOY_OK;
        joy_api_call("jmath_ft_pi", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_pi";
        st = JOY_OK;
        joy_api_call("jmath_ft_max", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_max";
        st = JOY_OK;
        joy_api_call("jmath_ft_min", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_min";
        st = JOY_OK;
        joy_api_call("jmath_ft_pow2", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_pow2";
        st = JOY_OK;
        joy_api_call("jmath_ft_sign", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_sign";
        st = JOY_OK;
        joy_api_call("jmath_ft_clamp", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_clamp";
        st = JOY_OK;
        joy_api_call("jmath_ft_lerp", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_lerp";
        st = JOY_OK;
        joy_api_call("jmath_vec2_negate_x", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_negate_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2_negate_y", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_negate_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2_negate", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_negate";
        st = JOY_OK;
        joy_api_call("jmath_vec2_dot", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_dot";
        st = JOY_OK;
        joy_api_call("jmath_vec2_cross", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_cross";
        st = JOY_OK;
        joy_api_call("jmath_vec2_cross2_x", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_cross2_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2_cross2_y", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_cross2_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2_cross2", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_cross2";
        st = JOY_OK;
        joy_api_call("jmath_vec2_cross3_x", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_cross3_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2_cross3_y", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_cross3_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2_cross3", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_cross3";
        st = JOY_OK;
        joy_api_call("jmath_vec2_add_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_add_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2_add_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_add_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2_add", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_add";
        st = JOY_OK;
        joy_api_call("jmath_vec2_sub_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_sub_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2_sub_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_sub_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2_sub", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_sub";
        st = JOY_OK;
        joy_api_call("jmath_vec2_scale_x", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_scale_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2_scale_y", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_scale_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2_scale", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_scale";
        st = JOY_OK;
        joy_api_call("jmath_vec2_abs_x", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_abs_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2_abs_y", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_abs_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2_abs", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_abs";
        st = JOY_OK;
        joy_api_call("jmath_vec2_length_squared", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_length_squared";
        st = JOY_OK;
        joy_api_call("jmath_vec2_length", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_length";
        st = JOY_OK;
        joy_api_call("jmath_vec2_normalize_x", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_normalize_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2_normalize_y", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_normalize_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2_normalize", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_normalize";
        st = JOY_OK;
        joy_api_call("jmath_vec2_normal_x", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_normal_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2_normal_y", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_normal_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2_normal", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_normal";
        st = JOY_OK;
        joy_api_call("jmath_vec2_rotate_x", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_rotate_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2_rotate_y", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_rotate_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2_rotate", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_rotate";
        st = JOY_OK;
        joy_api_call("jmath_vec2_lerp_x", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_lerp_x";
        st = JOY_OK;
        joy_api_call("jmath_vec2_lerp_y", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_lerp_y";
        st = JOY_OK;
        joy_api_call("jmath_vec2_lerp", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_lerp";
        st = JOY_OK;
        joy_api_call("jmath_vec2_distance", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec2_distance";
        st = JOY_OK;
        joy_api_call("jmath_vec3_negate_x", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_negate_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3_negate_y", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_negate_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3_negate_z", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_negate_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3_add_x", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_add_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3_add_y", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_add_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3_add_z", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_add_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3_sub_x", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_sub_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3_sub_y", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_sub_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3_sub_z", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_sub_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3_scale_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_scale_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3_scale_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_scale_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3_scale_z", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_scale_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3_abs_x", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_abs_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3_abs_y", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_abs_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3_abs_z", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_abs_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3_dot", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_dot";
        st = JOY_OK;
        joy_api_call("jmath_vec3_cross_x", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_cross_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3_cross_y", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_cross_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3_cross_z", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_cross_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3_square_length", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_square_length";
        st = JOY_OK;
        joy_api_call("jmath_vec3_length", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_length";
        st = JOY_OK;
        joy_api_call("jmath_vec3_normalize_x", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_normalize_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3_normalize_y", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_normalize_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3_normalize_z", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_normalize_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3_lerp_x", NULL, 8, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_lerp_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3_lerp_y", NULL, 8, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_lerp_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3_lerp_z", NULL, 8, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_lerp_z";
        st = JOY_OK;
        joy_api_call("jmath_vec3_distance", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_distance";
        st = JOY_OK;
        joy_api_call("jmath_mat22_mul_vec2_x", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_mul_vec2_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22_mul_vec2_y", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_mul_vec2_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22_mul_vec2", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_mul_vec2";
        st = JOY_OK;
        joy_api_call("jmath_mat22_rotate_col1_x", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_rotate_col1_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22_rotate_col1_y", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_rotate_col1_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22_rotate_col2_x", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_rotate_col2_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22_rotate_col2_y", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_rotate_col2_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22_transpose_col1_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_transpose_col1_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22_transpose_col1_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_transpose_col1_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22_transpose_col2_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_transpose_col2_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22_transpose_col2_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_transpose_col2_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22_invert_col1_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_invert_col1_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22_invert_col1_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_invert_col1_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22_invert_col2_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_invert_col2_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22_invert_col2_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_invert_col2_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22_add_col1_x", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_add_col1_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22_add_col1_y", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_add_col1_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22_add_col2_x", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_add_col2_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22_add_col2_y", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_add_col2_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22_mul_col1_x", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_mul_col1_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22_mul_col1_y", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_mul_col1_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22_mul_col2_x", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_mul_col2_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22_mul_col2_y", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_mul_col2_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22_abs_col1_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_abs_col1_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22_abs_col1_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_abs_col1_y";
        st = JOY_OK;
        joy_api_call("jmath_mat22_abs_col2_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_abs_col2_x";
        st = JOY_OK;
        joy_api_call("jmath_mat22_abs_col2_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_mat22_abs_col2_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3_div_x", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_div_x";
        st = JOY_OK;
        joy_api_call("jmath_vec3_div_y", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_div_y";
        st = JOY_OK;
        joy_api_call("jmath_vec3_div_z", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_vec3_div_z";
        st = JOY_OK;
        joy_api_call("jmath_ft_radians", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jmath_ft_radians";

        /* L2 试点：用展开后的参数个数调用，args 全为 NULL 值，
         * thunk 在类型校验处返回，不会执行任何 joy 业务函数。 */
        for (i = 0; i < 13; i++) args[i] = joy_v_null();
        st = JOY_OK;
        joy_api_call("joy_r2d_set_clip_rect", args, 5, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_r2d_set_clip_rect";
        st = JOY_OK;
        joy_api_call("joy_r2d_push_clip_rect", args, 5, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_r2d_push_clip_rect";
        st = JOY_OK;
        joy_api_call("shape_draw_rectangle", args, 6, &st);
        if (st != JOY_ERR_TYPE) return "L2/shape_draw_rectangle";
        st = JOY_OK;
        joy_api_call("joy_ui_text_updatex", args, 8, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_text_updatex";
        st = JOY_OK;
        joy_api_call("joy_r2d_jtext_printx", args, 10, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_r2d_jtext_printx";
        st = JOY_OK;
        joy_api_call("joy_r2d_jtext_printf", args, 10, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_r2d_jtext_printf";
        st = JOY_OK;
        joy_api_call("joy_r2d_jtext_printf_scaled", args, 11, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_r2d_jtext_printf_scaled";
        st = JOY_OK;
        joy_api_call("animation_add_clip", args, 7, &st);
        if (st != JOY_ERR_TYPE) return "L2/animation_add_clip";
        st = JOY_OK;
        joy_api_call("joy_r3d_checkbox_set_colors", args, 13, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_r3d_checkbox_set_colors";
        st = JOY_OK;
        joy_api_call("janimator_state_add_clip", args, 8, &st);
        if (st != JOY_ERR_TYPE) return "L2/janimator_state_add_clip";
        st = JOY_OK;
        joy_api_call("janimator_state_add_motion", args, 9, &st);
        if (st != JOY_ERR_TYPE) return "L2/janimator_state_add_motion";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_normal_color", args, 5, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_button_set_normal_color";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_hover_color", args, 5, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_button_set_hover_color";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_pressed_color", args, 5, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_button_set_pressed_color";
        st = JOY_OK;
        joy_api_call("joy_ui_button_set_textx", args, 8, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_button_set_textx";
        st = JOY_OK;
        joy_api_call("joy_ui_combobox_add_item", args, 8, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_combobox_add_item";
        st = JOY_OK;
        joy_api_call("joy_ui_checkbox_set_colors", args, 13, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_checkbox_set_colors";
        st = JOY_OK;
        joy_api_call("joy_ui_checkbox_set_text_color", args, 5, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_checkbox_set_text_color";
        st = JOY_OK;
        joy_api_call("joy_ui_push_clip", args, 5, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_push_clip";
        st = JOY_OK;
        joy_api_call("joy_ui_hit", args, 5, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_hit";
        st = JOY_OK;
        joy_api_call("joy_ui_fill_rect", args, 9, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_fill_rect";
        st = JOY_OK;
        joy_api_call("joy_ui_stroke_rect", args, 9, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_stroke_rect";
        st = JOY_OK;
        joy_api_call("joy_ui_line", args, 9, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_line";
        st = JOY_OK;
        joy_api_call("joy_ui_text", args, 8, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_text";
        st = JOY_OK;
        joy_api_call("joy_ui_text_centered", args, 10, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_text_centered";
        st = JOY_OK;
        joy_api_call("joy_ui_panel_begin", args, 6, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_panel_begin";
        st = JOY_OK;
        joy_api_call("joy_ui_panel_frame", args, 5, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_panel_frame";
        st = JOY_OK;
        joy_api_call("joy_ui_scroll_begin", args, 7, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_scroll_begin";
        st = JOY_OK;
        joy_api_call("joy_ui_scrollbar", args, 7, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_scrollbar";
        st = JOY_OK;
        joy_api_call("joy_ui_button", args, 7, &st);
        if (st != JOY_ERR_TYPE) return "L2/joy_ui_button";

        /* L0：纯标量往返 */
        st = JOY_OK;
        r = t_joy_core_init_env(NULL, 0, &st);
        if (st != JOY_OK || r.kind != JOY_V_BOOL) return "L0/joy_core_init_env";
        st = JOY_OK;
        t_joy_core_init_env(args, 1, &st);
        if (st != JOY_ERR_ARGC) return "argc-check";

        /* L1：create -> 句柄 -> 类型校验 -> 自动析构 */
        st = JOY_OK;
        args[0] = joy_v_u64((uint64_t)12345);
        r = t_joy_core_lcg_random_create(args, 1, &st);
        if (st != JOY_OK || r.kind != JOY_V_HANDLE) return "L1/joy_core_lcg_random_create";
        if (joy_obj_live_count() != live0 + 1) return "live-after-create";

        st = JOY_OK;
        {
                joy_value_t h = t_joy_core_lcg_random_next(&r, 1, &st);
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
