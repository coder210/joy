using System;
using System.Runtime.CompilerServices;

namespace Joy.Api
{
    /// <summary>
    /// 宿主自建对象（窗口 / 渲染器 / 字体 / 图片）→ 生成链路的强类型句柄。
    ///
    /// 生成链路的句柄只认 C 侧的 joy_obj 表，而宿主往往自己建好窗口和渲染器
    /// 再把裸地址传进 <c>Host.OnInit</c>。这里调 host 胶水把它们登记成
    /// 「借用句柄」：脚本可以拿句柄调 <c>Joy.Api.Bindings</c> 的绘制函数，
    /// 但对象归宿主所有，句柄析构不会销毁它。
    ///
    /// 每个宿主对象只应从 <c>Host.OnInit</c> 调用一次，并让返回的句柄活到进程结束
    /// （句柄被 GC 回收后，C 侧表项随之失效，旧句柄再用就会报 JOY_ERR_HANDLE）。
    /// </summary>
    public static class HostHandles
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ulong borrow_r2d(IntPtr p);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ulong borrow_window(IntPtr p);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ulong borrow_font(IntPtr p);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ulong borrow_image(IntPtr p);

        /// <summary>宿主创建的渲染器；传入 IntPtr.Zero 返回 null。</summary>
        public static Jr2d R2d(IntPtr p)
        {
            if (p == IntPtr.Zero) return null;
            ulong h = borrow_r2d(p);
            return h == 0 ? null : new Jr2d(h);
        }

        /// <summary>宿主创建的窗口；传入 IntPtr.Zero 返回 null。</summary>
        public static Jwindow Window(IntPtr p)
        {
            if (p == IntPtr.Zero) return null;
            ulong h = borrow_window(p);
            return h == 0 ? null : new Jwindow(h);
        }

        /// <summary>宿主创建的字体；传入 IntPtr.Zero 返回 null。</summary>
        public static Font FontHandle(IntPtr p)
        {
            if (p == IntPtr.Zero) return null;
            ulong h = borrow_font(p);
            return h == 0 ? null : new Font(h);
        }

        /// <summary>宿主创建的图片；传入 IntPtr.Zero 返回 null。</summary>
        public static Image ImageHandle(IntPtr p)
        {
            if (p == IntPtr.Zero) return null;
            ulong h = borrow_image(p);
            return h == 0 ? null : new Image(h);
        }
    }
}
