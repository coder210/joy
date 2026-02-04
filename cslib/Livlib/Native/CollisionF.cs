using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    using Fp = System.Int64;
    public struct Ray2dF
    {
        public Vec2F Origin { get; set; }
        public Vec2F Direction { get; set; }
    }

    public struct RectangleF
    {
        public Fp X { get; set; }
        public Fp Y { get; set; }
        public Fp Width { get; set; }
        public Fp Height { get; set; }
    }

    public struct PolygonF
    {
        public Vec2F[] Vertices { get; set; }
        public int NumVertices { get; set; }
    }


    public struct CircleF
    {
        public Vec2F Center { get; set; }
        public Fp Radius { get; set; }
    }


    public struct Ray2dCollisionF
    {
        public bool Hit { get; set; }
        public Fp Distance { get; set; }
        public Vec2F Point { get; set; }
        public Vec2F Normal { get; set; }
    }

    public struct CollisionF
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Ray2dCollisionF GetRayRectangle(Ray2dF ray2d, RectangleF rectangle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Ray2dCollisionF GetRayRectanglex(Ray2dF ray2d, RectangleF rectangle, Fp angle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool Circles(CircleF a, CircleF b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool Rectanglex(CircleF a, CircleF b);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool Polygons(PolygonF a, PolygonF b);
    }
}
