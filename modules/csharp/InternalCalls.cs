// 自动生成，勿手改：gen_csclib.js
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Joy
{
    // C: typedef struct { ... } jrect_t;
    [StructLayout(LayoutKind.Sequential)]
    public struct jrect_t
    {
        public float x;
        public float y;
        public float w;
        public float h;
    }

    // C: typedef struct { ... } vec2_t;
    [StructLayout(LayoutKind.Sequential)]
    public struct vec2_t
    {
        public float x;
        public float y;
    }

    public static class Font
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern IntPtr font_create(string filename, int fontsize);

    }

    public static class Image
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern IntPtr image_create(IntPtr r2d, string filename);

    }

}
