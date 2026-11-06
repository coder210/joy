# joy 绑定 API 覆盖率报告

> 自动生成，勿手改：由 `api/gen_bindings.js` 从 `joy.h` + `api/joy.idl` 产出。
> **P0 阶段**：本文件与 `joy.idl` 只描述契约，不生成任何会被调用的胶水代码；
> 现有 `modules/*/bindings/` 未被改动，构建行为不变。

## 总览

| 项 | 数量 |
|---|---|
| `joy.h` 中 `JOY_API` 声明 | 935 |
| `joy.h` 中值类型 `typedef struct` | 48 |
| IDL 条目 | 935 |
| 默认开放（`status: on`） | 4 |
| 待人工确认 `owns` | 1 |
| orphan（`joy.h` 中已消失） | 0 |

| 级别 | 含义 | 数量 |
|---|---|---|
| L0 | 纯标量 + const char*，可直接生成 | 44 |
| L1 | 不透明指针 → 句柄，需所有权测试 | 538 |
| L2 | 结构体按值 → 展开成标量，需静态断言 | 74 |
| L3 | 复杂签名 → 必须手写 thunk，不自动生成 | 229 |
| X | 有意排除，不进绑定层 | 50 |

默认全部 `status: off`。「能力全覆盖」指每条 API 都有 IDL 条目，
不等于已对脚本开放；验证过一条才把 `status` 改成 `on`。

## 按域分布

| domain | L0 | L1 | L2 | L3 | X | 合计 |
|---|---|---|---|---|---|---|
| animation | 0 | 8 | 1 | 1 | 0 | 10 |
| audio | 6 | 14 | 0 | 0 | 0 | 20 |
| box2d | 0 | 0 | 0 | 11 | 0 | 11 |
| button | 0 | 14 | 4 | 2 | 0 | 20 |
| checkbox | 0 | 10 | 2 | 1 | 0 | 13 |
| clipboard | 2 | 0 | 0 | 0 | 0 | 2 |
| collision2d | 0 | 0 | 10 | 8 | 0 | 18 |
| collision2df | 0 | 0 | 5 | 14 | 0 | 19 |
| collision3d | 0 | 0 | 3 | 4 | 0 | 7 |
| collision3df | 0 | 0 | 3 | 5 | 0 | 8 |
| combobox | 0 | 23 | 1 | 2 | 0 | 26 |
| datagrid | 0 | 10 | 0 | 1 | 0 | 11 |
| dispatch | 0 | 0 | 1 | 0 | 0 | 1 |
| env | 1 | 4 | 0 | 0 | 0 | 5 |
| error | 1 | 0 | 0 | 0 | 0 | 1 |
| event | 0 | 2 | 0 | 0 | 0 | 2 |
| fnv1a | 1 | 0 | 0 | 0 | 0 | 1 |
| font | 0 | 5 | 0 | 0 | 0 | 5 |
| game | 0 | 8 | 0 | 0 | 0 | 8 |
| gjk2d | 0 | 0 | 0 | 6 | 0 | 6 |
| gjk3d | 0 | 0 | 1 | 6 | 0 | 7 |
| gjk3df | 0 | 0 | 1 | 7 | 0 | 8 |
| image | 0 | 7 | 1 | 5 | 0 | 13 |
| inputbox | 0 | 27 | 0 | 1 | 0 | 28 |
| iocp | 0 | 3 | 0 | 0 | 0 | 3 |
| janimator | 0 | 23 | 2 | 2 | 0 | 27 |
| jecs | 0 | 5 | 0 | 7 | 1 | 13 |
| jenkins | 1 | 0 | 0 | 0 | 0 | 1 |
| jlayout | 0 | 0 | 0 | 7 | 0 | 7 |
| jlight | 0 | 9 | 2 | 0 | 0 | 11 |
| jmsgpack | 0 | 0 | 0 | 22 | 0 | 22 |
| joy | 0 | 0 | 0 | 6 | 0 | 6 |
| joystick | 0 | 14 | 0 | 3 | 0 | 17 |
| jparticle | 0 | 8 | 1 | 1 | 0 | 10 |
| jpm | 1 | 1 | 0 | 6 | 0 | 8 |
| jspine | 0 | 0 | 0 | 0 | 19 | 19 |
| jtext | 4 | 1 | 3 | 1 | 0 | 9 |
| jtilemap | 0 | 16 | 0 | 6 | 0 | 22 |
| jtimer | 0 | 6 | 0 | 1 | 0 | 7 |
| jui | 0 | 20 | 12 | 6 | 0 | 38 |
| kcpclient | 0 | 5 | 0 | 1 | 0 | 6 |
| kcpserver | 0 | 8 | 0 | 1 | 0 | 9 |
| kr | 1 | 0 | 0 | 0 | 0 | 1 |
| label | 0 | 3 | 0 | 2 | 0 | 5 |
| lcg | 0 | 4 | 0 | 1 | 0 | 5 |
| local | 0 | 0 | 0 | 1 | 0 | 1 |
| log | 1 | 0 | 0 | 3 | 0 | 4 |
| min | 0 | 0 | 0 | 1 | 0 | 1 |
| module | 0 | 0 | 0 | 0 | 5 | 5 |
| modules | 0 | 0 | 0 | 0 | 5 | 5 |
| moudles | 0 | 0 | 0 | 1 | 0 | 1 |
| mouse | 0 | 1 | 0 | 1 | 0 | 2 |
| murmur3 | 1 | 0 | 0 | 0 | 0 | 1 |
| netclient | 0 | 5 | 0 | 2 | 0 | 7 |
| netmessage | 0 | 1 | 0 | 0 | 0 | 1 |
| netserver | 0 | 8 | 0 | 2 | 0 | 10 |
| pack | 0 | 0 | 0 | 6 | 0 | 6 |
| popup | 0 | 13 | 0 | 0 | 0 | 13 |
| push | 2 | 0 | 0 | 0 | 0 | 2 |
| quit | 1 | 0 | 0 | 0 | 0 | 1 |
| r2d | 0 | 19 | 2 | 10 | 0 | 31 |
| r3d | 0 | 100 | 12 | 23 | 0 | 135 |
| radio | 0 | 5 | 0 | 0 | 0 | 5 |
| raw | 0 | 1 | 0 | 0 | 0 | 1 |
| scene | 0 | 50 | 0 | 15 | 0 | 65 |
| scrollview | 0 | 14 | 0 | 0 | 0 | 14 |
| sdbm | 1 | 0 | 0 | 0 | 0 | 1 |
| shape | 0 | 3 | 4 | 1 | 0 | 8 |
| should | 0 | 1 | 0 | 0 | 0 | 1 |
| simple | 0 | 4 | 0 | 0 | 0 | 4 |
| slider | 0 | 13 | 1 | 0 | 0 | 14 |
| sys | 19 | 0 | 0 | 3 | 0 | 22 |
| tcpclient | 0 | 5 | 0 | 0 | 0 | 5 |
| tcpserver | 0 | 8 | 0 | 0 | 0 | 8 |
| text | 0 | 3 | 2 | 2 | 0 | 7 |
| ticks | 1 | 0 | 0 | 0 | 0 | 1 |
| tooltip | 0 | 7 | 0 | 0 | 0 | 7 |
| unpack | 0 | 0 | 0 | 6 | 0 | 6 |
| user | 0 | 1 | 0 | 0 | 0 | 1 |
| utf8 | 0 | 0 | 0 | 3 | 0 | 3 |
| utils | 0 | 0 | 0 | 0 | 20 | 20 |
| window | 0 | 5 | 0 | 0 | 0 | 5 |
| wsnetclient | 0 | 5 | 0 | 1 | 0 | 6 |
| wsnetserver | 0 | 8 | 0 | 1 | 0 | 9 |

## 待办：返回句柄但缺 `owns`

这些函数返回 `*_p` / `void*`，却没推断出配对的析构函数。
`owns` 是**唯一无法自动发现**的语义，必须人工核对后填进 IDL。

| 函数 | 返回 | 位置 |
|---|---|---|
| `simple_fps_create` | `simple_fps_counter_p` | joy.h:1021 |

## L3 清单（必须手写 thunk）

| 函数 | 原因 | 位置 |
|---|---|---|
| `lcg_random_get_state` | 非不透明指针 / 输出参数：uint64_t* | joy.h:189 |
| `jpm_config_default` | 非不透明指针 / 输出参数：jpm_config_t* | joy.h:245 |
| `jpm_generate_seed` | 非不透明指针 / 输出参数：jpm_config_t* | joy.h:251 |
| `jpm_generate` | 非不透明指针 / 输出参数：jpm_config_t* | joy.h:253 |
| `jpm_rasterize` | 数组参数 | joy.h:255 |
| `jpm_bfs` | 数组参数 | joy.h:258 |
| `jpm_sample_open_tiles` | 非不透明指针 / 输出参数：jpm_config_t* | joy.h:261 |
| `utf8_decode` | 非不透明指针 / 输出参数：int32_t* | joy.h:936 |
| `utf8_next` | 非不透明指针 / 输出参数：size_t* | joy.h:946 |
| `utf8_decode_all` | 非不透明指针 / 输出参数：int32_t* | joy.h:958 |
| `pack_int8` | 非 const char* 输出缓冲 | joy.h:960 |
| `pack_int16` | 非 const char* 输出缓冲 | joy.h:961 |
| `pack_int32` | 非 const char* 输出缓冲 | joy.h:962 |
| `pack_uint32` | 非 const char* 输出缓冲 | joy.h:963 |
| `pack_int64` | 非 const char* 输出缓冲 | joy.h:964 |
| `unpack_int8` | 非不透明指针 / 输出参数：int8_t* | joy.h:965 |
| `unpack_int16` | 非不透明指针 / 输出参数：int16_t* | joy.h:966 |
| `unpack_int32` | 非不透明指针 / 输出参数：int32_t* | joy.h:967 |
| `unpack_uint32` | 非不透明指针 / 输出参数：uint32_t* | joy.h:968 |
| `unpack_int64` | 非不透明指针 / 输出参数：int64_t* | joy.h:969 |
| `pack_string` | 非 const char* 输出缓冲 | joy.h:970 |
| `unpack_string` | 非 const char* 输出缓冲 | joy.h:971 |
| `jtimer_create` | 未知类型需人工标注：jtimer_callback | joy.h:973 |
| `get_mouse_pos` | 非不透明指针 / 输出参数：float* | joy.h:991 |
| `log_info` | 可变参数 | joy.h:1011 |
| `log_debug` | 可变参数 | joy.h:1012 |
| `log_error` | 可变参数 | joy.h:1013 |
| `get_local_ips` | 数组参数 | joy.h:1043 |
| `sys_accept` | 非 const char* 输出缓冲 | joy.h:1057 |
| `sys_recv` | 非 const char* 输出缓冲 | joy.h:1059 |
| `sys_recvfrom` | 非 const char* 输出缓冲 | joy.h:1060 |
| `r2d_coordinates_from_window` | 结构体按值返回 | joy.h:1091 |
| `r2d_get_color` | 非不透明指针 / 输出参数：uint8_t* | joy.h:1094 |
| `r2d_get_logical_presentation` | 非不透明指针 / 输出参数：int* | joy.h:1100 |
| `r2d_get_size` | 非不透明指针 / 输出参数：int* | joy.h:1101 |
| `r2d_get_colorf` | 非不透明指针 / 输出参数：float* | joy.h:1105 |
| `r2d_draw_points` | 结构体指针参数 | joy.h:1108 |
| `r2d_get_clip_rect` | 结构体指针参数 | joy.h:1116 |
| `r2d_draw_lines` | 结构体指针参数 | joy.h:1125 |
| `shape_draw_polygon` | 结构体指针参数 | joy.h:1127 |
| `image_draw1` | 结构体指针参数 | joy.h:1138 |
| `image_draw_ex` | 结构体指针参数 | joy.h:1139 |
| `image_get_size` | 非不透明指针 / 输出参数：float* | joy.h:1142 |
| `image_update` | 结构体指针参数 | joy.h:1143 |
| `text_create` | 非不透明指针 / 输出参数：int* | joy.h:1148 |
| `text_update` | 非不透明指针 / 输出参数：int* | joy.h:1150 |
| `r2d_frect` | 结构体按值返回 | joy.h:1165 |
| `r2d_fpx` | 非不透明指针 / 输出参数：float* | joy.h:1166 |
| `image_draw1_f` | 结构体指针参数 | joy.h:1173 |
| `jtext_measurex` | 非不透明指针 / 输出参数：int* | joy.h:1200 |
| `animation_draw` | 结构体指针参数 | joy.h:1213 |
| `r3d_create_solid_texture` | 未知类型需人工标注：unsigned char | joy.h:1226 |
| `r3d_mat44_to_float16` | 数组参数 | joy.h:1230 |
| `r3d_load_texture_png` | 非不透明指针 / 输出参数：int* | joy.h:1275 |
| `r3d_font_set_color` | 未知类型需人工标注：Uint8 | joy.h:1287 |
| `r3d_font_draw_2d_color` | 未知类型需人工标注：Uint8 | joy.h:1302 |
| `r3d_font_draw_2d_outline` | 未知类型需人工标注：Uint8 | joy.h:1307 |
| `r3d_get_window_size` | 非不透明指针 / 输出参数：int* | joy.h:1314 |
| `r3d_world_to_screen` | 非不透明指针 / 输出参数：float* | joy.h:1332 |
| `r3d_get_logical_presentation` | 非不透明指针 / 输出参数：int* | joy.h:1344 |
| `r3d_window_to_logical` | 非不透明指针 / 输出参数：float* | joy.h:1346 |
| `r3d_logical_to_window` | 非不透明指针 / 输出参数：float* | joy.h:1348 |
| `r3d_button_set_onclick` | 函数指针 / 回调 | joy.h:1379 |
| `r3d_draw_rect_2d` | 未知类型需人工标注：Uint8 | joy.h:1386 |
| `r3d_draw_mesh_2d` | 未知类型需人工标注：Uint8 | joy.h:1392 |
| `r3d_input_handle_event` | 非不透明指针 / 输出参数：SDL_Event* | joy.h:1470 |
| `r3d_input_set_onsubmit` | 函数指针 / 回调 | joy.h:1472 |
| `r3d_combobox_input_handle_event` | 非不透明指针 / 输出参数：SDL_Event* | joy.h:1512 |
| `r3d_checkbox_set_state` | 未知类型需人工标注：jr3d_checkbox_state_t | joy.h:1583 |
| `r3d_draw_mesh_solid_basis` | 结构体指针参数 | joy.h:1608 |
| `r3d_draw_wire_basis` | 结构体指针参数 | joy.h:1614 |
| `r3d_set_light_block` | 结构体指针参数 | joy.h:1633 |
| `r3d_draw_mesh_lit` | 结构体指针参数 | joy.h:1635 |
| `r3d_draw_mesh_lit_color` | 结构体指针参数 | joy.h:1636 |
| `joy_fov_ray_dist` | 非不透明指针 / 输出参数：joy_fov_rect* | joy.h:1649 |
| `joy_fov_compute_radii` | 非不透明指针 / 输出参数：joy_fov_rect* | joy.h:1653 |
| `joy_fov_draw` | 非不透明指针 / 输出参数：float* | joy.h:1659 |
| `joy_fov_draw_2d` | 非不透明指针 / 输出参数：float* | joy.h:1666 |
| `collision2d_get_ray_circle` | 结构体按值返回 | joy.h:1674 |
| `collision2d_get_ray_rectangle` | 结构体按值返回 | joy.h:1675 |
| `collision2d_get_ray_rectanglex` | 结构体按值返回 | joy.h:1676 |
| `collision2d_get_ray_polygon` | 结构体按值返回 | joy.h:1677 |
| `collision2d_check_lines` | 结构体指针参数 | joy.h:1682 |
| `collision2d_get_circles` | 结构体指针参数 | joy.h:1689 |
| `collision2d_get_rectangles` | 结构体指针参数 | joy.h:1690 |
| `collision2d_get_polygons` | 结构体指针参数 | joy.h:1691 |
| `collision3d_get_ray_triangle` | 结构体按值返回 | joy.h:1692 |
| `collision3d_get_ray_quad` | 结构体按值返回 | joy.h:1693 |
| `collision3d_get_ray_sphere` | 结构体按值返回 | joy.h:1694 |
| `collision3d_get_ray_box` | 结构体按值返回 | joy.h:1695 |
| `collision2df_get_ray_circle` | 结构体按值返回 | joy.h:1700 |
| `collision2df_get_ray_rectangle` | 结构体按值返回 | joy.h:1702 |
| `collision2df_get_ray_rectanglex` | 结构体按值返回 | joy.h:1704 |
| `collision2df_get_ray_polygon` | 结构体按值返回 | joy.h:1706 |
| `collision2df_check_point_rectangle` | 未知类型需人工标注：vec2f_t | joy.h:1708 |
| `collision2df_check_point_circle` | 未知类型需人工标注：vec2f_t | joy.h:1710 |
| `collision2df_get_squared_distance_point_circle` | 未知类型需人工标注：vec2f_t | joy.h:1711 |
| `collision2df_check_point_triangle` | 未知类型需人工标注：vec2f_t | joy.h:1712 |
| `collision2df_check_lines` | 未知类型需人工标注：vec2f_t | joy.h:1714 |
| `collision2df_check_point_line` | 未知类型需人工标注：vec2f_t | joy.h:1716 |
| `collision2df_get_circles` | 结构体指针参数 | joy.h:1723 |
| `collision2df_get_rectangles` | 结构体指针参数 | joy.h:1725 |
| `find_min_separation` | 非不透明指针 / 输出参数：vec2f_t* | joy.h:1728 |
| `collision2df_get_polygons` | 结构体指针参数 | joy.h:1730 |
| `collision2df_swept_obb` | 未知类型需人工标注：vec2f_t | joy.h:1752 |
| `joy_tilemap_sweep_x` | 非不透明指针 / 输出参数：fp_t* | joy.h:1762 |
| `joy_tilemap_sweep_y` | 非不透明指针 / 输出参数：fp_t* | joy.h:1764 |
| `collision3df_get_ray_triangle` | 结构体按值返回 | joy.h:1767 |
| `collision3df_get_ray_quad` | 结构体按值返回 | joy.h:1769 |
| `collision3df_get_ray_sphere` | 结构体按值返回 | joy.h:1771 |
| `collision3df_get_ray_box` | 结构体按值返回 | joy.h:1773 |
| `collision3df_get_ray_obb` | 结构体按值返回 | joy.h:1776 |
| `gjk2d_collide` | 未知类型需人工标注：vec2f_t | joy.h:1786 |
| `gjk2d_init_circle` | 非不透明指针 / 输出参数：gjk2d_collider_t* | joy.h:1788 |
| `gjk2d_init_box` | 非不透明指针 / 输出参数：gjk2d_collider_t* | joy.h:1789 |
| `gjk2d_init_capsule` | 非不透明指针 / 输出参数：gjk2d_collider_t* | joy.h:1790 |
| `gjk2d_init_polygon` | 非不透明指针 / 输出参数：gjk2d_collider_t* | joy.h:1791 |
| `gjk2d_init_pie` | 非不透明指针 / 输出参数：gjk2d_collider_t* | joy.h:1792 |
| `gjk3df_init_sphere` | 非不透明指针 / 输出参数：gjk3df_collider_t* | joy.h:1796 |
| `gjk3df_init_box` | 非不透明指针 / 输出参数：gjk3df_collider_t* | joy.h:1797 |
| `gjk3df_init_obb` | 非不透明指针 / 输出参数：gjk3df_collider_t* | joy.h:1798 |
| `gjk3df_init_capsule` | 非不透明指针 / 输出参数：gjk3df_collider_t* | joy.h:1799 |
| `gjk3df_init_cylinder` | 非不透明指针 / 输出参数：gjk3df_collider_t* | joy.h:1800 |
| `gjk3df_init_cone` | 非不透明指针 / 输出参数：gjk3df_collider_t* | joy.h:1801 |
| `gjk3df_init_mesh` | 非不透明指针 / 输出参数：gjk3df_collider_t* | joy.h:1802 |
| `gjk3d_init_sphere` | 非不透明指针 / 输出参数：gjk3d_collider_t* | joy.h:1809 |
| `gjk3d_init_box` | 非不透明指针 / 输出参数：gjk3d_collider_t* | joy.h:1810 |
| `gjk3d_init_capsule` | 非不透明指针 / 输出参数：gjk3d_collider_t* | joy.h:1811 |
| `gjk3d_init_cylinder` | 非不透明指针 / 输出参数：gjk3d_collider_t* | joy.h:1812 |
| `gjk3d_init_cone` | 非不透明指针 / 输出参数：gjk3d_collider_t* | joy.h:1813 |
| `gjk3d_init_mesh` | 非不透明指针 / 输出参数：gjk3d_collider_t* | joy.h:1814 |
| `janimator_set_on_enter` | 未知类型需人工标注：janim_state_cb | joy.h:1845 |
| `janimator_set_on_exit` | 未知类型需人工标注：janim_state_cb | joy.h:1846 |
| `label_create` | 非不透明指针 / 输出参数：int* | joy.h:1920 |
| `label_set_text` | 非不透明指针 / 输出参数：int* | joy.h:1922 |
| `button_get_position` | 结构体按值返回 | joy.h:1928 |
| `button_set_text` | 非不透明指针 / 输出参数：int* | joy.h:1937 |
| `combobox_set_text` | 非不透明指针 / 输出参数：int* | joy.h:1987 |
| `combobox_add_itemx` | 非不透明指针 / 输出参数：int* | joy.h:1991 |
| `jlayout_grid_create` | 非不透明指针 / 输出参数：float* | joy.h:2008 |
| `jlayout_grid_destroy` | 非不透明指针 / 输出参数：jlayout_grid_t* | joy.h:2010 |
| `jlayout_grid_next_row` | 非不透明指针 / 输出参数：jlayout_grid_t* | joy.h:2011 |
| `jlayout_grid_set_row` | 非不透明指针 / 输出参数：jlayout_grid_t* | joy.h:2012 |
| `jlayout_grid_cell` | 非不透明指针 / 输出参数：jlayout_grid_t* | joy.h:2014 |
| `jlayout_grid_label` | 非不透明指针 / 输出参数：jlayout_grid_t* | joy.h:2016 |
| `jlayout_grid_button_row` | 非不透明指针 / 输出参数：jlayout_grid_t* | joy.h:2021 |
| `datagrid_addrow` | 非不透明指针 / 输出参数：char** | joy.h:2043 |
| `checkbox_set_state` | 未知类型需人工标注：checkbox_state_t | joy.h:2058 |
| `joystick_set_dir_mode` | 未知类型需人工标注：joystick_dir_mode | joy.h:2082 |
| `joystick_set_textures_for_mode` | 未知类型需人工标注：joystick_dir_mode | joy.h:2084 |
| `joystick_get_direction` | 结构体按值返回 | joy.h:2094 |
| `inputbox_set_callback` | 函数指针 / 回调 | joy.h:2101 |
| `jui_process_event` | 非不透明指针 / 输出参数：void** | joy.h:2162 |
| `jui_set_theme` | 结构体指针参数 | joy.h:2192 |
| `jui_get_mouse` | 非不透明指针 / 输出参数：float* | joy.h:2206 |
| `jui_text_measure` | 非不透明指针 / 输出参数：int* | joy.h:2226 |
| `jui_checkbox` | 非不透明指针 / 输出参数：bool* | joy.h:2243 |
| `jui_slider` | 非不透明指针 / 输出参数：float* | joy.h:2245 |
| `jtilemap_resolve_gid` | 结构体按值返回 | joy.h:2253 |
| `jtilemap_get_tile_def_size` | 非不透明指针 / 输出参数：int* | joy.h:2255 |
| `jtilemap_get_tile_uv` | 数组参数 | joy.h:2256 |
| `jtilemap_object_layer_foreach` | 函数指针 / 回调 | joy.h:2263 |
| `jtilemap_object_layer_foreach_all` | 函数指针 / 回调 | joy.h:2264 |
| `jtilemap_render_create` | 未知类型需人工标注：jtilemap_path_remap_t | joy.h:2267 |
| `kcpserver_set_callback` | 未知类型需人工标注：net_callback | joy.h:2297 |
| `kcpclient_set_callback` | 未知类型需人工标注：net_callback | joy.h:2303 |
| `wsnetserver_set_callback` | 未知类型需人工标注：net_callback | joy.h:2325 |
| `wsnetclient_set_callback` | 未知类型需人工标注：net_callback | joy.h:2331 |
| `netclient_create` | 未知类型需人工标注：net_client_type | joy.h:2332 |
| `netclient_set_callback` | 未知类型需人工标注：net_callback | joy.h:2337 |
| `netserver_create` | 未知类型需人工标注：net_server_type | joy.h:2339 |
| `netserver_set_callback` | 未知类型需人工标注：net_callback | joy.h:2347 |
| `scene_node_get_position` | 非不透明指针 / 输出参数：float* | joy.h:2361 |
| `scene_node_get_world_position` | 非不透明指针 / 输出参数：float* | joy.h:2363 |
| `scene_node_get_scale` | 非不透明指针 / 输出参数：float* | joy.h:2367 |
| `scene_find_nodes_by_tag` | 非不透明指针 / 输出参数：scene_node_p* | joy.h:2378 |
| `scene_find_nodes_by_tag_deep` | 非不透明指针 / 输出参数：scene_node_p* | joy.h:2379 |
| `scene_node_set_update_callback` | 未知类型需人工标注：scene_node_update_fn | joy.h:2380 |
| `scene_node_set_render_callback` | 未知类型需人工标注：scene_node_render_fn | joy.h:2381 |
| `scene_node_set_destroy_callback` | 未知类型需人工标注：scene_node_destroy_fn | joy.h:2382 |
| `scene_node_set_event_callback` | 未知类型需人工标注：scene_node_event_fn | joy.h:2383 |
| `scene_set_load_callback` | 未知类型需人工标注：scene_load_fn | joy.h:2396 |
| `scene_set_update_callback` | 未知类型需人工标注：scene_update_fn | joy.h:2397 |
| `scene_set_render_callback` | 未知类型需人工标注：scene_render_fn | joy.h:2398 |
| `scene_set_destroy_callback` | 未知类型需人工标注：scene_destroy_fn | joy.h:2399 |
| `scene_set_event_callback` | 未知类型需人工标注：scene_event_fn | joy.h:2400 |
| `scene_set_fixed_update_callback` | 未知类型需人工标注：scene_fixed_update_fn | joy.h:2401 |
| `jparticle_cfg_default` | 结构体按值返回 | joy.h:2419 |
| `jecs_kill` | 未知类型需人工标注：jecs_id_t | joy.h:2449 |
| `jecs_set` | 未知类型需人工标注：jecs_id_t | joy.h:2451 |
| `jecs_get` | 未知类型需人工标注：jecs_id_t | joy.h:2452 |
| `jecs_remove` | 未知类型需人工标注：jecs_id_t | joy.h:2453 |
| `jecs_query` | 可变参数 | joy.h:2454 |
| `jecs_queryx` | 非不透明指针 / 输出参数：char** | joy.h:2455 |
| `jecs_register` | 未知类型需人工标注：jecs_system_t | joy.h:2456 |
| `moudles_retire` | 非不透明指针 / 输出参数：module_p* | joy.h:2522 |
| `box2d_create_world` | 未知类型需人工标注：vec2f_t | joy.h:2530 |
| `box2d_clear_world` | 非不透明指针 / 输出参数：box2d_world_t* | joy.h:2531 |
| `box2d_destroy_world` | 非不透明指针 / 输出参数：box2d_world_t* | joy.h:2532 |
| `box2d_step` | 非不透明指针 / 输出参数：box2d_world_t* | joy.h:2533 |
| `box2d_create_body` | 非不透明指针 / 输出参数：box2d_world_t* | joy.h:2534 |
| `box2d_destroy_body` | 非不透明指针 / 输出参数：box2d_world_t* | joy.h:2535 |
| `box2d_create_joint` | 非不透明指针 / 输出参数：box2d_world_t* | joy.h:2536 |
| `box2d_destroy_joint` | 非不透明指针 / 输出参数：box2d_world_t* | joy.h:2537 |
| `box2d_foreach_bodies` | 函数指针 / 回调 | joy.h:2538 |
| `box2d_foreach_joints` | 函数指针 / 回调 | joy.h:2539 |
| `box2d_foreach_arbiters` | 函数指针 / 回调 | joy.h:2540 |
| `jmsgpack_packer_init` | 结构体指针参数 | joy.h:2542 |
| `jmsgpack_pack_int` | 结构体指针参数 | joy.h:2543 |
| `jmsgpack_pack_int64` | 结构体指针参数 | joy.h:2544 |
| `jmsgpack_pack_uint` | 结构体指针参数 | joy.h:2545 |
| `jmsgpack_pack_uint64` | 结构体指针参数 | joy.h:2546 |
| `jmsgpack_pack_nil` | 结构体指针参数 | joy.h:2547 |
| `jmsgpack_pack_array` | 结构体指针参数 | joy.h:2548 |
| `jmsgpack_pack_str` | 结构体指针参数 | joy.h:2549 |
| `jmsgpack_pack_str_body` | 结构体指针参数 | joy.h:2550 |
| `jmsgpack_unpacked_init` | 结构体指针参数 | joy.h:2551 |
| `jmsgpack_unpacked_destroy` | 结构体指针参数 | joy.h:2552 |
| `jmsgpack_unpack_next` | 结构体指针参数 | joy.h:2554 |
| `jmsgpack_unpacked_data` | 结构体指针参数 | joy.h:2557 |
| `jmsgpack_object_type` | 非不透明指针 / 输出参数：jmsgpack_object_t* | joy.h:2558 |
| `jmsgpack_object_array_size` | 非不透明指针 / 输出参数：jmsgpack_object_t* | joy.h:2559 |
| `jmsgpack_object_array_at` | 非不透明指针 / 输出参数：jmsgpack_object_t* | joy.h:2560 |
| `jmsgpack_object_str_size` | 非不透明指针 / 输出参数：jmsgpack_object_t* | joy.h:2561 |
| `jmsgpack_object_str_ptr` | 非不透明指针 / 输出参数：jmsgpack_object_t* | joy.h:2562 |
| `jmsgpack_object_u64` | 非不透明指针 / 输出参数：jmsgpack_object_t* | joy.h:2563 |
| `jmsgpack_object_i64` | 非不透明指针 / 输出参数：jmsgpack_object_t* | joy.h:2564 |
| `jmsgpack_object_boolean` | 非不透明指针 / 输出参数：jmsgpack_object_t* | joy.h:2565 |
| `jmsgpack_object_f64` | 非不透明指针 / 输出参数：jmsgpack_object_t* | joy.h:2566 |

## 有意排除（X）

| 函数 | 位置 |
|---|---|
| `utils_random_create` | joy.h:171 |
| `utils_random_destroy` | joy.h:172 |
| `utils_random_next` | joy.h:173 |
| `utils_random_get_state` | joy.h:176 |
| `utils_random_set_state` | joy.h:177 |
| `utils_strdup` | joy.h:905 |
| `utils_get_current_time` | joy.h:906 |
| `utils_current_datetime` | joy.h:907 |
| `utils_read_file` | joy.h:908 |
| `utils_check_file` | joy.h:909 |
| `utils_append_file` | joy.h:910 |
| `utils_read_script` | joy.h:911 |
| `utils_bit2short` | joy.h:912 |
| `utils_short2bit` | joy.h:913 |
| `utils_bit2int` | joy.h:915 |
| `utils_int2bit` | joy.h:917 |
| `utils_wait_delay` | joy.h:918 |
| `utils_md5x` | joy.h:919 |
| `utils_des_crypt` | joy.h:920 |
| `utils_des_decrypt` | joy.h:921 |
| `jspine_asset_load` | joy.h:1884 |
| `jspine_asset_free` | joy.h:1886 |
| `jspine_asset_animation_count` | joy.h:1888 |
| `jspine_asset_animation_duration` | joy.h:1890 |
| `jspine_instance_init` | joy.h:1893 |
| `jspine_instance_free` | joy.h:1894 |
| `jspine_instance_reset` | joy.h:1895 |
| `jspine_instance_set_animation` | joy.h:1896 |
| `jspine_instance_add_animation` | joy.h:1898 |
| `jspine_instance_update` | joy.h:1902 |
| `jspine_instance_sync` | joy.h:1903 |
| `jspine_instance_draw` | joy.h:1904 |
| `jspine_instance_set_position` | joy.h:1907 |
| `jspine_instance_set_scale` | joy.h:1908 |
| `jspine_instance_set_rotation` | joy.h:1909 |
| `jspine_instance_set_flip_x` | joy.h:1910 |
| `jspine_instance_set_alpha` | joy.h:1911 |
| `jspine_instance_set_tint` | joy.h:1912 |
| `jspine_instance_bounds` | joy.h:1915 |
| `jecs_test` | joy.h:2457 |
| `modules_create` | joy.h:2512 |
| `modules_destroy` | joy.h:2513 |
| `modules_query` | joy.h:2514 |
| `modules_register` | joy.h:2515 |
| `modules_require` | joy.h:2521 |
| `module_instance_create` | joy.h:2523 |
| `module_instance_init` | joy.h:2524 |
| `module_instance_event` | joy.h:2525 |
| `module_instance_update` | joy.h:2526 |
| `module_instance_release` | joy.h:2527 |

## `jmath.h`

`jmath.h` 是头文件内的 `JOY_INLINE` 值类型数学库，没有 `JOY_API` 声明，
因此不在上面的解析范围内，P1 需要单独的解析与「记录」映射。

| 项 | 数量（粗统计） |
|---|---|
| 行数 | 1756 |
| `JOY_INLINE` 函数 | 134 |
| `typedef struct` 值类型 | 10 |

## 三种常见操作

见 `api/README.md`。CI 用 `node api/gen_bindings.js --check` 保证本文件与 IDL 同步。
