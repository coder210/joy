# joy 绑定 API 契约（P0 / P1 / P2）

这里放**一份与脚本语言无关的接口清单**，以及维护它的工具。目标是让 Lua / JS / C#
三个后端共用同一套对象模型、方法名、参数与所有权规则。

> **边界**：本目录只产出契约、报告，以及可被三后端共用的 C 运行层 / 薄适配生成物。
> 旧的 `modules/*/bindings/` 手写胶水一行未删，新旧链路并行；哪些 API 真正开放由
> `joy.idl` 的 `status: on` 决定（当前 446 / 935）。**开放 ≠ 业务已调用**。
> 所有生成物都提交进仓，`git diff` 随时能看清改了什么。

## 文件

| 文件 | 谁维护 | 作用 |
|---|---|---|
| `gen_bindings.js` | 人工 | 解析 `joy.h`、维护 IDL、产出全部生成物。零 npm 依赖，只用 Node 内置 `fs` / `path` |
| `naming.js` | 人工 | **三端共用**的命名与句柄判定规则，避免各后端长出自己的方言 |
| `emit_thunks.js` | 人工 | 共享 thunk / 注册表生成器（纯字符串函数） |
| `emit_cs.js` | 人工 | cs 后端薄适配生成器，产出 C icall 与 C# 强类型包装 |
| `joy.idl` | 工具初始化 + 人工补充 | 唯一真相源。JSON 数组、**每条 API 占一行**，便于 diff |
| `coverage.md` | 工具生成，勿手改 | 分级统计、按域分布、`owns` 待裁决、L3 手写清单 |
| `thunks_generated.c` | 工具生成，勿手改 | 只有 `status: on` 的 L0/L1 会生成 thunk、注册表与自检 |
| `modules/cs/bindings/api_generated.c` | 工具生成，勿手改 | Mono 值 → `joy_value[]` → `joy_api_call()`，注册 `Joy.Api.Native::*` |
| `modules/cs/scripts/Api.generated.cs` | 工具生成，勿手改 | `Joy.Api.Bindings` / `Diagnostics` / 强类型句柄类，public API 无 `IntPtr` |
| `runtime/joy_value.*` | 人工 | 统一值容器（标量 / 字符串 / 句柄） |
| `runtime/joy_obj.*` | 人工 | 句柄表、引用计数、类型 tag 校验、自动析构 |
| `runtime/joy_thunk.*` | 人工 | thunk 签名 / 三端共用注册表，以及按名调用入口 `joy_api_call()` |
| `joy_api.h` | 人工 | 对外唯一入口：`joy_api_selftest()` |

## 命令

在任意目录执行（路径都相对脚本自身）：

```powershell
node third_party/joy/api/gen_bindings.js            # 重生成报告 + thunks + cs glue
node third_party/joy/api/gen_bindings.js --emit     # 同上（显式写法）
node third_party/joy/api/gen_bindings.js --scan     # 从 joy.h 更新 joy.idl + 重生成全部产物
node third_party/joy/api/gen_bindings.js --enable L0|L1|all|domain:<名>|name:<API>
node third_party/joy/api/gen_bindings.js --disable <同样的选择子>
node third_party/joy/api/gen_bindings.js --check    # 只校验不写文件，CI 门禁用
```

默认生成 4 个入库产物：`coverage.md`、`thunks_generated.c`、
`modules/cs/bindings/api_generated.c`、`modules/cs/scripts/Api.generated.cs`。
`--scan` 是幂等的：同一份 `joy.h` 连续跑两次，产物字节不变。

### 批量开放

`--enable` 不是「把选择到的全改成 on」，而是**逐条过闸**：

1. 只考虑 L0 / L1，且不是 `orphan`
2. 参数 / 返回类型必须能被 thunk 与 cs 适配生成（`fp_t`、枚举、结构体一律跳过）
3. 返回句柄的条目，`owns` 目标的唯一参数类型必须与返回类型**严格相同**
4. `void*` 入参没有脚本层裸缓冲语义，跳过
5. 返回 `char*` 的所有权未标注，跳过
6. 返回句柄的条目批量模式一律跳过（可能是借用指针），只能 `name:` 点名开放

命令会打印 `changed / skipped` 与每条跳过原因，写回 IDL 后重生成 4 个产物；
`git diff` 里每条 `status` 变化都可审查。`--disable` 是对称的回退手段。

## IDL 字段

```json
{"name":"font_create","domain":"font","level":"L1","reason":"...","status":"off",
 "owns":"font_destroy","ret":"font_p","params":[{"name":"filename","ctype":"char*","isConst":true}],
 "source":"joy.h:1076","note":""}
```

| 字段 | 归属 | 说明 |
|---|---|---|
| `name` / `domain` / `level` / `reason` / `ret` / `params` / `source` | **自动** | 每次 `--scan` 从 `joy.h` 重新解析并覆盖，不要手改 |
| `status` | **人工** | `off` / `on`。可手改，也可用 `--enable` / `--disable` 批量改 |
| `owns` | **人工** | 配对析构函数名。`--scan` 首次会推断填充，之后永不覆盖；推断错的会进「所有权待裁决」清单 |
| `note` | **人工** | 备注，永不覆盖 |

`--scan` 按 `name` 合并：自动字段刷新，`status` / `owns` / `note` 原样保留。
`joy.h` 里删掉的条目不会丢，会保留并标 `orphan: true`，在报告里单列。

## P1 / P2：共享运行层与 thunk

`runtime/` 是三后端共用的 C 层，`thunks_generated.c` 是 IDL 驱动的生成物：

```text
脚本 VM 值  →  vm.c（各后端唯一语言相关层）  →  joy_value[]
                                              ↓
                                     thunks_generated.c
                                              ↓
                    joy_obj（句柄 / refcount / dtor）→ 真正的 joy C API
```

**当前开放 446 条**：L0 42 条（另 2 条返回 `char*` 被跳过）、L1 404 条
（含首批人工验证的 2 条），跳过 136 条。其余 489 条 = L2 74 + L3 229 + X 50 +
被闸门跳过的 136 条，构成后续阶段的全部工作量。

`joy_thunk_entry` 除 `name` / `domain` / `argc` / `fn` 外，还带
`arg_kinds[]` 与 `ret_kind`：Lua / JS 的 `vm.c` 直接读这张表就知道第 j 个脚本值
该转成哪种 `joy_value`，不必各解析一遍 IDL；方法名统一由 `naming.js` 生成。

`joy_api_selftest()` 对**每一条**开放的 API 故意传错 `argc` 调一次
`joy_api_call()`，验证它确实注册在表里且参数守卫生效。thunk 第一行就返回，
不会执行任何 joy 业务函数，因此批量开放有副作用 / 需要上下文的 API 也不会在
自检里被真的调用。此外保留 L0 标量往返、create→句柄、句柄参数解引用、错误
type tag 被拒绝、`release` 后自动析构与计数配平、失效句柄不可复用等深度自检。
宿主只在 `--smoke` 时调用它，输出 `[api] selftest PASS`。

主仓 CMake 显式把 `runtime/*.c` + `thunks_generated.c` 编入宿主；cs 侧的
`api_generated.c` 位于 `modules/cs/bindings/`，由该模块的 GLOB 自动收集，
它调用的 `joy_api_call()` 来自 `runtime/joy_thunk.c`。等 lua / js 后端也接上后，
这部分装配会上提到 `joy_add.cmake`。

### cs 后端已接通（P1 第二刀）

`csx_create()` 在旧的 `csopen_generated/draw/input/host()` 之后多调一次
`csopen_api()`，注册 `Joy.Api.Native::*`；旧白名单胶水与 C# 业务调用点一行未改。

```text
Joy.Api.Bindings.*（强类型，无 IntPtr）
  → Joy.Api.Native.*（internal call）
  → api_generated.c：Mono 值转 joy_value[]（const char* 走 UTF-8 转换 + mono_free）
  → joy_api_call()（按名查注册表）
  → thunks_generated.c：调用真正的 joy C API + joy_obj 句柄
```

C 层创建的指针被注册为 `uint64_t` 句柄，C# 侧只拿到 `LcgNum` 这类强类型对象；
对象的 finalizer 调 `Native.Release()` → `joy_obj_release()` 自动析构。
`csharp/test/ApiBindingSmoke.cs` 在 `--smoke` 下验证调用链、强类型对象与
GC 后的计数配平，输出 `[api.cs] PASS ... destroyed=1`，并计入 `[smoke] PASS` 断言。

**当前边界**：`--check` 拒绝把 L2 / L3 / X 设成 `on`；生成 thunk 只覆盖标量、
`const char*`、不透明句柄；结构体展开、记录、数组、回调仍留待后续阶段。
Lua / JS 后端尚未写 `vm.c`，所以这 446 条目前只有 cs 端真正可调用。

## 分级

| 级别 | 含义 | 处理方式 |
|---|---|---|
| L0 | 纯标量 + `const char*` | 可直接生成 |
| L1 | 不透明指针 `*_p` / `void*` | 转句柄，需所有权测试 |
| L2 | 结构体按值 | 展开成标量，需 `sizeof` 静态断言 |
| L3 | 回调 / varargs / 数组 / 输出缓冲 / 结构体指针 / 结构体返回 | **不自动生成**，必须手写 thunk |
| X | 条件编译符号、插件系统、测试符号 | 有意排除 |

## 三种常见操作

### 1. 开放 API 给脚本

批量：`node third_party/joy/api/gen_bindings.js --enable L0`（或 `L1` / `all` /
`domain:lcg`），看 `changed / skipped` 与 `git diff`，一起提交 IDL + 4 个生成物。

逐条裁决（返回句柄、缺 `owns`、类型不匹配等）：先在 `coverage.md` 的
「所有权待裁决」一节核对，修正 IDL 的 `owns` 后用
`--enable name:<API>` 点名开放。硬校验（类型匹配）不能靠点名绕过。

### 2. `joy.h` 改了签名 / 加了新 API

1. `node third_party/joy/api/gen_bindings.js --scan`
2. 看 `git diff` —— 自动字段被刷新，`status` / `owns` / `note` 应保持不变
3. 新条目默认 `status: off`；需要开放的按上面操作 1 处理
4. 一起提交 IDL 与全部生成物

忘记 `--scan` 时 CI 的 `--check` 会直接报「joy.h 新增 API 未进 IDL / 签名已变更」。

### 3. 加一个脚本后端

cs 已按此落地：`emit_cs.js` 生成 `bindings/api_generated.c`（注册入口 + 值转换）
与 `scripts/Api.generated.cs`（强类型对象 + GC 通知），`joy_csharp.h` 登记
`csopen_api()`，`jcsx.c` 调用它，`stub.c` 提供关闭模块时的空桩。lua / js 将来
照 `joy_thunks_table()` 的 `arg_kinds` / `ret_kind` 接各自的 `bindings/vm.c`，
方法名从 `naming.js` 取，即可与 cs 端得到同一套 API 名与参数顺序。

## CI 门禁

主仓 `.github/workflows/windows-build.yml` 在构建前执行：

```yaml
node third_party/joy/api/gen_bindings.js --check
```

`--check` 不写文件，会校验：IDL 字段完整、无重复 `name`、`owns` 指向存在、
**开放条目的 `owns` 类型严格匹配**、只有 L0/L1 被开放、`joy.h` 与 IDL 同步，
且 `coverage.md`、`thunks_generated.c`、`modules/cs/bindings/api_generated.c`、
`modules/cs/scripts/Api.generated.cs` 四个生成物都是最新的。任一项不满足即失败；
比较前会统一换行，兼容 Windows CRLF checkout。冒烟另需看到
`[api] selftest PASS` 与 `[api.cs] PASS`。

## 已知边界

- `jmath.h` 是头文件内 `JOY_INLINE` 的值类型数学库，没有 `JOY_API`，不在本次解析范围；
  报告里只给粗统计，需要单独解析并映射成「记录」类型。
- `level` 是保守推断：不认识的类型一律归 L3，宁可要求手写，也不冒险自动生成。
- `runtime/` 与 `thunks_generated.c` 由宿主 CMake 显式编入；`api_generated.c` 由 cs
  模块的 GLOB 自动收集。旧 `MODULES cs` 链路只多了一行 `csopen_api()` 注册。
- 42 条「所有权待裁决」需要人工判定是拥有还是借用指针；判定前 `--check` 会拦住。
- L2 的结构体按值展开、L3 的输出参数 / 回调 / 数组、以及 Lua / JS 的 `vm.c`
  是下一阶段的主要工作。
