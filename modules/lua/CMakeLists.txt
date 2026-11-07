# ============================================================
# joy 模块：Lua 脚本后端
#
# 统一模块契约：始终提供 joy_module_lua（INTERFACE），对外只暴露 joy_lua.h。
# 源码经 target_sources(INTERFACE) 直接编入调用方 target，不建库、不产 DLL/LIB。
# 关闭时只编 stub.c（ENABLED_LUA_SCRIPT 未定义），链接符号由空桩承担。
# 第三方 VM 源码在 external/lua、external/lua_cjson，模块内只放胶水。
#
# bindings/ 按链路分层：api/ 是 IDL 驱动的新链路，legacy/ 是旧手写胶水。
# JOY_ENABLE_LEGACY_BINDINGS 控制 legacy/ 是否进构建（默认 ON，P4 迁移完成后可关）。
# ============================================================
if(NOT DEFINED JOY_MODULE_LUA)
    set(JOY_MODULE_LUA OFF CACHE BOOL "Lua 脚本模块（Lua VM + 绑定胶水）")
endif()
# MODULES 参数由 joy_add.cmake 通过全局属性传递，也支持 -DJOY_MODULE_LUA=ON
get_property(_joy_requested GLOBAL PROPERTY JOY_REQUESTED_MODULES)
if("lua" IN_LIST _joy_requested)
    set(JOY_MODULE_LUA ON)
endif()
# 独立构建（cmake -S modules/lua）时的兜底；由 JoyTargets.cmake 装配时已定义
if(NOT DEFINED JOY_ENABLE_LEGACY_BINDINGS)
    set(JOY_ENABLE_LEGACY_BINDINGS ON)
endif()

if(TARGET joy_module_lua)
    return()
endif()

# 独立构建时的兜底；由 joy_add_sources 调用时已由上层设置
if(NOT JOY_EXTERNAL_DIR)
    set(JOY_EXTERNAL_DIR "${CMAKE_CURRENT_LIST_DIR}/../../external")
endif()
if(NOT JOY_SCRIPT_INCLUDE_ROOT)
    set(JOY_SCRIPT_INCLUDE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../..")
endif()
if(NOT JOY_SCRIPT_EXTRA_INCLUDES)
    set(JOY_SCRIPT_EXTRA_INCLUDES "${JOY_SCRIPT_INCLUDE_ROOT}")
endif()
if(NOT DEFINED JOY_MSGPACK_DIR)
    set(JOY_MSGPACK_DIR "${JOY_SCRIPT_INCLUDE_ROOT}/joy/msgpack")
endif()

add_library(joy_module_lua INTERFACE)
target_include_directories(joy_module_lua INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/bindings/api"
    "${CMAKE_CURRENT_LIST_DIR}/bindings/legacy"
    "${JOY_EXTERNAL_DIR}"
    ${JOY_SCRIPT_EXTRA_INCLUDES}
    "${JOY_MSGPACK_DIR}/include")

# stub.c 始终编译：开关宏决定它的内容是否为空
target_sources(joy_module_lua INTERFACE "${CMAKE_CURRENT_LIST_DIR}/stub.c")

if(JOY_MODULE_LUA)
    file(GLOB _vm CONFIGURE_DEPENDS
        "${JOY_EXTERNAL_DIR}/lua/*.c"
        "${JOY_EXTERNAL_DIR}/lua_cjson/*.c")
    # external/lua/lua.c 是独立解释器入口（main 已改名 xmain2222），
    # 编进可执行文件只会带入无用符号，排除。
    list(REMOVE_ITEM _vm "${JOY_EXTERNAL_DIR}/lua/lua.c")

    # 模块根 + 新链路 api/；legacy/ 单独成组，由开关决定是否加入。
    file(GLOB _glue CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_LIST_DIR}/*.c"
        "${CMAKE_CURRENT_LIST_DIR}/bindings/*.c"
        "${CMAKE_CURRENT_LIST_DIR}/bindings/api/*.c")
    list(REMOVE_ITEM _glue "${CMAKE_CURRENT_LIST_DIR}/stub.c")
    if(JOY_ENABLE_LEGACY_BINDINGS)
        file(GLOB _legacy CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_LIST_DIR}/bindings/legacy/*.c")
        list(APPEND _glue ${_legacy})
        target_compile_definitions(joy_module_lua INTERFACE JOY_ENABLE_LEGACY_BINDINGS)
    endif()

    target_sources(joy_module_lua INTERFACE ${_glue} ${_vm})
    target_compile_definitions(joy_module_lua INTERFACE ENABLED_LUA_SCRIPT)

    if(TARGET SDL3::SDL3)
        target_link_libraries(joy_module_lua INTERFACE SDL3::SDL3)
    endif()
    if(TARGET msgpack-c-static)
        target_link_libraries(joy_module_lua INTERFACE msgpack-c-static)
    elseif(TARGET msgpack-c)
        target_link_libraries(joy_module_lua INTERFACE msgpack-c)
    endif()

    source_group(TREE "${JOY_SCRIPT_INCLUDE_ROOT}" FILES ${_glue} ${_vm})
    message(STATUS "joy: 模块 lua 已启用（legacy=${JOY_ENABLE_LEGACY_BINDINGS}）")
else()
    message(STATUS "joy: 模块 lua 未启用（stub.c 空桩）")
endif()
