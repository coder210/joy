using Livlib.FixMath;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Livlib.Collision
{
    public class Simplex
    {
        private FixVec3[] m_points;
        private int m_size;

        public Simplex()
        {
            this.m_points = new FixVec3[4];
            this.m_size = 0;
        }

        public FixVec3 this[int index]
        {
            get
            {
                return this.m_points[index];
            }
        }

        public Simplex(FixVec3[] points)
        {
            this.m_points = new FixVec3[4];
            this.m_size = 0;
            foreach (FixVec3 point in points)
            {
                this.m_points[m_size++] = point;
            }
        }

        public void push_front(FixVec3 point)
        {
            FixVec3[] points = {
                point, m_points[0], m_points[1], m_points[2]
            };
            this.m_points = points;
            this.m_size = this.m_size + 1 > 4 ? 4 : this.m_size + 1;
        }

        public int size() { return m_size; }
    }


    public struct Contact
    {
        public FixVec3 Start;
        public FixVec3 End;
        public FixVec3 Normal; // End - Start normalized
        public Fix64 Depth;
    };

    public class FixGjk3D
    {
        public static FixVec3 Support(FixGjk3DShape a, FixGjk3DShape b, FixVec3 direction)
        {
            FixVec3 aFurthestPoint = a.FindFurthestPoint(direction);
            FixVec3 bFurthestPoint = b.FindFurthestPoint(-direction);
            return aFurthestPoint - bFurthestPoint;
        }

        public static bool SameDirection(FixVec3 direction, FixVec3 ao)
        {
            return FixVec3.Dot(direction, ao) > Fix64.Zero;
        }

        public static Tuple<List<FixVec4>, int> GetFaceNormals(List<FixVec3> polytope, List<int> faces)
        {
            List<FixVec4> normals = new List<FixVec4>();
            int minTriangle = 0;
            int minNormal = 0;
            Fix64 minDistance = Fix64.MaxValue;

            for (int i = 0; i < faces.Count(); i += 3)
            {
                FixVec3 a = polytope[faces[i]];
                FixVec3 b = polytope[faces[i + 1]];
                FixVec3 c = polytope[faces[i + 2]];

                FixVec3 ba = FixVec3.Normalize(b - a);
                FixVec3 ca = FixVec3.Normalize(c - a);
                FixVec3 normal = FixVec3.Normalize(FixVec3.Cross(ba, ca));
                var distance = FixVec3.Dot(normal, a);
                if (distance < Fix64.Zero)
                {
                    normal = -normal;
                    distance = -distance;
                }

                normals.Add(new FixVec4(normal.X, normal.Y, normal.Z, distance));

                if (distance < minDistance)
                {
                    minTriangle = i / 3;
                    minNormal = normals.Count() - 1;
                    minDistance = distance;
                }
            }

            return new Tuple<List<FixVec4>, int>(normals, minNormal);
        }

        public static void AddIfUniqueEdge(List<Tuple<int, int>> edges, List<int> faces, int a, int b)
        {
            //      0--<--3
            //     / \ B /   A: 2-0
            //    / A \ /    B: 0-2
            //   1-->--2
            List<Tuple<int, int>> removedEdges = new List<Tuple<int, int>>();
            for (int i = 0; i < edges.Count(); i++)
            {
                var edge = edges[i];
                if (edge.Item1 == faces[b] && edge.Item2 == faces[a])
                {
                    removedEdges.Add(edge);
                }
            }

            if (removedEdges.Count() > 0)
            {
                foreach (var item in removedEdges)
                {
                    edges.Remove(item);
                }
            }
            else
            {
                edges.Add(new Tuple<int, int>(faces[a], faces[b]));
            }
        }

        static bool Line(ref Simplex points, ref FixVec3 direction)
        {
            FixVec3 a = points[0];
            FixVec3 b = points[1];

            FixVec3 ab = FixVec3.Normalize(b - a);
            FixVec3 ao = FixVec3.Normalize(-a);

            if (SameDirection(ab, ao))
            {
                direction = FixVec3.Normalize(FixVec3.Cross(FixVec3.Cross(ab, ao), ab));
            }
            else
            {
                points = new Simplex(new FixVec3[] { a });
                direction = ao;
            }

            return false;
        }


        public static bool Triangle(ref Simplex points, ref FixVec3 direction)
        {
            FixVec3 a = points[0];
            FixVec3 b = points[1];
            FixVec3 c = points[2];

            FixVec3 ab = FixVec3.Normalize(b - a);
            FixVec3 ac = FixVec3.Normalize(c - a);
            FixVec3 ao = FixVec3.Normalize(-a);

            FixVec3 abc = FixVec3.Normalize(FixVec3.Cross(ab, ac));

            if (SameDirection(FixVec3.Cross(abc, ac), ao))
            {
                if (SameDirection(ac, ao))
                {
                    points = new Simplex(new FixVec3[] { a, c });
                    direction = FixVec3.Normalize(FixVec3.Cross(FixVec3.Cross(ac, ao), ac));
                }
                else
                {
                    points = new Simplex(new FixVec3[] { a, b });
                    return Line(ref points, ref direction);
                }
            }
            else
            {
                if (SameDirection(FixVec3.Cross(ab, abc), ao))
                {
                    points = new Simplex(new FixVec3[] { a, b });
                    return Line(ref points, ref direction);
                }
                else
                {
                    if (SameDirection(abc, ao))
                    {
                        direction = abc;
                    }
                    else
                    {
                        points = new Simplex(new FixVec3[] { a, c, b });
                        direction = -abc;
                    }
                }
            }
            return false;
        }


        public static bool Tetrahedron(ref Simplex points, ref FixVec3 direction)
        {
            FixVec3 a = points[0];
            FixVec3 b = points[1];
            FixVec3 c = points[2];
            FixVec3 d = points[3];

            FixVec3 ab = FixVec3.Normalize(b - a);
            FixVec3 ac = FixVec3.Normalize(c - a);
            FixVec3 ad = FixVec3.Normalize(d - a);
            FixVec3 ao = FixVec3.Normalize(-a);

            FixVec3 abc = FixVec3.Normalize(FixVec3.Cross(ab, ac));
            FixVec3 acd = FixVec3.Normalize(FixVec3.Cross(ac, ad));
            FixVec3 adb = FixVec3.Normalize(FixVec3.Cross(ad, ab));

            if (SameDirection(abc, ao))
            {
                points = new Simplex(new FixVec3[] { a, b, c });
                return Triangle(ref points, ref direction);
            }

            if (SameDirection(acd, ao))
            {
                points = new Simplex(new FixVec3[] { a, c, d });
                return Triangle(ref points, ref direction);
            }

            if (SameDirection(adb, ao))
            {
                points = new Simplex(new FixVec3[] { a, d, b });
                return Triangle(ref points, ref direction);
            }

            return true;
        }

        public static bool NextSimplex(ref Simplex points, ref FixVec3 direction)
        {
            switch (points.size())
            {
                case 2: return Line(ref points, ref direction);
                case 3: return Triangle(ref points, ref direction);
                case 4: return Tetrahedron(ref points, ref direction);
            }
            // never should be here
            return false;
        }

        public static bool Collide(FixGjk3DShape a, FixGjk3DShape b, FixVec3 initalDir, out Contact contact)
        {
            // Get initial support point in any direction
            FixVec3 support = Support(a, b, initalDir);
            Simplex points = new Simplex();

            // Simplex is an array of points, max count is 4
            points.push_front(support);

            // New direction is towards the origin
            FixVec3 direction = -support;
            while (true)
            {
                support = Support(a, b, direction);
                if (FixVec3.Dot(support, direction) < Fix64.Zero)
                {
                    contact = new Contact();
                    return false; // no collision
                }
                points.push_front(support);
                if (NextSimplex(ref points, ref direction))
                {
                    contact = Epa(points, a, b);
                    return true;
                }
            }
        }


        static Contact Epa(Simplex simplex, FixGjk3DShape a, FixGjk3DShape b)
        {
      
            List<FixVec3> polytope = new List<FixVec3>();
            for (int i = 0; i < simplex.size(); i++)
                polytope.Add(simplex[i]);

            List<int> faces = new List<int>() {
                0, 1, 2, // <0,1> <1,2> <2, 0>
                0, 3, 1, // <0,3> <3,1> <1,0>
                0, 2, 3, // <0,2> <2,3> <3,0>
                1, 3, 2  // <1,3> <3,2> <2,1>
            };

            var faceNormals = GetFaceNormals(polytope, faces);
            var normals = faceNormals.Item1;
            var minFace = faceNormals.Item2;

            FixVec3 bakeMinNormal = new FixVec3(normals[minFace].X, normals[minFace].Y, normals[minFace].Z);
            Fix64 bakeMinDistance = normals[minFace].W;

            FixVec3 minNormal = new FixVec3();
            Fix64 minDistance = Fix64.MaxValue;

            while (minDistance == Fix64.MaxValue)
            {
                minNormal = new FixVec3(normals[minFace].X, normals[minFace].Y, normals[minFace].Z);
                minDistance = normals[minFace].W;
                FixVec3 support = Support(a, b, minNormal);
                Fix64 sDistance = FixVec3.Dot(minNormal, support);
                if (Fix64.Abs(sDistance - minDistance) > (Fix64)0.001f)
                {
                    minDistance = Fix64.MaxValue;
                    List<Tuple<int, int>> uniqueEdges = new List<Tuple<int, int>>();

                    for (int i = 0; i < normals.Count(); i++)
                    {
                        // 此线比较两个平行平面的最短距离和原点，通过一个强制点（支撑顶点与当前面的点）。如果支撑点的距离较小，则结果将是一个凹面多面体，这是不需要的。
                        FixVec3 direction = new FixVec3(normals[i].X, normals[i].Y, normals[i].Z);
                        int f = i * 3;
                        Fix64 aPoint = FixVec3.Dot(direction, support);
                        Fix64 bPoint = FixVec3.Dot(direction, polytope[faces[f]]);
                        if (aPoint > bPoint)
                        {
                            AddIfUniqueEdge(uniqueEdges, faces, f, f + 1);
                            AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
                            AddIfUniqueEdge(uniqueEdges, faces, f + 2, f);

                            faces[f + 2] = faces.Last(); faces.RemoveAt(faces.Count() - 1);
                            faces[f + 1] = faces.Last(); faces.RemoveAt(faces.Count() - 1);
                            faces[f] = faces.Last(); faces.RemoveAt(faces.Count() - 1);

                            normals[i] = normals.Last(); // pop-erase
                            normals.RemoveAt(normals.Count() - 1);

                            i--;
                        }
                    }

                    List<int> newFaces = new List<int>();
                    for (int i = 0; i < uniqueEdges.Count(); i++)
                    {
                        int edgeIndex1 = uniqueEdges[i].Item1;
                        int edgeIndex2 = uniqueEdges[i].Item2;
                        newFaces.Add(edgeIndex1);
                        newFaces.Add(edgeIndex2);
                        newFaces.Add(polytope.Count());
                    }

                    polytope.Add(support);

                    var newFaceNormals = GetFaceNormals(polytope, newFaces);
                    var newNormals = newFaceNormals.Item1;
                    var newMinFace = newFaceNormals.Item2;

                    Fix64 oldMinDistance = Fix64.MaxValue;
                    for (int i = 0; i < normals.Count(); i++)
                    {
                        if (normals[i].W < oldMinDistance)
                        {
                            oldMinDistance = normals[i].W;
                            minFace = i;
                        }
                    }

                    if (newNormals.Count() <= 0)
                    {
                        //Raylib.TraceLog(TraceLogLevel.Error, "这是一条ERROR级别的日志信息");
                        minNormal = bakeMinNormal;
                        minDistance = bakeMinDistance;
                        break;
                    }

                    if (newNormals[newMinFace].W < oldMinDistance)
                    {
                        minFace = newMinFace + normals.Count();
                    }

                    faces.AddRange(newFaces);
                    normals.AddRange(newNormals);
                }
            }
            Contact contact = new Contact();
            contact.Normal = minNormal;
            contact.Depth = minDistance;
            contact.Start = a.GetFurthestPoint(minNormal);
            contact.End = b.GetFurthestPoint(-minNormal);
            return contact;
        }
    }
}
