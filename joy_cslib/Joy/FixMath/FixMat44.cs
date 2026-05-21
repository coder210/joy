namespace Livlib.FixMath
{
    public class FixMat44
    {
        public Fix64 m0, m4, m8, m12;  // Matrix first row (4 components)
        public Fix64 m1, m5, m9, m13;  // Matrix second row (4 components)
        public Fix64 m2, m6, m10, m14; // Matrix third row (4 components)
        public Fix64 m3, m7, m11, m15; // Matrix fourth row (4 components)

        public FixMat44()
        {
            this.m0 = this.m4 = this.m8 = this.m12 = 0.0f;
            this.m1 = this.m5 = this.m9 = this.m13 = 0.0f;
            this.m2 = this.m6 = this.m10 = this.m14 = 0.0f;
            this.m3 = this.m7 = this.m11 = this.m15 = 0.0f;
        }

        public FixMat44(Fix64 m0, Fix64 m4, Fix64 m8, Fix64 m12, 
            Fix64 m1, Fix64 m5, Fix64 m9, Fix64 m13, 
            Fix64 m2, Fix64 m6, Fix64 m10, Fix64 m14,
            Fix64 m3, Fix64 m7, Fix64 m11, Fix64 m15)
        {
            this.m0 = m0; this.m4 = m4;  this.m8 = m8; this.m12 = m12;
            this.m1 = m1; this.m5 = m5; this.m9 = m9; this.m13 = m13;
            this.m2 = m2; this.m6 = m6; this.m10 = m10; this.m14 = m14;
            this.m3 = m3; this.m7 = m7; this.m11 = m11; this.m15 = m15;
        }

        public static FixMat44 Multiply(FixMat44 left, FixMat44 right)
        {
            FixMat44 result = new FixMat44();
            result.m0 = left.m0 * right.m0 + left.m1 * right.m4 + left.m2 * right.m8 + left.m3 * right.m12;
            result.m1 = left.m0 * right.m1 + left.m1 * right.m5 + left.m2 * right.m9 + left.m3 * right.m13;
            result.m2 = left.m0 * right.m2 + left.m1 * right.m6 + left.m2 * right.m10 + left.m3 * right.m14;
            result.m3 = left.m0 * right.m3 + left.m1 * right.m7 + left.m2 * right.m11 + left.m3 * right.m15;
            result.m4 = left.m4 * right.m0 + left.m5 * right.m4 + left.m6 * right.m8 + left.m7 * right.m12;
            result.m5 = left.m4 * right.m1 + left.m5 * right.m5 + left.m6 * right.m9 + left.m7 * right.m13;
            result.m6 = left.m4 * right.m2 + left.m5 * right.m6 + left.m6 * right.m10 + left.m7 * right.m14;
            result.m7 = left.m4 * right.m3 + left.m5 * right.m7 + left.m6 * right.m11 + left.m7 * right.m15;
            result.m8 = left.m8 * right.m0 + left.m9 * right.m4 + left.m10 * right.m8 + left.m11 * right.m12;
            result.m9 = left.m8 * right.m1 + left.m9 * right.m5 + left.m10 * right.m9 + left.m11 * right.m13;
            result.m10 = left.m8 * right.m2 + left.m9 * right.m6 + left.m10 * right.m10 + left.m11 * right.m14;
            result.m11 = left.m8 * right.m3 + left.m9 * right.m7 + left.m10 * right.m11 + left.m11 * right.m15;
            result.m12 = left.m12 * right.m0 + left.m13 * right.m4 + left.m14 * right.m8 + left.m15 * right.m12;
            result.m13 = left.m12 * right.m1 + left.m13 * right.m5 + left.m14 * right.m9 + left.m15 * right.m13;
            result.m14 = left.m12 * right.m2 + left.m13 * right.m6 + left.m14 * right.m10 + left.m15 * right.m14;
            result.m15 = left.m12 * right.m3 + left.m13 * right.m7 + left.m14 * right.m11 + left.m15 * right.m15;
            return result;
        }

        public static FixVec4 Multiply(FixMat44 mat, FixVec4 q)
        {
            FixVec4 result = new FixVec4();
            result.X = mat.m0 * q.X + mat.m4 * q.Y + mat.m8 * q.Z + mat.m12 * q.W;
            result.Y = mat.m1 * q.X + mat.m5 * q.Y + mat.m9 * q.Z + mat.m13 * q.W;
            result.Z = mat.m2 * q.X + mat.m6 * q.Y + mat.m10 * q.Z + mat.m14 * q.W;
            result.W = mat.m3 * q.X + mat.m7 * q.Y + mat.m11 * q.Z + mat.m15 * q.W;
            return result;
        }

        public static FixMat44 Translate(Fix64 x, Fix64 y, Fix64 z)
        {
            // <2,3,4,1>
            FixMat44 result = new FixMat44(1.0f, 0.0f, 0.0f, x,
                      0.0f, 1.0f, 0.0f, y,
                      0.0f, 0.0f, 1.0f, z,
                      0.0f, 0.0f, 0.0f, 1.0f);
            return result;
        }

        public static FixMat44 RotateX(Fix64 angle)
        {
            FixMat44 result = new FixMat44(1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f); // MatrixIdentity()

            Fix64 cosres = Fix64.Cos(angle);
            Fix64 sinres = Fix64.Sin(angle);

            result.m5 = cosres;
            result.m6 = sinres;
            result.m9 = -sinres;
            result.m10 = cosres;

            return result;
        }

        public static FixMat44 RotateZ(Fix64 angle)
        {
            FixMat44 result = new FixMat44(1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f ); // MatrixIdentity()

            Fix64 cosres = Fix64.Cos(angle);
            Fix64 sinres = Fix64.Sin(angle);

            result.m0 = cosres;
            result.m1 = sinres;
            result.m4 = -sinres;
            result.m5 = cosres;

            return result;
        }

        public static FixMat44 RotateY(Fix64 angle)
        {
            FixMat44 result = new FixMat44(1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f); // MatrixIdentity()

            Fix64 cosres = Fix64.Cos(angle);
            Fix64 sinres = Fix64.Sin(angle);

            result.m0 = cosres;
            result.m2 = -sinres;
            result.m8 = sinres;
            result.m10 = cosres;

            return result;
        }

        public static FixMat44 Rotate(FixVec3 axis, Fix64 angle)
        {
            FixMat44 result = new FixMat44();

            Fix64 x = axis.X, y = axis.Y, z = axis.Z;

            Fix64 lengthSquared = x * x + y * y + z * z;

            if ((lengthSquared != Fix64.One) && (lengthSquared != Fix64.Zero))
            {
                Fix64 ilength = Fix64.One / Fix64.Sqrt(lengthSquared);
                x = x * ilength;
                y = y * ilength;
                z = z * ilength;
            }

            Fix64 sinres = Fix64.Sin(angle);
            Fix64 cosres = Fix64.Cos(angle);
            Fix64 t = Fix64.One - cosres;

            result.m0 = x * x * t + cosres;
            result.m1 = y * x * t + z * sinres;
            result.m2 = z * x * t - y * sinres;
            result.m3 = 0.0f;

            result.m4 = x * y * t - z * sinres;
            result.m5 = y * y * t + cosres;
            result.m6 = z * y * t + x * sinres;
            result.m7 = 0.0f;

            result.m8 = x * z * t + y * sinres;
            result.m9 = y * z * t - x * sinres;
            result.m10 = z * z * t + cosres;
            result.m11 = 0.0f;

            result.m12 = 0.0f;
            result.m13 = 0.0f;
            result.m14 = 0.0f;
            result.m15 = 1.0f;

            return result;
        }

        public static FixMat44 Scale(Fix64 x, Fix64 y, Fix64 z)
        {
            FixMat44 result = new FixMat44(x, 0.0f, 0.0f, 0.0f,
                      0.0f, y, 0.0f, 0.0f,
                      0.0f, 0.0f, z, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f);
            return result;
        }
    }
}
