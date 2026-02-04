/************************************************
Copyright: 2021-2022, lanchong.xyz/Ltd.
File name: liv-math.h
Description: 数学库
Author: ydlc
Version: 1.0
Date: 2023.1.29
History:
*************************************************/
#ifndef LIVNET_MATH_H
#define LIVNET_MATH_H
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "liv-lut.h"
#include <float.h>
#include <math.h>
#include "liv-config.h"


#define FP_NUM_BITS 64
#define FP_FRACTIONAL_PLACES 32
#define FP_ONE (1LL << FP_FRACTIONAL_PLACES)

#define FP_MAX_VALUE 9223372036854775807LL
#define FP_MIN_VALUE -9223372036854775808LL
#define FP_PI_TIMES_2 0x6487ED511
#define FP_PI 0x3243F6A88
#define FP_PI_OVER_2 0x1921FB544
#define FP_LUT_SIZE (int)(FP_PI >> 16)

typedef int64_t fp_t;


typedef struct vec2f {
	fp_t x, y;
} vec2f_t, *vec2f_p;


typedef struct vec3f {
	fp_t x, y, z;
} vec3f_t;


typedef struct vec4f {
	fp_t x, y, z, w;
} vec4f_t;


typedef struct mat22f {
	vec2f_t col1;
	vec2f_t col2;
} mat22f_t;


typedef struct mat44f {
	fp_t m0, m4, m8, m12;  // Matrix first row (4 components)
	fp_t m1, m5, m9, m13;  // Matrix second row (4 components)
	fp_t m2, m6, m10, m14; // Matrix third row (4 components)
	fp_t m3, m7, m11, m15; // Matrix fourth row (4 components)
}mat44f_t;


typedef float ft_t;


typedef struct vec2 {
	ft_t x, y;
} vec2_t;

typedef struct vec3 {
	ft_t x, y, z;
} vec3_t;

typedef struct vec4 {
	ft_t x, y, z, w;
} vec4_t;

typedef struct mat22 {
	vec2_t col1;
	vec2_t col2;
} mat22_t;

typedef struct mat44 {
	ft_t m0, m4, m8, m12;  // Matrix first row (4 components)
	ft_t m1, m5, m9, m13;  // Matrix second row (4 components)
	ft_t m2, m6, m10, m14; // Matrix third row (4 components)
	ft_t m3, m7, m11, m15; // Matrix fourth row (4 components)
}mat44_t;



LIVNET_INLINE fp_t fp_from_float(float a)
{
	return (fp_t)(FP_ONE * a);
}

LIVNET_INLINE float fp_to_float(fp_t a)
{
	return a * 1.0f / FP_ONE;
}

#define FPFF fp_from_float
#define FPTF fp_to_float

LIVNET_INLINE fp_t fp_min_value()
{
	return FP_MIN_VALUE;
}

LIVNET_INLINE fp_t fp_max_value()
{
	return FP_MAX_VALUE;
}

LIVNET_INLINE fp_t fp_add(fp_t a, fp_t b)
{
	return a + b;
}

LIVNET_INLINE fp_t fp_sub(fp_t a, fp_t b)
{
	return a - b;
}

/* fp_mul */
static fp_t fp_mul(fp_t a, fp_t b) 
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

static fp_t fp_div(fp_t a, fp_t b)
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

static fp_t fp_abs(fp_t x)
{
	fp_t r;
	if (x == FP_MIN_VALUE) {
		r = FP_MAX_VALUE;
	}
	else {
		fp_t mask = x >> 63;
		r = (x + mask) ^ mask;
	}
	return r;
}

static fp_t fp_sqrt(fp_t x)
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

static fp_t 
clamp_sin_value(fp_t angle, bool *flip_horizontal, bool *flip_vertical)
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

static fp_t fp_sin(fp_t x)
{
	bool flip_horizontal = false;
	bool flip_vertical = false;
	fp_t clampedL = clamp_sin_value(x, &flip_horizontal, &flip_vertical);
	unsigned int rawIndex = (unsigned int)(clampedL >> 15);
	rawIndex = rawIndex >= FP_LUT_SIZE ? FP_LUT_SIZE - 1 : rawIndex;
	int sin_lut_length = sizeof(SIN_LUT) / sizeof(SIN_LUT[0]);
	int index = flip_horizontal ? sin_lut_length - 1 - rawIndex : rawIndex;
	fp_t nearest_value = SIN_LUT[index];
	return flip_vertical ? -nearest_value : nearest_value;
}

static fp_t fp_cos(fp_t x)
{
	fp_t angle = x + (x > 0 ? -FP_PI - FP_PI_OVER_2 : FP_PI_OVER_2);
	return fp_sin(angle);
}

LIVNET_INLINE fp_t fp_zero()
{
	return 0;
}

LIVNET_INLINE fp_t fp_half()
{
	return fp_from_float(0.5f);
}

LIVNET_INLINE fp_t fp_one()
{
	return fp_from_float(1.0f);
}

LIVNET_INLINE fp_t fp_pi()
{
	return fp_from_float(3.141592658f);
}

LIVNET_INLINE fp_t fp_max(fp_t a, fp_t b)
{
	return a > b ? a : b;
}

LIVNET_INLINE fp_t fp_min(fp_t a, fp_t b)
{
	return a < b ? a : b;
}

LIVNET_INLINE fp_t fp_pow2(fp_t x)
{
	return fp_mul(x, x);
}

LIVNET_INLINE int fp_sign(fp_t x)
{
	return x < 0 ? -1 : 1;
}

LIVNET_INLINE fp_t fp_clamp(fp_t a, fp_t low, fp_t high)
{
	return fp_max(low, fp_min(a, high));
}

LIVNET_INLINE fp_t fp_lerp(fp_t a, fp_t b, fp_t t)
{
	return fp_add(fp_mul(fp_sub(0, t), a), fp_mul(b, t));
}


LIVNET_INLINE vec2f_t vec2f_negate(vec2f_t a)
{
	vec2f_t r;
	r.x = -a.x;
	r.y = -a.y;
	return r;
}

LIVNET_INLINE fp_t vec2f_dot(vec2f_t a, vec2f_t b)
{
	return fp_add(fp_mul(a.x, b.x), fp_mul(a.y, b.y));
}

LIVNET_INLINE fp_t vec2f_cross(vec2f_t a, vec2f_t b)
{
	return fp_sub(fp_mul(a.x, b.y), fp_mul(a.y, b.x));
}

LIVNET_INLINE vec2f_t vec2f_cross2(vec2f_t a, fp_t s)
{
	vec2f_t r;
	r.x = fp_mul(a.y, s);
	r.y = fp_mul(a.x, -s);
	return r;
}

LIVNET_INLINE vec2f_t vec2f_cross3(vec2f_t a, fp_t s)
{
	vec2f_t r;
	r.x = fp_mul(a.y, -s);
	r.y = fp_mul(a.x, s);
	return r;
}

LIVNET_INLINE vec2f_t vec2f_add(vec2f_t a, vec2f_t b)
{
	vec2f_t r;
	r.x = fp_add(a.x, b.x);
	r.y = fp_add(a.y, b.y);
	return r;
}

LIVNET_INLINE vec2f_t vec2f_sub(vec2f_t a, vec2f_t b)
{
	vec2f_t r;
	r.x = fp_sub(a.x, b.x);
	r.y = fp_sub(a.y, b.y);
	return r;
}

LIVNET_INLINE vec2f_t vec2f_scale(vec2f_t a, fp_t s)
{
	vec2f_t r;
	r.x = fp_mul(a.x, s);
	r.y = fp_mul(a.y, s);
	return r;
}

LIVNET_INLINE vec2f_t vec2f_abs(vec2f_t a)
{
	vec2f_t r;
	r.x = fp_abs(a.x);
	r.y = fp_abs(a.y);
	return r;
}

LIVNET_INLINE fp_t vec2f_length_squared(vec2f_t a)
{
	return fp_add(fp_pow2(a.x), fp_pow2(a.y));
}

LIVNET_INLINE fp_t vec2f_length(vec2f_t a)
{
	return fp_sqrt(fp_add(fp_pow2(a.x), fp_pow2(a.y)));
}

LIVNET_INLINE vec2f_t vec2f_normalize(vec2f_t a)
{
	vec2f_t r;
	fp_t length, inv_length;
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


LIVNET_INLINE vec2f_t vec2f_normal(vec2f_t v)
{
	vec2f_t perpbicular;
	perpbicular.x = v.y;
	perpbicular.y = -v.x;
	return vec2f_normalize(perpbicular);
}

/*
 * x = x * cos(angle) - y * sin(angle);
 * y = x * sin(angle) + y * cos(angle);
 * vM: 行矢量
 *[x, y] [c, -s]
 *		 [s,  c]
*/
LIVNET_INLINE vec2f_t vec2f_rotate(vec2f_t v, fp_t angle)
{
	vec2f_t r;
	fp_t s, c;
	s = fp_sin(angle);
	c = fp_cos(angle);
	r.x = fp_sub(fp_mul(v.x, c), fp_mul(v.y, s));
	r.y = fp_add(fp_mul(v.x, s), fp_mul(v.y, c));
	return r;
}

LIVNET_INLINE vec2f_t vec2f_lerp(vec2f_t a, vec2f_t b, fp_t t)
{
	vec2f_t r;
	r.x = fp_lerp(a.x, b.x, t);
	r.y = fp_lerp(a.y, b.y, t);
	return r;
}

LIVNET_INLINE fp_t vec2f_distance(vec2f_t a, vec2f_t b)
{
	vec2f_t diff = vec2f_sub(b, a);
	return vec2f_length(diff);
}

LIVNET_INLINE vec3f_t vec3f_negate(vec3f_t a)
{
	vec3f_t r;
	r.x = -a.x;
	r.y = -a.y;
	r.z = -a.z;
	return r;
}

LIVNET_INLINE vec3f_t vec3f_add(vec3f_t a, vec3f_t b)
{
	vec3f_t r;
	r.x = fp_add(a.x, b.x);
	r.y = fp_add(a.y, b.y);
	r.z = fp_add(a.z, b.z);
	return r;
}

LIVNET_INLINE vec3f_t vec3f_sub(vec3f_t a, vec3f_t b)
{
	vec3f_t r;
	r.x = fp_sub(a.x, b.x);
	r.y = fp_sub(a.y, b.y);
	r.z = fp_sub(a.z, b.z);
	return r;
}

LIVNET_INLINE vec3f_t vec3f_scale(vec3f_t a, fp_t s)
{
	vec3f_t r;
	r.x = fp_mul(a.x, s);
	r.y = fp_mul(a.y, s);
	r.z = fp_mul(a.z, s);
	return r;
}

LIVNET_INLINE vec3f_t vec3f_abs(vec3f_t a)
{
	vec3f_t r;
	r.x = fp_abs(a.x);
	r.y = fp_abs(a.y);
	r.z = fp_abs(a.z);
	return r;
}

LIVNET_INLINE fp_t vec3f_dot(vec3f_t a, vec3f_t b)
{
	fp_t pow2_x, pow2_y, pow2_z;
	pow2_x = fp_mul(a.x, b.x);
	pow2_y = fp_mul(a.y, b.y);
	pow2_z = fp_mul(a.z, b.z);
	return fp_add(fp_add(pow2_x, pow2_y), pow2_z);
}

LIVNET_INLINE vec3f_t vec3f_cross(vec3f_t a, vec3f_t b)
{
	vec3f_t r;
	r.x = fp_sub(fp_mul(a.y, b.z), fp_mul(a.z, b.y));
	r.y = fp_sub(fp_mul(a.z, b.x), fp_mul(a.x, b.z));
	r.z = fp_sub(fp_mul(a.x, b.y), fp_mul(a.y, b.x));
	return r;
}

LIVNET_INLINE fp_t vec3f_square_length(vec3f_t a)
{
	fp_t pow2_x, pow2_y, pow2_z;
	pow2_x = fp_mul(a.x, a.x);
	pow2_y = fp_mul(a.y, a.y);
	pow2_z = fp_mul(a.z, a.z);
	return fp_add(fp_add(pow2_x, pow2_y), pow2_z);
}

LIVNET_INLINE fp_t vec3f_length(vec3f_t a)
{
	fp_t square_length;
	square_length = vec3f_square_length(a);
	return fp_sqrt(square_length);
}

LIVNET_INLINE vec3f_t vec3f_normalize(vec3f_t a)
{
	vec3f_t r;
	fp_t length, inv_length;
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

LIVNET_INLINE vec3f_t vec3f_lerp(vec3f_t a, vec3f_t b, fp_t t)
{
	vec3f_t r;
	r.x = fp_lerp(a.x, b.x, t);
	r.y = fp_lerp(a.y, b.y, t);
	r.z = fp_lerp(a.z, b.z, t);
	return r;
}

LIVNET_INLINE fp_t vec3f_distance(vec3f_t a, vec3f_t b)
{
	vec3f_t diff;
	diff = vec3f_sub(b, a);
	return vec3f_length(diff);
}


LIVNET_INLINE vec2f_t mat22f_mul_vec2f(mat22f_t a, vec2f_t b)
{
	vec2f_t r;
	r.x = fp_add(fp_mul(a.col1.x, b.x), fp_mul(a.col2.x, b.y));
	r.y = fp_add(fp_mul(a.col1.y, b.x), fp_mul(a.col2.y, b.y));
	return r;
}

LIVNET_INLINE mat22f_t mat22f_rotate(fp_t angle)
{
	fp_t c, s;
	mat22f_t r;
	c = fp_cos(angle);
	s = fp_sin(angle);
	r.col1.x = c; r.col2.x = -s;
	r.col1.y = s; r.col2.y = c;
	return r;
}

LIVNET_INLINE mat22f_t mat22f_transpose(mat22f_t m)
{
	mat22f_t r;
	r.col1.x = m.col1.x;
	r.col1.y = m.col2.x;
	r.col2.x = m.col1.y;
	r.col2.y = m.col2.y;
	return r;
}

LIVNET_INLINE mat22f_t mat22f_invert(mat22f_t m)
{
	fp_t a, b, c, d, det, negate_det;
	mat22f_t r;

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

LIVNET_INLINE mat22f_t mat22f_add(mat22f_t a, mat22f_t b)
{
	mat22f_t r;
	r.col1 = vec2f_add(a.col1, b.col1);
	r.col2 = vec2f_add(a.col2, b.col2);
	return r;
}

LIVNET_INLINE mat22f_t mat22f_mul(mat22f_t a, mat22f_t b)
{
	mat22f_t r;
	r.col1 = mat22f_mul_vec2f(a, b.col1);
	r.col2 = mat22f_mul_vec2f(a, b.col2);
	return r;
}

LIVNET_INLINE mat22f_t mat22f_abs(mat22f_t a)
{
	mat22f_t r;
	r.col1 = vec2f_abs(a.col1);
	r.col2 = vec2f_abs(a.col2);
	return r;
}

LIVNET_INLINE mat44f_t mat44f_mul(mat44f_t left, mat44f_t right)
{
	mat44f_t result;
	result.m0 = fp_add(
		fp_add(fp_mul(left.m0, right.m0), fp_mul(left.m1, right.m4)), 
		fp_add(fp_mul(left.m2, right.m8), fp_mul(left.m3, right.m12))
	);
	result.m1 = fp_add(
		fp_add(fp_mul(left.m0, right.m1), fp_mul(left.m1, right.m5)), 
		fp_add(fp_mul(left.m2, right.m9), fp_mul(left.m3, right.m13))
	);
	result.m2 = fp_add(
		fp_add(fp_mul(left.m0, right.m2), fp_mul(left.m1, right.m6)), 
		fp_add(fp_mul(left.m2, right.m10), fp_mul(left.m3, right.m14))
	);
	result.m3 = fp_add(
		fp_add(fp_mul(left.m0, right.m3), fp_mul(left.m1, right.m7)),
		fp_add(fp_mul(left.m2, right.m11), fp_mul(left.m3, right.m15))
	);
	result.m4 = fp_add(
		fp_add(fp_mul(left.m4, right.m0), fp_mul(left.m5, right.m4)),
		fp_add(fp_mul(left.m6, right.m8), fp_mul(left.m7, right.m12))
	);
	result.m5 = fp_add(
		fp_add(fp_mul(left.m4, right.m1), fp_mul(left.m5, right.m5)),
		fp_add(fp_mul(left.m6, right.m9), fp_mul(left.m7, right.m13))
	);
	result.m6 = fp_add(
		fp_add(fp_mul(left.m4, right.m2), fp_mul(left.m5, right.m6)),
		fp_add(fp_mul(left.m6, right.m10), fp_mul(left.m7, right.m14))
	);
	result.m7 = fp_add(
		fp_add(fp_mul(left.m4, right.m3), fp_mul(left.m5, right.m7)),
		fp_add(fp_mul(left.m6, right.m11), fp_mul(left.m7, right.m15))
	);
	result.m8 = fp_add(
		fp_add(fp_mul(left.m8, right.m0), fp_mul(left.m9, right.m4)),
		fp_add(fp_mul(left.m10, right.m8), fp_mul(left.m11, right.m12))
	);
	result.m9 = fp_add(
		fp_add(fp_mul(left.m8, right.m1), fp_mul(left.m9, right.m5)),
		fp_add(fp_mul(left.m10, right.m9), fp_mul(left.m11, right.m13))
	);
	result.m10 = fp_add(
		fp_add(fp_mul(left.m8, right.m2), fp_mul(left.m9, right.m6)),
		fp_add(fp_mul(left.m10, right.m10), fp_mul(left.m11, right.m14))
	);
	result.m11 = fp_add(
		fp_add(fp_mul(left.m8, right.m3), fp_mul(left.m9, right.m7)),
		fp_add(fp_mul(left.m10, right.m11), fp_mul(left.m11, right.m15))
	);
	result.m12 = fp_add(
		fp_add(fp_mul(left.m12, right.m0), fp_mul(left.m13, right.m4)),
		fp_add(fp_mul(left.m14, right.m8), fp_mul(left.m15, right.m12))
	);
	result.m13 = fp_add(
		fp_add(fp_mul(left.m12, right.m1), fp_mul(left.m13, right.m5)),
		fp_add(fp_mul(left.m14, right.m9), fp_mul(left.m15, right.m13))
	);
	result.m14 = fp_add(
		fp_add(fp_mul(left.m12, right.m2), fp_mul(left.m13, right.m6)),
		fp_add(fp_mul(left.m14, right.m10), fp_mul(left.m15, right.m14))
	);
	result.m15 = fp_add(
		fp_add(fp_mul(left.m12, right.m3), fp_mul(left.m13, right.m7)),
		fp_add(fp_mul(left.m14, right.m11), fp_mul(left.m15, right.m15))
	);
	return result;
}

LIVNET_INLINE mat44f_t mat44f_translate(fp_t x, fp_t y, fp_t z)
{
	// <2,3,4,1>
	mat44f_t result = {
		fp_one(), fp_zero(), fp_zero(), x,
		fp_zero(), fp_one(), fp_zero(), y,
		fp_zero(), fp_zero(), fp_one(), z,
		fp_zero(), fp_zero(), fp_zero(), fp_one()
	};
	return result;
}

LIVNET_INLINE mat44f_t mat44f_identity()
{
	mat44f_t result = {
		fp_one(), fp_zero(), fp_zero(), fp_zero(),
		fp_zero(), fp_one(), fp_zero(), fp_zero(),
		fp_zero(), fp_zero(), fp_one(), fp_zero(),
		fp_zero(), fp_zero(), fp_zero(), fp_one()
	};
	return result;
}

LIVNET_INLINE mat44f_t mat44f_rotate_x(fp_t angle)
{
	mat44f_t result;
	fp_t c, s;
	result = mat44f_identity();
	c = fp_cos(angle);
	s = fp_sin(angle);
	result.m5 = c;
	result.m6 = s;
	result.m9 = -s;
	result.m10 = c;
	return result;
}

LIVNET_INLINE mat44f_t mat44f_rotate_y(fp_t angle)
{
	mat44f_t result;
	fp_t c, s;
	
	result = mat44f_identity();
	c = fp_cos(angle);
	s = fp_sin(angle);

	result.m0 = c;
	result.m2 = -s;
	result.m8 = s;
	result.m10 = c;

	return result;
}

LIVNET_INLINE mat44f_t mat44f_rotate_z(fp_t angle)
{
	mat44f_t result;
	fp_t c, s;

	result = mat44f_identity();
	c = fp_cos(angle);
	s = fp_sin(angle);

	result.m0 = c;
	result.m1 = s;
	result.m4 = -s;
	result.m5 = c;

	return result;
}

LIVNET_INLINE mat44f_t mat44f_rotate(vec3f_t axis, fp_t angle)
{
	mat44f_t result;
	fp_t x = axis.x, y = axis.y, z = axis.z;
	fp_t inv_length, length_squared;
	fp_t s, c;

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
	fp_t t = fp_sub(fp_one(), c);

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

LIVNET_INLINE mat44f_t mat44f_scale(fp_t x, fp_t y, fp_t z)
{
	mat44f_t result = {
                x, fp_zero(), fp_zero(), fp_zero(),
                fp_zero(), y, fp_zero(), fp_zero(),
		fp_zero(), fp_zero(), z, fp_zero(),
		fp_zero(), fp_zero(), fp_zero(), fp_one()
	};
	return result;
}

LIVNET_INLINE vec4f_t mat44f_mul_vec4f(mat44f_t left, vec4f_t right)
{
	vec4f_t result;
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



LIVNET_INLINE ft_t ft_from_float(float x)
{
	return x;
}

LIVNET_INLINE float ft_to_float(ft_t x)
{
	return x;
}

#define FTFF ft_from_float
#define FTTf ft_to_float

LIVNET_INLINE ft_t ft_min_value()
{
	return -FLT_MAX;
}

LIVNET_INLINE ft_t ft_max_value()
{
	return FLT_MAX;
}

LIVNET_INLINE ft_t ft_add(ft_t a, ft_t b)
{
	return a + b;
}

LIVNET_INLINE ft_t ft_sub(ft_t a, ft_t b)
{
	return a - b;
}

LIVNET_INLINE ft_t ft_mul(ft_t a, ft_t b)
{
	return a * b;
}

LIVNET_INLINE ft_t ft_div(ft_t a, ft_t b)
{
	return a / b;
}

LIVNET_INLINE ft_t ft_abs(ft_t x)
{
	return x > 0.0f ? x : -x;
}

LIVNET_INLINE ft_t ft_sqrt(ft_t x)
{
	return sqrtf(x);
}

LIVNET_INLINE ft_t ft_sin(ft_t x)
{
	return sinf(x);
}


LIVNET_INLINE ft_t ft_cos(ft_t x)
{
	return cosf(x);
}

LIVNET_INLINE ft_t ft_zero()
{
	return 0;
}

LIVNET_INLINE ft_t ft_half()
{
	return 0.5f;
}

LIVNET_INLINE ft_t ft_one()
{
	return 1.0f;
}

LIVNET_INLINE ft_t ft_pi()
{
	return 3.141592658f;
}

LIVNET_INLINE ft_t ft_max(ft_t a, ft_t b)
{
	return a > b ? a : b;
}

LIVNET_INLINE ft_t ft_min(ft_t a, ft_t b)
{
	return a < b ? a : b;
}

LIVNET_INLINE ft_t ft_pow2(ft_t x)
{
	return x * x;
}

LIVNET_INLINE int ft_sign(ft_t x)
{
	return x < 0 ? -1 : 1;
}

LIVNET_INLINE ft_t ft_clamp(ft_t a, ft_t low, ft_t high)
{
	return ft_max(low, ft_min(a, high));
}

LIVNET_INLINE ft_t ft_lerp(ft_t a, ft_t b, ft_t t)
{
	return (1 - t) * a + b * t;
}

LIVNET_INLINE vec2_t vec2_negate(vec2_t v)
{
	vec2_t r;
	r.x = -v.x;
	r.y = -v.y;
	return r;
}

LIVNET_INLINE ft_t vec2_dot(vec2_t a, vec2_t b)
{
	return ft_add(ft_mul(a.x, b.x), ft_mul(a.y, b.y));
}

LIVNET_INLINE ft_t vec2_cross(vec2_t a, vec2_t b)
{
	return ft_sub(ft_mul(a.x, b.y), ft_mul(a.y, b.x));
}

LIVNET_INLINE vec2_t vec2_cross2(vec2_t a, ft_t s)
{
	vec2_t r;
	r.x = ft_mul(s, a.y);
	r.y = ft_mul(-s, a.x);
	return r;
}

LIVNET_INLINE vec2_t vec2_cross3(ft_t s, vec2_t a)
{
	vec2_t r;
	r.x = ft_mul(-s, a.y);
	r.y = ft_mul(s, a.x);
	return r;
}

LIVNET_INLINE vec2_t vec2_add(vec2_t a, vec2_t b)
{
	vec2_t r;
	r.x = ft_add(a.x, b.x);
	r.y = ft_add(a.y, b.y);
	return r;
}

LIVNET_INLINE vec2_t vec2_sub(vec2_t a, vec2_t b)
{
	vec2_t r;
	r.x = ft_sub(a.x, b.x);
	r.y = ft_sub(a.y, b.y);
	return r;
}

LIVNET_INLINE vec2_t vec2_scale(vec2_t a, ft_t s)
{
	vec2_t r;
	r.x = ft_mul(a.x, s);
	r.y = ft_mul(a.y, s);
	return r;
}

LIVNET_INLINE vec2_t vec2_abs(vec2_t a)
{
	vec2_t r;
	r.x = ft_abs(a.x);
	r.y = ft_abs(a.y);
	return r;
}

LIVNET_INLINE ft_t vec2_length_squared(vec2_t a)
{
	return ft_add(ft_pow2(a.x), ft_pow2(a.y));
}

LIVNET_INLINE ft_t vec2_length(vec2_t a)
{
	return ft_sqrt(ft_add(ft_pow2(a.x), ft_pow2(a.y)));
}

LIVNET_INLINE vec2_t vec2_normalize(vec2_t a)
{
	vec2_t r;
	ft_t length, inv_length;
	length = vec2_length(a);
	if (length != ft_zero()) {
		inv_length = ft_div(ft_one(), length);
		r.x = ft_mul(a.x, inv_length);
		r.y = ft_mul(a.y, inv_length);
	}
	return r;
}

LIVNET_INLINE vec2_t vec2_normal(vec2_t a)
{
	vec2_t perpbicular;
	perpbicular.x = a.y;
	perpbicular.y = -a.x;
	return vec2_normalize(perpbicular);
}

/*
 * x = x * cos(angle) - y * sin(angle);
 * y = x * sin(angle) + y * cos(angle);
 * vM: 行矢量
 *[x, y] [c, -s]
 *		 [s,  c]
*/
LIVNET_INLINE vec2_t vec2_rotate(vec2_t a, ft_t angle)
{
	ft_t s, c;
	vec2_t r;
	s = ft_sin(angle);
	c = ft_cos(angle);
	r.x = ft_sub(ft_mul(a.x, c), ft_mul(a.y, s));
	r.y = ft_add(ft_mul(a.x, s), ft_mul(a.y, c));
	return r;
}

LIVNET_INLINE vec2_t vec2_lerp(vec2_t a, vec2_t b, ft_t t)
{
	vec2_t r;
	r.x = ft_lerp(a.x, b.x, t);
	r.y = ft_lerp(a.y, b.y, t);
	return r;
}

LIVNET_INLINE ft_t vec2_distance(vec2_t a, vec2_t b)
{
	vec2_t diff;
	diff = vec2_sub(b, a);
	return vec2_length(diff);
}

LIVNET_INLINE vec3_t vec3_negate(vec3_t v)
{
	vec3_t r;
	r.x = -v.x;
	r.y = -v.y;
	r.z = -v.z;
	return r;
}

LIVNET_INLINE vec3_t vec3_add(vec3_t a, vec3_t b)
{
	vec3_t r;
	r.x = ft_add(a.x, b.x);
	r.y = ft_add(a.y, b.y);
	r.z = ft_add(a.z, b.z);
	return r;
}

LIVNET_INLINE vec3_t vec3_sub(vec3_t a, vec3_t b)
{
	vec3_t r;
	r.x = ft_sub(a.x, b.x);
	r.y = ft_sub(a.y, b.y);
	r.z = ft_sub(a.z, b.z);
	return r;
}

LIVNET_INLINE vec3_t vec3_scale(vec3_t a, ft_t s)
{
	vec3_t r;
	r.x = ft_mul(a.x, s);
	r.y = ft_mul(a.y, s);
	r.z = ft_mul(a.z, s);
	return r;
}

LIVNET_INLINE vec3_t vec3_abs(vec3_t a)
{
	vec3_t r;
	r.x = ft_abs(a.x);
	r.y = ft_abs(a.y);
	r.z = ft_abs(a.z);
	return r;
}

LIVNET_INLINE ft_t vec3_dot(vec3_t a, vec3_t b)
{
	/* a.x* b.x + a.y * b.y + a.z * b.z; */
	return ft_add(ft_add(ft_mul(a.x, b.x), ft_mul(a.y, b.y)), ft_mul(a.z, b.z));
}

LIVNET_INLINE vec3_t vec3_cross(vec3_t a, vec3_t b)
{
	/*
	* x = a.y * b.z - a.z * b.y;
	* y = a.z * b.x - a.x * b.z;
	* z = a.x * b.y - a.y * b.x;
	*/
	vec3_t r;
	r.x = ft_sub(ft_mul(a.y, b.z), ft_mul(a.z, b.y));
	r.y = ft_sub(ft_mul(a.z, b.x), ft_mul(a.x, b.z));
	r.z = ft_sub(ft_mul(a.x, b.y), ft_mul(a.y, b.x));
	return r;
}

LIVNET_INLINE ft_t vec3_square_length(vec3_t a)
{
	/*a.x * a.x + a.y * a.y + a.z * a.z;*/
	return ft_add(ft_add(ft_pow2(a.x), ft_pow2(a.y)), ft_pow2(a.z));
}

LIVNET_INLINE ft_t vec3_length(vec3_t a)
{
	ft_t square_length;
	square_length = vec3_square_length(a);
	return ft_sqrt(square_length);
}

LIVNET_INLINE vec3_t vec3_normalize(vec3_t a)
{
	vec3_t r;
	ft_t length, inv_length;
	length = vec3_length(a);
	if (length != ft_zero()) {
		inv_length = ft_div(ft_one(), length);
		r.x = ft_mul(a.x, inv_length);
		r.y = ft_mul(a.y, inv_length);
		r.z = ft_mul(a.z, inv_length);
	}
	return r;
}

LIVNET_INLINE vec3_t vec3_lerp(vec3_t a, vec3_t b, ft_t t)
{
	vec3_t r;
	r.x = ft_lerp(a.x, b.x, t);
	r.y = ft_lerp(a.y, b.y, t);
	r.z = ft_lerp(a.z, b.z, t);
	return r;
}

LIVNET_INLINE ft_t vec3_distance(vec3_t a, vec3_t b)
{
	vec3_t diff;
	diff = vec3_sub(b, a);
	return vec3_length(diff);
}

LIVNET_INLINE vec2_t mat22_mul_vec2(mat22_t a, vec2_t b)
{
	vec2_t r;
	r.x = ft_add(ft_mul(a.col1.x, b.x), ft_mul(a.col2.x, b.y));
	r.y = ft_add(ft_mul(a.col1.y, b.x), ft_mul(a.col2.y, b.y));
	return r;
}

LIVNET_INLINE mat22_t mat22_rotate(ft_t angle)
{
	ft_t c, s;
	mat22_t r;
	c = ft_cos(angle);
	s = ft_sin(angle);
	r.col1.x = c; r.col2.x = -s;
	r.col1.y = s; r.col2.y = c;
	return r;
}

LIVNET_INLINE mat22_t mat22_transpose(mat22_t m)
{
	mat22_t r;
	r.col1.x = m.col1.x;
	r.col1.y = m.col2.x;
	r.col2.x = m.col1.y;
	r.col2.y = m.col2.y;
	return r;
}

LIVNET_INLINE mat22_t mat22_invert(mat22_t m)
{
	ft_t a, b, c, d, det;
	mat22_t r;
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

LIVNET_INLINE mat22_t mat22_add(mat22_t a, mat22_t b)
{
	mat22_t r;
	r.col1 = vec2_add(a.col1, b.col1);
	r.col2 = vec2_add(a.col2, b.col2);
	return r;
}

LIVNET_INLINE mat22_t mat22_mul(mat22_t a, mat22_t b)
{
	mat22_t r;
	r.col1 = mat22_mul_vec2(a, b.col1);
	r.col2 = mat22_mul_vec2(a, b.col2);
	return r;
}

LIVNET_INLINE mat22_t mat22_abs(mat22_t a)
{
	mat22_t r;
	r.col1 = vec2_abs(a.col1);
	r.col2 = vec2_abs(a.col2);
	return r;
}


LIVNET_INLINE mat44_t mat44_mul(mat44_t left, mat44_t right)
{
	mat44_t result;
	result.m0 = ft_add(
		ft_add(ft_mul(left.m0, right.m0), ft_mul(left.m1, right.m4)),
		ft_add(ft_mul(left.m2, right.m8), ft_mul(left.m3, right.m12))
	);
	result.m1 = ft_add(
		ft_add(ft_mul(left.m0, right.m1), ft_mul(left.m1, right.m5)),
		ft_add(ft_mul(left.m2, right.m9), ft_mul(left.m3, right.m13))
	);
	result.m2 = ft_add(
		ft_add(ft_mul(left.m0, right.m2), ft_mul(left.m1, right.m6)),
		ft_add(ft_mul(left.m2, right.m10), ft_mul(left.m3, right.m14))
	);
	result.m3 = ft_add(
		ft_add(ft_mul(left.m0, right.m3), ft_mul(left.m1, right.m7)),
		ft_add(ft_mul(left.m2, right.m11), ft_mul(left.m3, right.m15))
	);
	result.m4 = ft_add(
		ft_add(ft_mul(left.m4, right.m0), ft_mul(left.m5, right.m4)),
		ft_add(ft_mul(left.m6, right.m8), ft_mul(left.m7, right.m12))
	);
	result.m5 = ft_add(
		ft_add(ft_mul(left.m4, right.m1), ft_mul(left.m5, right.m5)),
		ft_add(ft_mul(left.m6, right.m9), ft_mul(left.m7, right.m13))
	);
	result.m6 = ft_add(
		ft_add(ft_mul(left.m4, right.m2), ft_mul(left.m5, right.m6)),
		ft_add(ft_mul(left.m6, right.m10), ft_mul(left.m7, right.m14))
	);
	result.m7 = ft_add(
		ft_add(ft_mul(left.m4, right.m3), ft_mul(left.m5, right.m7)),
		ft_add(ft_mul(left.m6, right.m11), ft_mul(left.m7, right.m15))
	);
	result.m8 = ft_add(
		ft_add(ft_mul(left.m8, right.m0), ft_mul(left.m9, right.m4)),
		ft_add(ft_mul(left.m10, right.m8), ft_mul(left.m11, right.m12))
	);
	result.m9 = ft_add(
		ft_add(ft_mul(left.m8, right.m1), ft_mul(left.m9, right.m5)),
		ft_add(ft_mul(left.m10, right.m9), ft_mul(left.m11, right.m13))
	);
	result.m10 = ft_add(
		ft_add(ft_mul(left.m8, right.m2), ft_mul(left.m9, right.m6)),
		ft_add(ft_mul(left.m10, right.m10), ft_mul(left.m11, right.m14))
	);
	result.m11 = ft_add(
		ft_add(ft_mul(left.m8, right.m3), ft_mul(left.m9, right.m7)),
		ft_add(ft_mul(left.m10, right.m11), ft_mul(left.m11, right.m15))
	);
	result.m12 = ft_add(
		ft_add(ft_mul(left.m12, right.m0), ft_mul(left.m13, right.m4)),
		ft_add(ft_mul(left.m14, right.m8), ft_mul(left.m15, right.m12))
	);
	result.m13 = ft_add(
		ft_add(ft_mul(left.m12, right.m1), ft_mul(left.m13, right.m5)),
		ft_add(ft_mul(left.m14, right.m9), ft_mul(left.m15, right.m13))
	);
	result.m14 = ft_add(
		ft_add(ft_mul(left.m12, right.m2), ft_mul(left.m13, right.m6)),
		ft_add(ft_mul(left.m14, right.m10), ft_mul(left.m15, right.m14))
	);
	result.m15 = ft_add(
		ft_add(ft_mul(left.m12, right.m3), ft_mul(left.m13, right.m7)),
		ft_add(ft_mul(left.m14, right.m11), ft_mul(left.m15, right.m15))
	);
	return result;
}

LIVNET_INLINE mat44_t mat44_translate(ft_t x, ft_t y, ft_t z)
{
	// <2,3,4,1>
	mat44_t result = {
		ft_one(), ft_zero(), ft_zero(), x,
		ft_zero(), ft_one(), ft_zero(), y,
		ft_zero(), ft_zero(), ft_one(), z,
		ft_zero(), ft_zero(), ft_zero(), ft_one()
	};
	return result;
}

LIVNET_INLINE mat44_t mat44_identity()
{
	mat44_t result = {
		ft_one(), ft_zero(), ft_zero(), ft_zero(),
		ft_zero(), ft_one(), ft_zero(), ft_zero(),
		ft_zero(), ft_zero(), ft_one(), ft_zero(),
		ft_zero(), ft_zero(), ft_zero(), ft_one()
	};
	return result;
}

LIVNET_INLINE mat44_t mat44_rotate_x(ft_t angle)
{
	mat44_t result;
	ft_t c, s;

	result = mat44_identity();
	c = ft_cos(angle);
	s = ft_sin(angle);

	result.m5 = c;
	result.m6 = s;
	result.m9 = -s;
	result.m10 = c;

	return result;
}

LIVNET_INLINE mat44_t mat44_rotate_y(ft_t angle)
{
	mat44_t result;
	ft_t c, s;

	result = mat44_identity();
	c = ft_cos(angle);
	s = ft_sin(angle);

	result.m0 = c;
	result.m2 = -s;
	result.m8 = s;
	result.m10 = c;

	return result;
}

LIVNET_INLINE mat44_t mat44_rotate_z(ft_t angle)
{
	mat44_t result = mat44_identity();
	ft_t c, s;

	c = ft_cos(angle);
	s = ft_sin(angle);

	result.m0 = c;
	result.m1 = s;
	result.m4 = -s;
	result.m5 = c;

	return result;
}

LIVNET_INLINE mat44_t mat44_rotate(vec3_t axis, ft_t angle)
{
	mat44_t result;
	ft_t x, y, z, length_squared, inv_length;
	ft_t s, c, t;

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

LIVNET_INLINE mat44_t mat44_scale(ft_t x, ft_t y, ft_t z)
{
	mat44_t result = {
		x, ft_zero(), ft_zero(), ft_zero(),
		ft_zero(), y, ft_zero(), ft_zero(),
		ft_zero(), ft_zero(), z, ft_zero(),
		ft_zero(), ft_zero(), ft_zero(), ft_one()
	};
	return result;
}

LIVNET_INLINE vec4_t mat44_mul_vec4(mat44_t left, vec4_t right)
{
	vec4_t result;
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


#endif //!LIVNET_MATH_H