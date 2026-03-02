cmake_minimum_required(VERSION 3.10)
project(Calculator VERSION 1.0.0 LANGUAGES C)

# 统一输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")


# 设置 C 标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 默认隐藏符号，只导出 API 标记的函数（GCC/Clang）
if(NOT MSVC)
    set(CMAKE_C_VISIBILITY_PRESET hidden)
endif()

set(SRC_FILES "*.h" "*.c") 

file(GLOB PROJECT_FILES
    ${SRC_FILES}
)

# 定义库
add_library(core SHARED ${PROJECT_FILES})

# Windows：定义导出宏，设置运行时库（可选）
if(WIN32)
    target_compile_definitions(core PRIVATE CALCULATOR_EXPORTS)
    # 可选用 MT / MD 等，此处以动态运行时库为例
    # set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

# Android：通过 NDK 工具链构建，需要设置 ABI 等
if(ANDROID)
    # 通常 NDK 工具链已处理 -fPIC，这里可额外添加 Android 特定标志
    target_compile_options(core PRIVATE -fPIC)
    # 若需要日志等，可链接 log 库
#include "../"
    # target_link_libraries(core log)
endif()

# iOS：使用 Xcode 工具链或 ios.toolchain.cmake
if(IOS)
    # 启用 bitcode
    target_compile_options(core PRIVATE -fembed-bitcode)
    # 生成 framework 包（可选）
    set_target_properties(core PROPERTIES
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.example.core
    )
    # iOS 动态库需嵌入应用，通常建议使用静态库，但此处仍可生成动态库
endif()

# Web (Emscripten)：使用 Emscripten 工具链
if(EMSCRIPTEN)
    # 设置输出为 WebAssembly 模块（动态库模式）
    set_target_properties(core PROPERTIES
        PREFIX ""          # 取消 lib 前缀
        SUFFIX ".wasm"     # 后缀设为 .wasm
    )
    # 以 SIDE_MODULE 方式构建，用于动态链接
    target_link_options(core PRIVATE -sSIDE_MODULE=1)
    target_compile_options(core PRIVATE -fPIC)
    # 如果需要导出函数给 JavaScript，可添加 -sEXPORTED_FUNCTIONS
endif()

# 通用：对于非 Windows 平台，显式设置位置无关代码
if(NOT WIN32)
    set_target_properties(core PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()


source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${PROJECT_FILES})