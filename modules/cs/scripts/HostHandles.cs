using System;
using System.Runtime.InteropServices;

namespace Joy.Api
{
    /// <summary>
    /// 宿主自建对象（窗口 / 渲染器 / 字体 / 图片）→ 生成链路的强类型句柄。
    /// CoreCLR 函数指针表绑定。
    /// </summary>
    public static unsafe class HostHandles
    {
        // C 侧签名：uint64_t w_borrow_xxx(intptr_t p)
        private static delegate* unmanaged[Cdecl]<IntPtr, ulong> s_borrow_r2d;
        private static delegate* unmanaged[Cdecl]<IntPtr, ulong> s_borrow_window;
        private static delegate* unmanaged[Cdecl]<IntPtr, ulong> s_borrow_font;
        private static delegate* unmanaged[Cdecl]<IntPtr, ulong> s_borrow_image;

        internal static void Init()
        {
            s_borrow_r2d = (delegate* unmanaged[Cdecl]<IntPtr, ulong>)NativeFuncs.Lookup("Joy.Api.HostHandles::borrow_r2d");
            s_borrow_window = (delegate* unmanaged[Cdecl]<IntPtr, ulong>)NativeFuncs.Lookup("Joy.Api.HostHandles::borrow_window");
            s_borrow_font = (delegate* unmanaged[Cdecl]<IntPtr, ulong>)NativeFuncs.Lookup("Joy.Api.HostHandles::borrow_font");
            s_borrow_image = (delegate* unmanaged[Cdecl]<IntPtr, ulong>)NativeFuncs.Lookup("Joy.Api.HostHandles::borrow_image");
        }

        private static ulong borrow_r2d(IntPtr p) => s_borrow_r2d(p);
        private static ulong borrow_window(IntPtr p) => s_borrow_window(p);
        private static ulong borrow_font(IntPtr p) => s_borrow_font(p);
        private static ulong borrow_image(IntPtr p) => s_borrow_image(p);

        /// <summary>宿主创建的渲染器；传入 IntPtr.Zero 返回 null。</summary>
        public static Jr2d R2d(IntPtr p)
        {
            if (p == IntPtr.Zero) return null;
            ulong h = borrow_r2d(p);
            return h == 0 ? null : new Jr2d(h);
        }

        public static Jwindow Window(IntPtr p)
        {
            if (p == IntPtr.Zero) return null;
            ulong h = borrow_window(p);
            return h == 0 ? null : new Jwindow(h);
        }

        public static Font FontHandle(IntPtr p)
        {
            if (p == IntPtr.Zero) return null;
            ulong h = borrow_font(p);
            return h == 0 ? null : new Font(h);
        }

        public static Image ImageHandle(IntPtr p)
        {
            if (p == IntPtr.Zero) return null;
            ulong h = borrow_image(p);
            return h == 0 ? null : new Image(h);
        }
    }
}
