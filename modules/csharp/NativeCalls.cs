using System;
using System.Runtime.CompilerServices;

// 手写的 C# → C internal call 声明；对应的 C 实现位于
// modules/csharp/bindings/csclib_{draw,input,host}.c。
// 这些 API 用标量参数，避免跨 ABI 传递结构体；结构体与自动生成的声明见
// 同目录的 InternalCalls.cs（两者共同编译进 Joy.Bindings.dll）。
//
// 类必须是 public：业务程序集通过 ProjectReference 引用本程序集，
// internal 声明跨程序集不可见。
namespace Joy
{
    public static class Draw
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void draw_image_centered(IntPtr image,
            float sx, float sy, float sw, float sh,
            float cx, float cy, float dw, float dh);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float image_width(IntPtr image);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float image_height(IntPtr image);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void fill_rect(IntPtr r2d, float x, float y, float w, float h,
            int r, int g, int b, int a);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void stroke_rect(IntPtr r2d, float x, float y, float w, float h,
            int r, int g, int b, int a);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void draw_line(IntPtr r2d, float x1, float y1, float x2, float y2,
            int r, int g, int b, int a);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void draw_text(IntPtr r2d, IntPtr font, string text,
            int r, int g, int b, int a, float x, float y);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void draw_text_centered(IntPtr r2d, IntPtr font, string text,
            int r, int g, int b, int a, float cx, float cy);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int text_width(IntPtr r2d, IntPtr font, string text);

        /* 结构体按值传递的 ABI 自检（参数来自自动生成的 vec2_t / jrect_t） */
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float abi_vec2_sum(vec2_t v);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float abi_rect_area(jrect_t r);
    }

    public static class Input
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool key_down(int scancode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool mouse_button(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float mouse_x();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float mouse_y();
    }

    public static class Host
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void log(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void quit();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void set_exit_code(int code);
    }
}
