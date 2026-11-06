# ============================================================
# joy 模块：C# 脚本后端（CoreCLR / Mono 嵌入）
#
# 统一模块契约：始终提供 joy_module_cs（INTERFACE），对外只暴露 joy_csharp.h。
# 源码经 target_sources(INTERFACE) 直接编入调用方 target，不建库、不产 DLL/LIB。
# 关闭时只编 stub.c（ENABLED_CS_SCRIPT 未定义），链接符号由空桩承担。
# 与 lua/js 同构：bindings/ 放 C 侧胶水，scripts/ 放语言侧源码（*.cs），
# 根目录只放 CLR 宿主、模块契约与 MSBuild 入口。
# C# 绑定工程 Joy.Bindings.csproj 与本文件同目录；CLR 预编译二进制在仓库根
# runtimes/，由调用方通过 JOY_CLR_ROOT 定位，不在模块内。
# ============================================================
if(NOT DEFINED JOY_MODULE_CS)
    set(JOY_MODULE_CS OFF CACHE BOOL "C# 脚本模块（源码直接编入调用方 target）")
endif()
get_property(_joy_requested GLOBAL PROPERTY JOY_REQUESTED_MODULES)
if("cs" IN_LIST _joy_requested)
    set(JOY_MODULE_CS ON)
endif()

if(TARGET joy_module_cs)
    return()
endif()

add_library(joy_module_cs INTERFACE)
target_include_directories(joy_module_cs INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
target_sources(joy_module_cs INTERFACE "${CMAKE_CURRENT_LIST_DIR}/stub.c")

# 导出给调用方，避免把模块内部路径硬编码到主仓 CMakeLists / CI
set(JOY_MODULE_CS_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "joy cs 模块目录")
set(JOY_BINDINGS_CSPROJ "${CMAKE_CURRENT_LIST_DIR}/Joy.Bindings.csproj"
    CACHE FILEPATH "joy 提供的 C# 绑定工程" FORCE)

if(JOY_MODULE_CS)
    if(NOT MONO_INCLUDE_DIR OR NOT MONO_LIBRARIES)
        message(FATAL_ERROR
            "joy 模块 cs 需要 Mono/CoreCLR 开发包：请设置 MONO_INCLUDE_DIR 与 MONO_LIBRARIES")
    endif()

    file(GLOB _glue CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_LIST_DIR}/*.c"
        "${CMAKE_CURRENT_LIST_DIR}/bindings/*.c")
    list(REMOVE_ITEM _glue "${CMAKE_CURRENT_LIST_DIR}/stub.c")

    target_sources(joy_module_cs INTERFACE ${_glue})
    target_include_directories(joy_module_cs INTERFACE ${MONO_INCLUDE_DIR})
    target_compile_definitions(joy_module_cs INTERFACE ENABLED_CS_SCRIPT)
    target_link_libraries(joy_module_cs INTERFACE ${MONO_LIBRARIES})

    if(JOY_SCRIPT_INCLUDE_ROOT)
        source_group(TREE "${JOY_SCRIPT_INCLUDE_ROOT}" FILES ${_glue})
    endif()
    message(STATUS "joy: 模块 cs 已启用（源码直接编入调用方 target）")
else()
    message(STATUS "joy: 模块 cs 未启用（stub.c 空桩）")
endif()
