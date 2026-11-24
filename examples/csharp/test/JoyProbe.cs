using System;

namespace JoyGame
{
    /// <summary>
    /// 临时诊断标记：用于定位 Linux 上 `OnInit` 内的 segfault（gdb 显示 JIT 代码
    /// 调用地址 0x0）。
    ///
    /// 双通道，专门用来区分两种可能：
    ///   1) 标记自己崩（Console 路径有问题）——托管写入后崩溃，但上一条已经看到；
    ///   2) 托管到 native 的 `delegate*` 调用崩——那第二通道会走
    ///      `Joy.Host.log`，而 C 侧 `joy_api_call` 会打印 `[thunk] ...`。
    ///
    /// 判读方式：
    ///   - 看到 `[mark]` + `[thunk] call ... joy_host_log` + `[mark-native]`
    ///     → delegate* 正常，继续看后面的 `[thunk]` 停在哪；
    ///   - 看到 `[mark]` 但没有 `[thunk]`
    ///     → 崩在托管到 native 的边界上（此时 Console 与 native 两条路一起失效，
    ///       说明是更深层的问题，不是某个绑定为空）。
    ///
    /// 只在 --smoke 时输出，正常游玩静默；定位完即可删除本文件与全部调用点。
    /// </summary>
    internal static class JoyProbe
    {
        internal static bool Enabled;

        internal static void Mark(string s)
        {
            if (!Enabled) return;

            // 通道 1：纯 BCL。Console.Error 的 StreamWriter 自带 AutoFlush，
            // 不要再显式 Flush（上一版多一次 Flush，曾与崩溃点难以区分）。
            Console.Error.WriteLine("[mark] " + s);

            // 通道 2：经 joy 绑定（C 侧会打印 [thunk] call ... joy_host_log）。
            Joy.Host.log("[mark-native] " + s);
        }
    }
}
