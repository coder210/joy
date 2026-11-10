/* 自动生成，勿手改：由 api/tools/jmath_wrap.js 从 jmath.h 产出。
 * jmath.h 的数学函数是 static inline，这里给它们一层外部链接的全标量包装，
 * 让 jmath_wrappers.h 里的 JOY_API 声明能进 IDL、生成三端 thunk。
 * 改 jmath.h 后重跑 node api/tools/jmath_wrap.js。 */
#include "joy.h"
#include <string.h>

int64_t joy_math_fp_from_float(float a)
{
        return (int64_t)fp_from_float(a);
}

float joy_math_fp_to_float(int64_t a)
{
        return (float)fp_to_float(a);
}

int64_t joy_math_fp_from_uint(uint32_t a)
{
        return (int64_t)fp_from_uint(a);
}

int64_t joy_math_fp_from_int(int32_t a)
{
        return (int64_t)fp_from_int(a);
}

int64_t joy_math_fp_from_fraction(uint32_t a)
{
        return (int64_t)fp_from_fraction(a);
}

int64_t joy_math_fp_min_value(void)
{
        return (int64_t)fp_min_value();
}

int64_t joy_math_fp_max_value(void)
{
        return (int64_t)fp_max_value();
}

int64_t joy_math_fp_add(int64_t a, int64_t b)
{
        return (int64_t)fp_add(a, b);
}

int64_t joy_math_fp_sub(int64_t a, int64_t b)
{
        return (int64_t)fp_sub(a, b);
}

int64_t joy_math_fp_zero(void)
{
        return (int64_t)fp_zero();
}

int64_t joy_math_fp_half(void)
{
        return (int64_t)fp_half();
}

int64_t joy_math_fp_one(void)
{
        return (int64_t)fp_one();
}

int64_t joy_math_fp_pi(void)
{
        return (int64_t)fp_pi();
}

int64_t joy_math_fp_max(int64_t a, int64_t b)
{
        return (int64_t)fp_max(a, b);
}

int64_t joy_math_fp_min(int64_t a, int64_t b)
{
        return (int64_t)fp_min(a, b);
}

int64_t joy_math_fp_pow2(int64_t x)
{
        return (int64_t)fp_pow2(x);
}

int32_t joy_math_fp_sign(int64_t x)
{
        return (int32_t)fp_sign(x);
}

int64_t joy_math_fp_clamp(int64_t a, int64_t low, int64_t high)
{
        return (int64_t)fp_clamp(a, low, high);
}

int64_t joy_math_fp_lerp(int64_t a, int64_t b, int64_t t)
{
        return (int64_t)fp_lerp(a, b, t);
}

int64_t joy_math_vec2f_negate_x(int64_t a_x, int64_t a_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t _r;
        _r = vec2f_negate(a);
        return (int64_t)_r.x;
}

int64_t joy_math_vec2f_negate_y(int64_t a_x, int64_t a_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t _r;
        _r = vec2f_negate(a);
        return (int64_t)_r.y;
}

int64_t joy_math_vec2f_dot(int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t b;
        b.x = b_x;
        b.y = b_y;
        return (int64_t)vec2f_dot(a, b);
}

int64_t joy_math_vec2f_cross(int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t b;
        b.x = b_x;
        b.y = b_y;
        return (int64_t)vec2f_cross(a, b);
}

int64_t joy_math_vec2f_cross2_x(int64_t a_x, int64_t a_y, int64_t s)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t _r;
        _r = vec2f_cross2(a, s);
        return (int64_t)_r.x;
}

int64_t joy_math_vec2f_cross2_y(int64_t a_x, int64_t a_y, int64_t s)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t _r;
        _r = vec2f_cross2(a, s);
        return (int64_t)_r.y;
}

int64_t joy_math_vec2f_cross3_x(int64_t a_x, int64_t a_y, int64_t s)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t _r;
        _r = vec2f_cross3(a, s);
        return (int64_t)_r.x;
}

int64_t joy_math_vec2f_cross3_y(int64_t a_x, int64_t a_y, int64_t s)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t _r;
        _r = vec2f_cross3(a, s);
        return (int64_t)_r.y;
}

int64_t joy_math_vec2f_add_x(int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t b;
        b.x = b_x;
        b.y = b_y;
        vec2f_t _r;
        _r = vec2f_add(a, b);
        return (int64_t)_r.x;
}

int64_t joy_math_vec2f_add_y(int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t b;
        b.x = b_x;
        b.y = b_y;
        vec2f_t _r;
        _r = vec2f_add(a, b);
        return (int64_t)_r.y;
}

int64_t joy_math_vec2f_sub_x(int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t b;
        b.x = b_x;
        b.y = b_y;
        vec2f_t _r;
        _r = vec2f_sub(a, b);
        return (int64_t)_r.x;
}

int64_t joy_math_vec2f_sub_y(int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t b;
        b.x = b_x;
        b.y = b_y;
        vec2f_t _r;
        _r = vec2f_sub(a, b);
        return (int64_t)_r.y;
}

int64_t joy_math_vec2f_scale_x(int64_t a_x, int64_t a_y, int64_t s)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t _r;
        _r = vec2f_scale(a, s);
        return (int64_t)_r.x;
}

int64_t joy_math_vec2f_scale_y(int64_t a_x, int64_t a_y, int64_t s)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t _r;
        _r = vec2f_scale(a, s);
        return (int64_t)_r.y;
}

int64_t joy_math_vec2f_abs_x(int64_t a_x, int64_t a_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t _r;
        _r = vec2f_abs(a);
        return (int64_t)_r.x;
}

int64_t joy_math_vec2f_abs_y(int64_t a_x, int64_t a_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t _r;
        _r = vec2f_abs(a);
        return (int64_t)_r.y;
}

int64_t joy_math_vec2f_length_squared(int64_t a_x, int64_t a_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        return (int64_t)vec2f_length_squared(a);
}

int64_t joy_math_vec2f_length(int64_t a_x, int64_t a_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        return (int64_t)vec2f_length(a);
}

int64_t joy_math_vec2f_normalize_x(int64_t a_x, int64_t a_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t _r;
        _r = vec2f_normalize(a);
        return (int64_t)_r.x;
}

int64_t joy_math_vec2f_normalize_y(int64_t a_x, int64_t a_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t _r;
        _r = vec2f_normalize(a);
        return (int64_t)_r.y;
}

int64_t joy_math_vec2f_normal_x(int64_t v_x, int64_t v_y)
{
        vec2f_t v;
        v.x = v_x;
        v.y = v_y;
        vec2f_t _r;
        _r = vec2f_normal(v);
        return (int64_t)_r.x;
}

int64_t joy_math_vec2f_normal_y(int64_t v_x, int64_t v_y)
{
        vec2f_t v;
        v.x = v_x;
        v.y = v_y;
        vec2f_t _r;
        _r = vec2f_normal(v);
        return (int64_t)_r.y;
}

int64_t joy_math_vec2f_rotate_x(int64_t v_x, int64_t v_y, int64_t angle)
{
        vec2f_t v;
        v.x = v_x;
        v.y = v_y;
        vec2f_t _r;
        _r = vec2f_rotate(v, angle);
        return (int64_t)_r.x;
}

int64_t joy_math_vec2f_rotate_y(int64_t v_x, int64_t v_y, int64_t angle)
{
        vec2f_t v;
        v.x = v_x;
        v.y = v_y;
        vec2f_t _r;
        _r = vec2f_rotate(v, angle);
        return (int64_t)_r.y;
}

int64_t joy_math_vec2f_lerp_x(int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y, int64_t t)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t b;
        b.x = b_x;
        b.y = b_y;
        vec2f_t _r;
        _r = vec2f_lerp(a, b, t);
        return (int64_t)_r.x;
}

int64_t joy_math_vec2f_lerp_y(int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y, int64_t t)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t b;
        b.x = b_x;
        b.y = b_y;
        vec2f_t _r;
        _r = vec2f_lerp(a, b, t);
        return (int64_t)_r.y;
}

int64_t joy_math_vec2f_distance(int64_t a_x, int64_t a_y, int64_t b_x, int64_t b_y)
{
        vec2f_t a;
        a.x = a_x;
        a.y = a_y;
        vec2f_t b;
        b.x = b_x;
        b.y = b_y;
        return (int64_t)vec2f_distance(a, b);
}

int64_t joy_math_vec3f_negate_x(int64_t a_x, int64_t a_y, int64_t a_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t _r;
        _r = vec3f_negate(a);
        return (int64_t)_r.x;
}

int64_t joy_math_vec3f_negate_y(int64_t a_x, int64_t a_y, int64_t a_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t _r;
        _r = vec3f_negate(a);
        return (int64_t)_r.y;
}

int64_t joy_math_vec3f_negate_z(int64_t a_x, int64_t a_y, int64_t a_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t _r;
        _r = vec3f_negate(a);
        return (int64_t)_r.z;
}

int64_t joy_math_vec3f_add_x(int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3f_t _r;
        _r = vec3f_add(a, b);
        return (int64_t)_r.x;
}

int64_t joy_math_vec3f_add_y(int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3f_t _r;
        _r = vec3f_add(a, b);
        return (int64_t)_r.y;
}

int64_t joy_math_vec3f_add_z(int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3f_t _r;
        _r = vec3f_add(a, b);
        return (int64_t)_r.z;
}

int64_t joy_math_vec3f_sub_x(int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3f_t _r;
        _r = vec3f_sub(a, b);
        return (int64_t)_r.x;
}

int64_t joy_math_vec3f_sub_y(int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3f_t _r;
        _r = vec3f_sub(a, b);
        return (int64_t)_r.y;
}

int64_t joy_math_vec3f_sub_z(int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3f_t _r;
        _r = vec3f_sub(a, b);
        return (int64_t)_r.z;
}

int64_t joy_math_vec3f_scale_x(int64_t a_x, int64_t a_y, int64_t a_z, int64_t s)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t _r;
        _r = vec3f_scale(a, s);
        return (int64_t)_r.x;
}

int64_t joy_math_vec3f_scale_y(int64_t a_x, int64_t a_y, int64_t a_z, int64_t s)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t _r;
        _r = vec3f_scale(a, s);
        return (int64_t)_r.y;
}

int64_t joy_math_vec3f_scale_z(int64_t a_x, int64_t a_y, int64_t a_z, int64_t s)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t _r;
        _r = vec3f_scale(a, s);
        return (int64_t)_r.z;
}

int64_t joy_math_vec3f_abs_x(int64_t a_x, int64_t a_y, int64_t a_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t _r;
        _r = vec3f_abs(a);
        return (int64_t)_r.x;
}

int64_t joy_math_vec3f_abs_y(int64_t a_x, int64_t a_y, int64_t a_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t _r;
        _r = vec3f_abs(a);
        return (int64_t)_r.y;
}

int64_t joy_math_vec3f_abs_z(int64_t a_x, int64_t a_y, int64_t a_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t _r;
        _r = vec3f_abs(a);
        return (int64_t)_r.z;
}

int64_t joy_math_vec3f_dot(int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        return (int64_t)vec3f_dot(a, b);
}

int64_t joy_math_vec3f_cross_x(int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3f_t _r;
        _r = vec3f_cross(a, b);
        return (int64_t)_r.x;
}

int64_t joy_math_vec3f_cross_y(int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3f_t _r;
        _r = vec3f_cross(a, b);
        return (int64_t)_r.y;
}

int64_t joy_math_vec3f_cross_z(int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3f_t _r;
        _r = vec3f_cross(a, b);
        return (int64_t)_r.z;
}

int64_t joy_math_vec3f_square_length(int64_t a_x, int64_t a_y, int64_t a_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        return (int64_t)vec3f_square_length(a);
}

int64_t joy_math_vec3f_length(int64_t a_x, int64_t a_y, int64_t a_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        return (int64_t)vec3f_length(a);
}

int64_t joy_math_vec3f_normalize_x(int64_t a_x, int64_t a_y, int64_t a_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t _r;
        _r = vec3f_normalize(a);
        return (int64_t)_r.x;
}

int64_t joy_math_vec3f_normalize_y(int64_t a_x, int64_t a_y, int64_t a_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t _r;
        _r = vec3f_normalize(a);
        return (int64_t)_r.y;
}

int64_t joy_math_vec3f_normalize_z(int64_t a_x, int64_t a_y, int64_t a_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t _r;
        _r = vec3f_normalize(a);
        return (int64_t)_r.z;
}

int64_t joy_math_vec3f_lerp_x(int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z, int64_t t)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3f_t _r;
        _r = vec3f_lerp(a, b, t);
        return (int64_t)_r.x;
}

int64_t joy_math_vec3f_lerp_y(int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z, int64_t t)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3f_t _r;
        _r = vec3f_lerp(a, b, t);
        return (int64_t)_r.y;
}

int64_t joy_math_vec3f_lerp_z(int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z, int64_t t)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3f_t _r;
        _r = vec3f_lerp(a, b, t);
        return (int64_t)_r.z;
}

int64_t joy_math_vec3f_distance(int64_t a_x, int64_t a_y, int64_t a_z, int64_t b_x, int64_t b_y, int64_t b_z)
{
        vec3f_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3f_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        return (int64_t)vec3f_distance(a, b);
}

int64_t joy_math_mat22f_mul_vec2f_x(int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_x, int64_t b_y)
{
        mat22f_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        vec2f_t b;
        b.x = b_x;
        b.y = b_y;
        vec2f_t _r;
        _r = mat22f_mul_vec2f(a, b);
        return (int64_t)_r.x;
}

int64_t joy_math_mat22f_mul_vec2f_y(int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_x, int64_t b_y)
{
        mat22f_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        vec2f_t b;
        b.x = b_x;
        b.y = b_y;
        vec2f_t _r;
        _r = mat22f_mul_vec2f(a, b);
        return (int64_t)_r.y;
}

int64_t joy_math_mat22f_rotate_col1_x(int64_t angle)
{
        mat22f_t _r;
        _r = mat22f_rotate(angle);
        return (int64_t)_r.col1.x;
}

int64_t joy_math_mat22f_rotate_col1_y(int64_t angle)
{
        mat22f_t _r;
        _r = mat22f_rotate(angle);
        return (int64_t)_r.col1.y;
}

int64_t joy_math_mat22f_rotate_col2_x(int64_t angle)
{
        mat22f_t _r;
        _r = mat22f_rotate(angle);
        return (int64_t)_r.col2.x;
}

int64_t joy_math_mat22f_rotate_col2_y(int64_t angle)
{
        mat22f_t _r;
        _r = mat22f_rotate(angle);
        return (int64_t)_r.col2.y;
}

int64_t joy_math_mat22f_transpose_col1_x(int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        mat22f_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22f_t _r;
        _r = mat22f_transpose(m);
        return (int64_t)_r.col1.x;
}

int64_t joy_math_mat22f_transpose_col1_y(int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        mat22f_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22f_t _r;
        _r = mat22f_transpose(m);
        return (int64_t)_r.col1.y;
}

int64_t joy_math_mat22f_transpose_col2_x(int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        mat22f_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22f_t _r;
        _r = mat22f_transpose(m);
        return (int64_t)_r.col2.x;
}

int64_t joy_math_mat22f_transpose_col2_y(int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        mat22f_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22f_t _r;
        _r = mat22f_transpose(m);
        return (int64_t)_r.col2.y;
}

int64_t joy_math_mat22f_invert_col1_x(int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        mat22f_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22f_t _r;
        _r = mat22f_invert(m);
        return (int64_t)_r.col1.x;
}

int64_t joy_math_mat22f_invert_col1_y(int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        mat22f_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22f_t _r;
        _r = mat22f_invert(m);
        return (int64_t)_r.col1.y;
}

int64_t joy_math_mat22f_invert_col2_x(int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        mat22f_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22f_t _r;
        _r = mat22f_invert(m);
        return (int64_t)_r.col2.x;
}

int64_t joy_math_mat22f_invert_col2_y(int64_t m_col1_x, int64_t m_col1_y, int64_t m_col2_x, int64_t m_col2_y)
{
        mat22f_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22f_t _r;
        _r = mat22f_invert(m);
        return (int64_t)_r.col2.y;
}

int64_t joy_math_mat22f_add_col1_x(int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        mat22f_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22f_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22f_t _r;
        _r = mat22f_add(a, b);
        return (int64_t)_r.col1.x;
}

int64_t joy_math_mat22f_add_col1_y(int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        mat22f_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22f_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22f_t _r;
        _r = mat22f_add(a, b);
        return (int64_t)_r.col1.y;
}

int64_t joy_math_mat22f_add_col2_x(int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        mat22f_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22f_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22f_t _r;
        _r = mat22f_add(a, b);
        return (int64_t)_r.col2.x;
}

int64_t joy_math_mat22f_add_col2_y(int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        mat22f_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22f_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22f_t _r;
        _r = mat22f_add(a, b);
        return (int64_t)_r.col2.y;
}

int64_t joy_math_mat22f_mul_col1_x(int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        mat22f_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22f_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22f_t _r;
        _r = mat22f_mul(a, b);
        return (int64_t)_r.col1.x;
}

int64_t joy_math_mat22f_mul_col1_y(int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        mat22f_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22f_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22f_t _r;
        _r = mat22f_mul(a, b);
        return (int64_t)_r.col1.y;
}

int64_t joy_math_mat22f_mul_col2_x(int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        mat22f_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22f_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22f_t _r;
        _r = mat22f_mul(a, b);
        return (int64_t)_r.col2.x;
}

int64_t joy_math_mat22f_mul_col2_y(int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y, int64_t b_col1_x, int64_t b_col1_y, int64_t b_col2_x, int64_t b_col2_y)
{
        mat22f_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22f_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22f_t _r;
        _r = mat22f_mul(a, b);
        return (int64_t)_r.col2.y;
}

int64_t joy_math_mat22f_abs_col1_x(int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y)
{
        mat22f_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22f_t _r;
        _r = mat22f_abs(a);
        return (int64_t)_r.col1.x;
}

int64_t joy_math_mat22f_abs_col1_y(int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y)
{
        mat22f_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22f_t _r;
        _r = mat22f_abs(a);
        return (int64_t)_r.col1.y;
}

int64_t joy_math_mat22f_abs_col2_x(int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y)
{
        mat22f_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22f_t _r;
        _r = mat22f_abs(a);
        return (int64_t)_r.col2.x;
}

int64_t joy_math_mat22f_abs_col2_y(int64_t a_col1_x, int64_t a_col1_y, int64_t a_col2_x, int64_t a_col2_y)
{
        mat22f_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22f_t _r;
        _r = mat22f_abs(a);
        return (int64_t)_r.col2.y;
}

float joy_math_ft_from_float(float x)
{
        return (float)ft_from_float(x);
}

float joy_math_ft_to_float(float x)
{
        return (float)ft_to_float(x);
}

float joy_math_ft_min_value(void)
{
        return (float)ft_min_value();
}

float joy_math_ft_max_value(void)
{
        return (float)ft_max_value();
}

float joy_math_ft_add(float a, float b)
{
        return (float)ft_add(a, b);
}

float joy_math_ft_sub(float a, float b)
{
        return (float)ft_sub(a, b);
}

float joy_math_ft_mul(float a, float b)
{
        return (float)ft_mul(a, b);
}

float joy_math_ft_div(float a, float b)
{
        return (float)ft_div(a, b);
}

float joy_math_ft_abs(float x)
{
        return (float)ft_abs(x);
}

float joy_math_ft_sqrt(float x)
{
        return (float)ft_sqrt(x);
}

float joy_math_ft_sin(float x)
{
        return (float)ft_sin(x);
}

float joy_math_ft_cos(float x)
{
        return (float)ft_cos(x);
}

float joy_math_ft_atan2(float y, float x)
{
        return (float)ft_atan2(y, x);
}

float joy_math_ft_zero(void)
{
        return (float)ft_zero();
}

float joy_math_ft_half(void)
{
        return (float)ft_half();
}

float joy_math_ft_one(void)
{
        return (float)ft_one();
}

float joy_math_ft_pi(void)
{
        return (float)ft_pi();
}

float joy_math_ft_max(float a, float b)
{
        return (float)ft_max(a, b);
}

float joy_math_ft_min(float a, float b)
{
        return (float)ft_min(a, b);
}

float joy_math_ft_pow2(float x)
{
        return (float)ft_pow2(x);
}

int32_t joy_math_ft_sign(float x)
{
        return (int32_t)ft_sign(x);
}

float joy_math_ft_clamp(float a, float low, float high)
{
        return (float)ft_clamp(a, low, high);
}

float joy_math_ft_lerp(float a, float b, float t)
{
        return (float)ft_lerp(a, b, t);
}

float joy_math_vec2_negate_x(float v_x, float v_y)
{
        vec2_t v;
        v.x = v_x;
        v.y = v_y;
        vec2_t _r;
        _r = vec2_negate(v);
        return (float)_r.x;
}

float joy_math_vec2_negate_y(float v_x, float v_y)
{
        vec2_t v;
        v.x = v_x;
        v.y = v_y;
        vec2_t _r;
        _r = vec2_negate(v);
        return (float)_r.y;
}

uint64_t joy_math_vec2_negate(float v_x, float v_y)
{
        vec2_t v;
        v.x = v_x;
        v.y = v_y;
        vec2_t _r;
        float _v0, _v1;
        uint32_t _b0, _b1;
        _r = vec2_negate(v);
        _v0 = (float)_r.x;
        _v1 = (float)_r.y;
        memcpy(&_b0, &_v0, sizeof(_b0));
        memcpy(&_b1, &_v1, sizeof(_b1));
        return (uint64_t)_b0 | ((uint64_t)_b1 << 32);
}

float joy_math_vec2_dot(float a_x, float a_y, float b_x, float b_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        return (float)vec2_dot(a, b);
}

float joy_math_vec2_cross(float a_x, float a_y, float b_x, float b_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        return (float)vec2_cross(a, b);
}

float joy_math_vec2_cross2_x(float a_x, float a_y, float s)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        _r = vec2_cross2(a, s);
        return (float)_r.x;
}

float joy_math_vec2_cross2_y(float a_x, float a_y, float s)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        _r = vec2_cross2(a, s);
        return (float)_r.y;
}

uint64_t joy_math_vec2_cross2(float a_x, float a_y, float s)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        float _v0, _v1;
        uint32_t _b0, _b1;
        _r = vec2_cross2(a, s);
        _v0 = (float)_r.x;
        _v1 = (float)_r.y;
        memcpy(&_b0, &_v0, sizeof(_b0));
        memcpy(&_b1, &_v1, sizeof(_b1));
        return (uint64_t)_b0 | ((uint64_t)_b1 << 32);
}

float joy_math_vec2_cross3_x(float s, float a_x, float a_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        _r = vec2_cross3(s, a);
        return (float)_r.x;
}

float joy_math_vec2_cross3_y(float s, float a_x, float a_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        _r = vec2_cross3(s, a);
        return (float)_r.y;
}

uint64_t joy_math_vec2_cross3(float s, float a_x, float a_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        float _v0, _v1;
        uint32_t _b0, _b1;
        _r = vec2_cross3(s, a);
        _v0 = (float)_r.x;
        _v1 = (float)_r.y;
        memcpy(&_b0, &_v0, sizeof(_b0));
        memcpy(&_b1, &_v1, sizeof(_b1));
        return (uint64_t)_b0 | ((uint64_t)_b1 << 32);
}

float joy_math_vec2_add_x(float a_x, float a_y, float b_x, float b_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        vec2_t _r;
        _r = vec2_add(a, b);
        return (float)_r.x;
}

float joy_math_vec2_add_y(float a_x, float a_y, float b_x, float b_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        vec2_t _r;
        _r = vec2_add(a, b);
        return (float)_r.y;
}

uint64_t joy_math_vec2_add(float a_x, float a_y, float b_x, float b_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        vec2_t _r;
        float _v0, _v1;
        uint32_t _b0, _b1;
        _r = vec2_add(a, b);
        _v0 = (float)_r.x;
        _v1 = (float)_r.y;
        memcpy(&_b0, &_v0, sizeof(_b0));
        memcpy(&_b1, &_v1, sizeof(_b1));
        return (uint64_t)_b0 | ((uint64_t)_b1 << 32);
}

float joy_math_vec2_sub_x(float a_x, float a_y, float b_x, float b_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        vec2_t _r;
        _r = vec2_sub(a, b);
        return (float)_r.x;
}

float joy_math_vec2_sub_y(float a_x, float a_y, float b_x, float b_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        vec2_t _r;
        _r = vec2_sub(a, b);
        return (float)_r.y;
}

uint64_t joy_math_vec2_sub(float a_x, float a_y, float b_x, float b_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        vec2_t _r;
        float _v0, _v1;
        uint32_t _b0, _b1;
        _r = vec2_sub(a, b);
        _v0 = (float)_r.x;
        _v1 = (float)_r.y;
        memcpy(&_b0, &_v0, sizeof(_b0));
        memcpy(&_b1, &_v1, sizeof(_b1));
        return (uint64_t)_b0 | ((uint64_t)_b1 << 32);
}

float joy_math_vec2_scale_x(float a_x, float a_y, float s)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        _r = vec2_scale(a, s);
        return (float)_r.x;
}

float joy_math_vec2_scale_y(float a_x, float a_y, float s)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        _r = vec2_scale(a, s);
        return (float)_r.y;
}

uint64_t joy_math_vec2_scale(float a_x, float a_y, float s)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        float _v0, _v1;
        uint32_t _b0, _b1;
        _r = vec2_scale(a, s);
        _v0 = (float)_r.x;
        _v1 = (float)_r.y;
        memcpy(&_b0, &_v0, sizeof(_b0));
        memcpy(&_b1, &_v1, sizeof(_b1));
        return (uint64_t)_b0 | ((uint64_t)_b1 << 32);
}

float joy_math_vec2_abs_x(float a_x, float a_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        _r = vec2_abs(a);
        return (float)_r.x;
}

float joy_math_vec2_abs_y(float a_x, float a_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        _r = vec2_abs(a);
        return (float)_r.y;
}

uint64_t joy_math_vec2_abs(float a_x, float a_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        float _v0, _v1;
        uint32_t _b0, _b1;
        _r = vec2_abs(a);
        _v0 = (float)_r.x;
        _v1 = (float)_r.y;
        memcpy(&_b0, &_v0, sizeof(_b0));
        memcpy(&_b1, &_v1, sizeof(_b1));
        return (uint64_t)_b0 | ((uint64_t)_b1 << 32);
}

float joy_math_vec2_length_squared(float a_x, float a_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        return (float)vec2_length_squared(a);
}

float joy_math_vec2_length(float a_x, float a_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        return (float)vec2_length(a);
}

float joy_math_vec2_normalize_x(float a_x, float a_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        _r = vec2_normalize(a);
        return (float)_r.x;
}

float joy_math_vec2_normalize_y(float a_x, float a_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        _r = vec2_normalize(a);
        return (float)_r.y;
}

uint64_t joy_math_vec2_normalize(float a_x, float a_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        float _v0, _v1;
        uint32_t _b0, _b1;
        _r = vec2_normalize(a);
        _v0 = (float)_r.x;
        _v1 = (float)_r.y;
        memcpy(&_b0, &_v0, sizeof(_b0));
        memcpy(&_b1, &_v1, sizeof(_b1));
        return (uint64_t)_b0 | ((uint64_t)_b1 << 32);
}

float joy_math_vec2_normal_x(float a_x, float a_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        _r = vec2_normal(a);
        return (float)_r.x;
}

float joy_math_vec2_normal_y(float a_x, float a_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        _r = vec2_normal(a);
        return (float)_r.y;
}

uint64_t joy_math_vec2_normal(float a_x, float a_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        float _v0, _v1;
        uint32_t _b0, _b1;
        _r = vec2_normal(a);
        _v0 = (float)_r.x;
        _v1 = (float)_r.y;
        memcpy(&_b0, &_v0, sizeof(_b0));
        memcpy(&_b1, &_v1, sizeof(_b1));
        return (uint64_t)_b0 | ((uint64_t)_b1 << 32);
}

float joy_math_vec2_rotate_x(float a_x, float a_y, float angle)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        _r = vec2_rotate(a, angle);
        return (float)_r.x;
}

float joy_math_vec2_rotate_y(float a_x, float a_y, float angle)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        _r = vec2_rotate(a, angle);
        return (float)_r.y;
}

uint64_t joy_math_vec2_rotate(float a_x, float a_y, float angle)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t _r;
        float _v0, _v1;
        uint32_t _b0, _b1;
        _r = vec2_rotate(a, angle);
        _v0 = (float)_r.x;
        _v1 = (float)_r.y;
        memcpy(&_b0, &_v0, sizeof(_b0));
        memcpy(&_b1, &_v1, sizeof(_b1));
        return (uint64_t)_b0 | ((uint64_t)_b1 << 32);
}

float joy_math_vec2_lerp_x(float a_x, float a_y, float b_x, float b_y, float t)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        vec2_t _r;
        _r = vec2_lerp(a, b, t);
        return (float)_r.x;
}

float joy_math_vec2_lerp_y(float a_x, float a_y, float b_x, float b_y, float t)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        vec2_t _r;
        _r = vec2_lerp(a, b, t);
        return (float)_r.y;
}

uint64_t joy_math_vec2_lerp(float a_x, float a_y, float b_x, float b_y, float t)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        vec2_t _r;
        float _v0, _v1;
        uint32_t _b0, _b1;
        _r = vec2_lerp(a, b, t);
        _v0 = (float)_r.x;
        _v1 = (float)_r.y;
        memcpy(&_b0, &_v0, sizeof(_b0));
        memcpy(&_b1, &_v1, sizeof(_b1));
        return (uint64_t)_b0 | ((uint64_t)_b1 << 32);
}

float joy_math_vec2_distance(float a_x, float a_y, float b_x, float b_y)
{
        vec2_t a;
        a.x = a_x;
        a.y = a_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        return (float)vec2_distance(a, b);
}

float joy_math_vec3_negate_x(float v_x, float v_y, float v_z)
{
        vec3_t v;
        v.x = v_x;
        v.y = v_y;
        v.z = v_z;
        vec3_t _r;
        _r = vec3_negate(v);
        return (float)_r.x;
}

float joy_math_vec3_negate_y(float v_x, float v_y, float v_z)
{
        vec3_t v;
        v.x = v_x;
        v.y = v_y;
        v.z = v_z;
        vec3_t _r;
        _r = vec3_negate(v);
        return (float)_r.y;
}

float joy_math_vec3_negate_z(float v_x, float v_y, float v_z)
{
        vec3_t v;
        v.x = v_x;
        v.y = v_y;
        v.z = v_z;
        vec3_t _r;
        _r = vec3_negate(v);
        return (float)_r.z;
}

float joy_math_vec3_add_x(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3_t _r;
        _r = vec3_add(a, b);
        return (float)_r.x;
}

float joy_math_vec3_add_y(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3_t _r;
        _r = vec3_add(a, b);
        return (float)_r.y;
}

float joy_math_vec3_add_z(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3_t _r;
        _r = vec3_add(a, b);
        return (float)_r.z;
}

float joy_math_vec3_sub_x(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3_t _r;
        _r = vec3_sub(a, b);
        return (float)_r.x;
}

float joy_math_vec3_sub_y(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3_t _r;
        _r = vec3_sub(a, b);
        return (float)_r.y;
}

float joy_math_vec3_sub_z(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3_t _r;
        _r = vec3_sub(a, b);
        return (float)_r.z;
}

float joy_math_vec3_scale_x(float a_x, float a_y, float a_z, float s)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t _r;
        _r = vec3_scale(a, s);
        return (float)_r.x;
}

float joy_math_vec3_scale_y(float a_x, float a_y, float a_z, float s)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t _r;
        _r = vec3_scale(a, s);
        return (float)_r.y;
}

float joy_math_vec3_scale_z(float a_x, float a_y, float a_z, float s)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t _r;
        _r = vec3_scale(a, s);
        return (float)_r.z;
}

float joy_math_vec3_abs_x(float a_x, float a_y, float a_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t _r;
        _r = vec3_abs(a);
        return (float)_r.x;
}

float joy_math_vec3_abs_y(float a_x, float a_y, float a_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t _r;
        _r = vec3_abs(a);
        return (float)_r.y;
}

float joy_math_vec3_abs_z(float a_x, float a_y, float a_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t _r;
        _r = vec3_abs(a);
        return (float)_r.z;
}

float joy_math_vec3_dot(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        return (float)vec3_dot(a, b);
}

float joy_math_vec3_cross_x(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3_t _r;
        _r = vec3_cross(a, b);
        return (float)_r.x;
}

float joy_math_vec3_cross_y(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3_t _r;
        _r = vec3_cross(a, b);
        return (float)_r.y;
}

float joy_math_vec3_cross_z(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3_t _r;
        _r = vec3_cross(a, b);
        return (float)_r.z;
}

float joy_math_vec3_square_length(float a_x, float a_y, float a_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        return (float)vec3_square_length(a);
}

float joy_math_vec3_length(float a_x, float a_y, float a_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        return (float)vec3_length(a);
}

float joy_math_vec3_normalize_x(float a_x, float a_y, float a_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t _r;
        _r = vec3_normalize(a);
        return (float)_r.x;
}

float joy_math_vec3_normalize_y(float a_x, float a_y, float a_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t _r;
        _r = vec3_normalize(a);
        return (float)_r.y;
}

float joy_math_vec3_normalize_z(float a_x, float a_y, float a_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t _r;
        _r = vec3_normalize(a);
        return (float)_r.z;
}

float joy_math_vec3_lerp_x(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z, float t)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3_t _r;
        _r = vec3_lerp(a, b, t);
        return (float)_r.x;
}

float joy_math_vec3_lerp_y(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z, float t)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3_t _r;
        _r = vec3_lerp(a, b, t);
        return (float)_r.y;
}

float joy_math_vec3_lerp_z(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z, float t)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        vec3_t _r;
        _r = vec3_lerp(a, b, t);
        return (float)_r.z;
}

float joy_math_vec3_distance(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t b;
        b.x = b_x;
        b.y = b_y;
        b.z = b_z;
        return (float)vec3_distance(a, b);
}

float joy_math_mat22_mul_vec2_x(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_x, float b_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        vec2_t _r;
        _r = mat22_mul_vec2(a, b);
        return (float)_r.x;
}

float joy_math_mat22_mul_vec2_y(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_x, float b_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        vec2_t _r;
        _r = mat22_mul_vec2(a, b);
        return (float)_r.y;
}

uint64_t joy_math_mat22_mul_vec2(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_x, float b_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        vec2_t b;
        b.x = b_x;
        b.y = b_y;
        vec2_t _r;
        float _v0, _v1;
        uint32_t _b0, _b1;
        _r = mat22_mul_vec2(a, b);
        _v0 = (float)_r.x;
        _v1 = (float)_r.y;
        memcpy(&_b0, &_v0, sizeof(_b0));
        memcpy(&_b1, &_v1, sizeof(_b1));
        return (uint64_t)_b0 | ((uint64_t)_b1 << 32);
}

float joy_math_mat22_rotate_col1_x(float angle)
{
        mat22_t _r;
        _r = mat22_rotate(angle);
        return (float)_r.col1.x;
}

float joy_math_mat22_rotate_col1_y(float angle)
{
        mat22_t _r;
        _r = mat22_rotate(angle);
        return (float)_r.col1.y;
}

float joy_math_mat22_rotate_col2_x(float angle)
{
        mat22_t _r;
        _r = mat22_rotate(angle);
        return (float)_r.col2.x;
}

float joy_math_mat22_rotate_col2_y(float angle)
{
        mat22_t _r;
        _r = mat22_rotate(angle);
        return (float)_r.col2.y;
}

float joy_math_mat22_transpose_col1_x(float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        mat22_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22_t _r;
        _r = mat22_transpose(m);
        return (float)_r.col1.x;
}

float joy_math_mat22_transpose_col1_y(float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        mat22_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22_t _r;
        _r = mat22_transpose(m);
        return (float)_r.col1.y;
}

float joy_math_mat22_transpose_col2_x(float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        mat22_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22_t _r;
        _r = mat22_transpose(m);
        return (float)_r.col2.x;
}

float joy_math_mat22_transpose_col2_y(float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        mat22_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22_t _r;
        _r = mat22_transpose(m);
        return (float)_r.col2.y;
}

float joy_math_mat22_invert_col1_x(float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        mat22_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22_t _r;
        _r = mat22_invert(m);
        return (float)_r.col1.x;
}

float joy_math_mat22_invert_col1_y(float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        mat22_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22_t _r;
        _r = mat22_invert(m);
        return (float)_r.col1.y;
}

float joy_math_mat22_invert_col2_x(float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        mat22_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22_t _r;
        _r = mat22_invert(m);
        return (float)_r.col2.x;
}

float joy_math_mat22_invert_col2_y(float m_col1_x, float m_col1_y, float m_col2_x, float m_col2_y)
{
        mat22_t m;
        m.col1.x = m_col1_x;
        m.col1.y = m_col1_y;
        m.col2.x = m_col2_x;
        m.col2.y = m_col2_y;
        mat22_t _r;
        _r = mat22_invert(m);
        return (float)_r.col2.y;
}

float joy_math_mat22_add_col1_x(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22_t _r;
        _r = mat22_add(a, b);
        return (float)_r.col1.x;
}

float joy_math_mat22_add_col1_y(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22_t _r;
        _r = mat22_add(a, b);
        return (float)_r.col1.y;
}

float joy_math_mat22_add_col2_x(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22_t _r;
        _r = mat22_add(a, b);
        return (float)_r.col2.x;
}

float joy_math_mat22_add_col2_y(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22_t _r;
        _r = mat22_add(a, b);
        return (float)_r.col2.y;
}

float joy_math_mat22_mul_col1_x(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22_t _r;
        _r = mat22_mul(a, b);
        return (float)_r.col1.x;
}

float joy_math_mat22_mul_col1_y(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22_t _r;
        _r = mat22_mul(a, b);
        return (float)_r.col1.y;
}

float joy_math_mat22_mul_col2_x(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22_t _r;
        _r = mat22_mul(a, b);
        return (float)_r.col2.x;
}

float joy_math_mat22_mul_col2_y(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y, float b_col1_x, float b_col1_y, float b_col2_x, float b_col2_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22_t b;
        b.col1.x = b_col1_x;
        b.col1.y = b_col1_y;
        b.col2.x = b_col2_x;
        b.col2.y = b_col2_y;
        mat22_t _r;
        _r = mat22_mul(a, b);
        return (float)_r.col2.y;
}

float joy_math_mat22_abs_col1_x(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22_t _r;
        _r = mat22_abs(a);
        return (float)_r.col1.x;
}

float joy_math_mat22_abs_col1_y(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22_t _r;
        _r = mat22_abs(a);
        return (float)_r.col1.y;
}

float joy_math_mat22_abs_col2_x(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22_t _r;
        _r = mat22_abs(a);
        return (float)_r.col2.x;
}

float joy_math_mat22_abs_col2_y(float a_col1_x, float a_col1_y, float a_col2_x, float a_col2_y)
{
        mat22_t a;
        a.col1.x = a_col1_x;
        a.col1.y = a_col1_y;
        a.col2.x = a_col2_x;
        a.col2.y = a_col2_y;
        mat22_t _r;
        _r = mat22_abs(a);
        return (float)_r.col2.y;
}

float joy_math_vec3_div_x(float a_x, float a_y, float a_z, float s)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t _r;
        _r = vec3_div(a, s);
        return (float)_r.x;
}

float joy_math_vec3_div_y(float a_x, float a_y, float a_z, float s)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t _r;
        _r = vec3_div(a, s);
        return (float)_r.y;
}

float joy_math_vec3_div_z(float a_x, float a_y, float a_z, float s)
{
        vec3_t a;
        a.x = a_x;
        a.y = a_y;
        a.z = a_z;
        vec3_t _r;
        _r = vec3_div(a, s);
        return (float)_r.z;
}

float joy_math_ft_radians(float degrees)
{
        return (float)ft_radians(degrees);
}
