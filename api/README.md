# joy 绑定 API 契约（P0）

这里放**一份与脚本语言无关的接口清单**，以及维护它的工具。目标是让 Lua / JS / C#
三个后端将来共用同一套对象模型、方法名、参数与所有权规则。

> **边界**：本目录只产出契约、报告，以及可被三后端共用的 C 运行层 / 薄适配生成物。
> 旧的 `modules/*/bindings/` 手写胶水一行未删，新旧链路并行；哪些 API 真正开放由
> `joy.idl` 的 `status: on` 决定，默认全部关闭。所有生成物都提交进仓，
> `git diff` 随时能看清改了什么。

## 文件

| 文件 | 谁维护 | 作用 |
|---|---|---|
| `gen_bindings.js` | 人工 | 解析 `joy.h`、维护 IDL、产出全部生成物。零 npm 依赖，只用 Node 内置 `fs` / `path` |
| `emit_thunks.js` | 人工 | 共享 thunk 生成器（纯字符串函数），由 `gen_bindings.js` 调用 |
| `emit_cs.js` | 人工 | cs 后端薄适配生成器（纯字符串函数），产出 C icall 与 C# 强类型包装 |
| `joy.idl` | 工具初始化 + 人工补充 | 唯一真相源。JSON 数组、**每条 API 占一行**，便于 diff |
| `coverage.md` | 工具生成，勿手改 | 分级统计、按域分布、`owns` 待办、L3 手写清单 |
| `thunks_generated.c` | 工具生成，勿手改 | 只有 `status: on` 的 L0/L1 条目会生成 thunk 与自检 |
| `modules/cs/bindings/api_generated.c` | 工具生成，勿手改 | Mono 值 → `joy_value[]` → `joy_api_call()`，注册 `Joy.Api.Native::*` |
| `modules/cs/scripts/Api.generated.cs` | 工具生成，勿手改 | `Joy.Api.Bindings` / `Diagnostics` / 强类型句柄类，public API 无 `IntPtr` |
| `runtime/joy_value.*` | 人工 | 统一值容器（标量 / 字符串 / 句柄） |
| `runtime/joy_obj.*` | 人工 | 句柄表、引用计数、类型 tag 校验、自动析构 |
| `runtime/joy_thunk.*` | 人工 | thunk 签名 / 注册表，以及按名调用入口 `joy_api_call()` |
| `joy_api.h` | 人工 | 对外唯一入口：`joy_api_selftest()` |

## 命令

在任意目录执行（路径都相对脚本自身）：

```powershell
node third_party/joy/api/gen_bindings.js            # 重生成报告 + thunks + cs glue
node third_party/joy/api/gen_bindings.js --emit     # 同上（显式写法）
node third_party/joy/api/gen_bindings.js --scan     # 从 joy.h 更新 joy.idl + 重生成全部产物
node third_party/joy/api/gen_bindings.js --check    # 只校验不写文件，CI 门禁用
```

默认生成 4 个入库产物：`coverage.md`、`thunks_generated.c`、
`modules/cs/bindings/api_generated.c`、`modules/cs/scripts/Api.generated.cs`。
`--scan` 是幂等的：同一份 `joy.h` 连续跑两次，产物字节不变。

## IDL 字段

```json
{"name":"font_create","domain":"font","level":"L1","reason":"...","status":"off",
 "owns":"font_destroy","ret":"font_p","params":[{"name":"filename","ctype":"char*","isConst":true}],
 "source":"joy.h:1076","note":""}
```

| 字段 | 归属 | 说明 |
|---|---|---|
| `name` / `domain` / `level` / `reason` / `ret` / `params` / `source` | **自动** | 每次 `--scan` 从 `joy.h` 重新解析并覆盖，不要手改 |
| `status` | **人工** | `off`（默认）/ `on`。验证过一条才改成 `on` |
| `owns` | **人工** | 配对析构函数名。`--scan` 首次会推断填充，之后永不覆盖；返回 `*_p` 却没有析构的会列进报告待办 |
| `note` | **人工** | 备注，永不覆盖 |

`--scan` 按 `name` 合并：自动字段刷新，`status` / `owns` / `note` 原样保留。
`joy.h` 里删掉的条目不会丢，会保留并标 `orphan: true`，在报告里单列。

## P1：共享运行层与 thunk（进行中）

`runtime/` 是三后端共用的 C 层，`thunks_generated.c` 是 IDL 驱动的生成物：

```text
脚本 VM 值  →  vm.c（各后端唯一语言相关层）  →  joy_value[]
                                              ↓
                                     thunks_generated.c
                                              ↓
                    joy_obj（句柄 / refcount / dtor）→ 真正的 joy C API
```

**首批只开放 4 条零副作用 API**（`status: on`）：`get_ticks`、`jtext_cache_get_count`
（L0），`lcg_random_create`、`lcg_random_next`（L1，`owns=lcg_random_destroy`，纯 `SDL_malloc`）。
其余 931 条仍是 `off`。

`thunks_generated.c` 末尾的 `joy_api_selftest()` 验证 L0 标量往返、参数个数校验、
create→句柄、句柄参数解引用、错误 type tag 被拒绝、`release` 后自动析构与计数配平、
失效句柄不可复用。宿主只在 `--smoke` 时调用它，输出 `[api] selftest PASS`；
正常模式与现有 C# 业务链路完全不受影响。

主仓 CMake 显式把 `runtime/*.c` + `thunks_generated.c` 编入宿主；cs 侧的
`api_generated.c` 位于 `modules/cs/bindings/`，由该模块的 GLOB 自动收集，
它调用的 `joy_api_call()` 来自 `runtime/joy_thunk.c`。等 lua / js 后端也接上后，
这部分装配会上提到 `joy_add.cmake`。

### cs 后端已接通（P1 第二刀）

`csx_create()` 在旧的 `csopen_generated/draw/input/host()` 之后多调一次
`csopen_api()`，注册 `Joy.Api.Native::*`；旧白名单胶水与 C# 业务调用点一行未改。
链路是：

```text
Joy.Api.Bindings.*（强类型，无 IntPtr）
  → Joy.Api.Native.*（internal call）
  → api_generated.c：Mono 值转 joy_value[]
  → joy_api_call()（按名查注册表）
  → thunks_generated.c：调用真正的 joy C API + joy_obj 句柄
```

C 层创建的指针被注册为 `uint64_t` 句柄，C# 侧只拿到 `LcgNum` 这类强类型对象；
对象的 finalizer 调 `Native.Release()` → `joy_obj_release()` 自动析构。
`csharp/test/ApiBindingSmoke.cs` 在 `--smoke` 下验证调用链、强类型对象与
GC 后的计数配平，输出 `[api.cs] PASS ... destroyed=1`，并计入 `[smoke] PASS` 断言。

**当前边界**：`--check` 拒绝把 L2 / L3 / X 设成 `on`；生成 thunk 只覆盖标量、
`const char*`、不透明指针；结构体展开、记录、数组、回调仍留待后续阶段。
cs 侧目前只支持标量 + 不透明句柄，Lua / JS 后端尚未接 `vm.c`。

## 分级

| 级别 | 含义 | 处理方式 |
|---|---|---|
| L0 | 纯标量 + `const char*` | 可直接生成 |
| L1 | 不透明指针 `*_p` / `void*` | 转句柄，需所有权测试 |
| L2 | 结构体按值 | 展开成标量，需 `sizeof` 静态断言 |
| L3 | 回调 / varargs / 数组 / 输出缓冲 / 结构体指针 / 结构体返回 | **不自动生成**，必须手写 thunk |
| X | 条件编译符号、插件系统、测试符号 | 有意排除 |

`--check` 会拒绝把 L3 / X 直接设成 `status: on`。

## 三种常见操作

### 1. 开放一个 API 给脚本

1. 在 `coverage.md` 里确认它的级别是 L0 / L1（其余级别暂不能自动生成 thunk）
2. 在 `joy.idl` 里把该条 `status` 改成 `on`，必要时补 `note`
3. `node third_party/joy/api/gen_bindings.js` 重生成全部 4 个生成物
4. 看 `git diff` —— 应只有那一条 `status`、总览数字，以及新增的 thunk / cs 包装
5. 一起提交 `joy.idl` + 4 个生成物

`owns` 必须指向 `joy.h` 里真实存在的函数，否则 `--check` 会失败。
带 `const char*` / 结构体的条目暂不受 cs 适配支持，`--check` 会明确报错。

### 2. `joy.h` 改了签名 / 加了新 API

1. `node third_party/joy/api/gen_bindings.js --scan`
2. 看 `git diff` —— 自动字段被刷新，`status` / `owns` / `note` 应保持不变
3. 新条目默认 `status: off`；返回句柄却没推断出 `owns` 的，去报告「待办」一节人工补
4. 一起提交 IDL 与全部生成物

忘记 `--scan` 时 CI 的 `--check` 会直接报「joy.h 新增 API 未进 IDL / 签名已变更」。

### 3. 加一个脚本后端

cs 已按此落地：`emit_cs.js` 生成 `bindings/api_generated.c`（注册入口 + 值转换）
与 `scripts/Api.generated.cs`（强类型对象 + GC 通知），`joy_csharp.h` 登记
`csopen_api()`，`jcsx.c` 调用它，`stub.c` 提供关闭模块时的空桩。lua / js 将来
照同一份 `joy_thunks_table()` 接各自的 `bindings/vm.c` 即可；共享的 thunk 与对象表
由本目录的 IDL 驱动，不需要为每个后端重写一遍 joy API。

## CI 门禁

主仓 `.github/workflows/windows-build.yml` 在构建前执行：

```yaml
node third_party/joy/api/gen_bindings.js --check
```

`--check` 不写文件，会校验：IDL 字段完整、无重复 `name`、`owns` 指向存在、
只有 L0/L1 被开放、`joy.h` 与 IDL 同步，且 `coverage.md`、`thunks_generated.c`、
`modules/cs/bindings/api_generated.c`、`modules/cs/scripts/Api.generated.cs`
四个生成物都是最新的。任一项不满足即失败；比较前会统一换行，兼容 Windows
CRLF checkout。冒烟另需看到 `[api] selftest PASS` 与 `[api.cs] PASS`。

## 已知边界

- `jmath.h` 是头文件内 `JOY_INLINE` 的值类型数学库，没有 `JOY_API`，不在本次解析范围；
  报告里只给粗统计，P1 需要单独解析并映射成「记录」类型。
- `level` 是保守推断：不认识的类型一律归 L3，宁可要求手写，也不冒险自动生成。
- `runtime/` 与 `thunks_generated.c` 由宿主 CMake 显式编入；`api_generated.c` 由 cs
  模块的 GLOB 自动收集。旧 `MODULES cs` 链路只多了一行 `csopen_api()` 注册。
- cs 适配目前只覆盖标量 + 不透明句柄；`const char*` / 结构体参数即使设成 `on`，
  `emit_cs.js` 也会抛错并由 `--check` 拦下，需先扩展类型映射。
