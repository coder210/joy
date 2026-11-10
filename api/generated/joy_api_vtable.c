/* ============================================================
 * 自动生成，勿手改：node api/tools/gen_bindings.js --emit
 * 强签名 JoyApi vtable（C++ 后端 / 第 4 个后端）。
 * 覆盖 joy.idl 里 status:on 的 L0/L1 与可展开的 L2；参数类型逐字取自 IDL。
 * ============================================================ */
#include <string.h>
#include <joy/joy.h>
#include "generated/joy_api_vtable.h"
#include "runtime/joy_value.h"
#include "runtime/joy_thunk.h"

/* 宿主侧实现：DLL 侧只 include .h，不链接本文件。 */

static LcgNum shim_joy_core_lcg_random_create(void* _jv_ctx, uint64_t seed)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        LcgNum _jv_out;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_u64(seed);
        _jv_r = joy_api_call("joy_core_lcg_random_create", _jv_args, 1, &_jv_st);
        _jv_out.h = (_jv_st == JOY_OK && _jv_r.kind == JOY_V_HANDLE) ? _jv_r.as.handle : 0;
        return _jv_out;
}

static void shim_joy_core_lcg_random_destroy(void* _jv_ctx, LcgNum lcg)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(lcg.h);
        _jv_r = joy_api_call("joy_core_lcg_random_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static uint32_t shim_joy_core_lcg_random_next(void* _jv_ctx, LcgNum lcg)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(lcg.h);
        _jv_r = joy_api_call("joy_core_lcg_random_next", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U32) return (uint32_t)0;
        return (uint32_t) _jv_r.as.u32;
}

static void shim_joy_core_lcg_random_set_state(void* _jv_ctx, LcgNum lcg, uint64_t state)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(lcg.h);
        _jv_args[1] = joy_v_u64(state);
        _jv_r = joy_api_call("joy_core_lcg_random_set_state", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jpm_config_add_spawn(void* _jv_ctx, JpmConfig cfg, int cx10, int cz10, int radius10)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(cfg.h);
        _jv_args[1] = joy_v_i32(cx10);
        _jv_args[2] = joy_v_i32(cz10);
        _jv_args[3] = joy_v_i32(radius10);
        _jv_r = joy_api_call("jpm_config_add_spawn", _jv_args, 4, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static uint32_t shim_jpm_wave_seed(void* _jv_ctx, uint32_t base_seed, int wave)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_u32(base_seed);
        _jv_args[1] = joy_v_i32(wave);
        _jv_r = joy_api_call("jpm_wave_seed", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U32) return (uint32_t)0;
        return (uint32_t) _jv_r.as.u32;
}

static unsigned long shim_joy_core_sdbm_hash(void* _jv_ctx, const char* str, int size)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_str(str);
        _jv_args[1] = joy_v_i32(size);
        _jv_r = joy_api_call("joy_core_sdbm_hash", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (unsigned long)0;
        return (unsigned long) _jv_r.as.u64;
}

static unsigned long shim_joy_core_fnv1a_hash(void* _jv_ctx, const char* str, int size)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_str(str);
        _jv_args[1] = joy_v_i32(size);
        _jv_r = joy_api_call("joy_core_fnv1a_hash", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (unsigned long)0;
        return (unsigned long) _jv_r.as.u64;
}

static unsigned long shim_joy_core_jenkins_hash(void* _jv_ctx, const char* str, int size)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_str(str);
        _jv_args[1] = joy_v_i32(size);
        _jv_r = joy_api_call("joy_core_jenkins_hash", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (unsigned long)0;
        return (unsigned long) _jv_r.as.u64;
}

static unsigned long shim_joy_core_murmur3_hash(void* _jv_ctx, const char* str)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_str(str);
        _jv_r = joy_api_call("joy_core_murmur3_hash", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (unsigned long)0;
        return (unsigned long) _jv_r.as.u64;
}

static unsigned long shim_joy_core_kr_hash(void* _jv_ctx, const char* str, int size)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_str(str);
        _jv_args[1] = joy_v_i32(size);
        _jv_r = joy_api_call("joy_core_kr_hash", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (unsigned long)0;
        return (unsigned long) _jv_r.as.u64;
}

static void shim_joy_scene_jtimer_destroy(void* _jv_ctx, Jtimer timer)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(timer.h);
        _jv_r = joy_api_call("joy_scene_jtimer_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joy_scene_jtimer_trigger(void* _jv_ctx, Jtimer timer)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(timer.h);
        _jv_r = joy_api_call("joy_scene_jtimer_trigger", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joy_scene_jtimer_set_interval(void* _jv_ctx, Jtimer timer, int interval)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(timer.h);
        _jv_args[1] = joy_v_i32(interval);
        _jv_r = joy_api_call("joy_scene_jtimer_set_interval", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joy_scene_jtimer_set_active(void* _jv_ctx, Jtimer timer, bool active)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(timer.h);
        _jv_args[1] = joy_v_bool(active);
        _jv_r = joy_api_call("joy_scene_jtimer_set_active", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joy_scene_jtimer_reset(void* _jv_ctx, Jtimer timer)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(timer.h);
        _jv_r = joy_api_call("joy_scene_jtimer_reset", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static uint64_t shim_joy_scene_jtimer_get_elapsed(void* _jv_ctx, Jtimer timer)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(timer.h);
        _jv_r = joy_api_call("joy_scene_jtimer_get_elapsed", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (uint64_t)0;
        return (uint64_t) _jv_r.as.u64;
}

static bool shim_joy_core_init_env(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("joy_core_init_env", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_joy_core_quit_env(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("joy_core_quit_env", _jv_args, 0, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static uint64_t shim_joy_core_get_ticks(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("joy_core_get_ticks", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (uint64_t)0;
        return (uint64_t) _jv_r.as.u64;
}

static Jwindow shim_joy_core_create_window(void* _jv_ctx, const char* title, int w, int h)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        Jwindow _jv_out;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_str(title);
        _jv_args[1] = joy_v_i32(w);
        _jv_args[2] = joy_v_i32(h);
        _jv_r = joy_api_call("joy_core_create_window", _jv_args, 3, &_jv_st);
        _jv_out.h = (_jv_st == JOY_OK && _jv_r.kind == JOY_V_HANDLE) ? _jv_r.as.handle : 0;
        return _jv_out;
}

static void shim_joy_core_destroy_window(void* _jv_ctx, Jwindow win)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(win.h);
        _jv_r = joy_api_call("joy_core_destroy_window", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joy_core_window_set_fullscreen(void* _jv_ctx, Jwindow win, bool fullscreen)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(win.h);
        _jv_args[1] = joy_v_bool(fullscreen);
        _jv_r = joy_api_call("joy_core_window_set_fullscreen", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_joy_core_window_get_fullscreen(void* _jv_ctx, Jwindow win)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(win.h);
        _jv_r = joy_api_call("joy_core_window_get_fullscreen", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_joy_core_window_set_icon(void* _jv_ctx, Jwindow win, const char* png_path)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(win.h);
        _jv_args[1] = joy_v_str(png_path);
        _jv_r = joy_api_call("joy_core_window_set_icon", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_joy_core_push_user_event(void* _jv_ctx, int type, int code)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i32(type);
        _jv_args[1] = joy_v_i32(code);
        _jv_r = joy_api_call("joy_core_push_user_event", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_joy_core_push_quit_event(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("joy_core_push_quit_event", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_joy_core_log_title(void* _jv_ctx, int type)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i32(type);
        _jv_r = joy_api_call("joy_core_log_title", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joy_core_env_set(void* _jv_ctx, Env env, const char* key, const char* value)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(env.h);
        _jv_args[1] = joy_v_str(key);
        _jv_args[2] = joy_v_str(value);
        _jv_r = joy_api_call("joy_core_env_set", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joy_core_env_destroy(void* _jv_ctx, Env env)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(env.h);
        _jv_r = joy_api_call("joy_core_env_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joy_core_simple_fps_destory(void* _jv_ctx, SimpleFpsCounter fps)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(fps.h);
        _jv_r = joy_api_call("joy_core_simple_fps_destory", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joy_core_simple_fps_update(void* _jv_ctx, SimpleFpsCounter fps)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(fps.h);
        _jv_r = joy_api_call("joy_core_simple_fps_update", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_joy_core_simple_fps_value(void* _jv_ctx, SimpleFpsCounter fps)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(fps.h);
        _jv_r = joy_api_call("joy_core_simple_fps_value", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_joy_scene_game_timer_init(void* _jv_ctx, GameTimer clock)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(clock.h);
        _jv_r = joy_api_call("joy_scene_game_timer_init", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joy_scene_game_timer_update(void* _jv_ctx, GameTimer clock)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(clock.h);
        _jv_r = joy_api_call("joy_scene_game_timer_update", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static float shim_joy_scene_game_timer_get_delta_time(void* _jv_ctx, GameTimer clock)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(clock.h);
        _jv_r = joy_api_call("joy_scene_game_timer_get_delta_time", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_joy_scene_game_timer_get_unscaled_delta_time(void* _jv_ctx, GameTimer clock)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(clock.h);
        _jv_r = joy_api_call("joy_scene_game_timer_get_unscaled_delta_time", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static void shim_joy_scene_game_timer_set_time_scale(void* _jv_ctx, GameTimer clock, float scale)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(clock.h);
        _jv_args[1] = joy_v_f64(scale);
        _jv_r = joy_api_call("joy_scene_game_timer_set_time_scale", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static float shim_joy_scene_game_timer_get_time_scale(void* _jv_ctx, GameTimer clock)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(clock.h);
        _jv_r = joy_api_call("joy_scene_game_timer_get_time_scale", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static void shim_joy_scene_game_timer_set_target_fps(void* _jv_ctx, GameTimer clock, int fps)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(clock.h);
        _jv_args[1] = joy_v_i32(fps);
        _jv_r = joy_api_call("joy_scene_game_timer_set_target_fps", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_joy_scene_game_timer_get_target_fps(void* _jv_ctx, GameTimer clock)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(clock.h);
        _jv_r = joy_api_call("joy_scene_game_timer_get_target_fps", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_joy_core_set_clipboard_text(void* _jv_ctx, const char* text)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_str(text);
        _jv_r = joy_api_call("joy_core_set_clipboard_text", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_sys_init_netenv(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("sys_init_netenv", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_sys_release_netenv(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("sys_release_netenv", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static int shim_sys_anyaddr(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("sys_anyaddr", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static bool shim_sys_closesocket(void* _jv_ctx, int64_t sockfd)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(sockfd);
        _jv_r = joy_api_call("sys_closesocket", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static int shim_sys_set_sock_sndtimeo(void* _jv_ctx, int64_t sockfd, int ms)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(sockfd);
        _jv_args[1] = joy_v_i32(ms);
        _jv_r = joy_api_call("sys_set_sock_sndtimeo", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int shim_sys_set_sock_rcvtimeo(void* _jv_ctx, int64_t sockfd, int ms)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(sockfd);
        _jv_args[1] = joy_v_i32(ms);
        _jv_r = joy_api_call("sys_set_sock_rcvtimeo", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int shim_sys_set_sock_accpettimeo(void* _jv_ctx, int64_t sockfd, int ms)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(sockfd);
        _jv_args[1] = joy_v_i32(ms);
        _jv_r = joy_api_call("sys_set_sock_accpettimeo", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int shim_sys_set_sock_broadcast(void* _jv_ctx, int64_t sockfd)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(sockfd);
        _jv_r = joy_api_call("sys_set_sock_broadcast", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int shim_sys_set_sock_reuseaddr(void* _jv_ctx, int64_t sockfd)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(sockfd);
        _jv_r = joy_api_call("sys_set_sock_reuseaddr", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int64_t shim_sys_tcp(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("sys_tcp", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_sys_udp(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("sys_udp", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static bool shim_sys_bind(void* _jv_ctx, int64_t sockfd, const char* ip, int port)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(sockfd);
        _jv_args[1] = joy_v_str(ip);
        _jv_args[2] = joy_v_i32(port);
        _jv_r = joy_api_call("sys_bind", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_sys_connect(void* _jv_ctx, int64_t sockfd, const char* ip, int port)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(sockfd);
        _jv_args[1] = joy_v_str(ip);
        _jv_args[2] = joy_v_i32(port);
        _jv_r = joy_api_call("sys_connect", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_sys_listen(void* _jv_ctx, int64_t sockfd)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(sockfd);
        _jv_r = joy_api_call("sys_listen", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static int shim_sys_send(void* _jv_ctx, int64_t sockfd, const char* buf, int len)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(sockfd);
        _jv_args[1] = joy_v_str(buf);
        _jv_args[2] = joy_v_i32(len);
        _jv_r = joy_api_call("sys_send", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int shim_sys_sendto(void* _jv_ctx, int64_t sockfd, const char* buf, int len, const char* ip, int port)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(sockfd);
        _jv_args[1] = joy_v_str(buf);
        _jv_args[2] = joy_v_i32(len);
        _jv_args[3] = joy_v_str(ip);
        _jv_args[4] = joy_v_i32(port);
        _jv_r = joy_api_call("sys_sendto", _jv_args, 5, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int64_t shim_sys_current_time(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("sys_current_time", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static void shim_sys_sleep(void* _jv_ctx, int ms)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i32(ms);
        _jv_r = joy_api_call("sys_sleep", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_sys_install_crash_handlers(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("sys_install_crash_handlers", _jv_args, 0, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_iocp_server_destroy(void* _jv_ctx, IocpServer server)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(server.h);
        _jv_r = joy_api_call("iocp_server_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_iocp_process_events(void* _jv_ctx, IocpServer server, int timeout_ms)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(server.h);
        _jv_args[1] = joy_v_i32(timeout_ms);
        _jv_r = joy_api_call("iocp_process_events", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static Font shim_font_create(void* _jv_ctx, const char* filename, int fontsize)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        Font _jv_out;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_str(filename);
        _jv_args[1] = joy_v_i32(fontsize);
        _jv_r = joy_api_call("font_create", _jv_args, 2, &_jv_st);
        _jv_out.h = (_jv_st == JOY_OK && _jv_r.kind == JOY_V_HANDLE) ? _jv_r.as.handle : 0;
        return _jv_out;
}

static void shim_font_destroy(void* _jv_ctx, Font font)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(font.h);
        _jv_r = joy_api_call("font_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static float shim_font_get_scale(void* _jv_ctx, Font font)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(font.h);
        _jv_r = joy_api_call("font_get_scale", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static int shim_font_get_size(void* _jv_ctx, Font font)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(font.h);
        _jv_r = joy_api_call("font_get_size", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int shim_font_get_ascent(void* _jv_ctx, Font font)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(font.h);
        _jv_r = joy_api_call("font_get_ascent", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static Jr2d shim_r2d_create(void* _jv_ctx, Jwindow win)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        Jr2d _jv_out;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(win.h);
        _jv_r = joy_api_call("r2d_create", _jv_args, 1, &_jv_st);
        _jv_out.h = (_jv_st == JOY_OK && _jv_r.kind == JOY_V_HANDLE) ? _jv_r.as.handle : 0;
        return _jv_out;
}

static void shim_r2d_destroy(void* _jv_ctx, Jr2d r2d)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_r = joy_api_call("r2d_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r2d_set_color(void* _jv_ctx, Jr2d r2d, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_u32(r);
        _jv_args[2] = joy_v_u32(g);
        _jv_args[3] = joy_v_u32(b);
        _jv_args[4] = joy_v_u32(a);
        _jv_r = joy_api_call("r2d_set_color", _jv_args, 5, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_r2d_begin(void* _jv_ctx, Jr2d r2d)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_r = joy_api_call("r2d_begin", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r2d_end(void* _jv_ctx, Jr2d r2d)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_r = joy_api_call("r2d_end", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r2d_set_logical_presentation(void* _jv_ctx, Jr2d r2d, int w, int h)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_i32(w);
        _jv_args[2] = joy_v_i32(h);
        _jv_r = joy_api_call("r2d_set_logical_presentation", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_r2d_set_logical_presentation_mode(void* _jv_ctx, Jr2d r2d, int w, int h, int mode)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_i32(w);
        _jv_args[2] = joy_v_i32(h);
        _jv_args[3] = joy_v_i32(mode);
        _jv_r = joy_api_call("r2d_set_logical_presentation_mode", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_r2d_set_vsync(void* _jv_ctx, Jr2d r2d, int vsync)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_i32(vsync);
        _jv_r = joy_api_call("r2d_set_vsync", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_r2d_set_scale(void* _jv_ctx, Jr2d r2d, float sx, float sy)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_f64(sx);
        _jv_args[2] = joy_v_f64(sy);
        _jv_r = joy_api_call("r2d_set_scale", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_r2d_set_colorf(void* _jv_ctx, Jr2d r2d, float r, float g, float b, float a)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_f64(r);
        _jv_args[2] = joy_v_f64(g);
        _jv_args[3] = joy_v_f64(b);
        _jv_args[4] = joy_v_f64(a);
        _jv_r = joy_api_call("r2d_set_colorf", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r2d_set_blend_mode(void* _jv_ctx, Jr2d r2d, int mode)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_i32(mode);
        _jv_r = joy_api_call("r2d_set_blend_mode", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_r2d_draw_point(void* _jv_ctx, Jr2d r2d, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("r2d_draw_point", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_r2d_set_clip_rect(void* _jv_ctx, Jr2d r2d, float rect_x, float rect_y, float rect_w, float rect_h)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_f64(rect_x);
        _jv_args[2] = joy_v_f64(rect_y);
        _jv_args[3] = joy_v_f64(rect_w);
        _jv_args[4] = joy_v_f64(rect_h);
        _jv_r = joy_api_call("r2d_set_clip_rect", _jv_args, 5, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_r2d_clear_clip(void* _jv_ctx, Jr2d r2d)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_r = joy_api_call("r2d_clear_clip", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r2d_push_clip_rect(void* _jv_ctx, Jr2d r2d, float rect_x, float rect_y, float rect_w, float rect_h)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_f64(rect_x);
        _jv_args[2] = joy_v_f64(rect_y);
        _jv_args[3] = joy_v_f64(rect_w);
        _jv_args[4] = joy_v_f64(rect_h);
        _jv_r = joy_api_call("r2d_push_clip_rect", _jv_args, 5, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_r2d_pop_clip_rect(void* _jv_ctx, Jr2d r2d)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_r = joy_api_call("r2d_pop_clip_rect", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r2d_reset_clip_stack(void* _jv_ctx, Jr2d r2d)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_r = joy_api_call("r2d_reset_clip_stack", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r2d_point_in_clip(void* _jv_ctx, Jr2d r2d, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("r2d_point_in_clip", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_shape_draw_line(void* _jv_ctx, Jr2d r2d, float x1, float y1, float x2, float y2)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_f64(x1);
        _jv_args[2] = joy_v_f64(y1);
        _jv_args[3] = joy_v_f64(x2);
        _jv_args[4] = joy_v_f64(y2);
        _jv_r = joy_api_call("shape_draw_line", _jv_args, 5, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_shape_draw_rectangle(void* _jv_ctx, Jr2d r2d, const char* mode, float rect_x, float rect_y, float rect_w, float rect_h)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_str(mode);
        _jv_args[2] = joy_v_f64(rect_x);
        _jv_args[3] = joy_v_f64(rect_y);
        _jv_args[4] = joy_v_f64(rect_w);
        _jv_args[5] = joy_v_f64(rect_h);
        _jv_r = joy_api_call("shape_draw_rectangle", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static Image shim_image_create(void* _jv_ctx, Jr2d r2d, const char* filename)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        Image _jv_out;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_str(filename);
        _jv_r = joy_api_call("image_create", _jv_args, 2, &_jv_st);
        _jv_out.h = (_jv_st == JOY_OK && _jv_r.kind == JOY_V_HANDLE) ? _jv_r.as.handle : 0;
        return _jv_out;
}

static void shim_image_destroy(void* _jv_ctx, Image image)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(image.h);
        _jv_r = joy_api_call("image_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_image_set_alpha(void* _jv_ctx, Image image, uint8_t alpha)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(image.h);
        _jv_args[1] = joy_v_u32(alpha);
        _jv_r = joy_api_call("image_set_alpha", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_image_set_blend_mode(void* _jv_ctx, Image image, int mode)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(image.h);
        _jv_args[1] = joy_v_i32(mode);
        _jv_r = joy_api_call("image_set_blend_mode", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_image_set_scale_mode(void* _jv_ctx, Image image, int nearest)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(image.h);
        _jv_args[1] = joy_v_i32(nearest);
        _jv_r = joy_api_call("image_set_scale_mode", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_text_updatex(void* _jv_ctx, Jtext text, Font font, const char* str, int len, uint8_t color_r, uint8_t color_g, uint8_t color_b, uint8_t color_a)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(text.h);
        _jv_args[1] = joy_v_handle(font.h);
        _jv_args[2] = joy_v_str(str);
        _jv_args[3] = joy_v_i32(len);
        _jv_args[4] = joy_v_u32(color_r);
        _jv_args[5] = joy_v_u32(color_g);
        _jv_args[6] = joy_v_u32(color_b);
        _jv_args[7] = joy_v_u32(color_a);
        _jv_r = joy_api_call("text_updatex", _jv_args, 8, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_text_destroy(void* _jv_ctx, Jtext jtext_tex)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(jtext_tex.h);
        _jv_r = joy_api_call("text_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jtext_print(void* _jv_ctx, Jr2d r2d, Jtext jtext_tex, float x, float y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_handle(jtext_tex.h);
        _jv_args[2] = joy_v_f64(x);
        _jv_args[3] = joy_v_f64(y);
        _jv_r = joy_api_call("jtext_print", _jv_args, 4, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jtext_printx(void* _jv_ctx, Jr2d r2d, Font font, const char* str, int len, uint8_t color_r, uint8_t color_g, uint8_t color_b, uint8_t color_a, float x, float y)
{
        joy_value_t _jv_args[10];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_handle(font.h);
        _jv_args[2] = joy_v_str(str);
        _jv_args[3] = joy_v_i32(len);
        _jv_args[4] = joy_v_u32(color_r);
        _jv_args[5] = joy_v_u32(color_g);
        _jv_args[6] = joy_v_u32(color_b);
        _jv_args[7] = joy_v_u32(color_a);
        _jv_args[8] = joy_v_f64(x);
        _jv_args[9] = joy_v_f64(y);
        _jv_r = joy_api_call("jtext_printx", _jv_args, 10, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r2d_draw_point_f(void* _jv_ctx, Jr2d r2d, float fx, float fy)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_f64(fx);
        _jv_args[2] = joy_v_f64(fy);
        _jv_r = joy_api_call("r2d_draw_point_f", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_shape_draw_rectangle_f(void* _jv_ctx, Jr2d r2d, const char* mode, float fx, float fy, float fw, float fh)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_str(mode);
        _jv_args[2] = joy_v_f64(fx);
        _jv_args[3] = joy_v_f64(fy);
        _jv_args[4] = joy_v_f64(fw);
        _jv_args[5] = joy_v_f64(fh);
        _jv_r = joy_api_call("shape_draw_rectangle_f", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_shape_draw_line_f(void* _jv_ctx, Jr2d r2d, float fx1, float fy1, float fx2, float fy2)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_f64(fx1);
        _jv_args[2] = joy_v_f64(fy1);
        _jv_args[3] = joy_v_f64(fx2);
        _jv_args[4] = joy_v_f64(fy2);
        _jv_r = joy_api_call("shape_draw_line_f", _jv_args, 5, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_jtext_printf(void* _jv_ctx, Jr2d r2d, Font font, const char* str, int len, uint8_t color_r, uint8_t color_g, uint8_t color_b, uint8_t color_a, float fx, float fy)
{
        joy_value_t _jv_args[10];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_handle(font.h);
        _jv_args[2] = joy_v_str(str);
        _jv_args[3] = joy_v_i32(len);
        _jv_args[4] = joy_v_u32(color_r);
        _jv_args[5] = joy_v_u32(color_g);
        _jv_args[6] = joy_v_u32(color_b);
        _jv_args[7] = joy_v_u32(color_a);
        _jv_args[8] = joy_v_f64(fx);
        _jv_args[9] = joy_v_f64(fy);
        _jv_r = joy_api_call("jtext_printf", _jv_args, 10, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jtext_printf_scaled(void* _jv_ctx, Jr2d r2d, Font font, const char* str, int len, uint8_t color_r, uint8_t color_g, uint8_t color_b, uint8_t color_a, float fx, float fy, float scale)
{
        joy_value_t _jv_args[11];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_handle(font.h);
        _jv_args[2] = joy_v_str(str);
        _jv_args[3] = joy_v_i32(len);
        _jv_args[4] = joy_v_u32(color_r);
        _jv_args[5] = joy_v_u32(color_g);
        _jv_args[6] = joy_v_u32(color_b);
        _jv_args[7] = joy_v_u32(color_a);
        _jv_args[8] = joy_v_f64(fx);
        _jv_args[9] = joy_v_f64(fy);
        _jv_args[10] = joy_v_f64(scale);
        _jv_r = joy_api_call("jtext_printf_scaled", _jv_args, 11, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_text_get_width(void* _jv_ctx, Jtext jtext_tex)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(jtext_tex.h);
        _jv_r = joy_api_call("text_get_width", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int shim_text_get_height(void* _jv_ctx, Jtext jtext_tex)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(jtext_tex.h);
        _jv_r = joy_api_call("text_get_height", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_jtext_cache_clear(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("jtext_cache_clear", _jv_args, 0, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jtext_cache_set_limit(void* _jv_ctx, int max_entries)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i32(max_entries);
        _jv_r = joy_api_call("jtext_cache_set_limit", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_jtext_cache_get_count(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("jtext_cache_get_count", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_jtext_cache_sweep(void* _jv_ctx, int max_idle_frames)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i32(max_idle_frames);
        _jv_r = joy_api_call("jtext_cache_sweep", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_animation_destroy(void* _jv_ctx, Animation animation)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(animation.h);
        _jv_r = joy_api_call("animation_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_animation_reset(void* _jv_ctx, Animation animation)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(animation.h);
        _jv_r = joy_api_call("animation_reset", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_animation_add_clip(void* _jv_ctx, Animation animation, const char* image_path, float duration, float src_rect_x, float src_rect_y, float src_rect_w, float src_rect_h)
{
        joy_value_t _jv_args[7];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(animation.h);
        _jv_args[1] = joy_v_str(image_path);
        _jv_args[2] = joy_v_f64(duration);
        _jv_args[3] = joy_v_f64(src_rect_x);
        _jv_args[4] = joy_v_f64(src_rect_y);
        _jv_args[5] = joy_v_f64(src_rect_w);
        _jv_args[6] = joy_v_f64(src_rect_h);
        _jv_r = joy_api_call("animation_add_clip", _jv_args, 7, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_animation_is_finished(void* _jv_ctx, Animation animation)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(animation.h);
        _jv_r = joy_api_call("animation_is_finished", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_animation_set_position(void* _jv_ctx, Animation animation, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(animation.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("animation_set_position", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_animation_set_scale(void* _jv_ctx, Animation animation, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(animation.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("animation_set_scale", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_animation_set_rotation(void* _jv_ctx, Animation animation, float rotation)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(animation.h);
        _jv_args[1] = joy_v_f64(rotation);
        _jv_r = joy_api_call("animation_set_rotation", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_animation_update(void* _jv_ctx, Animation animation, float dt)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(animation.h);
        _jv_args[1] = joy_v_f64(dt);
        _jv_r = joy_api_call("animation_update", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_destroy(void* _jv_ctx, Jr3d r)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_r = joy_api_call("r3d_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r3d_begin(void* _jv_ctx, Jr3d r)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_r = joy_api_call("r3d_begin", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_r3d_end(void* _jv_ctx, Jr3d r)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_r = joy_api_call("r3d_end", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_r3d_get_draw_calls(void* _jv_ctx, Jr3d r)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_r = joy_api_call("r3d_get_draw_calls", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_r3d_set_clear_color(void* _jv_ctx, Jr3d r, uint8_t cr, uint8_t cg, uint8_t cb)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_u32(cr);
        _jv_args[2] = joy_v_u32(cg);
        _jv_args[3] = joy_v_u32(cb);
        _jv_r = joy_api_call("r3d_set_clear_color", _jv_args, 4, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_mesh_destroy(void* _jv_ctx, Jr3d r, RenderMesh m)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(m.h);
        _jv_r = joy_api_call("r3d_mesh_destroy", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r3d_font_init(void* _jv_ctx, Jr3d r, const char* ttf_path, int pixel_height)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_str(ttf_path);
        _jv_args[2] = joy_v_i32(pixel_height);
        _jv_r = joy_api_call("r3d_font_init", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_r3d_font_draw_2d(void* _jv_ctx, Jr3d r, RenderMesh m, const char* text, float pixelX, float pixelY, float fontSize, float winW, float winH)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(m.h);
        _jv_args[2] = joy_v_str(text);
        _jv_args[3] = joy_v_f64(pixelX);
        _jv_args[4] = joy_v_f64(pixelY);
        _jv_args[5] = joy_v_f64(fontSize);
        _jv_args[6] = joy_v_f64(winW);
        _jv_args[7] = joy_v_f64(winH);
        _jv_r = joy_api_call("r3d_font_draw_2d", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_r3d_font_cache_set_limit(void* _jv_ctx, Jr3d r, int max_entries)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_i32(max_entries);
        _jv_r = joy_api_call("r3d_font_cache_set_limit", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_r3d_font_cache_get_count(void* _jv_ctx, Jr3d r)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_r = joy_api_call("r3d_font_cache_get_count", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_r3d_font_cache_clear(void* _jv_ctx, Jr3d r)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_r = joy_api_call("r3d_font_cache_clear", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_font_cache_sweep(void* _jv_ctx, Jr3d r, int max_idle_frames)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_i32(max_idle_frames);
        _jv_r = joy_api_call("r3d_font_cache_sweep", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_set_logical_presentation(void* _jv_ctx, Jr3d r, int w, int h)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_i32(w);
        _jv_args[2] = joy_v_i32(h);
        _jv_r = joy_api_call("r3d_set_logical_presentation", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_set_logical_presentation_mode(void* _jv_ctx, Jr3d r, int w, int h, int mode)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_i32(w);
        _jv_args[2] = joy_v_i32(h);
        _jv_args[3] = joy_v_i32(mode);
        _jv_r = joy_api_call("r3d_set_logical_presentation_mode", _jv_args, 4, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_button_draw(void* _jv_ctx, Jr3d r, RenderMesh m, Jr3dButton btn, float winW, float winH)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(m.h);
        _jv_args[2] = joy_v_handle(btn.h);
        _jv_args[3] = joy_v_f64(winW);
        _jv_args[4] = joy_v_f64(winH);
        _jv_r = joy_api_call("r3d_button_draw", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_button_draw_circle(void* _jv_ctx, Jr3d r, RenderMesh m, Jr3dButton btn, float winW, float winH)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(m.h);
        _jv_args[2] = joy_v_handle(btn.h);
        _jv_args[3] = joy_v_f64(winW);
        _jv_args[4] = joy_v_f64(winH);
        _jv_r = joy_api_call("r3d_button_draw_circle", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r3d_button_is_hovered(void* _jv_ctx, Jr3d r, Jr3dButton btn, float mx, float my)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(btn.h);
        _jv_args[2] = joy_v_f64(mx);
        _jv_args[3] = joy_v_f64(my);
        _jv_r = joy_api_call("r3d_button_is_hovered", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_r3d_button_is_pressed(void* _jv_ctx, Jr3d r, Jr3dButton btn, bool mouse_down, float mx, float my)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(btn.h);
        _jv_args[2] = joy_v_bool(mouse_down);
        _jv_args[3] = joy_v_f64(mx);
        _jv_args[4] = joy_v_f64(my);
        _jv_r = joy_api_call("r3d_button_is_pressed", _jv_args, 5, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_r3d_button_release(void* _jv_ctx, Jr3d r, Jr3dButton btn, float mx, float my)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(btn.h);
        _jv_args[2] = joy_v_f64(mx);
        _jv_args[3] = joy_v_f64(my);
        _jv_r = joy_api_call("r3d_button_release", _jv_args, 4, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_button_destroy(void* _jv_ctx, Jr3dButton btn)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_r = joy_api_call("r3d_button_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_label_set_text(void* _jv_ctx, Jr3dLabel l, const char* text)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(l.h);
        _jv_args[1] = joy_v_str(text);
        _jv_r = joy_api_call("r3d_label_set_text", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_label_draw(void* _jv_ctx, Jr3d r, RenderMesh m, Jr3dLabel l, float winW, float winH)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(m.h);
        _jv_args[2] = joy_v_handle(l.h);
        _jv_args[3] = joy_v_f64(winW);
        _jv_args[4] = joy_v_f64(winH);
        _jv_r = joy_api_call("r3d_label_draw", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_label_destroy(void* _jv_ctx, Jr3dLabel l)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(l.h);
        _jv_r = joy_api_call("r3d_label_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_input_set_text(void* _jv_ctx, Jr3dInput inp, const char* text)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(inp.h);
        _jv_args[1] = joy_v_str(text);
        _jv_r = joy_api_call("r3d_input_set_text", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r3d_input_is_hovered(void* _jv_ctx, Jr3d r, Jr3dInput inp, float mx, float my)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(inp.h);
        _jv_args[2] = joy_v_f64(mx);
        _jv_args[3] = joy_v_f64(my);
        _jv_r = joy_api_call("r3d_input_is_hovered", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_r3d_input_focus(void* _jv_ctx, Jr3dInput inp)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(inp.h);
        _jv_r = joy_api_call("r3d_input_focus", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_input_defocus(void* _jv_ctx, Jr3dInput inp)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(inp.h);
        _jv_r = joy_api_call("r3d_input_defocus", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r3d_input_is_focused(void* _jv_ctx, Jr3dInput inp)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(inp.h);
        _jv_r = joy_api_call("r3d_input_is_focused", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_r3d_input_start_text_input(void* _jv_ctx, Jr3d r, Jr3dInput inp)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(inp.h);
        _jv_r = joy_api_call("r3d_input_start_text_input", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_input_stop_text_input(void* _jv_ctx, Jr3d r, Jr3dInput inp)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(inp.h);
        _jv_r = joy_api_call("r3d_input_stop_text_input", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_input_insert(void* _jv_ctx, Jr3dInput inp, const char* s)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(inp.h);
        _jv_args[1] = joy_v_str(s);
        _jv_r = joy_api_call("r3d_input_insert", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_input_backspace(void* _jv_ctx, Jr3dInput inp)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(inp.h);
        _jv_r = joy_api_call("r3d_input_backspace", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_input_delete(void* _jv_ctx, Jr3dInput inp)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(inp.h);
        _jv_r = joy_api_call("r3d_input_delete", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_input_cursor_left(void* _jv_ctx, Jr3dInput inp)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(inp.h);
        _jv_r = joy_api_call("r3d_input_cursor_left", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_input_cursor_right(void* _jv_ctx, Jr3dInput inp)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(inp.h);
        _jv_r = joy_api_call("r3d_input_cursor_right", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_input_cursor_home(void* _jv_ctx, Jr3dInput inp)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(inp.h);
        _jv_r = joy_api_call("r3d_input_cursor_home", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_input_cursor_end(void* _jv_ctx, Jr3dInput inp)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(inp.h);
        _jv_r = joy_api_call("r3d_input_cursor_end", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_input_click(void* _jv_ctx, Jr3d r, Jr3dInput inp, float mx, float my)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(inp.h);
        _jv_args[2] = joy_v_f64(mx);
        _jv_args[3] = joy_v_f64(my);
        _jv_r = joy_api_call("r3d_input_click", _jv_args, 4, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_input_submit(void* _jv_ctx, Jr3dInput inp)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(inp.h);
        _jv_r = joy_api_call("r3d_input_submit", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_input_draw(void* _jv_ctx, Jr3d r, RenderMesh m, Jr3dInput inp, float winW, float winH)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(m.h);
        _jv_args[2] = joy_v_handle(inp.h);
        _jv_args[3] = joy_v_f64(winW);
        _jv_args[4] = joy_v_f64(winH);
        _jv_r = joy_api_call("r3d_input_draw", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_input_destroy(void* _jv_ctx, Jr3dInput inp)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(inp.h);
        _jv_r = joy_api_call("r3d_input_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_combobox_input_destroy(void* _jv_ctx, Jr3dComboboxInput ci)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_r = joy_api_call("r3d_combobox_input_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_combobox_input_add_item(void* _jv_ctx, Jr3dComboboxInput ci, const char* str)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_args[1] = joy_v_str(str);
        _jv_r = joy_api_call("r3d_combobox_input_add_item", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_combobox_input_clear_items(void* _jv_ctx, Jr3dComboboxInput ci)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_r = joy_api_call("r3d_combobox_input_clear_items", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_combobox_input_set_text(void* _jv_ctx, Jr3dComboboxInput ci, const char* str)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_args[1] = joy_v_str(str);
        _jv_r = joy_api_call("r3d_combobox_input_set_text", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_r3d_combobox_input_hit_item(void* _jv_ctx, Jr3dComboboxInput ci, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("r3d_combobox_input_hit_item", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_r3d_combobox_input_draw(void* _jv_ctx, Jr3d r, RenderMesh m, Jr3dComboboxInput ci, float winW, float winH)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(m.h);
        _jv_args[2] = joy_v_handle(ci.h);
        _jv_args[3] = joy_v_f64(winW);
        _jv_args[4] = joy_v_f64(winH);
        _jv_r = joy_api_call("r3d_combobox_input_draw", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_combobox_input_draw_dropdown(void* _jv_ctx, Jr3d r, RenderMesh m, Jr3dComboboxInput ci, float winW, float winH)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(m.h);
        _jv_args[2] = joy_v_handle(ci.h);
        _jv_args[3] = joy_v_f64(winW);
        _jv_args[4] = joy_v_f64(winH);
        _jv_r = joy_api_call("r3d_combobox_input_draw_dropdown", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_slider_destroy(void* _jv_ctx, Jr3dSlider s)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_r = joy_api_call("r3d_slider_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_slider_set_value(void* _jv_ctx, Jr3dSlider s, float v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_args[1] = joy_v_f64(v);
        _jv_r = joy_api_call("r3d_slider_set_value", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static float shim_r3d_slider_get_value(void* _jv_ctx, Jr3dSlider s)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_r = joy_api_call("r3d_slider_get_value", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static void shim_r3d_slider_draw(void* _jv_ctx, Jr3d r, RenderMesh m, Jr3dSlider s, float winW, float winH)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(m.h);
        _jv_args[2] = joy_v_handle(s.h);
        _jv_args[3] = joy_v_f64(winW);
        _jv_args[4] = joy_v_f64(winH);
        _jv_r = joy_api_call("r3d_slider_draw", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r3d_slider_is_pressed(void* _jv_ctx, Jr3d r, Jr3dSlider s, float x, float y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(s.h);
        _jv_args[2] = joy_v_f64(x);
        _jv_args[3] = joy_v_f64(y);
        _jv_r = joy_api_call("r3d_slider_is_pressed", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_r3d_slider_set_pressed(void* _jv_ctx, Jr3dSlider s, bool v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_args[1] = joy_v_bool(v);
        _jv_r = joy_api_call("r3d_slider_set_pressed", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_slider_drag_move(void* _jv_ctx, Jr3d r, Jr3dSlider s, float x, float y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(s.h);
        _jv_args[2] = joy_v_f64(x);
        _jv_args[3] = joy_v_f64(y);
        _jv_r = joy_api_call("r3d_slider_drag_move", _jv_args, 4, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r3d_slider_is_hovered(void* _jv_ctx, Jr3d r, Jr3dSlider s, float x, float y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(s.h);
        _jv_args[2] = joy_v_f64(x);
        _jv_args[3] = joy_v_f64(y);
        _jv_r = joy_api_call("r3d_slider_is_hovered", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_r3d_slider_set_hovered(void* _jv_ctx, Jr3dSlider s, bool v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_args[1] = joy_v_bool(v);
        _jv_r = joy_api_call("r3d_slider_set_hovered", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_checkbox_destroy(void* _jv_ctx, Jr3dCheckbox cb)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(cb.h);
        _jv_r = joy_api_call("r3d_checkbox_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_checkbox_toggle(void* _jv_ctx, Jr3dCheckbox cb)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(cb.h);
        _jv_r = joy_api_call("r3d_checkbox_toggle", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r3d_checkbox_is_pressed(void* _jv_ctx, Jr3d r, Jr3dCheckbox cb, float x, float y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(cb.h);
        _jv_args[2] = joy_v_f64(x);
        _jv_args[3] = joy_v_f64(y);
        _jv_r = joy_api_call("r3d_checkbox_is_pressed", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_r3d_checkbox_set_hovered(void* _jv_ctx, Jr3dCheckbox cb, bool v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(cb.h);
        _jv_args[1] = joy_v_bool(v);
        _jv_r = joy_api_call("r3d_checkbox_set_hovered", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_r3d_checkbox_is_hovered(void* _jv_ctx, Jr3d r, Jr3dCheckbox cb, float x, float y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(cb.h);
        _jv_args[2] = joy_v_f64(x);
        _jv_args[3] = joy_v_f64(y);
        _jv_r = joy_api_call("r3d_checkbox_is_hovered", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_r3d_checkbox_set_colors(void* _jv_ctx, Jr3dCheckbox cb, uint8_t box_r, uint8_t box_g, uint8_t box_b, uint8_t box_a, uint8_t check_r, uint8_t check_g, uint8_t check_b, uint8_t check_a, uint8_t hover_r, uint8_t hover_g, uint8_t hover_b, uint8_t hover_a)
{
        joy_value_t _jv_args[13];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(cb.h);
        _jv_args[1] = joy_v_u32(box_r);
        _jv_args[2] = joy_v_u32(box_g);
        _jv_args[3] = joy_v_u32(box_b);
        _jv_args[4] = joy_v_u32(box_a);
        _jv_args[5] = joy_v_u32(check_r);
        _jv_args[6] = joy_v_u32(check_g);
        _jv_args[7] = joy_v_u32(check_b);
        _jv_args[8] = joy_v_u32(check_a);
        _jv_args[9] = joy_v_u32(hover_r);
        _jv_args[10] = joy_v_u32(hover_g);
        _jv_args[11] = joy_v_u32(hover_b);
        _jv_args[12] = joy_v_u32(hover_a);
        _jv_r = joy_api_call("r3d_checkbox_set_colors", _jv_args, 13, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_checkbox_draw(void* _jv_ctx, Jr3d r, RenderMesh m, Jr3dCheckbox cb, float winW, float winH)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(m.h);
        _jv_args[2] = joy_v_handle(cb.h);
        _jv_args[3] = joy_v_f64(winW);
        _jv_args[4] = joy_v_f64(winH);
        _jv_r = joy_api_call("r3d_checkbox_draw", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_r3d_draw_grid(void* _jv_ctx, Jr3d r, int slices, float spacing, float line_width)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_i32(slices);
        _jv_args[2] = joy_v_f64(spacing);
        _jv_args[3] = joy_v_f64(line_width);
        _jv_r = joy_api_call("r3d_draw_grid", _jv_args, 4, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_destroy(void* _jv_ctx, Janimator anim)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_r = joy_api_call("janimator_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_set_float(void* _jv_ctx, Janimator anim, const char* name, float v)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(name);
        _jv_args[2] = joy_v_f64(v);
        _jv_r = joy_api_call("janimator_set_float", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_set_int(void* _jv_ctx, Janimator anim, const char* name, int v)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(name);
        _jv_args[2] = joy_v_i32(v);
        _jv_r = joy_api_call("janimator_set_int", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_set_bool(void* _jv_ctx, Janimator anim, const char* name, bool v)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(name);
        _jv_args[2] = joy_v_bool(v);
        _jv_r = joy_api_call("janimator_set_bool", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_set_trigger(void* _jv_ctx, Janimator anim, const char* name)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(name);
        _jv_r = joy_api_call("janimator_set_trigger", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static float shim_janimator_get_float(void* _jv_ctx, Janimator anim, const char* name)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(name);
        _jv_r = joy_api_call("janimator_get_float", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static int shim_janimator_get_int(void* _jv_ctx, Janimator anim, const char* name)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(name);
        _jv_r = joy_api_call("janimator_get_int", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static bool shim_janimator_get_bool(void* _jv_ctx, Janimator anim, const char* name)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(name);
        _jv_r = joy_api_call("janimator_get_bool", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_janimator_add_state(void* _jv_ctx, Janimator anim, const char* name)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(name);
        _jv_r = joy_api_call("janimator_add_state", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_set_default_state(void* _jv_ctx, Janimator anim, const char* name)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(name);
        _jv_r = joy_api_call("janimator_set_default_state", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_state_add_clip(void* _jv_ctx, Janimator anim, const char* state_name, const char* image_path, float duration, float src_rect_x, float src_rect_y, float src_rect_w, float src_rect_h)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(state_name);
        _jv_args[2] = joy_v_str(image_path);
        _jv_args[3] = joy_v_f64(duration);
        _jv_args[4] = joy_v_f64(src_rect_x);
        _jv_args[5] = joy_v_f64(src_rect_y);
        _jv_args[6] = joy_v_f64(src_rect_w);
        _jv_args[7] = joy_v_f64(src_rect_h);
        _jv_r = joy_api_call("janimator_state_add_clip", _jv_args, 8, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_state_add_color(void* _jv_ctx, Janimator anim, const char* state_name, float duration, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
        joy_value_t _jv_args[9];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(state_name);
        _jv_args[2] = joy_v_f64(duration);
        _jv_args[3] = joy_v_i32(w);
        _jv_args[4] = joy_v_i32(h);
        _jv_args[5] = joy_v_u32(r);
        _jv_args[6] = joy_v_u32(g);
        _jv_args[7] = joy_v_u32(b);
        _jv_args[8] = joy_v_u32(a);
        _jv_r = joy_api_call("janimator_state_add_color", _jv_args, 9, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_state_set_loop(void* _jv_ctx, Janimator anim, const char* state_name, bool loop)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(state_name);
        _jv_args[2] = joy_v_bool(loop);
        _jv_r = joy_api_call("janimator_state_set_loop", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_state_set_speed(void* _jv_ctx, Janimator anim, const char* state_name, float speed)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(state_name);
        _jv_args[2] = joy_v_f64(speed);
        _jv_r = joy_api_call("janimator_state_set_speed", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_state_set_blend(void* _jv_ctx, Janimator anim, const char* state_name, const char* param_name)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(state_name);
        _jv_args[2] = joy_v_str(param_name);
        _jv_r = joy_api_call("janimator_state_set_blend", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_state_add_motion(void* _jv_ctx, Janimator anim, const char* state_name, const char* image_path, float duration, float src_rect_x, float src_rect_y, float src_rect_w, float src_rect_h, float threshold)
{
        joy_value_t _jv_args[9];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(state_name);
        _jv_args[2] = joy_v_str(image_path);
        _jv_args[3] = joy_v_f64(duration);
        _jv_args[4] = joy_v_f64(src_rect_x);
        _jv_args[5] = joy_v_f64(src_rect_y);
        _jv_args[6] = joy_v_f64(src_rect_w);
        _jv_args[7] = joy_v_f64(src_rect_h);
        _jv_args[8] = joy_v_f64(threshold);
        _jv_r = joy_api_call("janimator_state_add_motion", _jv_args, 9, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_janimator_add_transition(void* _jv_ctx, Janimator anim, const char* from, const char* to)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(from);
        _jv_args[2] = joy_v_str(to);
        _jv_r = joy_api_call("janimator_add_transition", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int shim_janimator_add_any_transition(void* _jv_ctx, Janimator anim, const char* to)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_str(to);
        _jv_r = joy_api_call("janimator_add_any_transition", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_janimator_transition_add_cond(void* _jv_ctx, Janimator anim, int trans_idx, const char* param_name, int mode, float threshold)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_i32(trans_idx);
        _jv_args[2] = joy_v_str(param_name);
        _jv_args[3] = joy_v_i32(mode);
        _jv_args[4] = joy_v_f64(threshold);
        _jv_r = joy_api_call("janimator_transition_add_cond", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_transition_set_exit_time(void* _jv_ctx, Janimator anim, int trans_idx, float seconds)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_i32(trans_idx);
        _jv_args[2] = joy_v_f64(seconds);
        _jv_r = joy_api_call("janimator_transition_set_exit_time", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_update(void* _jv_ctx, Janimator anim, float dt)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_f64(dt);
        _jv_r = joy_api_call("janimator_update", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_draw(void* _jv_ctx, Janimator anim, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("janimator_draw", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_janimator_set_flip_h(void* _jv_ctx, Janimator anim, bool flip)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(anim.h);
        _jv_args[1] = joy_v_bool(flip);
        _jv_r = joy_api_call("janimator_set_flip_h", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_label_destroy(void* _jv_ctx, Label label)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(label.h);
        _jv_r = joy_api_call("label_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_label_draw(void* _jv_ctx, Label label)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(label.h);
        _jv_r = joy_api_call("label_draw", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_button_destroy(void* _jv_ctx, Button btn)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_r = joy_api_call("button_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_button_set_position(void* _jv_ctx, Button btn, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("button_set_position", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_button_set_size(void* _jv_ctx, Button btn, float width, float height)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_args[1] = joy_v_f64(width);
        _jv_args[2] = joy_v_f64(height);
        _jv_r = joy_api_call("button_set_size", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_button_set_normal_color(void* _jv_ctx, Button btn, uint8_t color_r, uint8_t color_g, uint8_t color_b, uint8_t color_a)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_args[1] = joy_v_u32(color_r);
        _jv_args[2] = joy_v_u32(color_g);
        _jv_args[3] = joy_v_u32(color_b);
        _jv_args[4] = joy_v_u32(color_a);
        _jv_r = joy_api_call("button_set_normal_color", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_button_set_hover_color(void* _jv_ctx, Button btn, uint8_t color_r, uint8_t color_g, uint8_t color_b, uint8_t color_a)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_args[1] = joy_v_u32(color_r);
        _jv_args[2] = joy_v_u32(color_g);
        _jv_args[3] = joy_v_u32(color_b);
        _jv_args[4] = joy_v_u32(color_a);
        _jv_r = joy_api_call("button_set_hover_color", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_button_set_pressed_color(void* _jv_ctx, Button btn, uint8_t color_r, uint8_t color_g, uint8_t color_b, uint8_t color_a)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_args[1] = joy_v_u32(color_r);
        _jv_args[2] = joy_v_u32(color_g);
        _jv_args[3] = joy_v_u32(color_b);
        _jv_args[4] = joy_v_u32(color_a);
        _jv_r = joy_api_call("button_set_pressed_color", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_button_set_image(void* _jv_ctx, Button btn, Image image)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_args[1] = joy_v_handle(image.h);
        _jv_r = joy_api_call("button_set_image", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_button_set_pressed_image(void* _jv_ctx, Button btn, Image image)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_args[1] = joy_v_handle(image.h);
        _jv_r = joy_api_call("button_set_pressed_image", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_button_set_textx(void* _jv_ctx, Button btn, Font fontinfo, const char* str, int len, uint8_t color_r, uint8_t color_g, uint8_t color_b, uint8_t color_a)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_args[1] = joy_v_handle(fontinfo.h);
        _jv_args[2] = joy_v_str(str);
        _jv_args[3] = joy_v_i32(len);
        _jv_args[4] = joy_v_u32(color_r);
        _jv_args[5] = joy_v_u32(color_g);
        _jv_args[6] = joy_v_u32(color_b);
        _jv_args[7] = joy_v_u32(color_a);
        _jv_r = joy_api_call("button_set_textx", _jv_args, 8, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_button_is_hovered(void* _jv_ctx, Button btn, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("button_is_hovered", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_button_is_pressed(void* _jv_ctx, Button btn, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("button_is_pressed", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_button_set_hovered(void* _jv_ctx, Button btn, bool v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_args[1] = joy_v_bool(v);
        _jv_r = joy_api_call("button_set_hovered", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_button_set_pressed(void* _jv_ctx, Button btn, bool v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_args[1] = joy_v_bool(v);
        _jv_r = joy_api_call("button_set_pressed", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_button_get_pressed(void* _jv_ctx, Button btn)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_r = joy_api_call("button_get_pressed", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_button_set_text_offset(void* _jv_ctx, Button btn, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("button_set_text_offset", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_button_draw(void* _jv_ctx, Button btn)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(btn.h);
        _jv_r = joy_api_call("button_draw", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scrollview_destroy(void* _jv_ctx, Scrollview v)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(v.h);
        _jv_r = joy_api_call("scrollview_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scrollview_set_view_px(void* _jv_ctx, Scrollview v, float x, float y, float w, float h)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(v.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_args[3] = joy_v_f64(w);
        _jv_args[4] = joy_v_f64(h);
        _jv_r = joy_api_call("scrollview_set_view_px", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scrollview_set_content(void* _jv_ctx, Scrollview v, float content_w, float content_h)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(v.h);
        _jv_args[1] = joy_v_f64(content_w);
        _jv_args[2] = joy_v_f64(content_h);
        _jv_r = joy_api_call("scrollview_set_content", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scrollview_set_offset(void* _jv_ctx, Scrollview v, float ox, float oy)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(v.h);
        _jv_args[1] = joy_v_f64(ox);
        _jv_args[2] = joy_v_f64(oy);
        _jv_r = joy_api_call("scrollview_set_offset", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scrollview_clamp(void* _jv_ctx, Scrollview v)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(v.h);
        _jv_r = joy_api_call("scrollview_clamp", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static float shim_scrollview_get_offset_x(void* _jv_ctx, Scrollview v)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(v.h);
        _jv_r = joy_api_call("scrollview_get_offset_x", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_scrollview_get_offset_y(void* _jv_ctx, Scrollview v)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(v.h);
        _jv_r = joy_api_call("scrollview_get_offset_y", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static void shim_scrollview_set_show_bar(void* _jv_ctx, Scrollview v, bool show)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(v.h);
        _jv_args[1] = joy_v_bool(show);
        _jv_r = joy_api_call("scrollview_set_show_bar", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_scrollview_dragging(void* _jv_ctx, Scrollview v)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(v.h);
        _jv_r = joy_api_call("scrollview_dragging", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_scrollview_hit(void* _jv_ctx, Scrollview v, float win_x, float win_y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(v.h);
        _jv_args[1] = joy_v_f64(win_x);
        _jv_args[2] = joy_v_f64(win_y);
        _jv_r = joy_api_call("scrollview_hit", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_scrollview_begin(void* _jv_ctx, Scrollview v)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(v.h);
        _jv_r = joy_api_call("scrollview_begin", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scrollview_end(void* _jv_ctx, Scrollview v)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(v.h);
        _jv_r = joy_api_call("scrollview_end", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_combobox_destroy(void* _jv_ctx, Combobox combobox)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(combobox.h);
        _jv_r = joy_api_call("combobox_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_combobox_add_item(void* _jv_ctx, Combobox combobox, Font font, const char* str, int len, uint8_t color_r, uint8_t color_g, uint8_t color_b, uint8_t color_a)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(combobox.h);
        _jv_args[1] = joy_v_handle(font.h);
        _jv_args[2] = joy_v_str(str);
        _jv_args[3] = joy_v_i32(len);
        _jv_args[4] = joy_v_u32(color_r);
        _jv_args[5] = joy_v_u32(color_g);
        _jv_args[6] = joy_v_u32(color_b);
        _jv_args[7] = joy_v_u32(color_a);
        _jv_r = joy_api_call("combobox_add_item", _jv_args, 8, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_combobox_is_pressed(void* _jv_ctx, Combobox combobox, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(combobox.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("combobox_is_pressed", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_combobox_is_open(void* _jv_ctx, Combobox combobox)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(combobox.h);
        _jv_r = joy_api_call("combobox_is_open", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_combobox_set_open(void* _jv_ctx, Combobox combobox, bool v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(combobox.h);
        _jv_args[1] = joy_v_bool(v);
        _jv_r = joy_api_call("combobox_set_open", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_combobox_hit_item(void* _jv_ctx, Combobox combobox, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(combobox.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("combobox_hit_item", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_combobox_set_selected(void* _jv_ctx, Combobox combobox, int index)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(combobox.h);
        _jv_args[1] = joy_v_i32(index);
        _jv_r = joy_api_call("combobox_set_selected", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_combobox_get_selected(void* _jv_ctx, Combobox combobox)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(combobox.h);
        _jv_r = joy_api_call("combobox_get_selected", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_combobox_draw(void* _jv_ctx, Combobox combobox)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(combobox.h);
        _jv_r = joy_api_call("combobox_draw", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_combobox_input_destroy(void* _jv_ctx, ComboboxInput ci)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_r = joy_api_call("combobox_input_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_combobox_input_add_item(void* _jv_ctx, ComboboxInput ci, const char* str, int len)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_args[1] = joy_v_str(str);
        _jv_args[2] = joy_v_i32(len);
        _jv_r = joy_api_call("combobox_input_add_item", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_combobox_input_clear_items(void* _jv_ctx, ComboboxInput ci)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_r = joy_api_call("combobox_input_clear_items", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_combobox_input_set_text(void* _jv_ctx, ComboboxInput ci, const char* str)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_args[1] = joy_v_str(str);
        _jv_r = joy_api_call("combobox_input_set_text", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_combobox_input_draw(void* _jv_ctx, ComboboxInput ci)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_r = joy_api_call("combobox_input_draw", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_combobox_input_draw_dropdown(void* _jv_ctx, ComboboxInput ci)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_r = joy_api_call("combobox_input_draw_dropdown", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_combobox_input_is_open(void* _jv_ctx, ComboboxInput ci)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_r = joy_api_call("combobox_input_is_open", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_combobox_input_set_popup_node(void* _jv_ctx, ComboboxInput ci, SceneNode node)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_args[1] = joy_v_handle(node.h);
        _jv_r = joy_api_call("combobox_input_set_popup_node", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_combobox_input_hit_item(void* _jv_ctx, ComboboxInput ci, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("combobox_input_hit_item", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_combobox_input_set_open(void* _jv_ctx, ComboboxInput ci, bool v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ci.h);
        _jv_args[1] = joy_v_bool(v);
        _jv_r = joy_api_call("combobox_input_set_open", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_datagrid_setheaders(void* _jv_ctx, Datagrid grid, const char* headers)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(grid.h);
        _jv_args[1] = joy_v_str(headers);
        _jv_r = joy_api_call("datagrid_setheaders", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_datagrid_setheader(void* _jv_ctx, Datagrid grid, int index, const char* header)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(grid.h);
        _jv_args[1] = joy_v_i32(index);
        _jv_args[2] = joy_v_str(header);
        _jv_r = joy_api_call("datagrid_setheader", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_datagrid_setcell(void* _jv_ctx, Datagrid grid, int row_index, int col_index, const char* data)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(grid.h);
        _jv_args[1] = joy_v_i32(row_index);
        _jv_args[2] = joy_v_i32(col_index);
        _jv_args[3] = joy_v_str(data);
        _jv_r = joy_api_call("datagrid_setcell", _jv_args, 4, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_datagrid_destroy(void* _jv_ctx, Datagrid grid)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(grid.h);
        _jv_r = joy_api_call("datagrid_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_datagrid_draw(void* _jv_ctx, Datagrid grid, Jr2d r2d)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(grid.h);
        _jv_args[1] = joy_v_handle(r2d.h);
        _jv_r = joy_api_call("datagrid_draw", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_datagrid_hit_row(void* _jv_ctx, Datagrid grid, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(grid.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("datagrid_hit_row", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_datagrid_scroll(void* _jv_ctx, Datagrid grid, int lines)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(grid.h);
        _jv_args[1] = joy_v_i32(lines);
        _jv_r = joy_api_call("datagrid_scroll", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_datagrid_set_selected_row(void* _jv_ctx, Datagrid grid, int row)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(grid.h);
        _jv_args[1] = joy_v_i32(row);
        _jv_r = joy_api_call("datagrid_set_selected_row", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_radio_destroy(void* _jv_ctx, Radio radio)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(radio.h);
        _jv_r = joy_api_call("radio_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_radio_is_pressed(void* _jv_ctx, Radio radio, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(radio.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("radio_is_pressed", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_radio_set_hovered(void* _jv_ctx, Radio radio, bool v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(radio.h);
        _jv_args[1] = joy_v_bool(v);
        _jv_r = joy_api_call("radio_set_hovered", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_checkbox_destroy(void* _jv_ctx, Checkbox checkbox)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(checkbox.h);
        _jv_r = joy_api_call("checkbox_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_checkbox_set_colors(void* _jv_ctx, Checkbox checkbox, uint8_t box_r, uint8_t box_g, uint8_t box_b, uint8_t box_a, uint8_t check_r, uint8_t check_g, uint8_t check_b, uint8_t check_a, uint8_t hover_r, uint8_t hover_g, uint8_t hover_b, uint8_t hover_a)
{
        joy_value_t _jv_args[13];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(checkbox.h);
        _jv_args[1] = joy_v_u32(box_r);
        _jv_args[2] = joy_v_u32(box_g);
        _jv_args[3] = joy_v_u32(box_b);
        _jv_args[4] = joy_v_u32(box_a);
        _jv_args[5] = joy_v_u32(check_r);
        _jv_args[6] = joy_v_u32(check_g);
        _jv_args[7] = joy_v_u32(check_b);
        _jv_args[8] = joy_v_u32(check_a);
        _jv_args[9] = joy_v_u32(hover_r);
        _jv_args[10] = joy_v_u32(hover_g);
        _jv_args[11] = joy_v_u32(hover_b);
        _jv_args[12] = joy_v_u32(hover_a);
        _jv_r = joy_api_call("checkbox_set_colors", _jv_args, 13, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_checkbox_set_r2d(void* _jv_ctx, Checkbox checkbox, Jr2d r2d)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(checkbox.h);
        _jv_args[1] = joy_v_handle(r2d.h);
        _jv_r = joy_api_call("checkbox_set_r2d", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_checkbox_set_position(void* _jv_ctx, Checkbox checkbox, int x, int y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(checkbox.h);
        _jv_args[1] = joy_v_i32(x);
        _jv_args[2] = joy_v_i32(y);
        _jv_r = joy_api_call("checkbox_set_position", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_checkbox_set_text_color(void* _jv_ctx, Checkbox checkbox, uint8_t color_r, uint8_t color_g, uint8_t color_b, uint8_t color_a)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(checkbox.h);
        _jv_args[1] = joy_v_u32(color_r);
        _jv_args[2] = joy_v_u32(color_g);
        _jv_args[3] = joy_v_u32(color_b);
        _jv_args[4] = joy_v_u32(color_a);
        _jv_r = joy_api_call("checkbox_set_text_color", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_checkbox_toggle(void* _jv_ctx, Checkbox checkbox)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(checkbox.h);
        _jv_r = joy_api_call("checkbox_toggle", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_checkbox_is_pressed(void* _jv_ctx, Checkbox cb, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(cb.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("checkbox_is_pressed", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_checkbox_set_hovered(void* _jv_ctx, Checkbox cb, bool v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(cb.h);
        _jv_args[1] = joy_v_bool(v);
        _jv_r = joy_api_call("checkbox_set_hovered", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joystick_destroy(void* _jv_ctx, Joystick joystick)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(joystick.h);
        _jv_r = joy_api_call("joystick_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joystick_set_textures(void* _jv_ctx, Joystick joystick, const char* base_path, const char* thumb_path)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(joystick.h);
        _jv_args[1] = joy_v_str(base_path);
        _jv_args[2] = joy_v_str(thumb_path);
        _jv_r = joy_api_call("joystick_set_textures", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joystick_set_vector(void* _jv_ctx, Joystick joystick, float dx, float dy)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(joystick.h);
        _jv_args[1] = joy_v_f64(dx);
        _jv_args[2] = joy_v_f64(dy);
        _jv_r = joy_api_call("joystick_set_vector", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joystick_draw(void* _jv_ctx, Joystick joystick)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(joystick.h);
        _jv_r = joy_api_call("joystick_draw", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joystick_reset(void* _jv_ctx, Joystick joystick)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(joystick.h);
        _jv_r = joy_api_call("joystick_reset", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joystick_set_position(void* _jv_ctx, Joystick joystick, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(joystick.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("joystick_set_position", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joystick_set_radius(void* _jv_ctx, Joystick joystick, float radius)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(joystick.h);
        _jv_args[1] = joy_v_f64(radius);
        _jv_r = joy_api_call("joystick_set_radius", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_joystick_is_dragging(void* _jv_ctx, Joystick joystick)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(joystick.h);
        _jv_r = joy_api_call("joystick_is_dragging", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_joystick_touch_down(void* _jv_ctx, Joystick joystick, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(joystick.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("joystick_touch_down", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joystick_touch_move(void* _jv_ctx, Joystick joystick, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(joystick.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("joystick_touch_move", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_joystick_touch_up(void* _jv_ctx, Joystick joystick)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(joystick.h);
        _jv_r = joy_api_call("joystick_touch_up", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static float shim_joystick_get_magnitude(void* _jv_ctx, Joystick joystick)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(joystick.h);
        _jv_r = joy_api_call("joystick_get_magnitude", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static void shim_inputbox_destroy(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_set_text(void* _jv_ctx, Inputbox ti, const char* text)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_args[1] = joy_v_str(text);
        _jv_r = joy_api_call("inputbox_set_text", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_draw(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_draw", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_inputbox_is_pressed(void* _jv_ctx, Inputbox ti, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("inputbox_is_pressed", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_inputbox_is_hovered(void* _jv_ctx, Inputbox ti, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("inputbox_is_hovered", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_inputbox_focus(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_focus", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_defocus(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_defocus", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_blur(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_blur", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_insert(void* _jv_ctx, Inputbox ti, const char* s, int len)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_args[1] = joy_v_str(s);
        _jv_args[2] = joy_v_i32(len);
        _jv_r = joy_api_call("inputbox_insert", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_backspace(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_backspace", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_delete(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_delete", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_cursor_left(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_cursor_left", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_cursor_right(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_cursor_right", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_cursor_home(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_cursor_home", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_cursor_end(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_cursor_end", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_select_left(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_select_left", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_select_right(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_select_right", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_select_home(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_select_home", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_select_end(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_select_end", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_select_all(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_select_all", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_paste(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_paste", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_copy(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_copy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_inputbox_cut(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_cut", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_inputbox_is_focused(void* _jv_ctx, Inputbox ti)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ti.h);
        _jv_r = joy_api_call("inputbox_is_focused", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_slider_destroy(void* _jv_ctx, Slider s)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_r = joy_api_call("slider_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_slider_set_value(void* _jv_ctx, Slider s, float v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_args[1] = joy_v_f64(v);
        _jv_r = joy_api_call("slider_set_value", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static float shim_slider_get_value(void* _jv_ctx, Slider s)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_r = joy_api_call("slider_get_value", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static void shim_slider_set_rect(void* _jv_ctx, Slider s, float x, float y, float w, float h)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_args[3] = joy_v_f64(w);
        _jv_args[4] = joy_v_f64(h);
        _jv_r = joy_api_call("slider_set_rect", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_slider_set_range(void* _jv_ctx, Slider s, float min, float max)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_args[1] = joy_v_f64(min);
        _jv_args[2] = joy_v_f64(max);
        _jv_r = joy_api_call("slider_set_range", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_slider_draw(void* _jv_ctx, Slider s)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_r = joy_api_call("slider_draw", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_slider_is_pressed(void* _jv_ctx, Slider s, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("slider_is_pressed", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_slider_set_pressed(void* _jv_ctx, Slider s, bool v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_args[1] = joy_v_bool(v);
        _jv_r = joy_api_call("slider_set_pressed", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_slider_drag_move(void* _jv_ctx, Slider s, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("slider_drag_move", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_slider_is_hovered(void* _jv_ctx, Slider s, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("slider_is_hovered", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_slider_set_hovered(void* _jv_ctx, Slider s, bool v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(s.h);
        _jv_args[1] = joy_v_bool(v);
        _jv_r = joy_api_call("slider_set_hovered", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_popup_destroy(void* _jv_ctx, Popup p)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_r = joy_api_call("popup_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_popup_set_title(void* _jv_ctx, Popup p, const char* text, int len)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_args[1] = joy_v_str(text);
        _jv_args[2] = joy_v_i32(len);
        _jv_r = joy_api_call("popup_set_title", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_popup_set_content(void* _jv_ctx, Popup p, const char* text, int len)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_args[1] = joy_v_str(text);
        _jv_args[2] = joy_v_i32(len);
        _jv_r = joy_api_call("popup_set_content", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_popup_set_position(void* _jv_ctx, Popup p, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("popup_set_position", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_popup_set_visible(void* _jv_ctx, Popup p, bool v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_args[1] = joy_v_bool(v);
        _jv_r = joy_api_call("popup_set_visible", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_popup_set_closeable(void* _jv_ctx, Popup p, bool v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_args[1] = joy_v_bool(v);
        _jv_r = joy_api_call("popup_set_closeable", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_popup_draw(void* _jv_ctx, Popup p, Font title_font, Font content_font)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_args[1] = joy_v_handle(title_font.h);
        _jv_args[2] = joy_v_handle(content_font.h);
        _jv_r = joy_api_call("popup_draw", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_popup_is_close_clicked(void* _jv_ctx, Popup p, float mx, float my)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_args[1] = joy_v_f64(mx);
        _jv_args[2] = joy_v_f64(my);
        _jv_r = joy_api_call("popup_is_close_clicked", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_popup_is_title_clicked(void* _jv_ctx, Popup p, float mx, float my)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_args[1] = joy_v_f64(mx);
        _jv_args[2] = joy_v_f64(my);
        _jv_r = joy_api_call("popup_is_title_clicked", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_popup_drag_move(void* _jv_ctx, Popup p, float mx, float my)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_args[1] = joy_v_f64(mx);
        _jv_args[2] = joy_v_f64(my);
        _jv_r = joy_api_call("popup_drag_move", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_popup_is_inside(void* _jv_ctx, Popup p, float mx, float my)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_args[1] = joy_v_f64(mx);
        _jv_args[2] = joy_v_f64(my);
        _jv_r = joy_api_call("popup_is_inside", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_tooltip_destroy(void* _jv_ctx, Tooltip t)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(t.h);
        _jv_r = joy_api_call("tooltip_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_tooltip_set_text(void* _jv_ctx, Tooltip t, const char* text, int len)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(t.h);
        _jv_args[1] = joy_v_str(text);
        _jv_args[2] = joy_v_i32(len);
        _jv_r = joy_api_call("tooltip_set_text", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_tooltip_set_position(void* _jv_ctx, Tooltip t, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(t.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("tooltip_set_position", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_tooltip_set_visible(void* _jv_ctx, Tooltip t, bool v)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(t.h);
        _jv_args[1] = joy_v_bool(v);
        _jv_r = joy_api_call("tooltip_set_visible", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_tooltip_draw(void* _jv_ctx, Tooltip t, Font font)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(t.h);
        _jv_args[1] = joy_v_handle(font.h);
        _jv_r = joy_api_call("tooltip_draw", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_ctx_destroy(void* _jv_ctx, JuiCtx ctx)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_r = joy_api_call("jui_ctx_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_begin(void* _jv_ctx, JuiCtx ctx)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_r = joy_api_call("jui_begin", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_end(void* _jv_ctx, JuiCtx ctx)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_r = joy_api_call("jui_end", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_input_motion(void* _jv_ctx, JuiCtx ctx, float win_x, float win_y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_f64(win_x);
        _jv_args[2] = joy_v_f64(win_y);
        _jv_r = joy_api_call("jui_input_motion", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_input_down(void* _jv_ctx, JuiCtx ctx, int button)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_i32(button);
        _jv_r = joy_api_call("jui_input_down", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_input_up(void* _jv_ctx, JuiCtx ctx, int button)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_i32(button);
        _jv_r = joy_api_call("jui_input_up", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_input_wheel(void* _jv_ctx, JuiCtx ctx, float dy)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_f64(dy);
        _jv_r = joy_api_call("jui_input_wheel", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_input_key(void* _jv_ctx, JuiCtx ctx, bool shift, bool ctrl)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_bool(shift);
        _jv_args[2] = joy_v_bool(ctrl);
        _jv_r = joy_api_call("jui_input_key", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_jui_is_down(void* _jv_ctx, JuiCtx ctx)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_r = joy_api_call("jui_is_down", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_jui_is_pressed(void* _jv_ctx, JuiCtx ctx)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_r = joy_api_call("jui_is_pressed", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_jui_is_released(void* _jv_ctx, JuiCtx ctx)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_r = joy_api_call("jui_is_released", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static float shim_jui_take_wheel(void* _jv_ctx, JuiCtx ctx)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_r = joy_api_call("jui_take_wheel", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static bool shim_jui_is_shift(void* _jv_ctx, JuiCtx ctx)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_r = joy_api_call("jui_is_shift", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_jui_is_ctrl(void* _jv_ctx, JuiCtx ctx)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_r = joy_api_call("jui_is_ctrl", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_jui_push_clip(void* _jv_ctx, JuiCtx ctx, float rect_x, float rect_y, float rect_w, float rect_h)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_f64(rect_x);
        _jv_args[2] = joy_v_f64(rect_y);
        _jv_args[3] = joy_v_f64(rect_w);
        _jv_args[4] = joy_v_f64(rect_h);
        _jv_r = joy_api_call("jui_push_clip", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_pop_clip(void* _jv_ctx, JuiCtx ctx)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_r = joy_api_call("jui_pop_clip", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_reset_clip(void* _jv_ctx, JuiCtx ctx)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_r = joy_api_call("jui_reset_clip", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_jui_hit(void* _jv_ctx, JuiCtx ctx, float rect_x, float rect_y, float rect_w, float rect_h)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_f64(rect_x);
        _jv_args[2] = joy_v_f64(rect_y);
        _jv_args[3] = joy_v_f64(rect_w);
        _jv_args[4] = joy_v_f64(rect_h);
        _jv_r = joy_api_call("jui_hit", _jv_args, 5, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_jui_fill_rect(void* _jv_ctx, JuiCtx ctx, float r_x, float r_y, float r_w, float r_h, uint8_t c_r, uint8_t c_g, uint8_t c_b, uint8_t c_a)
{
        joy_value_t _jv_args[9];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_f64(r_x);
        _jv_args[2] = joy_v_f64(r_y);
        _jv_args[3] = joy_v_f64(r_w);
        _jv_args[4] = joy_v_f64(r_h);
        _jv_args[5] = joy_v_u32(c_r);
        _jv_args[6] = joy_v_u32(c_g);
        _jv_args[7] = joy_v_u32(c_b);
        _jv_args[8] = joy_v_u32(c_a);
        _jv_r = joy_api_call("jui_fill_rect", _jv_args, 9, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_stroke_rect(void* _jv_ctx, JuiCtx ctx, float r_x, float r_y, float r_w, float r_h, uint8_t c_r, uint8_t c_g, uint8_t c_b, uint8_t c_a)
{
        joy_value_t _jv_args[9];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_f64(r_x);
        _jv_args[2] = joy_v_f64(r_y);
        _jv_args[3] = joy_v_f64(r_w);
        _jv_args[4] = joy_v_f64(r_h);
        _jv_args[5] = joy_v_u32(c_r);
        _jv_args[6] = joy_v_u32(c_g);
        _jv_args[7] = joy_v_u32(c_b);
        _jv_args[8] = joy_v_u32(c_a);
        _jv_r = joy_api_call("jui_stroke_rect", _jv_args, 9, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_line(void* _jv_ctx, JuiCtx ctx, float x1, float y1, float x2, float y2, uint8_t c_r, uint8_t c_g, uint8_t c_b, uint8_t c_a)
{
        joy_value_t _jv_args[9];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_f64(x1);
        _jv_args[2] = joy_v_f64(y1);
        _jv_args[3] = joy_v_f64(x2);
        _jv_args[4] = joy_v_f64(y2);
        _jv_args[5] = joy_v_u32(c_r);
        _jv_args[6] = joy_v_u32(c_g);
        _jv_args[7] = joy_v_u32(c_b);
        _jv_args[8] = joy_v_u32(c_a);
        _jv_r = joy_api_call("jui_line", _jv_args, 9, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_jui_text(void* _jv_ctx, JuiCtx ctx, float x, float y, const char* s, uint8_t c_r, uint8_t c_g, uint8_t c_b, uint8_t c_a)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_args[3] = joy_v_str(s);
        _jv_args[4] = joy_v_u32(c_r);
        _jv_args[5] = joy_v_u32(c_g);
        _jv_args[6] = joy_v_u32(c_b);
        _jv_args[7] = joy_v_u32(c_a);
        _jv_r = joy_api_call("jui_text", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int shim_jui_text_centered(void* _jv_ctx, JuiCtx ctx, float r_x, float r_y, float r_w, float r_h, const char* s, uint8_t c_r, uint8_t c_g, uint8_t c_b, uint8_t c_a)
{
        joy_value_t _jv_args[10];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_f64(r_x);
        _jv_args[2] = joy_v_f64(r_y);
        _jv_args[3] = joy_v_f64(r_w);
        _jv_args[4] = joy_v_f64(r_h);
        _jv_args[5] = joy_v_str(s);
        _jv_args[6] = joy_v_u32(c_r);
        _jv_args[7] = joy_v_u32(c_g);
        _jv_args[8] = joy_v_u32(c_b);
        _jv_args[9] = joy_v_u32(c_a);
        _jv_r = joy_api_call("jui_text_centered", _jv_args, 10, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_jui_panel_begin(void* _jv_ctx, JuiCtx ctx, float r_x, float r_y, float r_w, float r_h, const char* title)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_f64(r_x);
        _jv_args[2] = joy_v_f64(r_y);
        _jv_args[3] = joy_v_f64(r_w);
        _jv_args[4] = joy_v_f64(r_h);
        _jv_args[5] = joy_v_str(title);
        _jv_r = joy_api_call("jui_panel_begin", _jv_args, 6, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_panel_end(void* _jv_ctx, JuiCtx ctx)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_r = joy_api_call("jui_panel_end", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_panel_frame(void* _jv_ctx, JuiCtx ctx, float r_x, float r_y, float r_w, float r_h)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_f64(r_x);
        _jv_args[2] = joy_v_f64(r_y);
        _jv_args[3] = joy_v_f64(r_w);
        _jv_args[4] = joy_v_f64(r_h);
        _jv_r = joy_api_call("jui_panel_frame", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static float shim_jui_scroll_begin(void* _jv_ctx, JuiCtx ctx, int id, float view_x, float view_y, float view_w, float view_h, float content_h)
{
        joy_value_t _jv_args[7];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_i32(id);
        _jv_args[2] = joy_v_f64(view_x);
        _jv_args[3] = joy_v_f64(view_y);
        _jv_args[4] = joy_v_f64(view_w);
        _jv_args[5] = joy_v_f64(view_h);
        _jv_args[6] = joy_v_f64(content_h);
        _jv_r = joy_api_call("jui_scroll_begin", _jv_args, 7, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static void shim_jui_scroll_end(void* _jv_ctx, JuiCtx ctx)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_r = joy_api_call("jui_scroll_end", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_scrollbar(void* _jv_ctx, JuiCtx ctx, float view_x, float view_y, float view_w, float view_h, float content_h, float off)
{
        joy_value_t _jv_args[7];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_f64(view_x);
        _jv_args[2] = joy_v_f64(view_y);
        _jv_args[3] = joy_v_f64(view_w);
        _jv_args[4] = joy_v_f64(view_h);
        _jv_args[5] = joy_v_f64(content_h);
        _jv_args[6] = joy_v_f64(off);
        _jv_r = joy_api_call("jui_scrollbar", _jv_args, 7, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jui_scroll_set_offset(void* _jv_ctx, JuiCtx ctx, int id, float off, float content_h, float view_h)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_i32(id);
        _jv_args[2] = joy_v_f64(off);
        _jv_args[3] = joy_v_f64(content_h);
        _jv_args[4] = joy_v_f64(view_h);
        _jv_r = joy_api_call("jui_scroll_set_offset", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_jui_button(void* _jv_ctx, JuiCtx ctx, int id, float r_x, float r_y, float r_w, float r_h, const char* text)
{
        joy_value_t _jv_args[7];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ctx.h);
        _jv_args[1] = joy_v_i32(id);
        _jv_args[2] = joy_v_f64(r_x);
        _jv_args[3] = joy_v_f64(r_y);
        _jv_args[4] = joy_v_f64(r_w);
        _jv_args[5] = joy_v_f64(r_h);
        _jv_args[6] = joy_v_str(text);
        _jv_r = joy_api_call("jui_button", _jv_args, 7, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_jtilemap_destroy(void* _jv_ctx, Jtilemap tm)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tm.h);
        _jv_r = joy_api_call("jtilemap_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_jtilemap_get_width(void* _jv_ctx, Jtilemap tm)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tm.h);
        _jv_r = joy_api_call("jtilemap_get_width", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int shim_jtilemap_get_height(void* _jv_ctx, Jtilemap tm)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tm.h);
        _jv_r = joy_api_call("jtilemap_get_height", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int shim_jtilemap_get_tilewidth(void* _jv_ctx, Jtilemap tm)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tm.h);
        _jv_r = joy_api_call("jtilemap_get_tilewidth", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int shim_jtilemap_get_tileheight(void* _jv_ctx, Jtilemap tm)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tm.h);
        _jv_r = joy_api_call("jtilemap_get_tileheight", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int shim_jtilemap_get_anim_tileid(void* _jv_ctx, Jtilemap tm, int gid, uint32_t time_ms)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tm.h);
        _jv_args[1] = joy_v_i32(gid);
        _jv_args[2] = joy_v_u32(time_ms);
        _jv_r = joy_api_call("jtilemap_get_anim_tileid", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static int shim_jtilemap_get_tile(void* _jv_ctx, JtilemapTileLayer layer, int tx, int ty)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(layer.h);
        _jv_args[1] = joy_v_i32(tx);
        _jv_args[2] = joy_v_i32(ty);
        _jv_r = joy_api_call("jtilemap_get_tile", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_jtilemap_render_all(void* _jv_ctx, JtilemapRender r, Jtilemap tm, float camera_x, float camera_y, uint32_t time_ms)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(tm.h);
        _jv_args[2] = joy_v_f64(camera_x);
        _jv_args[3] = joy_v_f64(camera_y);
        _jv_args[4] = joy_v_u32(time_ms);
        _jv_r = joy_api_call("jtilemap_render_all", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jtilemap_render_tiles(void* _jv_ctx, JtilemapRender r, Jtilemap tm, float camera_x, float camera_y, uint32_t time_ms)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(tm.h);
        _jv_args[2] = joy_v_f64(camera_x);
        _jv_args[3] = joy_v_f64(camera_y);
        _jv_args[4] = joy_v_u32(time_ms);
        _jv_r = joy_api_call("jtilemap_render_tiles", _jv_args, 5, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jtilemap_render_set_tile_px(void* _jv_ctx, JtilemapRender r, float px)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_f64(px);
        _jv_r = joy_api_call("jtilemap_render_set_tile_px", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_jtilemap_render_layer(void* _jv_ctx, JtilemapRender r, Jtilemap tm, const char* layer_name, float camera_x, float camera_y, uint32_t time_ms)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_args[1] = joy_v_handle(tm.h);
        _jv_args[2] = joy_v_str(layer_name);
        _jv_args[3] = joy_v_f64(camera_x);
        _jv_args[4] = joy_v_f64(camera_y);
        _jv_args[5] = joy_v_u32(time_ms);
        _jv_r = joy_api_call("jtilemap_render_layer", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_jtilemap_render_destroy(void* _jv_ctx, JtilemapRender r)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r.h);
        _jv_r = joy_api_call("jtilemap_render_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_kcpserver_destroy(void* _jv_ctx, Kcpserver ks)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ks.h);
        _jv_r = joy_api_call("kcpserver_destroy", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_kcpserver_send(void* _jv_ctx, Kcpserver ks, int conv, const char* data, int len)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ks.h);
        _jv_args[1] = joy_v_i32(conv);
        _jv_args[2] = joy_v_str(data);
        _jv_args[3] = joy_v_i32(len);
        _jv_r = joy_api_call("kcpserver_send", _jv_args, 4, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_kcpserver_broadcast(void* _jv_ctx, Kcpserver ks, const char* data, int len)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ks.h);
        _jv_args[1] = joy_v_str(data);
        _jv_args[2] = joy_v_i32(len);
        _jv_r = joy_api_call("kcpserver_broadcast", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_kcpserver_offline(void* _jv_ctx, Kcpserver ks, int conv)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ks.h);
        _jv_args[1] = joy_v_i32(conv);
        _jv_r = joy_api_call("kcpserver_offline", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_kcpserver_update(void* _jv_ctx, Kcpserver ks)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ks.h);
        _jv_r = joy_api_call("kcpserver_update", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_kcpserver_connection_count(void* _jv_ctx, Kcpserver ks)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ks.h);
        _jv_r = joy_api_call("kcpserver_connection_count", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static bool shim_kcpserver_poll_message(void* _jv_ctx, Kcpserver ks, NetMessage msg)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ks.h);
        _jv_args[1] = joy_v_handle(msg.h);
        _jv_r = joy_api_call("kcpserver_poll_message", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_kcpclient_destroy(void* _jv_ctx, Kcpclient kc)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(kc.h);
        _jv_r = joy_api_call("kcpclient_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_kcpclient_send(void* _jv_ctx, Kcpclient kc, const char* data, int len)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(kc.h);
        _jv_args[1] = joy_v_str(data);
        _jv_args[2] = joy_v_i32(len);
        _jv_r = joy_api_call("kcpclient_send", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_kcpclient_update(void* _jv_ctx, Kcpclient kc)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(kc.h);
        _jv_r = joy_api_call("kcpclient_update", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_kcpclient_poll_message(void* _jv_ctx, Kcpclient kc, NetMessage msg)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(kc.h);
        _jv_args[1] = joy_v_handle(msg.h);
        _jv_r = joy_api_call("kcpclient_poll_message", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static int shim_tcpserver_destroy(void* _jv_ctx, Tcpserver tcpserver)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tcpserver.h);
        _jv_r = joy_api_call("tcpserver_destroy", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_tcpserver_send(void* _jv_ctx, Tcpserver tcpserver, int conv, const char* data, int len)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tcpserver.h);
        _jv_args[1] = joy_v_i32(conv);
        _jv_args[2] = joy_v_str(data);
        _jv_args[3] = joy_v_i32(len);
        _jv_r = joy_api_call("tcpserver_send", _jv_args, 4, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_tcpserver_broadcast(void* _jv_ctx, Tcpserver tcpserver, const char* data, int len)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tcpserver.h);
        _jv_args[1] = joy_v_str(data);
        _jv_args[2] = joy_v_i32(len);
        _jv_r = joy_api_call("tcpserver_broadcast", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_tcpserver_offline(void* _jv_ctx, Tcpserver tcpserver, int conv)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tcpserver.h);
        _jv_args[1] = joy_v_i32(conv);
        _jv_r = joy_api_call("tcpserver_offline", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_tcpserver_update(void* _jv_ctx, Tcpserver tcpserver)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tcpserver.h);
        _jv_r = joy_api_call("tcpserver_update", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_tcpserver_connection_count(void* _jv_ctx, Tcpserver tcpserver)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tcpserver.h);
        _jv_r = joy_api_call("tcpserver_connection_count", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static bool shim_tcpserver_poll_message(void* _jv_ctx, Tcpserver tcpserver, NetMessage msg)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tcpserver.h);
        _jv_args[1] = joy_v_handle(msg.h);
        _jv_r = joy_api_call("tcpserver_poll_message", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_tcpclient_destroy(void* _jv_ctx, Tcpclient tcpclient)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tcpclient.h);
        _jv_r = joy_api_call("tcpclient_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_tcpclient_send(void* _jv_ctx, Tcpclient tcpclient, const char* data, int len)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tcpclient.h);
        _jv_args[1] = joy_v_str(data);
        _jv_args[2] = joy_v_i32(len);
        _jv_r = joy_api_call("tcpclient_send", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_tcpclient_update(void* _jv_ctx, Tcpclient tcpclient)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tcpclient.h);
        _jv_r = joy_api_call("tcpclient_update", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_tcpclient_poll_message(void* _jv_ctx, Tcpclient tcpclient, NetMessage msg)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(tcpclient.h);
        _jv_args[1] = joy_v_handle(msg.h);
        _jv_r = joy_api_call("tcpclient_poll_message", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static int shim_wsnetserver_destroy(void* _jv_ctx, Wsnetserver ws)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ws.h);
        _jv_r = joy_api_call("wsnetserver_destroy", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_wsnetserver_send(void* _jv_ctx, Wsnetserver ws, int conv, const char* data, int len)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ws.h);
        _jv_args[1] = joy_v_i32(conv);
        _jv_args[2] = joy_v_str(data);
        _jv_args[3] = joy_v_i32(len);
        _jv_r = joy_api_call("wsnetserver_send", _jv_args, 4, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_wsnetserver_broadcast(void* _jv_ctx, Wsnetserver ws, const char* data, int len)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ws.h);
        _jv_args[1] = joy_v_str(data);
        _jv_args[2] = joy_v_i32(len);
        _jv_r = joy_api_call("wsnetserver_broadcast", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_wsnetserver_offline(void* _jv_ctx, Wsnetserver ws, int conv)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ws.h);
        _jv_args[1] = joy_v_i32(conv);
        _jv_r = joy_api_call("wsnetserver_offline", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_wsnetserver_update(void* _jv_ctx, Wsnetserver ws)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ws.h);
        _jv_r = joy_api_call("wsnetserver_update", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_wsnetserver_connection_count(void* _jv_ctx, Wsnetserver ws)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ws.h);
        _jv_r = joy_api_call("wsnetserver_connection_count", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static bool shim_wsnetserver_poll_message(void* _jv_ctx, Wsnetserver ws, NetMessage msg)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ws.h);
        _jv_args[1] = joy_v_handle(msg.h);
        _jv_r = joy_api_call("wsnetserver_poll_message", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_wsnetclient_destroy(void* _jv_ctx, Wsnetclient wc)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(wc.h);
        _jv_r = joy_api_call("wsnetclient_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_wsnetclient_send(void* _jv_ctx, Wsnetclient wc, const char* data, int len)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(wc.h);
        _jv_args[1] = joy_v_str(data);
        _jv_args[2] = joy_v_i32(len);
        _jv_r = joy_api_call("wsnetclient_send", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_wsnetclient_update(void* _jv_ctx, Wsnetclient wc)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(wc.h);
        _jv_r = joy_api_call("wsnetclient_update", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_wsnetclient_poll_message(void* _jv_ctx, Wsnetclient wc, NetMessage msg)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(wc.h);
        _jv_args[1] = joy_v_handle(msg.h);
        _jv_r = joy_api_call("wsnetclient_poll_message", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_netclient_destroy(void* _jv_ctx, Netclient nc)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(nc.h);
        _jv_r = joy_api_call("netclient_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_netclient_send(void* _jv_ctx, Netclient nc, const char* data, int len)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(nc.h);
        _jv_args[1] = joy_v_str(data);
        _jv_args[2] = joy_v_i32(len);
        _jv_r = joy_api_call("netclient_send", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_netclient_update(void* _jv_ctx, Netclient nc)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(nc.h);
        _jv_r = joy_api_call("netclient_update", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_netclient_poll_message(void* _jv_ctx, Netclient nc, NetMessage msg)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(nc.h);
        _jv_args[1] = joy_v_handle(msg.h);
        _jv_r = joy_api_call("netclient_poll_message", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_netserver_destroy(void* _jv_ctx, Netserver ns)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ns.h);
        _jv_r = joy_api_call("netserver_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_netserver_send(void* _jv_ctx, Netserver ns, int conv, const char* data, int len)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ns.h);
        _jv_args[1] = joy_v_i32(conv);
        _jv_args[2] = joy_v_str(data);
        _jv_args[3] = joy_v_i32(len);
        _jv_r = joy_api_call("netserver_send", _jv_args, 4, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_netserver_broadcast(void* _jv_ctx, Netserver ns, const char* data, int len)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ns.h);
        _jv_args[1] = joy_v_str(data);
        _jv_args[2] = joy_v_i32(len);
        _jv_r = joy_api_call("netserver_broadcast", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_netserver_offline(void* _jv_ctx, Netserver ns, int conv)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ns.h);
        _jv_args[1] = joy_v_i32(conv);
        _jv_r = joy_api_call("netserver_offline", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_netserver_update(void* _jv_ctx, Netserver ns)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ns.h);
        _jv_r = joy_api_call("netserver_update", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_netserver_connection_count(void* _jv_ctx, Netserver ns)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ns.h);
        _jv_r = joy_api_call("netserver_connection_count", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static bool shim_netserver_poll_message(void* _jv_ctx, Netserver ns, NetMessage msg)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(ns.h);
        _jv_args[1] = joy_v_handle(msg.h);
        _jv_r = joy_api_call("netserver_poll_message", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_netmessage_free_data(void* _jv_ctx, NetMessage msg)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(msg.h);
        _jv_r = joy_api_call("netmessage_free_data", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_node_destroy(void* _jv_ctx, SceneNode node)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(node.h);
        _jv_r = joy_api_call("scene_node_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_node_add_child(void* _jv_ctx, SceneNode parent, SceneNode child)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(parent.h);
        _jv_args[1] = joy_v_handle(child.h);
        _jv_r = joy_api_call("scene_node_add_child", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_node_remove_child(void* _jv_ctx, SceneNode parent, SceneNode child)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(parent.h);
        _jv_args[1] = joy_v_handle(child.h);
        _jv_r = joy_api_call("scene_node_remove_child", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_scene_node_get_child_count(void* _jv_ctx, SceneNode node)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(node.h);
        _jv_r = joy_api_call("scene_node_get_child_count", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_scene_node_set_position(void* _jv_ctx, SceneNode node, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(node.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("scene_node_set_position", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_node_set_world_position(void* _jv_ctx, SceneNode node, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(node.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("scene_node_set_world_position", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_node_set_rotation(void* _jv_ctx, SceneNode node, float radians)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(node.h);
        _jv_args[1] = joy_v_f64(radians);
        _jv_r = joy_api_call("scene_node_set_rotation", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static float shim_scene_node_get_rotation(void* _jv_ctx, SceneNode node)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(node.h);
        _jv_r = joy_api_call("scene_node_get_rotation", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static void shim_scene_node_set_scale(void* _jv_ctx, SceneNode node, float scale_x, float scale_y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(node.h);
        _jv_args[1] = joy_v_f64(scale_x);
        _jv_args[2] = joy_v_f64(scale_y);
        _jv_r = joy_api_call("scene_node_set_scale", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_node_set_zorder(void* _jv_ctx, SceneNode node, int zorder)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(node.h);
        _jv_args[1] = joy_v_i32(zorder);
        _jv_r = joy_api_call("scene_node_set_zorder", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_scene_node_get_zorder(void* _jv_ctx, SceneNode node)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(node.h);
        _jv_r = joy_api_call("scene_node_get_zorder", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_scene_node_set_name(void* _jv_ctx, SceneNode node, const char* name)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(node.h);
        _jv_args[1] = joy_v_str(name);
        _jv_r = joy_api_call("scene_node_set_name", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_node_set_tag(void* _jv_ctx, SceneNode node, const char* tag)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(node.h);
        _jv_args[1] = joy_v_str(tag);
        _jv_r = joy_api_call("scene_node_set_tag", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_node_update(void* _jv_ctx, SceneNode node, float delta_time)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(node.h);
        _jv_args[1] = joy_v_f64(delta_time);
        _jv_r = joy_api_call("scene_node_update", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_destroy(void* _jv_ctx, Scene scene)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(scene.h);
        _jv_r = joy_api_call("scene_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_add_root_node(void* _jv_ctx, Scene scene, SceneNode root)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(scene.h);
        _jv_args[1] = joy_v_handle(root.h);
        _jv_r = joy_api_call("scene_add_root_node", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_remove_root_node(void* _jv_ctx, Scene scene, SceneNode root)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(scene.h);
        _jv_args[1] = joy_v_handle(root.h);
        _jv_r = joy_api_call("scene_remove_root_node", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_scene_get_root_count(void* _jv_ctx, Scene scene)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(scene.h);
        _jv_r = joy_api_call("scene_get_root_count", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_scene_update(void* _jv_ctx, Scene scene, float delta_time)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(scene.h);
        _jv_args[1] = joy_v_f64(delta_time);
        _jv_r = joy_api_call("scene_update", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_set_popup_node(void* _jv_ctx, Scene scene, SceneNode node)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(scene.h);
        _jv_args[1] = joy_v_handle(node.h);
        _jv_r = joy_api_call("scene_set_popup_node", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_manager_destroy(void* _jv_ctx, SceneManager mgr)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(mgr.h);
        _jv_r = joy_api_call("scene_manager_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_manager_push(void* _jv_ctx, SceneManager mgr, Scene scene)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(mgr.h);
        _jv_args[1] = joy_v_handle(scene.h);
        _jv_r = joy_api_call("scene_manager_push", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_manager_replace(void* _jv_ctx, SceneManager mgr, Scene scene)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(mgr.h);
        _jv_args[1] = joy_v_handle(scene.h);
        _jv_r = joy_api_call("scene_manager_replace", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_manager_pop(void* _jv_ctx, SceneManager mgr)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(mgr.h);
        _jv_r = joy_api_call("scene_manager_pop", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_scene_manager_update(void* _jv_ctx, SceneManager mgr, float delta_time)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(mgr.h);
        _jv_args[1] = joy_v_f64(delta_time);
        _jv_r = joy_api_call("scene_manager_update", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_scene_manager_get_count(void* _jv_ctx, SceneManager mgr)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(mgr.h);
        _jv_r = joy_api_call("scene_manager_get_count", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_jparticle_destroy(void* _jv_ctx, Jparticle p)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_r = joy_api_call("jparticle_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jparticle_set_position(void* _jv_ctx, Jparticle p, float x, float y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_args[1] = joy_v_f64(x);
        _jv_args[2] = joy_v_f64(y);
        _jv_r = joy_api_call("jparticle_set_position", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jparticle_emit(void* _jv_ctx, Jparticle p)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_r = joy_api_call("jparticle_emit", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jparticle_clear(void* _jv_ctx, Jparticle p)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_r = joy_api_call("jparticle_clear", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jparticle_update(void* _jv_ctx, Jparticle p, float dt)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_args[1] = joy_v_f64(dt);
        _jv_r = joy_api_call("jparticle_update", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jparticle_render(void* _jv_ctx, Jparticle p, Jr2d r2d)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_args[1] = joy_v_handle(r2d.h);
        _jv_r = joy_api_call("jparticle_render", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_jparticle_count(void* _jv_ctx, Jparticle p)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_r = joy_api_call("jparticle_count", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static bool shim_jparticle_is_active(void* _jv_ctx, Jparticle p)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(p.h);
        _jv_r = joy_api_call("jparticle_is_active", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_jlight_sys_destroy(void* _jv_ctx, JlightSys sys)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(sys.h);
        _jv_r = joy_api_call("jlight_sys_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jlight_sys_resize(void* _jv_ctx, JlightSys sys, int width, int height)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(sys.h);
        _jv_args[1] = joy_v_i32(width);
        _jv_args[2] = joy_v_i32(height);
        _jv_r = joy_api_call("jlight_sys_resize", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jlight_remove(void* _jv_ctx, JlightSys sys, int id)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(sys.h);
        _jv_args[1] = joy_v_i32(id);
        _jv_r = joy_api_call("jlight_remove", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jlight_clear(void* _jv_ctx, JlightSys sys)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(sys.h);
        _jv_r = joy_api_call("jlight_clear", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int shim_jlight_add_occluder(void* _jv_ctx, JlightSys sys, float x1, float y1, float x2, float y2)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(sys.h);
        _jv_args[1] = joy_v_f64(x1);
        _jv_args[2] = joy_v_f64(y1);
        _jv_args[3] = joy_v_f64(x2);
        _jv_args[4] = joy_v_f64(y2);
        _jv_r = joy_api_call("jlight_add_occluder", _jv_args, 5, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int)0;
        return (int) _jv_r.as.i32;
}

static void shim_jlight_clear_occluders(void* _jv_ctx, JlightSys sys)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(sys.h);
        _jv_r = joy_api_call("jlight_clear_occluders", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jlight_set_ambient(void* _jv_ctx, JlightSys sys, uint8_t r, uint8_t g, uint8_t b)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(sys.h);
        _jv_args[1] = joy_v_u32(r);
        _jv_args[2] = joy_v_u32(g);
        _jv_args[3] = joy_v_u32(b);
        _jv_r = joy_api_call("jlight_set_ambient", _jv_args, 4, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jlight_render(void* _jv_ctx, JlightSys sys, Jr2d r2d)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(sys.h);
        _jv_args[1] = joy_v_handle(r2d.h);
        _jv_r = joy_api_call("jlight_render", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jecs_destroy(void* _jv_ctx, JecsWorld world)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(world.h);
        _jv_r = joy_api_call("jecs_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jecs_process(void* _jv_ctx, JecsWorld world, float dt)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(world.h);
        _jv_args[1] = joy_v_f64(dt);
        _jv_r = joy_api_call("jecs_process", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_jecs_define(void* _jv_ctx, JecsWorld world, const char* name, size_t size)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(world.h);
        _jv_args[1] = joy_v_str(name);
        _jv_args[2] = joy_v_u64(size);
        _jv_r = joy_api_call("jecs_define", _jv_args, 3, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static uint32_t shim_audio_open_device(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("audio_open_device", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U32) return (uint32_t)0;
        return (uint32_t) _jv_r.as.u32;
}

static void shim_audio_close_device(void* _jv_ctx, uint32_t device_id)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_u32(device_id);
        _jv_r = joy_api_call("audio_close_device", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static bool shim_audio_create_wav(void* _jv_ctx, const char* filepath, uint32_t device_id)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_str(filepath);
        _jv_args[1] = joy_v_u32(device_id);
        _jv_r = joy_api_call("audio_create_wav", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_audio_create_mp3(void* _jv_ctx, const char* filepath, uint32_t device_id)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_str(filepath);
        _jv_args[1] = joy_v_u32(device_id);
        _jv_r = joy_api_call("audio_create_mp3", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_audio_create_flac(void* _jv_ctx, const char* filepath, uint32_t device_id)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_str(filepath);
        _jv_args[1] = joy_v_u32(device_id);
        _jv_r = joy_api_call("audio_create_flac", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static bool shim_audio_create_ogg(void* _jv_ctx, const char* filepath, uint32_t device_id)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_str(filepath);
        _jv_args[1] = joy_v_u32(device_id);
        _jv_r = joy_api_call("audio_create_ogg", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_BOOL) return (bool)0;
        return (bool) _jv_r.as.b;
}

static void shim_audio_shot_play(void* _jv_ctx, AudioShot shot)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(shot.h);
        _jv_r = joy_api_call("audio_shot_play", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_audio_shot_set_gain(void* _jv_ctx, AudioShot shot, float gain)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(shot.h);
        _jv_args[1] = joy_v_f64(gain);
        _jv_r = joy_api_call("audio_shot_set_gain", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_audio_shot_destroy(void* _jv_ctx, AudioShot shot)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(shot.h);
        _jv_r = joy_api_call("audio_shot_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_audio_bgm_stop(void* _jv_ctx, AudioBgm bgm)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(bgm.h);
        _jv_r = joy_api_call("audio_bgm_stop", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_audio_bgm_set_gain(void* _jv_ctx, AudioBgm bgm, float gain)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(bgm.h);
        _jv_args[1] = joy_v_f64(gain);
        _jv_r = joy_api_call("audio_bgm_set_gain", _jv_args, 2, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_audio_bgm_destroy(void* _jv_ctx, AudioBgm bgm)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(bgm.h);
        _jv_r = joy_api_call("audio_bgm_destroy", _jv_args, 1, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static void shim_image_draw_rect(void* _jv_ctx, Jr2d r2d, Image image, float x, float y, float w, float h)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_handle(r2d.h);
        _jv_args[1] = joy_v_handle(image.h);
        _jv_args[2] = joy_v_f64(x);
        _jv_args[3] = joy_v_f64(y);
        _jv_args[4] = joy_v_f64(w);
        _jv_args[5] = joy_v_f64(h);
        _jv_r = joy_api_call("image_draw_rect", _jv_args, 6, &_jv_st);
        (void)_jv_r;
        (void)_jv_st;
        return;
}

static int64_t shim_jmath_fp_from_float(void* _jv_ctx, float a)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a);
        _jv_r = joy_api_call("jmath_fp_from_float", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static float shim_jmath_fp_to_float(void* _jv_ctx, int64_t a)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a);
        _jv_r = joy_api_call("jmath_fp_to_float", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static int64_t shim_jmath_fp_from_uint(void* _jv_ctx, uint32_t a)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_u32(a);
        _jv_r = joy_api_call("jmath_fp_from_uint", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_fp_from_int(void* _jv_ctx, int32_t a)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i32(a);
        _jv_r = joy_api_call("jmath_fp_from_int", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_fp_from_fraction(void* _jv_ctx, uint32_t a)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_u32(a);
        _jv_r = joy_api_call("jmath_fp_from_fraction", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_fp_min_value(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("jmath_fp_min_value", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_fp_max_value(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("jmath_fp_max_value", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_fp_add(void* _jv_ctx, int64_t a, int64_t b)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a);
        _jv_args[1] = joy_v_i64(b);
        _jv_r = joy_api_call("jmath_fp_add", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_fp_sub(void* _jv_ctx, int64_t a, int64_t b)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a);
        _jv_args[1] = joy_v_i64(b);
        _jv_r = joy_api_call("jmath_fp_sub", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_fp_zero(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("jmath_fp_zero", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_fp_half(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("jmath_fp_half", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_fp_one(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("jmath_fp_one", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_fp_pi(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("jmath_fp_pi", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_fp_max(void* _jv_ctx, int64_t a, int64_t b)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a);
        _jv_args[1] = joy_v_i64(b);
        _jv_r = joy_api_call("jmath_fp_max", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_fp_min(void* _jv_ctx, int64_t a, int64_t b)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a);
        _jv_args[1] = joy_v_i64(b);
        _jv_r = joy_api_call("jmath_fp_min", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_fp_pow2(void* _jv_ctx, int64_t x)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(x);
        _jv_r = joy_api_call("jmath_fp_pow2", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int32_t shim_jmath_fp_sign(void* _jv_ctx, int64_t x)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(x);
        _jv_r = joy_api_call("jmath_fp_sign", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int32_t)0;
        return (int32_t) _jv_r.as.i32;
}

static int64_t shim_jmath_fp_clamp(void* _jv_ctx, int64_t a, int64_t low, int64_t high)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a);
        _jv_args[1] = joy_v_i64(low);
        _jv_args[2] = joy_v_i64(high);
        _jv_r = joy_api_call("jmath_fp_clamp", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_fp_lerp(void* _jv_ctx, int64_t a, int64_t b, int64_t t)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a);
        _jv_args[1] = joy_v_i64(b);
        _jv_args[2] = joy_v_i64(t);
        _jv_r = joy_api_call("jmath_fp_lerp", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_negate_x(void* _jv_ctx, int64_t a_x, int64_t a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_r = joy_api_call("jmath_vec2f_negate_x", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_negate_y(void* _jv_ctx, int64_t a_x, int64_t a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_r = joy_api_call("jmath_vec2f_negate_y", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_dot(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(b_x);
        _jv_args[3] = joy_v_i64(b_y);
        _jv_r = joy_api_call("jmath_vec2f_dot", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_cross(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(b_x);
        _jv_args[3] = joy_v_i64(b_y);
        _jv_r = joy_api_call("jmath_vec2f_cross", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_cross2_x(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t s)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(s);
        _jv_r = joy_api_call("jmath_vec2f_cross2_x", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_cross2_y(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t s)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(s);
        _jv_r = joy_api_call("jmath_vec2f_cross2_y", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_cross3_x(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t s)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(s);
        _jv_r = joy_api_call("jmath_vec2f_cross3_x", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_cross3_y(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t s)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(s);
        _jv_r = joy_api_call("jmath_vec2f_cross3_y", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_add_x(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(b_x);
        _jv_args[3] = joy_v_i64(b_y);
        _jv_r = joy_api_call("jmath_vec2f_add_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_add_y(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(b_x);
        _jv_args[3] = joy_v_i64(b_y);
        _jv_r = joy_api_call("jmath_vec2f_add_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_sub_x(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(b_x);
        _jv_args[3] = joy_v_i64(b_y);
        _jv_r = joy_api_call("jmath_vec2f_sub_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_sub_y(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(b_x);
        _jv_args[3] = joy_v_i64(b_y);
        _jv_r = joy_api_call("jmath_vec2f_sub_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_scale_x(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t s)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(s);
        _jv_r = joy_api_call("jmath_vec2f_scale_x", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_scale_y(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t s)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(s);
        _jv_r = joy_api_call("jmath_vec2f_scale_y", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_abs_x(void* _jv_ctx, int64_t a_x, int64_t a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_r = joy_api_call("jmath_vec2f_abs_x", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_abs_y(void* _jv_ctx, int64_t a_x, int64_t a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_r = joy_api_call("jmath_vec2f_abs_y", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_length_squared(void* _jv_ctx, int64_t a_x, int64_t a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_r = joy_api_call("jmath_vec2f_length_squared", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_length(void* _jv_ctx, int64_t a_x, int64_t a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_r = joy_api_call("jmath_vec2f_length", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_normalize_x(void* _jv_ctx, int64_t a_x, int64_t a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_r = joy_api_call("jmath_vec2f_normalize_x", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_normalize_y(void* _jv_ctx, int64_t a_x, int64_t a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_r = joy_api_call("jmath_vec2f_normalize_y", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_normal_x(void* _jv_ctx, int64_t v_x, int64_t v_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(v_x);
        _jv_args[1] = joy_v_i64(v_y);
        _jv_r = joy_api_call("jmath_vec2f_normal_x", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_normal_y(void* _jv_ctx, int64_t v_x, int64_t v_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(v_x);
        _jv_args[1] = joy_v_i64(v_y);
        _jv_r = joy_api_call("jmath_vec2f_normal_y", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_rotate_x(void* _jv_ctx, int64_t v_x, int64_t v_y, int64_t angle)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(v_x);
        _jv_args[1] = joy_v_i64(v_y);
        _jv_args[2] = joy_v_i64(angle);
        _jv_r = joy_api_call("jmath_vec2f_rotate_x", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_rotate_y(void* _jv_ctx, int64_t v_x, int64_t v_y, int64_t angle)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(v_x);
        _jv_args[1] = joy_v_i64(v_y);
        _jv_args[2] = joy_v_i64(angle);
        _jv_r = joy_api_call("jmath_vec2f_rotate_y", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_lerp_x(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y, int64_t t)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(b_x);
        _jv_args[3] = joy_v_i64(b_y);
        _jv_args[4] = joy_v_i64(t);
        _jv_r = joy_api_call("jmath_vec2f_lerp_x", _jv_args, 5, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_lerp_y(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y, int64_t t)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(b_x);
        _jv_args[3] = joy_v_i64(b_y);
        _jv_args[4] = joy_v_i64(t);
        _jv_r = joy_api_call("jmath_vec2f_lerp_y", _jv_args, 5, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec2f_distance(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(b_x);
        _jv_args[3] = joy_v_i64(b_y);
        _jv_r = joy_api_call("jmath_vec2f_distance", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_negate_x(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_r = joy_api_call("jmath_vec3f_negate_x", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_negate_y(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_r = joy_api_call("jmath_vec3f_negate_y", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_negate_z(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_r = joy_api_call("jmath_vec3f_negate_z", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_add_x(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(b_x);
        _jv_args[4] = joy_v_i64(b_y);
        _jv_args[5] = joy_v_i64(b_z);
        _jv_r = joy_api_call("jmath_vec3f_add_x", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_add_y(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(b_x);
        _jv_args[4] = joy_v_i64(b_y);
        _jv_args[5] = joy_v_i64(b_z);
        _jv_r = joy_api_call("jmath_vec3f_add_y", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_add_z(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(b_x);
        _jv_args[4] = joy_v_i64(b_y);
        _jv_args[5] = joy_v_i64(b_z);
        _jv_r = joy_api_call("jmath_vec3f_add_z", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_sub_x(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(b_x);
        _jv_args[4] = joy_v_i64(b_y);
        _jv_args[5] = joy_v_i64(b_z);
        _jv_r = joy_api_call("jmath_vec3f_sub_x", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_sub_y(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(b_x);
        _jv_args[4] = joy_v_i64(b_y);
        _jv_args[5] = joy_v_i64(b_z);
        _jv_r = joy_api_call("jmath_vec3f_sub_y", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_sub_z(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(b_x);
        _jv_args[4] = joy_v_i64(b_y);
        _jv_args[5] = joy_v_i64(b_z);
        _jv_r = joy_api_call("jmath_vec3f_sub_z", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_scale_x(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t s)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(s);
        _jv_r = joy_api_call("jmath_vec3f_scale_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_scale_y(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t s)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(s);
        _jv_r = joy_api_call("jmath_vec3f_scale_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_scale_z(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t s)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(s);
        _jv_r = joy_api_call("jmath_vec3f_scale_z", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_abs_x(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_r = joy_api_call("jmath_vec3f_abs_x", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_abs_y(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_r = joy_api_call("jmath_vec3f_abs_y", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_abs_z(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_r = joy_api_call("jmath_vec3f_abs_z", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_dot(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(b_x);
        _jv_args[4] = joy_v_i64(b_y);
        _jv_args[5] = joy_v_i64(b_z);
        _jv_r = joy_api_call("jmath_vec3f_dot", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_cross_x(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(b_x);
        _jv_args[4] = joy_v_i64(b_y);
        _jv_args[5] = joy_v_i64(b_z);
        _jv_r = joy_api_call("jmath_vec3f_cross_x", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_cross_y(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(b_x);
        _jv_args[4] = joy_v_i64(b_y);
        _jv_args[5] = joy_v_i64(b_z);
        _jv_r = joy_api_call("jmath_vec3f_cross_y", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_cross_z(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(b_x);
        _jv_args[4] = joy_v_i64(b_y);
        _jv_args[5] = joy_v_i64(b_z);
        _jv_r = joy_api_call("jmath_vec3f_cross_z", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_square_length(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_r = joy_api_call("jmath_vec3f_square_length", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_length(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_r = joy_api_call("jmath_vec3f_length", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_normalize_x(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_r = joy_api_call("jmath_vec3f_normalize_x", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_normalize_y(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_r = joy_api_call("jmath_vec3f_normalize_y", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_normalize_z(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_r = joy_api_call("jmath_vec3f_normalize_z", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_lerp_x(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z, int64_t t)
{
        joy_value_t _jv_args[7];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(b_x);
        _jv_args[4] = joy_v_i64(b_y);
        _jv_args[5] = joy_v_i64(b_z);
        _jv_args[6] = joy_v_i64(t);
        _jv_r = joy_api_call("jmath_vec3f_lerp_x", _jv_args, 7, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_lerp_y(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z, int64_t t)
{
        joy_value_t _jv_args[7];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(b_x);
        _jv_args[4] = joy_v_i64(b_y);
        _jv_args[5] = joy_v_i64(b_z);
        _jv_args[6] = joy_v_i64(t);
        _jv_r = joy_api_call("jmath_vec3f_lerp_y", _jv_args, 7, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_lerp_z(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z, int64_t t)
{
        joy_value_t _jv_args[7];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(b_x);
        _jv_args[4] = joy_v_i64(b_y);
        _jv_args[5] = joy_v_i64(b_z);
        _jv_args[6] = joy_v_i64(t);
        _jv_r = joy_api_call("jmath_vec3f_lerp_z", _jv_args, 7, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_vec3f_distance(void* _jv_ctx, int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_x);
        _jv_args[1] = joy_v_i64(a_y);
        _jv_args[2] = joy_v_i64(a_z);
        _jv_args[3] = joy_v_i64(b_x);
        _jv_args[4] = joy_v_i64(b_y);
        _jv_args[5] = joy_v_i64(b_z);
        _jv_r = joy_api_call("jmath_vec3f_distance", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_mul_vec2f_x(void* _jv_ctx, int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_x, int64_t b_y)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_col1_x);
        _jv_args[1] = joy_v_i64(a_col1_y);
        _jv_args[2] = joy_v_i64(a_col2_x);
        _jv_args[3] = joy_v_i64(a_col2_y);
        _jv_args[4] = joy_v_i64(b_x);
        _jv_args[5] = joy_v_i64(b_y);
        _jv_r = joy_api_call("jmath_mat22f_mul_vec2f_x", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_mul_vec2f_y(void* _jv_ctx, int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_x, int64_t b_y)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_col1_x);
        _jv_args[1] = joy_v_i64(a_col1_y);
        _jv_args[2] = joy_v_i64(a_col2_x);
        _jv_args[3] = joy_v_i64(a_col2_y);
        _jv_args[4] = joy_v_i64(b_x);
        _jv_args[5] = joy_v_i64(b_y);
        _jv_r = joy_api_call("jmath_mat22f_mul_vec2f_y", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_rotate_col1_x(void* _jv_ctx, int64_t angle)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(angle);
        _jv_r = joy_api_call("jmath_mat22f_rotate_col1_x", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_rotate_col1_y(void* _jv_ctx, int64_t angle)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(angle);
        _jv_r = joy_api_call("jmath_mat22f_rotate_col1_y", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_rotate_col2_x(void* _jv_ctx, int64_t angle)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(angle);
        _jv_r = joy_api_call("jmath_mat22f_rotate_col2_x", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_rotate_col2_y(void* _jv_ctx, int64_t angle)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(angle);
        _jv_r = joy_api_call("jmath_mat22f_rotate_col2_y", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_transpose_col1_x(void* _jv_ctx, int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(m_col1_x);
        _jv_args[1] = joy_v_i64(m_col1_y);
        _jv_args[2] = joy_v_i64(m_col2_x);
        _jv_args[3] = joy_v_i64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_transpose_col1_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_transpose_col1_y(void* _jv_ctx, int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(m_col1_x);
        _jv_args[1] = joy_v_i64(m_col1_y);
        _jv_args[2] = joy_v_i64(m_col2_x);
        _jv_args[3] = joy_v_i64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_transpose_col1_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_transpose_col2_x(void* _jv_ctx, int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(m_col1_x);
        _jv_args[1] = joy_v_i64(m_col1_y);
        _jv_args[2] = joy_v_i64(m_col2_x);
        _jv_args[3] = joy_v_i64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_transpose_col2_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_transpose_col2_y(void* _jv_ctx, int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(m_col1_x);
        _jv_args[1] = joy_v_i64(m_col1_y);
        _jv_args[2] = joy_v_i64(m_col2_x);
        _jv_args[3] = joy_v_i64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_transpose_col2_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_invert_col1_x(void* _jv_ctx, int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(m_col1_x);
        _jv_args[1] = joy_v_i64(m_col1_y);
        _jv_args[2] = joy_v_i64(m_col2_x);
        _jv_args[3] = joy_v_i64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_invert_col1_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_invert_col1_y(void* _jv_ctx, int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(m_col1_x);
        _jv_args[1] = joy_v_i64(m_col1_y);
        _jv_args[2] = joy_v_i64(m_col2_x);
        _jv_args[3] = joy_v_i64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_invert_col1_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_invert_col2_x(void* _jv_ctx, int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(m_col1_x);
        _jv_args[1] = joy_v_i64(m_col1_y);
        _jv_args[2] = joy_v_i64(m_col2_x);
        _jv_args[3] = joy_v_i64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_invert_col2_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_invert_col2_y(void* _jv_ctx, int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(m_col1_x);
        _jv_args[1] = joy_v_i64(m_col1_y);
        _jv_args[2] = joy_v_i64(m_col2_x);
        _jv_args[3] = joy_v_i64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_invert_col2_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_add_col1_x(void* _jv_ctx, int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_col1_x);
        _jv_args[1] = joy_v_i64(a_col1_y);
        _jv_args[2] = joy_v_i64(a_col2_x);
        _jv_args[3] = joy_v_i64(a_col2_y);
        _jv_args[4] = joy_v_i64(b_col1_x);
        _jv_args[5] = joy_v_i64(b_col1_y);
        _jv_args[6] = joy_v_i64(b_col2_x);
        _jv_args[7] = joy_v_i64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_add_col1_x", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_add_col1_y(void* _jv_ctx, int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_col1_x);
        _jv_args[1] = joy_v_i64(a_col1_y);
        _jv_args[2] = joy_v_i64(a_col2_x);
        _jv_args[3] = joy_v_i64(a_col2_y);
        _jv_args[4] = joy_v_i64(b_col1_x);
        _jv_args[5] = joy_v_i64(b_col1_y);
        _jv_args[6] = joy_v_i64(b_col2_x);
        _jv_args[7] = joy_v_i64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_add_col1_y", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_add_col2_x(void* _jv_ctx, int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_col1_x);
        _jv_args[1] = joy_v_i64(a_col1_y);
        _jv_args[2] = joy_v_i64(a_col2_x);
        _jv_args[3] = joy_v_i64(a_col2_y);
        _jv_args[4] = joy_v_i64(b_col1_x);
        _jv_args[5] = joy_v_i64(b_col1_y);
        _jv_args[6] = joy_v_i64(b_col2_x);
        _jv_args[7] = joy_v_i64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_add_col2_x", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_add_col2_y(void* _jv_ctx, int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_col1_x);
        _jv_args[1] = joy_v_i64(a_col1_y);
        _jv_args[2] = joy_v_i64(a_col2_x);
        _jv_args[3] = joy_v_i64(a_col2_y);
        _jv_args[4] = joy_v_i64(b_col1_x);
        _jv_args[5] = joy_v_i64(b_col1_y);
        _jv_args[6] = joy_v_i64(b_col2_x);
        _jv_args[7] = joy_v_i64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_add_col2_y", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_mul_col1_x(void* _jv_ctx, int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_col1_x);
        _jv_args[1] = joy_v_i64(a_col1_y);
        _jv_args[2] = joy_v_i64(a_col2_x);
        _jv_args[3] = joy_v_i64(a_col2_y);
        _jv_args[4] = joy_v_i64(b_col1_x);
        _jv_args[5] = joy_v_i64(b_col1_y);
        _jv_args[6] = joy_v_i64(b_col2_x);
        _jv_args[7] = joy_v_i64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_mul_col1_x", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_mul_col1_y(void* _jv_ctx, int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_col1_x);
        _jv_args[1] = joy_v_i64(a_col1_y);
        _jv_args[2] = joy_v_i64(a_col2_x);
        _jv_args[3] = joy_v_i64(a_col2_y);
        _jv_args[4] = joy_v_i64(b_col1_x);
        _jv_args[5] = joy_v_i64(b_col1_y);
        _jv_args[6] = joy_v_i64(b_col2_x);
        _jv_args[7] = joy_v_i64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_mul_col1_y", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_mul_col2_x(void* _jv_ctx, int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_col1_x);
        _jv_args[1] = joy_v_i64(a_col1_y);
        _jv_args[2] = joy_v_i64(a_col2_x);
        _jv_args[3] = joy_v_i64(a_col2_y);
        _jv_args[4] = joy_v_i64(b_col1_x);
        _jv_args[5] = joy_v_i64(b_col1_y);
        _jv_args[6] = joy_v_i64(b_col2_x);
        _jv_args[7] = joy_v_i64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_mul_col2_x", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_mul_col2_y(void* _jv_ctx, int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_col1_x);
        _jv_args[1] = joy_v_i64(a_col1_y);
        _jv_args[2] = joy_v_i64(a_col2_x);
        _jv_args[3] = joy_v_i64(a_col2_y);
        _jv_args[4] = joy_v_i64(b_col1_x);
        _jv_args[5] = joy_v_i64(b_col1_y);
        _jv_args[6] = joy_v_i64(b_col2_x);
        _jv_args[7] = joy_v_i64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_mul_col2_y", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_abs_col1_x(void* _jv_ctx, int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_col1_x);
        _jv_args[1] = joy_v_i64(a_col1_y);
        _jv_args[2] = joy_v_i64(a_col2_x);
        _jv_args[3] = joy_v_i64(a_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_abs_col1_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_abs_col1_y(void* _jv_ctx, int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_col1_x);
        _jv_args[1] = joy_v_i64(a_col1_y);
        _jv_args[2] = joy_v_i64(a_col2_x);
        _jv_args[3] = joy_v_i64(a_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_abs_col1_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_abs_col2_x(void* _jv_ctx, int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_col1_x);
        _jv_args[1] = joy_v_i64(a_col1_y);
        _jv_args[2] = joy_v_i64(a_col2_x);
        _jv_args[3] = joy_v_i64(a_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_abs_col2_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static int64_t shim_jmath_mat22f_abs_col2_y(void* _jv_ctx, int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_i64(a_col1_x);
        _jv_args[1] = joy_v_i64(a_col1_y);
        _jv_args[2] = joy_v_i64(a_col2_x);
        _jv_args[3] = joy_v_i64(a_col2_y);
        _jv_r = joy_api_call("jmath_mat22f_abs_col2_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I64) return (int64_t)0;
        return (int64_t) _jv_r.as.i64;
}

static float shim_jmath_ft_from_float(void* _jv_ctx, float x)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(x);
        _jv_r = joy_api_call("jmath_ft_from_float", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_to_float(void* _jv_ctx, float x)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(x);
        _jv_r = joy_api_call("jmath_ft_to_float", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_min_value(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("jmath_ft_min_value", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_max_value(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("jmath_ft_max_value", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_add(void* _jv_ctx, float a, float b)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a);
        _jv_args[1] = joy_v_f64(b);
        _jv_r = joy_api_call("jmath_ft_add", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_sub(void* _jv_ctx, float a, float b)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a);
        _jv_args[1] = joy_v_f64(b);
        _jv_r = joy_api_call("jmath_ft_sub", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_mul(void* _jv_ctx, float a, float b)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a);
        _jv_args[1] = joy_v_f64(b);
        _jv_r = joy_api_call("jmath_ft_mul", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_div(void* _jv_ctx, float a, float b)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a);
        _jv_args[1] = joy_v_f64(b);
        _jv_r = joy_api_call("jmath_ft_div", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_abs(void* _jv_ctx, float x)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(x);
        _jv_r = joy_api_call("jmath_ft_abs", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_sqrt(void* _jv_ctx, float x)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(x);
        _jv_r = joy_api_call("jmath_ft_sqrt", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_sin(void* _jv_ctx, float x)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(x);
        _jv_r = joy_api_call("jmath_ft_sin", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_cos(void* _jv_ctx, float x)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(x);
        _jv_r = joy_api_call("jmath_ft_cos", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_atan2(void* _jv_ctx, float y, float x)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(y);
        _jv_args[1] = joy_v_f64(x);
        _jv_r = joy_api_call("jmath_ft_atan2", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_zero(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("jmath_ft_zero", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_half(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("jmath_ft_half", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_one(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("jmath_ft_one", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_pi(void* _jv_ctx)
{
        joy_value_t* _jv_args = NULL;
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_r = joy_api_call("jmath_ft_pi", _jv_args, 0, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_max(void* _jv_ctx, float a, float b)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a);
        _jv_args[1] = joy_v_f64(b);
        _jv_r = joy_api_call("jmath_ft_max", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_min(void* _jv_ctx, float a, float b)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a);
        _jv_args[1] = joy_v_f64(b);
        _jv_r = joy_api_call("jmath_ft_min", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_pow2(void* _jv_ctx, float x)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(x);
        _jv_r = joy_api_call("jmath_ft_pow2", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static int32_t shim_jmath_ft_sign(void* _jv_ctx, float x)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(x);
        _jv_r = joy_api_call("jmath_ft_sign", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_I32) return (int32_t)0;
        return (int32_t) _jv_r.as.i32;
}

static float shim_jmath_ft_clamp(void* _jv_ctx, float a, float low, float high)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a);
        _jv_args[1] = joy_v_f64(low);
        _jv_args[2] = joy_v_f64(high);
        _jv_r = joy_api_call("jmath_ft_clamp", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_lerp(void* _jv_ctx, float a, float b, float t)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a);
        _jv_args[1] = joy_v_f64(b);
        _jv_args[2] = joy_v_f64(t);
        _jv_r = joy_api_call("jmath_ft_lerp", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_negate_x(void* _jv_ctx, float v_x, float v_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(v_x);
        _jv_args[1] = joy_v_f64(v_y);
        _jv_r = joy_api_call("jmath_vec2_negate_x", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_negate_y(void* _jv_ctx, float v_x, float v_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(v_x);
        _jv_args[1] = joy_v_f64(v_y);
        _jv_r = joy_api_call("jmath_vec2_negate_y", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static uint64_t shim_jmath_vec2_negate(void* _jv_ctx, float v_x, float v_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(v_x);
        _jv_args[1] = joy_v_f64(v_y);
        _jv_r = joy_api_call("jmath_vec2_negate", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (uint64_t)0;
        return (uint64_t) _jv_r.as.u64;
}

static float shim_jmath_vec2_dot(void* _jv_ctx, float a_x, float a_y, float b_x, float b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(b_x);
        _jv_args[3] = joy_v_f64(b_y);
        _jv_r = joy_api_call("jmath_vec2_dot", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_cross(void* _jv_ctx, float a_x, float a_y, float b_x, float b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(b_x);
        _jv_args[3] = joy_v_f64(b_y);
        _jv_r = joy_api_call("jmath_vec2_cross", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_cross2_x(void* _jv_ctx, float a_x, float a_y, float s)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(s);
        _jv_r = joy_api_call("jmath_vec2_cross2_x", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_cross2_y(void* _jv_ctx, float a_x, float a_y, float s)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(s);
        _jv_r = joy_api_call("jmath_vec2_cross2_y", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static uint64_t shim_jmath_vec2_cross2(void* _jv_ctx, float a_x, float a_y, float s)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(s);
        _jv_r = joy_api_call("jmath_vec2_cross2", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (uint64_t)0;
        return (uint64_t) _jv_r.as.u64;
}

static float shim_jmath_vec2_cross3_x(void* _jv_ctx, float s, float a_x, float a_y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(s);
        _jv_args[1] = joy_v_f64(a_x);
        _jv_args[2] = joy_v_f64(a_y);
        _jv_r = joy_api_call("jmath_vec2_cross3_x", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_cross3_y(void* _jv_ctx, float s, float a_x, float a_y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(s);
        _jv_args[1] = joy_v_f64(a_x);
        _jv_args[2] = joy_v_f64(a_y);
        _jv_r = joy_api_call("jmath_vec2_cross3_y", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static uint64_t shim_jmath_vec2_cross3(void* _jv_ctx, float s, float a_x, float a_y)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(s);
        _jv_args[1] = joy_v_f64(a_x);
        _jv_args[2] = joy_v_f64(a_y);
        _jv_r = joy_api_call("jmath_vec2_cross3", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (uint64_t)0;
        return (uint64_t) _jv_r.as.u64;
}

static float shim_jmath_vec2_add_x(void* _jv_ctx, float a_x, float a_y, float b_x, float b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(b_x);
        _jv_args[3] = joy_v_f64(b_y);
        _jv_r = joy_api_call("jmath_vec2_add_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_add_y(void* _jv_ctx, float a_x, float a_y, float b_x, float b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(b_x);
        _jv_args[3] = joy_v_f64(b_y);
        _jv_r = joy_api_call("jmath_vec2_add_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static uint64_t shim_jmath_vec2_add(void* _jv_ctx, float a_x, float a_y, float b_x, float b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(b_x);
        _jv_args[3] = joy_v_f64(b_y);
        _jv_r = joy_api_call("jmath_vec2_add", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (uint64_t)0;
        return (uint64_t) _jv_r.as.u64;
}

static float shim_jmath_vec2_sub_x(void* _jv_ctx, float a_x, float a_y, float b_x, float b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(b_x);
        _jv_args[3] = joy_v_f64(b_y);
        _jv_r = joy_api_call("jmath_vec2_sub_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_sub_y(void* _jv_ctx, float a_x, float a_y, float b_x, float b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(b_x);
        _jv_args[3] = joy_v_f64(b_y);
        _jv_r = joy_api_call("jmath_vec2_sub_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static uint64_t shim_jmath_vec2_sub(void* _jv_ctx, float a_x, float a_y, float b_x, float b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(b_x);
        _jv_args[3] = joy_v_f64(b_y);
        _jv_r = joy_api_call("jmath_vec2_sub", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (uint64_t)0;
        return (uint64_t) _jv_r.as.u64;
}

static float shim_jmath_vec2_scale_x(void* _jv_ctx, float a_x, float a_y, float s)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(s);
        _jv_r = joy_api_call("jmath_vec2_scale_x", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_scale_y(void* _jv_ctx, float a_x, float a_y, float s)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(s);
        _jv_r = joy_api_call("jmath_vec2_scale_y", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static uint64_t shim_jmath_vec2_scale(void* _jv_ctx, float a_x, float a_y, float s)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(s);
        _jv_r = joy_api_call("jmath_vec2_scale", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (uint64_t)0;
        return (uint64_t) _jv_r.as.u64;
}

static float shim_jmath_vec2_abs_x(void* _jv_ctx, float a_x, float a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_r = joy_api_call("jmath_vec2_abs_x", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_abs_y(void* _jv_ctx, float a_x, float a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_r = joy_api_call("jmath_vec2_abs_y", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static uint64_t shim_jmath_vec2_abs(void* _jv_ctx, float a_x, float a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_r = joy_api_call("jmath_vec2_abs", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (uint64_t)0;
        return (uint64_t) _jv_r.as.u64;
}

static float shim_jmath_vec2_length_squared(void* _jv_ctx, float a_x, float a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_r = joy_api_call("jmath_vec2_length_squared", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_length(void* _jv_ctx, float a_x, float a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_r = joy_api_call("jmath_vec2_length", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_normalize_x(void* _jv_ctx, float a_x, float a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_r = joy_api_call("jmath_vec2_normalize_x", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_normalize_y(void* _jv_ctx, float a_x, float a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_r = joy_api_call("jmath_vec2_normalize_y", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static uint64_t shim_jmath_vec2_normalize(void* _jv_ctx, float a_x, float a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_r = joy_api_call("jmath_vec2_normalize", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (uint64_t)0;
        return (uint64_t) _jv_r.as.u64;
}

static float shim_jmath_vec2_normal_x(void* _jv_ctx, float a_x, float a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_r = joy_api_call("jmath_vec2_normal_x", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_normal_y(void* _jv_ctx, float a_x, float a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_r = joy_api_call("jmath_vec2_normal_y", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static uint64_t shim_jmath_vec2_normal(void* _jv_ctx, float a_x, float a_y)
{
        joy_value_t _jv_args[2];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_r = joy_api_call("jmath_vec2_normal", _jv_args, 2, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (uint64_t)0;
        return (uint64_t) _jv_r.as.u64;
}

static float shim_jmath_vec2_rotate_x(void* _jv_ctx, float a_x, float a_y, float angle)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(angle);
        _jv_r = joy_api_call("jmath_vec2_rotate_x", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_rotate_y(void* _jv_ctx, float a_x, float a_y, float angle)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(angle);
        _jv_r = joy_api_call("jmath_vec2_rotate_y", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static uint64_t shim_jmath_vec2_rotate(void* _jv_ctx, float a_x, float a_y, float angle)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(angle);
        _jv_r = joy_api_call("jmath_vec2_rotate", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (uint64_t)0;
        return (uint64_t) _jv_r.as.u64;
}

static float shim_jmath_vec2_lerp_x(void* _jv_ctx, float a_x, float a_y, float b_x, float b_y, float t)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(b_x);
        _jv_args[3] = joy_v_f64(b_y);
        _jv_args[4] = joy_v_f64(t);
        _jv_r = joy_api_call("jmath_vec2_lerp_x", _jv_args, 5, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec2_lerp_y(void* _jv_ctx, float a_x, float a_y, float b_x, float b_y, float t)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(b_x);
        _jv_args[3] = joy_v_f64(b_y);
        _jv_args[4] = joy_v_f64(t);
        _jv_r = joy_api_call("jmath_vec2_lerp_y", _jv_args, 5, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static uint64_t shim_jmath_vec2_lerp(void* _jv_ctx, float a_x, float a_y, float b_x, float b_y, float t)
{
        joy_value_t _jv_args[5];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(b_x);
        _jv_args[3] = joy_v_f64(b_y);
        _jv_args[4] = joy_v_f64(t);
        _jv_r = joy_api_call("jmath_vec2_lerp", _jv_args, 5, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (uint64_t)0;
        return (uint64_t) _jv_r.as.u64;
}

static float shim_jmath_vec2_distance(void* _jv_ctx, float a_x, float a_y, float b_x, float b_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(b_x);
        _jv_args[3] = joy_v_f64(b_y);
        _jv_r = joy_api_call("jmath_vec2_distance", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_negate_x(void* _jv_ctx, float v_x, float v_y, float v_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(v_x);
        _jv_args[1] = joy_v_f64(v_y);
        _jv_args[2] = joy_v_f64(v_z);
        _jv_r = joy_api_call("jmath_vec3_negate_x", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_negate_y(void* _jv_ctx, float v_x, float v_y, float v_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(v_x);
        _jv_args[1] = joy_v_f64(v_y);
        _jv_args[2] = joy_v_f64(v_z);
        _jv_r = joy_api_call("jmath_vec3_negate_y", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_negate_z(void* _jv_ctx, float v_x, float v_y, float v_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(v_x);
        _jv_args[1] = joy_v_f64(v_y);
        _jv_args[2] = joy_v_f64(v_z);
        _jv_r = joy_api_call("jmath_vec3_negate_z", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_add_x(void* _jv_ctx, float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(b_x);
        _jv_args[4] = joy_v_f64(b_y);
        _jv_args[5] = joy_v_f64(b_z);
        _jv_r = joy_api_call("jmath_vec3_add_x", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_add_y(void* _jv_ctx, float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(b_x);
        _jv_args[4] = joy_v_f64(b_y);
        _jv_args[5] = joy_v_f64(b_z);
        _jv_r = joy_api_call("jmath_vec3_add_y", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_add_z(void* _jv_ctx, float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(b_x);
        _jv_args[4] = joy_v_f64(b_y);
        _jv_args[5] = joy_v_f64(b_z);
        _jv_r = joy_api_call("jmath_vec3_add_z", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_sub_x(void* _jv_ctx, float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(b_x);
        _jv_args[4] = joy_v_f64(b_y);
        _jv_args[5] = joy_v_f64(b_z);
        _jv_r = joy_api_call("jmath_vec3_sub_x", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_sub_y(void* _jv_ctx, float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(b_x);
        _jv_args[4] = joy_v_f64(b_y);
        _jv_args[5] = joy_v_f64(b_z);
        _jv_r = joy_api_call("jmath_vec3_sub_y", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_sub_z(void* _jv_ctx, float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(b_x);
        _jv_args[4] = joy_v_f64(b_y);
        _jv_args[5] = joy_v_f64(b_z);
        _jv_r = joy_api_call("jmath_vec3_sub_z", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_scale_x(void* _jv_ctx, float a_x, float a_y, float a_z, float s)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(s);
        _jv_r = joy_api_call("jmath_vec3_scale_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_scale_y(void* _jv_ctx, float a_x, float a_y, float a_z, float s)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(s);
        _jv_r = joy_api_call("jmath_vec3_scale_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_scale_z(void* _jv_ctx, float a_x, float a_y, float a_z, float s)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(s);
        _jv_r = joy_api_call("jmath_vec3_scale_z", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_abs_x(void* _jv_ctx, float a_x, float a_y, float a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_r = joy_api_call("jmath_vec3_abs_x", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_abs_y(void* _jv_ctx, float a_x, float a_y, float a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_r = joy_api_call("jmath_vec3_abs_y", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_abs_z(void* _jv_ctx, float a_x, float a_y, float a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_r = joy_api_call("jmath_vec3_abs_z", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_dot(void* _jv_ctx, float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(b_x);
        _jv_args[4] = joy_v_f64(b_y);
        _jv_args[5] = joy_v_f64(b_z);
        _jv_r = joy_api_call("jmath_vec3_dot", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_cross_x(void* _jv_ctx, float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(b_x);
        _jv_args[4] = joy_v_f64(b_y);
        _jv_args[5] = joy_v_f64(b_z);
        _jv_r = joy_api_call("jmath_vec3_cross_x", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_cross_y(void* _jv_ctx, float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(b_x);
        _jv_args[4] = joy_v_f64(b_y);
        _jv_args[5] = joy_v_f64(b_z);
        _jv_r = joy_api_call("jmath_vec3_cross_y", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_cross_z(void* _jv_ctx, float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(b_x);
        _jv_args[4] = joy_v_f64(b_y);
        _jv_args[5] = joy_v_f64(b_z);
        _jv_r = joy_api_call("jmath_vec3_cross_z", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_square_length(void* _jv_ctx, float a_x, float a_y, float a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_r = joy_api_call("jmath_vec3_square_length", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_length(void* _jv_ctx, float a_x, float a_y, float a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_r = joy_api_call("jmath_vec3_length", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_normalize_x(void* _jv_ctx, float a_x, float a_y, float a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_r = joy_api_call("jmath_vec3_normalize_x", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_normalize_y(void* _jv_ctx, float a_x, float a_y, float a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_r = joy_api_call("jmath_vec3_normalize_y", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_normalize_z(void* _jv_ctx, float a_x, float a_y, float a_z)
{
        joy_value_t _jv_args[3];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_r = joy_api_call("jmath_vec3_normalize_z", _jv_args, 3, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_lerp_x(void* _jv_ctx, float a_x, float a_y, float a_z, float b_x, float b_y, float b_z, float t)
{
        joy_value_t _jv_args[7];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(b_x);
        _jv_args[4] = joy_v_f64(b_y);
        _jv_args[5] = joy_v_f64(b_z);
        _jv_args[6] = joy_v_f64(t);
        _jv_r = joy_api_call("jmath_vec3_lerp_x", _jv_args, 7, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_lerp_y(void* _jv_ctx, float a_x, float a_y, float a_z, float b_x, float b_y, float b_z, float t)
{
        joy_value_t _jv_args[7];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(b_x);
        _jv_args[4] = joy_v_f64(b_y);
        _jv_args[5] = joy_v_f64(b_z);
        _jv_args[6] = joy_v_f64(t);
        _jv_r = joy_api_call("jmath_vec3_lerp_y", _jv_args, 7, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_lerp_z(void* _jv_ctx, float a_x, float a_y, float a_z, float b_x, float b_y, float b_z, float t)
{
        joy_value_t _jv_args[7];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(b_x);
        _jv_args[4] = joy_v_f64(b_y);
        _jv_args[5] = joy_v_f64(b_z);
        _jv_args[6] = joy_v_f64(t);
        _jv_r = joy_api_call("jmath_vec3_lerp_z", _jv_args, 7, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_distance(void* _jv_ctx, float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(b_x);
        _jv_args[4] = joy_v_f64(b_y);
        _jv_args[5] = joy_v_f64(b_z);
        _jv_r = joy_api_call("jmath_vec3_distance", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_mul_vec2_x(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_x, float b_y)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_args[4] = joy_v_f64(b_x);
        _jv_args[5] = joy_v_f64(b_y);
        _jv_r = joy_api_call("jmath_mat22_mul_vec2_x", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_mul_vec2_y(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_x, float b_y)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_args[4] = joy_v_f64(b_x);
        _jv_args[5] = joy_v_f64(b_y);
        _jv_r = joy_api_call("jmath_mat22_mul_vec2_y", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static uint64_t shim_jmath_mat22_mul_vec2(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_x, float b_y)
{
        joy_value_t _jv_args[6];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_args[4] = joy_v_f64(b_x);
        _jv_args[5] = joy_v_f64(b_y);
        _jv_r = joy_api_call("jmath_mat22_mul_vec2", _jv_args, 6, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_U64) return (uint64_t)0;
        return (uint64_t) _jv_r.as.u64;
}

static float shim_jmath_mat22_rotate_col1_x(void* _jv_ctx, float angle)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(angle);
        _jv_r = joy_api_call("jmath_mat22_rotate_col1_x", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_rotate_col1_y(void* _jv_ctx, float angle)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(angle);
        _jv_r = joy_api_call("jmath_mat22_rotate_col1_y", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_rotate_col2_x(void* _jv_ctx, float angle)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(angle);
        _jv_r = joy_api_call("jmath_mat22_rotate_col2_x", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_rotate_col2_y(void* _jv_ctx, float angle)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(angle);
        _jv_r = joy_api_call("jmath_mat22_rotate_col2_y", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_transpose_col1_x(void* _jv_ctx, float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(m_col1_x);
        _jv_args[1] = joy_v_f64(m_col1_y);
        _jv_args[2] = joy_v_f64(m_col2_x);
        _jv_args[3] = joy_v_f64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22_transpose_col1_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_transpose_col1_y(void* _jv_ctx, float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(m_col1_x);
        _jv_args[1] = joy_v_f64(m_col1_y);
        _jv_args[2] = joy_v_f64(m_col2_x);
        _jv_args[3] = joy_v_f64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22_transpose_col1_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_transpose_col2_x(void* _jv_ctx, float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(m_col1_x);
        _jv_args[1] = joy_v_f64(m_col1_y);
        _jv_args[2] = joy_v_f64(m_col2_x);
        _jv_args[3] = joy_v_f64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22_transpose_col2_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_transpose_col2_y(void* _jv_ctx, float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(m_col1_x);
        _jv_args[1] = joy_v_f64(m_col1_y);
        _jv_args[2] = joy_v_f64(m_col2_x);
        _jv_args[3] = joy_v_f64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22_transpose_col2_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_invert_col1_x(void* _jv_ctx, float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(m_col1_x);
        _jv_args[1] = joy_v_f64(m_col1_y);
        _jv_args[2] = joy_v_f64(m_col2_x);
        _jv_args[3] = joy_v_f64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22_invert_col1_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_invert_col1_y(void* _jv_ctx, float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(m_col1_x);
        _jv_args[1] = joy_v_f64(m_col1_y);
        _jv_args[2] = joy_v_f64(m_col2_x);
        _jv_args[3] = joy_v_f64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22_invert_col1_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_invert_col2_x(void* _jv_ctx, float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(m_col1_x);
        _jv_args[1] = joy_v_f64(m_col1_y);
        _jv_args[2] = joy_v_f64(m_col2_x);
        _jv_args[3] = joy_v_f64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22_invert_col2_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_invert_col2_y(void* _jv_ctx, float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(m_col1_x);
        _jv_args[1] = joy_v_f64(m_col1_y);
        _jv_args[2] = joy_v_f64(m_col2_x);
        _jv_args[3] = joy_v_f64(m_col2_y);
        _jv_r = joy_api_call("jmath_mat22_invert_col2_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_add_col1_x(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_args[4] = joy_v_f64(b_col1_x);
        _jv_args[5] = joy_v_f64(b_col1_y);
        _jv_args[6] = joy_v_f64(b_col2_x);
        _jv_args[7] = joy_v_f64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22_add_col1_x", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_add_col1_y(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_args[4] = joy_v_f64(b_col1_x);
        _jv_args[5] = joy_v_f64(b_col1_y);
        _jv_args[6] = joy_v_f64(b_col2_x);
        _jv_args[7] = joy_v_f64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22_add_col1_y", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_add_col2_x(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_args[4] = joy_v_f64(b_col1_x);
        _jv_args[5] = joy_v_f64(b_col1_y);
        _jv_args[6] = joy_v_f64(b_col2_x);
        _jv_args[7] = joy_v_f64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22_add_col2_x", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_add_col2_y(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_args[4] = joy_v_f64(b_col1_x);
        _jv_args[5] = joy_v_f64(b_col1_y);
        _jv_args[6] = joy_v_f64(b_col2_x);
        _jv_args[7] = joy_v_f64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22_add_col2_y", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_mul_col1_x(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_args[4] = joy_v_f64(b_col1_x);
        _jv_args[5] = joy_v_f64(b_col1_y);
        _jv_args[6] = joy_v_f64(b_col2_x);
        _jv_args[7] = joy_v_f64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22_mul_col1_x", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_mul_col1_y(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_args[4] = joy_v_f64(b_col1_x);
        _jv_args[5] = joy_v_f64(b_col1_y);
        _jv_args[6] = joy_v_f64(b_col2_x);
        _jv_args[7] = joy_v_f64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22_mul_col1_y", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_mul_col2_x(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_args[4] = joy_v_f64(b_col1_x);
        _jv_args[5] = joy_v_f64(b_col1_y);
        _jv_args[6] = joy_v_f64(b_col2_x);
        _jv_args[7] = joy_v_f64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22_mul_col2_x", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_mul_col2_y(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        joy_value_t _jv_args[8];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_args[4] = joy_v_f64(b_col1_x);
        _jv_args[5] = joy_v_f64(b_col1_y);
        _jv_args[6] = joy_v_f64(b_col2_x);
        _jv_args[7] = joy_v_f64(b_col2_y);
        _jv_r = joy_api_call("jmath_mat22_mul_col2_y", _jv_args, 8, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_abs_col1_x(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_r = joy_api_call("jmath_mat22_abs_col1_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_abs_col1_y(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_r = joy_api_call("jmath_mat22_abs_col1_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_abs_col2_x(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_r = joy_api_call("jmath_mat22_abs_col2_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_mat22_abs_col2_y(void* _jv_ctx, float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_col1_x);
        _jv_args[1] = joy_v_f64(a_col1_y);
        _jv_args[2] = joy_v_f64(a_col2_x);
        _jv_args[3] = joy_v_f64(a_col2_y);
        _jv_r = joy_api_call("jmath_mat22_abs_col2_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_div_x(void* _jv_ctx, float a_x, float a_y, float a_z, float s)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(s);
        _jv_r = joy_api_call("jmath_vec3_div_x", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_div_y(void* _jv_ctx, float a_x, float a_y, float a_z, float s)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(s);
        _jv_r = joy_api_call("jmath_vec3_div_y", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_vec3_div_z(void* _jv_ctx, float a_x, float a_y, float a_z, float s)
{
        joy_value_t _jv_args[4];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(a_x);
        _jv_args[1] = joy_v_f64(a_y);
        _jv_args[2] = joy_v_f64(a_z);
        _jv_args[3] = joy_v_f64(s);
        _jv_r = joy_api_call("jmath_vec3_div_z", _jv_args, 4, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

static float shim_jmath_ft_radians(void* _jv_ctx, float degrees)
{
        joy_value_t _jv_args[1];
        joy_value_t _jv_r;
        joy_status_t _jv_st = JOY_OK;
        (void)_jv_ctx;
        _jv_args[0] = joy_v_f64(degrees);
        _jv_r = joy_api_call("jmath_ft_radians", _jv_args, 1, &_jv_st);
        if (_jv_st != JOY_OK || _jv_r.kind != JOY_V_F64) return (float)0;
        return (float) _jv_r.as.f64;
}

int joy_api_vtable_init(JoyApi* api, void* ctx)
{
        if (!api) return -1;
        memset(api, 0, sizeof(*api));
        api->abi_version = JOY_API_VTABLE_ABI;
        api->size = (uint32_t)sizeof(JoyApi);
        api->ctx = ctx;
        api->LcgRandomCreate = shim_joy_core_lcg_random_create;
        api->LcgRandomDestroy = shim_joy_core_lcg_random_destroy;
        api->LcgRandomNext = shim_joy_core_lcg_random_next;
        api->LcgRandomSetState = shim_joy_core_lcg_random_set_state;
        api->JpmConfigAddSpawn = shim_jpm_config_add_spawn;
        api->JpmWaveSeed = shim_jpm_wave_seed;
        api->SdbmHash = shim_joy_core_sdbm_hash;
        api->Fnv1aHash = shim_joy_core_fnv1a_hash;
        api->JenkinsHash = shim_joy_core_jenkins_hash;
        api->Murmur3Hash = shim_joy_core_murmur3_hash;
        api->KrHash = shim_joy_core_kr_hash;
        api->JtimerDestroy = shim_joy_scene_jtimer_destroy;
        api->JtimerTrigger = shim_joy_scene_jtimer_trigger;
        api->JtimerSetInterval = shim_joy_scene_jtimer_set_interval;
        api->JtimerSetActive = shim_joy_scene_jtimer_set_active;
        api->JtimerReset = shim_joy_scene_jtimer_reset;
        api->JtimerGetElapsed = shim_joy_scene_jtimer_get_elapsed;
        api->InitEnv = shim_joy_core_init_env;
        api->QuitEnv = shim_joy_core_quit_env;
        api->GetTicks = shim_joy_core_get_ticks;
        api->CreateWindow = shim_joy_core_create_window;
        api->DestroyWindow = shim_joy_core_destroy_window;
        api->WindowSetFullscreen = shim_joy_core_window_set_fullscreen;
        api->WindowGetFullscreen = shim_joy_core_window_get_fullscreen;
        api->WindowSetIcon = shim_joy_core_window_set_icon;
        api->PushUserEvent = shim_joy_core_push_user_event;
        api->PushQuitEvent = shim_joy_core_push_quit_event;
        api->LogTitle = shim_joy_core_log_title;
        api->EnvSet = shim_joy_core_env_set;
        api->EnvDestroy = shim_joy_core_env_destroy;
        api->SimpleFpsDestory = shim_joy_core_simple_fps_destory;
        api->SimpleFpsUpdate = shim_joy_core_simple_fps_update;
        api->SimpleFpsValue = shim_joy_core_simple_fps_value;
        api->GameTimerInit = shim_joy_scene_game_timer_init;
        api->GameTimerUpdate = shim_joy_scene_game_timer_update;
        api->GameTimerGetDeltaTime = shim_joy_scene_game_timer_get_delta_time;
        api->GameTimerGetUnscaledDeltaTime = shim_joy_scene_game_timer_get_unscaled_delta_time;
        api->GameTimerSetTimeScale = shim_joy_scene_game_timer_set_time_scale;
        api->GameTimerGetTimeScale = shim_joy_scene_game_timer_get_time_scale;
        api->GameTimerSetTargetFps = shim_joy_scene_game_timer_set_target_fps;
        api->GameTimerGetTargetFps = shim_joy_scene_game_timer_get_target_fps;
        api->SetClipboardText = shim_joy_core_set_clipboard_text;
        api->SysInitNetenv = shim_sys_init_netenv;
        api->SysReleaseNetenv = shim_sys_release_netenv;
        api->SysAnyaddr = shim_sys_anyaddr;
        api->SysClosesocket = shim_sys_closesocket;
        api->SysSetSockSndtimeo = shim_sys_set_sock_sndtimeo;
        api->SysSetSockRcvtimeo = shim_sys_set_sock_rcvtimeo;
        api->SysSetSockAccpettimeo = shim_sys_set_sock_accpettimeo;
        api->SysSetSockBroadcast = shim_sys_set_sock_broadcast;
        api->SysSetSockReuseaddr = shim_sys_set_sock_reuseaddr;
        api->SysTcp = shim_sys_tcp;
        api->SysUdp = shim_sys_udp;
        api->SysBind = shim_sys_bind;
        api->SysConnect = shim_sys_connect;
        api->SysListen = shim_sys_listen;
        api->SysSend = shim_sys_send;
        api->SysSendto = shim_sys_sendto;
        api->SysCurrentTime = shim_sys_current_time;
        api->SysSleep = shim_sys_sleep;
        api->SysInstallCrashHandlers = shim_sys_install_crash_handlers;
        api->IocpServerDestroy = shim_iocp_server_destroy;
        api->IocpProcessEvents = shim_iocp_process_events;
        api->FontCreate = shim_font_create;
        api->FontDestroy = shim_font_destroy;
        api->FontGetScale = shim_font_get_scale;
        api->FontGetSize = shim_font_get_size;
        api->FontGetAscent = shim_font_get_ascent;
        api->R2dCreate = shim_r2d_create;
        api->R2dDestroy = shim_r2d_destroy;
        api->R2dSetColor = shim_r2d_set_color;
        api->R2dBegin = shim_r2d_begin;
        api->R2dEnd = shim_r2d_end;
        api->R2dSetLogicalPresentation = shim_r2d_set_logical_presentation;
        api->R2dSetLogicalPresentationMode = shim_r2d_set_logical_presentation_mode;
        api->R2dSetVsync = shim_r2d_set_vsync;
        api->R2dSetScale = shim_r2d_set_scale;
        api->R2dSetColorf = shim_r2d_set_colorf;
        api->R2dSetBlendMode = shim_r2d_set_blend_mode;
        api->R2dDrawPoint = shim_r2d_draw_point;
        api->R2dSetClipRect = shim_r2d_set_clip_rect;
        api->R2dClearClip = shim_r2d_clear_clip;
        api->R2dPushClipRect = shim_r2d_push_clip_rect;
        api->R2dPopClipRect = shim_r2d_pop_clip_rect;
        api->R2dResetClipStack = shim_r2d_reset_clip_stack;
        api->R2dPointInClip = shim_r2d_point_in_clip;
        api->ShapeDrawLine = shim_shape_draw_line;
        api->ShapeDrawRectangle = shim_shape_draw_rectangle;
        api->ImageCreate = shim_image_create;
        api->ImageDestroy = shim_image_destroy;
        api->ImageSetAlpha = shim_image_set_alpha;
        api->ImageSetBlendMode = shim_image_set_blend_mode;
        api->ImageSetScaleMode = shim_image_set_scale_mode;
        api->TextUpdatex = shim_text_updatex;
        api->TextDestroy = shim_text_destroy;
        api->JtextPrint = shim_jtext_print;
        api->JtextPrintx = shim_jtext_printx;
        api->R2dDrawPointF = shim_r2d_draw_point_f;
        api->ShapeDrawRectangleF = shim_shape_draw_rectangle_f;
        api->ShapeDrawLineF = shim_shape_draw_line_f;
        api->JtextPrintf = shim_jtext_printf;
        api->JtextPrintfScaled = shim_jtext_printf_scaled;
        api->TextGetWidth = shim_text_get_width;
        api->TextGetHeight = shim_text_get_height;
        api->JtextCacheClear = shim_jtext_cache_clear;
        api->JtextCacheSetLimit = shim_jtext_cache_set_limit;
        api->JtextCacheGetCount = shim_jtext_cache_get_count;
        api->JtextCacheSweep = shim_jtext_cache_sweep;
        api->AnimationDestroy = shim_animation_destroy;
        api->AnimationReset = shim_animation_reset;
        api->AnimationAddClip = shim_animation_add_clip;
        api->AnimationIsFinished = shim_animation_is_finished;
        api->AnimationSetPosition = shim_animation_set_position;
        api->AnimationSetScale = shim_animation_set_scale;
        api->AnimationSetRotation = shim_animation_set_rotation;
        api->AnimationUpdate = shim_animation_update;
        api->R3dDestroy = shim_r3d_destroy;
        api->R3dBegin = shim_r3d_begin;
        api->R3dEnd = shim_r3d_end;
        api->R3dGetDrawCalls = shim_r3d_get_draw_calls;
        api->R3dSetClearColor = shim_r3d_set_clear_color;
        api->R3dMeshDestroy = shim_r3d_mesh_destroy;
        api->R3dFontInit = shim_r3d_font_init;
        api->R3dFontDraw2d = shim_r3d_font_draw_2d;
        api->R3dFontCacheSetLimit = shim_r3d_font_cache_set_limit;
        api->R3dFontCacheGetCount = shim_r3d_font_cache_get_count;
        api->R3dFontCacheClear = shim_r3d_font_cache_clear;
        api->R3dFontCacheSweep = shim_r3d_font_cache_sweep;
        api->R3dSetLogicalPresentation = shim_r3d_set_logical_presentation;
        api->R3dSetLogicalPresentationMode = shim_r3d_set_logical_presentation_mode;
        api->R3dButtonDraw = shim_r3d_button_draw;
        api->R3dButtonDrawCircle = shim_r3d_button_draw_circle;
        api->R3dButtonIsHovered = shim_r3d_button_is_hovered;
        api->R3dButtonIsPressed = shim_r3d_button_is_pressed;
        api->R3dButtonRelease = shim_r3d_button_release;
        api->R3dButtonDestroy = shim_r3d_button_destroy;
        api->R3dLabelSetText = shim_r3d_label_set_text;
        api->R3dLabelDraw = shim_r3d_label_draw;
        api->R3dLabelDestroy = shim_r3d_label_destroy;
        api->R3dInputSetText = shim_r3d_input_set_text;
        api->R3dInputIsHovered = shim_r3d_input_is_hovered;
        api->R3dInputFocus = shim_r3d_input_focus;
        api->R3dInputDefocus = shim_r3d_input_defocus;
        api->R3dInputIsFocused = shim_r3d_input_is_focused;
        api->R3dInputStartTextInput = shim_r3d_input_start_text_input;
        api->R3dInputStopTextInput = shim_r3d_input_stop_text_input;
        api->R3dInputInsert = shim_r3d_input_insert;
        api->R3dInputBackspace = shim_r3d_input_backspace;
        api->R3dInputDelete = shim_r3d_input_delete;
        api->R3dInputCursorLeft = shim_r3d_input_cursor_left;
        api->R3dInputCursorRight = shim_r3d_input_cursor_right;
        api->R3dInputCursorHome = shim_r3d_input_cursor_home;
        api->R3dInputCursorEnd = shim_r3d_input_cursor_end;
        api->R3dInputClick = shim_r3d_input_click;
        api->R3dInputSubmit = shim_r3d_input_submit;
        api->R3dInputDraw = shim_r3d_input_draw;
        api->R3dInputDestroy = shim_r3d_input_destroy;
        api->R3dComboboxInputDestroy = shim_r3d_combobox_input_destroy;
        api->R3dComboboxInputAddItem = shim_r3d_combobox_input_add_item;
        api->R3dComboboxInputClearItems = shim_r3d_combobox_input_clear_items;
        api->R3dComboboxInputSetText = shim_r3d_combobox_input_set_text;
        api->R3dComboboxInputHitItem = shim_r3d_combobox_input_hit_item;
        api->R3dComboboxInputDraw = shim_r3d_combobox_input_draw;
        api->R3dComboboxInputDrawDropdown = shim_r3d_combobox_input_draw_dropdown;
        api->R3dSliderDestroy = shim_r3d_slider_destroy;
        api->R3dSliderSetValue = shim_r3d_slider_set_value;
        api->R3dSliderGetValue = shim_r3d_slider_get_value;
        api->R3dSliderDraw = shim_r3d_slider_draw;
        api->R3dSliderIsPressed = shim_r3d_slider_is_pressed;
        api->R3dSliderSetPressed = shim_r3d_slider_set_pressed;
        api->R3dSliderDragMove = shim_r3d_slider_drag_move;
        api->R3dSliderIsHovered = shim_r3d_slider_is_hovered;
        api->R3dSliderSetHovered = shim_r3d_slider_set_hovered;
        api->R3dCheckboxDestroy = shim_r3d_checkbox_destroy;
        api->R3dCheckboxToggle = shim_r3d_checkbox_toggle;
        api->R3dCheckboxIsPressed = shim_r3d_checkbox_is_pressed;
        api->R3dCheckboxSetHovered = shim_r3d_checkbox_set_hovered;
        api->R3dCheckboxIsHovered = shim_r3d_checkbox_is_hovered;
        api->R3dCheckboxSetColors = shim_r3d_checkbox_set_colors;
        api->R3dCheckboxDraw = shim_r3d_checkbox_draw;
        api->R3dDrawGrid = shim_r3d_draw_grid;
        api->JanimatorDestroy = shim_janimator_destroy;
        api->JanimatorSetFloat = shim_janimator_set_float;
        api->JanimatorSetInt = shim_janimator_set_int;
        api->JanimatorSetBool = shim_janimator_set_bool;
        api->JanimatorSetTrigger = shim_janimator_set_trigger;
        api->JanimatorGetFloat = shim_janimator_get_float;
        api->JanimatorGetInt = shim_janimator_get_int;
        api->JanimatorGetBool = shim_janimator_get_bool;
        api->JanimatorAddState = shim_janimator_add_state;
        api->JanimatorSetDefaultState = shim_janimator_set_default_state;
        api->JanimatorStateAddClip = shim_janimator_state_add_clip;
        api->JanimatorStateAddColor = shim_janimator_state_add_color;
        api->JanimatorStateSetLoop = shim_janimator_state_set_loop;
        api->JanimatorStateSetSpeed = shim_janimator_state_set_speed;
        api->JanimatorStateSetBlend = shim_janimator_state_set_blend;
        api->JanimatorStateAddMotion = shim_janimator_state_add_motion;
        api->JanimatorAddTransition = shim_janimator_add_transition;
        api->JanimatorAddAnyTransition = shim_janimator_add_any_transition;
        api->JanimatorTransitionAddCond = shim_janimator_transition_add_cond;
        api->JanimatorTransitionSetExitTime = shim_janimator_transition_set_exit_time;
        api->JanimatorUpdate = shim_janimator_update;
        api->JanimatorDraw = shim_janimator_draw;
        api->JanimatorSetFlipH = shim_janimator_set_flip_h;
        api->LabelDestroy = shim_label_destroy;
        api->LabelDraw = shim_label_draw;
        api->ButtonDestroy = shim_button_destroy;
        api->ButtonSetPosition = shim_button_set_position;
        api->ButtonSetSize = shim_button_set_size;
        api->ButtonSetNormalColor = shim_button_set_normal_color;
        api->ButtonSetHoverColor = shim_button_set_hover_color;
        api->ButtonSetPressedColor = shim_button_set_pressed_color;
        api->ButtonSetImage = shim_button_set_image;
        api->ButtonSetPressedImage = shim_button_set_pressed_image;
        api->ButtonSetTextx = shim_button_set_textx;
        api->ButtonIsHovered = shim_button_is_hovered;
        api->ButtonIsPressed = shim_button_is_pressed;
        api->ButtonSetHovered = shim_button_set_hovered;
        api->ButtonSetPressed = shim_button_set_pressed;
        api->ButtonGetPressed = shim_button_get_pressed;
        api->ButtonSetTextOffset = shim_button_set_text_offset;
        api->ButtonDraw = shim_button_draw;
        api->ScrollviewDestroy = shim_scrollview_destroy;
        api->ScrollviewSetViewPx = shim_scrollview_set_view_px;
        api->ScrollviewSetContent = shim_scrollview_set_content;
        api->ScrollviewSetOffset = shim_scrollview_set_offset;
        api->ScrollviewClamp = shim_scrollview_clamp;
        api->ScrollviewGetOffsetX = shim_scrollview_get_offset_x;
        api->ScrollviewGetOffsetY = shim_scrollview_get_offset_y;
        api->ScrollviewSetShowBar = shim_scrollview_set_show_bar;
        api->ScrollviewDragging = shim_scrollview_dragging;
        api->ScrollviewHit = shim_scrollview_hit;
        api->ScrollviewBegin = shim_scrollview_begin;
        api->ScrollviewEnd = shim_scrollview_end;
        api->ComboboxDestroy = shim_combobox_destroy;
        api->ComboboxAddItem = shim_combobox_add_item;
        api->ComboboxIsPressed = shim_combobox_is_pressed;
        api->ComboboxIsOpen = shim_combobox_is_open;
        api->ComboboxSetOpen = shim_combobox_set_open;
        api->ComboboxHitItem = shim_combobox_hit_item;
        api->ComboboxSetSelected = shim_combobox_set_selected;
        api->ComboboxGetSelected = shim_combobox_get_selected;
        api->ComboboxDraw = shim_combobox_draw;
        api->ComboboxInputDestroy = shim_combobox_input_destroy;
        api->ComboboxInputAddItem = shim_combobox_input_add_item;
        api->ComboboxInputClearItems = shim_combobox_input_clear_items;
        api->ComboboxInputSetText = shim_combobox_input_set_text;
        api->ComboboxInputDraw = shim_combobox_input_draw;
        api->ComboboxInputDrawDropdown = shim_combobox_input_draw_dropdown;
        api->ComboboxInputIsOpen = shim_combobox_input_is_open;
        api->ComboboxInputSetPopupNode = shim_combobox_input_set_popup_node;
        api->ComboboxInputHitItem = shim_combobox_input_hit_item;
        api->ComboboxInputSetOpen = shim_combobox_input_set_open;
        api->DatagridSetheaders = shim_datagrid_setheaders;
        api->DatagridSetheader = shim_datagrid_setheader;
        api->DatagridSetcell = shim_datagrid_setcell;
        api->DatagridDestroy = shim_datagrid_destroy;
        api->DatagridDraw = shim_datagrid_draw;
        api->DatagridHitRow = shim_datagrid_hit_row;
        api->DatagridScroll = shim_datagrid_scroll;
        api->DatagridSetSelectedRow = shim_datagrid_set_selected_row;
        api->RadioDestroy = shim_radio_destroy;
        api->RadioIsPressed = shim_radio_is_pressed;
        api->RadioSetHovered = shim_radio_set_hovered;
        api->CheckboxDestroy = shim_checkbox_destroy;
        api->CheckboxSetColors = shim_checkbox_set_colors;
        api->CheckboxSetR2d = shim_checkbox_set_r2d;
        api->CheckboxSetPosition = shim_checkbox_set_position;
        api->CheckboxSetTextColor = shim_checkbox_set_text_color;
        api->CheckboxToggle = shim_checkbox_toggle;
        api->CheckboxIsPressed = shim_checkbox_is_pressed;
        api->CheckboxSetHovered = shim_checkbox_set_hovered;
        api->JoystickDestroy = shim_joystick_destroy;
        api->JoystickSetTextures = shim_joystick_set_textures;
        api->JoystickSetVector = shim_joystick_set_vector;
        api->JoystickDraw = shim_joystick_draw;
        api->JoystickReset = shim_joystick_reset;
        api->JoystickSetPosition = shim_joystick_set_position;
        api->JoystickSetRadius = shim_joystick_set_radius;
        api->JoystickIsDragging = shim_joystick_is_dragging;
        api->JoystickTouchDown = shim_joystick_touch_down;
        api->JoystickTouchMove = shim_joystick_touch_move;
        api->JoystickTouchUp = shim_joystick_touch_up;
        api->JoystickGetMagnitude = shim_joystick_get_magnitude;
        api->InputboxDestroy = shim_inputbox_destroy;
        api->InputboxSetText = shim_inputbox_set_text;
        api->InputboxDraw = shim_inputbox_draw;
        api->InputboxIsPressed = shim_inputbox_is_pressed;
        api->InputboxIsHovered = shim_inputbox_is_hovered;
        api->InputboxFocus = shim_inputbox_focus;
        api->InputboxDefocus = shim_inputbox_defocus;
        api->InputboxBlur = shim_inputbox_blur;
        api->InputboxInsert = shim_inputbox_insert;
        api->InputboxBackspace = shim_inputbox_backspace;
        api->InputboxDelete = shim_inputbox_delete;
        api->InputboxCursorLeft = shim_inputbox_cursor_left;
        api->InputboxCursorRight = shim_inputbox_cursor_right;
        api->InputboxCursorHome = shim_inputbox_cursor_home;
        api->InputboxCursorEnd = shim_inputbox_cursor_end;
        api->InputboxSelectLeft = shim_inputbox_select_left;
        api->InputboxSelectRight = shim_inputbox_select_right;
        api->InputboxSelectHome = shim_inputbox_select_home;
        api->InputboxSelectEnd = shim_inputbox_select_end;
        api->InputboxSelectAll = shim_inputbox_select_all;
        api->InputboxPaste = shim_inputbox_paste;
        api->InputboxCopy = shim_inputbox_copy;
        api->InputboxCut = shim_inputbox_cut;
        api->InputboxIsFocused = shim_inputbox_is_focused;
        api->SliderDestroy = shim_slider_destroy;
        api->SliderSetValue = shim_slider_set_value;
        api->SliderGetValue = shim_slider_get_value;
        api->SliderSetRect = shim_slider_set_rect;
        api->SliderSetRange = shim_slider_set_range;
        api->SliderDraw = shim_slider_draw;
        api->SliderIsPressed = shim_slider_is_pressed;
        api->SliderSetPressed = shim_slider_set_pressed;
        api->SliderDragMove = shim_slider_drag_move;
        api->SliderIsHovered = shim_slider_is_hovered;
        api->SliderSetHovered = shim_slider_set_hovered;
        api->PopupDestroy = shim_popup_destroy;
        api->PopupSetTitle = shim_popup_set_title;
        api->PopupSetContent = shim_popup_set_content;
        api->PopupSetPosition = shim_popup_set_position;
        api->PopupSetVisible = shim_popup_set_visible;
        api->PopupSetCloseable = shim_popup_set_closeable;
        api->PopupDraw = shim_popup_draw;
        api->PopupIsCloseClicked = shim_popup_is_close_clicked;
        api->PopupIsTitleClicked = shim_popup_is_title_clicked;
        api->PopupDragMove = shim_popup_drag_move;
        api->PopupIsInside = shim_popup_is_inside;
        api->TooltipDestroy = shim_tooltip_destroy;
        api->TooltipSetText = shim_tooltip_set_text;
        api->TooltipSetPosition = shim_tooltip_set_position;
        api->TooltipSetVisible = shim_tooltip_set_visible;
        api->TooltipDraw = shim_tooltip_draw;
        api->JuiCtxDestroy = shim_jui_ctx_destroy;
        api->JuiBegin = shim_jui_begin;
        api->JuiEnd = shim_jui_end;
        api->JuiInputMotion = shim_jui_input_motion;
        api->JuiInputDown = shim_jui_input_down;
        api->JuiInputUp = shim_jui_input_up;
        api->JuiInputWheel = shim_jui_input_wheel;
        api->JuiInputKey = shim_jui_input_key;
        api->JuiIsDown = shim_jui_is_down;
        api->JuiIsPressed = shim_jui_is_pressed;
        api->JuiIsReleased = shim_jui_is_released;
        api->JuiTakeWheel = shim_jui_take_wheel;
        api->JuiIsShift = shim_jui_is_shift;
        api->JuiIsCtrl = shim_jui_is_ctrl;
        api->JuiPushClip = shim_jui_push_clip;
        api->JuiPopClip = shim_jui_pop_clip;
        api->JuiResetClip = shim_jui_reset_clip;
        api->JuiHit = shim_jui_hit;
        api->JuiFillRect = shim_jui_fill_rect;
        api->JuiStrokeRect = shim_jui_stroke_rect;
        api->JuiLine = shim_jui_line;
        api->JuiText = shim_jui_text;
        api->JuiTextCentered = shim_jui_text_centered;
        api->JuiPanelBegin = shim_jui_panel_begin;
        api->JuiPanelEnd = shim_jui_panel_end;
        api->JuiPanelFrame = shim_jui_panel_frame;
        api->JuiScrollBegin = shim_jui_scroll_begin;
        api->JuiScrollEnd = shim_jui_scroll_end;
        api->JuiScrollbar = shim_jui_scrollbar;
        api->JuiScrollSetOffset = shim_jui_scroll_set_offset;
        api->JuiButton = shim_jui_button;
        api->JtilemapDestroy = shim_jtilemap_destroy;
        api->JtilemapGetWidth = shim_jtilemap_get_width;
        api->JtilemapGetHeight = shim_jtilemap_get_height;
        api->JtilemapGetTilewidth = shim_jtilemap_get_tilewidth;
        api->JtilemapGetTileheight = shim_jtilemap_get_tileheight;
        api->JtilemapGetAnimTileid = shim_jtilemap_get_anim_tileid;
        api->JtilemapGetTile = shim_jtilemap_get_tile;
        api->JtilemapRenderAll = shim_jtilemap_render_all;
        api->JtilemapRenderTiles = shim_jtilemap_render_tiles;
        api->JtilemapRenderSetTilePx = shim_jtilemap_render_set_tile_px;
        api->JtilemapRenderLayer = shim_jtilemap_render_layer;
        api->JtilemapRenderDestroy = shim_jtilemap_render_destroy;
        api->KcpserverDestroy = shim_kcpserver_destroy;
        api->KcpserverSend = shim_kcpserver_send;
        api->KcpserverBroadcast = shim_kcpserver_broadcast;
        api->KcpserverOffline = shim_kcpserver_offline;
        api->KcpserverUpdate = shim_kcpserver_update;
        api->KcpserverConnectionCount = shim_kcpserver_connection_count;
        api->KcpserverPollMessage = shim_kcpserver_poll_message;
        api->KcpclientDestroy = shim_kcpclient_destroy;
        api->KcpclientSend = shim_kcpclient_send;
        api->KcpclientUpdate = shim_kcpclient_update;
        api->KcpclientPollMessage = shim_kcpclient_poll_message;
        api->TcpserverDestroy = shim_tcpserver_destroy;
        api->TcpserverSend = shim_tcpserver_send;
        api->TcpserverBroadcast = shim_tcpserver_broadcast;
        api->TcpserverOffline = shim_tcpserver_offline;
        api->TcpserverUpdate = shim_tcpserver_update;
        api->TcpserverConnectionCount = shim_tcpserver_connection_count;
        api->TcpserverPollMessage = shim_tcpserver_poll_message;
        api->TcpclientDestroy = shim_tcpclient_destroy;
        api->TcpclientSend = shim_tcpclient_send;
        api->TcpclientUpdate = shim_tcpclient_update;
        api->TcpclientPollMessage = shim_tcpclient_poll_message;
        api->WsnetserverDestroy = shim_wsnetserver_destroy;
        api->WsnetserverSend = shim_wsnetserver_send;
        api->WsnetserverBroadcast = shim_wsnetserver_broadcast;
        api->WsnetserverOffline = shim_wsnetserver_offline;
        api->WsnetserverUpdate = shim_wsnetserver_update;
        api->WsnetserverConnectionCount = shim_wsnetserver_connection_count;
        api->WsnetserverPollMessage = shim_wsnetserver_poll_message;
        api->WsnetclientDestroy = shim_wsnetclient_destroy;
        api->WsnetclientSend = shim_wsnetclient_send;
        api->WsnetclientUpdate = shim_wsnetclient_update;
        api->WsnetclientPollMessage = shim_wsnetclient_poll_message;
        api->NetclientDestroy = shim_netclient_destroy;
        api->NetclientSend = shim_netclient_send;
        api->NetclientUpdate = shim_netclient_update;
        api->NetclientPollMessage = shim_netclient_poll_message;
        api->NetserverDestroy = shim_netserver_destroy;
        api->NetserverSend = shim_netserver_send;
        api->NetserverBroadcast = shim_netserver_broadcast;
        api->NetserverOffline = shim_netserver_offline;
        api->NetserverUpdate = shim_netserver_update;
        api->NetserverConnectionCount = shim_netserver_connection_count;
        api->NetserverPollMessage = shim_netserver_poll_message;
        api->NetmessageFreeData = shim_netmessage_free_data;
        api->SceneNodeDestroy = shim_scene_node_destroy;
        api->SceneNodeAddChild = shim_scene_node_add_child;
        api->SceneNodeRemoveChild = shim_scene_node_remove_child;
        api->SceneNodeGetChildCount = shim_scene_node_get_child_count;
        api->SceneNodeSetPosition = shim_scene_node_set_position;
        api->SceneNodeSetWorldPosition = shim_scene_node_set_world_position;
        api->SceneNodeSetRotation = shim_scene_node_set_rotation;
        api->SceneNodeGetRotation = shim_scene_node_get_rotation;
        api->SceneNodeSetScale = shim_scene_node_set_scale;
        api->SceneNodeSetZorder = shim_scene_node_set_zorder;
        api->SceneNodeGetZorder = shim_scene_node_get_zorder;
        api->SceneNodeSetName = shim_scene_node_set_name;
        api->SceneNodeSetTag = shim_scene_node_set_tag;
        api->SceneNodeUpdate = shim_scene_node_update;
        api->SceneDestroy = shim_scene_destroy;
        api->SceneAddRootNode = shim_scene_add_root_node;
        api->SceneRemoveRootNode = shim_scene_remove_root_node;
        api->SceneGetRootCount = shim_scene_get_root_count;
        api->SceneUpdate = shim_scene_update;
        api->SceneSetPopupNode = shim_scene_set_popup_node;
        api->SceneManagerDestroy = shim_scene_manager_destroy;
        api->SceneManagerPush = shim_scene_manager_push;
        api->SceneManagerReplace = shim_scene_manager_replace;
        api->SceneManagerPop = shim_scene_manager_pop;
        api->SceneManagerUpdate = shim_scene_manager_update;
        api->SceneManagerGetCount = shim_scene_manager_get_count;
        api->JparticleDestroy = shim_jparticle_destroy;
        api->JparticleSetPosition = shim_jparticle_set_position;
        api->JparticleEmit = shim_jparticle_emit;
        api->JparticleClear = shim_jparticle_clear;
        api->JparticleUpdate = shim_jparticle_update;
        api->JparticleRender = shim_jparticle_render;
        api->JparticleCount = shim_jparticle_count;
        api->JparticleIsActive = shim_jparticle_is_active;
        api->JlightSysDestroy = shim_jlight_sys_destroy;
        api->JlightSysResize = shim_jlight_sys_resize;
        api->JlightRemove = shim_jlight_remove;
        api->JlightClear = shim_jlight_clear;
        api->JlightAddOccluder = shim_jlight_add_occluder;
        api->JlightClearOccluders = shim_jlight_clear_occluders;
        api->JlightSetAmbient = shim_jlight_set_ambient;
        api->JlightRender = shim_jlight_render;
        api->JecsDestroy = shim_jecs_destroy;
        api->JecsProcess = shim_jecs_process;
        api->JecsDefine = shim_jecs_define;
        api->AudioOpenDevice = shim_audio_open_device;
        api->AudioCloseDevice = shim_audio_close_device;
        api->AudioCreateWav = shim_audio_create_wav;
        api->AudioCreateMp3 = shim_audio_create_mp3;
        api->AudioCreateFlac = shim_audio_create_flac;
        api->AudioCreateOgg = shim_audio_create_ogg;
        api->AudioShotPlay = shim_audio_shot_play;
        api->AudioShotSetGain = shim_audio_shot_set_gain;
        api->AudioShotDestroy = shim_audio_shot_destroy;
        api->AudioBgmStop = shim_audio_bgm_stop;
        api->AudioBgmSetGain = shim_audio_bgm_set_gain;
        api->AudioBgmDestroy = shim_audio_bgm_destroy;
        api->ImageDrawRect = shim_image_draw_rect;
        api->JmathFpFromFloat = shim_jmath_fp_from_float;
        api->JmathFpToFloat = shim_jmath_fp_to_float;
        api->JmathFpFromUint = shim_jmath_fp_from_uint;
        api->JmathFpFromInt = shim_jmath_fp_from_int;
        api->JmathFpFromFraction = shim_jmath_fp_from_fraction;
        api->JmathFpMinValue = shim_jmath_fp_min_value;
        api->JmathFpMaxValue = shim_jmath_fp_max_value;
        api->JmathFpAdd = shim_jmath_fp_add;
        api->JmathFpSub = shim_jmath_fp_sub;
        api->JmathFpZero = shim_jmath_fp_zero;
        api->JmathFpHalf = shim_jmath_fp_half;
        api->JmathFpOne = shim_jmath_fp_one;
        api->JmathFpPi = shim_jmath_fp_pi;
        api->JmathFpMax = shim_jmath_fp_max;
        api->JmathFpMin = shim_jmath_fp_min;
        api->JmathFpPow2 = shim_jmath_fp_pow2;
        api->JmathFpSign = shim_jmath_fp_sign;
        api->JmathFpClamp = shim_jmath_fp_clamp;
        api->JmathFpLerp = shim_jmath_fp_lerp;
        api->JmathVec2fNegateX = shim_jmath_vec2f_negate_x;
        api->JmathVec2fNegateY = shim_jmath_vec2f_negate_y;
        api->JmathVec2fDot = shim_jmath_vec2f_dot;
        api->JmathVec2fCross = shim_jmath_vec2f_cross;
        api->JmathVec2fCross2X = shim_jmath_vec2f_cross2_x;
        api->JmathVec2fCross2Y = shim_jmath_vec2f_cross2_y;
        api->JmathVec2fCross3X = shim_jmath_vec2f_cross3_x;
        api->JmathVec2fCross3Y = shim_jmath_vec2f_cross3_y;
        api->JmathVec2fAddX = shim_jmath_vec2f_add_x;
        api->JmathVec2fAddY = shim_jmath_vec2f_add_y;
        api->JmathVec2fSubX = shim_jmath_vec2f_sub_x;
        api->JmathVec2fSubY = shim_jmath_vec2f_sub_y;
        api->JmathVec2fScaleX = shim_jmath_vec2f_scale_x;
        api->JmathVec2fScaleY = shim_jmath_vec2f_scale_y;
        api->JmathVec2fAbsX = shim_jmath_vec2f_abs_x;
        api->JmathVec2fAbsY = shim_jmath_vec2f_abs_y;
        api->JmathVec2fLengthSquared = shim_jmath_vec2f_length_squared;
        api->JmathVec2fLength = shim_jmath_vec2f_length;
        api->JmathVec2fNormalizeX = shim_jmath_vec2f_normalize_x;
        api->JmathVec2fNormalizeY = shim_jmath_vec2f_normalize_y;
        api->JmathVec2fNormalX = shim_jmath_vec2f_normal_x;
        api->JmathVec2fNormalY = shim_jmath_vec2f_normal_y;
        api->JmathVec2fRotateX = shim_jmath_vec2f_rotate_x;
        api->JmathVec2fRotateY = shim_jmath_vec2f_rotate_y;
        api->JmathVec2fLerpX = shim_jmath_vec2f_lerp_x;
        api->JmathVec2fLerpY = shim_jmath_vec2f_lerp_y;
        api->JmathVec2fDistance = shim_jmath_vec2f_distance;
        api->JmathVec3fNegateX = shim_jmath_vec3f_negate_x;
        api->JmathVec3fNegateY = shim_jmath_vec3f_negate_y;
        api->JmathVec3fNegateZ = shim_jmath_vec3f_negate_z;
        api->JmathVec3fAddX = shim_jmath_vec3f_add_x;
        api->JmathVec3fAddY = shim_jmath_vec3f_add_y;
        api->JmathVec3fAddZ = shim_jmath_vec3f_add_z;
        api->JmathVec3fSubX = shim_jmath_vec3f_sub_x;
        api->JmathVec3fSubY = shim_jmath_vec3f_sub_y;
        api->JmathVec3fSubZ = shim_jmath_vec3f_sub_z;
        api->JmathVec3fScaleX = shim_jmath_vec3f_scale_x;
        api->JmathVec3fScaleY = shim_jmath_vec3f_scale_y;
        api->JmathVec3fScaleZ = shim_jmath_vec3f_scale_z;
        api->JmathVec3fAbsX = shim_jmath_vec3f_abs_x;
        api->JmathVec3fAbsY = shim_jmath_vec3f_abs_y;
        api->JmathVec3fAbsZ = shim_jmath_vec3f_abs_z;
        api->JmathVec3fDot = shim_jmath_vec3f_dot;
        api->JmathVec3fCrossX = shim_jmath_vec3f_cross_x;
        api->JmathVec3fCrossY = shim_jmath_vec3f_cross_y;
        api->JmathVec3fCrossZ = shim_jmath_vec3f_cross_z;
        api->JmathVec3fSquareLength = shim_jmath_vec3f_square_length;
        api->JmathVec3fLength = shim_jmath_vec3f_length;
        api->JmathVec3fNormalizeX = shim_jmath_vec3f_normalize_x;
        api->JmathVec3fNormalizeY = shim_jmath_vec3f_normalize_y;
        api->JmathVec3fNormalizeZ = shim_jmath_vec3f_normalize_z;
        api->JmathVec3fLerpX = shim_jmath_vec3f_lerp_x;
        api->JmathVec3fLerpY = shim_jmath_vec3f_lerp_y;
        api->JmathVec3fLerpZ = shim_jmath_vec3f_lerp_z;
        api->JmathVec3fDistance = shim_jmath_vec3f_distance;
        api->JmathMat22fMulVec2fX = shim_jmath_mat22f_mul_vec2f_x;
        api->JmathMat22fMulVec2fY = shim_jmath_mat22f_mul_vec2f_y;
        api->JmathMat22fRotateCol1X = shim_jmath_mat22f_rotate_col1_x;
        api->JmathMat22fRotateCol1Y = shim_jmath_mat22f_rotate_col1_y;
        api->JmathMat22fRotateCol2X = shim_jmath_mat22f_rotate_col2_x;
        api->JmathMat22fRotateCol2Y = shim_jmath_mat22f_rotate_col2_y;
        api->JmathMat22fTransposeCol1X = shim_jmath_mat22f_transpose_col1_x;
        api->JmathMat22fTransposeCol1Y = shim_jmath_mat22f_transpose_col1_y;
        api->JmathMat22fTransposeCol2X = shim_jmath_mat22f_transpose_col2_x;
        api->JmathMat22fTransposeCol2Y = shim_jmath_mat22f_transpose_col2_y;
        api->JmathMat22fInvertCol1X = shim_jmath_mat22f_invert_col1_x;
        api->JmathMat22fInvertCol1Y = shim_jmath_mat22f_invert_col1_y;
        api->JmathMat22fInvertCol2X = shim_jmath_mat22f_invert_col2_x;
        api->JmathMat22fInvertCol2Y = shim_jmath_mat22f_invert_col2_y;
        api->JmathMat22fAddCol1X = shim_jmath_mat22f_add_col1_x;
        api->JmathMat22fAddCol1Y = shim_jmath_mat22f_add_col1_y;
        api->JmathMat22fAddCol2X = shim_jmath_mat22f_add_col2_x;
        api->JmathMat22fAddCol2Y = shim_jmath_mat22f_add_col2_y;
        api->JmathMat22fMulCol1X = shim_jmath_mat22f_mul_col1_x;
        api->JmathMat22fMulCol1Y = shim_jmath_mat22f_mul_col1_y;
        api->JmathMat22fMulCol2X = shim_jmath_mat22f_mul_col2_x;
        api->JmathMat22fMulCol2Y = shim_jmath_mat22f_mul_col2_y;
        api->JmathMat22fAbsCol1X = shim_jmath_mat22f_abs_col1_x;
        api->JmathMat22fAbsCol1Y = shim_jmath_mat22f_abs_col1_y;
        api->JmathMat22fAbsCol2X = shim_jmath_mat22f_abs_col2_x;
        api->JmathMat22fAbsCol2Y = shim_jmath_mat22f_abs_col2_y;
        api->JmathFtFromFloat = shim_jmath_ft_from_float;
        api->JmathFtToFloat = shim_jmath_ft_to_float;
        api->JmathFtMinValue = shim_jmath_ft_min_value;
        api->JmathFtMaxValue = shim_jmath_ft_max_value;
        api->JmathFtAdd = shim_jmath_ft_add;
        api->JmathFtSub = shim_jmath_ft_sub;
        api->JmathFtMul = shim_jmath_ft_mul;
        api->JmathFtDiv = shim_jmath_ft_div;
        api->JmathFtAbs = shim_jmath_ft_abs;
        api->JmathFtSqrt = shim_jmath_ft_sqrt;
        api->JmathFtSin = shim_jmath_ft_sin;
        api->JmathFtCos = shim_jmath_ft_cos;
        api->JmathFtAtan2 = shim_jmath_ft_atan2;
        api->JmathFtZero = shim_jmath_ft_zero;
        api->JmathFtHalf = shim_jmath_ft_half;
        api->JmathFtOne = shim_jmath_ft_one;
        api->JmathFtPi = shim_jmath_ft_pi;
        api->JmathFtMax = shim_jmath_ft_max;
        api->JmathFtMin = shim_jmath_ft_min;
        api->JmathFtPow2 = shim_jmath_ft_pow2;
        api->JmathFtSign = shim_jmath_ft_sign;
        api->JmathFtClamp = shim_jmath_ft_clamp;
        api->JmathFtLerp = shim_jmath_ft_lerp;
        api->JmathVec2NegateX = shim_jmath_vec2_negate_x;
        api->JmathVec2NegateY = shim_jmath_vec2_negate_y;
        api->JmathVec2Negate = shim_jmath_vec2_negate;
        api->JmathVec2Dot = shim_jmath_vec2_dot;
        api->JmathVec2Cross = shim_jmath_vec2_cross;
        api->JmathVec2Cross2X = shim_jmath_vec2_cross2_x;
        api->JmathVec2Cross2Y = shim_jmath_vec2_cross2_y;
        api->JmathVec2Cross2 = shim_jmath_vec2_cross2;
        api->JmathVec2Cross3X = shim_jmath_vec2_cross3_x;
        api->JmathVec2Cross3Y = shim_jmath_vec2_cross3_y;
        api->JmathVec2Cross3 = shim_jmath_vec2_cross3;
        api->JmathVec2AddX = shim_jmath_vec2_add_x;
        api->JmathVec2AddY = shim_jmath_vec2_add_y;
        api->JmathVec2Add = shim_jmath_vec2_add;
        api->JmathVec2SubX = shim_jmath_vec2_sub_x;
        api->JmathVec2SubY = shim_jmath_vec2_sub_y;
        api->JmathVec2Sub = shim_jmath_vec2_sub;
        api->JmathVec2ScaleX = shim_jmath_vec2_scale_x;
        api->JmathVec2ScaleY = shim_jmath_vec2_scale_y;
        api->JmathVec2Scale = shim_jmath_vec2_scale;
        api->JmathVec2AbsX = shim_jmath_vec2_abs_x;
        api->JmathVec2AbsY = shim_jmath_vec2_abs_y;
        api->JmathVec2Abs = shim_jmath_vec2_abs;
        api->JmathVec2LengthSquared = shim_jmath_vec2_length_squared;
        api->JmathVec2Length = shim_jmath_vec2_length;
        api->JmathVec2NormalizeX = shim_jmath_vec2_normalize_x;
        api->JmathVec2NormalizeY = shim_jmath_vec2_normalize_y;
        api->JmathVec2Normalize = shim_jmath_vec2_normalize;
        api->JmathVec2NormalX = shim_jmath_vec2_normal_x;
        api->JmathVec2NormalY = shim_jmath_vec2_normal_y;
        api->JmathVec2Normal = shim_jmath_vec2_normal;
        api->JmathVec2RotateX = shim_jmath_vec2_rotate_x;
        api->JmathVec2RotateY = shim_jmath_vec2_rotate_y;
        api->JmathVec2Rotate = shim_jmath_vec2_rotate;
        api->JmathVec2LerpX = shim_jmath_vec2_lerp_x;
        api->JmathVec2LerpY = shim_jmath_vec2_lerp_y;
        api->JmathVec2Lerp = shim_jmath_vec2_lerp;
        api->JmathVec2Distance = shim_jmath_vec2_distance;
        api->JmathVec3NegateX = shim_jmath_vec3_negate_x;
        api->JmathVec3NegateY = shim_jmath_vec3_negate_y;
        api->JmathVec3NegateZ = shim_jmath_vec3_negate_z;
        api->JmathVec3AddX = shim_jmath_vec3_add_x;
        api->JmathVec3AddY = shim_jmath_vec3_add_y;
        api->JmathVec3AddZ = shim_jmath_vec3_add_z;
        api->JmathVec3SubX = shim_jmath_vec3_sub_x;
        api->JmathVec3SubY = shim_jmath_vec3_sub_y;
        api->JmathVec3SubZ = shim_jmath_vec3_sub_z;
        api->JmathVec3ScaleX = shim_jmath_vec3_scale_x;
        api->JmathVec3ScaleY = shim_jmath_vec3_scale_y;
        api->JmathVec3ScaleZ = shim_jmath_vec3_scale_z;
        api->JmathVec3AbsX = shim_jmath_vec3_abs_x;
        api->JmathVec3AbsY = shim_jmath_vec3_abs_y;
        api->JmathVec3AbsZ = shim_jmath_vec3_abs_z;
        api->JmathVec3Dot = shim_jmath_vec3_dot;
        api->JmathVec3CrossX = shim_jmath_vec3_cross_x;
        api->JmathVec3CrossY = shim_jmath_vec3_cross_y;
        api->JmathVec3CrossZ = shim_jmath_vec3_cross_z;
        api->JmathVec3SquareLength = shim_jmath_vec3_square_length;
        api->JmathVec3Length = shim_jmath_vec3_length;
        api->JmathVec3NormalizeX = shim_jmath_vec3_normalize_x;
        api->JmathVec3NormalizeY = shim_jmath_vec3_normalize_y;
        api->JmathVec3NormalizeZ = shim_jmath_vec3_normalize_z;
        api->JmathVec3LerpX = shim_jmath_vec3_lerp_x;
        api->JmathVec3LerpY = shim_jmath_vec3_lerp_y;
        api->JmathVec3LerpZ = shim_jmath_vec3_lerp_z;
        api->JmathVec3Distance = shim_jmath_vec3_distance;
        api->JmathMat22MulVec2X = shim_jmath_mat22_mul_vec2_x;
        api->JmathMat22MulVec2Y = shim_jmath_mat22_mul_vec2_y;
        api->JmathMat22MulVec2 = shim_jmath_mat22_mul_vec2;
        api->JmathMat22RotateCol1X = shim_jmath_mat22_rotate_col1_x;
        api->JmathMat22RotateCol1Y = shim_jmath_mat22_rotate_col1_y;
        api->JmathMat22RotateCol2X = shim_jmath_mat22_rotate_col2_x;
        api->JmathMat22RotateCol2Y = shim_jmath_mat22_rotate_col2_y;
        api->JmathMat22TransposeCol1X = shim_jmath_mat22_transpose_col1_x;
        api->JmathMat22TransposeCol1Y = shim_jmath_mat22_transpose_col1_y;
        api->JmathMat22TransposeCol2X = shim_jmath_mat22_transpose_col2_x;
        api->JmathMat22TransposeCol2Y = shim_jmath_mat22_transpose_col2_y;
        api->JmathMat22InvertCol1X = shim_jmath_mat22_invert_col1_x;
        api->JmathMat22InvertCol1Y = shim_jmath_mat22_invert_col1_y;
        api->JmathMat22InvertCol2X = shim_jmath_mat22_invert_col2_x;
        api->JmathMat22InvertCol2Y = shim_jmath_mat22_invert_col2_y;
        api->JmathMat22AddCol1X = shim_jmath_mat22_add_col1_x;
        api->JmathMat22AddCol1Y = shim_jmath_mat22_add_col1_y;
        api->JmathMat22AddCol2X = shim_jmath_mat22_add_col2_x;
        api->JmathMat22AddCol2Y = shim_jmath_mat22_add_col2_y;
        api->JmathMat22MulCol1X = shim_jmath_mat22_mul_col1_x;
        api->JmathMat22MulCol1Y = shim_jmath_mat22_mul_col1_y;
        api->JmathMat22MulCol2X = shim_jmath_mat22_mul_col2_x;
        api->JmathMat22MulCol2Y = shim_jmath_mat22_mul_col2_y;
        api->JmathMat22AbsCol1X = shim_jmath_mat22_abs_col1_x;
        api->JmathMat22AbsCol1Y = shim_jmath_mat22_abs_col1_y;
        api->JmathMat22AbsCol2X = shim_jmath_mat22_abs_col2_x;
        api->JmathMat22AbsCol2Y = shim_jmath_mat22_abs_col2_y;
        api->JmathVec3DivX = shim_jmath_vec3_div_x;
        api->JmathVec3DivY = shim_jmath_vec3_div_y;
        api->JmathVec3DivZ = shim_jmath_vec3_div_z;
        api->JmathFtRadians = shim_jmath_ft_radians;
        return 0;
}

const char* joy_api_vtable_selftest(void)
{
        JoyApi api;
        if (joy_api_vtable_init(&api, NULL) != 0) return "init";
        if (api.abi_version != JOY_API_VTABLE_ABI) return "abi_version";
        if (api.size != sizeof(JoyApi)) return "size";
        if (!api.JpmWaveSeed) return "missing shim: JpmWaveSeed";
        {
                uint32_t direct = jpm_wave_seed((uint32_t)12345, (int)3);
                uint32_t via = api.JpmWaveSeed(api.ctx, (uint32_t)12345, (int)3);
                if (direct != via) return "jpm_wave_seed mismatch";
        }
        if (!api.SdbmHash) return "missing shim: SdbmHash";
        {
                unsigned long direct = joy_core_sdbm_hash((const char*)"joy-vtable", (int)3);
                unsigned long via = api.SdbmHash(api.ctx, (const char*)"joy-vtable", (int)3);
                if (direct != via) return "joy_core_sdbm_hash mismatch";
        }
        if (!api.Fnv1aHash) return "missing shim: Fnv1aHash";
        {
                unsigned long direct = joy_core_fnv1a_hash((const char*)"joy-vtable", (int)3);
                unsigned long via = api.Fnv1aHash(api.ctx, (const char*)"joy-vtable", (int)3);
                if (direct != via) return "joy_core_fnv1a_hash mismatch";
        }
        if (!api.JenkinsHash) return "missing shim: JenkinsHash";
        {
                unsigned long direct = joy_core_jenkins_hash((const char*)"joy-vtable", (int)3);
                unsigned long via = api.JenkinsHash(api.ctx, (const char*)"joy-vtable", (int)3);
                if (direct != via) return "joy_core_jenkins_hash mismatch";
        }
        if (!api.Murmur3Hash) return "missing shim: Murmur3Hash";
        {
                unsigned long direct = joy_core_murmur3_hash((const char*)"joy-vtable");
                unsigned long via = api.Murmur3Hash(api.ctx, (const char*)"joy-vtable");
                if (direct != via) return "joy_core_murmur3_hash mismatch";
        }
        if (!api.KrHash) return "missing shim: KrHash";
        {
                unsigned long direct = joy_core_kr_hash((const char*)"joy-vtable", (int)3);
                unsigned long via = api.KrHash(api.ctx, (const char*)"joy-vtable", (int)3);
                if (direct != via) return "joy_core_kr_hash mismatch";
        }
        if (!api.JmathFpFromFloat) return "missing shim: JmathFpFromFloat";
        {
                int64_t direct = jmath_fp_from_float((float)1.5f);
                int64_t via = api.JmathFpFromFloat(api.ctx, (float)1.5f);
                if (direct != via) return "jmath_fp_from_float mismatch";
        }
        /* 强句柄链路：传 h=0，thunk 的 joy_obj_deref 必须拒绝，
         * shim 应翻译成零值而不是崩溃（不触碰任何真实对象）。 */
        if (!api.WindowGetFullscreen) return "missing shim: WindowGetFullscreen";
        {
                Jwindow _jv_bad = {0};
                if (api.WindowGetFullscreen(api.ctx, _jv_bad)) return "invalid handle accepted";
        }
        return NULL;
}