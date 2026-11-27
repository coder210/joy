using System;
using System.Runtime.InteropServices;

namespace Joy.Api
{
    /// <summary>
    /// r3d L3/L2 手写适配第二批。CoreCLR 函数指针表绑定。
    /// 这些 API 在 joy.idl 里多为 status:off（void* 缓冲、按值结构体、返回 char*、
    /// 函数指针回调），生成器不出 thunk，故在 C 侧 csclib_r3d_plus.c 手写。
    /// </summary>
    public static unsafe class R3dExtPlus
    {
        // ---- native 函数指针（按注册名查表）----
        private static delegate* unmanaged[Cdecl]<ulong, int, int, byte*, byte, IntPtr> s_create_texture;
        private static delegate* unmanaged[Cdecl]<ulong, byte*, int*, int*, IntPtr> s_load_texture_png;
        private static delegate* unmanaged[Cdecl]<ulong, int*, int*, void> s_get_logical_presentation;
        private static delegate* unmanaged[Cdecl]<ulong, float, float, float*, float*, void> s_window_to_logical;
        private static delegate* unmanaged[Cdecl]<ulong, float, float, float*, float*, void> s_logical_to_window;
        private static delegate* unmanaged[Cdecl]<ulong, ulong, float, float, float, float, float, float, byte, byte, byte, byte, void> s_draw_mesh;
        private static delegate* unmanaged[Cdecl]<ulong, ulong, float, float, float, float, float, float, IntPtr, void> s_draw_mesh_tex;
        private static delegate* unmanaged[Cdecl]<ulong, ulong, float*, IntPtr, void> s_draw_mesh_lit_tex;
        private static delegate* unmanaged[Cdecl]<ulong, float*, void> s_set_light_block;
        private static delegate* unmanaged[Cdecl]<float, float, float, float, byte*, ulong> s_button_create;
        private static delegate* unmanaged[Cdecl]<float, float, float, byte, byte, byte, byte*, ulong> s_label_create;
        private static delegate* unmanaged[Cdecl]<float, float, float, float, float, byte, byte, byte, byte*, ulong> s_input_create;
        private static delegate* unmanaged[Cdecl]<float, float, float, float, float, byte, byte, byte, byte*, ulong> s_combobox_input_create;
        private static delegate* unmanaged[Cdecl]<float, float, float, float, float, float, float, ulong> s_slider_create;
        private static delegate* unmanaged[Cdecl]<float, float, float, byte*, ulong> s_checkbox_create;
        private static delegate* unmanaged[Cdecl]<ulong, IntPtr> s_input_get_text;
        private static delegate* unmanaged[Cdecl]<ulong, IntPtr> s_combobox_input_get_text;
        private static delegate* unmanaged[Cdecl]<ulong,
            byte, byte, byte, byte, byte, byte, byte, byte,
            byte, byte, byte, byte, byte, byte, byte, byte, void> s_slider_set_colors;
        private static delegate* unmanaged[Cdecl]<ulong, int, void> s_checkbox_set_state;
        private static delegate* unmanaged[Cdecl]<ulong, int> s_checkbox_get_state;

        internal static void Init()
        {
            s_create_texture = (delegate* unmanaged[Cdecl]<ulong, int, int, byte*, byte, IntPtr>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::create_texture");
            s_load_texture_png = (delegate* unmanaged[Cdecl]<ulong, byte*, int*, int*, IntPtr>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::load_texture_png");
            s_get_logical_presentation = (delegate* unmanaged[Cdecl]<ulong, int*, int*, void>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::get_logical_presentation");
            s_window_to_logical = (delegate* unmanaged[Cdecl]<ulong, float, float, float*, float*, void>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::window_to_logical");
            s_logical_to_window = (delegate* unmanaged[Cdecl]<ulong, float, float, float*, float*, void>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::logical_to_window");
            s_draw_mesh = (delegate* unmanaged[Cdecl]<ulong, ulong, float, float, float, float, float, float, byte, byte, byte, byte, void>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::draw_mesh");
            s_draw_mesh_tex = (delegate* unmanaged[Cdecl]<ulong, ulong, float, float, float, float, float, float, IntPtr, void>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::draw_mesh_tex");
            s_draw_mesh_lit_tex = (delegate* unmanaged[Cdecl]<ulong, ulong, float*, IntPtr, void>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::draw_mesh_lit_tex");
            s_set_light_block = (delegate* unmanaged[Cdecl]<ulong, float*, void>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::set_light_block");
            s_button_create = (delegate* unmanaged[Cdecl]<float, float, float, float, byte*, ulong>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::button_create");
            s_label_create = (delegate* unmanaged[Cdecl]<float, float, float, byte, byte, byte, byte*, ulong>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::label_create");
            s_input_create = (delegate* unmanaged[Cdecl]<float, float, float, float, float, byte, byte, byte, byte*, ulong>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::input_create");
            s_combobox_input_create = (delegate* unmanaged[Cdecl]<float, float, float, float, float, byte, byte, byte, byte*, ulong>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::combobox_input_create");
            s_slider_create = (delegate* unmanaged[Cdecl]<float, float, float, float, float, float, float, ulong>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::slider_create");
            s_checkbox_create = (delegate* unmanaged[Cdecl]<float, float, float, byte*, ulong>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::checkbox_create");
            s_input_get_text = (delegate* unmanaged[Cdecl]<ulong, IntPtr>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::input_get_text");
            s_combobox_input_get_text = (delegate* unmanaged[Cdecl]<ulong, IntPtr>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::combobox_input_get_text");
            s_slider_set_colors = (delegate* unmanaged[Cdecl]<ulong,
                byte, byte, byte, byte, byte, byte, byte, byte,
                byte, byte, byte, byte, byte, byte, byte, byte, void>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::slider_set_colors");
            s_checkbox_set_state = (delegate* unmanaged[Cdecl]<ulong, int, void>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::checkbox_set_state");
            s_checkbox_get_state = (delegate* unmanaged[Cdecl]<ulong, int>)NativeFuncs.Lookup("Joy.Api.R3dExtPlus::checkbox_get_state");
        }

        // ---- private 包装（string -> UTF-8, out -> 指针, 数组 -> fixed）----
        private static IntPtr create_texture(ulong r, int w, int h, byte[] rgba32, bool flip)
        {
            fixed (byte* p = rgba32) return s_create_texture(r, w, h, p, (byte)(flip ? 1 : 0));
        }

        private static IntPtr load_texture_png(ulong r, string path, out int outW, out int outH)
        {
            int w = 0, h = 0;
            byte* p = NativeFuncs.Utf8(path ?? "");
            try {
                IntPtr res = s_load_texture_png(r, p, &w, &h);
                outW = w; outH = h;
                return res;
            } finally { NativeFuncs.FreeUtf8(p); }
        }

        private static void get_logical_presentation(ulong r, out int w, out int h)
        {
            int lw = 0, lh = 0; s_get_logical_presentation(r, &lw, &lh); w = lw; h = lh;
        }

        private static void window_to_logical(ulong r, float wx, float wy, out float lx, out float ly)
        {
            float a = 0, b = 0; s_window_to_logical(r, wx, wy, &a, &b); lx = a; ly = b;
        }

        private static void logical_to_window(ulong r, float lx, float ly, out float wx, out float wy)
        {
            float a = 0, b = 0; s_logical_to_window(r, lx, ly, &a, &b); wx = a; wy = b;
        }

        private static void draw_mesh(ulong r, ulong m, float px, float py, float pz, float sx, float sy, float sz, byte cr, byte cg, byte cb, byte ca)
            => s_draw_mesh(r, m, px, py, pz, sx, sy, sz, cr, cg, cb, ca);

        private static void draw_mesh_tex(ulong r, ulong m, float px, float py, float pz, float sx, float sy, float sz, IntPtr tex)
            => s_draw_mesh_tex(r, m, px, py, pz, sx, sy, sz, tex);

        private static ulong button_create(float x, float y, float w, float h, string text)
        {
            byte* p = NativeFuncs.Utf8(text ?? ""); try { return s_button_create(x, y, w, h, p); } finally { NativeFuncs.FreeUtf8(p); }
        }
        private static ulong label_create(float x, float y, float fontSize, byte cr, byte cg, byte cb, string text)
        {
            byte* p = NativeFuncs.Utf8(text ?? ""); try { return s_label_create(x, y, fontSize, cr, cg, cb, p); } finally { NativeFuncs.FreeUtf8(p); }
        }
        private static ulong input_create(float x, float y, float w, float h, float fontSize, byte cr, byte cg, byte cb, string text)
        {
            byte* p = NativeFuncs.Utf8(text ?? ""); try { return s_input_create(x, y, w, h, fontSize, cr, cg, cb, p); } finally { NativeFuncs.FreeUtf8(p); }
        }
        private static ulong combobox_input_create(float x, float y, float w, float h, float fontSize, byte cr, byte cg, byte cb, string text)
        {
            byte* p = NativeFuncs.Utf8(text ?? ""); try { return s_combobox_input_create(x, y, w, h, fontSize, cr, cg, cb, p); } finally { NativeFuncs.FreeUtf8(p); }
        }
        private static ulong slider_create(float x, float y, float w, float h, float min, float max, float initial)
            => s_slider_create(x, y, w, h, min, max, initial);
        private static ulong checkbox_create(float x, float y, float size, string label)
        {
            byte* p = NativeFuncs.Utf8(label ?? ""); try { return s_checkbox_create(x, y, size, p); } finally { NativeFuncs.FreeUtf8(p); }
        }
        private static string input_get_text(ulong inp) => NativeFuncs.Str(s_input_get_text(inp));
        private static string combobox_input_get_text(ulong ci) => NativeFuncs.Str(s_combobox_input_get_text(ci));

        private static void slider_set_colors(ulong s,
            byte tr, byte tg, byte tb, byte ta, byte fr, byte fg, byte fb, byte fa,
            byte hr, byte hg, byte hb, byte ha, byte mr, byte mg, byte mb, byte ma)
            => s_slider_set_colors(s, tr, tg, tb, ta, fr, fg, fb, fa, hr, hg, hb, ha, mr, mg, mb, ma);

        private static void checkbox_set_state(ulong cb, int state) => s_checkbox_set_state(cb, state);
        private static int checkbox_get_state(ulong cb) => s_checkbox_get_state(cb);

        // ---- public 包装（保持原签名不变）----
        public static IntPtr CreateTexture(Jr3d r, int w, int h, byte[] rgba32, bool flip)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            if (rgba32 == null || rgba32.Length < w * h * 4) throw new ArgumentException("rgba32 too small", nameof(rgba32));
            return create_texture(r.Handle, w, h, rgba32, flip);
        }
        public static IntPtr LoadTexturePng(Jr3d r, string path, out int w, out int h)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            return load_texture_png(r.Handle, path ?? "", out w, out h);
        }
        public static void GetLogicalPresentation(Jr3d r, out int w, out int h)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            get_logical_presentation(r.Handle, out w, out h);
        }
        public static void WindowToLogical(Jr3d r, float wx, float wy, out float lx, out float ly)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            window_to_logical(r.Handle, wx, wy, out lx, out ly);
        }
        public static void LogicalToWindow(Jr3d r, float lx, float ly, out float wx, out float wy)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            logical_to_window(r.Handle, lx, ly, out wx, out wy);
        }
        public static void DrawMesh(Jr3d r, RenderMesh m, float px, float py, float pz, float sx, float sy, float sz, byte cr, byte cg, byte cb, byte ca)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            if (m == null) throw new ArgumentNullException(nameof(m));
            draw_mesh(r.Handle, m.Handle, px, py, pz, sx, sy, sz, cr, cg, cb, ca);
        }
        public static void DrawMeshTex(Jr3d r, RenderMesh m, float px, float py, float pz, float sx, float sy, float sz, IntPtr tex)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            if (m == null) throw new ArgumentNullException(nameof(m));
            draw_mesh_tex(r.Handle, m.Handle, px, py, pz, sx, sy, sz, tex);
        }

        // ---- lit 纹理绘制 + 光照覆盖 ----
        // lit mesh（pos+normal+uv）才能用；mat 为 float[16]，布局与 R3dExt.MakeTrs 一致。
        public static void DrawMeshLitTex(Jr3d r, RenderMesh m, float[] mat, IntPtr tex)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            if (m == null) throw new ArgumentNullException(nameof(m));
            if (mat == null || mat.Length < 16) throw new ArgumentException("mat needs 16 floats", nameof(mat));
            fixed (float* p = mat) s_draw_mesh_lit_tex(r.Handle, m.Handle, p, tex);
        }

        // 光照块：float[72] = 4 盏灯各 16 float (64B) + ambient[4] + viewPos[4]。
        // 每盏灯 16 float 布局: [0]=enabled, [1]=type, [4..6]=position,
        // [8..10]=target, [12..15]=color(rgba)。viewPos 由相机每帧覆盖，传入值无效。
        public static void SetLightBlock(Jr3d r, float[] block)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            if (block == null || block.Length < 72) throw new ArgumentException("block needs 72 floats", nameof(block));
            fixed (float* p = block) s_set_light_block(r.Handle, p);
        }

        public static Jr3dButton ButtonCreate(float x, float y, float w, float h, string text)
        {
            ulong hnd = button_create(x, y, w, h, text ?? "");
            return hnd == 0 ? null : new Jr3dButton(hnd);
        }
        public static Jr3dLabel LabelCreate(float x, float y, float fontSize, byte cr, byte cg, byte cb, string text)
        {
            ulong hnd = label_create(x, y, fontSize, cr, cg, cb, text ?? "");
            return hnd == 0 ? null : new Jr3dLabel(hnd);
        }
        public static Jr3dInput InputCreate(float x, float y, float w, float h, float fontSize, byte cr, byte cg, byte cb, string text)
        {
            ulong hnd = input_create(x, y, w, h, fontSize, cr, cg, cb, text ?? "");
            return hnd == 0 ? null : new Jr3dInput(hnd);
        }
        public static Jr3dComboboxInput ComboboxInputCreate(float x, float y, float w, float h, float fontSize, byte cr, byte cg, byte cb, string text)
        {
            ulong hnd = combobox_input_create(x, y, w, h, fontSize, cr, cg, cb, text ?? "");
            return hnd == 0 ? null : new Jr3dComboboxInput(hnd);
        }
        public static Jr3dSlider SliderCreate(float x, float y, float w, float h, float min, float max, float initial)
        {
            ulong hnd = slider_create(x, y, w, h, min, max, initial);
            return hnd == 0 ? null : new Jr3dSlider(hnd);
        }
        public static Jr3dCheckbox CheckboxCreate(float x, float y, float size, string label)
        {
            ulong hnd = checkbox_create(x, y, size, label ?? "");
            return hnd == 0 ? null : new Jr3dCheckbox(hnd);
        }
        public static string InputGetText(Jr3dInput inp)
        {
            if (inp == null) throw new ArgumentNullException(nameof(inp));
            return input_get_text(inp.Handle);
        }
        public static string ComboboxInputGetText(Jr3dComboboxInput ci)
        {
            if (ci == null) throw new ArgumentNullException(nameof(ci));
            return combobox_input_get_text(ci.Handle);
        }
        public static void SliderSetColors(Jr3dSlider s,
            byte tr, byte tg, byte tb, byte ta, byte fr, byte fg, byte fb, byte fa,
            byte hr, byte hg, byte hb, byte ha, byte mr, byte mg, byte mb, byte ma)
        {
            if (s == null) throw new ArgumentNullException(nameof(s));
            slider_set_colors(s.Handle, tr, tg, tb, ta, fr, fg, fb, fa, hr, hg, hb, ha, mr, mg, mb, ma);
        }
        public static void CheckboxSetState(Jr3dCheckbox cb, int state)
        {
            if (cb == null) throw new ArgumentNullException(nameof(cb));
            checkbox_set_state(cb.Handle, state);
        }
        public static int CheckboxGetState(Jr3dCheckbox cb)
        {
            if (cb == null) throw new ArgumentNullException(nameof(cb));
            return checkbox_get_state(cb.Handle);
        }

        public static bool SelfTest(out string detail)
        {
            try {
                int w, h; float fx, fy;
                byte[] px = new byte[16];
                if (create_texture(0UL, 1, 1, px, false) != IntPtr.Zero) { detail = "create_texture!=0"; return false; }
                if (load_texture_png(0UL, "", out w, out h) != IntPtr.Zero || w != 0 || h != 0) { detail = "load_texture_png"; return false; }
                get_logical_presentation(0UL, out w, out h);
                if (w != 0 || h != 0) { detail = "get_logical_presentation"; return false; }
                window_to_logical(0UL, 0f, 0f, out fx, out fy);
                if (fx != 0f || fy != 0f) { detail = "window_to_logical"; return false; }
                logical_to_window(0UL, 0f, 0f, out fx, out fy);
                if (fx != 0f || fy != 0f) { detail = "logical_to_window"; return false; }
                draw_mesh(0UL, 0UL, 0f, 0f, 0f, 1f, 1f, 1f, 255, 255, 255, 255);
                draw_mesh_tex(0UL, 0UL, 0f, 0f, 0f, 1f, 1f, 1f, IntPtr.Zero);
                // lit 绑定：null r3d/mesh 必须安全返回，不崩
                float[] idmat = new float[16];
                fixed (float* pm = idmat) s_draw_mesh_lit_tex(0UL, 0UL, pm, IntPtr.Zero);
                float[] light = new float[72];
                fixed (float* pl = light) s_set_light_block(0UL, pl);
                var created = new object[] {
                    ButtonCreate(0f, 0f, 1f, 1f, ""), LabelCreate(0f, 0f, 10f, 255, 255, 255, ""),
                    InputCreate(0f, 0f, 1f, 1f, 10f, 255, 255, 255, ""), ComboboxInputCreate(0f, 0f, 1f, 1f, 10f, 255, 255, 255, ""),
                    SliderCreate(0f, 0f, 1f, 1f, 0f, 1f, 0f), CheckboxCreate(0f, 0f, 1f, "")
                };
                created = null;
                if (input_get_text(0UL) != "") { detail = "input_get_text"; return false; }
                if (combobox_input_get_text(0UL) != "") { detail = "combobox_get_text"; return false; }
                slider_set_colors(0UL, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
                checkbox_set_state(0UL, 0);
                if (checkbox_get_state(0UL) != 0) { detail = "checkbox_get_state"; return false; }
                detail = "20 internal calls OK (null-handle safe)";
                return true;
            } catch (Exception ex) {
                detail = ex.GetType().Name + ": " + ex.Message;
                return false;
            }
        }
    }
}
