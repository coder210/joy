# 托管程序集白名单 —— 桌面（CoreCLR 自包含）与 Android（MonoVM）共用一份
#
# 为什么必须共用：两边的运行时实现不同，但 JoyGame/Joy.Bindings 用到的 BCL 面
# 是同一套。分两处维护出过一次事——Android 只放了 4 个核心程序集，
# BootstrapMono 解析 Marshal/StringBuilder/LINQ 时抛 FileNotFoundException
# （表现为 P9-FAIL）。统一到本文件后，桌面裁剪与 Android 解包同源，
# 不会再出现"一边补了、另一边忘了"。
#
# 规则：
#   - 桌面：publish 产出 161 个 BCL，按本表裁剪（见 JoySelfContainedPrune.cmake）
#   - Android：按本表从 MonoVM runtime pack 的 nupkg 里解包（见 JoyMonoVm.cmake）
# 缺失任意一个会在首次用到时抛 FileNotFoundException（0x80070002，很难定位），
# 所以宁可略多不可遗漏。

if(DEFINED JOY_SELFCONTAINED_BCL_KEEP)
    return()   # 已被 include 过（多个 CMakeLists 都会 include）
endif()

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
