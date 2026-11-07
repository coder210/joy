# JS 版：宿主一百来行，游戏逻辑在 scripts/main.js。
if(NOT TARGET joy_module_js)
    message(WARNING "example_js 已跳过：本次配置没有启用 joy 的 js 模块")
    return()
endif()

add_executable(example_js main.cpp)
target_link_libraries(example_js PRIVATE joy::all)

add_custom_command(TARGET example_js POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${JOY_EXAMPLE_ASSETS}" "$<TARGET_FILE_DIR:example_js>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_LIST_DIR}/scripts" "$<TARGET_FILE_DIR:example_js>/scripts"
    COMMENT "Copying js example assets + scripts")

# joy::all 含 cs 模块时 exe 会导入 coreclr.dll，必须一起部署
joy_examples_deploy_clr(example_js)

add_test(NAME example_js_smoke COMMAND example_js --smoke 180)
set_tests_properties(example_js_smoke PROPERTIES
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:example_js>"
    ENVIRONMENT "SDL_VIDEODRIVER=dummy")
