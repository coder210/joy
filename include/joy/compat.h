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
#endif

#endif /* JOY_COMPAT_H */
