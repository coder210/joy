# ============================================================
# joy 框架源码"纯文件引入"助手
#
# joy 不再构建独立库 target（不产 DLL/SO，也没有导出符号），
# 调用方把本文件 include 进自己的 CMakeLists.txt，
# 再执行 joy_add_sources(<target> [EXCLUDE <绝对路径>.c ...])
# 即可把 joy 全部源码直接编入 <target>。
#
# 用法示例（在 add_executable 之后）：
#   include(${CMAKE_SOURCE_DIR}/joy/joy_add.cmake)
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

if(NOT DEFINED JOY_SOURCE_DIR)
    set(JOY_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "joy 源码根目录")
endif()

function(joy_add_sources TARGET)
    # SPINE 为可选开关：启用后编译 joy/jspine.c（Spine 骨骼动画模块）并链接 spine-c。
    # 用法：joy_add_sources(${TARGET} SPINE)
    cmake_parse_arguments(JAS "SPINE" "" "EXCLUDE" ${ARGN})

    file(GLOB JOY_CORE_FILES     CONFIGURE_DEPENDS "${JOY_SOURCE_DIR}/*.h"  "${JOY_SOURCE_DIR}/*.c")
    file(GLOB JOY_EXTERNAL_FILES CONFIGURE_DEPENDS
        "${JOY_SOURCE_DIR}/external/*.h"  "${JOY_SOURCE_DIR}/external/*.c"
        "${JOY_SOURCE_DIR}/external/lua/*.h"       "${JOY_SOURCE_DIR}/external/lua/*.c"
        "${JOY_SOURCE_DIR}/external/duktape/*.h"   "${JOY_SOURCE_DIR}/external/duktape/*.c"
        "${JOY_SOURCE_DIR}/external/lua_cjson/*.h" "${JOY_SOURCE_DIR}/external/lua_cjson/*.c")
    file(GLOB JOY_LUACLIB_FILES  CONFIGURE_DEPENDS "${JOY_SOURCE_DIR}/luaclib/*.h" "${JOY_SOURCE_DIR}/luaclib/*.c")
    file(GLOB JOY_JSCLIB_FILES   CONFIGURE_DEPENDS "${JOY_SOURCE_DIR}/jsclib/*.h"  "${JOY_SOURCE_DIR}/jsclib/*.c")
    file(GLOB JOY_TEST_FILES     CONFIGURE_DEPENDS "${JOY_SOURCE_DIR}/test/*.h"  "${JOY_SOURCE_DIR}/test/*.c")

    set(JOY_ALL_FILES
        ${JOY_CORE_FILES}
        ${JOY_EXTERNAL_FILES}
        ${JOY_LUACLIB_FILES}
        ${JOY_JSCLIB_FILES}
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
    # 使 #include <joy/joy.h> 可用（joy 源码位于 <根>/joy/，根由 SDL/msgpack 等项目约定）
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
    target_include_directories(${TARGET} PRIVATE
        "${CMAKE_SOURCE_DIR}/msgpack/include"
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
endfunction()