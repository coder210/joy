using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    using Fp = System.Int64;
    public struct Mat44F
    {
        public Fp M0 { get; set; }
        public Fp M4 { get; set; }
        public Fp M8 { get; set; }
        public Fp M12 { get; set; }
        public Fp M1 { get; set; }
        public Fp M5 { get; set; }
        public Fp M9 { get; set; }
        public Fp M13 { get; set; }
        public Fp M2 { get; set; }
        public Fp M6 { get; set; }
        public Fp M10 { get; set; }
        public Fp M14 { get; set; }
        public Fp M3 { get; set; }
        public Fp M7 { get; set; }
        public Fp M11 { get; set; }
        public Fp M15 { get; set; }


        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44F Mat44FMul(Mat44F a, Mat44F b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44F Mat44FTranslate(Fp x, Fp y, Fp z);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44F Mat44FIdentity();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44F Mat44FRotateX(Fp angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44F Mat44FRotateY(Fp angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44F Mat44FRotateZ(Fp angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44F Mat44FRotate(Fp angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Mat44F Mat44FScale(Fp x, Fp y, Fp z);
        //[MethodImpl(MethodImplOptions.InternalCall)]
        //public extern static Mat44F MulVec4F(Mat44F left, Vec4F right);
    }
}
