using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    public struct Mat22
    {
        public Vec2 Col1 { get; set; }
        public Vec2 Col2 { get; set; }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 MulVec2(Mat22 a, Vec2F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat22 Rotate(float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat22 Invert(Mat22 m);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat22 Add(Mat22 a, Mat22 b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat22 Mul(Mat22 a, Mat22 b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat22 Abs(Mat22 a);
    }
}
