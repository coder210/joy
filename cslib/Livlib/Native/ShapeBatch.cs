using System;
using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    public struct ShapeBatch
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr Create();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Destroy(IntPtr inst);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Line(IntPtr inst, SDL.FPoint start, SDL.FPoint end, float thickness, SDL.FColor color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Rectangle(IntPtr inst, SDL.FRect rect, float thickness, SDL.FColor color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void RectangleRotate(IntPtr inst, SDL.FRect rect, float angle, float thickness, SDL.FColor color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Circle(IntPtr inst, SDL.FPoint center, float radius, int points,
        float thickness, SDL.FColor color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Polygon(IntPtr inst, SDL.FPoint[] vertices, int num,
        float thickness, SDL.FColor color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Render(IntPtr inst, IntPtr renderer, bool clear = true);
    }
}
