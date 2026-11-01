cmake_minimum_required(VERSION 3.16)

# 全局输出目录设置（影响所有目标）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# 防止安装到系统目录
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

# 项目声明：本仓库只包含 joy 引擎本体 + 第三方依赖。
# 游戏项目均为独立仓库，通过 submodule 引入本仓库，再用 joy_add.cmake 把源码编入自身目标。
project(joy C CXX)

# 平台相关共享库设置（可能影响所有子目录）
if ((APPLE AND NOT CMAKE_SYSTEM_NAME MATCHES "Darwin") OR EMSCRIPTEN)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
    set(SDL_SHARED OFF)
else()
    set(SDL_SHARED ON)
endif()

# MSVC 并行编译
if(MSVC)
    if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        add_definitions(/MP)
    endif()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# 第三方依赖
add_subdirectory(msgpack)
add_subdirectory(spine)
add_subdirectory(SDL)

# 说明：joy 不再作为独立库构建（无 DLL/SO、无导出符号）。
# 使用方式（见 joy_add.cmake）：
#   include(<joy 仓库>/joy_add.cmake)
#   joy_add_sources(<你的可执行目标>)
#
# 目录布局：引擎源码（joy.h / j*.c / external/ / test/）直接平铺在本仓库根，
# 与 SDL/ msgpack/ spine/ 三个第三方目录并列；引用时统一写 #include <joy/xxx.h>
# （include 根为 <joy 仓库>/.. 即 third_party/）。
#
# 脚本后端已拆为独立仓库，本仓库不含任何绑定层：
#   - Lua     → https://github.com/coder210/joy_lua
#   - JS      → https://github.com/coder210/joy_js
#   - C#(Mono)→ https://github.com/coder210/joy_csharp
