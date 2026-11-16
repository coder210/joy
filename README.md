# joy
This is an easy game framework.

## 目录

- `src/j<域>.c` + `src/j<域>.h` —— 引擎核心，9 个手写域各一对文件
  （`core` / `collision` / `r2d` / `r3d` / `scene` / `net` / `sys` / `codec` / `audio`），
  另有数据分片 `src/jlut.{c,h}`。`math` 是生成物域、没有手写 `.c`：`jmath.h`
  的标量/向量数学由 `jmath_wrap.js` 生成到 `api/generated/jmath_wrappers.{h,c}`。
  不依赖任何脚本运行时。
  域边界按渲染上下文与抽象层次划分：依赖 `joy_r2d_p` 的 2D 控件归 `r2d`，
  依赖 `joy_r3d_p` 的 3D HUD 控件归 `r3d`，两边都不单设 UI 文件；
  网络协议与服务器归 `net`，跨平台 OS 原语（socket / 时间 / 崩溃 / 网卡枚举）
  归 `sys`，碰撞求交归 `collision`。
- `include/joy/` —— 公开头：`joy.h` 聚合门面、`japi.h` 聚合入口（只 include
  9 个域头，自身不含声明）、`jdefs.h` 公开类型、`jtypes.h` 基础类型、
  `jmath.h` 标量/向量数学、`jlut.h` 查表 extern。
- `external/` —— vendored 第三方源码（含一个自有包装 `aes_util.c`）。
- `examples/` —— cpp / lua / js / csharp 四份同构示例，同时是下游项目的模板；
  `ctest` 的四语言冒烟用例在这里注册。
- `android/` —— Android 打包脚本与应用标签。
- `runtimes/` —— 预编译脚本运行时（CLR），按 RID 分目录。
- `api/` —— 与脚本语言无关的绑定 API 契约：IDL、覆盖率报告、共享 C 运行层与各后端薄适配生成物，见 `api/README.md`。
- `modules/` —— 可选的脚本后端：`lua`、`js`、`cs`。
- `SDL/` `msgpack/` `spine/` —— 第三方源码。

引用时统一写 `#include <joy/xxx.h>`（include 根为本仓库的父目录，
即下游的 `third_party/`）。

## 文档

- `README.md` —— 本页：目录与构建用法。
- `NEXT_SESSION.md` —— 交接：当前状态、待办与验证命令。
- `HANDOFF.md` —— 历史决策、关键技术事实与踩坑清单。
- `api/README.md` —— 绑定契约、域表、IDL 与生成链规则。
- `TYPE_RENAME_TASK.md` —— 类型名统一任务书（已完成，留存设计依据）。

## 脚本模块

joy 本身不带脚本运行时，语言后端按需启用。`-DJOY_BUILD_EXAMPLES=ON` 时
会按示例实际需要自动勾选 `JOY_MODULE_LUA` / `JOY_MODULE_JS` / `JOY_MODULE_CS`
（探测到 CLR 才含 cs），不需要再手动勾：

```cmake
include(${CMAKE_SOURCE_DIR}/third_party/joy/joy_add.cmake)
joy_add_sources(${TARGET_NAME} MODULES lua js)   # 需要 C# 再加 cs
```

- 三个模块遵循同一契约：各自的 `CMakeLists.txt` 提供 `joy_module_<name>`（INTERFACE），
  源码经 `target_sources(INTERFACE)` 直接编入调用方 target，不建库、不产 DLL/LIB。
  三个模块统一分两层：`bindings/` 放「把 joy C API 暴露给该语言」的 C 侧胶水，
  `scripts/` 放语言侧源码（cs 的 `*.cs`；lua/js 以后按需建立同名目录），
  根目录只放运行时宿主、模块契约与构建入口；模块内只放 joy 自己的代码。
  第三方 VM 源码在 `external/`，预编译运行时在 `runtimes/`（详见两个目录的 README）。
- 未启用的模块仍提供 `INTERFACE` 目标，只暴露头文件路径；实现由模块自带的
  `stub.c` 编译出的空函数承担，`luax_*` / `jsx_*` / `csx_*`
  调用点始终可编译、可链接，创建返回 `NULL`，调用方据此跳过脚本逻辑。
- 也可以不用 `MODULES`，直接 `-DJOY_MODULE_LUA=ON -DJOY_MODULE_JS=ON`。
- `cs` 模块额外需要 `MONO_INCLUDE_DIR`、`MONO_LIBRARIES`；
  显式启用却未提供时，配置阶段直接 `FATAL_ERROR`，不会静默退化为桩。
