# 交接：当前进展与下一步（2026-11-14）

> 换 AI 交接文档。先读这一页，再读 `HANDOFF.md` 的 §2 / §4 / §5 / §6 / §7。
> 本文件只记**当前状态**与**接下来该做什么**；历史细节在 `HANDOFF.md`。
> 当前提交一律以 `git log -1` 为准，不要从本文猜 hash。
> **下一任 AI 直接从 §3.0「本轮遗留待办」开始**；§3 里 P0–P3 是更早一轮的项。
>
> 新 AI 上手顺序：`NEXT_SESSION.md`（本页）→ `api/README.md` →
> `cmake/JoySources.cmake` → `api/tools/audit_domain_layout.js`（规则真相源）
> → `api/tools/naming.js`（域/方法名真相源）→ `HANDOFF.md`（历史，含大量已作废的旧文件名）。

## 0. 一句话现状

`src/` 已收拢为 **9 个域 = 9 个 `joy_<域>.c` + 9 个同名 `.h`**（另有 1 个登记的数据分片），
一个域不再有多个 `.c`；新增 `audit_domain_layout.js` 门禁把三条规则固化到 CI。
本轮全部改动已作为**一次提交**推送到 `origin/main`，`main` 与远端同步。
**三条 CI 是否针对本轮全绿尚未确认**，见 §3.0 T5；按 §6 约定，CI 出结果前不要宣布完成。

## 1. 最近做了什么（按提交）

| 提交 | 内容 | 规模 |
|---|---|---|
| `9f9f0cde` | 源码按域重划：让函数名的域前缀跟着文件的技术职责走，26 个 `.c` 全部纯域 | 119 条 API 改名 / 562 处 / 25 文件 |
| `795d021a` | 固化域纯度规则为可执行门禁 | 新增 `api/tools/audit_domain_purity.js` + CI gate |
| `bc2b9c1f` | P0-a：补齐 `src/jgltf.c` 缺失的 glTF 句柄 typedef | 2 行 typedef + 全文件句柄写法统一 |
| `a0c3cabe` | P0-b：3 个从未实现的 `r3d_mesh_skinned_*` 走最小降级适配，GLTF 实建可链接 | 3 处调用替换 + 注释 |
| **本轮收尾提交（见 `git log -1`）** | 见下方四项，同一次提交 | `src/` 20 个新文件 / 26 个旧 `.c` 删除 / 11 个契约源 / 13 条 API 改名 |

本轮收尾提交包含四件事（细节见「关键决策」段）：

- **一域一 `.c` 一 `.h`**：28 个旧 `.c` 按域物理合并为域文件，新增 R1–R4 布局门禁并接入 Desktop CI；
- **反转声明归属方向**：公开声明全部下沉 `src/joy_<域>.h`，`japi.h` 改为纯 include 聚合，
  `DECL_SOURCES` = `jdefs.h` + 9 个域头 + `jmath_wrappers.h` 共 11 个来源，新增 R5；
- **查表数据独立成数据分片**：`SIN_LUT` / `tan_lut` 拆到 `src/joy_lut.c` + `src/joy_lut.h`，
  `joy_math.c` 从 56032 行缩到 4543 行，新增 R6 白名单；
- **`input` 并回 `core` 并统一前缀**：13 个 `joy_input_*` → `joy_core_input_*`，
  域白名单 10 → 9，`DOMAIN_ALIASES` 临时机制已删除，`open=739` / `status:on` 不丢、orphan 0。

### 关键决策：物理合并 + 数据分片例外

用户提出更严格的组织规则：**一个 `.c` 一定是一个域；一个域不允许多个 `.c`；
每个 `.c` 要有一个头文件**。此前（第 29 条）的「一个域可以有多个 `.c`、
不合并成巨型 TU」决策随之作废，改名为 `joy_<域>.c/.h` 后物理拼接：

- 域名直接进文件名，9 个域 9 对文件，一眼可数；`.h` 是该域的配套头。
  公开 API 声明按域归属在 `src/joy_<域>.h`，`japi.h` 只做 include 聚合；
  9 个域头全部登记为 `DECL_SOURCES`（连同 `jdefs.h` / `jmath_wrappers.h` 共 11 个来源），
  `source` 行号由 `--scan` 刷新即可，`audit_domain_layout.js` 的 R5 固化引用方向；
  `input` 已并回 `core`，13 个 API 统一为 `joy_core_input_*`，域前缀与文件名一致；
- 3 个可选模块不保留独立 `.c`：`jgltf` / `jspine` / `jmodule` 作为
  `#ifdef JOY_USE_*` 段并入所属域 `.c`，开关宏由 `JoyTargets.cmake` 转发；
- **唯一例外是数据分片**：`SIN_LUT` / `tan_lut`（约 5.1 万行）独立为
  `src/joy_lut.c` + `src/joy_lut.h`。它不含任何函数定义，不计入
  「一域一 .c」，在 `audit_domain_layout.js` 的 `DATA_SHARDS` 与
  `cmake/JoySources.cmake` 的 `JOY_SRC_MATH` 双重登记，R6 校验配套头与纯数据；
- `jvendor_impl.c`（stb/dr 实现宏）并入 `core` 且**必须置顶**，
  否则实现宏晚于其它分片对同名头的 include；
- 各分片原本是独立 TU，合并后在分片末尾 `#undef` 私有宏，避免污染后续分片；
  合并脚本见 `build/merge_domains.js`（构建期临时产物，不入版本库）。
- 生成物 `api/generated/`、第三方 `external/`、`examples/`、`modules/` 不属于
  `src/` 手写域实现，不在本规则管辖内。

### 域归属判据（沿用）

| 判据 | 归入 |
|---|---|
| 纹理 / 文字 / 图元 / 动画 / 2D FOV | `r2d` |
| 3D FOV / glTF | `r3d` |
| 控件（joystick / label / radio） | `ui` |
| 行为树（bt） | `scene` |
| tilemap 碰撞扫描 | `math` |
| 网络打包 / 时间工具 | `core` |

**一个域只有一个 `.c`，一个 `.c` 只有一个域，且必须配一个同名 `.h`。**
显式登记的数据分片（`joy_lut.c`）是唯一例外：它同样配同名 `.h`，
但不含函数定义，也不参与域集合校验。

## 2. 当前硬事实

| 项 | 值 |
|---|---|
| HEAD / 远端 | 见 `git log -1`；本轮改动已 push，`main` 与 `origin/main` 同步 |
| IDL | 1170 条，`status:on` 739 条，vtable 739；`--scan` 幂等、orphan 0 |
| 域布局 | `src/` **10 个 `.c`（9 域实现 + 1 数据分片）/ 10 个 `.h` / 1013 条函数定义 / 混域 0 / 违规 0** |
| `--check` | OK（report/thunks/vtable/cs glue/jmath 全部最新）、`--scan` 内容幂等 |
| 全量 Debug 构建 | 0 错误；GLTF+SPINE+PLUGINS 全开 `example_cpp` 实建 0 错误 |
| `ctest` | 4/4 |
| CI | 上一轮 3/3 success；**本轮推送后的三条 CI 尚未确认**（§3.0 T5） |
| 人工字段 | `borrowed` 12、`owns` 非空 83、`note` 非空 42 |

### 域 → 文件 归属表（9 个域 `.c` + 1 个数据分片，均配同名 `.h`）

| 域 | 文件（函数定义条数） | 合并自分片 |
|---|---|---|
| `core` | `joy_core.c`(110) | `jvendor_impl` / `jcore` / `jutils` / **`joy_input`（原 input 域）** |
| `math` | `joy_math.c`(90) | `jcollision` / `jgjk` / `jbox2d` |
| `r2d` | `joy_r2d.c`(129) | `jr2d` / `jfont` / `jparticle` / `jspine` |
| `r3d` | `joy_r3d.c`(165) | `jr3d` / `jlight` / `jgltf` |
| `ui` | `joy_ui.c`(220) | `jui` / `jui_im` |
| `scene` | `joy_scene.c`(150) | `jscene` / `janimator` / `jtilemap` / `jecs` / `jprocmap` / `jbehavior` |
| `net` | `joy_net.c`(96) | `jnetwork` / `jsys` |
| `script` | `joy_script.c`(33) | `jmsgpack` / `jmodule` |
| `audio` | `joy_audio.c`(20) | `jaudio` |
| 数据分片 | `joy_lut.c`(0) | `jlut`（`SIN_LUT` / `tan_lut`，不在域集合内） |

函数定义总数 1013 条，与合并前 26 个 `.c` 的统计逐字一致（一次纯搬运，无增删）。

## 3. 下一步（按优先级）

### 3.0 本轮遗留待办（换 AI 交接，优先看这里）

> 用户临时换 AI，本轮**只做了「把待办写进文档 + 提交 + 推送」**。
> 下面 T1–T6 都**没有执行**，不要误以为已完成。§3.1 起的 P0–P3 是更早一轮的项：
> P0 两条已修、P1 需先定口径、P2/P3 明确「不是待办」。

#### T1（必须先让用户拍板）`r3d` 的 3D 控件与 `ui` 的 2D 控件归属不一致

现象：`src/joy_r3d.h/.c` 里有 `joy_r3d_button_*`、`joy_r3d_slider_*`、
`joy_r3d_combobox_*`、`joy_r3d_checkbox_*`、`joy_r3d_label_*`、`joy_r3d_input_*`
共 **65 条** 3D 控件 API（其中 `status:on` 50 条；分布 input 22 / slider 11 /
combobox 10 / checkbox 10 / button 8 / label 4），而 2D 控件在 `src/joy_ui.h/.c`。
IDL 里 `ui` 域的 3D 控件条目为 0，外部 C 调用点也基本为 0（名字主要存在于
IDL / 文档 / 生成物）。

定性：不是「放错文件」，是**两种划分轴混用** —— `ui` 按控件职责划，
`r2d` / `r3d` 按渲染技术层划。两者的依赖完全不同：

```c
/* 3D 控件：强依赖 3D 渲染器与 mesh */
JOY_API void joy_r3d_button_draw(joy_r3d_p r, joy_render_mesh_p m,
                                 joy_r3d_button_p btn, float winW, float winH);
/* 2D 控件：强依赖 2D 渲染器 */
JOY_API joy_button_p joy_ui_button_create(joy_r2d_p r2d, float cx, float cy,
                                          float wf, float hf);
```

`api/idl/handle_aliases.json` 里 6 个 3D 控件句柄类名（`Jr3dButton` 等）已冻结脚本层名字。

方案（**先问用户选 A 还是 C，不要自己决定**）：

- **A（推荐，零代码风险）**：不改代码，只把边界规则写清楚 —— `ui` = 依赖
  `joy_r2d_p` 的 2D 控件体系；依赖 `joy_r3d_p` / `joy_render_mesh_p` 的 3D 控件
  归 `r3d`（与渲染管线同域，避免同域文件里塞进另一条渲染链的依赖）。
  需同步：`api/README.md` 域表、本文件 §2 域归属判据表、
  `cmake/JoySources.cmake` 头部、`HANDOFF.md` 第 29 条判据（394–403 行）、
  `src/joy_r3d.h` 头部注释。
- **B（已排除）**：只搬文件、保留 `joy_r3d_*` 名字 → `audit_domain_layout.js`
  的 R4 会判混域；除非恢复本轮已删除的 `DOMAIN_ALIASES`，不要走。
- **C（彻底统一，成本高）**：65 条改名 `joy_ui_3d_*` 并搬到 `joy_ui.h/.c`。
  动手前按项目约定**先做只读冲突扫描**（同名 `static` / `typedef` / `#define`、
  `#if` 平衡、分片边界），再依次同步 `rename_map.json`、`naming.js` 的 `ALIASES`
  （只改 key 不改 value）、`type_rename_map.json`、`handle_aliases.json`、
  `joy.idl`、`api/generated/*` 与 C# glue（C 名变、PascalCase 方法名不变）、
  两个门禁、`cmake/JoySources.cmake`、`api/README.md`、本文件。
  验收：`--check OK`、`--scan` 内容幂等、`open=739`、orphan 0、默认构建 0、
  `ctest` 4/4、`git diff --check` 0。

#### T2 `README.md` 目录说明仍是旧的

第 6 行还写 `` `joy.h` / `j*.c` / `external/` / `test/` ``，与现在的
`src/joy_<域>.c` + 9 个域头 + `japi.h` 纯聚合不符。改为按域描述，
并说明 `japi.h` 是聚合入口（见 T3）。

#### T3 `japi.h` 的安装分发风险（需要决策，不是 bug）

`include/joy/japi.h` 用 `#include "../../src/joy_<域>.h"` 引用私有目录头；
源码树构建正常，但**单独安装 / 分发 `include/` 时会断链**。两条路：
生成一份安装版聚合头，或把域头搬进 `include/joy/`（会动 R5 门禁、
`DECL_SOURCES` 全部路径和 IDL 的 `source` 行号）。**先问用户要不要处理**。

#### T4 `HANDOFF.md` 历史叙述未统一（有意保留）

仍大量出现旧 `.c`（`jr2d.c` / `jr3d.c` / `jgltf.c`）、`joy_input_joystick_*`
等历史名。这是**有意保留的历史记录**，用户未要求改，别当成待修 bug；
建议在开头加一句「历史记录，当前结构以 `NEXT_SESSION.md` 为准」即可。

#### T5 推送后确认 CI

本轮提交已 push 到 `origin/main`，但 Desktop（Windows/Linux/macOS）/ Android /
Web 三条工作流**尚未针对本轮确认**。本机没有 `gh` CLI，需到 GitHub Actions 页面
查看，或装 `gh` 后 `gh run list --limit 3`。按 §6 约定，CI 出结果前不要宣布完成。

#### T6 提交状态（下一任无需处理，仅作说明）

收尾时 26 个旧 `.c` 的删除已 staged，而 20 个 `src/joy_*` 新文件和
`api/tools/audit_domain_layout.js` 是 `??` 未跟踪；本轮已显式 `git add src/`
并提交，工作区应干净。以后遇到同类「删除 + 新建」重构，见 §4 第 10 条。

### 3.1 更早一轮的遗留项

### P0 — `src/jgltf.c` 预存缺陷（P0-a 句柄别名已修，P0-b 已最小降级适配）

#### P0-a 句柄别名缺失 —— 已修（2026-11-13）

`struct joy_gltf_asset` / `struct joy_gltf_instance` 从未 typedef 成句柄类型，
全文却用旧的 `jgltf_asset_p` / `jgltf_instance_p`（收口版 `240f5f7a` 即如此，
不是改名引入）。已补两行 typedef 并全量替换为 `joy_*` 前缀：

- asset 的 typedef **必须放在 `struct joy_gltf_instance` 之前**：该结构体第一个
  字段就引用它。原记录里「放在 instance 定义之后」是错的。
- `jgltf_asset_p` 是函数 `jgltf_asset_prim_count` 的前缀，全词替换会把它误改成
  `joy_gltf_asset_prim_count`，被域纯度门禁判成 `gltf` 域混域；该行已还原。

`cmake -S examples -B build/opt_check -DJOY_ENABLE_GLTF=ON -DJOY_ENABLE_SPINE=ON`
的**编译阶段已通过**（`jgltf.obj` 生成成功）；生成链 / 域纯度 / 默认构建 /
`ctest` 均不受影响。

#### P0-b 三个 r3d 动态蒙皮 API 从未实现 —— 已最小降级适配（2026-11-13）

同一个实建在**链接期**仍失败：

```text
jgltf.obj : error LNK2019: 无法解析的外部符号 r3d_mesh_skinned_create
jgltf.obj : error LNK2019: 无法解析的外部符号 r3d_mesh_skinned_update
jgltf.obj : error LNK2019: 无法解析的外部符号 r3d_draw_mesh_skinned
```

历史核查：`git log --all -S'r3d_mesh_skinned_create'` 只命中目录重构；
`f124824f` / `240f5f7a` 全仓只有 `jgltf.c` / `jgltf.h` 的调用与注释，
**从来没有实现**。这不是改名遗漏，`jgltf.c` 是从未链接过的半成品，
原记录「约 10 分钟可修完」的判断不成立。

现有可对齐的 API：`joy_r3d_mesh_custom_lit(r, verts, vc)`、
`joy_r3d_draw_mesh_lit(r, m, model, tex)`、`joy_r3d_mesh_destroy(r, m)`；
**没有动态顶点更新接口**。已与用户确认走**最小降级**（路线 1）：

- `create`：`r3d_mesh_skinned_create` → `joy_r3d_mesh_custom_lit(r, buf, vertex_count)`；
- `update`：改为「`joy_r3d_mesh_destroy` 销毁旧 mesh + 按新顶点 `joy_r3d_mesh_custom_lit`
  重建」，`inst->gpu[ni]` 指回新 mesh；
- `draw`：`r3d_draw_mesh_skinned` → `joy_r3d_draw_mesh_lit(r, m, model, tex)`。
  文件头注释也同步更新为降级路径说明。
- 代价：每帧建销 GPU buffer（蒙皮网格走延迟释放队列），且无 GLTF 模型 / 真机可
  验运行；正式补齐（路线 2）仍属新功能缺口，未做。
- 验证：`cmake -S examples -B build/opt_check -DJOY_ENABLE_GLTF=ON -DJOY_ENABLE_SPINE=ON
  --target example_cpp` 链接通过（`example_cpp.exe` 生成，0 错误）；
  `--check` / 域纯度 / 默认 `ctest` 4/4 均不受影响。

> **验证方式纠正**：根入口 `cmake -S . -B ...` 默认 `JOY_BUILD_EXAMPLES=OFF`，
> `joy::all` 是 INTERFACE，不会编译/链接 `jgltf.c`；此前记的「SPINE+GLTF 独立
> 构建 0 错误」是这种空验证。必须用 `cmake -S examples -B ...` 并构建具体
> target 才覆盖得到（见 §5 第 5 条）。本机没有 gcc，`gcc -fsyntax-only` 不可用。

### P1 — 公共句柄类型的前缀与域不一致（需先定口径，不要直接动手）

`include/joy/jdefs.h:314-318` 有 4 个公开句柄类型，前缀是**概念名**而非域：

```c
typedef struct joy_font      joy_font_t,      * joy_font_p;
typedef struct joy_image     joy_image_t,     * joy_image_p;
typedef struct joy_text      joy_text_t,      * joy_text_p;
typedef struct joy_animation joy_animation_t, * joy_animation_p;
```

它们被 `joy_r2d_*` 创建/返回（`japi.h` 里 6 条 `JOY_API joy_(image|text|animation)_p joy_r2d_*`），
即**函数是 `r2d` 域、类型是概念名**。`image` / `text` / `animation` 在 P1 域表里
原本分别归 `ui` / `ui` / `scene`，所以看起来仍像「跨域」。

这不是 bug，是历史命名；但若要彻底统一成 `joy_r2d_image_t`，影响面是：

- `jdefs.h` 的 typedef、`japi.h` 的 6 条返回值；
- `api/idl/handle_aliases.json`（脚本层类名 `Image` / `Text` / `Animation` 靠它冻结，
  必须只改 C 名、不动脚本层名字，否则四份示例全崩）；
- `type_rename_map.json`、C# `Api.generated.cs`、`vt` 名称。

**建议**：先和用户确认「类型名是否也要跟域走」，再决定做不做。
若做，走 `api/tools/rename_types.js` 的 `--dry-run/--write/--audit` 流程，
不要手改。风险点是脚本层类名冻结，务必先比对 `handle_aliases.json` 前后。

### P2 — `HANDOFF.md` §7 里仍未做的工程项

这些是**已评估、明确不是待办**的方向，只有用户重新提出时才做：

- 资源管理器 / 绘制指令层（`jr2d.c` 无资源管理器，纹理生命周期靠调用方）；
- 两套分配器并存（`malloc` + `SDL_malloc`），跨堆释放风险；
- Winsock 初始化/清理缺引用计数；`set_sock_*timeo` 在 macOS/UNIX 是空实现；
- 控件 `void* event` API 与 `joy_ui_process_widget_event` 空壳待真实消费者。

### P3 — 仍未验证的项（见 `HANDOFF.md` §6）

iOS / wasm 静态形态、Android 真机冒烟、C# 在 iOS/wasm（宿主模型不兼容，已定方案 A
只发 cpp/lua/js）。这些本机没法验，只能靠 CI 或真机。

## 4. 踩坑清单（下次改名前必读）

1. **改公开 API 的 C 名，必须同步两处**，否则会静默坏掉：
   - `api/idl/rename_map.json` 追加一条「当前 C 名 → 新 C 名」。不追加的话
     `--scan` 会把改名后的条目当成新条目，**把 `status:on` 重置成 `off` 并丢掉
     `owns` / `note` / `borrowed`**。
   - `api/tools/naming.js` 的 `ALIASES` **只改 key、保留 value**。value 是冻结的
     脚本层方法名（`ImageCreate` / `JtextPrintx` …），改了它四份示例和 C# 绑定全崩。
2. **新增 `.c` 必须登记到 `cmake/JoySources.cmake`**（该文件是显式清单，不用 glob），
   并归到对应域组。**但一个域只允许一个 `.c`**——新功能应写进已有的
   `src/joy_<域>.c`，不要新建 `src/xxx.c`，否则布局门禁直接 FAIL。
   纯数据文件是唯一例外：必须同时登记 `DATA_SHARDS` 白名单并配同名 `.h`
   （当前只有 `src/joy_lut.c`），R6 会校验它不含任何 `joy_*` 函数定义。
3. **`legacy_names.txt` 里那 6 条旧名（4 个 `joy_fov_*` + `joy_tilemap_sweep_x|y`）
   已经失效**：本轮改名后 C 名分别是 `joy_r2d_fov_draw_2d`、`joy_r3d_fov_*`、
   `joy_math_tilemap_sweep_x|y`，全部落在白名单域内。别再当成「例外」去豁免。
   该文件仍须保留（存量 1156 条靠它通过 `--check`）。
4. **两个域门禁的判定口径**（`audit_domain_purity.js` 查纯域，
   `audit_domain_layout.js` 再加「一域一 .c / 每 .c 配同名 .h」）：
   只算函数定义（签名后紧跟 `{`）。跨域**调用**、注释提及、共享句柄类型
   （`joy_image_p` 等）都**不算**混域。`joy_r2d.c` 调 `joy_core_get_raw_window`、
   `joy_core_utf8_decode_all`、`joy_core_utils_get_current_time` 是正常单向依赖，
   不要去「修」。
5. **注释里写别的域的函数名也会让人误判成混域**。新写注释时用「按钮 / 摇杆等
   控件」这类描述，不要点名别域符号。
6. **合并到巨型 TU 的三个坑（本轮实际踩过）**：
   - 分片开头的 UTF-8 BOM 拼到文件中段会变成非法 `U+FEFF`（MSVC C2014/C2054），
     合并脚本必须剥离 BOM；
   - 各分片原本是独立 TU，其 `#define` 私有宏只在本 TU 可见，合并后必须
     在分片末尾 `#undef`，否则污染同域后续分片；
   - `jvendor_impl` 的 `STB_*_IMPLEMENTATION` / `jgltf` 的
     `CGLTF_IMPLEMENTATION` 必须先于任何对同名头的 `#include` 展开，
     因此该分片要置顶（或置于域文件内首次 include 之前）。
7. **`--scan` 在 Windows 会把生成物写成 LF**（仓库按 CRLF 检出），
   `git status` 会显示 5~6 个生成物 M 但内容 diff 为空。验证幂等要按 EOL
   归一化比较；提交前把这些文件转回原有换行风格，避免无关 diff。
8. 冒烟必须带 `--timeout 25`，历史上出现过渲染卡死拖死整轮测试。
9. 本机的 `git push` 输出会走 stderr（PowerShell 包成 NativeCommandError），
   只要看到 `xxx..yyy main -> main` 就是成功，不要被红字吓到。
10. **「删旧 `.c` + 建新 `.c`」重构提交前必须显式 `git add src/`**：`git rm` 删掉的
    旧文件已在暂存区，新域文件却是 `??` 未跟踪；直接 `git commit` 会推上去一个
    「源码全被删光」的提交。提交前用 `git status -sb` 与
    `git ls-files --others --exclude-standard src/` 复核。
11. **改名后若新条目被整体排到 `joy.idl` 末尾**，先查 `gen_bindings.js` 的
    `mergeIdl()`：`rank()` 会通过 `rename_map.json` 的 `renamedFrom` 回查旧名
    在旧 IDL 中的位置来保持稳定顺序。改名时必须先追加 rename_map，
    否则 `status:on` / `owns` / `note` 会丢，且顺序也会乱。不要手工挪 IDL。

## 5. 验证命令（复制即用）

```powershell
cd d:/github/joy

# 1. 两个域门禁（纯域 + 布局：一域一 .c、每 .c 配同名 .h）
node api/tools/audit_domain_purity.js        # 退出码 0 = 通过
node api/tools/audit_domain_layout.js        # 退出码 0 = 通过

# 2. 生成链门禁
node api/tools/gen_bindings.js --check        # OK: 1170 条

# 3. 幂等性（改过名之后必查；Windows 上必须按 EOL 归一化比较，见 §4 第 7 条）
$norm = { param($p) ((Get-Content -Raw $p) -replace "`r`n", "`n") }
$b = $norm.Invoke('api/idl/joy.idl')
node api/tools/gen_bindings.js --scan
$a = $norm.Invoke('api/idl/joy.idl')
"IDL 内容未变(EOL 归一化): $($b -eq $a)"

# 4. 全量构建 + 四语言冒烟
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure --timeout 25   # 必须 4/4

# 5. 可选模块（默认 OFF，改过域文件里的 gltf/spine/plugins 分片必查）
#    必须用 examples 入口并指定 target：根入口 JOY_BUILD_EXAMPLES=OFF 时
#    joy::all 是 INTERFACE，不会真正编译/链接分片
cmake -S examples -B build/opt_check -DJOY_ENABLE_GLTF=ON -DJOY_ENABLE_SPINE=ON -DJOY_ENABLE_PLUGINS=ON
cmake --build build/opt_check --config Debug --target example_cpp

# 6. 收尾
git --no-pager diff --check                   # 无输出
git status -sb                                # 确认 src/ 新文件已 add，不是 ??
git push origin main                          # 然后等三条 CI 全绿再收工
# 本机未装 gh CLI；装了则 gh run list --limit 3 看三条工作流，否则看 Actions 页面
```

## 6. 协作约定（用户偏好，已被多次强调）

- 用户要的是**动手**，不是方案；说「开始处理」就直接改，改完跑验证、提交、推送。
- 提交信息写清「改了什么 / 为什么 / 怎么验证的」，中文正文，按 §8 的风格。
- 推送后**必须等 CI 出结果**再宣布完成（用户明确批评过「没等 CI 就收尾」）。
- 不要新建无谓的文档；报表、审计脚本这类临时产物放 `build/`（已被 gitignore），
  沉淀成正式工具的才放 `api/tools/`。
- 大改动前先把「影响面 + 冲突检查 + 继承链」算清楚再动手，`--dry-run` 先跑一遍。
