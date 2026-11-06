# ============================================================
# joy 框架源码"纯文件引入"助手
#
# joy 不再构建独立库 target（不产 DLL/SO，也没有导出符号），
# 调用方把本文件 include 进自己的 CMakeLists.txt，
# 再执行 joy_add_sources(<target> [EXCLUDE <绝对路径>.c ...])
# 即可把 joy 全部源码直接编入 <target>。
#
# 用法示例（在 add_executable 之后）：
#   include(${CMAKE_SOURCE_DIR}/joy_add.cmake)
#   joy_add_sources(${TARGET_NAME})
#
# 说明：
#  - jmodule.c（SDL_LoadObject 动态加载插件系统）默认不参与编译（停用），
#    但文件保留；其它项目需要启用时删掉 joy_add_sources 函数里的排除行即可。
#  - 内部定义 JOY_STATIC 宏把 jconfig.h 里的 JOY_API 置空，
#    源码编入 exe 后不产生任何 PE/ELF 导出符号。
#  - 原 joy 库 target PUBLIC 传递的 SDL3::SDL3 与 msgpack-c-static
#    在这里一并链接，调用方无需再单独处理。
# ============================================================

# 目录改名（joy/ → src/）后，旧 build 目录里的 CMakeCache 可能仍指向已不存在的
# .../joy/joy，导致 GLOB 收不到任何源文件、最终链接报一堆未解析符号。
# 这里顺带校验 joy.h 是否存在，陈旧 cache 自动按当前文件位置纠正。
if(NOT DEFINED JOY_SOURCE_DIR OR NOT EXISTS "${JOY_SOURCE_DIR}/joy.h")
    set(JOY_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "joy 源码根目录（含 joy.h）" FORCE)
endif()

# 本文件所在目录（joy 仓库根）。函数体里的 CMAKE_CURRENT_LIST_DIR 指向调用方的
# listfile，不能用来定位 modules/，故在此固化。
set(JOY_ADD_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")

# 让 MODULES 参数能以普通变量形式覆盖各模块 CMakeLists 里 option() 的默认值
cmake_policy(SET CMP0077 NEW)

# C# 后端不建库 target，其开关在此统一定义（lua/js 的开关仍在各自 CMakeLists）。
option(JOY_MODULE_CSHARP "C# 脚本模块（源码直接编入调用方 target）" OFF)

function(joy_add_sources TARGET)
    # SPINE 为可选开关：启用后编译 joy/jspine.c（Spine 骨骼动画模块）并链接 spine-c。
    # 用法：joy_add_sources(${TARGET} SPINE)
    cmake_parse_arguments(JAS "SPINE" "" "EXCLUDE;MODULES" ${ARGN})

    file(GLOB JOY_CORE_FILES     CONFIGURE_DEPENDS "${JOY_SOURCE_DIR}/*.h"  "${JOY_SOURCE_DIR}/*.c")
    # 脚本后端（lua/luaclib/jsclib/csclib/duktape/lua_cjson）已全部迁出到
    # joy_lua / joy_js / joy_csharp 独立仓库；jtilemap.c 改用 cJSON 解析地图，
    # 核心不再携带 Lua VM，故这里只收集 external/ 顶层的第三方源码。
    file(GLOB JOY_EXTERNAL_FILES CONFIGURE_DEPENDS
        "${JOY_SOURCE_DIR}/external/*.h"  "${JOY_SOURCE_DIR}/external/*.c")
    file(GLOB JOY_TEST_FILES     CONFIGURE_DEPENDS "${JOY_SOURCE_DIR}/test/*.h"  "${JOY_SOURCE_DIR}/test/*.c")

    set(JOY_ALL_FILES
        ${JOY_CORE_FILES}
        ${JOY_EXTERNAL_FILES}
        ${JOY_TEST_FILES}
    )

    if(JAS_EXCLUDE)
        list(REMOVE_ITEM JOY_ALL_FILES ${JAS_EXCLUDE})
    endif()

    # 插件系统默认停用：jmodule.c 不参与编译，但文件保留（可能被其它项目复用）。
    # 需要在某项目里启用插件系统时，删掉下面这行（或手动把 jmodule.c 加回目标）。
    list(REMOVE_ITEM JOY_ALL_FILES "${JOY_SOURCE_DIR}/jmodule.c")

    target_sources(${TARGET} PRIVATE ${JOY_ALL_FILES})
    # IDE 显示分组：joy 源码在目标下以 joy/... 目录树展示（TREE 到 joy 的父目录）
    source_group(TREE "${JOY_SOURCE_DIR}/.." FILES ${JOY_ALL_FILES})
    # 使 #include <joy/joy.h> 可用（joy 源码位于 <根>/，根由 SDL/msgpack 等项目约定）
    target_include_directories(${TARGET} PRIVATE "${JOY_SOURCE_DIR}/..")

    # 原 joy 库 target 的 PUBLIC 链接（SDL3、msgpack）。
    #
    # 注意：msgpack 仅在 MSGPACK_ENABLE_SHARED 与 MSGPACK_ENABLE_STATIC 同时为 ON 时
    # 才创建 msgpack-c-static 目标（见 msgpack/CMakeLists.txt:146）。Emscripten 等平台
    # 把 BUILD_SHARED_LIBS 设为 OFF 时，MSGPACK_ENABLE_SHARED 被置 OFF，该目标不存在；
    # 此时写死的 "msgpack-c-static" 会被 CMake 当成普通库名，include 目录不会传递，
    # 于是 joy/jmsgpack.c 报 'msgpack/pack.h' file not found。
    # 故按目标实际是否存在选择，并显式补上 include 目录兜底。
    if (TARGET msgpack-c-static)
        target_link_libraries(${TARGET} PUBLIC SDL3::SDL3 msgpack-c-static)
    else()
        target_link_libraries(${TARGET} PUBLIC SDL3::SDL3 msgpack-c)
    endif()
    # msgpack 位置：默认与 joy/ 同级（单仓根目录、或引擎仓库根目录都成立）。
    # 拆仓后第三方若在别处，可 -DJOY_MSGPACK_DIR=... 覆盖。
    if(NOT DEFINED JOY_MSGPACK_DIR)
        set(JOY_MSGPACK_DIR "${JOY_SOURCE_DIR}/../msgpack")
    endif()
    target_include_directories(${TARGET} PRIVATE
        "${JOY_MSGPACK_DIR}/include"
        "${CMAKE_BINARY_DIR}/msgpack/include")

    # C 源码统一按 C99 编译
    set_property(TARGET ${TARGET} PROPERTY C_STANDARD 99)
    set_property(TARGET ${TARGET} PROPERTY C_STANDARD_REQUIRED ON)

    # 纯源码引入：清空导出宏，不产生库导出符号
    target_compile_definitions(${TARGET} PRIVATE JOY_STATIC)

    # 可选：Spine 骨骼动画模块（joy/jspine.c）
    if(JAS_SPINE)
        target_compile_definitions(${TARGET} PRIVATE JOY_USE_SPINE)
        target_link_libraries(${TARGET} PRIVATE spine-c)
        target_include_directories(${TARGET} PRIVATE "${JOY_SOURCE_DIR}/../spine/spine-c/include")
    endif()

    # ---- 平台特定配置（从原 joy/CMakeLists.txt 迁移）----
    if(APPLE)
        target_compile_definitions(${TARGET} PRIVATE LUA_USE_IOS MG_ENABLE_HTTP_CGI=0)
    endif()
    if(ANDROID)
        target_compile_options(${TARGET} PRIVATE -fPIC)
        target_compile_definitions(${TARGET} PRIVATE MG_ENABLE_FILESYSTEM=0)
    endif()
    if(IOS)
        target_compile_options(${TARGET} PRIVATE -fembed-bitcode)
    endif()
    if(EMSCRIPTEN)
        target_compile_options(${TARGET} PRIVATE -fPIC)
        target_link_libraries(${TARGET} PRIVATE websocket)
    endif()
    if(MSVC)
        # joy 源码含中文注释，MSVC 按 UTF-8 解析避免 GBK 误读
        target_compile_options(${TARGET} PRIVATE /utf-8)
    endif()

    # ---- 脚本后端模块 ----
    # 用法：joy_add_sources(<target> MODULES lua js csharp)
    #   列出的模块强制启用；未列出的模块默认关闭（也可用 -DJOY_MODULE_XXX=ON 启用）。
    # 关闭的模块退化为 INTERFACE 目标，只暴露头文件路径，实现由
    # joy_script_stub.c 提供的空函数承担——调用点始终可编译、可链接。
    if(NOT DEFINED JOY_MSGPACK_DIR)
        set(JOY_MSGPACK_DIR "${JOY_SOURCE_DIR}/../msgpack")
    endif()
    # 供各模块子目录继承的编译上下文（include 根为 joy 仓库的父目录，使
    # #include <joy/joy.h> 成立）
    set(JOY_SCRIPT_INCLUDE_ROOT "${JOY_SOURCE_DIR}/..")
    set(JOY_SCRIPT_EXTRA_INCLUDES
        "${JOY_SCRIPT_INCLUDE_ROOT}"
        "${JOY_MSGPACK_DIR}/include"
        "${CMAKE_BINARY_DIR}/msgpack/include")

    set(_joy_known_modules lua js csharp)
    foreach(_joy_mod ${_joy_known_modules})
        if(_joy_mod IN_LIST JAS_MODULES)
            string(TOUPPER "${_joy_mod}" _joy_mod_upper)
            set(JOY_MODULE_${_joy_mod_upper} ON)
        endif()
        set(_joy_mod_dir "${JOY_ADD_CMAKE_DIR}/modules/${_joy_mod}")

        if(_joy_mod STREQUAL "csharp")
            # C# 后端与 joy 核心一样走"纯源码引入"：不建库 target、不产 DLL/LIB。
            # 选中 MODULES csharp（或 -DJOY_MODULE_CSHARP=ON）后，源码直接编入
            # 调用方 <target> 并按 joy 源码树在 IDE 里显示；对外只暴露 joy_csharp.h。
            # 头文件路径始终可用：未启用时 joy_script_stub.c 也要包含它。
            target_include_directories(${TARGET} PRIVATE "${_joy_mod_dir}")
            if(JOY_MODULE_CSHARP)
                if(NOT MONO_INCLUDE_DIR OR NOT MONO_LIBRARIES)
                    message(FATAL_ERROR
                        "joy 模块 csharp 需要 Mono/CoreCLR 开发包：请设置 MONO_INCLUDE_DIR 与 MONO_LIBRARIES")
                endif()
                file(GLOB _joy_cs_sources CONFIGURE_DEPENDS
                    "${_joy_mod_dir}/*.c"
                    "${_joy_mod_dir}/csclib/*.c"
                    "${_joy_mod_dir}/csclib/generated/*.c")
                target_sources(${TARGET} PRIVATE ${_joy_cs_sources})
                source_group(TREE "${JOY_SOURCE_DIR}/.." FILES ${_joy_cs_sources})
                target_include_directories(${TARGET} PRIVATE ${MONO_INCLUDE_DIR})
                target_compile_definitions(${TARGET} PRIVATE ENABLED_CSHARP_SCRIPT)
                target_link_libraries(${TARGET} PRIVATE ${MONO_LIBRARIES})
                message(STATUS "joy: 模块 csharp 已启用（源码直接编入 ${TARGET}）")
            else()
                message(STATUS "joy: 模块 csharp 未启用（桩实现）")
            endif()
        else()
            if(EXISTS "${_joy_mod_dir}/CMakeLists.txt"
               AND NOT TARGET joy_module_${_joy_mod})
                add_subdirectory("${_joy_mod_dir}"
                                 "${CMAKE_BINARY_DIR}/joy_modules/${_joy_mod}")
            endif()
            if(TARGET joy_module_${_joy_mod})
                target_link_libraries(${TARGET} PRIVATE joy_module_${_joy_mod})
            endif()
        endif()
    endforeach()
endfunction()