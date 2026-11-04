# ============================================================
# joy 模块：JavaScript (duktape) 脚本后端（原独立仓 joy_js）
#
# 由 joy_add_sources(<target> MODULES js) 自动 add_subdirectory 并链接。
# 关闭（默认）时只生成空 INTERFACE 目标：核心零成本，duktape 不参与编译。
# ============================================================
option(JOY_MODULE_JS "JavaScript (duktape) 脚本模块" OFF)

if(TARGET joy_module_js)
    return()
endif()

if(NOT JOY_MODULE_JS)
    add_library(joy_module_js INTERFACE)
    # 关闭时仍暴露头文件路径：调用方可以照常 #include "joy_js.h"，
    # 链接到 src/joy_script_stub.c 提供的空实现。
    target_include_directories(joy_module_js INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
    message(STATUS "joy: 模块 js 未启用（桩实现）")
    return()
endif()

if(NOT JOY_SCRIPT_INCLUDE_ROOT)
    # 独立构建（cmake -S modules/js）时的兜底：include 根为 joy 仓库的父目录，
    # 以 #include <joy/joy.h> 成立为前提。
    set(JOY_SCRIPT_INCLUDE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../..")
endif()
if(NOT JOY_SCRIPT_EXTRA_INCLUDES)
    set(JOY_SCRIPT_EXTRA_INCLUDES "${JOY_SCRIPT_INCLUDE_ROOT}")
endif()
if(NOT DEFINED JOY_MSGPACK_DIR)
    set(JOY_MSGPACK_DIR "${JOY_SCRIPT_INCLUDE_ROOT}/joy/msgpack")
endif()

file(GLOB JOY_JS_JSCLIB CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/jsclib/*.c")

add_library(joy_module_js OBJECT
    "${CMAKE_CURRENT_LIST_DIR}/jsx.c"
    "${CMAKE_CURRENT_LIST_DIR}/external/duktape/duktape.c"
    ${JOY_JS_JSCLIB})

target_include_directories(joy_module_js PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(joy_module_js PRIVATE ${JOY_SCRIPT_EXTRA_INCLUDES})

# 开关宏以 PUBLIC 传递：宿主的 src/joy_script_stub.c 据此关闭桩实现
target_compile_definitions(joy_module_js PUBLIC ENABLED_JS_SCRIPT JOY_STATIC)

if(TARGET SDL3::SDL3)
    target_link_libraries(joy_module_js PUBLIC SDL3::SDL3)
endif()
if(TARGET msgpack-c-static)
    target_link_libraries(joy_module_js PUBLIC msgpack-c-static)
elseif(TARGET msgpack-c)
    target_link_libraries(joy_module_js PUBLIC msgpack-c)
endif()

set_target_properties(joy_module_js PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON)

if(MSVC)
    target_compile_options(joy_module_js PRIVATE /utf-8)
endif()

source_group(TREE "${JOY_SCRIPT_INCLUDE_ROOT}" FILES
    "${CMAKE_CURRENT_LIST_DIR}/jsx.c" ${JOY_JS_JSCLIB})

message(STATUS "joy: 模块 js 已启用")
