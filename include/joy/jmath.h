/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: mathx.h
Description: 数学库
Author: ydlc
Version: 1.0
Date: 2023.1.29
History:
*************************************************/
#ifndef MATHX_H
#define MATHX_H
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <float.h>
#include <math.h>
#include "jlut.h"
#include "jconfig.h"


#define FP_NUM_BITS 64
#define FP_FRACTIONAL_PLACES 32
#define FP_ONE (1LL << FP_FRACTIONAL_PLACES)

#define FP_MAX_VALUE 9223372036854775807LL
#define FP_MIN_VALUE (-9223372036854775807LL - 1LL)
#define FP_PI_TIMES_2 0x6487ED511
#define FP_PI 0x3243F6A88
#define FP_PI_OVER_2 0x1921FB544
#define FP_PI_OVER_4 0xC90FDAA2
#define FP_E 0x2B7E15162
#define FP_EPOW4 0x3699205C4E
#define FP_LN2 0xB17217F7
#define FP_LNMAX 0x157CD0E702
#define FP_LNMIN (-0x162E42FEFA)
#define FP_LUT_SIZE (int)(FP_PI >> 16)

#define joy_fp_t int64_t
#define joy_ft_t float

typedef struct vec2f {
	joy_fp_t x, y;
} joy_vec2f_t, *joy_vec2f_p;


typedef struct vec3f {
	joy_fp_t x, y, z;
} joy_vec3f_t;


typedef struct vec4f {
	joy_fp_t x, y, z, w;
} joy_vec4f_t;


typedef struct mat22f {
	joy_vec2f_t col1;
	joy_vec2f_t col2;
} joy_mat22f_t;


// Column-major layout: m0,m4,m8,m12 = column 1, m1,m5,m9,m13 = column 2, etc.
// (same convention as GLSL mat4 / joy_mat44_t / the rest of the mat44f_* functions)
typedef struct mat44f {
	joy_fp_t m0, m4, m8, m12;  // Column 1 (4 components)
	joy_fp_t m1, m5, m9, m13;  // Column 2 (4 components)
	joy_fp_t m2, m6, m10, m14; // Column 3 (4 components)
	joy_fp_t m3, m7, m11, m15; // Column 4 (4 components)
}joy_mat44f_t;




typedef struct vec2 {
	joy_ft_t x, y;
} joy_vec2_t;

typedef struct vec3 {
	joy_ft_t x, y, z;
} joy_vec3_t;

typedef struct vec4 {
	joy_ft_t x, y, z, w;
} joy_vec4_t;

typedef struct mat22 {
	joy_vec2_t col1;
	joy_vec2_t col2;
} joy_mat22_t;

// Column-major layout: m0,m4,m8,m12 = column 1, m1,m5,m9,m13 = column 2, etc.
// (same convention as GLSL mat4 / joy_mat44f_t / the rest of the mat44_* functions)
typedef struct mat44 {
	joy_ft_t m0, m4, m8, m12;  // Column 1 (4 components)
	joy_ft_t m1, m5, m9, m13;  // Column 2 (4 components)
	joy_ft_t m2, m6, m10, m14; // Column 3 (4 components)
	joy_ft_t m3, m7, m11, m15; // Column 4 (4 components)
}joy_mat44_t;



JOY_INLINE joy_fp_t fp_from_float(float a)
{
	return (joy_fp_t)(FP_ONE * a);
}

JOY_INLINE float fp_to_float(joy_fp_t a)
{
	return a * 1.0f / FP_ONE;
}

// 无符号 32 位整数值 → 定点数（与 fp_from_int 对称，语义为整数部分）。
// 注意值域：a 接近 2^32 时 (joy_fp_t)a<<32 可能溢出 int64，请保证 a 不接近 2^32。纯整数、帧同步确定性。
JOY_INLINE joy_fp_t fp_from_uint(uint32_t a)
{
	return (joy_fp_t)a << FP_FRACTIONAL_PLACES;
}

// 有符号 32 位整数值 → 定点数，纯整数、帧同步确定性。
JOY_INLINE joy_fp_t fp_from_int(int32_t a)
{
	return (joy_fp_t)a << FP_FRACTIONAL_PLACES;
}

// 32 位小数 → 定点数：把 a∈[0,2^32) 当作定点数 [0,1.0) 的小数部分（整数部分为 0）。
// 常用于将 joy_core_utils_random_next（返回普通 uint32_t）映射为 [0,1.0) 的定点数。纯整数、帧同步确定性。
JOY_INLINE joy_fp_t fp_from_fraction(uint32_t a)
{
	return (joy_fp_t)a;
}

#define FPFF fp_from_float
#define FPTF fp_to_float

JOY_INLINE joy_fp_t fp_min_value()
{
	return FP_MIN_VALUE;
}

JOY_INLINE joy_fp_t fp_max_value()
{
	return FP_MAX_VALUE;
}

JOY_INLINE joy_fp_t fp_add(joy_fp_t a, joy_fp_t b)
{
	return a + b;
}

JOY_INLINE joy_fp_t fp_sub(joy_fp_t a, joy_fp_t b)
{
	return a - b;
}

/* fp_mul */
static joy_fp_t fp_mul(joy_fp_t a, joy_fp_t b)
{
	int64_t xl = a;
	int64_t yl = b;
	uint64_t xlo = (uint64_t)(xl & 0x00000000FFFFFFFF);
	int64_t xhi = xl >> FP_FRACTIONAL_PLACES;
	uint64_t ylo = (uint64_t)(yl & 0x00000000FFFFFFFF);
	int64_t yhi = yl >> FP_FRACTIONAL_PLACES;
	uint64_t lolo = xlo * ylo;
	int64_t lohi = (int64_t)xlo * yhi;
	int64_t hilo = xhi * (int64_t)ylo;
	int64_t hihi = xhi * yhi;
	uint64_t loResult = lolo >> FP_FRACTIONAL_PLACES;
	int64_t midResult1 = lohi;
	int64_t midResult2 = hilo;
	int64_t hiResult = hihi << FP_FRACTIONAL_PLACES;
	return (int64_t)loResult + midResult1 + midResult2 + hiResult;
}

static int count_leading_zeroes(uint64_t x)
{
	int result = 0;
	while ((x & 0xF000000000000000) == 0) {
		result += 4;
		x <<= 4;
	}

	while ((x & 0x8000000000000000) == 0) {
		result += 1;
		x <<= 1;
	}

	return result;
}

static joy_fp_t fp_div(joy_fp_t a, joy_fp_t b)
{
	int64_t xl = a;
	int64_t yl = b;
	if (yl == 0)
		return fp_from_float(0);
	uint64_t remainder = (uint64_t)(xl >= 0 ? xl : -xl);
	uint64_t divider = (uint64_t)(yl >= 0 ? yl : -yl);
	uint64_t quotient = 0;
	int bitPos = FP_NUM_BITS / 2 + 1;

	// If the divider is divisible by 2^n, take advantage of it.
	while ((divider & 0xF) == 0 && bitPos >= 4) {
		divider >>= 4;
		bitPos -= 4;
	}

	while (remainder != 0 && bitPos >= 0) {
		int shift = count_leading_zeroes(remainder);
		if (shift > bitPos) {
			shift = bitPos;
		}

		remainder <<= shift;
		bitPos -= shift;

		uint64_t div = remainder / divider;
		remainder = remainder % divider;
		quotient += div << bitPos;

		// Detect overflow
		if ((div & ~(0xFFFFFFFFFFFFFFFF >> bitPos)) != 0) {
			if (((xl ^ yl) & FP_MIN_VALUE) == 0)
                                return fp_max_value();
			else 
				return fp_min_value();
		}

		remainder <<= 1;
		--bitPos;
	}

	// rounding
	++quotient;
	int64_t result = (int64_t)(quotient >> 1);
	if (((xl ^ yl) & FP_MIN_VALUE) != 0) {
		result = -result;
	}
	return result;
}

static joy_fp_t fp_eq(joy_fp_t a, joy_fp_t b)
{
	return a == b;
}

static joy_fp_t fp_abs(joy_fp_t x)
{
	joy_fp_t r;
	if (x == FP_MIN_VALUE) {
		r = FP_MAX_VALUE;
	}
	else {
		joy_fp_t mask = x >> 63;
		r = (x + mask) ^ mask;
	}
	return r;
}

static joy_fp_t fp_sqrt(joy_fp_t x)
{
	int64_t xl = x;
	if (xl < 0) 
		return 0;

	uint64_t num = (uint64_t)xl;
	uint64_t result = 0;

	uint64_t bit = (uint64_t)1 << (FP_NUM_BITS - 2);

	while (bit > num) bit >>= 2;

	for (int i = 0; i < 2; ++i) {
		while (bit != 0) {
			if (num >= result + bit) {
				num -= result + bit;
				result = (result >> 1) + bit;
			}
			else {
				result = result >> 1;
			}
			bit >>= 2;
		}

		if (i == 0) {
			if (num > ((uint64_t)1 << (FP_NUM_BITS / 2)) - 1) {
				num -= result;
				num = (num << (FP_NUM_BITS / 2)) - 0x80000000;
				result = (result << (FP_NUM_BITS / 2)) + 0x80000000;
			}
			else {
				num <<= (FP_NUM_BITS / 2);
				result <<= (FP_NUM_BITS / 2);
			}

			bit = (uint64_t)1 << (FP_NUM_BITS / 2 - 2);
		}
	}

	if (num > result) ++result;
	return (int64_t)result;
}

static joy_fp_t
clamp_sin_value(joy_fp_t angle, bool *flip_horizontal, bool *flip_vertical)
{
	int64_t largePI = 7244019458077122842;

	int64_t clamped2Pi = angle;
	for (int i = 0; i < 29; ++i) {
		clamped2Pi %= (largePI >> i);
	}

	if (angle < 0) {
		clamped2Pi += FP_PI_TIMES_2;
	}

	*flip_vertical = clamped2Pi >= FP_PI;

	int64_t clampedPi = clamped2Pi;
	while (clampedPi >= FP_PI) {
		clampedPi -= FP_PI;
	}

	*flip_horizontal = clampedPi >= FP_PI_OVER_2;
	int64_t clampedPiOver2 = clampedPi;
	if (clampedPiOver2 >= FP_PI_OVER_2) {
		clampedPiOver2 -= FP_PI_OVER_2;
	}

	return clampedPiOver2;
}

static joy_fp_t fp_sin(joy_fp_t x)
{
	bool flip_horizontal = false;
	bool flip_vertical = false;
	joy_fp_t clampedL = clamp_sin_value(x, &flip_horizontal, &flip_vertical);
	unsigned int rawIndex = (unsigned int)(clampedL >> 15);
	rawIndex = rawIndex >= FP_LUT_SIZE ? FP_LUT_SIZE - 1 : rawIndex;
	int sin_lut_length = sizeof(SIN_LUT) / sizeof(SIN_LUT[0]);
	int index = flip_horizontal ? sin_lut_length - 1 - rawIndex : rawIndex;
	joy_fp_t nearest_value = SIN_LUT[index];
	return flip_vertical ? -nearest_value : nearest_value;
}

static joy_fp_t fp_cos(joy_fp_t x)
{
	joy_fp_t angle = x + (x > 0 ? -FP_PI - FP_PI_OVER_2 : FP_PI_OVER_2);
	return fp_sin(angle);
}

JOY_INLINE joy_fp_t fp_zero()
{
	return 0;
}

JOY_INLINE joy_fp_t fp_half()
{
	return fp_from_float(0.5f);
}

JOY_INLINE joy_fp_t fp_one()
{
	return fp_from_float(1.0f);
}

JOY_INLINE joy_fp_t fp_pi()
{
	return fp_from_float(3.141592658f);
}

JOY_INLINE joy_fp_t fp_max(joy_fp_t a, joy_fp_t b)
{
	return a > b ? a : b;
}

JOY_INLINE joy_fp_t fp_min(joy_fp_t a, joy_fp_t b)
{
	return a < b ? a : b;
}

JOY_INLINE joy_fp_t fp_pow2(joy_fp_t x)
{
	return fp_mul(x, x);
}

JOY_INLINE int fp_sign(joy_fp_t x)
{
	return x < 0 ? -1 : 1;
}

JOY_INLINE joy_fp_t fp_clamp(joy_fp_t a, joy_fp_t low, joy_fp_t high)
{
	return fp_max(low, fp_min(a, high));
}

JOY_INLINE joy_fp_t fp_lerp(joy_fp_t a, joy_fp_t b, joy_fp_t t)
{
	return fp_add(fp_mul(fp_sub(0, t), a), fp_mul(b, t));
}

/* ===================== Fix64.cs 对齐补充函数 =====================
 * 以下函数与 JoySharp/FixMath/Fix64.cs 逐一对齐，补全引擎缺失的
 * 三角函数 / 指对数运算。全程定点（Q31.32），无浮点，保证帧同步确定性。
 * tan_lut 取自 jlut.h（与 SIN_LUT 同构，都用于插值）。 */

// Fix64.Floor：截断到不大于自身的整数（按 Q32 整数部分取整）
static joy_fp_t fp_floor(joy_fp_t value)
{
	return (joy_fp_t)((uint64_t)value & 0xFFFFFFFF00000000ULL);
}

// Fix64.Ceiling：不小于自身的最小整数
static joy_fp_t fp_ceil(joy_fp_t value)
{
	bool has_fractional = (value & 0x00000000FFFFFFFFLL) != 0;
	return has_fractional ? fp_floor(value) + FP_ONE : value;
}

// Fix64.Round：四舍五入到最近整数，恰为 .5 时取偶数（银行家舍入）
static joy_fp_t fp_round(joy_fp_t value)
{
	uint64_t fractional = (uint64_t)(value & 0x00000000FFFFFFFFLL);
	joy_fp_t integral = fp_floor(value);
	if (fractional < 0x80000000ULL) return integral;
	if (fractional > 0x80000000ULL) return integral + FP_ONE;
	return (integral & FP_ONE) == 0 ? integral : integral + FP_ONE;
}

// Fix64.% / FastMod：定点取模（raw 取模即小数取模）
static joy_fp_t fp_mod(joy_fp_t x, joy_fp_t y)
{
	if (x == FP_MIN_VALUE && y == -1) return 0;
	return x % y;
}

// Fix64.Tan：切线（tan_lut 线性插值，π/2 处由表内最大值饱和）
static joy_fp_t fp_tan(joy_fp_t x)
{
	int64_t lut_len = (int64_t)(sizeof(tan_lut) / sizeof(tan_lut[0]));
	joy_fp_t clampedPi = fp_mod(x, FP_PI);
	bool flip = false;
	if (clampedPi < 0) { clampedPi = -clampedPi; flip = true; }
	if (clampedPi > FP_PI_OVER_2) {
		flip = !flip;
		clampedPi = FP_PI_OVER_2 - (clampedPi - FP_PI_OVER_2);
	}
	joy_fp_t lut_interval = fp_div(fp_from_int((int)(lut_len - 1)), FP_PI_OVER_2);
	joy_fp_t raw_index = fp_mul(clampedPi, lut_interval);
	joy_fp_t rounded = fp_round(raw_index);
	joy_fp_t index_error = fp_sub(raw_index, rounded);
	int64_t idx = rounded >> FP_FRACTIONAL_PLACES;
	int sign = index_error > 0 ? 1 : (index_error < 0 ? -1 : 0);
	if (idx >= lut_len - 1) idx = lut_len - 1;
	if (idx < 0) idx = 0;
	joy_fp_t nearest = tan_lut[idx];
	int64_t second = idx + sign;
	if (second < 0) second = 0;
	if (second > lut_len - 1) second = lut_len - 1;
	joy_fp_t delta = fp_mul(index_error, fp_abs(fp_sub(nearest, tan_lut[second])));
	joy_fp_t interp = fp_add(nearest, delta);
	return flip ? -interp : interp;
}

// Fix64.Atan：反正切（欧拉级数，收敛到 [0,π/2)）
static joy_fp_t fp_atan(joy_fp_t z)
{
	if (z == 0) return 0;
	bool neg = z < 0;
	if (neg) z = -z;
	joy_fp_t result;
	if (z == fp_one()) {
		result = FP_PI_OVER_4;
	} else {
		bool invert = z > fp_one();
		if (invert) z = fp_div(fp_one(), z);
		result = fp_one();
		joy_fp_t term = fp_one();
		joy_fp_t zSq = fp_mul(z, z);
		joy_fp_t zSq2 = fp_mul(zSq, fp_from_int(2));
		joy_fp_t zSqPlusOne = fp_add(zSq, fp_one());
		joy_fp_t zSq12 = fp_mul(zSqPlusOne, fp_from_int(2));
		joy_fp_t dividend = zSq2;
		joy_fp_t divisor = fp_mul(zSqPlusOne, fp_from_int(3));
		for (int i = 2; i < 30; i++) {
			term = fp_mul(term, fp_div(dividend, divisor));
			result = fp_add(result, term);
			dividend = fp_add(dividend, zSq2);
			divisor = fp_add(divisor, zSq12);
			if (term == 0) break;
		}
		result = fp_div(fp_mul(result, z), zSqPlusOne);
		if (invert) result = fp_sub(FP_PI_OVER_2, result);
	}
	if (neg) result = -result;
	return result;
}

// Fix64.Atan2：四象限反正切（基于 Atan）
static joy_fp_t fp_atan2(joy_fp_t y, joy_fp_t x)
{
	if (x == 0) {
		if (y > 0) return FP_PI_OVER_2;
		if (y == 0) return 0;
		return -FP_PI_OVER_2;
	}
	joy_fp_t atan = fp_atan(fp_div(y, x));
	if (x < 0) {
		if (y < 0) return fp_sub(atan, FP_PI);
		return fp_add(atan, FP_PI);
	}
	return atan;
}

// Fix64.Acos：反余弦（基于 Atan + Sqrt）
static joy_fp_t fp_acos(joy_fp_t x)
{
	if (x == 0) return FP_PI_OVER_2;
	joy_fp_t result = fp_atan(fp_div(fp_sqrt(fp_sub(fp_one(), fp_mul(x, x))), x));
	if (x < 0) return fp_add(result, FP_PI);
	return result;
}

// Fix64.Exp：e^x（幂级数展开）
static joy_fp_t fp_exp(joy_fp_t x)
{
	if (x == 0) return fp_one();
	if (x == fp_one()) return FP_E;
	if (x >= FP_LNMAX) return fp_max_value();
	if (x <= FP_LNMIN) return 0;
	bool neg = x < 0;
	if (neg) x = -x;
	joy_fp_t result = fp_add(x, fp_one());
	joy_fp_t term = x;
	for (int i = 2; i < 40; i++) {
		term = fp_div(fp_mul(x, term), fp_from_int(i));
		result = fp_add(result, term);
		if (term == 0) break;
	}
	if (neg) result = fp_div(fp_one(), result);
	return result;
}

// Fix64.Ln：自然对数（牛顿迭代）
static joy_fp_t fp_ln(joy_fp_t x)
{
	if (x <= 0) return 0;
	int scaling = 0;
	while (x > FP_EPOW4) { x = fp_div(x, FP_EPOW4); scaling += 4; }
	while (x < fp_one()) { x = fp_mul(x, FP_EPOW4); scaling -= 4; }
	joy_fp_t guess = fp_from_int(2);
	joy_fp_t delta = 0;
	int count = 0;
	do {
		joy_fp_t e = fp_exp(guess);
		delta = fp_div(fp_sub(x, e), e);
		if (delta > fp_from_int(3)) delta = fp_from_int(3);
		guess = fp_add(guess, delta);
	} while ((count++ < 10) && (delta != 0));
	return fp_add(guess, fp_from_int(scaling));
}

// Fix64.Pow(b, exp)：b^exp = e^(exp·ln b)
static joy_fp_t fp_pow(joy_fp_t b, joy_fp_t exp)
{
	if (b == fp_one()) return fp_one();
	if (exp == 0) return fp_one();
	if (b == 0) return 0;
	return fp_exp(fp_mul(exp, fp_ln(b)));
}

// Fix64.Pow(x, n)：正整数次幂（快速幂）
static joy_fp_t fp_powi(joy_fp_t x, int64_t n)
{
	if (n == 0) return fp_one();
	joy_fp_t y = fp_one();
	while (n > 1) {
		if ((n & 1LL) == 0) { x = fp_mul(x, x); n >>= 1; }
		else { y = fp_mul(x, y); x = fp_mul(x, x); n = (n - 1) >> 1; }
	}
	return fp_mul(x, y);
}

// Fix64.Sin（插值版）：对 SIN_LUT 相邻两点做线性插值，精度高于 fp_sin（Fast 版）。
// 帧同步确定性两端一致；fp_sin / fp_cos 为 LUT 直取（Fast），本函数为插值（精确）。
static joy_fp_t fp_sin_interp(joy_fp_t x)
{
	bool flip_horizontal = false;
	bool flip_vertical = false;
	joy_fp_t clamped = clamp_sin_value(x, &flip_horizontal, &flip_vertical);
	int64_t len = (int64_t)(sizeof(SIN_LUT) / sizeof(SIN_LUT[0]));
	joy_fp_t lut_interval = fp_div(fp_from_int((int)(len - 1)), FP_PI_OVER_2);
	joy_fp_t raw_index = fp_mul(clamped, lut_interval);
	joy_fp_t rounded = fp_round(raw_index);
	joy_fp_t index_error = fp_sub(raw_index, rounded);
	int64_t base = rounded >> FP_FRACTIONAL_PLACES;
	int sign = index_error > 0 ? 1 : (index_error < 0 ? -1 : 0);
	if (base >= len - 1) base = len - 1;
	if (base < 0) base = 0;
	int64_t ni = flip_horizontal ? (len - 1 - base) : base;
	int64_t oi = flip_horizontal ? (len - 1 - base - sign) : (base + sign);
	if (oi < 0) oi = 0;
	if (oi > len - 1) oi = len - 1;
	joy_fp_t nearest = SIN_LUT[ni];
	joy_fp_t second = SIN_LUT[oi];
	joy_fp_t delta = fp_mul(index_error, fp_abs(fp_sub(nearest, second)));
	joy_fp_t interp = flip_horizontal ? fp_sub(nearest, delta) : fp_add(nearest, delta);
	return flip_vertical ? -interp : interp;
}

// Fix64.Cos（插值版）：角度偏移后复用 fp_sin_interp
static joy_fp_t fp_cos_interp(joy_fp_t x)
{
	joy_fp_t angle = x + (x > 0 ? -FP_PI - FP_PI_OVER_2 : FP_PI_OVER_2);
	return fp_sin_interp(angle);
}


JOY_INLINE joy_vec2f_t vec2f_negate(joy_vec2f_t a)
{
	joy_vec2f_t r;
	r.x = -a.x;
	r.y = -a.y;
	return r;
}

JOY_INLINE joy_fp_t vec2f_dot(joy_vec2f_t a, joy_vec2f_t b)
{
	return fp_add(fp_mul(a.x, b.x), fp_mul(a.y, b.y));
}

JOY_INLINE joy_fp_t vec2f_cross(joy_vec2f_t a, joy_vec2f_t b)
{
	return fp_sub(fp_mul(a.x, b.y), fp_mul(a.y, b.x));
}

JOY_INLINE joy_vec2f_t vec2f_cross2(joy_vec2f_t a, joy_fp_t s)
{
	joy_vec2f_t r;
	r.x = fp_mul(a.y, s);
	r.y = fp_mul(a.x, -s);
	return r;
}

JOY_INLINE joy_vec2f_t vec2f_cross3(joy_vec2f_t a, joy_fp_t s)
{
	joy_vec2f_t r;
	r.x = fp_mul(a.y, -s);
	r.y = fp_mul(a.x, s);
	return r;
}

JOY_INLINE joy_vec2f_t vec2f_add(joy_vec2f_t a, joy_vec2f_t b)
{
	joy_vec2f_t r;
	r.x = fp_add(a.x, b.x);
	r.y = fp_add(a.y, b.y);
	return r;
}

JOY_INLINE joy_vec2f_t vec2f_sub(joy_vec2f_t a, joy_vec2f_t b)
{
	joy_vec2f_t r;
	r.x = fp_sub(a.x, b.x);
	r.y = fp_sub(a.y, b.y);
	return r;
}

JOY_INLINE joy_vec2f_t vec2f_scale(joy_vec2f_t a, joy_fp_t s)
{
	joy_vec2f_t r;
	r.x = fp_mul(a.x, s);
	r.y = fp_mul(a.y, s);
	return r;
}

JOY_INLINE joy_vec2f_t vec2f_abs(joy_vec2f_t a)
{
	joy_vec2f_t r;
	r.x = fp_abs(a.x);
	r.y = fp_abs(a.y);
	return r;
}

JOY_INLINE joy_fp_t vec2f_length_squared(joy_vec2f_t a)
{
	return fp_add(fp_pow2(a.x), fp_pow2(a.y));
}

JOY_INLINE joy_fp_t vec2f_length(joy_vec2f_t a)
{
	return fp_sqrt(fp_add(fp_pow2(a.x), fp_pow2(a.y)));
}

JOY_INLINE joy_vec2f_t vec2f_normalize(joy_vec2f_t a)
{
	joy_vec2f_t r;
	joy_fp_t length, inv_length;
	length = vec2f_length(a);
	if (length != 0) {
		inv_length = fp_div(fp_one(), length);
		r.x = fp_mul(a.x, inv_length);
		r.y = fp_mul(a.y, inv_length);
	}
	else {
		r.x = 0;
		r.y = 0;
	}
	return r;
}


JOY_INLINE joy_vec2f_t vec2f_normal(joy_vec2f_t v)
{
	joy_vec2f_t perpbicular;
	perpbicular.x = v.y;
	perpbicular.y = -v.x;
	return vec2f_normalize(perpbicular);
}

/*
 * x = x * cos(angle) - y * sin(angle);
 * y = x * sin(angle) + y * cos(angle);
 * vM: 旋转矢量
 *[x, y] [c, -s]
 *		 [s,  c]
*/
JOY_INLINE joy_vec2f_t vec2f_rotate(joy_vec2f_t v, joy_fp_t angle)
{
	joy_vec2f_t r;
	joy_fp_t s, c;
	s = fp_sin(angle);
	c = fp_cos(angle);
	r.x = fp_sub(fp_mul(v.x, c), fp_mul(v.y, s));
	r.y = fp_add(fp_mul(v.x, s), fp_mul(v.y, c));
	return r;
}

JOY_INLINE joy_vec2f_t vec2f_lerp(joy_vec2f_t a, joy_vec2f_t b, joy_fp_t t)
{
	joy_vec2f_t r;
	r.x = fp_lerp(a.x, b.x, t);
	r.y = fp_lerp(a.y, b.y, t);
	return r;
}

JOY_INLINE joy_fp_t vec2f_distance(joy_vec2f_t a, joy_vec2f_t b)
{
	joy_vec2f_t diff = vec2f_sub(b, a);
	return vec2f_length(diff);
}

JOY_INLINE joy_vec3f_t vec3f_negate(joy_vec3f_t a)
{
	joy_vec3f_t r;
	r.x = -a.x;
	r.y = -a.y;
	r.z = -a.z;
	return r;
}

JOY_INLINE joy_vec3f_t vec3f_add(joy_vec3f_t a, joy_vec3f_t b)
{
	joy_vec3f_t r;
	r.x = fp_add(a.x, b.x);
	r.y = fp_add(a.y, b.y);
	r.z = fp_add(a.z, b.z);
	return r;
}

JOY_INLINE joy_vec3f_t vec3f_sub(joy_vec3f_t a, joy_vec3f_t b)
{
	joy_vec3f_t r;
	r.x = fp_sub(a.x, b.x);
	r.y = fp_sub(a.y, b.y);
	r.z = fp_sub(a.z, b.z);
	return r;
}

JOY_INLINE joy_vec3f_t vec3f_scale(joy_vec3f_t a, joy_fp_t s)
{
	joy_vec3f_t r;
	r.x = fp_mul(a.x, s);
	r.y = fp_mul(a.y, s);
	r.z = fp_mul(a.z, s);
	return r;
}

JOY_INLINE joy_vec3f_t vec3f_abs(joy_vec3f_t a)
{
	joy_vec3f_t r;
	r.x = fp_abs(a.x);
	r.y = fp_abs(a.y);
	r.z = fp_abs(a.z);
	return r;
}

JOY_INLINE joy_fp_t vec3f_dot(joy_vec3f_t a, joy_vec3f_t b)
{
	joy_fp_t pow2_x, pow2_y, pow2_z;
	pow2_x = fp_mul(a.x, b.x);
	pow2_y = fp_mul(a.y, b.y);
	pow2_z = fp_mul(a.z, b.z);
	return fp_add(fp_add(pow2_x, pow2_y), pow2_z);
}

JOY_INLINE joy_vec3f_t vec3f_cross(joy_vec3f_t a, joy_vec3f_t b)
{
	joy_vec3f_t r;
	r.x = fp_sub(fp_mul(a.y, b.z), fp_mul(a.z, b.y));
	r.y = fp_sub(fp_mul(a.z, b.x), fp_mul(a.x, b.z));
	r.z = fp_sub(fp_mul(a.x, b.y), fp_mul(a.y, b.x));
	return r;
}

JOY_INLINE joy_fp_t vec3f_square_length(joy_vec3f_t a)
{
	joy_fp_t pow2_x, pow2_y, pow2_z;
	pow2_x = fp_mul(a.x, a.x);
	pow2_y = fp_mul(a.y, a.y);
	pow2_z = fp_mul(a.z, a.z);
	return fp_add(fp_add(pow2_x, pow2_y), pow2_z);
}

JOY_INLINE joy_fp_t vec3f_length(joy_vec3f_t a)
{
	joy_fp_t square_length;
	square_length = vec3f_square_length(a);
	return fp_sqrt(square_length);
}

JOY_INLINE joy_vec3f_t vec3f_normalize(joy_vec3f_t a)
{
	joy_vec3f_t r;
	joy_fp_t length, inv_length;
	length = vec3f_length(a);
	if (length != fp_zero()) {
		inv_length = fp_div(fp_one(), length);
		r.x = fp_mul(a.x, inv_length);
		r.y = fp_mul(a.y, inv_length);
		r.z = fp_mul(a.z, inv_length);
	}
	else {
		r.x = r.y = r.z = 0;
	}
	return r;
}

JOY_INLINE joy_vec3f_t vec3f_lerp(joy_vec3f_t a, joy_vec3f_t b, joy_fp_t t)
{
	joy_vec3f_t r;
	r.x = fp_lerp(a.x, b.x, t);
	r.y = fp_lerp(a.y, b.y, t);
	r.z = fp_lerp(a.z, b.z, t);
	return r;
}

JOY_INLINE joy_fp_t vec3f_distance(joy_vec3f_t a, joy_vec3f_t b)
{
	joy_vec3f_t diff;
	diff = vec3f_sub(b, a);
	return vec3f_length(diff);
}


JOY_INLINE joy_vec2f_t mat22f_mul_vec2f(joy_mat22f_t a, joy_vec2f_t b)
{
	joy_vec2f_t r;
	r.x = fp_add(fp_mul(a.col1.x, b.x), fp_mul(a.col2.x, b.y));
	r.y = fp_add(fp_mul(a.col1.y, b.x), fp_mul(a.col2.y, b.y));
	return r;
}

JOY_INLINE joy_mat22f_t mat22f_rotate(joy_fp_t angle)
{
	joy_fp_t c, s;
	joy_mat22f_t r;
	c = fp_cos(angle);
	s = fp_sin(angle);
	r.col1.x = c; r.col2.x = -s;
	r.col1.y = s; r.col2.y = c;
	return r;
}

JOY_INLINE joy_mat22f_t mat22f_transpose(joy_mat22f_t m)
{
	joy_mat22f_t r;
	r.col1.x = m.col1.x;
	r.col1.y = m.col2.x;
	r.col2.x = m.col1.y;
	r.col2.y = m.col2.y;
	return r;
}

JOY_INLINE joy_mat22f_t mat22f_invert(joy_mat22f_t m)
{
	joy_fp_t a, b, c, d, det, negate_det;
	joy_mat22f_t r;

	a = m.col1.x, b = m.col2.x;
	c = m.col1.y, d = m.col2.y;

	det = fp_sub(fp_mul(a, d), fp_mul(b, c));
	assert(det != fp_zero());
	det = fp_div(fp_one(), det);
	negate_det = -det;

	r.col1.x = fp_mul(det, d);
	r.col2.x = fp_mul(negate_det, b);
	r.col1.y = fp_mul(negate_det, c);
	r.col2.y = fp_mul(det, a);
	return r;
}

JOY_INLINE joy_mat22f_t mat22f_add(joy_mat22f_t a, joy_mat22f_t b)
{
	joy_mat22f_t r;
	r.col1 = vec2f_add(a.col1, b.col1);
	r.col2 = vec2f_add(a.col2, b.col2);
	return r;
}

JOY_INLINE joy_mat22f_t mat22f_mul(joy_mat22f_t a, joy_mat22f_t b)
{
	joy_mat22f_t r;
	r.col1 = mat22f_mul_vec2f(a, b.col1);
	r.col2 = mat22f_mul_vec2f(a, b.col2);
	return r;
}

JOY_INLINE joy_mat22f_t mat22f_abs(joy_mat22f_t a)
{
	joy_mat22f_t r;
	r.col1 = vec2f_abs(a.col1);
	r.col2 = vec2f_abs(a.col2);
	return r;
}

JOY_INLINE joy_mat44f_t mat44f_mul(joy_mat44f_t left, joy_mat44f_t right)
{
	// 列主序乘法：result = left · right
	// joy_mat44f_t 按列主序存储（每 4 个连续成员为一列），与 joy_mat44_t / GLSL mat4 / 其余 mat44f_* 函数约定一致
	// (A·B)[i][j] = Σ_k A[i][k]·B[k][j] = Σ_k left.m_{i+4k} · right.m_{4j+k}
	joy_mat44f_t result;
	result.m0  = fp_add(fp_add(fp_mul(left.m0,  right.m0),  fp_mul(left.m4,  right.m1)),  fp_add(fp_mul(left.m8,  right.m2),  fp_mul(left.m12, right.m3)));
	result.m1  = fp_add(fp_add(fp_mul(left.m1,  right.m0),  fp_mul(left.m5,  right.m1)),  fp_add(fp_mul(left.m9,  right.m2),  fp_mul(left.m13, right.m3)));
	result.m2  = fp_add(fp_add(fp_mul(left.m2,  right.m0),  fp_mul(left.m6,  right.m1)),  fp_add(fp_mul(left.m10, right.m2),  fp_mul(left.m14, right.m3)));
	result.m3  = fp_add(fp_add(fp_mul(left.m3,  right.m0),  fp_mul(left.m7,  right.m1)),  fp_add(fp_mul(left.m11, right.m2),  fp_mul(left.m15, right.m3)));
	result.m4  = fp_add(fp_add(fp_mul(left.m0,  right.m4),  fp_mul(left.m4,  right.m5)),  fp_add(fp_mul(left.m8,  right.m6),  fp_mul(left.m12, right.m7)));
	result.m5  = fp_add(fp_add(fp_mul(left.m1,  right.m4),  fp_mul(left.m5,  right.m5)),  fp_add(fp_mul(left.m9,  right.m6),  fp_mul(left.m13, right.m7)));
	result.m6  = fp_add(fp_add(fp_mul(left.m2,  right.m4),  fp_mul(left.m6,  right.m5)),  fp_add(fp_mul(left.m10, right.m6),  fp_mul(left.m14, right.m7)));
	result.m7  = fp_add(fp_add(fp_mul(left.m3,  right.m4),  fp_mul(left.m7,  right.m5)),  fp_add(fp_mul(left.m11, right.m6),  fp_mul(left.m15, right.m7)));
	result.m8  = fp_add(fp_add(fp_mul(left.m0,  right.m8),  fp_mul(left.m4,  right.m9)),  fp_add(fp_mul(left.m8,  right.m10), fp_mul(left.m12, right.m11)));
	result.m9  = fp_add(fp_add(fp_mul(left.m1,  right.m8),  fp_mul(left.m5,  right.m9)),  fp_add(fp_mul(left.m9,  right.m10), fp_mul(left.m13, right.m11)));
	result.m10 = fp_add(fp_add(fp_mul(left.m2,  right.m8),  fp_mul(left.m6,  right.m9)),  fp_add(fp_mul(left.m10, right.m10), fp_mul(left.m14, right.m11)));
	result.m11 = fp_add(fp_add(fp_mul(left.m3,  right.m8),  fp_mul(left.m7,  right.m9)),  fp_add(fp_mul(left.m11, right.m10), fp_mul(left.m15, right.m11)));
	result.m12 = fp_add(fp_add(fp_mul(left.m0,  right.m12), fp_mul(left.m4,  right.m13)), fp_add(fp_mul(left.m8,  right.m14), fp_mul(left.m12, right.m15)));
	result.m13 = fp_add(fp_add(fp_mul(left.m1,  right.m12), fp_mul(left.m5,  right.m13)), fp_add(fp_mul(left.m9,  right.m14), fp_mul(left.m13, right.m15)));
	result.m14 = fp_add(fp_add(fp_mul(left.m2,  right.m12), fp_mul(left.m6,  right.m13)), fp_add(fp_mul(left.m10, right.m14), fp_mul(left.m14, right.m15)));
	result.m15 = fp_add(fp_add(fp_mul(left.m3,  right.m12), fp_mul(left.m7,  right.m13)), fp_add(fp_mul(left.m11, right.m14), fp_mul(left.m15, right.m15)));
	return result;
}

JOY_INLINE joy_mat44f_t mat44f_translate(joy_fp_t x, joy_fp_t y, joy_fp_t z)
{
	// <2,3,4,1>
	joy_mat44f_t result = {
		fp_one(), fp_zero(), fp_zero(), x,
		fp_zero(), fp_one(), fp_zero(), y,
		fp_zero(), fp_zero(), fp_one(), z,
		fp_zero(), fp_zero(), fp_zero(), fp_one()
	};
	return result;
}

JOY_INLINE joy_mat44f_t mat44f_identity()
{
	joy_mat44f_t result = {
		fp_one(), fp_zero(), fp_zero(), fp_zero(),
		fp_zero(), fp_one(), fp_zero(), fp_zero(),
		fp_zero(), fp_zero(), fp_one(), fp_zero(),
		fp_zero(), fp_zero(), fp_zero(), fp_one()
	};
	return result;
}

JOY_INLINE joy_mat44f_t mat44f_rotate_x(joy_fp_t angle)
{
	joy_mat44f_t result;
	joy_fp_t c, s;
	result = mat44f_identity();
	c = fp_cos(angle);
	s = fp_sin(angle);
	result.m5 = c;
	result.m6 = s;
	result.m9 = -s;
	result.m10 = c;
	return result;
}

JOY_INLINE joy_mat44f_t mat44f_rotate_y(joy_fp_t angle)
{
	joy_mat44f_t result;
	joy_fp_t c, s;
	
	result = mat44f_identity();
	c = fp_cos(angle);
	s = fp_sin(angle);

	result.m0 = c;
	result.m2 = -s;
	result.m8 = s;
	result.m10 = c;

	return result;
}

JOY_INLINE joy_mat44f_t mat44f_rotate_z(joy_fp_t angle)
{
	joy_mat44f_t result;
	joy_fp_t c, s;

	result = mat44f_identity();
	c = fp_cos(angle);
	s = fp_sin(angle);

	result.m0 = c;
	result.m1 = s;
	result.m4 = -s;
	result.m5 = c;

	return result;
}

JOY_INLINE joy_mat44f_t mat44f_rotate(joy_vec3f_t axis, joy_fp_t angle)
{
	joy_mat44f_t result;
	joy_fp_t x = axis.x, y = axis.y, z = axis.z;
	joy_fp_t inv_length, length_squared;
	joy_fp_t s, c;

	length_squared = fp_add(fp_add(fp_pow2(x), fp_pow2(y)), fp_pow2(z));

	if ((length_squared != fp_one()) && (length_squared != fp_zero()))
	{
		inv_length = fp_div(fp_one(), fp_sqrt(length_squared));
		x = fp_mul(x, inv_length);
		y = fp_mul(y, inv_length);
		z = fp_mul(z, inv_length);
	}

	s = fp_sin(angle);
	c = fp_cos(angle);
	joy_fp_t t = fp_sub(fp_one(), c);

	result.m0 = fp_add(fp_mul(fp_mul(x, x), t), c);
	result.m1 = fp_add(fp_mul(fp_mul(y, x), t), fp_mul(z, s));
	result.m2 = fp_sub(fp_mul(fp_mul(z, x), t), fp_mul(y, s));
	result.m3 = fp_zero();

	result.m4 = fp_sub(fp_mul(fp_mul(x, y), t), fp_mul(z, s));
	result.m5 = fp_add(fp_mul(fp_mul(y, y), t), c);
	result.m6 = fp_add(fp_mul(fp_mul(z, y), t), fp_mul(x, s));
	result.m7 = fp_zero();

	result.m8 = fp_add(fp_mul(fp_mul(x, z), t), fp_mul(y, s));
	result.m9 = fp_sub(fp_mul(fp_mul(y, z), t), fp_mul(x, s));
	result.m10 = fp_add(fp_mul(fp_mul(z, z), t), c);
	result.m11 = fp_zero();

	result.m12 = fp_zero();
	result.m13 = fp_zero();
	result.m14 = fp_zero();
	result.m15 = fp_one();

	return result;
}

JOY_INLINE joy_mat44f_t mat44f_scale(joy_fp_t x, joy_fp_t y, joy_fp_t z)
{
	joy_mat44f_t result = {
                x, fp_zero(), fp_zero(), fp_zero(),
                fp_zero(), y, fp_zero(), fp_zero(),
		fp_zero(), fp_zero(), z, fp_zero(),
		fp_zero(), fp_zero(), fp_zero(), fp_one()
	};
	return result;
}

JOY_INLINE joy_vec4f_t mat44f_mul_vec4f(joy_mat44f_t left, joy_vec4f_t right)
{
	joy_vec4f_t result;
	result.x = fp_add(
		fp_add(fp_mul(left.m0, right.x), fp_mul(left.m4, right.y)),
		fp_add(fp_mul(left.m8, right.z), fp_mul(left.m12, right.w))
	);
	result.y = fp_add(
		fp_add(fp_mul(left.m1, right.x), fp_mul(left.m5, right.y)), 
		fp_add(fp_mul(left.m9, right.z), fp_mul(left.m13, right.w))
	);
	result.z = fp_add(
		fp_add(fp_mul(left.m2, right.x), fp_mul(left.m6, right.y)),
		fp_add(fp_mul(left.m10, right.z), fp_mul(left.m14, right.w))
	);
	result.w = fp_add(
                fp_add(fp_mul(left.m3, right.x), fp_mul(left.m7, right.y)),
		fp_add(fp_mul(left.m11, right.z), fp_mul(left.m15, right.w))
	);
	return result;
}



JOY_INLINE joy_ft_t ft_from_float(float x)
{
	return x;
}

JOY_INLINE float ft_to_float(joy_ft_t x)
{
	return x;
}

#define FTFF ft_from_float
#define FTTf ft_to_float

JOY_INLINE joy_ft_t ft_min_value()
{
	return -FLT_MAX;
}

JOY_INLINE joy_ft_t ft_max_value()
{
	return FLT_MAX;
}

JOY_INLINE joy_ft_t ft_add(joy_ft_t a, joy_ft_t b)
{
	return a + b;
}

JOY_INLINE joy_ft_t ft_sub(joy_ft_t a, joy_ft_t b)
{
	return a - b;
}

JOY_INLINE joy_ft_t ft_mul(joy_ft_t a, joy_ft_t b)
{
	return a * b;
}

JOY_INLINE joy_ft_t ft_div(joy_ft_t a, joy_ft_t b)
{
	return a / b;
}

JOY_INLINE joy_ft_t ft_abs(joy_ft_t x)
{
	return x > 0.0f ? x : -x;
}

JOY_INLINE joy_ft_t ft_sqrt(joy_ft_t x)
{
	return sqrtf(x);
}

JOY_INLINE joy_ft_t ft_sin(joy_ft_t x)
{
	return sinf(x);
}


JOY_INLINE joy_ft_t ft_cos(joy_ft_t x)
{
	return cosf(x);
}

JOY_INLINE joy_ft_t ft_atan2(joy_ft_t y, joy_ft_t x)
{
	return atan2f(y, x);
}

JOY_INLINE joy_ft_t ft_zero()
{
	return 0;
}

JOY_INLINE joy_ft_t ft_half()
{
	return 0.5f;
}

JOY_INLINE joy_ft_t ft_one()
{
	return 1.0f;
}

JOY_INLINE joy_ft_t ft_pi()
{
	return 3.141592658f;
}

JOY_INLINE joy_ft_t ft_max(joy_ft_t a, joy_ft_t b)
{
	return a > b ? a : b;
}

JOY_INLINE joy_ft_t ft_min(joy_ft_t a, joy_ft_t b)
{
	return a < b ? a : b;
}

JOY_INLINE joy_ft_t ft_pow2(joy_ft_t x)
{
	return x * x;
}

JOY_INLINE int ft_sign(joy_ft_t x)
{
	return x < 0 ? -1 : 1;
}

JOY_INLINE joy_ft_t ft_clamp(joy_ft_t a, joy_ft_t low, joy_ft_t high)
{
	return ft_max(low, ft_min(a, high));
}

JOY_INLINE joy_ft_t ft_lerp(joy_ft_t a, joy_ft_t b, joy_ft_t t)
{
	return (1 - t) * a + b * t;
}

JOY_INLINE joy_vec2_t vec2_negate(joy_vec2_t v)
{
	joy_vec2_t r;
	r.x = -v.x;
	r.y = -v.y;
	return r;
}

JOY_INLINE joy_ft_t vec2_dot(joy_vec2_t a, joy_vec2_t b)
{
	return ft_add(ft_mul(a.x, b.x), ft_mul(a.y, b.y));
}

JOY_INLINE joy_ft_t vec2_cross(joy_vec2_t a, joy_vec2_t b)
{
	return ft_sub(ft_mul(a.x, b.y), ft_mul(a.y, b.x));
}

JOY_INLINE joy_vec2_t vec2_cross2(joy_vec2_t a, joy_ft_t s)
{
	joy_vec2_t r;
	r.x = ft_mul(s, a.y);
	r.y = ft_mul(-s, a.x);
	return r;
}

JOY_INLINE joy_vec2_t vec2_cross3(joy_ft_t s, joy_vec2_t a)
{
	joy_vec2_t r;
	r.x = ft_mul(-s, a.y);
	r.y = ft_mul(s, a.x);
	return r;
}

JOY_INLINE joy_vec2_t vec2_add(joy_vec2_t a, joy_vec2_t b)
{
	joy_vec2_t r;
	r.x = ft_add(a.x, b.x);
	r.y = ft_add(a.y, b.y);
	return r;
}

JOY_INLINE joy_vec2_t vec2_sub(joy_vec2_t a, joy_vec2_t b)
{
	joy_vec2_t r;
	r.x = ft_sub(a.x, b.x);
	r.y = ft_sub(a.y, b.y);
	return r;
}

JOY_INLINE joy_vec2_t vec2_scale(joy_vec2_t a, joy_ft_t s)
{
	joy_vec2_t r;
	r.x = ft_mul(a.x, s);
	r.y = ft_mul(a.y, s);
	return r;
}

JOY_INLINE joy_vec2_t vec2_abs(joy_vec2_t a)
{
	joy_vec2_t r;
	r.x = ft_abs(a.x);
	r.y = ft_abs(a.y);
	return r;
}

JOY_INLINE joy_ft_t vec2_length_squared(joy_vec2_t a)
{
	return ft_add(ft_pow2(a.x), ft_pow2(a.y));
}

JOY_INLINE joy_ft_t vec2_length(joy_vec2_t a)
{
	return ft_sqrt(ft_add(ft_pow2(a.x), ft_pow2(a.y)));
}

JOY_INLINE joy_vec2_t vec2_normalize(joy_vec2_t a)
{
	joy_vec2_t r = { 0 };
	joy_ft_t length, inv_length;
	length = vec2_length(a);
	if (length != ft_zero()) {
		inv_length = ft_div(ft_one(), length);
		r.x = ft_mul(a.x, inv_length);
		r.y = ft_mul(a.y, inv_length);
	}
	return r;
}

JOY_INLINE joy_vec2_t vec2_normal(joy_vec2_t a)
{
	joy_vec2_t perpbicular;
	perpbicular.x = a.y;
	perpbicular.y = -a.x;
	return vec2_normalize(perpbicular);
}

/*
 * x = x * cos(angle) - y * sin(angle);
 * y = x * sin(angle) + y * cos(angle);
 * vM: 旋转矢量
 *[x, y] [c, -s]
 *		 [s,  c]
*/
JOY_INLINE joy_vec2_t vec2_rotate(joy_vec2_t a, joy_ft_t angle)
{
	joy_ft_t s, c;
	joy_vec2_t r;
	s = ft_sin(angle);
	c = ft_cos(angle);
	r.x = ft_sub(ft_mul(a.x, c), ft_mul(a.y, s));
	r.y = ft_add(ft_mul(a.x, s), ft_mul(a.y, c));
	return r;
}

JOY_INLINE joy_vec2_t vec2_lerp(joy_vec2_t a, joy_vec2_t b, joy_ft_t t)
{
	joy_vec2_t r;
	r.x = ft_lerp(a.x, b.x, t);
	r.y = ft_lerp(a.y, b.y, t);
	return r;
}

JOY_INLINE joy_ft_t vec2_distance(joy_vec2_t a, joy_vec2_t b)
{
	joy_vec2_t diff;
	diff = vec2_sub(b, a);
	return vec2_length(diff);
}

JOY_INLINE joy_vec3_t vec3_negate(joy_vec3_t v)
{
	joy_vec3_t r;
	r.x = -v.x;
	r.y = -v.y;
	r.z = -v.z;
	return r;
}

JOY_INLINE joy_vec3_t vec3_add(joy_vec3_t a, joy_vec3_t b)
{
	joy_vec3_t r;
	r.x = ft_add(a.x, b.x);
	r.y = ft_add(a.y, b.y);
	r.z = ft_add(a.z, b.z);
	return r;
}

JOY_INLINE joy_vec3_t vec3_sub(joy_vec3_t a, joy_vec3_t b)
{
	joy_vec3_t r;
	r.x = ft_sub(a.x, b.x);
	r.y = ft_sub(a.y, b.y);
	r.z = ft_sub(a.z, b.z);
	return r;
}

JOY_INLINE joy_vec3_t vec3_scale(joy_vec3_t a, joy_ft_t s)
{
	joy_vec3_t r;
	r.x = ft_mul(a.x, s);
	r.y = ft_mul(a.y, s);
	r.z = ft_mul(a.z, s);
	return r;
}

JOY_INLINE joy_vec3_t vec3_abs(joy_vec3_t a)
{
	joy_vec3_t r;
	r.x = ft_abs(a.x);
	r.y = ft_abs(a.y);
	r.z = ft_abs(a.z);
	return r;
}

JOY_INLINE joy_ft_t vec3_dot(joy_vec3_t a, joy_vec3_t b)
{
	/* a.x* b.x + a.y * b.y + a.z * b.z; */
	return ft_add(ft_add(ft_mul(a.x, b.x), ft_mul(a.y, b.y)), ft_mul(a.z, b.z));
}

JOY_INLINE joy_vec3_t vec3_cross(joy_vec3_t a, joy_vec3_t b)
{
	/*
	* x = a.y * b.z - a.z * b.y;
	* y = a.z * b.x - a.x * b.z;
	* z = a.x * b.y - a.y * b.x;
	*/
	joy_vec3_t r;
	r.x = ft_sub(ft_mul(a.y, b.z), ft_mul(a.z, b.y));
	r.y = ft_sub(ft_mul(a.z, b.x), ft_mul(a.x, b.z));
	r.z = ft_sub(ft_mul(a.x, b.y), ft_mul(a.y, b.x));
	return r;
}

JOY_INLINE joy_ft_t vec3_square_length(joy_vec3_t a)
{
	/*a.x * a.x + a.y * a.y + a.z * a.z;*/
	return ft_add(ft_add(ft_pow2(a.x), ft_pow2(a.y)), ft_pow2(a.z));
}

JOY_INLINE joy_ft_t vec3_length(joy_vec3_t a)
{
	joy_ft_t square_length;
	square_length = vec3_square_length(a);
	return ft_sqrt(square_length);
}

JOY_INLINE joy_vec3_t vec3_normalize(joy_vec3_t a)
{
	joy_vec3_t r = { 0 };
	joy_ft_t length, inv_length;
	length = vec3_length(a);
	if (length != ft_zero()) {
		inv_length = ft_div(ft_one(), length);
		r.x = ft_mul(a.x, inv_length);
		r.y = ft_mul(a.y, inv_length);
		r.z = ft_mul(a.z, inv_length);
	}
	return r;
}

JOY_INLINE joy_vec3_t vec3_lerp(joy_vec3_t a, joy_vec3_t b, joy_ft_t t)
{
	joy_vec3_t r;
	r.x = ft_lerp(a.x, b.x, t);
	r.y = ft_lerp(a.y, b.y, t);
	r.z = ft_lerp(a.z, b.z, t);
	return r;
}

JOY_INLINE joy_ft_t vec3_distance(joy_vec3_t a, joy_vec3_t b)
{
	joy_vec3_t diff;
	diff = vec3_sub(b, a);
	return vec3_length(diff);
}

JOY_INLINE joy_vec2_t mat22_mul_vec2(joy_mat22_t a, joy_vec2_t b)
{
	joy_vec2_t r;
	r.x = ft_add(ft_mul(a.col1.x, b.x), ft_mul(a.col2.x, b.y));
	r.y = ft_add(ft_mul(a.col1.y, b.x), ft_mul(a.col2.y, b.y));
	return r;
}

JOY_INLINE joy_mat22_t mat22_rotate(joy_ft_t angle)
{
	joy_ft_t c, s;
	joy_mat22_t r;
	c = ft_cos(angle);
	s = ft_sin(angle);
	r.col1.x = c; r.col2.x = -s;
	r.col1.y = s; r.col2.y = c;
	return r;
}

JOY_INLINE joy_mat22_t mat22_transpose(joy_mat22_t m)
{
	joy_mat22_t r;
	r.col1.x = m.col1.x;
	r.col1.y = m.col2.x;
	r.col2.x = m.col1.y;
	r.col2.y = m.col2.y;
	return r;
}

JOY_INLINE joy_mat22_t mat22_invert(joy_mat22_t m)
{
	joy_ft_t a, b, c, d, det;
	joy_mat22_t r;
	a = m.col1.x, b = m.col2.x;
	c = m.col1.y, d = m.col2.y;
	det = ft_sub(ft_mul(a, d), ft_mul(b, c));
	assert(det != ft_zero());
	det = ft_div(ft_one(), det);
	r.col1.x = ft_mul(det, d);
	r.col2.x = ft_mul(-det, b);
	r.col1.y = ft_mul(-det, c);
	r.col2.y = ft_mul(det, a);
	return r;
}

JOY_INLINE joy_mat22_t mat22_add(joy_mat22_t a, joy_mat22_t b)
{
	joy_mat22_t r;
	r.col1 = vec2_add(a.col1, b.col1);
	r.col2 = vec2_add(a.col2, b.col2);
	return r;
}

JOY_INLINE joy_mat22_t mat22_mul(joy_mat22_t a, joy_mat22_t b)
{
	joy_mat22_t r;
	r.col1 = mat22_mul_vec2(a, b.col1);
	r.col2 = mat22_mul_vec2(a, b.col2);
	return r;
}

JOY_INLINE joy_mat22_t mat22_abs(joy_mat22_t a)
{
	joy_mat22_t r;
	r.col1 = vec2_abs(a.col1);
	r.col2 = vec2_abs(a.col2);
	return r;
}


JOY_INLINE joy_mat44_t mat44_mul(joy_mat44_t left, joy_mat44_t right)
{
	// 列主序乘法：result = left · right
	// joy_mat44_t 按列主序存储（每 4 个连续成员为一列），与 GLSL mat4 / 其余矩阵函数约定一致
	// (A·B)[i][j] = Σ_k A[i][k]·B[k][j] = Σ_k left.m_{i+4k} · right.m_{4j+k}
	joy_mat44_t result;
	result.m0  = ft_add(ft_add(ft_mul(left.m0,  right.m0),  ft_mul(left.m4,  right.m1)),  ft_add(ft_mul(left.m8,  right.m2),  ft_mul(left.m12, right.m3)));
	result.m1  = ft_add(ft_add(ft_mul(left.m1,  right.m0),  ft_mul(left.m5,  right.m1)),  ft_add(ft_mul(left.m9,  right.m2),  ft_mul(left.m13, right.m3)));
	result.m2  = ft_add(ft_add(ft_mul(left.m2,  right.m0),  ft_mul(left.m6,  right.m1)),  ft_add(ft_mul(left.m10, right.m2),  ft_mul(left.m14, right.m3)));
	result.m3  = ft_add(ft_add(ft_mul(left.m3,  right.m0),  ft_mul(left.m7,  right.m1)),  ft_add(ft_mul(left.m11, right.m2),  ft_mul(left.m15, right.m3)));
	result.m4  = ft_add(ft_add(ft_mul(left.m0,  right.m4),  ft_mul(left.m4,  right.m5)),  ft_add(ft_mul(left.m8,  right.m6),  ft_mul(left.m12, right.m7)));
	result.m5  = ft_add(ft_add(ft_mul(left.m1,  right.m4),  ft_mul(left.m5,  right.m5)),  ft_add(ft_mul(left.m9,  right.m6),  ft_mul(left.m13, right.m7)));
	result.m6  = ft_add(ft_add(ft_mul(left.m2,  right.m4),  ft_mul(left.m6,  right.m5)),  ft_add(ft_mul(left.m10, right.m6),  ft_mul(left.m14, right.m7)));
	result.m7  = ft_add(ft_add(ft_mul(left.m3,  right.m4),  ft_mul(left.m7,  right.m5)),  ft_add(ft_mul(left.m11, right.m6),  ft_mul(left.m15, right.m7)));
	result.m8  = ft_add(ft_add(ft_mul(left.m0,  right.m8),  ft_mul(left.m4,  right.m9)),  ft_add(ft_mul(left.m8,  right.m10), ft_mul(left.m12, right.m11)));
	result.m9  = ft_add(ft_add(ft_mul(left.m1,  right.m8),  ft_mul(left.m5,  right.m9)),  ft_add(ft_mul(left.m9,  right.m10), ft_mul(left.m13, right.m11)));
	result.m10 = ft_add(ft_add(ft_mul(left.m2,  right.m8),  ft_mul(left.m6,  right.m9)),  ft_add(ft_mul(left.m10, right.m10), ft_mul(left.m14, right.m11)));
	result.m11 = ft_add(ft_add(ft_mul(left.m3,  right.m8),  ft_mul(left.m7,  right.m9)),  ft_add(ft_mul(left.m11, right.m10), ft_mul(left.m15, right.m11)));
	result.m12 = ft_add(ft_add(ft_mul(left.m0,  right.m12), ft_mul(left.m4,  right.m13)), ft_add(ft_mul(left.m8,  right.m14), ft_mul(left.m12, right.m15)));
	result.m13 = ft_add(ft_add(ft_mul(left.m1,  right.m12), ft_mul(left.m5,  right.m13)), ft_add(ft_mul(left.m9,  right.m14), ft_mul(left.m13, right.m15)));
	result.m14 = ft_add(ft_add(ft_mul(left.m2,  right.m12), ft_mul(left.m6,  right.m13)), ft_add(ft_mul(left.m10, right.m14), ft_mul(left.m14, right.m15)));
	result.m15 = ft_add(ft_add(ft_mul(left.m3,  right.m12), ft_mul(left.m7,  right.m13)), ft_add(ft_mul(left.m11, right.m14), ft_mul(left.m15, right.m15)));
	return result;
}

JOY_INLINE joy_mat44_t mat44_translate(joy_ft_t x, joy_ft_t y, joy_ft_t z)
{
	// <2,3,4,1>
	joy_mat44_t result = {
		ft_one(), ft_zero(), ft_zero(), x,
		ft_zero(), ft_one(), ft_zero(), y,
		ft_zero(), ft_zero(), ft_one(), z,
		ft_zero(), ft_zero(), ft_zero(), ft_one()
	};
	return result;
}

JOY_INLINE joy_mat44_t mat44_identity()
{
	joy_mat44_t result = {
		ft_one(), ft_zero(), ft_zero(), ft_zero(),
		ft_zero(), ft_one(), ft_zero(), ft_zero(),
		ft_zero(), ft_zero(), ft_one(), ft_zero(),
		ft_zero(), ft_zero(), ft_zero(), ft_one()
	};
	return result;
}

JOY_INLINE joy_mat44_t mat44_rotate_x(joy_ft_t angle)
{
	joy_mat44_t result;
	joy_ft_t c, s;

	result = mat44_identity();
	c = ft_cos(angle);
	s = ft_sin(angle);

	result.m5 = c;
	result.m6 = s;
	result.m9 = -s;
	result.m10 = c;

	return result;
}

JOY_INLINE joy_mat44_t mat44_rotate_y(joy_ft_t angle)
{
	joy_mat44_t result;
	joy_ft_t c, s;

	result = mat44_identity();
	c = ft_cos(angle);
	s = ft_sin(angle);

	result.m0 = c;
	result.m2 = -s;
	result.m8 = s;
	result.m10 = c;

	return result;
}

JOY_INLINE joy_mat44_t mat44_rotate_z(joy_ft_t angle)
{
	joy_mat44_t result = mat44_identity();
	joy_ft_t c, s;

	c = ft_cos(angle);
	s = ft_sin(angle);

	result.m0 = c;
	result.m1 = s;
	result.m4 = -s;
	result.m5 = c;

	return result;
}

JOY_INLINE joy_mat44_t mat44_rotate(joy_vec3_t axis, joy_ft_t angle)
{
	joy_mat44_t result;
	joy_ft_t x, y, z, length_squared, inv_length;
	joy_ft_t s, c, t;

	x = axis.x, y = axis.y, z = axis.z;
	length_squared = ft_add(ft_add(ft_pow2(x), ft_pow2(y)), ft_pow2(z));

	if ((length_squared != ft_one()) && (length_squared != ft_zero()))
	{
		inv_length = ft_div(ft_one(), ft_sqrt(length_squared));
		x = ft_mul(x, inv_length);
		y = ft_mul(y, inv_length);
		z = ft_mul(z, inv_length);
	}

	s = ft_sin(angle);
	c = ft_cos(angle);
	t = ft_sub(ft_one(), c);

	result.m0 = ft_add(ft_mul(ft_mul(x, x), t), c);
	result.m1 = ft_add(ft_mul(ft_mul(y, x), t), ft_mul(z, s));
	result.m2 = ft_sub(ft_mul(ft_mul(z, x), t), ft_mul(y, s));
	result.m3 = ft_zero();

	result.m4 = ft_sub(ft_mul(ft_mul(x, y), t), ft_mul(z, s));
	result.m5 = ft_add(ft_mul(ft_mul(y, y), t), c);
	result.m6 = ft_add(ft_mul(ft_mul(z, y), t), ft_mul(x, s));
	result.m7 = ft_zero();

	result.m8 = ft_add(ft_mul(ft_mul(x, z), t), ft_mul(y, s));
	result.m9 = ft_sub(ft_mul(ft_mul(y, z), t), ft_mul(x, s));
	result.m10 = ft_add(ft_mul(ft_mul(z, z), t), c);
	result.m11 = ft_zero();

	result.m12 = ft_zero();
	result.m13 = ft_zero();
	result.m14 = ft_zero();
	result.m15 = ft_one();

	return result;
}

JOY_INLINE joy_mat44_t mat44_scale(joy_ft_t x, joy_ft_t y, joy_ft_t z)
{
	joy_mat44_t result = {
		x, ft_zero(), ft_zero(), ft_zero(),
		ft_zero(), y, ft_zero(), ft_zero(),
		ft_zero(), ft_zero(), z, ft_zero(),
		ft_zero(), ft_zero(), ft_zero(), ft_one()
	};
	return result;
}

JOY_INLINE joy_vec4_t mat44_mul_vec4(joy_mat44_t left, joy_vec4_t right)
{
	joy_vec4_t result;
	result.x = ft_add(
		ft_add(ft_mul(left.m0, right.x), ft_mul(left.m4, right.y)),
		ft_add(ft_mul(left.m8, right.z), ft_mul(left.m12, right.w))
	);
	result.y = ft_add(
		ft_add(ft_mul(left.m1, right.x), ft_mul(left.m5, right.y)),
		ft_add(ft_mul(left.m9, right.z), ft_mul(left.m13, right.w))
	);
	result.z = ft_add(
		ft_add(ft_mul(left.m2, right.x), ft_mul(left.m6, right.y)),
		ft_add(ft_mul(left.m10, right.z), ft_mul(left.m14, right.w))
	);
	result.w = ft_add(
		ft_add(ft_mul(left.m3, right.x), ft_mul(left.m7, right.y)),
		ft_add(ft_mul(left.m11, right.z), ft_mul(left.m15, right.w))
	);
	return result;
}

JOY_INLINE joy_vec3_t vec3_div(joy_vec3_t a, joy_ft_t s) {
	joy_vec3_t r;
	r.x = ft_div(a.x, s);
	r.y = ft_div(a.y, s);
	r.z = ft_div(a.z, s);
	return r;
}

JOY_INLINE joy_ft_t ft_radians(joy_ft_t degrees) {
	return degrees * (3.14159265358979323846f / 180.0f);
}

JOY_INLINE joy_mat44_t mat44_perspective(
	joy_ft_t fov, joy_ft_t aspect,
	joy_ft_t near_value, joy_ft_t far_value) {
	joy_ft_t tan_half = tanf(fov / 2.0f);
	joy_mat44_t result = { 0 };
	result.m0 = 1.0f / (aspect * tan_half);
	result.m5 = 1.0f / tan_half;
	result.m10 = far_value / (near_value - far_value);
	result.m11 = -1.0f;
	result.m14 = (near_value * far_value) / (near_value - far_value);
	return result;
}

JOY_INLINE joy_mat44_t mat44_lookAt(joy_vec3_t eye, joy_vec3_t center, joy_vec3_t up) {
	joy_vec3_t f = vec3_normalize(vec3_sub(center, eye));
	joy_vec3_t s = vec3_normalize(vec3_cross(f, up));
	joy_vec3_t u = vec3_cross(s, f);
	joy_mat44_t result = { 0 };
	result.m0 = s.x; result.m1 = s.y; result.m2 = s.z; result.m3 = 0;
	result.m4 = u.x; result.m5 = u.y; result.m6 = u.z; result.m7 = 0;
	result.m8 = -f.x; result.m9 = -f.y; result.m10 = -f.z; result.m11 = 0;
	result.m12 = -vec3_dot(s, eye); result.m13 = -vec3_dot(u, eye);
	result.m14 = vec3_dot(f, eye); result.m15 = 1.0f;
	return result;
}


#endif //!MATHX_H