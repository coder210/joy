# ============================================================
# joy 四语言示例的共享构建入口
#
# 四个 target：example_cpp / example_csharp / example_lua / example_js
# 功能完全相同（见 README.md），只是业务逻辑分别用 C++ / C# / Lua / JS 写。
#
# 两种用法：
#   1) 从 joy 仓根构建：根 CMakeLists 里 option(JOY_BUILD_EXAMPLES) 打开后
#      add_subdirectory(examples)，此时 joy::all 已存在。
#   2) 独立构建：cmake -S examples -B build/examples，本文件会自行把 joy 与
#      三个第三方拉进来。
# ============================================================
cmake_minimum_required(VERSION 3.16)
project(joy_examples C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

set(JOY_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." CACHE PATH "joy 仓根目录")

if(NOT TARGET joy::all)
    add_subdirectory("${JOY_ROOT}/msgpack" "${CMAKE_BINARY_DIR}/msgpack")
    add_subdirectory("${JOY_ROOT}/spine"   "${CMAKE_BINARY_DIR}/spine")
    add_subdirectory("${JOY_ROOT}/SDL"     "${CMAKE_BINARY_DIR}/SDL")

    # ---- C# 示例需要的 CLR 开发包：默认取 joy 仓自带的 runtimes/ ----
    # 缺失时只关掉 cs 模块（csharp 示例不生成），不让 lua / js / cpp 一起失败。
    if(NOT MONO_INCLUDE_DIR OR NOT MONO_LIBRARIES)
        set(_clr_root "${JOY_ROOT}/runtimes")
        if(WIN32)
            set(_clr_plat "${_clr_root}/windows-x64")
        elseif(APPLE)
            set(_clr_plat "${_clr_root}/macos-arm64")
        else()
            set(_clr_plat "${_clr_root}/linux-x64")
        endif()
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
        message(STATUS "joy examples: 开启 cs / lua / js 三个后端")
    else()
        set(_joy_modules lua js)
        message(WARNING
            "joy examples: 未找到 CLR 开发包（MONO_INCLUDE_DIR / MONO_LIBRARIES），"
            "本次只构建 cpp / lua / js 三个示例；要用 csharp 示例请设 "
            "-DMONO_INCLUDE_DIR=<.../include/mono-2.0> -DMONO_LIBRARIES=<.../coreclr.import.lib>")
    endif()
    set_property(GLOBAL PROPERTY JOY_REQUESTED_MODULES ${_joy_modules})
    set(JOY_EXAMPLES_CLR_OK ${_joy_modules} MATCHES "cs")
    add_subdirectory("${JOY_ROOT}" "${CMAKE_BINARY_DIR}/joy")
endif()

# 四份实现共用的资源（图片 / 字体）。缺失时各示例走纯色 + 无文字退化路径。
set(JOY_EXAMPLE_ASSETS "${CMAKE_CURRENT_LIST_DIR}/assets")

# 每个子目录都会定义自己的 target 与一次资源拷贝
enable_testing()
foreach(_lang cpp lua js csharp)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${_lang}/CMakeLists.txt")
        add_subdirectory(${_lang})
    endif()
endforeach()
