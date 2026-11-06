# joy
This is an easy game framework.

## 目录

- `joy.h` / `j*.c` / `external/` / `test/` —— 引擎核心。不依赖任何脚本运行时。
- `modules/` —— 可选的脚本后端：`lua`、`js`、`csharp`。
- `SDL/` `msgpack/` `spine/` —— 第三方源码。

引用时统一写 `#include <joy/xxx.h>`（include 根为本仓库的父目录，
即下游的 `third_party/`）。

## 脚本模块

joy 本身不带脚本运行时，语言后端按需启用：

```cmake
include(${CMAKE_SOURCE_DIR}/third_party/joy/joy_add.cmake)
joy_add_sources(${TARGET_NAME} MODULES lua js)   # 需要 C# 再加 csharp
```

- 三个模块遵循同一契约：各自的 `CMakeLists.txt` 提供 `joy_module_<name>`（INTERFACE），
  源码经 `target_sources(INTERFACE)` 直接编入调用方 target，不建库、不产 DLL/LIB。
  模块内只放 joy 自己的胶水与契约；第三方 VM 源码在 `external/`，预编译运行时在
  `runtimes/`（详见两个目录的 README）。
- 未启用的模块仍提供 `INTERFACE` 目标，只暴露头文件路径；实现由模块自带的
  `stub.c` 编译出的空函数承担，`luax_*` / `jsx_*` / `csx_*`
  调用点始终可编译、可链接，创建返回 `NULL`，调用方据此跳过脚本逻辑。
- 也可以不用 `MODULES`，直接 `-DJOY_MODULE_LUA=ON -DJOY_MODULE_JS=ON`。
- `csharp` 模块额外需要 `MONO_INCLUDE_DIR`、`MONO_LIBRARIES`；
  显式启用却未提供时，配置阶段直接 `FATAL_ERROR`，不会静默退化为桩。
