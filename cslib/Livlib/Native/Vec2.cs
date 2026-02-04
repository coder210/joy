using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    public struct Vec2
    {
        public float X { get; set; }
        public float Y { get; set; }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 Negate(Vec2 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 Dot(Vec2 a, Vec2 b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 Cross(Vec2 a, Vec2 b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 Cross2(Vec2 a, Vec2 b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 Cross3(Vec2 a, Vec2 b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 Add(Vec2 a, Vec2 b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 Sub(Vec2 a, Vec2 b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 Scale(Vec2 a, Vec2 b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 Abs(Vec2 a);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Vec2LengthSquared(Vec2 x);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float Vec2Length(Vec2 x);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 Normalize(Vec2 v);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 Normal(Vec2 v);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 Rotate(Vec2 v, float angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 Lerp(Vec2 v, float angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Vec2 Distance(Vec2 a, Vec2 b);
    }
}
