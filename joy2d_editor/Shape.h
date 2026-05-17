//#ifndef SHAPE_H
//#define SHAPE_H
//#include "T_Math.h"
//#include <vector>
//#include <iostream>
//
//enum ShapeType {
//	SHPERE,
//	BOX
//};
//
//struct Shape {
//protected:
//	ShapeType m_shapeType;
//public:
//	virtual ShapeType GetShapeType() = 0;
//	virtual Vec3 FindFurthestPoint(Vec3 direction) = 0;
//	virtual void UpdateVertices(Vec3 axis, FP angle, const Vec3& position) = 0;
//};
//
//struct SphereShape : Shape
//{
//private:
//	FP m_radius;
//	Vec3 m_position;
//public:
//	SphereShape(FP radius) {
//		this->m_shapeType = SHPERE;
//		this->m_radius = radius;
//		this->m_position = { 0.0f, 0.0f, 0.0f };
//	}
//public:
//	ShapeType GetShapeType() override {
//		return this->m_shapeType;
//	}
//
//	FP GetRadius() {
//		return this->m_radius;
//	}
//
//	Vec3 FindFurthestPoint(Vec3 direction) override
//	{
//		// local to world
//		// after: used martix
//		direction = Vec3Normalize(direction);
//		Vec3 maxPoint = this->m_position + (direction * m_radius);
//		return maxPoint;
//	}
//
//	void UpdateVertices(Vec3 axis, FP angle, const Vec3& position) override {
//		this->m_position = position;
//	}
//};
//
//struct MeshShape : Shape
//{
//protected:
//	std::vector<Vec3> m_vertices;
//	std::vector<Vec3> m_localVertices;
//public:
//	Vec3 FindFurthestPoint(Vec3 direction) override
//	{
//		Vec3 maxPoint = { 0.0f, 0.0f, 0.0f };
//		FP maxDistance = -FLT_MAX;
//		for (int i = 0; i < m_vertices.size(); i++) {
//			Vec3 vertex = m_vertices[i];
//			FP distance = Vec3Dot(vertex, direction);
//			if (distance > maxDistance) {
//				maxDistance = distance;
//				maxPoint = vertex;
//			}
//		}
//		return maxPoint;
//	}
//
//	void UpdateVertices(Vec3 axis, FP angle, const Vec3& position) override {
//		Mat4 translateTransform = Mat4Translate(position.x, position.y, position.z);
//		Mat4 rotateTransform = Mat4Rotate(axis, angle);
//		Mat4 complexTransform = Mat4Multiply(rotateTransform, translateTransform);
//		//std::cout << "=================" << std::endl;
//		for (int i = 0; i < m_localVertices.size(); i++) {
//		//Mat4 rotateTransform = Mat4Rotate(axis, angle);
//			Vec4 result = Mat4Multiply(complexTransform, m_localVertices[i]);
//			m_vertices[i] = { result.x, result.y, result.z };
//			//std::cout << "<" << (float)result.x << "," << (float)result.y << "," << (float)result.z << "," << (float)result.w << ">" << std::endl;
//			//result = m_localVertices[i] + position;
//			//std::cout << "  " << "<" << (float)result.x << "," << (float)result.y << "," << (float)result.z << "," << (float)result.w << ">" << std::endl;
//		}
//		//std::cout << "=================" << std::endl;
//	}
//
//	std::vector<Vec3> GetVertices() {
//		return this->m_vertices;
//	}
//};
//
//struct BoxShape : MeshShape
//{
//private:
//	Vec3 m_HalfSize;
//
//public:
//	ShapeType GetShapeType() override {
//		return this->m_shapeType;
//	}
//
//	Vec3 GetHalfSize() {
//		return this->m_HalfSize;
//	}
//
//	BoxShape(Vec3 halfSize) {
//		this->m_shapeType = BOX;
//		this->m_HalfSize = halfSize;
//		// top
//		m_localVertices.push_back({ -halfSize.x, halfSize.y, -halfSize.z });
//		m_localVertices.push_back({ halfSize.x, halfSize.y, -halfSize.z });
//		m_localVertices.push_back({ halfSize.x, halfSize.y, halfSize.z });
//		m_localVertices.push_back({ -halfSize.x, halfSize.y, halfSize.z });
//		// bottom
//		m_localVertices.push_back({ -halfSize.x, -halfSize.y, -halfSize.z });
//		m_localVertices.push_back({ halfSize.x, -halfSize.y, -halfSize.z });
//		m_localVertices.push_back({ halfSize.x, -halfSize.y, halfSize.z });
//		m_localVertices.push_back({ -halfSize.x, -halfSize.y, halfSize.z });
//
//		for (int i = 0; i < m_localVertices.size(); i++) {
//			m_vertices.push_back(m_localVertices[i]);
//		}
//	};
//};
//
//
//#endif // !SHAPE_H
