using System;
using System.Runtime.CompilerServices;

namespace Joy.Api
{
    /// <summary>
    /// r3d L3/L2 手写适配第二批（R3dExt 的补充）：纹理加载、逻辑坐标换算、
    /// 带 vec3/color 按值的绘制，以及 r3d UI 对象的创建/取值/配色/状态/回调。
    ///
    /// 这些 API 在 joy.idl 里多为 status:off（void* 缓冲、按值结构体、返回 char*、
    /// 函数指针回调），生成器不出 thunk，故在 C 侧 csclib_r3d_plus.c 手写 InternalCall。
    /// 与 Joy.Api.Bindings 同程序集，可访问句柄类型的 internal Handle。
    ///
    /// 句柄所有权：create 系列返回的对象注册了对应 *_destroy 析构，交给 GC/Release 释放；
    /// 纹理是 void*（SDL_GPUTexture*），无强类型句柄类，业务层自行持有 IntPtr。
    /// </summary>
    public static class R3dExtPlus
    {
        // ---- 纹理 ----
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr create_texture(ulong r, int w, int h, byte[] rgba32, bool flip);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr load_texture_png(ulong r, string path, out int outW, out int outH);

        // ---- 逻辑坐标换算 ----
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void get_logical_presentation(ulong r, out int w, out int h);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void window_to_logical(ulong r, float wx, float wy, out float lx, out float ly);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void logical_to_window(ulong r, float lx, float ly, out float wx, out float wy);

        // ---- 带 vec3/color 按值的绘制 ----
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void draw_mesh(ulong r, ulong m,
            float px, float py, float pz, float sx, float sy, float sz,
            byte cr, byte cg, byte cb, byte ca);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void draw_mesh_tex(ulong r, ulong m,
            float px, float py, float pz, float sx, float sy, float sz, IntPtr tex);

        // ---- UI create（返回带析构的强类型句柄）----
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ulong button_create(float x, float y, float w, float h, string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ulong label_create(float x, float y, float fontSize,
            byte cr, byte cg, byte cb, string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ulong input_create(float x, float y, float w, float h, float fontSize,
            byte cr, byte cg, byte cb, string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ulong combobox_input_create(float x, float y, float w, float h, float fontSize,
            byte cr, byte cg, byte cb, string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ulong slider_create(float x, float y, float w, float h,
            float min, float max, float initial);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ulong checkbox_create(float x, float y, float size, string label);

        // ---- get_text（返回 char*）----
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string input_get_text(ulong inp);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string combobox_input_get_text(ulong ci);

        // ---- slider / checkbox 颜色与状态 ----
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void slider_set_colors(ulong s,
            byte tr, byte tg, byte tb, byte ta,
            byte fr, byte fg, byte fb, byte fa,
            byte hr, byte hg, byte hb, byte ha,
            byte mr, byte mg, byte mb, byte ma);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void checkbox_set_state(ulong cb, int state);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int checkbox_get_state(ulong cb);

        // ---- 纹理 ----
        public static IntPtr CreateTexture(Jr3d r, int w, int h, byte[] rgba32, bool flip)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            if (rgba32 == null || rgba32.Length < w * h * 4)
                throw new ArgumentException("rgba32 too small", nameof(rgba32));
            return create_texture(r.Handle, w, h, rgba32, flip);
        }

        public static IntPtr LoadTexturePng(Jr3d r, string path, out int w, out int h)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            return load_texture_png(r.Handle, path ?? "", out w, out h);
        }

        // ---- 逻辑坐标换算 ----
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

        // ---- 绘制 ----
        public static void DrawMesh(Jr3d r, RenderMesh m,
            float px, float py, float pz, float sx, float sy, float sz,
            byte cr, byte cg, byte cb, byte ca)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            if (m == null) throw new ArgumentNullException(nameof(m));
            draw_mesh(r.Handle, m.Handle, px, py, pz, sx, sy, sz, cr, cg, cb, ca);
        }

        public static void DrawMeshTex(Jr3d r, RenderMesh m,
            float px, float py, float pz, float sx, float sy, float sz, IntPtr tex)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            if (m == null) throw new ArgumentNullException(nameof(m));
            draw_mesh_tex(r.Handle, m.Handle, px, py, pz, sx, sy, sz, tex);
        }

        // ---- UI create ----
        public static Jr3dButton ButtonCreate(float x, float y, float w, float h, string text)
        {
            ulong hnd = button_create(x, y, w, h, text ?? "");
            return hnd == 0 ? null : new Jr3dButton(hnd);
        }

        public static Jr3dLabel LabelCreate(float x, float y, float fontSize,
            byte cr, byte cg, byte cb, string text)
        {
            ulong hnd = label_create(x, y, fontSize, cr, cg, cb, text ?? "");
            return hnd == 0 ? null : new Jr3dLabel(hnd);
        }

        public static Jr3dInput InputCreate(float x, float y, float w, float h, float fontSize,
            byte cr, byte cg, byte cb, string text)
        {
            ulong hnd = input_create(x, y, w, h, fontSize, cr, cg, cb, text ?? "");
            return hnd == 0 ? null : new Jr3dInput(hnd);
        }

        public static Jr3dComboboxInput ComboboxInputCreate(float x, float y, float w, float h, float fontSize,
            byte cr, byte cg, byte cb, string text)
        {
            ulong hnd = combobox_input_create(x, y, w, h, fontSize, cr, cg, cb, text ?? "");
            return hnd == 0 ? null : new Jr3dComboboxInput(hnd);
        }

        public static Jr3dSlider SliderCreate(float x, float y, float w, float h,
            float min, float max, float initial)
        {
            ulong hnd = slider_create(x, y, w, h, min, max, initial);
            return hnd == 0 ? null : new Jr3dSlider(hnd);
        }

        public static Jr3dCheckbox CheckboxCreate(float x, float y, float size, string label)
        {
            ulong hnd = checkbox_create(x, y, size, label ?? "");
            return hnd == 0 ? null : new Jr3dCheckbox(hnd);
        }

        // ---- get_text ----
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

        // ---- slider / checkbox ----
        public static void SliderSetColors(Jr3dSlider s,
            byte tr, byte tg, byte tb, byte ta,
            byte fr, byte fg, byte fb, byte fa,
            byte hr, byte hg, byte hb, byte ha,
            byte mr, byte mg, byte mb, byte ma)
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

        /// <summary>
        /// Headless 自检：用空句柄调用全部 18 个 InternalCall，确认注册成功且空句柄安全。
        /// C 侧 deref 失败即安全返回，不触碰 GPU，可在 dummy 驱动 --smoke 下跑。
        /// </summary>
        public static bool SelfTest(out string detail)
        {
            try {
                int w, h;
                float fx, fy;
                byte[] px = new byte[16]; // 1x1 RGBA，仅走空 r3d 句柄路径

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
                // UI create 不依赖 r3d/GPU，空参数也会成功分配对象；只验证 InternalCall
                // 可调通（不抛异常）。用 public 包装拿到强类型对象，方法退出后由 GC
                // finalizer -> Native.Release -> dtor 释放，避免句柄泄漏。
                var created = new object[] {
                    ButtonCreate(0f, 0f, 1f, 1f, ""),
                    LabelCreate(0f, 0f, 10f, 255, 255, 255, ""),
                    InputCreate(0f, 0f, 1f, 1f, 10f, 255, 255, 255, ""),
                    ComboboxInputCreate(0f, 0f, 1f, 1f, 10f, 255, 255, 255, ""),
                    SliderCreate(0f, 0f, 1f, 1f, 0f, 1f, 0f),
                    CheckboxCreate(0f, 0f, 1f, "")
                };
                created = null;
                if (input_get_text(0UL) != "") { detail = "input_get_text"; return false; }
                if (combobox_input_get_text(0UL) != "") { detail = "combobox_get_text"; return false; }
                slider_set_colors(0UL, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
                checkbox_set_state(0UL, 0);
                if (checkbox_get_state(0UL) != 0) { detail = "checkbox_get_state"; return false; }

                detail = "18 internal calls OK (null-handle safe)";
                return true;
            } catch (Exception ex) {
                detail = ex.GetType().Name + ": " + ex.Message;
                return false;
            }
        }

        // 注：button_set_onclick（函数指针回调）暂不提供托管包装。UI 场景的点击
        // 判定统一走已有的 Bindings.R3dButtonIsPressed / IsHovered 轮询，语义等价，
        // 避免引入 C→C# 反向 mono_runtime_invoke 的复杂性。
    }
}
