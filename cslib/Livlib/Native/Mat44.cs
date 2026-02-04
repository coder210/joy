using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    public struct Mat44
    {
        public float M0 { get; set; }
        public float M4 { get; set; }
        public float M8 { get; set; }
        public float M12 { get; set; }
        public float M1 { get; set; }
        public float M5 { get; set; }
        public float M9 { get; set; }
        public float M13 { get; set; }
        public float M2 { get; set; }
        public float M6 { get; set; }
        public float M10 { get; set; }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44 Mul(Mat44 a, Mat44 b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44 Translate(float x, float y, float z);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44 Identity();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44 RotateX(float angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44 RotateY(float angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44 RotateZ(float angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44 Rotate(float angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44 Scale(float x, float y, float z);
        //[MethodImpl(MethodImplOptions.InternalCall)]
        //public extern static Mat44 MulVec4F(Mat44 left, Vec4F right);
    }
}
