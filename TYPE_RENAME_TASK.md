# 任务书：C 类型名统一为 `joy_<概念>_t` / `joy_<概念>_p`

> **状态：待执行，尚未开始任何改动。** 2026-11-11 立项，用户已确认命名规则。
> 前置阅读：`HANDOFF.md`（项目现状与生成链路背景）、`api/README.md`（命名域表与改名记录）。
> 本文件是**可执行任务书**，不是讨论稿。按 §7 分批做，每批验收通过再进下一批。

## 0. 一句话目标

把所有公开 C 类型名（`_t` / `_p`）统一成 `joy_` 前缀；**脚本层（C# / Lua / JS）的
类名与方法名保持不变**，四份示例的游戏逻辑零改动。

## 1. 命名规则（唯一一条，不留例外）

```
j<概念>   →  joy_<概念>
<概念>    →  joy_<概念>
后缀 _t / _p 保持不变
```

| 改前 | 改后 |
|---|---|
| `jr2d_p` | `joy_r2d_p` |
| `jwindow_p` | `joy_window_p` |
| `jrect_t` | `joy_rect_t` |
| `vec2_t` | `joy_vec2_t` |
| `font_p` | `joy_font_p` |
| `image_p` | `joy_image_p` |
| `jtilemap_p` | `joy_tilemap_p` |
| `box2d_world_t` | `joy_box2d_world_t` |
| `joystick_p` | `joy_joystick_p` |

**不插域名。** 不要做成 `joy_r2d_font_p` 这种形式——类型的命名空间是扁平的，插域会把
平均长度从 12.0 拉到 19.4（+68%），且 231 个类型里有 120 个无法从 IDL 推断归属域。
这个取舍已经和用户确认过（`joy_` 前缀 + 不插域，平均 +30%）。

`struct` tag 与 typedef 同前缀：

```c
typedef struct joy_font joy_font_t, *joy_font_p;
```

## 2. 范围与实测规模（2026-11-11 统计）

| 层 | 数量 | 处理方式 |
|---|---|---|
| 公开头 `_t` / `_p` 名字 | 239 个（含 8 个标准 C 类型，须排除） | 实际改名 **231** |
| `src/` + `include/joy/` 引用 | 6533 处 / 30 文件 | 脚本替换 |
| `examples/` + `modules/` 手写源 | 2926 处 / 15 文件（含生成物） | 脚本替换，手改量更少 |
| `api/idl/` + `api/generated/` | 7959 处 / 7 文件 | **不手改**，`--emit` 重新生成 |
| `.c` 内私有类型 | 约 60 个 | 一并统一（生成器不扫 `src/*.c`，零风险） |

效果：平均长度 12.0 → 15.7（**+30%**），最长 26 字符（`joy_tilemap_object_layer_p`），
新名冲突 **0**。对照上一版「带域」方案（+68%、最长 32、120 条需人工裁定）。

## 3. 四类必须单独处理的例外

### 3.1 `joystick_p` → `joy_joystick_p`

`j` 是单词本身的一部分，不是项目前缀。全仓仅此一例，加白名单。

### 3.2 排除 8 个标准 C 类型

`int8_t`、`int16_t`、`int32_t`、`int64_t`、`uint8_t`、`uint32_t`、`uint64_t`、`size_t`

不排除会生成 `joy_uint32_t`、`joy_core_size_t` 这类名字。采集正则必须显式排除。

### 3.3 `jvector_t` / `jvector_p` 是**宏函数**，不是类型

```c
/* include/joy/defs.h:143-144 */
#define jvector_t(name) jvector_##name##_t
#define jvector_p(name) jvector_##name##_p
```

真类型在 `include/joy/jcont.h:72-77`：`typedef struct vector { ... } vector_t;`

处理方式：

- `vector_t` → `joy_vector_t`（正常改名，这是 jcont 的容器结构体）
- **宏不要改成 `joy_vector_t` / `joy_vector_p`** —— 宏会被预处理直接替换，和类型名撞车。
  建议改为 `joy_vector_decl(name)` / `joy_vector_handle(name)`，或干脆保持原名。
- 映射表里**不要**给 `jvector_t` / `jvector_p` 建条目，这是假冲突。

### 3.4 `fp_t` / `ft_t` 是 `#define` 标量宏

```c
/* include/joy/jmath.h:38-39 */
#define fp_t int64_t
#define ft_t float
```

它们不是结构体，但既然要「统一」，一并改成 `joy_fp_t` / `joy_ft_t`。
**必须同步两处硬编码**（见 §5），否则生成的 `jmath_wrappers.c` 会引用不存在的类型。

## 4. 关键设计：冻结脚本层类名（否则四份示例会炸）

`api/tools/naming.js` 的 `handleClass()` 当前是：

```js
function handleClass(ret) { return pascal(ret.replace(/_p$/, '')); }
```

`font_p → Font`。改名后会变成 `joy_font_p → JoyFont`，波及 **57 个 C# 句柄类**、
**56 个 C++ vtable 强句柄 typedef**、以及四份示例源码。

**做法**：新建 `api/idl/handle_aliases.json`，key 用**新类型名**、value 是**当前类名基线**：

1. 从 `api/generated/joy_api_vtable.h` 提取 `typedef struct X { uint64_t h; } X;` 的 X（56 个）
2. 从 `modules/cs/scripts/Api.generated.cs` 提取 `public sealed class X`（57 个）
3. `handleClass()` 优先查表，查不到才 fallback 到 `pascal()`

**验收**：改名前后两个类名集合**逐字相同**。这与 P1 用 `ALIASES` 冻结脚本方法名是同一套已验证的思路。

## 5. 必须同步的生成器硬编码（漏一个就编译失败）

| 位置 | 内容 | 处理 |
|---|---|---|
| `api/tools/jmath_wrap.js` 约 L176 | 硬编码 `vec2_t _r;` | 改 `joy_vec2_t` |
| `api/tools/jmath_wrap.js` 约 L39-40 | `fp_t` / `ft_t` 标量映射表 | 同步新名 |
| `api/tools/structs.js` 约 L25 | `TYPE_ALIASES = { 'ft_t': 'float' }` | 同步新名 |
| `api/tools/structs.js` 约 L51-54 | 正则只收「收尾单个、以 `_t` 结尾」的别名 | **新名必须保持 `_t` 结尾** |
| `api/tools/naming.js` 约 L1166-1168 | `isOpaque()` 只认 `_p` 结尾 | **新名必须保持 `_p` 结尾** |
| `api/tools/naming.js` 约 L1178 | `handleClass()` | 加冻结表（§4） |
| `api/tools/gen_bindings.js` `inferOwns()` | 靠 `params[0].ctype === ret` 比较 | 两边同时改名仍相等，**无需改** |

## 6. 映射表与替换工具

### 6.1 `api/idl/type_rename_map.json`（新建）

约 231 条 `"旧名": "新名"`。按 §1 规则**纯规则生成**，再逐条确认 §3 的四类例外。
不要手敲 231 条。

### 6.2 `api/tools/rename_types.js`（新建）

硬性要求：

- 读映射表，按**字符类边界**全词替换：`(^|[^A-Za-z0-9_])old(?=$|[^A-Za-z0-9_])`
  - 不要用 `\b`，不要用子串 `replaceAll`（P1 踩过这个坑）
- 保留每个文件原有换行风格（`src/jr3d.c` 是 **CRLF**，`HANDOFF.md` 是 LF）
- **原位替换、不增删行** —— 保住 IDL 的 `source` 行号
- 先 `--dry-run` 打印每个名字的命中数与文件清单，确认后再 `--write`
- 排除目录：`SDL/`、`msgpack/`、`spine/`、`build/`
- 排除生成物：`api/generated/*`、`modules/cs/scripts/Api.generated.cs`

## 7. 分 6 批执行（每批一个可 revert 的提交）

| 批 | 内容 | 验收 |
|---|---|---|
| 0 | 映射表 + 工具 + `handle_aliases.json`；**不改任何现有源文件** | dry-run 报告；231 条映射 + 例外逐条确认 |
| 1 | `.c` 内私有类型（约 60 个，不外泄） | `--check` 应 **0 变化**、构建 0 错误、`ctest` 4/4 |
| 2 | **原子批**：公开头 + `src/` 定义 + §5 硬编码 + `handleClass` 冻结表 + `--scan` + `--emit` | 中途不可编译，**不许拆开验收**；完成后全量构建 + `ctest` + `--check` |
| 3 | `examples/` / `modules/` 手写源残留 | 构建 + `ctest` + 全仓全词审计 0 旧名 |
| 4 | 公开头 52 个无前缀短宏加 `JOY_`（含把 `CONIFG_H` 拼写改对） | 构建 + `ctest` |
| 5 | 全量验收 + 推送 + CI 三工作流 | 见 §8 |

## 8. 验收门禁

每批至少：

- `node api/tools/gen_bindings.js --check` OK
- `cmake --build build --config Debug` 0 错误
- `ctest --test-dir build -C Debug --timeout 25` **4/4**
- `git --no-pager diff --check` 0、lint 0

最终追加：

- `--scan` 幂等（IDL 字节不变），人工字段 `status` / `owns` / `borrowed` / `note` 全部保留
- `status:on` 仍是 **721** 条；`level` 分布（L0/L1/L2/L3/X）不变
- 脚本层类名集合逐字相同（§4）
- 全仓全词审计：0 处旧类型名（改名记录 / 历史留档除外）
- GitHub Actions 三工作流 `success`

## 9. 风险

1. **批 2 中间态不可编译**（生成物仍引用旧名）→ 必须原子完成。
2. `jvector_t` 是宏，误当类型改名会撞车（§3.3）。
3. `ft_t` / `fp_t` 漏同步会让 `jmath_wrappers.c` 编译失败（§5）。
4. `_t` 是 POSIX 保留后缀 —— 现状如此，本次不改变这个事实。
5. 生成物一律由 `--emit` 重生成、**绝不手改**，保证 `git revert` 干净。

## 10. 开工顺序

1. 读 `HANDOFF.md` §10 确认基线（`--check` / 全量构建 / `ctest`）。
2. 批 0：生成映射表 + 工具 + 冻结表，`--dry-run` 核对命中数。
3. 批 1 独立验收后先提交（风险最低，可单独回滚）。
4. 批 2 一次做完再验收，不要中途编译。
5. 每批提交信息按 `HANDOFF.md` §8 的风格写（中文、`type(scope): 摘要`、正文列改动点、末行写验证）。
