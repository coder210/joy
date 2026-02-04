#include "liv-mono.h"
#include "liv-math.h"
#include "csclib.h"


void csopen_math()
{
        livS_mono_add_internal_call("Livlib.Native.FP::FromFloat", fp_from_float);
        livS_mono_add_internal_call("Livlib.Native.FP::ToFloat", fp_to_float);
        livS_mono_add_internal_call("Livlib.Native.FP::MinValue", fp_min_value);
        livS_mono_add_internal_call("Livlib.Native.FP::MaxValue", fp_max_value);
        livS_mono_add_internal_call("Livlib.Native.FP::Add", fp_add);
        livS_mono_add_internal_call("Livlib.Native.FP::Sub", fp_sub);
        livS_mono_add_internal_call("Livlib.Native.FP::Mul", fp_mul);
        livS_mono_add_internal_call("Livlib.Native.FP::Div", fp_div);
        livS_mono_add_internal_call("Livlib.Native.FP::Abs", fp_abs);
        livS_mono_add_internal_call("Livlib.Native.FP::Sqrt", fp_sqrt);
        livS_mono_add_internal_call("Livlib.Native.FP::Sin", fp_sin);
        livS_mono_add_internal_call("Livlib.Native.FP::Cos", fp_cos);
        livS_mono_add_internal_call("Livlib.Native.FP::Zero", fp_zero);
        livS_mono_add_internal_call("Livlib.Native.FP::Half", fp_half);
        livS_mono_add_internal_call("Livlib.Native.FP::One", fp_one);
        livS_mono_add_internal_call("Livlib.Native.FP::Pi", fp_pi);
        livS_mono_add_internal_call("Livlib.Native.FP::Max", fp_max);
        livS_mono_add_internal_call("Livlib.Native.FP::Min", fp_min);
        livS_mono_add_internal_call("Livlib.Native.FP::Pow2", fp_pow2);
        livS_mono_add_internal_call("Livlib.Native.FP::Sign", fp_sign);
        livS_mono_add_internal_call("Livlib.Native.FP::Clamp", fp_clamp);
        livS_mono_add_internal_call("Livlib.Native.FP::Lerp", fp_lerp);

        livS_mono_add_internal_call("Livlib.Native.FT::FromFloat", ft_from_float);
        livS_mono_add_internal_call("Livlib.Native.FT::MinValue", ft_min_value);
        livS_mono_add_internal_call("Livlib.Native.FT::MaxValue", ft_max_value);
        livS_mono_add_internal_call("Livlib.Native.FT::Add", ft_add);
        livS_mono_add_internal_call("Livlib.Native.FT::Sub", ft_sub);
        livS_mono_add_internal_call("Livlib.Native.FT::Mul", ft_mul);
        livS_mono_add_internal_call("Livlib.Native.FT::Div", ft_div);
        livS_mono_add_internal_call("Livlib.Native.FT::Abs", ft_abs);
        livS_mono_add_internal_call("Livlib.Native.FT::Sqrt", ft_sqrt);
        livS_mono_add_internal_call("Livlib.Native.FT::Sin", ft_sin);
        livS_mono_add_internal_call("Livlib.Native.FT::Cos", ft_cos);
        livS_mono_add_internal_call("Livlib.Native.FT::Zero", ft_zero);
        livS_mono_add_internal_call("Livlib.Native.FT::Half", ft_half);
        livS_mono_add_internal_call("Livlib.Native.FT::One", ft_one);
        livS_mono_add_internal_call("Livlib.Native.FT::PI", ft_pi);
        livS_mono_add_internal_call("Livlib.Native.FT::Max", ft_max);
        livS_mono_add_internal_call("Livlib.Native.FT::Min", ft_min);
        livS_mono_add_internal_call("Livlib.Native.FT::Pow2", ft_pow2);
        livS_mono_add_internal_call("Livlib.Native.FT::Sign", ft_sign);
        livS_mono_add_internal_call("Livlib.Native.FT::Clamp", ft_clamp);
        livS_mono_add_internal_call("Livlib.Native.FT::Lerp", ft_lerp);


        livS_mono_add_internal_call("Livlib.Native.Vec2::Negate", vec2_negate);
        livS_mono_add_internal_call("Livlib.Native.Vec2::Dot", vec2_dot);
        livS_mono_add_internal_call("Livlib.Native.Vec2::Cross", vec2_cross);
        livS_mono_add_internal_call("Livlib.Native.Vec2::Cross2", vec2_cross2);
        livS_mono_add_internal_call("Livlib.Native.Vec2::Cross3", vec2_cross3);
        livS_mono_add_internal_call("Livlib.Native.Vec2::Add", vec2_add);
        livS_mono_add_internal_call("Livlib.Native.Vec2::Sub", vec2_sub);
        livS_mono_add_internal_call("Livlib.Native.Vec2::Scale", vec2_scale);
        livS_mono_add_internal_call("Livlib.Native.Vec2::Abs", vec2_abs);
        livS_mono_add_internal_call("Livlib.Native.Vec2::LengthSquared", vec2_length_squared);
        livS_mono_add_internal_call("Livlib.Native.Vec2::Length", vec2_length);
        livS_mono_add_internal_call("Livlib.Native.Vec2::Normalize", vec2_normalize);
        livS_mono_add_internal_call("Livlib.Native.Vec2::Normal", vec2_normal);
        livS_mono_add_internal_call("Livlib.Native.Vec2::Rotate", vec2_rotate);
        livS_mono_add_internal_call("Livlib.Native.Vec2::Lerp", vec2_lerp);
        livS_mono_add_internal_call("Livlib.Native.Vec2::Distance", vec2_distance);

        livS_mono_add_internal_call("Livlib.Native.Vec2F::Negate", vec2f_negate);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::Dot", vec2f_dot);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::Cross", vec2f_cross);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::Cross2", vec2f_cross2);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::Cross3", vec2f_cross3);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::Add", vec2f_add);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::Sub", vec2f_sub);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::Scale", vec2f_scale);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::Abs", vec2f_abs);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::LengthSquared", vec2f_length_squared);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::Length", vec2f_length);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::Normalize", vec2f_normalize);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::Normal", vec2f_normal);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::Rotate", vec2f_rotate);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::Lerp", vec2f_lerp);
        livS_mono_add_internal_call("Livlib.Native.Vec2F::Distance", vec2f_distance);

        livS_mono_add_internal_call("Livlib.Native.Vec3F::Negate", vec3f_negate);
        livS_mono_add_internal_call("Livlib.Native.Vec3F::Dot", vec3f_dot);
        livS_mono_add_internal_call("Livlib.Native.Vec3F::Cross", vec3f_cross);
        livS_mono_add_internal_call("Livlib.Native.Vec3F::Add", vec3f_add);
        livS_mono_add_internal_call("Livlib.Native.Vec3F::Sub", vec3f_sub);
        livS_mono_add_internal_call("Livlib.Native.Vec3F::Scale", vec3f_scale);
        livS_mono_add_internal_call("Livlib.Native.Vec3F::Abs", vec3f_abs);
        livS_mono_add_internal_call("Livlib.Native.Vec3F::LengthSquared", vec3f_square_length);
        livS_mono_add_internal_call("Livlib.Native.Vec3F::Length", vec3f_length);
        livS_mono_add_internal_call("Livlib.Native.Vec3F::Normalize", vec3f_normalize);
        livS_mono_add_internal_call("Livlib.Native.Vec3F::Lerp", vec3f_lerp);
        livS_mono_add_internal_call("Livlib.Native.Vec3F::Distance", vec3f_distance);

        livS_mono_add_internal_call("Livlib.Native.Mat22::Abs", mat22_abs);
        livS_mono_add_internal_call("Livlib.Native.Mat22::Add", mat22_add);
        livS_mono_add_internal_call("Livlib.Native.Mat22::Invert", mat22_invert);
        livS_mono_add_internal_call("Livlib.Native.Mat22::Mul", mat22_mul);
        livS_mono_add_internal_call("Livlib.Native.Mat22::MulVec2", mat22_mul_vec2);
        livS_mono_add_internal_call("Livlib.Native.Mat22::Rotate", mat22_rotate);
        livS_mono_add_internal_call("Livlib.Native.Mat22::Transpose", mat22_transpose);

        livS_mono_add_internal_call("Livlib.Native.Mat22::FAbs", mat22f_abs);
        livS_mono_add_internal_call("Livlib.Native.Mat22::FAdd", mat22f_add);
        livS_mono_add_internal_call("Livlib.Native.Mat22::FInvert", mat22f_invert);
        livS_mono_add_internal_call("Livlib.Native.Mat22::FMul", mat22f_mul);
        livS_mono_add_internal_call("Livlib.Native.Mat22::FMulVec2f", mat22f_mul_vec2f);
        livS_mono_add_internal_call("Livlib.Native.Mat22::FRotate", mat22f_rotate);
        livS_mono_add_internal_call("Livlib.Native.Mat22::FTranspose", mat22f_transpose);
}
