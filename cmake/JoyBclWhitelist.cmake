# Android 的托管程序集白名单（桌面已不用裁剪，见 JoySelfContained.cmake）
#
# 为什么只有 Android 用：APK 体积敏感，且 Android 有真机/CI 验证；
# 桌面曾经共用这份清单，结果反复踩坑——CoreLib 的 AssemblyRef 闭包有 115 个
# （大量是类型转发，按需才加载），缺口只在特定平台路径上暴露：
#   * Android 只放 4 个核心程序集 → BootstrapMono 解析 Marshal/LINQ 抛
#     FileNotFoundException（P9-FAIL）
#   * Linux 的 Console 初始化要 Microsoft.Win32.Primitives → 桌面裁剪后崩
# 桌面因此改为部署完整运行时（+22MB），不再维护白名单。
#
# 使用方：cmake/JoyMonoVm.cmake 按本表从 MonoVM runtime pack 里解包。
# 规则：Console 在白名单里，就必须连它的依赖一起放（Microsoft.Win32.Primitives），
# 否则第一次 Console 调用就 FileNotFoundException（0x80070002，很难定位）。

if(DEFINED JOY_BCL_KEEP)
    return()   # 已被 include 过（多个 CMakeLists 都会 include）
endif()

set(JOY_BCL_KEEP
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
    # Console 的依赖：Unix 的 ConsolePal.EnsureInitializedCore 会加载它。
    # 少了它在桌面 Linux 上直接 FileNotFoundException（CI 实测）。
    Microsoft.Win32.Primitives.dll
)
