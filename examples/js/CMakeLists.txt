# JS 版：宿主一百来行，游戏逻辑在 src/main.js。
if(NOT TARGET joy_module_js)
    message(WARNING "example_js 已跳过：本次配置没有启用 joy 的 js 模块")
    return()
endif()

# 注入的 joy 引擎源码收在 joy\... 下（source_group 只在当前目录生效）
joy_ide_source_groups()

# 本示例真正的游戏代码在真实 src/ 目录里；脚本不参与编译，但要在 VS 中可见。
set(_game_sources
    "${CMAKE_CURRENT_LIST_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/main.js")
set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/src/main.js"
    PROPERTIES HEADER_FILE_ONLY TRUE)
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${_game_sources})

add_executable(example_js ${_game_sources})
target_link_libraries(example_js PRIVATE joy::all)

add_custom_command(TARGET example_js POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${JOY_EXAMPLE_ASSETS}" "$<TARGET_FILE_DIR:example_js>/assets"
    # 生成物统一落进 assets/：CI（含 Android APK）只收录这个资源根，
    # 桌面与移动端保持同一相对布局，宿主按 assets/scripts/main.js 加载。
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:example_js>/assets/scripts"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_LIST_DIR}/src/main.js"
            "$<TARGET_FILE_DIR:example_js>/assets/scripts/main.js"
    COMMENT "Copying js example assets + scripts")

# joy::all 含 cs 模块时 exe 会导入 coreclr.dll，必须一起部署
joy_examples_deploy_clr(example_js)

add_test(NAME example_js_smoke COMMAND example_js --smoke 180)
set_tests_properties(example_js_smoke PROPERTIES
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:example_js>"
    ENVIRONMENT "SDL_VIDEODRIVER=dummy")
