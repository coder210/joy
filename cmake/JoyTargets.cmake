# ============================================================
# joy 的构建契约：源码注入，唯一入口 joy::all
#
# 故意不建静态库。公开头里有编译期可配置的 inline 契约：
#   include/joy/defs.h 的 PIXELS_PER_METER（#ifndef 可覆盖）与
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

option(JOY_ENABLE_GLTF   "编译 jgltf.c（默认关闭：文件整体被 JOY_USE_GLTF 包裹）" OFF)
option(JOY_ENABLE_SPINE  "编译 jspine.c 并链接 spine-c" OFF)
option(JOY_ENABLE_PLUGINS "编译 jmodule.c 插件系统（SDL_LoadObject）" OFF)
# P4 迁移完成前必须保持 ON：旧业务仍调用 Joy.Draw / Input / Host / Image / Font。
option(JOY_ENABLE_LEGACY_BINDINGS "编译 bindings/legacy/ 旧手写胶水" ON)

include("${CMAKE_CURRENT_LIST_DIR}/JoySources.cmake")
joy_collect_sources(_joy_sources)

# api/ 的运行层与生成 thunk：调用 joy.h 里的 C 函数，必须与 joy 源码同 target。
set(_joy_api_sources
    "${JOY_ROOT}/api/runtime/joy_value.c"
    "${JOY_ROOT}/api/runtime/joy_obj.c"
    "${JOY_ROOT}/api/runtime/joy_thunk.c"
    "${JOY_ROOT}/api/generated/thunks_generated.c")

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
