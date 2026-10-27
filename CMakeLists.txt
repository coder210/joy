cmake_minimum_required(VERSION 3.16)

# 全局输出目录设置（影响所有目标）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# 防止安装到系统目录
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

# 项目声明：本仓库只包含 joy 引擎本体 + 第三方依赖。
# 游戏项目均为独立仓库，通过 submodule 引入本仓库，再用 joy/joy_add.cmake 把源码编入自身目标。
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
# 使用方式（见 joy/joy_add.cmake）：
#   include(<joy 仓库>/joy/joy_add.cmake)
#   joy_add_sources(<你的可执行目标>)
