# ============================================================
# joy 模块：C# (Mono) 脚本后端（原独立仓 joy_csharp）
#
# 由 joy_add_sources(<target> MODULES csharp) 自动 add_subdirectory 并链接。
# 关闭（默认）时只生成空 INTERFACE 目标：核心零成本，无需 Mono 开发包。
# 显式启用却没给 Mono 路径时直接报错——静默退化只会让人查半天为什么
# csx_create 返回 NULL。
# ============================================================
option(JOY_MODULE_CSHARP "C# (Mono) 脚本模块" OFF)

if(TARGET joy_module_csharp)
    return()
endif()

if(NOT JOY_MODULE_CSHARP)
    add_library(joy_module_csharp INTERFACE)
    # 关闭时仍暴露头文件路径：调用方可以照常 #include "joy_csharp.h"，
    # 链接到 src/joy_script_stub.c 提供的空实现。
    target_include_directories(joy_module_csharp INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/glue")
    message(STATUS "joy: 模块 csharp 未启用（桩实现）")
    return()
endif()

if(NOT MONO_INCLUDE_DIR OR NOT MONO_LIBRARIES)
    message(FATAL_ERROR
        "joy 模块 csharp 需要 Mono 开发包：请设置 MONO_INCLUDE_DIR（mono/metadata/*.h 所在目录）"
        " 与 MONO_LIBRARIES（如 mono-2）。若不需要 C# 脚本，请去掉 MODULES csharp。")
endif()

if(NOT JOY_SCRIPT_INCLUDE_ROOT)
    # 独立构建（cmake -S modules/csharp）时的兜底：include 根为 joy 仓库的父目录，
    # 以 #include <joy/joy.h> 成立为前提。
    set(JOY_SCRIPT_INCLUDE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../..")
endif()
if(NOT JOY_SCRIPT_EXTRA_INCLUDES)
    set(JOY_SCRIPT_EXTRA_INCLUDES "${JOY_SCRIPT_INCLUDE_ROOT}")
endif()

file(GLOB JOY_CS_GLUE CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/glue/*.c")

add_library(joy_module_csharp OBJECT
    "${CMAKE_CURRENT_LIST_DIR}/joy_csharp.c"
    ${JOY_CS_GLUE})

target_include_directories(joy_module_csharp PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/glue")
target_include_directories(joy_module_csharp PRIVATE
    ${JOY_SCRIPT_EXTRA_INCLUDES}
    ${MONO_INCLUDE_DIR})

# 开关宏以 PUBLIC 传递：宿主的 src/joy_script_stub.c 据此关闭桩实现
target_compile_definitions(joy_module_csharp PUBLIC ENABLED_CSHARP_SCRIPT JOY_STATIC)

target_link_libraries(joy_module_csharp PUBLIC ${MONO_LIBRARIES})

set_target_properties(joy_module_csharp PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON)

if(MSVC)
    target_compile_options(joy_module_csharp PRIVATE /utf-8)
    # glue/ 当前为空翻译单元（全注释），抑制 C4206
    target_compile_options(joy_module_csharp PRIVATE /wd4206)
endif()

source_group(TREE "${JOY_SCRIPT_INCLUDE_ROOT}" FILES
    "${CMAKE_CURRENT_LIST_DIR}/joy_csharp.c" ${JOY_CS_GLUE})

message(STATUS "joy: 模块 csharp 已启用")
