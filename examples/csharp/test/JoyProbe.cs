using System;

namespace JoyGame
{
    /// <summary>
    /// 临时诊断标记：用于定位 Linux 上 `OnInit` 内的 segfault。
    /// 只走 Console（纯 BCL，不经过 joy 的 delegate*），保证标记本身不会崩。
    /// 只在 --smoke 时输出，正常游玩静默；定位完即可删除。
    /// </summary>
    internal static class JoyProbe
    {
        internal static bool Enabled;

        internal static void Mark(string s)
        {
            if (!Enabled) return;
            Console.Error.WriteLine("[mark] " + s);
        }
    }
}
