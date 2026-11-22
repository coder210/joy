if(EMSCRIPTEN)
    # MonoVM 没有 wasm 运行时（iOS 禁 JIT、wasm 需专门 runtime）；
    # Web 只构建 cpp/lua/js 三个后端，这里不定义 example_csharp target。
    message(STATUS "joy examples: EMSCRIPTEN 跳过 csharp（MonoVM 无 wasm 运行时）")
    return()
endif()

# C# 示例的 native 宿主：只做窗口/主循环/事件泵，游戏逻辑在 ../csharp/JoyGame.csproj
# 产出的 JoyGame.dll 里。这里刻意与 C# 项目分目录，避免 MSBuild 的默认 None
# glob 把 main.cpp / CMakeLists.txt 挂进 JoyGame 项目节点。
# 需要 joy 的 cs 模块（JOY_REQUESTED_MODULES 含 cs）与 CLR 开发包。
if(NOT TARGET joy_module_cs)
    message(WARNING "example_csharp 已跳过：本次配置没有启用 joy 的 cs 模块")
    return()
endif()

# 注入的 joy 引擎源码收在 joy\... 下（source_group 只在当前目录生效）
joy_ide_source_groups()

# 宿主代码在本目录 src/；C# 游戏逻辑在 ../csharp/ 的 JoyGame.csproj（game/ runtime/ test/）
set(_game_sources "${CMAKE_CURRENT_LIST_DIR}/src/main.cpp")
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${_game_sources})

if(ANDROID)
    # SDL3 Android 应用由 SDLActivity 加载 libmain.so，不是独立可执行文件；
    # SDL_AppInit 回调入口保持不变。
    add_library(example_csharp SHARED ${_game_sources})
    set_target_properties(example_csharp PROPERTIES OUTPUT_NAME "main")
else()
    add_executable(example_csharp ${_game_sources})
endif()
target_link_libraries(example_csharp PRIVATE joy::all)

# ---- C# 程序集：dotnet build 产出 JoyGame.dll + Joy.Bindings.dll ----
set(_cs_out  "${CMAKE_BINARY_DIR}/csharp")
# C# 项目在平级的 csharp/ 目录，本目录只放 native 宿主与构建入口。
set(_cs_proj "${CMAKE_CURRENT_LIST_DIR}/../csharp/JoyGame.csproj")

# 全新 checkout 没有 obj/（已取消跟踪）。VS 生成器下 include_external_msproject
# 会让 JoyGame/JoyBindings 参与命令行 MSBuild 构建，而命令行 MSBuild 不会隐式
# 做 NuGet restore，于是报 NETSDK1004 找不到 obj/project.assets.json。
# 显式 restore 并串进依赖链，保证 cmake --build / CI / IDE 都能从干净工作区构建；
# dotnet restore 会递归还原 ProjectReference 的 Joy.Bindings.csproj。
add_custom_target(example_csharp_restore
    COMMAND dotnet restore "${_cs_proj}"
    COMMENT "dotnet restore examples/csharp (+ Joy.Bindings)"
    VERBATIM)

add_custom_target(example_csharp_managed
    COMMAND dotnet build "${_cs_proj}" -c $<IF:$<CONFIG:Debug>,Debug,Release> -o "${_cs_out}"
    BYPRODUCTS
        "${_cs_out}/JoyGame.dll"
        "${_cs_out}/Joy.Bindings.dll"
    COMMENT "dotnet build examples/csharp -> JoyGame.dll"
    VERBATIM)
add_dependencies(example_csharp example_csharp_managed)
add_dependencies(example_csharp_managed example_csharp_restore)

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
    # 外部项目参与命令行构建却不隐式 restore，必须先生成 assets.json。
    add_dependencies(JoyBindings example_csharp_restore)
    add_dependencies(JoyGame JoyBindings)
    add_dependencies(example_csharp_managed JoyGame)
endif()

# ---- 托管运行时部署：桌面自包含 / Android MonoVM ----
# 桌面（Godot 4 导出版做法）：dotnet publish --self-contained 把完整 CoreCLR +
# BCL（按白名单裁剪）铺到 exe 目录；玩家无需装 .NET，VS 断点照常用。
# Android：仍用 libmonosgen 的 coreclr_* + 4 个平台 BCL（CoreLib 按 RID 特化）。
get_filename_component(_clr_bin "${MONO_LIBRARIES}" DIRECTORY)
set(_clr_libs "${_clr_bin}/libs")

set(_out_dir "$<TARGET_FILE_DIR:example_csharp>")

# 资源 + JoyGame.dll 通用部署（两个平台都要）
add_custom_command(TARGET example_csharp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${JOY_EXAMPLE_ASSETS}" "${JOY_EXAMPLE_ASSET_ROOT}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_cs_out}/JoyGame.dll" "${JOY_EXAMPLE_ASSET_ROOT}/JoyGame.dll"
    COMMENT "Copying C# example assets + JoyGame.dll")

if(ANDROID)
    # Android：4 个 BCL 按 RID 特化，放 lib/；Joy.Bindings.dll 同目录，
    # 宿主把 MONO_LIB_DIR 指向 lib（见 main.cpp 的 setup_clr_dirs）。
    if(NOT EXISTS "${_clr_libs}/System.Private.CoreLib.dll")
        message(FATAL_ERROR
            "缺平台 BCL: ${_clr_libs}/System.Private.CoreLib.dll\n"
            "把 Microsoft.NETCore.App.Runtime.Mono.<rid> 同版本的 4 个 BCL 放进\n"
            "${_clr_bin}/libs/ 。CoreLib 按 RID 构建，不能复用其它平台的那份。")
    endif()
    add_custom_command(TARGET example_csharp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${JOY_EXAMPLE_ASSET_ROOT}/lib"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${_clr_libs}" "${JOY_EXAMPLE_ASSET_ROOT}/lib"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_cs_out}/Joy.Bindings.dll" "${JOY_EXAMPLE_ASSET_ROOT}/lib/Joy.Bindings.dll"
        COMMENT "Copying Android MonoVM BCL + Joy.Bindings.dll into lib/")
    # coreclr.dll / libmonosgen 主库 + libSystem.* shim 由共享函数部署到 APK lib/<abi>
    joy_examples_deploy_clr(example_csharp)
else()
    # 桌面自包含：publish + 裁剪 BCL + JoyGame.dll/pdb 覆盖（单 ALC 实例，保断点）。
    # Joy.Bindings.dll 随 publish 落在 exe 根目录，与 JoyGame.dll 同目录——
    # 这是 hostfxr 命令行模式下避免双 ALC 实例的关键（见主工程同样约束）。
    include(${JOY_ROOT}/cmake/JoySelfContained.cmake)
    joy_deploy_selfcontained(example_csharp "${_cs_proj}" "${_cs_out}"
                             "${JOY_EXAMPLE_ASSET_ROOT}" "$<IF:$<CONFIG:Debug>,Debug,Release>")
    # sc_deploy 最后一步用 _cs_out 的 DLL/PDB 覆盖 publish 版，必须先有 dotnet build 产物
    add_dependencies(example_csharp_sc_deploy example_csharp_managed)
    add_dependencies(example_csharp example_csharp_sc_deploy)

    # 自包含 runtimeconfig.json（无 framework 节点）与 exe 同目录。
    add_custom_command(TARGET example_csharp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_LIST_DIR}/../csharp/JoyGame.runtimeconfig.json"
                "${JOY_EXAMPLE_ASSET_ROOT}/JoyGame.runtimeconfig.json"
        COMMENT "Deploying self-contained JoyGame.runtimeconfig.json")
endif()

enable_testing()
if(NOT ANDROID)
    add_test(NAME example_csharp_smoke COMMAND example_csharp --smoke 180)
    set_tests_properties(example_csharp_smoke PROPERTIES
        WORKING_DIRECTORY "${_out_dir}"
        ENVIRONMENT "SDL_VIDEODRIVER=dummy")
endif()
