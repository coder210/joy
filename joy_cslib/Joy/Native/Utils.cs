using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    public struct Utils
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int CurrentDatetime(string format);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static long CurrentTime();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int Bit2Short(byte[] buf);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static byte[] Short2Bit(short value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int Bit2Int(byte[] buf);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static byte[] Int2Bit(int value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string Md5(string value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string Md5x(string value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string B64Encode(string value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string B64Decode(string value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string AesEncrypt256(string key, string iv, string input);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string AesDecrypt256(string key, string iv, string input);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string AesEncrypt128(string key, string iv, string input);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string AesDecrypt128(string key, string iv, string input);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string DesEncrypt(string key, string text);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string DesDecrypt(string key, string text);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string EncryptFile(string key, string iv, out string filename);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string DecryptFile(string key, string iv, out string filename);
    }
}
