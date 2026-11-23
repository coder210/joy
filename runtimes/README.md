# runtimes/ — 预编译运行时（不参与编译）

本目录只有两块内容，**运行时二进制一律以 `nupkg/` 为唯一来源**：

| 路径 | 内容 | 谁在用 |
|---|---|---|
| `include/mono-2.0/` | Mono 嵌入 API 头文件（88 个 `.h`，保持 SDK 原始目录布局，`<mono/metadata/...>` include 依赖这个结构） | 编译 cs 模块（跨平台共享；runtime pack 里不含头文件）|
| `nupkg/` | 官方 runtime pack：桌面 CoreCLR 3 个 RID（8.0.11）+ Android MonoVM 1 个（8.0.17） | 桌面 publish 与 Android 解包 |

> **仓库里不再有任何散装托管 DLL。** 两个平台都只认 `nupkg/`：
> - 桌面（CoreCLR）：`dotnet publish --self-contained` 直接消费，**部署完整运行时**；
> - Android（MonoVM）：`cmake/JoyMonoVm.cmake` 在 configure 阶段解包，
>   按白名单 `cmake/JoyBclWhitelist.cmake` 暂存（APK 体积敏感，且有真机验证）。
>
> **桌面刻意不裁剪 BCL**：曾按 30 个白名单裁到 48.6MB，但 CoreLib 的 AssemblyRef
> 闭包有 115 个，真正的缺口只在特定平台的特定路径上暴露——Linux 的 Console 要
> `Microsoft.Win32.Primitives`，Windows 走不到那段代码，本机永远测不出来，
> 只能靠 CI 一轮轮试。22MB 换不来这份维护负担。
> 桌面平台目录 `windows-x64/`、`linux-x64/`、`macos-arm64/` 已删除（-33MB）。

## Android 的运行时

平台名按 ABI 映射：`android-arm64` / `android-armv7` / `android-x64`，
可用 `-DJOY_CLR_PLATFORM=<目录名>` 覆盖；`-DJOY_MONOVM_ANDROID_VERSION=<版本>`
指定 runtime pack 版本（默认 8.0.17，必须与 `nupkg/` 里的文件名一致）。

`joy_monovm_stage()`（`cmake/JoyMonoVm.cmake`）在 configure 阶段把
`nupkg/microsoft.netcore.app.runtime.mono.android-arm64.<ver>.nupkg` 解到
`<build>/_monovm/<ver>/`，按白名单暂存成：

| 暂存产物 | 包内来源 |
|---|---|
| `<stage>/libs/*.dll`（白名单 31 个） | `native/System.Private.CoreLib.dll` + `lib/net8.0/<其余 30 个>` |
| `<stage>/libmonosgen-2.0.so` | `native/libmonosgen-2.0.so` |
| `<stage>/libSystem.*.so`（4 个） | `native/libSystem.*.so` |

暂存布局刻意与历史上手工入库的 `runtimes/<platform>/` 完全一致，所以下游
（`assets/lib` 拷贝、APK `lib/<abi>` 打包）一行都没改。已验证解包结果与原先
散装文件**逐字节一致**（35 个文件，0 差异）。

BCL 不能跨平台共用：`System.Private.CoreLib.dll` 按 RID 构建，Android 版含
`liblog` 等 P/Invoke 目标，与桌面 CoreCLR 的版本完全不兼容（4.2MB vs 12.6MB）。

`include/` 是共享的，也是本目录唯一保留的散装文件——runtime pack 不带头文件。

## 来源

| 平台 | 包 | 版本 | 消费方式 |
|---|---|---|---|
| 桌面 | `Microsoft.NETCore.App.Runtime.<rid>` | 8.0.11 | `dotnet publish`（附加 NuGet 源）|
| Android | `Microsoft.NETCore.App.Runtime.Mono.android-arm64` | 8.0.17 | configure 时解包暂存 |

两个版本不同是正常的：MonoVM 与 CoreCLR 是两套运行时实现，各自的 CoreLib
不能互借。升级时只换对应的 nupkg；Android 还要同步改
`-DJOY_MONOVM_ANDROID_VERSION`（默认 8.0.17），否则找不到包。

CI 的 macOS job 必须固定 `-DCMAKE_OSX_ARCHITECTURES=arm64`。

**为什么 Android 白名单是 31 个而不是 4 个**：`Joy.Bindings.dll` 会解析
`Marshal` / 反射 / LINQ / `StringBuilder` 等类型，实测只放 4 个核心程序集时
`BootstrapMono` 抛 `FileNotFoundException`（Android 上表现为 P9-FAIL）。
再减体积只能靠 IL trimming 或重建 CoreLib。

## Android 运行时细节

`libmonosgen-2.0.so` 同时导出 6 个 `coreclr_*` 与 1312 个 `mono_*`，
`modules/cs` 的嵌入代码无需改写。

**4 个 `libSystem.*.so` 不是可选项**：Android 版 `System.Private.CoreLib.dll`
把 `SystemNative_*` / 全球化 / zlib 等 P/Invoke 目标拆成独立共享库
（`libmonosgen-2.0.so` 的 `DT_NEEDED` 只有 `libm/libdl/liblog/libz/libc`，
不含这些实现）。MonoVM 启动与首次 JIT 时按 SONAME `dlopen`，缺任一个都会
闪退，而 cpp / lua / js 因为从不初始化 CLR 看不出问题。`joy_examples_deploy_clr()`
把主库和 4 个 shim 一起放进 `lib/<abi>/`，`android.yml` 只给 csharp flavor
打包并在 APK 清单里做防回归校验。

`native/` 里还有 `libmono-component-*.so`（debugger / diagnostics /
hot_reload / marshal-ilgen）：MonoVM 只在使用对应功能时 dlopen，正常游玩用不到，
因此**不进暂存集**——保持与历史入库集一致，避免改变已验证的行为。

其它 ABI 同理：把对应 RID 的 `Microsoft.NETCore.App.Runtime.Mono.<abi>` 放进
`nupkg/` 并指定版本即可。注意暂存目录名目前固定为 `android-arm64`
（`JoyMonoVm.cmake` 的 `_stage`），支持多 ABI 时要同时改那里。

CI（`.github/workflows/android.yml`）只装 NDK、交叉编译出 `libmain.so` 并上传
Artifacts，本机不装 NDK 也能拿到 Android 产物。
打 APK 与真机冒烟仍需单独的 Android 工程，见下方部署说明。

## `runtimes/nupkg/` —— 两个平台共用的唯一运行时来源

预置官方 runtime pack，clone 后不需要联网：

| 文件 | 提供 | 消费方 |
|---|---|---|
| `microsoft.netcore.app.runtime.win-x64.8.0.11.nupkg` | Windows x64 CoreCLR + BCL | 桌面 publish |
| `microsoft.netcore.app.runtime.linux-x64.8.0.11.nupkg` | Linux x64 CoreCLR + BCL | 桌面 publish |
| `microsoft.netcore.app.runtime.osx-arm64.8.0.11.nupkg` | macOS arm64 CoreCLR + BCL | 桌面 publish |
| `microsoft.netcore.app.runtime.mono.android-arm64.8.0.17.nupkg` | Android MonoVM + BCL + shims | Android 解包暂存 |

桌面的 hostfxr / coreclr / BCL 由 `dotnet publish --self-contained` 消费上表；
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

**版本约束**：
- 桌面：nupkg 版本要与 SDK 解析出的 runtime pack 版本一致（当前 8.0.11，对应
  .NET 8.0.11 与 SDK 9.0.1xx 自带的 8.0.11 targeting pack）。升级时取
  `Microsoft.NETCore.App.Runtime.<rid>/<新版本>` 替换本目录 3 个 nupkg。
- Android：版本由 `JOY_MONOVM_ANDROID_VERSION`（默认 8.0.17）决定，必须与文件名
  一致；换版本就改这个变量。
- 两边都要同步更新本节表格。

之所以用"附加源"而不是 `--source` 完全替换，是为了在线/CI 行为不变：本地源缺包时
仍可从 nuget.org 补齐，不会因 SDK 版本差异把构建打挂。

## 部署

> 两个平台的运行时都从 `nupkg/` 取：桌面 `dotnet publish` 后**整体**铺到 exe 目录
> （hostfxr 命令行模式），Android 在 configure 阶段解包暂存后进 APK（MonoVM）。
> 仓库里已经没有任何平台目录，只剩 `include/`（头文件）与 `nupkg/`。

- **桌面（Windows / Linux / macOS）**：`joy_deploy_selfcontained()` 负责
  publish → 复制完整运行时 → 覆盖 `JoyGame.dll`/`pdb`；`JoyGame.runtimeconfig.json`
  （无 framework 节点）由各 CMakeLists 的 POST_BUILD 单独拷入。Linux/macOS 只额外
  设置 `BUILD_RPATH`（`$ORIGIN` / `@loader_path`）供 SDL3 等共享库解析。
  **不要改回白名单裁剪**：`desktop.yml` 有两条断言（`System.*.dll` ≥100、
  `Microsoft.Win32.Primitives.dll` 必须存在）专门防这个回归。
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
