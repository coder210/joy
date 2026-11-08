# C# 版：宿主只做窗口/主循环/事件泵，游戏逻辑在 JoyGame.dll。
# 需要 joy 的 cs 模块（JOY_REQUESTED_MODULES 含 cs）与 CLR 开发包。
if(NOT TARGET joy_module_cs)
    message(WARNING "example_csharp 已跳过：本次配置没有启用 joy 的 cs 模块")
    return()
endif()

# 注入的 joy 引擎源码收在 joy\... 下（source_group 只在当前目录生效）
joy_ide_source_groups()

# 宿主代码在真实 src/ 目录里；C# 游戏逻辑由 JoyGame.csproj 的 game/ runtime/ 提供
set(_game_sources "${CMAKE_CURRENT_LIST_DIR}/src/main.cpp")
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${_game_sources})

add_executable(example_csharp ${_game_sources})
target_link_libraries(example_csharp PRIVATE joy::all)

# ---- C# 程序集：dotnet build 产出 JoyGame.dll + Joy.Bindings.dll ----
set(_cs_out  "${CMAKE_BINARY_DIR}/csharp")
set(_cs_proj "${CMAKE_CURRENT_LIST_DIR}/JoyGame.csproj")

add_custom_target(example_csharp_managed
    COMMAND dotnet build "${_cs_proj}" -c $<IF:$<CONFIG:Debug>,Debug,Release> -o "${_cs_out}"
    BYPRODUCTS
        "${_cs_out}/JoyGame.dll"
        "${_cs_out}/Joy.Bindings.dll"
    COMMENT "dotnet build examples/csharp -> JoyGame.dll"
    VERBATIM)
add_dependencies(example_csharp example_csharp_managed)

# 把 C# 项目挂进 VS 解决方案，否则 IDE 里只看得到一条 dotnet build
# 命令，game/ runtime/ 的 .cs 无法打开。
#
# Joy.Bindings.csproj 必须一起挂进去：JoyGame.csproj 通过 ProjectReference
# 引用它，而 VS 的解决方案级 NuGet restore 只包含 .sln 里的项目。缺了它，
# 还原时找不到引用目标的项目信息，会报「找不到 Joy.Bindings.csproj 的项目
# 信息…该项目已被卸载或不属于当前解决方案」(NU1105)，assets.json 里退化为
# bin/placeholder/Joy.Bindings.dll。命令行 dotnet build 会递归遍历
# ProjectReference，所以这个错只在 VS 里出现。
#
# include_external_msproject 的项目总会参与「生成解决方案」（EXCLUDE_FROM_ALL
# 对它无效），因此三个 C# 构建点（JoyBindings / JoyGame / dotnet build 命令）
# 必须用 add_dependencies 串成一条链：避免 VS 与 dotnet build 两个进程同时
# 写同一 csproj 的 obj/，最后一次是增量的，只负责把产物补到 -o 指定的
# _cs_out，POST_BUILD 拷贝路径保持不变。
if(CMAKE_GENERATOR MATCHES "Visual Studio")
    include_external_msproject(JoyBindings
        "${JOY_MODULE_CS_DIR}/Joy.Bindings.csproj" PLATFORM "Any CPU")
    include_external_msproject(JoyGame "${_cs_proj}" PLATFORM "Any CPU")
    add_dependencies(JoyGame JoyBindings)
    add_dependencies(example_csharp_managed JoyGame)
endif()

# ---- 托管运行时文件：全部并入 assets/，CI 与 Android APK 只收录这个资源根 ----
# coreclr.dll 是原生库，由系统加载器从 exe 旁查找，必须留在根目录（见下方部署函数）。
get_filename_component(_clr_bin "${MONO_LIBRARIES}" DIRECTORY)
set(_clr_libs "${JOY_ROOT}/runtimes/libs")
if(EXISTS "${_clr_bin}/libs/System.Private.CoreLib.dll")
    set(_clr_libs "${_clr_bin}/libs")
endif()

set(_out_dir "$<TARGET_FILE_DIR:example_csharp>")

add_custom_command(TARGET example_csharp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${JOY_EXAMPLE_ASSETS}" "${_out_dir}/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_cs_out}/JoyGame.dll" "${_out_dir}/assets/JoyGame.dll"
    # 先整块同步 BCL，再把绑定程序集塞进 assets/lib/：
    # 自定义 CLR 只从 MONO_LIB_DIR 解析依赖程序集，宿主已把它指向 assets/lib。
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${_out_dir}/assets/lib"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${_clr_libs}" "${_out_dir}/assets/lib"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_cs_out}/Joy.Bindings.dll" "${_out_dir}/assets/lib/Joy.Bindings.dll"
    COMMENT "Copying C# example runtime (assets/JoyGame.dll + assets/lib/)")

# coreclr.dll 走共享部署函数：另外三个示例同样导入它（joy::all 含 cs 模块）
joy_examples_deploy_clr(example_csharp)

enable_testing()
add_test(NAME example_csharp_smoke COMMAND example_csharp --smoke 180)
set_tests_properties(example_csharp_smoke PROPERTIES
    WORKING_DIRECTORY "${_out_dir}"
    ENVIRONMENT "SDL_VIDEODRIVER=dummy")
