using Livlib.FixMath;
using System.Collections.Generic;
using System.Linq;

namespace Livlib.Collision
{
    public enum ShapeType
    {
        Shpere,
        Box
    }

    public abstract class FixGjk3DShape
    {
        public abstract ShapeType GetShapeType();
	    public abstract FixVec3 FindFurthestPoint(FixVec3 direction);
        public abstract void UpdateVertices(FixVec3 axis, Fix64 angle, FixVec3 position);
        public abstract FixVec3 GetFurthestPoint(FixVec3 direction);
    }

    public class FixGjk3DSphere : FixGjk3DShape
    {
        private Fix64 m_radius;
        private FixVec3 m_position;
        public FixGjk3DSphere(Fix64 radius)
        {
            this.m_radius = radius;
            this.m_position = new FixVec3();
        }

        public override ShapeType GetShapeType(){
            return ShapeType.Shpere;
        }

        public Fix64 GetRadius()
        {
            return this.m_radius;
        }

        public override FixVec3 FindFurthestPoint(FixVec3 direction)
        {
            // local to world
            // after: used martix
            direction = FixVec3.Normalize(direction);
            FixVec3 maxPoint = this.m_position + (direction * m_radius);
            return maxPoint;
        }

        public override FixVec3 GetFurthestPoint(FixVec3 direction)
        {
            direction = FixVec3.Normalize(direction);
            FixVec3 maxPoint = this.m_position + (direction * m_radius);
            return maxPoint;
        }

        public override void UpdateVertices(FixVec3 axis, Fix64 angle, FixVec3 position)
        {
            this.m_position = position;
        }
    }

    public abstract class MeshShape : FixGjk3DShape
    {
        protected List<FixVec3> vertices;
        protected List<FixVec3> localVertices;
        protected FixVec3 m_position;
        public MeshShape()
        {
            this.vertices = new List<FixVec3>();
            this.localVertices = new List<FixVec3>();
        }

        public override FixVec3 FindFurthestPoint(FixVec3 direction)
        {
            FixVec3 maxPoint = new FixVec3();
            Fix64 maxDistance = Fix64.MinValue;
            for (int i = 0; i< vertices.Count(); i++) 
            {
                FixVec3 vertex = vertices[i];
                Fix64 distance = FixVec3.Dot(vertex, direction);
                if (distance > maxDistance) 
                {
                    maxDistance = distance;
                    maxPoint = vertex;
                }
            }
            return maxPoint;
        }

        public override void UpdateVertices(FixVec3 axis, Fix64 angle, FixVec3 position)
        {
            this.m_position = position;
            FixMat44 translateTransform = FixMat44.Translate(position.X, position.Y, position.Z);
            FixMat44 rotateTransform = FixMat44.Rotate(axis, angle);
            FixMat44 complexTransform = FixMat44.Multiply(rotateTransform, translateTransform);
            //std::cout << "=================" << std::endl;
            for (int i = 0; i < localVertices.Count(); i++)
            {
                //FixMat44 rotateTransform = FixMat44Rotate(axis, angle);
                FixVec4 result = FixMat44.Multiply(complexTransform, new FixVec4(localVertices[i]));
                vertices[i] = new FixVec3(result.X, result.Y, result.Z);
                //std::cout << "<" << (float)result.x << "," << (float)result.y << "," << (float)result.z << "," << (float)result.w << ">" << std::endl;
                //result = localVertices[i] + position;
                //std::cout << "  " << "<" << (float)result.x << "," << (float)result.y << "," << (float)result.z << "," << (float)result.w << ">" << std::endl;
            }
            //for (int i = 0; i <localVertices.Count(); i++)
            //{
            //    vertices[i] = localVertices[i] + this.m_position;
            //}
            //std::cout << "=================" << std::endl;
        }

        public override FixVec3 GetFurthestPoint(FixVec3 direction)
        {
            FixVec3 maxPoint = new FixVec3();
            Fix64 maxDistance = Fix64.MinValue;
            for (int i = 0; i < vertices.Count(); i++)
            {
                FixVec3 vertex = vertices[i];
                Fix64 distance = FixVec3.Dot(vertex, direction);
                if (distance > maxDistance)
                {
                    maxDistance = distance;
                    maxPoint = vertex;
                }
            }
            return maxPoint;
        }

        public List<FixVec3> GetVertices()
        {
            return this.vertices;
        }
    }


    public class BoxShape : MeshShape
    {
        private FixVec3 halfSize;
        public override ShapeType GetShapeType()
        {
            return ShapeType.Box;
        }

        public FixVec3 GetHalfSize()
        {
            return this.halfSize;
        }

        public BoxShape(FixVec3 halfSize)
        {
            this.halfSize = halfSize;
            // top
            localVertices.Add(new FixVec3(-halfSize.X, halfSize.Y, -halfSize.Z));
            localVertices.Add(new FixVec3(halfSize.X, halfSize.Y, -halfSize.Z));
            localVertices.Add(new FixVec3(halfSize.X, halfSize.Y, halfSize.Z));
            localVertices.Add(new FixVec3(-halfSize.X, halfSize.Y, halfSize.Z));
            // bottom
            localVertices.Add(new FixVec3(-halfSize.X, -halfSize.Y, -halfSize.Z));
            localVertices.Add(new FixVec3(halfSize.X, -halfSize.Y, -halfSize.Z));
            localVertices.Add(new FixVec3(halfSize.X, -halfSize.Y, halfSize.Z));
            localVertices.Add(new FixVec3(-halfSize.X, -halfSize.Y, halfSize.Z));
            for (int i = 0; i < localVertices.Count(); i++)
            {
                vertices.Add(localVertices[i]);
            }
        }
    }
}
