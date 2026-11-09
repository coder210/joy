# runtimes/ — 预编译运行时（不参与编译）

约定一句话：**根目录只放跨平台共享物（头文件），每个 `<platform>/` 自带该平台的
原生运行时与 BCL。** 刻意不套 `common/` 一层，避免不必要的目录深度。
各平台运行时二进制都随仓库入库，clone 后即可 configure。

| 路径 | 内容 | 平台相关性 |
|---|---|---|
| `include/mono-2.0/` | Mono 嵌入 API 头文件（88 个 `.h`，保持 SDK 原始目录布局，`<mono/metadata/...>` include 依赖这个结构） | 跨平台共享 |
| `windows-x64/coreclr.dll`、`coreclr.import.lib` | Windows x64 的 MonoVM 运行时（官方包内即叫 `coreclr.dll`）与导入库 | 平台相关 |
| `windows-x64/libs/` | Windows 版 4 个 BCL | 平台相关 |
| `android-arm64/libmonosgen-2.0.so` | Android arm64 的 MonoVM 运行时 | 平台相关 |
| `android-arm64/libs/` | Android 版 4 个 BCL | 平台相关 |
| `linux-x64/libcoreclr.so`、`libSystem.*.so` | Linux x64 的 MonoVM 运行时与 5 个原生 shim | 平台相关 |
| `linux-x64/libs/` | Linux 版 4 个 BCL | 平台相关 |
| `macos-arm64/libcoreclr.dylib`、`libSystem.*.dylib` | macOS arm64 的 MonoVM 运行时与 6 个原生 shim | 平台相关 |
| `macos-arm64/libs/` | macOS 版 4 个 BCL | 平台相关 |

## 平台目录命名

`windows-x64` / `android-arm64` / `android-armv7` / `android-x64` / `linux-x64` /
`macos-arm64` …
由调用方 CMake 依据 `CMAKE_SYSTEM_NAME` 与 `ANDROID_ABI` 推导 `JOY_CLR_PLATFORM`，
也可用 `-DJOY_CLR_PLATFORM=<目录名>` 覆盖。新增平台只需放一个同名目录并补一行
ABI 映射，不需要改动目录结构。当前 `examples/CMakeLists.txt` 把 Apple 一律推成
`macos-arm64`、其余 Unix 一律推成 `linux-x64`；Intel Mac / Linux arm64 需要先补
对应 RID 的 runtime pack，并扩展这段推导（或显式传 `-DJOY_CLR_PLATFORM=`）。

## BCL 按平台存放

BCL 不做跨平台共享：`System.Private.CoreLib.dll` 按 RID 构建，Windows 版含
`kernel32` 等 Win32 P/Invoke、Android 版含 `liblog`，两者字节差异 91%、体积差
115 KB（4,476,416 vs 4,361,480），混用会在原生化调用上直接失败。

因此每个 `<platform>/libs/` 自带该平台 runtime pack 的 4 个 BCL，
`examples/csharp_host/CMakeLists.txt` 只取 `<MONO_LIBRARIES 所在目录>/libs/`，
缺失时在 configure 阶段报错，不回退到别的平台。

`include/` 仍是共享的：Mono 嵌入 API 头文件各平台一致。

升级 BCL 时所有 `<platform>/libs/` 与对应运行时必须一起换成同一版本号，
不能只改其中一个平台，否则会加载到混版本的程序集。

## 来源

运行时来自微软官方 NuGet 的 MonoVM runtime pack
`Microsoft.NETCore.App.Runtime.Mono.<rid>`，版本 .NET 8.0.17。`win-x64` 包里的主
DLL 名为 `coreclr.dll`（apphost 按这个名字查找），`android-arm64` 包里为
`runtimes/android-arm64/native/libmonosgen-2.0.so`，`linux-x64` / `osx-arm64`
包里为 `runtimes/<rid>/native/libcoreclr.so` / `.dylib`。四者都同时导出 6 个
`coreclr_*` 与 1300+ 个 `mono_*` 宿主 API，动态符号数（1328）几乎一致，
因此 `modules/cs` 的 `csx_*` 嵌入代码无需改写，各平台也不需要自行交叉编译。
升级 = 换同版本号的 NuGet 包，`libs/` 的 BCL 同步更新。

Linux / macOS 包额外把 `libSystem.*.so` / `.dylib` 拆成独立文件（CoreLib 的
P/Invoke 目标）。ELF 实测 `libcoreclr.so` 的 `DT_SONAME` 为 `libcoreclr.so`、
**没有 DT_RUNPATH**，所以 exe 必须用 `BUILD_RPATH` 找主库，而 shim 由宿主在
`csx_create()` 之前用 `SDL_LoadObject` 按绝对路径预加载，后续 CoreLib 的同名
加载直接复用。`libhostfxr` / `libhostpolicy` 与 `libmono-component-*.a` 是
apphost / 静态链接用的，嵌入 CLR 用不到，未入库。

### 入库校验和（8.0.17 官方包）

| 文件 | SHA256 |
|---|---|
| `linux-x64/libcoreclr.so` | `015BB6E815A0061AA0B692E5CE88620C0BB17481AD71AFB450BDF662130BEA2B` |
| `macos-arm64/libcoreclr.dylib` | `3C5237E8728263309CFCA08E3CFFF7775576D434555E3CE94426245C0CA8E12C` |

CI 的 macOS job 必须固定 `-DCMAKE_OSX_ARCHITECTURES=arm64`：本目录只有 arm64
运行时，双架构构建会在链接 x86_64 切片时失败。Intel Mac / Linux arm64 需另补
对应 RID 的 runtime pack 并扩展 `JOY_CLR_PLATFORM` 推导。

注：当前入库的 `windows-x64/coreclr.dll` 内嵌
`C:\Users\liwei\Downloads\runtime-8.0.17\src\mono\...` 调试路径，是从
`dotnet/runtime` 8.0.17 源码在本机构建的那一份；官方包的二进制只有相对路径
`src/mono/mono/mini` 与版本串 `8.0.1725.26602 @Commit: 77545d6fd5ca`。可直接用
官方 NuGet 包里的同名 DLL 替换。`coreclr.import.lib` 是从该 DLL 生成的导入库，
NuGet 包不附带。

每个 `<platform>/libs/` 只保留 4 个文件约 4.5 MB，是 netstandard2.0 编译目标下的
下限：CoreLib 独占 4.48 MB（基础类型、集合、委托、字符串），其余 3 个是 facade，
逐个移出都会导致运行时加载失败。再减体积只能靠 IL trimming 或重建 CoreLib。

## Android 运行时（已入库）

`android-arm64/` 已随仓库入库，clone 后不需要联网或额外准备就能 configure。
文件来自官方 `Microsoft.NETCore.App.Runtime.Mono.android-arm64` **8.0.17**：

| 仓库路径 | 包内来源 |
|---|---|
| `android-arm64/libmonosgen-2.0.so` | `runtimes/android-arm64/native/libmonosgen-2.0.so` |
| `android-arm64/libs/System.Private.CoreLib.dll` | `runtimes/android-arm64/native/System.Private.CoreLib.dll` |
| `android-arm64/libs/{netstandard,System.Runtime,System.Collections}.dll` | `runtimes/android-arm64/lib/net8.0/` |

`libmonosgen-2.0.so` 同时导出 6 个 `coreclr_*` 与 1312 个 `mono_*`，
`modules/cs` 的嵌入代码无需改写。BCL 与 `windows-x64/libs/` 是两套独立构建，
各自跟随本平台的 runtime pack，不能互借。

升级时把两个平台的运行时与 BCL 一起换成同一版本号；其它 ABI 同理，目录名与
`ANDROID_ABI` 的映射见 `examples/CMakeLists.txt`。

CI（`.github/workflows/android.yml`）只装 NDK、校验这些已入库文件、交叉编译出
`libmain.so` 并上传 Artifacts，本机不装 NDK 也能拿到 Android 产物。
打 APK 与真机冒烟仍需单独的 Android 工程，见下方部署说明。

## 部署

- **Windows**：post-build 把 `coreclr.dll` 拷到 exe 旁、`windows-x64/libs/` 拷成
  `assets/lib/`，宿主用 `MONO_LIB_DIR` 指向它，让 `mono_set_dirs()` 解析依赖程序集。
- **Linux / macOS**：`joy_examples_deploy_clr()` 的 `UNIX` 分支把主库与全部
  `libSystem.*` 一次性拷到 exe 同目录，并给 target 追加 `BUILD_RPATH` /
  `INSTALL_RPATH`（Linux `$ORIGIN`、macOS `@loader_path`）；宿主在
  `setup_clr_dirs()` 的 Unix 分支预加载这些 shim，`MONO_LIB_DIR` 仍指向
  `assets/lib/`。若 CoreLib 报 `libSystem.Native` 找不到，先查主库与 shim 是否
  同目录、rpath 是否写进 exe、预加载清单是否覆盖新版本 runtime pack。
- **Android**：`libmonosgen-2.0.so` 由 `joy_examples_deploy_clr()` 放进
  `lib/<abi>/`（系统解压后可被加载器找到）；BCL 仍随 `assets/lib/`，但
  `mono_set_dirs()` 要真实路径，宿主在 `setup_clr_dirs()` 的 `__ANDROID__` 分支里
  用 `SDL_GetPrefPath()`（内部存储）落盘后再指过去。宿主 target 也改成
  `libmain.so`。APK 打包与真机冒烟仍需 Android NDK / SDK 工程，本仓库目前只
  提供到构建配置与产物布局。

预编译二进制不要放进 `external/`（那里只放参与编译的第三方源码）。
