//#ifndef FP_H
//#define FP_H
//#include <cstdint>
//
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
//	bool FP::operator==(const FP& other);
//	bool FP::operator!=(const FP& other);
//	bool FP::operator>=(const FP& other);
//	bool FP::operator<=(const FP& other);
//	bool FP::operator>(const FP& other);
//	bool FP::operator<(const FP& other);
//
//	static FP MinValue();
//	static FP MaxValue();
//	static FP Zero();
//	static FP Half();
//	static FP One();
//
//	static FP Max(FP a, FP b);
//	static FP Min(FP a, FP b);
//	static FP Pow2(FP x);
//	static int Sign(FP x);
//	static FP Clamp(FP a, FP low, FP high);
//
//	static FP Sqrt(FP x);
//	static FP Abs(FP x);
//	static FP Cos(FP x);
//	static FP Sin(FP x);
//
//private:
//	int64_t rawValue;
//};
//
//
//#endif