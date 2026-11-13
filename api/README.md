# joy 绑定 API 契约（P0 / P1 / P2）

这里放**一份与脚本语言无关的接口清单**，以及维护它的工具。目标是让 Lua / JS / C#
三个后端共用同一套对象模型、方法名、参数与所有权规则。

> **边界**：本目录只产出契约、报告，以及可被三后端共用的 C 运行层 / 薄适配生成物。
> 旧的 `modules/*/bindings/` 手写胶水一行未删，新旧链路并行；哪些 API 真正开放由
> `joy.idl` 的 `status: on` 决定（当前 668 / 1155，含 L2 试点 2 条、jmath 包装 220 条）。**开放 ≠ 业务已调用**。
> 所有生成物都提交进仓，`git diff` 随时能看清改了什么。

## 文件

| 文件 | 谁维护 | 作用 |
|---|---|---|
| `gen_bindings.js` | 人工 | 解析 `include/joy/{defs,api}.h` + `generated/jmath_wrappers.h`、维护 IDL、产出全部生成物。零 npm 依赖，只用 Node 内置 `fs` / `path` |
| `naming.js` | 人工 | **三端共用**的命名与句柄判定规则，避免各后端长出自己的方言 |
| `emit_thunks.js` | 人工 | 共享 thunk / 注册表生成器（纯字符串函数） |
| `emit_cs.js` | 人工 | cs 后端薄适配生成器，产出 C icall 与 C# 强类型包装 |
| `emit_vtable.js` | 人工 | C++ 后端强签名 vtable 生成器：覆盖 `status:on` 的 L0/L1 与可展开的 L2，支持标量 / `const char*` / 强句柄类型 |
| `structs.js` | 人工 | 值类型结构体布局解析（含 `ft_t` → `float` 别名归一化）+ L2 参数按字段展开，四个生成器共用 |
| `jmath_wrap.js` | 人工 | 解析 `jmath.h`，生成 `generated/jmath_wrappers.{h,c}`；作为模块被 `--check` 直接调用 |
| `generated/jmath_wrappers.h` | 工具生成，勿手改 | 220 条全标量 `JOY_API` 声明，第二个契约输入源；`joy.h` 只留一行 include |
| `generated/jmath_wrappers.c` | 工具生成，勿手改 | jmath 数学函数的外部链接包装实现，由 `cmake/JoySources.cmake` 显式收编 |
| `joy.idl` | 工具初始化 + 人工补充 | 唯一真相源。JSON 数组、**每条 API 占一行**，便于 diff |
| `coverage.md` | 工具生成，勿手改 | 分级统计、按域分布、`owns` 待裁决、L3 手写清单 |
| `thunks_generated.c` | 工具生成，勿手改 | `status: on` 的 L0/L1 及可展开的 L2 会生成 thunk、注册表与自检 |
| `generated/joy_api_vtable.h` | 工具生成，勿手改 | 721 个强签名函数指针 + 56 个句柄薄包装 typedef（`jr2d_p`→`Jr2d`，编译期不可混传）+ `abi_version` / `size` |
| `generated/joy_api_vtable.c` | 工具生成，勿手改 | 721 个 shim（强签名 ↔ `joy_value`）、宿主侧填表 `joy_api_vtable_init()`、自检 `joy_api_vtable_selftest()` |
| `modules/cs/bindings/api/api_generated.c` | 工具生成，勿手改 | Mono 值 → `joy_value[]` → `joy_api_call()`，注册 `Joy.Api.Native::*` |
| `modules/cs/scripts/Api.generated.cs` | 工具生成，勿手改 | `Joy.Api.Bindings` / `Diagnostics` / 强类型句柄类，public API 无 `IntPtr` |
| `modules/lua/bindings/lapi.c` | 人工 | Lua 薄适配：遍历注册表、`joy_value` ↔ Lua 值、句柄 userdata `__gc` |
| `modules/js/bindings/japi.c` | 人工 | JS 薄适配：遍历注册表、`joy_value` ↔ duktape 值、句柄对象 finalizer |
| `runtime/joy_value.*` | 人工 | 统一值容器（标量 / 字符串 / 句柄） |
| `runtime/joy_obj.*` | 人工 | 句柄表、引用计数、类型 tag 校验、自动析构 |
| `runtime/joy_thunk.*` | 人工 | thunk 签名 / 三端共用注册表，以及按名调用入口 `joy_api_call()` |
| `joy_api.h` | 人工 | 对外唯一入口：`joy_api_selftest()` |
| `idl/legacy_names.txt` | 工具冻结，只减不增 | P0 冻结的存量 API 名，命名门禁的豁免基线。**不能删除**：6 条 `joy_fov_*` / `joy_tilemap_sweep_*` 例外的域名不在白名单，靠它过门禁 |

> `include/joy/compat.h`（旧名兼容层）与 `include/joy/jcont.h`（孤岛容器头）已于
> 2026-11-11 删除：前者在 P1 收尾清空宏块后已无内容，后者仅被 `src/jutils.c`
> 死引用且从未使用其中符号。两者都不在 `joy.h` 链与生成链内。

## 公开 API 命名规范（P0 起，分阶段迁移）

**目标格式**：`joy_<域>_<动作>[_宾语]`，全小写下划线，例如
`joy_core_create_window`、`joy_net_create_server`。`joy` 是项目命名空间，
`<域>` 必须取自 `api/tools/naming.js` 的封闭白名单 `DOMAINS`（唯一真相源，
新增域要改它并评审）。

C 层保持小写下划线：既避开 Win32 的 PascalCase 全局符号（`Windows.h` 下
`CreateWindow` 是 `CreateWindowW` 宏），又让 `domainOf()` 能按 `_` 分词。
脚本层与 C++ vtable 仍是 PascalCase，由 `naming.js` 的 `methodName()` 生成 ——
**两层命名是有意分层，不是不一致**。

### 域表（把现有 85 个自发生长的 domain 归并成 10 个）

| 域 | 吸收的历史 domain | IDL 条数（2026-11-13 实测） |
|---|---|---|
| `core` | env / window / log / error / ticks / quit / clipboard / event / mouse / utf8 / hash 系列 / game_timer / pack / unpack | 93 |
| `math` | jmath(220) / collision* / gjk* / box2d / tilemap 碰撞（shape 已移到 r2d） | 307 |
| `r2d` | r2d / font / jtext / jspine / jparticle / **image / text / animation / shape / fov(2D)** | 114 |
| `r3d` | r3d / jlight / jgltf / fov(3D) | 150 |
| `ui` | jui / jui_im / jlayout / 控件域 / **joystick** | 218 |
| `scene` | scene / game / jecs / jtilemap / janimator / jpm / **bt** | 135 |
| `net` | net* / tcp* / kcp* / ws* / iocp / netmessage | 87 |
| `audio` | audio | 20 |
| `input` | mouse / 输入状态快照 | 13 |
| `script` | modules / module / moudles / jmsgpack | 33 |

划分原则：按「谁会一起改 / 谁依赖谁」分；一个 API 只属一个域，跨域时归被
调用方；控件、协议、碰撞维度一律合并。域清单封闭，防止再次长到 85 个
（`GENERIC_FIRST` 兜底漏出的 `min` / `raw` / `should` 等垃圾域一并收编）。

**2026-11-13 起追加一条硬规则**：一个 `.c` 只承载一个域的公开接口，函数名
前缀必须与该文件的域一致；一个域可以有多个 `.c`（按子系统拆分，不合并成
巨型 TU）。域归属按**技术职责**判定，不按历史名：纹理 / 文字 / 图元 / 动画 /
FOV 等渲染层功能归 `r2d`（或 `r3d`），控件（joystick 等）归 `ui`，行为树归
`scene`，glTF 归 `r3d`。全仓 26 个含 `joy_*` 定义的 `.c` 已全部纯域、
0 混域；完整文件归属表见 `cmake/JoySources.cmake` 头部。

该规则由 `node api/tools/audit_domain_purity.js` 校验（CI 的 Domain purity
gate）：只统计**函数定义**（签名后紧跟 `{`），不把跨域调用、注释提及、共享
句柄类型（`joy_image_p` 等）算作混域。退出码非 0 即存在混域，必须修掉再提交。

改公开 API 的 C 名必须同步两处，否则 `--scan` 会重置 `status:on`、脚本层
方法名会退化成 `pascal(新名)`：`api/idl/rename_map.json` 追加「当前名 → 新名」
以继承 `status` / `owns` / `note` / `borrowed`；`api/tools/naming.js` 的
`ALIASES` 同步改 key、保留 value（脚本层方法名冻结）。

### 门禁与生成链路

- `--check` 只卡**新增**：不在 `idl/legacy_names.txt` 里的 `JOY_API` 必须匹配
  `joy_<白名单域>_`，否则报错。存量 1156 条豁免，删掉该文件即变成全量强制。
- `domainOf()` 对三段式名字取第二段；存量名字仍走 `GENERIC_FIRST` 旧分词，
  **P0 期间 IDL 的 domain 列一字不变**。全量迁移后 `GENERIC_FIRST` 退休。
- `SKIP_PREFIXES` 管旧前缀、`SKIP_DOMAINS` 管新域，两者并存到迁移结束。
- 迁移改名时在 `naming.js` 的 `ALIASES` 登记 `新C名 → 旧PascalCase名`，使
  三端方法名与 `api->R2dCreate` 保持冻结；不能用「剥掉 `joy_<域>`」自动推导
  （历史名如 `JanimatorCreate` 会坏）。
- P0 曾用 `compat.h` 放旧名 `#define` 桥接；P1 收尾已把 `src/` 与各模块里的定义与
  调用点真正改成 `joy_*` 新名，随后该文件连同 `joy.h` 的 include 一起删除。
  现在全仓不存在任何旧名兼容宏。

### 迁移顺序（P1 进行中）

**批次记录**：
- 第一批 `core/window` 子域 6 条（2026-11）。发现 `emit_vtable.js` 的
  `HANDLE_PROBE` 按名字硬编码，漏改会让强句柄自检被静默跳过。
- 第二批 `core` 剩余 39 条（hash / utf8 / lcg / env / log / event / fps /
  clipboard / local_ips），`core` 域累计 45 条。同步了 `SELFTEST_SAFE` 里
  5 个 hash 名。`src/jcore.c` / `jutils.c` / `jsys.c` 里的定义与调用点也同步
  改成 `joy_*` 新名（见 P1 收尾说明）。`push_*` / `game_timer_*` /
  `jtimer_*` 等 domain 碎片留待后续批次归并。
- 第三批 **碎片归位 30 条**：`pack/unpack` → `net`（12）、
  `jtimer/game_timer` → `scene`（15）、`push_*` → `core`（2）、
  `find_min_separation` → `math`（1）。至此 `GENERIC_FIRST` 兜底产生的
  伪域（`min` / `push` / `game` / `jtimer` / `pack` / `unpack`）全部消失，
  剩下的大域只有 `ui`（20+ 控件域）与 `net`（协议域）待合并。
- 第四批 **`net` 协议域合并 64 条**：`iocp` / `kcp*` / `tcp*` / `ws*` /
  `net*` / `netmessage` 全部并入 `net`，`net` 域累计 76 条（含上一批的
  pack/unpack）。剩下最大的就是 `ui`（约 230 条、20+ 控件域）。
- 第五批 **`ui-A1` 45 条**：即时模式 UI（`jui_*` → `joy_ui_*`）与栅格布局
  （`jlayout_grid_*` → `joy_ui_layout_grid_*`）并入 `ui`。这里开始用「剥离
  冗余前缀」而非无脑加域前缀：`jui_begin` → `joy_ui_begin`，不是
  `joy_ui_jui_begin`；`jui_process_event` 是 retained 控件的事件分发，改名
  时顺带明确为 `joy_ui_process_widget_event`（脚本名仍冻结为 `JuiProcessEvent`）。
  剩下 `ui-A2`（button / checkbox / radio / label / tooltip / popup / image /
  text，84 条）与输入类控件待迁。
- 第六批 **`ui-A2` 84 条**：image(14) / text(7) / label(5) / button(20) /
  radio(5) / checkbox(13) / popup(13) / tooltip(7) 并入 `ui`，`ui` 域累计
  129 条。`jtext_*`（jtext 域）不在这批，随 `r2d` 域一起迁。剩下输入类控件
  `inputbox`(28) / `combobox`(26) / `slider`(14) / `scrollview`(14) /
  `datagrid`(11) 约 93 条，以及 `ui` 的 `jui_im` 增量接口。
- 第七批 **`ui-B` 93 条**：`scrollview`(14) / `combobox`(26，含
  `combobox_input_*` 13) / `datagrid`(11) / `inputbox`(28) / `slider`(14) 并入
  `ui`，`ui` 域累计 222 条。至此 `ui` 主体收完，剩 `joystick`（归 `input`）、
  `jtext`（归 `r2d`）与少量 `jui_im` 增量接口。
- 第八批 **31 条**：`font`(5) / `jtext`(9) → `r2d`，`joystick`(17) → `input`。
  `r2d` 域现有 45 条（含尚未迁的 `r2d_*` 31 条），`input` 17 条。
- 第九批 **41 条**：`r2d` 本体 31 条（`r2d_create` → `joy_r2d_create`）+
  `jparticle` 10 条（→ `joy_r2d_particle_*`）并入 `r2d`，`r2d` 域累计 55 条。
  `jlight`(11) 与 `jgltf` 按域表归 `r3d`，留到下一批。
- 第十批 **30 条**：`jspine`(19，→ `joy_r2d_spine_*`) 并入 `r2d`，`jlight`(11,
  → `joy_r3d_light_*`) 并入 `r3d`。`jspine` 是 `JOY_USE_SPINE` 条件编译的
  有意排除项（19 条全是 X 级），迁名后必须在 `SKIP_PREFIXES` 里补
  `joy_r2d_spine_`，否则会从 X 掉回 L0/L1 混进 thunk 生成。`jgltf` 在 IDL
  里 0 条（构建默认关），无需处理。`r2d` 域 74 条、`r3d` 146 条。
- 第十一批 **`r3d` 本体 135 条**：`r3d_*` → `joy_r3d_<动作>`（去掉重复的
  `r3d_`）。这批声明散在 `api.h` 的 334–750 共约 25 个区块，改用项目自带的
  Node 工具链按 IDL 名单做精确批量改名（字符类边界，避免 `\b` 被 shell 转义
  成退格符），改完用 `git diff` 逐项核对。`r3d` 域 146 条，`Api.generated.cs`
  零变化说明 on 条目的方法名全部冻结成功。
- 第十二批 **`scene` 域旧 domain 收敛 145 条**：`scene_*` → `joy_scene_*`，
  `jecs_*` → `joy_scene_ecs_*`，`jtilemap_*` → `joy_scene_tilemap_*`，
  `janimator_*` → `joy_scene_animator_*`，`animation_*` →
  `joy_scene_animation_*`，`jpm_*`（程序化地图）→ `joy_scene_procmap_*`。
  `jpm_*` 的声明在 `defs.h` 而不是 `api.h`，批量脚本必须同时扫两个文件。
  同时同步了两处生成器硬编码：`emit_vtable.js` 的 `SELFTEST_SAFE` 里的
  `jpm_wave_seed`、`gen_bindings.js` 的 `SKIP_NAMES` 里的 `jecs_test`
  （后者迁名后仍要靠白名单保住 X 级）。`scene` 域累计 160 条，旧 domain
  全部归零。2026-11-10 收尾时把已不存在的 `jecs_test` 从 `SKIP_NAMES` 清掉，
  并把 `SELFTEST_SAFE` 里漏改的 `jmath_fp_from_float` 换成
  `joy_math_fp_from_float`（这条白名单名字写旧会让该 API 完全不进自检）。
- 第十三批 **math 类 92 条**：`collision2d/2df/3d/3df`、`gjk2d/3d/3df`、
  `box2d`、`shape` 统一到 `joy_math_<旧 domain>_<动作>`，`math` 域累计 93 条
  （含第三批的 `find_min_separation`），  这些旧 domain 全部归零。
- 第十四批 **42 条**：`sys`（网络环境，22 条）→ `joy_net_sys_*`、
  `audio`（20 条）→ `joy_audio_*`。`sys` 是 `net` 域的子命名空间，按
  `joy_net_sys_*` 收敛；`audio` 自成一个白名单域。`net` 域 98 条、`audio`
  域 20 条，旧 `sys`/`audio` domain 归零。
- 第十五批 **`jmath` 220 条**：这 220 条不是手写在 `api.h` 里，而是
  `jmath.h` 的 134 个 `JOY_INLINE` 内联函数，由 `jmath_wrap.js` 套 `jmath_`
  前缀生成 `jmath_wrappers.h/.c` 后被扫进 IDL（全部 L0 公开 API，会进
  vtable）。改名点是 `jmath_wrap.js` 里 3 处 `jmath_' + f.name` →
  `joy_math_' + f.name`，重生成后 IDL 名变成 `joy_math_*`，按 `math` 域收敛
  （顺带把原本不在白名单里的 `jmath` 域也纠正成 `math`）。脚本层名用 ALIASES
  冻结为 `Jmath*`，`jmath_wrappers.c/.h` 已用新名重生成；`src/jgjk.c` /
  `jbox2d.c` / `jcollision.c` 等旧名调用点也同步改名。`math` 域 313 条。
- **P1 收尾（真正改名）**：前 15 批只改了 `api.h` / `defs.h` / 生成物里的声明，
  `src/*.c` 与各模块定义、调用点仍写旧名，靠 `compat.h` 的 `#define` 在预处理期
  切到新符号——等于"只重定义了名字、没改源码"。收尾阶段用字符边界批量替换，把
  `src/`（jui.c 538、jr3d.c 281、jnetwork.c 187、jr2d.c 143 …）、`examples/`、
  `modules/` 共 57 个手写源文件里的 1881 处旧名全部改成 `joy_*` 新名；改写后
  手写源零旧名残留，且**不再需要兼容宏**即可编译通过；`compat.h` 当时清空宏块
  留作占位，2026-11-11 已连同 include 一并删除。
- **`script`（33 条）与 `utils`（20 条）收尾**：二者都曾因带函数指针参数 / 属于有意排除项
  而留在 `SKIP_DOMAINS` 里（X 级，不进 thunk/vtable/C# 绑定）。按同样方式把它们的
  定义与调用点真正改名——`script` → `joy_script_*`（仍归 `script` 域、X 级），
  `utils` → `joy_core_utils_*`（归 `core` 域、靠 `SKIP_PREFIXES` 维持 X 级）；
  旧 `utils`/`jspine`/`modules`/`module` 等死域从跳过清单里清掉。

映射见 `idl/rename_map.json`（累加式）。

`rename_map.json` 只写**本批**的旧名 → 新名，可以换成下一批也可以累加
（`mergeIdl` 在旧条目已消费时会回退按本名匹配，重复 `--scan` 不会把
`status:on` 清掉）。

0. 改名前先搜 `api/tools/` 里的硬编码旧名（`SELFTEST_SAFE`、`HANDLE_PROBE`、
   `SKIP_NAMES` 等）——`--check` 只比对生成物，这类漏改查不出来。
1. 每批一个域：`--scan → --enable domain:<域> → --check` → 四语言 `ctest`
   → Desktop / Android / Web CI 全绿 → 下一个 commit。
2. 先 `core` / `audio` / `input` 小域试通，再合并 `ui`（20 域 → 1）、`net`，
   最后 `math` / `r3d` 大域；`jmath_wrappers.h` 的 220 条走 `jmath_wrap.js`。
3. **改名必须原位替换**，不要增删 `api.h` / `defs.h` 的行，否则 IDL 的
   `source` 行号整体漂移（见 `HANDOFF.md`）。
4. `legacy_names.txt` **必须保留**：6 条 `joy_fov_*` / `joy_tilemap_sweep_*` 例外的
   域名回退成白名单外的 `joy`，删掉会让 `--check` 立刻失败。`compat.h` 已删除。

### vtable 的自检边界

`joy_api_vtable_selftest()` **只**对 `emit_vtable.js` 里 `SELFTEST_SAFE` 列出的
确定性纯函数（hash 系列、`joy_scene_procmap_wave_seed`、
`joy_math_fp_from_float`）做「强签名调用 vs 直接 C 调用」数值比对，另用
`joy_core_window_get_fullscreen({0})` 验证强句柄的无效值会被 thunk 拒绝而不是
崩溃。表里其余 690+ 条 API（`joy_core_create_window` / `joy_net_sys_bind` /
`joy_audio_create_*` / `joy_core_quit_env` 等有副作用或依赖外部状态）
**不参与运行时调用**，由编译期签名、`joy_api_vtable_init()` 填表和
`--check` 生成物一致性保证。新增纯函数到白名单前请确认它无副作用、结果确定。

`status:on` 的 L2（`joy_r2d_text_printx` / `joy_r2d_shape_draw_rectangle` /
`joy_r2d_text_printf` 等 30 条）已按 `structs.js` 的字段布局展开成标量进表，
与 thunk 表 argc 一致；`structs.js` 会把 `jmath.h` 的 `#define ft_t float`
归一化，否则 `vec2_t` / `mat22_t` 之类会被判成不可展开。L3（结构体指针 /
输出参数）仍是 `status:off`，不在表内；图片绘制改由全标量 L1
`joy_r2d_image_draw_rect` 绕开 `jrect_t*`，四份示例已恢复真实贴图。剩下的
退化只有文字宽度按字号估算（`joy_r2d_text_measurex` 是 `int*` 出参，暂无
对应标量包装）。

## 命令

在任意目录执行（路径都相对脚本自身）：

```powershell
node third_party/joy/api/tools/gen_bindings.js            # 重生成报告 + thunks + vtable + cs glue
node third_party/joy/api/tools/gen_bindings.js --emit     # 同上（显式写法）
node third_party/joy/api/tools/gen_bindings.js --scan     # 从两个契约头更新 joy.idl + 重生成全部产物
node third_party/joy/api/tools/jmath_wrap.js              # 改过 jmath.h 后先跑：重生成 jmath_wrappers.{h,c}
node third_party/joy/api/tools/gen_bindings.js --enable L0|L1|all|domain:<名>|name:<API>
node third_party/joy/api/tools/gen_bindings.js --disable <同样的选择子>
node third_party/joy/api/tools/gen_bindings.js --check    # 只校验不写文件，CI 门禁用
```

默认生成 6 个入库产物：`coverage.md`、`thunks_generated.c`、
`generated/joy_api_vtable.{h,c}`、
`modules/cs/bindings/api/api_generated.c`、`modules/cs/scripts/Api.generated.cs`。
另有 `jmath_wrap.js` 单独生成 `generated/jmath_wrappers.{h,c}`：先跑它再跑
`--scan`，因为 `.h` 是契约输入。`--check` 直接调用该模块的 `check()` 汇总错误，
不再起子进程；改过 `jmath.h` 后两个生成物任一过期、或 `joy.h` 少了那行 include，
都会让门禁失败。`--scan` 是幂等的：同一份契约头连续跑两次，产物字节不变。

### 批量开放

`--enable` 不是「把选择到的全改成 on」，而是**逐条过闸**：

1. `all` / `L0` / `L1` 只考虑 L0 / L1 且不是 `orphan`；可展开的 L2 试点条目只能 `name:` 点名
2. 参数 / 返回类型必须能被 thunk 与 cs 适配生成（`fp_t`、枚举、结构体一律跳过）
3. 返回句柄的条目，`owns` 目标的唯一参数类型必须与返回类型**严格相同**；
   `inferOwns()` 初次生成时也按同一硬校验过滤候选，`--scan` 不会覆盖人工修正后的 `owns`
4. `void*` 入参没有脚本层裸缓冲语义，跳过
5. 返回 `char*` 的所有权未标注，跳过
6. 返回句柄的条目批量模式一律跳过（可能是借用指针），只能 `name:` 点名开放

命令会打印 `changed / skipped` 与每条跳过原因，写回 IDL 后重生成 4 个产物；
`git diff` 里每条 `status` 变化都可审查。`--disable` 是对称的回退手段。

## IDL 字段

```json
{"name":"joy_r2d_font_create","domain":"r2d","level":"L1","reason":"不透明指针 → 句柄 / 所有权","status":"on",
 "owns":"joy_r2d_font_destroy","ret":"font_p","params":[{"name":"filename","ctype":"char*","isConst":true},
 {"name":"fontsize","ctype":"int","isConst":false}],
 "source":"include/joy/japi.h:190","note":""}
```

| 字段 | 归属 | 说明 |
|---|---|---|
| `name` / `domain` / `level` / `reason` / `ret` / `params` / `source` | **自动** | 每次 `--scan` 从 `include/joy/{defs,api}.h` 重新解析并覆盖，不要手改 |
| `status` | **人工** | `off` / `on`。可手改，也可用 `--enable` / `--disable` 批量改 |
| `owns` | **人工** | 配对析构函数名。`--scan` 首次会推断填充，之后永不覆盖；推断错的会进「所有权待裁决」清单 |
| `borrowed` | **人工** | `true` = 返回借用句柄，thunk 用 `dtor=NULL` 注册；`joy_obj_release` 只减引用、不释放底层对象。与 `owns` 互斥，`--scan` 不覆盖 |
| `note` | **人工** | 备注，永不覆盖 |

`--scan` 按 `name` 合并：自动字段刷新，`status` / `owns` / `borrowed` / `note` 原样保留。

### 借用句柄（`borrowed`）

`borrowed: true` 只用于返回 `*_p` 句柄、但所有权仍归引擎的函数（getter / find /
unwrap 等）。`emit_thunks.js` 的 `ownsIssue()` 会放行这类条目而不要求 `owns`，
生成的 thunk 用 `joy_obj_register(ptr, tag, NULL, name)` 注册，注册失败时也不
释放指针。`api/runtime/joy_obj.c` 的 `joy_obj_release()` 只在 `dtor != NULL` 时
调用析构，所以脚本层 GC 不会 double free。

`--check` 校验 `borrowed` 必须配句柄返回、且不能与 `owns` 同时存在。返回
`void*` 的条目即使标了 `borrowed` 也不能开放：C# 强句柄（`emit_cs.js` 的
`canEmitCs`）与 C++ vtable（`emit_vtable.js` 的 `okHandle`）都排除 `void*`，
否则会生成非法的 `Void*` 类名，且所有 `void*` 共用 tag 失去类型安全。当前
12 条借用句柄里 7 条已开放，5 条 `void*` 保持 `status:off`。

### 单参数 dtor 包装（`owns` 的另一条出路）

3D mesh 原本只能靠 `joy_r3d_mesh_destroy(jr3d_p, render_mesh_p)` 双参数销毁，
不满足 `ownsIssue()` 的「单参数、参数类型与 `ret` 严格相同」硬校验。引擎侧
（`src/jr3d.c`）给 `render_mesh` 加了 `owner` 回指，11 处网格分配统一走
`r3d_mesh_alloc(r)`，并新增公开 API
`void joy_r3d_mesh_destroy_one(render_mesh_p)` 转调双参数版本（复用延迟释放队列，
避免释放仍被 GPU 引用的 buffer）。15 条 mesh 的 `owns` 全部指向它，13 条已开放。

**生命周期约束**：mesh 句柄不得比创建它的 r3d 上下文活得久，否则 `owner`
悬空；脚本层需保证先销毁 mesh 再销毁 r3d。

返回 `void*` 的 SDL GPU 对象（`load_shader` / `create_pipeline` /
`create_*_texture` 等）以及含 `void*` 缓冲参数的 `mesh_custom*` 仍保持 `off`：
前者没有强句柄类型，后者脚本层传不了裸缓冲。
`include/joy/{defs,api}.h` 里删掉的条目不会丢，会保留并标 `orphan: true`，在报告里单列。

## P1 / P2：共享运行层与 thunk

`runtime/` 是三后端共用的 C 层，`thunks_generated.c` 是 IDL 驱动的生成物：

```text
脚本 VM 值  →  vm.c（各后端唯一语言相关层）  →  joy_value[]
                                              ↓
                                     thunks_generated.c
                                              ↓
                    joy_obj（句柄 / refcount / dtor）→ 真正的 joy C API
```

**当前开放 668 条**：L0 42 条 + P5 自动生成的 jmath 包装 220 条（另 2 条返回
`char*` 被跳过）、L1 404 条（含首批人工验证的 2 条）、L2 试点 2 条，跳过 136 条。
其余 487 条 = L2 72 + L3 229 + X 50 + 被闸门跳过的 136 条，构成后续阶段工作量。

`joy_thunk_entry` 除 `name` / `domain` / `argc` / `fn` 外，还带
`arg_kinds[]` 与 `ret_kind`：Lua / JS 的 `vm.c` 直接读这张表就知道第 j 个脚本值
该转成哪种 `joy_value`，不必各解析一遍 IDL；方法名统一由 `naming.js` 生成。

`joy_api_selftest()` 对**每一条**开放的 API 故意传错 `argc` 调一次
`joy_api_call()`，验证它确实注册在表里且参数守卫生效。thunk 第一行就返回，
不会执行任何 joy 业务函数，因此批量开放有副作用 / 需要上下文的 API 也不会在
自检里被真的调用。此外保留 L0 标量往返、create→句柄、句柄参数解引用、错误
type tag 被拒绝、`release` 后自动析构与计数配平、失效句柄不可复用等深度自检。
宿主只在 `--smoke` 时调用它，输出 `[api] selftest PASS`。

主仓 CMake 显式把 `runtime/*.c` + `thunks_generated.c` 编入宿主，并用
`joy_add_sources(... MODULES cs lua js)` 启用三个后端；各后端的 `bindings/*.c`
由模块自己的 GLOB 收集。它们都只依赖 `runtime/joy_thunk.c` 的 `joy_api_call()`
和 `joy_thunks_table()`，不需要各自解析 IDL。

### 三端已接通（P1 第二刀 / P2 第三刀）

三个后端都在旧注册入口之后多调一次新入口，旧胶水与业务调用点一行未改：

| 后端 | 入口 | 脚本侧写法 | 句柄生命周期 |
|---|---|---|---|
| C# | `csopen_api()` | `Joy.Api.Bindings.GetTicks()` | finalizer → `Native.Release()` |
| Lua | `luax_open_api(L)` | `Joy.Api.Bindings.GetTicks()` | userdata `__gc` |
| JS | `jsopen_api(ctx)` | `Joy.Api.Bindings.GetTicks()` | object finalizer |

```text
Joy.Api.Bindings.GetTicks()   ← 三端同名、同参数顺序
  → 后端薄适配（cs: api_generated.c / lua: lapi.c / js: japi.c）
  → joy_api_call() / e->fn（按注册表）
  → thunks_generated.c：真正的 joy C API + joy_obj 句柄
```

Lua / JS 的适配层遍历 `joy_thunks_table()` 直接注册，**不需要为每条 API 生成
后端代码**：新增 API 只重新生成 `thunks_generated.c`，两个后端自动跟随。
参数按 `arg_kinds[]` 转换、返回值按 `ret_kind` 转回；句柄只以不透明对象出现，
脚本拿不到 `uint64_t`，GC 时自动 `joy_obj_release()`。

`--smoke` 依次打印 `[api] selftest PASS`（C 层）、`[api] lua selftest PASS`、
`[api] js selftest PASS`、`[api.cs] PASS ... destroyed=1`（C# 层），后两者计入
`[smoke] PASS` 断言。三端自检都验证：调用、句柄创建、传错句柄被拒、
GC 后 live 归零且 destroyed +1。

**当前边界**：`--check` 放行「字段全是已知标量」的可展开 L2 试点，仍拒绝 L3 / X 与
返回句柄的 L2；生成 thunk 覆盖标量、`const char*`、不透明句柄，以及按字段展开的
POD 结构体。记录对象、数组、回调、其余 72 条 L2 仍留待后续阶段。
`jmath.h` 的 134 个数学函数里 114 个经 `jmath_wrap.js` 生成 220 条全标量包装
（结构体入参展开、结构体返回拆分量），声明与实现都在 `generated/`，主头
`joy.h` 只留一行 include；20 个 mat44 函数因 16 分量超限未接。
三端都能调用这 668 条，但业务代码尚未迁移到 `Joy.Api.Bindings.*`。

## 分级

| 级别 | 含义 | 处理方式 |
|---|---|---|
| L0 | 纯标量 + `const char*` | 可直接生成 |
| L1 | 不透明指针 `*_p` / `void*` | 转句柄，需所有权测试 |
| L2 | 结构体按值 | P2-A 试点：全标量 POD 按字段展开成标量序列；其余不自动生成 |
| L3 | 回调 / varargs / 数组 / 输出缓冲 / 结构体指针 / 结构体返回 | **不自动生成**，必须手写 thunk |
| X | 条件编译符号、插件系统、测试符号 | 有意排除 |

## 三种常见操作

### 1. 开放 API 给脚本

批量：`node third_party/joy/api/tools/gen_bindings.js --enable L0`（或 `L1` / `all` /
`domain:lcg`），看 `changed / skipped` 与 `git diff`，一起提交 IDL + 4 个生成物。

逐条裁决（返回句柄、缺 `owns`、类型不匹配等）：先在 `coverage.md` 的
「所有权待裁决」一节核对，修正 IDL 的 `owns` 后用
`--enable name:<API>` 点名开放。硬校验（类型匹配）不能靠点名绕过。

### 2. `joy.h` 改了签名 / 加了新 API

1. `node third_party/joy/api/tools/gen_bindings.js --scan`
2. 看 `git diff` —— 自动字段被刷新，`status` / `owns` / `note` 应保持不变
3. 新条目默认 `status: off`；需要开放的按上面操作 1 处理
4. 一起提交 IDL 与全部生成物

忘记 `--scan` 时 CI 的 `--check` 会直接报「joy.h 新增 API 未进 IDL / 签名已变更」。

### 3. 加一个脚本后端

cs / lua / js 已按此落地。新后端只需要两份东西：一个「VM 值 ↔ `joy_value`」
的转换表，和一个遍历 `joy_thunks_table()` 的注册循环（方法名取 `script_name`，
参数类型取 `arg_kinds`，返回类型取 `ret_kind`）。把入口登记到 `joy_<name>.h`，
在宿主创建时调用一次，`stub.c` 补空桩即可；`thunks_generated.c` 由 IDL 驱动，
**不需要为每条 API 写后端代码**。

## CI 门禁

主仓 `.github/workflows/windows-build.yml` 在构建前执行：

```yaml
node third_party/joy/api/tools/gen_bindings.js --check
```

`--check` 不写文件，会校验：IDL 字段完整、无重复 `name`、`owns` 指向存在、
**开放条目的 `owns` 类型严格匹配**、只有 L0/L1 及可展开的 L2 被开放、三个契约头
（`include/joy/jdefs.h` + `include/joy/japi.h` + `generated/jmath_wrappers.h`）与 IDL 同步，且 `coverage.md`、
`thunks_generated.c`、`modules/cs/bindings/api/api_generated.c`、
`modules/cs/scripts/Api.generated.cs` 四个生成物，以及
`generated/jmath_wrappers.{h,c}`、`include/joy/joy.h` 里那行 include 都是最新的。
任一项不满足即失败；
比较前会统一换行，兼容 Windows CRLF checkout。冒烟另需看到
`[api] selftest PASS`、`[api] lua selftest PASS`、`[api] js selftest PASS`
与 `[api.cs] PASS` 四条。

## 已知边界

- `jmath.h` 是头文件内 `JOY_INLINE` 的值类型数学库，没有 `JOY_API`，不在本次解析范围；
  报告里只给粗统计，需要单独解析并映射成「记录」类型。
- `level` 是保守推断：不认识的类型一律归 L3，宁可要求手写，也不冒险自动生成。
- `runtime/` 与 `thunks_generated.c` 由宿主 CMake 显式编入；三端适配层由各模块
  GLOB 收集。旧链路只多了一行 `csopen_api()` / `luax_open_api()` / `jsopen_api()` 注册。
- 38 条「所有权待裁决」已逐条人工核对实现并裁决（借用指针 / dtor 双参数 / 无合规 dtor），
  结论写在 `joy.idl` 的 `note`，并由 `coverage.md` 的两张待办表展示。它们没有可通过硬校验的
  `owns`，`--check` 仍会拦住 `status: on`；后续要开放得先手写 thunk 或引入显式 `borrowed` 标记。
- L2 试点（`checkbox` 域 2 条）用「结构体按字段展开成标量序列」：脚本侧传展开后的
  多个标量而不是单个对象，参数个数与 `joy.h` 不一致；三端仍同名、同顺序、同类型。
  展开后参数个数受 `LUAX_MAX_ARGS` / `JSAPI_MAX_ARGS`（16）限制。要恢复「脚本传一个
  对象」需引入 `JOY_V_RECORD` 与字段 schema，见主仓 `HANDOFF.md` 第 8 节。
- 其余 72 条 L2 的结构体按值展开、L3 的输出参数 / 回调 / 数组，是下一阶段的主要工作；
  之后才是把旧 `Joy.*` 业务调用点迁到 `Joy.Api.Bindings.*` 并删除旧胶水。
