# 交接：当前进展与下一步（2026-11-13）

> 换 AI 交接文档。先读这一页，再读 `HANDOFF.md` 的 §2 / §4 / §5 / §6 / §7。
> 本文件只记**当前状态**与**接下来该做什么**；历史细节在 `HANDOFF.md`。
> 当前提交一律以 `git log -1` 为准，本文写作时 HEAD = `795d021a`。

## 0. 一句话现状

全仓 **26 个 `.c` 已做到「一个 `.c` 只承载一个域的函数定义」**，0 混域，
并有 CI 门禁防回归；`main` 与 `origin/main` 同步，三条 CI 工作流全绿。

## 1. 最近做了什么（按提交）

| 提交 | 内容 | 规模 |
|---|---|---|
| `321ceabf` | 清扫全仓命名残留：`jspine_asset_*` 漏网名、26 个公开函数指针 typedef、12 个 `src` 私有 typedef 统一为 `joy_*` | 40 种 C 符号 / 172 处 / 12 文件 |
| `9f9f0cde` | **源码按域重划**：让函数名的域前缀跟着文件的技术职责走，26 个 `.c` 全部纯域 | 119 条 API 改名 / 562 处 / 25 文件 |
| `795d021a` | 固化域纯度规则为可执行门禁；清理 `jr2d.c` 注释里会误导的跨域函数名 | 新增 `api/tools/audit_domain_purity.js` + CI gate |

### 关键决策：为什么是「改域名」而不是「搬 400 条函数」

用户要求「一个 `.c` 只能有一个域的函数」，并指出 `joy_ui_image_create`
本质是渲染层资源而非 ui 控件。做法是**让函数名的域前缀跟着文件的技术职责走**
（函数留在原地），而不是把实现搬进对应域的文件。原因：

- 搬实现要外提 `struct joy_r2d`（含 `sdl`、`frame`、裁剪栈、FOV 纹理、
  文字缓存）等私有状态，风险远大于收益；
- `JoySources.cmake` 头部明确要求各 `.c` 保持独立数据模型，不合并成巨型 TU；
- 搬 `.c` 对生成链零影响（IDL 的 `source` 记的是 `japi.h` 声明行号），
  但改域名会动 IDL 的 `domain` 字段与 `coverage.md` 分组 —— 这是预期内的。

### 域归属判据（后续同类问题沿用）

| 判据 | 归入 |
|---|---|
| 纹理 / 文字 / 图元 / 动画 / 2D FOV | `r2d` |
| 3D FOV / glTF | `r3d` |
| 控件（joystick / label / radio） | `ui` |
| 行为树（bt） | `scene` |
| tilemap 碰撞扫描 | `math` |
| 网络打包 / 时间工具 | `core` |

一个域**可以**有多个 `.c`（按子系统拆分），但一个 `.c` 只能有一个域。

## 2. 当前硬事实

| 项 | 值 |
|---|---|
| HEAD / 远端 | `795d021a`，`main` 与 `origin/main` 一致，工作区干净 |
| IDL | 1170 条，`status:on` 739 条，vtable 739 |
| 域纯度 | 26 个 `.c` / 1013 条函数定义 / **混域 0** |
| `--check` | OK（report/thunks/vtable/cs glue/jmath 全部最新） |
| 全量 Debug 构建 | 0 错误 |
| `ctest` | 4/4 |
| CI | Desktop（Windows/Linux/macOS）+ Android + Web **3/3 success** |
| 人工字段 | `borrowed` 12、`owns` 非空 83、`note` 非空 42 |

### 文件 → 域 归属表（26 个 `.c`）

| 域 | 文件（函数定义条数） |
|---|---|
| `core` | `jcore.c`(41)、`jutils.c`(56) |
| `math` | `jcollision.c`(58)、`jgjk.c`(21)、`jbox2d.c`(11) |
| `r2d` | `jr2d.c`(92)、`jspine.c`(22)、`jparticle.c`(10)、`jfont.c`(5) |
| `r3d` | `jr3d.c`(149)、`jlight.c`(11)、`jgltf.c`(5) |
| `ui` | `jui.c`(183)、`jui_im.c`(37) |
| `scene` | `jscene.c`(65)、`janimator.c`(27)、`jtilemap.c`(23)、`jbehavior.c`(14)、`jecs.c`(13)、`jprocmap.c`(8) |
| `net` | `jnetwork.c`(67)、`jsys.c`(29) |
| `script` | `jmsgpack.c`(22)、`jmodule.c`(11) |
| `audio` | `jaudio.c`(20) |
| `input` | `jinput.c`(13) |

`jlut.c` / `jvendor_impl.c` 不含 `joy_*` 函数定义（查表与第三方实现），不参与域归属。

## 3. 下一步（按优先级）

### P0 — 修 `src/jgltf.c` 的预存编译错误（有明确修法，约 10 分钟）

`HANDOFF.md` §6 记着「`src/jgltf.c` 预存语法错误，文件本身待修」，本轮已定位到根因：

```text
src/jgltf.c:66    jgltf_asset_p asset;              → error: unknown type name 'jgltf_asset_p'
src/jgltf.c:409   jgltf_asset_p jgltf_asset_load(...)
```

文件里定义了 `struct joy_gltf_asset {...}`（`src/jgltf.c:48`）和
`struct joy_gltf_instance {...}`（`:65`），但**从未给它们 typedef 别名**；
全文却用 `jgltf_asset_p` 指代。收口版本 `240f5f7a` 里同样没有这个 typedef，
所以是**一直在的预存缺陷**，与本轮改名无关（`9f9f0cde` 只改了 5 个函数名）。

修法：

1. 在 `struct joy_gltf_instance` 定义之后补一行
   `typedef struct joy_gltf_asset joy_gltf_asset_t, *joy_gltf_asset_p;`
   （`joy_gltf_instance` 同理，若它在别处也被用）。
2. 把该文件内所有 `jgltf_asset_p` 全词替换为 `joy_gltf_asset_p`。
3. 验证（`JOY_ENABLE_GLTF` 默认 OFF，桌面构建覆盖不到，必须单独查语法）：

   ```powershell
   gcc -fsyntax-only -I. -Iinclude -Iinclude/joy -ISDL/include `
       -Imsgpack/include -DJOY_USE_GLTF src/jgltf.c
   ```

   注意 `#include "external/cgltf/cgltf.h"` 是相对仓库根的，`-I.` 不能少；
   `#include "joy.h"` 需要 `-Iinclude/joy`。
4. 修完跑一次 `-DJOY_ENABLE_GLTF=ON` 的实际构建确认（不只是语法检查）。

> 修之前先确认 `jgltf.c` 里这 5 个函数是否真的是它自己的：`joy_r3d_gltf_asset_animation_name` /
   `_animation_duration` / `_has_skin` / `_bounds` / `joy_r3d_gltf_instance_draw_bones`。
   它们不在 IDL（可选模块默认不构建，`--scan` 扫不到），改类型名不影响生成链。

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
   并归到对应域组。
3. **`legacy_names.txt` 里那 6 条旧名（4 个 `joy_fov_*` + `joy_tilemap_sweep_x|y`）
   已经失效**：本轮改名后 C 名分别是 `joy_r2d_fov_draw_2d`、`joy_r3d_fov_*`、
   `joy_math_tilemap_sweep_x|y`，全部落在白名单域内。别再当成「例外」去豁免。
   该文件仍须保留（存量 1156 条靠它通过 `--check`）。
4. **域纯度门禁的判定口径**（`api/tools/audit_domain_purity.js`）：
   只算函数定义（签名后紧跟 `{`）。跨域**调用**、注释提及、共享句柄类型
   （`joy_image_p` 等）都**不算**混域。`jr2d.c` 调 `joy_core_get_raw_window`、
   `joy_core_utf8_decode_all`、`joy_core_utils_get_current_time` 是正常单向依赖，
   不要去「修」。
5. **注释里写别的域的函数名也会让人误判成混域**（本轮就清过 `jr2d.c` 两处）。
   新写注释时用「按钮 / 摇杆等控件」这类描述，不要点名别域符号。
6. 冒烟必须带 `--timeout 25`，历史上出现过渲染卡死拖死整轮测试。
7. 本机的 `git push` 输出会走 stderr（PowerShell 包成 NativeCommandError），
   只要看到 `xxx..yyy main -> main` 就是成功，不要被红字吓到。

## 5. 验证命令（复制即用）

```powershell
cd e:/c++/joy

# 1. 域纯度（一个 .c 一个域）
node api/tools/audit_domain_purity.js        # 退出码 0 = 通过

# 2. 生成链门禁
node api/tools/gen_bindings.js --check        # OK: 1170 条

# 3. 幂等性（改过名之后必查）
$b=(Get-FileHash api/idl/joy.idl).Hash
node api/tools/gen_bindings.js --scan
$a=(Get-FileHash api/idl/joy.idl).Hash
"IDL 字节未变: $($b -eq $a)"                  # 必须 True

# 4. 全量构建 + 四语言冒烟
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure --timeout 25   # 必须 4/4

# 5. 可选模块（默认 OFF，改过 jspine/jgltf/jmodule 必查）
cmake -S . -B build/opt_check -DJOY_ENABLE_SPINE=ON -DJOY_ENABLE_GLTF=ON
cmake --build build/opt_check --config Debug

# 6. 收尾
git --no-pager diff --check                   # 无输出
git push origin main                          # 然后等三条 CI 全绿再收工
```

## 6. 协作约定（用户偏好，已被多次强调）

- 用户要的是**动手**，不是方案；说「开始处理」就直接改，改完跑验证、提交、推送。
- 提交信息写清「改了什么 / 为什么 / 怎么验证的」，中文正文，按 §8 的风格。
- 推送后**必须等 CI 出结果**再宣布完成（用户明确批评过「没等 CI 就收尾」）。
- 不要新建无谓的文档；报表、审计脚本这类临时产物放 `build/`（已被 gitignore），
  沉淀成正式工具的才放 `api/tools/`。
- 大改动前先把「影响面 + 冲突检查 + 继承链」算清楚再动手，`--dry-run` 先跑一遍。
