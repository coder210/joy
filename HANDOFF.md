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

当前：**1157 条 IDL，721 条 `status:on`，vtable 721 个强签名**（其中 7 条是
`borrowed` 借用句柄见 §3 的 14，13 条 3D mesh 靠单参数 dtor 开放见 §3 的 15）。

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
   验证：`--scan` 幂等（1157 条，r2d_render_geometry 不进 IDL）、`--check` OK、构建 0 错误、`ctest` 4/4。

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
> 类名逐字不变；`struct tag` 因 127 个无后缀通用词会误伤变量而**有意保持原名**。

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
   - 对一组名字批量替换时，旧名互不为前缀子串（如 `r2d_create` 与 `r2d_create_texture`）
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
   - `struct tag`（`label`/`button`/`circle` 等 127 个无后缀通用词）**不能全词替换**，
     会误伤同名变量；类型与 tag 在 C 里属不同命名空间，保持原名即可。
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

