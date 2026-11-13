# joy 绑定 API 覆盖率报告

> 自动生成，勿手改：由 `api/tools/gen_bindings.js` 从 `include/joy/{defs,api}.h` + `api/idl/joy.idl` 产出。
> 生成物（共享 thunk 与 cs 薄适配）覆盖 `status: on` 的 L0/L1，
> 以及按字段展开成标量序列的 L2 条目；
> 注册表同时携带参数 / 返回类型，供 lua / js / cs 三端 vm.c 共用；
> `modules/*/bindings/legacy/` 旧手写胶水已删除，IDL 驱动层是唯一入口。

## 总览

| 项 | 数量 |
|---|---|
| `joy.h` 中 `JOY_API` 声明 | 1170 |
| `joy.h` 中值类型 `typedef struct` | 47 |
| IDL 条目 | 1170 |
| 已开放（`status: on`） | 739 |
| 借用句柄（`borrowed`，不注册析构） | 12（已开放 7） |
| 待人工确认 `owns` | 11 |
| 所有权待裁决（类型不匹配 / 缺 `owns`） | 11 |
| orphan（`joy.h` 中已消失） | 0 |

| 级别 | 含义 | 数量 |
|---|---|---|
| L0 | 纯标量 + const char*，可直接生成 | 277 |
| L1 | 不透明指针 → 句柄，需所有权测试 | 547 |
| L2 | 结构体按值 → P2-A 试点按字段展开成标量序列 | 74 |
| L3 | 复杂签名 → 必须手写 thunk，不自动生成 | 199 |
| X | 有意排除，不进绑定层 | 73 |

开放由 `joy.idl` 的 `status` 控制。`--enable` 只放行通过类型、所有权与
缓冲校验的条目，被跳过的原因会在命令输出和下面各节列出；**开放 ≠ 业务已调用**。

## 按域分布

| domain | L0 | L1 | L2 | L3 | X | 合计 |
|---|---|---|---|---|---|---|
| audio | 6 | 14 | 0 | 0 | 0 | 20 |
| core | 27 | 37 | 1 | 21 | 20 | 106 |
| math | 220 | 0 | 23 | 64 | 0 | 307 |
| net | 19 | 56 | 0 | 12 | 0 | 87 |
| r2d | 4 | 55 | 14 | 22 | 19 | 114 |
| r3d | 0 | 117 | 14 | 19 | 0 | 150 |
| scene | 1 | 95 | 2 | 36 | 1 | 135 |
| script | 0 | 0 | 0 | 0 | 33 | 33 |
| ui | 0 | 173 | 20 | 25 | 0 | 218 |

## 待办：返回句柄但缺 `owns`

这些函数返回 `*_p` / `void*`，却没推断出配对的析构函数。
`owns` 是**唯一无法自动发现**的语义，必须人工核对后填进 IDL。

| 函数 | 返回 | 位置 | 人工裁决（`note`） |
|---|---|---|---|
| `joy_r3d_load_shader` | `void*` | src/joy_r3d.h:21 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_create_pipeline` | `void*` | src/joy_r3d.h:23 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_upload_buffer` | `void*` | src/joy_r3d.h:26 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_create_solid_texture` | `void*` | src/joy_r3d.h:27 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_create_depth_texture` | `void*` | src/joy_r3d.h:29 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_create_sampler` | `void*` | src/joy_r3d.h:30 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_create_texture` | `void*` | src/joy_r3d.h:73 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_load_texture_png` | `void*` | src/joy_r3d.h:76 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_gen_checkerboard` | `void*` | src/joy_r3d.h:77 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_create_line_pipeline` | `void*` | src/joy_r3d.h:417 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_create_lighting_pipeline` | `void*` | src/joy_r3d.h:424 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |

## 待办：所有权待裁决（`owns` 不可信）

返回句柄但没有可通过硬校验的 `owns`：借用指针、dtor 参数不匹配，
或压根没有合规的单参数 dtor。`note` 列是逐条人工核对后的所有权裁决。
批量开放会跳过它们；`--check` 也禁止把这类条目设成 `on`，
否则 dtor 会把错误类型的指针交给析构函数，运行时才崩。

| 函数 | 返回 | `owns` | 问题 | 人工裁决（`note`） |
|---|---|---|---|---|
| `joy_r3d_load_shader` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_create_pipeline` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_upload_buffer` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_create_solid_texture` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_create_depth_texture` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_create_sampler` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_create_texture` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_load_texture_png` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_gen_checkerboard` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_create_line_pipeline` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |
| `joy_r3d_create_lighting_pipeline` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h；返回 void* 被 canEmitCs / okHandle 排除，保持 off（B 不覆盖） |

## L3 清单（必须手写 thunk）

| 函数 | 原因 | 位置 |
|---|---|---|
| `joy_core_lcg_random_get_state` | 非不透明指针 / 输出参数：uint64_t* | include/joy/jdefs.h:183 |
| `joy_scene_procmap_config_default` | 非不透明指针 / 输出参数：joy_pm_config_t* | include/joy/jdefs.h:239 |
| `joy_scene_procmap_generate_seed` | 非不透明指针 / 输出参数：joy_pm_config_t* | include/joy/jdefs.h:245 |
| `joy_scene_procmap_generate` | 非不透明指针 / 输出参数：joy_pm_config_t* | include/joy/jdefs.h:247 |
| `joy_scene_procmap_rasterize` | 数组参数 | include/joy/jdefs.h:249 |
| `joy_scene_procmap_bfs` | 数组参数 | include/joy/jdefs.h:252 |
| `joy_scene_procmap_sample_open_tiles` | 非不透明指针 / 输出参数：joy_pm_config_t* | include/joy/jdefs.h:255 |
| `joy_core_utf8_decode` | 非不透明指针 / 输出参数：int32_t* | src/joy_core.h:51 |
| `joy_core_utf8_next` | 非不透明指针 / 输出参数：size_t* | src/joy_core.h:61 |
| `joy_core_utf8_decode_all` | 非不透明指针 / 输出参数：int32_t* | src/joy_core.h:73 |
| `joy_core_pack_int8` | 非 const char* 输出缓冲 | src/joy_core.h:75 |
| `joy_core_pack_int16` | 非 const char* 输出缓冲 | src/joy_core.h:76 |
| `joy_core_pack_int32` | 非 const char* 输出缓冲 | src/joy_core.h:77 |
| `joy_core_pack_uint32` | 非 const char* 输出缓冲 | src/joy_core.h:78 |
| `joy_core_pack_int64` | 非 const char* 输出缓冲 | src/joy_core.h:79 |
| `joy_core_unpack_int8` | 非不透明指针 / 输出参数：int8_t* | src/joy_core.h:80 |
| `joy_core_unpack_int16` | 非不透明指针 / 输出参数：int16_t* | src/joy_core.h:81 |
| `joy_core_unpack_int32` | 非不透明指针 / 输出参数：int32_t* | src/joy_core.h:82 |
| `joy_core_unpack_uint32` | 非不透明指针 / 输出参数：uint32_t* | src/joy_core.h:83 |
| `joy_core_unpack_int64` | 非不透明指针 / 输出参数：int64_t* | src/joy_core.h:84 |
| `joy_core_pack_string` | 非 const char* 输出缓冲 | src/joy_core.h:85 |
| `joy_core_unpack_string` | 非 const char* 输出缓冲 | src/joy_core.h:86 |
| `joy_core_timer_create` | 未知类型需人工标注：joy_timer_callback | src/joy_core.h:88 |
| `joy_core_get_mouse_pos` | 非不透明指针 / 输出参数：float* | src/joy_core.h:106 |
| `joy_core_log_info` | 可变参数 | src/joy_core.h:126 |
| `joy_core_log_debug` | 可变参数 | src/joy_core.h:127 |
| `joy_core_log_error` | 可变参数 | src/joy_core.h:128 |
| `joy_net_get_local_ips` | 数组参数 | src/joy_net.h:19 |
| `joy_net_sys_accept` | 非 const char* 输出缓冲 | src/joy_net.h:33 |
| `joy_net_sys_recv` | 非 const char* 输出缓冲 | src/joy_net.h:35 |
| `joy_net_sys_recvfrom` | 非 const char* 输出缓冲 | src/joy_net.h:36 |
| `joy_r2d_coordinates_from_window` | 结构体按值返回 | src/joy_r2d.h:36 |
| `joy_r2d_get_color` | 非不透明指针 / 输出参数：uint8_t* | src/joy_r2d.h:39 |
| `joy_r2d_get_logical_presentation` | 非不透明指针 / 输出参数：int* | src/joy_r2d.h:45 |
| `joy_r2d_get_size` | 非不透明指针 / 输出参数：int* | src/joy_r2d.h:46 |
| `joy_r2d_get_colorf` | 非不透明指针 / 输出参数：float* | src/joy_r2d.h:50 |
| `joy_r2d_draw_points` | 结构体指针参数 | src/joy_r2d.h:53 |
| `joy_r2d_get_clip_rect` | 结构体指针参数 | src/joy_r2d.h:61 |
| `joy_r2d_draw_lines` | 结构体指针参数 | src/joy_r2d.h:70 |
| `joy_r2d_shape_draw_polygon` | 结构体指针参数 | src/joy_r2d.h:72 |
| `joy_r2d_image_draw1` | 结构体指针参数 | src/joy_r2d.h:83 |
| `joy_r2d_image_draw_ex` | 结构体指针参数 | src/joy_r2d.h:84 |
| `joy_r2d_image_get_size` | 非不透明指针 / 输出参数：float* | src/joy_r2d.h:87 |
| `joy_r2d_image_update` | 结构体指针参数 | src/joy_r2d.h:88 |
| `joy_r2d_text_create` | 非不透明指针 / 输出参数：int* | src/joy_r2d.h:93 |
| `joy_r2d_text_update` | 非不透明指针 / 输出参数：int* | src/joy_r2d.h:95 |
| `joy_r2d_frect` | 结构体按值返回 | src/joy_r2d.h:110 |
| `joy_r2d_fpx` | 非不透明指针 / 输出参数：float* | src/joy_r2d.h:111 |
| `joy_r2d_image_draw1_f` | 结构体指针参数 | src/joy_r2d.h:118 |
| `joy_r2d_text_measurex` | 非不透明指针 / 输出参数：int* | src/joy_r2d.h:145 |
| `joy_r2d_animation_draw` | 结构体指针参数 | src/joy_r2d.h:158 |
| `joy_r3d_create_solid_texture` | 未知类型需人工标注：unsigned char | src/joy_r3d.h:27 |
| `joy_r3d_mat44_to_float16` | 数组参数 | src/joy_r3d.h:31 |
| `joy_r3d_load_texture_png` | 非不透明指针 / 输出参数：int* | src/joy_r3d.h:76 |
| `joy_r3d_get_window_size` | 非不透明指针 / 输出参数：int* | src/joy_r3d.h:115 |
| `joy_r3d_world_to_screen` | 非不透明指针 / 输出参数：float* | src/joy_r3d.h:133 |
| `joy_r3d_get_logical_presentation` | 非不透明指针 / 输出参数：int* | src/joy_r3d.h:145 |
| `joy_r3d_window_to_logical` | 非不透明指针 / 输出参数：float* | src/joy_r3d.h:147 |
| `joy_r3d_logical_to_window` | 非不透明指针 / 输出参数：float* | src/joy_r3d.h:149 |
| `joy_r3d_button_set_onclick` | 函数指针 / 回调 | src/joy_r3d.h:180 |
| `joy_r3d_input_set_onsubmit` | 函数指针 / 回调 | src/joy_r3d.h:273 |
| `joy_r3d_checkbox_set_state` | 未知类型需人工标注：joy_r3d_checkbox_state_t | src/joy_r3d.h:384 |
| `joy_r3d_draw_mesh_solid_basis` | 结构体指针参数 | src/joy_r3d.h:409 |
| `joy_r3d_draw_wire_basis` | 结构体指针参数 | src/joy_r3d.h:415 |
| `joy_r3d_set_light_block` | 结构体指针参数 | src/joy_r3d.h:434 |
| `joy_r3d_draw_mesh_lit` | 结构体指针参数 | src/joy_r3d.h:436 |
| `joy_r3d_draw_mesh_lit_color` | 结构体指针参数 | src/joy_r3d.h:437 |
| `joy_r3d_fov_ray_dist` | 非不透明指针 / 输出参数：joy_fov_rect* | src/joy_r3d.h:450 |
| `joy_r3d_fov_compute_radii` | 非不透明指针 / 输出参数：joy_fov_rect* | src/joy_r3d.h:454 |
| `joy_r3d_fov_draw` | 非不透明指针 / 输出参数：float* | src/joy_r3d.h:460 |
| `joy_r2d_fov_draw_2d` | 非不透明指针 / 输出参数：float* | src/joy_r2d.h:164 |
| `joy_math_collision2d_get_ray_circle` | 结构体按值返回 | src/joy_math.h:22 |
| `joy_math_collision2d_get_ray_rectangle` | 结构体按值返回 | src/joy_math.h:23 |
| `joy_math_collision2d_get_ray_rectanglex` | 结构体按值返回 | src/joy_math.h:24 |
| `joy_math_collision2d_get_ray_polygon` | 结构体按值返回 | src/joy_math.h:25 |
| `joy_math_collision2d_check_lines` | 结构体指针参数 | src/joy_math.h:30 |
| `joy_math_collision2d_get_circles` | 结构体指针参数 | src/joy_math.h:37 |
| `joy_math_collision2d_get_rectangles` | 结构体指针参数 | src/joy_math.h:38 |
| `joy_math_collision2d_get_polygons` | 结构体指针参数 | src/joy_math.h:39 |
| `joy_math_collision3d_get_ray_triangle` | 结构体按值返回 | src/joy_math.h:40 |
| `joy_math_collision3d_get_ray_quad` | 结构体按值返回 | src/joy_math.h:41 |
| `joy_math_collision3d_get_ray_sphere` | 结构体按值返回 | src/joy_math.h:42 |
| `joy_math_collision3d_get_ray_box` | 结构体按值返回 | src/joy_math.h:43 |
| `joy_math_collision2df_get_ray_circle` | 结构体按值返回 | src/joy_math.h:48 |
| `joy_math_collision2df_get_ray_rectangle` | 结构体按值返回 | src/joy_math.h:50 |
| `joy_math_collision2df_get_ray_rectanglex` | 结构体按值返回 | src/joy_math.h:52 |
| `joy_math_collision2df_get_ray_polygon` | 结构体按值返回 | src/joy_math.h:54 |
| `joy_math_collision2df_check_point_rectangle` | 未知类型需人工标注：joy_vec2f_t | src/joy_math.h:56 |
| `joy_math_collision2df_check_point_circle` | 未知类型需人工标注：joy_vec2f_t | src/joy_math.h:58 |
| `joy_math_collision2df_get_squared_distance_point_circle` | 未知类型需人工标注：joy_vec2f_t | src/joy_math.h:59 |
| `joy_math_collision2df_check_point_triangle` | 未知类型需人工标注：joy_vec2f_t | src/joy_math.h:60 |
| `joy_math_collision2df_check_lines` | 未知类型需人工标注：joy_vec2f_t | src/joy_math.h:62 |
| `joy_math_collision2df_check_point_line` | 未知类型需人工标注：joy_vec2f_t | src/joy_math.h:64 |
| `joy_math_collision2df_get_circles` | 结构体指针参数 | src/joy_math.h:71 |
| `joy_math_collision2df_get_rectangles` | 结构体指针参数 | src/joy_math.h:73 |
| `joy_math_find_min_separation` | 非不透明指针 / 输出参数：joy_vec2f_t* | src/joy_math.h:76 |
| `joy_math_collision2df_get_polygons` | 结构体指针参数 | src/joy_math.h:78 |
| `joy_math_collision2df_swept_obb` | 未知类型需人工标注：joy_vec2f_t | src/joy_math.h:100 |
| `joy_math_tilemap_sweep_x` | 非不透明指针 / 输出参数：joy_fp_t* | src/joy_math.h:110 |
| `joy_math_tilemap_sweep_y` | 非不透明指针 / 输出参数：joy_fp_t* | src/joy_math.h:112 |
| `joy_math_collision3df_get_ray_triangle` | 结构体按值返回 | src/joy_math.h:115 |
| `joy_math_collision3df_get_ray_quad` | 结构体按值返回 | src/joy_math.h:117 |
| `joy_math_collision3df_get_ray_sphere` | 结构体按值返回 | src/joy_math.h:119 |
| `joy_math_collision3df_get_ray_box` | 结构体按值返回 | src/joy_math.h:121 |
| `joy_math_collision3df_get_ray_obb` | 结构体按值返回 | src/joy_math.h:124 |
| `joy_math_gjk2d_collide` | 未知类型需人工标注：joy_vec2f_t | src/joy_math.h:134 |
| `joy_math_gjk2d_init_circle` | 非不透明指针 / 输出参数：joy_gjk2d_collider_t* | src/joy_math.h:136 |
| `joy_math_gjk2d_init_box` | 非不透明指针 / 输出参数：joy_gjk2d_collider_t* | src/joy_math.h:137 |
| `joy_math_gjk2d_init_capsule` | 非不透明指针 / 输出参数：joy_gjk2d_collider_t* | src/joy_math.h:138 |
| `joy_math_gjk2d_init_polygon` | 非不透明指针 / 输出参数：joy_gjk2d_collider_t* | src/joy_math.h:139 |
| `joy_math_gjk2d_init_pie` | 非不透明指针 / 输出参数：joy_gjk2d_collider_t* | src/joy_math.h:140 |
| `joy_math_gjk3df_init_sphere` | 非不透明指针 / 输出参数：joy_gjk3df_collider_t* | src/joy_math.h:144 |
| `joy_math_gjk3df_init_box` | 非不透明指针 / 输出参数：joy_gjk3df_collider_t* | src/joy_math.h:145 |
| `joy_math_gjk3df_init_obb` | 非不透明指针 / 输出参数：joy_gjk3df_collider_t* | src/joy_math.h:146 |
| `joy_math_gjk3df_init_capsule` | 非不透明指针 / 输出参数：joy_gjk3df_collider_t* | src/joy_math.h:147 |
| `joy_math_gjk3df_init_cylinder` | 非不透明指针 / 输出参数：joy_gjk3df_collider_t* | src/joy_math.h:148 |
| `joy_math_gjk3df_init_cone` | 非不透明指针 / 输出参数：joy_gjk3df_collider_t* | src/joy_math.h:149 |
| `joy_math_gjk3df_init_mesh` | 非不透明指针 / 输出参数：joy_gjk3df_collider_t* | src/joy_math.h:150 |
| `joy_math_gjk3d_init_sphere` | 非不透明指针 / 输出参数：joy_gjk3d_collider_t* | src/joy_math.h:157 |
| `joy_math_gjk3d_init_box` | 非不透明指针 / 输出参数：joy_gjk3d_collider_t* | src/joy_math.h:158 |
| `joy_math_gjk3d_init_capsule` | 非不透明指针 / 输出参数：joy_gjk3d_collider_t* | src/joy_math.h:159 |
| `joy_math_gjk3d_init_cylinder` | 非不透明指针 / 输出参数：joy_gjk3d_collider_t* | src/joy_math.h:160 |
| `joy_math_gjk3d_init_cone` | 非不透明指针 / 输出参数：joy_gjk3d_collider_t* | src/joy_math.h:161 |
| `joy_math_gjk3d_init_mesh` | 非不透明指针 / 输出参数：joy_gjk3d_collider_t* | src/joy_math.h:162 |
| `joy_scene_animator_set_on_enter` | 未知类型需人工标注：joy_anim_state_cb | src/joy_scene.h:46 |
| `joy_scene_animator_set_on_exit` | 未知类型需人工标注：joy_anim_state_cb | src/joy_scene.h:47 |
| `joy_ui_label_create` | 非不透明指针 / 输出参数：int* | src/joy_ui.h:17 |
| `joy_ui_label_set_text` | 非不透明指针 / 输出参数：int* | src/joy_ui.h:19 |
| `joy_ui_button_get_position` | 结构体按值返回 | src/joy_ui.h:25 |
| `joy_ui_button_set_text` | 非不透明指针 / 输出参数：int* | src/joy_ui.h:34 |
| `joy_ui_combobox_set_text` | 非不透明指针 / 输出参数：int* | src/joy_ui.h:84 |
| `joy_ui_combobox_add_itemx` | 非不透明指针 / 输出参数：int* | src/joy_ui.h:88 |
| `joy_ui_layout_grid_create` | 非不透明指针 / 输出参数：float* | src/joy_ui.h:105 |
| `joy_ui_layout_grid_destroy` | 非不透明指针 / 输出参数：joy_layout_grid_t* | src/joy_ui.h:107 |
| `joy_ui_layout_grid_next_row` | 非不透明指针 / 输出参数：joy_layout_grid_t* | src/joy_ui.h:108 |
| `joy_ui_layout_grid_set_row` | 非不透明指针 / 输出参数：joy_layout_grid_t* | src/joy_ui.h:109 |
| `joy_ui_layout_grid_cell` | 非不透明指针 / 输出参数：joy_layout_grid_t* | src/joy_ui.h:111 |
| `joy_ui_layout_grid_label` | 非不透明指针 / 输出参数：joy_layout_grid_t* | src/joy_ui.h:113 |
| `joy_ui_layout_grid_button_row` | 非不透明指针 / 输出参数：joy_layout_grid_t* | src/joy_ui.h:118 |
| `joy_ui_datagrid_addrow` | 非不透明指针 / 输出参数：char** | src/joy_ui.h:140 |
| `joy_ui_checkbox_set_state` | 未知类型需人工标注：joy_checkbox_state_t | src/joy_ui.h:155 |
| `joy_ui_joystick_set_dir_mode` | 未知类型需人工标注：joy_joystick_dir_mode | src/joy_ui.h:179 |
| `joy_ui_joystick_set_textures_for_mode` | 未知类型需人工标注：joy_joystick_dir_mode | src/joy_ui.h:181 |
| `joy_ui_joystick_get_direction` | 结构体按值返回 | src/joy_ui.h:191 |
| `joy_ui_inputbox_set_callback` | 函数指针 / 回调 | src/joy_ui.h:198 |
| `joy_ui_process_widget_event` | 非不透明指针 / 输出参数：void** | src/joy_ui.h:259 |
| `joy_ui_set_theme` | 非不透明指针 / 输出参数：joy_ui_theme_t* | src/joy_ui.h:289 |
| `joy_ui_get_mouse` | 非不透明指针 / 输出参数：float* | src/joy_ui.h:303 |
| `joy_ui_text_measure` | 非不透明指针 / 输出参数：int* | src/joy_ui.h:323 |
| `joy_ui_checkbox` | 非不透明指针 / 输出参数：bool* | src/joy_ui.h:340 |
| `joy_ui_slider` | 非不透明指针 / 输出参数：float* | src/joy_ui.h:342 |
| `joy_scene_tilemap_resolve_gid` | 结构体按值返回 | src/joy_scene.h:55 |
| `joy_scene_tilemap_get_tile_def_size` | 非不透明指针 / 输出参数：int* | src/joy_scene.h:57 |
| `joy_scene_tilemap_get_tile_uv` | 数组参数 | src/joy_scene.h:58 |
| `joy_scene_tilemap_object_layer_foreach` | 函数指针 / 回调 | src/joy_scene.h:65 |
| `joy_scene_tilemap_object_layer_foreach_all` | 函数指针 / 回调 | src/joy_scene.h:66 |
| `joy_scene_tilemap_render_create` | 未知类型需人工标注：joy_tilemap_path_remap_t | src/joy_scene.h:69 |
| `joy_net_kcpserver_set_callback` | 未知类型需人工标注：joy_net_callback | src/joy_net.h:58 |
| `joy_net_kcpclient_set_callback` | 未知类型需人工标注：joy_net_callback | src/joy_net.h:64 |
| `joy_net_wsnetserver_set_callback` | 未知类型需人工标注：joy_net_callback | src/joy_net.h:86 |
| `joy_net_wsnetclient_set_callback` | 未知类型需人工标注：joy_net_callback | src/joy_net.h:92 |
| `joy_net_netclient_create` | 未知类型需人工标注：joy_net_client_type | src/joy_net.h:93 |
| `joy_net_netclient_set_callback` | 未知类型需人工标注：joy_net_callback | src/joy_net.h:98 |
| `joy_net_netserver_create` | 未知类型需人工标注：joy_net_server_type | src/joy_net.h:100 |
| `joy_net_netserver_set_callback` | 未知类型需人工标注：joy_net_callback | src/joy_net.h:108 |
| `joy_scene_node_get_position` | 非不透明指针 / 输出参数：float* | src/joy_scene.h:98 |
| `joy_scene_node_get_world_position` | 非不透明指针 / 输出参数：float* | src/joy_scene.h:100 |
| `joy_scene_node_get_scale` | 非不透明指针 / 输出参数：float* | src/joy_scene.h:104 |
| `joy_scene_find_nodes_by_tag` | 非不透明指针 / 输出参数：joy_scene_node_p* | src/joy_scene.h:115 |
| `joy_scene_find_nodes_by_tag_deep` | 非不透明指针 / 输出参数：joy_scene_node_p* | src/joy_scene.h:116 |
| `joy_scene_node_set_update_callback` | 未知类型需人工标注：joy_scene_node_update_fn | src/joy_scene.h:117 |
| `joy_scene_node_set_render_callback` | 未知类型需人工标注：joy_scene_node_render_fn | src/joy_scene.h:118 |
| `joy_scene_node_set_destroy_callback` | 未知类型需人工标注：joy_scene_node_destroy_fn | src/joy_scene.h:119 |
| `joy_scene_node_set_event_callback` | 未知类型需人工标注：joy_scene_node_event_fn | src/joy_scene.h:120 |
| `joy_scene_set_load_callback` | 未知类型需人工标注：joy_scene_load_fn | src/joy_scene.h:133 |
| `joy_scene_set_update_callback` | 未知类型需人工标注：joy_scene_update_fn | src/joy_scene.h:134 |
| `joy_scene_set_render_callback` | 未知类型需人工标注：joy_scene_render_fn | src/joy_scene.h:135 |
| `joy_scene_set_destroy_callback` | 未知类型需人工标注：joy_scene_destroy_fn | src/joy_scene.h:136 |
| `joy_scene_set_event_callback` | 未知类型需人工标注：joy_scene_event_fn | src/joy_scene.h:137 |
| `joy_scene_set_fixed_update_callback` | 未知类型需人工标注：joy_scene_fixed_update_fn | src/joy_scene.h:138 |
| `joy_r2d_particle_cfg_default` | 结构体按值返回 | src/joy_r2d.h:240 |
| `joy_scene_ecs_kill` | 未知类型需人工标注：joy_ecs_id_t | src/joy_scene.h:161 |
| `joy_scene_ecs_set` | 未知类型需人工标注：joy_ecs_id_t | src/joy_scene.h:163 |
| `joy_scene_ecs_get` | 未知类型需人工标注：joy_ecs_id_t | src/joy_scene.h:164 |
| `joy_scene_ecs_remove` | 未知类型需人工标注：joy_ecs_id_t | src/joy_scene.h:165 |
| `joy_scene_ecs_query` | 可变参数 | src/joy_scene.h:166 |
| `joy_scene_ecs_queryx` | 非不透明指针 / 输出参数：char** | src/joy_scene.h:167 |
| `joy_scene_ecs_register` | 未知类型需人工标注：joy_ecs_system_t | src/joy_scene.h:168 |
| `joy_math_box2d_create_world` | 未知类型需人工标注：joy_vec2f_t | src/joy_math.h:165 |
| `joy_math_box2d_clear_world` | 非不透明指针 / 输出参数：joy_box2d_world_t* | src/joy_math.h:166 |
| `joy_math_box2d_destroy_world` | 非不透明指针 / 输出参数：joy_box2d_world_t* | src/joy_math.h:167 |
| `joy_math_box2d_step` | 非不透明指针 / 输出参数：joy_box2d_world_t* | src/joy_math.h:168 |
| `joy_math_box2d_create_body` | 非不透明指针 / 输出参数：joy_box2d_world_t* | src/joy_math.h:169 |
| `joy_math_box2d_destroy_body` | 非不透明指针 / 输出参数：joy_box2d_world_t* | src/joy_math.h:170 |
| `joy_math_box2d_create_joint` | 非不透明指针 / 输出参数：joy_box2d_world_t* | src/joy_math.h:171 |
| `joy_math_box2d_destroy_joint` | 非不透明指针 / 输出参数：joy_box2d_world_t* | src/joy_math.h:172 |
| `joy_math_box2d_foreach_bodies` | 函数指针 / 回调 | src/joy_math.h:173 |
| `joy_math_box2d_foreach_joints` | 函数指针 / 回调 | src/joy_math.h:174 |
| `joy_math_box2d_foreach_arbiters` | 函数指针 / 回调 | src/joy_math.h:175 |

## 有意排除（X）

| 函数 | 位置 |
|---|---|
| `joy_core_utils_random_create` | include/joy/jdefs.h:165 |
| `joy_core_utils_random_destroy` | include/joy/jdefs.h:166 |
| `joy_core_utils_random_next` | include/joy/jdefs.h:167 |
| `joy_core_utils_random_get_state` | include/joy/jdefs.h:170 |
| `joy_core_utils_random_set_state` | include/joy/jdefs.h:171 |
| `joy_core_utils_strdup` | src/joy_core.h:20 |
| `joy_core_utils_get_current_time` | src/joy_core.h:21 |
| `joy_core_utils_current_datetime` | src/joy_core.h:22 |
| `joy_core_utils_read_file` | src/joy_core.h:23 |
| `joy_core_utils_check_file` | src/joy_core.h:24 |
| `joy_core_utils_append_file` | src/joy_core.h:25 |
| `joy_core_utils_read_script` | src/joy_core.h:26 |
| `joy_core_utils_bit2short` | src/joy_core.h:27 |
| `joy_core_utils_short2bit` | src/joy_core.h:28 |
| `joy_core_utils_bit2int` | src/joy_core.h:30 |
| `joy_core_utils_int2bit` | src/joy_core.h:32 |
| `joy_core_utils_wait_delay` | src/joy_core.h:33 |
| `joy_core_utils_md5x` | src/joy_core.h:34 |
| `joy_core_utils_des_crypt` | src/joy_core.h:35 |
| `joy_core_utils_des_decrypt` | src/joy_core.h:36 |
| `joy_r2d_spine_asset_load` | src/joy_r2d.h:203 |
| `joy_r2d_spine_asset_free` | src/joy_r2d.h:205 |
| `joy_r2d_spine_asset_animation_count` | src/joy_r2d.h:207 |
| `joy_r2d_spine_asset_animation_duration` | src/joy_r2d.h:209 |
| `joy_r2d_spine_instance_init` | src/joy_r2d.h:212 |
| `joy_r2d_spine_instance_free` | src/joy_r2d.h:213 |
| `joy_r2d_spine_instance_reset` | src/joy_r2d.h:214 |
| `joy_r2d_spine_instance_set_animation` | src/joy_r2d.h:215 |
| `joy_r2d_spine_instance_add_animation` | src/joy_r2d.h:217 |
| `joy_r2d_spine_instance_update` | src/joy_r2d.h:221 |
| `joy_r2d_spine_instance_sync` | src/joy_r2d.h:222 |
| `joy_r2d_spine_instance_draw` | src/joy_r2d.h:223 |
| `joy_r2d_spine_instance_set_position` | src/joy_r2d.h:226 |
| `joy_r2d_spine_instance_set_scale` | src/joy_r2d.h:227 |
| `joy_r2d_spine_instance_set_rotation` | src/joy_r2d.h:228 |
| `joy_r2d_spine_instance_set_flip_x` | src/joy_r2d.h:229 |
| `joy_r2d_spine_instance_set_alpha` | src/joy_r2d.h:230 |
| `joy_r2d_spine_instance_set_tint` | src/joy_r2d.h:231 |
| `joy_r2d_spine_instance_bounds` | src/joy_r2d.h:234 |
| `joy_scene_ecs_test` | src/joy_scene.h:169 |
| `joy_script_module_create` | src/joy_script.h:18 |
| `joy_script_module_destroy` | src/joy_script.h:19 |
| `joy_script_module_query` | src/joy_script.h:20 |
| `joy_script_module_register` | src/joy_script.h:21 |
| `joy_script_module_require` | src/joy_script.h:27 |
| `joy_script_modules_retire` | src/joy_script.h:28 |
| `joy_script_module_instance_create` | src/joy_script.h:29 |
| `joy_script_module_instance_init` | src/joy_script.h:30 |
| `joy_script_module_instance_event` | src/joy_script.h:31 |
| `joy_script_module_instance_update` | src/joy_script.h:32 |
| `joy_script_module_instance_release` | src/joy_script.h:33 |
| `joy_script_msgpack_packer_init` | src/joy_script.h:36 |
| `joy_script_msgpack_pack_int` | src/joy_script.h:37 |
| `joy_script_msgpack_pack_int64` | src/joy_script.h:38 |
| `joy_script_msgpack_pack_uint` | src/joy_script.h:39 |
| `joy_script_msgpack_pack_uint64` | src/joy_script.h:40 |
| `joy_script_msgpack_pack_nil` | src/joy_script.h:41 |
| `joy_script_msgpack_pack_array` | src/joy_script.h:42 |
| `joy_script_msgpack_pack_str` | src/joy_script.h:43 |
| `joy_script_msgpack_pack_str_body` | src/joy_script.h:44 |
| `joy_script_msgpack_unpacked_init` | src/joy_script.h:45 |
| `joy_script_msgpack_unpacked_destroy` | src/joy_script.h:46 |
| `joy_script_msgpack_unpack_next` | src/joy_script.h:48 |
| `joy_script_msgpack_unpacked_data` | src/joy_script.h:51 |
| `joy_script_msgpack_object_type` | src/joy_script.h:52 |
| `joy_script_msgpack_object_array_size` | src/joy_script.h:53 |
| `joy_script_msgpack_object_array_at` | src/joy_script.h:54 |
| `joy_script_msgpack_object_str_size` | src/joy_script.h:55 |
| `joy_script_msgpack_object_str_ptr` | src/joy_script.h:56 |
| `joy_script_msgpack_object_u64` | src/joy_script.h:57 |
| `joy_script_msgpack_object_i64` | src/joy_script.h:58 |
| `joy_script_msgpack_object_boolean` | src/joy_script.h:59 |
| `joy_script_msgpack_object_f64` | src/joy_script.h:60 |

## `jmath.h`

`jmath.h` 是头文件内的 `JOY_INLINE` 值类型数学库，没有 `JOY_API` 声明，
因此不在上面的解析范围内，P1 需要单独的解析与「记录」映射。

| 项 | 数量（粗统计） |
|---|---|
| 行数 | 1757 |
| `JOY_INLINE` 函数 | 134 |
| `typedef struct` 值类型 | 10 |

## 三种常见操作

见 `api/README.md`。CI 用 `node api/tools/gen_bindings.js --check` 保证本文件与 IDL 同步。
