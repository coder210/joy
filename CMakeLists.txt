cmake_minimum_required(VERSION 3.16)

# 全局输出目录设置（影响所有目标）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# 防止安装到系统目录
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

# 项目声明
project(joy2d C CXX)

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
add_subdirectory(joy2d_core)   # joy2d 核心库
add_subdirectory(joy2d_loader) # 可执行程序

# 可选：CPack 打包配置（如果原来有）
# include(CPack)