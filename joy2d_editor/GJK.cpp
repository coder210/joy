//#include "GJK.h"
//
//#define FP_NUM_BITS 64
//#define FP_FRACTIONAL_PLACES 32
//#define FP_ONE (1LL << FP_FRACTIONAL_PLACES)
//#define FP_MAX_VALUE 9223372036854775807LL
//#define FP_MIN_VALUE -9223372036854775808LL
//#define FP_PI 0x3243F6A88
//#define FP_PI_OVER_2 0x1921FB544
//
//// ============ FP 实现 ============
//
//static int CountLeadingZeroes(uint64_t x) {
//	int result = 0;
//	while ((x & 0xF000000000000000) == 0) { result += 4; x <<= 4; }
//	while ((x & 0x8000000000000000) == 0) { result += 1; x <<= 1; }
//	return result;
//}
//
//FP::FP() : rawValue(0) {}
//FP::FP(float value) { rawValue = (int64_t)(FP_ONE * value); }
//FP::FP(int64_t value) : rawValue(value) {}
//FP::operator float() const { return (float)rawValue / FP_ONE; }
//FP::operator int64_t() const { return rawValue; }
//FP FP::operator-() const { return -rawValue; }
//
//FP& FP::operator+=(const FP& other) { rawValue += other.rawValue; return *this; }
//FP& FP::operator-=(const FP& other) { rawValue -= other.rawValue; return *this; }
//
//FP& FP::operator*=(const FP& other) {
//	int64_t xl = rawValue, yl = other.rawValue;
//	uint64_t xlo = (uint64_t)(xl & 0x00000000FFFFFFFF), ylo = (uint64_t)(yl & 0x00000000FFFFFFFF);
//	int64_t xhi = xl >> FP_FRACTIONAL_PLACES, yhi = yl >> FP_FRACTIONAL_PLACES;
//	uint64_t lolo = xlo * ylo;
//	int64_t lohi = (int64_t)xlo * yhi, hilo = xhi * (int64_t)ylo, hihi = xhi * yhi;
//	uint64_t loResult = lolo >> FP_FRACTIONAL_PLACES;
//	int64_t midResult1 = lohi, midResult2 = hilo, hiResult = hihi << FP_FRACTIONAL_PLACES;
//	rawValue = (int64_t)loResult + midResult1 + midResult2 + hiResult;
//	return *this;
//}
//
//FP& FP::operator/=(const FP& other) {
//	int64_t xl = rawValue, yl = other.rawValue;
//	if (yl == 0) { rawValue = 0; return *this; }
//	uint64_t remainder = (uint64_t)(xl >= 0 ? xl : -xl);
//	uint64_t divider = (uint64_t)(yl >= 0 ? yl : -yl);
//	uint64_t quotient = 0;
//	int bitPos = FP_NUM_BITS / 2 + 1;
//	while ((divider & 0xF) == 0 && bitPos >= 4) { divider >>= 4; bitPos -= 4; }
//	while (remainder != 0 && bitPos >= 0) {
//		int shift = CountLeadingZeroes(remainder);
//		if (shift > bitPos) shift = bitPos;
//		remainder <<= shift; bitPos -= shift;
//		uint64_t div = remainder / divider;
//		remainder = remainder % divider;
//		quotient += div << bitPos;
//		if ((div & ~(0xFFFFFFFFFFFFFFFF >> bitPos)) != 0) {
//			rawValue = ((xl ^ yl) & FP_MIN_VALUE) == 0 ? FP::MaxValue() : FP::MinValue();
//			return *this;
//		}
//		remainder <<= 1; --bitPos;
//	}
//	++quotient;
//	int64_t result = (int64_t)(quotient >> 1);
//	if (((xl ^ yl) & FP_MIN_VALUE) != 0) result = -result;
//	rawValue = result;
//	return *this;
//}
//
//FP FP::operator+(const FP& other) const { return rawValue + other.rawValue; }
//FP FP::operator-(const FP& other) const { return rawValue - other.rawValue; }
//
//FP FP::operator*(const FP& other) const {
//	int64_t xl = rawValue, yl = other.rawValue;
//	uint64_t xlo = (uint64_t)(xl & 0x00000000FFFFFFFF), ylo = (uint64_t)(yl & 0x00000000FFFFFFFF);
//	int64_t xhi = xl >> FP_FRACTIONAL_PLACES, yhi = yl >> FP_FRACTIONAL_PLACES;
//	uint64_t lolo = xlo * ylo;
//	int64_t lohi = (int64_t)xlo * yhi, hilo = xhi * (int64_t)ylo, hihi = xhi * yhi;
//	uint64_t loResult = lolo >> FP_FRACTIONAL_PLACES;
//	return (int64_t)loResult + lohi + hilo + (hihi << FP_FRACTIONAL_PLACES);
//}
//
//FP FP::operator/(const FP& other) const {
//	int64_t xl = rawValue, yl = other.rawValue;
//	if (yl == 0) return FP::Zero();
//	uint64_t remainder = (uint64_t)(xl >= 0 ? xl : -xl);
//	uint64_t divider = (uint64_t)(yl >= 0 ? yl : -yl);
//	uint64_t quotient = 0;
//	int bitPos = FP_NUM_BITS / 2 + 1;
//	while ((divider & 0xF) == 0 && bitPos >= 4) { divider >>= 4; bitPos -= 4; }
//	while (remainder != 0 && bitPos >= 0) {
//		int shift = CountLeadingZeroes(remainder);
//		if (shift > bitPos) shift = bitPos;
//		remainder <<= shift; bitPos -= shift;
//		uint64_t div = remainder / divider;
//		remainder = remainder % divider;
//		quotient += div << bitPos;
//		if ((div & ~(0xFFFFFFFFFFFFFFFF >> bitPos)) != 0)
//			return ((xl ^ yl) & FP_MIN_VALUE) == 0 ? FP::MaxValue() : FP::MinValue();
//		remainder <<= 1; --bitPos;
//	}
//	++quotient;
//	int64_t result = (int64_t)(quotient >> 1);
//	if (((xl ^ yl) & FP_MIN_VALUE) != 0) result = -result;
//	return result;
//}
//
//bool FP::operator==(const FP& other) { return rawValue == other.rawValue; }
//bool FP::operator!=(const FP& other) { return rawValue != other.rawValue; }
//bool FP::operator>=(const FP& other) { return rawValue >= other.rawValue; }
//bool FP::operator<=(const FP& other) { return rawValue <= other.rawValue; }
//bool FP::operator>(const FP& other) { return rawValue > other.rawValue; }
//bool FP::operator<(const FP& other) { return rawValue < other.rawValue; }
//
//FP FP::MinValue() { FP r; r.rawValue = FP_MIN_VALUE; return r; }
//FP FP::MaxValue() { FP r; r.rawValue = FP_MAX_VALUE; return r; }
//FP FP::Zero() { return FP(0.0f); }
//FP FP::Half() { return FP(0.5f); }
//FP FP::One() { return FP(1.0f); }
//
//FP FP::Abs(FP x) {
//	if (x == FP::MinValue()) return FP::MaxValue();
//	int64_t mask = x >> 63;
//	return (x.rawValue + mask) ^ mask;
//}
//FP FP::Max(FP a, FP b) { return a > b ? a : b; }
//FP FP::Min(FP a, FP b) { return a < b ? a : b; }
//FP FP::Pow2(FP x) { return x * x; }
//int FP::Sign(FP x) { return x < FP::Zero() ? -1 : 1; }
//FP FP::Clamp(FP a, FP low, FP high) { return FP::Max(low, FP::Min(a, high)); }
//
//FP FP::Sqrt(FP x) {
//	int64_t xl = x; if (xl < 0) return FP::Zero();
//	uint64_t num = (uint64_t)xl, result = 0, bit = (uint64_t)1 << (FP_NUM_BITS - 2);
//	while (bit > num) bit >>= 2;
//	for (int i = 0; i < 2; ++i) {
//		while (bit != 0) {
//			if (num >= result + bit) { num -= result + bit; result = (result >> 1) + bit; }
//			else { result >>= 1; }
//			bit >>= 2;
//		}
//		if (i == 0) {
//			if (num > ((uint64_t)1 << (FP_NUM_BITS / 2)) - 1) { num -= result; num = (num << (FP_NUM_BITS / 2)) - 0x80000000; result = (result << (FP_NUM_BITS / 2)) + 0x80000000; }
//			else { num <<= (FP_NUM_BITS / 2); result <<= (FP_NUM_BITS / 2); }
//			bit = (uint64_t)1 << (FP_NUM_BITS / 2 - 2);
//		}
//	}
//	if (num > result) ++result;
//	return (int64_t)result;
//}
//
//static int64_t ClampSinValue(int64_t angle, bool* flipHorizontal, bool* flipVertical) {
//	int64_t largePI = 7244019458077122842;
//	int64_t clamped2Pi = angle;
//	for (int i = 0; i < 29; ++i) clamped2Pi %= (largePI >> i);
//	if (angle < 0) clamped2Pi += 0x6487ED511; // FP_PI_TIMES_2
//	*flipVertical = clamped2Pi >= FP_PI;
//	int64_t clampedPi = clamped2Pi;
//	while (clampedPi >= FP_PI) clampedPi -= FP_PI;
//	*flipHorizontal = clampedPi >= FP_PI_OVER_2;
//	int64_t clampedPiOver2 = clampedPi;
//	if (clampedPiOver2 >= FP_PI_OVER_2) clampedPiOver2 -= FP_PI_OVER_2;
//	return clampedPiOver2;
//}
//
//FP FP::Sin(FP x) {
//	// NOTE: 需要 sin_lut 查找表（T_Lut.h）才能工作，当前暂缺
//	return FP::Zero();
//}
//FP FP::Cos(FP x) {
//	int64_t xl = x;
//	FP angle = xl + (xl > 0 ? -FP_PI - FP_PI_OVER_2 : FP_PI_OVER_2);
//	return FP::Sin(angle);
//}
//
//// ============ Mat4 实现 ============
//static inline Mat4 Mat4Multiply(Mat4 left, Mat4 right) {
//	Mat4 r = { FP::Zero() };
//	r.m0 = left.m0 * right.m0 + left.m1 * right.m4 + left.m2 * right.m8 + left.m3 * right.m12;
//	r.m1 = left.m0 * right.m1 + left.m1 * right.m5 + left.m2 * right.m9 + left.m3 * right.m13;
//	r.m2 = left.m0 * right.m2 + left.m1 * right.m6 + left.m2 * right.m10 + left.m3 * right.m14;
//	r.m3 = left.m0 * right.m3 + left.m1 * right.m7 + left.m2 * right.m11 + left.m3 * right.m15;
//	r.m4 = left.m4 * right.m0 + left.m5 * right.m4 + left.m6 * right.m8 + left.m7 * right.m12;
//	r.m5 = left.m4 * right.m1 + left.m5 * right.m5 + left.m6 * right.m9 + left.m7 * right.m13;
//	r.m6 = left.m4 * right.m2 + left.m5 * right.m6 + left.m6 * right.m10 + left.m7 * right.m14;
//	r.m7 = left.m4 * right.m3 + left.m5 * right.m7 + left.m6 * right.m11 + left.m7 * right.m15;
//	r.m8 = left.m8 * right.m0 + left.m9 * right.m4 + left.m10 * right.m8 + left.m11 * right.m12;
//	r.m9 = left.m8 * right.m1 + left.m9 * right.m5 + left.m10 * right.m9 + left.m11 * right.m13;
//	r.m10 = left.m8 * right.m2 + left.m9 * right.m6 + left.m10 * right.m10 + left.m11 * right.m14;
//	r.m11 = left.m8 * right.m3 + left.m9 * right.m7 + left.m10 * right.m11 + left.m11 * right.m15;
//	r.m12 = left.m12 * right.m0 + left.m13 * right.m4 + left.m14 * right.m8 + left.m15 * right.m12;
//	r.m13 = left.m12 * right.m1 + left.m13 * right.m5 + left.m14 * right.m9 + left.m15 * right.m13;
//	r.m14 = left.m12 * right.m2 + left.m13 * right.m6 + left.m14 * right.m10 + left.m15 * right.m14;
//	r.m15 = left.m12 * right.m3 + left.m13 * right.m7 + left.m14 * right.m11 + left.m15 * right.m15;
//	return r;
//}
//static inline Vec4 Mat4Multiply(Mat4 left, Vec4 right) {
//	Vec4 r;
//	r.x = left.m0 * right.x + left.m4 * right.y + left.m8 * right.z + left.m12 * right.w;
//	r.y = left.m1 * right.x + left.m5 * right.y + left.m9 * right.z + left.m13 * right.w;
//	r.z = left.m2 * right.x + left.m6 * right.y + left.m10 * right.z + left.m14 * right.w;
//	r.w = left.m3 * right.x + left.m7 * right.y + left.m11 * right.z + left.m15 * right.w;
//	return r;
//}
//static inline Mat4 Mat4Translate(FP x, FP y, FP z) {
//	return { FP::One(), FP::Zero(), FP::Zero(), x, FP::Zero(), FP::One(), FP::Zero(), y, FP::Zero(), FP::Zero(), FP::One(), z, FP::Zero(), FP::Zero(), FP::Zero(), FP::One() };
//}
//static inline Mat4 Mat4Rotate(Vec3 axis, FP angle) {
//	Mat4 r = { FP::Zero() };
//	FP x = axis.x, y = axis.y, z = axis.z;
//	FP lengthSquared = x * x + y * y + z * z;
//	if (lengthSquared != FP::One() && lengthSquared != FP::Zero()) {
//		FP ilength = FP::One() / FP::Sqrt(lengthSquared);
//		x *= ilength; y *= ilength; z *= ilength;
//	}
//	FP sinres = FP::Sin(angle), cosres = FP::Cos(angle), t = FP::One() - cosres;
//	r.m0 = x * x * t + cosres;           r.m1 = y * x * t + z * sinres;         r.m2 = z * x * t - y * sinres;
//	r.m4 = x * y * t - z * sinres;       r.m5 = y * y * t + cosres;             r.m6 = z * y * t + x * sinres;
//	r.m8 = x * z * t + y * sinres;       r.m9 = y * z * t - x * sinres;         r.m10 = z * z * t + cosres;
//	r.m15 = FP::One();
//	return r;
//}
//static inline Mat4 Mat4Scale(FP x, FP y, FP z) {
//	return { x, FP::Zero(), FP::Zero(), FP::Zero(), FP::Zero(), y, FP::Zero(), FP::Zero(), FP::Zero(), FP::Zero(), z, FP::Zero(), FP::Zero(), FP::Zero(), FP::Zero(), FP::One() };
//}
//
//// ============ GJK 核心算法 ============
//static Vec3 Support(Shape* shapeA, Shape* shapeB, Vec3 direction) {
//	Vec3 aFurthestPoint = shapeA->FindFurthestPoint(direction);
//	Vec3 bFurthestPoint = shapeB->FindFurthestPoint(-direction);
//	return aFurthestPoint - bFurthestPoint;
//}
//
//static bool SameDirection(const Vec3& direction, const Vec3& ao) {
//	return Vec3Dot(direction, ao) > FP::Zero();
//}
//
//static std::pair<std::vector<Normal>, size_t> GetFaceNormals(const std::vector<Vec3>& polytope, const std::vector<size_t>& faces) {
//	std::vector<Normal> normals;
//	size_t minTriangle = 0;
//	FP minDistance = FLT_MAX;
//	for (size_t i = 0; i < faces.size(); i += 3) {
//		Vec3 a = polytope[faces[i]], b = polytope[faces[i + 1]], c = polytope[faces[i + 2]];
//		Vec3 ba = Vec3Normalize(b - a), ca = Vec3Normalize(c - a);
//		Vec3 normal = Vec3Normalize(Vec3Cross(ba, ca));
//		FP distance = Vec3Dot(normal, a);
//		if (distance < FP::Zero()) { normal = -normal; distance = -distance; }
//		normals.push_back({ normal.x, normal.y, normal.z, distance });
//		if (distance < minDistance) { minTriangle = i / 3; minDistance = distance; }
//	}
//	return { normals, minTriangle };
//}
//
//static void AddIfUniqueEdge(std::vector<std::pair<size_t, size_t>>& edges, const std::vector<size_t>& faces, size_t a, size_t b) {
//	auto reverse = std::find(edges.begin(), edges.end(), std::make_pair(faces[b], faces[a]));
//	if (reverse != edges.end()) edges.erase(reverse);
//	else edges.emplace_back(faces[a], faces[b]);
//}
//
//static Contact EPA(const Simplex& simplex, Shape* shapeA, Shape* shapeB) {
//	std::vector<Vec3> polytope(simplex.begin(), simplex.end());
//	std::vector<size_t> faces = { 0, 1, 2, 0, 3, 1, 0, 2, 3, 1, 3, 2 };
//	auto faceNormals = GetFaceNormals(polytope, faces);
//	std::vector<Normal> normals = faceNormals.first;
//	size_t minFace = faceNormals.second;
//	Vec3 bakeMinNormal = { normals[minFace].x, normals[minFace].y, normals[minFace].z };
//	FP bakeMinDistance = normals[minFace].w;
//	Vec3 minNormal; FP minDistance = FP::MaxValue();
//	while (minDistance == FP::MaxValue()) {
//		minNormal = { normals[minFace].x, normals[minFace].y, normals[minFace].z };
//		minDistance = normals[minFace].w;
//		Vec3 support = Support(shapeA, shapeB, minNormal);
//		FP sDistance = Vec3Dot(minNormal, support);
//		if (FP::Abs(sDistance - minDistance) > FP(0.001f)) {
//			minDistance = FP::MaxValue();
//			std::vector<std::pair<size_t, size_t>> uniqueEdges;
//			for (size_t i = 0; i < normals.size(); i++) {
//				Vec3 direction = { normals[i].x, normals[i].y, normals[i].z };
//				size_t f = i * 3;
//				if (Vec3Dot(direction, support) > Vec3Dot(direction, polytope[faces[f]])) {
//					AddIfUniqueEdge(uniqueEdges, faces, f, f + 1);
//					AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
//					AddIfUniqueEdge(uniqueEdges, faces, f + 2, f);
//					faces[f + 2] = faces.back(); faces.pop_back();
//					faces[f + 1] = faces.back(); faces.pop_back();
//					faces[f] = faces.back(); faces.pop_back();
//					normals[i] = normals.back(); normals.pop_back();
//					i--;
//				}
//			}
//			std::vector<size_t> newFaces;
//			for (size_t i = 0; i < uniqueEdges.size(); i++) {
//				newFaces.push_back(uniqueEdges[i].first);
//				newFaces.push_back(uniqueEdges[i].second);
//				newFaces.push_back(polytope.size());
//			}
//			polytope.push_back(support);
//			auto newFaceNormals = GetFaceNormals(polytope, newFaces);
//			std::vector<Normal> newNormals = newFaceNormals.first;
//			size_t newMinFace = newFaceNormals.second;
//			FP oldMinDistance = FP::MaxValue();
//			for (size_t i = 0; i < normals.size(); i++) if (normals[i].w < oldMinDistance) { oldMinDistance = normals[i].w; minFace = i; }
//			if (newNormals.size() <= 0) { minNormal = bakeMinNormal; minDistance = bakeMinDistance; break; }
//			if (newNormals[newMinFace].w < oldMinDistance) minFace = newMinFace + normals.size();
//			faces.insert(faces.end(), newFaces.begin(), newFaces.end());
//			normals.insert(normals.end(), newNormals.begin(), newNormals.end());
//		}
//	}
//	Contact contact; contact.Normal = minNormal; contact.Depth = minDistance + FP(0.001f);
//	return contact;
//}
//
//static bool Line(Simplex& points, Vec3& direction) {
//	Vec3 a = points[0], b = points[1];
//	Vec3 ab = Vec3Normalize(b - a), ao = Vec3Normalize(-a);
//	if (SameDirection(ab, ao)) direction = Vec3Normalize(Vec3Cross(Vec3Cross(ab, ao), ab));
//	else { points = { a }; direction = ao; }
//	return false;
//}
//
//static bool Triangle(Simplex& points, Vec3& direction) {
//	Vec3 a = points[0], b = points[1], c = points[2];
//	Vec3 ab = Vec3Normalize(b - a), ac = Vec3Normalize(c - a), ao = Vec3Normalize(-a);
//	Vec3 abc = Vec3Normalize(Vec3Cross(ab, ac));
//	if (SameDirection(Vec3Cross(abc, ac), ao)) {
//		if (SameDirection(ac, ao)) { points = { a, c }; direction = Vec3Normalize(Vec3Cross(Vec3Cross(ac, ao), ac)); }
//		else return Line(points = { a, b }, direction);
//	} else {
//		if (SameDirection(Vec3Cross(ab, abc), ao)) return Line(points = { a, b }, direction);
//		else { if (SameDirection(abc, ao)) direction = abc; else { points = { a, c, b }; direction = -abc; } }
//	}
//	return false;
//}
//
//static bool Tetrahedron(Simplex& points, Vec3& direction) {
//	Vec3 a = points[0], b = points[1], c = points[2], d = points[3];
//	Vec3 ab = Vec3Normalize(b - a), ac = Vec3Normalize(c - a), ad = Vec3Normalize(d - a), ao = Vec3Normalize(-a);
//	Vec3 abc = Vec3Normalize(Vec3Cross(ab, ac)), acd = Vec3Normalize(Vec3Cross(ac, ad)), adb = Vec3Normalize(Vec3Cross(ad, ab));
//	if (SameDirection(abc, ao)) return Triangle(points = { a, b, c }, direction);
//	if (SameDirection(acd, ao)) return Triangle(points = { a, c, d }, direction);
//	if (SameDirection(adb, ao)) return Triangle(points = { a, d, b }, direction);
//	return true;
//}
//
//static bool NextSimplex(Simplex& points, Vec3& direction) {
//	switch (points.size()) {
//	case 2: return Line(points, direction);
//	case 3: return Triangle(points, direction);
//	case 4: return Tetrahedron(points, direction);
//	}
//	return false;
//}
//
//bool Gjk(Shape* shapeA, Shape* shapeB, Vec3 initalDir, Contact* contect) {
//	Vec3 support = Support(shapeA, shapeB, initalDir);
//	Simplex points;
//	points.push_front(support);
//	Vec3 direction = Vec3Normalize(-support);
//	while (true) {
//		support = Support(shapeA, shapeB, direction);
//		if (Vec3Dot(support, direction) < FP::Zero()) return false;
//		points.push_front(support);
//		if (NextSimplex(points, direction)) { *contect = EPA(points, shapeA, shapeB); return true; }
//	}
//}
//
//bool Gjk(Shape* shapeA, Shape* shapeB, Contact* contect) {
//	Vec3 initalDir = { FP::One(), FP::Zero(), FP::Zero() };
//	return Gjk(shapeA, shapeB, initalDir, contect);
//}
