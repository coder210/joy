# ============================================================
# joy 的构建契约：源码注入，唯一入口 joy::all
#
# 故意不建静态库。公开头里有编译期可配置的 inline 契约：
#   include/joy/jdefs.h 的 PIXELS_PER_METER（#ifndef 可覆盖）与
#   meters_to_pixels / pixels_to_meters（static inline，每个 TU 各编一份）。
# 这类契约要求 joy 的 .c 与宿主的 .c 看到同一组编译宏。做成静态库后，
# 库内按默认值固化、宿主按自己的 PRIVATE -D 固化，同一进程会出现两套换算。
# 源码注入让宿主的编译定义自然作用于 joy 源码，这也是旧 joy_add_sources()
# 一直以来的语义，外部消费方无需改动。
#
# 与旧实现方式的区别只有一处：源码来自 cmake/JoySources.cmake 的显式清单，
# 不再是 file(GLOB) 把 *.c / external/*.c / test/*.c 一锅端。
# ============================================================

get_filename_component(JOY_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
get_filename_component(JOY_INCLUDE_ROOT "${JOY_ROOT}/.." ABSOLUTE)

option(JOY_ENABLE_GLTF   "启用 joy_r3d.c 中的 glTF 分片（JOY_USE_GLTF）" OFF)
option(JOY_ENABLE_SPINE  "启用 joy_r2d.c 中的 spine 分片并链接 spine-c" OFF)
option(JOY_ENABLE_PLUGINS "启用 joy_script.c 中的插件分片（SDL_LoadObject）" OFF)
# 旧手写胶水 modules/*/bindings/legacy/ 已整体删除，开关随之移除；
# 脚本层只剩 IDL 驱动的 Joy.Api.Bindings，宿主接口留在 modules/cs/bindings/host/。

include("${CMAKE_CURRENT_LIST_DIR}/JoySources.cmake")
joy_collect_sources(_joy_sources)

# api/ 的运行层与生成 thunk：调用 joy.h 里的 C 函数，必须与 joy 源码同 target。
set(_joy_api_sources
    "${JOY_ROOT}/api/runtime/joy_value.c"
    "${JOY_ROOT}/api/runtime/joy_obj.c"
    "${JOY_ROOT}/api/runtime/joy_thunk.c"
    "${JOY_ROOT}/api/generated/thunks_generated.c"
    "${JOY_ROOT}/api/generated/joy_api_vtable.c")

# --------------------------------------------------------------- joy::all
add_library(joy_all INTERFACE)
add_library(joy::all ALIAS joy_all)

# INTERFACE sources 会原样编进 link 本目标的 target；路径必须是绝对路径，
# 否则会按消费方的 CMakeLists 位置解析。
target_sources(joy_all INTERFACE ${_joy_sources} ${_joy_api_sources})

target_include_directories(joy_all INTERFACE
    "${JOY_INCLUDE_ROOT}"
    "${JOY_ROOT}"
    "${JOY_ROOT}/include"
    "${JOY_ROOT}/include/joy"
    "${JOY_ROOT}/api"
    "${JOY_ROOT}/msgpack/include"
    "${CMAKE_BINARY_DIR}/msgpack/include")

# JOY_STATIC 让 jconfig.h 里的 JOY_API 置空（纯源码引入不产生导出符号）。
target_compile_definitions(joy_all INTERFACE JOY_STATIC)
target_compile_features(joy_all INTERFACE c_std_99)

if(TARGET SDL3::SDL3)
    target_link_libraries(joy_all INTERFACE SDL3::SDL3)
endif()
if(TARGET msgpack-c-static)
    target_link_libraries(joy_all INTERFACE msgpack-c-static)
elseif(TARGET msgpack-c)
    target_link_libraries(joy_all INTERFACE msgpack-c)
endif()

if(JOY_ENABLE_GLTF)
    target_compile_definitions(joy_all INTERFACE JOY_USE_GLTF)
endif()
if(JOY_ENABLE_SPINE)
    target_compile_definitions(joy_all INTERFACE JOY_USE_SPINE)
    if(TARGET spine-c)
        target_link_libraries(joy_all INTERFACE spine-c)
        target_include_directories(joy_all INTERFACE "${JOY_ROOT}/spine/spine-c/include")
    endif()
endif()
if(JOY_ENABLE_PLUGINS)
    target_compile_definitions(joy_all INTERFACE JOY_USE_PLUGINS)
endif()

# 平台与工具链配置：源码注入下这些宏会作用于宿主的全部 TU（含宿主自身代码），
# 这是与旧 joy_add.cmake 一致的语义，不是新增行为。
if(MSVC)
    target_compile_options(joy_all INTERFACE /utf-8)
endif()
if(APPLE)
    target_compile_definitions(joy_all INTERFACE LUA_USE_IOS MG_ENABLE_HTTP_CGI=0)
endif()
if(ANDROID)
    target_compile_options(joy_all INTERFACE -fPIC)
    target_compile_definitions(joy_all INTERFACE MG_ENABLE_FILESYSTEM=0)
endif()
if(IOS)
    target_compile_options(joy_all INTERFACE -fembed-bitcode)
endif()
if(EMSCRIPTEN)
    target_compile_options(joy_all INTERFACE -fPIC)
    target_link_libraries(joy_all INTERFACE websocket)
endif()

# ------------------------------------------------------------ 脚本后端模块
set(JOY_EXTERNAL_DIR          "${JOY_ROOT}/external")
set(JOY_SCRIPT_INCLUDE_ROOT   "${JOY_INCLUDE_ROOT}")
set(JOY_SCRIPT_EXTRA_INCLUDES "${JOY_INCLUDE_ROOT}")
set(JOY_MSGPACK_DIR           "${JOY_ROOT}/msgpack")

foreach(_joy_mod IN ITEMS lua js cs)
    set(_joy_mod_dir "${JOY_ROOT}/modules/${_joy_mod}")
    if(EXISTS "${_joy_mod_dir}/CMakeLists.txt" AND NOT TARGET joy_module_${_joy_mod})
        add_subdirectory("${_joy_mod_dir}" "${CMAKE_BINARY_DIR}/joy_modules/${_joy_mod}")
    endif()
    if(TARGET joy_module_${_joy_mod})
        target_link_libraries(joy_all INTERFACE joy_module_${_joy_mod})
    endif()
endforeach()

# ------------------------------------------------------------ IDE 源码分组
# joy::all 用 INTERFACE 源码注入，joy 的 .c 会直接编进消费方 target。
# source_group 是「目录作用域」的，模块目录里定义的规则传不到消费方，
# 结果 VS 里几十个 joy 源码全平铺在 Source Files 下。
#
# 消费方在**创建自己 target 的那个目录**调用一次 joy_ide_source_groups()，
# 即按 joy 仓真实目录树（src/ api/ external/ modules/...）生成筛选器。
# 父目录调用不会传到子目录，必须每个定义 target 的目录各调一次。
set_property(GLOBAL PROPERTY JOY_ROOT_DIR "${JOY_ROOT}")

function(_joy_collect_interface_sources _target)
    get_property(_seen GLOBAL PROPERTY _JOY_IDE_SEEN)
    if(_target IN_LIST _seen)
        return()
    endif()
    set_property(GLOBAL APPEND PROPERTY _JOY_IDE_SEEN "${_target}")

    get_target_property(_srcs "${_target}" INTERFACE_SOURCES)
    if(_srcs)
        set_property(GLOBAL APPEND PROPERTY _JOY_IDE_FILES ${_srcs})
    endif()
    get_target_property(_deps "${_target}" INTERFACE_LINK_LIBRARIES)
    if(_deps)
        foreach(_dep IN LISTS _deps)
            if(TARGET "${_dep}")
                _joy_collect_interface_sources("${_dep}")
            endif()
        endforeach()
    endif()
endfunction()

function(joy_ide_source_groups)
    # 用全局属性取 joy 仓根：本函数可能在调用方目录执行，那里不一定有
    # JOY_ROOT（add_subdirectory 的普通变量不会传回父目录）。
    get_property(_joy_tree GLOBAL PROPERTY JOY_ROOT_DIR)
    if(NOT _joy_tree)
        get_filename_component(_joy_tree "${JOY_ROOT}" ABSOLUTE)
    endif()

    set_property(GLOBAL PROPERTY _JOY_IDE_SEEN "")
    set_property(GLOBAL PROPERTY _JOY_IDE_FILES "")
    _joy_collect_interface_sources(joy_all)

    get_property(_files GLOBAL PROPERTY _JOY_IDE_FILES)
    if(NOT _files)
        return()
    endif()
    # 只保留 joy 仓内的真实文件：source_group(TREE) 要求 FILES 都位于 TREE 根下，
    # 生成器表达式和仓外路径必须剔除。
    set(_under_root "")
    foreach(_f IN LISTS _files)
        string(FIND "${_f}" "$<" _genex)
        if(NOT _genex EQUAL -1)
            continue()
        endif()
        get_filename_component(_abs "${_f}" ABSOLUTE)
        file(RELATIVE_PATH _rel "${_joy_tree}" "${_abs}")
        if(_rel MATCHES "^\\.\\." OR _rel MATCHES "^[A-Za-z]:")
            continue()
        endif()
        list(APPEND _under_root "${_abs}")
    endforeach()
    if(_under_root)
        list(REMOVE_DUPLICATES _under_root)
        # 统一收在 joy 节点下：joy 仓自己的 src/ 会占用顶层 src 筛选器，
        # 那个名字要留给宿主真正的游戏代码（joy\src / joy\api / ...）。
        source_group(TREE "${_joy_tree}" PREFIX "joy" FILES ${_under_root})
    endif()
endfunction()
