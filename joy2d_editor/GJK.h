//#ifndef GJK_H
//#define GJK_H
//#include <cstdint>
//#include <array>
//#include <vector>
//
//// ============ FP 定点数类型 ============
//struct FP {
//	FP();
//	FP(float value);
//	FP(int64_t value);
//	operator float() const;
//	operator int64_t() const;
//	FP operator-() const;
//	FP& operator+=(const FP& other);
//	FP& operator-=(const FP& other);
//	FP& operator*=(const FP& other);
//	FP& operator/=(const FP& other);
//	FP operator+(const FP& other) const;
//	FP operator-(const FP& other) const;
//	FP operator*(const FP& other) const;
//	FP operator/(const FP& other) const;
//	bool operator==(const FP& other);
//	bool operator!=(const FP& other);
//	bool operator>=(const FP& other);
//	bool operator<=(const FP& other);
//	bool operator>(const FP& other);
//	bool operator<(const FP& other);
//
//	static FP MinValue();
//	static FP MaxValue();
//	static FP Zero();
//	static FP Half();
//	static FP One();
//	static FP Max(FP a, FP b);
//	static FP Min(FP a, FP b);
//	static FP Pow2(FP x);
//	static int Sign(FP x);
//	static FP Clamp(FP a, FP low, FP high);
//	static FP Sqrt(FP x);
//	static FP Abs(FP x);
//	static FP Cos(FP x);
//	static FP Sin(FP x);
//
//private:
//	int64_t rawValue;
//};
//
//// ============ 3D/4D 向量和矩阵 ============
//struct Vec3 { FP x, y, z; };
//
//struct Vec4 {
//	FP x, y, z, w;
//	Vec4() : x(FP::Zero()), y(FP::Zero()), z(FP::Zero()), w(FP::Zero()) {}
//	Vec4(Vec3 vec3) : x(vec3.x), y(vec3.y), z(vec3.z), w(FP::One()) {}
//};
//
//struct Mat4 {
//	FP m0, m4, m8, m12;
//	FP m1, m5, m9, m13;
//	FP m2, m6, m10, m14;
//	FP m3, m7, m11, m15;
//};
//
//static inline FP Vec3Dot(const Vec3& a, const Vec3& b) {
//	return a.x * b.x + a.y * b.y + a.z * b.z;
//}
//static inline Vec3 Vec3Cross(const Vec3& a, const Vec3& b) {
//	return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
//}
//static inline FP Vec3SquareLength(const Vec3& a) { return a.x * a.x + a.y * a.y + a.z * a.z; }
//static inline FP Vec3Length(const Vec3& a) { return FP::Sqrt(Vec3SquareLength(a)); }
//static inline Vec3 Vec3Normalize(const Vec3& a) {
//	Vec3 r = { FP::Zero(), FP::Zero(), FP::Zero() };
//	FP length = Vec3Length(a);
//	if (length != FP::Zero()) {
//		FP invLength = FP::One() / length;
//		r.x = a.x * invLength; r.y = a.y * invLength; r.z = a.z * invLength;
//	}
//	return r;
//}
//static inline Vec3 operator-(Vec3 a) { return { -a.x, -a.y, -a.z }; }
//static inline Vec3 operator-(const Vec3& a, const Vec3& b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
//static inline Vec3 operator+(const Vec3& a, const Vec3& b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
//static inline Vec3 operator*(const Vec3& a, FP s) { return { a.x * s, a.y * s, a.z * s }; }
//
//static inline Mat4 Mat4Multiply(Mat4 left, Mat4 right);
//static inline Vec4 Mat4Multiply(Mat4 left, Vec4 right);
//static inline Mat4 Mat4Translate(FP x, FP y, FP z);
//static inline Mat4 Mat4Rotate(Vec3 axis, FP angle);
//static inline Mat4 Mat4Scale(FP x, FP y, FP z);
//
//// ============ GJK 碰撞检测类型 ============
//struct Normal { FP x, y, z, w; };
//struct Contact { Vec3 Normal; FP Depth; };
//
//struct Simplex {
//private:
//	std::array<Vec3, 4> m_points;
//	int m_size;
//public:
//	Simplex() : m_size(0) {}
//	Simplex& operator=(std::initializer_list<Vec3> list) {
//		m_size = 0;
//		for (Vec3 point : list) m_points[m_size++] = point;
//		return *this;
//	}
//	void push_front(Vec3 point) {
//		m_points = { point, m_points[0], m_points[1], m_points[2] };
//		m_size = std::min(m_size + 1, 4);
//	}
//	Vec3& operator[](int i) { return m_points[i]; }
//	size_t size() const { return m_size; }
//	auto begin() const { return m_points.begin(); }
//	auto end() const { return m_points.end() - (4 - m_size); }
//};
//
//enum ShapeType { SHPERE, BOX };
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
//struct SphereShape : Shape {
//private:
//	FP m_radius;
//	Vec3 m_position;
//public:
//	SphereShape(FP radius) { m_shapeType = SHPERE; m_radius = radius; m_position = { FP::Zero(), FP::Zero(), FP::Zero() }; }
//	ShapeType GetShapeType() override { return m_shapeType; }
//	FP GetRadius() { return m_radius; }
//	Vec3 FindFurthestPoint(Vec3 direction) override {
//		direction = Vec3Normalize(direction);
//		return m_position + (direction * m_radius);
//	}
//	void UpdateVertices(Vec3 axis, FP angle, const Vec3& position) override { m_position = position; }
//};
//
//struct MeshShape : Shape {
//protected:
//	std::vector<Vec3> m_vertices;
//	std::vector<Vec3> m_localVertices;
//public:
//	Vec3 FindFurthestPoint(Vec3 direction) override {
//		Vec3 maxPoint = { FP::Zero(), FP::Zero(), FP::Zero() };
//		FP maxDistance = -FLT_MAX;
//		for (size_t i = 0; i < m_vertices.size(); i++) {
//			FP distance = Vec3Dot(m_vertices[i], direction);
//			if (distance > maxDistance) { maxDistance = distance; maxPoint = m_vertices[i]; }
//		}
//		return maxPoint;
//	}
//	void UpdateVertices(Vec3 axis, FP angle, const Vec3& position) override {
//		Mat4 translateTransform = Mat4Translate(position.x, position.y, position.z);
//		Mat4 rotateTransform = Mat4Rotate(axis, angle);
//		Mat4 complexTransform = Mat4Multiply(rotateTransform, translateTransform);
//		for (size_t i = 0; i < m_localVertices.size(); i++) {
//			Vec4 result = Mat4Multiply(complexTransform, m_localVertices[i]);
//			m_vertices[i] = { result.x, result.y, result.z };
//		}
//	}
//	std::vector<Vec3> GetVertices() { return m_vertices; }
//};
//
//struct BoxShape : MeshShape {
//private:
//	Vec3 m_HalfSize;
//public:
//	ShapeType GetShapeType() override { return m_shapeType; }
//	Vec3 GetHalfSize() { return m_HalfSize; }
//	BoxShape(Vec3 halfSize) {
//		m_shapeType = BOX; m_HalfSize = halfSize;
//		m_localVertices = {
//			{ -halfSize.x, halfSize.y, -halfSize.z }, { halfSize.x, halfSize.y, -halfSize.z },
//			{ halfSize.x, halfSize.y, halfSize.z },   { -halfSize.x, halfSize.y, halfSize.z },
//			{ -halfSize.x, -halfSize.y, -halfSize.z },{ halfSize.x, -halfSize.y, -halfSize.z },
//			{ halfSize.x, -halfSize.y, halfSize.z },  { -halfSize.x, -halfSize.y, halfSize.z }
//		};
//		m_vertices = m_localVertices;
//	}
//};
//
//// ============ GJK 声明 ============
//bool Gjk(Shape* shapeA, Shape* shapeB, Vec3 initalDir, Contact* contect);
//bool Gjk(Shape* shapeA, Shape* shapeB, Contact* contect);
//
//#endif // !GJK_H
