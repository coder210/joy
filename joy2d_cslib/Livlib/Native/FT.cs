using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    public struct FT
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float FromFloat(float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float MinValue();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float MaxValue();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Add(float a, float b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Sub(float a, float b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Mul(float a, float b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Div(float a, float b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Abs(float a);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Sqrt(float a);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Sin(float angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Cos(float angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Zero();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Half();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float One();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float PI();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Max(float a, float b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Min(float a, float b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Pow2(float a);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Sign(float a);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Clamp(float a, float low, float high);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Lerp(float a, float b, float t);
    }
}
