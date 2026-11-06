# runtimes/ — 预编译运行时（不参与编译）

约定一句话：**根目录放跨平台共享物，`<platform>/` 只放该平台独有的原生二进制。**
刻意不套 `common/` 一层，避免不必要的目录深度。

| 路径 | 内容 | 平台相关性 |
|---|---|---|
| `libs/` | 4 个 .NET BCL 程序集：`System.Private.CoreLib.dll`、`netstandard.dll`、`System.Runtime.dll`、`System.Collections.dll` | 跨平台共享，全平台唯一一份 |
| `include/mono-2.0/` | Mono 嵌入 API 头文件（88 个 `.h`，保持 SDK 原始目录布局，`<mono/metadata/...>` include 依赖这个结构） | 跨平台共享 |
| `windows-x64/coreclr.dll`、`coreclr.import.lib` | Windows x64 的 CoreCLR 宿主与导入库 | 平台相关 |
| `android-arm64/libcoreclr.so` 等 | 后续 Android 原生运行时 | 平台相关 |

## 平台目录命名

`windows-x64` / `android-arm64` / `android-armv7` / `android-x64` / `linux-x64` …
由调用方 CMake 依据 `CMAKE_SYSTEM_NAME` 与 `ANDROID_ABI` 推导 `JOY_CLR_PLATFORM`，
也可用 `-DJOY_CLR_PLATFORM=<目录名>` 覆盖。新增平台只需放一个同名目录并补一行
ABI 映射，不需要改动目录结构。

## 共享物覆盖

默认所有平台共用根目录的 `libs/` 与 `include/`。若某平台的
`System.Private.CoreLib.dll` 做过 R2R/AOT 预编译（含平台原生代码），或两个 SDK 的
头文件版本不一致，可在 `<platform>/libs/`、`<platform>/include/` 各放一份覆盖，
CMake 会优先取平台目录。正常情况平台目录里只有 `.dll` / `.so` / `.lib`。

升级 BCL 时只改根目录 `libs/`，全平台同时生效。

## 来源

`coreclr.dll` 是 `dotnet/runtime` 的自定义构建（CoreCLR + 静态链接 Mono 运行时组件），
同时导出 `coreclr_*` 与 `mono_*` 两套宿主 API，因此 `modules/csharp` 的 `csx_*`
嵌入代码无需改写。版本 .NET 8.0.17；**这不是微软官方分发包**，没有官方补丁渠道，
升级需要从 `dotnet/runtime` 自行重建这套二进制。

`libs/` 只保留 4 个文件约 4.5 MB，是 netstandard2.0 编译目标下的下限：CoreLib 独占
4.48 MB（基础类型、集合、委托、字符串），其余 3 个是 facade，逐个移出都会导致运行时
加载失败。再减体积只能靠 IL trimming 或重建 CoreLib。

## 部署

- **Windows**：CMake post-build 把 `coreclr.dll` 拷到 exe 旁、`libs/` 拷成 `lib/`，
  宿主用 `mono_set_dirs(lib, lib)` 让依赖程序集从那里解析。
- **Android（未实现，仅预留接口）**：原生库需进 APK 的 `lib/<abi>/`；`mono_set_dirs`
  需要真实文件系统路径，BCL 通常得放 assets 再解压到内部存储。落地时要在宿主的
  `setup_clr_dirs()` 里按 `#ifdef __ANDROID__` 分支处理，本次重构不提前实现。

预编译二进制不要放进 `external/`（那里只放参与编译的第三方源码）。
