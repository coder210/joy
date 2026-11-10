# joy 绑定 API 覆盖率报告

> 自动生成，勿手改：由 `api/tools/gen_bindings.js` 从 `include/joy/{defs,api}.h` + `api/idl/joy.idl` 产出。
> 生成物（共享 thunk 与 cs 薄适配）覆盖 `status: on` 的 L0/L1，
> 以及 P2-A 试点里按字段展开成标量序列的 L2 条目；
> 注册表同时携带参数 / 返回类型，供 lua / js / cs 三端 vm.c 共用；
> 旧的 `modules/*/bindings/` 手写胶水未删，新旧链路并行。

## 总览

| 项 | 数量 |
|---|---|
| `joy.h` 中 `JOY_API` 声明 | 1156 |
| `joy.h` 中值类型 `typedef struct` | 47 |
| IDL 条目 | 1156 |
| 已开放（`status: on`） | 701 |
| 待人工确认 `owns` | 38 |
| 所有权待裁决（类型不匹配 / 缺 `owns`） | 38 |
| orphan（`joy.h` 中已消失） | 0 |

| 级别 | 含义 | 数量 |
|---|---|---|
| L0 | 纯标量 + const char*，可直接生成 | 264 |
| L1 | 不透明指针 → 句柄，需所有权测试 | 539 |
| L2 | 结构体按值 → P2-A 试点按字段展开成标量序列 | 74 |
| L3 | 复杂签名 → 必须手写 thunk，不自动生成 | 229 |
| X | 有意排除，不进绑定层 | 50 |

开放由 `joy.idl` 的 `status` 控制。`--enable` 只放行通过类型、所有权与
缓冲校验的条目，被跳过的原因会在命令输出和下面各节列出；**开放 ≠ 业务已调用**。

## 按域分布

| domain | L0 | L1 | L2 | L3 | X | 合计 |
|---|---|---|---|---|---|---|
| audio | 6 | 14 | 0 | 0 | 0 | 20 |
| box2d | 0 | 0 | 0 | 11 | 0 | 11 |
| collision2d | 0 | 0 | 10 | 8 | 0 | 18 |
| collision2df | 0 | 0 | 5 | 14 | 0 | 19 |
| collision3d | 0 | 0 | 3 | 4 | 0 | 7 |
| collision3df | 0 | 0 | 3 | 5 | 0 | 8 |
| core | 14 | 23 | 1 | 9 | 0 | 47 |
| gjk2d | 0 | 0 | 0 | 6 | 0 | 6 |
| gjk3d | 0 | 0 | 1 | 6 | 0 | 7 |
| gjk3df | 0 | 0 | 1 | 7 | 0 | 8 |
| input | 0 | 14 | 0 | 3 | 0 | 17 |
| jmath | 220 | 0 | 0 | 0 | 0 | 220 |
| jmsgpack | 0 | 0 | 0 | 22 | 0 | 22 |
| joy | 0 | 0 | 0 | 6 | 0 | 6 |
| math | 0 | 0 | 0 | 1 | 0 | 1 |
| module | 0 | 0 | 0 | 0 | 5 | 5 |
| modules | 0 | 0 | 0 | 0 | 5 | 5 |
| moudles | 0 | 0 | 0 | 1 | 0 | 1 |
| net | 0 | 56 | 0 | 20 | 0 | 76 |
| r2d | 4 | 33 | 6 | 12 | 19 | 74 |
| r3d | 0 | 109 | 14 | 23 | 0 | 146 |
| scene | 1 | 117 | 3 | 38 | 1 | 160 |
| shape | 0 | 3 | 4 | 1 | 0 | 8 |
| sys | 19 | 0 | 0 | 3 | 0 | 22 |
| ui | 0 | 170 | 23 | 29 | 0 | 222 |
| utils | 0 | 0 | 0 | 0 | 20 | 20 |

## 待办：返回句柄但缺 `owns`

这些函数返回 `*_p` / `void*`，却没推断出配对的析构函数。
`owns` 是**唯一无法自动发现**的语义，必须人工核对后填进 IDL。

| 函数 | 返回 | 位置 | 人工裁决（`note`） |
|---|---|---|---|
| `joy_core_get_raw_window` | `void*` | include/joy/api.h:111 | 借用：内部 SDL_Window*，由 destroy_window 一并销毁 |
| `joy_r2d_unwrap` | `void*` | include/joy/api.h:204 | 借用：内部 SDL_Renderer*，所有权归 jr2d |
| `joy_r3d_load_shader` | `void*` | include/joy/api.h:334 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_create_pipeline` | `void*` | include/joy/api.h:336 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_upload_buffer` | `void*` | include/joy/api.h:339 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_create_solid_texture` | `void*` | include/joy/api.h:340 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_create_depth_texture` | `void*` | include/joy/api.h:342 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_create_sampler` | `void*` | include/joy/api.h:343 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_get_device` | `void*` | include/joy/api.h:354 | 借用：内部 SDL_GPUDevice*，由 r3d_destroy 销毁 |
| `joy_r3d_mesh_cube` | `render_mesh_p` | include/joy/api.h:369 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_sphere` | `render_mesh_p` | include/joy/api.h:370 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_plane` | `render_mesh_p` | include/joy/api.h:371 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_ramp` | `render_mesh_p` | include/joy/api.h:372 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_custom` | `render_mesh_p` | include/joy/api.h:373 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_quad` | `render_mesh_p` | include/joy/api.h:375 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_cube_wire` | `render_mesh_p` | include/joy/api.h:378 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_sphere_wire` | `render_mesh_p` | include/joy/api.h:379 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_plane_wire` | `render_mesh_p` | include/joy/api.h:380 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_ramp_wire` | `render_mesh_p` | include/joy/api.h:381 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_create_texture` | `void*` | include/joy/api.h:386 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_load_texture_png` | `void*` | include/joy/api.h:389 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_gen_checkerboard` | `void*` | include/joy/api.h:390 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_create_line_pipeline` | `void*` | include/joy/api.h:730 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_create_lighting_pipeline` | `void*` | include/joy/api.h:737 | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_mesh_cube_lit` | `render_mesh_p` | include/joy/api.h:741 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_sphere_lit` | `render_mesh_p` | include/joy/api.h:742 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_plane_lit` | `render_mesh_p` | include/joy/api.h:743 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_ramp_lit` | `render_mesh_p` | include/joy/api.h:744 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_custom_lit` | `render_mesh_p` | include/joy/api.h:745 | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_scene_tilemap_find_tile_layer` | `jtilemap_tile_layer_p` | include/joy/api.h:1374 | 借用：tm->layers 内部元素，由 jtilemap_destroy 统一销毁 |
| `joy_scene_tilemap_find_object_layer` | `jtilemap_object_layer_p` | include/joy/api.h:1376 | 借用：tm->layers 内部元素，由 jtilemap_destroy 统一销毁 |
| `joy_scene_node_get_userdata` | `void*` | include/joy/api.h:1485 | 借用：外部传入的 userdata，引擎不释放 |
| `joy_scene_find_node_by_name` | `scene_node_p` | include/joy/api.h:1490 | 借用：DFS 返回树内节点；scene_node_destroy 类型匹配但填 owns 会双重释放 |
| `joy_scene_find_node_by_name_deep` | `scene_node_p` | include/joy/api.h:1491 | 借用：树内节点，由 scene_destroy 递归销毁 |
| `joy_scene_get_root_at` | `scene_node_p` | include/joy/api.h:1506 | 借用：scene->root_nodes 内部指针 |
| `joy_scene_get_popup_node` | `scene_node_p` | include/joy/api.h:1517 | 借用：scene->popup_node，由 scene_destroy 销毁 |
| `joy_scene_get_userdata` | `void*` | include/joy/api.h:1519 | 借用：外部 userdata，scene_destroy 不释放 |
| `joy_scene_manager_get_current` | `scene_p` | include/joy/api.h:1529 | 借用：manager 栈顶 scene，所有权归 manager |

## 待办：所有权待裁决（`owns` 不可信）

返回句柄但没有可通过硬校验的 `owns`：借用指针、dtor 参数不匹配，
或压根没有合规的单参数 dtor。`note` 列是逐条人工核对后的所有权裁决。
批量开放会跳过它们；`--check` 也禁止把这类条目设成 `on`，
否则 dtor 会把错误类型的指针交给析构函数，运行时才崩。

| 函数 | 返回 | `owns` | 问题 | 人工裁决（`note`） |
|---|---|---|---|---|
| `joy_core_get_raw_window` | `void*` | （无） | 返回句柄但缺 owns | 借用：内部 SDL_Window*，由 destroy_window 一并销毁 |
| `joy_r2d_unwrap` | `void*` | （无） | 返回句柄但缺 owns | 借用：内部 SDL_Renderer*，所有权归 jr2d |
| `joy_r3d_load_shader` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_create_pipeline` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_upload_buffer` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_create_solid_texture` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_create_depth_texture` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_create_sampler` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_get_device` | `void*` | （无） | 返回句柄但缺 owns | 借用：内部 SDL_GPUDevice*，由 r3d_destroy 销毁 |
| `joy_r3d_mesh_cube` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_sphere` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_plane` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_ramp` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_custom` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_quad` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_cube_wire` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_sphere_wire` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_plane_wire` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_ramp_wire` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_create_texture` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_load_texture_png` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_gen_checkerboard` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_create_line_pipeline` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_create_lighting_pipeline` | `void*` | （无） | 返回句柄但缺 owns | 新建 SDL GPU 对象；释放需 (device,obj) 双参数且不在 joy.h，r3d_destroy(jr3d_p) 类型不符 |
| `joy_r3d_mesh_cube_lit` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_sphere_lit` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_plane_lit` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_ramp_lit` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_r3d_mesh_custom_lit` | `render_mesh_p` | （无） | 返回句柄但缺 owns | 语义新建 mesh；唯一 dtor r3d_mesh_destroy(jr3d_p,render_mesh_p) 是双参数，硬约束不允许填 owns |
| `joy_scene_tilemap_find_tile_layer` | `jtilemap_tile_layer_p` | （无） | 返回句柄但缺 owns | 借用：tm->layers 内部元素，由 jtilemap_destroy 统一销毁 |
| `joy_scene_tilemap_find_object_layer` | `jtilemap_object_layer_p` | （无） | 返回句柄但缺 owns | 借用：tm->layers 内部元素，由 jtilemap_destroy 统一销毁 |
| `joy_scene_node_get_userdata` | `void*` | （无） | 返回句柄但缺 owns | 借用：外部传入的 userdata，引擎不释放 |
| `joy_scene_find_node_by_name` | `scene_node_p` | （无） | 返回句柄但缺 owns | 借用：DFS 返回树内节点；scene_node_destroy 类型匹配但填 owns 会双重释放 |
| `joy_scene_find_node_by_name_deep` | `scene_node_p` | （无） | 返回句柄但缺 owns | 借用：树内节点，由 scene_destroy 递归销毁 |
| `joy_scene_get_root_at` | `scene_node_p` | （无） | 返回句柄但缺 owns | 借用：scene->root_nodes 内部指针 |
| `joy_scene_get_popup_node` | `scene_node_p` | （无） | 返回句柄但缺 owns | 借用：scene->popup_node，由 scene_destroy 销毁 |
| `joy_scene_get_userdata` | `void*` | （无） | 返回句柄但缺 owns | 借用：外部 userdata，scene_destroy 不释放 |
| `joy_scene_manager_get_current` | `scene_p` | （无） | 返回句柄但缺 owns | 借用：manager 栈顶 scene，所有权归 manager |

## L3 清单（必须手写 thunk）

| 函数 | 原因 | 位置 |
|---|---|---|
| `joy_core_lcg_random_get_state` | 非不透明指针 / 输出参数：uint64_t* | include/joy/defs.h:183 |
| `joy_scene_procmap_config_default` | 非不透明指针 / 输出参数：jpm_config_t* | include/joy/defs.h:239 |
| `joy_scene_procmap_generate_seed` | 非不透明指针 / 输出参数：jpm_config_t* | include/joy/defs.h:245 |
| `joy_scene_procmap_generate` | 非不透明指针 / 输出参数：jpm_config_t* | include/joy/defs.h:247 |
| `joy_scene_procmap_rasterize` | 数组参数 | include/joy/defs.h:249 |
| `joy_scene_procmap_bfs` | 数组参数 | include/joy/defs.h:252 |
| `joy_scene_procmap_sample_open_tiles` | 非不透明指针 / 输出参数：jpm_config_t* | include/joy/defs.h:255 |
| `joy_core_utf8_decode` | 非不透明指针 / 输出参数：int32_t* | include/joy/api.h:50 |
| `joy_core_utf8_next` | 非不透明指针 / 输出参数：size_t* | include/joy/api.h:60 |
| `joy_core_utf8_decode_all` | 非不透明指针 / 输出参数：int32_t* | include/joy/api.h:72 |
| `joy_net_pack_int8` | 非 const char* 输出缓冲 | include/joy/api.h:74 |
| `joy_net_pack_int16` | 非 const char* 输出缓冲 | include/joy/api.h:75 |
| `joy_net_pack_int32` | 非 const char* 输出缓冲 | include/joy/api.h:76 |
| `joy_net_pack_uint32` | 非 const char* 输出缓冲 | include/joy/api.h:77 |
| `joy_net_pack_int64` | 非 const char* 输出缓冲 | include/joy/api.h:78 |
| `joy_net_unpack_int8` | 非不透明指针 / 输出参数：int8_t* | include/joy/api.h:79 |
| `joy_net_unpack_int16` | 非不透明指针 / 输出参数：int16_t* | include/joy/api.h:80 |
| `joy_net_unpack_int32` | 非不透明指针 / 输出参数：int32_t* | include/joy/api.h:81 |
| `joy_net_unpack_uint32` | 非不透明指针 / 输出参数：uint32_t* | include/joy/api.h:82 |
| `joy_net_unpack_int64` | 非不透明指针 / 输出参数：int64_t* | include/joy/api.h:83 |
| `joy_net_pack_string` | 非 const char* 输出缓冲 | include/joy/api.h:84 |
| `joy_net_unpack_string` | 非 const char* 输出缓冲 | include/joy/api.h:85 |
| `joy_scene_jtimer_create` | 未知类型需人工标注：jtimer_callback | include/joy/api.h:87 |
| `joy_core_get_mouse_pos` | 非不透明指针 / 输出参数：float* | include/joy/api.h:105 |
| `joy_core_log_info` | 可变参数 | include/joy/api.h:125 |
| `joy_core_log_debug` | 可变参数 | include/joy/api.h:126 |
| `joy_core_log_error` | 可变参数 | include/joy/api.h:127 |
| `joy_core_get_local_ips` | 数组参数 | include/joy/api.h:157 |
| `sys_accept` | 非 const char* 输出缓冲 | include/joy/api.h:171 |
| `sys_recv` | 非 const char* 输出缓冲 | include/joy/api.h:173 |
| `sys_recvfrom` | 非 const char* 输出缓冲 | include/joy/api.h:174 |
| `joy_r2d_coordinates_from_window` | 结构体按值返回 | include/joy/api.h:205 |
| `joy_r2d_get_color` | 非不透明指针 / 输出参数：uint8_t* | include/joy/api.h:208 |
| `joy_r2d_get_logical_presentation` | 非不透明指针 / 输出参数：int* | include/joy/api.h:214 |
| `joy_r2d_get_size` | 非不透明指针 / 输出参数：int* | include/joy/api.h:215 |
| `joy_r2d_get_colorf` | 非不透明指针 / 输出参数：float* | include/joy/api.h:219 |
| `joy_r2d_draw_points` | 结构体指针参数 | include/joy/api.h:222 |
| `joy_r2d_get_clip_rect` | 结构体指针参数 | include/joy/api.h:230 |
| `joy_r2d_draw_lines` | 结构体指针参数 | include/joy/api.h:239 |
| `shape_draw_polygon` | 结构体指针参数 | include/joy/api.h:241 |
| `joy_ui_image_draw1` | 结构体指针参数 | include/joy/api.h:252 |
| `joy_ui_image_draw_ex` | 结构体指针参数 | include/joy/api.h:253 |
| `joy_ui_image_get_size` | 非不透明指针 / 输出参数：float* | include/joy/api.h:256 |
| `joy_ui_image_update` | 结构体指针参数 | include/joy/api.h:257 |
| `joy_ui_text_create` | 非不透明指针 / 输出参数：int* | include/joy/api.h:262 |
| `joy_ui_text_update` | 非不透明指针 / 输出参数：int* | include/joy/api.h:264 |
| `joy_r2d_frect` | 结构体按值返回 | include/joy/api.h:279 |
| `joy_r2d_fpx` | 非不透明指针 / 输出参数：float* | include/joy/api.h:280 |
| `joy_ui_image_draw1_f` | 结构体指针参数 | include/joy/api.h:287 |
| `joy_r2d_jtext_measurex` | 非不透明指针 / 输出参数：int* | include/joy/api.h:314 |
| `joy_scene_animation_draw` | 结构体指针参数 | include/joy/api.h:327 |
| `joy_r3d_create_solid_texture` | 未知类型需人工标注：unsigned char | include/joy/api.h:340 |
| `joy_r3d_mat44_to_float16` | 数组参数 | include/joy/api.h:344 |
| `joy_r3d_load_texture_png` | 非不透明指针 / 输出参数：int* | include/joy/api.h:389 |
| `joy_r3d_font_set_color` | 未知类型需人工标注：Uint8 | include/joy/api.h:401 |
| `joy_r3d_font_draw_2d_color` | 未知类型需人工标注：Uint8 | include/joy/api.h:416 |
| `joy_r3d_font_draw_2d_outline` | 未知类型需人工标注：Uint8 | include/joy/api.h:421 |
| `joy_r3d_get_window_size` | 非不透明指针 / 输出参数：int* | include/joy/api.h:428 |
| `joy_r3d_world_to_screen` | 非不透明指针 / 输出参数：float* | include/joy/api.h:446 |
| `joy_r3d_get_logical_presentation` | 非不透明指针 / 输出参数：int* | include/joy/api.h:458 |
| `joy_r3d_window_to_logical` | 非不透明指针 / 输出参数：float* | include/joy/api.h:460 |
| `joy_r3d_logical_to_window` | 非不透明指针 / 输出参数：float* | include/joy/api.h:462 |
| `joy_r3d_button_set_onclick` | 函数指针 / 回调 | include/joy/api.h:493 |
| `joy_r3d_draw_rect_2d` | 未知类型需人工标注：Uint8 | include/joy/api.h:500 |
| `joy_r3d_draw_mesh_2d` | 未知类型需人工标注：Uint8 | include/joy/api.h:506 |
| `joy_r3d_input_handle_event` | 非不透明指针 / 输出参数：SDL_Event* | include/joy/api.h:584 |
| `joy_r3d_input_set_onsubmit` | 函数指针 / 回调 | include/joy/api.h:586 |
| `joy_r3d_combobox_input_handle_event` | 非不透明指针 / 输出参数：SDL_Event* | include/joy/api.h:626 |
| `joy_r3d_checkbox_set_state` | 未知类型需人工标注：jr3d_checkbox_state_t | include/joy/api.h:697 |
| `joy_r3d_draw_mesh_solid_basis` | 结构体指针参数 | include/joy/api.h:722 |
| `joy_r3d_draw_wire_basis` | 结构体指针参数 | include/joy/api.h:728 |
| `joy_r3d_set_light_block` | 结构体指针参数 | include/joy/api.h:747 |
| `joy_r3d_draw_mesh_lit` | 结构体指针参数 | include/joy/api.h:749 |
| `joy_r3d_draw_mesh_lit_color` | 结构体指针参数 | include/joy/api.h:750 |
| `joy_fov_ray_dist` | 非不透明指针 / 输出参数：joy_fov_rect* | include/joy/api.h:763 |
| `joy_fov_compute_radii` | 非不透明指针 / 输出参数：joy_fov_rect* | include/joy/api.h:767 |
| `joy_fov_draw` | 非不透明指针 / 输出参数：float* | include/joy/api.h:773 |
| `joy_fov_draw_2d` | 非不透明指针 / 输出参数：float* | include/joy/api.h:780 |
| `collision2d_get_ray_circle` | 结构体按值返回 | include/joy/api.h:788 |
| `collision2d_get_ray_rectangle` | 结构体按值返回 | include/joy/api.h:789 |
| `collision2d_get_ray_rectanglex` | 结构体按值返回 | include/joy/api.h:790 |
| `collision2d_get_ray_polygon` | 结构体按值返回 | include/joy/api.h:791 |
| `collision2d_check_lines` | 结构体指针参数 | include/joy/api.h:796 |
| `collision2d_get_circles` | 结构体指针参数 | include/joy/api.h:803 |
| `collision2d_get_rectangles` | 结构体指针参数 | include/joy/api.h:804 |
| `collision2d_get_polygons` | 结构体指针参数 | include/joy/api.h:805 |
| `collision3d_get_ray_triangle` | 结构体按值返回 | include/joy/api.h:806 |
| `collision3d_get_ray_quad` | 结构体按值返回 | include/joy/api.h:807 |
| `collision3d_get_ray_sphere` | 结构体按值返回 | include/joy/api.h:808 |
| `collision3d_get_ray_box` | 结构体按值返回 | include/joy/api.h:809 |
| `collision2df_get_ray_circle` | 结构体按值返回 | include/joy/api.h:814 |
| `collision2df_get_ray_rectangle` | 结构体按值返回 | include/joy/api.h:816 |
| `collision2df_get_ray_rectanglex` | 结构体按值返回 | include/joy/api.h:818 |
| `collision2df_get_ray_polygon` | 结构体按值返回 | include/joy/api.h:820 |
| `collision2df_check_point_rectangle` | 未知类型需人工标注：vec2f_t | include/joy/api.h:822 |
| `collision2df_check_point_circle` | 未知类型需人工标注：vec2f_t | include/joy/api.h:824 |
| `collision2df_get_squared_distance_point_circle` | 未知类型需人工标注：vec2f_t | include/joy/api.h:825 |
| `collision2df_check_point_triangle` | 未知类型需人工标注：vec2f_t | include/joy/api.h:826 |
| `collision2df_check_lines` | 未知类型需人工标注：vec2f_t | include/joy/api.h:828 |
| `collision2df_check_point_line` | 未知类型需人工标注：vec2f_t | include/joy/api.h:830 |
| `collision2df_get_circles` | 结构体指针参数 | include/joy/api.h:837 |
| `collision2df_get_rectangles` | 结构体指针参数 | include/joy/api.h:839 |
| `joy_math_find_min_separation` | 非不透明指针 / 输出参数：vec2f_t* | include/joy/api.h:842 |
| `collision2df_get_polygons` | 结构体指针参数 | include/joy/api.h:844 |
| `collision2df_swept_obb` | 未知类型需人工标注：vec2f_t | include/joy/api.h:866 |
| `joy_tilemap_sweep_x` | 非不透明指针 / 输出参数：fp_t* | include/joy/api.h:876 |
| `joy_tilemap_sweep_y` | 非不透明指针 / 输出参数：fp_t* | include/joy/api.h:878 |
| `collision3df_get_ray_triangle` | 结构体按值返回 | include/joy/api.h:881 |
| `collision3df_get_ray_quad` | 结构体按值返回 | include/joy/api.h:883 |
| `collision3df_get_ray_sphere` | 结构体按值返回 | include/joy/api.h:885 |
| `collision3df_get_ray_box` | 结构体按值返回 | include/joy/api.h:887 |
| `collision3df_get_ray_obb` | 结构体按值返回 | include/joy/api.h:890 |
| `gjk2d_collide` | 未知类型需人工标注：vec2f_t | include/joy/api.h:900 |
| `gjk2d_init_circle` | 非不透明指针 / 输出参数：gjk2d_collider_t* | include/joy/api.h:902 |
| `gjk2d_init_box` | 非不透明指针 / 输出参数：gjk2d_collider_t* | include/joy/api.h:903 |
| `gjk2d_init_capsule` | 非不透明指针 / 输出参数：gjk2d_collider_t* | include/joy/api.h:904 |
| `gjk2d_init_polygon` | 非不透明指针 / 输出参数：gjk2d_collider_t* | include/joy/api.h:905 |
| `gjk2d_init_pie` | 非不透明指针 / 输出参数：gjk2d_collider_t* | include/joy/api.h:906 |
| `gjk3df_init_sphere` | 非不透明指针 / 输出参数：gjk3df_collider_t* | include/joy/api.h:910 |
| `gjk3df_init_box` | 非不透明指针 / 输出参数：gjk3df_collider_t* | include/joy/api.h:911 |
| `gjk3df_init_obb` | 非不透明指针 / 输出参数：gjk3df_collider_t* | include/joy/api.h:912 |
| `gjk3df_init_capsule` | 非不透明指针 / 输出参数：gjk3df_collider_t* | include/joy/api.h:913 |
| `gjk3df_init_cylinder` | 非不透明指针 / 输出参数：gjk3df_collider_t* | include/joy/api.h:914 |
| `gjk3df_init_cone` | 非不透明指针 / 输出参数：gjk3df_collider_t* | include/joy/api.h:915 |
| `gjk3df_init_mesh` | 非不透明指针 / 输出参数：gjk3df_collider_t* | include/joy/api.h:916 |
| `gjk3d_init_sphere` | 非不透明指针 / 输出参数：gjk3d_collider_t* | include/joy/api.h:923 |
| `gjk3d_init_box` | 非不透明指针 / 输出参数：gjk3d_collider_t* | include/joy/api.h:924 |
| `gjk3d_init_capsule` | 非不透明指针 / 输出参数：gjk3d_collider_t* | include/joy/api.h:925 |
| `gjk3d_init_cylinder` | 非不透明指针 / 输出参数：gjk3d_collider_t* | include/joy/api.h:926 |
| `gjk3d_init_cone` | 非不透明指针 / 输出参数：gjk3d_collider_t* | include/joy/api.h:927 |
| `gjk3d_init_mesh` | 非不透明指针 / 输出参数：gjk3d_collider_t* | include/joy/api.h:928 |
| `joy_scene_animator_set_on_enter` | 未知类型需人工标注：janim_state_cb | include/joy/api.h:959 |
| `joy_scene_animator_set_on_exit` | 未知类型需人工标注：janim_state_cb | include/joy/api.h:960 |
| `joy_ui_label_create` | 非不透明指针 / 输出参数：int* | include/joy/api.h:1034 |
| `joy_ui_label_set_text` | 非不透明指针 / 输出参数：int* | include/joy/api.h:1036 |
| `joy_ui_button_get_position` | 结构体按值返回 | include/joy/api.h:1042 |
| `joy_ui_button_set_text` | 非不透明指针 / 输出参数：int* | include/joy/api.h:1051 |
| `joy_ui_combobox_set_text` | 非不透明指针 / 输出参数：int* | include/joy/api.h:1101 |
| `joy_ui_combobox_add_itemx` | 非不透明指针 / 输出参数：int* | include/joy/api.h:1105 |
| `joy_ui_layout_grid_create` | 非不透明指针 / 输出参数：float* | include/joy/api.h:1122 |
| `joy_ui_layout_grid_destroy` | 非不透明指针 / 输出参数：jlayout_grid_t* | include/joy/api.h:1124 |
| `joy_ui_layout_grid_next_row` | 非不透明指针 / 输出参数：jlayout_grid_t* | include/joy/api.h:1125 |
| `joy_ui_layout_grid_set_row` | 非不透明指针 / 输出参数：jlayout_grid_t* | include/joy/api.h:1126 |
| `joy_ui_layout_grid_cell` | 非不透明指针 / 输出参数：jlayout_grid_t* | include/joy/api.h:1128 |
| `joy_ui_layout_grid_label` | 非不透明指针 / 输出参数：jlayout_grid_t* | include/joy/api.h:1130 |
| `joy_ui_layout_grid_button_row` | 非不透明指针 / 输出参数：jlayout_grid_t* | include/joy/api.h:1135 |
| `joy_ui_datagrid_addrow` | 非不透明指针 / 输出参数：char** | include/joy/api.h:1157 |
| `joy_ui_checkbox_set_state` | 未知类型需人工标注：checkbox_state_t | include/joy/api.h:1172 |
| `joy_input_joystick_set_dir_mode` | 未知类型需人工标注：joystick_dir_mode | include/joy/api.h:1196 |
| `joy_input_joystick_set_textures_for_mode` | 未知类型需人工标注：joystick_dir_mode | include/joy/api.h:1198 |
| `joy_input_joystick_get_direction` | 结构体按值返回 | include/joy/api.h:1208 |
| `joy_ui_inputbox_set_callback` | 函数指针 / 回调 | include/joy/api.h:1215 |
| `joy_ui_process_widget_event` | 非不透明指针 / 输出参数：void** | include/joy/api.h:1276 |
| `joy_ui_set_theme` | 非不透明指针 / 输出参数：jui_theme_t* | include/joy/api.h:1306 |
| `joy_ui_get_mouse` | 非不透明指针 / 输出参数：float* | include/joy/api.h:1320 |
| `joy_ui_text_measure` | 非不透明指针 / 输出参数：int* | include/joy/api.h:1340 |
| `joy_ui_checkbox` | 非不透明指针 / 输出参数：bool* | include/joy/api.h:1357 |
| `joy_ui_slider` | 非不透明指针 / 输出参数：float* | include/joy/api.h:1359 |
| `joy_scene_tilemap_resolve_gid` | 结构体按值返回 | include/joy/api.h:1367 |
| `joy_scene_tilemap_get_tile_def_size` | 非不透明指针 / 输出参数：int* | include/joy/api.h:1369 |
| `joy_scene_tilemap_get_tile_uv` | 数组参数 | include/joy/api.h:1370 |
| `joy_scene_tilemap_object_layer_foreach` | 函数指针 / 回调 | include/joy/api.h:1377 |
| `joy_scene_tilemap_object_layer_foreach_all` | 函数指针 / 回调 | include/joy/api.h:1378 |
| `joy_scene_tilemap_render_create` | 未知类型需人工标注：jtilemap_path_remap_t | include/joy/api.h:1381 |
| `joy_net_kcpserver_set_callback` | 未知类型需人工标注：net_callback | include/joy/api.h:1411 |
| `joy_net_kcpclient_set_callback` | 未知类型需人工标注：net_callback | include/joy/api.h:1417 |
| `joy_net_wsnetserver_set_callback` | 未知类型需人工标注：net_callback | include/joy/api.h:1439 |
| `joy_net_wsnetclient_set_callback` | 未知类型需人工标注：net_callback | include/joy/api.h:1445 |
| `joy_net_netclient_create` | 未知类型需人工标注：net_client_type | include/joy/api.h:1446 |
| `joy_net_netclient_set_callback` | 未知类型需人工标注：net_callback | include/joy/api.h:1451 |
| `joy_net_netserver_create` | 未知类型需人工标注：net_server_type | include/joy/api.h:1453 |
| `joy_net_netserver_set_callback` | 未知类型需人工标注：net_callback | include/joy/api.h:1461 |
| `joy_scene_node_get_position` | 非不透明指针 / 输出参数：float* | include/joy/api.h:1475 |
| `joy_scene_node_get_world_position` | 非不透明指针 / 输出参数：float* | include/joy/api.h:1477 |
| `joy_scene_node_get_scale` | 非不透明指针 / 输出参数：float* | include/joy/api.h:1481 |
| `joy_scene_find_nodes_by_tag` | 非不透明指针 / 输出参数：scene_node_p* | include/joy/api.h:1492 |
| `joy_scene_find_nodes_by_tag_deep` | 非不透明指针 / 输出参数：scene_node_p* | include/joy/api.h:1493 |
| `joy_scene_node_set_update_callback` | 未知类型需人工标注：scene_node_update_fn | include/joy/api.h:1494 |
| `joy_scene_node_set_render_callback` | 未知类型需人工标注：scene_node_render_fn | include/joy/api.h:1495 |
| `joy_scene_node_set_destroy_callback` | 未知类型需人工标注：scene_node_destroy_fn | include/joy/api.h:1496 |
| `joy_scene_node_set_event_callback` | 未知类型需人工标注：scene_node_event_fn | include/joy/api.h:1497 |
| `joy_scene_set_load_callback` | 未知类型需人工标注：scene_load_fn | include/joy/api.h:1510 |
| `joy_scene_set_update_callback` | 未知类型需人工标注：scene_update_fn | include/joy/api.h:1511 |
| `joy_scene_set_render_callback` | 未知类型需人工标注：scene_render_fn | include/joy/api.h:1512 |
| `joy_scene_set_destroy_callback` | 未知类型需人工标注：scene_destroy_fn | include/joy/api.h:1513 |
| `joy_scene_set_event_callback` | 未知类型需人工标注：scene_event_fn | include/joy/api.h:1514 |
| `joy_scene_set_fixed_update_callback` | 未知类型需人工标注：scene_fixed_update_fn | include/joy/api.h:1515 |
| `joy_r2d_particle_cfg_default` | 结构体按值返回 | include/joy/api.h:1533 |
| `joy_scene_ecs_kill` | 未知类型需人工标注：jecs_id_t | include/joy/api.h:1563 |
| `joy_scene_ecs_set` | 未知类型需人工标注：jecs_id_t | include/joy/api.h:1565 |
| `joy_scene_ecs_get` | 未知类型需人工标注：jecs_id_t | include/joy/api.h:1566 |
| `joy_scene_ecs_remove` | 未知类型需人工标注：jecs_id_t | include/joy/api.h:1567 |
| `joy_scene_ecs_query` | 可变参数 | include/joy/api.h:1568 |
| `joy_scene_ecs_queryx` | 非不透明指针 / 输出参数：char** | include/joy/api.h:1569 |
| `joy_scene_ecs_register` | 未知类型需人工标注：jecs_system_t | include/joy/api.h:1570 |
| `moudles_retire` | 非不透明指针 / 输出参数：module_p* | include/joy/api.h:1636 |
| `box2d_create_world` | 未知类型需人工标注：vec2f_t | include/joy/api.h:1644 |
| `box2d_clear_world` | 非不透明指针 / 输出参数：box2d_world_t* | include/joy/api.h:1645 |
| `box2d_destroy_world` | 非不透明指针 / 输出参数：box2d_world_t* | include/joy/api.h:1646 |
| `box2d_step` | 非不透明指针 / 输出参数：box2d_world_t* | include/joy/api.h:1647 |
| `box2d_create_body` | 非不透明指针 / 输出参数：box2d_world_t* | include/joy/api.h:1648 |
| `box2d_destroy_body` | 非不透明指针 / 输出参数：box2d_world_t* | include/joy/api.h:1649 |
| `box2d_create_joint` | 非不透明指针 / 输出参数：box2d_world_t* | include/joy/api.h:1650 |
| `box2d_destroy_joint` | 非不透明指针 / 输出参数：box2d_world_t* | include/joy/api.h:1651 |
| `box2d_foreach_bodies` | 函数指针 / 回调 | include/joy/api.h:1652 |
| `box2d_foreach_joints` | 函数指针 / 回调 | include/joy/api.h:1653 |
| `box2d_foreach_arbiters` | 函数指针 / 回调 | include/joy/api.h:1654 |
| `jmsgpack_packer_init` | 结构体指针参数 | include/joy/api.h:1656 |
| `jmsgpack_pack_int` | 结构体指针参数 | include/joy/api.h:1657 |
| `jmsgpack_pack_int64` | 结构体指针参数 | include/joy/api.h:1658 |
| `jmsgpack_pack_uint` | 结构体指针参数 | include/joy/api.h:1659 |
| `jmsgpack_pack_uint64` | 结构体指针参数 | include/joy/api.h:1660 |
| `jmsgpack_pack_nil` | 结构体指针参数 | include/joy/api.h:1661 |
| `jmsgpack_pack_array` | 结构体指针参数 | include/joy/api.h:1662 |
| `jmsgpack_pack_str` | 结构体指针参数 | include/joy/api.h:1663 |
| `jmsgpack_pack_str_body` | 结构体指针参数 | include/joy/api.h:1664 |
| `jmsgpack_unpacked_init` | 结构体指针参数 | include/joy/api.h:1665 |
| `jmsgpack_unpacked_destroy` | 结构体指针参数 | include/joy/api.h:1666 |
| `jmsgpack_unpack_next` | 结构体指针参数 | include/joy/api.h:1668 |
| `jmsgpack_unpacked_data` | 结构体指针参数 | include/joy/api.h:1671 |
| `jmsgpack_object_type` | 非不透明指针 / 输出参数：jmsgpack_object_t* | include/joy/api.h:1672 |
| `jmsgpack_object_array_size` | 非不透明指针 / 输出参数：jmsgpack_object_t* | include/joy/api.h:1673 |
| `jmsgpack_object_array_at` | 非不透明指针 / 输出参数：jmsgpack_object_t* | include/joy/api.h:1674 |
| `jmsgpack_object_str_size` | 非不透明指针 / 输出参数：jmsgpack_object_t* | include/joy/api.h:1675 |
| `jmsgpack_object_str_ptr` | 非不透明指针 / 输出参数：jmsgpack_object_t* | include/joy/api.h:1676 |
| `jmsgpack_object_u64` | 非不透明指针 / 输出参数：jmsgpack_object_t* | include/joy/api.h:1677 |
| `jmsgpack_object_i64` | 非不透明指针 / 输出参数：jmsgpack_object_t* | include/joy/api.h:1678 |
| `jmsgpack_object_boolean` | 非不透明指针 / 输出参数：jmsgpack_object_t* | include/joy/api.h:1679 |
| `jmsgpack_object_f64` | 非不透明指针 / 输出参数：jmsgpack_object_t* | include/joy/api.h:1680 |

## 有意排除（X）

| 函数 | 位置 |
|---|---|
| `utils_random_create` | include/joy/defs.h:165 |
| `utils_random_destroy` | include/joy/defs.h:166 |
| `utils_random_next` | include/joy/defs.h:167 |
| `utils_random_get_state` | include/joy/defs.h:170 |
| `utils_random_set_state` | include/joy/defs.h:171 |
| `utils_strdup` | include/joy/api.h:19 |
| `utils_get_current_time` | include/joy/api.h:20 |
| `utils_current_datetime` | include/joy/api.h:21 |
| `utils_read_file` | include/joy/api.h:22 |
| `utils_check_file` | include/joy/api.h:23 |
| `utils_append_file` | include/joy/api.h:24 |
| `utils_read_script` | include/joy/api.h:25 |
| `utils_bit2short` | include/joy/api.h:26 |
| `utils_short2bit` | include/joy/api.h:27 |
| `utils_bit2int` | include/joy/api.h:29 |
| `utils_int2bit` | include/joy/api.h:31 |
| `utils_wait_delay` | include/joy/api.h:32 |
| `utils_md5x` | include/joy/api.h:33 |
| `utils_des_crypt` | include/joy/api.h:34 |
| `utils_des_decrypt` | include/joy/api.h:35 |
| `joy_r2d_spine_asset_load` | include/joy/api.h:998 |
| `joy_r2d_spine_asset_free` | include/joy/api.h:1000 |
| `joy_r2d_spine_asset_animation_count` | include/joy/api.h:1002 |
| `joy_r2d_spine_asset_animation_duration` | include/joy/api.h:1004 |
| `joy_r2d_spine_instance_init` | include/joy/api.h:1007 |
| `joy_r2d_spine_instance_free` | include/joy/api.h:1008 |
| `joy_r2d_spine_instance_reset` | include/joy/api.h:1009 |
| `joy_r2d_spine_instance_set_animation` | include/joy/api.h:1010 |
| `joy_r2d_spine_instance_add_animation` | include/joy/api.h:1012 |
| `joy_r2d_spine_instance_update` | include/joy/api.h:1016 |
| `joy_r2d_spine_instance_sync` | include/joy/api.h:1017 |
| `joy_r2d_spine_instance_draw` | include/joy/api.h:1018 |
| `joy_r2d_spine_instance_set_position` | include/joy/api.h:1021 |
| `joy_r2d_spine_instance_set_scale` | include/joy/api.h:1022 |
| `joy_r2d_spine_instance_set_rotation` | include/joy/api.h:1023 |
| `joy_r2d_spine_instance_set_flip_x` | include/joy/api.h:1024 |
| `joy_r2d_spine_instance_set_alpha` | include/joy/api.h:1025 |
| `joy_r2d_spine_instance_set_tint` | include/joy/api.h:1026 |
| `joy_r2d_spine_instance_bounds` | include/joy/api.h:1029 |
| `joy_scene_ecs_test` | include/joy/api.h:1571 |
| `modules_create` | include/joy/api.h:1626 |
| `modules_destroy` | include/joy/api.h:1627 |
| `modules_query` | include/joy/api.h:1628 |
| `modules_register` | include/joy/api.h:1629 |
| `modules_require` | include/joy/api.h:1635 |
| `module_instance_create` | include/joy/api.h:1637 |
| `module_instance_init` | include/joy/api.h:1638 |
| `module_instance_event` | include/joy/api.h:1639 |
| `module_instance_update` | include/joy/api.h:1640 |
| `module_instance_release` | include/joy/api.h:1641 |

## `jmath.h`

`jmath.h` 是头文件内的 `JOY_INLINE` 值类型数学库，没有 `JOY_API` 声明，
因此不在上面的解析范围内，P1 需要单独的解析与「记录」映射。

| 项 | 数量（粗统计） |
|---|---|
| 行数 | 1756 |
| `JOY_INLINE` 函数 | 134 |
| `typedef struct` 值类型 | 10 |

## 三种常见操作

见 `api/README.md`。CI 用 `node api/tools/gen_bindings.js --check` 保证本文件与 IDL 同步。
