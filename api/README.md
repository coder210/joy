# joy 绑定 API 契约（P0）

这里放**一份与脚本语言无关的接口清单**，以及维护它的工具。目标是让 Lua / JS / C#
三个后端将来共用同一套对象模型、方法名、参数与所有权规则。

> **P0 边界**：本目录只产出契约与报告，**不生成任何会被调用的胶水代码**。
> `modules/*/bindings/` 里现有的手写胶水一行未动，宿主构建行为不变。
> 生成物（`joy.idl`、`coverage.md`）都提交进仓，`git diff` 随时能看清改了什么。

## 文件

| 文件 | 谁维护 | 作用 |
|---|---|---|
| `gen_bindings.js` | 人工 | 解析 `joy.h`、维护 IDL、产出报告。零 npm 依赖，只用 Node 内置 `fs` / `path` |
| `emit_thunks.js` | 人工 | P1 thunk 生成器（纯字符串函数），由 `gen_bindings.js` 调用 |
| `joy.idl` | 工具初始化 + 人工补充 | 唯一真相源。JSON 数组、**每条 API 占一行**，便于 diff |
| `coverage.md` | 工具生成，勿手改 | 分级统计、按域分布、`owns` 待办、L3 手写清单 |
| `thunks_generated.c` | 工具生成，勿手改 | 只有 `status: on` 的 L0/L1 条目会生成 thunk 与自检 |
| `runtime/joy_value.*` | 人工 | 统一值容器（标量 / 字符串 / 句柄） |
| `runtime/joy_obj.*` | 人工 | 句柄表、引用计数、类型 tag 校验、自动析构 |
| `runtime/joy_thunk.h` | 人工 | thunk 签名与注册表类型 |
| `joy_api.h` | 人工 | 对外唯一入口：`joy_api_selftest()` |

## 命令

在任意目录执行（路径都相对脚本自身）：

```powershell
node third_party/joy/api/gen_bindings.js            # 重生成 coverage.md + thunks_generated.c
node third_party/joy/api/gen_bindings.js --emit     # 同上（显式写法）
node third_party/joy/api/gen_bindings.js --scan     # 从 joy.h 更新 joy.idl + 重生成两者
node third_party/joy/api/gen_bindings.js --check    # 只校验不写文件，CI 门禁用
```

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

主仓 CMake 目前显式把 `runtime/*.c` + `thunks_generated.c` 编入宿主做 C 层验证；
等 lua / js 后端接上后，这部分装配会上提到 `joy_add.cmake`。

**当前边界**：`--check` 拒绝把 L2 / L3 / X 设成 `on`；生成 thunk 只覆盖标量、
`const char*`、不透明指针；结构体展开、记录、数组、回调仍留待后续阶段。
三个后端尚未接 `vm.c`，所以这套 API 目前只对本仓 C 层自检可见，脚本层还看不到。

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
3. `node third_party/joy/api/gen_bindings.js` 重生成报告
4. 看 `git diff` —— 应只有那一条 `status` 和总览数字变化
5. 一起提交 `joy.idl` + `coverage.md`

`owns` 必须指向 `joy.h` 里真实存在的函数，否则 `--check` 会失败。

### 2. `joy.h` 改了签名 / 加了新 API

1. `node third_party/joy/api/gen_bindings.js --scan`
2. 看 `git diff` —— 自动字段被刷新，`status` / `owns` / `note` 应保持不变
3. 新条目默认 `status: off`；返回句柄却没推断出 `owns` 的，去报告「待办」一节人工补
4. 一起提交两个生成物

忘记 `--scan` 时 CI 的 `--check` 会直接报「joy.h 新增 API 未进 IDL / 签名已变更」。

### 3. 加一个脚本后端

P0 阶段不产生后端代码。将来在 `modules/<name>/` 下建 `bindings/vm.c`（VM 值 ↔
统一值容器、GC 回收通知、错误抛出）和 `bindings/registry.c`（注册入口）即可；
共享的 thunk 与对象表由本目录的 IDL 驱动，不需要为每个后端重写一遍 joy API。

## CI 门禁

主仓 `.github/workflows/windows-build.yml` 在构建前执行：

```yaml
node third_party/joy/api/gen_bindings.js --check
```

`--check` 不写文件，会校验：IDL 字段完整、无重复 `name`、`owns` 指向存在、
只有 L0/L1 被开放、`joy.h` 与 IDL 同步、`coverage.md` 与 `thunks_generated.c`
都是最新的。任一项不满足即失败；比较前会统一换行，兼容 Windows CRLF checkout。

## 已知边界

- `jmath.h` 是头文件内 `JOY_INLINE` 的值类型数学库，没有 `JOY_API`，不在本次解析范围；
  报告里只给粗统计，P1 需要单独解析并映射成「记录」类型。
- `level` 是保守推断：不认识的类型一律归 L3，宁可要求手写，也不冒险自动生成。
- 本目录不参与 CMake 构建，不影响现有 `MODULES cs` 链路。
