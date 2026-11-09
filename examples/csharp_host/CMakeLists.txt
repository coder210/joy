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

# ---- 托管运行时文件：全部铺进 JOY_EXAMPLE_ASSET_ROOT（无 assets/ 前缀） ----
# 桌面就是 exe 同目录；Android 是 pack/，打包时原样成为 APK 的 assets 根。
# coreclr.dll 是原生库，由系统加载器从 exe 旁查找，必须留在根目录（见下方部署函数）。
# BCL 按 RID 存放在 <平台目录>/libs/：CoreLib 含平台特化的 P/Invoke 面
# （Windows 走 kernel32、Android 走 liblog），实测两者 91% 字节不同，
# 跨平台混用会在原生化调用上直接失败，所以不做共享、也不做回退。
get_filename_component(_clr_bin "${MONO_LIBRARIES}" DIRECTORY)
set(_clr_libs "${_clr_bin}/libs")
if(NOT EXISTS "${_clr_libs}/System.Private.CoreLib.dll")
    message(FATAL_ERROR
        "缺平台 BCL: ${_clr_libs}/System.Private.CoreLib.dll\n"
        "把 Microsoft.NETCore.App.Runtime.Mono.<rid> 同版本的 4 个 BCL 放进\n"
        "${_clr_bin}/libs/ 。CoreLib 按 RID 构建，不能复用其它平台的那份。")
endif()

set(_out_dir "$<TARGET_FILE_DIR:example_csharp>")

add_custom_command(TARGET example_csharp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${JOY_EXAMPLE_ASSETS}" "${JOY_EXAMPLE_ASSET_ROOT}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_cs_out}/JoyGame.dll" "${JOY_EXAMPLE_ASSET_ROOT}/JoyGame.dll"
    # 先整块同步 BCL，再把绑定程序集塞进 lib/：
    # 自定义 CLR 只从 MONO_LIB_DIR 解析依赖程序集，宿主已把它指向 lib。
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${JOY_EXAMPLE_ASSET_ROOT}/lib"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${_clr_libs}" "${JOY_EXAMPLE_ASSET_ROOT}/lib"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_cs_out}/Joy.Bindings.dll" "${JOY_EXAMPLE_ASSET_ROOT}/lib/Joy.Bindings.dll"
    COMMENT "Copying C# example runtime (JoyGame.dll + lib/)")

# coreclr.dll 走共享部署函数：另外三个示例同样导入它（joy::all 含 cs 模块）
joy_examples_deploy_clr(example_csharp)

enable_testing()
if(NOT ANDROID)
    add_test(NAME example_csharp_smoke COMMAND example_csharp --smoke 180)
    set_tests_properties(example_csharp_smoke PROPERTIES
        WORKING_DIRECTORY "${_out_dir}"
        ENVIRONMENT "SDL_VIDEODRIVER=dummy")
endif()
