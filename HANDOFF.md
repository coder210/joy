# 交接文档（临时）

> 用途：让接手的 AI / 人快速接手本仓库当前状态。任务收敛后可删除。
> 对应提交：`bd12eab2`（2026-11-09），分支 `main`，已推送 `origin/main`。

## 1. 项目是什么

joy 是一个 C 引擎（`src/` + `include/joy/`），用 `api/tools/gen_bindings.js` 从
`include/joy/api.h` 生成四份后端的绑定（C++ vtable、C thunk、C# P/Invoke、jmath
包装）。`examples/` 下有 cpp / lua / js / csharp 四份功能相同的示例，**它们同时是
给以后项目用的模板**，所以改动要按模板标准处理（注释、平台分支、可覆盖参数）。

生成链路的分级（`api/idl/joy.idl` 每条带 `level` + `status`）：

| 级别 | 含义 | 能否自动生成 |
|---|---|---|
| L0/L1 | 全标量 / 句柄 + 标量 | 能，`status:on` |
| L2 | 值类型结构体按字段展开 | 能，需 `structs.js` 认得字段类型 |
| L3 | 结构体指针 / 出参 / 回调 / 可变参 | 不能，`status:off`，消费方退化 |

当前：**1156 条 IDL，701 条 `status:on`，vtable 701 个强签名**。

## 2. 当前状态

- 工作区干净，本地与 `origin/main` 同为 `bd12eab2`。
- 桌面（Windows / VS2022 / Debug）：全量构建 0 错误，`ctest` 4/4 通过。
- `node api/tools/gen_bindings.js --check` 通过（report/thunks/vtable/cs glue/jmath 全部最新）。
- Android：代码与 CI 已就绪，**但从未在 CI 上跑过**（见第 6 节）。

## 3. 已经做完的事（按提交）

`7f37e951` — 强签名 JoyApi vtable，C++ 示例改原生 DLL 插件
`ccb6232a` — C++ 双形态模板 + 贴图恢复，生成链路开放至 701 条
`bd12eab2` — 打通 C# 的 Android 构建，MonoVM 运行时按平台入库

具体内容：

1. **`image_draw_rect`**（`include/joy/api.h` 末尾、`src/jr2d.c`）
   全标量 L1，`srcrect` 固定 NULL，坐标是**像素**。绕开 `image_draw1` 的
   `jrect_t*`（L3）。四份示例的 `draw_sprite` 都改用它，缺图才退色块。
2. **`structs.js` 的 `TYPE_ALIASES { ft_t: 'float' }`**
   `jmath.h` 用 `#define ft_t float`，不归一化会把 `vec2_t` 等判成不可展开。
   归一化后可展开 L2 从 4 → 33，显式开放 26 条（跳过 2 条展开后 17 参数超
   `LUAX_MAX_ARGS=16`、1 条 L2 返回句柄）。
3. **三份脚本的线段坐标系**：`ShapeDrawLineF` → `ShapeDrawLine`（前者归一化）。
4. **仓库卫生**：`examples/csharp/obj` 取消跟踪（16 个带本机绝对路径的还原产物，
   工作区文件保留）；根 `CMakeLists.txt` 加 `enable_testing()`；新增
   `.github/workflows/build.yml`。
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
1. 改 `include/joy/api.h`（**声明放文件末尾**，避免 IDL `source` 行号整体漂移）
2. `node api/tools/gen_bindings.js --scan`
3. `node api/tools/gen_bindings.js --enable name:<api_name>`（必须带 `name:` 前缀）
4. `node api/tools/gen_bindings.js --check`
   （`--enable` 会直接 emit；`--check` 只有在 emit 之后才通过）

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
Get-ChildItem runtimes/android-arm64 -Recurse -File   # .so + 4 个 BCL
```

## 6. 明确未验证 / 已知风险

| 项 | 状态 |
|---|---|
| Android 交叉编译 | **从未跑过**。本机无 NDK，`android.yml` 刚随推送触发，结果未确认 |
| APK 打包 | 未做。没有 Gradle / `AndroidManifest.xml` / `SDLActivity` 工程 |
| Android 真机冒烟 | 未做。桌面 `--smoke` 走 dummy 驱动，不能替代 |
| iOS / wasm | `JOY_GAME_STATIC` 静态形态只在桌面验证过，两平台工具链都没跑 |
| C# 在 iOS / wasm | 更根本的障碍：iOS 禁 JIT（这套是 `mono_jit_init`）、wasm 需要专门的运行时 |
| `runtimes/windows-x64/coreclr.dll` | 是本地编译版，可考虑换成官方包同名 DLL |

## 7. 接下来的建议（按优先级）

1. **看 Android CI 首次结果**——整条链路唯一的空白。job 名
   `Android cross-build (C# host)` / `Android / arm64-v8a / Release`，产物
   `joy-android-arm64`。失败是正常预期，优先看这几个点：
   - SDL3 / msgpack / spine 三个 vendored 子项目在 Android toolchain 下能否 configure
   - `example_csharp` 链接预编译 `libmonosgen-2.0.so` 是否通过（NDK 允许链 `.so`）
   - `add_library(SHARED)` + `OUTPUT_NAME main` 是否产出 `libmain.so`
   - `joy_examples_deploy_clr()` 的 Android 分支是否把 `.so` 放进 `build-android/lib/<abi>/`
2. **APK 打包工程**：Gradle + `AndroidManifest.xml` + `SDLActivity`，把
   `libmain.so`、`lib/arm64-v8a/*.so`、`assets/**` 装进 APK。这是让产物变成可安装包的前置。
   会引入 Gradle 目录结构，动手前先确认放不放仓库。
3. **真机 / 模拟器冒烟**：CI 上可用 `android-emulator-runner`（慢、不稳），或手动跑。
4. **旧清单（不阻塞 Android）**：
   - L1 里 130 条 `status:off` 需要逐条裁决 `owns`（`api/generated/coverage.md` 有 38 条待确认）
   - L3 的 229 条中，出参 99 条最值得做 —— 给 `joy_value` 加出参通道，
     能顺带解决 `jtext_measurex`（现在文字宽度还是按字号估算）
   - `canThunk()` 目前没有「展开后参数 ≤ 16」的硬约束，批量开 L2 前建议补上
   - C++ 的 iOS / wasm 分支需要对应工具链才能验证

## 8. 提交风格

中文、`type(scope): 摘要`，正文用 `- ` 列改动点、最后一行写验证结果。
参考 `git log -3`。
