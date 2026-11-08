cmake_minimum_required(VERSION 3.16)

# ============================================================
# joy 构建入口
#
# 两种用法：
#   1) 作为子模块（推荐）：调用方 add_subdirectory(<joy 目录>)
#      然后 target_link_libraries(<宿主> PRIVATE joy::all)。
#      第三方 msgpack / spine / SDL 由调用方自行 add_subdirectory，
#      本文件不重复添加，避免同名 target 冲突。
#   2) 单独配置本仓库：cmake -S . -B build，此时由本文件添加第三方。
#
# 构建契约定义在 cmake/JoyTargets.cmake（joy::all，INTERFACE 源码注入），
# 源码清单在 cmake/JoySources.cmake；joy_add.cmake 只是旧接口的兼容 shim。
# ============================================================

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(joy C CXX)

    # 顶层启用 CTest：examples/ 子目录里的 enable_testing() 只在该目录生效，
    # 不会生成根 CTestTestfile.cmake，导致 `ctest --test-dir build` 报
    # "No tests were found"。在根调用后，子目录的 add_test 会被聚合上来。
    enable_testing()

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

    if((APPLE AND NOT CMAKE_SYSTEM_NAME MATCHES "Darwin") OR EMSCRIPTEN)
        set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
        set(SDL_SHARED OFF)
    else()
        set(SDL_SHARED ON)
    endif()

    if(MSVC)
        if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
            add_definitions(/MP)
        endif()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()

    add_subdirectory(msgpack)
    add_subdirectory(spine)
    add_subdirectory(SDL)
endif()

# ============================================================
# 总开关：是否构建 examples（cpp / lua / js / csharp 四语言示例）。
# 默认 OFF —— 把 joy 作为子项目时只拿到引擎本身，不引入任何示例。
# 打开：-DJOY_BUILD_EXAMPLES=ON，或在 cmake-gui 里勾选。
#
# 必须放在 include(JoyTargets.cmake) 之前：JoyTargets 会 foreach(lua js cs)
# 引入各模块，模块在这一步读取全局属性 JOY_REQUESTED_MODULES 决定启用哪些
# 脚本后端；晚于它设置就只能拿到空桩。
# ============================================================
option(JOY_BUILD_EXAMPLES "构建 examples 四语言示例（cpp / lua / js / csharp）" OFF)

if(JOY_BUILD_EXAMPLES)
    # CLR 探测与 examples 独立构建时的逻辑一致：默认取 joy 自带的 runtimes/。
    set(_clr_root "${CMAKE_CURRENT_LIST_DIR}/runtimes")
    if(WIN32)
        set(_clr_plat "${_clr_root}/windows-x64")
    elseif(APPLE)
        set(_clr_plat "${_clr_root}/macos-arm64")
    else()
        set(_clr_plat "${_clr_root}/linux-x64")
    endif()
    if(NOT MONO_INCLUDE_DIR OR NOT MONO_LIBRARIES)
        if(EXISTS "${_clr_plat}/include/mono-2.0")
            set(MONO_INCLUDE_DIR "${_clr_plat}/include/mono-2.0")
        elseif(EXISTS "${_clr_root}/include/mono-2.0")
            set(MONO_INCLUDE_DIR "${_clr_root}/include/mono-2.0")
        endif()
        if(WIN32 AND EXISTS "${_clr_plat}/coreclr.import.lib")
            set(MONO_LIBRARIES "${_clr_plat}/coreclr.import.lib")
        elseif(EXISTS "${_clr_plat}/libcoreclr.so")
            set(MONO_LIBRARIES "${_clr_plat}/libcoreclr.so")
        endif()
    endif()

    if(MONO_INCLUDE_DIR AND MONO_LIBRARIES AND EXISTS "${MONO_INCLUDE_DIR}")
        set(_joy_modules cs lua js)
        message(STATUS "joy: 开启 cs / lua / js 三个后端（examples）")
    else()
        set(_joy_modules lua js)
        message(WARNING
            "joy: 未找到 CLR 开发包（MONO_INCLUDE_DIR / MONO_LIBRARIES），"
            "csharp 示例将跳过；请设 -DMONO_INCLUDE_DIR=<.../include/mono-2.0> "
            "-DMONO_LIBRARIES=<.../coreclr.import.lib>")
    endif()
    set_property(GLOBAL PROPERTY JOY_REQUESTED_MODULES ${_joy_modules})
    # examples/CMakeLists.txt 的 joy_examples_deploy_clr() 依赖这个标记
    set(JOY_EXAMPLES_CLR_OK ${_joy_modules} MATCHES "cs")
endif()

# 作为子目录时不调用 project()、不改全局输出目录、不重复添加第三方。
include("${CMAKE_CURRENT_LIST_DIR}/cmake/JoyTargets.cmake")

# examples 由根目录统一引入。JOY_EXAMPLES_ACTIVE 由 examples/CMakeLists.txt
# 设置，用于避免 cmake -S examples 独立构建时又回到这里重复引入。
if(JOY_BUILD_EXAMPLES AND NOT JOY_EXAMPLES_ACTIVE)
    add_subdirectory(examples)
endif()
