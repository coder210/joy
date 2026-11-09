# Lua 版：宿主只有 100 多行，游戏逻辑在 src/main.lua。
if(NOT TARGET joy_module_lua)
    message(WARNING "example_lua 已跳过：本次配置没有启用 joy 的 lua 模块")
    return()
endif()

# 注入的 joy 引擎源码收在 joy\... 下（source_group 只在当前目录生效）
joy_ide_source_groups()

# 本示例真正的游戏代码在真实 src/ 目录里；脚本不参与编译，但要在 VS 中可见。
set(_game_sources
    "${CMAKE_CURRENT_LIST_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/main.lua")
set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/src/main.lua"
    PROPERTIES HEADER_FILE_ONLY TRUE)
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${_game_sources})

if(ANDROID)
    # Android 由 SDLActivity 加载 libmain.so；四个后端各出一个，见 cpp/ 注释。
    add_library(example_lua SHARED ${_game_sources})
    set_target_properties(example_lua PROPERTIES OUTPUT_NAME "main")
else()
    add_executable(example_lua ${_game_sources})
endif()
target_link_libraries(example_lua PRIVATE joy::all)

add_custom_command(TARGET example_lua POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${JOY_EXAMPLE_ASSETS}" "${JOY_EXAMPLE_ASSET_ROOT}"
    # 资源根内容直接铺开：桌面是 exe 同目录，Android 是 APK 的 assets 根，
    # 两者相对布局一致，宿主按 scripts/main.lua 加载（不带 assets/ 前缀）。
    COMMAND ${CMAKE_COMMAND} -E make_directory "${JOY_EXAMPLE_ASSET_ROOT}/scripts"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_LIST_DIR}/src/main.lua"
            "${JOY_EXAMPLE_ASSET_ROOT}/scripts/main.lua"
    COMMENT "Copying lua example assets + scripts")

# joy::all 含 cs 模块时 exe 会导入 coreclr.dll，必须一起部署
joy_examples_deploy_clr(example_lua)

add_test(NAME example_lua_smoke COMMAND example_lua --smoke 180)
set_tests_properties(example_lua_smoke PROPERTIES
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:example_lua>"
    ENVIRONMENT "SDL_VIDEODRIVER=dummy")
