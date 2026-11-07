# Lua 版：宿主只有 100 多行，游戏逻辑在 scripts/main.lua。
if(NOT TARGET joy_module_lua)
    message(WARNING "example_lua 已跳过：本次配置没有启用 joy 的 lua 模块")
    return()
endif()

add_executable(example_lua main.cpp)
target_link_libraries(example_lua PRIVATE joy::all)

add_custom_command(TARGET example_lua POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${JOY_EXAMPLE_ASSETS}" "$<TARGET_FILE_DIR:example_lua>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_LIST_DIR}/scripts" "$<TARGET_FILE_DIR:example_lua>/scripts"
    COMMENT "Copying lua example assets + scripts")

add_test(NAME example_lua_smoke COMMAND example_lua --smoke 180)
set_tests_properties(example_lua_smoke PROPERTIES
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:example_lua>"
    ENVIRONMENT "SDL_VIDEODRIVER=dummy")
