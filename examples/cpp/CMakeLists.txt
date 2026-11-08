# ============================================================
# C++ 版：薄宿主 + 游戏 DLL（native plugin 试点）
#
#   example_cpp  链接 joy::all，负责 SDL 主循环、加载 DLL、填 JoyApi 表并注入
#   JoyGameCpp   游戏逻辑（窗口 / 场景 / 物理 / 绘制），只通过 JoyApi 表调引擎
#
# 关键约束：JoyGameCpp 绝不链接 joy::all。joy::all 是 INTERFACE 源码注入，
# 一旦链上就会把整个引擎再编进 DLL，出现两套全局状态和两套 joy_obj 句柄表。
# DLL 只 include 契约头（纯 C POD），不 include joy.h、不链 SDL 库。
#
# 单一导出入口 joy_game_get_hooks：宿主用 SDL_LoadFunction 取，不需要 .lib /
# __declspec(dllimport)，DLL 也不反向链接宿主。
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

# ---------------- 薄宿主（唯一链接 joy::all 的一方） ----------------
add_executable(example_cpp ${_host} ${_contract} ${_vtable_h})
target_link_libraries(example_cpp PRIVATE joy::all)
target_include_directories(example_cpp PRIVATE
    "${JOY_ROOT}/api"
    "${CMAKE_CURRENT_LIST_DIR}/src")

# ---------------- 游戏 DLL ----------------
add_library(JoyGameCpp SHARED ${_game} ${_contract} ${_vtable_h})
target_include_directories(JoyGameCpp PRIVATE
    "${JOY_ROOT}/api"                  # generated/joy_api_vtable.h
    "${CMAKE_CURRENT_LIST_DIR}/src")  # joy_game_hooks.h
# /utf-8 原本由 joy_all 的 INTERFACE 编译选项传给消费方；DLL 刻意不链
# joy::all，必须自己补上，否则 MSVC 按 GBK(936) 解析 UTF-8 源码，中文字符串
# 会触发 C4819 / C3688 并把字符串截断成语法错误。
if(MSVC)
    target_compile_options(JoyGameCpp PRIVATE /utf-8)
endif()

# 生成物统一落进 assets/：CI（含 Android APK 的资产收集）只认这个资源根。
# 用 CMAKE_BINARY_DIR/$<CONFIG> 而不是 $<TARGET_FILE_DIR:example_cpp>，
# 避免在 JoyGameCpp 的 POST_BUILD 里反向引用宿主 target 形成依赖环。
add_custom_command(TARGET JoyGameCpp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_BINARY_DIR}/$<CONFIG>/assets/cpp"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:JoyGameCpp>"
            "${CMAKE_BINARY_DIR}/$<CONFIG>/assets/cpp/$<TARGET_FILE_NAME:JoyGameCpp>"
    COMMENT "Deploying game DLL into assets/cpp/")
add_dependencies(example_cpp JoyGameCpp)

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
