using System;
using System.Runtime.CompilerServices;

namespace Joy.Api
{
    /// <summary>
    /// MD5 桥：委托 joy 的 joy_core_utils_md5x（与 C++ 帧同步 checksum 同一实现）。
    /// 该 C API 在 IDL 里是 X 级、生成器不出绑定，故手写 InternalCall。
    /// </summary>
    public static class CryptoExt
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string md5_hex(string input);

        /// <summary>返回 32 字符小写 hex MD5（与 utils_md5x 完全一致）。</summary>
        public static string Md5Hex(string input)
        {
            if (input == null) throw new ArgumentNullException(nameof(input));
            return md5_hex(input);
        }
    }
}
