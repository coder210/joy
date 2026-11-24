using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace Joy
{
    /// <summary>
    /// CoreCLR 函数指针表基础设施（Godot 模式）。
    ///
    /// C 侧把 797 个 internal call 按名字注册到一张表，只把一个查找函数
    /// lookup(name_utf8) -> fn_ptr 作为唯一的 callback 传给 C#。C# 侧在本类
    /// 里按名字查到每个函数的 native 指针，转成 delegate* unmanaged 调用。
    /// 顺序无关：增改绑定不会导致函数指针错位。
    /// </summary>
    public static unsafe class NativeFuncs
    {
        // lookup: const char* name -> void* fn。由 C 侧 coreclr 宿主在启动时传入。
        private static delegate* unmanaged[Cdecl]<byte*, void*> s_lookup;

        private static bool s_inited;

        /// <summary>由 GameApplication.Bootstrap (UCO) 转调，传入 C 侧 lookup 函数指针。
        /// 本方法不标记 UCO：UCO 方法不能被 C# 直接调用，须由 Bootstrap 转发。</summary>
        public static void Initialize(IntPtr lookupFn)
        {
            if (s_inited) return;
            s_lookup = (delegate* unmanaged[Cdecl]<byte*, void*>)lookupFn;
            if (s_lookup == null) throw new InvalidOperationException("Joy.NativeFuncs: null lookup fn");

            // 逐个绑定类解析自己的函数指针（生成链路 + 7 组手写绑定）。
            Api.Native.Init();
            Input.Init();
            Host.Init();
            Api.HostHandles.Init();
            Api.R3dExt.Init();
            Api.R3dExtPlus.Init();
            Api.CryptoExt.Init();
            Api.CollisionExt.Init();

            // [临时诊断] 在 Initialize 的上下文里立即调一次宿主日志，验证 delegate*
            // 与 s_log 在这里是否可用。若这里能打印、而 OnInit 里却崩，则锁定 ALC 分裂；
            // 若这里也崩，说明 delegate* 调用本身坏了（与 s_lookup 正常矛盾，极有信息量）。
            Host.log("[self-test] NativeFuncs.Initialize done, host log reachable");

            s_inited = true;
        }

        /// <summary>按注册名查 native 函数指针；缺失时抛异常（ fail fast，便于定位漏注册的绑定）。</summary>
        internal static void* Lookup(string name)
        {
            // C 侧用 strcmp，必须传 null 结尾的 UTF-8；Encoding.GetBytes 不带终止符。
            byte* p = (byte*)Marshal.StringToCoTaskMemUTF8(name);
            try
            {
                void* fn = s_lookup(p);
                if (fn == null) throw new EntryPointNotFoundException("Joy internal call not registered: " + name);
                return fn;
            }
            finally
            {
                Marshal.FreeCoTaskMem((IntPtr)p);
            }
        }

        /// <summary>
        /// 诊断用：按注册名返回 native 函数指针（0 表示未初始化或未注册）。
        /// 用于确认崩溃点上的绑定指针是否为空；定位完成后可删除。
        /// </summary>
        public static unsafe ulong DebugPointer(string name)
        {
            try { return (ulong)(nuint)Lookup(name); }
            catch (Exception) { return 0; }
        }

        // ---- string / 数组 / 指针编组辅助 ----

        /// <summary>C# string -> UTF-8 (CoTaskMem)。调用方必须 FreeUtf8。</summary>
        internal static byte* Utf8(string s)
        {
            return s == null ? null : (byte*)Marshal.StringToCoTaskMemUTF8(s);
        }

        internal static void FreeUtf8(byte* p)
        {
            if (p != null) Marshal.FreeCoTaskMem((IntPtr)p);
        }

        /// <summary>native 返回的 UTF-8 指针 -> C# string（约定由 C 侧静态缓冲持有，无需释放）。</summary>
        internal static string Str(IntPtr p)
        {
            return p == IntPtr.Zero ? null : Marshal.PtrToStringUTF8(p);
        }
    }
}
