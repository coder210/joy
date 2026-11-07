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

# 作为子目录时不调用 project()、不改全局输出目录、不重复添加第三方。
include("${CMAKE_CURRENT_LIST_DIR}/cmake/JoyTargets.cmake")
