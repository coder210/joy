/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: compat.h
Description: 公开 API 命名迁移期的旧名兼容层（P0 骨架）。
*************************************************/
#ifndef JOY_COMPAT_H
#define JOY_COMPAT_H

/*
 * 公开 API 正在统一成 joy_<域>_<动作>[_宾语]（域白名单见
 * api/tools/naming.js 的 DOMAINS，划分见 api/README.md）。P1 起按域分批迁移，
 * 每迁一条就在这里补一条旧名映射，例如：
 *
 *   #define create_window joy_core_create_window
 *
 * 本文件不是 gen_bindings.js 的 DECL_SOURCES 输入（只读 defs.h / api.h /
 * jmath_wrappers.h），且 collectDecls() 跳过 # 开头的行，所以这里的宏不会
 * 污染 joy.idl、coverage.md 或三端生成物。
 *
 * 定义 JOY_NO_LEGACY_NAMES 可关掉全部旧名，用于检查是否还有残留调用点；
 * 全量迁移完成、外部调用方（含 joy_topdown_template）切到新名后删除本文件。
 */
#ifndef JOY_NO_LEGACY_NAMES
/* P1 第一批：core/window 子域。本仓库内部调用点与 joy_topdown_template
 * 仍用旧名时由这里兜住；逐步切到新名后可逐条删除。 */
#define create_window             joy_core_create_window
#define destroy_window            joy_core_destroy_window
#define get_raw_window            joy_core_get_raw_window
#define window_set_fullscreen     joy_core_window_set_fullscreen
#define window_get_fullscreen     joy_core_window_get_fullscreen
#define window_set_icon           joy_core_window_set_icon

/* P1 第二批：core 剩余 39 条。定义文件（jcore.c / jutils.c / jsys.c）暂不
 * 改名，由这里的宏在预处理期把定义与调用点一起切到新符号；全量迁移完成、
 * 删除本文件前再统一替换定义处的源码名。 */
#define lcg_random_create          joy_core_lcg_random_create
#define lcg_random_destroy         joy_core_lcg_random_destroy
#define lcg_random_next            joy_core_lcg_random_next
#define lcg_random_get_state       joy_core_lcg_random_get_state
#define lcg_random_set_state       joy_core_lcg_random_set_state
#define sdbm_hash                  joy_core_sdbm_hash
#define fnv1a_hash                 joy_core_fnv1a_hash
#define jenkins_hash               joy_core_jenkins_hash
#define murmur3_hash               joy_core_murmur3_hash
#define kr_hash                    joy_core_kr_hash
#define utf8_decode                joy_core_utf8_decode
#define utf8_next                  joy_core_utf8_next
#define utf8_decode_all            joy_core_utf8_decode_all
#define init_env                   joy_core_init_env
#define quit_env                   joy_core_quit_env
#define get_error                  joy_core_get_error
#define get_ticks                  joy_core_get_ticks
#define get_mouse_pos              joy_core_get_mouse_pos
#define get_mouse_button           joy_core_get_mouse_button
#define dispatch_event             joy_core_dispatch_event
#define get_event_type             joy_core_get_event_type
#define get_event_code             joy_core_get_event_code
#define is_user_event              joy_core_is_user_event
#define should_close               joy_core_should_close
#define log_info                   joy_core_log_info
#define log_debug                  joy_core_log_debug
#define log_error                  joy_core_log_error
#define log_title                  joy_core_log_title
#define env_create                 joy_core_env_create
#define env_get                    joy_core_env_get
#define env_set                    joy_core_env_set
#define env_destroy                joy_core_env_destroy
#define simple_fps_create          joy_core_simple_fps_create
#define simple_fps_destory         joy_core_simple_fps_destory
#define simple_fps_update          joy_core_simple_fps_update
#define simple_fps_value           joy_core_simple_fps_value
#define get_clipboard_text         joy_core_get_clipboard_text
#define set_clipboard_text         joy_core_set_clipboard_text
#define get_local_ips              joy_core_get_local_ips
#endif

#endif /* JOY_COMPAT_H */
