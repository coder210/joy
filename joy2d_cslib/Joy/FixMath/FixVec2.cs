namespace Livlib.FixMath
{
    public struct FixVec2
    {
        public Fix64 X { get; set; }
        public Fix64 Y { get; set; }

        public static FixVec2 Zero = new FixVec2(0, 0);
        public static FixVec2 One = new FixVec2(1, 1);

        public FixVec2(Fix64 x, Fix64 y)
        {
            this.X = x;
            this.Y = y;
        }

        public void Add(FixVec2 v)
        {
            this.X += v.X;
            this.Y += v.Y;
        }

        public void Sub(FixVec2 v)
        {
            this.X -= v.X;
            this.Y -= v.Y;
        }

        public void Scale(Fix64 n)
        {
            this.X *= n;
            this.Y *= n;
        }

        public FixVec2 Rotate(Fix64 angle)
        {
            FixVec2 result = new FixVec2();
            result.X = X * Fix64.Cos(angle) - Y * Fix64.Sin(angle);
            result.Y = X * Fix64.Sin(angle) + Y * Fix64.Cos(angle);
            return result;
        }

        public Fix64 Magnitude()
        {
            return Fix64.Sqrt(X * X + Y * Y);
        }

        public Fix64 MagnitudeSquared()
        {
            return X * X + Y * Y;
        }

        public FixVec2 Normalize()
        {
            Fix64 length = Magnitude();
            if (length != Fix64.Zero)
            {
                X /= length;
                Y /= length;
            }
            return new FixVec2(X, Y);
        }

        public FixVec2 UnitVector()
        {
            FixVec2 result = new FixVec2(0, 0);
            Fix64 length = Magnitude();
            if (length != Fix64.Zero)
            {
                result.X = X / length;
                result.Y = Y / length;
            }
            return result;
        }

        public FixVec2 Normal()
        {
            return new FixVec2(Y, -X).Normalize();
        }

        public Fix64 Dot(FixVec2 v)
        {
            return X * v.X + Y * v.Y;
        }

        public Fix64 Cross(FixVec2 v)
        {
            return X * v.Y - Y * v.X;
        }

        public void Negate()
        {
            X = -X;
            Y = -Y;
        }

        public override bool Equals(object obj)
        {
            if (obj is FixVec2)
            {
                FixVec2 v = (FixVec2)obj;
                return v == this;
            }
            return false;
        }

        public override int GetHashCode()
        {
            return X.GetHashCode() + Y.GetHashCode();
        }

        public override string ToString()
        {
            return "{" + X + ", " + Y + "}";
        }

        public static bool operator ==(FixVec2 left, FixVec2 right)
        {
            return left.X == right.X && left.Y == right.Y;
        }

        public static bool operator !=(FixVec2 left, FixVec2 right)
        {
            return left.X != right.X || left.Y != right.Y;
        }

        public static FixVec2 operator +(FixVec2 left, FixVec2 right)
        {
            FixVec2 result = new FixVec2();
            result.X = left.X + right.X;
            result.Y = left.Y + right.Y;
            return result;
        }

        public static FixVec2 operator -(FixVec2 left, FixVec2 right)
        {
            FixVec2 result = new FixVec2();
            result.X = left.X - right.X;
            result.Y = left.Y - right.Y;
            return result;
        }

        public static FixVec2 operator *(FixVec2 left, Fix64 n)
        {
            FixVec2 result = new FixVec2();
            result.X = left.X * n;
            result.Y = left.Y * n;
            return result;
        }

        public static FixVec2 operator /(FixVec2 left, Fix64 n)
        {
            FixVec2 result = new FixVec2();
            result.X = left.X / n;
            result.Y = left.Y / n;
            return result;
        }

        public static FixVec2 operator -(FixVec2 v)
        {
            v.X = -v.X;
            v.Y = -v.Y;
            return v;
        }
    }
}
