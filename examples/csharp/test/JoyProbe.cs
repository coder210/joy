using System;

namespace JoyGame
{
    /// <summary>
    /// 临时诊断标记：用于定位 Linux/macOS 上 OnInit 内的 segfault
    /// （gdb 显示 JIT 代码调用地址 0x0）。
    ///
    /// 刻意用 Console.Error 而不是 Joy.Host.log：
    ///   - Console 是 BCL 实现，不经过 joy 的函数指针表，不会被可疑的
    ///     空指针连累；
    ///   - stderr 无缓冲，崩溃前最后一条一定落盘。
    /// 只在 --smoke 时输出，正常游玩静默；定位完即可删除。
    /// </summary>
    internal static class JoyProbe
    {
        internal static bool Enabled;

        internal static void Mark(string s)
        {
            if (!Enabled) return;
            Console.Error.WriteLine("[mark] " + s);
            Console.Error.Flush();
        }
    }
}
