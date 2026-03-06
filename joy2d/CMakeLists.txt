cmake_minimum_required(VERSION 3.10)

# 统一输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# 设置 C 标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(LIB_NAME joy2d)

# 默认隐藏符号，只导出 API 标记的函数（GCC/Clang）
if(NOT MSVC)
    set(CMAKE_C_VISIBILITY_PRESET hidden)
endif()

# 定义各模块源文件通配符（使用相对路径）
file(GLOB EXTERNAL_FILES
    "external/lua/*.h" "external/lua/*.c"
    "external/mujs/*.h" "external/mujs/*.c"
    "external/klib/*.h" "external/klib/*.c"
    "external/*.h" "external/*.c"
)

file(GLOB LUACLIB_FILES "luaclib/*.h" "luaclib/*.c")
file(GLOB JSCLIB_FILES "jsclib/*.h" "jsclib/*.c")
file(GLOB CSCLIB_FILES "csclib/*.h" "csclib/*.c")
file(GLOB TEST_FILES "test/*.h" "test/*.c")
file(GLOB SRC_FILES "*.h" "*.c")

# 合并所有源文件
set(PROJECT_FILES
    ${EXTERNAL_FILES}
    ${CORE_FILES}
    ${TEST_FILES}
    ${LUACLIB_FILES}
    ${JSCLIB_FILES}
    ${CSCLIB_FILES}
    ${SRC_FILES}
)

file(GLOB PROJECT_FILES
    ${PROJECT_FILES}
)

# 根据平台选择库类型：Emscripten 下构建静态库，其他平台构建共享库
if(EMSCRIPTEN)
    set(LIB_TYPE STATIC)
else()
    set(LIB_TYPE SHARED)
endif()

add_library(${LIB_NAME} ${LIB_TYPE} ${PROJECT_FILES})

# Windows：定义导出宏，设置运行时库（可选）
if(WIN32)
    target_compile_definitions(${LIB_NAME} PRIVATE CALCULATOR_EXPORTS)
    # 可选用 MT / MD 等，此处以动态运行时库为例
    # set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

# Apple
if(APPLE)
    add_definitions(-DLUA_USE_IOS)
endif()

# Android：通过 NDK 工具链构建，需要设置 ABI 等
if(ANDROID)
    # 通常 NDK 工具链已处理 -fPIC，这里可额外添加 Android 特定标志
    target_compile_options(${LIB_NAME} PRIVATE -fPIC)
    # 若需要日志等，可链接 log 库
    # target_link_libraries(${LIB_NAME} log)
endif()

# iOS：使用 Xcode 工具链或 ios.toolchain.cmake
if(IOS)
    # 启用 bitcode
    target_compile_options(${LIB_NAME} PRIVATE -fembed-bitcode)
    # 生成 framework 包（可选）
    set_target_properties(${LIB_NAME} PROPERTIES
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.example.${LIB_NAME}
    )
    # iOS 动态库需嵌入应用，通常建议使用静态库，但此处仍可生成动态库
endif()

# Web (Emscripten)：构建静态库
if(EMSCRIPTEN)
    # 静态库不需要特殊前缀/后缀，也不需要 SIDE_MODULE 链接选项
    # 可选的 -fPIC 保留或移除均可，此处保留以保持一致性
    target_compile_options(${LIB_NAME} PRIVATE -fPIC)
    # 如果需要导出函数给链接器，可添加 -sEXPORTED_FUNCTIONS（但通常用于可执行文件）
endif()

# 通用：对于非 Windows 平台，显式设置位置无关代码
if(NOT WIN32)
    set_target_properties(${LIB_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# 链接依赖库（由根目录的 add_subdirectory 引入）
target_link_libraries(${LIB_NAME} PUBLIC
    SDL3::SDL3
)

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${PROJECT_FILES})