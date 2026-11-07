# ============================================================
# joy 模块：JavaScript (duktape) 脚本后端
#
# 统一模块契约：始终提供 joy_module_js（INTERFACE），对外只暴露 joy_js.h。
# 源码经 target_sources(INTERFACE) 直接编入调用方 target，不建库、不产 DLL/LIB。
# 关闭时只编 stub.c（ENABLED_JS_SCRIPT 未定义），链接符号由空桩承担。
# 第三方 VM 源码在 external/duktape，模块内只放胶水。
#
# bindings/ 按链路分层：api/ 是 IDL 驱动的新链路，legacy/ 是旧手写胶水。
# JOY_ENABLE_LEGACY_BINDINGS 控制 legacy/ 是否进构建（默认 ON，P4 迁移完成后可关）。
# ============================================================
if(NOT DEFINED JOY_MODULE_JS)
    set(JOY_MODULE_JS OFF CACHE BOOL "JavaScript (duktape) 脚本模块")
endif()
get_property(_joy_requested GLOBAL PROPERTY JOY_REQUESTED_MODULES)
if("js" IN_LIST _joy_requested)
    set(JOY_MODULE_JS ON)
endif()
if(NOT DEFINED JOY_ENABLE_LEGACY_BINDINGS)
    set(JOY_ENABLE_LEGACY_BINDINGS ON)
endif()

if(TARGET joy_module_js)
    return()
endif()

# 独立构建（cmake -S modules/js）时的兜底；由 joy_add_sources 调用时已由上层设置
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

add_library(joy_module_js INTERFACE)
target_include_directories(joy_module_js INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/bindings/api"
    "${CMAKE_CURRENT_LIST_DIR}/bindings/legacy"
    "${JOY_EXTERNAL_DIR}"
    ${JOY_SCRIPT_EXTRA_INCLUDES}
    "${JOY_MSGPACK_DIR}/include")

target_sources(joy_module_js INTERFACE "${CMAKE_CURRENT_LIST_DIR}/stub.c")

if(JOY_MODULE_JS)
    file(GLOB _glue CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_LIST_DIR}/*.c"
        "${CMAKE_CURRENT_LIST_DIR}/bindings/*.c"
        "${CMAKE_CURRENT_LIST_DIR}/bindings/api/*.c")
    list(REMOVE_ITEM _glue "${CMAKE_CURRENT_LIST_DIR}/stub.c")
    if(JOY_ENABLE_LEGACY_BINDINGS)
        file(GLOB _legacy CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_LIST_DIR}/bindings/legacy/*.c")
        list(APPEND _glue ${_legacy})
        target_compile_definitions(joy_module_js INTERFACE JOY_ENABLE_LEGACY_BINDINGS)
    endif()

    target_sources(joy_module_js INTERFACE
        ${_glue} "${JOY_EXTERNAL_DIR}/duktape/duktape.c")
    target_compile_definitions(joy_module_js INTERFACE ENABLED_JS_SCRIPT)

    if(TARGET SDL3::SDL3)
        target_link_libraries(joy_module_js INTERFACE SDL3::SDL3)
    endif()
    if(TARGET msgpack-c-static)
        target_link_libraries(joy_module_js INTERFACE msgpack-c-static)
    elseif(TARGET msgpack-c)
        target_link_libraries(joy_module_js INTERFACE msgpack-c)
    endif()

    source_group(TREE "${JOY_SCRIPT_INCLUDE_ROOT}" FILES ${_glue}
        "${JOY_EXTERNAL_DIR}/duktape/duktape.c")
    message(STATUS "joy: 模块 js 已启用（legacy=${JOY_ENABLE_LEGACY_BINDINGS}）")
else()
    message(STATUS "joy: 模块 js 未启用（stub.c 空桩）")
endif()
