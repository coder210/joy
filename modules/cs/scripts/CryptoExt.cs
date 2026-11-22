using System;
using System.Runtime.InteropServices;

namespace Joy.Api
{
    /// <summary>MD5 桥：委托 joy_core_utils_md5x（与 C++ 帧同步 checksum 同一实现）。CoreCLR 函数指针表绑定。</summary>
    public static unsafe class CryptoExt
    {
        // C 侧签名：const char* w_md5_hex(const char* input)
        // 返回值指向 C 侧静态缓冲区（线程内复用）；调用方立即拷贝，无需释放。
        private static delegate* unmanaged[Cdecl]<byte*, IntPtr> s_md5_hex;

        internal static void Init()
        {
            s_md5_hex = (delegate* unmanaged[Cdecl]<byte*, IntPtr>)NativeFuncs.Lookup("Joy.Api.CryptoExt::md5_hex");
        }

        /// <summary>返回 32 字符小写 hex MD5（与 utils_md5x 完全一致）。</summary>
        public static string Md5Hex(string input)
        {
            if (input == null) throw new ArgumentNullException(nameof(input));
            byte* p = NativeFuncs.Utf8(input);
            try {
                return NativeFuncs.Str(s_md5_hex(p));
            } finally {
                NativeFuncs.FreeUtf8(p);
            }
        }
    }
}
