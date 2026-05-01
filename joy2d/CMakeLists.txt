cmake_minimum_required(VERSION 3.10)

# 统一输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# 设置 C 标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)  # 必须打开，否则 alloca 报错

if (WIN32)
	set(ENABLED_CSHARP_SCRIPT OFF)
else()
	set(ENABLED_CSHARP_SCRIPT OFF)
endif()

set(LIB_NAME joy2d)

# 默认隐藏符号，只导出 API 标记的函数（GCC/Clang）
if(NOT MSVC)
    set(CMAKE_C_VISIBILITY_PRESET hidden)
endif()

if (ENABLED_CSHARP_SCRIPT)
	include_directories(${PROJECT_SOURCE_DIR}/mono/windows.x64.Release/include/mono-2.0)
	link_directories(${PROJECT_SOURCE_DIR}/mono/windows.x64.Release)
    file(GLOB CSCLIB_FILES "csclib/*.h" "csclib/*.c")
else()
    set(CSCLIB_FILES "")
endif()

# 定义各模块源文件通配符（使用相对路径）
file(GLOB EXTERNAL_FILES
    "external/*.h" "external/*.c"
    "external/lua/*.h" "external/lua/*.c"
    "external/mujs/*.h" "external/mujs/*.c"
    "external/lua_cjson/*.h" "external/lua_cjson/*.c"
)

file(GLOB LUACLIB_FILES "luaclib/*.h" "luaclib/*.c")
file(GLOB JSCLIB_FILES "jsclib/*.h" "jsclib/*.c")
file(GLOB TEST_FILES "test/*.h" "test/*.c")
file(GLOB SRC_FILES "*.h" "*.c")

# 合并所有源文件
set(PROJECT_FILES
    ${EXTERNAL_FILES}
    ${TEST_FILES}
    ${LUACLIB_FILES}
    ${JSCLIB_FILES}
    ${CSCLIB_FILES}
    ${SRC_FILES}
)

# 根据平台选择库类型：Emscripten 下构建静态库，其他平台构建共享库
if(EMSCRIPTEN)
    set(LIB_TYPE STATIC)
else()
    set(LIB_TYPE SHARED)
endif()

add_library(${LIB_NAME} ${LIB_TYPE} ${PROJECT_FILES})

# Windows：定义导出宏
if(WIN32)
    target_compile_definitions(${LIB_NAME} PRIVATE CALCULATOR_EXPORTS)
endif()

# Apple
if(APPLE)
    add_definitions(-DLUA_USE_IOS)
endif()

# Android
if(ANDROID)
    target_compile_options(${LIB_NAME} PRIVATE -fPIC)
endif()

# iOS
if(IOS)
    target_compile_options(${LIB_NAME} PRIVATE -fembed-bitcode)
    set_target_properties(${LIB_NAME} PROPERTIES
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.example.${LIB_NAME}
    )
endif()

# ==============================================
# 【终极修复】Emscripten + Mongoose 完全兼容
# 解决 struct ip_mreq / alloca / clock_gettime 全部错误
# ==============================================
if(EMSCRIPTEN)
    target_compile_options(${LIB_NAME} PRIVATE -fPIC)
    target_compile_definitions(${LIB_NAME} PRIVATE
        MG_EMBEDDED
        MG_DISABLE_CLOCK_GETTIME
        MG_DISABLE_MULTICAST   # 禁用组播，解决 struct ip_mreq 错误
        _GNU_SOURCE           # 自动包含 alloca
        __BSD_VISIBLE=1       # 暴露 alloca 声明
    )
endif()

# 通用：对于非 Windows 平台，显式设置位置无关代码
if(NOT WIN32)
    set_target_properties(${LIB_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Unix/Linux 平台
if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
    target_compile_definitions(${LIB_NAME} PRIVATE
        _GNU_SOURCE
        _POSIX_C_SOURCE=199309L
    )
endif()

if (ENABLED_CSHARP_SCRIPT)
	target_link_libraries(${LIB_NAME} PUBLIC
		SDL3::SDL3
        coreclr.import
	)
else() 
	target_link_libraries(${LIB_NAME} PUBLIC
		SDL3::SDL3
	)
endif()

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${PROJECT_FILES})