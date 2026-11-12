# joy 交付记录（已收口）

> **项目于 2026-11-11 收口，收口时没有计划内工作。本文不是待办清单。**
> 用途：记录仓库的最终状态与已知事实，供以后查阅或另开新议题时参考。
> **收口后另开的新议题不在本文范围**：C 类型名统一，任务书见 `TYPE_RENAME_TASK.md`
> （待执行，尚未改动任何文件）。
> 当前提交一律以 `git log -1` 为准；收口提交前 HEAD 为 `240f5f7a`。
> §3 记录已完成的实现，§7 写明交付范围与「明确不做」的方向，§10 是验收结果。

## 1. 项目是什么

joy 是一个 C 引擎（`src/` + `include/joy/`），用 `api/tools/gen_bindings.js` 从
`include/joy/japi.h` 生成四份后端的绑定（C++ vtable、C thunk、C# P/Invoke、jmath
包装）。`examples/` 下有 cpp / lua / js / csharp 四份功能相同的示例，**它们同时是
给以后项目用的模板**，所以改动要按模板标准处理（注释、平台分支、可覆盖参数）。

生成链路的分级（`api/idl/joy.idl` 每条带 `level` + `status`）：

| 级别 | 含义 | 能否自动生成 |
|---|---|---|
| L0/L1 | 全标量 / 句柄 + 标量 | 能，`status:on` |
| L2 | 值类型结构体按字段展开 | 能，需 `structs.js` 认得字段类型 |
| L3 | 结构体指针 / 出参 / 回调 / 可变参 | 不能，`status:off`，消费方退化 |

当前：**1170 条 IDL，739 条 `status:on`，vtable 739 个强签名**（其中 7 条是
`borrowed` 借用句柄见 §3 的 14，13 条 3D mesh 靠单参数 dtor 开放见 §3 的 15；
较收口时 IDL +13 条（§3 的 22 新增 11 条输入 API、§3 的 23 新增 2 条边沿
API），`status:on` +18 条（这 13 条再加 §3 的 23 修复 `Uint8` 分类后开放的
5 条 r3d 绘制 API）。

## 2. 当前状态

- 工作区状态以 `git status` 为准；本轮收尾前 `main` 与 `origin/main` 同步。
- 桌面（Windows / VS2022 / Debug）：全量构建 0 错误，`ctest` 4/4 通过；**构建不依赖任何旧名兼容宏**。2026-11-11 复验仍是 0 错误 / 4/4（含本轮新增的 vtable 自检条目）。
- `node api/tools/gen_bindings.js --check` 通过（report/thunks/vtable/cs glue/jmath 全部最新）。
- **`compat.h` 与 `jcont.h` 已删除**（2026-11-11）：前者是空兼容层、后者是仅被
  `src/jutils.c` 死引用且从未使用符号的 900 行孤岛容器头；已移除 `joy.h` 与
  `jutils.c` 两处 include，删除后全量构建 0 错误。
- **公开 API 命名迁移 P1 基本完成**：1150 条收敛为 `joy_<域>_<动作>`，声明 / 定义 /
  调用点 / 生成物四处 C 符号一致。**6 条例外**：
  `joy_fov_*`(4) 与 `joy_tilemap_sweep_x|y`(2)，域名回退成白名单外的 `joy`，
  均为 L3 / `status:off` / 不暴露；因在 `legacy_names.txt` 豁免名单里 `--check`
  查不到。本轮决定保持现状（不暴露，改名收益低于动公开头的风险），只如实记录。
- Android：CI 已多次跑通（`Android APKs` 工作流 `success`），**已产出 4 个 APK**（见 §6）。
- Web：已部署到 GitHub Pages（`web.yml` 仅 push 触发），站点 https://coder210.github.io/joy/ 可访问。

## 3. 已经做完的事（按提交）

`7f37e951` — 强签名 JoyApi vtable，C++ 示例改原生 DLL 插件
`ccb6232a` — C++ 双形态模板 + 贴图恢复，生成链路开放至 701 条
`bd12eab2` — 打通 C# 的 Android 构建，MonoVM 运行时按平台入库

**Web 部署 + P1 命名统一（2026-11-09 ~ 11-10，提交 `74b8b71d` ~ `69ffaa64`）**：

7. **Web/WASM 走 GitHub Pages**：`web.yml` 改为仅 `push(main)` 触发，`configure-pages` 用
   `enablement:true` 在 Actions 里开 Pages Source=GitHub Actions；Emscripten 构建修复
   `BUILD_SHARED_LIBS=OFF`（msgpack SHARED 在 wasm 不支持）、`CMAKE_EXECUTABLE_SUFFIX=.html`、
   各后端 `--preload-file` 资源。线上站点 https://coder210.github.io/joy/。
8. **P0 命名基础设施**：`naming.js` 加 10 域白名单 `DOMAINS` + `ALIASES`（冻结脚本层 PascalCase
   方法名）、`gen_bindings.js` 加 `domainOf()` / `SKIP_DOMAINS` / `--check` lint / `RENAME_MAP`
   继承逻辑；`legacy_names.txt` 冻结 1156 存量；`compat.h` 放旧名 `#define` 桥接。
9. **P1 分批改名（共 15 批 + 收尾）**：`core`/`net`/`scene`/`ui`/`r2d`/`r3d`/`input`/`audio`/`math`
   逐个域收敛到 `joy_<域>_<动作>`；`jmath` 220 条走 `jmath_wrap.js` 改名前缀；`script`(33)/
   `utils`(20) 因带函数指针/有意排除维持 X 级（改名后 `joy_script_*` / `joy_core_utils_*`）。
10. **P1 收尾（真正改名）**：前 15 批只在头文件改名 + 靠 `compat.h` `#define` 桥接，`.c`
   实现仍是旧名。收尾用字符边界批量替换，把 `src/`(24) / `examples`(13) /
   `modules`(15) 共 **52 个手写源文件**里约 1881 处旧名真正改成 `joy_*`
   （其中约 25 处是日志 / 注释字符串里的函数名引用，不全是代码），随后清空
   `compat.h` 宏块。**代码里零旧名残留**，但注释里曾有 4 处、文档对照表还是旧名，
   已在下面的 11 补齐。无兼容宏也能编译。
11. **改名收尾（2026-11-10）**：独立审计后修掉遗留 —— 四份模板注释里 4 处
   `r2d_create` 旧名与过时宿主路径；`rename_map.json` 尾部 4 条无声明、IDL 无条目的
   `utils_aes_*` 死映射；`api/README.md` 域表把 `image` 从 `r2d` 改到实际使用的
   `ui`；`examples/README.md` 的 6 条 IDL 对照表全部换成 P1 新名、旧胶水能力表加
   历史留档提示；`gen_bindings.js` 里生成 `coverage.md` 的「旧胶水未删 / 新旧链路
   并行」错误文案；以及 `joy.h` / `emit_vtable.js` / `emit_cs.js` 的过时注释。
   生成物只重生成了 `coverage.md`（2 行），其余四个 hash 与索引一致。
12. **构建 / 生成链路加固（2026-11-10）**：根 `CMakeLists.txt` 的 CLR 探测与
   `examples/CMakeLists.txt:67-111` 统一（`JOY_CLR_PLATFORM` 推导、Android ABI
   映射、全部平台库名），消除从根构建 Android 选错运行时；`emit_thunks.js`
   增加 `THUNK_MAX_ARGS = 16`（对齐 `modules/lua/bindings/api/lapi.c` 的
   `LUAX_MAX_ARGS`），`canThunk()` 与 `gen_bindings.js --check` 双重拦截展开后
   参数超限。实测 16 参数放行、17 参数被拒；IDL 里 2 条超限 L2 正好是历史人工
   跳过的 `joy_r3d_slider_set_colors` / `joy_ui_slider_set_colors`；已开放条目
   最大 argc 13，门禁与生成物不变。
13. **命名收尾第二轮（2026-11-10）**：对 1150 个旧名做全仓全词审计，发现 39 处
   残留，其中「把当前 API 写成旧名」的已全部清掉：三份脚本模板的 `r2d_fpx` /
   `jtext_measurex`、C# 冒烟测试的消息与注释、`api/README.md` 自检边界段与
   IDL 字段示例、`api/tools` 四处注释举例。另修两处**功能性**硬编码：
   `emit_vtable.js` 的 `SELFTEST_SAFE` 里漏改的 `jmath_fp_from_float`
   （名字写旧会让该 API 完全不进自检，修正后 `joy_api_vtable.c` 多 6 行），
   以及 `gen_bindings.js` 的 `SKIP_NAMES` 里已不存在的 `jecs_test`。
   剩余 16 处旧名全部是改名记录 / 旧胶水历史留档，有意保留，不要再清。
14. **借用句柄语义（机制 A，2026-11-11）**：IDL 新增人工字段 `borrowed`；
   `emit_thunks.js` 的 `ownsIssue()` 放行借用条目，thunk 用
   `joy_obj_register(..., NULL, ...)` 注册、不生成 dtor 包装、注册失败不释放指针；
   `gen_bindings.js` 的 `mergeIdl()` 把 `borrowed` 列为人工字段（随改名继承、
   `--scan` 不覆盖），`--check` 校验与 `owns` 互斥，`coverage.md` 单列借用统计。
   12 条已裁决的借用句柄里 **7 条 typed 句柄已开放**（`status:on` 701 → 708）；
   另 5 条返回 `void*`，因 C# 强句柄与 C++ vtable 都不支持 `void*`（会生成非法
   `Void*` 类名），`canEmitCs()` 补上与 `okHandle()` 一致的拦截，这 5 条保持
   `off`。验证：`--scan` 幂等且 borrowed 保留、`--check` OK、全量构建 0 错误、
   `ctest` 4/4。
15. **单参数 dtor 包装（机制 B，2026-11-11）**：`src/jr3d.c` 给 `render_mesh`
   加 `jr3d_p owner` 回指，11 处 `SDL_calloc(1, sizeof(render_mesh_t))` 统一改走
   `r3d_mesh_alloc(r)`；`include/joy/japi.h` 末尾新增
   `joy_r3d_mesh_destroy_one(render_mesh_p)`，内部转调
   `joy_r3d_mesh_destroy(m->owner, m)`（复用延迟释放队列，避免 GPU buffer 提前
   释放）。15 条 mesh 的 `owns` 全部指向它，**13 条开放**（`status:on` 708 → 721，
   IDL 1156 → 1157）。另 2 条 `mesh_custom*` 因 `void*` 缓冲参数被 `canOpen`
   拦下、11 条返回 `void*` 的 SDL GPU 对象因 `canEmitCs`/`okHandle` 排除
   `void*` 保持 `off`，note 已写明原因。**约束**：mesh 句柄不得比创建它的 r3d
   上下文活得久（owner 会悬空）。验证：`--scan` 幂等、`--check` OK、
   全量构建 0 错误、`ctest` 4/4。
16. **删除 `compat.h` / `jcont.h`（2026-11-11）**：`compat.h` 在 P1 收尾清空宏块后
   只剩 include guard，唯一引用是 `joy.h:17`；`jcont.h` 是 900 行孤岛容器头
   （队列/链表/vector/红黑树），不被 `joy.h` 链与 `DECL_SOURCES` 包含，唯一引用
   `src/jutils.c:8` 是死引用（其中 50 个符号一个都没被使用，`src/jecs.c` 里
   `joy_vector_t` 等命中全在注释块中）。删除两个文件 + 两处 include，并清理
   `rename_types.js` 的 tag 采集列表、`gen_bindings.js` 与三份文档里的过期描述。
   删除后全量构建 0 错误。
17. **清理重复 `M_PI` 与 `r2d_render_geometry` 的 extern hack（2026-11-11）**：
   `jr3d.c`/`jparticle.c`/`jlight.c` 各 `#define M_PI`，而 `jmath.h` 无统一 π 常量；
   改为在 `jmath.h` 提供 `JOY_PI`，三文件删各自 define、使用点改 `JOY_PI`。
   另 `jlight.c` 用 `extern` 前向声明 `jr2d.c` 内部的 `r2d_render_geometry`（跨文件
   复用内部函数的反模式），改为在 `jdefs.h` 正式声明该函数、`jlight.c` 删 extern 块。
   该函数随后按第 18 条统一改名为 `joy_r2d_render_geometry`。
   验证：`--scan` 幂等（1157 条，不进 IDL）、`--check` OK、构建 0 错误、`ctest` 4/4。
18. **`src/` 非内部函数统一加 `joy_` 前缀（2026-11-12）**：42 个非 `static`、
   无 `joy_` 前缀的函数全部改名，共 77 处（10 个文件）：`jbehavior.c` 的 `bt_*` 14 个
   （含 `japi.h` 14 条公开声明）、`jr2d.c` 的 `r2d_*` 9 个、`jr3d.c` 的 `r3d_*` 9 个、
   `jgltf.c` 的 `jgltf_*` 5 个、`jspine.c` 的 `jspine_instance_animation`、
   `jui.c` 的 `label_handle_event`/`radio_render`、`jecs.c` 的 `ecs_test`、
   `jtilemap.c` 的 `jtilemap_load_cjson`。
   - 命名口径：`jgltf_*`/`jspine_*`/`jtilemap_*` 开头的 `j` 视作 joy 缩写，**展开**为
     `joy_gltf_*`/`joy_spine_*`/`joy_tilemap_*`（不叠加成 `joy_jgltf_*`）。
   - `jspine.c` 的 `_spAtlasPage_createTexture`/`_spAtlasPage_disposeTexture`/
     `_spUtil_readFile` 是 spine 第三方回调，**保持原名**。
   - 这些函数都不在 IDL / 生成物内（`japi.h` 的 `bt_*` 声明不带 `JOY_API`），
     故 `--scan`/`--check` 零变化（仍 1157 条）；全量构建 0 错误、`ctest` 4/4、
     旧名 0 残留、42 个新名齐全。
19. **架构审视后的前三项修复（2026-11-12）**：只读审视 `src/` 全部 `.c` 后，
   按优先级落地前三项，第 4 项（平台抽象 / 按 API 域拆文件）单独排期未动。
   - **修注释事实错误**：上一轮写进 `JoySources.cmake` 的"janimator 只设颜色、
     与渲染解耦"（实际调 `joy_ui_image_create/draw_ex`）与"三个可选模块整文件
     被 `JOY_USE_*` 包裹"（`jmodule.c` 当时没有）均不符实，已改正；并新增
     "已知偏差"段记录命名域与分组不一致、`jgltf.c` 缺公开声明等未修项。
   - **补齐 PLUGINS 边界**：`JOY_ENABLE_PLUGINS` 原本没有对应的 `JOY_USE_PLUGINS`，
     `jmodule.c` 无条件包裹、`japi.h` 却无条件声明 11 个 `joy_script_module_*`
     （默认 OFF 构建下调用会链接失败）。现在 `JoyTargets.cmake` 转发宏、
     `jmodule.c` 整文件 `#ifdef`、`japi.h:1625/1642` 用**原有空行**放置
     `#ifdef`/`#endif`，因此 `source` 行号零漂移、`--check` 直接 OK。
     已用独立构建目录验证 `-DJOY_ENABLE_PLUGINS=ON` 下 4 个宿主均编过。
   - **消除 `jr2d` 假句柄**：文字缓存原是 6 个进程级 `static`（`g_frame`、
     `g_buckets`/`g_lru_*`/`g_cache_*`），哈希键不含 `r2d` → 两个 renderer
     会跨用同一张 `SDL_Texture`，`joy_r2d_destroy(A)` 还会清掉存活 B 的纹理。
     改为：`struct joy_r2d` 持有独立 `frame`；条目带 `owner` 并纳入哈希与命中比较；
     新增内部 `jtext_cache_clear_for(r2d)` 让 destroy 只清自己；sweep 按各条目
     `owner->frame` 判断。**4 个公开 cache API 签名与全局语义保持不变**，
     故 IDL / thunks / vtable / C# glue 零变化。
   - 验证：`--scan` 幂等（1157 条）、`--check` OK、默认构建 0 错误、
     `ctest` 4/4、PLUGINS=ON 构建 0 错误、`git diff --check` 0。
   - **仍未修（单独排期）**：`jr2d.c` 承载 UI/场景 API、无资源管理器/绘制
     指令层、两套分配器并存、Winsock 初始化/清理缺少引用计数、
     `set_sock_*timeo` 在 macOS/UNIX 仍为空实现；控件 `void* event` API 与
     `joy_ui_process_widget_event` 空壳待真实消费者（见第 23 条）。
     SDL 类型进入公开头、事件抽象的平台依赖部分已在第 23 条完成。
     其中 `jecs.c` 越界写、`jsys.c` 返回值写反、`bitset` 泄漏、函数级
     `static` GPU 资源及第 21 条的 13 处运行期缺陷均已修复（第 20/21 条）。
20. **继续处理：纯 bug 与函数级 static GPU 资源（2026-11-12）**：承接第 19 条，
    把已确证的 3 个纯 bug + 4 处 GPU 资源全部收口，**公开头零改动**。
    - **3 个纯 bug**：`jecs.c` 的 `joy_scene_ecs_queryx` 对固定容量 `ids[512]`
      无上界检查，改为按 `sizeof(result.ids)/sizeof(result.ids[0])` 截断；
      `joy_scene_ecs_destroy` 漏掉 `store->bitset`（它与 `sparse_set` 内部的
      `bitset` 是两份独立分配），补 `bitset_destroy`；`jsys.c` 的 `WSACleanup`
      成功返回 0，原 `return r != 0` 写反，改为 `== 0`。
    - **4 处函数级 `static` GPU 资源**：`jr2d.c` 的 FOV 渐变 `SDL_Texture*`、
      `jr3d.c` 的单位圆盘 mesh / FOV 渐变 `SDL_GPUTexture*` / FOV 上一帧 mesh
      全部改为 `struct joy_r2d` / `struct joy_r3d` 的字段（两者 create 都用
      `SDL_calloc`，新字段自动为 NULL）。这些对象绑定创建它们的 renderer /
      GPU device，进程级缓存会让第二个实例跨设备复用，且销毁时无处释放。
    - **释放顺序**：`joy_r2d_destroy` 在 `SDL_DestroyRenderer` 前销毁 FOV 纹理；
      `joy_r3d_destroy` 在销毁 device 前释放 FOV 纹理，并把两个共享 mesh 经
      `joy_r3d_mesh_destroy` 推入延迟队列，由 `r3d_flush_pending` 统一释放。
    - 验证：`--scan` 幂等（1157 条）、`--check` OK、生成物零变化、
      默认构建 0 错误、`ctest` 4/4、PLUGINS=ON 构建 0 错误、`git diff --check` 0。
    21. **继续修复：运行期缺陷二批（2026-11-12）**：对 `src/*.c` 再做一轮只读审视，
     修 13 处可确证的缺陷，公开 API 签名与 IDL 仍零改动。
     - **崩溃 / 越界**：`jr2d.c` 的 `joy_r2d_create` 在 `SDL_GetWindowSize` 前
       先判空（`joy_core_get_raw_window(NULL)` 会直接解引用）；`joy_r2d_wrap`
       借用的外部 renderer 不再被 `joy_r2d_destroy` 销毁（新增 `owns_sdl`）；
       `jecs.c` 组件拷贝长度按注册的 `meta` 截断；`jsys.c` 的 `recv` 留 1 字节
       给尾零、`recvfrom` 失败时不再读未初始化的 `peer`；`jtilemap.c` 的
       `default_remap` 去掉共享 `static buf`，所有哈希 key 改为堆副本（原先
       失败条目把静态数组 / 外来字符串当 key，destroy 会 `free` 非堆内存）。
     - **句柄 / 生命周期**：`jmodule.c` 补 `mod->handle = handle`（原先
       `_open_sym` 拿野指针去 `SDL_LoadFunction`、失败时卸载野句柄），并修正
       `joy_script_modules_retire` 查不到却对 `kh_end` 取值的条件写反；
       `jsys.c` 的 IOCP 回显把数据复制进发送包自有的 buffer（原先 `WSASend`
       异步引用本函数末尾就 `free` 的接收包），create 的 `calloc` 失败补
       `WSACleanup`、`listen_socket` 显式置 `INVALID_SOCKET`（避免
       `closesocket(0)`）；`jr3d.c` 字体重复 init 先释放旧 ttf 缓冲与全部烘焙
       纹理，`SDL_LoadFile` 的缓冲统一走 `SDL_free`（原 `free` 跨 CRT 堆）。
     - **缓存隔离 / 平台分支**：`jfont.c` 销毁字体改为只清该字体的缓存条目
       （`jdefs.h` 末尾追加不带 `JOY_API` 的内部声明，位于最后一个公开声明
       之后，故 `source` 行号零漂移）；`jsys.c` 的 `anyaddr` 统一返回 0
       （macOS/UNIX 原返回 `true`=1.0.0.0）、`closesocket` 各平台真正关闭句柄。
     - **本轮未做**：Winsock 多处各自 `WSAStartup/WSACleanup` 的引用计数收敛
       （当前每处自行配对，仅补齐失败路径）；`set_sock_*timeo` 在 macOS/UNIX
       仍是空实现后返回 0。与第 19 条的排期项一并留待单独处理。
     - 验证：`--scan` 幂等（1157 条）、`--check` OK、生成物零变化、默认构建
       0 错误、`ctest` 4/4、PLUGINS=ON 构建 0 错误、`git diff --check` 0。
     22. **B 方案 P1：引擎层输入状态快照（2026-11-12）**：按"不暴露事件对象、
     只给标量状态"的路线做第一步，目的是验证生成链能吃到事件能力。
     - **选 B 的依据**：`api/tools/structs.js` 只放行字段全是已知标量的 POD
       结构体（数组 / 嵌套 / 指针一律不展开），事件结构必然含 `char text[]` /
       union / 枚举，按值传进不了 L2；按指针传又退化成不透明句柄，脚本端仍
       读不到字段。A 方案（自有 `joy_event_t`）要兑现就得先改三个生成器。
       纯标量是现成 L0，且项目已有两次同思路先例（`image_draw_rect` 绕开
       L3 的 `jrect_t*`、`joy_game_hooks.h` 的 `JoyInput` 快照）。
     - **引擎层**：新增 `src/jinput.c`（登记进 `JOY_SRC_CORE`），进程级单例
       状态（按键 / 鼠标按钮 / 坐标 / 滚轮帧增量），11 条纯标量 API：
       `joy_input_feed_*` 由宿主喂入、`is_*` / `get_*` 供查询、`end_frame`
       清帧级增量、`reset` 全清。明确是单窗口单例，不提供句柄、
       不假装支持多实例。
     - **四端喂入**：C++ 宿主在 `SDL_AppEvent` 里喂入（`g_input` 供 Native
       DLL ABI，暂保留双写）；Lua / JS 走 `joy_core_dispatch_event`，在
       `jcore.c` 翻译鼠标/键盘事件时顺带喂入，按键按 scancode 记录且放在
       `key_name()` 判空之前，绕过只有 8 个键的键名表；C# 的
       `csclib_input.c` 从自持第二份 `s_keys/s_mouse_*` 改为薄转发到引擎层，
       宿主原有的 `csx_input_end_frame()` 自动接上。
     - **生成链**：声明全部放在 `japi.h` 末尾，既有条目 `source` 行号零漂移；
       `--scan` 后 11 条默认 `off`（`status` 是人工字段，不自动开），用
       `--enable name:<API>` 点名开放，0 skipped。IDL 1157 → 1168、
       `status:on` / vtable 721 → 732，thunks / vtable / cs glue / C# 自动
       生成 11 个强签名。这正是 `*_process_event(const void*)`（L1 却 `off`）
       拿不到的能力。
     - 验证：`--scan` + `--check` OK、默认构建 0 错误（`jinput.c` 在 4 个宿主
       均编译）、`ctest` 4/4、PLUGINS=ON 构建 0 错误、`git diff --check` 0。
     - **后续状态**：P0（`jdefs.h` 去 SDL include、删 `JOY_EVENT_*`）与
       `g_input` 双写收敛已在第 23 条完成，顺序按第 23 条的只读结论调整；
       控件标量入口与 `joy_ui_process_widget_event` 空壳仍待真实消费者；
       P3 再评估是否走到 A。Lua / JS 暂未处理 `SDL_EVENT_MOUSE_WHEEL`，
       该端滚轮查询恒 0。
     23. **B 方案 P2 前半：公开头去 SDL 依赖 + 状态快照完整化（2026-11-12）**：
     承接第 22 条，兑现"公开头解耦"，并补上帧级离散输入。
     - **范围调整（重要）**：先做只读梳理，确认 `jui.c` 的 20 多个
       `joy_ui_*_process_event` 与 `jr3d.c` 的两条 `*_handle_event`
       **全仓库没有外部调用点**（只有 `jr3d.c` 内部两处转发），
       `joy_ui_process_widget_event`（`jui.c:3583`）仍是只有 `(void)e;` 的空壳。
       给死 API 逐个加标量入口没有收益，因此本轮**不做控件标量入口**；
       等出现真实消费者再补。
     - **P0 完成**：`jdefs.h:17` 的 `<SDL3/SDL_events.h>` 换成 `<stdint.h>`，
       零使用的 `JOY_EVENT_*`（数值抄自 SDL3 枚举）整块移除，用等行数注释
       保持后续 `JOY_API` 行号不变；`japi.h` 的 6 处 `Uint8` 改成 `uint8_t`；
       `japi.h:584/626` 的 `SDL_Event* event` 改成 `const void* event`，
       `jr3d.c` 两个实现内部强转回 `const SDL_Event*`。至此 `include/joy/`
       不再包含任何 SDL 头、不再出现 SDL 类型，与 `joy_game_hooks.h:21`
       的 DLL 解耦约定一致。
     - **顺带的生成链收益**：`Uint8` 是生成器不认识的未知类型，曾把 5 条
       r3d 绘制 API（`joy_r3d_font_set_color` / `font_draw_2d_color` /
       `font_draw_2d_outline` / `draw_rect_2d` / `draw_mesh_2d`）误判为
       `L3/off`；换成 `uint8_t` 后降为 `L1`，点名开放成功（实现都在
       `jr3d.c`）。两条 `SDL_Event*` 改成 `void*` 后仍是 `off`，保持原状。
     - **帧级离散输入**：`src/jinput.c` 增加 `s_key_pressed` /
       `s_key_released` 两个帧级边沿数组，`feed_key` 一次喂入持续状态与边沿，
       `end_frame` 每帧清边沿与滚轮；新增 `joy_input_was_key_pressed` /
       `was_key_released` 两条纯标量 API。Lua / JS 宿主补上
       `joy_input_end_frame()`，四端现在按同一节奏清理帧级状态。
     - **收敛双写**：C++ 宿主 `SDL_AppEvent` 不再维护第二份 `g_input`，
       改为每帧 `on_frame` 前从 `joy_input_*` getter 重建这份 Native DLL
       ABI 快照；状态源只剩引擎层一份。
     - 生成链：IDL 1168 → 1170、`status:on` / vtable 732 → 739，
       `--enable` 点名开放 7 条、0 skipped，既有条目 `source` 行号零漂移。
     - 验证：`--scan` + `--check` OK、默认构建 0 错误（公开头不再含 SDL 头，
       全部编译单元仍通过）、`ctest` 4/4、PLUGINS=ON 构建 0 错误、
       `git diff --check` 0。
     - **仍未做**：控件标量入口与 `joy_ui_process_widget_event` 空壳（等真实
       消费者）；文本输入的标量通道（`const char*` 返回值的跨语言生命周期
       尚未验证）；其余 20+ 个 `const void* event` API 仍是 `off`；以及第 19
       条列的按 API 域拆 `jr2d.c`、资源管理器、两套分配器等。
     24. **Android CI 回归修复：补齐 SDL 显式依赖（2026-11-12）**：第 23 条把
     `jdefs.h:17` 的 `<SDL3/SDL_events.h>` 换成 `<stdint.h>` 后，Android clang
     （C99 严格模式）报 `src/jtilemap.c` 的 `SDL_LoadFile` / `SDL_free`
     隐式函数声明。根因是这些 TU 长期靠 joy.h 间接拿 SDL 声明，公共头去
     SDL 依赖后隐式依赖才暴露；桌面构建因其它头恰好补齐声明而未报错。
     - 全仓区分大小写扫描 `SDL_*` 与 `Uint*/Sint*` 标识符，确认真实受影响的
       只有 3 个 TU：`jtilemap.c`（`SDL_LoadFile` / `SDL_free`）、`jgltf.c`
       （`SDL_IOFromFile` / `ReadIO` / `WriteIO` / `CloseIO` / `GetIOSize`、
       `Sint64`，仅 `JOY_USE_GLTF` 启用时编译）、`jsys.c`（第 21 条 IOCP
       修复引入的 `SDL_memcpy`）。其余命中都是注释或
       `examples/cpp/src/game.cpp` 的自有 `SDL_SCANCODE_*` 枚举。
     - 修复：前两个 TU 显式 `#include <SDL3/SDL.h>`（聚合头确实含
       `SDL_stdinc.h` / `SDL_filesystem.h` / `SDL_iostream.h`，覆盖全部所需
       声明）；`jsys.c` 改用标准 `memcpy` 并显式包含 `<string.h>`，
       系统层不再依赖 SDL。
     - 复查扫描后真实隐式依赖为 0；`--check` OK（1170 条，生成物零变化）、
       默认构建 0 错误、`ctest` 4/4、PLUGINS=ON 构建 0 错误。
     - 注意：本地没有 Android NDK，Android 端仍以 CI 复验为准。
25. **Android APK 应用名对齐 desktop（2026-11-12）**：CI 出的 4 个 APK 安装后
    统一显示 `Game`。根因是 `SDL/android-project/app/src/main/res/values/strings.xml`
    的 `<string name="app_name">Game</string>` 被唯一的 AndroidManifest 用
    `android:label="@string/app_name"` 引用，而 4 个 productFlavor 只差异化了
    `applicationIdSuffix` / jniLibs / assets，没有按 flavor 覆盖 label。
    - 目标名称取 desktop 各后端窗口标题：`examples/cpp/src/game.cpp` 的
      "Joy CPP Topdown"、`examples/lua/src/main.lua` 的 "Joy Lua Topdown"、
      `examples/js/src/main.js` 的 "Joy JS Topdown"、
      `examples/csharp/runtime/GameApp.cs` 的 "Joy C# Topdown"。
    - 做法：新增 `android/labels/<backend>/res/values/strings.xml` 四份，
      `.github/workflows/android.yml` 在打包循环里复制到
      `build-apk/app/src/<backend>/res/values/`，`android/app.build.gradle`
      为每个 flavor 显式声明 `res.srcDirs`。AGP 资源合并里 flavor 优先于
      main，同名 `app_name` 被覆盖；**不改 SDL 上游文件、不改 manifest**。
    - 防漂移：workflow 用 `grep -F` 校验 desktop 源码的窗口标题与
      `android/labels/` 的 `app_name` 一致，任一侧改动不同步即失败。
      desktop 源码是唯一数据源。
    - 验证：4 份 XML 均可解析、映射本地模拟全成立、`--check` OK（1170 条，
      生成物零变化）、`git diff --check` 0；桌面构建 / ctest 不受影响。
    - 注意：本地无 Android SDK/Gradle，flavor 资源覆盖是否生效以 CI 产出的
      APK 安装结果为准；launcher 名称约 15 字符，部分启动器可能截断。
26. **四端相对资源路径根治「贴图又不见了」（2026-11-12）**：`example_cpp`
    从 IDE 默认调试目录 / 仓库根启动时，玩家与敌人退化为纯色块。根因是
    `load_assets()` 传 `sprites/player.png` 这类相对路径，而 `load_png()`
    直接交给 `SDL_LoadFile()` 按**进程 CWD** 解析；`examples/CMakeLists.txt`
    注释声称「桌面靠 chdir 到 exe 目录」，但源码里从未有 chdir（只有
    `csharp_host` 自己用 `SDL_GetBasePath()` 做过）。本地实证：从
    `build/Debug` 启动无告警，从仓库根启动必现
    `[warn] missing sprites/player.png` / `enemy.png`。
    - 按用户要求**全程相对路径、不使用 `SDL_GetBasePath()`**（Android 返回
      `"./"`、iOS 返回 bundle 路径，跨平台语义不一致）。
      `examples/mobile_display.h` 新增四端共享的
      `joy_examples_align_working_dir(argv0)`：只从 `argv[0]` 取最后一个
      `/` 或 `\` 的目录部分并 `chdir`；Android / wasm 为空实现。
    - 四个宿主在 `SDL_AppInit` 最前面调用。`cpp/src/host.cpp` 的
      `load_game()` 移除 `SDL_GetBasePath()`，桌面改相对
      `"cpp/JoyGameCpp.dll"`、Android 仍用裸库名；`csharp_host` 删掉旧的
      `chdir_to_exe_dir()`，`MONO_LIB_DIR` 改相对 `"lib"`，Linux/macOS 的
      CLR shim 预加载改 `"./<name>"`（裸库名只搜系统路径）。
      `examples/` 下 `SDL_GetBasePath` 实际调用归零。
    - `cpp/src/game.cpp` 的 `game_init()` 增加 `JoyApi` 校验：
      `abi_version` 相等且 `size` **严格等于** `sizeof(JoyApi)`。vtable
      按 IDL 顺序生成、新增指针可能插在中间，`size >=` 拦不住反向错配；
      错配时 `ImageCreate` / `ImageDrawRect` 静默指到别的 API，只表现为
      贴图变块。
    - 防复发：`.github/workflows/desktop.yml` 新增
      `Verify relative assets from foreign CWD`，从仓库根启动
      `example_cpp --smoke 3`，输出含 `missing sprites/player.png` 即失败
      （ctest 从构建树内跑，抓不到这类回归）。CMake / README 里与实现不符
      的 chdir 注释同步改正。
    - 验证：四 target 全量重编 0 错误、`--check` OK（1170 条）、`ctest` 4/4、
      从仓库根四端冒烟均 `exit=0` 且无 missing 告警、从资源根对照通过、
      lint 0、`git diff --check` 0。
    - 保留：`src/jr3d.c` 的 `joy_r3d_load_texture_png()` 仍有一处
      `SDL_GetBasePath()` 兜底（引擎层 r3d，不在四端 2D 链路内）。

    具体内容：

1. **`image_draw_rect`**（`include/joy/japi.h` 末尾、`src/jr2d.c`）
   全标量 L1，`srcrect` 固定 NULL，坐标是**像素**。绕开 `image_draw1` 的
   `jrect_t*`（L3）。四份示例的 `draw_sprite` 都改用它，缺图才退色块。
2. **`structs.js` 的 `TYPE_ALIASES { ft_t: 'float' }`**
   `jmath.h` 用 `#define ft_t float`，不归一化会把 `vec2_t` 等判成不可展开。
   归一化后可展开 L2 从 4 → 33，显式开放 26 条（跳过 2 条展开后 17 参数超
   `LUAX_MAX_ARGS=16`、1 条 L2 返回句柄）。
3. **三份脚本的线段坐标系**：`ShapeDrawLineF` → `ShapeDrawLine`（前者归一化）。
4. **仓库卫生**：`examples/csharp/obj` 取消跟踪（16 个带本机绝对路径的还原产物，
   工作区文件保留）；根 `CMakeLists.txt` 加 `enable_testing()`；新增桌面 CI
   `.github/workflows/desktop.yml`（现为 Win / Linux / macOS 三平台 matrix）。
5. **C++ 双形态**：`examples/cpp` 新增 option `JOY_GAME_STATIC`（iOS/wasm 默认 ON）。
   静态形态把 `game.cpp` 编进宿主、链接期解析 `joy_game_get_hooks`；动态形态保留
   `SDL_LoadObject`，Android 产物落 `lib/<abi>/` 并用裸库名让 linker 查找。
   `game.cpp` 不感知形态。桌面两种形态都验证过。
6. **C# 的 Android**：运行时入库、BCL 分平台、CMake 平台推导、`libmain.so`、
   宿主 BCL 解压到内部存储、`android.yml`。

## 4. 关键技术事实（最容易踩错的地方）

### CLR 的真实来源
`runtimes/` 里的运行时来自微软**官方 NuGet** 的 MonoVM runtime pack
`Microsoft.NETCore.App.Runtime.Mono.<rid>` **8.0.17**（曾误记为「自定义构建、
非官方分发包」，已在 `runtimes/README.md` 更正）。

- Windows 包的主 DLL 名就是 `coreclr.dll`（apphost 按此名查找），
  Android 包是 `runtimes/android-arm64/native/libmonosgen-2.0.so`。
- 两者都导出 6 个 `coreclr_*` + 1300+ 个 `mono_*`（dynsym 各 1328 个），
  所以 `modules/cs/jcsx.c` 的 `csx_*` 嵌入代码跨平台不用改。
- **仓库里 `windows-x64/coreclr.dll` 例外**：它内嵌
  `C:\Users\liwei\Downloads\runtime-8.0.17\src\mono\...` 调试路径，是从
  dotnet/runtime 8.0.17 源码在本机编译的那一份。官方包二进制只有相对路径
  `src/mono/mono/mini` 与版本串 `8.0.1725.26602 @Commit: 77545d6fd5ca`。
- `coreclr.import.lib` 是从该 DLL 生成的导入库，NuGet 包不附带。

### BCL 不能跨平台共用
`System.Private.CoreLib.dll` 按 RID 构建，实测：

| 文件 | windows-x64 | android-arm64 | 字节差异 |
|---|---|---|---|
| CoreLib | 4,476,416 | 4,361,480 | 91.15%，Windows 含 `kernel32`、Android 含 `liblog` |
| netstandard | 101,176 | 101,144 | 6.25% |
| System.Runtime | 43,792 | 43,832 | 23.25% |
| System.Collections | 102,168 | 102,176 | 46.33% |

所以 `runtimes/` 结构是**每个平台目录自带 4 个 BCL**，`examples/csharp_host/CMakeLists.txt`
只取 `<MONO_LIBRARIES 所在目录>/libs/`，缺失就 `FATAL_ERROR`，**没有向其它平台回退**。
不要再加回退逻辑——曾经的根目录 `runtimes/libs/` 就是这么把 Windows 版静默拷给别的平台的。

### `_f` 是归一化，`_rect` 是像素
`r2d_fpx()` 会乘窗口宽高。历史上把像素坐标传给 `_f` 系列，导致 software renderer
绘制约 `12480×8320` 的纹理，冒烟进程 CPU 跑满卡死。**新增 API 命名要区分清楚**。

### `SDL_GetBasePath()` 不能 free
SDL3 返回内部静态缓存，`SDL_free` 会在退出时堆损坏（0xC0000374）。

### 生成链路的正确操作顺序
1. 改 `include/joy/japi.h`（**声明放文件末尾**，避免 IDL `source` 行号整体漂移）
2. `node api/tools/gen_bindings.js --scan`
3. `node api/tools/gen_bindings.js --enable name:<api_name>`（必须带 `name:` 前缀）
4. `node api/tools/gen_bindings.js --check`
   （`--enable` 会直接 emit；`--check` 只有在 emit 之后才通过）

### 公开 API 命名（P1 已完成）
`j` 前缀是最初设计的项目命名空间，后来没贯彻，IDL 里一度长出 85 个 domain。
现已统一为 `joy_<域>_<动作>`，10 个域的封闭白名单在
`api/tools/naming.js` 的 `DOMAINS`（归并表与门禁说明见 `api/README.md`）。
`--check` 只卡新增，存量 1156 条冻结在 `api/idl/legacy_names.txt`。
**迁移已收尾**：`api/idl/rename_map.json` 现有 **1150 条**旧名→新名映射
（2026-11-10 清掉 4 条无声明的 `utils_aes_*` 死映射），`--scan` 据此继承
`status`/`owns`/`note`；`include/joy/compat.h` 与 `include/joy/jcont.h` 均已删除，
全仓不再有 `#define` 桥接。`legacy_names.txt` 仍须保留（6 条例外靠它豁免）。
`script`(33) 与 `core` 域里的
`joy_core_utils_*`(20) 因带函数指针参数或属有意排除项，维持 X 级（不进
thunk/vtable/C# 绑定）。另有 6 条 `joy_fov_*`/`joy_tilemap_sweep_*` 域名不在
10 域白名单（见 §2），因 legacy 豁免不被门禁拦截。脚本层 PascalCase 方法名由
`ALIASES` 冻结，C 名改了不受影响。**新增声明仍放 `japi.h` 末尾，改名必须原位
替换**，避免 IDL 的 `source` 行号整体漂移。

### 其他
- `examples/csharp/obj` 已从版本库移除，`.gitignore` 有 `obj/`，不要加回来。
- `ctest` 需要根级 `enable_testing()`，只在 `examples/` 里写不生成根 `CTestTestfile.cmake`。
- 本机没有 NDK / Xcode / Emscripten；有 Node、Python(含 pyyaml)、dotnet 8、VS2022。

## 5. 如何验证（本机可跑）

```powershell
cd e:/c++/joy
node api/tools/gen_bindings.js --check          # 生成门禁
cmake -S . -B build                              # 配置
cmake --build build --config Debug               # 全量构建
ctest --test-dir build -C Debug --output-on-failure --timeout 25   # 四语言冒烟 4/4
```

冒烟必须带 `--timeout`：历史上出现过渲染卡死拖死整轮测试。

检查 Android 产物布局（本机无法编译，只能看文件）：

```powershell
Get-ChildItem runtimes/android-arm64 -Recurse -File   # 主库 + 4 个 libSystem.*.so + 4 个 BCL
```

## 6. 明确未验证 / 已知风险

| 项 | 状态 |
|---|---|
| Android 交叉编译 | **已多次在 CI 跑通**（`Android APKs` 工作流 `success`）；本机无 NDK，仍靠 CI 验证 |
| Web / WASM | **已部署 GitHub Pages**（仅 push 触发），站点 https://coder210.github.io/joy/ 可访问；方案 A（只发 cpp/lua/js）已落地 |
| APK 打包 | **已完成**：`android/app.build.gradle` 4 个 productFlavor，`android.yml` 的 `assembleDebug` 产出 4 个 APK 并校验清单、上传 Artifacts |
| 根构建的 CLR 探测 | **已修复**（2026-11-10）：根 `CMakeLists.txt` 已与 `examples/CMakeLists.txt` 同步，支持 `JOY_CLR_PLATFORM` / Android ABI 映射 / 全部平台库名；`-DJOY_CLR_PLATFORM=android-arm64` 在根入口实测能找到 `libmonosgen-2.0.so`。**Android toolchain 真配置仍只能靠 CI** |
| Android 真机冒烟 | 未做。桌面 `--smoke` 走 dummy 驱动，不能替代 |
| iOS / wasm（C++/lua/js） | `JOY_GAME_STATIC` 静态形态只在桌面验证过，两平台工具链都没跑 |
| C# 在 iOS / wasm | iOS 禁 JIT。wasm 不是「.NET 8 不支持」：.NET 8 正式支持 `browser-wasm`，但那是 SDK 托管的 interpreter 应用；joy 的 C# 后端是嵌入 MonoVM 的宿主 + `InternalCall` 绑定，宿主模型不兼容。Web 已定方案 A：只发 cpp / lua / js，见 `examples/README.md` 平台矩阵 |
| `runtimes/windows-x64/coreclr.dll` | 是本地编译版，可考虑换成官方包同名 DLL |
| `src/jgltf.c` | **预存语法错误**，`JOY_ENABLE_GLTF` 默认关、不编译；文件本身待修 |

## 7. 项目收口（2026-11-11）

**收口时没有计划内工作。** 前几节是已完成的实现与已知事实，本节剩下的是
「已评估后明确排除的方向」——**不是待办，不要再从中挑任务继续做**。
若以后出现新需求，另开议题，不要把这里当 backlog。

> **后续议题：C 类型名统一**（`joy_<概念>_t` / `_p`）——2026-11-11 **已完成**，
> 分 6 批提交，任务书 `TYPE_RENAME_TASK.md` 头部有完整执行结果与口径差异说明。
> 新增 `api/idl/{type_rename_map,private_type_map,handle_aliases,macro_rename_map}.json`
> 与 `api/tools/rename_types.js`（支持 `--build-map/--dry-run/--write/--audit/--trim-ws`）。
> 要点：脚本层 57 个句柄类名由 `handle_aliases.json` 冻结，C 类型改名后 C#/vtable
> 类名逐字不变。`struct tag` **已于 2026-11-13 补做统一**：147 个 tag 定向改名
> 为 `joy_*`（含 `jevent_callbacks → joy_event_callbacks`），现全仓 161 种 tag
> 全部合规，详见 `TYPE_RENAME_TASK.md` 头部「执行结果」。

### 交付范围（已完成）
生成链路（IDL → thunk / vtable / C# / 四语言示例）、公开 API 命名统一、
`borrowed` 借用语义、单参数 mesh dtor、四平台 CLR 运行时入库、Android 4 个
APK、Web Pages 部署、桌面 / Android / Web 三套 CI。当前 **1157 条 IDL，
721 条 `status:on`**；验收结果见 §10。

### 已完成，不要当待办
Android CI + 4 个 APK、Web Pages 部署、P1 命名统一、两轮改名收尾、根 CMake
的 CLR 探测统一、`canThunk()` 参数上限、`SELFTEST_SAFE` 白名单修正、
`SKIP_NAMES` 清理、**借用句柄语义（机制 A，7 条已开放）**、**单参数 mesh dtor
（机制 B，13 条已开放）**。均见 §3。

### #1 的真实状态（2026-11-11 核实；A、B 均已实现）

`api/generated/coverage.md` 那 38 条**不是「还没裁决」**：IDL 的 `note` 已逐条写明
所有权语义，缺的是生成链路的机制。精确分组：

| 分组 | 条数 | 代表 | 为什么填不了 `owns` |
|---|---|---|---|
| 借用句柄 | **12** | `joy_core_get_raw_window`、`joy_r2d_unwrap`、`joy_r3d_get_device`；9 条 `joy_scene_*`（`tilemap_find_*_layer`、`node_get_userdata`、`find_node_by_name*`、`get_root_at`、`get_popup_node`、`get_userdata`、`manager_get_current`） | 填了会让生成链路注册析构 → double free。正确语义是「**不注册析构**」，但 IDL 没有这个状态 |
| 新建但 dtor 不可用 | **26** | 全是 `joy_r3d_*`：11 条新建 SDL GPU 对象（`load_shader` / `create_pipeline` / `upload_buffer` / `create_*_texture` / `create_sampler` / `create_*_pipeline` / `load_texture_png` / `gen_checkerboard`），15 条 `mesh_*` 系列 | 释放需 `(device,obj)` 双参数且不在 `joy.h`；`r3d_mesh_destroy(jr3d_p, render_mesh_p)` 也是双参数 |

卡点在 `api/tools/emit_thunks.js` 的 `ownsIssue()`：只接受「**单参数、参数类型与
`ret` 严格相同**」的析构函数，且没有「借用」状态。L1 的 130 条 `status:off` 里
有 79 条无 `owns`，主要就是这批。

**两条机制（A、B 均已实现）**：

- **A. 借用语义 —— 已实现（2026-11-11）**：IDL 加 `borrowed` 人工字段，
  `ownsIssue()` 放行且不注册 dtor，thunk 用 `joy_obj_register(..., NULL, ...)`；
  `--check` 校验与 `owns` 互斥，`coverage.md` 单列统计。12 条借用里 **7 条
  typed 句柄已开放**（`status:on` 701 → 708）；另 5 条返回 `void*` 因 C#
  强句柄 / C++ vtable 不支持 `void*` 而保持 `off`。见 §3 的 14。
- **B. 单参数 dtor 包装 —— 已实现（2026-11-11）**：`src/jr3d.c` 给
  `render_mesh` 加 `owner` 回指、11 处分配统一走 `r3d_mesh_alloc(r)`，新增公开
  API `joy_r3d_mesh_destroy_one(render_mesh_p)` 内部转调双参数
  `joy_r3d_mesh_destroy(m->owner, m)`；15 条 mesh 的 `owns` 全部指向它，
  **13 条已开放**（`status:on` 708 → 721）。见 §3 的 15。

**仍未开放的条目**（不是遗漏，是类型 / 签名限制，均保持 `off` 且已写入 note）：
B 的 26 条里 13 条开放、另 13 条保持 off（11 条返回 `void*` 的 SDL GPU 对象，
2 条 `mesh_custom*` 含 `void*` 缓冲参数需手写适配）；A 的 12 条借用里 5 条
`void*` 保持 off。C# 强句柄（`canEmitCs`）、C++ vtable（`okHandle`）与
`canOpen` 共同排除 `void*`，**不要为了凑数放开**。

改生成器 / 引擎后必须全量回归：`--scan` 幂等 + `--check` + 全量构建 +
`ctest` 4/4 + 三个 CI 工作流。

### 明确不做（已评估，非待办）

| 方向 | 结论 |
|---|---|
| 继续扩容 `status:on`：L3 出参通道 / 放开 X 级 73 条 / 放开 `void*` 句柄 | 不做。L3 要给 `joy_value` 加出参通道并四端同步，属于新架构；X 级当初有意排除；`void*` 没有强句柄类型，放开会破坏类型安全 |
| Android 真机 / iOS / wasm 工具链验证 | 不在本机范围（无 NDK / Xcode / Emscripten），现有 CI 已覆盖能覆盖的部分 |
| `src/jgltf.c` 修语法错、Windows `coreclr.dll` 换官方包 | 低价值清理，不影响现有构建；等功能真被用到再说 |
| 6 条 `joy` 域例外、全仓剩余 16 处历史旧名 | **有意保留，不是漏网**，见 §2 / §3 的 13 / §10 |
| 再做一轮**函数名** / 文档「收尾」 | 不做。已全仓审计并清完该清的，重复清理只产生噪声。类型名统一是另一件事，见 `TYPE_RENAME_TASK.md` |

**完成判定**：`--check` 通过 + 全量构建 0 错误 + `ctest` 4/4 + CI 三工作流
success。四项均已满足，所以没有遗留任务。

## 8. 提交风格

中文、`type(scope): 摘要`，正文用 `- ` 列改动点、最后一行写验证结果。
参考 `git log -3`。

## 9. 自动化踩坑（历史经验）

本仓库的改名/生成链路在 PowerShell + 中文环境下有几个反直觉的坑，按发生频率排序：

1. **`node -e "..."` 内联脚本里的 `${}` 会让整条命令失效。**
   PowerShell 会把正则 `/[.*+?^${}()|[\]\\]/g` 里的 `${}` 当变量插值，
   抛出「空的 ${} 变量引用」，且**整条命令（含前面已写好的 `node .tmp-x.js`）都不会执行**。
   含正则/特殊字符时一律把脚本写成 `.tmp-*.js` 文件再 `node` 执行，验证也写文件，别用内联 `-e`。

2. **`rename_map.json` 用 `JSON.parse/stringify` 重写，别用字符串插入。**
   曾用 `s.lastIndexOf('\n}')` 在末尾插入，碰到 CRLF 时 `\r` 被甩到逗号前面，
   整文件行尾错乱、diff 上千行。正确做法：
   `JSON.parse` → 改对象 → `JSON.stringify(obj, null, 2).split('\n').join(eol) + eol`，
   其中 `eol = raw.includes('\r\n') ? '\r\n' : '\n'`（原样保留换行风格，避免大 diff）。

3. **批量改名用字符类边界，别用 `\b`，别用子串替换。**
   正则 `(^|[^A-Za-z0-9_])name(?=$|[^A-Za-z0-9_])`：
   - 不用 `\b` —— 经 PowerShell 传递后 `\b` 会变成退格符（U+0008），匹配数为 0 且文件不动。
   - 不加边界会误伤子串（如 `r2d_create` 命中 `r2d_create_pipeline`）。
   - 对一组名字批量替换时，旧名互不为前缀子串（如 `r2d_create` 与 `joy_r2d_create_texture`）
     靠边界互不干扰，顺序无所谓。

4. **`jmath` 220 条不手写，走 `jmath_wrap.js`。**
   `jmath.h` 的 134 个 `JOY_INLINE` 内联函数（名字本就无前缀，如 `fp_from_float`/`vec2_add`）
   由 `jmath_wrap.js` 套前缀生成 `jmath_wrappers.h/.c` 后再扫进 IDL。
   改名点是该脚本里 3 处 `'<prefix>_' + f.name`（已改为 `'joy_math_' + f.name`），
   不是 `api.h`。改完必须重跑 `node api/tools/jmath_wrap.js` 重新生成包装头。

5. **`compat.h` 已删除，不要重建。**
   P1 收尾把 `src/`/`examples`/`modules` 共 57 个手写源文件的 1881 处旧名
   **真正改成 `joy_*`**，兼容宏清空后该文件连同 `joy.h` 的 include 一起删除。
   后续新增改名直接改源码，不要再靠 `#define` 桥接。
   同理 `jcont.h`（900 行孤岛容器头）已删除：它不被 `joy.h` 链包含，
   仅 `src/jutils.c` 有一行死引用，且其中符号从未被使用。

6. **`--scan` 会丢弃「源码里已不存在」的 IDL 过期条目。**
   改名前务必先 `--scan` 确认真实条目：曾发现 IDL 里残留 `utils_mouse_pos`/
   `utils_aes_*`/`utils_file_size` 等早已删除的函数，scan 后自动消失，
   若基于过期 IDL 改名会改错名字。

7. **X 级（不暴露）靠 `SKIP_DOMAINS` 或 `SKIP_PREFIXES` 维持。**
   `script`（改 `joy_script_*`）、`utils`（改 `joy_core_utils_*`）因带函数指针参数/
   属有意排除项，改名后仍判 X 级、不进 thunk/vtable/C# 绑定。若要暴露，
   去掉 `gen_bindings.js` 里对应的跳过项即可，命名已就位。

8. **本机无 NDK / Emscripten，CI 只能靠 GitHub REST API 查。**
   `Invoke-RestMethod -Uri "https://api.github.com/repos/coder210/joy/actions/runs?per_page=10"`
   可看三个构建型工作流（Android APKs / Desktop examples / Web examples）的状态；
   第四个 `cleanup-artifacts.yml` 是定时清理，没有 push 运行记录。出网正常。
   Web 站点本身用 `Invoke-WebRequest` 直接 GET `https://coder210.github.io/joy/` 验证。

9. **类型名统一（P2）新增的坑**：见 `api/tools/rename_types.js`。
   - 替换工具**必须把自己加入排除名单**：它内含旧映射表字面量，否则 `--write` 会改坏自身。
   - 映射表**只在 `--build-map` 时从公开头采集一次**，运行期一律读文件。若每次运行时
     重新采集，批 2 改完头后会算出 `joy_x -> joy_x` 自映射。
   - 采集类型名要**剥离注释**：`src/jr3d.c` 与 `api.h` 注释里的 `r3d_p` 是少写 `j` 的
     笔误，进映射表会把冻结表 `joy_r3d_p` 的类名从 `Jr3d` 覆盖成 `R3d`。`（注：这里的 `api.h` 指原主头，现已改名 `japi.h`）
   - `struct tag` **不能全词替换**：`scene` / `image` / `label` / `button` 等通用词
     同时是变量名、字段名、参数名（实测 `scene` 379 处裸用）。2026-11-13 改用
     `(\bstruct\s+)tag` **定向替换**，只匹配 `struct <tag>` 这一模式，147 个 tag
     统一为 `joy_*` 且不动同名变量，`--check` 与全部生成物零变化。
   - 类型与 tag 在 C 里属不同命名空间；生成器只按 typedef 别名采集（`structs.js`
     的正则里 `\w*` 只是跳过 tag），所以 tag 改名不影响 IDL / 生成物 / 脚本层类名。
     排除项：PascalCase 强句柄类名（脚本层冻结）、`http_message` / `_Mono*` /
     `stat` 等系统与第三方 tag。
   - `--trim-ws`：改名后原本带行尾空格的行变成 diff 新增行，会被 `git diff --check`
     抓到；只清含新名行的行尾空白，不增删行。
   - PowerShell 里内联 `node -e` 的 `\\\\b` 会被叠加转义成匹配字面反斜杠，导致
     「0 命中」的假结果；校验一律写成临时 `.js` 文件执行。

## 10. 交付验收（已完成）

| 项 | 结果 |
|---|---|
| `node api/tools/gen_bindings.js --check` | OK（1157 条，6 个生成物最新） |
| `cmake --build build --config Debug` | 0 错误，构建不依赖 `compat.h` |
| `ctest --test-dir build -C Debug --timeout 25` | 四语言冒烟 4/4 |
| GitHub Actions 三个构建型工作流 | success（见 §9.8） |

以下是**有意保留的事实**，别误当成缺陷去「修」：

- 6 条 `joy` 域例外（L3 / `off` / 不暴露）与全仓剩余 16 处历史旧名，见 §2 / §3 的 13。
- `void*` 返回 / 参数被 `canEmitCs` / `okHandle` / `canOpen` 排除；借用句柄靠 `borrowed` + `dtor=NULL` 实现。
- mesh 句柄不得比创建它的 r3d 上下文活得久：`joy_r3d_mesh_destroy_one` 依赖 `m->owner`。

