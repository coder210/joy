# ============================================================
# joy 四语言示例的共享构建入口
#
# 四个 target：example_cpp / example_csharp / example_lua / example_js
# 功能完全相同（见 README.md），只是业务逻辑分别用 C++ / C# / Lua / JS 写。
#
# 两种用法（是否启用由 joy 仓根 CMakeLists.txt 的 JOY_BUILD_EXAMPLES 决定）：
#   1) 从 joy 仓根构建：-DJOY_BUILD_EXAMPLES=ON，根 CMakeLists 会
#      add_subdirectory(examples)，此时 joy::all 已存在。
#   2) 独立构建：cmake -S examples -B build/examples，本文件会自行把 joy 与
#      三个第三方拉进来（不经过总开关，默认就构建全部示例）。
# ============================================================
cmake_minimum_required(VERSION 3.16)
project(joy_examples C CXX)

# 标记「examples 已被引入」：独立构建时本文件是顶层，随后会 add_subdirectory
# (joy 根)；根 CMakeLists 靠这个标记避免再次 add_subdirectory(examples)。
set(JOY_EXAMPLES_ACTIVE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

# Linux/macOS 上 SDL3 是共享库（LIBRARY 目标），默认落在 build/examples/SDL/，
# 而打包只收 build/examples/Release/；解压到别的机器就会报
# "libSDL3.so.0: cannot open shared object file"。让 LIBRARY 与 RUNTIME 同目录。
# Android 的 libmain.so / lib/<abi>/ 布局已由打包脚本按 find 收集，保持不变。
if(NOT ANDROID)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
endif()

# 打包目录要能整体搬走：所有 ELF/Mach-O 都按自身所在目录解析依赖库。
# 这段 rpath 原先只写在 joy_examples_deploy_clr() 里（CLR 可用时才生效），
# 但 SDL3 shared 无论开不开 cs 模块都需要它。
if(APPLE)
    set(CMAKE_BUILD_RPATH "@loader_path")
elseif(UNIX AND NOT ANDROID)
    set(CMAKE_BUILD_RPATH "\$ORIGIN")
endif()

set(JOY_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." CACHE PATH "joy 仓根目录")

if(NOT TARGET joy::all)
    add_subdirectory("${JOY_ROOT}/msgpack" "${CMAKE_BINARY_DIR}/msgpack")
    add_subdirectory("${JOY_ROOT}/spine"   "${CMAKE_BINARY_DIR}/spine")
    add_subdirectory("${JOY_ROOT}/SDL"     "${CMAKE_BINARY_DIR}/SDL")

    # ---- C# 示例需要的 CLR 开发包：默认取 joy 仓自带的 runtimes/ ----
    # 缺失时只关掉 cs 模块（csharp 示例不生成），不让 lua / js / cpp 一起失败。
    if(NOT MONO_INCLUDE_DIR OR NOT MONO_LIBRARIES)
        set(_clr_root "${JOY_ROOT}/runtimes")
        # 平台目录名可用 -DJOY_CLR_PLATFORM=<目录名> 覆盖（见 runtimes/README.md）。
        # Android 按 ABI 映射；官方 MonoVM runtime pack 的原生库在 Android 上是
        # libmonosgen-2.0.so，Windows 包里则沿用 coreclr.dll 这个名字。
        if(NOT JOY_CLR_PLATFORM)
            if(WIN32)
                set(JOY_CLR_PLATFORM "windows-x64")
            elseif(ANDROID)
                if(ANDROID_ABI STREQUAL "arm64-v8a")
                    set(JOY_CLR_PLATFORM "android-arm64")
                elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
                    set(JOY_CLR_PLATFORM "android-armv7")
                elseif(ANDROID_ABI STREQUAL "x86_64")
                    set(JOY_CLR_PLATFORM "android-x64")
                else()
                    set(JOY_CLR_PLATFORM "android-${ANDROID_ABI}")
                endif()
            elseif(APPLE)
                set(JOY_CLR_PLATFORM "macos-arm64")
            elseif(EMSCRIPTEN)
                # MonoVM 没有 wasm 运行时；让下面的 CLR 库检查落空，只编 cpp/lua/js。
                set(JOY_CLR_PLATFORM "unsupported-wasm")
            else()
                set(JOY_CLR_PLATFORM "linux-x64")
            endif()
        endif()
        set(_clr_plat "${_clr_root}/${JOY_CLR_PLATFORM}")
        if(EXISTS "${_clr_plat}/include/mono-2.0")
            set(MONO_INCLUDE_DIR "${_clr_plat}/include/mono-2.0")
        elseif(EXISTS "${_clr_root}/include/mono-2.0")
            set(MONO_INCLUDE_DIR "${_clr_root}/include/mono-2.0")
        endif()
        if(WIN32 AND EXISTS "${_clr_plat}/coreclr.import.lib")
            set(MONO_LIBRARIES "${_clr_plat}/coreclr.import.lib")
        elseif(EXISTS "${_clr_plat}/libmonosgen-2.0.so")
            set(MONO_LIBRARIES "${_clr_plat}/libmonosgen-2.0.so")
        elseif(EXISTS "${_clr_plat}/libcoreclr.so")
            set(MONO_LIBRARIES "${_clr_plat}/libcoreclr.so")
        elseif(EXISTS "${_clr_plat}/libcoreclr.dylib")
            # macOS runtime pack 的主库是 .dylib（install name 为 @rpath/...）
            set(MONO_LIBRARIES "${_clr_plat}/libcoreclr.dylib")
        elseif(EXISTS "${_clr_plat}/libmonosgen-2.0.dylib")
            set(MONO_LIBRARIES "${_clr_plat}/libmonosgen-2.0.dylib")
        endif()
    endif()

    if(MONO_INCLUDE_DIR AND MONO_LIBRARIES AND EXISTS "${MONO_INCLUDE_DIR}")
        set(_joy_modules cs lua js)
        message(STATUS "joy examples: 开启 cs / lua / js 三个后端")
    else()
        set(_joy_modules lua js)
        message(WARNING
            "joy examples: 未找到 CLR 开发包（MONO_INCLUDE_DIR / MONO_LIBRARIES），"
            "本次只构建 cpp / lua / js 三个示例；要用 csharp 示例请设 "
            "-DMONO_INCLUDE_DIR=<.../include/mono-2.0> -DMONO_LIBRARIES=<.../coreclr.import.lib>")
    endif()
    set_property(GLOBAL PROPERTY JOY_REQUESTED_MODULES ${_joy_modules})
    set(JOY_EXAMPLES_CLR_OK ${_joy_modules} MATCHES "cs")
    add_subdirectory("${JOY_ROOT}" "${CMAKE_BINARY_DIR}/joy")
endif()

# 四份实现共用的资源（图片 / 字体）。缺失时各示例走纯色 + 无文字退化路径。
# 这是「源码资源根」；下面的 JOY_EXAMPLE_ASSET_ROOT 是「运行期资源根」。
set(JOY_EXAMPLE_ASSETS "${CMAKE_CURRENT_LIST_DIR}/assets")

# 运行期资源根：桌面与 exe 同目录，Android 收进独立的 pack/ 供 APK 打包。
# 代码里的资源路径一律不带 "assets/" 前缀（与 joy_topdown_template 一致）：
# Android 的 SDL_IOFromFile 会把路径原样交给 AAssetManager，而 AAssetManager
# 的路径已经相对 APK 的 assets/ 根，写成 "assets/foo" 会去找
# "assets/assets/foo"；桌面则靠 chdir 到 exe 目录解析同一套相对路径。
if(ANDROID)
    set(JOY_EXAMPLE_ASSET_ROOT "${CMAKE_BINARY_DIR}/pack")
else()
    set(JOY_EXAMPLE_ASSET_ROOT "${CMAKE_BINARY_DIR}/$<CONFIG>")
endif()

# ---- 部署 coreclr.dll（各子目录调用；必须在 add_subdirectory 之前定义） ----
# joy::all 是「所有已启用模块的源码合集」，只要开了 cs 模块，**每个**示例的
# 导入表里都会有 coreclr.dll 的符号（mono_free 等），哪怕 cpp/lua/js 从不调用
# csx_*。只给 example_csharp 部署时，单独构建 example_js 会报
# 「无法定位程序输入点 mono_free 于动态链接库 example_js.exe 上」。
# BCL(lib/) 不需要跟着走：不走 CLR 初始化的进程不会加载它。
#
# 必须做成函数让子目录自己调用：add_custom_command(TARGET) 只能在创建该
# target 的同一目录里调用，父目录调用会报
# "TARGET 'example_xxx' was not created in this directory"。
function(joy_examples_deploy_clr _target)
    if(NOT JOY_EXAMPLES_CLR_OK OR NOT MONO_LIBRARIES)
        return()
    endif()
    get_filename_component(_clr_bin "${MONO_LIBRARIES}" DIRECTORY)
    get_filename_component(_clr_file "${MONO_LIBRARIES}" NAME)

    if(ANDROID)
        # APK 只把 lib/<abi>/*.so 解压成可执行文件；assets/ 是压缩数据，
        # 系统加载器找不到。产物落进构建树的 lib/<abi>/ 供打包工程收集。
        set(_apk_lib "${CMAKE_BINARY_DIR}/lib/${ANDROID_ABI}")
        add_custom_command(TARGET ${_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_apk_lib}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${MONO_LIBRARIES}" "${_apk_lib}/${_clr_file}"
            COMMENT "Deploying ${_clr_file} into APK lib/${ANDROID_ABI}/")
    elseif(EXISTS "${_clr_bin}/coreclr.dll")
        add_custom_command(TARGET ${_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${_clr_bin}/coreclr.dll"
                    "$<TARGET_FILE_DIR:${_target}>/coreclr.dll"
            COMMENT "Deploying coreclr.dll for ${_target} (joy::all 含 cs 模块)")
    elseif(UNIX)
        # Linux/macOS runtime pack 把主库与 libSystem.* shim 拆成多个文件；
        # 主库无 DT_RUNPATH：exe 靠下面的 BUILD_RPATH 找主库，shim 由宿主在
        # csx_create() 之前按绝对路径预加载（见 csharp_host/src/main.cpp）。
        file(GLOB _clr_native_libs
            "${_clr_bin}/libcoreclr.so" "${_clr_bin}/libcoreclr.dylib"
            "${_clr_bin}/libmonosgen-2.0.so" "${_clr_bin}/libmonosgen-2.0.dylib"
            "${_clr_bin}/libSystem.*.so" "${_clr_bin}/libSystem.*.dylib")
        list(LENGTH _clr_native_libs _clr_native_count)
        if(_clr_native_count GREATER 0)
            add_custom_command(TARGET ${_target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${_clr_native_libs} "$<TARGET_FILE_DIR:${_target}>"
                COMMENT "Deploying MonoVM native libs for ${_target}")
        endif()
        # joy::all 含 cs 模块，四个示例的导入表里都有 mono_*，exe 必须能找到主库。
        if(APPLE)
            set_property(TARGET ${_target} APPEND PROPERTY BUILD_RPATH "@loader_path")
            set_property(TARGET ${_target} APPEND PROPERTY INSTALL_RPATH "@loader_path")
        else()
            set_property(TARGET ${_target} APPEND PROPERTY BUILD_RPATH "\$ORIGIN")
            set_property(TARGET ${_target} APPEND PROPERTY INSTALL_RPATH "\$ORIGIN")
        endif()
    endif()
endfunction()

# 每个子目录都会定义自己的 target 与一次资源拷贝。
# IDE 分组 joy_ide_source_groups() 由各子目录自己调用：source_group 只在
# 「创建 target 的那个目录」生效，父目录定义不会传到子目录。
# csharp/ 是纯 C# 项目目录（JoyGame.csproj），不参与 add_subdirectory；
# 它的 native 宿主与 example_csharp target 定义在 csharp_host/。
enable_testing()
foreach(_lang cpp lua js csharp_host)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${_lang}/CMakeLists.txt")
        add_subdirectory(${_lang})
    endif()
endforeach()
