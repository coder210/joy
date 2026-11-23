# Android 的 MonoVM 运行时：从 runtimes/nupkg 里的官方 runtime pack 解包暂存
#
# 两个平台的共同点：都只认 runtimes/nupkg 里的官方包，不再入库散装托管 DLL。
#   桌面（CoreCLR）  : dotnet publish 直接消费，且**部署完整运行时不做裁剪**
#   Android（MonoVM）: 本文件在 configure 阶段解包 + 按白名单暂存（APK 体积敏感）
# 升级只需替换 runtimes/nupkg 里的包（版本见下方变量），不会出现
# "主库换了、BCL 忘了换"这种漂移（Android 曾因只放 4 个 BCL 出过 P9-FAIL）。
#
# 暂存布局刻意与历史上手工入库的 runtimes/<platform>/ 保持一致，
# 下游（assets/lib 拷贝、APK lib/<abi> 打包）一行都不用改：
#   <stage>/libs/*.dll            ← 白名单 BCL（CoreLib 取自 native/，按 RID 构建）
#   <stage>/libmonosgen-2.0.so    ← MonoVM 主库
#   <stage>/libSystem.*.so        ← CoreLib 的 P/Invoke shim（缺任一首次 JIT 闪退）

# 多个 CMakeLists（主工程 / joy 根 / examples）都会 include 本文件，只生效一次。
if(COMMAND joy_monovm_stage)
    return()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/JoyBclWhitelist.cmake")

# joy 仓根目录。必须在文件作用域求值：在 function() 内 CMAKE_CURRENT_LIST_DIR
# 指向的是**调用方**的 list 文件，会算成主工程目录（踩过）。
# （CMAKE_CURRENT_FUNCTION_LIST_DIR 是 3.17+，本项目最低 3.16，不能用。）
get_filename_component(_JOY_MONOVM_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# 版本必须与 runtimes/nupkg 里的文件名一致。升级 = 换包 + 改这里。
set(JOY_MONOVM_ANDROID_VERSION "8.0.17" CACHE STRING "Android MonoVM runtime pack 版本")

# joy_monovm_stage(<out_var>)
# 解包 nupkg 并按白名单暂存，返回暂存目录。任何缺失都 FATAL_ERROR——
# 宁可配置阶段报错，也不要在设备上退化成"没有 CLR"。
function(joy_monovm_stage _out_var)
    set(_joy_root "${_JOY_MONOVM_ROOT}")
    set(_ver "${JOY_MONOVM_ANDROID_VERSION}")
    set(_pkg "microsoft.netcore.app.runtime.mono.android-arm64.${_ver}")
    set(_nupkg "${_joy_root}/runtimes/nupkg/${_pkg}.nupkg")

    if(NOT EXISTS "${_nupkg}")
        message(FATAL_ERROR
            "缺 Android 的 MonoVM runtime pack：${_nupkg}\n"
            "Android 运行时现在从 runtimes/nupkg 解包，不再入库散装 DLL。\n"
            "下载：https://api.nuget.org/v3-flatcontainer/"
            "microsoft.netcore.app.runtime.mono.android-arm64/${_ver}/${_pkg}.nupkg\n"
            "放到 ${_joy_root}/runtimes/nupkg/ 即可（文件名必须一致）。")
    endif()

    set(_raw   "${CMAKE_BINARY_DIR}/_monovm/${_ver}/raw")
    set(_stage "${CMAKE_BINARY_DIR}/_monovm/${_ver}/android-arm64")
    set(_src   "${_raw}/runtimes/android-arm64")

    # 解包一次即可复用；cmake -E tar 由 libarchive 按内容识别 zip，nupkg 可直接解。
    if(NOT EXISTS "${_src}/native/libmonosgen-2.0.so")
        file(MAKE_DIRECTORY "${_raw}")
        execute_process(
            COMMAND "${CMAKE_COMMAND}" -E tar xf "${_nupkg}"
            WORKING_DIRECTORY "${_raw}"
            RESULT_VARIABLE _rc
            OUTPUT_QUIET)
        if(NOT _rc EQUAL 0 OR NOT EXISTS "${_src}/native/libmonosgen-2.0.so")
            message(FATAL_ERROR
                "解包 MonoVM runtime pack 失败（rc=${_rc}）：${_nupkg}\n"
                "包可能损坏，删除 ${_raw} 后重新 configure 再试。")
        endif()
    endif()

    file(MAKE_DIRECTORY "${_stage}/libs")
    foreach(_name IN LISTS JOY_BCL_KEEP)
        # CoreLib 按 RID 构建，只存在于 native/；其余程序集在 lib/net8.0/。
        if(EXISTS "${_src}/native/${_name}")
            set(_from "${_src}/native/${_name}")
        elseif(EXISTS "${_src}/lib/net8.0/${_name}")
            set(_from "${_src}/lib/net8.0/${_name}")
        else()
            message(FATAL_ERROR
                "MonoVM runtime pack 里缺 ${_name}\n"
                "白名单见 cmake/JoyBclWhitelist.cmake（桌面与 Android 共用）。")
        endif()
        execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        "${_from}" "${_stage}/libs/${_name}")
    endforeach()

    file(GLOB _shims LIST_DIRECTORIES false "${_src}/native/libSystem.*.so")
    if(NOT _shims)
        message(FATAL_ERROR "MonoVM runtime pack 里没有 libSystem.*.so：${_src}/native")
    endif()
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${_src}/native/libmonosgen-2.0.so" ${_shims} "${_stage}")

    if(NOT EXISTS "${_stage}/libmonosgen-2.0.so")
        message(FATAL_ERROR "暂存 MonoVM 主库失败：${_stage}")
    endif()

    set(${_out_var} "${_stage}" PARENT_SCOPE)
endfunction()
