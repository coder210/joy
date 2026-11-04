# ============================================================
# joy 模块：Lua 脚本后端（原独立仓 joy_lua）
#
# 由 joy_add_sources(<target> MODULES lua) 自动 add_subdirectory 并链接。
# 关闭（默认）时只生成空 INTERFACE 目标：核心零成本，Lua 源码不参与编译。
# ============================================================
option(JOY_MODULE_LUA "Lua 脚本模块（Lua VM + luaclib）" OFF)

if(TARGET joy_module_lua)
    return()
endif()

if(NOT JOY_MODULE_LUA)
    add_library(joy_module_lua INTERFACE)
    # 关闭时仍暴露头文件路径：调用方可以照常 #include "joy_lua.h"，
    # 链接到 src/joy_script_stub.c 提供的空实现。
    target_include_directories(joy_module_lua INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
    message(STATUS "joy: 模块 lua 未启用（桩实现）")
    return()
endif()

# ---- 独立构建（cmake -S modules/lua）时的兜底 ----
if(NOT JOY_SCRIPT_INCLUDE_ROOT)
    # 独立构建（cmake -S modules/lua）时的兜底：include 根为 joy 仓库的父目录，
    # 以 #include <joy/joy.h> 成立为前提。
    set(JOY_SCRIPT_INCLUDE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../..")
endif()
if(NOT JOY_SCRIPT_EXTRA_INCLUDES)
    set(JOY_SCRIPT_EXTRA_INCLUDES "${JOY_SCRIPT_INCLUDE_ROOT}")
endif()
if(NOT DEFINED JOY_MSGPACK_DIR)
    set(JOY_MSGPACK_DIR "${JOY_SCRIPT_INCLUDE_ROOT}/joy/msgpack")
endif()

file(GLOB JOY_LUA_VM CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/external/lua/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/external/lua_cjson/*.c")
file(GLOB JOY_LUA_LUACLIB CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/luaclib/*.c")

# external/lua/lua.c 是独立解释器入口（main 已改名 xmain2222），编进可执行文件
# 只会带入无用符号，排除。
list(REMOVE_ITEM JOY_LUA_VM "${CMAKE_CURRENT_LIST_DIR}/external/lua/lua.c")

add_library(joy_module_lua OBJECT
    "${CMAKE_CURRENT_LIST_DIR}/jluax.c"
    ${JOY_LUA_VM}
    ${JOY_LUA_LUACLIB})

target_include_directories(joy_module_lua PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(joy_module_lua PRIVATE ${JOY_SCRIPT_EXTRA_INCLUDES})

# 开关宏以 PUBLIC 传递：宿主的 src/joy_script_stub.c 据此关闭桩实现
target_compile_definitions(joy_module_lua PUBLIC ENABLED_LUA_SCRIPT JOY_STATIC)

if(TARGET SDL3::SDL3)
    target_link_libraries(joy_module_lua PUBLIC SDL3::SDL3)
endif()
if(TARGET msgpack-c-static)
    target_link_libraries(joy_module_lua PUBLIC msgpack-c-static)
elseif(TARGET msgpack-c)
    target_link_libraries(joy_module_lua PUBLIC msgpack-c)
endif()

set_target_properties(joy_module_lua PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON)

if(MSVC)
    target_compile_options(joy_module_lua PRIVATE /utf-8)
endif()

source_group(TREE "${JOY_SCRIPT_INCLUDE_ROOT}" FILES
    "${CMAKE_CURRENT_LIST_DIR}/jluax.c" ${JOY_LUA_VM} ${JOY_LUA_LUACLIB})

message(STATUS "joy: 模块 lua 已启用")
