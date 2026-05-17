//#ifndef MATH_H
//#define MATH_H
//#include "T_FP.h"
//
//struct Vec3 {
//	FP x, y, z;
//};
//
//struct Vec4 {
//	FP x, y, z, w;
//	Vec4() {
//		this->x = this->y = this->z = this->w = FP::Zero();
//	}
//	Vec4(Vec3 vec3) {
//		this->x = vec3.x;
//		this->y = vec3.y;
//		this->z = vec3.z;
//		this->w = FP::One();
//	}
//};
//
//struct Mat4 {
//	FP m0, m4, m8, m12;  // Matrix first row (4 components)
//	FP m1, m5, m9, m13;  // Matrix second row (4 components)
//	FP m2, m6, m10, m14; // Matrix third row (4 components)
//	FP m3, m7, m11, m15; // Matrix fourth row (4 components)
//};
//
//static inline FP
//Vec3Dot(const Vec3& a, const Vec3& b) {
//	return a.x * b.x + a.y * b.y + a.z * b.z;
//}
//
//static inline Vec3
//Vec3Cross(const Vec3& a, const Vec3& b) {
//	return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
//}
//
//static inline FP
//Vec3SquareLength(const Vec3& a) {
//	return a.x * a.x + a.y * a.y + a.z * a.z;
//}
//
//static inline FP
//Vec3Length(const Vec3& a) {
//	return FP::Sqrt(Vec3SquareLength(a));
//}
//
//static inline Vec3
//Vec3Normalize(const Vec3& a) {
//	Vec3 r = { 0.0f, 0.0f, 0.0f };
//	FP length = Vec3Length(a);
//	if (length != FP::Zero()) {
//		FP invLength = FP::One() / length;
//		r.x = a.x * invLength;
//		r.y = a.y * invLength;
//		r.z = a.z * invLength;
//	}
//	return r;
//}
//
//static inline Vec3
//operator-(Vec3 a) {
//	return { -a.x, -a.y, -a.z };
//}
//
//static inline Vec3
//operator-(const Vec3& a, const Vec3& b) {
//	return { a.x - b.x, a.y - b.y, a.z - b.z };
//}
//
//static inline Vec3
//operator+(const Vec3& a, const Vec3& b) {
//	return { a.x + b.x, a.y + b.y, a.z + b.z };
//}
//
//static inline Vec3
//operator* (const Vec3& a, FP s) {
//	return { a.x * s, a.y * s, a.z * s };
//}
//
//
//static inline Mat4
//Mat4Multiply(Mat4 left, Mat4 right) {
//	Mat4 result = { 0.0f };
//
//	result.m0 = left.m0 * right.m0 + left.m1 * right.m4 + left.m2 * right.m8 + left.m3 * right.m12;
//	result.m1 = left.m0 * right.m1 + left.m1 * right.m5 + left.m2 * right.m9 + left.m3 * right.m13;
//	result.m2 = left.m0 * right.m2 + left.m1 * right.m6 + left.m2 * right.m10 + left.m3 * right.m14;
//	result.m3 = left.m0 * right.m3 + left.m1 * right.m7 + left.m2 * right.m11 + left.m3 * right.m15;
//	result.m4 = left.m4 * right.m0 + left.m5 * right.m4 + left.m6 * right.m8 + left.m7 * right.m12;
//	result.m5 = left.m4 * right.m1 + left.m5 * right.m5 + left.m6 * right.m9 + left.m7 * right.m13;
//	result.m6 = left.m4 * right.m2 + left.m5 * right.m6 + left.m6 * right.m10 + left.m7 * right.m14;
//	result.m7 = left.m4 * right.m3 + left.m5 * right.m7 + left.m6 * right.m11 + left.m7 * right.m15;
//	result.m8 = left.m8 * right.m0 + left.m9 * right.m4 + left.m10 * right.m8 + left.m11 * right.m12;
//	result.m9 = left.m8 * right.m1 + left.m9 * right.m5 + left.m10 * right.m9 + left.m11 * right.m13;
//	result.m10 = left.m8 * right.m2 + left.m9 * right.m6 + left.m10 * right.m10 + left.m11 * right.m14;
//	result.m11 = left.m8 * right.m3 + left.m9 * right.m7 + left.m10 * right.m11 + left.m11 * right.m15;
//	result.m12 = left.m12 * right.m0 + left.m13 * right.m4 + left.m14 * right.m8 + left.m15 * right.m12;
//	result.m13 = left.m12 * right.m1 + left.m13 * right.m5 + left.m14 * right.m9 + left.m15 * right.m13;
//	result.m14 = left.m12 * right.m2 + left.m13 * right.m6 + left.m14 * right.m10 + left.m15 * right.m14;
//	result.m15 = left.m12 * right.m3 + left.m13 * right.m7 + left.m14 * right.m11 + left.m15 * right.m15;
//
//	return result;
//}
//
//static inline Vec4
//Mat4Multiply(Mat4 left, Vec4 right) {
//	Vec4 result;
//	result.x = left.m0 * right.x + left.m4 * right.y + left.m8 * right.z + left.m12 * right.w;
//	result.y = left.m1 * right.x + left.m5 * right.y + left.m9 * right.z + left.m13 * right.w;
//	result.z = left.m2 * right.x + left.m6 * right.y + left.m10 * right.z + left.m14 * right.w;
//	result.w = left.m3 * right.x + left.m7 * right.y + left.m11 * right.z + left.m15 * right.w;
//	return result;
//}
//
//static inline Mat4
//Mat4Translate(FP x, FP y, FP z) {
//	// <2,3,4,1>
//	Mat4 result = { 1.0f, 0.0f, 0.0f, x,
//					  0.0f, 1.0f, 0.0f, y,
//					  0.0f, 0.0f, 1.0f, z,
//					  0.0f, 0.0f, 0.0f, 1.0f };
//	return result;
//}
//
//static inline Mat4
//Mat4RotateX(FP angle) {
//	Mat4 result = { 1.0f, 0.0f, 0.0f, 0.0f,
//					  0.0f, 1.0f, 0.0f, 0.0f,
//					  0.0f, 0.0f, 1.0f, 0.0f,
//					  0.0f, 0.0f, 0.0f, 1.0f }; // MatrixIdentity()
//
//	FP cosres = FP::Cos(angle);
//	FP sinres = FP::Sin(angle);
//
//	result.m5 = cosres;
//	result.m6 = sinres;
//	result.m9 = -sinres;
//	result.m10 = cosres;
//
//	return result;
//}
//
//static inline Mat4 
//Mat4RotateZ(FP angle) {
//	Mat4 result = { 1.0f, 0.0f, 0.0f, 0.0f,
//					  0.0f, 1.0f, 0.0f, 0.0f,
//					  0.0f, 0.0f, 1.0f, 0.0f,
//					  0.0f, 0.0f, 0.0f, 1.0f }; // MatrixIdentity()
//
//	FP cosres = FP::Cos(angle);
//	FP sinres = FP::Sin(angle);
//
//	result.m0 = cosres;
//	result.m1 = sinres;
//	result.m4 = -sinres;
//	result.m5 = cosres;
//
//	return result;
//}
//
//static inline Mat4
//Mat4RotateY(FP angle) {
//	Mat4 result = { 1.0f, 0.0f, 0.0f, 0.0f,
//					  0.0f, 1.0f, 0.0f, 0.0f,
//					  0.0f, 0.0f, 1.0f, 0.0f,
//					  0.0f, 0.0f, 0.0f, 1.0f }; // MatrixIdentity()
//
//	FP cosres = FP::Cos(angle);
//	FP sinres = FP::Sin(angle);
//
//	result.m0 = cosres;
//	result.m2 = -sinres;
//	result.m8 = sinres;
//	result.m10 = cosres;
//
//	return result;
//}
//
//static inline Mat4
//Mat4Rotate(Vec3 axis, FP angle) {
//	Mat4 result = { 0.0f };
//
//	FP x = axis.x, y = axis.y, z = axis.z;
//
//	FP lengthSquared = x * x + y * y + z * z;
//
//	if ((lengthSquared != FP(1.0f)) && (lengthSquared != FP(0.0f)))
//	{
//		FP ilength = FP(1.0f) / FP::Sqrt(lengthSquared);
//		x *= ilength;
//		y *= ilength;
//		z *= ilength;
//	}
//
//	FP sinres = FP::Sin(angle);
//	FP cosres = FP::Cos(angle);
//	FP t = FP(1.0f) - cosres;
//
//	result.m0 = x * x * t + cosres;
//	result.m1 = y * x * t + z * sinres;
//	result.m2 = z * x * t - y * sinres;
//	result.m3 = 0.0f;
//
//	result.m4 = x * y * t - z * sinres;
//	result.m5 = y * y * t + cosres;
//	result.m6 = z * y * t + x * sinres;
//	result.m7 = 0.0f;
//
//	result.m8 = x * z * t + y * sinres;
//	result.m9 = y * z * t - x * sinres;
//	result.m10 = z * z * t + cosres;
//	result.m11 = 0.0f;
//
//	result.m12 = 0.0f;
//	result.m13 = 0.0f;
//	result.m14 = 0.0f;
//	result.m15 = 1.0f;
//
//	return result;
//}
//
//static inline Mat4 
//Mat4Scale(FP x, FP y, FP z) {
//	Mat4 result = { x, 0.0f, 0.0f, 0.0f,
//					  0.0f, y, 0.0f, 0.0f,
//					  0.0f, 0.0f, z, 0.0f,
//					  0.0f, 0.0f, 0.0f, 1.0f };
//	return result;
//}
//
//
//#endif // !MATH_H
