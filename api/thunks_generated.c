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

static joy_value_t t_lcg_random_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        lcg_num_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "lcg_random_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "lcg_random_destroy.arg0");
        p0 = (lcg_num_p)joy_obj_deref(a[0].as.handle, "lcg_num_p", st);
        if (!p0) return joy_v_err(st, *st, "lcg_random_destroy.arg0");
        lcg_random_destroy(p0);
        return joy_v_null();
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

static joy_value_t t_lcg_random_set_state(joy_value_t* a, int n, joy_status_t* st)
{
        lcg_num_p p0;
        uint64_t p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "lcg_random_set_state");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "lcg_random_set_state.arg0");
        if (a[1].kind != JOY_V_U64) return joy_v_err(st, JOY_ERR_TYPE, "lcg_random_set_state.arg1");
        p0 = (lcg_num_p)joy_obj_deref(a[0].as.handle, "lcg_num_p", st);
        if (!p0) return joy_v_err(st, *st, "lcg_random_set_state.arg0");
        p1 = (uint64_t)a[1].as.u64;
        lcg_random_set_state(p0, p1);
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

static joy_value_t t_sdbm_hash(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        int p1;
        unsigned long _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "sdbm_hash");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "sdbm_hash.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "sdbm_hash.arg1");
        p0 = (const char*)a[0].as.str;
        p1 = (int)a[1].as.i32;
        _r = sdbm_hash(p0, p1);
        return joy_v_u64((unsigned long)_r);
}

static joy_value_t t_fnv1a_hash(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        int p1;
        unsigned long _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "fnv1a_hash");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "fnv1a_hash.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "fnv1a_hash.arg1");
        p0 = (const char*)a[0].as.str;
        p1 = (int)a[1].as.i32;
        _r = fnv1a_hash(p0, p1);
        return joy_v_u64((unsigned long)_r);
}

static joy_value_t t_jenkins_hash(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        int p1;
        unsigned long _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jenkins_hash");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "jenkins_hash.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jenkins_hash.arg1");
        p0 = (const char*)a[0].as.str;
        p1 = (int)a[1].as.i32;
        _r = jenkins_hash(p0, p1);
        return joy_v_u64((unsigned long)_r);
}

static joy_value_t t_murmur3_hash(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        unsigned long _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "murmur3_hash");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "murmur3_hash.arg0");
        p0 = (const char*)a[0].as.str;
        _r = murmur3_hash(p0);
        return joy_v_u64((unsigned long)_r);
}

static joy_value_t t_kr_hash(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        int p1;
        unsigned long _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "kr_hash");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "kr_hash.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "kr_hash.arg1");
        p0 = (const char*)a[0].as.str;
        p1 = (int)a[1].as.i32;
        _r = kr_hash(p0, p1);
        return joy_v_u64((unsigned long)_r);
}

static joy_value_t t_jtimer_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jtimer_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jtimer_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtimer_destroy.arg0");
        p0 = (jtimer_p)joy_obj_deref(a[0].as.handle, "jtimer_p", st);
        if (!p0) return joy_v_err(st, *st, "jtimer_destroy.arg0");
        jtimer_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_jtimer_trigger(joy_value_t* a, int n, joy_status_t* st)
{
        jtimer_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jtimer_trigger");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtimer_trigger.arg0");
        p0 = (jtimer_p)joy_obj_deref(a[0].as.handle, "jtimer_p", st);
        if (!p0) return joy_v_err(st, *st, "jtimer_trigger.arg0");
        jtimer_trigger(p0);
        return joy_v_null();
}

static joy_value_t t_jtimer_set_interval(joy_value_t* a, int n, joy_status_t* st)
{
        jtimer_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jtimer_set_interval");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtimer_set_interval.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jtimer_set_interval.arg1");
        p0 = (jtimer_p)joy_obj_deref(a[0].as.handle, "jtimer_p", st);
        if (!p0) return joy_v_err(st, *st, "jtimer_set_interval.arg0");
        p1 = (int)a[1].as.i32;
        jtimer_set_interval(p0, p1);
        return joy_v_null();
}

static joy_value_t t_jtimer_set_active(joy_value_t* a, int n, joy_status_t* st)
{
        jtimer_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jtimer_set_active");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtimer_set_active.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "jtimer_set_active.arg1");
        p0 = (jtimer_p)joy_obj_deref(a[0].as.handle, "jtimer_p", st);
        if (!p0) return joy_v_err(st, *st, "jtimer_set_active.arg0");
        p1 = (bool)a[1].as.b;
        jtimer_set_active(p0, p1);
        return joy_v_null();
}

static joy_value_t t_jtimer_reset(joy_value_t* a, int n, joy_status_t* st)
{
        jtimer_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jtimer_reset");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtimer_reset.arg0");
        p0 = (jtimer_p)joy_obj_deref(a[0].as.handle, "jtimer_p", st);
        if (!p0) return joy_v_err(st, *st, "jtimer_reset.arg0");
        jtimer_reset(p0);
        return joy_v_null();
}

static joy_value_t t_jtimer_get_elapsed(joy_value_t* a, int n, joy_status_t* st)
{
        jtimer_p p0;
        uint64_t _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jtimer_get_elapsed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtimer_get_elapsed.arg0");
        p0 = (jtimer_p)joy_obj_deref(a[0].as.handle, "jtimer_p", st);
        if (!p0) return joy_v_err(st, *st, "jtimer_get_elapsed.arg0");
        _r = jtimer_get_elapsed(p0);
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_init_env(joy_value_t* a, int n, joy_status_t* st)
{
        bool _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "init_env");
        _r = init_env();
        return joy_v_bool((bool)_r);
}

static joy_value_t t_quit_env(joy_value_t* a, int n, joy_status_t* st)
{
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "quit_env");
        quit_env();
        return joy_v_null();
}

static joy_value_t t_get_ticks(joy_value_t* a, int n, joy_status_t* st)
{
        uint64_t _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "get_ticks");
        _r = get_ticks();
        return joy_v_u64((uint64_t)_r);
}

static joy_value_t t_destroy_window(joy_value_t* a, int n, joy_status_t* st)
{
        jwindow_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "destroy_window");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "destroy_window.arg0");
        p0 = (jwindow_p)joy_obj_deref(a[0].as.handle, "jwindow_p", st);
        if (!p0) return joy_v_err(st, *st, "destroy_window.arg0");
        destroy_window(p0);
        return joy_v_null();
}

static joy_value_t t_window_set_fullscreen(joy_value_t* a, int n, joy_status_t* st)
{
        jwindow_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "window_set_fullscreen");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "window_set_fullscreen.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "window_set_fullscreen.arg1");
        p0 = (jwindow_p)joy_obj_deref(a[0].as.handle, "jwindow_p", st);
        if (!p0) return joy_v_err(st, *st, "window_set_fullscreen.arg0");
        p1 = (bool)a[1].as.b;
        window_set_fullscreen(p0, p1);
        return joy_v_null();
}

static joy_value_t t_window_get_fullscreen(joy_value_t* a, int n, joy_status_t* st)
{
        jwindow_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "window_get_fullscreen");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "window_get_fullscreen.arg0");
        p0 = (jwindow_p)joy_obj_deref(a[0].as.handle, "jwindow_p", st);
        if (!p0) return joy_v_err(st, *st, "window_get_fullscreen.arg0");
        _r = window_get_fullscreen(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_window_set_icon(joy_value_t* a, int n, joy_status_t* st)
{
        jwindow_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "window_set_icon");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "window_set_icon.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "window_set_icon.arg1");
        p0 = (jwindow_p)joy_obj_deref(a[0].as.handle, "jwindow_p", st);
        if (!p0) return joy_v_err(st, *st, "window_set_icon.arg0");
        p1 = (const char*)a[1].as.str;
        window_set_icon(p0, p1);
        return joy_v_null();
}

static joy_value_t t_push_user_event(joy_value_t* a, int n, joy_status_t* st)
{
        int p0;
        int p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "push_user_event");
        if (a[0].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "push_user_event.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "push_user_event.arg1");
        p0 = (int)a[0].as.i32;
        p1 = (int)a[1].as.i32;
        _r = push_user_event(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_push_quit_event(joy_value_t* a, int n, joy_status_t* st)
{
        bool _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "push_quit_event");
        _r = push_quit_event();
        return joy_v_bool((bool)_r);
}

static joy_value_t t_log_title(joy_value_t* a, int n, joy_status_t* st)
{
        int p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "log_title");
        if (a[0].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "log_title.arg0");
        p0 = (int)a[0].as.i32;
        log_title(p0);
        return joy_v_null();
}

static joy_value_t t_env_set(joy_value_t* a, int n, joy_status_t* st)
{
        env_p p0;
        const char* p1;
        const char* p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "env_set");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "env_set.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "env_set.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "env_set.arg2");
        p0 = (env_p)joy_obj_deref(a[0].as.handle, "env_p", st);
        if (!p0) return joy_v_err(st, *st, "env_set.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (const char*)a[2].as.str;
        env_set(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_env_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        env_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "env_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "env_destroy.arg0");
        p0 = (env_p)joy_obj_deref(a[0].as.handle, "env_p", st);
        if (!p0) return joy_v_err(st, *st, "env_destroy.arg0");
        env_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_simple_fps_destory(joy_value_t* a, int n, joy_status_t* st)
{
        simple_fps_counter_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "simple_fps_destory");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "simple_fps_destory.arg0");
        p0 = (simple_fps_counter_p)joy_obj_deref(a[0].as.handle, "simple_fps_counter_p", st);
        if (!p0) return joy_v_err(st, *st, "simple_fps_destory.arg0");
        simple_fps_destory(p0);
        return joy_v_null();
}

static joy_value_t t_simple_fps_update(joy_value_t* a, int n, joy_status_t* st)
{
        simple_fps_counter_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "simple_fps_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "simple_fps_update.arg0");
        p0 = (simple_fps_counter_p)joy_obj_deref(a[0].as.handle, "simple_fps_counter_p", st);
        if (!p0) return joy_v_err(st, *st, "simple_fps_update.arg0");
        simple_fps_update(p0);
        return joy_v_null();
}

static joy_value_t t_simple_fps_value(joy_value_t* a, int n, joy_status_t* st)
{
        simple_fps_counter_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "simple_fps_value");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "simple_fps_value.arg0");
        p0 = (simple_fps_counter_p)joy_obj_deref(a[0].as.handle, "simple_fps_counter_p", st);
        if (!p0) return joy_v_err(st, *st, "simple_fps_value.arg0");
        _r = simple_fps_value(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_game_timer_init(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "game_timer_init");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "game_timer_init.arg0");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "game_timer_init.arg0");
        game_timer_init(p0);
        return joy_v_null();
}

static joy_value_t t_game_timer_update(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "game_timer_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "game_timer_update.arg0");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "game_timer_update.arg0");
        game_timer_update(p0);
        return joy_v_null();
}

static joy_value_t t_game_timer_get_delta_time(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "game_timer_get_delta_time");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "game_timer_get_delta_time.arg0");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "game_timer_get_delta_time.arg0");
        _r = game_timer_get_delta_time(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_game_timer_get_unscaled_delta_time(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "game_timer_get_unscaled_delta_time");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "game_timer_get_unscaled_delta_time.arg0");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "game_timer_get_unscaled_delta_time.arg0");
        _r = game_timer_get_unscaled_delta_time(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_game_timer_set_time_scale(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "game_timer_set_time_scale");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "game_timer_set_time_scale.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "game_timer_set_time_scale.arg1");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "game_timer_set_time_scale.arg0");
        p1 = (float)a[1].as.f64;
        game_timer_set_time_scale(p0, p1);
        return joy_v_null();
}

static joy_value_t t_game_timer_get_time_scale(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "game_timer_get_time_scale");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "game_timer_get_time_scale.arg0");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "game_timer_get_time_scale.arg0");
        _r = game_timer_get_time_scale(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_game_timer_set_target_fps(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "game_timer_set_target_fps");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "game_timer_set_target_fps.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "game_timer_set_target_fps.arg1");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "game_timer_set_target_fps.arg0");
        p1 = (int)a[1].as.i32;
        game_timer_set_target_fps(p0, p1);
        return joy_v_null();
}

static joy_value_t t_game_timer_get_target_fps(joy_value_t* a, int n, joy_status_t* st)
{
        game_timer_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "game_timer_get_target_fps");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "game_timer_get_target_fps.arg0");
        p0 = (game_timer_p)joy_obj_deref(a[0].as.handle, "game_timer_p", st);
        if (!p0) return joy_v_err(st, *st, "game_timer_get_target_fps.arg0");
        _r = game_timer_get_target_fps(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_set_clipboard_text(joy_value_t* a, int n, joy_status_t* st)
{
        const char* p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "set_clipboard_text");
        if (a[0].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "set_clipboard_text.arg0");
        p0 = (const char*)a[0].as.str;
        set_clipboard_text(p0);
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

static joy_value_t t_iocp_server_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        iocp_server_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "iocp_server_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "iocp_server_destroy.arg0");
        p0 = (iocp_server_p)joy_obj_deref(a[0].as.handle, "iocp_server_p", st);
        if (!p0) return joy_v_err(st, *st, "iocp_server_destroy.arg0");
        iocp_server_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_iocp_process_events(joy_value_t* a, int n, joy_status_t* st)
{
        iocp_server_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "iocp_process_events");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "iocp_process_events.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "iocp_process_events.arg1");
        p0 = (iocp_server_p)joy_obj_deref(a[0].as.handle, "iocp_server_p", st);
        if (!p0) return joy_v_err(st, *st, "iocp_process_events.arg0");
        p1 = (int)a[1].as.i32;
        iocp_process_events(p0, p1);
        return joy_v_null();
}

static joy_value_t t_font_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        font_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "font_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "font_destroy.arg0");
        p0 = (font_p)joy_obj_deref(a[0].as.handle, "font_p", st);
        if (!p0) return joy_v_err(st, *st, "font_destroy.arg0");
        font_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_font_get_scale(joy_value_t* a, int n, joy_status_t* st)
{
        font_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "font_get_scale");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "font_get_scale.arg0");
        p0 = (font_p)joy_obj_deref(a[0].as.handle, "font_p", st);
        if (!p0) return joy_v_err(st, *st, "font_get_scale.arg0");
        _r = font_get_scale(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_font_get_size(joy_value_t* a, int n, joy_status_t* st)
{
        font_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "font_get_size");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "font_get_size.arg0");
        p0 = (font_p)joy_obj_deref(a[0].as.handle, "font_p", st);
        if (!p0) return joy_v_err(st, *st, "font_get_size.arg0");
        _r = font_get_size(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_font_get_ascent(joy_value_t* a, int n, joy_status_t* st)
{
        font_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "font_get_ascent");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "font_get_ascent.arg0");
        p0 = (font_p)joy_obj_deref(a[0].as.handle, "font_p", st);
        if (!p0) return joy_v_err(st, *st, "font_get_ascent.arg0");
        _r = font_get_ascent(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_r2d_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r2d_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_destroy.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_destroy.arg0");
        r2d_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_r2d_set_color(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        uint8_t p1;
        uint8_t p2;
        uint8_t p3;
        uint8_t p4;
        bool _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "r2d_set_color");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_color.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_color.arg1");
        if (a[2].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_color.arg2");
        if (a[3].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_color.arg3");
        if (a[4].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_color.arg4");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_set_color.arg0");
        p1 = (uint8_t)a[1].as.u32;
        p2 = (uint8_t)a[2].as.u32;
        p3 = (uint8_t)a[3].as.u32;
        p4 = (uint8_t)a[4].as.u32;
        _r = r2d_set_color(p0, p1, p2, p3, p4);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r2d_begin(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r2d_begin");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_begin.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_begin.arg0");
        r2d_begin(p0);
        return joy_v_null();
}

static joy_value_t t_r2d_end(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r2d_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_end.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_end.arg0");
        r2d_end(p0);
        return joy_v_null();
}

static joy_value_t t_r2d_set_logical_presentation(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        int p1;
        int p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "r2d_set_logical_presentation");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_logical_presentation.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_logical_presentation.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_logical_presentation.arg2");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_set_logical_presentation.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        _r = r2d_set_logical_presentation(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r2d_set_logical_presentation_mode(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        int p1;
        int p2;
        int p3;
        bool _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "r2d_set_logical_presentation_mode");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_logical_presentation_mode.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_logical_presentation_mode.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_logical_presentation_mode.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_logical_presentation_mode.arg3");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_set_logical_presentation_mode.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        p3 = (int)a[3].as.i32;
        _r = r2d_set_logical_presentation_mode(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r2d_set_vsync(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        int p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r2d_set_vsync");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_vsync.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_vsync.arg1");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_set_vsync.arg0");
        p1 = (int)a[1].as.i32;
        _r = r2d_set_vsync(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r2d_set_scale(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "r2d_set_scale");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_scale.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_scale.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_scale.arg2");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_set_scale.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = r2d_set_scale(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r2d_set_colorf(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        float p1;
        float p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "r2d_set_colorf");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_colorf.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_colorf.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_colorf.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_colorf.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_colorf.arg4");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_set_colorf.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        r2d_set_colorf(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_r2d_set_blend_mode(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        int p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r2d_set_blend_mode");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_blend_mode.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r2d_set_blend_mode.arg1");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_set_blend_mode.arg0");
        p1 = (int)a[1].as.i32;
        _r = r2d_set_blend_mode(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r2d_draw_point(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "r2d_draw_point");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_draw_point.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r2d_draw_point.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r2d_draw_point.arg2");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_draw_point.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = r2d_draw_point(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r2d_clear_clip(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r2d_clear_clip");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_clear_clip.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_clear_clip.arg0");
        r2d_clear_clip(p0);
        return joy_v_null();
}

static joy_value_t t_r2d_pop_clip_rect(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r2d_pop_clip_rect");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_pop_clip_rect.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_pop_clip_rect.arg0");
        r2d_pop_clip_rect(p0);
        return joy_v_null();
}

static joy_value_t t_r2d_reset_clip_stack(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r2d_reset_clip_stack");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_reset_clip_stack.arg0");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_reset_clip_stack.arg0");
        r2d_reset_clip_stack(p0);
        return joy_v_null();
}

static joy_value_t t_r2d_point_in_clip(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "r2d_point_in_clip");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_point_in_clip.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r2d_point_in_clip.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r2d_point_in_clip.arg2");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_point_in_clip.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = r2d_point_in_clip(p0, p1, p2);
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

static joy_value_t t_image_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        image_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "image_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "image_destroy.arg0");
        p0 = (image_p)joy_obj_deref(a[0].as.handle, "image_p", st);
        if (!p0) return joy_v_err(st, *st, "image_destroy.arg0");
        image_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_image_set_alpha(joy_value_t* a, int n, joy_status_t* st)
{
        image_p p0;
        uint8_t p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "image_set_alpha");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "image_set_alpha.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "image_set_alpha.arg1");
        p0 = (image_p)joy_obj_deref(a[0].as.handle, "image_p", st);
        if (!p0) return joy_v_err(st, *st, "image_set_alpha.arg0");
        p1 = (uint8_t)a[1].as.u32;
        image_set_alpha(p0, p1);
        return joy_v_null();
}

static joy_value_t t_image_set_blend_mode(joy_value_t* a, int n, joy_status_t* st)
{
        image_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "image_set_blend_mode");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "image_set_blend_mode.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "image_set_blend_mode.arg1");
        p0 = (image_p)joy_obj_deref(a[0].as.handle, "image_p", st);
        if (!p0) return joy_v_err(st, *st, "image_set_blend_mode.arg0");
        p1 = (int)a[1].as.i32;
        image_set_blend_mode(p0, p1);
        return joy_v_null();
}

static joy_value_t t_image_set_scale_mode(joy_value_t* a, int n, joy_status_t* st)
{
        image_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "image_set_scale_mode");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "image_set_scale_mode.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "image_set_scale_mode.arg1");
        p0 = (image_p)joy_obj_deref(a[0].as.handle, "image_p", st);
        if (!p0) return joy_v_err(st, *st, "image_set_scale_mode.arg0");
        p1 = (int)a[1].as.i32;
        image_set_scale_mode(p0, p1);
        return joy_v_null();
}

static joy_value_t t_text_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jtext_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "text_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "text_destroy.arg0");
        p0 = (jtext_p)joy_obj_deref(a[0].as.handle, "jtext_p", st);
        if (!p0) return joy_v_err(st, *st, "text_destroy.arg0");
        text_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_jtext_print(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        jtext_p p1;
        float p2;
        float p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jtext_print");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtext_print.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jtext_print.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jtext_print.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jtext_print.arg3");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "jtext_print.arg0");
        p1 = (jtext_p)joy_obj_deref(a[1].as.handle, "jtext_p", st);
        if (!p1) return joy_v_err(st, *st, "jtext_print.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        jtext_print(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_r2d_draw_point_f(joy_value_t* a, int n, joy_status_t* st)
{
        jr2d_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "r2d_draw_point_f");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r2d_draw_point_f.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r2d_draw_point_f.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r2d_draw_point_f.arg2");
        p0 = (jr2d_p)joy_obj_deref(a[0].as.handle, "jr2d_p", st);
        if (!p0) return joy_v_err(st, *st, "r2d_draw_point_f.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = r2d_draw_point_f(p0, p1, p2);
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

static joy_value_t t_text_get_width(joy_value_t* a, int n, joy_status_t* st)
{
        jtext_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "text_get_width");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "text_get_width.arg0");
        p0 = (jtext_p)joy_obj_deref(a[0].as.handle, "jtext_p", st);
        if (!p0) return joy_v_err(st, *st, "text_get_width.arg0");
        _r = text_get_width(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_text_get_height(joy_value_t* a, int n, joy_status_t* st)
{
        jtext_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "text_get_height");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "text_get_height.arg0");
        p0 = (jtext_p)joy_obj_deref(a[0].as.handle, "jtext_p", st);
        if (!p0) return joy_v_err(st, *st, "text_get_height.arg0");
        _r = text_get_height(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_jtext_cache_clear(joy_value_t* a, int n, joy_status_t* st)
{
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jtext_cache_clear");
        jtext_cache_clear();
        return joy_v_null();
}

static joy_value_t t_jtext_cache_set_limit(joy_value_t* a, int n, joy_status_t* st)
{
        int p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jtext_cache_set_limit");
        if (a[0].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jtext_cache_set_limit.arg0");
        p0 = (int)a[0].as.i32;
        jtext_cache_set_limit(p0);
        return joy_v_null();
}

static joy_value_t t_jtext_cache_get_count(joy_value_t* a, int n, joy_status_t* st)
{
        int _r;
        if (n != 0) return joy_v_err(st, JOY_ERR_ARGC, "jtext_cache_get_count");
        _r = jtext_cache_get_count();
        return joy_v_i32((int)_r);
}

static joy_value_t t_jtext_cache_sweep(joy_value_t* a, int n, joy_status_t* st)
{
        int p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jtext_cache_sweep");
        if (a[0].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jtext_cache_sweep.arg0");
        p0 = (int)a[0].as.i32;
        jtext_cache_sweep(p0);
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

static joy_value_t t_r3d_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_destroy.arg0");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_destroy.arg0");
        r3d_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_begin(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_begin");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_begin.arg0");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_begin.arg0");
        _r = r3d_begin(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r3d_end(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_end.arg0");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_end.arg0");
        r3d_end(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_get_draw_calls(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_get_draw_calls");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_get_draw_calls.arg0");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_get_draw_calls.arg0");
        _r = r3d_get_draw_calls(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_r3d_set_clear_color(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        uint8_t p1;
        uint8_t p2;
        uint8_t p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "r3d_set_clear_color");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_set_clear_color.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "r3d_set_clear_color.arg1");
        if (a[2].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "r3d_set_clear_color.arg2");
        if (a[3].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "r3d_set_clear_color.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_set_clear_color.arg0");
        p1 = (uint8_t)a[1].as.u32;
        p2 = (uint8_t)a[2].as.u32;
        p3 = (uint8_t)a[3].as.u32;
        r3d_set_clear_color(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_r3d_mesh_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r3d_mesh_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_mesh_destroy.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_mesh_destroy.arg1");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_mesh_destroy.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_mesh_destroy.arg1");
        r3d_mesh_destroy(p0, p1);
        return joy_v_null();
}

static joy_value_t t_r3d_font_init(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        const char* p1;
        int p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "r3d_font_init");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_init.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_init.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_init.arg2");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_font_init.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        _r = r3d_font_init(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r3d_font_draw_2d(joy_value_t* a, int n, joy_status_t* st)
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
        if (n != 8) return joy_v_err(st, JOY_ERR_ARGC, "r3d_font_draw_2d");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_draw_2d.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_draw_2d.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_draw_2d.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_draw_2d.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_draw_2d.arg4");
        if (a[5].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_draw_2d.arg5");
        if (a[6].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_draw_2d.arg6");
        if (a[7].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_draw_2d.arg7");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_font_draw_2d.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_font_draw_2d.arg1");
        p2 = (const char*)a[2].as.str;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        p5 = (float)a[5].as.f64;
        p6 = (float)a[6].as.f64;
        p7 = (float)a[7].as.f64;
        _r = r3d_font_draw_2d(p0, p1, p2, p3, p4, p5, p6, p7);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r3d_font_cache_set_limit(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r3d_font_cache_set_limit");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_cache_set_limit.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_cache_set_limit.arg1");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_font_cache_set_limit.arg0");
        p1 = (int)a[1].as.i32;
        r3d_font_cache_set_limit(p0, p1);
        return joy_v_null();
}

static joy_value_t t_r3d_font_cache_get_count(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_font_cache_get_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_cache_get_count.arg0");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_font_cache_get_count.arg0");
        _r = r3d_font_cache_get_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_r3d_font_cache_clear(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_font_cache_clear");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_cache_clear.arg0");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_font_cache_clear.arg0");
        r3d_font_cache_clear(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_font_cache_sweep(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r3d_font_cache_sweep");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_cache_sweep.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r3d_font_cache_sweep.arg1");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_font_cache_sweep.arg0");
        p1 = (int)a[1].as.i32;
        r3d_font_cache_sweep(p0, p1);
        return joy_v_null();
}

static joy_value_t t_r3d_set_logical_presentation(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        int p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "r3d_set_logical_presentation");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_set_logical_presentation.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r3d_set_logical_presentation.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r3d_set_logical_presentation.arg2");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_set_logical_presentation.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        r3d_set_logical_presentation(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_r3d_set_logical_presentation_mode(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        int p1;
        int p2;
        int p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "r3d_set_logical_presentation_mode");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_set_logical_presentation_mode.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r3d_set_logical_presentation_mode.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r3d_set_logical_presentation_mode.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r3d_set_logical_presentation_mode.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_set_logical_presentation_mode.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        p3 = (int)a[3].as.i32;
        r3d_set_logical_presentation_mode(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_r3d_button_draw(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_button_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "r3d_button_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_draw.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_draw.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_draw.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_draw.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_button_draw.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_button_draw.arg1");
        p2 = (jr3d_button_p)joy_obj_deref(a[2].as.handle, "jr3d_button_p", st);
        if (!p2) return joy_v_err(st, *st, "r3d_button_draw.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        r3d_button_draw(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_r3d_button_draw_circle(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_button_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "r3d_button_draw_circle");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_draw_circle.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_draw_circle.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_draw_circle.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_draw_circle.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_draw_circle.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_button_draw_circle.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_button_draw_circle.arg1");
        p2 = (jr3d_button_p)joy_obj_deref(a[2].as.handle, "jr3d_button_p", st);
        if (!p2) return joy_v_err(st, *st, "r3d_button_draw_circle.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        r3d_button_draw_circle(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_r3d_button_is_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_button_p p1;
        float p2;
        float p3;
        bool _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "r3d_button_is_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_is_hovered.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_is_hovered.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_is_hovered.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_is_hovered.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_button_is_hovered.arg0");
        p1 = (jr3d_button_p)joy_obj_deref(a[1].as.handle, "jr3d_button_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_button_is_hovered.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = r3d_button_is_hovered(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r3d_button_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_button_p p1;
        bool p2;
        float p3;
        float p4;
        bool _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "r3d_button_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_is_pressed.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_is_pressed.arg1");
        if (a[2].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_is_pressed.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_is_pressed.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_is_pressed.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_button_is_pressed.arg0");
        p1 = (jr3d_button_p)joy_obj_deref(a[1].as.handle, "jr3d_button_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_button_is_pressed.arg1");
        p2 = (bool)a[2].as.b;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        _r = r3d_button_is_pressed(p0, p1, p2, p3, p4);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r3d_button_release(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_button_p p1;
        float p2;
        float p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "r3d_button_release");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_release.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_release.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_release.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_release.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_button_release.arg0");
        p1 = (jr3d_button_p)joy_obj_deref(a[1].as.handle, "jr3d_button_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_button_release.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        r3d_button_release(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_r3d_button_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_button_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_button_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_button_destroy.arg0");
        p0 = (jr3d_button_p)joy_obj_deref(a[0].as.handle, "jr3d_button_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_button_destroy.arg0");
        r3d_button_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_label_set_text(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_label_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r3d_label_set_text");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_label_set_text.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "r3d_label_set_text.arg1");
        p0 = (jr3d_label_p)joy_obj_deref(a[0].as.handle, "jr3d_label_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_label_set_text.arg0");
        p1 = (const char*)a[1].as.str;
        r3d_label_set_text(p0, p1);
        return joy_v_null();
}

static joy_value_t t_r3d_label_draw(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_label_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "r3d_label_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_label_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_label_draw.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_label_draw.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_label_draw.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_label_draw.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_label_draw.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_label_draw.arg1");
        p2 = (jr3d_label_p)joy_obj_deref(a[2].as.handle, "jr3d_label_p", st);
        if (!p2) return joy_v_err(st, *st, "r3d_label_draw.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        r3d_label_draw(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_r3d_label_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_label_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_label_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_label_destroy.arg0");
        p0 = (jr3d_label_p)joy_obj_deref(a[0].as.handle, "jr3d_label_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_label_destroy.arg0");
        r3d_label_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_input_set_text(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_set_text");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_set_text.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_set_text.arg1");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_set_text.arg0");
        p1 = (const char*)a[1].as.str;
        r3d_input_set_text(p0, p1);
        return joy_v_null();
}

static joy_value_t t_r3d_input_is_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_input_p p1;
        float p2;
        float p3;
        bool _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_is_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_is_hovered.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_is_hovered.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_is_hovered.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_is_hovered.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_is_hovered.arg0");
        p1 = (jr3d_input_p)joy_obj_deref(a[1].as.handle, "jr3d_input_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_input_is_hovered.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = r3d_input_is_hovered(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r3d_input_focus(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_focus");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_focus.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_focus.arg0");
        r3d_input_focus(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_input_defocus(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_defocus");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_defocus.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_defocus.arg0");
        r3d_input_defocus(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_input_is_focused(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_is_focused");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_is_focused.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_is_focused.arg0");
        _r = r3d_input_is_focused(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r3d_input_start_text_input(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_input_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_start_text_input");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_start_text_input.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_start_text_input.arg1");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_start_text_input.arg0");
        p1 = (jr3d_input_p)joy_obj_deref(a[1].as.handle, "jr3d_input_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_input_start_text_input.arg1");
        r3d_input_start_text_input(p0, p1);
        return joy_v_null();
}

static joy_value_t t_r3d_input_stop_text_input(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_input_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_stop_text_input");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_stop_text_input.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_stop_text_input.arg1");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_stop_text_input.arg0");
        p1 = (jr3d_input_p)joy_obj_deref(a[1].as.handle, "jr3d_input_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_input_stop_text_input.arg1");
        r3d_input_stop_text_input(p0, p1);
        return joy_v_null();
}

static joy_value_t t_r3d_input_insert(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_insert");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_insert.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_insert.arg1");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_insert.arg0");
        p1 = (const char*)a[1].as.str;
        r3d_input_insert(p0, p1);
        return joy_v_null();
}

static joy_value_t t_r3d_input_backspace(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_backspace");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_backspace.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_backspace.arg0");
        r3d_input_backspace(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_input_delete(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_delete");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_delete.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_delete.arg0");
        r3d_input_delete(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_input_cursor_left(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_cursor_left");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_cursor_left.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_cursor_left.arg0");
        r3d_input_cursor_left(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_input_cursor_right(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_cursor_right");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_cursor_right.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_cursor_right.arg0");
        r3d_input_cursor_right(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_input_cursor_home(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_cursor_home");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_cursor_home.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_cursor_home.arg0");
        r3d_input_cursor_home(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_input_cursor_end(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_cursor_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_cursor_end.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_cursor_end.arg0");
        r3d_input_cursor_end(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_input_click(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_input_p p1;
        float p2;
        float p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_click");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_click.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_click.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_click.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_click.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_click.arg0");
        p1 = (jr3d_input_p)joy_obj_deref(a[1].as.handle, "jr3d_input_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_input_click.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        r3d_input_click(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_r3d_input_submit(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_submit");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_submit.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_submit.arg0");
        r3d_input_submit(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_input_draw(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_input_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_draw.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_draw.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_draw.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_draw.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_draw.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_input_draw.arg1");
        p2 = (jr3d_input_p)joy_obj_deref(a[2].as.handle, "jr3d_input_p", st);
        if (!p2) return joy_v_err(st, *st, "r3d_input_draw.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        r3d_input_draw(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_r3d_input_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_input_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_input_destroy.arg0");
        p0 = (jr3d_input_p)joy_obj_deref(a[0].as.handle, "jr3d_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_input_destroy.arg0");
        r3d_input_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_combobox_input_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_combobox_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_combobox_input_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_destroy.arg0");
        p0 = (jr3d_combobox_input_p)joy_obj_deref(a[0].as.handle, "jr3d_combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_combobox_input_destroy.arg0");
        r3d_combobox_input_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_combobox_input_add_item(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_combobox_input_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r3d_combobox_input_add_item");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_add_item.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_add_item.arg1");
        p0 = (jr3d_combobox_input_p)joy_obj_deref(a[0].as.handle, "jr3d_combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_combobox_input_add_item.arg0");
        p1 = (const char*)a[1].as.str;
        r3d_combobox_input_add_item(p0, p1);
        return joy_v_null();
}

static joy_value_t t_r3d_combobox_input_clear_items(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_combobox_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_combobox_input_clear_items");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_clear_items.arg0");
        p0 = (jr3d_combobox_input_p)joy_obj_deref(a[0].as.handle, "jr3d_combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_combobox_input_clear_items.arg0");
        r3d_combobox_input_clear_items(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_combobox_input_set_text(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_combobox_input_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r3d_combobox_input_set_text");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_set_text.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_set_text.arg1");
        p0 = (jr3d_combobox_input_p)joy_obj_deref(a[0].as.handle, "jr3d_combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_combobox_input_set_text.arg0");
        p1 = (const char*)a[1].as.str;
        r3d_combobox_input_set_text(p0, p1);
        return joy_v_null();
}

static joy_value_t t_r3d_combobox_input_hit_item(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_combobox_input_p p0;
        float p1;
        float p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "r3d_combobox_input_hit_item");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_hit_item.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_hit_item.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_hit_item.arg2");
        p0 = (jr3d_combobox_input_p)joy_obj_deref(a[0].as.handle, "jr3d_combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_combobox_input_hit_item.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = r3d_combobox_input_hit_item(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_r3d_combobox_input_draw(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_combobox_input_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "r3d_combobox_input_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_draw.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_draw.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_draw.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_draw.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_combobox_input_draw.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_combobox_input_draw.arg1");
        p2 = (jr3d_combobox_input_p)joy_obj_deref(a[2].as.handle, "jr3d_combobox_input_p", st);
        if (!p2) return joy_v_err(st, *st, "r3d_combobox_input_draw.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        r3d_combobox_input_draw(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_r3d_combobox_input_draw_dropdown(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_combobox_input_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "r3d_combobox_input_draw_dropdown");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_draw_dropdown.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_draw_dropdown.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_draw_dropdown.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_draw_dropdown.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_combobox_input_draw_dropdown.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_combobox_input_draw_dropdown.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_combobox_input_draw_dropdown.arg1");
        p2 = (jr3d_combobox_input_p)joy_obj_deref(a[2].as.handle, "jr3d_combobox_input_p", st);
        if (!p2) return joy_v_err(st, *st, "r3d_combobox_input_draw_dropdown.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        r3d_combobox_input_draw_dropdown(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_r3d_slider_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_slider_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_slider_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_destroy.arg0");
        p0 = (jr3d_slider_p)joy_obj_deref(a[0].as.handle, "jr3d_slider_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_slider_destroy.arg0");
        r3d_slider_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_slider_set_value(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_slider_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r3d_slider_set_value");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_set_value.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_set_value.arg1");
        p0 = (jr3d_slider_p)joy_obj_deref(a[0].as.handle, "jr3d_slider_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_slider_set_value.arg0");
        p1 = (float)a[1].as.f64;
        r3d_slider_set_value(p0, p1);
        return joy_v_null();
}

static joy_value_t t_r3d_slider_get_value(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_slider_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_slider_get_value");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_get_value.arg0");
        p0 = (jr3d_slider_p)joy_obj_deref(a[0].as.handle, "jr3d_slider_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_slider_get_value.arg0");
        _r = r3d_slider_get_value(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_r3d_slider_draw(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_slider_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "r3d_slider_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_draw.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_draw.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_draw.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_draw.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_slider_draw.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_slider_draw.arg1");
        p2 = (jr3d_slider_p)joy_obj_deref(a[2].as.handle, "jr3d_slider_p", st);
        if (!p2) return joy_v_err(st, *st, "r3d_slider_draw.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        r3d_slider_draw(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_r3d_slider_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_slider_p p1;
        float p2;
        float p3;
        bool _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "r3d_slider_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_is_pressed.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_is_pressed.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_is_pressed.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_slider_is_pressed.arg0");
        p1 = (jr3d_slider_p)joy_obj_deref(a[1].as.handle, "jr3d_slider_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_slider_is_pressed.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = r3d_slider_is_pressed(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r3d_slider_set_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_slider_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r3d_slider_set_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_set_pressed.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_set_pressed.arg1");
        p0 = (jr3d_slider_p)joy_obj_deref(a[0].as.handle, "jr3d_slider_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_slider_set_pressed.arg0");
        p1 = (bool)a[1].as.b;
        r3d_slider_set_pressed(p0, p1);
        return joy_v_null();
}

static joy_value_t t_r3d_slider_drag_move(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_slider_p p1;
        float p2;
        float p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "r3d_slider_drag_move");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_drag_move.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_drag_move.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_drag_move.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_drag_move.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_slider_drag_move.arg0");
        p1 = (jr3d_slider_p)joy_obj_deref(a[1].as.handle, "jr3d_slider_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_slider_drag_move.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        r3d_slider_drag_move(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_r3d_slider_is_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_slider_p p1;
        float p2;
        float p3;
        bool _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "r3d_slider_is_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_is_hovered.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_is_hovered.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_is_hovered.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_is_hovered.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_slider_is_hovered.arg0");
        p1 = (jr3d_slider_p)joy_obj_deref(a[1].as.handle, "jr3d_slider_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_slider_is_hovered.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = r3d_slider_is_hovered(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r3d_slider_set_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_slider_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r3d_slider_set_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_set_hovered.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "r3d_slider_set_hovered.arg1");
        p0 = (jr3d_slider_p)joy_obj_deref(a[0].as.handle, "jr3d_slider_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_slider_set_hovered.arg0");
        p1 = (bool)a[1].as.b;
        r3d_slider_set_hovered(p0, p1);
        return joy_v_null();
}

static joy_value_t t_r3d_checkbox_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_checkbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_checkbox_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_destroy.arg0");
        p0 = (jr3d_checkbox_p)joy_obj_deref(a[0].as.handle, "jr3d_checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_checkbox_destroy.arg0");
        r3d_checkbox_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_checkbox_toggle(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_checkbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "r3d_checkbox_toggle");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_toggle.arg0");
        p0 = (jr3d_checkbox_p)joy_obj_deref(a[0].as.handle, "jr3d_checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_checkbox_toggle.arg0");
        r3d_checkbox_toggle(p0);
        return joy_v_null();
}

static joy_value_t t_r3d_checkbox_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_checkbox_p p1;
        float p2;
        float p3;
        bool _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "r3d_checkbox_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_is_pressed.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_is_pressed.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_is_pressed.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_checkbox_is_pressed.arg0");
        p1 = (jr3d_checkbox_p)joy_obj_deref(a[1].as.handle, "jr3d_checkbox_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_checkbox_is_pressed.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = r3d_checkbox_is_pressed(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r3d_checkbox_set_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_checkbox_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "r3d_checkbox_set_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_set_hovered.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_set_hovered.arg1");
        p0 = (jr3d_checkbox_p)joy_obj_deref(a[0].as.handle, "jr3d_checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_checkbox_set_hovered.arg0");
        p1 = (bool)a[1].as.b;
        r3d_checkbox_set_hovered(p0, p1);
        return joy_v_null();
}

static joy_value_t t_r3d_checkbox_is_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        jr3d_checkbox_p p1;
        float p2;
        float p3;
        bool _r;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "r3d_checkbox_is_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_is_hovered.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_is_hovered.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_is_hovered.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_is_hovered.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_checkbox_is_hovered.arg0");
        p1 = (jr3d_checkbox_p)joy_obj_deref(a[1].as.handle, "jr3d_checkbox_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_checkbox_is_hovered.arg1");
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        _r = r3d_checkbox_is_hovered(p0, p1, p2, p3);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_r3d_checkbox_draw(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        render_mesh_p p1;
        jr3d_checkbox_p p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "r3d_checkbox_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_draw.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_draw.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_draw.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_checkbox_draw.arg4");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_checkbox_draw.arg0");
        p1 = (render_mesh_p)joy_obj_deref(a[1].as.handle, "render_mesh_p", st);
        if (!p1) return joy_v_err(st, *st, "r3d_checkbox_draw.arg1");
        p2 = (jr3d_checkbox_p)joy_obj_deref(a[2].as.handle, "jr3d_checkbox_p", st);
        if (!p2) return joy_v_err(st, *st, "r3d_checkbox_draw.arg2");
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        r3d_checkbox_draw(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_r3d_draw_grid(joy_value_t* a, int n, joy_status_t* st)
{
        jr3d_p p0;
        int p1;
        float p2;
        float p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "r3d_draw_grid");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "r3d_draw_grid.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "r3d_draw_grid.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_draw_grid.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "r3d_draw_grid.arg3");
        p0 = (jr3d_p)joy_obj_deref(a[0].as.handle, "jr3d_p", st);
        if (!p0) return joy_v_err(st, *st, "r3d_draw_grid.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        r3d_draw_grid(p0, p1, p2, p3);
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

static joy_value_t t_label_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        label_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "label_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "label_destroy.arg0");
        p0 = (label_p)joy_obj_deref(a[0].as.handle, "label_p", st);
        if (!p0) return joy_v_err(st, *st, "label_destroy.arg0");
        label_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_label_draw(joy_value_t* a, int n, joy_status_t* st)
{
        label_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "label_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "label_draw.arg0");
        p0 = (label_p)joy_obj_deref(a[0].as.handle, "label_p", st);
        if (!p0) return joy_v_err(st, *st, "label_draw.arg0");
        label_draw(p0);
        return joy_v_null();
}

static joy_value_t t_button_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "button_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "button_destroy.arg0");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "button_destroy.arg0");
        button_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_button_set_position(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "button_set_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "button_set_position.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "button_set_position.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "button_set_position.arg2");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "button_set_position.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        button_set_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_button_set_size(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "button_set_size");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "button_set_size.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "button_set_size.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "button_set_size.arg2");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "button_set_size.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        button_set_size(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_button_set_image(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        image_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "button_set_image");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "button_set_image.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "button_set_image.arg1");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "button_set_image.arg0");
        p1 = (image_p)joy_obj_deref(a[1].as.handle, "image_p", st);
        if (!p1) return joy_v_err(st, *st, "button_set_image.arg1");
        button_set_image(p0, p1);
        return joy_v_null();
}

static joy_value_t t_button_set_pressed_image(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        image_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "button_set_pressed_image");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "button_set_pressed_image.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "button_set_pressed_image.arg1");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "button_set_pressed_image.arg0");
        p1 = (image_p)joy_obj_deref(a[1].as.handle, "image_p", st);
        if (!p1) return joy_v_err(st, *st, "button_set_pressed_image.arg1");
        button_set_pressed_image(p0, p1);
        return joy_v_null();
}

static joy_value_t t_button_is_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "button_is_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "button_is_hovered.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "button_is_hovered.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "button_is_hovered.arg2");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "button_is_hovered.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = button_is_hovered(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_button_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "button_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "button_is_pressed.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "button_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "button_is_pressed.arg2");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "button_is_pressed.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = button_is_pressed(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_button_set_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "button_set_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "button_set_hovered.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "button_set_hovered.arg1");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "button_set_hovered.arg0");
        p1 = (bool)a[1].as.b;
        button_set_hovered(p0, p1);
        return joy_v_null();
}

static joy_value_t t_button_set_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "button_set_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "button_set_pressed.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "button_set_pressed.arg1");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "button_set_pressed.arg0");
        p1 = (bool)a[1].as.b;
        button_set_pressed(p0, p1);
        return joy_v_null();
}

static joy_value_t t_button_get_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "button_get_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "button_get_pressed.arg0");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "button_get_pressed.arg0");
        _r = button_get_pressed(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_button_set_text_offset(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "button_set_text_offset");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "button_set_text_offset.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "button_set_text_offset.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "button_set_text_offset.arg2");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "button_set_text_offset.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        button_set_text_offset(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_button_draw(joy_value_t* a, int n, joy_status_t* st)
{
        button_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "button_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "button_draw.arg0");
        p0 = (button_p)joy_obj_deref(a[0].as.handle, "button_p", st);
        if (!p0) return joy_v_err(st, *st, "button_draw.arg0");
        button_draw(p0);
        return joy_v_null();
}

static joy_value_t t_scrollview_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scrollview_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_destroy.arg0");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "scrollview_destroy.arg0");
        scrollview_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_scrollview_set_view_px(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        float p1;
        float p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "scrollview_set_view_px");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_set_view_px.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_set_view_px.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_set_view_px.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_set_view_px.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_set_view_px.arg4");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "scrollview_set_view_px.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        scrollview_set_view_px(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_scrollview_set_content(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "scrollview_set_content");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_set_content.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_set_content.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_set_content.arg2");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "scrollview_set_content.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        scrollview_set_content(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_scrollview_set_offset(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "scrollview_set_offset");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_set_offset.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_set_offset.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_set_offset.arg2");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "scrollview_set_offset.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        scrollview_set_offset(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_scrollview_clamp(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scrollview_clamp");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_clamp.arg0");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "scrollview_clamp.arg0");
        scrollview_clamp(p0);
        return joy_v_null();
}

static joy_value_t t_scrollview_get_offset_x(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scrollview_get_offset_x");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_get_offset_x.arg0");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "scrollview_get_offset_x.arg0");
        _r = scrollview_get_offset_x(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_scrollview_get_offset_y(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scrollview_get_offset_y");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_get_offset_y.arg0");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "scrollview_get_offset_y.arg0");
        _r = scrollview_get_offset_y(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_scrollview_set_show_bar(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "scrollview_set_show_bar");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_set_show_bar.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_set_show_bar.arg1");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "scrollview_set_show_bar.arg0");
        p1 = (bool)a[1].as.b;
        scrollview_set_show_bar(p0, p1);
        return joy_v_null();
}

static joy_value_t t_scrollview_dragging(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scrollview_dragging");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_dragging.arg0");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "scrollview_dragging.arg0");
        _r = scrollview_dragging(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_scrollview_hit(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "scrollview_hit");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_hit.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_hit.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_hit.arg2");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "scrollview_hit.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = scrollview_hit(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_scrollview_begin(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scrollview_begin");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_begin.arg0");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "scrollview_begin.arg0");
        scrollview_begin(p0);
        return joy_v_null();
}

static joy_value_t t_scrollview_end(joy_value_t* a, int n, joy_status_t* st)
{
        scrollview_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "scrollview_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "scrollview_end.arg0");
        p0 = (scrollview_p)joy_obj_deref(a[0].as.handle, "scrollview_p", st);
        if (!p0) return joy_v_err(st, *st, "scrollview_end.arg0");
        scrollview_end(p0);
        return joy_v_null();
}

static joy_value_t t_combobox_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "combobox_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_destroy.arg0");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_destroy.arg0");
        combobox_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_combobox_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "combobox_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_is_pressed.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "combobox_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "combobox_is_pressed.arg2");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_is_pressed.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = combobox_is_pressed(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_combobox_is_open(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "combobox_is_open");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_is_open.arg0");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_is_open.arg0");
        _r = combobox_is_open(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_combobox_set_open(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "combobox_set_open");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_set_open.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "combobox_set_open.arg1");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_set_open.arg0");
        p1 = (bool)a[1].as.b;
        combobox_set_open(p0, p1);
        return joy_v_null();
}

static joy_value_t t_combobox_hit_item(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        float p1;
        float p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "combobox_hit_item");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_hit_item.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "combobox_hit_item.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "combobox_hit_item.arg2");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_hit_item.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = combobox_hit_item(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_combobox_set_selected(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "combobox_set_selected");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_set_selected.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "combobox_set_selected.arg1");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_set_selected.arg0");
        p1 = (int)a[1].as.i32;
        combobox_set_selected(p0, p1);
        return joy_v_null();
}

static joy_value_t t_combobox_get_selected(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "combobox_get_selected");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_get_selected.arg0");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_get_selected.arg0");
        _r = combobox_get_selected(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_combobox_draw(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "combobox_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_draw.arg0");
        p0 = (combobox_p)joy_obj_deref(a[0].as.handle, "combobox_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_draw.arg0");
        combobox_draw(p0);
        return joy_v_null();
}

static joy_value_t t_combobox_input_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "combobox_input_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_destroy.arg0");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_input_destroy.arg0");
        combobox_input_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_combobox_input_add_item(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "combobox_input_add_item");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_add_item.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_add_item.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_add_item.arg2");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_input_add_item.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        combobox_input_add_item(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_combobox_input_clear_items(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "combobox_input_clear_items");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_clear_items.arg0");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_input_clear_items.arg0");
        combobox_input_clear_items(p0);
        return joy_v_null();
}

static joy_value_t t_combobox_input_set_text(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "combobox_input_set_text");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_set_text.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_set_text.arg1");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_input_set_text.arg0");
        p1 = (const char*)a[1].as.str;
        combobox_input_set_text(p0, p1);
        return joy_v_null();
}

static joy_value_t t_combobox_input_draw(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "combobox_input_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_draw.arg0");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_input_draw.arg0");
        combobox_input_draw(p0);
        return joy_v_null();
}

static joy_value_t t_combobox_input_draw_dropdown(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "combobox_input_draw_dropdown");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_draw_dropdown.arg0");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_input_draw_dropdown.arg0");
        combobox_input_draw_dropdown(p0);
        return joy_v_null();
}

static joy_value_t t_combobox_input_is_open(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "combobox_input_is_open");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_is_open.arg0");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_input_is_open.arg0");
        _r = combobox_input_is_open(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_combobox_input_set_popup_node(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        scene_node_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "combobox_input_set_popup_node");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_set_popup_node.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_set_popup_node.arg1");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_input_set_popup_node.arg0");
        p1 = (scene_node_p)joy_obj_deref(a[1].as.handle, "scene_node_p", st);
        if (!p1) return joy_v_err(st, *st, "combobox_input_set_popup_node.arg1");
        combobox_input_set_popup_node(p0, p1);
        return joy_v_null();
}

static joy_value_t t_combobox_input_hit_item(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        float p1;
        float p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "combobox_input_hit_item");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_hit_item.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_hit_item.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_hit_item.arg2");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_input_hit_item.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = combobox_input_hit_item(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_combobox_input_set_open(joy_value_t* a, int n, joy_status_t* st)
{
        combobox_input_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "combobox_input_set_open");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_set_open.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "combobox_input_set_open.arg1");
        p0 = (combobox_input_p)joy_obj_deref(a[0].as.handle, "combobox_input_p", st);
        if (!p0) return joy_v_err(st, *st, "combobox_input_set_open.arg0");
        p1 = (bool)a[1].as.b;
        combobox_input_set_open(p0, p1);
        return joy_v_null();
}

static joy_value_t t_datagrid_setheaders(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "datagrid_setheaders");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_setheaders.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_setheaders.arg1");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "datagrid_setheaders.arg0");
        p1 = (const char*)a[1].as.str;
        datagrid_setheaders(p0, p1);
        return joy_v_null();
}

static joy_value_t t_datagrid_setheader(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        int p1;
        const char* p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "datagrid_setheader");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_setheader.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_setheader.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_setheader.arg2");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "datagrid_setheader.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (const char*)a[2].as.str;
        datagrid_setheader(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_datagrid_setcell(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        int p1;
        int p2;
        const char* p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "datagrid_setcell");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_setcell.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_setcell.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_setcell.arg2");
        if (a[3].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_setcell.arg3");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "datagrid_setcell.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        p3 = (const char*)a[3].as.str;
        datagrid_setcell(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_datagrid_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "datagrid_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_destroy.arg0");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "datagrid_destroy.arg0");
        datagrid_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_datagrid_draw(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        jr2d_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "datagrid_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_draw.arg1");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "datagrid_draw.arg0");
        p1 = (jr2d_p)joy_obj_deref(a[1].as.handle, "jr2d_p", st);
        if (!p1) return joy_v_err(st, *st, "datagrid_draw.arg1");
        datagrid_draw(p0, p1);
        return joy_v_null();
}

static joy_value_t t_datagrid_hit_row(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        float p1;
        float p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "datagrid_hit_row");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_hit_row.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_hit_row.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_hit_row.arg2");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "datagrid_hit_row.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = datagrid_hit_row(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_datagrid_scroll(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "datagrid_scroll");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_scroll.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_scroll.arg1");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "datagrid_scroll.arg0");
        p1 = (int)a[1].as.i32;
        datagrid_scroll(p0, p1);
        return joy_v_null();
}

static joy_value_t t_datagrid_set_selected_row(joy_value_t* a, int n, joy_status_t* st)
{
        datagrid_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "datagrid_set_selected_row");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_set_selected_row.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "datagrid_set_selected_row.arg1");
        p0 = (datagrid_p)joy_obj_deref(a[0].as.handle, "datagrid_p", st);
        if (!p0) return joy_v_err(st, *st, "datagrid_set_selected_row.arg0");
        p1 = (int)a[1].as.i32;
        datagrid_set_selected_row(p0, p1);
        return joy_v_null();
}

static joy_value_t t_radio_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        radio_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "radio_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "radio_destroy.arg0");
        p0 = (radio_p)joy_obj_deref(a[0].as.handle, "radio_p", st);
        if (!p0) return joy_v_err(st, *st, "radio_destroy.arg0");
        radio_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_radio_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        radio_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "radio_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "radio_is_pressed.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "radio_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "radio_is_pressed.arg2");
        p0 = (radio_p)joy_obj_deref(a[0].as.handle, "radio_p", st);
        if (!p0) return joy_v_err(st, *st, "radio_is_pressed.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = radio_is_pressed(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_radio_set_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        radio_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "radio_set_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "radio_set_hovered.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "radio_set_hovered.arg1");
        p0 = (radio_p)joy_obj_deref(a[0].as.handle, "radio_p", st);
        if (!p0) return joy_v_err(st, *st, "radio_set_hovered.arg0");
        p1 = (bool)a[1].as.b;
        radio_set_hovered(p0, p1);
        return joy_v_null();
}

static joy_value_t t_checkbox_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        checkbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "checkbox_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "checkbox_destroy.arg0");
        p0 = (checkbox_p)joy_obj_deref(a[0].as.handle, "checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "checkbox_destroy.arg0");
        checkbox_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_checkbox_set_r2d(joy_value_t* a, int n, joy_status_t* st)
{
        checkbox_p p0;
        jr2d_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "checkbox_set_r2d");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "checkbox_set_r2d.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "checkbox_set_r2d.arg1");
        p0 = (checkbox_p)joy_obj_deref(a[0].as.handle, "checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "checkbox_set_r2d.arg0");
        p1 = (jr2d_p)joy_obj_deref(a[1].as.handle, "jr2d_p", st);
        if (!p1) return joy_v_err(st, *st, "checkbox_set_r2d.arg1");
        checkbox_set_r2d(p0, p1);
        return joy_v_null();
}

static joy_value_t t_checkbox_set_position(joy_value_t* a, int n, joy_status_t* st)
{
        checkbox_p p0;
        int p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "checkbox_set_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "checkbox_set_position.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "checkbox_set_position.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "checkbox_set_position.arg2");
        p0 = (checkbox_p)joy_obj_deref(a[0].as.handle, "checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "checkbox_set_position.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        checkbox_set_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_checkbox_toggle(joy_value_t* a, int n, joy_status_t* st)
{
        checkbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "checkbox_toggle");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "checkbox_toggle.arg0");
        p0 = (checkbox_p)joy_obj_deref(a[0].as.handle, "checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "checkbox_toggle.arg0");
        checkbox_toggle(p0);
        return joy_v_null();
}

static joy_value_t t_checkbox_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        checkbox_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "checkbox_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "checkbox_is_pressed.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "checkbox_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "checkbox_is_pressed.arg2");
        p0 = (checkbox_p)joy_obj_deref(a[0].as.handle, "checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "checkbox_is_pressed.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = checkbox_is_pressed(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_checkbox_set_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        checkbox_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "checkbox_set_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "checkbox_set_hovered.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "checkbox_set_hovered.arg1");
        p0 = (checkbox_p)joy_obj_deref(a[0].as.handle, "checkbox_p", st);
        if (!p0) return joy_v_err(st, *st, "checkbox_set_hovered.arg0");
        p1 = (bool)a[1].as.b;
        checkbox_set_hovered(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joystick_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joystick_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joystick_destroy.arg0");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joystick_destroy.arg0");
        joystick_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_joystick_set_textures(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        const char* p1;
        const char* p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joystick_set_textures");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joystick_set_textures.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joystick_set_textures.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "joystick_set_textures.arg2");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joystick_set_textures.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (const char*)a[2].as.str;
        joystick_set_textures(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joystick_set_vector(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joystick_set_vector");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joystick_set_vector.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joystick_set_vector.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joystick_set_vector.arg2");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joystick_set_vector.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joystick_set_vector(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joystick_draw(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joystick_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joystick_draw.arg0");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joystick_draw.arg0");
        joystick_draw(p0);
        return joy_v_null();
}

static joy_value_t t_joystick_reset(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joystick_reset");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joystick_reset.arg0");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joystick_reset.arg0");
        joystick_reset(p0);
        return joy_v_null();
}

static joy_value_t t_joystick_set_position(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joystick_set_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joystick_set_position.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joystick_set_position.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joystick_set_position.arg2");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joystick_set_position.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joystick_set_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joystick_set_radius(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "joystick_set_radius");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joystick_set_radius.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joystick_set_radius.arg1");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joystick_set_radius.arg0");
        p1 = (float)a[1].as.f64;
        joystick_set_radius(p0, p1);
        return joy_v_null();
}

static joy_value_t t_joystick_is_dragging(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joystick_is_dragging");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joystick_is_dragging.arg0");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joystick_is_dragging.arg0");
        _r = joystick_is_dragging(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_joystick_touch_down(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joystick_touch_down");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joystick_touch_down.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joystick_touch_down.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joystick_touch_down.arg2");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joystick_touch_down.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joystick_touch_down(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joystick_touch_move(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "joystick_touch_move");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joystick_touch_move.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joystick_touch_move.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "joystick_touch_move.arg2");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joystick_touch_move.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        joystick_touch_move(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_joystick_touch_up(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joystick_touch_up");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joystick_touch_up.arg0");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joystick_touch_up.arg0");
        joystick_touch_up(p0);
        return joy_v_null();
}

static joy_value_t t_joystick_get_magnitude(joy_value_t* a, int n, joy_status_t* st)
{
        joystick_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "joystick_get_magnitude");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "joystick_get_magnitude.arg0");
        p0 = (joystick_p)joy_obj_deref(a[0].as.handle, "joystick_p", st);
        if (!p0) return joy_v_err(st, *st, "joystick_get_magnitude.arg0");
        _r = joystick_get_magnitude(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_inputbox_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_destroy.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_destroy.arg0");
        inputbox_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_set_text(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        const char* p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_set_text");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_set_text.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_set_text.arg1");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_set_text.arg0");
        p1 = (const char*)a[1].as.str;
        inputbox_set_text(p0, p1);
        return joy_v_null();
}

static joy_value_t t_inputbox_draw(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_draw.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_draw.arg0");
        inputbox_draw(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_is_pressed.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_is_pressed.arg2");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_is_pressed.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = inputbox_is_pressed(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_inputbox_is_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_is_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_is_hovered.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_is_hovered.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_is_hovered.arg2");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_is_hovered.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = inputbox_is_hovered(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_inputbox_focus(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_focus");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_focus.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_focus.arg0");
        inputbox_focus(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_defocus(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_defocus");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_defocus.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_defocus.arg0");
        inputbox_defocus(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_blur(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_blur");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_blur.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_blur.arg0");
        inputbox_blur(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_insert(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_insert");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_insert.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_insert.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_insert.arg2");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_insert.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        inputbox_insert(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_inputbox_backspace(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_backspace");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_backspace.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_backspace.arg0");
        inputbox_backspace(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_delete(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_delete");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_delete.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_delete.arg0");
        inputbox_delete(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_cursor_left(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_cursor_left");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_cursor_left.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_cursor_left.arg0");
        inputbox_cursor_left(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_cursor_right(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_cursor_right");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_cursor_right.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_cursor_right.arg0");
        inputbox_cursor_right(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_cursor_home(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_cursor_home");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_cursor_home.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_cursor_home.arg0");
        inputbox_cursor_home(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_cursor_end(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_cursor_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_cursor_end.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_cursor_end.arg0");
        inputbox_cursor_end(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_select_left(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_select_left");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_select_left.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_select_left.arg0");
        inputbox_select_left(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_select_right(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_select_right");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_select_right.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_select_right.arg0");
        inputbox_select_right(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_select_home(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_select_home");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_select_home.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_select_home.arg0");
        inputbox_select_home(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_select_end(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_select_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_select_end.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_select_end.arg0");
        inputbox_select_end(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_select_all(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_select_all");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_select_all.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_select_all.arg0");
        inputbox_select_all(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_paste(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_paste");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_paste.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_paste.arg0");
        inputbox_paste(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_copy(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_copy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_copy.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_copy.arg0");
        inputbox_copy(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_cut(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_cut");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_cut.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_cut.arg0");
        inputbox_cut(p0);
        return joy_v_null();
}

static joy_value_t t_inputbox_is_focused(joy_value_t* a, int n, joy_status_t* st)
{
        inputbox_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "inputbox_is_focused");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "inputbox_is_focused.arg0");
        p0 = (inputbox_p)joy_obj_deref(a[0].as.handle, "inputbox_p", st);
        if (!p0) return joy_v_err(st, *st, "inputbox_is_focused.arg0");
        _r = inputbox_is_focused(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_slider_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "slider_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "slider_destroy.arg0");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "slider_destroy.arg0");
        slider_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_slider_set_value(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "slider_set_value");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "slider_set_value.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "slider_set_value.arg1");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "slider_set_value.arg0");
        p1 = (float)a[1].as.f64;
        slider_set_value(p0, p1);
        return joy_v_null();
}

static joy_value_t t_slider_get_value(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "slider_get_value");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "slider_get_value.arg0");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "slider_get_value.arg0");
        _r = slider_get_value(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_slider_set_rect(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        float p1;
        float p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "slider_set_rect");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "slider_set_rect.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "slider_set_rect.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "slider_set_rect.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "slider_set_rect.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "slider_set_rect.arg4");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "slider_set_rect.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        slider_set_rect(p0, p1, p2, p3, p4);
        return joy_v_null();
}

static joy_value_t t_slider_set_range(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "slider_set_range");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "slider_set_range.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "slider_set_range.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "slider_set_range.arg2");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "slider_set_range.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        slider_set_range(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_slider_draw(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "slider_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "slider_draw.arg0");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "slider_draw.arg0");
        slider_draw(p0);
        return joy_v_null();
}

static joy_value_t t_slider_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "slider_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "slider_is_pressed.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "slider_is_pressed.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "slider_is_pressed.arg2");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "slider_is_pressed.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = slider_is_pressed(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_slider_set_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "slider_set_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "slider_set_pressed.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "slider_set_pressed.arg1");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "slider_set_pressed.arg0");
        p1 = (bool)a[1].as.b;
        slider_set_pressed(p0, p1);
        return joy_v_null();
}

static joy_value_t t_slider_drag_move(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "slider_drag_move");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "slider_drag_move.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "slider_drag_move.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "slider_drag_move.arg2");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "slider_drag_move.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        slider_drag_move(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_slider_is_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "slider_is_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "slider_is_hovered.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "slider_is_hovered.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "slider_is_hovered.arg2");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "slider_is_hovered.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = slider_is_hovered(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_slider_set_hovered(joy_value_t* a, int n, joy_status_t* st)
{
        slider_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "slider_set_hovered");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "slider_set_hovered.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "slider_set_hovered.arg1");
        p0 = (slider_p)joy_obj_deref(a[0].as.handle, "slider_p", st);
        if (!p0) return joy_v_err(st, *st, "slider_set_hovered.arg0");
        p1 = (bool)a[1].as.b;
        slider_set_hovered(p0, p1);
        return joy_v_null();
}

static joy_value_t t_popup_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "popup_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "popup_destroy.arg0");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "popup_destroy.arg0");
        popup_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_popup_set_title(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "popup_set_title");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "popup_set_title.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "popup_set_title.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "popup_set_title.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "popup_set_title.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        popup_set_title(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_popup_set_content(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "popup_set_content");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "popup_set_content.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "popup_set_content.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "popup_set_content.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "popup_set_content.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        popup_set_content(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_popup_set_position(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "popup_set_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "popup_set_position.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "popup_set_position.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "popup_set_position.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "popup_set_position.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        popup_set_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_popup_set_visible(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "popup_set_visible");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "popup_set_visible.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "popup_set_visible.arg1");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "popup_set_visible.arg0");
        p1 = (bool)a[1].as.b;
        popup_set_visible(p0, p1);
        return joy_v_null();
}

static joy_value_t t_popup_set_closeable(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "popup_set_closeable");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "popup_set_closeable.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "popup_set_closeable.arg1");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "popup_set_closeable.arg0");
        p1 = (bool)a[1].as.b;
        popup_set_closeable(p0, p1);
        return joy_v_null();
}

static joy_value_t t_popup_draw(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        font_p p1;
        font_p p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "popup_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "popup_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "popup_draw.arg1");
        if (a[2].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "popup_draw.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "popup_draw.arg0");
        p1 = (font_p)joy_obj_deref(a[1].as.handle, "font_p", st);
        if (!p1) return joy_v_err(st, *st, "popup_draw.arg1");
        p2 = (font_p)joy_obj_deref(a[2].as.handle, "font_p", st);
        if (!p2) return joy_v_err(st, *st, "popup_draw.arg2");
        popup_draw(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_popup_is_close_clicked(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "popup_is_close_clicked");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "popup_is_close_clicked.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "popup_is_close_clicked.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "popup_is_close_clicked.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "popup_is_close_clicked.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = popup_is_close_clicked(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_popup_is_title_clicked(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "popup_is_title_clicked");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "popup_is_title_clicked.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "popup_is_title_clicked.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "popup_is_title_clicked.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "popup_is_title_clicked.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = popup_is_title_clicked(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_popup_drag_move(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "popup_drag_move");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "popup_drag_move.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "popup_drag_move.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "popup_drag_move.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "popup_drag_move.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        popup_drag_move(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_popup_is_inside(joy_value_t* a, int n, joy_status_t* st)
{
        popup_p p0;
        float p1;
        float p2;
        bool _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "popup_is_inside");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "popup_is_inside.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "popup_is_inside.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "popup_is_inside.arg2");
        p0 = (popup_p)joy_obj_deref(a[0].as.handle, "popup_p", st);
        if (!p0) return joy_v_err(st, *st, "popup_is_inside.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        _r = popup_is_inside(p0, p1, p2);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_tooltip_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        tooltip_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "tooltip_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tooltip_destroy.arg0");
        p0 = (tooltip_p)joy_obj_deref(a[0].as.handle, "tooltip_p", st);
        if (!p0) return joy_v_err(st, *st, "tooltip_destroy.arg0");
        tooltip_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_tooltip_set_text(joy_value_t* a, int n, joy_status_t* st)
{
        tooltip_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "tooltip_set_text");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tooltip_set_text.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "tooltip_set_text.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "tooltip_set_text.arg2");
        p0 = (tooltip_p)joy_obj_deref(a[0].as.handle, "tooltip_p", st);
        if (!p0) return joy_v_err(st, *st, "tooltip_set_text.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        tooltip_set_text(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_tooltip_set_position(joy_value_t* a, int n, joy_status_t* st)
{
        tooltip_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "tooltip_set_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tooltip_set_position.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "tooltip_set_position.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "tooltip_set_position.arg2");
        p0 = (tooltip_p)joy_obj_deref(a[0].as.handle, "tooltip_p", st);
        if (!p0) return joy_v_err(st, *st, "tooltip_set_position.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        tooltip_set_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_tooltip_set_visible(joy_value_t* a, int n, joy_status_t* st)
{
        tooltip_p p0;
        bool p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "tooltip_set_visible");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tooltip_set_visible.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "tooltip_set_visible.arg1");
        p0 = (tooltip_p)joy_obj_deref(a[0].as.handle, "tooltip_p", st);
        if (!p0) return joy_v_err(st, *st, "tooltip_set_visible.arg0");
        p1 = (bool)a[1].as.b;
        tooltip_set_visible(p0, p1);
        return joy_v_null();
}

static joy_value_t t_tooltip_draw(joy_value_t* a, int n, joy_status_t* st)
{
        tooltip_p p0;
        font_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "tooltip_draw");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tooltip_draw.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tooltip_draw.arg1");
        p0 = (tooltip_p)joy_obj_deref(a[0].as.handle, "tooltip_p", st);
        if (!p0) return joy_v_err(st, *st, "tooltip_draw.arg0");
        p1 = (font_p)joy_obj_deref(a[1].as.handle, "font_p", st);
        if (!p1) return joy_v_err(st, *st, "tooltip_draw.arg1");
        tooltip_draw(p0, p1);
        return joy_v_null();
}

static joy_value_t t_jui_ctx_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jui_ctx_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_ctx_destroy.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_ctx_destroy.arg0");
        jui_ctx_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_jui_begin(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jui_begin");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_begin.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_begin.arg0");
        jui_begin(p0);
        return joy_v_null();
}

static joy_value_t t_jui_end(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jui_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_end.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_end.arg0");
        jui_end(p0);
        return joy_v_null();
}

static joy_value_t t_jui_input_motion(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jui_input_motion");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_input_motion.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jui_input_motion.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jui_input_motion.arg2");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_input_motion.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        jui_input_motion(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_jui_input_down(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jui_input_down");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_input_down.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jui_input_down.arg1");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_input_down.arg0");
        p1 = (int)a[1].as.i32;
        jui_input_down(p0, p1);
        return joy_v_null();
}

static joy_value_t t_jui_input_up(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jui_input_up");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_input_up.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jui_input_up.arg1");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_input_up.arg0");
        p1 = (int)a[1].as.i32;
        jui_input_up(p0, p1);
        return joy_v_null();
}

static joy_value_t t_jui_input_wheel(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jui_input_wheel");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_input_wheel.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jui_input_wheel.arg1");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_input_wheel.arg0");
        p1 = (float)a[1].as.f64;
        jui_input_wheel(p0, p1);
        return joy_v_null();
}

static joy_value_t t_jui_input_key(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        bool p1;
        bool p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jui_input_key");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_input_key.arg0");
        if (a[1].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "jui_input_key.arg1");
        if (a[2].kind != JOY_V_BOOL) return joy_v_err(st, JOY_ERR_TYPE, "jui_input_key.arg2");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_input_key.arg0");
        p1 = (bool)a[1].as.b;
        p2 = (bool)a[2].as.b;
        jui_input_key(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_jui_is_down(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jui_is_down");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_is_down.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_is_down.arg0");
        _r = jui_is_down(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_jui_is_pressed(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jui_is_pressed");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_is_pressed.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_is_pressed.arg0");
        _r = jui_is_pressed(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_jui_is_released(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jui_is_released");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_is_released.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_is_released.arg0");
        _r = jui_is_released(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_jui_take_wheel(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        float _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jui_take_wheel");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_take_wheel.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_take_wheel.arg0");
        _r = jui_take_wheel(p0);
        return joy_v_f64((float)_r);
}

static joy_value_t t_jui_is_shift(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jui_is_shift");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_is_shift.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_is_shift.arg0");
        _r = jui_is_shift(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_jui_is_ctrl(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jui_is_ctrl");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_is_ctrl.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_is_ctrl.arg0");
        _r = jui_is_ctrl(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_jui_pop_clip(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jui_pop_clip");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_pop_clip.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_pop_clip.arg0");
        jui_pop_clip(p0);
        return joy_v_null();
}

static joy_value_t t_jui_reset_clip(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jui_reset_clip");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_reset_clip.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_reset_clip.arg0");
        jui_reset_clip(p0);
        return joy_v_null();
}

static joy_value_t t_jui_panel_end(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jui_panel_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_panel_end.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_panel_end.arg0");
        jui_panel_end(p0);
        return joy_v_null();
}

static joy_value_t t_jui_scroll_end(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jui_scroll_end");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_scroll_end.arg0");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_scroll_end.arg0");
        jui_scroll_end(p0);
        return joy_v_null();
}

static joy_value_t t_jui_scroll_set_offset(joy_value_t* a, int n, joy_status_t* st)
{
        jui_ctx_p p0;
        int p1;
        float p2;
        float p3;
        float p4;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "jui_scroll_set_offset");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jui_scroll_set_offset.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jui_scroll_set_offset.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jui_scroll_set_offset.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jui_scroll_set_offset.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jui_scroll_set_offset.arg4");
        p0 = (jui_ctx_p)joy_obj_deref(a[0].as.handle, "jui_ctx_p", st);
        if (!p0) return joy_v_err(st, *st, "jui_scroll_set_offset.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        jui_scroll_set_offset(p0, p1, p2, p3, p4);
        return joy_v_null();
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

static joy_value_t t_kcpserver_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        kcpserver_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "kcpserver_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "kcpserver_destroy.arg0");
        p0 = (kcpserver_p)joy_obj_deref(a[0].as.handle, "kcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "kcpserver_destroy.arg0");
        _r = kcpserver_destroy(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_kcpserver_send(joy_value_t* a, int n, joy_status_t* st)
{
        kcpserver_p p0;
        int p1;
        const char* p2;
        int p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "kcpserver_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "kcpserver_send.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "kcpserver_send.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "kcpserver_send.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "kcpserver_send.arg3");
        p0 = (kcpserver_p)joy_obj_deref(a[0].as.handle, "kcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "kcpserver_send.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (const char*)a[2].as.str;
        p3 = (int)a[3].as.i32;
        kcpserver_send(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_kcpserver_broadcast(joy_value_t* a, int n, joy_status_t* st)
{
        kcpserver_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "kcpserver_broadcast");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "kcpserver_broadcast.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "kcpserver_broadcast.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "kcpserver_broadcast.arg2");
        p0 = (kcpserver_p)joy_obj_deref(a[0].as.handle, "kcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "kcpserver_broadcast.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        kcpserver_broadcast(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_kcpserver_offline(joy_value_t* a, int n, joy_status_t* st)
{
        kcpserver_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "kcpserver_offline");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "kcpserver_offline.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "kcpserver_offline.arg1");
        p0 = (kcpserver_p)joy_obj_deref(a[0].as.handle, "kcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "kcpserver_offline.arg0");
        p1 = (int)a[1].as.i32;
        kcpserver_offline(p0, p1);
        return joy_v_null();
}

static joy_value_t t_kcpserver_update(joy_value_t* a, int n, joy_status_t* st)
{
        kcpserver_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "kcpserver_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "kcpserver_update.arg0");
        p0 = (kcpserver_p)joy_obj_deref(a[0].as.handle, "kcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "kcpserver_update.arg0");
        kcpserver_update(p0);
        return joy_v_null();
}

static joy_value_t t_kcpserver_connection_count(joy_value_t* a, int n, joy_status_t* st)
{
        kcpserver_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "kcpserver_connection_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "kcpserver_connection_count.arg0");
        p0 = (kcpserver_p)joy_obj_deref(a[0].as.handle, "kcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "kcpserver_connection_count.arg0");
        _r = kcpserver_connection_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_kcpserver_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        kcpserver_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "kcpserver_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "kcpserver_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "kcpserver_poll_message.arg1");
        p0 = (kcpserver_p)joy_obj_deref(a[0].as.handle, "kcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "kcpserver_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "kcpserver_poll_message.arg1");
        _r = kcpserver_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_kcpclient_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        kcpclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "kcpclient_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "kcpclient_destroy.arg0");
        p0 = (kcpclient_p)joy_obj_deref(a[0].as.handle, "kcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "kcpclient_destroy.arg0");
        kcpclient_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_kcpclient_send(joy_value_t* a, int n, joy_status_t* st)
{
        kcpclient_p p0;
        const char* p1;
        int p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "kcpclient_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "kcpclient_send.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "kcpclient_send.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "kcpclient_send.arg2");
        p0 = (kcpclient_p)joy_obj_deref(a[0].as.handle, "kcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "kcpclient_send.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        _r = kcpclient_send(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_kcpclient_update(joy_value_t* a, int n, joy_status_t* st)
{
        kcpclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "kcpclient_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "kcpclient_update.arg0");
        p0 = (kcpclient_p)joy_obj_deref(a[0].as.handle, "kcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "kcpclient_update.arg0");
        kcpclient_update(p0);
        return joy_v_null();
}

static joy_value_t t_kcpclient_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        kcpclient_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "kcpclient_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "kcpclient_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "kcpclient_poll_message.arg1");
        p0 = (kcpclient_p)joy_obj_deref(a[0].as.handle, "kcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "kcpclient_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "kcpclient_poll_message.arg1");
        _r = kcpclient_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_tcpserver_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        tcpserver_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "tcpserver_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tcpserver_destroy.arg0");
        p0 = (tcpserver_p)joy_obj_deref(a[0].as.handle, "tcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "tcpserver_destroy.arg0");
        _r = tcpserver_destroy(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_tcpserver_send(joy_value_t* a, int n, joy_status_t* st)
{
        tcpserver_p p0;
        int p1;
        const char* p2;
        int p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "tcpserver_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tcpserver_send.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "tcpserver_send.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "tcpserver_send.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "tcpserver_send.arg3");
        p0 = (tcpserver_p)joy_obj_deref(a[0].as.handle, "tcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "tcpserver_send.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (const char*)a[2].as.str;
        p3 = (int)a[3].as.i32;
        tcpserver_send(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_tcpserver_broadcast(joy_value_t* a, int n, joy_status_t* st)
{
        tcpserver_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "tcpserver_broadcast");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tcpserver_broadcast.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "tcpserver_broadcast.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "tcpserver_broadcast.arg2");
        p0 = (tcpserver_p)joy_obj_deref(a[0].as.handle, "tcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "tcpserver_broadcast.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        tcpserver_broadcast(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_tcpserver_offline(joy_value_t* a, int n, joy_status_t* st)
{
        tcpserver_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "tcpserver_offline");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tcpserver_offline.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "tcpserver_offline.arg1");
        p0 = (tcpserver_p)joy_obj_deref(a[0].as.handle, "tcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "tcpserver_offline.arg0");
        p1 = (int)a[1].as.i32;
        tcpserver_offline(p0, p1);
        return joy_v_null();
}

static joy_value_t t_tcpserver_update(joy_value_t* a, int n, joy_status_t* st)
{
        tcpserver_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "tcpserver_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tcpserver_update.arg0");
        p0 = (tcpserver_p)joy_obj_deref(a[0].as.handle, "tcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "tcpserver_update.arg0");
        tcpserver_update(p0);
        return joy_v_null();
}

static joy_value_t t_tcpserver_connection_count(joy_value_t* a, int n, joy_status_t* st)
{
        tcpserver_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "tcpserver_connection_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tcpserver_connection_count.arg0");
        p0 = (tcpserver_p)joy_obj_deref(a[0].as.handle, "tcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "tcpserver_connection_count.arg0");
        _r = tcpserver_connection_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_tcpserver_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        tcpserver_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "tcpserver_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tcpserver_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tcpserver_poll_message.arg1");
        p0 = (tcpserver_p)joy_obj_deref(a[0].as.handle, "tcpserver_p", st);
        if (!p0) return joy_v_err(st, *st, "tcpserver_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "tcpserver_poll_message.arg1");
        _r = tcpserver_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_tcpclient_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        tcpclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "tcpclient_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tcpclient_destroy.arg0");
        p0 = (tcpclient_p)joy_obj_deref(a[0].as.handle, "tcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "tcpclient_destroy.arg0");
        tcpclient_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_tcpclient_send(joy_value_t* a, int n, joy_status_t* st)
{
        tcpclient_p p0;
        const char* p1;
        int p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "tcpclient_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tcpclient_send.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "tcpclient_send.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "tcpclient_send.arg2");
        p0 = (tcpclient_p)joy_obj_deref(a[0].as.handle, "tcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "tcpclient_send.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        _r = tcpclient_send(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_tcpclient_update(joy_value_t* a, int n, joy_status_t* st)
{
        tcpclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "tcpclient_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tcpclient_update.arg0");
        p0 = (tcpclient_p)joy_obj_deref(a[0].as.handle, "tcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "tcpclient_update.arg0");
        tcpclient_update(p0);
        return joy_v_null();
}

static joy_value_t t_tcpclient_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        tcpclient_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "tcpclient_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tcpclient_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "tcpclient_poll_message.arg1");
        p0 = (tcpclient_p)joy_obj_deref(a[0].as.handle, "tcpclient_p", st);
        if (!p0) return joy_v_err(st, *st, "tcpclient_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "tcpclient_poll_message.arg1");
        _r = tcpclient_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_wsnetserver_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetserver_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "wsnetserver_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "wsnetserver_destroy.arg0");
        p0 = (wsnetserver_p)joy_obj_deref(a[0].as.handle, "wsnetserver_p", st);
        if (!p0) return joy_v_err(st, *st, "wsnetserver_destroy.arg0");
        _r = wsnetserver_destroy(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_wsnetserver_send(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetserver_p p0;
        int p1;
        const char* p2;
        int p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "wsnetserver_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "wsnetserver_send.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "wsnetserver_send.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "wsnetserver_send.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "wsnetserver_send.arg3");
        p0 = (wsnetserver_p)joy_obj_deref(a[0].as.handle, "wsnetserver_p", st);
        if (!p0) return joy_v_err(st, *st, "wsnetserver_send.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (const char*)a[2].as.str;
        p3 = (int)a[3].as.i32;
        wsnetserver_send(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_wsnetserver_broadcast(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetserver_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "wsnetserver_broadcast");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "wsnetserver_broadcast.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "wsnetserver_broadcast.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "wsnetserver_broadcast.arg2");
        p0 = (wsnetserver_p)joy_obj_deref(a[0].as.handle, "wsnetserver_p", st);
        if (!p0) return joy_v_err(st, *st, "wsnetserver_broadcast.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        wsnetserver_broadcast(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_wsnetserver_offline(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetserver_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "wsnetserver_offline");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "wsnetserver_offline.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "wsnetserver_offline.arg1");
        p0 = (wsnetserver_p)joy_obj_deref(a[0].as.handle, "wsnetserver_p", st);
        if (!p0) return joy_v_err(st, *st, "wsnetserver_offline.arg0");
        p1 = (int)a[1].as.i32;
        wsnetserver_offline(p0, p1);
        return joy_v_null();
}

static joy_value_t t_wsnetserver_update(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetserver_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "wsnetserver_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "wsnetserver_update.arg0");
        p0 = (wsnetserver_p)joy_obj_deref(a[0].as.handle, "wsnetserver_p", st);
        if (!p0) return joy_v_err(st, *st, "wsnetserver_update.arg0");
        wsnetserver_update(p0);
        return joy_v_null();
}

static joy_value_t t_wsnetserver_connection_count(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetserver_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "wsnetserver_connection_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "wsnetserver_connection_count.arg0");
        p0 = (wsnetserver_p)joy_obj_deref(a[0].as.handle, "wsnetserver_p", st);
        if (!p0) return joy_v_err(st, *st, "wsnetserver_connection_count.arg0");
        _r = wsnetserver_connection_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_wsnetserver_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetserver_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "wsnetserver_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "wsnetserver_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "wsnetserver_poll_message.arg1");
        p0 = (wsnetserver_p)joy_obj_deref(a[0].as.handle, "wsnetserver_p", st);
        if (!p0) return joy_v_err(st, *st, "wsnetserver_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "wsnetserver_poll_message.arg1");
        _r = wsnetserver_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_wsnetclient_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "wsnetclient_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "wsnetclient_destroy.arg0");
        p0 = (wsnetclient_p)joy_obj_deref(a[0].as.handle, "wsnetclient_p", st);
        if (!p0) return joy_v_err(st, *st, "wsnetclient_destroy.arg0");
        wsnetclient_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_wsnetclient_send(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetclient_p p0;
        const char* p1;
        int p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "wsnetclient_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "wsnetclient_send.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "wsnetclient_send.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "wsnetclient_send.arg2");
        p0 = (wsnetclient_p)joy_obj_deref(a[0].as.handle, "wsnetclient_p", st);
        if (!p0) return joy_v_err(st, *st, "wsnetclient_send.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        _r = wsnetclient_send(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_wsnetclient_update(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "wsnetclient_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "wsnetclient_update.arg0");
        p0 = (wsnetclient_p)joy_obj_deref(a[0].as.handle, "wsnetclient_p", st);
        if (!p0) return joy_v_err(st, *st, "wsnetclient_update.arg0");
        wsnetclient_update(p0);
        return joy_v_null();
}

static joy_value_t t_wsnetclient_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        wsnetclient_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "wsnetclient_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "wsnetclient_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "wsnetclient_poll_message.arg1");
        p0 = (wsnetclient_p)joy_obj_deref(a[0].as.handle, "wsnetclient_p", st);
        if (!p0) return joy_v_err(st, *st, "wsnetclient_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "wsnetclient_poll_message.arg1");
        _r = wsnetclient_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_netclient_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        netclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "netclient_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "netclient_destroy.arg0");
        p0 = (netclient_p)joy_obj_deref(a[0].as.handle, "netclient_p", st);
        if (!p0) return joy_v_err(st, *st, "netclient_destroy.arg0");
        netclient_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_netclient_send(joy_value_t* a, int n, joy_status_t* st)
{
        netclient_p p0;
        const char* p1;
        int p2;
        int _r;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "netclient_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "netclient_send.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "netclient_send.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "netclient_send.arg2");
        p0 = (netclient_p)joy_obj_deref(a[0].as.handle, "netclient_p", st);
        if (!p0) return joy_v_err(st, *st, "netclient_send.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        _r = netclient_send(p0, p1, p2);
        return joy_v_i32((int)_r);
}

static joy_value_t t_netclient_update(joy_value_t* a, int n, joy_status_t* st)
{
        netclient_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "netclient_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "netclient_update.arg0");
        p0 = (netclient_p)joy_obj_deref(a[0].as.handle, "netclient_p", st);
        if (!p0) return joy_v_err(st, *st, "netclient_update.arg0");
        netclient_update(p0);
        return joy_v_null();
}

static joy_value_t t_netclient_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        netclient_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "netclient_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "netclient_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "netclient_poll_message.arg1");
        p0 = (netclient_p)joy_obj_deref(a[0].as.handle, "netclient_p", st);
        if (!p0) return joy_v_err(st, *st, "netclient_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "netclient_poll_message.arg1");
        _r = netclient_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_netserver_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        netserver_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "netserver_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "netserver_destroy.arg0");
        p0 = (netserver_p)joy_obj_deref(a[0].as.handle, "netserver_p", st);
        if (!p0) return joy_v_err(st, *st, "netserver_destroy.arg0");
        netserver_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_netserver_send(joy_value_t* a, int n, joy_status_t* st)
{
        netserver_p p0;
        int p1;
        const char* p2;
        int p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "netserver_send");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "netserver_send.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "netserver_send.arg1");
        if (a[2].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "netserver_send.arg2");
        if (a[3].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "netserver_send.arg3");
        p0 = (netserver_p)joy_obj_deref(a[0].as.handle, "netserver_p", st);
        if (!p0) return joy_v_err(st, *st, "netserver_send.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (const char*)a[2].as.str;
        p3 = (int)a[3].as.i32;
        netserver_send(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_netserver_broadcast(joy_value_t* a, int n, joy_status_t* st)
{
        netserver_p p0;
        const char* p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "netserver_broadcast");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "netserver_broadcast.arg0");
        if (a[1].kind != JOY_V_STR) return joy_v_err(st, JOY_ERR_TYPE, "netserver_broadcast.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "netserver_broadcast.arg2");
        p0 = (netserver_p)joy_obj_deref(a[0].as.handle, "netserver_p", st);
        if (!p0) return joy_v_err(st, *st, "netserver_broadcast.arg0");
        p1 = (const char*)a[1].as.str;
        p2 = (int)a[2].as.i32;
        netserver_broadcast(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_netserver_offline(joy_value_t* a, int n, joy_status_t* st)
{
        netserver_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "netserver_offline");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "netserver_offline.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "netserver_offline.arg1");
        p0 = (netserver_p)joy_obj_deref(a[0].as.handle, "netserver_p", st);
        if (!p0) return joy_v_err(st, *st, "netserver_offline.arg0");
        p1 = (int)a[1].as.i32;
        netserver_offline(p0, p1);
        return joy_v_null();
}

static joy_value_t t_netserver_update(joy_value_t* a, int n, joy_status_t* st)
{
        netserver_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "netserver_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "netserver_update.arg0");
        p0 = (netserver_p)joy_obj_deref(a[0].as.handle, "netserver_p", st);
        if (!p0) return joy_v_err(st, *st, "netserver_update.arg0");
        netserver_update(p0);
        return joy_v_null();
}

static joy_value_t t_netserver_connection_count(joy_value_t* a, int n, joy_status_t* st)
{
        netserver_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "netserver_connection_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "netserver_connection_count.arg0");
        p0 = (netserver_p)joy_obj_deref(a[0].as.handle, "netserver_p", st);
        if (!p0) return joy_v_err(st, *st, "netserver_connection_count.arg0");
        _r = netserver_connection_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_netserver_poll_message(joy_value_t* a, int n, joy_status_t* st)
{
        netserver_p p0;
        net_message_p p1;
        bool _r;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "netserver_poll_message");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "netserver_poll_message.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "netserver_poll_message.arg1");
        p0 = (netserver_p)joy_obj_deref(a[0].as.handle, "netserver_p", st);
        if (!p0) return joy_v_err(st, *st, "netserver_poll_message.arg0");
        p1 = (net_message_p)joy_obj_deref(a[1].as.handle, "net_message_p", st);
        if (!p1) return joy_v_err(st, *st, "netserver_poll_message.arg1");
        _r = netserver_poll_message(p0, p1);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_netmessage_free_data(joy_value_t* a, int n, joy_status_t* st)
{
        net_message_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "netmessage_free_data");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "netmessage_free_data.arg0");
        p0 = (net_message_p)joy_obj_deref(a[0].as.handle, "net_message_p", st);
        if (!p0) return joy_v_err(st, *st, "netmessage_free_data.arg0");
        netmessage_free_data(p0);
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

static joy_value_t t_jparticle_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jparticle_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jparticle_destroy.arg0");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "jparticle_destroy.arg0");
        jparticle_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_jparticle_set_position(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        float p1;
        float p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jparticle_set_position");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jparticle_set_position.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jparticle_set_position.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jparticle_set_position.arg2");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "jparticle_set_position.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        jparticle_set_position(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_jparticle_emit(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jparticle_emit");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jparticle_emit.arg0");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "jparticle_emit.arg0");
        jparticle_emit(p0);
        return joy_v_null();
}

static joy_value_t t_jparticle_clear(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jparticle_clear");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jparticle_clear.arg0");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "jparticle_clear.arg0");
        jparticle_clear(p0);
        return joy_v_null();
}

static joy_value_t t_jparticle_update(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        float p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jparticle_update");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jparticle_update.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jparticle_update.arg1");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "jparticle_update.arg0");
        p1 = (float)a[1].as.f64;
        jparticle_update(p0, p1);
        return joy_v_null();
}

static joy_value_t t_jparticle_render(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        jr2d_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jparticle_render");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jparticle_render.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jparticle_render.arg1");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "jparticle_render.arg0");
        p1 = (jr2d_p)joy_obj_deref(a[1].as.handle, "jr2d_p", st);
        if (!p1) return joy_v_err(st, *st, "jparticle_render.arg1");
        jparticle_render(p0, p1);
        return joy_v_null();
}

static joy_value_t t_jparticle_count(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        int _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jparticle_count");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jparticle_count.arg0");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "jparticle_count.arg0");
        _r = jparticle_count(p0);
        return joy_v_i32((int)_r);
}

static joy_value_t t_jparticle_is_active(joy_value_t* a, int n, joy_status_t* st)
{
        jparticle_p p0;
        bool _r;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jparticle_is_active");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jparticle_is_active.arg0");
        p0 = (jparticle_p)joy_obj_deref(a[0].as.handle, "jparticle_p", st);
        if (!p0) return joy_v_err(st, *st, "jparticle_is_active.arg0");
        _r = jparticle_is_active(p0);
        return joy_v_bool((bool)_r);
}

static joy_value_t t_jlight_sys_destroy(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jlight_sys_destroy");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jlight_sys_destroy.arg0");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "jlight_sys_destroy.arg0");
        jlight_sys_destroy(p0);
        return joy_v_null();
}

static joy_value_t t_jlight_sys_resize(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        int p1;
        int p2;
        if (n != 3) return joy_v_err(st, JOY_ERR_ARGC, "jlight_sys_resize");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jlight_sys_resize.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jlight_sys_resize.arg1");
        if (a[2].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jlight_sys_resize.arg2");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "jlight_sys_resize.arg0");
        p1 = (int)a[1].as.i32;
        p2 = (int)a[2].as.i32;
        jlight_sys_resize(p0, p1, p2);
        return joy_v_null();
}

static joy_value_t t_jlight_remove(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        int p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jlight_remove");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jlight_remove.arg0");
        if (a[1].kind != JOY_V_I32) return joy_v_err(st, JOY_ERR_TYPE, "jlight_remove.arg1");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "jlight_remove.arg0");
        p1 = (int)a[1].as.i32;
        jlight_remove(p0, p1);
        return joy_v_null();
}

static joy_value_t t_jlight_clear(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jlight_clear");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jlight_clear.arg0");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "jlight_clear.arg0");
        jlight_clear(p0);
        return joy_v_null();
}

static joy_value_t t_jlight_add_occluder(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        float p1;
        float p2;
        float p3;
        float p4;
        int _r;
        if (n != 5) return joy_v_err(st, JOY_ERR_ARGC, "jlight_add_occluder");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jlight_add_occluder.arg0");
        if (a[1].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jlight_add_occluder.arg1");
        if (a[2].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jlight_add_occluder.arg2");
        if (a[3].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jlight_add_occluder.arg3");
        if (a[4].kind != JOY_V_F64) return joy_v_err(st, JOY_ERR_TYPE, "jlight_add_occluder.arg4");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "jlight_add_occluder.arg0");
        p1 = (float)a[1].as.f64;
        p2 = (float)a[2].as.f64;
        p3 = (float)a[3].as.f64;
        p4 = (float)a[4].as.f64;
        _r = jlight_add_occluder(p0, p1, p2, p3, p4);
        return joy_v_i32((int)_r);
}

static joy_value_t t_jlight_clear_occluders(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        if (n != 1) return joy_v_err(st, JOY_ERR_ARGC, "jlight_clear_occluders");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jlight_clear_occluders.arg0");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "jlight_clear_occluders.arg0");
        jlight_clear_occluders(p0);
        return joy_v_null();
}

static joy_value_t t_jlight_set_ambient(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        uint8_t p1;
        uint8_t p2;
        uint8_t p3;
        if (n != 4) return joy_v_err(st, JOY_ERR_ARGC, "jlight_set_ambient");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jlight_set_ambient.arg0");
        if (a[1].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "jlight_set_ambient.arg1");
        if (a[2].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "jlight_set_ambient.arg2");
        if (a[3].kind != JOY_V_U32) return joy_v_err(st, JOY_ERR_TYPE, "jlight_set_ambient.arg3");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "jlight_set_ambient.arg0");
        p1 = (uint8_t)a[1].as.u32;
        p2 = (uint8_t)a[2].as.u32;
        p3 = (uint8_t)a[3].as.u32;
        jlight_set_ambient(p0, p1, p2, p3);
        return joy_v_null();
}

static joy_value_t t_jlight_render(joy_value_t* a, int n, joy_status_t* st)
{
        jlight_sys_p p0;
        jr2d_p p1;
        if (n != 2) return joy_v_err(st, JOY_ERR_ARGC, "jlight_render");
        if (a[0].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jlight_render.arg0");
        if (a[1].kind != JOY_V_HANDLE) return joy_v_err(st, JOY_ERR_TYPE, "jlight_render.arg1");
        p0 = (jlight_sys_p)joy_obj_deref(a[0].as.handle, "jlight_sys_p", st);
        if (!p0) return joy_v_err(st, *st, "jlight_render.arg0");
        p1 = (jr2d_p)joy_obj_deref(a[1].as.handle, "jr2d_p", st);
        if (!p1) return joy_v_err(st, *st, "jlight_render.arg1");
        jlight_render(p0, p1);
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

static const joy_value_kind k_lcg_random_create[] = { JOY_V_U64 };
static const joy_value_kind k_lcg_random_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_lcg_random_next[] = { JOY_V_HANDLE };
static const joy_value_kind k_lcg_random_set_state[] = { JOY_V_HANDLE, JOY_V_U64 };
static const joy_value_kind k_jpm_config_add_spawn[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_jpm_wave_seed[] = { JOY_V_U32, JOY_V_I32 };
static const joy_value_kind k_sdbm_hash[] = { JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_fnv1a_hash[] = { JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_jenkins_hash[] = { JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_murmur3_hash[] = { JOY_V_STR };
static const joy_value_kind k_kr_hash[] = { JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_jtimer_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_jtimer_trigger[] = { JOY_V_HANDLE };
static const joy_value_kind k_jtimer_set_interval[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_jtimer_set_active[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_jtimer_reset[] = { JOY_V_HANDLE };
static const joy_value_kind k_jtimer_get_elapsed[] = { JOY_V_HANDLE };
static const joy_value_kind k_destroy_window[] = { JOY_V_HANDLE };
static const joy_value_kind k_window_set_fullscreen[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_window_get_fullscreen[] = { JOY_V_HANDLE };
static const joy_value_kind k_window_set_icon[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_push_user_event[] = { JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_log_title[] = { JOY_V_I32 };
static const joy_value_kind k_env_set[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_STR };
static const joy_value_kind k_env_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_simple_fps_destory[] = { JOY_V_HANDLE };
static const joy_value_kind k_simple_fps_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_simple_fps_value[] = { JOY_V_HANDLE };
static const joy_value_kind k_game_timer_init[] = { JOY_V_HANDLE };
static const joy_value_kind k_game_timer_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_game_timer_get_delta_time[] = { JOY_V_HANDLE };
static const joy_value_kind k_game_timer_get_unscaled_delta_time[] = { JOY_V_HANDLE };
static const joy_value_kind k_game_timer_set_time_scale[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_game_timer_get_time_scale[] = { JOY_V_HANDLE };
static const joy_value_kind k_game_timer_set_target_fps[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_game_timer_get_target_fps[] = { JOY_V_HANDLE };
static const joy_value_kind k_set_clipboard_text[] = { JOY_V_STR };
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
static const joy_value_kind k_iocp_server_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_iocp_process_events[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_font_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_font_get_scale[] = { JOY_V_HANDLE };
static const joy_value_kind k_font_get_size[] = { JOY_V_HANDLE };
static const joy_value_kind k_font_get_ascent[] = { JOY_V_HANDLE };
static const joy_value_kind k_r2d_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_r2d_set_color[] = { JOY_V_HANDLE, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_r2d_begin[] = { JOY_V_HANDLE };
static const joy_value_kind k_r2d_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_r2d_set_logical_presentation[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_r2d_set_logical_presentation_mode[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_r2d_set_vsync[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_r2d_set_scale[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r2d_set_colorf[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r2d_set_blend_mode[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_r2d_draw_point[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r2d_clear_clip[] = { JOY_V_HANDLE };
static const joy_value_kind k_r2d_pop_clip_rect[] = { JOY_V_HANDLE };
static const joy_value_kind k_r2d_reset_clip_stack[] = { JOY_V_HANDLE };
static const joy_value_kind k_r2d_point_in_clip[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_shape_draw_line[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_image_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_image_set_alpha[] = { JOY_V_HANDLE, JOY_V_U32 };
static const joy_value_kind k_image_set_blend_mode[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_image_set_scale_mode[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_text_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_jtext_print[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r2d_draw_point_f[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_shape_draw_rectangle_f[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_shape_draw_line_f[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_text_get_width[] = { JOY_V_HANDLE };
static const joy_value_kind k_text_get_height[] = { JOY_V_HANDLE };
static const joy_value_kind k_jtext_cache_set_limit[] = { JOY_V_I32 };
static const joy_value_kind k_jtext_cache_sweep[] = { JOY_V_I32 };
static const joy_value_kind k_animation_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_animation_reset[] = { JOY_V_HANDLE };
static const joy_value_kind k_animation_is_finished[] = { JOY_V_HANDLE };
static const joy_value_kind k_animation_set_position[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_animation_set_scale[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_animation_set_rotation[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_animation_update[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_r3d_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_begin[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_get_draw_calls[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_set_clear_color[] = { JOY_V_HANDLE, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_r3d_mesh_destroy[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_r3d_font_init[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_r3d_font_draw_2d[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_STR, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_font_cache_set_limit[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_r3d_font_cache_get_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_font_cache_clear[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_font_cache_sweep[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_r3d_set_logical_presentation[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_r3d_set_logical_presentation_mode[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_r3d_button_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_button_draw_circle[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_button_is_hovered[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_button_is_pressed[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_BOOL, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_button_release[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_button_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_label_set_text[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_r3d_label_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_label_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_input_set_text[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_r3d_input_is_hovered[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_input_focus[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_input_defocus[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_input_is_focused[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_input_start_text_input[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_r3d_input_stop_text_input[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_r3d_input_insert[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_r3d_input_backspace[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_input_delete[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_input_cursor_left[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_input_cursor_right[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_input_cursor_home[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_input_cursor_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_input_click[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_input_submit[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_input_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_input_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_combobox_input_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_combobox_input_add_item[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_r3d_combobox_input_clear_items[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_combobox_input_set_text[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_r3d_combobox_input_hit_item[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_combobox_input_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_combobox_input_draw_dropdown[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_slider_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_slider_set_value[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_r3d_slider_get_value[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_slider_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_slider_is_pressed[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_slider_set_pressed[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_r3d_slider_drag_move[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_slider_is_hovered[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_slider_set_hovered[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_r3d_checkbox_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_checkbox_toggle[] = { JOY_V_HANDLE };
static const joy_value_kind k_r3d_checkbox_is_pressed[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_checkbox_set_hovered[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_r3d_checkbox_is_hovered[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_checkbox_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_r3d_draw_grid[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_F64, JOY_V_F64 };
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
static const joy_value_kind k_janimator_state_add_color[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_F64, JOY_V_I32, JOY_V_I32, JOY_V_U32, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_janimator_state_set_loop[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_BOOL };
static const joy_value_kind k_janimator_state_set_speed[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_F64 };
static const joy_value_kind k_janimator_state_set_blend[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_STR };
static const joy_value_kind k_janimator_add_transition[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_STR };
static const joy_value_kind k_janimator_add_any_transition[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_janimator_transition_add_cond[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_STR, JOY_V_I32, JOY_V_F64 };
static const joy_value_kind k_janimator_transition_set_exit_time[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_F64 };
static const joy_value_kind k_janimator_update[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_janimator_draw[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_janimator_set_flip_h[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_label_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_label_draw[] = { JOY_V_HANDLE };
static const joy_value_kind k_button_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_button_set_position[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_button_set_size[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_button_set_image[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_button_set_pressed_image[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_button_is_hovered[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_button_is_pressed[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_button_set_hovered[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_button_set_pressed[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_button_get_pressed[] = { JOY_V_HANDLE };
static const joy_value_kind k_button_set_text_offset[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_button_draw[] = { JOY_V_HANDLE };
static const joy_value_kind k_scrollview_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_scrollview_set_view_px[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_scrollview_set_content[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_scrollview_set_offset[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_scrollview_clamp[] = { JOY_V_HANDLE };
static const joy_value_kind k_scrollview_get_offset_x[] = { JOY_V_HANDLE };
static const joy_value_kind k_scrollview_get_offset_y[] = { JOY_V_HANDLE };
static const joy_value_kind k_scrollview_set_show_bar[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_scrollview_dragging[] = { JOY_V_HANDLE };
static const joy_value_kind k_scrollview_hit[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_scrollview_begin[] = { JOY_V_HANDLE };
static const joy_value_kind k_scrollview_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_combobox_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_combobox_is_pressed[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_combobox_is_open[] = { JOY_V_HANDLE };
static const joy_value_kind k_combobox_set_open[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_combobox_hit_item[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_combobox_set_selected[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_combobox_get_selected[] = { JOY_V_HANDLE };
static const joy_value_kind k_combobox_draw[] = { JOY_V_HANDLE };
static const joy_value_kind k_combobox_input_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_combobox_input_add_item[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_combobox_input_clear_items[] = { JOY_V_HANDLE };
static const joy_value_kind k_combobox_input_set_text[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_combobox_input_draw[] = { JOY_V_HANDLE };
static const joy_value_kind k_combobox_input_draw_dropdown[] = { JOY_V_HANDLE };
static const joy_value_kind k_combobox_input_is_open[] = { JOY_V_HANDLE };
static const joy_value_kind k_combobox_input_set_popup_node[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_combobox_input_hit_item[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_combobox_input_set_open[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_datagrid_setheaders[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_datagrid_setheader[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_STR };
static const joy_value_kind k_datagrid_setcell[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32, JOY_V_STR };
static const joy_value_kind k_datagrid_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_datagrid_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_datagrid_hit_row[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_datagrid_scroll[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_datagrid_set_selected_row[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_radio_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_radio_is_pressed[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_radio_set_hovered[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_checkbox_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_checkbox_set_r2d[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_checkbox_set_position[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_checkbox_toggle[] = { JOY_V_HANDLE };
static const joy_value_kind k_checkbox_is_pressed[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_checkbox_set_hovered[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_joystick_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_joystick_set_textures[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_STR };
static const joy_value_kind k_joystick_set_vector[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joystick_draw[] = { JOY_V_HANDLE };
static const joy_value_kind k_joystick_reset[] = { JOY_V_HANDLE };
static const joy_value_kind k_joystick_set_position[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joystick_set_radius[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_joystick_is_dragging[] = { JOY_V_HANDLE };
static const joy_value_kind k_joystick_touch_down[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joystick_touch_move[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_joystick_touch_up[] = { JOY_V_HANDLE };
static const joy_value_kind k_joystick_get_magnitude[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_set_text[] = { JOY_V_HANDLE, JOY_V_STR };
static const joy_value_kind k_inputbox_draw[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_is_pressed[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_inputbox_is_hovered[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_inputbox_focus[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_defocus[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_blur[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_insert[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_inputbox_backspace[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_delete[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_cursor_left[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_cursor_right[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_cursor_home[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_cursor_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_select_left[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_select_right[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_select_home[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_select_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_select_all[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_paste[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_copy[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_cut[] = { JOY_V_HANDLE };
static const joy_value_kind k_inputbox_is_focused[] = { JOY_V_HANDLE };
static const joy_value_kind k_slider_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_slider_set_value[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_slider_get_value[] = { JOY_V_HANDLE };
static const joy_value_kind k_slider_set_rect[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_slider_set_range[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_slider_draw[] = { JOY_V_HANDLE };
static const joy_value_kind k_slider_is_pressed[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_slider_set_pressed[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_slider_drag_move[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_slider_is_hovered[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_slider_set_hovered[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_popup_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_popup_set_title[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_popup_set_content[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_popup_set_position[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_popup_set_visible[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_popup_set_closeable[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_popup_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_popup_is_close_clicked[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_popup_is_title_clicked[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_popup_drag_move[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_popup_is_inside[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_tooltip_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_tooltip_set_text[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_tooltip_set_position[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_tooltip_set_visible[] = { JOY_V_HANDLE, JOY_V_BOOL };
static const joy_value_kind k_tooltip_draw[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_jui_ctx_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_jui_begin[] = { JOY_V_HANDLE };
static const joy_value_kind k_jui_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_jui_input_motion[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jui_input_down[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_jui_input_up[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_jui_input_wheel[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_jui_input_key[] = { JOY_V_HANDLE, JOY_V_BOOL, JOY_V_BOOL };
static const joy_value_kind k_jui_is_down[] = { JOY_V_HANDLE };
static const joy_value_kind k_jui_is_pressed[] = { JOY_V_HANDLE };
static const joy_value_kind k_jui_is_released[] = { JOY_V_HANDLE };
static const joy_value_kind k_jui_take_wheel[] = { JOY_V_HANDLE };
static const joy_value_kind k_jui_is_shift[] = { JOY_V_HANDLE };
static const joy_value_kind k_jui_is_ctrl[] = { JOY_V_HANDLE };
static const joy_value_kind k_jui_pop_clip[] = { JOY_V_HANDLE };
static const joy_value_kind k_jui_reset_clip[] = { JOY_V_HANDLE };
static const joy_value_kind k_jui_panel_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_jui_scroll_end[] = { JOY_V_HANDLE };
static const joy_value_kind k_jui_scroll_set_offset[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
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
static const joy_value_kind k_kcpserver_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_kcpserver_send[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_kcpserver_broadcast[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_kcpserver_offline[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_kcpserver_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_kcpserver_connection_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_kcpserver_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_kcpclient_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_kcpclient_send[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_kcpclient_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_kcpclient_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_tcpserver_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_tcpserver_send[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_tcpserver_broadcast[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_tcpserver_offline[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_tcpserver_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_tcpserver_connection_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_tcpserver_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_tcpclient_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_tcpclient_send[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_tcpclient_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_tcpclient_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_wsnetserver_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_wsnetserver_send[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_wsnetserver_broadcast[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_wsnetserver_offline[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_wsnetserver_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_wsnetserver_connection_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_wsnetserver_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_wsnetclient_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_wsnetclient_send[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_wsnetclient_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_wsnetclient_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_netclient_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_netclient_send[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_netclient_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_netclient_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_netserver_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_netserver_send[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_netserver_broadcast[] = { JOY_V_HANDLE, JOY_V_STR, JOY_V_I32 };
static const joy_value_kind k_netserver_offline[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_netserver_update[] = { JOY_V_HANDLE };
static const joy_value_kind k_netserver_connection_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_netserver_poll_message[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_netmessage_free_data[] = { JOY_V_HANDLE };
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
static const joy_value_kind k_jparticle_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_jparticle_set_position[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jparticle_emit[] = { JOY_V_HANDLE };
static const joy_value_kind k_jparticle_clear[] = { JOY_V_HANDLE };
static const joy_value_kind k_jparticle_update[] = { JOY_V_HANDLE, JOY_V_F64 };
static const joy_value_kind k_jparticle_render[] = { JOY_V_HANDLE, JOY_V_HANDLE };
static const joy_value_kind k_jparticle_count[] = { JOY_V_HANDLE };
static const joy_value_kind k_jparticle_is_active[] = { JOY_V_HANDLE };
static const joy_value_kind k_jlight_sys_destroy[] = { JOY_V_HANDLE };
static const joy_value_kind k_jlight_sys_resize[] = { JOY_V_HANDLE, JOY_V_I32, JOY_V_I32 };
static const joy_value_kind k_jlight_remove[] = { JOY_V_HANDLE, JOY_V_I32 };
static const joy_value_kind k_jlight_clear[] = { JOY_V_HANDLE };
static const joy_value_kind k_jlight_add_occluder[] = { JOY_V_HANDLE, JOY_V_F64, JOY_V_F64, JOY_V_F64, JOY_V_F64 };
static const joy_value_kind k_jlight_clear_occluders[] = { JOY_V_HANDLE };
static const joy_value_kind k_jlight_set_ambient[] = { JOY_V_HANDLE, JOY_V_U32, JOY_V_U32, JOY_V_U32 };
static const joy_value_kind k_jlight_render[] = { JOY_V_HANDLE, JOY_V_HANDLE };
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

static const joy_thunk_entry s_thunks[] = {
        { "lcg", "lcg_random_create", t_lcg_random_create, 1, k_lcg_random_create, JOY_V_HANDLE },
        { "lcg", "lcg_random_destroy", t_lcg_random_destroy, 1, k_lcg_random_destroy, JOY_V_NULL },
        { "lcg", "lcg_random_next", t_lcg_random_next, 1, k_lcg_random_next, JOY_V_U32 },
        { "lcg", "lcg_random_set_state", t_lcg_random_set_state, 2, k_lcg_random_set_state, JOY_V_NULL },
        { "jpm", "jpm_config_add_spawn", t_jpm_config_add_spawn, 4, k_jpm_config_add_spawn, JOY_V_NULL },
        { "jpm", "jpm_wave_seed", t_jpm_wave_seed, 2, k_jpm_wave_seed, JOY_V_U32 },
        { "sdbm", "sdbm_hash", t_sdbm_hash, 2, k_sdbm_hash, JOY_V_U64 },
        { "fnv1a", "fnv1a_hash", t_fnv1a_hash, 2, k_fnv1a_hash, JOY_V_U64 },
        { "jenkins", "jenkins_hash", t_jenkins_hash, 2, k_jenkins_hash, JOY_V_U64 },
        { "murmur3", "murmur3_hash", t_murmur3_hash, 1, k_murmur3_hash, JOY_V_U64 },
        { "kr", "kr_hash", t_kr_hash, 2, k_kr_hash, JOY_V_U64 },
        { "jtimer", "jtimer_destroy", t_jtimer_destroy, 1, k_jtimer_destroy, JOY_V_NULL },
        { "jtimer", "jtimer_trigger", t_jtimer_trigger, 1, k_jtimer_trigger, JOY_V_NULL },
        { "jtimer", "jtimer_set_interval", t_jtimer_set_interval, 2, k_jtimer_set_interval, JOY_V_NULL },
        { "jtimer", "jtimer_set_active", t_jtimer_set_active, 2, k_jtimer_set_active, JOY_V_NULL },
        { "jtimer", "jtimer_reset", t_jtimer_reset, 1, k_jtimer_reset, JOY_V_NULL },
        { "jtimer", "jtimer_get_elapsed", t_jtimer_get_elapsed, 1, k_jtimer_get_elapsed, JOY_V_U64 },
        { "env", "init_env", t_init_env, 0, NULL, JOY_V_BOOL },
        { "quit", "quit_env", t_quit_env, 0, NULL, JOY_V_NULL },
        { "ticks", "get_ticks", t_get_ticks, 0, NULL, JOY_V_U64 },
        { "window", "destroy_window", t_destroy_window, 1, k_destroy_window, JOY_V_NULL },
        { "window", "window_set_fullscreen", t_window_set_fullscreen, 2, k_window_set_fullscreen, JOY_V_NULL },
        { "window", "window_get_fullscreen", t_window_get_fullscreen, 1, k_window_get_fullscreen, JOY_V_BOOL },
        { "window", "window_set_icon", t_window_set_icon, 2, k_window_set_icon, JOY_V_NULL },
        { "push", "push_user_event", t_push_user_event, 2, k_push_user_event, JOY_V_BOOL },
        { "push", "push_quit_event", t_push_quit_event, 0, NULL, JOY_V_BOOL },
        { "log", "log_title", t_log_title, 1, k_log_title, JOY_V_NULL },
        { "env", "env_set", t_env_set, 3, k_env_set, JOY_V_NULL },
        { "env", "env_destroy", t_env_destroy, 1, k_env_destroy, JOY_V_NULL },
        { "simple", "simple_fps_destory", t_simple_fps_destory, 1, k_simple_fps_destory, JOY_V_NULL },
        { "simple", "simple_fps_update", t_simple_fps_update, 1, k_simple_fps_update, JOY_V_NULL },
        { "simple", "simple_fps_value", t_simple_fps_value, 1, k_simple_fps_value, JOY_V_I32 },
        { "game", "game_timer_init", t_game_timer_init, 1, k_game_timer_init, JOY_V_NULL },
        { "game", "game_timer_update", t_game_timer_update, 1, k_game_timer_update, JOY_V_NULL },
        { "game", "game_timer_get_delta_time", t_game_timer_get_delta_time, 1, k_game_timer_get_delta_time, JOY_V_F64 },
        { "game", "game_timer_get_unscaled_delta_time", t_game_timer_get_unscaled_delta_time, 1, k_game_timer_get_unscaled_delta_time, JOY_V_F64 },
        { "game", "game_timer_set_time_scale", t_game_timer_set_time_scale, 2, k_game_timer_set_time_scale, JOY_V_NULL },
        { "game", "game_timer_get_time_scale", t_game_timer_get_time_scale, 1, k_game_timer_get_time_scale, JOY_V_F64 },
        { "game", "game_timer_set_target_fps", t_game_timer_set_target_fps, 2, k_game_timer_set_target_fps, JOY_V_NULL },
        { "game", "game_timer_get_target_fps", t_game_timer_get_target_fps, 1, k_game_timer_get_target_fps, JOY_V_I32 },
        { "clipboard", "set_clipboard_text", t_set_clipboard_text, 1, k_set_clipboard_text, JOY_V_NULL },
        { "sys", "sys_init_netenv", t_sys_init_netenv, 0, NULL, JOY_V_BOOL },
        { "sys", "sys_release_netenv", t_sys_release_netenv, 0, NULL, JOY_V_BOOL },
        { "sys", "sys_anyaddr", t_sys_anyaddr, 0, NULL, JOY_V_I32 },
        { "sys", "sys_closesocket", t_sys_closesocket, 1, k_sys_closesocket, JOY_V_BOOL },
        { "sys", "sys_set_sock_sndtimeo", t_sys_set_sock_sndtimeo, 2, k_sys_set_sock_sndtimeo, JOY_V_I32 },
        { "sys", "sys_set_sock_rcvtimeo", t_sys_set_sock_rcvtimeo, 2, k_sys_set_sock_rcvtimeo, JOY_V_I32 },
        { "sys", "sys_set_sock_accpettimeo", t_sys_set_sock_accpettimeo, 2, k_sys_set_sock_accpettimeo, JOY_V_I32 },
        { "sys", "sys_set_sock_broadcast", t_sys_set_sock_broadcast, 1, k_sys_set_sock_broadcast, JOY_V_I32 },
        { "sys", "sys_set_sock_reuseaddr", t_sys_set_sock_reuseaddr, 1, k_sys_set_sock_reuseaddr, JOY_V_I32 },
        { "sys", "sys_tcp", t_sys_tcp, 0, NULL, JOY_V_I64 },
        { "sys", "sys_udp", t_sys_udp, 0, NULL, JOY_V_I64 },
        { "sys", "sys_bind", t_sys_bind, 3, k_sys_bind, JOY_V_BOOL },
        { "sys", "sys_connect", t_sys_connect, 3, k_sys_connect, JOY_V_BOOL },
        { "sys", "sys_listen", t_sys_listen, 1, k_sys_listen, JOY_V_BOOL },
        { "sys", "sys_send", t_sys_send, 3, k_sys_send, JOY_V_I32 },
        { "sys", "sys_sendto", t_sys_sendto, 5, k_sys_sendto, JOY_V_I32 },
        { "sys", "sys_current_time", t_sys_current_time, 0, NULL, JOY_V_I64 },
        { "sys", "sys_sleep", t_sys_sleep, 1, k_sys_sleep, JOY_V_NULL },
        { "sys", "sys_install_crash_handlers", t_sys_install_crash_handlers, 0, NULL, JOY_V_NULL },
        { "iocp", "iocp_server_destroy", t_iocp_server_destroy, 1, k_iocp_server_destroy, JOY_V_NULL },
        { "iocp", "iocp_process_events", t_iocp_process_events, 2, k_iocp_process_events, JOY_V_NULL },
        { "font", "font_destroy", t_font_destroy, 1, k_font_destroy, JOY_V_NULL },
        { "font", "font_get_scale", t_font_get_scale, 1, k_font_get_scale, JOY_V_F64 },
        { "font", "font_get_size", t_font_get_size, 1, k_font_get_size, JOY_V_I32 },
        { "font", "font_get_ascent", t_font_get_ascent, 1, k_font_get_ascent, JOY_V_I32 },
        { "r2d", "r2d_destroy", t_r2d_destroy, 1, k_r2d_destroy, JOY_V_NULL },
        { "r2d", "r2d_set_color", t_r2d_set_color, 5, k_r2d_set_color, JOY_V_BOOL },
        { "r2d", "r2d_begin", t_r2d_begin, 1, k_r2d_begin, JOY_V_NULL },
        { "r2d", "r2d_end", t_r2d_end, 1, k_r2d_end, JOY_V_NULL },
        { "r2d", "r2d_set_logical_presentation", t_r2d_set_logical_presentation, 3, k_r2d_set_logical_presentation, JOY_V_BOOL },
        { "r2d", "r2d_set_logical_presentation_mode", t_r2d_set_logical_presentation_mode, 4, k_r2d_set_logical_presentation_mode, JOY_V_BOOL },
        { "r2d", "r2d_set_vsync", t_r2d_set_vsync, 2, k_r2d_set_vsync, JOY_V_BOOL },
        { "r2d", "r2d_set_scale", t_r2d_set_scale, 3, k_r2d_set_scale, JOY_V_BOOL },
        { "r2d", "r2d_set_colorf", t_r2d_set_colorf, 5, k_r2d_set_colorf, JOY_V_NULL },
        { "r2d", "r2d_set_blend_mode", t_r2d_set_blend_mode, 2, k_r2d_set_blend_mode, JOY_V_BOOL },
        { "r2d", "r2d_draw_point", t_r2d_draw_point, 3, k_r2d_draw_point, JOY_V_BOOL },
        { "r2d", "r2d_clear_clip", t_r2d_clear_clip, 1, k_r2d_clear_clip, JOY_V_NULL },
        { "r2d", "r2d_pop_clip_rect", t_r2d_pop_clip_rect, 1, k_r2d_pop_clip_rect, JOY_V_NULL },
        { "r2d", "r2d_reset_clip_stack", t_r2d_reset_clip_stack, 1, k_r2d_reset_clip_stack, JOY_V_NULL },
        { "r2d", "r2d_point_in_clip", t_r2d_point_in_clip, 3, k_r2d_point_in_clip, JOY_V_BOOL },
        { "shape", "shape_draw_line", t_shape_draw_line, 5, k_shape_draw_line, JOY_V_BOOL },
        { "image", "image_destroy", t_image_destroy, 1, k_image_destroy, JOY_V_NULL },
        { "image", "image_set_alpha", t_image_set_alpha, 2, k_image_set_alpha, JOY_V_NULL },
        { "image", "image_set_blend_mode", t_image_set_blend_mode, 2, k_image_set_blend_mode, JOY_V_NULL },
        { "image", "image_set_scale_mode", t_image_set_scale_mode, 2, k_image_set_scale_mode, JOY_V_NULL },
        { "text", "text_destroy", t_text_destroy, 1, k_text_destroy, JOY_V_NULL },
        { "jtext", "jtext_print", t_jtext_print, 4, k_jtext_print, JOY_V_NULL },
        { "r2d", "r2d_draw_point_f", t_r2d_draw_point_f, 3, k_r2d_draw_point_f, JOY_V_BOOL },
        { "shape", "shape_draw_rectangle_f", t_shape_draw_rectangle_f, 6, k_shape_draw_rectangle_f, JOY_V_BOOL },
        { "shape", "shape_draw_line_f", t_shape_draw_line_f, 5, k_shape_draw_line_f, JOY_V_BOOL },
        { "text", "text_get_width", t_text_get_width, 1, k_text_get_width, JOY_V_I32 },
        { "text", "text_get_height", t_text_get_height, 1, k_text_get_height, JOY_V_I32 },
        { "jtext", "jtext_cache_clear", t_jtext_cache_clear, 0, NULL, JOY_V_NULL },
        { "jtext", "jtext_cache_set_limit", t_jtext_cache_set_limit, 1, k_jtext_cache_set_limit, JOY_V_NULL },
        { "jtext", "jtext_cache_get_count", t_jtext_cache_get_count, 0, NULL, JOY_V_I32 },
        { "jtext", "jtext_cache_sweep", t_jtext_cache_sweep, 1, k_jtext_cache_sweep, JOY_V_NULL },
        { "animation", "animation_destroy", t_animation_destroy, 1, k_animation_destroy, JOY_V_NULL },
        { "animation", "animation_reset", t_animation_reset, 1, k_animation_reset, JOY_V_NULL },
        { "animation", "animation_is_finished", t_animation_is_finished, 1, k_animation_is_finished, JOY_V_BOOL },
        { "animation", "animation_set_position", t_animation_set_position, 3, k_animation_set_position, JOY_V_NULL },
        { "animation", "animation_set_scale", t_animation_set_scale, 3, k_animation_set_scale, JOY_V_NULL },
        { "animation", "animation_set_rotation", t_animation_set_rotation, 2, k_animation_set_rotation, JOY_V_NULL },
        { "animation", "animation_update", t_animation_update, 2, k_animation_update, JOY_V_NULL },
        { "r3d", "r3d_destroy", t_r3d_destroy, 1, k_r3d_destroy, JOY_V_NULL },
        { "r3d", "r3d_begin", t_r3d_begin, 1, k_r3d_begin, JOY_V_BOOL },
        { "r3d", "r3d_end", t_r3d_end, 1, k_r3d_end, JOY_V_NULL },
        { "r3d", "r3d_get_draw_calls", t_r3d_get_draw_calls, 1, k_r3d_get_draw_calls, JOY_V_I32 },
        { "r3d", "r3d_set_clear_color", t_r3d_set_clear_color, 4, k_r3d_set_clear_color, JOY_V_NULL },
        { "r3d", "r3d_mesh_destroy", t_r3d_mesh_destroy, 2, k_r3d_mesh_destroy, JOY_V_NULL },
        { "r3d", "r3d_font_init", t_r3d_font_init, 3, k_r3d_font_init, JOY_V_BOOL },
        { "r3d", "r3d_font_draw_2d", t_r3d_font_draw_2d, 8, k_r3d_font_draw_2d, JOY_V_BOOL },
        { "r3d", "r3d_font_cache_set_limit", t_r3d_font_cache_set_limit, 2, k_r3d_font_cache_set_limit, JOY_V_NULL },
        { "r3d", "r3d_font_cache_get_count", t_r3d_font_cache_get_count, 1, k_r3d_font_cache_get_count, JOY_V_I32 },
        { "r3d", "r3d_font_cache_clear", t_r3d_font_cache_clear, 1, k_r3d_font_cache_clear, JOY_V_NULL },
        { "r3d", "r3d_font_cache_sweep", t_r3d_font_cache_sweep, 2, k_r3d_font_cache_sweep, JOY_V_NULL },
        { "r3d", "r3d_set_logical_presentation", t_r3d_set_logical_presentation, 3, k_r3d_set_logical_presentation, JOY_V_NULL },
        { "r3d", "r3d_set_logical_presentation_mode", t_r3d_set_logical_presentation_mode, 4, k_r3d_set_logical_presentation_mode, JOY_V_NULL },
        { "r3d", "r3d_button_draw", t_r3d_button_draw, 5, k_r3d_button_draw, JOY_V_NULL },
        { "r3d", "r3d_button_draw_circle", t_r3d_button_draw_circle, 5, k_r3d_button_draw_circle, JOY_V_NULL },
        { "r3d", "r3d_button_is_hovered", t_r3d_button_is_hovered, 4, k_r3d_button_is_hovered, JOY_V_BOOL },
        { "r3d", "r3d_button_is_pressed", t_r3d_button_is_pressed, 5, k_r3d_button_is_pressed, JOY_V_BOOL },
        { "r3d", "r3d_button_release", t_r3d_button_release, 4, k_r3d_button_release, JOY_V_NULL },
        { "r3d", "r3d_button_destroy", t_r3d_button_destroy, 1, k_r3d_button_destroy, JOY_V_NULL },
        { "r3d", "r3d_label_set_text", t_r3d_label_set_text, 2, k_r3d_label_set_text, JOY_V_NULL },
        { "r3d", "r3d_label_draw", t_r3d_label_draw, 5, k_r3d_label_draw, JOY_V_NULL },
        { "r3d", "r3d_label_destroy", t_r3d_label_destroy, 1, k_r3d_label_destroy, JOY_V_NULL },
        { "r3d", "r3d_input_set_text", t_r3d_input_set_text, 2, k_r3d_input_set_text, JOY_V_NULL },
        { "r3d", "r3d_input_is_hovered", t_r3d_input_is_hovered, 4, k_r3d_input_is_hovered, JOY_V_BOOL },
        { "r3d", "r3d_input_focus", t_r3d_input_focus, 1, k_r3d_input_focus, JOY_V_NULL },
        { "r3d", "r3d_input_defocus", t_r3d_input_defocus, 1, k_r3d_input_defocus, JOY_V_NULL },
        { "r3d", "r3d_input_is_focused", t_r3d_input_is_focused, 1, k_r3d_input_is_focused, JOY_V_BOOL },
        { "r3d", "r3d_input_start_text_input", t_r3d_input_start_text_input, 2, k_r3d_input_start_text_input, JOY_V_NULL },
        { "r3d", "r3d_input_stop_text_input", t_r3d_input_stop_text_input, 2, k_r3d_input_stop_text_input, JOY_V_NULL },
        { "r3d", "r3d_input_insert", t_r3d_input_insert, 2, k_r3d_input_insert, JOY_V_NULL },
        { "r3d", "r3d_input_backspace", t_r3d_input_backspace, 1, k_r3d_input_backspace, JOY_V_NULL },
        { "r3d", "r3d_input_delete", t_r3d_input_delete, 1, k_r3d_input_delete, JOY_V_NULL },
        { "r3d", "r3d_input_cursor_left", t_r3d_input_cursor_left, 1, k_r3d_input_cursor_left, JOY_V_NULL },
        { "r3d", "r3d_input_cursor_right", t_r3d_input_cursor_right, 1, k_r3d_input_cursor_right, JOY_V_NULL },
        { "r3d", "r3d_input_cursor_home", t_r3d_input_cursor_home, 1, k_r3d_input_cursor_home, JOY_V_NULL },
        { "r3d", "r3d_input_cursor_end", t_r3d_input_cursor_end, 1, k_r3d_input_cursor_end, JOY_V_NULL },
        { "r3d", "r3d_input_click", t_r3d_input_click, 4, k_r3d_input_click, JOY_V_NULL },
        { "r3d", "r3d_input_submit", t_r3d_input_submit, 1, k_r3d_input_submit, JOY_V_NULL },
        { "r3d", "r3d_input_draw", t_r3d_input_draw, 5, k_r3d_input_draw, JOY_V_NULL },
        { "r3d", "r3d_input_destroy", t_r3d_input_destroy, 1, k_r3d_input_destroy, JOY_V_NULL },
        { "r3d", "r3d_combobox_input_destroy", t_r3d_combobox_input_destroy, 1, k_r3d_combobox_input_destroy, JOY_V_NULL },
        { "r3d", "r3d_combobox_input_add_item", t_r3d_combobox_input_add_item, 2, k_r3d_combobox_input_add_item, JOY_V_NULL },
        { "r3d", "r3d_combobox_input_clear_items", t_r3d_combobox_input_clear_items, 1, k_r3d_combobox_input_clear_items, JOY_V_NULL },
        { "r3d", "r3d_combobox_input_set_text", t_r3d_combobox_input_set_text, 2, k_r3d_combobox_input_set_text, JOY_V_NULL },
        { "r3d", "r3d_combobox_input_hit_item", t_r3d_combobox_input_hit_item, 3, k_r3d_combobox_input_hit_item, JOY_V_I32 },
        { "r3d", "r3d_combobox_input_draw", t_r3d_combobox_input_draw, 5, k_r3d_combobox_input_draw, JOY_V_NULL },
        { "r3d", "r3d_combobox_input_draw_dropdown", t_r3d_combobox_input_draw_dropdown, 5, k_r3d_combobox_input_draw_dropdown, JOY_V_NULL },
        { "r3d", "r3d_slider_destroy", t_r3d_slider_destroy, 1, k_r3d_slider_destroy, JOY_V_NULL },
        { "r3d", "r3d_slider_set_value", t_r3d_slider_set_value, 2, k_r3d_slider_set_value, JOY_V_NULL },
        { "r3d", "r3d_slider_get_value", t_r3d_slider_get_value, 1, k_r3d_slider_get_value, JOY_V_F64 },
        { "r3d", "r3d_slider_draw", t_r3d_slider_draw, 5, k_r3d_slider_draw, JOY_V_NULL },
        { "r3d", "r3d_slider_is_pressed", t_r3d_slider_is_pressed, 4, k_r3d_slider_is_pressed, JOY_V_BOOL },
        { "r3d", "r3d_slider_set_pressed", t_r3d_slider_set_pressed, 2, k_r3d_slider_set_pressed, JOY_V_NULL },
        { "r3d", "r3d_slider_drag_move", t_r3d_slider_drag_move, 4, k_r3d_slider_drag_move, JOY_V_NULL },
        { "r3d", "r3d_slider_is_hovered", t_r3d_slider_is_hovered, 4, k_r3d_slider_is_hovered, JOY_V_BOOL },
        { "r3d", "r3d_slider_set_hovered", t_r3d_slider_set_hovered, 2, k_r3d_slider_set_hovered, JOY_V_NULL },
        { "r3d", "r3d_checkbox_destroy", t_r3d_checkbox_destroy, 1, k_r3d_checkbox_destroy, JOY_V_NULL },
        { "r3d", "r3d_checkbox_toggle", t_r3d_checkbox_toggle, 1, k_r3d_checkbox_toggle, JOY_V_NULL },
        { "r3d", "r3d_checkbox_is_pressed", t_r3d_checkbox_is_pressed, 4, k_r3d_checkbox_is_pressed, JOY_V_BOOL },
        { "r3d", "r3d_checkbox_set_hovered", t_r3d_checkbox_set_hovered, 2, k_r3d_checkbox_set_hovered, JOY_V_NULL },
        { "r3d", "r3d_checkbox_is_hovered", t_r3d_checkbox_is_hovered, 4, k_r3d_checkbox_is_hovered, JOY_V_BOOL },
        { "r3d", "r3d_checkbox_draw", t_r3d_checkbox_draw, 5, k_r3d_checkbox_draw, JOY_V_NULL },
        { "r3d", "r3d_draw_grid", t_r3d_draw_grid, 4, k_r3d_draw_grid, JOY_V_NULL },
        { "janimator", "janimator_destroy", t_janimator_destroy, 1, k_janimator_destroy, JOY_V_NULL },
        { "janimator", "janimator_set_float", t_janimator_set_float, 3, k_janimator_set_float, JOY_V_NULL },
        { "janimator", "janimator_set_int", t_janimator_set_int, 3, k_janimator_set_int, JOY_V_NULL },
        { "janimator", "janimator_set_bool", t_janimator_set_bool, 3, k_janimator_set_bool, JOY_V_NULL },
        { "janimator", "janimator_set_trigger", t_janimator_set_trigger, 2, k_janimator_set_trigger, JOY_V_NULL },
        { "janimator", "janimator_get_float", t_janimator_get_float, 2, k_janimator_get_float, JOY_V_F64 },
        { "janimator", "janimator_get_int", t_janimator_get_int, 2, k_janimator_get_int, JOY_V_I32 },
        { "janimator", "janimator_get_bool", t_janimator_get_bool, 2, k_janimator_get_bool, JOY_V_BOOL },
        { "janimator", "janimator_add_state", t_janimator_add_state, 2, k_janimator_add_state, JOY_V_NULL },
        { "janimator", "janimator_set_default_state", t_janimator_set_default_state, 2, k_janimator_set_default_state, JOY_V_NULL },
        { "janimator", "janimator_state_add_color", t_janimator_state_add_color, 9, k_janimator_state_add_color, JOY_V_NULL },
        { "janimator", "janimator_state_set_loop", t_janimator_state_set_loop, 3, k_janimator_state_set_loop, JOY_V_NULL },
        { "janimator", "janimator_state_set_speed", t_janimator_state_set_speed, 3, k_janimator_state_set_speed, JOY_V_NULL },
        { "janimator", "janimator_state_set_blend", t_janimator_state_set_blend, 3, k_janimator_state_set_blend, JOY_V_NULL },
        { "janimator", "janimator_add_transition", t_janimator_add_transition, 3, k_janimator_add_transition, JOY_V_I32 },
        { "janimator", "janimator_add_any_transition", t_janimator_add_any_transition, 2, k_janimator_add_any_transition, JOY_V_I32 },
        { "janimator", "janimator_transition_add_cond", t_janimator_transition_add_cond, 5, k_janimator_transition_add_cond, JOY_V_NULL },
        { "janimator", "janimator_transition_set_exit_time", t_janimator_transition_set_exit_time, 3, k_janimator_transition_set_exit_time, JOY_V_NULL },
        { "janimator", "janimator_update", t_janimator_update, 2, k_janimator_update, JOY_V_NULL },
        { "janimator", "janimator_draw", t_janimator_draw, 3, k_janimator_draw, JOY_V_NULL },
        { "janimator", "janimator_set_flip_h", t_janimator_set_flip_h, 2, k_janimator_set_flip_h, JOY_V_NULL },
        { "label", "label_destroy", t_label_destroy, 1, k_label_destroy, JOY_V_NULL },
        { "label", "label_draw", t_label_draw, 1, k_label_draw, JOY_V_NULL },
        { "button", "button_destroy", t_button_destroy, 1, k_button_destroy, JOY_V_NULL },
        { "button", "button_set_position", t_button_set_position, 3, k_button_set_position, JOY_V_NULL },
        { "button", "button_set_size", t_button_set_size, 3, k_button_set_size, JOY_V_NULL },
        { "button", "button_set_image", t_button_set_image, 2, k_button_set_image, JOY_V_NULL },
        { "button", "button_set_pressed_image", t_button_set_pressed_image, 2, k_button_set_pressed_image, JOY_V_NULL },
        { "button", "button_is_hovered", t_button_is_hovered, 3, k_button_is_hovered, JOY_V_BOOL },
        { "button", "button_is_pressed", t_button_is_pressed, 3, k_button_is_pressed, JOY_V_BOOL },
        { "button", "button_set_hovered", t_button_set_hovered, 2, k_button_set_hovered, JOY_V_NULL },
        { "button", "button_set_pressed", t_button_set_pressed, 2, k_button_set_pressed, JOY_V_NULL },
        { "button", "button_get_pressed", t_button_get_pressed, 1, k_button_get_pressed, JOY_V_BOOL },
        { "button", "button_set_text_offset", t_button_set_text_offset, 3, k_button_set_text_offset, JOY_V_NULL },
        { "button", "button_draw", t_button_draw, 1, k_button_draw, JOY_V_NULL },
        { "scrollview", "scrollview_destroy", t_scrollview_destroy, 1, k_scrollview_destroy, JOY_V_NULL },
        { "scrollview", "scrollview_set_view_px", t_scrollview_set_view_px, 5, k_scrollview_set_view_px, JOY_V_NULL },
        { "scrollview", "scrollview_set_content", t_scrollview_set_content, 3, k_scrollview_set_content, JOY_V_NULL },
        { "scrollview", "scrollview_set_offset", t_scrollview_set_offset, 3, k_scrollview_set_offset, JOY_V_NULL },
        { "scrollview", "scrollview_clamp", t_scrollview_clamp, 1, k_scrollview_clamp, JOY_V_NULL },
        { "scrollview", "scrollview_get_offset_x", t_scrollview_get_offset_x, 1, k_scrollview_get_offset_x, JOY_V_F64 },
        { "scrollview", "scrollview_get_offset_y", t_scrollview_get_offset_y, 1, k_scrollview_get_offset_y, JOY_V_F64 },
        { "scrollview", "scrollview_set_show_bar", t_scrollview_set_show_bar, 2, k_scrollview_set_show_bar, JOY_V_NULL },
        { "scrollview", "scrollview_dragging", t_scrollview_dragging, 1, k_scrollview_dragging, JOY_V_BOOL },
        { "scrollview", "scrollview_hit", t_scrollview_hit, 3, k_scrollview_hit, JOY_V_BOOL },
        { "scrollview", "scrollview_begin", t_scrollview_begin, 1, k_scrollview_begin, JOY_V_NULL },
        { "scrollview", "scrollview_end", t_scrollview_end, 1, k_scrollview_end, JOY_V_NULL },
        { "combobox", "combobox_destroy", t_combobox_destroy, 1, k_combobox_destroy, JOY_V_NULL },
        { "combobox", "combobox_is_pressed", t_combobox_is_pressed, 3, k_combobox_is_pressed, JOY_V_BOOL },
        { "combobox", "combobox_is_open", t_combobox_is_open, 1, k_combobox_is_open, JOY_V_BOOL },
        { "combobox", "combobox_set_open", t_combobox_set_open, 2, k_combobox_set_open, JOY_V_NULL },
        { "combobox", "combobox_hit_item", t_combobox_hit_item, 3, k_combobox_hit_item, JOY_V_I32 },
        { "combobox", "combobox_set_selected", t_combobox_set_selected, 2, k_combobox_set_selected, JOY_V_NULL },
        { "combobox", "combobox_get_selected", t_combobox_get_selected, 1, k_combobox_get_selected, JOY_V_I32 },
        { "combobox", "combobox_draw", t_combobox_draw, 1, k_combobox_draw, JOY_V_NULL },
        { "combobox", "combobox_input_destroy", t_combobox_input_destroy, 1, k_combobox_input_destroy, JOY_V_NULL },
        { "combobox", "combobox_input_add_item", t_combobox_input_add_item, 3, k_combobox_input_add_item, JOY_V_NULL },
        { "combobox", "combobox_input_clear_items", t_combobox_input_clear_items, 1, k_combobox_input_clear_items, JOY_V_NULL },
        { "combobox", "combobox_input_set_text", t_combobox_input_set_text, 2, k_combobox_input_set_text, JOY_V_NULL },
        { "combobox", "combobox_input_draw", t_combobox_input_draw, 1, k_combobox_input_draw, JOY_V_NULL },
        { "combobox", "combobox_input_draw_dropdown", t_combobox_input_draw_dropdown, 1, k_combobox_input_draw_dropdown, JOY_V_NULL },
        { "combobox", "combobox_input_is_open", t_combobox_input_is_open, 1, k_combobox_input_is_open, JOY_V_BOOL },
        { "combobox", "combobox_input_set_popup_node", t_combobox_input_set_popup_node, 2, k_combobox_input_set_popup_node, JOY_V_NULL },
        { "combobox", "combobox_input_hit_item", t_combobox_input_hit_item, 3, k_combobox_input_hit_item, JOY_V_I32 },
        { "combobox", "combobox_input_set_open", t_combobox_input_set_open, 2, k_combobox_input_set_open, JOY_V_NULL },
        { "datagrid", "datagrid_setheaders", t_datagrid_setheaders, 2, k_datagrid_setheaders, JOY_V_NULL },
        { "datagrid", "datagrid_setheader", t_datagrid_setheader, 3, k_datagrid_setheader, JOY_V_NULL },
        { "datagrid", "datagrid_setcell", t_datagrid_setcell, 4, k_datagrid_setcell, JOY_V_NULL },
        { "datagrid", "datagrid_destroy", t_datagrid_destroy, 1, k_datagrid_destroy, JOY_V_NULL },
        { "datagrid", "datagrid_draw", t_datagrid_draw, 2, k_datagrid_draw, JOY_V_NULL },
        { "datagrid", "datagrid_hit_row", t_datagrid_hit_row, 3, k_datagrid_hit_row, JOY_V_I32 },
        { "datagrid", "datagrid_scroll", t_datagrid_scroll, 2, k_datagrid_scroll, JOY_V_NULL },
        { "datagrid", "datagrid_set_selected_row", t_datagrid_set_selected_row, 2, k_datagrid_set_selected_row, JOY_V_NULL },
        { "radio", "radio_destroy", t_radio_destroy, 1, k_radio_destroy, JOY_V_NULL },
        { "radio", "radio_is_pressed", t_radio_is_pressed, 3, k_radio_is_pressed, JOY_V_BOOL },
        { "radio", "radio_set_hovered", t_radio_set_hovered, 2, k_radio_set_hovered, JOY_V_NULL },
        { "checkbox", "checkbox_destroy", t_checkbox_destroy, 1, k_checkbox_destroy, JOY_V_NULL },
        { "checkbox", "checkbox_set_r2d", t_checkbox_set_r2d, 2, k_checkbox_set_r2d, JOY_V_NULL },
        { "checkbox", "checkbox_set_position", t_checkbox_set_position, 3, k_checkbox_set_position, JOY_V_NULL },
        { "checkbox", "checkbox_toggle", t_checkbox_toggle, 1, k_checkbox_toggle, JOY_V_NULL },
        { "checkbox", "checkbox_is_pressed", t_checkbox_is_pressed, 3, k_checkbox_is_pressed, JOY_V_BOOL },
        { "checkbox", "checkbox_set_hovered", t_checkbox_set_hovered, 2, k_checkbox_set_hovered, JOY_V_NULL },
        { "joystick", "joystick_destroy", t_joystick_destroy, 1, k_joystick_destroy, JOY_V_NULL },
        { "joystick", "joystick_set_textures", t_joystick_set_textures, 3, k_joystick_set_textures, JOY_V_NULL },
        { "joystick", "joystick_set_vector", t_joystick_set_vector, 3, k_joystick_set_vector, JOY_V_NULL },
        { "joystick", "joystick_draw", t_joystick_draw, 1, k_joystick_draw, JOY_V_NULL },
        { "joystick", "joystick_reset", t_joystick_reset, 1, k_joystick_reset, JOY_V_NULL },
        { "joystick", "joystick_set_position", t_joystick_set_position, 3, k_joystick_set_position, JOY_V_NULL },
        { "joystick", "joystick_set_radius", t_joystick_set_radius, 2, k_joystick_set_radius, JOY_V_NULL },
        { "joystick", "joystick_is_dragging", t_joystick_is_dragging, 1, k_joystick_is_dragging, JOY_V_BOOL },
        { "joystick", "joystick_touch_down", t_joystick_touch_down, 3, k_joystick_touch_down, JOY_V_NULL },
        { "joystick", "joystick_touch_move", t_joystick_touch_move, 3, k_joystick_touch_move, JOY_V_NULL },
        { "joystick", "joystick_touch_up", t_joystick_touch_up, 1, k_joystick_touch_up, JOY_V_NULL },
        { "joystick", "joystick_get_magnitude", t_joystick_get_magnitude, 1, k_joystick_get_magnitude, JOY_V_F64 },
        { "inputbox", "inputbox_destroy", t_inputbox_destroy, 1, k_inputbox_destroy, JOY_V_NULL },
        { "inputbox", "inputbox_set_text", t_inputbox_set_text, 2, k_inputbox_set_text, JOY_V_NULL },
        { "inputbox", "inputbox_draw", t_inputbox_draw, 1, k_inputbox_draw, JOY_V_NULL },
        { "inputbox", "inputbox_is_pressed", t_inputbox_is_pressed, 3, k_inputbox_is_pressed, JOY_V_BOOL },
        { "inputbox", "inputbox_is_hovered", t_inputbox_is_hovered, 3, k_inputbox_is_hovered, JOY_V_BOOL },
        { "inputbox", "inputbox_focus", t_inputbox_focus, 1, k_inputbox_focus, JOY_V_NULL },
        { "inputbox", "inputbox_defocus", t_inputbox_defocus, 1, k_inputbox_defocus, JOY_V_NULL },
        { "inputbox", "inputbox_blur", t_inputbox_blur, 1, k_inputbox_blur, JOY_V_NULL },
        { "inputbox", "inputbox_insert", t_inputbox_insert, 3, k_inputbox_insert, JOY_V_NULL },
        { "inputbox", "inputbox_backspace", t_inputbox_backspace, 1, k_inputbox_backspace, JOY_V_NULL },
        { "inputbox", "inputbox_delete", t_inputbox_delete, 1, k_inputbox_delete, JOY_V_NULL },
        { "inputbox", "inputbox_cursor_left", t_inputbox_cursor_left, 1, k_inputbox_cursor_left, JOY_V_NULL },
        { "inputbox", "inputbox_cursor_right", t_inputbox_cursor_right, 1, k_inputbox_cursor_right, JOY_V_NULL },
        { "inputbox", "inputbox_cursor_home", t_inputbox_cursor_home, 1, k_inputbox_cursor_home, JOY_V_NULL },
        { "inputbox", "inputbox_cursor_end", t_inputbox_cursor_end, 1, k_inputbox_cursor_end, JOY_V_NULL },
        { "inputbox", "inputbox_select_left", t_inputbox_select_left, 1, k_inputbox_select_left, JOY_V_NULL },
        { "inputbox", "inputbox_select_right", t_inputbox_select_right, 1, k_inputbox_select_right, JOY_V_NULL },
        { "inputbox", "inputbox_select_home", t_inputbox_select_home, 1, k_inputbox_select_home, JOY_V_NULL },
        { "inputbox", "inputbox_select_end", t_inputbox_select_end, 1, k_inputbox_select_end, JOY_V_NULL },
        { "inputbox", "inputbox_select_all", t_inputbox_select_all, 1, k_inputbox_select_all, JOY_V_NULL },
        { "inputbox", "inputbox_paste", t_inputbox_paste, 1, k_inputbox_paste, JOY_V_NULL },
        { "inputbox", "inputbox_copy", t_inputbox_copy, 1, k_inputbox_copy, JOY_V_NULL },
        { "inputbox", "inputbox_cut", t_inputbox_cut, 1, k_inputbox_cut, JOY_V_NULL },
        { "inputbox", "inputbox_is_focused", t_inputbox_is_focused, 1, k_inputbox_is_focused, JOY_V_BOOL },
        { "slider", "slider_destroy", t_slider_destroy, 1, k_slider_destroy, JOY_V_NULL },
        { "slider", "slider_set_value", t_slider_set_value, 2, k_slider_set_value, JOY_V_NULL },
        { "slider", "slider_get_value", t_slider_get_value, 1, k_slider_get_value, JOY_V_F64 },
        { "slider", "slider_set_rect", t_slider_set_rect, 5, k_slider_set_rect, JOY_V_NULL },
        { "slider", "slider_set_range", t_slider_set_range, 3, k_slider_set_range, JOY_V_NULL },
        { "slider", "slider_draw", t_slider_draw, 1, k_slider_draw, JOY_V_NULL },
        { "slider", "slider_is_pressed", t_slider_is_pressed, 3, k_slider_is_pressed, JOY_V_BOOL },
        { "slider", "slider_set_pressed", t_slider_set_pressed, 2, k_slider_set_pressed, JOY_V_NULL },
        { "slider", "slider_drag_move", t_slider_drag_move, 3, k_slider_drag_move, JOY_V_NULL },
        { "slider", "slider_is_hovered", t_slider_is_hovered, 3, k_slider_is_hovered, JOY_V_BOOL },
        { "slider", "slider_set_hovered", t_slider_set_hovered, 2, k_slider_set_hovered, JOY_V_NULL },
        { "popup", "popup_destroy", t_popup_destroy, 1, k_popup_destroy, JOY_V_NULL },
        { "popup", "popup_set_title", t_popup_set_title, 3, k_popup_set_title, JOY_V_NULL },
        { "popup", "popup_set_content", t_popup_set_content, 3, k_popup_set_content, JOY_V_NULL },
        { "popup", "popup_set_position", t_popup_set_position, 3, k_popup_set_position, JOY_V_NULL },
        { "popup", "popup_set_visible", t_popup_set_visible, 2, k_popup_set_visible, JOY_V_NULL },
        { "popup", "popup_set_closeable", t_popup_set_closeable, 2, k_popup_set_closeable, JOY_V_NULL },
        { "popup", "popup_draw", t_popup_draw, 3, k_popup_draw, JOY_V_NULL },
        { "popup", "popup_is_close_clicked", t_popup_is_close_clicked, 3, k_popup_is_close_clicked, JOY_V_BOOL },
        { "popup", "popup_is_title_clicked", t_popup_is_title_clicked, 3, k_popup_is_title_clicked, JOY_V_BOOL },
        { "popup", "popup_drag_move", t_popup_drag_move, 3, k_popup_drag_move, JOY_V_NULL },
        { "popup", "popup_is_inside", t_popup_is_inside, 3, k_popup_is_inside, JOY_V_BOOL },
        { "tooltip", "tooltip_destroy", t_tooltip_destroy, 1, k_tooltip_destroy, JOY_V_NULL },
        { "tooltip", "tooltip_set_text", t_tooltip_set_text, 3, k_tooltip_set_text, JOY_V_NULL },
        { "tooltip", "tooltip_set_position", t_tooltip_set_position, 3, k_tooltip_set_position, JOY_V_NULL },
        { "tooltip", "tooltip_set_visible", t_tooltip_set_visible, 2, k_tooltip_set_visible, JOY_V_NULL },
        { "tooltip", "tooltip_draw", t_tooltip_draw, 2, k_tooltip_draw, JOY_V_NULL },
        { "jui", "jui_ctx_destroy", t_jui_ctx_destroy, 1, k_jui_ctx_destroy, JOY_V_NULL },
        { "jui", "jui_begin", t_jui_begin, 1, k_jui_begin, JOY_V_NULL },
        { "jui", "jui_end", t_jui_end, 1, k_jui_end, JOY_V_NULL },
        { "jui", "jui_input_motion", t_jui_input_motion, 3, k_jui_input_motion, JOY_V_NULL },
        { "jui", "jui_input_down", t_jui_input_down, 2, k_jui_input_down, JOY_V_NULL },
        { "jui", "jui_input_up", t_jui_input_up, 2, k_jui_input_up, JOY_V_NULL },
        { "jui", "jui_input_wheel", t_jui_input_wheel, 2, k_jui_input_wheel, JOY_V_NULL },
        { "jui", "jui_input_key", t_jui_input_key, 3, k_jui_input_key, JOY_V_NULL },
        { "jui", "jui_is_down", t_jui_is_down, 1, k_jui_is_down, JOY_V_BOOL },
        { "jui", "jui_is_pressed", t_jui_is_pressed, 1, k_jui_is_pressed, JOY_V_BOOL },
        { "jui", "jui_is_released", t_jui_is_released, 1, k_jui_is_released, JOY_V_BOOL },
        { "jui", "jui_take_wheel", t_jui_take_wheel, 1, k_jui_take_wheel, JOY_V_F64 },
        { "jui", "jui_is_shift", t_jui_is_shift, 1, k_jui_is_shift, JOY_V_BOOL },
        { "jui", "jui_is_ctrl", t_jui_is_ctrl, 1, k_jui_is_ctrl, JOY_V_BOOL },
        { "jui", "jui_pop_clip", t_jui_pop_clip, 1, k_jui_pop_clip, JOY_V_NULL },
        { "jui", "jui_reset_clip", t_jui_reset_clip, 1, k_jui_reset_clip, JOY_V_NULL },
        { "jui", "jui_panel_end", t_jui_panel_end, 1, k_jui_panel_end, JOY_V_NULL },
        { "jui", "jui_scroll_end", t_jui_scroll_end, 1, k_jui_scroll_end, JOY_V_NULL },
        { "jui", "jui_scroll_set_offset", t_jui_scroll_set_offset, 5, k_jui_scroll_set_offset, JOY_V_NULL },
        { "jtilemap", "jtilemap_destroy", t_jtilemap_destroy, 1, k_jtilemap_destroy, JOY_V_NULL },
        { "jtilemap", "jtilemap_get_width", t_jtilemap_get_width, 1, k_jtilemap_get_width, JOY_V_I32 },
        { "jtilemap", "jtilemap_get_height", t_jtilemap_get_height, 1, k_jtilemap_get_height, JOY_V_I32 },
        { "jtilemap", "jtilemap_get_tilewidth", t_jtilemap_get_tilewidth, 1, k_jtilemap_get_tilewidth, JOY_V_I32 },
        { "jtilemap", "jtilemap_get_tileheight", t_jtilemap_get_tileheight, 1, k_jtilemap_get_tileheight, JOY_V_I32 },
        { "jtilemap", "jtilemap_get_anim_tileid", t_jtilemap_get_anim_tileid, 3, k_jtilemap_get_anim_tileid, JOY_V_I32 },
        { "jtilemap", "jtilemap_get_tile", t_jtilemap_get_tile, 3, k_jtilemap_get_tile, JOY_V_I32 },
        { "jtilemap", "jtilemap_render_all", t_jtilemap_render_all, 5, k_jtilemap_render_all, JOY_V_NULL },
        { "jtilemap", "jtilemap_render_tiles", t_jtilemap_render_tiles, 5, k_jtilemap_render_tiles, JOY_V_NULL },
        { "jtilemap", "jtilemap_render_set_tile_px", t_jtilemap_render_set_tile_px, 2, k_jtilemap_render_set_tile_px, JOY_V_NULL },
        { "jtilemap", "jtilemap_render_layer", t_jtilemap_render_layer, 6, k_jtilemap_render_layer, JOY_V_BOOL },
        { "jtilemap", "jtilemap_render_destroy", t_jtilemap_render_destroy, 1, k_jtilemap_render_destroy, JOY_V_NULL },
        { "kcpserver", "kcpserver_destroy", t_kcpserver_destroy, 1, k_kcpserver_destroy, JOY_V_I32 },
        { "kcpserver", "kcpserver_send", t_kcpserver_send, 4, k_kcpserver_send, JOY_V_NULL },
        { "kcpserver", "kcpserver_broadcast", t_kcpserver_broadcast, 3, k_kcpserver_broadcast, JOY_V_NULL },
        { "kcpserver", "kcpserver_offline", t_kcpserver_offline, 2, k_kcpserver_offline, JOY_V_NULL },
        { "kcpserver", "kcpserver_update", t_kcpserver_update, 1, k_kcpserver_update, JOY_V_NULL },
        { "kcpserver", "kcpserver_connection_count", t_kcpserver_connection_count, 1, k_kcpserver_connection_count, JOY_V_I32 },
        { "kcpserver", "kcpserver_poll_message", t_kcpserver_poll_message, 2, k_kcpserver_poll_message, JOY_V_BOOL },
        { "kcpclient", "kcpclient_destroy", t_kcpclient_destroy, 1, k_kcpclient_destroy, JOY_V_NULL },
        { "kcpclient", "kcpclient_send", t_kcpclient_send, 3, k_kcpclient_send, JOY_V_I32 },
        { "kcpclient", "kcpclient_update", t_kcpclient_update, 1, k_kcpclient_update, JOY_V_NULL },
        { "kcpclient", "kcpclient_poll_message", t_kcpclient_poll_message, 2, k_kcpclient_poll_message, JOY_V_BOOL },
        { "tcpserver", "tcpserver_destroy", t_tcpserver_destroy, 1, k_tcpserver_destroy, JOY_V_I32 },
        { "tcpserver", "tcpserver_send", t_tcpserver_send, 4, k_tcpserver_send, JOY_V_NULL },
        { "tcpserver", "tcpserver_broadcast", t_tcpserver_broadcast, 3, k_tcpserver_broadcast, JOY_V_NULL },
        { "tcpserver", "tcpserver_offline", t_tcpserver_offline, 2, k_tcpserver_offline, JOY_V_NULL },
        { "tcpserver", "tcpserver_update", t_tcpserver_update, 1, k_tcpserver_update, JOY_V_NULL },
        { "tcpserver", "tcpserver_connection_count", t_tcpserver_connection_count, 1, k_tcpserver_connection_count, JOY_V_I32 },
        { "tcpserver", "tcpserver_poll_message", t_tcpserver_poll_message, 2, k_tcpserver_poll_message, JOY_V_BOOL },
        { "tcpclient", "tcpclient_destroy", t_tcpclient_destroy, 1, k_tcpclient_destroy, JOY_V_NULL },
        { "tcpclient", "tcpclient_send", t_tcpclient_send, 3, k_tcpclient_send, JOY_V_I32 },
        { "tcpclient", "tcpclient_update", t_tcpclient_update, 1, k_tcpclient_update, JOY_V_NULL },
        { "tcpclient", "tcpclient_poll_message", t_tcpclient_poll_message, 2, k_tcpclient_poll_message, JOY_V_BOOL },
        { "wsnetserver", "wsnetserver_destroy", t_wsnetserver_destroy, 1, k_wsnetserver_destroy, JOY_V_I32 },
        { "wsnetserver", "wsnetserver_send", t_wsnetserver_send, 4, k_wsnetserver_send, JOY_V_NULL },
        { "wsnetserver", "wsnetserver_broadcast", t_wsnetserver_broadcast, 3, k_wsnetserver_broadcast, JOY_V_NULL },
        { "wsnetserver", "wsnetserver_offline", t_wsnetserver_offline, 2, k_wsnetserver_offline, JOY_V_NULL },
        { "wsnetserver", "wsnetserver_update", t_wsnetserver_update, 1, k_wsnetserver_update, JOY_V_NULL },
        { "wsnetserver", "wsnetserver_connection_count", t_wsnetserver_connection_count, 1, k_wsnetserver_connection_count, JOY_V_I32 },
        { "wsnetserver", "wsnetserver_poll_message", t_wsnetserver_poll_message, 2, k_wsnetserver_poll_message, JOY_V_BOOL },
        { "wsnetclient", "wsnetclient_destroy", t_wsnetclient_destroy, 1, k_wsnetclient_destroy, JOY_V_NULL },
        { "wsnetclient", "wsnetclient_send", t_wsnetclient_send, 3, k_wsnetclient_send, JOY_V_I32 },
        { "wsnetclient", "wsnetclient_update", t_wsnetclient_update, 1, k_wsnetclient_update, JOY_V_NULL },
        { "wsnetclient", "wsnetclient_poll_message", t_wsnetclient_poll_message, 2, k_wsnetclient_poll_message, JOY_V_BOOL },
        { "netclient", "netclient_destroy", t_netclient_destroy, 1, k_netclient_destroy, JOY_V_NULL },
        { "netclient", "netclient_send", t_netclient_send, 3, k_netclient_send, JOY_V_I32 },
        { "netclient", "netclient_update", t_netclient_update, 1, k_netclient_update, JOY_V_NULL },
        { "netclient", "netclient_poll_message", t_netclient_poll_message, 2, k_netclient_poll_message, JOY_V_BOOL },
        { "netserver", "netserver_destroy", t_netserver_destroy, 1, k_netserver_destroy, JOY_V_NULL },
        { "netserver", "netserver_send", t_netserver_send, 4, k_netserver_send, JOY_V_NULL },
        { "netserver", "netserver_broadcast", t_netserver_broadcast, 3, k_netserver_broadcast, JOY_V_NULL },
        { "netserver", "netserver_offline", t_netserver_offline, 2, k_netserver_offline, JOY_V_NULL },
        { "netserver", "netserver_update", t_netserver_update, 1, k_netserver_update, JOY_V_NULL },
        { "netserver", "netserver_connection_count", t_netserver_connection_count, 1, k_netserver_connection_count, JOY_V_I32 },
        { "netserver", "netserver_poll_message", t_netserver_poll_message, 2, k_netserver_poll_message, JOY_V_BOOL },
        { "netmessage", "netmessage_free_data", t_netmessage_free_data, 1, k_netmessage_free_data, JOY_V_NULL },
        { "scene", "scene_node_destroy", t_scene_node_destroy, 1, k_scene_node_destroy, JOY_V_NULL },
        { "scene", "scene_node_add_child", t_scene_node_add_child, 2, k_scene_node_add_child, JOY_V_NULL },
        { "scene", "scene_node_remove_child", t_scene_node_remove_child, 2, k_scene_node_remove_child, JOY_V_NULL },
        { "scene", "scene_node_get_child_count", t_scene_node_get_child_count, 1, k_scene_node_get_child_count, JOY_V_I32 },
        { "scene", "scene_node_set_position", t_scene_node_set_position, 3, k_scene_node_set_position, JOY_V_NULL },
        { "scene", "scene_node_set_world_position", t_scene_node_set_world_position, 3, k_scene_node_set_world_position, JOY_V_NULL },
        { "scene", "scene_node_set_rotation", t_scene_node_set_rotation, 2, k_scene_node_set_rotation, JOY_V_NULL },
        { "scene", "scene_node_get_rotation", t_scene_node_get_rotation, 1, k_scene_node_get_rotation, JOY_V_F64 },
        { "scene", "scene_node_set_scale", t_scene_node_set_scale, 3, k_scene_node_set_scale, JOY_V_NULL },
        { "scene", "scene_node_set_zorder", t_scene_node_set_zorder, 2, k_scene_node_set_zorder, JOY_V_NULL },
        { "scene", "scene_node_get_zorder", t_scene_node_get_zorder, 1, k_scene_node_get_zorder, JOY_V_I32 },
        { "scene", "scene_node_set_name", t_scene_node_set_name, 2, k_scene_node_set_name, JOY_V_NULL },
        { "scene", "scene_node_set_tag", t_scene_node_set_tag, 2, k_scene_node_set_tag, JOY_V_NULL },
        { "scene", "scene_node_update", t_scene_node_update, 2, k_scene_node_update, JOY_V_NULL },
        { "scene", "scene_destroy", t_scene_destroy, 1, k_scene_destroy, JOY_V_NULL },
        { "scene", "scene_add_root_node", t_scene_add_root_node, 2, k_scene_add_root_node, JOY_V_NULL },
        { "scene", "scene_remove_root_node", t_scene_remove_root_node, 2, k_scene_remove_root_node, JOY_V_NULL },
        { "scene", "scene_get_root_count", t_scene_get_root_count, 1, k_scene_get_root_count, JOY_V_I32 },
        { "scene", "scene_update", t_scene_update, 2, k_scene_update, JOY_V_NULL },
        { "scene", "scene_set_popup_node", t_scene_set_popup_node, 2, k_scene_set_popup_node, JOY_V_NULL },
        { "scene", "scene_manager_destroy", t_scene_manager_destroy, 1, k_scene_manager_destroy, JOY_V_NULL },
        { "scene", "scene_manager_push", t_scene_manager_push, 2, k_scene_manager_push, JOY_V_NULL },
        { "scene", "scene_manager_replace", t_scene_manager_replace, 2, k_scene_manager_replace, JOY_V_NULL },
        { "scene", "scene_manager_pop", t_scene_manager_pop, 1, k_scene_manager_pop, JOY_V_NULL },
        { "scene", "scene_manager_update", t_scene_manager_update, 2, k_scene_manager_update, JOY_V_NULL },
        { "scene", "scene_manager_get_count", t_scene_manager_get_count, 1, k_scene_manager_get_count, JOY_V_I32 },
        { "jparticle", "jparticle_destroy", t_jparticle_destroy, 1, k_jparticle_destroy, JOY_V_NULL },
        { "jparticle", "jparticle_set_position", t_jparticle_set_position, 3, k_jparticle_set_position, JOY_V_NULL },
        { "jparticle", "jparticle_emit", t_jparticle_emit, 1, k_jparticle_emit, JOY_V_NULL },
        { "jparticle", "jparticle_clear", t_jparticle_clear, 1, k_jparticle_clear, JOY_V_NULL },
        { "jparticle", "jparticle_update", t_jparticle_update, 2, k_jparticle_update, JOY_V_NULL },
        { "jparticle", "jparticle_render", t_jparticle_render, 2, k_jparticle_render, JOY_V_NULL },
        { "jparticle", "jparticle_count", t_jparticle_count, 1, k_jparticle_count, JOY_V_I32 },
        { "jparticle", "jparticle_is_active", t_jparticle_is_active, 1, k_jparticle_is_active, JOY_V_BOOL },
        { "jlight", "jlight_sys_destroy", t_jlight_sys_destroy, 1, k_jlight_sys_destroy, JOY_V_NULL },
        { "jlight", "jlight_sys_resize", t_jlight_sys_resize, 3, k_jlight_sys_resize, JOY_V_NULL },
        { "jlight", "jlight_remove", t_jlight_remove, 2, k_jlight_remove, JOY_V_NULL },
        { "jlight", "jlight_clear", t_jlight_clear, 1, k_jlight_clear, JOY_V_NULL },
        { "jlight", "jlight_add_occluder", t_jlight_add_occluder, 5, k_jlight_add_occluder, JOY_V_I32 },
        { "jlight", "jlight_clear_occluders", t_jlight_clear_occluders, 1, k_jlight_clear_occluders, JOY_V_NULL },
        { "jlight", "jlight_set_ambient", t_jlight_set_ambient, 4, k_jlight_set_ambient, JOY_V_NULL },
        { "jlight", "jlight_render", t_jlight_render, 2, k_jlight_render, JOY_V_NULL },
        { "jecs", "jecs_destroy", t_jecs_destroy, 1, k_jecs_destroy, JOY_V_NULL },
        { "jecs", "jecs_process", t_jecs_process, 2, k_jecs_process, JOY_V_NULL },
        { "jecs", "jecs_define", t_jecs_define, 3, k_jecs_define, JOY_V_NULL },
        { "audio", "audio_open_device", t_audio_open_device, 0, NULL, JOY_V_U32 },
        { "audio", "audio_close_device", t_audio_close_device, 1, k_audio_close_device, JOY_V_NULL },
        { "audio", "audio_create_wav", t_audio_create_wav, 2, k_audio_create_wav, JOY_V_BOOL },
        { "audio", "audio_create_mp3", t_audio_create_mp3, 2, k_audio_create_mp3, JOY_V_BOOL },
        { "audio", "audio_create_flac", t_audio_create_flac, 2, k_audio_create_flac, JOY_V_BOOL },
        { "audio", "audio_create_ogg", t_audio_create_ogg, 2, k_audio_create_ogg, JOY_V_BOOL },
        { "audio", "audio_shot_play", t_audio_shot_play, 1, k_audio_shot_play, JOY_V_NULL },
        { "audio", "audio_shot_set_gain", t_audio_shot_set_gain, 2, k_audio_shot_set_gain, JOY_V_NULL },
        { "audio", "audio_shot_destroy", t_audio_shot_destroy, 1, k_audio_shot_destroy, JOY_V_NULL },
        { "audio", "audio_bgm_stop", t_audio_bgm_stop, 1, k_audio_bgm_stop, JOY_V_NULL },
        { "audio", "audio_bgm_set_gain", t_audio_bgm_set_gain, 2, k_audio_bgm_set_gain, JOY_V_NULL },
        { "audio", "audio_bgm_destroy", t_audio_bgm_destroy, 1, k_audio_bgm_destroy, JOY_V_NULL },
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

        /* 每条开放的 API 都必须在注册表里且 argc 守卫生效。
         * 故意传错 argc：thunk 第一行就返回，不执行任何 joy 业务函数，
         * 因此批量开放有副作用 / 需要上下文的 API 也不会在这里被调用。 */
        st = JOY_OK;
        joy_api_call("lcg_random_create", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/lcg_random_create";
        st = JOY_OK;
        joy_api_call("lcg_random_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/lcg_random_destroy";
        st = JOY_OK;
        joy_api_call("lcg_random_next", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/lcg_random_next";
        st = JOY_OK;
        joy_api_call("lcg_random_set_state", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/lcg_random_set_state";
        st = JOY_OK;
        joy_api_call("jpm_config_add_spawn", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jpm_config_add_spawn";
        st = JOY_OK;
        joy_api_call("jpm_wave_seed", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jpm_wave_seed";
        st = JOY_OK;
        joy_api_call("sdbm_hash", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/sdbm_hash";
        st = JOY_OK;
        joy_api_call("fnv1a_hash", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/fnv1a_hash";
        st = JOY_OK;
        joy_api_call("jenkins_hash", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jenkins_hash";
        st = JOY_OK;
        joy_api_call("murmur3_hash", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/murmur3_hash";
        st = JOY_OK;
        joy_api_call("kr_hash", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/kr_hash";
        st = JOY_OK;
        joy_api_call("jtimer_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtimer_destroy";
        st = JOY_OK;
        joy_api_call("jtimer_trigger", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtimer_trigger";
        st = JOY_OK;
        joy_api_call("jtimer_set_interval", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtimer_set_interval";
        st = JOY_OK;
        joy_api_call("jtimer_set_active", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtimer_set_active";
        st = JOY_OK;
        joy_api_call("jtimer_reset", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtimer_reset";
        st = JOY_OK;
        joy_api_call("jtimer_get_elapsed", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtimer_get_elapsed";
        st = JOY_OK;
        joy_api_call("init_env", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/init_env";
        st = JOY_OK;
        joy_api_call("quit_env", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/quit_env";
        st = JOY_OK;
        joy_api_call("get_ticks", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/get_ticks";
        st = JOY_OK;
        joy_api_call("destroy_window", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/destroy_window";
        st = JOY_OK;
        joy_api_call("window_set_fullscreen", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/window_set_fullscreen";
        st = JOY_OK;
        joy_api_call("window_get_fullscreen", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/window_get_fullscreen";
        st = JOY_OK;
        joy_api_call("window_set_icon", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/window_set_icon";
        st = JOY_OK;
        joy_api_call("push_user_event", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/push_user_event";
        st = JOY_OK;
        joy_api_call("push_quit_event", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/push_quit_event";
        st = JOY_OK;
        joy_api_call("log_title", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/log_title";
        st = JOY_OK;
        joy_api_call("env_set", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/env_set";
        st = JOY_OK;
        joy_api_call("env_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/env_destroy";
        st = JOY_OK;
        joy_api_call("simple_fps_destory", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/simple_fps_destory";
        st = JOY_OK;
        joy_api_call("simple_fps_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/simple_fps_update";
        st = JOY_OK;
        joy_api_call("simple_fps_value", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/simple_fps_value";
        st = JOY_OK;
        joy_api_call("game_timer_init", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/game_timer_init";
        st = JOY_OK;
        joy_api_call("game_timer_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/game_timer_update";
        st = JOY_OK;
        joy_api_call("game_timer_get_delta_time", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/game_timer_get_delta_time";
        st = JOY_OK;
        joy_api_call("game_timer_get_unscaled_delta_time", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/game_timer_get_unscaled_delta_time";
        st = JOY_OK;
        joy_api_call("game_timer_set_time_scale", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/game_timer_set_time_scale";
        st = JOY_OK;
        joy_api_call("game_timer_get_time_scale", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/game_timer_get_time_scale";
        st = JOY_OK;
        joy_api_call("game_timer_set_target_fps", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/game_timer_set_target_fps";
        st = JOY_OK;
        joy_api_call("game_timer_get_target_fps", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/game_timer_get_target_fps";
        st = JOY_OK;
        joy_api_call("set_clipboard_text", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/set_clipboard_text";
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
        joy_api_call("iocp_server_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/iocp_server_destroy";
        st = JOY_OK;
        joy_api_call("iocp_process_events", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/iocp_process_events";
        st = JOY_OK;
        joy_api_call("font_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/font_destroy";
        st = JOY_OK;
        joy_api_call("font_get_scale", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/font_get_scale";
        st = JOY_OK;
        joy_api_call("font_get_size", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/font_get_size";
        st = JOY_OK;
        joy_api_call("font_get_ascent", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/font_get_ascent";
        st = JOY_OK;
        joy_api_call("r2d_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_destroy";
        st = JOY_OK;
        joy_api_call("r2d_set_color", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_set_color";
        st = JOY_OK;
        joy_api_call("r2d_begin", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_begin";
        st = JOY_OK;
        joy_api_call("r2d_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_end";
        st = JOY_OK;
        joy_api_call("r2d_set_logical_presentation", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_set_logical_presentation";
        st = JOY_OK;
        joy_api_call("r2d_set_logical_presentation_mode", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_set_logical_presentation_mode";
        st = JOY_OK;
        joy_api_call("r2d_set_vsync", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_set_vsync";
        st = JOY_OK;
        joy_api_call("r2d_set_scale", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_set_scale";
        st = JOY_OK;
        joy_api_call("r2d_set_colorf", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_set_colorf";
        st = JOY_OK;
        joy_api_call("r2d_set_blend_mode", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_set_blend_mode";
        st = JOY_OK;
        joy_api_call("r2d_draw_point", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_draw_point";
        st = JOY_OK;
        joy_api_call("r2d_clear_clip", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_clear_clip";
        st = JOY_OK;
        joy_api_call("r2d_pop_clip_rect", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_pop_clip_rect";
        st = JOY_OK;
        joy_api_call("r2d_reset_clip_stack", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_reset_clip_stack";
        st = JOY_OK;
        joy_api_call("r2d_point_in_clip", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_point_in_clip";
        st = JOY_OK;
        joy_api_call("shape_draw_line", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/shape_draw_line";
        st = JOY_OK;
        joy_api_call("image_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/image_destroy";
        st = JOY_OK;
        joy_api_call("image_set_alpha", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/image_set_alpha";
        st = JOY_OK;
        joy_api_call("image_set_blend_mode", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/image_set_blend_mode";
        st = JOY_OK;
        joy_api_call("image_set_scale_mode", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/image_set_scale_mode";
        st = JOY_OK;
        joy_api_call("text_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/text_destroy";
        st = JOY_OK;
        joy_api_call("jtext_print", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtext_print";
        st = JOY_OK;
        joy_api_call("r2d_draw_point_f", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/r2d_draw_point_f";
        st = JOY_OK;
        joy_api_call("shape_draw_rectangle_f", NULL, 7, &st);
        if (st != JOY_ERR_ARGC) return "registry/shape_draw_rectangle_f";
        st = JOY_OK;
        joy_api_call("shape_draw_line_f", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/shape_draw_line_f";
        st = JOY_OK;
        joy_api_call("text_get_width", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/text_get_width";
        st = JOY_OK;
        joy_api_call("text_get_height", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/text_get_height";
        st = JOY_OK;
        joy_api_call("jtext_cache_clear", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtext_cache_clear";
        st = JOY_OK;
        joy_api_call("jtext_cache_set_limit", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtext_cache_set_limit";
        st = JOY_OK;
        joy_api_call("jtext_cache_get_count", NULL, 1, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtext_cache_get_count";
        st = JOY_OK;
        joy_api_call("jtext_cache_sweep", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jtext_cache_sweep";
        st = JOY_OK;
        joy_api_call("animation_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/animation_destroy";
        st = JOY_OK;
        joy_api_call("animation_reset", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/animation_reset";
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
        joy_api_call("r3d_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_destroy";
        st = JOY_OK;
        joy_api_call("r3d_begin", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_begin";
        st = JOY_OK;
        joy_api_call("r3d_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_end";
        st = JOY_OK;
        joy_api_call("r3d_get_draw_calls", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_get_draw_calls";
        st = JOY_OK;
        joy_api_call("r3d_set_clear_color", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_set_clear_color";
        st = JOY_OK;
        joy_api_call("r3d_mesh_destroy", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_mesh_destroy";
        st = JOY_OK;
        joy_api_call("r3d_font_init", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_font_init";
        st = JOY_OK;
        joy_api_call("r3d_font_draw_2d", NULL, 9, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_font_draw_2d";
        st = JOY_OK;
        joy_api_call("r3d_font_cache_set_limit", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_font_cache_set_limit";
        st = JOY_OK;
        joy_api_call("r3d_font_cache_get_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_font_cache_get_count";
        st = JOY_OK;
        joy_api_call("r3d_font_cache_clear", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_font_cache_clear";
        st = JOY_OK;
        joy_api_call("r3d_font_cache_sweep", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_font_cache_sweep";
        st = JOY_OK;
        joy_api_call("r3d_set_logical_presentation", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_set_logical_presentation";
        st = JOY_OK;
        joy_api_call("r3d_set_logical_presentation_mode", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_set_logical_presentation_mode";
        st = JOY_OK;
        joy_api_call("r3d_button_draw", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_button_draw";
        st = JOY_OK;
        joy_api_call("r3d_button_draw_circle", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_button_draw_circle";
        st = JOY_OK;
        joy_api_call("r3d_button_is_hovered", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_button_is_hovered";
        st = JOY_OK;
        joy_api_call("r3d_button_is_pressed", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_button_is_pressed";
        st = JOY_OK;
        joy_api_call("r3d_button_release", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_button_release";
        st = JOY_OK;
        joy_api_call("r3d_button_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_button_destroy";
        st = JOY_OK;
        joy_api_call("r3d_label_set_text", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_label_set_text";
        st = JOY_OK;
        joy_api_call("r3d_label_draw", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_label_draw";
        st = JOY_OK;
        joy_api_call("r3d_label_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_label_destroy";
        st = JOY_OK;
        joy_api_call("r3d_input_set_text", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_set_text";
        st = JOY_OK;
        joy_api_call("r3d_input_is_hovered", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_is_hovered";
        st = JOY_OK;
        joy_api_call("r3d_input_focus", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_focus";
        st = JOY_OK;
        joy_api_call("r3d_input_defocus", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_defocus";
        st = JOY_OK;
        joy_api_call("r3d_input_is_focused", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_is_focused";
        st = JOY_OK;
        joy_api_call("r3d_input_start_text_input", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_start_text_input";
        st = JOY_OK;
        joy_api_call("r3d_input_stop_text_input", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_stop_text_input";
        st = JOY_OK;
        joy_api_call("r3d_input_insert", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_insert";
        st = JOY_OK;
        joy_api_call("r3d_input_backspace", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_backspace";
        st = JOY_OK;
        joy_api_call("r3d_input_delete", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_delete";
        st = JOY_OK;
        joy_api_call("r3d_input_cursor_left", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_cursor_left";
        st = JOY_OK;
        joy_api_call("r3d_input_cursor_right", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_cursor_right";
        st = JOY_OK;
        joy_api_call("r3d_input_cursor_home", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_cursor_home";
        st = JOY_OK;
        joy_api_call("r3d_input_cursor_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_cursor_end";
        st = JOY_OK;
        joy_api_call("r3d_input_click", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_click";
        st = JOY_OK;
        joy_api_call("r3d_input_submit", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_submit";
        st = JOY_OK;
        joy_api_call("r3d_input_draw", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_draw";
        st = JOY_OK;
        joy_api_call("r3d_input_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_input_destroy";
        st = JOY_OK;
        joy_api_call("r3d_combobox_input_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_combobox_input_destroy";
        st = JOY_OK;
        joy_api_call("r3d_combobox_input_add_item", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_combobox_input_add_item";
        st = JOY_OK;
        joy_api_call("r3d_combobox_input_clear_items", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_combobox_input_clear_items";
        st = JOY_OK;
        joy_api_call("r3d_combobox_input_set_text", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_combobox_input_set_text";
        st = JOY_OK;
        joy_api_call("r3d_combobox_input_hit_item", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_combobox_input_hit_item";
        st = JOY_OK;
        joy_api_call("r3d_combobox_input_draw", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_combobox_input_draw";
        st = JOY_OK;
        joy_api_call("r3d_combobox_input_draw_dropdown", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_combobox_input_draw_dropdown";
        st = JOY_OK;
        joy_api_call("r3d_slider_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_slider_destroy";
        st = JOY_OK;
        joy_api_call("r3d_slider_set_value", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_slider_set_value";
        st = JOY_OK;
        joy_api_call("r3d_slider_get_value", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_slider_get_value";
        st = JOY_OK;
        joy_api_call("r3d_slider_draw", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_slider_draw";
        st = JOY_OK;
        joy_api_call("r3d_slider_is_pressed", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_slider_is_pressed";
        st = JOY_OK;
        joy_api_call("r3d_slider_set_pressed", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_slider_set_pressed";
        st = JOY_OK;
        joy_api_call("r3d_slider_drag_move", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_slider_drag_move";
        st = JOY_OK;
        joy_api_call("r3d_slider_is_hovered", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_slider_is_hovered";
        st = JOY_OK;
        joy_api_call("r3d_slider_set_hovered", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_slider_set_hovered";
        st = JOY_OK;
        joy_api_call("r3d_checkbox_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_checkbox_destroy";
        st = JOY_OK;
        joy_api_call("r3d_checkbox_toggle", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_checkbox_toggle";
        st = JOY_OK;
        joy_api_call("r3d_checkbox_is_pressed", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_checkbox_is_pressed";
        st = JOY_OK;
        joy_api_call("r3d_checkbox_set_hovered", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_checkbox_set_hovered";
        st = JOY_OK;
        joy_api_call("r3d_checkbox_is_hovered", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_checkbox_is_hovered";
        st = JOY_OK;
        joy_api_call("r3d_checkbox_draw", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_checkbox_draw";
        st = JOY_OK;
        joy_api_call("r3d_draw_grid", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/r3d_draw_grid";
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
        joy_api_call("label_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/label_destroy";
        st = JOY_OK;
        joy_api_call("label_draw", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/label_draw";
        st = JOY_OK;
        joy_api_call("button_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/button_destroy";
        st = JOY_OK;
        joy_api_call("button_set_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/button_set_position";
        st = JOY_OK;
        joy_api_call("button_set_size", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/button_set_size";
        st = JOY_OK;
        joy_api_call("button_set_image", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/button_set_image";
        st = JOY_OK;
        joy_api_call("button_set_pressed_image", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/button_set_pressed_image";
        st = JOY_OK;
        joy_api_call("button_is_hovered", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/button_is_hovered";
        st = JOY_OK;
        joy_api_call("button_is_pressed", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/button_is_pressed";
        st = JOY_OK;
        joy_api_call("button_set_hovered", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/button_set_hovered";
        st = JOY_OK;
        joy_api_call("button_set_pressed", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/button_set_pressed";
        st = JOY_OK;
        joy_api_call("button_get_pressed", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/button_get_pressed";
        st = JOY_OK;
        joy_api_call("button_set_text_offset", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/button_set_text_offset";
        st = JOY_OK;
        joy_api_call("button_draw", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/button_draw";
        st = JOY_OK;
        joy_api_call("scrollview_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scrollview_destroy";
        st = JOY_OK;
        joy_api_call("scrollview_set_view_px", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/scrollview_set_view_px";
        st = JOY_OK;
        joy_api_call("scrollview_set_content", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/scrollview_set_content";
        st = JOY_OK;
        joy_api_call("scrollview_set_offset", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/scrollview_set_offset";
        st = JOY_OK;
        joy_api_call("scrollview_clamp", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scrollview_clamp";
        st = JOY_OK;
        joy_api_call("scrollview_get_offset_x", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scrollview_get_offset_x";
        st = JOY_OK;
        joy_api_call("scrollview_get_offset_y", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scrollview_get_offset_y";
        st = JOY_OK;
        joy_api_call("scrollview_set_show_bar", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/scrollview_set_show_bar";
        st = JOY_OK;
        joy_api_call("scrollview_dragging", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scrollview_dragging";
        st = JOY_OK;
        joy_api_call("scrollview_hit", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/scrollview_hit";
        st = JOY_OK;
        joy_api_call("scrollview_begin", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scrollview_begin";
        st = JOY_OK;
        joy_api_call("scrollview_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/scrollview_end";
        st = JOY_OK;
        joy_api_call("combobox_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_destroy";
        st = JOY_OK;
        joy_api_call("combobox_is_pressed", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_is_pressed";
        st = JOY_OK;
        joy_api_call("combobox_is_open", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_is_open";
        st = JOY_OK;
        joy_api_call("combobox_set_open", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_set_open";
        st = JOY_OK;
        joy_api_call("combobox_hit_item", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_hit_item";
        st = JOY_OK;
        joy_api_call("combobox_set_selected", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_set_selected";
        st = JOY_OK;
        joy_api_call("combobox_get_selected", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_get_selected";
        st = JOY_OK;
        joy_api_call("combobox_draw", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_draw";
        st = JOY_OK;
        joy_api_call("combobox_input_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_input_destroy";
        st = JOY_OK;
        joy_api_call("combobox_input_add_item", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_input_add_item";
        st = JOY_OK;
        joy_api_call("combobox_input_clear_items", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_input_clear_items";
        st = JOY_OK;
        joy_api_call("combobox_input_set_text", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_input_set_text";
        st = JOY_OK;
        joy_api_call("combobox_input_draw", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_input_draw";
        st = JOY_OK;
        joy_api_call("combobox_input_draw_dropdown", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_input_draw_dropdown";
        st = JOY_OK;
        joy_api_call("combobox_input_is_open", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_input_is_open";
        st = JOY_OK;
        joy_api_call("combobox_input_set_popup_node", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_input_set_popup_node";
        st = JOY_OK;
        joy_api_call("combobox_input_hit_item", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_input_hit_item";
        st = JOY_OK;
        joy_api_call("combobox_input_set_open", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/combobox_input_set_open";
        st = JOY_OK;
        joy_api_call("datagrid_setheaders", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/datagrid_setheaders";
        st = JOY_OK;
        joy_api_call("datagrid_setheader", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/datagrid_setheader";
        st = JOY_OK;
        joy_api_call("datagrid_setcell", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/datagrid_setcell";
        st = JOY_OK;
        joy_api_call("datagrid_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/datagrid_destroy";
        st = JOY_OK;
        joy_api_call("datagrid_draw", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/datagrid_draw";
        st = JOY_OK;
        joy_api_call("datagrid_hit_row", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/datagrid_hit_row";
        st = JOY_OK;
        joy_api_call("datagrid_scroll", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/datagrid_scroll";
        st = JOY_OK;
        joy_api_call("datagrid_set_selected_row", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/datagrid_set_selected_row";
        st = JOY_OK;
        joy_api_call("radio_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/radio_destroy";
        st = JOY_OK;
        joy_api_call("radio_is_pressed", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/radio_is_pressed";
        st = JOY_OK;
        joy_api_call("radio_set_hovered", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/radio_set_hovered";
        st = JOY_OK;
        joy_api_call("checkbox_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/checkbox_destroy";
        st = JOY_OK;
        joy_api_call("checkbox_set_r2d", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/checkbox_set_r2d";
        st = JOY_OK;
        joy_api_call("checkbox_set_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/checkbox_set_position";
        st = JOY_OK;
        joy_api_call("checkbox_toggle", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/checkbox_toggle";
        st = JOY_OK;
        joy_api_call("checkbox_is_pressed", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/checkbox_is_pressed";
        st = JOY_OK;
        joy_api_call("checkbox_set_hovered", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/checkbox_set_hovered";
        st = JOY_OK;
        joy_api_call("joystick_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joystick_destroy";
        st = JOY_OK;
        joy_api_call("joystick_set_textures", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joystick_set_textures";
        st = JOY_OK;
        joy_api_call("joystick_set_vector", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joystick_set_vector";
        st = JOY_OK;
        joy_api_call("joystick_draw", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joystick_draw";
        st = JOY_OK;
        joy_api_call("joystick_reset", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joystick_reset";
        st = JOY_OK;
        joy_api_call("joystick_set_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joystick_set_position";
        st = JOY_OK;
        joy_api_call("joystick_set_radius", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/joystick_set_radius";
        st = JOY_OK;
        joy_api_call("joystick_is_dragging", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joystick_is_dragging";
        st = JOY_OK;
        joy_api_call("joystick_touch_down", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joystick_touch_down";
        st = JOY_OK;
        joy_api_call("joystick_touch_move", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/joystick_touch_move";
        st = JOY_OK;
        joy_api_call("joystick_touch_up", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joystick_touch_up";
        st = JOY_OK;
        joy_api_call("joystick_get_magnitude", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/joystick_get_magnitude";
        st = JOY_OK;
        joy_api_call("inputbox_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_destroy";
        st = JOY_OK;
        joy_api_call("inputbox_set_text", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_set_text";
        st = JOY_OK;
        joy_api_call("inputbox_draw", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_draw";
        st = JOY_OK;
        joy_api_call("inputbox_is_pressed", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_is_pressed";
        st = JOY_OK;
        joy_api_call("inputbox_is_hovered", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_is_hovered";
        st = JOY_OK;
        joy_api_call("inputbox_focus", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_focus";
        st = JOY_OK;
        joy_api_call("inputbox_defocus", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_defocus";
        st = JOY_OK;
        joy_api_call("inputbox_blur", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_blur";
        st = JOY_OK;
        joy_api_call("inputbox_insert", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_insert";
        st = JOY_OK;
        joy_api_call("inputbox_backspace", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_backspace";
        st = JOY_OK;
        joy_api_call("inputbox_delete", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_delete";
        st = JOY_OK;
        joy_api_call("inputbox_cursor_left", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_cursor_left";
        st = JOY_OK;
        joy_api_call("inputbox_cursor_right", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_cursor_right";
        st = JOY_OK;
        joy_api_call("inputbox_cursor_home", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_cursor_home";
        st = JOY_OK;
        joy_api_call("inputbox_cursor_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_cursor_end";
        st = JOY_OK;
        joy_api_call("inputbox_select_left", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_select_left";
        st = JOY_OK;
        joy_api_call("inputbox_select_right", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_select_right";
        st = JOY_OK;
        joy_api_call("inputbox_select_home", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_select_home";
        st = JOY_OK;
        joy_api_call("inputbox_select_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_select_end";
        st = JOY_OK;
        joy_api_call("inputbox_select_all", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_select_all";
        st = JOY_OK;
        joy_api_call("inputbox_paste", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_paste";
        st = JOY_OK;
        joy_api_call("inputbox_copy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_copy";
        st = JOY_OK;
        joy_api_call("inputbox_cut", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_cut";
        st = JOY_OK;
        joy_api_call("inputbox_is_focused", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/inputbox_is_focused";
        st = JOY_OK;
        joy_api_call("slider_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/slider_destroy";
        st = JOY_OK;
        joy_api_call("slider_set_value", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/slider_set_value";
        st = JOY_OK;
        joy_api_call("slider_get_value", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/slider_get_value";
        st = JOY_OK;
        joy_api_call("slider_set_rect", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/slider_set_rect";
        st = JOY_OK;
        joy_api_call("slider_set_range", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/slider_set_range";
        st = JOY_OK;
        joy_api_call("slider_draw", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/slider_draw";
        st = JOY_OK;
        joy_api_call("slider_is_pressed", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/slider_is_pressed";
        st = JOY_OK;
        joy_api_call("slider_set_pressed", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/slider_set_pressed";
        st = JOY_OK;
        joy_api_call("slider_drag_move", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/slider_drag_move";
        st = JOY_OK;
        joy_api_call("slider_is_hovered", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/slider_is_hovered";
        st = JOY_OK;
        joy_api_call("slider_set_hovered", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/slider_set_hovered";
        st = JOY_OK;
        joy_api_call("popup_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/popup_destroy";
        st = JOY_OK;
        joy_api_call("popup_set_title", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/popup_set_title";
        st = JOY_OK;
        joy_api_call("popup_set_content", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/popup_set_content";
        st = JOY_OK;
        joy_api_call("popup_set_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/popup_set_position";
        st = JOY_OK;
        joy_api_call("popup_set_visible", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/popup_set_visible";
        st = JOY_OK;
        joy_api_call("popup_set_closeable", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/popup_set_closeable";
        st = JOY_OK;
        joy_api_call("popup_draw", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/popup_draw";
        st = JOY_OK;
        joy_api_call("popup_is_close_clicked", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/popup_is_close_clicked";
        st = JOY_OK;
        joy_api_call("popup_is_title_clicked", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/popup_is_title_clicked";
        st = JOY_OK;
        joy_api_call("popup_drag_move", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/popup_drag_move";
        st = JOY_OK;
        joy_api_call("popup_is_inside", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/popup_is_inside";
        st = JOY_OK;
        joy_api_call("tooltip_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/tooltip_destroy";
        st = JOY_OK;
        joy_api_call("tooltip_set_text", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/tooltip_set_text";
        st = JOY_OK;
        joy_api_call("tooltip_set_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/tooltip_set_position";
        st = JOY_OK;
        joy_api_call("tooltip_set_visible", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/tooltip_set_visible";
        st = JOY_OK;
        joy_api_call("tooltip_draw", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/tooltip_draw";
        st = JOY_OK;
        joy_api_call("jui_ctx_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_ctx_destroy";
        st = JOY_OK;
        joy_api_call("jui_begin", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_begin";
        st = JOY_OK;
        joy_api_call("jui_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_end";
        st = JOY_OK;
        joy_api_call("jui_input_motion", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_input_motion";
        st = JOY_OK;
        joy_api_call("jui_input_down", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_input_down";
        st = JOY_OK;
        joy_api_call("jui_input_up", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_input_up";
        st = JOY_OK;
        joy_api_call("jui_input_wheel", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_input_wheel";
        st = JOY_OK;
        joy_api_call("jui_input_key", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_input_key";
        st = JOY_OK;
        joy_api_call("jui_is_down", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_is_down";
        st = JOY_OK;
        joy_api_call("jui_is_pressed", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_is_pressed";
        st = JOY_OK;
        joy_api_call("jui_is_released", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_is_released";
        st = JOY_OK;
        joy_api_call("jui_take_wheel", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_take_wheel";
        st = JOY_OK;
        joy_api_call("jui_is_shift", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_is_shift";
        st = JOY_OK;
        joy_api_call("jui_is_ctrl", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_is_ctrl";
        st = JOY_OK;
        joy_api_call("jui_pop_clip", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_pop_clip";
        st = JOY_OK;
        joy_api_call("jui_reset_clip", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_reset_clip";
        st = JOY_OK;
        joy_api_call("jui_panel_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_panel_end";
        st = JOY_OK;
        joy_api_call("jui_scroll_end", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_scroll_end";
        st = JOY_OK;
        joy_api_call("jui_scroll_set_offset", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/jui_scroll_set_offset";
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
        joy_api_call("kcpserver_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/kcpserver_destroy";
        st = JOY_OK;
        joy_api_call("kcpserver_send", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/kcpserver_send";
        st = JOY_OK;
        joy_api_call("kcpserver_broadcast", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/kcpserver_broadcast";
        st = JOY_OK;
        joy_api_call("kcpserver_offline", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/kcpserver_offline";
        st = JOY_OK;
        joy_api_call("kcpserver_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/kcpserver_update";
        st = JOY_OK;
        joy_api_call("kcpserver_connection_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/kcpserver_connection_count";
        st = JOY_OK;
        joy_api_call("kcpserver_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/kcpserver_poll_message";
        st = JOY_OK;
        joy_api_call("kcpclient_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/kcpclient_destroy";
        st = JOY_OK;
        joy_api_call("kcpclient_send", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/kcpclient_send";
        st = JOY_OK;
        joy_api_call("kcpclient_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/kcpclient_update";
        st = JOY_OK;
        joy_api_call("kcpclient_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/kcpclient_poll_message";
        st = JOY_OK;
        joy_api_call("tcpserver_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/tcpserver_destroy";
        st = JOY_OK;
        joy_api_call("tcpserver_send", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/tcpserver_send";
        st = JOY_OK;
        joy_api_call("tcpserver_broadcast", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/tcpserver_broadcast";
        st = JOY_OK;
        joy_api_call("tcpserver_offline", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/tcpserver_offline";
        st = JOY_OK;
        joy_api_call("tcpserver_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/tcpserver_update";
        st = JOY_OK;
        joy_api_call("tcpserver_connection_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/tcpserver_connection_count";
        st = JOY_OK;
        joy_api_call("tcpserver_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/tcpserver_poll_message";
        st = JOY_OK;
        joy_api_call("tcpclient_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/tcpclient_destroy";
        st = JOY_OK;
        joy_api_call("tcpclient_send", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/tcpclient_send";
        st = JOY_OK;
        joy_api_call("tcpclient_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/tcpclient_update";
        st = JOY_OK;
        joy_api_call("tcpclient_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/tcpclient_poll_message";
        st = JOY_OK;
        joy_api_call("wsnetserver_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/wsnetserver_destroy";
        st = JOY_OK;
        joy_api_call("wsnetserver_send", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/wsnetserver_send";
        st = JOY_OK;
        joy_api_call("wsnetserver_broadcast", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/wsnetserver_broadcast";
        st = JOY_OK;
        joy_api_call("wsnetserver_offline", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/wsnetserver_offline";
        st = JOY_OK;
        joy_api_call("wsnetserver_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/wsnetserver_update";
        st = JOY_OK;
        joy_api_call("wsnetserver_connection_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/wsnetserver_connection_count";
        st = JOY_OK;
        joy_api_call("wsnetserver_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/wsnetserver_poll_message";
        st = JOY_OK;
        joy_api_call("wsnetclient_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/wsnetclient_destroy";
        st = JOY_OK;
        joy_api_call("wsnetclient_send", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/wsnetclient_send";
        st = JOY_OK;
        joy_api_call("wsnetclient_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/wsnetclient_update";
        st = JOY_OK;
        joy_api_call("wsnetclient_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/wsnetclient_poll_message";
        st = JOY_OK;
        joy_api_call("netclient_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/netclient_destroy";
        st = JOY_OK;
        joy_api_call("netclient_send", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/netclient_send";
        st = JOY_OK;
        joy_api_call("netclient_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/netclient_update";
        st = JOY_OK;
        joy_api_call("netclient_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/netclient_poll_message";
        st = JOY_OK;
        joy_api_call("netserver_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/netserver_destroy";
        st = JOY_OK;
        joy_api_call("netserver_send", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/netserver_send";
        st = JOY_OK;
        joy_api_call("netserver_broadcast", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/netserver_broadcast";
        st = JOY_OK;
        joy_api_call("netserver_offline", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/netserver_offline";
        st = JOY_OK;
        joy_api_call("netserver_update", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/netserver_update";
        st = JOY_OK;
        joy_api_call("netserver_connection_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/netserver_connection_count";
        st = JOY_OK;
        joy_api_call("netserver_poll_message", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/netserver_poll_message";
        st = JOY_OK;
        joy_api_call("netmessage_free_data", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/netmessage_free_data";
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
        joy_api_call("jparticle_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jparticle_destroy";
        st = JOY_OK;
        joy_api_call("jparticle_set_position", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jparticle_set_position";
        st = JOY_OK;
        joy_api_call("jparticle_emit", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jparticle_emit";
        st = JOY_OK;
        joy_api_call("jparticle_clear", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jparticle_clear";
        st = JOY_OK;
        joy_api_call("jparticle_update", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jparticle_update";
        st = JOY_OK;
        joy_api_call("jparticle_render", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jparticle_render";
        st = JOY_OK;
        joy_api_call("jparticle_count", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jparticle_count";
        st = JOY_OK;
        joy_api_call("jparticle_is_active", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jparticle_is_active";
        st = JOY_OK;
        joy_api_call("jlight_sys_destroy", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jlight_sys_destroy";
        st = JOY_OK;
        joy_api_call("jlight_sys_resize", NULL, 4, &st);
        if (st != JOY_ERR_ARGC) return "registry/jlight_sys_resize";
        st = JOY_OK;
        joy_api_call("jlight_remove", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jlight_remove";
        st = JOY_OK;
        joy_api_call("jlight_clear", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jlight_clear";
        st = JOY_OK;
        joy_api_call("jlight_add_occluder", NULL, 6, &st);
        if (st != JOY_ERR_ARGC) return "registry/jlight_add_occluder";
        st = JOY_OK;
        joy_api_call("jlight_clear_occluders", NULL, 2, &st);
        if (st != JOY_ERR_ARGC) return "registry/jlight_clear_occluders";
        st = JOY_OK;
        joy_api_call("jlight_set_ambient", NULL, 5, &st);
        if (st != JOY_ERR_ARGC) return "registry/jlight_set_ambient";
        st = JOY_OK;
        joy_api_call("jlight_render", NULL, 3, &st);
        if (st != JOY_ERR_ARGC) return "registry/jlight_render";
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

        /* L0：纯标量往返 */
        st = JOY_OK;
        r = t_init_env(NULL, 0, &st);
        if (st != JOY_OK || r.kind != JOY_V_BOOL) return "L0/init_env";
        st = JOY_OK;
        t_init_env(args, 1, &st);
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
