using System;

namespace Livlib.FixMath
{
    public struct FixVec4 : IEquatable<FixVec4>
    {
        public Fix64 X { get; set; }
        public Fix64 Y { get; set; }
        public Fix64 Z { get; set; }
        public Fix64 W { get; set; }

        public FixVec4(Fix64 x, Fix64 y, Fix64 z, Fix64 w)
        {
            this.X = x;
            this.Y = y;
            this.Z = z;
            this.W = w;
        }

        public FixVec4(FixVec3 vec3)
        {
            this.X = vec3.X;
            this.Y = vec3.Y;
            this.Z = vec3.Z;
            this.W = Fix64.One;
        }

        public bool Equals(FixVec4 other)
        {
            return this.X == other.X && this.Y == other.Y && this.Z == other.Z && this.W == other.W;
        }
    }
}
