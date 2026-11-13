/* ============================================================
 * 自动生成，勿手改：node api/tools/gen_bindings.js --emit
 * cs 后端薄适配：Mono 值 → joy_value[] → 共享 thunk（不含业务逻辑）。
 * joy.idl 里 status:on 的 L0/L1，以及可展开的 L2 试点条目会出现在这里。
 * ============================================================ */
#include <stdint.h>
#include <stdio.h>
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>
#include <mono/utils/mono-publib.h>
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
        r = joy_api_call("joy_core_lcg_random_create", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_lcg_random_create", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static void icall_LcgRandomDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_core_lcg_random_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_lcg_random_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static uint32_t icall_LcgRandomNext(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_core_lcg_random_next", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_lcg_random_next", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint32_t)r.as.u32;
}

static void icall_LcgRandomSetState(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_u64((uint64_t)p1);
        (void)joy_api_call("joy_core_lcg_random_set_state", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_lcg_random_set_state", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JpmConfigAddSpawn(uint64_t p0, int32_t p1, int32_t p2, int32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_i32((int32_t)p2);
        args[3] = joy_v_i32((int32_t)p3);
        (void)joy_api_call("joy_scene_procmap_config_add_spawn", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_procmap_config_add_spawn", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static uint32_t icall_JpmWaveSeed(uint32_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_u32((uint32_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        r = joy_api_call("joy_scene_procmap_wave_seed", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_procmap_wave_seed", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint32_t)r.as.u32;
}

static uint64_t icall_SdbmHash(MonoString* p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s0 = NULL;
        s0 = p0 ? mono_string_to_utf8(p0) : NULL;
        args[0] = joy_v_str((const char*)s0);
        args[1] = joy_v_i32((int32_t)p1);
        r = joy_api_call("joy_core_sdbm_hash", args, 2, &st);
        if (s0) mono_free(s0);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_sdbm_hash", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static uint64_t icall_Fnv1aHash(MonoString* p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s0 = NULL;
        s0 = p0 ? mono_string_to_utf8(p0) : NULL;
        args[0] = joy_v_str((const char*)s0);
        args[1] = joy_v_i32((int32_t)p1);
        r = joy_api_call("joy_core_fnv1a_hash", args, 2, &st);
        if (s0) mono_free(s0);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_fnv1a_hash", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static uint64_t icall_JenkinsHash(MonoString* p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s0 = NULL;
        s0 = p0 ? mono_string_to_utf8(p0) : NULL;
        args[0] = joy_v_str((const char*)s0);
        args[1] = joy_v_i32((int32_t)p1);
        r = joy_api_call("joy_core_jenkins_hash", args, 2, &st);
        if (s0) mono_free(s0);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_jenkins_hash", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static uint64_t icall_Murmur3Hash(MonoString* p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        char* s0 = NULL;
        s0 = p0 ? mono_string_to_utf8(p0) : NULL;
        args[0] = joy_v_str((const char*)s0);
        r = joy_api_call("joy_core_murmur3_hash", args, 1, &st);
        if (s0) mono_free(s0);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_murmur3_hash", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static uint64_t icall_KrHash(MonoString* p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s0 = NULL;
        s0 = p0 ? mono_string_to_utf8(p0) : NULL;
        args[0] = joy_v_str((const char*)s0);
        args[1] = joy_v_i32((int32_t)p1);
        r = joy_api_call("joy_core_kr_hash", args, 2, &st);
        if (s0) mono_free(s0);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_kr_hash", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static void icall_JtimerDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_core_timer_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_timer_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JtimerTrigger(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_core_timer_trigger", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_timer_trigger", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JtimerSetInterval(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_core_timer_set_interval", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_timer_set_interval", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JtimerSetActive(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_core_timer_set_active", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_timer_set_active", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JtimerReset(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_core_timer_reset", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_timer_reset", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static uint64_t icall_JtimerGetElapsed(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_core_timer_get_elapsed", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_timer_get_elapsed", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static int32_t icall_InitEnv(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_core_init_env", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_init_env", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_QuitEnv(void)
{
        joy_status_t st = JOY_OK;
        (void)joy_api_call("joy_core_quit_env", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_quit_env", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static uint64_t icall_GetTicks(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_core_get_ticks", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_get_ticks", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static uint64_t icall_CreateWindow(MonoString* p0, int32_t p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        char* s0 = NULL;
        s0 = p0 ? mono_string_to_utf8(p0) : NULL;
        args[0] = joy_v_str((const char*)s0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_i32((int32_t)p2);
        r = joy_api_call("joy_core_create_window", args, 3, &st);
        if (s0) mono_free(s0);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_create_window", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static void icall_DestroyWindow(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_core_destroy_window", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_destroy_window", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_WindowSetFullscreen(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_core_window_set_fullscreen", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_window_set_fullscreen", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_WindowGetFullscreen(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_core_window_get_fullscreen", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_window_get_fullscreen", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_WindowSetIcon(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        (void)joy_api_call("joy_core_window_set_icon", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_window_set_icon", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_PushUserEvent(int32_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i32((int32_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        r = joy_api_call("joy_core_push_user_event", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_push_user_event", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_PushQuitEvent(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_core_push_quit_event", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_push_quit_event", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_LogTitle(int32_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_i32((int32_t)p0);
        (void)joy_api_call("joy_core_log_title", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_log_title", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_EnvSet(uint64_t p0, MonoString* p1, MonoString* p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        (void)joy_api_call("joy_core_env_set", args, 3, &st);
        if (s1) mono_free(s1);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_env_set", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_EnvDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_core_env_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_env_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SimpleFpsDestory(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_core_simple_fps_destory", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_simple_fps_destory", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SimpleFpsUpdate(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_core_simple_fps_update", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_simple_fps_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_SimpleFpsValue(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_core_simple_fps_value", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_simple_fps_value", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_GameTimerInit(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_core_game_timer_init", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_game_timer_init", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_GameTimerUpdate(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_core_game_timer_update", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_game_timer_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static float icall_GameTimerGetDeltaTime(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_core_game_timer_get_delta_time", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_game_timer_get_delta_time", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_GameTimerGetUnscaledDeltaTime(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_core_game_timer_get_unscaled_delta_time", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_game_timer_get_unscaled_delta_time", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static void icall_GameTimerSetTimeScale(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_core_game_timer_set_time_scale", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_game_timer_set_time_scale", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static float icall_GameTimerGetTimeScale(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_core_game_timer_get_time_scale", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_game_timer_get_time_scale", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static void icall_GameTimerSetTargetFps(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_core_game_timer_set_target_fps", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_game_timer_set_target_fps", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_GameTimerGetTargetFps(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_core_game_timer_get_target_fps", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_game_timer_get_target_fps", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_SetClipboardText(MonoString* p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        char* s0 = NULL;
        s0 = p0 ? mono_string_to_utf8(p0) : NULL;
        args[0] = joy_v_str((const char*)s0);
        (void)joy_api_call("joy_core_set_clipboard_text", args, 1, &st);
        if (s0) mono_free(s0);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_core_set_clipboard_text", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_SysInitNetenv(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_net_sys_init_netenv", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_init_netenv", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_SysReleaseNetenv(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_net_sys_release_netenv", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_release_netenv", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_SysAnyaddr(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_net_sys_anyaddr", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_anyaddr", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_SysClosesocket(int64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i64((int64_t)p0);
        r = joy_api_call("joy_net_sys_closesocket", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_closesocket", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_SysSetSockSndtimeo(int64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        r = joy_api_call("joy_net_sys_set_sock_sndtimeo", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_set_sock_sndtimeo", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_SysSetSockRcvtimeo(int64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        r = joy_api_call("joy_net_sys_set_sock_rcvtimeo", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_set_sock_rcvtimeo", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_SysSetSockAccpettimeo(int64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        r = joy_api_call("joy_net_sys_set_sock_accpettimeo", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_set_sock_accpettimeo", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_SysSetSockBroadcast(int64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i64((int64_t)p0);
        r = joy_api_call("joy_net_sys_set_sock_broadcast", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_set_sock_broadcast", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_SysSetSockReuseaddr(int64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i64((int64_t)p0);
        r = joy_api_call("joy_net_sys_set_sock_reuseaddr", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_set_sock_reuseaddr", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int64_t icall_SysTcp(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_net_sys_tcp", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_tcp", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_SysUdp(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_net_sys_udp", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_udp", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int32_t icall_SysBind(int64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_i64((int64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        r = joy_api_call("joy_net_sys_bind", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_bind", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_SysConnect(int64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_i64((int64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        r = joy_api_call("joy_net_sys_connect", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_connect", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_SysListen(int64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i64((int64_t)p0);
        r = joy_api_call("joy_net_sys_listen", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_listen", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_SysSend(int64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_i64((int64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        r = joy_api_call("joy_net_sys_send", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_send", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_SysSendto(int64_t p0, MonoString* p1, int32_t p2, MonoString* p3, int32_t p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[5];
        char* s1 = NULL;
        char* s3 = NULL;
        args[0] = joy_v_i64((int64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        s3 = p3 ? mono_string_to_utf8(p3) : NULL;
        args[3] = joy_v_str((const char*)s3);
        args[4] = joy_v_i32((int32_t)p4);
        r = joy_api_call("joy_net_sys_sendto", args, 5, &st);
        if (s1) mono_free(s1);
        if (s3) mono_free(s3);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_sendto", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int64_t icall_SysCurrentTime(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_net_sys_current_time", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_current_time", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static void icall_SysSleep(int32_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_i32((int32_t)p0);
        (void)joy_api_call("joy_net_sys_sleep", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_sleep", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SysInstallCrashHandlers(void)
{
        joy_status_t st = JOY_OK;
        (void)joy_api_call("joy_net_sys_install_crash_handlers", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_sys_install_crash_handlers", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_IocpServerDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_iocp_server_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_iocp_server_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_IocpProcessEvents(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_net_iocp_process_events", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_iocp_process_events", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static uint64_t icall_FontCreate(MonoString* p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s0 = NULL;
        s0 = p0 ? mono_string_to_utf8(p0) : NULL;
        args[0] = joy_v_str((const char*)s0);
        args[1] = joy_v_i32((int32_t)p1);
        r = joy_api_call("joy_r2d_font_create", args, 2, &st);
        if (s0) mono_free(s0);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_font_create", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static void icall_FontDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r2d_font_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_font_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static float icall_FontGetScale(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r2d_font_get_scale", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_font_get_scale", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static int32_t icall_FontGetSize(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r2d_font_get_size", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_font_get_size", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_FontGetAscent(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r2d_font_get_ascent", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_font_get_ascent", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static uint64_t icall_R2dCreate(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r2d_create", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_create", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static void icall_R2dDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r2d_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R2dSetColor(uint64_t p0, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_u32((uint32_t)p1);
        args[2] = joy_v_u32((uint32_t)p2);
        args[3] = joy_v_u32((uint32_t)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        r = joy_api_call("joy_r2d_set_color", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_set_color", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_R2dBegin(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r2d_begin", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_begin", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R2dEnd(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r2d_end", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_end", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R2dSetLogicalPresentation(uint64_t p0, int32_t p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_i32((int32_t)p2);
        r = joy_api_call("joy_r2d_set_logical_presentation", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_set_logical_presentation", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_R2dSetLogicalPresentationMode(uint64_t p0, int32_t p1, int32_t p2, int32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_i32((int32_t)p2);
        args[3] = joy_v_i32((int32_t)p3);
        r = joy_api_call("joy_r2d_set_logical_presentation_mode", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_set_logical_presentation_mode", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_R2dSetVsync(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        r = joy_api_call("joy_r2d_set_vsync", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_set_vsync", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_R2dSetScale(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_r2d_set_scale", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_set_scale", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_R2dSetColorf(uint64_t p0, float p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_r2d_set_colorf", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_set_colorf", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R2dSetBlendMode(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        r = joy_api_call("joy_r2d_set_blend_mode", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_set_blend_mode", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_R2dDrawPoint(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_r2d_draw_point", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_draw_point", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_R2dSetClipRect(uint64_t p0, float p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        r = joy_api_call("joy_r2d_set_clip_rect", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_set_clip_rect", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_R2dClearClip(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r2d_clear_clip", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_clear_clip", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R2dPushClipRect(uint64_t p0, float p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        r = joy_api_call("joy_r2d_push_clip_rect", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_push_clip_rect", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_R2dPopClipRect(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r2d_pop_clip_rect", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_pop_clip_rect", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R2dResetClipStack(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r2d_reset_clip_stack", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_reset_clip_stack", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R2dPointInClip(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_r2d_point_in_clip", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_point_in_clip", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_ShapeDrawLine(uint64_t p0, float p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        r = joy_api_call("joy_r2d_shape_draw_line", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_shape_draw_line", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_ShapeDrawRectangle(uint64_t p0, MonoString* p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_r2d_shape_draw_rectangle", args, 6, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_shape_draw_rectangle", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static uint64_t icall_ImageCreate(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        r = joy_api_call("joy_r2d_image_create", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_image_create", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static void icall_ImageDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r2d_image_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_image_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ImageSetAlpha(uint64_t p0, uint32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_u32((uint32_t)p1);
        (void)joy_api_call("joy_r2d_image_set_alpha", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_image_set_alpha", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ImageSetBlendMode(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_r2d_image_set_blend_mode", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_image_set_blend_mode", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ImageSetScaleMode(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_r2d_image_set_scale_mode", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_image_set_scale_mode", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_TextUpdatex(uint64_t p0, uint64_t p1, MonoString* p2, int32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[8];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_i32((int32_t)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        args[5] = joy_v_u32((uint32_t)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        (void)joy_api_call("joy_r2d_text_updatex", args, 8, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_text_updatex", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_TextDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r2d_text_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_text_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JtextPrint(uint64_t p0, uint64_t p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        (void)joy_api_call("joy_r2d_text_print", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_text_print", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JtextPrintx(uint64_t p0, uint64_t p1, MonoString* p2, int32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t p7, float p8, float p9)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[10];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_i32((int32_t)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        args[5] = joy_v_u32((uint32_t)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        args[8] = joy_v_f64((double)p8);
        args[9] = joy_v_f64((double)p9);
        (void)joy_api_call("joy_r2d_text_printx", args, 10, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_text_printx", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R2dDrawPointF(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_r2d_draw_point_f", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_draw_point_f", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_ShapeDrawRectangleF(uint64_t p0, MonoString* p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_r2d_shape_draw_rectangle_f", args, 6, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_shape_draw_rectangle_f", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_ShapeDrawLineF(uint64_t p0, float p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        r = joy_api_call("joy_r2d_shape_draw_line_f", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_shape_draw_line_f", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_JtextPrintf(uint64_t p0, uint64_t p1, MonoString* p2, int32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t p7, float p8, float p9)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[10];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_i32((int32_t)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        args[5] = joy_v_u32((uint32_t)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        args[8] = joy_v_f64((double)p8);
        args[9] = joy_v_f64((double)p9);
        (void)joy_api_call("joy_r2d_text_printf", args, 10, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_text_printf", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JtextPrintfScaled(uint64_t p0, uint64_t p1, MonoString* p2, int32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t p7, float p8, float p9, float p10)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[11];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_i32((int32_t)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        args[5] = joy_v_u32((uint32_t)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        args[8] = joy_v_f64((double)p8);
        args[9] = joy_v_f64((double)p9);
        args[10] = joy_v_f64((double)p10);
        (void)joy_api_call("joy_r2d_text_printf_scaled", args, 11, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_text_printf_scaled", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_TextGetWidth(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r2d_text_get_width", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_text_get_width", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_TextGetHeight(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r2d_text_get_height", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_text_get_height", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_JtextCacheClear(void)
{
        joy_status_t st = JOY_OK;
        (void)joy_api_call("joy_r2d_text_cache_clear", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_text_cache_clear", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JtextCacheSetLimit(int32_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_i32((int32_t)p0);
        (void)joy_api_call("joy_r2d_text_cache_set_limit", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_text_cache_set_limit", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_JtextCacheGetCount(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_r2d_text_cache_get_count", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_text_cache_get_count", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_JtextCacheSweep(int32_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_i32((int32_t)p0);
        (void)joy_api_call("joy_r2d_text_cache_sweep", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_text_cache_sweep", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_AnimationDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r2d_animation_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_animation_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_AnimationReset(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r2d_animation_reset", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_animation_reset", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_AnimationAddClip(uint64_t p0, MonoString* p1, float p2, float p3, float p4, float p5, float p6)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[7];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        (void)joy_api_call("joy_r2d_animation_add_clip", args, 7, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_animation_add_clip", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_AnimationIsFinished(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r2d_animation_is_finished", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_animation_is_finished", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_AnimationSetPosition(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_r2d_animation_set_position", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_animation_set_position", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_AnimationSetScale(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_r2d_animation_set_scale", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_animation_set_scale", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_AnimationSetRotation(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_r2d_animation_set_rotation", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_animation_set_rotation", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_AnimationUpdate(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_r2d_animation_update", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_animation_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R3dBegin(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r3d_begin", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_begin", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_R3dEnd(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_end", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_end", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R3dGetDrawCalls(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r3d_get_draw_calls", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_get_draw_calls", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_R3dSetClearColor(uint64_t p0, uint32_t p1, uint32_t p2, uint32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_u32((uint32_t)p1);
        args[2] = joy_v_u32((uint32_t)p2);
        args[3] = joy_v_u32((uint32_t)p3);
        (void)joy_api_call("joy_r3d_set_clear_color", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_set_clear_color", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static uint64_t icall_R3dMeshCube(uint64_t p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_r3d_mesh_cube", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_mesh_cube", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static uint64_t icall_R3dMeshSphere(uint64_t p0, float p1, int32_t p2, int32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_i32((int32_t)p2);
        args[3] = joy_v_i32((int32_t)p3);
        r = joy_api_call("joy_r3d_mesh_sphere", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_mesh_sphere", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static uint64_t icall_R3dMeshPlane(uint64_t p0, float p1, float p2, int32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_i32((int32_t)p3);
        r = joy_api_call("joy_r3d_mesh_plane", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_mesh_plane", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static uint64_t icall_R3dMeshRamp(uint64_t p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_r3d_mesh_ramp", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_mesh_ramp", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static uint64_t icall_R3dMeshQuad(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r3d_mesh_quad", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_mesh_quad", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static uint64_t icall_R3dMeshCubeWire(uint64_t p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_r3d_mesh_cube_wire", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_mesh_cube_wire", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static uint64_t icall_R3dMeshSphereWire(uint64_t p0, float p1, int32_t p2, int32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_i32((int32_t)p2);
        args[3] = joy_v_i32((int32_t)p3);
        r = joy_api_call("joy_r3d_mesh_sphere_wire", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_mesh_sphere_wire", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static uint64_t icall_R3dMeshPlaneWire(uint64_t p0, float p1, float p2, int32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_i32((int32_t)p3);
        r = joy_api_call("joy_r3d_mesh_plane_wire", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_mesh_plane_wire", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static uint64_t icall_R3dMeshRampWire(uint64_t p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_r3d_mesh_ramp_wire", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_mesh_ramp_wire", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static void icall_R3dMeshDestroy(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_r3d_mesh_destroy", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_mesh_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R3dFontInit(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        r = joy_api_call("joy_r3d_font_init", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_font_init", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_R3dFontSetColor(uint64_t p0, uint32_t p1, uint32_t p2, uint32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_u32((uint32_t)p1);
        args[2] = joy_v_u32((uint32_t)p2);
        args[3] = joy_v_u32((uint32_t)p3);
        (void)joy_api_call("joy_r3d_font_set_color", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_font_set_color", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R3dFontDraw2d(uint64_t p0, uint64_t p1, MonoString* p2, float p3, float p4, float p5, float p6, float p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        args[7] = joy_v_f64((double)p7);
        r = joy_api_call("joy_r3d_font_draw_2d", args, 8, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_font_draw_2d", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_R3dFontDraw2dColor(uint64_t p0, uint64_t p1, MonoString* p2, float p3, float p4, float p5, float p6, float p7, uint32_t p8, uint32_t p9, uint32_t p10)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[11];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        args[7] = joy_v_f64((double)p7);
        args[8] = joy_v_u32((uint32_t)p8);
        args[9] = joy_v_u32((uint32_t)p9);
        args[10] = joy_v_u32((uint32_t)p10);
        r = joy_api_call("joy_r3d_font_draw_2d_color", args, 11, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_font_draw_2d_color", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_R3dFontDraw2dOutline(uint64_t p0, uint64_t p1, MonoString* p2, float p3, float p4, float p5, float p6, float p7, uint32_t p8, uint32_t p9, uint32_t p10, uint32_t p11, uint32_t p12, uint32_t p13, float p14)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[15];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        args[7] = joy_v_f64((double)p7);
        args[8] = joy_v_u32((uint32_t)p8);
        args[9] = joy_v_u32((uint32_t)p9);
        args[10] = joy_v_u32((uint32_t)p10);
        args[11] = joy_v_u32((uint32_t)p11);
        args[12] = joy_v_u32((uint32_t)p12);
        args[13] = joy_v_u32((uint32_t)p13);
        args[14] = joy_v_f64((double)p14);
        r = joy_api_call("joy_r3d_font_draw_2d_outline", args, 15, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_font_draw_2d_outline", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_R3dFontCacheSetLimit(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_r3d_font_cache_set_limit", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_font_cache_set_limit", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R3dFontCacheGetCount(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r3d_font_cache_get_count", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_font_cache_get_count", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_R3dFontCacheClear(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_font_cache_clear", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_font_cache_clear", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dFontCacheSweep(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_r3d_font_cache_sweep", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_font_cache_sweep", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dSetLogicalPresentation(uint64_t p0, int32_t p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_i32((int32_t)p2);
        (void)joy_api_call("joy_r3d_set_logical_presentation", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_set_logical_presentation", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dSetLogicalPresentationMode(uint64_t p0, int32_t p1, int32_t p2, int32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_i32((int32_t)p2);
        args[3] = joy_v_i32((int32_t)p3);
        (void)joy_api_call("joy_r3d_set_logical_presentation_mode", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_set_logical_presentation_mode", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dButtonDraw(uint64_t p0, uint64_t p1, uint64_t p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_handle((uint64_t)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_r3d_button_draw", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_button_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dButtonDrawCircle(uint64_t p0, uint64_t p1, uint64_t p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_handle((uint64_t)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_r3d_button_draw_circle", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_button_draw_circle", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R3dButtonIsHovered(uint64_t p0, uint64_t p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_r3d_button_is_hovered", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_button_is_hovered", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_R3dButtonIsPressed(uint64_t p0, uint64_t p1, int32_t p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_bool((int32_t)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        r = joy_api_call("joy_r3d_button_is_pressed", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_button_is_pressed", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_R3dButtonRelease(uint64_t p0, uint64_t p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        (void)joy_api_call("joy_r3d_button_release", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_button_release", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dButtonDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_button_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_button_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dDrawRect2d(uint64_t p0, uint64_t p1, float p2, float p3, float p4, float p5, uint32_t p6, uint32_t p7, uint32_t p8, float p9, float p10)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[11];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        args[8] = joy_v_u32((uint32_t)p8);
        args[9] = joy_v_f64((double)p9);
        args[10] = joy_v_f64((double)p10);
        (void)joy_api_call("joy_r3d_draw_rect_2d", args, 11, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_draw_rect_2d", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dDrawMesh2d(uint64_t p0, uint64_t p1, float p2, float p3, float p4, float p5, uint32_t p6, uint32_t p7, uint32_t p8, float p9, float p10)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[11];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        args[8] = joy_v_u32((uint32_t)p8);
        args[9] = joy_v_f64((double)p9);
        args[10] = joy_v_f64((double)p10);
        (void)joy_api_call("joy_r3d_draw_mesh_2d", args, 11, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_draw_mesh_2d", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dLabelSetText(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        (void)joy_api_call("joy_r3d_label_set_text", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_label_set_text", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dLabelDraw(uint64_t p0, uint64_t p1, uint64_t p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_handle((uint64_t)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_r3d_label_draw", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_label_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dLabelDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_label_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_label_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dInputSetText(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        (void)joy_api_call("joy_r3d_input_set_text", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_set_text", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R3dInputIsHovered(uint64_t p0, uint64_t p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_r3d_input_is_hovered", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_is_hovered", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_R3dInputFocus(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_input_focus", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_focus", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dInputDefocus(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_input_defocus", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_defocus", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R3dInputIsFocused(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r3d_input_is_focused", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_is_focused", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_R3dInputStartTextInput(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_r3d_input_start_text_input", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_start_text_input", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dInputStopTextInput(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_r3d_input_stop_text_input", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_stop_text_input", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dInputInsert(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        (void)joy_api_call("joy_r3d_input_insert", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_insert", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dInputBackspace(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_input_backspace", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_backspace", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dInputDelete(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_input_delete", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_delete", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dInputCursorLeft(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_input_cursor_left", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_cursor_left", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dInputCursorRight(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_input_cursor_right", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_cursor_right", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dInputCursorHome(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_input_cursor_home", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_cursor_home", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dInputCursorEnd(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_input_cursor_end", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_cursor_end", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dInputClick(uint64_t p0, uint64_t p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        (void)joy_api_call("joy_r3d_input_click", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_click", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dInputSubmit(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_input_submit", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_submit", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dInputDraw(uint64_t p0, uint64_t p1, uint64_t p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_handle((uint64_t)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_r3d_input_draw", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dInputDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_input_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_input_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dComboboxInputDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_combobox_input_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_combobox_input_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dComboboxInputAddItem(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        (void)joy_api_call("joy_r3d_combobox_input_add_item", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_combobox_input_add_item", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dComboboxInputClearItems(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_combobox_input_clear_items", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_combobox_input_clear_items", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dComboboxInputSetText(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        (void)joy_api_call("joy_r3d_combobox_input_set_text", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_combobox_input_set_text", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R3dComboboxInputHitItem(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_r3d_combobox_input_hit_item", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_combobox_input_hit_item", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_R3dComboboxInputDraw(uint64_t p0, uint64_t p1, uint64_t p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_handle((uint64_t)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_r3d_combobox_input_draw", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_combobox_input_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dComboboxInputDrawDropdown(uint64_t p0, uint64_t p1, uint64_t p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_handle((uint64_t)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_r3d_combobox_input_draw_dropdown", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_combobox_input_draw_dropdown", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dSliderDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_slider_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_slider_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dSliderSetValue(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_r3d_slider_set_value", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_slider_set_value", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static float icall_R3dSliderGetValue(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r3d_slider_get_value", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_slider_get_value", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static void icall_R3dSliderDraw(uint64_t p0, uint64_t p1, uint64_t p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_handle((uint64_t)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_r3d_slider_draw", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_slider_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R3dSliderIsPressed(uint64_t p0, uint64_t p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_r3d_slider_is_pressed", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_slider_is_pressed", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_R3dSliderSetPressed(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_r3d_slider_set_pressed", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_slider_set_pressed", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dSliderDragMove(uint64_t p0, uint64_t p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        (void)joy_api_call("joy_r3d_slider_drag_move", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_slider_drag_move", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R3dSliderIsHovered(uint64_t p0, uint64_t p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_r3d_slider_is_hovered", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_slider_is_hovered", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_R3dSliderSetHovered(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_r3d_slider_set_hovered", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_slider_set_hovered", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dCheckboxDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_checkbox_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_checkbox_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dCheckboxToggle(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_checkbox_toggle", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_checkbox_toggle", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R3dCheckboxIsPressed(uint64_t p0, uint64_t p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_r3d_checkbox_is_pressed", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_checkbox_is_pressed", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_R3dCheckboxSetHovered(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_r3d_checkbox_set_hovered", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_checkbox_set_hovered", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_R3dCheckboxIsHovered(uint64_t p0, uint64_t p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_r3d_checkbox_is_hovered", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_checkbox_is_hovered", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_R3dCheckboxSetColors(uint64_t p0, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t p7, uint32_t p8, uint32_t p9, uint32_t p10, uint32_t p11, uint32_t p12)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[13];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_u32((uint32_t)p1);
        args[2] = joy_v_u32((uint32_t)p2);
        args[3] = joy_v_u32((uint32_t)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        args[5] = joy_v_u32((uint32_t)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        args[8] = joy_v_u32((uint32_t)p8);
        args[9] = joy_v_u32((uint32_t)p9);
        args[10] = joy_v_u32((uint32_t)p10);
        args[11] = joy_v_u32((uint32_t)p11);
        args[12] = joy_v_u32((uint32_t)p12);
        (void)joy_api_call("joy_r3d_checkbox_set_colors", args, 13, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_checkbox_set_colors", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dCheckboxDraw(uint64_t p0, uint64_t p1, uint64_t p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_handle((uint64_t)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_r3d_checkbox_draw", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_checkbox_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_R3dDrawGrid(uint64_t p0, int32_t p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        (void)joy_api_call("joy_r3d_draw_grid", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_draw_grid", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static uint64_t icall_R3dMeshCubeLit(uint64_t p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_r3d_mesh_cube_lit", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_mesh_cube_lit", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static uint64_t icall_R3dMeshSphereLit(uint64_t p0, float p1, int32_t p2, int32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_i32((int32_t)p2);
        args[3] = joy_v_i32((int32_t)p3);
        r = joy_api_call("joy_r3d_mesh_sphere_lit", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_mesh_sphere_lit", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static uint64_t icall_R3dMeshPlaneLit(uint64_t p0, float p1, float p2, int32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_i32((int32_t)p3);
        r = joy_api_call("joy_r3d_mesh_plane_lit", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_mesh_plane_lit", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static uint64_t icall_R3dMeshRampLit(uint64_t p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_r3d_mesh_ramp_lit", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_mesh_ramp_lit", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static void icall_JanimatorDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_scene_animator_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorSetFloat(uint64_t p0, MonoString* p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_scene_animator_set_float", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_set_float", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorSetInt(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        (void)joy_api_call("joy_scene_animator_set_int", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_set_int", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorSetBool(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_bool((int32_t)p2);
        (void)joy_api_call("joy_scene_animator_set_bool", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_set_bool", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorSetTrigger(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        (void)joy_api_call("joy_scene_animator_set_trigger", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_set_trigger", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static float icall_JanimatorGetFloat(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        r = joy_api_call("joy_scene_animator_get_float", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_get_float", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static int32_t icall_JanimatorGetInt(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        r = joy_api_call("joy_scene_animator_get_int", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_get_int", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_JanimatorGetBool(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        r = joy_api_call("joy_scene_animator_get_bool", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_get_bool", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_JanimatorAddState(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        (void)joy_api_call("joy_scene_animator_add_state", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_add_state", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorSetDefaultState(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        (void)joy_api_call("joy_scene_animator_set_default_state", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_set_default_state", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorStateAddClip(uint64_t p0, MonoString* p1, MonoString* p2, float p3, float p4, float p5, float p6, float p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[8];
        char* s1 = NULL;
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        args[7] = joy_v_f64((double)p7);
        (void)joy_api_call("joy_scene_animator_state_add_clip", args, 8, &st);
        if (s1) mono_free(s1);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_state_add_clip", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorStateAddColor(uint64_t p0, MonoString* p1, float p2, int32_t p3, int32_t p4, uint32_t p5, uint32_t p6, uint32_t p7, uint32_t p8)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[9];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_i32((int32_t)p3);
        args[4] = joy_v_i32((int32_t)p4);
        args[5] = joy_v_u32((uint32_t)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        args[8] = joy_v_u32((uint32_t)p8);
        (void)joy_api_call("joy_scene_animator_state_add_color", args, 9, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_state_add_color", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorStateSetLoop(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_bool((int32_t)p2);
        (void)joy_api_call("joy_scene_animator_state_set_loop", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_state_set_loop", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorStateSetSpeed(uint64_t p0, MonoString* p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_scene_animator_state_set_speed", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_state_set_speed", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorStateSetBlend(uint64_t p0, MonoString* p1, MonoString* p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        (void)joy_api_call("joy_scene_animator_state_set_blend", args, 3, &st);
        if (s1) mono_free(s1);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_state_set_blend", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorStateAddMotion(uint64_t p0, MonoString* p1, MonoString* p2, float p3, float p4, float p5, float p6, float p7, float p8)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[9];
        char* s1 = NULL;
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        args[7] = joy_v_f64((double)p7);
        args[8] = joy_v_f64((double)p8);
        (void)joy_api_call("joy_scene_animator_state_add_motion", args, 9, &st);
        if (s1) mono_free(s1);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_state_add_motion", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_JanimatorAddTransition(uint64_t p0, MonoString* p1, MonoString* p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        char* s1 = NULL;
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        r = joy_api_call("joy_scene_animator_add_transition", args, 3, &st);
        if (s1) mono_free(s1);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_add_transition", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_JanimatorAddAnyTransition(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        r = joy_api_call("joy_scene_animator_add_any_transition", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_add_any_transition", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_JanimatorTransitionAddCond(uint64_t p0, int32_t p1, MonoString* p2, int32_t p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_i32((int32_t)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_scene_animator_transition_add_cond", args, 5, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_transition_add_cond", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorTransitionSetExitTime(uint64_t p0, int32_t p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_scene_animator_transition_set_exit_time", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_transition_set_exit_time", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorUpdate(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_scene_animator_update", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorDraw(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_scene_animator_draw", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JanimatorSetFlipH(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_scene_animator_set_flip_h", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_animator_set_flip_h", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_LabelDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_label_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_label_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_LabelDraw(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_label_draw", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_label_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ButtonDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_button_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ButtonSetPosition(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_button_set_position", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_set_position", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ButtonSetSize(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_button_set_size", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_set_size", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ButtonSetNormalColor(uint64_t p0, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_u32((uint32_t)p1);
        args[2] = joy_v_u32((uint32_t)p2);
        args[3] = joy_v_u32((uint32_t)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        (void)joy_api_call("joy_ui_button_set_normal_color", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_set_normal_color", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ButtonSetHoverColor(uint64_t p0, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_u32((uint32_t)p1);
        args[2] = joy_v_u32((uint32_t)p2);
        args[3] = joy_v_u32((uint32_t)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        (void)joy_api_call("joy_ui_button_set_hover_color", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_set_hover_color", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ButtonSetPressedColor(uint64_t p0, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_u32((uint32_t)p1);
        args[2] = joy_v_u32((uint32_t)p2);
        args[3] = joy_v_u32((uint32_t)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        (void)joy_api_call("joy_ui_button_set_pressed_color", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_set_pressed_color", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ButtonSetImage(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_ui_button_set_image", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_set_image", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ButtonSetPressedImage(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_ui_button_set_pressed_image", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_set_pressed_image", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ButtonSetTextx(uint64_t p0, uint64_t p1, MonoString* p2, int32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[8];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_i32((int32_t)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        args[5] = joy_v_u32((uint32_t)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        (void)joy_api_call("joy_ui_button_set_textx", args, 8, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_set_textx", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_ButtonIsHovered(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_button_is_hovered", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_is_hovered", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_ButtonIsPressed(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_button_is_pressed", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_is_pressed", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_ButtonSetHovered(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_ui_button_set_hovered", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_set_hovered", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ButtonSetPressed(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_ui_button_set_pressed", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_set_pressed", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_ButtonGetPressed(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_button_get_pressed", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_get_pressed", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_ButtonSetTextOffset(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_button_set_text_offset", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_set_text_offset", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ButtonDraw(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_button_draw", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ScrollviewDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_scrollview_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scrollview_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ScrollviewSetViewPx(uint64_t p0, float p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_ui_scrollview_set_view_px", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scrollview_set_view_px", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ScrollviewSetContent(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_scrollview_set_content", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scrollview_set_content", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ScrollviewSetOffset(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_scrollview_set_offset", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scrollview_set_offset", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ScrollviewClamp(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_scrollview_clamp", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scrollview_clamp", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static float icall_ScrollviewGetOffsetX(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_scrollview_get_offset_x", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scrollview_get_offset_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_ScrollviewGetOffsetY(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_scrollview_get_offset_y", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scrollview_get_offset_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static void icall_ScrollviewSetShowBar(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_ui_scrollview_set_show_bar", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scrollview_set_show_bar", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_ScrollviewDragging(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_scrollview_dragging", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scrollview_dragging", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_ScrollviewHit(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_scrollview_hit", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scrollview_hit", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_ScrollviewBegin(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_scrollview_begin", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scrollview_begin", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ScrollviewEnd(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_scrollview_end", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scrollview_end", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ComboboxDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_combobox_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ComboboxAddItem(uint64_t p0, uint64_t p1, MonoString* p2, int32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[8];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_i32((int32_t)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        args[5] = joy_v_u32((uint32_t)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        (void)joy_api_call("joy_ui_combobox_add_item", args, 8, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_add_item", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_ComboboxIsPressed(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_combobox_is_pressed", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_is_pressed", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_ComboboxIsOpen(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_combobox_is_open", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_is_open", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_ComboboxSetOpen(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_ui_combobox_set_open", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_set_open", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_ComboboxHitItem(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_combobox_hit_item", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_hit_item", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_ComboboxSetSelected(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_ui_combobox_set_selected", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_set_selected", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_ComboboxGetSelected(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_combobox_get_selected", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_get_selected", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_ComboboxDraw(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_combobox_draw", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ComboboxInputDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_combobox_input_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_input_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ComboboxInputAddItem(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        (void)joy_api_call("joy_ui_combobox_input_add_item", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_input_add_item", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ComboboxInputClearItems(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_combobox_input_clear_items", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_input_clear_items", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ComboboxInputSetText(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        (void)joy_api_call("joy_ui_combobox_input_set_text", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_input_set_text", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ComboboxInputDraw(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_combobox_input_draw", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_input_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ComboboxInputDrawDropdown(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_combobox_input_draw_dropdown", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_input_draw_dropdown", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_ComboboxInputIsOpen(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_combobox_input_is_open", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_input_is_open", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_ComboboxInputSetPopupNode(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_ui_combobox_input_set_popup_node", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_input_set_popup_node", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_ComboboxInputHitItem(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_combobox_input_hit_item", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_input_hit_item", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_ComboboxInputSetOpen(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_ui_combobox_input_set_open", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_combobox_input_set_open", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_DatagridSetheaders(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        (void)joy_api_call("joy_ui_datagrid_setheaders", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_datagrid_setheaders", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_DatagridSetheader(uint64_t p0, int32_t p1, MonoString* p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        (void)joy_api_call("joy_ui_datagrid_setheader", args, 3, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_datagrid_setheader", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_DatagridSetcell(uint64_t p0, int32_t p1, int32_t p2, MonoString* p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        char* s3 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_i32((int32_t)p2);
        s3 = p3 ? mono_string_to_utf8(p3) : NULL;
        args[3] = joy_v_str((const char*)s3);
        (void)joy_api_call("joy_ui_datagrid_setcell", args, 4, &st);
        if (s3) mono_free(s3);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_datagrid_setcell", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_DatagridDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_datagrid_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_datagrid_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_DatagridDraw(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_ui_datagrid_draw", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_datagrid_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_DatagridHitRow(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_datagrid_hit_row", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_datagrid_hit_row", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_DatagridScroll(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_ui_datagrid_scroll", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_datagrid_scroll", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_DatagridSetSelectedRow(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_ui_datagrid_set_selected_row", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_datagrid_set_selected_row", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_RadioDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_radio_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_radio_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_RadioIsPressed(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_radio_is_pressed", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_radio_is_pressed", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_RadioSetHovered(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_ui_radio_set_hovered", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_radio_set_hovered", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_CheckboxDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_checkbox_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_checkbox_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_CheckboxSetColors(uint64_t p0, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t p7, uint32_t p8, uint32_t p9, uint32_t p10, uint32_t p11, uint32_t p12)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[13];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_u32((uint32_t)p1);
        args[2] = joy_v_u32((uint32_t)p2);
        args[3] = joy_v_u32((uint32_t)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        args[5] = joy_v_u32((uint32_t)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        args[8] = joy_v_u32((uint32_t)p8);
        args[9] = joy_v_u32((uint32_t)p9);
        args[10] = joy_v_u32((uint32_t)p10);
        args[11] = joy_v_u32((uint32_t)p11);
        args[12] = joy_v_u32((uint32_t)p12);
        (void)joy_api_call("joy_ui_checkbox_set_colors", args, 13, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_checkbox_set_colors", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_CheckboxSetR2d(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_ui_checkbox_set_r2d", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_checkbox_set_r2d", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_CheckboxSetPosition(uint64_t p0, int32_t p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_i32((int32_t)p2);
        (void)joy_api_call("joy_ui_checkbox_set_position", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_checkbox_set_position", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_CheckboxSetTextColor(uint64_t p0, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_u32((uint32_t)p1);
        args[2] = joy_v_u32((uint32_t)p2);
        args[3] = joy_v_u32((uint32_t)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        (void)joy_api_call("joy_ui_checkbox_set_text_color", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_checkbox_set_text_color", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_CheckboxToggle(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_checkbox_toggle", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_checkbox_toggle", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_CheckboxIsPressed(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_checkbox_is_pressed", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_checkbox_is_pressed", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_CheckboxSetHovered(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_ui_checkbox_set_hovered", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_checkbox_set_hovered", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoystickDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_joystick_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_joystick_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoystickSetTextures(uint64_t p0, MonoString* p1, MonoString* p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        (void)joy_api_call("joy_ui_joystick_set_textures", args, 3, &st);
        if (s1) mono_free(s1);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_joystick_set_textures", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoystickSetVector(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_joystick_set_vector", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_joystick_set_vector", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoystickDraw(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_joystick_draw", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_joystick_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoystickReset(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_joystick_reset", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_joystick_reset", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoystickSetPosition(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_joystick_set_position", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_joystick_set_position", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoystickSetRadius(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_ui_joystick_set_radius", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_joystick_set_radius", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_JoystickIsDragging(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_joystick_is_dragging", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_joystick_is_dragging", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_JoystickTouchDown(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_joystick_touch_down", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_joystick_touch_down", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoystickTouchMove(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_joystick_touch_move", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_joystick_touch_move", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoystickTouchUp(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_joystick_touch_up", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_joystick_touch_up", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static float icall_JoystickGetMagnitude(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_joystick_get_magnitude", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_joystick_get_magnitude", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static void icall_InputboxDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxSetText(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        (void)joy_api_call("joy_ui_inputbox_set_text", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_set_text", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxDraw(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_draw", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_InputboxIsPressed(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_inputbox_is_pressed", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_is_pressed", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_InputboxIsHovered(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_inputbox_is_hovered", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_is_hovered", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_InputboxFocus(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_focus", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_focus", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxDefocus(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_defocus", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_defocus", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxBlur(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_blur", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_blur", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxInsert(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        (void)joy_api_call("joy_ui_inputbox_insert", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_insert", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxBackspace(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_backspace", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_backspace", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxDelete(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_delete", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_delete", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxCursorLeft(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_cursor_left", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_cursor_left", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxCursorRight(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_cursor_right", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_cursor_right", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxCursorHome(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_cursor_home", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_cursor_home", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxCursorEnd(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_cursor_end", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_cursor_end", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxSelectLeft(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_select_left", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_select_left", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxSelectRight(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_select_right", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_select_right", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxSelectHome(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_select_home", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_select_home", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxSelectEnd(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_select_end", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_select_end", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxSelectAll(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_select_all", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_select_all", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxPaste(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_paste", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_paste", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxCopy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_copy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_copy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_InputboxCut(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_inputbox_cut", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_cut", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_InputboxIsFocused(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_inputbox_is_focused", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_inputbox_is_focused", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_SliderDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_slider_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_slider_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SliderSetValue(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_ui_slider_set_value", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_slider_set_value", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static float icall_SliderGetValue(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_slider_get_value", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_slider_get_value", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static void icall_SliderSetRect(uint64_t p0, float p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_ui_slider_set_rect", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_slider_set_rect", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SliderSetRange(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_slider_set_range", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_slider_set_range", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SliderDraw(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_slider_draw", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_slider_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_SliderIsPressed(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_slider_is_pressed", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_slider_is_pressed", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_SliderSetPressed(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_ui_slider_set_pressed", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_slider_set_pressed", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SliderDragMove(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_slider_drag_move", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_slider_drag_move", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_SliderIsHovered(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_slider_is_hovered", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_slider_is_hovered", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_SliderSetHovered(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_ui_slider_set_hovered", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_slider_set_hovered", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_PopupDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_popup_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_popup_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_PopupSetTitle(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        (void)joy_api_call("joy_ui_popup_set_title", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_popup_set_title", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_PopupSetContent(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        (void)joy_api_call("joy_ui_popup_set_content", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_popup_set_content", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_PopupSetPosition(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_popup_set_position", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_popup_set_position", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_PopupSetVisible(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_ui_popup_set_visible", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_popup_set_visible", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_PopupSetCloseable(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_ui_popup_set_closeable", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_popup_set_closeable", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_PopupDraw(uint64_t p0, uint64_t p1, uint64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_handle((uint64_t)p2);
        (void)joy_api_call("joy_ui_popup_draw", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_popup_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_PopupIsCloseClicked(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_popup_is_close_clicked", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_popup_is_close_clicked", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_PopupIsTitleClicked(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_popup_is_title_clicked", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_popup_is_title_clicked", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_PopupDragMove(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_popup_drag_move", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_popup_drag_move", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_PopupIsInside(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_ui_popup_is_inside", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_popup_is_inside", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_TooltipDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_tooltip_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_tooltip_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_TooltipSetText(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        (void)joy_api_call("joy_ui_tooltip_set_text", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_tooltip_set_text", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_TooltipSetPosition(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_tooltip_set_position", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_tooltip_set_position", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_TooltipSetVisible(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_ui_tooltip_set_visible", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_tooltip_set_visible", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_TooltipDraw(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_ui_tooltip_draw", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_tooltip_draw", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiCtxDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_ctx_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_ctx_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiBegin(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_begin", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_begin", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiEnd(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_end", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_end", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiInputMotion(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_ui_input_motion", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_input_motion", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiInputDown(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_ui_input_down", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_input_down", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiInputUp(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_ui_input_up", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_input_up", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiInputWheel(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_ui_input_wheel", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_input_wheel", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiInputKey(uint64_t p0, int32_t p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        args[2] = joy_v_bool((int32_t)p2);
        (void)joy_api_call("joy_ui_input_key", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_input_key", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_JuiIsDown(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_is_down", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_is_down", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_JuiIsPressed(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_is_pressed", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_is_pressed", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_JuiIsReleased(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_is_released", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_is_released", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static float icall_JuiTakeWheel(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_take_wheel", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_take_wheel", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static int32_t icall_JuiIsShift(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_is_shift", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_is_shift", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_JuiIsCtrl(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_ui_is_ctrl", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_is_ctrl", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_JuiPushClip(uint64_t p0, float p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_ui_push_clip", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_push_clip", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiPopClip(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_pop_clip", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_pop_clip", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiResetClip(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_reset_clip", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_reset_clip", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_JuiHit(uint64_t p0, float p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        r = joy_api_call("joy_ui_hit", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_hit", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_JuiFillRect(uint64_t p0, float p1, float p2, float p3, float p4, uint32_t p5, uint32_t p6, uint32_t p7, uint32_t p8)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[9];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_u32((uint32_t)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        args[8] = joy_v_u32((uint32_t)p8);
        (void)joy_api_call("joy_ui_fill_rect", args, 9, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_fill_rect", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiStrokeRect(uint64_t p0, float p1, float p2, float p3, float p4, uint32_t p5, uint32_t p6, uint32_t p7, uint32_t p8)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[9];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_u32((uint32_t)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        args[8] = joy_v_u32((uint32_t)p8);
        (void)joy_api_call("joy_ui_stroke_rect", args, 9, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_stroke_rect", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiLine(uint64_t p0, float p1, float p2, float p3, float p4, uint32_t p5, uint32_t p6, uint32_t p7, uint32_t p8)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[9];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_u32((uint32_t)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        args[8] = joy_v_u32((uint32_t)p8);
        (void)joy_api_call("joy_ui_line", args, 9, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_line", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_JuiText(uint64_t p0, float p1, float p2, MonoString* p3, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        char* s3 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        s3 = p3 ? mono_string_to_utf8(p3) : NULL;
        args[3] = joy_v_str((const char*)s3);
        args[4] = joy_v_u32((uint32_t)p4);
        args[5] = joy_v_u32((uint32_t)p5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        r = joy_api_call("joy_ui_text", args, 8, &st);
        if (s3) mono_free(s3);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_text", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_JuiTextCentered(uint64_t p0, float p1, float p2, float p3, float p4, MonoString* p5, uint32_t p6, uint32_t p7, uint32_t p8, uint32_t p9)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[10];
        char* s5 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        s5 = p5 ? mono_string_to_utf8(p5) : NULL;
        args[5] = joy_v_str((const char*)s5);
        args[6] = joy_v_u32((uint32_t)p6);
        args[7] = joy_v_u32((uint32_t)p7);
        args[8] = joy_v_u32((uint32_t)p8);
        args[9] = joy_v_u32((uint32_t)p9);
        r = joy_api_call("joy_ui_text_centered", args, 10, &st);
        if (s5) mono_free(s5);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_text_centered", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_JuiPanelBegin(uint64_t p0, float p1, float p2, float p3, float p4, MonoString* p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[6];
        char* s5 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        s5 = p5 ? mono_string_to_utf8(p5) : NULL;
        args[5] = joy_v_str((const char*)s5);
        (void)joy_api_call("joy_ui_panel_begin", args, 6, &st);
        if (s5) mono_free(s5);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_panel_begin", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiPanelEnd(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_panel_end", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_panel_end", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiPanelFrame(uint64_t p0, float p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_ui_panel_frame", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_panel_frame", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static float icall_JuiScrollBegin(uint64_t p0, int32_t p1, float p2, float p3, float p4, float p5, float p6)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[7];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        r = joy_api_call("joy_ui_scroll_begin", args, 7, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scroll_begin", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static void icall_JuiScrollEnd(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_ui_scroll_end", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scroll_end", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiScrollbar(uint64_t p0, float p1, float p2, float p3, float p4, float p5, float p6)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[7];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        (void)joy_api_call("joy_ui_scrollbar", args, 7, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scrollbar", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JuiScrollSetOffset(uint64_t p0, int32_t p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        (void)joy_api_call("joy_ui_scroll_set_offset", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_scroll_set_offset", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_JuiButton(uint64_t p0, int32_t p1, float p2, float p3, float p4, float p5, MonoString* p6)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[7];
        char* s6 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        s6 = p6 ? mono_string_to_utf8(p6) : NULL;
        args[6] = joy_v_str((const char*)s6);
        r = joy_api_call("joy_ui_button", args, 7, &st);
        if (s6) mono_free(s6);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_ui_button", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_JtilemapDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_scene_tilemap_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_tilemap_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_JtilemapGetWidth(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_scene_tilemap_get_width", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_tilemap_get_width", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_JtilemapGetHeight(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_scene_tilemap_get_height", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_tilemap_get_height", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_JtilemapGetTilewidth(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_scene_tilemap_get_tilewidth", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_tilemap_get_tilewidth", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_JtilemapGetTileheight(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_scene_tilemap_get_tileheight", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_tilemap_get_tileheight", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_JtilemapGetAnimTileid(uint64_t p0, int32_t p1, uint32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_u32((uint32_t)p2);
        r = joy_api_call("joy_scene_tilemap_get_anim_tileid", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_tilemap_get_anim_tileid", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static uint64_t icall_JtilemapFindTileLayer(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        r = joy_api_call("joy_scene_tilemap_find_tile_layer", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_tilemap_find_tile_layer", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static uint64_t icall_JtilemapFindObjectLayer(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        r = joy_api_call("joy_scene_tilemap_find_object_layer", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_tilemap_find_object_layer", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static int32_t icall_JtilemapGetTile(uint64_t p0, int32_t p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_i32((int32_t)p2);
        r = joy_api_call("joy_scene_tilemap_get_tile", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_tilemap_get_tile", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_JtilemapRenderAll(uint64_t p0, uint64_t p1, float p2, float p3, uint32_t p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        (void)joy_api_call("joy_scene_tilemap_render_all", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_tilemap_render_all", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JtilemapRenderTiles(uint64_t p0, uint64_t p1, float p2, float p3, uint32_t p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_u32((uint32_t)p4);
        (void)joy_api_call("joy_scene_tilemap_render_tiles", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_tilemap_render_tiles", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JtilemapRenderSetTilePx(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_scene_tilemap_render_set_tile_px", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_tilemap_render_set_tile_px", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_JtilemapRenderLayer(uint64_t p0, uint64_t p1, MonoString* p2, float p3, float p4, uint32_t p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_u32((uint32_t)p5);
        r = joy_api_call("joy_scene_tilemap_render_layer", args, 6, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_tilemap_render_layer", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_JtilemapRenderDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_scene_tilemap_render_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_tilemap_render_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_KcpserverDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_net_kcpserver_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_kcpserver_destroy", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_KcpserverSend(uint64_t p0, int32_t p1, MonoString* p2, int32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_i32((int32_t)p3);
        (void)joy_api_call("joy_net_kcpserver_send", args, 4, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_kcpserver_send", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_KcpserverBroadcast(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        (void)joy_api_call("joy_net_kcpserver_broadcast", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_kcpserver_broadcast", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_KcpserverOffline(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_net_kcpserver_offline", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_kcpserver_offline", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_KcpserverUpdate(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_kcpserver_update", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_kcpserver_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_KcpserverConnectionCount(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_net_kcpserver_connection_count", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_kcpserver_connection_count", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_KcpserverPollMessage(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        r = joy_api_call("joy_net_kcpserver_poll_message", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_kcpserver_poll_message", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_KcpclientDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_kcpclient_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_kcpclient_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_KcpclientSend(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        r = joy_api_call("joy_net_kcpclient_send", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_kcpclient_send", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_KcpclientUpdate(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_kcpclient_update", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_kcpclient_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_KcpclientPollMessage(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        r = joy_api_call("joy_net_kcpclient_poll_message", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_kcpclient_poll_message", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_TcpserverDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_net_tcpserver_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_tcpserver_destroy", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_TcpserverSend(uint64_t p0, int32_t p1, MonoString* p2, int32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_i32((int32_t)p3);
        (void)joy_api_call("joy_net_tcpserver_send", args, 4, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_tcpserver_send", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_TcpserverBroadcast(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        (void)joy_api_call("joy_net_tcpserver_broadcast", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_tcpserver_broadcast", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_TcpserverOffline(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_net_tcpserver_offline", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_tcpserver_offline", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_TcpserverUpdate(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_tcpserver_update", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_tcpserver_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_TcpserverConnectionCount(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_net_tcpserver_connection_count", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_tcpserver_connection_count", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_TcpserverPollMessage(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        r = joy_api_call("joy_net_tcpserver_poll_message", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_tcpserver_poll_message", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_TcpclientDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_tcpclient_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_tcpclient_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_TcpclientSend(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        r = joy_api_call("joy_net_tcpclient_send", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_tcpclient_send", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_TcpclientUpdate(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_tcpclient_update", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_tcpclient_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_TcpclientPollMessage(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        r = joy_api_call("joy_net_tcpclient_poll_message", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_tcpclient_poll_message", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_WsnetserverDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_net_wsnetserver_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_wsnetserver_destroy", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_WsnetserverSend(uint64_t p0, int32_t p1, MonoString* p2, int32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_i32((int32_t)p3);
        (void)joy_api_call("joy_net_wsnetserver_send", args, 4, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_wsnetserver_send", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_WsnetserverBroadcast(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        (void)joy_api_call("joy_net_wsnetserver_broadcast", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_wsnetserver_broadcast", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_WsnetserverOffline(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_net_wsnetserver_offline", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_wsnetserver_offline", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_WsnetserverUpdate(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_wsnetserver_update", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_wsnetserver_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_WsnetserverConnectionCount(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_net_wsnetserver_connection_count", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_wsnetserver_connection_count", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_WsnetserverPollMessage(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        r = joy_api_call("joy_net_wsnetserver_poll_message", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_wsnetserver_poll_message", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_WsnetclientDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_wsnetclient_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_wsnetclient_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_WsnetclientSend(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        r = joy_api_call("joy_net_wsnetclient_send", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_wsnetclient_send", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_WsnetclientUpdate(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_wsnetclient_update", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_wsnetclient_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_WsnetclientPollMessage(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        r = joy_api_call("joy_net_wsnetclient_poll_message", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_wsnetclient_poll_message", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_NetclientDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_netclient_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_netclient_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_NetclientSend(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        r = joy_api_call("joy_net_netclient_send", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_netclient_send", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_NetclientUpdate(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_netclient_update", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_netclient_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_NetclientPollMessage(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        r = joy_api_call("joy_net_netclient_poll_message", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_netclient_poll_message", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_NetserverDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_netserver_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_netserver_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_NetserverSend(uint64_t p0, int32_t p1, MonoString* p2, int32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        char* s2 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        s2 = p2 ? mono_string_to_utf8(p2) : NULL;
        args[2] = joy_v_str((const char*)s2);
        args[3] = joy_v_i32((int32_t)p3);
        (void)joy_api_call("joy_net_netserver_send", args, 4, &st);
        if (s2) mono_free(s2);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_netserver_send", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_NetserverBroadcast(uint64_t p0, MonoString* p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_i32((int32_t)p2);
        (void)joy_api_call("joy_net_netserver_broadcast", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_netserver_broadcast", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_NetserverOffline(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_net_netserver_offline", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_netserver_offline", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_NetserverUpdate(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_netserver_update", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_netserver_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_NetserverConnectionCount(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_net_netserver_connection_count", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_netserver_connection_count", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_NetserverPollMessage(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        r = joy_api_call("joy_net_netserver_poll_message", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_netserver_poll_message", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_NetmessageFreeData(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_net_netmessage_free_data", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_net_netmessage_free_data", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneNodeDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_scene_node_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_node_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneNodeAddChild(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_scene_node_add_child", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_node_add_child", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneNodeRemoveChild(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_scene_node_remove_child", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_node_remove_child", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_SceneNodeGetChildCount(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_scene_node_get_child_count", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_node_get_child_count", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_SceneNodeSetPosition(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_scene_node_set_position", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_node_set_position", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneNodeSetWorldPosition(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_scene_node_set_world_position", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_node_set_world_position", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneNodeSetRotation(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_scene_node_set_rotation", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_node_set_rotation", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static float icall_SceneNodeGetRotation(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_scene_node_get_rotation", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_node_get_rotation", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static void icall_SceneNodeSetScale(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_scene_node_set_scale", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_node_set_scale", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneNodeSetZorder(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_scene_node_set_zorder", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_node_set_zorder", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_SceneNodeGetZorder(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_scene_node_get_zorder", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_node_get_zorder", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_SceneNodeSetName(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        (void)joy_api_call("joy_scene_node_set_name", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_node_set_name", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneNodeSetTag(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        (void)joy_api_call("joy_scene_node_set_tag", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_node_set_tag", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static uint64_t icall_SceneFindNodeByName(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        r = joy_api_call("joy_scene_find_node_by_name", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_find_node_by_name", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static uint64_t icall_SceneFindNodeByNameDeep(uint64_t p0, MonoString* p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        r = joy_api_call("joy_scene_find_node_by_name_deep", args, 2, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_find_node_by_name_deep", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static void icall_SceneNodeUpdate(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_scene_node_update", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_node_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_scene_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneAddRootNode(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_scene_add_root_node", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_add_root_node", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneRemoveRootNode(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_scene_remove_root_node", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_remove_root_node", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_SceneGetRootCount(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_scene_get_root_count", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_get_root_count", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static uint64_t icall_SceneGetRootAt(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        r = joy_api_call("joy_scene_get_root_at", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_get_root_at", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static void icall_SceneUpdate(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_scene_update", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneSetPopupNode(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_scene_set_popup_node", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_set_popup_node", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static uint64_t icall_SceneGetPopupNode(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_scene_get_popup_node", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_get_popup_node", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static void icall_SceneManagerDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_scene_manager_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_manager_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneManagerPush(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_scene_manager_push", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_manager_push", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneManagerReplace(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_scene_manager_replace", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_manager_replace", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneManagerPop(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_scene_manager_pop", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_manager_pop", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_SceneManagerUpdate(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_scene_manager_update", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_manager_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static uint64_t icall_SceneManagerGetCurrent(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_scene_manager_get_current", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_manager_get_current", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.handle;
}

static int32_t icall_SceneManagerGetCount(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_scene_manager_get_count", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_manager_get_count", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_JparticleDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r2d_particle_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_particle_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JparticleSetPosition(uint64_t p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        (void)joy_api_call("joy_r2d_particle_set_position", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_particle_set_position", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JparticleEmit(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r2d_particle_emit", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_particle_emit", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JparticleClear(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r2d_particle_clear", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_particle_clear", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JparticleUpdate(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_r2d_particle_update", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_particle_update", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JparticleRender(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_r2d_particle_render", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_particle_render", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_JparticleCount(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r2d_particle_count", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_particle_count", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int32_t icall_JparticleIsActive(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        r = joy_api_call("joy_r2d_particle_is_active", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_particle_is_active", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_JlightSysDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_light_sys_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_light_sys_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JlightSysResize(uint64_t p0, int32_t p1, int32_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        args[2] = joy_v_i32((int32_t)p2);
        (void)joy_api_call("joy_r3d_light_sys_resize", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_light_sys_resize", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JlightRemove(uint64_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_i32((int32_t)p1);
        (void)joy_api_call("joy_r3d_light_remove", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_light_remove", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JlightClear(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_light_clear", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_light_clear", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_JlightAddOccluder(uint64_t p0, float p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[5];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        r = joy_api_call("joy_r3d_light_add_occluder", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_light_add_occluder", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static void icall_JlightClearOccluders(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_r3d_light_clear_occluders", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_light_clear_occluders", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JlightSetAmbient(uint64_t p0, uint32_t p1, uint32_t p2, uint32_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[4];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_u32((uint32_t)p1);
        args[2] = joy_v_u32((uint32_t)p2);
        args[3] = joy_v_u32((uint32_t)p3);
        (void)joy_api_call("joy_r3d_light_set_ambient", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_light_set_ambient", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JlightRender(uint64_t p0, uint64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        (void)joy_api_call("joy_r3d_light_render", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r3d_light_render", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JecsDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_scene_ecs_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_ecs_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JecsProcess(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_scene_ecs_process", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_ecs_process", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JecsDefine(uint64_t p0, MonoString* p1, uint64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[3];
        char* s1 = NULL;
        args[0] = joy_v_handle((uint64_t)p0);
        s1 = p1 ? mono_string_to_utf8(p1) : NULL;
        args[1] = joy_v_str((const char*)s1);
        args[2] = joy_v_u64((uint64_t)p2);
        (void)joy_api_call("joy_scene_ecs_define", args, 3, &st);
        if (s1) mono_free(s1);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_scene_ecs_define", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static uint32_t icall_AudioOpenDevice(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_audio_open_device", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_audio_open_device", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint32_t)r.as.u32;
}

static void icall_AudioCloseDevice(uint32_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_u32((uint32_t)p0);
        (void)joy_api_call("joy_audio_close_device", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_audio_close_device", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_AudioCreateWav(MonoString* p0, uint32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s0 = NULL;
        s0 = p0 ? mono_string_to_utf8(p0) : NULL;
        args[0] = joy_v_str((const char*)s0);
        args[1] = joy_v_u32((uint32_t)p1);
        r = joy_api_call("joy_audio_create_wav", args, 2, &st);
        if (s0) mono_free(s0);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_audio_create_wav", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_AudioCreateMp3(MonoString* p0, uint32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s0 = NULL;
        s0 = p0 ? mono_string_to_utf8(p0) : NULL;
        args[0] = joy_v_str((const char*)s0);
        args[1] = joy_v_u32((uint32_t)p1);
        r = joy_api_call("joy_audio_create_mp3", args, 2, &st);
        if (s0) mono_free(s0);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_audio_create_mp3", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_AudioCreateFlac(MonoString* p0, uint32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s0 = NULL;
        s0 = p0 ? mono_string_to_utf8(p0) : NULL;
        args[0] = joy_v_str((const char*)s0);
        args[1] = joy_v_u32((uint32_t)p1);
        r = joy_api_call("joy_audio_create_flac", args, 2, &st);
        if (s0) mono_free(s0);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_audio_create_flac", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_AudioCreateOgg(MonoString* p0, uint32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        char* s0 = NULL;
        s0 = p0 ? mono_string_to_utf8(p0) : NULL;
        args[0] = joy_v_str((const char*)s0);
        args[1] = joy_v_u32((uint32_t)p1);
        r = joy_api_call("joy_audio_create_ogg", args, 2, &st);
        if (s0) mono_free(s0);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_audio_create_ogg", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static void icall_AudioShotPlay(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_audio_shot_play", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_audio_shot_play", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_AudioShotSetGain(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_audio_shot_set_gain", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_audio_shot_set_gain", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_AudioShotDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_audio_shot_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_audio_shot_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_AudioBgmStop(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_audio_bgm_stop", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_audio_bgm_stop", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_AudioBgmSetGain(uint64_t p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_audio_bgm_set_gain", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_audio_bgm_set_gain", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_AudioBgmDestroy(uint64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_handle((uint64_t)p0);
        (void)joy_api_call("joy_audio_bgm_destroy", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_audio_bgm_destroy", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_ImageDrawRect(uint64_t p0, uint64_t p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[6];
        args[0] = joy_v_handle((uint64_t)p0);
        args[1] = joy_v_handle((uint64_t)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        (void)joy_api_call("joy_r2d_image_draw_rect", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_r2d_image_draw_rect", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoyInputFeedKey(int32_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_i32((int32_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_input_feed_key", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_input_feed_key", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoyInputFeedMouseMove(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        (void)joy_api_call("joy_input_feed_mouse_move", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_input_feed_mouse_move", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoyInputFeedMouseButton(int32_t p0, int32_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[2];
        args[0] = joy_v_i32((int32_t)p0);
        args[1] = joy_v_bool((int32_t)p1);
        (void)joy_api_call("joy_input_feed_mouse_button", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_input_feed_mouse_button", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoyInputFeedWheel(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        (void)joy_api_call("joy_input_feed_wheel", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_input_feed_wheel", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoyInputEndFrame(void)
{
        joy_status_t st = JOY_OK;
        (void)joy_api_call("joy_input_end_frame", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_input_end_frame", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static void icall_JoyInputReset(void)
{
        joy_status_t st = JOY_OK;
        (void)joy_api_call("joy_input_reset", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_input_reset", joy_status_name(st), joy_last_error_where());
                return;
        }
        return;
}

static int32_t icall_JoyInputIsKeyDown(int32_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i32((int32_t)p0);
        r = joy_api_call("joy_input_is_key_down", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_input_is_key_down", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_JoyInputIsMouseButtonDown(int32_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i32((int32_t)p0);
        r = joy_api_call("joy_input_is_mouse_button_down", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_input_is_mouse_button_down", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static float icall_JoyInputGetMouseX(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_input_get_mouse_x", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_input_get_mouse_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JoyInputGetMouseY(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_input_get_mouse_y", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_input_get_mouse_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JoyInputGetWheel(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_input_get_wheel", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_input_get_wheel", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static int32_t icall_JoyInputWasKeyPressed(int32_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i32((int32_t)p0);
        r = joy_api_call("joy_input_was_key_pressed", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_input_was_key_pressed", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int32_t icall_JoyInputWasKeyReleased(int32_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i32((int32_t)p0);
        r = joy_api_call("joy_input_was_key_released", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_input_was_key_released", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.b;
}

static int64_t icall_JmathFpFromFloat(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        r = joy_api_call("joy_math_fp_from_float", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_from_float", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static float icall_JmathFpToFloat(int64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i64((int64_t)p0);
        r = joy_api_call("joy_math_fp_to_float", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_to_float", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static int64_t icall_JmathFpFromUint(uint32_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_u32((uint32_t)p0);
        r = joy_api_call("joy_math_fp_from_uint", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_from_uint", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathFpFromInt(int32_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i32((int32_t)p0);
        r = joy_api_call("joy_math_fp_from_int", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_from_int", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathFpFromFraction(uint32_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_u32((uint32_t)p0);
        r = joy_api_call("joy_math_fp_from_fraction", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_from_fraction", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathFpMinValue(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_math_fp_min_value", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_min_value", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathFpMaxValue(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_math_fp_max_value", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_max_value", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathFpAdd(int64_t p0, int64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        r = joy_api_call("joy_math_fp_add", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_add", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathFpSub(int64_t p0, int64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        r = joy_api_call("joy_math_fp_sub", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_sub", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathFpZero(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_math_fp_zero", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_zero", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathFpHalf(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_math_fp_half", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_half", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathFpOne(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_math_fp_one", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_one", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathFpPi(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_math_fp_pi", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_pi", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathFpMax(int64_t p0, int64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        r = joy_api_call("joy_math_fp_max", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_max", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathFpMin(int64_t p0, int64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        r = joy_api_call("joy_math_fp_min", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_min", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathFpPow2(int64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i64((int64_t)p0);
        r = joy_api_call("joy_math_fp_pow2", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_pow2", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int32_t icall_JmathFpSign(int64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i64((int64_t)p0);
        r = joy_api_call("joy_math_fp_sign", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_sign", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static int64_t icall_JmathFpClamp(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_fp_clamp", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_clamp", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathFpLerp(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_fp_lerp", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_fp_lerp", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fNegateX(int64_t p0, int64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        r = joy_api_call("joy_math_vec2f_negate_x", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_negate_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fNegateY(int64_t p0, int64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        r = joy_api_call("joy_math_vec2f_negate_y", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_negate_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fDot(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_vec2f_dot", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_dot", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fCross(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_vec2f_cross", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_cross", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fCross2X(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec2f_cross2_x", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_cross2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fCross2Y(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec2f_cross2_y", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_cross2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fCross3X(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec2f_cross3_x", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_cross3_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fCross3Y(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec2f_cross3_y", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_cross3_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fAddX(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_vec2f_add_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_add_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fAddY(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_vec2f_add_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_add_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fSubX(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_vec2f_sub_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_sub_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fSubY(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_vec2f_sub_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_sub_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fScaleX(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec2f_scale_x", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_scale_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fScaleY(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec2f_scale_y", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_scale_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fAbsX(int64_t p0, int64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        r = joy_api_call("joy_math_vec2f_abs_x", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_abs_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fAbsY(int64_t p0, int64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        r = joy_api_call("joy_math_vec2f_abs_y", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_abs_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fLengthSquared(int64_t p0, int64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        r = joy_api_call("joy_math_vec2f_length_squared", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_length_squared", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fLength(int64_t p0, int64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        r = joy_api_call("joy_math_vec2f_length", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_length", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fNormalizeX(int64_t p0, int64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        r = joy_api_call("joy_math_vec2f_normalize_x", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_normalize_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fNormalizeY(int64_t p0, int64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        r = joy_api_call("joy_math_vec2f_normalize_y", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_normalize_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fNormalX(int64_t p0, int64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        r = joy_api_call("joy_math_vec2f_normal_x", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_normal_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fNormalY(int64_t p0, int64_t p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        r = joy_api_call("joy_math_vec2f_normal_y", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_normal_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fRotateX(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec2f_rotate_x", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_rotate_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fRotateY(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec2f_rotate_y", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_rotate_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fLerpX(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[5];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        r = joy_api_call("joy_math_vec2f_lerp_x", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_lerp_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fLerpY(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[5];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        r = joy_api_call("joy_math_vec2f_lerp_y", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_lerp_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec2fDistance(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_vec2f_distance", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2f_distance", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fNegateX(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec3f_negate_x", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_negate_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fNegateY(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec3f_negate_y", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_negate_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fNegateZ(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec3f_negate_z", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_negate_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fAddX(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        r = joy_api_call("joy_math_vec3f_add_x", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_add_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fAddY(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        r = joy_api_call("joy_math_vec3f_add_y", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_add_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fAddZ(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        r = joy_api_call("joy_math_vec3f_add_z", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_add_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fSubX(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        r = joy_api_call("joy_math_vec3f_sub_x", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_sub_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fSubY(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        r = joy_api_call("joy_math_vec3f_sub_y", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_sub_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fSubZ(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        r = joy_api_call("joy_math_vec3f_sub_z", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_sub_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fScaleX(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_vec3f_scale_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_scale_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fScaleY(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_vec3f_scale_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_scale_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fScaleZ(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_vec3f_scale_z", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_scale_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fAbsX(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec3f_abs_x", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_abs_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fAbsY(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec3f_abs_y", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_abs_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fAbsZ(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec3f_abs_z", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_abs_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fDot(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        r = joy_api_call("joy_math_vec3f_dot", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_dot", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fCrossX(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        r = joy_api_call("joy_math_vec3f_cross_x", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_cross_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fCrossY(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        r = joy_api_call("joy_math_vec3f_cross_y", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_cross_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fCrossZ(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        r = joy_api_call("joy_math_vec3f_cross_z", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_cross_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fSquareLength(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec3f_square_length", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_square_length", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fLength(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec3f_length", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_length", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fNormalizeX(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec3f_normalize_x", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_normalize_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fNormalizeY(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec3f_normalize_y", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_normalize_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fNormalizeZ(int64_t p0, int64_t p1, int64_t p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        r = joy_api_call("joy_math_vec3f_normalize_z", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_normalize_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fLerpX(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5, int64_t p6)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[7];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        args[6] = joy_v_i64((int64_t)p6);
        r = joy_api_call("joy_math_vec3f_lerp_x", args, 7, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_lerp_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fLerpY(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5, int64_t p6)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[7];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        args[6] = joy_v_i64((int64_t)p6);
        r = joy_api_call("joy_math_vec3f_lerp_y", args, 7, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_lerp_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fLerpZ(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5, int64_t p6)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[7];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        args[6] = joy_v_i64((int64_t)p6);
        r = joy_api_call("joy_math_vec3f_lerp_z", args, 7, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_lerp_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathVec3fDistance(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        r = joy_api_call("joy_math_vec3f_distance", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3f_distance", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fMulVec2fX(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        r = joy_api_call("joy_math_mat22f_mul_vec2f_x", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_mul_vec2f_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fMulVec2fY(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        r = joy_api_call("joy_math_mat22f_mul_vec2f_y", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_mul_vec2f_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fRotateCol1X(int64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i64((int64_t)p0);
        r = joy_api_call("joy_math_mat22f_rotate_col1_x", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_rotate_col1_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fRotateCol1Y(int64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i64((int64_t)p0);
        r = joy_api_call("joy_math_mat22f_rotate_col1_y", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_rotate_col1_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fRotateCol2X(int64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i64((int64_t)p0);
        r = joy_api_call("joy_math_mat22f_rotate_col2_x", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_rotate_col2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fRotateCol2Y(int64_t p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_i64((int64_t)p0);
        r = joy_api_call("joy_math_mat22f_rotate_col2_y", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_rotate_col2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fTransposeCol1X(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_mat22f_transpose_col1_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_transpose_col1_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fTransposeCol1Y(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_mat22f_transpose_col1_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_transpose_col1_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fTransposeCol2X(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_mat22f_transpose_col2_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_transpose_col2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fTransposeCol2Y(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_mat22f_transpose_col2_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_transpose_col2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fInvertCol1X(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_mat22f_invert_col1_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_invert_col1_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fInvertCol1Y(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_mat22f_invert_col1_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_invert_col1_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fInvertCol2X(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_mat22f_invert_col2_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_invert_col2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fInvertCol2Y(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_mat22f_invert_col2_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_invert_col2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fAddCol1X(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5, int64_t p6, int64_t p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        args[6] = joy_v_i64((int64_t)p6);
        args[7] = joy_v_i64((int64_t)p7);
        r = joy_api_call("joy_math_mat22f_add_col1_x", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_add_col1_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fAddCol1Y(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5, int64_t p6, int64_t p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        args[6] = joy_v_i64((int64_t)p6);
        args[7] = joy_v_i64((int64_t)p7);
        r = joy_api_call("joy_math_mat22f_add_col1_y", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_add_col1_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fAddCol2X(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5, int64_t p6, int64_t p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        args[6] = joy_v_i64((int64_t)p6);
        args[7] = joy_v_i64((int64_t)p7);
        r = joy_api_call("joy_math_mat22f_add_col2_x", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_add_col2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fAddCol2Y(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5, int64_t p6, int64_t p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        args[6] = joy_v_i64((int64_t)p6);
        args[7] = joy_v_i64((int64_t)p7);
        r = joy_api_call("joy_math_mat22f_add_col2_y", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_add_col2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fMulCol1X(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5, int64_t p6, int64_t p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        args[6] = joy_v_i64((int64_t)p6);
        args[7] = joy_v_i64((int64_t)p7);
        r = joy_api_call("joy_math_mat22f_mul_col1_x", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_mul_col1_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fMulCol1Y(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5, int64_t p6, int64_t p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        args[6] = joy_v_i64((int64_t)p6);
        args[7] = joy_v_i64((int64_t)p7);
        r = joy_api_call("joy_math_mat22f_mul_col1_y", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_mul_col1_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fMulCol2X(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5, int64_t p6, int64_t p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        args[6] = joy_v_i64((int64_t)p6);
        args[7] = joy_v_i64((int64_t)p7);
        r = joy_api_call("joy_math_mat22f_mul_col2_x", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_mul_col2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fMulCol2Y(int64_t p0, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5, int64_t p6, int64_t p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        args[4] = joy_v_i64((int64_t)p4);
        args[5] = joy_v_i64((int64_t)p5);
        args[6] = joy_v_i64((int64_t)p6);
        args[7] = joy_v_i64((int64_t)p7);
        r = joy_api_call("joy_math_mat22f_mul_col2_y", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_mul_col2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fAbsCol1X(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_mat22f_abs_col1_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_abs_col1_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fAbsCol1Y(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_mat22f_abs_col1_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_abs_col1_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fAbsCol2X(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_mat22f_abs_col2_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_abs_col2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static int64_t icall_JmathMat22fAbsCol2Y(int64_t p0, int64_t p1, int64_t p2, int64_t p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_i64((int64_t)p0);
        args[1] = joy_v_i64((int64_t)p1);
        args[2] = joy_v_i64((int64_t)p2);
        args[3] = joy_v_i64((int64_t)p3);
        r = joy_api_call("joy_math_mat22f_abs_col2_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22f_abs_col2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int64_t)r.as.i64;
}

static float icall_JmathFtFromFloat(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        r = joy_api_call("joy_math_ft_from_float", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_from_float", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtToFloat(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        r = joy_api_call("joy_math_ft_to_float", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_to_float", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtMinValue(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_math_ft_min_value", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_min_value", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtMaxValue(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_math_ft_max_value", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_max_value", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtAdd(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_ft_add", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_add", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtSub(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_ft_sub", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_sub", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtMul(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_ft_mul", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_mul", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtDiv(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_ft_div", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_div", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtAbs(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        r = joy_api_call("joy_math_ft_abs", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_abs", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtSqrt(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        r = joy_api_call("joy_math_ft_sqrt", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_sqrt", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtSin(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        r = joy_api_call("joy_math_ft_sin", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_sin", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtCos(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        r = joy_api_call("joy_math_ft_cos", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_cos", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtAtan2(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_ft_atan2", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_atan2", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtZero(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_math_ft_zero", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_zero", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtHalf(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_math_ft_half", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_half", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtOne(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_math_ft_one", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_one", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtPi(void)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        r = joy_api_call("joy_math_ft_pi", NULL, 0, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_pi", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtMax(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_ft_max", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_max", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtMin(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_ft_min", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_min", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtPow2(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        r = joy_api_call("joy_math_ft_pow2", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_pow2", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static int32_t icall_JmathFtSign(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        r = joy_api_call("joy_math_ft_sign", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_sign", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (int32_t)r.as.i32;
}

static float icall_JmathFtClamp(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_ft_clamp", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_clamp", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtLerp(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_ft_lerp", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_lerp", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2NegateX(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_vec2_negate_x", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_negate_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2NegateY(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_vec2_negate_y", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_negate_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static uint64_t icall_JmathVec2Negate(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_vec2_negate", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_negate", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static float icall_JmathVec2Dot(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec2_dot", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_dot", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2Cross(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec2_cross", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_cross", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2Cross2X(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec2_cross2_x", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_cross2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2Cross2Y(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec2_cross2_y", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_cross2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static uint64_t icall_JmathVec2Cross2(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec2_cross2", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_cross2", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static float icall_JmathVec2Cross3X(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec2_cross3_x", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_cross3_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2Cross3Y(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec2_cross3_y", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_cross3_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static uint64_t icall_JmathVec2Cross3(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec2_cross3", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_cross3", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static float icall_JmathVec2AddX(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec2_add_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_add_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2AddY(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec2_add_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_add_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static uint64_t icall_JmathVec2Add(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec2_add", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_add", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static float icall_JmathVec2SubX(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec2_sub_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_sub_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2SubY(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec2_sub_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_sub_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static uint64_t icall_JmathVec2Sub(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec2_sub", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_sub", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static float icall_JmathVec2ScaleX(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec2_scale_x", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_scale_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2ScaleY(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec2_scale_y", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_scale_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static uint64_t icall_JmathVec2Scale(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec2_scale", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_scale", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static float icall_JmathVec2AbsX(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_vec2_abs_x", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_abs_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2AbsY(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_vec2_abs_y", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_abs_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static uint64_t icall_JmathVec2Abs(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_vec2_abs", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_abs", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static float icall_JmathVec2LengthSquared(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_vec2_length_squared", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_length_squared", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2Length(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_vec2_length", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_length", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2NormalizeX(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_vec2_normalize_x", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_normalize_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2NormalizeY(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_vec2_normalize_y", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_normalize_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static uint64_t icall_JmathVec2Normalize(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_vec2_normalize", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_normalize", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static float icall_JmathVec2NormalX(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_vec2_normal_x", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_normal_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2NormalY(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_vec2_normal_y", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_normal_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static uint64_t icall_JmathVec2Normal(float p0, float p1)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[2];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        r = joy_api_call("joy_math_vec2_normal", args, 2, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_normal", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static float icall_JmathVec2RotateX(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec2_rotate_x", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_rotate_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2RotateY(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec2_rotate_y", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_rotate_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static uint64_t icall_JmathVec2Rotate(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec2_rotate", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_rotate", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static float icall_JmathVec2LerpX(float p0, float p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[5];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        r = joy_api_call("joy_math_vec2_lerp_x", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_lerp_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec2LerpY(float p0, float p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[5];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        r = joy_api_call("joy_math_vec2_lerp_y", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_lerp_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static uint64_t icall_JmathVec2Lerp(float p0, float p1, float p2, float p3, float p4)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[5];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        r = joy_api_call("joy_math_vec2_lerp", args, 5, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_lerp", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static float icall_JmathVec2Distance(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec2_distance", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec2_distance", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3NegateX(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec3_negate_x", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_negate_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3NegateY(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec3_negate_y", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_negate_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3NegateZ(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec3_negate_z", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_negate_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3AddX(float p0, float p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_math_vec3_add_x", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_add_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3AddY(float p0, float p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_math_vec3_add_y", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_add_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3AddZ(float p0, float p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_math_vec3_add_z", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_add_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3SubX(float p0, float p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_math_vec3_sub_x", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_sub_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3SubY(float p0, float p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_math_vec3_sub_y", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_sub_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3SubZ(float p0, float p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_math_vec3_sub_z", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_sub_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3ScaleX(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec3_scale_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_scale_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3ScaleY(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec3_scale_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_scale_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3ScaleZ(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec3_scale_z", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_scale_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3AbsX(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec3_abs_x", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_abs_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3AbsY(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec3_abs_y", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_abs_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3AbsZ(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec3_abs_z", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_abs_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3Dot(float p0, float p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_math_vec3_dot", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_dot", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3CrossX(float p0, float p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_math_vec3_cross_x", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_cross_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3CrossY(float p0, float p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_math_vec3_cross_y", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_cross_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3CrossZ(float p0, float p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_math_vec3_cross_z", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_cross_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3SquareLength(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec3_square_length", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_square_length", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3Length(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec3_length", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_length", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3NormalizeX(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec3_normalize_x", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_normalize_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3NormalizeY(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec3_normalize_y", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_normalize_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3NormalizeZ(float p0, float p1, float p2)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[3];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        r = joy_api_call("joy_math_vec3_normalize_z", args, 3, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_normalize_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3LerpX(float p0, float p1, float p2, float p3, float p4, float p5, float p6)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[7];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        r = joy_api_call("joy_math_vec3_lerp_x", args, 7, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_lerp_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3LerpY(float p0, float p1, float p2, float p3, float p4, float p5, float p6)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[7];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        r = joy_api_call("joy_math_vec3_lerp_y", args, 7, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_lerp_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3LerpZ(float p0, float p1, float p2, float p3, float p4, float p5, float p6)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[7];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        r = joy_api_call("joy_math_vec3_lerp_z", args, 7, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_lerp_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3Distance(float p0, float p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_math_vec3_distance", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_distance", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22MulVec2X(float p0, float p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_math_mat22_mul_vec2_x", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_mul_vec2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22MulVec2Y(float p0, float p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_math_mat22_mul_vec2_y", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_mul_vec2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static uint64_t icall_JmathMat22MulVec2(float p0, float p1, float p2, float p3, float p4, float p5)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[6];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        r = joy_api_call("joy_math_mat22_mul_vec2", args, 6, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_mul_vec2", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (uint64_t)r.as.u64;
}

static float icall_JmathMat22RotateCol1X(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        r = joy_api_call("joy_math_mat22_rotate_col1_x", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_rotate_col1_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22RotateCol1Y(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        r = joy_api_call("joy_math_mat22_rotate_col1_y", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_rotate_col1_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22RotateCol2X(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        r = joy_api_call("joy_math_mat22_rotate_col2_x", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_rotate_col2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22RotateCol2Y(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        r = joy_api_call("joy_math_mat22_rotate_col2_y", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_rotate_col2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22TransposeCol1X(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_mat22_transpose_col1_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_transpose_col1_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22TransposeCol1Y(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_mat22_transpose_col1_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_transpose_col1_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22TransposeCol2X(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_mat22_transpose_col2_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_transpose_col2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22TransposeCol2Y(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_mat22_transpose_col2_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_transpose_col2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22InvertCol1X(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_mat22_invert_col1_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_invert_col1_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22InvertCol1Y(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_mat22_invert_col1_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_invert_col1_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22InvertCol2X(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_mat22_invert_col2_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_invert_col2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22InvertCol2Y(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_mat22_invert_col2_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_invert_col2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22AddCol1X(float p0, float p1, float p2, float p3, float p4, float p5, float p6, float p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        args[7] = joy_v_f64((double)p7);
        r = joy_api_call("joy_math_mat22_add_col1_x", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_add_col1_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22AddCol1Y(float p0, float p1, float p2, float p3, float p4, float p5, float p6, float p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        args[7] = joy_v_f64((double)p7);
        r = joy_api_call("joy_math_mat22_add_col1_y", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_add_col1_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22AddCol2X(float p0, float p1, float p2, float p3, float p4, float p5, float p6, float p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        args[7] = joy_v_f64((double)p7);
        r = joy_api_call("joy_math_mat22_add_col2_x", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_add_col2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22AddCol2Y(float p0, float p1, float p2, float p3, float p4, float p5, float p6, float p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        args[7] = joy_v_f64((double)p7);
        r = joy_api_call("joy_math_mat22_add_col2_y", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_add_col2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22MulCol1X(float p0, float p1, float p2, float p3, float p4, float p5, float p6, float p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        args[7] = joy_v_f64((double)p7);
        r = joy_api_call("joy_math_mat22_mul_col1_x", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_mul_col1_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22MulCol1Y(float p0, float p1, float p2, float p3, float p4, float p5, float p6, float p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        args[7] = joy_v_f64((double)p7);
        r = joy_api_call("joy_math_mat22_mul_col1_y", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_mul_col1_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22MulCol2X(float p0, float p1, float p2, float p3, float p4, float p5, float p6, float p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        args[7] = joy_v_f64((double)p7);
        r = joy_api_call("joy_math_mat22_mul_col2_x", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_mul_col2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22MulCol2Y(float p0, float p1, float p2, float p3, float p4, float p5, float p6, float p7)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[8];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        args[4] = joy_v_f64((double)p4);
        args[5] = joy_v_f64((double)p5);
        args[6] = joy_v_f64((double)p6);
        args[7] = joy_v_f64((double)p7);
        r = joy_api_call("joy_math_mat22_mul_col2_y", args, 8, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_mul_col2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22AbsCol1X(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_mat22_abs_col1_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_abs_col1_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22AbsCol1Y(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_mat22_abs_col1_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_abs_col1_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22AbsCol2X(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_mat22_abs_col2_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_abs_col2_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathMat22AbsCol2Y(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_mat22_abs_col2_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_mat22_abs_col2_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3DivX(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec3_div_x", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_div_x", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3DivY(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec3_div_y", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_div_y", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathVec3DivZ(float p0, float p1, float p2, float p3)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[4];
        args[0] = joy_v_f64((double)p0);
        args[1] = joy_v_f64((double)p1);
        args[2] = joy_v_f64((double)p2);
        args[3] = joy_v_f64((double)p3);
        r = joy_api_call("joy_math_vec3_div_z", args, 4, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_vec3_div_z", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

static float icall_JmathFtRadians(float p0)
{
        joy_status_t st = JOY_OK;
        joy_value_t r;
        joy_value_t args[1];
        args[0] = joy_v_f64((double)p0);
        r = joy_api_call("joy_math_ft_radians", args, 1, &st);
        if (st != JOY_OK) {
                fprintf(stderr, "[joy.api] %s failed: %s at %s\n", "joy_math_ft_radians", joy_status_name(st), joy_last_error_where());
                return 0;
        }
        return (float)r.as.f64;
}

/* 句柄对象被 GC 时由 C# finalizer 调这里；计数仅供自检。 */
static void icall_release(uint64_t h) { joy_obj_release((uint64_t)h); }
static uint32_t icall_live_count(void) { return joy_obj_live_count(); }
static uint32_t icall_destroy_count(void) { return joy_obj_destroy_count(); }

void csopen_api(void)
{
        csx_register_internal_call("Joy.Api.Native::LcgRandomCreate", (const void*)icall_LcgRandomCreate);
        csx_register_internal_call("Joy.Api.Native::LcgRandomDestroy", (const void*)icall_LcgRandomDestroy);
        csx_register_internal_call("Joy.Api.Native::LcgRandomNext", (const void*)icall_LcgRandomNext);
        csx_register_internal_call("Joy.Api.Native::LcgRandomSetState", (const void*)icall_LcgRandomSetState);
        csx_register_internal_call("Joy.Api.Native::JpmConfigAddSpawn", (const void*)icall_JpmConfigAddSpawn);
        csx_register_internal_call("Joy.Api.Native::JpmWaveSeed", (const void*)icall_JpmWaveSeed);
        csx_register_internal_call("Joy.Api.Native::SdbmHash", (const void*)icall_SdbmHash);
        csx_register_internal_call("Joy.Api.Native::Fnv1aHash", (const void*)icall_Fnv1aHash);
        csx_register_internal_call("Joy.Api.Native::JenkinsHash", (const void*)icall_JenkinsHash);
        csx_register_internal_call("Joy.Api.Native::Murmur3Hash", (const void*)icall_Murmur3Hash);
        csx_register_internal_call("Joy.Api.Native::KrHash", (const void*)icall_KrHash);
        csx_register_internal_call("Joy.Api.Native::JtimerDestroy", (const void*)icall_JtimerDestroy);
        csx_register_internal_call("Joy.Api.Native::JtimerTrigger", (const void*)icall_JtimerTrigger);
        csx_register_internal_call("Joy.Api.Native::JtimerSetInterval", (const void*)icall_JtimerSetInterval);
        csx_register_internal_call("Joy.Api.Native::JtimerSetActive", (const void*)icall_JtimerSetActive);
        csx_register_internal_call("Joy.Api.Native::JtimerReset", (const void*)icall_JtimerReset);
        csx_register_internal_call("Joy.Api.Native::JtimerGetElapsed", (const void*)icall_JtimerGetElapsed);
        csx_register_internal_call("Joy.Api.Native::InitEnv", (const void*)icall_InitEnv);
        csx_register_internal_call("Joy.Api.Native::QuitEnv", (const void*)icall_QuitEnv);
        csx_register_internal_call("Joy.Api.Native::GetTicks", (const void*)icall_GetTicks);
        csx_register_internal_call("Joy.Api.Native::CreateWindow", (const void*)icall_CreateWindow);
        csx_register_internal_call("Joy.Api.Native::DestroyWindow", (const void*)icall_DestroyWindow);
        csx_register_internal_call("Joy.Api.Native::WindowSetFullscreen", (const void*)icall_WindowSetFullscreen);
        csx_register_internal_call("Joy.Api.Native::WindowGetFullscreen", (const void*)icall_WindowGetFullscreen);
        csx_register_internal_call("Joy.Api.Native::WindowSetIcon", (const void*)icall_WindowSetIcon);
        csx_register_internal_call("Joy.Api.Native::PushUserEvent", (const void*)icall_PushUserEvent);
        csx_register_internal_call("Joy.Api.Native::PushQuitEvent", (const void*)icall_PushQuitEvent);
        csx_register_internal_call("Joy.Api.Native::LogTitle", (const void*)icall_LogTitle);
        csx_register_internal_call("Joy.Api.Native::EnvSet", (const void*)icall_EnvSet);
        csx_register_internal_call("Joy.Api.Native::EnvDestroy", (const void*)icall_EnvDestroy);
        csx_register_internal_call("Joy.Api.Native::SimpleFpsDestory", (const void*)icall_SimpleFpsDestory);
        csx_register_internal_call("Joy.Api.Native::SimpleFpsUpdate", (const void*)icall_SimpleFpsUpdate);
        csx_register_internal_call("Joy.Api.Native::SimpleFpsValue", (const void*)icall_SimpleFpsValue);
        csx_register_internal_call("Joy.Api.Native::GameTimerInit", (const void*)icall_GameTimerInit);
        csx_register_internal_call("Joy.Api.Native::GameTimerUpdate", (const void*)icall_GameTimerUpdate);
        csx_register_internal_call("Joy.Api.Native::GameTimerGetDeltaTime", (const void*)icall_GameTimerGetDeltaTime);
        csx_register_internal_call("Joy.Api.Native::GameTimerGetUnscaledDeltaTime", (const void*)icall_GameTimerGetUnscaledDeltaTime);
        csx_register_internal_call("Joy.Api.Native::GameTimerSetTimeScale", (const void*)icall_GameTimerSetTimeScale);
        csx_register_internal_call("Joy.Api.Native::GameTimerGetTimeScale", (const void*)icall_GameTimerGetTimeScale);
        csx_register_internal_call("Joy.Api.Native::GameTimerSetTargetFps", (const void*)icall_GameTimerSetTargetFps);
        csx_register_internal_call("Joy.Api.Native::GameTimerGetTargetFps", (const void*)icall_GameTimerGetTargetFps);
        csx_register_internal_call("Joy.Api.Native::SetClipboardText", (const void*)icall_SetClipboardText);
        csx_register_internal_call("Joy.Api.Native::SysInitNetenv", (const void*)icall_SysInitNetenv);
        csx_register_internal_call("Joy.Api.Native::SysReleaseNetenv", (const void*)icall_SysReleaseNetenv);
        csx_register_internal_call("Joy.Api.Native::SysAnyaddr", (const void*)icall_SysAnyaddr);
        csx_register_internal_call("Joy.Api.Native::SysClosesocket", (const void*)icall_SysClosesocket);
        csx_register_internal_call("Joy.Api.Native::SysSetSockSndtimeo", (const void*)icall_SysSetSockSndtimeo);
        csx_register_internal_call("Joy.Api.Native::SysSetSockRcvtimeo", (const void*)icall_SysSetSockRcvtimeo);
        csx_register_internal_call("Joy.Api.Native::SysSetSockAccpettimeo", (const void*)icall_SysSetSockAccpettimeo);
        csx_register_internal_call("Joy.Api.Native::SysSetSockBroadcast", (const void*)icall_SysSetSockBroadcast);
        csx_register_internal_call("Joy.Api.Native::SysSetSockReuseaddr", (const void*)icall_SysSetSockReuseaddr);
        csx_register_internal_call("Joy.Api.Native::SysTcp", (const void*)icall_SysTcp);
        csx_register_internal_call("Joy.Api.Native::SysUdp", (const void*)icall_SysUdp);
        csx_register_internal_call("Joy.Api.Native::SysBind", (const void*)icall_SysBind);
        csx_register_internal_call("Joy.Api.Native::SysConnect", (const void*)icall_SysConnect);
        csx_register_internal_call("Joy.Api.Native::SysListen", (const void*)icall_SysListen);
        csx_register_internal_call("Joy.Api.Native::SysSend", (const void*)icall_SysSend);
        csx_register_internal_call("Joy.Api.Native::SysSendto", (const void*)icall_SysSendto);
        csx_register_internal_call("Joy.Api.Native::SysCurrentTime", (const void*)icall_SysCurrentTime);
        csx_register_internal_call("Joy.Api.Native::SysSleep", (const void*)icall_SysSleep);
        csx_register_internal_call("Joy.Api.Native::SysInstallCrashHandlers", (const void*)icall_SysInstallCrashHandlers);
        csx_register_internal_call("Joy.Api.Native::IocpServerDestroy", (const void*)icall_IocpServerDestroy);
        csx_register_internal_call("Joy.Api.Native::IocpProcessEvents", (const void*)icall_IocpProcessEvents);
        csx_register_internal_call("Joy.Api.Native::FontCreate", (const void*)icall_FontCreate);
        csx_register_internal_call("Joy.Api.Native::FontDestroy", (const void*)icall_FontDestroy);
        csx_register_internal_call("Joy.Api.Native::FontGetScale", (const void*)icall_FontGetScale);
        csx_register_internal_call("Joy.Api.Native::FontGetSize", (const void*)icall_FontGetSize);
        csx_register_internal_call("Joy.Api.Native::FontGetAscent", (const void*)icall_FontGetAscent);
        csx_register_internal_call("Joy.Api.Native::R2dCreate", (const void*)icall_R2dCreate);
        csx_register_internal_call("Joy.Api.Native::R2dDestroy", (const void*)icall_R2dDestroy);
        csx_register_internal_call("Joy.Api.Native::R2dSetColor", (const void*)icall_R2dSetColor);
        csx_register_internal_call("Joy.Api.Native::R2dBegin", (const void*)icall_R2dBegin);
        csx_register_internal_call("Joy.Api.Native::R2dEnd", (const void*)icall_R2dEnd);
        csx_register_internal_call("Joy.Api.Native::R2dSetLogicalPresentation", (const void*)icall_R2dSetLogicalPresentation);
        csx_register_internal_call("Joy.Api.Native::R2dSetLogicalPresentationMode", (const void*)icall_R2dSetLogicalPresentationMode);
        csx_register_internal_call("Joy.Api.Native::R2dSetVsync", (const void*)icall_R2dSetVsync);
        csx_register_internal_call("Joy.Api.Native::R2dSetScale", (const void*)icall_R2dSetScale);
        csx_register_internal_call("Joy.Api.Native::R2dSetColorf", (const void*)icall_R2dSetColorf);
        csx_register_internal_call("Joy.Api.Native::R2dSetBlendMode", (const void*)icall_R2dSetBlendMode);
        csx_register_internal_call("Joy.Api.Native::R2dDrawPoint", (const void*)icall_R2dDrawPoint);
        csx_register_internal_call("Joy.Api.Native::R2dSetClipRect", (const void*)icall_R2dSetClipRect);
        csx_register_internal_call("Joy.Api.Native::R2dClearClip", (const void*)icall_R2dClearClip);
        csx_register_internal_call("Joy.Api.Native::R2dPushClipRect", (const void*)icall_R2dPushClipRect);
        csx_register_internal_call("Joy.Api.Native::R2dPopClipRect", (const void*)icall_R2dPopClipRect);
        csx_register_internal_call("Joy.Api.Native::R2dResetClipStack", (const void*)icall_R2dResetClipStack);
        csx_register_internal_call("Joy.Api.Native::R2dPointInClip", (const void*)icall_R2dPointInClip);
        csx_register_internal_call("Joy.Api.Native::ShapeDrawLine", (const void*)icall_ShapeDrawLine);
        csx_register_internal_call("Joy.Api.Native::ShapeDrawRectangle", (const void*)icall_ShapeDrawRectangle);
        csx_register_internal_call("Joy.Api.Native::ImageCreate", (const void*)icall_ImageCreate);
        csx_register_internal_call("Joy.Api.Native::ImageDestroy", (const void*)icall_ImageDestroy);
        csx_register_internal_call("Joy.Api.Native::ImageSetAlpha", (const void*)icall_ImageSetAlpha);
        csx_register_internal_call("Joy.Api.Native::ImageSetBlendMode", (const void*)icall_ImageSetBlendMode);
        csx_register_internal_call("Joy.Api.Native::ImageSetScaleMode", (const void*)icall_ImageSetScaleMode);
        csx_register_internal_call("Joy.Api.Native::TextUpdatex", (const void*)icall_TextUpdatex);
        csx_register_internal_call("Joy.Api.Native::TextDestroy", (const void*)icall_TextDestroy);
        csx_register_internal_call("Joy.Api.Native::JtextPrint", (const void*)icall_JtextPrint);
        csx_register_internal_call("Joy.Api.Native::JtextPrintx", (const void*)icall_JtextPrintx);
        csx_register_internal_call("Joy.Api.Native::R2dDrawPointF", (const void*)icall_R2dDrawPointF);
        csx_register_internal_call("Joy.Api.Native::ShapeDrawRectangleF", (const void*)icall_ShapeDrawRectangleF);
        csx_register_internal_call("Joy.Api.Native::ShapeDrawLineF", (const void*)icall_ShapeDrawLineF);
        csx_register_internal_call("Joy.Api.Native::JtextPrintf", (const void*)icall_JtextPrintf);
        csx_register_internal_call("Joy.Api.Native::JtextPrintfScaled", (const void*)icall_JtextPrintfScaled);
        csx_register_internal_call("Joy.Api.Native::TextGetWidth", (const void*)icall_TextGetWidth);
        csx_register_internal_call("Joy.Api.Native::TextGetHeight", (const void*)icall_TextGetHeight);
        csx_register_internal_call("Joy.Api.Native::JtextCacheClear", (const void*)icall_JtextCacheClear);
        csx_register_internal_call("Joy.Api.Native::JtextCacheSetLimit", (const void*)icall_JtextCacheSetLimit);
        csx_register_internal_call("Joy.Api.Native::JtextCacheGetCount", (const void*)icall_JtextCacheGetCount);
        csx_register_internal_call("Joy.Api.Native::JtextCacheSweep", (const void*)icall_JtextCacheSweep);
        csx_register_internal_call("Joy.Api.Native::AnimationDestroy", (const void*)icall_AnimationDestroy);
        csx_register_internal_call("Joy.Api.Native::AnimationReset", (const void*)icall_AnimationReset);
        csx_register_internal_call("Joy.Api.Native::AnimationAddClip", (const void*)icall_AnimationAddClip);
        csx_register_internal_call("Joy.Api.Native::AnimationIsFinished", (const void*)icall_AnimationIsFinished);
        csx_register_internal_call("Joy.Api.Native::AnimationSetPosition", (const void*)icall_AnimationSetPosition);
        csx_register_internal_call("Joy.Api.Native::AnimationSetScale", (const void*)icall_AnimationSetScale);
        csx_register_internal_call("Joy.Api.Native::AnimationSetRotation", (const void*)icall_AnimationSetRotation);
        csx_register_internal_call("Joy.Api.Native::AnimationUpdate", (const void*)icall_AnimationUpdate);
        csx_register_internal_call("Joy.Api.Native::R3dDestroy", (const void*)icall_R3dDestroy);
        csx_register_internal_call("Joy.Api.Native::R3dBegin", (const void*)icall_R3dBegin);
        csx_register_internal_call("Joy.Api.Native::R3dEnd", (const void*)icall_R3dEnd);
        csx_register_internal_call("Joy.Api.Native::R3dGetDrawCalls", (const void*)icall_R3dGetDrawCalls);
        csx_register_internal_call("Joy.Api.Native::R3dSetClearColor", (const void*)icall_R3dSetClearColor);
        csx_register_internal_call("Joy.Api.Native::R3dMeshCube", (const void*)icall_R3dMeshCube);
        csx_register_internal_call("Joy.Api.Native::R3dMeshSphere", (const void*)icall_R3dMeshSphere);
        csx_register_internal_call("Joy.Api.Native::R3dMeshPlane", (const void*)icall_R3dMeshPlane);
        csx_register_internal_call("Joy.Api.Native::R3dMeshRamp", (const void*)icall_R3dMeshRamp);
        csx_register_internal_call("Joy.Api.Native::R3dMeshQuad", (const void*)icall_R3dMeshQuad);
        csx_register_internal_call("Joy.Api.Native::R3dMeshCubeWire", (const void*)icall_R3dMeshCubeWire);
        csx_register_internal_call("Joy.Api.Native::R3dMeshSphereWire", (const void*)icall_R3dMeshSphereWire);
        csx_register_internal_call("Joy.Api.Native::R3dMeshPlaneWire", (const void*)icall_R3dMeshPlaneWire);
        csx_register_internal_call("Joy.Api.Native::R3dMeshRampWire", (const void*)icall_R3dMeshRampWire);
        csx_register_internal_call("Joy.Api.Native::R3dMeshDestroy", (const void*)icall_R3dMeshDestroy);
        csx_register_internal_call("Joy.Api.Native::R3dFontInit", (const void*)icall_R3dFontInit);
        csx_register_internal_call("Joy.Api.Native::R3dFontSetColor", (const void*)icall_R3dFontSetColor);
        csx_register_internal_call("Joy.Api.Native::R3dFontDraw2d", (const void*)icall_R3dFontDraw2d);
        csx_register_internal_call("Joy.Api.Native::R3dFontDraw2dColor", (const void*)icall_R3dFontDraw2dColor);
        csx_register_internal_call("Joy.Api.Native::R3dFontDraw2dOutline", (const void*)icall_R3dFontDraw2dOutline);
        csx_register_internal_call("Joy.Api.Native::R3dFontCacheSetLimit", (const void*)icall_R3dFontCacheSetLimit);
        csx_register_internal_call("Joy.Api.Native::R3dFontCacheGetCount", (const void*)icall_R3dFontCacheGetCount);
        csx_register_internal_call("Joy.Api.Native::R3dFontCacheClear", (const void*)icall_R3dFontCacheClear);
        csx_register_internal_call("Joy.Api.Native::R3dFontCacheSweep", (const void*)icall_R3dFontCacheSweep);
        csx_register_internal_call("Joy.Api.Native::R3dSetLogicalPresentation", (const void*)icall_R3dSetLogicalPresentation);
        csx_register_internal_call("Joy.Api.Native::R3dSetLogicalPresentationMode", (const void*)icall_R3dSetLogicalPresentationMode);
        csx_register_internal_call("Joy.Api.Native::R3dButtonDraw", (const void*)icall_R3dButtonDraw);
        csx_register_internal_call("Joy.Api.Native::R3dButtonDrawCircle", (const void*)icall_R3dButtonDrawCircle);
        csx_register_internal_call("Joy.Api.Native::R3dButtonIsHovered", (const void*)icall_R3dButtonIsHovered);
        csx_register_internal_call("Joy.Api.Native::R3dButtonIsPressed", (const void*)icall_R3dButtonIsPressed);
        csx_register_internal_call("Joy.Api.Native::R3dButtonRelease", (const void*)icall_R3dButtonRelease);
        csx_register_internal_call("Joy.Api.Native::R3dButtonDestroy", (const void*)icall_R3dButtonDestroy);
        csx_register_internal_call("Joy.Api.Native::R3dDrawRect2d", (const void*)icall_R3dDrawRect2d);
        csx_register_internal_call("Joy.Api.Native::R3dDrawMesh2d", (const void*)icall_R3dDrawMesh2d);
        csx_register_internal_call("Joy.Api.Native::R3dLabelSetText", (const void*)icall_R3dLabelSetText);
        csx_register_internal_call("Joy.Api.Native::R3dLabelDraw", (const void*)icall_R3dLabelDraw);
        csx_register_internal_call("Joy.Api.Native::R3dLabelDestroy", (const void*)icall_R3dLabelDestroy);
        csx_register_internal_call("Joy.Api.Native::R3dInputSetText", (const void*)icall_R3dInputSetText);
        csx_register_internal_call("Joy.Api.Native::R3dInputIsHovered", (const void*)icall_R3dInputIsHovered);
        csx_register_internal_call("Joy.Api.Native::R3dInputFocus", (const void*)icall_R3dInputFocus);
        csx_register_internal_call("Joy.Api.Native::R3dInputDefocus", (const void*)icall_R3dInputDefocus);
        csx_register_internal_call("Joy.Api.Native::R3dInputIsFocused", (const void*)icall_R3dInputIsFocused);
        csx_register_internal_call("Joy.Api.Native::R3dInputStartTextInput", (const void*)icall_R3dInputStartTextInput);
        csx_register_internal_call("Joy.Api.Native::R3dInputStopTextInput", (const void*)icall_R3dInputStopTextInput);
        csx_register_internal_call("Joy.Api.Native::R3dInputInsert", (const void*)icall_R3dInputInsert);
        csx_register_internal_call("Joy.Api.Native::R3dInputBackspace", (const void*)icall_R3dInputBackspace);
        csx_register_internal_call("Joy.Api.Native::R3dInputDelete", (const void*)icall_R3dInputDelete);
        csx_register_internal_call("Joy.Api.Native::R3dInputCursorLeft", (const void*)icall_R3dInputCursorLeft);
        csx_register_internal_call("Joy.Api.Native::R3dInputCursorRight", (const void*)icall_R3dInputCursorRight);
        csx_register_internal_call("Joy.Api.Native::R3dInputCursorHome", (const void*)icall_R3dInputCursorHome);
        csx_register_internal_call("Joy.Api.Native::R3dInputCursorEnd", (const void*)icall_R3dInputCursorEnd);
        csx_register_internal_call("Joy.Api.Native::R3dInputClick", (const void*)icall_R3dInputClick);
        csx_register_internal_call("Joy.Api.Native::R3dInputSubmit", (const void*)icall_R3dInputSubmit);
        csx_register_internal_call("Joy.Api.Native::R3dInputDraw", (const void*)icall_R3dInputDraw);
        csx_register_internal_call("Joy.Api.Native::R3dInputDestroy", (const void*)icall_R3dInputDestroy);
        csx_register_internal_call("Joy.Api.Native::R3dComboboxInputDestroy", (const void*)icall_R3dComboboxInputDestroy);
        csx_register_internal_call("Joy.Api.Native::R3dComboboxInputAddItem", (const void*)icall_R3dComboboxInputAddItem);
        csx_register_internal_call("Joy.Api.Native::R3dComboboxInputClearItems", (const void*)icall_R3dComboboxInputClearItems);
        csx_register_internal_call("Joy.Api.Native::R3dComboboxInputSetText", (const void*)icall_R3dComboboxInputSetText);
        csx_register_internal_call("Joy.Api.Native::R3dComboboxInputHitItem", (const void*)icall_R3dComboboxInputHitItem);
        csx_register_internal_call("Joy.Api.Native::R3dComboboxInputDraw", (const void*)icall_R3dComboboxInputDraw);
        csx_register_internal_call("Joy.Api.Native::R3dComboboxInputDrawDropdown", (const void*)icall_R3dComboboxInputDrawDropdown);
        csx_register_internal_call("Joy.Api.Native::R3dSliderDestroy", (const void*)icall_R3dSliderDestroy);
        csx_register_internal_call("Joy.Api.Native::R3dSliderSetValue", (const void*)icall_R3dSliderSetValue);
        csx_register_internal_call("Joy.Api.Native::R3dSliderGetValue", (const void*)icall_R3dSliderGetValue);
        csx_register_internal_call("Joy.Api.Native::R3dSliderDraw", (const void*)icall_R3dSliderDraw);
        csx_register_internal_call("Joy.Api.Native::R3dSliderIsPressed", (const void*)icall_R3dSliderIsPressed);
        csx_register_internal_call("Joy.Api.Native::R3dSliderSetPressed", (const void*)icall_R3dSliderSetPressed);
        csx_register_internal_call("Joy.Api.Native::R3dSliderDragMove", (const void*)icall_R3dSliderDragMove);
        csx_register_internal_call("Joy.Api.Native::R3dSliderIsHovered", (const void*)icall_R3dSliderIsHovered);
        csx_register_internal_call("Joy.Api.Native::R3dSliderSetHovered", (const void*)icall_R3dSliderSetHovered);
        csx_register_internal_call("Joy.Api.Native::R3dCheckboxDestroy", (const void*)icall_R3dCheckboxDestroy);
        csx_register_internal_call("Joy.Api.Native::R3dCheckboxToggle", (const void*)icall_R3dCheckboxToggle);
        csx_register_internal_call("Joy.Api.Native::R3dCheckboxIsPressed", (const void*)icall_R3dCheckboxIsPressed);
        csx_register_internal_call("Joy.Api.Native::R3dCheckboxSetHovered", (const void*)icall_R3dCheckboxSetHovered);
        csx_register_internal_call("Joy.Api.Native::R3dCheckboxIsHovered", (const void*)icall_R3dCheckboxIsHovered);
        csx_register_internal_call("Joy.Api.Native::R3dCheckboxSetColors", (const void*)icall_R3dCheckboxSetColors);
        csx_register_internal_call("Joy.Api.Native::R3dCheckboxDraw", (const void*)icall_R3dCheckboxDraw);
        csx_register_internal_call("Joy.Api.Native::R3dDrawGrid", (const void*)icall_R3dDrawGrid);
        csx_register_internal_call("Joy.Api.Native::R3dMeshCubeLit", (const void*)icall_R3dMeshCubeLit);
        csx_register_internal_call("Joy.Api.Native::R3dMeshSphereLit", (const void*)icall_R3dMeshSphereLit);
        csx_register_internal_call("Joy.Api.Native::R3dMeshPlaneLit", (const void*)icall_R3dMeshPlaneLit);
        csx_register_internal_call("Joy.Api.Native::R3dMeshRampLit", (const void*)icall_R3dMeshRampLit);
        csx_register_internal_call("Joy.Api.Native::JanimatorDestroy", (const void*)icall_JanimatorDestroy);
        csx_register_internal_call("Joy.Api.Native::JanimatorSetFloat", (const void*)icall_JanimatorSetFloat);
        csx_register_internal_call("Joy.Api.Native::JanimatorSetInt", (const void*)icall_JanimatorSetInt);
        csx_register_internal_call("Joy.Api.Native::JanimatorSetBool", (const void*)icall_JanimatorSetBool);
        csx_register_internal_call("Joy.Api.Native::JanimatorSetTrigger", (const void*)icall_JanimatorSetTrigger);
        csx_register_internal_call("Joy.Api.Native::JanimatorGetFloat", (const void*)icall_JanimatorGetFloat);
        csx_register_internal_call("Joy.Api.Native::JanimatorGetInt", (const void*)icall_JanimatorGetInt);
        csx_register_internal_call("Joy.Api.Native::JanimatorGetBool", (const void*)icall_JanimatorGetBool);
        csx_register_internal_call("Joy.Api.Native::JanimatorAddState", (const void*)icall_JanimatorAddState);
        csx_register_internal_call("Joy.Api.Native::JanimatorSetDefaultState", (const void*)icall_JanimatorSetDefaultState);
        csx_register_internal_call("Joy.Api.Native::JanimatorStateAddClip", (const void*)icall_JanimatorStateAddClip);
        csx_register_internal_call("Joy.Api.Native::JanimatorStateAddColor", (const void*)icall_JanimatorStateAddColor);
        csx_register_internal_call("Joy.Api.Native::JanimatorStateSetLoop", (const void*)icall_JanimatorStateSetLoop);
        csx_register_internal_call("Joy.Api.Native::JanimatorStateSetSpeed", (const void*)icall_JanimatorStateSetSpeed);
        csx_register_internal_call("Joy.Api.Native::JanimatorStateSetBlend", (const void*)icall_JanimatorStateSetBlend);
        csx_register_internal_call("Joy.Api.Native::JanimatorStateAddMotion", (const void*)icall_JanimatorStateAddMotion);
        csx_register_internal_call("Joy.Api.Native::JanimatorAddTransition", (const void*)icall_JanimatorAddTransition);
        csx_register_internal_call("Joy.Api.Native::JanimatorAddAnyTransition", (const void*)icall_JanimatorAddAnyTransition);
        csx_register_internal_call("Joy.Api.Native::JanimatorTransitionAddCond", (const void*)icall_JanimatorTransitionAddCond);
        csx_register_internal_call("Joy.Api.Native::JanimatorTransitionSetExitTime", (const void*)icall_JanimatorTransitionSetExitTime);
        csx_register_internal_call("Joy.Api.Native::JanimatorUpdate", (const void*)icall_JanimatorUpdate);
        csx_register_internal_call("Joy.Api.Native::JanimatorDraw", (const void*)icall_JanimatorDraw);
        csx_register_internal_call("Joy.Api.Native::JanimatorSetFlipH", (const void*)icall_JanimatorSetFlipH);
        csx_register_internal_call("Joy.Api.Native::LabelDestroy", (const void*)icall_LabelDestroy);
        csx_register_internal_call("Joy.Api.Native::LabelDraw", (const void*)icall_LabelDraw);
        csx_register_internal_call("Joy.Api.Native::ButtonDestroy", (const void*)icall_ButtonDestroy);
        csx_register_internal_call("Joy.Api.Native::ButtonSetPosition", (const void*)icall_ButtonSetPosition);
        csx_register_internal_call("Joy.Api.Native::ButtonSetSize", (const void*)icall_ButtonSetSize);
        csx_register_internal_call("Joy.Api.Native::ButtonSetNormalColor", (const void*)icall_ButtonSetNormalColor);
        csx_register_internal_call("Joy.Api.Native::ButtonSetHoverColor", (const void*)icall_ButtonSetHoverColor);
        csx_register_internal_call("Joy.Api.Native::ButtonSetPressedColor", (const void*)icall_ButtonSetPressedColor);
        csx_register_internal_call("Joy.Api.Native::ButtonSetImage", (const void*)icall_ButtonSetImage);
        csx_register_internal_call("Joy.Api.Native::ButtonSetPressedImage", (const void*)icall_ButtonSetPressedImage);
        csx_register_internal_call("Joy.Api.Native::ButtonSetTextx", (const void*)icall_ButtonSetTextx);
        csx_register_internal_call("Joy.Api.Native::ButtonIsHovered", (const void*)icall_ButtonIsHovered);
        csx_register_internal_call("Joy.Api.Native::ButtonIsPressed", (const void*)icall_ButtonIsPressed);
        csx_register_internal_call("Joy.Api.Native::ButtonSetHovered", (const void*)icall_ButtonSetHovered);
        csx_register_internal_call("Joy.Api.Native::ButtonSetPressed", (const void*)icall_ButtonSetPressed);
        csx_register_internal_call("Joy.Api.Native::ButtonGetPressed", (const void*)icall_ButtonGetPressed);
        csx_register_internal_call("Joy.Api.Native::ButtonSetTextOffset", (const void*)icall_ButtonSetTextOffset);
        csx_register_internal_call("Joy.Api.Native::ButtonDraw", (const void*)icall_ButtonDraw);
        csx_register_internal_call("Joy.Api.Native::ScrollviewDestroy", (const void*)icall_ScrollviewDestroy);
        csx_register_internal_call("Joy.Api.Native::ScrollviewSetViewPx", (const void*)icall_ScrollviewSetViewPx);
        csx_register_internal_call("Joy.Api.Native::ScrollviewSetContent", (const void*)icall_ScrollviewSetContent);
        csx_register_internal_call("Joy.Api.Native::ScrollviewSetOffset", (const void*)icall_ScrollviewSetOffset);
        csx_register_internal_call("Joy.Api.Native::ScrollviewClamp", (const void*)icall_ScrollviewClamp);
        csx_register_internal_call("Joy.Api.Native::ScrollviewGetOffsetX", (const void*)icall_ScrollviewGetOffsetX);
        csx_register_internal_call("Joy.Api.Native::ScrollviewGetOffsetY", (const void*)icall_ScrollviewGetOffsetY);
        csx_register_internal_call("Joy.Api.Native::ScrollviewSetShowBar", (const void*)icall_ScrollviewSetShowBar);
        csx_register_internal_call("Joy.Api.Native::ScrollviewDragging", (const void*)icall_ScrollviewDragging);
        csx_register_internal_call("Joy.Api.Native::ScrollviewHit", (const void*)icall_ScrollviewHit);
        csx_register_internal_call("Joy.Api.Native::ScrollviewBegin", (const void*)icall_ScrollviewBegin);
        csx_register_internal_call("Joy.Api.Native::ScrollviewEnd", (const void*)icall_ScrollviewEnd);
        csx_register_internal_call("Joy.Api.Native::ComboboxDestroy", (const void*)icall_ComboboxDestroy);
        csx_register_internal_call("Joy.Api.Native::ComboboxAddItem", (const void*)icall_ComboboxAddItem);
        csx_register_internal_call("Joy.Api.Native::ComboboxIsPressed", (const void*)icall_ComboboxIsPressed);
        csx_register_internal_call("Joy.Api.Native::ComboboxIsOpen", (const void*)icall_ComboboxIsOpen);
        csx_register_internal_call("Joy.Api.Native::ComboboxSetOpen", (const void*)icall_ComboboxSetOpen);
        csx_register_internal_call("Joy.Api.Native::ComboboxHitItem", (const void*)icall_ComboboxHitItem);
        csx_register_internal_call("Joy.Api.Native::ComboboxSetSelected", (const void*)icall_ComboboxSetSelected);
        csx_register_internal_call("Joy.Api.Native::ComboboxGetSelected", (const void*)icall_ComboboxGetSelected);
        csx_register_internal_call("Joy.Api.Native::ComboboxDraw", (const void*)icall_ComboboxDraw);
        csx_register_internal_call("Joy.Api.Native::ComboboxInputDestroy", (const void*)icall_ComboboxInputDestroy);
        csx_register_internal_call("Joy.Api.Native::ComboboxInputAddItem", (const void*)icall_ComboboxInputAddItem);
        csx_register_internal_call("Joy.Api.Native::ComboboxInputClearItems", (const void*)icall_ComboboxInputClearItems);
        csx_register_internal_call("Joy.Api.Native::ComboboxInputSetText", (const void*)icall_ComboboxInputSetText);
        csx_register_internal_call("Joy.Api.Native::ComboboxInputDraw", (const void*)icall_ComboboxInputDraw);
        csx_register_internal_call("Joy.Api.Native::ComboboxInputDrawDropdown", (const void*)icall_ComboboxInputDrawDropdown);
        csx_register_internal_call("Joy.Api.Native::ComboboxInputIsOpen", (const void*)icall_ComboboxInputIsOpen);
        csx_register_internal_call("Joy.Api.Native::ComboboxInputSetPopupNode", (const void*)icall_ComboboxInputSetPopupNode);
        csx_register_internal_call("Joy.Api.Native::ComboboxInputHitItem", (const void*)icall_ComboboxInputHitItem);
        csx_register_internal_call("Joy.Api.Native::ComboboxInputSetOpen", (const void*)icall_ComboboxInputSetOpen);
        csx_register_internal_call("Joy.Api.Native::DatagridSetheaders", (const void*)icall_DatagridSetheaders);
        csx_register_internal_call("Joy.Api.Native::DatagridSetheader", (const void*)icall_DatagridSetheader);
        csx_register_internal_call("Joy.Api.Native::DatagridSetcell", (const void*)icall_DatagridSetcell);
        csx_register_internal_call("Joy.Api.Native::DatagridDestroy", (const void*)icall_DatagridDestroy);
        csx_register_internal_call("Joy.Api.Native::DatagridDraw", (const void*)icall_DatagridDraw);
        csx_register_internal_call("Joy.Api.Native::DatagridHitRow", (const void*)icall_DatagridHitRow);
        csx_register_internal_call("Joy.Api.Native::DatagridScroll", (const void*)icall_DatagridScroll);
        csx_register_internal_call("Joy.Api.Native::DatagridSetSelectedRow", (const void*)icall_DatagridSetSelectedRow);
        csx_register_internal_call("Joy.Api.Native::RadioDestroy", (const void*)icall_RadioDestroy);
        csx_register_internal_call("Joy.Api.Native::RadioIsPressed", (const void*)icall_RadioIsPressed);
        csx_register_internal_call("Joy.Api.Native::RadioSetHovered", (const void*)icall_RadioSetHovered);
        csx_register_internal_call("Joy.Api.Native::CheckboxDestroy", (const void*)icall_CheckboxDestroy);
        csx_register_internal_call("Joy.Api.Native::CheckboxSetColors", (const void*)icall_CheckboxSetColors);
        csx_register_internal_call("Joy.Api.Native::CheckboxSetR2d", (const void*)icall_CheckboxSetR2d);
        csx_register_internal_call("Joy.Api.Native::CheckboxSetPosition", (const void*)icall_CheckboxSetPosition);
        csx_register_internal_call("Joy.Api.Native::CheckboxSetTextColor", (const void*)icall_CheckboxSetTextColor);
        csx_register_internal_call("Joy.Api.Native::CheckboxToggle", (const void*)icall_CheckboxToggle);
        csx_register_internal_call("Joy.Api.Native::CheckboxIsPressed", (const void*)icall_CheckboxIsPressed);
        csx_register_internal_call("Joy.Api.Native::CheckboxSetHovered", (const void*)icall_CheckboxSetHovered);
        csx_register_internal_call("Joy.Api.Native::JoystickDestroy", (const void*)icall_JoystickDestroy);
        csx_register_internal_call("Joy.Api.Native::JoystickSetTextures", (const void*)icall_JoystickSetTextures);
        csx_register_internal_call("Joy.Api.Native::JoystickSetVector", (const void*)icall_JoystickSetVector);
        csx_register_internal_call("Joy.Api.Native::JoystickDraw", (const void*)icall_JoystickDraw);
        csx_register_internal_call("Joy.Api.Native::JoystickReset", (const void*)icall_JoystickReset);
        csx_register_internal_call("Joy.Api.Native::JoystickSetPosition", (const void*)icall_JoystickSetPosition);
        csx_register_internal_call("Joy.Api.Native::JoystickSetRadius", (const void*)icall_JoystickSetRadius);
        csx_register_internal_call("Joy.Api.Native::JoystickIsDragging", (const void*)icall_JoystickIsDragging);
        csx_register_internal_call("Joy.Api.Native::JoystickTouchDown", (const void*)icall_JoystickTouchDown);
        csx_register_internal_call("Joy.Api.Native::JoystickTouchMove", (const void*)icall_JoystickTouchMove);
        csx_register_internal_call("Joy.Api.Native::JoystickTouchUp", (const void*)icall_JoystickTouchUp);
        csx_register_internal_call("Joy.Api.Native::JoystickGetMagnitude", (const void*)icall_JoystickGetMagnitude);
        csx_register_internal_call("Joy.Api.Native::InputboxDestroy", (const void*)icall_InputboxDestroy);
        csx_register_internal_call("Joy.Api.Native::InputboxSetText", (const void*)icall_InputboxSetText);
        csx_register_internal_call("Joy.Api.Native::InputboxDraw", (const void*)icall_InputboxDraw);
        csx_register_internal_call("Joy.Api.Native::InputboxIsPressed", (const void*)icall_InputboxIsPressed);
        csx_register_internal_call("Joy.Api.Native::InputboxIsHovered", (const void*)icall_InputboxIsHovered);
        csx_register_internal_call("Joy.Api.Native::InputboxFocus", (const void*)icall_InputboxFocus);
        csx_register_internal_call("Joy.Api.Native::InputboxDefocus", (const void*)icall_InputboxDefocus);
        csx_register_internal_call("Joy.Api.Native::InputboxBlur", (const void*)icall_InputboxBlur);
        csx_register_internal_call("Joy.Api.Native::InputboxInsert", (const void*)icall_InputboxInsert);
        csx_register_internal_call("Joy.Api.Native::InputboxBackspace", (const void*)icall_InputboxBackspace);
        csx_register_internal_call("Joy.Api.Native::InputboxDelete", (const void*)icall_InputboxDelete);
        csx_register_internal_call("Joy.Api.Native::InputboxCursorLeft", (const void*)icall_InputboxCursorLeft);
        csx_register_internal_call("Joy.Api.Native::InputboxCursorRight", (const void*)icall_InputboxCursorRight);
        csx_register_internal_call("Joy.Api.Native::InputboxCursorHome", (const void*)icall_InputboxCursorHome);
        csx_register_internal_call("Joy.Api.Native::InputboxCursorEnd", (const void*)icall_InputboxCursorEnd);
        csx_register_internal_call("Joy.Api.Native::InputboxSelectLeft", (const void*)icall_InputboxSelectLeft);
        csx_register_internal_call("Joy.Api.Native::InputboxSelectRight", (const void*)icall_InputboxSelectRight);
        csx_register_internal_call("Joy.Api.Native::InputboxSelectHome", (const void*)icall_InputboxSelectHome);
        csx_register_internal_call("Joy.Api.Native::InputboxSelectEnd", (const void*)icall_InputboxSelectEnd);
        csx_register_internal_call("Joy.Api.Native::InputboxSelectAll", (const void*)icall_InputboxSelectAll);
        csx_register_internal_call("Joy.Api.Native::InputboxPaste", (const void*)icall_InputboxPaste);
        csx_register_internal_call("Joy.Api.Native::InputboxCopy", (const void*)icall_InputboxCopy);
        csx_register_internal_call("Joy.Api.Native::InputboxCut", (const void*)icall_InputboxCut);
        csx_register_internal_call("Joy.Api.Native::InputboxIsFocused", (const void*)icall_InputboxIsFocused);
        csx_register_internal_call("Joy.Api.Native::SliderDestroy", (const void*)icall_SliderDestroy);
        csx_register_internal_call("Joy.Api.Native::SliderSetValue", (const void*)icall_SliderSetValue);
        csx_register_internal_call("Joy.Api.Native::SliderGetValue", (const void*)icall_SliderGetValue);
        csx_register_internal_call("Joy.Api.Native::SliderSetRect", (const void*)icall_SliderSetRect);
        csx_register_internal_call("Joy.Api.Native::SliderSetRange", (const void*)icall_SliderSetRange);
        csx_register_internal_call("Joy.Api.Native::SliderDraw", (const void*)icall_SliderDraw);
        csx_register_internal_call("Joy.Api.Native::SliderIsPressed", (const void*)icall_SliderIsPressed);
        csx_register_internal_call("Joy.Api.Native::SliderSetPressed", (const void*)icall_SliderSetPressed);
        csx_register_internal_call("Joy.Api.Native::SliderDragMove", (const void*)icall_SliderDragMove);
        csx_register_internal_call("Joy.Api.Native::SliderIsHovered", (const void*)icall_SliderIsHovered);
        csx_register_internal_call("Joy.Api.Native::SliderSetHovered", (const void*)icall_SliderSetHovered);
        csx_register_internal_call("Joy.Api.Native::PopupDestroy", (const void*)icall_PopupDestroy);
        csx_register_internal_call("Joy.Api.Native::PopupSetTitle", (const void*)icall_PopupSetTitle);
        csx_register_internal_call("Joy.Api.Native::PopupSetContent", (const void*)icall_PopupSetContent);
        csx_register_internal_call("Joy.Api.Native::PopupSetPosition", (const void*)icall_PopupSetPosition);
        csx_register_internal_call("Joy.Api.Native::PopupSetVisible", (const void*)icall_PopupSetVisible);
        csx_register_internal_call("Joy.Api.Native::PopupSetCloseable", (const void*)icall_PopupSetCloseable);
        csx_register_internal_call("Joy.Api.Native::PopupDraw", (const void*)icall_PopupDraw);
        csx_register_internal_call("Joy.Api.Native::PopupIsCloseClicked", (const void*)icall_PopupIsCloseClicked);
        csx_register_internal_call("Joy.Api.Native::PopupIsTitleClicked", (const void*)icall_PopupIsTitleClicked);
        csx_register_internal_call("Joy.Api.Native::PopupDragMove", (const void*)icall_PopupDragMove);
        csx_register_internal_call("Joy.Api.Native::PopupIsInside", (const void*)icall_PopupIsInside);
        csx_register_internal_call("Joy.Api.Native::TooltipDestroy", (const void*)icall_TooltipDestroy);
        csx_register_internal_call("Joy.Api.Native::TooltipSetText", (const void*)icall_TooltipSetText);
        csx_register_internal_call("Joy.Api.Native::TooltipSetPosition", (const void*)icall_TooltipSetPosition);
        csx_register_internal_call("Joy.Api.Native::TooltipSetVisible", (const void*)icall_TooltipSetVisible);
        csx_register_internal_call("Joy.Api.Native::TooltipDraw", (const void*)icall_TooltipDraw);
        csx_register_internal_call("Joy.Api.Native::JuiCtxDestroy", (const void*)icall_JuiCtxDestroy);
        csx_register_internal_call("Joy.Api.Native::JuiBegin", (const void*)icall_JuiBegin);
        csx_register_internal_call("Joy.Api.Native::JuiEnd", (const void*)icall_JuiEnd);
        csx_register_internal_call("Joy.Api.Native::JuiInputMotion", (const void*)icall_JuiInputMotion);
        csx_register_internal_call("Joy.Api.Native::JuiInputDown", (const void*)icall_JuiInputDown);
        csx_register_internal_call("Joy.Api.Native::JuiInputUp", (const void*)icall_JuiInputUp);
        csx_register_internal_call("Joy.Api.Native::JuiInputWheel", (const void*)icall_JuiInputWheel);
        csx_register_internal_call("Joy.Api.Native::JuiInputKey", (const void*)icall_JuiInputKey);
        csx_register_internal_call("Joy.Api.Native::JuiIsDown", (const void*)icall_JuiIsDown);
        csx_register_internal_call("Joy.Api.Native::JuiIsPressed", (const void*)icall_JuiIsPressed);
        csx_register_internal_call("Joy.Api.Native::JuiIsReleased", (const void*)icall_JuiIsReleased);
        csx_register_internal_call("Joy.Api.Native::JuiTakeWheel", (const void*)icall_JuiTakeWheel);
        csx_register_internal_call("Joy.Api.Native::JuiIsShift", (const void*)icall_JuiIsShift);
        csx_register_internal_call("Joy.Api.Native::JuiIsCtrl", (const void*)icall_JuiIsCtrl);
        csx_register_internal_call("Joy.Api.Native::JuiPushClip", (const void*)icall_JuiPushClip);
        csx_register_internal_call("Joy.Api.Native::JuiPopClip", (const void*)icall_JuiPopClip);
        csx_register_internal_call("Joy.Api.Native::JuiResetClip", (const void*)icall_JuiResetClip);
        csx_register_internal_call("Joy.Api.Native::JuiHit", (const void*)icall_JuiHit);
        csx_register_internal_call("Joy.Api.Native::JuiFillRect", (const void*)icall_JuiFillRect);
        csx_register_internal_call("Joy.Api.Native::JuiStrokeRect", (const void*)icall_JuiStrokeRect);
        csx_register_internal_call("Joy.Api.Native::JuiLine", (const void*)icall_JuiLine);
        csx_register_internal_call("Joy.Api.Native::JuiText", (const void*)icall_JuiText);
        csx_register_internal_call("Joy.Api.Native::JuiTextCentered", (const void*)icall_JuiTextCentered);
        csx_register_internal_call("Joy.Api.Native::JuiPanelBegin", (const void*)icall_JuiPanelBegin);
        csx_register_internal_call("Joy.Api.Native::JuiPanelEnd", (const void*)icall_JuiPanelEnd);
        csx_register_internal_call("Joy.Api.Native::JuiPanelFrame", (const void*)icall_JuiPanelFrame);
        csx_register_internal_call("Joy.Api.Native::JuiScrollBegin", (const void*)icall_JuiScrollBegin);
        csx_register_internal_call("Joy.Api.Native::JuiScrollEnd", (const void*)icall_JuiScrollEnd);
        csx_register_internal_call("Joy.Api.Native::JuiScrollbar", (const void*)icall_JuiScrollbar);
        csx_register_internal_call("Joy.Api.Native::JuiScrollSetOffset", (const void*)icall_JuiScrollSetOffset);
        csx_register_internal_call("Joy.Api.Native::JuiButton", (const void*)icall_JuiButton);
        csx_register_internal_call("Joy.Api.Native::JtilemapDestroy", (const void*)icall_JtilemapDestroy);
        csx_register_internal_call("Joy.Api.Native::JtilemapGetWidth", (const void*)icall_JtilemapGetWidth);
        csx_register_internal_call("Joy.Api.Native::JtilemapGetHeight", (const void*)icall_JtilemapGetHeight);
        csx_register_internal_call("Joy.Api.Native::JtilemapGetTilewidth", (const void*)icall_JtilemapGetTilewidth);
        csx_register_internal_call("Joy.Api.Native::JtilemapGetTileheight", (const void*)icall_JtilemapGetTileheight);
        csx_register_internal_call("Joy.Api.Native::JtilemapGetAnimTileid", (const void*)icall_JtilemapGetAnimTileid);
        csx_register_internal_call("Joy.Api.Native::JtilemapFindTileLayer", (const void*)icall_JtilemapFindTileLayer);
        csx_register_internal_call("Joy.Api.Native::JtilemapFindObjectLayer", (const void*)icall_JtilemapFindObjectLayer);
        csx_register_internal_call("Joy.Api.Native::JtilemapGetTile", (const void*)icall_JtilemapGetTile);
        csx_register_internal_call("Joy.Api.Native::JtilemapRenderAll", (const void*)icall_JtilemapRenderAll);
        csx_register_internal_call("Joy.Api.Native::JtilemapRenderTiles", (const void*)icall_JtilemapRenderTiles);
        csx_register_internal_call("Joy.Api.Native::JtilemapRenderSetTilePx", (const void*)icall_JtilemapRenderSetTilePx);
        csx_register_internal_call("Joy.Api.Native::JtilemapRenderLayer", (const void*)icall_JtilemapRenderLayer);
        csx_register_internal_call("Joy.Api.Native::JtilemapRenderDestroy", (const void*)icall_JtilemapRenderDestroy);
        csx_register_internal_call("Joy.Api.Native::KcpserverDestroy", (const void*)icall_KcpserverDestroy);
        csx_register_internal_call("Joy.Api.Native::KcpserverSend", (const void*)icall_KcpserverSend);
        csx_register_internal_call("Joy.Api.Native::KcpserverBroadcast", (const void*)icall_KcpserverBroadcast);
        csx_register_internal_call("Joy.Api.Native::KcpserverOffline", (const void*)icall_KcpserverOffline);
        csx_register_internal_call("Joy.Api.Native::KcpserverUpdate", (const void*)icall_KcpserverUpdate);
        csx_register_internal_call("Joy.Api.Native::KcpserverConnectionCount", (const void*)icall_KcpserverConnectionCount);
        csx_register_internal_call("Joy.Api.Native::KcpserverPollMessage", (const void*)icall_KcpserverPollMessage);
        csx_register_internal_call("Joy.Api.Native::KcpclientDestroy", (const void*)icall_KcpclientDestroy);
        csx_register_internal_call("Joy.Api.Native::KcpclientSend", (const void*)icall_KcpclientSend);
        csx_register_internal_call("Joy.Api.Native::KcpclientUpdate", (const void*)icall_KcpclientUpdate);
        csx_register_internal_call("Joy.Api.Native::KcpclientPollMessage", (const void*)icall_KcpclientPollMessage);
        csx_register_internal_call("Joy.Api.Native::TcpserverDestroy", (const void*)icall_TcpserverDestroy);
        csx_register_internal_call("Joy.Api.Native::TcpserverSend", (const void*)icall_TcpserverSend);
        csx_register_internal_call("Joy.Api.Native::TcpserverBroadcast", (const void*)icall_TcpserverBroadcast);
        csx_register_internal_call("Joy.Api.Native::TcpserverOffline", (const void*)icall_TcpserverOffline);
        csx_register_internal_call("Joy.Api.Native::TcpserverUpdate", (const void*)icall_TcpserverUpdate);
        csx_register_internal_call("Joy.Api.Native::TcpserverConnectionCount", (const void*)icall_TcpserverConnectionCount);
        csx_register_internal_call("Joy.Api.Native::TcpserverPollMessage", (const void*)icall_TcpserverPollMessage);
        csx_register_internal_call("Joy.Api.Native::TcpclientDestroy", (const void*)icall_TcpclientDestroy);
        csx_register_internal_call("Joy.Api.Native::TcpclientSend", (const void*)icall_TcpclientSend);
        csx_register_internal_call("Joy.Api.Native::TcpclientUpdate", (const void*)icall_TcpclientUpdate);
        csx_register_internal_call("Joy.Api.Native::TcpclientPollMessage", (const void*)icall_TcpclientPollMessage);
        csx_register_internal_call("Joy.Api.Native::WsnetserverDestroy", (const void*)icall_WsnetserverDestroy);
        csx_register_internal_call("Joy.Api.Native::WsnetserverSend", (const void*)icall_WsnetserverSend);
        csx_register_internal_call("Joy.Api.Native::WsnetserverBroadcast", (const void*)icall_WsnetserverBroadcast);
        csx_register_internal_call("Joy.Api.Native::WsnetserverOffline", (const void*)icall_WsnetserverOffline);
        csx_register_internal_call("Joy.Api.Native::WsnetserverUpdate", (const void*)icall_WsnetserverUpdate);
        csx_register_internal_call("Joy.Api.Native::WsnetserverConnectionCount", (const void*)icall_WsnetserverConnectionCount);
        csx_register_internal_call("Joy.Api.Native::WsnetserverPollMessage", (const void*)icall_WsnetserverPollMessage);
        csx_register_internal_call("Joy.Api.Native::WsnetclientDestroy", (const void*)icall_WsnetclientDestroy);
        csx_register_internal_call("Joy.Api.Native::WsnetclientSend", (const void*)icall_WsnetclientSend);
        csx_register_internal_call("Joy.Api.Native::WsnetclientUpdate", (const void*)icall_WsnetclientUpdate);
        csx_register_internal_call("Joy.Api.Native::WsnetclientPollMessage", (const void*)icall_WsnetclientPollMessage);
        csx_register_internal_call("Joy.Api.Native::NetclientDestroy", (const void*)icall_NetclientDestroy);
        csx_register_internal_call("Joy.Api.Native::NetclientSend", (const void*)icall_NetclientSend);
        csx_register_internal_call("Joy.Api.Native::NetclientUpdate", (const void*)icall_NetclientUpdate);
        csx_register_internal_call("Joy.Api.Native::NetclientPollMessage", (const void*)icall_NetclientPollMessage);
        csx_register_internal_call("Joy.Api.Native::NetserverDestroy", (const void*)icall_NetserverDestroy);
        csx_register_internal_call("Joy.Api.Native::NetserverSend", (const void*)icall_NetserverSend);
        csx_register_internal_call("Joy.Api.Native::NetserverBroadcast", (const void*)icall_NetserverBroadcast);
        csx_register_internal_call("Joy.Api.Native::NetserverOffline", (const void*)icall_NetserverOffline);
        csx_register_internal_call("Joy.Api.Native::NetserverUpdate", (const void*)icall_NetserverUpdate);
        csx_register_internal_call("Joy.Api.Native::NetserverConnectionCount", (const void*)icall_NetserverConnectionCount);
        csx_register_internal_call("Joy.Api.Native::NetserverPollMessage", (const void*)icall_NetserverPollMessage);
        csx_register_internal_call("Joy.Api.Native::NetmessageFreeData", (const void*)icall_NetmessageFreeData);
        csx_register_internal_call("Joy.Api.Native::SceneNodeDestroy", (const void*)icall_SceneNodeDestroy);
        csx_register_internal_call("Joy.Api.Native::SceneNodeAddChild", (const void*)icall_SceneNodeAddChild);
        csx_register_internal_call("Joy.Api.Native::SceneNodeRemoveChild", (const void*)icall_SceneNodeRemoveChild);
        csx_register_internal_call("Joy.Api.Native::SceneNodeGetChildCount", (const void*)icall_SceneNodeGetChildCount);
        csx_register_internal_call("Joy.Api.Native::SceneNodeSetPosition", (const void*)icall_SceneNodeSetPosition);
        csx_register_internal_call("Joy.Api.Native::SceneNodeSetWorldPosition", (const void*)icall_SceneNodeSetWorldPosition);
        csx_register_internal_call("Joy.Api.Native::SceneNodeSetRotation", (const void*)icall_SceneNodeSetRotation);
        csx_register_internal_call("Joy.Api.Native::SceneNodeGetRotation", (const void*)icall_SceneNodeGetRotation);
        csx_register_internal_call("Joy.Api.Native::SceneNodeSetScale", (const void*)icall_SceneNodeSetScale);
        csx_register_internal_call("Joy.Api.Native::SceneNodeSetZorder", (const void*)icall_SceneNodeSetZorder);
        csx_register_internal_call("Joy.Api.Native::SceneNodeGetZorder", (const void*)icall_SceneNodeGetZorder);
        csx_register_internal_call("Joy.Api.Native::SceneNodeSetName", (const void*)icall_SceneNodeSetName);
        csx_register_internal_call("Joy.Api.Native::SceneNodeSetTag", (const void*)icall_SceneNodeSetTag);
        csx_register_internal_call("Joy.Api.Native::SceneFindNodeByName", (const void*)icall_SceneFindNodeByName);
        csx_register_internal_call("Joy.Api.Native::SceneFindNodeByNameDeep", (const void*)icall_SceneFindNodeByNameDeep);
        csx_register_internal_call("Joy.Api.Native::SceneNodeUpdate", (const void*)icall_SceneNodeUpdate);
        csx_register_internal_call("Joy.Api.Native::SceneDestroy", (const void*)icall_SceneDestroy);
        csx_register_internal_call("Joy.Api.Native::SceneAddRootNode", (const void*)icall_SceneAddRootNode);
        csx_register_internal_call("Joy.Api.Native::SceneRemoveRootNode", (const void*)icall_SceneRemoveRootNode);
        csx_register_internal_call("Joy.Api.Native::SceneGetRootCount", (const void*)icall_SceneGetRootCount);
        csx_register_internal_call("Joy.Api.Native::SceneGetRootAt", (const void*)icall_SceneGetRootAt);
        csx_register_internal_call("Joy.Api.Native::SceneUpdate", (const void*)icall_SceneUpdate);
        csx_register_internal_call("Joy.Api.Native::SceneSetPopupNode", (const void*)icall_SceneSetPopupNode);
        csx_register_internal_call("Joy.Api.Native::SceneGetPopupNode", (const void*)icall_SceneGetPopupNode);
        csx_register_internal_call("Joy.Api.Native::SceneManagerDestroy", (const void*)icall_SceneManagerDestroy);
        csx_register_internal_call("Joy.Api.Native::SceneManagerPush", (const void*)icall_SceneManagerPush);
        csx_register_internal_call("Joy.Api.Native::SceneManagerReplace", (const void*)icall_SceneManagerReplace);
        csx_register_internal_call("Joy.Api.Native::SceneManagerPop", (const void*)icall_SceneManagerPop);
        csx_register_internal_call("Joy.Api.Native::SceneManagerUpdate", (const void*)icall_SceneManagerUpdate);
        csx_register_internal_call("Joy.Api.Native::SceneManagerGetCurrent", (const void*)icall_SceneManagerGetCurrent);
        csx_register_internal_call("Joy.Api.Native::SceneManagerGetCount", (const void*)icall_SceneManagerGetCount);
        csx_register_internal_call("Joy.Api.Native::JparticleDestroy", (const void*)icall_JparticleDestroy);
        csx_register_internal_call("Joy.Api.Native::JparticleSetPosition", (const void*)icall_JparticleSetPosition);
        csx_register_internal_call("Joy.Api.Native::JparticleEmit", (const void*)icall_JparticleEmit);
        csx_register_internal_call("Joy.Api.Native::JparticleClear", (const void*)icall_JparticleClear);
        csx_register_internal_call("Joy.Api.Native::JparticleUpdate", (const void*)icall_JparticleUpdate);
        csx_register_internal_call("Joy.Api.Native::JparticleRender", (const void*)icall_JparticleRender);
        csx_register_internal_call("Joy.Api.Native::JparticleCount", (const void*)icall_JparticleCount);
        csx_register_internal_call("Joy.Api.Native::JparticleIsActive", (const void*)icall_JparticleIsActive);
        csx_register_internal_call("Joy.Api.Native::JlightSysDestroy", (const void*)icall_JlightSysDestroy);
        csx_register_internal_call("Joy.Api.Native::JlightSysResize", (const void*)icall_JlightSysResize);
        csx_register_internal_call("Joy.Api.Native::JlightRemove", (const void*)icall_JlightRemove);
        csx_register_internal_call("Joy.Api.Native::JlightClear", (const void*)icall_JlightClear);
        csx_register_internal_call("Joy.Api.Native::JlightAddOccluder", (const void*)icall_JlightAddOccluder);
        csx_register_internal_call("Joy.Api.Native::JlightClearOccluders", (const void*)icall_JlightClearOccluders);
        csx_register_internal_call("Joy.Api.Native::JlightSetAmbient", (const void*)icall_JlightSetAmbient);
        csx_register_internal_call("Joy.Api.Native::JlightRender", (const void*)icall_JlightRender);
        csx_register_internal_call("Joy.Api.Native::JecsDestroy", (const void*)icall_JecsDestroy);
        csx_register_internal_call("Joy.Api.Native::JecsProcess", (const void*)icall_JecsProcess);
        csx_register_internal_call("Joy.Api.Native::JecsDefine", (const void*)icall_JecsDefine);
        csx_register_internal_call("Joy.Api.Native::AudioOpenDevice", (const void*)icall_AudioOpenDevice);
        csx_register_internal_call("Joy.Api.Native::AudioCloseDevice", (const void*)icall_AudioCloseDevice);
        csx_register_internal_call("Joy.Api.Native::AudioCreateWav", (const void*)icall_AudioCreateWav);
        csx_register_internal_call("Joy.Api.Native::AudioCreateMp3", (const void*)icall_AudioCreateMp3);
        csx_register_internal_call("Joy.Api.Native::AudioCreateFlac", (const void*)icall_AudioCreateFlac);
        csx_register_internal_call("Joy.Api.Native::AudioCreateOgg", (const void*)icall_AudioCreateOgg);
        csx_register_internal_call("Joy.Api.Native::AudioShotPlay", (const void*)icall_AudioShotPlay);
        csx_register_internal_call("Joy.Api.Native::AudioShotSetGain", (const void*)icall_AudioShotSetGain);
        csx_register_internal_call("Joy.Api.Native::AudioShotDestroy", (const void*)icall_AudioShotDestroy);
        csx_register_internal_call("Joy.Api.Native::AudioBgmStop", (const void*)icall_AudioBgmStop);
        csx_register_internal_call("Joy.Api.Native::AudioBgmSetGain", (const void*)icall_AudioBgmSetGain);
        csx_register_internal_call("Joy.Api.Native::AudioBgmDestroy", (const void*)icall_AudioBgmDestroy);
        csx_register_internal_call("Joy.Api.Native::ImageDrawRect", (const void*)icall_ImageDrawRect);
        csx_register_internal_call("Joy.Api.Native::JoyInputFeedKey", (const void*)icall_JoyInputFeedKey);
        csx_register_internal_call("Joy.Api.Native::JoyInputFeedMouseMove", (const void*)icall_JoyInputFeedMouseMove);
        csx_register_internal_call("Joy.Api.Native::JoyInputFeedMouseButton", (const void*)icall_JoyInputFeedMouseButton);
        csx_register_internal_call("Joy.Api.Native::JoyInputFeedWheel", (const void*)icall_JoyInputFeedWheel);
        csx_register_internal_call("Joy.Api.Native::JoyInputEndFrame", (const void*)icall_JoyInputEndFrame);
        csx_register_internal_call("Joy.Api.Native::JoyInputReset", (const void*)icall_JoyInputReset);
        csx_register_internal_call("Joy.Api.Native::JoyInputIsKeyDown", (const void*)icall_JoyInputIsKeyDown);
        csx_register_internal_call("Joy.Api.Native::JoyInputIsMouseButtonDown", (const void*)icall_JoyInputIsMouseButtonDown);
        csx_register_internal_call("Joy.Api.Native::JoyInputGetMouseX", (const void*)icall_JoyInputGetMouseX);
        csx_register_internal_call("Joy.Api.Native::JoyInputGetMouseY", (const void*)icall_JoyInputGetMouseY);
        csx_register_internal_call("Joy.Api.Native::JoyInputGetWheel", (const void*)icall_JoyInputGetWheel);
        csx_register_internal_call("Joy.Api.Native::JoyInputWasKeyPressed", (const void*)icall_JoyInputWasKeyPressed);
        csx_register_internal_call("Joy.Api.Native::JoyInputWasKeyReleased", (const void*)icall_JoyInputWasKeyReleased);
        csx_register_internal_call("Joy.Api.Native::JmathFpFromFloat", (const void*)icall_JmathFpFromFloat);
        csx_register_internal_call("Joy.Api.Native::JmathFpToFloat", (const void*)icall_JmathFpToFloat);
        csx_register_internal_call("Joy.Api.Native::JmathFpFromUint", (const void*)icall_JmathFpFromUint);
        csx_register_internal_call("Joy.Api.Native::JmathFpFromInt", (const void*)icall_JmathFpFromInt);
        csx_register_internal_call("Joy.Api.Native::JmathFpFromFraction", (const void*)icall_JmathFpFromFraction);
        csx_register_internal_call("Joy.Api.Native::JmathFpMinValue", (const void*)icall_JmathFpMinValue);
        csx_register_internal_call("Joy.Api.Native::JmathFpMaxValue", (const void*)icall_JmathFpMaxValue);
        csx_register_internal_call("Joy.Api.Native::JmathFpAdd", (const void*)icall_JmathFpAdd);
        csx_register_internal_call("Joy.Api.Native::JmathFpSub", (const void*)icall_JmathFpSub);
        csx_register_internal_call("Joy.Api.Native::JmathFpZero", (const void*)icall_JmathFpZero);
        csx_register_internal_call("Joy.Api.Native::JmathFpHalf", (const void*)icall_JmathFpHalf);
        csx_register_internal_call("Joy.Api.Native::JmathFpOne", (const void*)icall_JmathFpOne);
        csx_register_internal_call("Joy.Api.Native::JmathFpPi", (const void*)icall_JmathFpPi);
        csx_register_internal_call("Joy.Api.Native::JmathFpMax", (const void*)icall_JmathFpMax);
        csx_register_internal_call("Joy.Api.Native::JmathFpMin", (const void*)icall_JmathFpMin);
        csx_register_internal_call("Joy.Api.Native::JmathFpPow2", (const void*)icall_JmathFpPow2);
        csx_register_internal_call("Joy.Api.Native::JmathFpSign", (const void*)icall_JmathFpSign);
        csx_register_internal_call("Joy.Api.Native::JmathFpClamp", (const void*)icall_JmathFpClamp);
        csx_register_internal_call("Joy.Api.Native::JmathFpLerp", (const void*)icall_JmathFpLerp);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fNegateX", (const void*)icall_JmathVec2fNegateX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fNegateY", (const void*)icall_JmathVec2fNegateY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fDot", (const void*)icall_JmathVec2fDot);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fCross", (const void*)icall_JmathVec2fCross);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fCross2X", (const void*)icall_JmathVec2fCross2X);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fCross2Y", (const void*)icall_JmathVec2fCross2Y);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fCross3X", (const void*)icall_JmathVec2fCross3X);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fCross3Y", (const void*)icall_JmathVec2fCross3Y);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fAddX", (const void*)icall_JmathVec2fAddX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fAddY", (const void*)icall_JmathVec2fAddY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fSubX", (const void*)icall_JmathVec2fSubX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fSubY", (const void*)icall_JmathVec2fSubY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fScaleX", (const void*)icall_JmathVec2fScaleX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fScaleY", (const void*)icall_JmathVec2fScaleY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fAbsX", (const void*)icall_JmathVec2fAbsX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fAbsY", (const void*)icall_JmathVec2fAbsY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fLengthSquared", (const void*)icall_JmathVec2fLengthSquared);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fLength", (const void*)icall_JmathVec2fLength);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fNormalizeX", (const void*)icall_JmathVec2fNormalizeX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fNormalizeY", (const void*)icall_JmathVec2fNormalizeY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fNormalX", (const void*)icall_JmathVec2fNormalX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fNormalY", (const void*)icall_JmathVec2fNormalY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fRotateX", (const void*)icall_JmathVec2fRotateX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fRotateY", (const void*)icall_JmathVec2fRotateY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fLerpX", (const void*)icall_JmathVec2fLerpX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fLerpY", (const void*)icall_JmathVec2fLerpY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2fDistance", (const void*)icall_JmathVec2fDistance);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fNegateX", (const void*)icall_JmathVec3fNegateX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fNegateY", (const void*)icall_JmathVec3fNegateY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fNegateZ", (const void*)icall_JmathVec3fNegateZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fAddX", (const void*)icall_JmathVec3fAddX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fAddY", (const void*)icall_JmathVec3fAddY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fAddZ", (const void*)icall_JmathVec3fAddZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fSubX", (const void*)icall_JmathVec3fSubX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fSubY", (const void*)icall_JmathVec3fSubY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fSubZ", (const void*)icall_JmathVec3fSubZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fScaleX", (const void*)icall_JmathVec3fScaleX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fScaleY", (const void*)icall_JmathVec3fScaleY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fScaleZ", (const void*)icall_JmathVec3fScaleZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fAbsX", (const void*)icall_JmathVec3fAbsX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fAbsY", (const void*)icall_JmathVec3fAbsY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fAbsZ", (const void*)icall_JmathVec3fAbsZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fDot", (const void*)icall_JmathVec3fDot);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fCrossX", (const void*)icall_JmathVec3fCrossX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fCrossY", (const void*)icall_JmathVec3fCrossY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fCrossZ", (const void*)icall_JmathVec3fCrossZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fSquareLength", (const void*)icall_JmathVec3fSquareLength);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fLength", (const void*)icall_JmathVec3fLength);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fNormalizeX", (const void*)icall_JmathVec3fNormalizeX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fNormalizeY", (const void*)icall_JmathVec3fNormalizeY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fNormalizeZ", (const void*)icall_JmathVec3fNormalizeZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fLerpX", (const void*)icall_JmathVec3fLerpX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fLerpY", (const void*)icall_JmathVec3fLerpY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fLerpZ", (const void*)icall_JmathVec3fLerpZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3fDistance", (const void*)icall_JmathVec3fDistance);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fMulVec2fX", (const void*)icall_JmathMat22fMulVec2fX);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fMulVec2fY", (const void*)icall_JmathMat22fMulVec2fY);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fRotateCol1X", (const void*)icall_JmathMat22fRotateCol1X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fRotateCol1Y", (const void*)icall_JmathMat22fRotateCol1Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fRotateCol2X", (const void*)icall_JmathMat22fRotateCol2X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fRotateCol2Y", (const void*)icall_JmathMat22fRotateCol2Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fTransposeCol1X", (const void*)icall_JmathMat22fTransposeCol1X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fTransposeCol1Y", (const void*)icall_JmathMat22fTransposeCol1Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fTransposeCol2X", (const void*)icall_JmathMat22fTransposeCol2X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fTransposeCol2Y", (const void*)icall_JmathMat22fTransposeCol2Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fInvertCol1X", (const void*)icall_JmathMat22fInvertCol1X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fInvertCol1Y", (const void*)icall_JmathMat22fInvertCol1Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fInvertCol2X", (const void*)icall_JmathMat22fInvertCol2X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fInvertCol2Y", (const void*)icall_JmathMat22fInvertCol2Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fAddCol1X", (const void*)icall_JmathMat22fAddCol1X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fAddCol1Y", (const void*)icall_JmathMat22fAddCol1Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fAddCol2X", (const void*)icall_JmathMat22fAddCol2X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fAddCol2Y", (const void*)icall_JmathMat22fAddCol2Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fMulCol1X", (const void*)icall_JmathMat22fMulCol1X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fMulCol1Y", (const void*)icall_JmathMat22fMulCol1Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fMulCol2X", (const void*)icall_JmathMat22fMulCol2X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fMulCol2Y", (const void*)icall_JmathMat22fMulCol2Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fAbsCol1X", (const void*)icall_JmathMat22fAbsCol1X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fAbsCol1Y", (const void*)icall_JmathMat22fAbsCol1Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fAbsCol2X", (const void*)icall_JmathMat22fAbsCol2X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22fAbsCol2Y", (const void*)icall_JmathMat22fAbsCol2Y);
        csx_register_internal_call("Joy.Api.Native::JmathFtFromFloat", (const void*)icall_JmathFtFromFloat);
        csx_register_internal_call("Joy.Api.Native::JmathFtToFloat", (const void*)icall_JmathFtToFloat);
        csx_register_internal_call("Joy.Api.Native::JmathFtMinValue", (const void*)icall_JmathFtMinValue);
        csx_register_internal_call("Joy.Api.Native::JmathFtMaxValue", (const void*)icall_JmathFtMaxValue);
        csx_register_internal_call("Joy.Api.Native::JmathFtAdd", (const void*)icall_JmathFtAdd);
        csx_register_internal_call("Joy.Api.Native::JmathFtSub", (const void*)icall_JmathFtSub);
        csx_register_internal_call("Joy.Api.Native::JmathFtMul", (const void*)icall_JmathFtMul);
        csx_register_internal_call("Joy.Api.Native::JmathFtDiv", (const void*)icall_JmathFtDiv);
        csx_register_internal_call("Joy.Api.Native::JmathFtAbs", (const void*)icall_JmathFtAbs);
        csx_register_internal_call("Joy.Api.Native::JmathFtSqrt", (const void*)icall_JmathFtSqrt);
        csx_register_internal_call("Joy.Api.Native::JmathFtSin", (const void*)icall_JmathFtSin);
        csx_register_internal_call("Joy.Api.Native::JmathFtCos", (const void*)icall_JmathFtCos);
        csx_register_internal_call("Joy.Api.Native::JmathFtAtan2", (const void*)icall_JmathFtAtan2);
        csx_register_internal_call("Joy.Api.Native::JmathFtZero", (const void*)icall_JmathFtZero);
        csx_register_internal_call("Joy.Api.Native::JmathFtHalf", (const void*)icall_JmathFtHalf);
        csx_register_internal_call("Joy.Api.Native::JmathFtOne", (const void*)icall_JmathFtOne);
        csx_register_internal_call("Joy.Api.Native::JmathFtPi", (const void*)icall_JmathFtPi);
        csx_register_internal_call("Joy.Api.Native::JmathFtMax", (const void*)icall_JmathFtMax);
        csx_register_internal_call("Joy.Api.Native::JmathFtMin", (const void*)icall_JmathFtMin);
        csx_register_internal_call("Joy.Api.Native::JmathFtPow2", (const void*)icall_JmathFtPow2);
        csx_register_internal_call("Joy.Api.Native::JmathFtSign", (const void*)icall_JmathFtSign);
        csx_register_internal_call("Joy.Api.Native::JmathFtClamp", (const void*)icall_JmathFtClamp);
        csx_register_internal_call("Joy.Api.Native::JmathFtLerp", (const void*)icall_JmathFtLerp);
        csx_register_internal_call("Joy.Api.Native::JmathVec2NegateX", (const void*)icall_JmathVec2NegateX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2NegateY", (const void*)icall_JmathVec2NegateY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Negate", (const void*)icall_JmathVec2Negate);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Dot", (const void*)icall_JmathVec2Dot);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Cross", (const void*)icall_JmathVec2Cross);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Cross2X", (const void*)icall_JmathVec2Cross2X);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Cross2Y", (const void*)icall_JmathVec2Cross2Y);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Cross2", (const void*)icall_JmathVec2Cross2);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Cross3X", (const void*)icall_JmathVec2Cross3X);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Cross3Y", (const void*)icall_JmathVec2Cross3Y);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Cross3", (const void*)icall_JmathVec2Cross3);
        csx_register_internal_call("Joy.Api.Native::JmathVec2AddX", (const void*)icall_JmathVec2AddX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2AddY", (const void*)icall_JmathVec2AddY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Add", (const void*)icall_JmathVec2Add);
        csx_register_internal_call("Joy.Api.Native::JmathVec2SubX", (const void*)icall_JmathVec2SubX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2SubY", (const void*)icall_JmathVec2SubY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Sub", (const void*)icall_JmathVec2Sub);
        csx_register_internal_call("Joy.Api.Native::JmathVec2ScaleX", (const void*)icall_JmathVec2ScaleX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2ScaleY", (const void*)icall_JmathVec2ScaleY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Scale", (const void*)icall_JmathVec2Scale);
        csx_register_internal_call("Joy.Api.Native::JmathVec2AbsX", (const void*)icall_JmathVec2AbsX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2AbsY", (const void*)icall_JmathVec2AbsY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Abs", (const void*)icall_JmathVec2Abs);
        csx_register_internal_call("Joy.Api.Native::JmathVec2LengthSquared", (const void*)icall_JmathVec2LengthSquared);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Length", (const void*)icall_JmathVec2Length);
        csx_register_internal_call("Joy.Api.Native::JmathVec2NormalizeX", (const void*)icall_JmathVec2NormalizeX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2NormalizeY", (const void*)icall_JmathVec2NormalizeY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Normalize", (const void*)icall_JmathVec2Normalize);
        csx_register_internal_call("Joy.Api.Native::JmathVec2NormalX", (const void*)icall_JmathVec2NormalX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2NormalY", (const void*)icall_JmathVec2NormalY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Normal", (const void*)icall_JmathVec2Normal);
        csx_register_internal_call("Joy.Api.Native::JmathVec2RotateX", (const void*)icall_JmathVec2RotateX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2RotateY", (const void*)icall_JmathVec2RotateY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Rotate", (const void*)icall_JmathVec2Rotate);
        csx_register_internal_call("Joy.Api.Native::JmathVec2LerpX", (const void*)icall_JmathVec2LerpX);
        csx_register_internal_call("Joy.Api.Native::JmathVec2LerpY", (const void*)icall_JmathVec2LerpY);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Lerp", (const void*)icall_JmathVec2Lerp);
        csx_register_internal_call("Joy.Api.Native::JmathVec2Distance", (const void*)icall_JmathVec2Distance);
        csx_register_internal_call("Joy.Api.Native::JmathVec3NegateX", (const void*)icall_JmathVec3NegateX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3NegateY", (const void*)icall_JmathVec3NegateY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3NegateZ", (const void*)icall_JmathVec3NegateZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3AddX", (const void*)icall_JmathVec3AddX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3AddY", (const void*)icall_JmathVec3AddY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3AddZ", (const void*)icall_JmathVec3AddZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3SubX", (const void*)icall_JmathVec3SubX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3SubY", (const void*)icall_JmathVec3SubY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3SubZ", (const void*)icall_JmathVec3SubZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3ScaleX", (const void*)icall_JmathVec3ScaleX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3ScaleY", (const void*)icall_JmathVec3ScaleY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3ScaleZ", (const void*)icall_JmathVec3ScaleZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3AbsX", (const void*)icall_JmathVec3AbsX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3AbsY", (const void*)icall_JmathVec3AbsY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3AbsZ", (const void*)icall_JmathVec3AbsZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3Dot", (const void*)icall_JmathVec3Dot);
        csx_register_internal_call("Joy.Api.Native::JmathVec3CrossX", (const void*)icall_JmathVec3CrossX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3CrossY", (const void*)icall_JmathVec3CrossY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3CrossZ", (const void*)icall_JmathVec3CrossZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3SquareLength", (const void*)icall_JmathVec3SquareLength);
        csx_register_internal_call("Joy.Api.Native::JmathVec3Length", (const void*)icall_JmathVec3Length);
        csx_register_internal_call("Joy.Api.Native::JmathVec3NormalizeX", (const void*)icall_JmathVec3NormalizeX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3NormalizeY", (const void*)icall_JmathVec3NormalizeY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3NormalizeZ", (const void*)icall_JmathVec3NormalizeZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3LerpX", (const void*)icall_JmathVec3LerpX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3LerpY", (const void*)icall_JmathVec3LerpY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3LerpZ", (const void*)icall_JmathVec3LerpZ);
        csx_register_internal_call("Joy.Api.Native::JmathVec3Distance", (const void*)icall_JmathVec3Distance);
        csx_register_internal_call("Joy.Api.Native::JmathMat22MulVec2X", (const void*)icall_JmathMat22MulVec2X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22MulVec2Y", (const void*)icall_JmathMat22MulVec2Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22MulVec2", (const void*)icall_JmathMat22MulVec2);
        csx_register_internal_call("Joy.Api.Native::JmathMat22RotateCol1X", (const void*)icall_JmathMat22RotateCol1X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22RotateCol1Y", (const void*)icall_JmathMat22RotateCol1Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22RotateCol2X", (const void*)icall_JmathMat22RotateCol2X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22RotateCol2Y", (const void*)icall_JmathMat22RotateCol2Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22TransposeCol1X", (const void*)icall_JmathMat22TransposeCol1X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22TransposeCol1Y", (const void*)icall_JmathMat22TransposeCol1Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22TransposeCol2X", (const void*)icall_JmathMat22TransposeCol2X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22TransposeCol2Y", (const void*)icall_JmathMat22TransposeCol2Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22InvertCol1X", (const void*)icall_JmathMat22InvertCol1X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22InvertCol1Y", (const void*)icall_JmathMat22InvertCol1Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22InvertCol2X", (const void*)icall_JmathMat22InvertCol2X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22InvertCol2Y", (const void*)icall_JmathMat22InvertCol2Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22AddCol1X", (const void*)icall_JmathMat22AddCol1X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22AddCol1Y", (const void*)icall_JmathMat22AddCol1Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22AddCol2X", (const void*)icall_JmathMat22AddCol2X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22AddCol2Y", (const void*)icall_JmathMat22AddCol2Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22MulCol1X", (const void*)icall_JmathMat22MulCol1X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22MulCol1Y", (const void*)icall_JmathMat22MulCol1Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22MulCol2X", (const void*)icall_JmathMat22MulCol2X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22MulCol2Y", (const void*)icall_JmathMat22MulCol2Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22AbsCol1X", (const void*)icall_JmathMat22AbsCol1X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22AbsCol1Y", (const void*)icall_JmathMat22AbsCol1Y);
        csx_register_internal_call("Joy.Api.Native::JmathMat22AbsCol2X", (const void*)icall_JmathMat22AbsCol2X);
        csx_register_internal_call("Joy.Api.Native::JmathMat22AbsCol2Y", (const void*)icall_JmathMat22AbsCol2Y);
        csx_register_internal_call("Joy.Api.Native::JmathVec3DivX", (const void*)icall_JmathVec3DivX);
        csx_register_internal_call("Joy.Api.Native::JmathVec3DivY", (const void*)icall_JmathVec3DivY);
        csx_register_internal_call("Joy.Api.Native::JmathVec3DivZ", (const void*)icall_JmathVec3DivZ);
        csx_register_internal_call("Joy.Api.Native::JmathFtRadians", (const void*)icall_JmathFtRadians);
        csx_register_internal_call("Joy.Api.Native::Release", (const void*)icall_release);
        csx_register_internal_call("Joy.Api.Native::DebugLiveCount", (const void*)icall_live_count);
        csx_register_internal_call("Joy.Api.Native::DebugDestroyCount", (const void*)icall_destroy_count);
}
