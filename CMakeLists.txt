cmake_minimum_required(VERSION 3.16)

# 全局输出目录设置（影响所有目标）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# 防止安装到系统目录
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

# 项目声明
project(joy2d C CXX)


option(BUILD_JOY2D_EDITOR "Build joy2d_editor" OFF)
option(BUILD_ADVENTURE_SERVER "Build adventure_server" OFF)
option(BUILD_ADVENTURE_CLIENT "Build adventure_client" OFF)


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
endif()

# 添加子目录
add_subdirectory(SDL)          # SDL3 库
add_subdirectory(joy2d)   # joy2d 核心库
add_subdirectory(joy2d_lua)   # joy2d_lua lua绑定库
add_subdirectory(joy2d_js)   # joy2d_js js绑定库

if (WIN32)
	add_subdirectory(joy2d_csharp)   # joy2d_csharp chsarp绑定库
	add_subdirectory(joy2d_cslib)
elseif (ANDROID)
	 message(STATUS "Root MOBILE_MAIN_DIR = '${MOBILE_MAIN_DIR}'")
endif()

 message(STATUS "Root MOBILE_MAIN_DIR = '${MOBILE_MAIN_DIR}'")

if(BUILD_JOY2D_EDITOR)
    add_subdirectory(joy2d_editor)
endif()

# examples
if(BUILD_ADVENTURE_SERVER)
    add_subdirectory(adventure_server)
endif()

if(BUILD_ADVENTURE_CLIENT)
    add_subdirectory(adventure_client)
endif()

# 可选：CPack 打包配置（如果原来有）
# include(CPack)