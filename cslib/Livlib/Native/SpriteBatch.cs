using System;
using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    public struct Texture2d
    {
        public IntPtr Texture { get; set; }
        public string Id { get; set; }
    }

    public struct SpriteBatch
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr Create(int sz);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Destroy(IntPtr inst);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Texture2d LoadBmp(IntPtr renderer, string filename);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Texture2d LoadPng(IntPtr renderer, string filename);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Draw(IntPtr inst, Texture2d texture, SDL.FRect srcrect, SDL.FRect detrect, SDL.FColor color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Render(IntPtr inst, bool clear = true);
    }
}
