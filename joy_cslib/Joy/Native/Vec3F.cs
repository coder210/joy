using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    using Fp = System.Int64;
    public struct Vec3F
    {
        public Fp X { get; set; }
        public Fp Y { get; set; }
        public Fp Z { get; set; }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3F Negate(Vec3F a);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3F Add(Vec3F a, Vec3F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3F Sub(Vec3F a, Vec3F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3F Scale(Vec3F a, Fp b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3F Abs(Vec3F a);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Dot(Vec3F a, Vec3F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3F Cross(Vec3F a, Vec3F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp LengthSquared(Vec3F x);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Length(Vec3F x);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3F Normalize(Vec3F v);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3F Lerp(Vec3F v, Fp angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Fp Distance(Vec3F a, Vec3F b);
    }
}
