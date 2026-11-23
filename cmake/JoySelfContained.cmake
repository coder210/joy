# 桌面自包含 .NET 运行时部署（Godot 4 导出版的做法）
#
# hostfxr_initialize_for_dotnet_command_line 从 exe 目录加载完整 CoreCLR，
# 玩家机器无需安装 .NET。dotnet publish --self-contained 默认产出 161 个
# System.* BCL（约 71MB），但 CLR 按需加载——只有代码真正用到的程序集才读盘。
# 本模块把 BCL 收敛到一份白名单（实测覆盖全部 smoke/parity/文件IO/反射路径），
# 把发布包从 ~88MB 降到 ~48MB，同时保留断点所需的 PDB。
#
# 用法：在定义 target 后 include 本文件，调用：
#   joy_deploy_selfcontained(<target> <csproj> <cs_out_dir> <runtime_out_dir> <config>)

# 必须保留的 BCL。缺失任意一个会在首次用到时抛 FileNotFoundException
# （错误码 0x80070002，很难定位），所以宁可略多不可遗漏。
set(JOY_SELFCONTAINED_BCL_KEEP
    # 核心（缺任一直接起不来）
    System.Private.CoreLib.dll
    System.Runtime.dll
    netstandard.dll
    System.Collections.dll
    mscorlib.dll
    # C# 代码实际用到的命名空间
    System.IO.FileSystem.dll
    System.IO.FileSystem.Primitives.dll
    System.IO.dll
    System.Reflection.dll
    System.Reflection.Extensions.dll
    System.Reflection.Primitives.dll
    System.Globalization.dll
    System.Globalization.Extensions.dll
    System.Text.Encoding.dll
    System.Text.Encoding.Extensions.dll
    System.Runtime.InteropServices.dll
    System.Runtime.Extensions.dll
    System.Linq.dll
    System.Linq.Expressions.dll
    System.Threading.dll
    System.Threading.Tasks.dll
    System.Diagnostics.Debug.dll
    System.Memory.dll
    System.Collections.Concurrent.dll
    System.Runtime.Serialization.Primitives.dll
    # 常用支撑（启动/JIT/异常路径会用到，代价很小）
    System.ComponentModel.Primitives.dll
    System.Diagnostics.Tools.dll
    System.Runtime.CompilerServices.Unsafe.dll
    System.Threading.Thread.dll
    System.Console.dll
)

# RID：与 CMake 平台对应。Android 不调用本函数（走 libmonosgen）。
function(_joy_sc_rid_out out)
    if(WIN32)
        set(${out} "win-x64" PARENT_SCOPE)
    elseif(APPLE)
        if(CMAKE_OSX_ARCHITECTURES MATCHES "arm64")
            set(${out} "osx-arm64" PARENT_SCOPE)
        else()
            set(${out} "osx-x64" PARENT_SCOPE)
        endif()
    else()
        set(${out} "linux-x64" PARENT_SCOPE)
    endif()
endfunction()

# 生成"删除白名单外 BCL"的 POST_BUILD 命令列表，写到 _out 变量。
function(_joy_sc_prune_cmds _publish_dir _runtime_dir _out)
    set(_cmds)
    file(GLOB _all_bcl LIST_DIRECTORIES false
        "${_publish_dir}/System.*.dll" "${_publish_dir}/Microsoft.*.dll")
    foreach(_bcl IN LISTS _all_bcl)
        get_filename_component(_name "${_bcl}" NAME)
        if(NOT _name IN_LIST JOY_SELFCONTAINED_BCL_KEEP)
            list(APPEND _cmds COMMAND ${CMAKE_COMMAND} -E rm -f "${_runtime_dir}/${_name}")
        endif()
    endforeach()
    set(${_out} ${_cmds} PARENT_SCOPE)
endfunction()

# joy_deploy_selfcontained(target csproj cs_out runtime_out config)
# 创建两个自定义目标：
#   <target>_sc_publish  : dotnet publish --self-contained 到缓存目录
#   <target>_sc_deploy   : 部署运行时 + 裁剪 BCL + 覆盖游戏 DLL/PDB（单实例，保断点）
# 调用方需把 <target>_sc_deploy 加进 <target> 的 add_dependencies。
function(joy_deploy_selfcontained _target _csproj _cs_out _runtime_dir _config)
    _joy_sc_rid_out(_rid)
    set(_publish_dir "${CMAKE_BINARY_DIR}/_selfcontained/${_rid}/${_config}")

    # 仓库自带 runtime pack（runtimes/nupkg/*.nupkg），作为**附加** NuGet 源：
    #   - 不替换 nuget.org，在线构建行为与之前完全一致（零回归）
    #   - 离线时由本地 nupkg 提供 Microsoft.NETCore.App.Runtime.<rid>，
    #     SDK 自带的 Ref/Host pack 提供其余部分，clone 后即可构建
    # 版本必须与 SDK 选择的 runtime pack 一致（当前 8.0.11），升级 .NET 时
    # 同步替换 runtimes/nupkg 里的 nupkg 即可。
    set(_nupkg_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../runtimes/nupkg")
    set(_sc_src_args "")
    if(EXISTS "${_nupkg_dir}")
        set(_sc_src_args
            "-p:RestoreAdditionalProjectSources=${_nupkg_dir}"
            "--ignore-failed-sources")
    endif()

    add_custom_target(${_target}_sc_publish
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_publish_dir}"
        COMMAND dotnet publish "${_csproj}"
                -c ${_config}
                -r "${_rid}" --self-contained true -o "${_publish_dir}"
                # 不下载 AspNetCore/WindowsDesktop 的 runtime pack：本项目只用到
                # Microsoft.NETCore.App，缺这个开关时 self-contained 会为所有已知
                # 框架引用拉包，离线构建必然失败（NU1101）。
                -p:DisableTransitiveFrameworkReferenceDownloads=true
                # 不需要 .NET apphost：游戏由 C++ 宿主经 hostfxr 加载 JoyGame.dll，
                # 关掉可省掉 Microsoft.NETCore.App.Host.<rid>（离线更少依赖）。
                -p:UseAppHost=false
                ${_sc_src_args}
        COMMENT "dotnet publish self-contained runtime for ${_target} (${_rid})"
        VERBATIM)

    # file(GLOB) 在 configure 时 publish 目录还不存在，拿不到 BCL 清单。
    # 改为部署时用一个 cmake -P 脚本按白名单裁剪（脚本在 build 时读取真实目录）。
    #
    # 白名单通过文件传递而非 -DKEEP=a;b;c：分号列表在 -D 参数里会被 shell
    # 当成命令分隔符（等价于 `System.Runtime.dll: command not found`），
    # 即使加 VERBATIM 在不同 CMake/shell 组合下也不可靠。写成 .cmake 文件
    # 由 -P 脚本 include，路径不含分号，跨平台安全。
    set(_prune_script "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/JoySelfContainedPrune.cmake")
    set(_keep_file "${CMAKE_BINARY_DIR}/_bcl_keep_${_target}.cmake")
    file(WRITE "${_keep_file}"
         "set(JOY_BCL_KEEP \"${JOY_SELFCONTAINED_BCL_KEEP}\")\n")
    add_custom_target(${_target}_sc_deploy
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_runtime_dir}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${_publish_dir}" "${_runtime_dir}"
        COMMAND ${CMAKE_COMMAND}
                "-DPublishDir=${_publish_dir}"
                "-DRuntimeDir=${_runtime_dir}"
                "-DKeepFile=${_keep_file}"
                -P "${_prune_script}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_cs_out}/JoyGame.dll" "${_runtime_dir}/JoyGame.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_cs_out}/JoyGame.pdb" "${_runtime_dir}/JoyGame.pdb"
        DEPENDS ${_target}_sc_publish
        COMMENT "Deploying self-contained runtime (BCL pruned) + JoyGame.dll/pdb for ${_target}"
        VERBATIM)
endfunction()
