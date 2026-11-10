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

/* P1 第三批：碎片归位。
 * pack/unpack → net，jtimer/game_timer → scene，push → core，
 * find_min_separation → math。定义文件同样暂不改名，由宏兜住。 */
#define pack_int8                  joy_net_pack_int8
#define pack_int16                 joy_net_pack_int16
#define pack_int32                 joy_net_pack_int32
#define pack_uint32                joy_net_pack_uint32
#define pack_int64                 joy_net_pack_int64
#define unpack_int8                joy_net_unpack_int8
#define unpack_int16               joy_net_unpack_int16
#define unpack_int32               joy_net_unpack_int32
#define unpack_uint32              joy_net_unpack_uint32
#define unpack_int64               joy_net_unpack_int64
#define pack_string                joy_net_pack_string
#define unpack_string              joy_net_unpack_string
#define jtimer_create              joy_scene_jtimer_create
#define jtimer_destroy             joy_scene_jtimer_destroy
#define jtimer_trigger             joy_scene_jtimer_trigger
#define jtimer_set_interval        joy_scene_jtimer_set_interval
#define jtimer_set_active          joy_scene_jtimer_set_active
#define jtimer_reset               joy_scene_jtimer_reset
#define jtimer_get_elapsed         joy_scene_jtimer_get_elapsed
#define push_user_event            joy_core_push_user_event
#define push_quit_event            joy_core_push_quit_event
#define game_timer_init            joy_scene_game_timer_init
#define game_timer_update          joy_scene_game_timer_update
#define game_timer_get_delta_time  joy_scene_game_timer_get_delta_time
#define game_timer_get_unscaled_delta_time joy_scene_game_timer_get_unscaled_delta_time
#define game_timer_set_time_scale  joy_scene_game_timer_set_time_scale
#define game_timer_get_time_scale  joy_scene_game_timer_get_time_scale
#define game_timer_set_target_fps  joy_scene_game_timer_set_target_fps
#define game_timer_get_target_fps  joy_scene_game_timer_get_target_fps
#define find_min_separation        joy_math_find_min_separation

/* P1 第四批：net 协议域合并（iocp / kcp / tcp / ws / net / netmessage）。 */
#define iocp_server_create          joy_net_iocp_server_create
#define iocp_server_destroy         joy_net_iocp_server_destroy
#define iocp_process_events         joy_net_iocp_process_events
#define kcpserver_create            joy_net_kcpserver_create
#define kcpserver_destroy           joy_net_kcpserver_destroy
#define kcpserver_send              joy_net_kcpserver_send
#define kcpserver_broadcast         joy_net_kcpserver_broadcast
#define kcpserver_offline           joy_net_kcpserver_offline
#define kcpserver_update            joy_net_kcpserver_update
#define kcpserver_connection_count  joy_net_kcpserver_connection_count
#define kcpserver_poll_message      joy_net_kcpserver_poll_message
#define kcpserver_set_callback      joy_net_kcpserver_set_callback
#define kcpclient_create            joy_net_kcpclient_create
#define kcpclient_destroy           joy_net_kcpclient_destroy
#define kcpclient_send              joy_net_kcpclient_send
#define kcpclient_update            joy_net_kcpclient_update
#define kcpclient_poll_message      joy_net_kcpclient_poll_message
#define kcpclient_set_callback      joy_net_kcpclient_set_callback
#define tcpserver_create            joy_net_tcpserver_create
#define tcpserver_destroy           joy_net_tcpserver_destroy
#define tcpserver_send              joy_net_tcpserver_send
#define tcpserver_broadcast         joy_net_tcpserver_broadcast
#define tcpserver_offline           joy_net_tcpserver_offline
#define tcpserver_update            joy_net_tcpserver_update
#define tcpserver_connection_count  joy_net_tcpserver_connection_count
#define tcpserver_poll_message      joy_net_tcpserver_poll_message
#define tcpclient_create            joy_net_tcpclient_create
#define tcpclient_destroy           joy_net_tcpclient_destroy
#define tcpclient_send              joy_net_tcpclient_send
#define tcpclient_update            joy_net_tcpclient_update
#define tcpclient_poll_message      joy_net_tcpclient_poll_message
#define wsnetserver_create          joy_net_wsnetserver_create
#define wsnetserver_destroy         joy_net_wsnetserver_destroy
#define wsnetserver_send            joy_net_wsnetserver_send
#define wsnetserver_broadcast       joy_net_wsnetserver_broadcast
#define wsnetserver_offline         joy_net_wsnetserver_offline
#define wsnetserver_update          joy_net_wsnetserver_update
#define wsnetserver_connection_count joy_net_wsnetserver_connection_count
#define wsnetserver_poll_message    joy_net_wsnetserver_poll_message
#define wsnetserver_set_callback    joy_net_wsnetserver_set_callback
#define wsnetclient_create          joy_net_wsnetclient_create
#define wsnetclient_destroy         joy_net_wsnetclient_destroy
#define wsnetclient_send            joy_net_wsnetclient_send
#define wsnetclient_update          joy_net_wsnetclient_update
#define wsnetclient_poll_message    joy_net_wsnetclient_poll_message
#define wsnetclient_set_callback    joy_net_wsnetclient_set_callback
#define netclient_create            joy_net_netclient_create
#define netclient_destroy           joy_net_netclient_destroy
#define netclient_send              joy_net_netclient_send
#define netclient_update            joy_net_netclient_update
#define netclient_poll_message      joy_net_netclient_poll_message
#define netclient_set_callback      joy_net_netclient_set_callback
#define netclient_get_type          joy_net_netclient_get_type
#define netserver_create            joy_net_netserver_create
#define netserver_destroy           joy_net_netserver_destroy
#define netserver_send              joy_net_netserver_send
#define netserver_broadcast         joy_net_netserver_broadcast
#define netserver_offline           joy_net_netserver_offline
#define netserver_update            joy_net_netserver_update
#define netserver_connection_count  joy_net_netserver_connection_count
#define netserver_poll_message      joy_net_netserver_poll_message
#define netserver_set_callback      joy_net_netserver_set_callback
#define netserver_get_type          joy_net_netserver_get_type
#define netmessage_free_data        joy_net_netmessage_free_data

/* P1 第五批：ui-A1，jui / jlayout 并入 ui 域。定义文件暂不改名，由宏兜住。 */
#define jlayout_grid_create        joy_ui_layout_grid_create
#define jlayout_grid_destroy       joy_ui_layout_grid_destroy
#define jlayout_grid_next_row      joy_ui_layout_grid_next_row
#define jlayout_grid_set_row       joy_ui_layout_grid_set_row
#define jlayout_grid_cell          joy_ui_layout_grid_cell
#define jlayout_grid_label         joy_ui_layout_grid_label
#define jlayout_grid_button_row    joy_ui_layout_grid_button_row
#define jui_process_event          joy_ui_process_widget_event
#define jui_ctx_create             joy_ui_ctx_create
#define jui_ctx_destroy            joy_ui_ctx_destroy
#define jui_set_theme              joy_ui_set_theme
#define jui_begin                  joy_ui_begin
#define jui_end                    joy_ui_end
#define jui_input_motion           joy_ui_input_motion
#define jui_input_down             joy_ui_input_down
#define jui_input_up               joy_ui_input_up
#define jui_input_wheel            joy_ui_input_wheel
#define jui_input_key              joy_ui_input_key
#define jui_get_mouse              joy_ui_get_mouse
#define jui_is_down                joy_ui_is_down
#define jui_is_pressed             joy_ui_is_pressed
#define jui_is_released            joy_ui_is_released
#define jui_take_wheel             joy_ui_take_wheel
#define jui_is_shift               joy_ui_is_shift
#define jui_is_ctrl                joy_ui_is_ctrl
#define jui_push_clip              joy_ui_push_clip
#define jui_pop_clip               joy_ui_pop_clip
#define jui_reset_clip             joy_ui_reset_clip
#define jui_hit                    joy_ui_hit
#define jui_fill_rect              joy_ui_fill_rect
#define jui_stroke_rect            joy_ui_stroke_rect
#define jui_line                   joy_ui_line
#define jui_text                   joy_ui_text
#define jui_text_measure           joy_ui_text_measure
#define jui_text_centered          joy_ui_text_centered
#define jui_panel_begin            joy_ui_panel_begin
#define jui_panel_end              joy_ui_panel_end
#define jui_panel_frame            joy_ui_panel_frame
#define jui_scroll_begin           joy_ui_scroll_begin
#define jui_scroll_end             joy_ui_scroll_end
#define jui_scrollbar              joy_ui_scrollbar
#define jui_scroll_set_offset      joy_ui_scroll_set_offset
#define jui_button                 joy_ui_button
#define jui_checkbox               joy_ui_checkbox
#define jui_slider                 joy_ui_slider

/* P1 第六批：ui-A2，基础控件并入 ui 域。定义文件暂不改名，由宏兜住。 */
#define image_create               joy_ui_image_create
#define image_create_from_data     joy_ui_image_create_from_data
#define image_create_empty         joy_ui_image_create_empty
#define image_destroy              joy_ui_image_destroy
#define image_draw                 joy_ui_image_draw
#define image_draw1                joy_ui_image_draw1
#define image_draw_ex              joy_ui_image_draw_ex
#define image_set_alpha            joy_ui_image_set_alpha
#define image_set_blend_mode       joy_ui_image_set_blend_mode
#define image_get_size             joy_ui_image_get_size
#define image_update               joy_ui_image_update
#define image_set_scale_mode       joy_ui_image_set_scale_mode
#define image_draw1_f              joy_ui_image_draw1_f
#define image_draw_rect            joy_ui_image_draw_rect
#define text_createx               joy_ui_text_createx
#define text_create                joy_ui_text_create
#define text_updatex               joy_ui_text_updatex
#define text_update                joy_ui_text_update
#define text_destroy               joy_ui_text_destroy
#define text_get_width             joy_ui_text_get_width
#define text_get_height            joy_ui_text_get_height
#define label_create               joy_ui_label_create
#define label_destroy              joy_ui_label_destroy
#define label_set_text             joy_ui_label_set_text
#define label_draw                 joy_ui_label_draw
#define label_process_event        joy_ui_label_process_event
#define button_create              joy_ui_button_create
#define button_destroy             joy_ui_button_destroy
#define button_set_position        joy_ui_button_set_position
#define button_get_position        joy_ui_button_get_position
#define button_set_size            joy_ui_button_set_size
#define button_set_normal_color    joy_ui_button_set_normal_color
#define button_set_hover_color     joy_ui_button_set_hover_color
#define button_set_pressed_color   joy_ui_button_set_pressed_color
#define button_set_image           joy_ui_button_set_image
#define button_set_pressed_image   joy_ui_button_set_pressed_image
#define button_set_textx           joy_ui_button_set_textx
#define button_set_text            joy_ui_button_set_text
#define button_is_hovered          joy_ui_button_is_hovered
#define button_is_pressed          joy_ui_button_is_pressed
#define button_set_hovered         joy_ui_button_set_hovered
#define button_set_pressed         joy_ui_button_set_pressed
#define button_get_pressed         joy_ui_button_get_pressed
#define button_set_text_offset     joy_ui_button_set_text_offset
#define button_draw                joy_ui_button_draw
#define button_process_event       joy_ui_button_process_event
#define radio_create               joy_ui_radio_create
#define radio_destroy              joy_ui_radio_destroy
#define radio_is_pressed           joy_ui_radio_is_pressed
#define radio_set_hovered          joy_ui_radio_set_hovered
#define radio_process_event        joy_ui_radio_process_event
#define checkbox_create            joy_ui_checkbox_create
#define checkbox_destroy           joy_ui_checkbox_destroy
#define checkbox_set_state         joy_ui_checkbox_set_state
#define checkbox_get_state         joy_ui_checkbox_get_state
#define checkbox_set_colors        joy_ui_checkbox_set_colors
#define checkbox_set_r2d           joy_ui_checkbox_set_r2d
#define checkbox_set_position      joy_ui_checkbox_set_position
#define checkbox_set_text_color    joy_ui_checkbox_set_text_color
#define checkbox_toggle            joy_ui_checkbox_toggle
#define checkbox_is_pressed        joy_ui_checkbox_is_pressed
#define checkbox_set_hovered       joy_ui_checkbox_set_hovered
#define checkbox_draw              joy_ui_checkbox_draw
#define checkbox_process_event     joy_ui_checkbox_process_event
#define popup_create               joy_ui_popup_create
#define popup_destroy              joy_ui_popup_destroy
#define popup_set_title            joy_ui_popup_set_title
#define popup_set_content          joy_ui_popup_set_content
#define popup_set_position         joy_ui_popup_set_position
#define popup_set_visible          joy_ui_popup_set_visible
#define popup_set_closeable        joy_ui_popup_set_closeable
#define popup_draw                 joy_ui_popup_draw
#define popup_is_close_clicked     joy_ui_popup_is_close_clicked
#define popup_is_title_clicked     joy_ui_popup_is_title_clicked
#define popup_drag_move            joy_ui_popup_drag_move
#define popup_is_inside            joy_ui_popup_is_inside
#define popup_process_event        joy_ui_popup_process_event
#define tooltip_create             joy_ui_tooltip_create
#define tooltip_destroy            joy_ui_tooltip_destroy
#define tooltip_set_text           joy_ui_tooltip_set_text
#define tooltip_set_position       joy_ui_tooltip_set_position
#define tooltip_set_visible        joy_ui_tooltip_set_visible
#define tooltip_draw               joy_ui_tooltip_draw
#define tooltip_process_event      joy_ui_tooltip_process_event
#endif

#endif /* JOY_COMPAT_H */
