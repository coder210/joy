# ============================================================
# C++ 版：薄宿主 + 游戏代码（native plugin 模板）
#
#   example_cpp  链接 joy::all，负责 SDL 主循环、填 JoyApi 表并注入游戏侧
#   game.cpp     游戏逻辑（窗口 / 场景 / 物理 / 绘制），只通过 JoyApi 表调引擎
#
# 两种构建形态，由 JOY_GAME_STATIC 决定（iOS / wasm 默认开启）：
#   动态（桌面 / Android）：game.cpp 编成独立共享库，宿主运行时 SDL_LoadObject
#   静态（iOS / wasm）：    game.cpp 直接编进宿主，链接期解析 joy_game_get_hooks
#
# 平台依据：
#   Android — SDL_LoadObject 可用（SDL 走 dlopen），但 .so 必须进 APK 的
#             lib/<abi>/ 由系统解压；压缩的 assets/ 里 dlopen 打不开。
#   iOS     — 运行时只能加载构建期签名并随包的代码，普通 dylib 不可。
#   wasm    — dlopen 需要 MAIN_MODULE/SIDE_MODULE + 虚拟 FS 预加载。
# 后两者默认走静态分支，避免把签名 / 动态链接配置带进模板。
#
# 关键约束：游戏代码绝不链接 joy::all。joy::all 是 INTERFACE 源码注入，
# 一旦链上就会把整个引擎再编一份，出现两套全局状态和两套 joy_obj 句柄表。
# 游戏侧只 include 契约头（纯 C POD），不 include joy.h、不链 SDL 库。
# 两种形态都靠 JoyApi 函数指针表调引擎，因此 game.cpp 一行都不用改。
#
# 单一入口 joy_game_get_hooks：动态形态由 SDL_LoadFunction 取，不需要 .lib /
# __declspec(dllimport)；静态形态由链接器直接解析。
# ============================================================

# 注入的 joy 引擎源码收在 joy\... 下（source_group 只在当前目录生效）
joy_ide_source_groups()

set(_host     "${CMAKE_CURRENT_LIST_DIR}/src/host.cpp")
set(_game     "${CMAKE_CURRENT_LIST_DIR}/src/game.cpp")
set(_contract "${CMAKE_CURRENT_LIST_DIR}/src/joy_game_hooks.h")
set(_vtable_h "${JOY_ROOT}/api/generated/joy_api_vtable.h")
set_source_files_properties(${_contract} ${_vtable_h} PROPERTIES HEADER_FILE_ONLY TRUE)

# 本示例真正的代码在真实 src/ 目录里；引擎源码由 joy_ide_source_groups() 归到 joy\...
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${_host} ${_game} ${_contract})
source_group("joy\\api\\generated" FILES ${_vtable_h})

# ---------------- 构建形态开关 ----------------
# iOS / wasm 默认静态；其它平台默认动态。新项目可用 -DJOY_GAME_STATIC=ON/OFF
# 覆盖，两种形态共用同一份 game.cpp 与契约头。
set(_static_default OFF)
if(IOS OR EMSCRIPTEN)
    set(_static_default ON)
endif()
option(JOY_GAME_STATIC
       "把 game.cpp 直接编进 example_cpp（iOS/wasm 默认开启）" ${_static_default})

# ---------------- 薄宿主（唯一链接 joy::all 的一方） ----------------
if(JOY_GAME_STATIC)
    set(_host_sources ${_host} ${_game} ${_contract} ${_vtable_h})
else()
    set(_host_sources ${_host} ${_contract} ${_vtable_h})
endif()

add_executable(example_cpp ${_host_sources})
target_link_libraries(example_cpp PRIVATE joy::all)
target_include_directories(example_cpp PRIVATE
    "${JOY_ROOT}/api"
    "${CMAKE_CURRENT_LIST_DIR}/src")
if(JOY_GAME_STATIC)
    target_compile_definitions(example_cpp PRIVATE JOY_GAME_STATIC=1)
endif()

# ---------------- 游戏共享库（仅动态形态） ----------------
if(NOT JOY_GAME_STATIC)
    add_library(JoyGameCpp SHARED ${_game} ${_contract} ${_vtable_h})
    target_include_directories(JoyGameCpp PRIVATE
        "${JOY_ROOT}/api"                  # generated/joy_api_vtable.h
        "${CMAKE_CURRENT_LIST_DIR}/src")  # joy_game_hooks.h
    # /utf-8 原本由 joy_all 的 INTERFACE 编译选项传给消费方；游戏库刻意不链
    # joy::all，必须自己补上，否则 MSVC 按 GBK(936) 解析 UTF-8 源码，中文字符串
    # 会触发 C4819 / C3688 并把字符串截断成语法错误。
    if(MSVC)
        target_compile_options(JoyGameCpp PRIVATE /utf-8)
    endif()

    if(ANDROID)
        # APK 只把 lib/<abi>/*.so 解压成可执行文件；assets/ 是压缩数据，
        # dlopen 打不开。产物直接落进 lib/<abi>/ 供打包工程收集，
        # 宿主用裸库名让系统 linker 查找（见 host.cpp 的 __ANDROID__ 分支）。
        set(_abi "${CMAKE_ANDROID_ARCH_ABI}")
        if(NOT _abi)
            set(_abi "${ANDROID_ABI}")
        endif()
        set_target_properties(JoyGameCpp PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${_abi}")
    else()
        # 桌面：生成物统一落进 assets/，CI 只收录这个资源根。
        # 用 CMAKE_BINARY_DIR/$<CONFIG> 而不是 $<TARGET_FILE_DIR:example_cpp>，
        # 避免在 POST_BUILD 里反向引用宿主 target 形成依赖环。
        add_custom_command(TARGET JoyGameCpp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                    "${CMAKE_BINARY_DIR}/$<CONFIG>/assets/cpp"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:JoyGameCpp>"
                    "${CMAKE_BINARY_DIR}/$<CONFIG>/assets/cpp/$<TARGET_FILE_NAME:JoyGameCpp>"
            COMMENT "Deploying game DLL into assets/cpp/")
    endif()
    add_dependencies(example_cpp JoyGameCpp)
endif()

# 资源目录按相对路径加载，和 exe 放一起
add_custom_command(TARGET example_cpp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${JOY_EXAMPLE_ASSETS}" "$<TARGET_FILE_DIR:example_cpp>/assets"
    COMMENT "Copying shared assets for example_cpp")

# joy::all 含 cs 模块时 exe 会导入 coreclr.dll，必须一起部署
joy_examples_deploy_clr(example_cpp)

add_test(NAME example_cpp_smoke COMMAND example_cpp --smoke 180)
set_tests_properties(example_cpp_smoke PROPERTIES
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:example_cpp>"
    ENVIRONMENT "SDL_VIDEODRIVER=dummy")
