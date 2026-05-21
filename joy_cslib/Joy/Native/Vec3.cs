using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    public struct Vec3
    {
        public float X { get; set; }
        public float Y { get; set; }
        public float Z { get; set; }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3 Negate(Vec3 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3 Add(Vec3 a, Vec3 b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3 Sub(Vec3 a, Vec3 b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3 Scale(Vec3 a, float b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3 Abs(Vec3 a);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Dot(Vec3 a, Vec3 b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3 Cross(Vec3 a, Vec3 b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Vec3LengthSquared(Vec3 x);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Vec3Length(Vec3 x);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3 Normalize(Vec3 v);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec3 Lerp(Vec3 v, float angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Distance(Vec3 a, Vec3 b);
    }
}
