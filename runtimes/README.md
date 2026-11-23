# runtimes/ — 预编译运行时（不参与编译）

本目录有三块内容，用途完全不同：

| 路径 | 内容 | 谁在用 |
|---|---|---|
| `include/mono-2.0/` | Mono 嵌入 API 头文件（88 个 `.h`，保持 SDK 原始目录布局，`<mono/metadata/...>` include 依赖这个结构） | 编译 cs 模块（跨平台共享）|
| `android-arm64/` | Android 的 MonoVM 运行时 + 4 个 CoreLib shim + 30 个 BCL | **Android APK**（唯一还在用平台 BCL 的目标）|
| `nupkg/` | 桌面 CoreCLR 的 runtime pack（3 个 RID，8.0.11） | 桌面自包含构建的离线 NuGet 源 |

> **桌面（Windows / Linux / macOS）不再需要 `runtimes/<platform>/` 目录。**
> 迁移到 CoreCLR 自包含后，运行时与 BCL 由 `dotnet publish --self-contained`
> 从 `nupkg/` 解出并按白名单裁剪，宿主走 hostfxr。历史上的 `windows-x64/`、
> `linux-x64/`、`macos-arm64/`（MonoVM 8.0.17）已删除：它们既不再被链接
> （`modules/cs` 已完全不调用 `mono_*`），在 Linux/macOS 上还会把自包含的
> 真 CoreCLR 覆盖成 MonoVM 版本，直接 segfault。

## Android 平台目录

平台目录名按 ABI 映射：`android-arm64` / `android-armv7` / `android-x64`，
也可用 `-DJOY_CLR_PLATFORM=<目录名>` 覆盖。新增 ABI 需要先补对应 RID 的
`Microsoft.NETCore.App.Runtime.Mono.<abi>` 包与 `libSystem.*` shim。

`android-arm64/libs/` 放该平台 runtime pack 的 30 个 BCL（白名单，见
`cmake/JoySelfContained.cmake` 的 `JOY_SELFCONTAINED_BCL_KEEP`）。
BCL 不能跨平台共用：`System.Private.CoreLib.dll` 按 RID 构建，Android 版含
`liblog` 等 P/Invoke 目标，与桌面 CoreCLR 的版本完全不兼容（4.3MB vs 12.6MB）。

`include/` 是共享的：各平台 Mono 嵌入 API 头文件一致。

升级 BCL 时 `<platform>/libs/` 与同目录的运行时主库必须一起换版本，
否则会加载到混版本的程序集。

## 来源

**Android（MonoVM）**：`android-arm64/` 来自官方 NuGet
`Microsoft.NETCore.App.Runtime.Mono.android-arm64` 8.0.17，主库为
`runtimes/android-arm64/native/libmonosgen-2.0.so`（导出 6 个 `coreclr_*` 与
1300+ 个 `mono_*`），4 个 `libSystem.*.so` 是该 RID CoreLib 的 P/Invoke 目标。
升级 = 换同版本号的 NuGet 包，`libs/` 的 BCL 一并更新。

**桌面（CoreCLR）**：运行时与 BCL 不入库，由 `nupkg/` 里
`Microsoft.NETCore.App.Runtime.<rid>` 8.0.11 经 `dotnet publish --self-contained`
解出，详见下方「桌面离线构建」。hostfxr 用 `BUILD_RPATH`（`$ORIGIN` /
`@loader_path`）找主库；这部分由 `JoySelfContained.cmake` 与各 CMakeLists 负责，
仓库里不再保留桌面平台目录。

CI 的 macOS job 必须固定 `-DCMAKE_OSX_ARCHITECTURES=arm64`。

`android-arm64/libs/` 放 30 个文件约 6 MB。**曾经只放 4 个核心程序集**
（CoreLib + netstandard + System.Runtime + System.Collections），在只用基础类型的
极简程序里够用；但 `Joy.Bindings.dll` 会解析 `Marshal` / 反射 / LINQ /
`StringBuilder` 等类型，实测只放 4 个时 `BootstrapMono` 抛 `FileNotFoundException`
（Android 上表现为 P9-FAIL），因此改为按 29 个 BCL 白名单 + CoreLib 补齐。
再减体积只能靠 IL trimming 或重建 CoreLib。

## Android 运行时（已入库）

`android-arm64/` 已随仓库入库，clone 后不需要联网或额外准备就能 configure。
文件来自官方 `Microsoft.NETCore.App.Runtime.Mono.android-arm64` **8.0.17**：

| 仓库路径 | 包内来源 |
|---|---|
| `android-arm64/libmonosgen-2.0.so` | `runtimes/android-arm64/native/libmonosgen-2.0.so` |
| `android-arm64/libSystem.Native.so` | `runtimes/android-arm64/native/libSystem.Native.so` |
| `android-arm64/libSystem.Globalization.Native.so` | 同上 |
| `android-arm64/libSystem.IO.Compression.Native.so` | 同上 |
| `android-arm64/libSystem.Security.Cryptography.Native.Android.so` | 同上 |
| `android-arm64/libs/System.Private.CoreLib.dll` | `runtimes/android-arm64/native/System.Private.CoreLib.dll` |
| `android-arm64/libs/` 其余 29 个 BCL（白名单） | `runtimes/android-arm64/lib/net8.0/` |

`libmonosgen-2.0.so` 同时导出 6 个 `coreclr_*` 与 1312 个 `mono_*`，
`modules/cs` 的嵌入代码无需改写。这套 BCL 是 MonoVM 专用实现，与桌面 CoreCLR
的 BCL 完全不同，不能互借。

**Android 的 4 个 `libSystem.*.so` 不是可选项**：Android 版
`System.Private.CoreLib.dll` 把 `SystemNative_*` / 全球化 / zlib 等 P/Invoke
目标拆成独立共享库（`libmonosgen-2.0.so` 的 `DT_NEEDED` 只有 `libm/libdl/
liblog/libz/libc`，不含这些实现）。MonoVM 启动与首次 JIT 时会按 SONAME
`dlopen`，缺任一个都会直接闪退，而 cpp / lua / js 三个后端因为从不初始化
CLR 看不出问题。它们只依赖 Android 系统库，`joy_examples_deploy_clr()`
把主库和 4 个 shim 一起放进 `lib/<abi>/`，`android.yml` 只给 csharp flavor
打包并在 APK 清单里做防回归校验。

升级时把主库、4 个 shim 与 `libs/` 的 BCL 一起换成同一版本号；其它 ABI 同理，
目录名与 `ANDROID_ABI` 的映射见 `examples/CMakeLists.txt`。

CI（`.github/workflows/android.yml`）只装 NDK、校验这些已入库文件、交叉编译出
`libmain.so` 并上传 Artifacts，本机不装 NDK 也能拿到 Android 产物。
打 APK 与真机冒烟仍需单独的 Android 工程，见下方部署说明。

## 桌面离线构建：`runtimes/nupkg/`

桌面自包含的 hostfxr / coreclr / BCL 由 `dotnet publish --self-contained` 从 NuGet 的
runtime pack 解出。为了 clone 后不依赖网络，这里预置了 3 个平台的 runtime pack：

| 文件 | 提供 |
|---|---|
| `microsoft.netcore.app.runtime.win-x64.8.0.11.nupkg` | Windows x64 运行时 + BCL |
| `microsoft.netcore.app.runtime.linux-x64.8.0.11.nupkg` | Linux x64 运行时 + BCL |
| `microsoft.netcore.app.runtime.osx-arm64.8.0.11.nupkg` | macOS arm64 运行时 + BCL |

`cmake/JoySelfContained.cmake` 把它们作为**附加** NuGet 源
（`-p:RestoreAdditionalProjectSources`，不替换 nuget.org），并打开两个开关：

- `DisableTransitiveFrameworkReferenceDownloads=true`
  否则 self-contained 会连 AspNetCore / WindowsDesktop 的 runtime pack 一起下载
  （本项目根本不用），离线构建直接 NU1101 失败。
- `UseAppHost=false`
  游戏由 C++ 宿主经 hostfxr 加载 `JoyGame.dll`，不需要 .NET apphost；关掉可省掉
  `Microsoft.NETCore.App.Host.<rid>` 这一层依赖（仓库里也没有任何地方用到它）。

离线构建实际只需：目标 RID 的 runtime pack（上表 3 个之一）+ SDK 自带的
`Microsoft.NETCore.App.Ref`、`Host.<本机 RID>` pack。

**版本约束**：nupkg 版本要与 SDK 解析出的 runtime pack 版本一致（当前 8.0.11，
对应 .NET 8.0.11 与 SDK 9.0.1xx 自带的 8.0.11 targeting pack）。升级 .NET 时：
1. 取 `Microsoft.NETCore.App.Runtime.<rid>/<新版本>` 替换本目录 3 个 nupkg；
2. 同步更新本节表格与版本说明。

之所以用"附加源"而不是 `--source` 完全替换，是为了在线/CI 行为不变：本地源缺包时
仍可从 nuget.org 补齐，不会因 SDK 版本差异把构建打挂。

## 部署

> 桌面（Windows / Linux / macOS）走 CoreCLR 自包含：`dotnet publish --self-contained`
> 从 `nupkg/` 解出运行时并按白名单裁剪铺到 exe 目录，宿主用 hostfxr 命令行模式。
> 仓库里已经没有桌面平台目录，`<platform>/libs` 只剩 Android 一份。

- **桌面（Windows / Linux / macOS）**：`joy_deploy_selfcontained()` 负责
  publish → 按白名单裁剪 → 覆盖 `JoyGame.dll`/`pdb`；`JoyGame.runtimeconfig.json`
  （无 framework 节点）由各 CMakeLists 的 POST_BUILD 单独拷入。Linux/macOS 只额外
  设置 `BUILD_RPATH`（`$ORIGIN` / `@loader_path`）供 SDL3 等共享库解析。
  **不要再改回从 `runtimes/` 拷 `libcoreclr` / `libSystem.*`**：那会把自包含的
  真 CoreCLR 覆盖成 MonoVM 版本，hostfxr 加载后直接 segfault。
- **Android**：`libmonosgen-2.0.so` 与 4 个 `libSystem.*.so` 由
  `joy_examples_deploy_clr()` 放进 `lib/<abi>/`（系统解压后可被加载器找到）；
  BCL 仍随 `assets/lib/` 进 APK，但 `mono_set_dirs()` 要真实路径，宿主在
  `setup_clr_dirs()` 的 `__ANDROID__` 分支里用 `SDL_GetPrefPath()`（内部存储）
  落盘后，把 `MONO_PATH` / `MONO_LIB_DIR` 一起指过去；`modules/cs/jcsx.c`
  再调 `mono_set_assemblies_path()`（.NET 8 已废弃 `mono_set_dirs` 的程序集
  搜索语义）。解包失败会直接返回 `SDL_APP_FAILURE` 并打 logcat 日志，不再带着
  半个运行时进 `mono_jit_init()`。宿主 target 也改成 `libmain.so`。

预编译二进制不要放进 `external/`（那里只放参与编译的第三方源码）。
