using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    using Fp = System.Int64;

    public struct FP
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp FromFloat(float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float ToFloat(Fp value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp MinValue();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp MaxValue();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Add(Fp a, Fp b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Sub(Fp a, Fp b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Mul(Fp a, Fp b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Div(Fp a, Fp b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Abs(Fp x);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Sqrt(Fp x);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Sin(Fp x);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Cos(Fp x);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Zero();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Half();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp One();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Pi();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Max(Fp a, Fp b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Min(Fp a, Fp b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Pow2(Fp x);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Sign(Fp x);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Clamp(Fp a, Fp low, Fp high);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Lerp(Fp a, Fp b, Fp t);
    }
}
