cmake_minimum_required(VERSION 3.16)

# 全局输出目录设置（影响所有目标）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# 防止安装到系统目录
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

# 项目声明
project(joy C CXX)


# 仅保留三个游戏项目（其它目标按需注释停用，目录与 CMakeLists 保留）
# option(BUILD_JOY_EDITOR "Build joy_editor" OFF)
# option(BUILD_JOY_BEAT "Build joy_beat" OFF)
# option(BUILD_ADVENTURE2D "Build adventure2d (single binary: single/host/client/server)" OFF)
option(BUILD_PROJECT_PROXY2D "Build project_proxy2d (single binary: single/host/client/server)" OFF)
option(BUILD_PROJECT_PROXY3D "Build project_proxy3d (merged client/server single binary)" OFF)
option(BUILD_SPINE_SDL3_TEST "Build spine_sdl3_test (spine-c + SDL3 render test)" ON)
option(BUILD_JOY_SIDE_TEMPLATE "Build joy_side_template (copy of project_proxy2d, single binary: single/host/client/server)" ON)


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

# 添加子目录
add_subdirectory(msgpack)   # msgpack 库
add_subdirectory(spine)   # spine 库
add_subdirectory(SDL)          # SDL3 库
# joy 不再作为独立库子目录：源码由各游戏项目通过 joy/joy_add.cmake 直接编入

if (WIN32)
	# add_subdirectory(joy_cslib)   # C# 侧独立项目，暂不加入
elseif (ANDROID)
	 message(STATUS "Root MOBILE_MAIN_DIR = '${MOBILE_MAIN_DIR}'")
endif()

# if(BUILD_JOY_EDITOR)
#     add_subdirectory(joy_editor)
# endif()
#
# if(BUILD_JOY_BEAT)
#     add_subdirectory(joy_beat)
# endif()

# examples
# if(BUILD_ADVENTURE2D)
#     add_subdirectory(adventure2d)
# endif()

if(BUILD_PROJECT_PROXY2D)
    add_subdirectory(project_proxy2d)
endif()

if(BUILD_PROJECT_PROXY3D)
    add_subdirectory(project_proxy3d)
endif()

if(BUILD_SPINE_SDL3_TEST)
    add_subdirectory(spine_sdl3_test)
endif()

if(BUILD_JOY_SIDE_TEMPLATE)
    add_subdirectory(joy_side_template)
endif()

# 可选：CPack 打包配置（如果原来有）
# include(CPack)