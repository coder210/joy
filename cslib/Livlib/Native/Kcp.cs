using System;
using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    public struct Kcp
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr Create(int conv, IntPtr userdata);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void SetWndSize(IntPtr kcpcb, int sndwnd, int rcvwnd);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void SetOutput(IntPtr kcpcb, Func<int, byte[], int, IntPtr, IntPtr> output);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Release(IntPtr kcpcb);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int GetConv(IntPtr kcpcb);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr Check(IntPtr kcpcb, int current);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Update(IntPtr kcpcb, int current);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Input(IntPtr kcpcb, byte[] data);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int PeekSize(IntPtr kcpcb);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int Recv(IntPtr kcpcb, out byte[] data);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int Send(IntPtr kcpcb, byte[] data);
    }
}
