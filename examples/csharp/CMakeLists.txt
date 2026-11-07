# C# 版：宿主只做窗口/主循环/事件泵，游戏逻辑在 JoyGame.dll。
# 需要 joy 的 cs 模块（JOY_REQUESTED_MODULES 含 cs）与 CLR 开发包。
if(NOT TARGET joy_module_cs)
    message(WARNING "example_csharp 已跳过：本次配置没有启用 joy 的 cs 模块")
    return()
endif()

add_executable(example_csharp main.cpp)
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

# ---- CLR 运行时文件：coreclr.dll 与 BCL(lib/) 必须与 exe 同目录 ----
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
            "${_cs_out}/JoyGame.dll" "${_out_dir}/JoyGame.dll"
    # 先整块同步 BCL，再把绑定程序集塞进 lib/：
    # 自定义 CLR 只从 MONO_LIB_DIR(lib/) 解析依赖程序集，放 exe 旁无效。
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${_out_dir}/lib"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${_clr_libs}" "${_out_dir}/lib"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_cs_out}/Joy.Bindings.dll" "${_out_dir}/lib/Joy.Bindings.dll"
    COMMENT "Copying C# example runtime (JoyGame.dll + CLR lib/)")

if(EXISTS "${_clr_bin}/coreclr.dll")
    add_custom_command(TARGET example_csharp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_clr_bin}/coreclr.dll" "${_out_dir}/coreclr.dll"
        COMMENT "Copying coreclr.dll")
else()
    message(WARNING "未找到 ${_clr_bin}/coreclr.dll：example_csharp 运行时会缺运行时")
endif()

enable_testing()
add_test(NAME example_csharp_smoke COMMAND example_csharp --smoke 180)
set_tests_properties(example_csharp_smoke PROPERTIES
    WORKING_DIRECTORY "${_out_dir}"
    ENVIRONMENT "SDL_VIDEODRIVER=dummy")
