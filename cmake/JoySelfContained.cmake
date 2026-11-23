# 桌面自包含 .NET 运行时部署（Godot 4 导出版的做法）
#
# hostfxr_initialize_for_dotnet_command_line 从 exe 目录加载完整 CoreCLR，
# 玩家机器无需安装 .NET。dotnet publish --self-contained 产出完整运行时
# （约 71MB），本模块把它整体铺到 exe 目录，并覆盖 JoyGame.dll / pdb。
#
# **不要重新引入 BCL 白名单裁剪**：曾按 30 个程序集的清单裁剪到 48.6MB，
# 但缺口只在特定平台的特定路径上暴露（Linux 的 Console 要
# Microsoft.Win32.Primitives，Windows 走不到），只能靠 CI 反复试。
# 22MB 换不来这份维护负担。Android 侧仍用白名单（APK 体积敏感，且有真机验证）。
#
# 用法：在定义 target 后 include 本文件，调用：
#   joy_deploy_selfcontained(<target> <csproj> <cs_out_dir> <runtime_out_dir> <config>)

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



# joy_deploy_selfcontained(target csproj cs_out runtime_out config)
# 创建两个自定义目标：
#   <target>_sc_publish  : dotnet publish --self-contained 到缓存目录
#   <target>_sc_deploy   : 部署完整运行时 + 覆盖游戏 DLL/PDB（单实例，保断点）
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

    # 桌面**不再按白名单裁剪 BCL**（2024-11 决策，有实测依据）：
    #   裁剪只省 22MB（70.9MB -> 48.6MB），却需要手工维护一份"够用"的程序集清单。
    #   而 CoreLib 的 AssemblyRef 闭包有 115 个（大量是类型转发，按需才加载），
    #   真正的缺口只在特定平台的特定代码路径上暴露：Linux 的 Console 初始化要
    #   Microsoft.Win32.Primitives，Windows 走不到那段代码，本机永远测不出来。
    # 结果就是 CI 一轮轮试错（Android 曾因只放 4 个 BCL 出过 P9-FAIL）。
    # 既然代价只有 22MB，直接部署完整运行时，让三个平台行为一致。
    add_custom_target(${_target}_sc_deploy
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_runtime_dir}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${_publish_dir}" "${_runtime_dir}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_cs_out}/JoyGame.dll" "${_runtime_dir}/JoyGame.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_cs_out}/JoyGame.pdb" "${_runtime_dir}/JoyGame.pdb"
        DEPENDS ${_target}_sc_publish
        COMMENT "Deploying self-contained runtime + JoyGame.dll/pdb for ${_target}"
        VERBATIM)
endfunction()
