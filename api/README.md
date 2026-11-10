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
| `generated/joy_api_vtable.h` | 工具生成，勿手改 | 701 个强签名函数指针 + 56 个句柄薄包装 typedef（`jr2d_p`→`Jr2d`，编译期不可混传）+ `abi_version` / `size` |
| `generated/joy_api_vtable.c` | 工具生成，勿手改 | 701 个 shim（强签名 ↔ `joy_value`）、宿主侧填表 `joy_api_vtable_init()`、自检 `joy_api_vtable_selftest()` |
| `modules/cs/bindings/api/api_generated.c` | 工具生成，勿手改 | Mono 值 → `joy_value[]` → `joy_api_call()`，注册 `Joy.Api.Native::*` |
| `modules/cs/scripts/Api.generated.cs` | 工具生成，勿手改 | `Joy.Api.Bindings` / `Diagnostics` / 强类型句柄类，public API 无 `IntPtr` |
| `modules/lua/bindings/lapi.c` | 人工 | Lua 薄适配：遍历注册表、`joy_value` ↔ Lua 值、句柄 userdata `__gc` |
| `modules/js/bindings/japi.c` | 人工 | JS 薄适配：遍历注册表、`joy_value` ↔ duktape 值、句柄对象 finalizer |
| `runtime/joy_value.*` | 人工 | 统一值容器（标量 / 字符串 / 句柄） |
| `runtime/joy_obj.*` | 人工 | 句柄表、引用计数、类型 tag 校验、自动析构 |
| `runtime/joy_thunk.*` | 人工 | thunk 签名 / 三端共用注册表，以及按名调用入口 `joy_api_call()` |
| `joy_api.h` | 人工 | 对外唯一入口：`joy_api_selftest()` |
| `idl/legacy_names.txt` | 工具冻结，只减不增 | P0 冻结的 1156 条存量 API 名，命名门禁的豁免基线；全量迁移后删除 |
| `include/joy/compat.h` | 人工 | 旧名 → 新名的宏映射，迁移期保护外部调用方；`joy.h` 引入，但不属于 `DECL_SOURCES` |

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

| 域 | 吸收的历史 domain | 约条数 |
|---|---|---|
| `core` | env / window / log / error / ticks / quit / clipboard / event / mouse / utf8 / hash 系列 | ~60 |
| `math` | jmath(220) / collision* / gjk* / box2d / shape | ~300 |
| `r2d` | r2d / font / jtext / image / jspine / jparticle | ~100 |
| `r3d` | r3d / jlight / jgltf | ~150 |
| `ui` | jui / jui_im / jlayout + 全部控件域 | ~230 |
| `scene` | scene / game / jecs / jtilemap / janimator / animation / jtimer / jpm | ~150 |
| `net` | net* / tcp* / kcp* / ws* / iocp / pack / unpack / netmessage | ~90 |
| `audio` | audio | 20 |
| `input` | joystick / mouse | ~20 |
| `script` | modules / module / moudles / jmsgpack | ~30 |

划分原则：按「谁会一起改 / 谁依赖谁」分，不按文件、不按算法；一个 API 只属
一个域，跨域时归被调用方；控件、协议、碰撞维度一律合并。域清单封闭，防止
再次长到 85 个（`GENERIC_FIRST` 兜底漏出的 `min` / `raw` / `should` 等垃圾域
一并收编）。

### 门禁与生成链路

- `--check` 只卡**新增**：不在 `idl/legacy_names.txt` 里的 `JOY_API` 必须匹配
  `joy_<白名单域>_`，否则报错。存量 1156 条豁免，删掉该文件即变成全量强制。
- `domainOf()` 对三段式名字取第二段；存量名字仍走 `GENERIC_FIRST` 旧分词，
  **P0 期间 IDL 的 domain 列一字不变**。全量迁移后 `GENERIC_FIRST` 退休。
- `SKIP_PREFIXES` 管旧前缀、`SKIP_DOMAINS` 管新域，两者并存到迁移结束。
- 迁移改名时在 `naming.js` 的 `ALIASES` 登记 `新C名 → 旧PascalCase名`，使
  三端方法名与 `api->R2dCreate` 保持冻结；不能用「剥掉 `joy_<域>`」自动推导
  （历史名如 `JanimatorCreate` 会坏）。
- `compat.h` 放旧名 `#define`，不在 `DECL_SOURCES` 里且 `collectDecls()` 跳过
  `#` 行，不会污染 IDL / 生成物。定义 `JOY_NO_LEGACY_NAMES` 可查残留调用点。

### 迁移顺序（P1 进行中）

**批次记录**：
- 第一批 `core/window` 子域 6 条（2026-11）。发现 `emit_vtable.js` 的
  `HANDLE_PROBE` 按名字硬编码，漏改会让强句柄自检被静默跳过。
- 第二批 `core` 剩余 39 条（hash / utf8 / lcg / env / log / event / fps /
  clipboard / local_ips），`core` 域累计 45 条。同步了 `SELFTEST_SAFE` 里
  5 个 hash 名。这批改的是声明；`src/jcore.c` / `jutils.c` / `jsys.c` 里的
  **定义仍写旧名**，由 `compat.h` 宏在预处理期切到新符号，全量迁移完成、
  删除 `compat.h` 前再统一替换定义处源码名。`push_*` / `game_timer_*` /
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
4. 全迁完删除 `legacy_names.txt` 与 `compat.h`。

### vtable 的自检边界

`joy_api_vtable_selftest()` **只**对 `emit_vtable.js` 里 `SELFTEST_SAFE` 列出的
确定性纯函数（hash 系列、`jpm_wave_seed`、`jmath_fp_from_float`）做
「强签名调用 vs 直接 C 调用」数值比对，另用 `window_get_fullscreen({0})`
验证强句柄的无效值会被 thunk 拒绝而不是崩溃。表里其余 690+ 条 API
（`create_window` / `sys_bind` / `audio_create_*` / `quit_env` 等有副作用或
依赖外部状态）**不参与运行时调用**，由编译期签名、`joy_api_vtable_init()`
填表和 `--check` 生成物一致性保证。新增纯函数到白名单前请确认它无副作用、
结果确定。

`status:on` 的 L2（`jtext_printx` / `shape_draw_rectangle` / `shape_draw_circle`
等 30 条）已按 `structs.js` 的字段布局展开成标量进表，与 thunk 表 argc 一致；
`structs.js` 会把 `jmath.h` 的 `#define ft_t float` 归一化，否则 `vec2_t` /
`mat22_t` 之类会被判成不可展开。L3（结构体指针 / 输出参数）仍是
`status:off`，不在表内；图片绘制改由全标量 L1 `image_draw_rect` 绕开
`jrect_t*`，四份示例已恢复真实贴图。剩下的退化只有文字宽度按字号估算
（`jtext_measurex` 是 `int*` 出参，暂无对应标量包装）。

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
{"name":"font_create","domain":"font","level":"L1","reason":"...","status":"off",
 "owns":"font_destroy","ret":"font_p","params":[{"name":"filename","ctype":"char*","isConst":true}],
 "source":"include/joy/api.h:1076","note":""}
```

| 字段 | 归属 | 说明 |
|---|---|---|
| `name` / `domain` / `level` / `reason` / `ret` / `params` / `source` | **自动** | 每次 `--scan` 从 `include/joy/{defs,api}.h` 重新解析并覆盖，不要手改 |
| `status` | **人工** | `off` / `on`。可手改，也可用 `--enable` / `--disable` 批量改 |
| `owns` | **人工** | 配对析构函数名。`--scan` 首次会推断填充，之后永不覆盖；推断错的会进「所有权待裁决」清单 |
| `note` | **人工** | 备注，永不覆盖 |

`--scan` 按 `name` 合并：自动字段刷新，`status` / `owns` / `note` 原样保留。
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
（`include/joy/defs.h` + `include/joy/api.h` + `generated/jmath_wrappers.h`）与 IDL 同步，且 `coverage.md`、
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
