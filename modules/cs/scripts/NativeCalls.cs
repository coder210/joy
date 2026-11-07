using System;
using System.Runtime.CompilerServices;

// 手写的 C# → C internal call 声明；对应的 C 实现位于
// modules/cs/bindings/host/csclib_{input,host,borrow}.c。
// 这些 API 用标量参数，避免跨 ABI 传递结构体。
// 绘制 / 字体 / 图片一律走生成链路 Joy.Api.Bindings，不在这里声明。
//
// 类必须是 public：业务程序集通过 ProjectReference 引用本程序集，
// internal 声明跨程序集不可见。
namespace Joy
{
    public static class Input
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool key_down(int scancode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool mouse_button(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float mouse_x();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float mouse_y();
    }

    public static class Host
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void log(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void quit();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void set_exit_code(int code);
    }
}
