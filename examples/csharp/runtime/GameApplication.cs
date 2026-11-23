using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace JoyGame
{
    /// <summary>
    /// CoreCLR 绑定初始化入口。jcsx.c 的 csx_load_assembly() 硬编码取
    /// "JoyGame.GameApplication, JoyGame" 的 Bootstrap（UCO），把 native 的
    /// lookup(name) 函数指针传进来；Bootstrap 转调 NativeFuncs.Initialize
    /// 建立 797 个函数指针绑定，保证与 Host 的入口落在同一个 ALC 实例。
    ///
    /// 注意：Bootstrap 必须是 UCO（供 native 取函数指针），但它转调的
    /// NativeFuncs.Initialize 是普通 C# 方法（UCO 方法不能被 C# 直接调用）。
    /// </summary>
    public static class GameApplication
    {
        // 桌面：hostfxr 取 UCO 函数指针直调。
        [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
        public static void Bootstrap(IntPtr lookupFn)
        {
            BootstrapMono(lookupFn);
        }

        // Android：mono_runtime_invoke 只能调普通 public static 方法
        // （UCO 方法禁止反射调用），故提供普通入口，逻辑与桌面共用。
        public static void BootstrapMono(IntPtr lookupFn)
        {
            Joy.NativeFuncs.Initialize(lookupFn);
        }
    }
}
