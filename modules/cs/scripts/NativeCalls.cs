using System;
using System.Runtime.InteropServices;

// 手写的 C# → C internal call（CoreCLR 函数指针表绑定）。
// 对应的 C 实现位于 modules/cs/bindings/host/csclib_{input,host}.c。
// 这些 API 用标量参数，避免跨 ABI 传递结构体。
// 绘制 / 字体 / 图片一律走生成链路 Joy.Api.Bindings，不在这里声明。
//
// 类必须是 public：业务程序集通过 ProjectReference 引用本程序集。
namespace Joy
{
    public static unsafe class Input
    {
        // C 侧签名：bool w_key_down(int)（C bool = 1 字节，用 byte 表达）
        private static delegate* unmanaged[Cdecl]<int, byte> s_key_down;
        private static delegate* unmanaged[Cdecl]<int, byte> s_mouse_button;
        private static delegate* unmanaged[Cdecl]<float> s_mouse_x;
        private static delegate* unmanaged[Cdecl]<float> s_mouse_y;

        internal static void Init()
        {
            s_key_down = (delegate* unmanaged[Cdecl]<int, byte>)NativeFuncs.Lookup("Joy.Input::key_down");
            s_mouse_button = (delegate* unmanaged[Cdecl]<int, byte>)NativeFuncs.Lookup("Joy.Input::mouse_button");
            s_mouse_x = (delegate* unmanaged[Cdecl]<float>)NativeFuncs.Lookup("Joy.Input::mouse_x");
            s_mouse_y = (delegate* unmanaged[Cdecl]<float>)NativeFuncs.Lookup("Joy.Input::mouse_y");
        }

        public static bool key_down(int scancode) => s_key_down(scancode) != 0;
        public static bool mouse_button(int button) => s_mouse_button(button) != 0;
        public static float mouse_x() => s_mouse_x();
        public static float mouse_y() => s_mouse_y();
    }

    public static unsafe class Host
    {
        // C 侧签名：void w_log(const char*) / void w_quit() / void w_set_exit_code(int)
        private static delegate* unmanaged[Cdecl]<byte*, void> s_log;
        private static delegate* unmanaged[Cdecl]<void> s_quit;
        private static delegate* unmanaged[Cdecl]<int, void> s_set_exit_code;

        internal static void Init()
        {
            s_log = (delegate* unmanaged[Cdecl]<byte*, void>)NativeFuncs.Lookup("Joy.Host::log");
            s_quit = (delegate* unmanaged[Cdecl]<void>)NativeFuncs.Lookup("Joy.Host::quit");
            s_set_exit_code = (delegate* unmanaged[Cdecl]<int, void>)NativeFuncs.Lookup("Joy.Host::set_exit_code");
        }

        public static void log(string message)
        {
            byte* p = NativeFuncs.Utf8(message ?? "(null)");
            try { s_log(p); } finally { NativeFuncs.FreeUtf8(p); }
        }

        public static void quit() => s_quit();
        public static void set_exit_code(int code) => s_set_exit_code(code);

        /// <summary>[临时诊断] 直接读出 s_log 的 native 指针值（只读静态字段，不做
        /// delegate* 调用，因此即使调用本身会崩也能安全取值）。用于判断崩溃时
        /// s_log 在当前 ALC 里到底是 0（=绑定表没填到这里）还是非 0（=调用约定问题）。
        /// 定位完成后删除。</summary>
        public static unsafe ulong PtrLog() => (ulong)(nuint)s_log;
    }
}
