# C++ 版：没有脚本层，窗口 / 主循环 / 游戏逻辑全在一个 main.cpp 里。
# 只依赖 joy::all（引擎本身），不依赖任何脚本后端模块。
add_executable(example_cpp main.cpp)
target_link_libraries(example_cpp PRIVATE joy::all)

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
