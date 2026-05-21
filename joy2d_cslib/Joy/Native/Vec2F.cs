using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    using Fp = System.Int64;

    public struct Vec2F
    {
        public Fp X { get; set; }
        public Fp Y { get; set; }


        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2F Negate(Vec2F value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Vec2FDot(Vec2F a, Vec2F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Vec2FCross(Vec2F a, Vec2F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2F Cross2(Vec2F a, Vec2F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2F Cross3(Vec2F a, Vec2F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2F Add(Vec2F a, Vec2F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2F Sub(Vec2F a, Vec2F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2F Scale(Vec2F a, Vec2F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2F Abs(Vec2F a);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Vec2FLengthSquared(Vec2F x);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Vec2FLength(Vec2F x);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2F Normalize(Vec2F v);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2F Normal(Vec2F v);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2F Rotate(Vec2F v, Fp angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2F Lerp(Vec2F v, Fp angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2F Distance(Vec2F a, Vec2F b);
    }
}
