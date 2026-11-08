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

add_executable(example_lua ${_game_sources})
target_link_libraries(example_lua PRIVATE joy::all)

add_custom_command(TARGET example_lua POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${JOY_EXAMPLE_ASSETS}" "$<TARGET_FILE_DIR:example_lua>/assets"
    # 生成物统一落进 assets/：CI（含 Android APK）只收录这个资源根，
    # 桌面与移动端保持同一相对布局，宿主按 assets/scripts/main.lua 加载。
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:example_lua>/assets/scripts"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_LIST_DIR}/src/main.lua"
            "$<TARGET_FILE_DIR:example_lua>/assets/scripts/main.lua"
    COMMENT "Copying lua example assets + scripts")

# joy::all 含 cs 模块时 exe 会导入 coreclr.dll，必须一起部署
joy_examples_deploy_clr(example_lua)

add_test(NAME example_lua_smoke COMMAND example_lua --smoke 180)
set_tests_properties(example_lua_smoke PROPERTIES
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:example_lua>"
    ENVIRONMENT "SDL_VIDEODRIVER=dummy")
