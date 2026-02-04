using System;
using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    public struct Core
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool Running(IntPtr ctx);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Quit(IntPtr ctx);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string GetEnv(IntPtr ctx, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void SetEnv(IntPtr ctx, string name, string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Log(IntPtr ctx, string str);

    }
}
