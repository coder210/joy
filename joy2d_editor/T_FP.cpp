//#include "T_FP.h"
//#include "T_Lut.h"
//
//#define FP_NUM_BITS 64
//#define FP_FRACTIONAL_PLACES 32
//#define FP_ONE (1LL << FP_FRACTIONAL_PLACES)
//#define FP_MAX_VALUE 9223372036854775807LL
//#define FP_MIN_VALUE -9223372036854775808LL
//#define FP_PI_TIMES_2 0x6487ED511
//#define FP_PI 0x3243F6A88
//#define FP_PI_OVER_2 0x1921FB544
//#define FP_LUT_SIZE (int)(FP_PI >> 16)
//
//static int
//CountLeadingZeroes(uint64_t x) {
//	int result = 0;
//	while ((x & 0xF000000000000000) == 0) {
//		result += 4;
//		x <<= 4;
//	}
//
//	while ((x & 0x8000000000000000) == 0) {
//		result += 1;
//		x <<= 1;
//	}
//
//	return result;
//}
//
//FP::FP() {
//	this->rawValue = 0;
//}
//FP::FP(float value) {
//	this->rawValue = FP_ONE * value;
//}
//FP::FP(int64_t value) {
//	this->rawValue = value;
//}
//FP::operator float() const {
//	return this->rawValue * 1.0f / FP_ONE;
//}
//FP::operator int64_t() const {
//	return this->rawValue;
//}
//
//FP FP::operator-() const {
//	return -this->rawValue;
//}
//FP& FP::operator+=(const FP& other) {
//	this->rawValue += other.rawValue;
//	return *this;
//}
//FP& FP::operator-=(const FP& other) {
//	this->rawValue -= other.rawValue;
//	return *this;
//}
//FP& FP::operator*=(const FP& other) {
//	int64_t xl = this->rawValue;
//	int64_t yl = other.rawValue;
//	uint64_t xlo = (uint64_t)(xl & 0x00000000FFFFFFFF);
//	int64_t xhi = xl >> FP_FRACTIONAL_PLACES;
//	uint64_t ylo = (uint64_t)(yl & 0x00000000FFFFFFFF);
//	int64_t yhi = yl >> FP_FRACTIONAL_PLACES;
//	uint64_t lolo = xlo * ylo;
//	int64_t lohi = (int64_t)xlo * yhi;
//	int64_t hilo = xhi * (int64_t)ylo;
//	int64_t hihi = xhi * yhi;
//	uint64_t loResult = lolo >> FP_FRACTIONAL_PLACES;
//	int64_t midResult1 = lohi;
//	int64_t midResult2 = hilo;
//	int64_t hiResult = hihi << FP_FRACTIONAL_PLACES;
//	this->rawValue = (int64_t)loResult + midResult1 + midResult2 + hiResult;
//	return *this;
//}
//FP& FP::operator/=(const FP& other) {
//	int64_t xl = this->rawValue;
//	int64_t yl = other.rawValue;
//	if (yl == 0)
//		return FP(0.0f);
//	uint64_t remainder = (uint64_t)(xl >= 0 ? xl : -xl);
//	uint64_t divider = (uint64_t)(yl >= 0 ? yl : -yl);
//	uint64_t quotient = 0;
//	int bitPos = FP_NUM_BITS / 2 + 1;
//
//	// If the divider is divisible by 2^n, take advantage of it.
//	while ((divider & 0xF) == 0 && bitPos >= 4) {
//		divider >>= 4;
//		bitPos -= 4;
//	}
//
//	while (remainder != 0 && bitPos >= 0) {
//		int shift = CountLeadingZeroes(remainder);
//		if (shift > bitPos) {
//			shift = bitPos;
//		}
//
//		remainder <<= shift;
//		bitPos -= shift;
//
//		uint64_t div = remainder / divider;
//		remainder = remainder % divider;
//		quotient += div << bitPos;
//
//		// Detect overflow
//		if ((div & ~(0xFFFFFFFFFFFFFFFF >> bitPos)) != 0) {
//			return ((xl ^ yl) & FP_MIN_VALUE) == 0 ? FP::MaxValue() : FP::MinValue();
//		}
//
//		remainder <<= 1;
//		--bitPos;
//	}
//
//	// rounding
//	++quotient;
//	int64_t result = (int64_t)(quotient >> 1);
//	if (((xl ^ yl) & FP_MIN_VALUE) != 0) {
//		result = -result;
//	}
//	this->rawValue = result;
//	return *this;
//}
//FP FP::operator+(const FP& other) const {
//	return this->rawValue + other.rawValue;
//}
//FP FP::operator-(const FP& other) const {
//	return this->rawValue - other.rawValue;
//}
//FP FP::operator*(const FP& other) const {
//	int64_t xl = this->rawValue;
//	int64_t yl = other.rawValue;
//	uint64_t xlo = (uint64_t)(xl & 0x00000000FFFFFFFF);
//	int64_t xhi = xl >> FP_FRACTIONAL_PLACES;
//	uint64_t ylo = (uint64_t)(yl & 0x00000000FFFFFFFF);
//	int64_t yhi = yl >> FP_FRACTIONAL_PLACES;
//	uint64_t lolo = xlo * ylo;
//	int64_t lohi = (int64_t)xlo * yhi;
//	int64_t hilo = xhi * (int64_t)ylo;
//	int64_t hihi = xhi * yhi;
//	uint64_t loResult = lolo >> FP_FRACTIONAL_PLACES;
//	int64_t midResult1 = lohi;
//	int64_t midResult2 = hilo;
//	int64_t hiResult = hihi << FP_FRACTIONAL_PLACES;
//	return (int64_t)loResult + midResult1 + midResult2 + hiResult;
//}
//FP FP::operator/(const FP& other) const {
//	int64_t xl = this->rawValue;
//	int64_t yl = other.rawValue;
//	if (yl == 0)
//		return FP(0.0f);
//	uint64_t remainder = (uint64_t)(xl >= 0 ? xl : -xl);
//	uint64_t divider = (uint64_t)(yl >= 0 ? yl : -yl);
//	uint64_t quotient = 0;
//	int bitPos = FP_NUM_BITS / 2 + 1;
//
//	// If the divider is divisible by 2^n, take advantage of it.
//	while ((divider & 0xF) == 0 && bitPos >= 4) {
//		divider >>= 4;
//		bitPos -= 4;
//	}
//
//	while (remainder != 0 && bitPos >= 0) {
//		int shift = CountLeadingZeroes(remainder);
//		if (shift > bitPos) {
//			shift = bitPos;
//		}
//
//		remainder <<= shift;
//		bitPos -= shift;
//
//		uint64_t div = remainder / divider;
//		remainder = remainder % divider;
//		quotient += div << bitPos;
//
//		// Detect overflow
//		if ((div & ~(0xFFFFFFFFFFFFFFFF >> bitPos)) != 0) {
//			return ((xl ^ yl) & FP_MIN_VALUE) == 0 ? FP::MaxValue() : FP::MinValue();
//		}
//
//		remainder <<= 1;
//		--bitPos;
//	}
//
//	// rounding
//	++quotient;
//	int64_t result = (int64_t)(quotient >> 1);
//	if (((xl ^ yl) & FP_MIN_VALUE) != 0) {
//		result = -result;
//	}
//	return result;
//}
//bool FP::operator==(const FP& other) {
//	return this->rawValue == other.rawValue;
//}
//bool FP::operator!=(const FP& other) {
//	return this->rawValue != other.rawValue;
//}
//bool FP::operator>=(const FP& other) {
//	return this->rawValue >= other.rawValue;
//}
//bool FP::operator<=(const FP& other) {
//	return this->rawValue <= other.rawValue;
//}
//bool FP::operator>(const FP& other) {
//	return this->rawValue > other.rawValue;
//}
//bool FP::operator<(const FP& other) {
//	return this->rawValue < other.rawValue;
//}
//FP FP::MinValue() {
//	FP r;
//	r.rawValue = FP_MIN_VALUE;
//	return r;
//}
//FP FP::MaxValue() {
//	FP r;
//	r.rawValue = FP_MAX_VALUE;
//	return r;
//}
//FP FP::Zero() {
//	return FP(0.0f);
//}
//FP FP::Half() {
//	return FP(0.5f);
//}
//FP FP::One() {
//	return FP(1.0f);
//}
//
//
//FP FP::Abs(FP x) {
//	FP r;
//	if (x == FP::MinValue()) {
//		r = FP::MaxValue();
//	}
//	else {
//		int64_t mask = x >> 63;
//		r = (x.rawValue + mask) ^ mask;
//	}
//	return r;
//}
//
//
//
//FP FP::Max(FP a, FP b) {
//	return a > b ? a : b;
//}
//
//FP FP::Min(FP a, FP b) {
//	return a < b ? a : b;
//}
//
//FP FP::Pow2(FP x) {
//	return x * x;
//}
//
//int FP::Sign(FP x) {
//	return x < FP(0.0f) ? -1 : 1;
//}
//
//FP FP::Clamp(FP a, FP low, FP high) {
//	return FP::Max(low, FP::Min(a, high));
//}
//
//
//FP FP::Sqrt(FP x) {
//	int64_t xl = x;
//	// We cannot represent infinities like Single and Double, and Sqrt is
//	// mathematically undefined for x < 0. So we just throw an exception.
//	if (xl < 0) return FP::Zero();
//
//	uint64_t num = (uint64_t)xl;
//	uint64_t result = 0;
//
//	// second-to-top bit
//	uint64_t bit = (uint64_t)1 << (FP_NUM_BITS - 2);
//
//	while (bit > num) bit >>= 2;
//
//	// The main part is executed twice, in order to avoid
//	// using 128 bit values in computations.
//	for (int i = 0; i < 2; ++i) {
//		// First we get the top 48 bits of the answer.
//		while (bit != 0) {
//			if (num >= result + bit) {
//				num -= result + bit;
//				result = (result >> 1) + bit;
//			}
//			else {
//				result = result >> 1;
//			}
//			bit >>= 2;
//		}
//
//		if (i == 0) {
//			// Then process it again to get the lowest 16 bits.
//			if (num > ((uint64_t)1 << (FP_NUM_BITS / 2)) - 1) {
//				// The remainder 'num' is too large to be shifted left
//				// by 32, so we have to add 1 to result manually and
//				// adjust 'num' accordingly.
//				// num = a - (result + 0.5)^2
//				//       = num + result^2 - (result + 0.5)^2
//				//       = num - result - 0.5
//				num -= result;
//				num = (num << (FP_NUM_BITS / 2)) - 0x80000000;
//				result = (result << (FP_NUM_BITS / 2)) + 0x80000000;
//			}
//			else {
//				num <<= (FP_NUM_BITS / 2);
//				result <<= (FP_NUM_BITS / 2);
//			}
//
//			bit = (uint64_t)1 << (FP_NUM_BITS / 2 - 2);
//		}
//	}
//
//	// Finally, if next bit would have been 1, round the result upwards.
//	if (num > result) ++result;
//	return (int64_t)result;
//}
//
//
//static int64_t
//ClampSinValue(int64_t angle, bool* flipHorizontal, bool* flipVertical) {
//	int64_t largePI = 7244019458077122842;
//
//	// Obtained from ((Fix64)1686629713.065252369824872831112M).m_rawValue
//	// This is (2^29)*PI, where 29 is the largest N such that (2^N)*PI < MaxValue.
//	// The idea is that this number contains way more precision than JAM_PI_TIMES_2,
//	// and (((x % (2^29*PI)) % (2^28*PI)) % ... (2^1*PI) = x % (2 * PI)
//	// In practice this gives us an error of about 1,25e-9 in the worst case scenario (Sin(MaxValue))
//	// Whereas simply doing x % JAM_PI_TIMES_2 is the 2e-3 range.
//
//	int64_t clamped2Pi = angle;
//	for (int i = 0; i < 29; ++i) {
//		clamped2Pi %= (largePI >> i);
//	}
//
//	if (angle < 0) {
//		clamped2Pi += FP_PI_TIMES_2;
//	}
//
//	// The LUT contains values for 0 - PiOver2; every other value must be obtained by
//	// vertical or horizontal mirroring
//	*flipVertical = clamped2Pi >= FP_PI;
//
//	// obtain (angle % PI) from (angle % 2PI) - much faster than doing another modulo
//	int64_t clampedPi = clamped2Pi;
//	while (clampedPi >= FP_PI) {
//		clampedPi -= FP_PI;
//	}
//
//	*flipHorizontal = clampedPi >= FP_PI_OVER_2;
//
//	// obtain (angle % JAM_PI_OVER_2) from (angle % PI) - much faster than doing another modulo
//	int64_t clampedPiOver2 = clampedPi;
//	if (clampedPiOver2 >= FP_PI_OVER_2) {
//		clampedPiOver2 -= FP_PI_OVER_2;
//	}
//
//	return clampedPiOver2;
//}
//
//
//FP FP::Sin(FP x) {
//	bool flipHorizontal = false;
//	bool flipVertical = false;
//	int64_t clampedL = ClampSinValue(x, &flipHorizontal, &flipVertical);
//	// Here we use the fact that the SinLut table has a number of entries
//	// equal to (JAM_PI_OVER_2 >> 15) to use the angle to index directly into it
//	uint32_t rawIndex = (uint32_t)(clampedL >> 15);
//	rawIndex = rawIndex >= FP_LUT_SIZE ? FP_LUT_SIZE - 1 : rawIndex;
//	int sin_lut_length = sizeof(sin_lut) / sizeof(sin_lut[0]);
//	int64_t nearestValue = sin_lut[flipHorizontal ? sin_lut_length - 1 - (int)rawIndex : (int)rawIndex];
//	return flipVertical ? -nearestValue : nearestValue;
//}
//
//FP FP::Cos(FP x) {
//	int64_t xl = x;
//	FP angle = xl + (xl > 0 ? -FP_PI - FP_PI_OVER_2 : FP_PI_OVER_2);
//	return FP::Sin(angle);
//}
