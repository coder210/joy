using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    public struct Mat22F
    {
        public Vec2F Col1 { get; set; }
        public Vec2F Col2 { get; set; }


        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2F MulVec2(Mat22F a, Vec2F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat22F Rotate(long value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat22F FInvert(Mat22F m);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat22F Add(Mat22F a, Mat22F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat22F Mul(Mat22F a, Mat22F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat22F Abs(Mat22F a);
    }
}
