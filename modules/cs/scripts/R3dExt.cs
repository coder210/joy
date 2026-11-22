using System;
using System.Runtime.InteropServices;

namespace Joy.Api
{
    /// <summary>
    /// r3d 的 L3 手写适配（IDL 里 status:off，生成器不出 thunk 的 API）。CoreCLR 函数指针表绑定。
    /// 与 Joy.Api.Bindings 同程序集，可访问句柄类型的 internal Handle。
    /// </summary>
    public static unsafe class R3dExt
    {
        // C 侧签名（bool=byte；MonoString*->byte*；MonoArray*->float*；out->指针）
        private static delegate* unmanaged[Cdecl]<IntPtr, ulong> s_borrow_r3d;
        private static delegate* unmanaged[Cdecl]<ulong, byte*, ulong> s_r3d_create;
        private static delegate* unmanaged[Cdecl]<ulong, ulong, float*, byte, byte, byte, byte, void> s_draw_mesh_solid_basis;
        private static delegate* unmanaged[Cdecl]<ulong, ulong, float*, byte, byte, byte, byte, void> s_draw_wire_basis;
        private static delegate* unmanaged[Cdecl]<ulong, float, float, float, float*, float*, byte> s_world_to_screen;
        private static delegate* unmanaged[Cdecl]<ulong, int*, int*, void> s_get_window_size;
        private static delegate* unmanaged[Cdecl]<ulong, float*, int, ulong> s_mesh_custom;

        internal static void Init()
        {
            s_borrow_r3d = (delegate* unmanaged[Cdecl]<IntPtr, ulong>)NativeFuncs.Lookup("Joy.Api.R3dExt::borrow_r3d");
            s_r3d_create = (delegate* unmanaged[Cdecl]<ulong, byte*, ulong>)NativeFuncs.Lookup("Joy.Api.R3dExt::r3d_create");
            s_draw_mesh_solid_basis = (delegate* unmanaged[Cdecl]<ulong, ulong, float*, byte, byte, byte, byte, void>)NativeFuncs.Lookup("Joy.Api.R3dExt::draw_mesh_solid_basis");
            s_draw_wire_basis = (delegate* unmanaged[Cdecl]<ulong, ulong, float*, byte, byte, byte, byte, void>)NativeFuncs.Lookup("Joy.Api.R3dExt::draw_wire_basis");
            s_world_to_screen = (delegate* unmanaged[Cdecl]<ulong, float, float, float, float*, float*, byte>)NativeFuncs.Lookup("Joy.Api.R3dExt::world_to_screen");
            s_get_window_size = (delegate* unmanaged[Cdecl]<ulong, int*, int*, void>)NativeFuncs.Lookup("Joy.Api.R3dExt::get_window_size");
            s_mesh_custom = (delegate* unmanaged[Cdecl]<ulong, float*, int, ulong>)NativeFuncs.Lookup("Joy.Api.R3dExt::mesh_custom");
        }

        private static ulong borrow_r3d(IntPtr p) => s_borrow_r3d(p);

        private static ulong r3d_create(ulong win, string shaderDir)
        {
            byte* p = NativeFuncs.Utf8(shaderDir ?? "");
            try { return s_r3d_create(win, p); } finally { NativeFuncs.FreeUtf8(p); }
        }

        private static void draw_mesh_solid_basis(ulong r, ulong m, float[] mat, byte cr, byte cg, byte cb, byte ca)
        {
            fixed (float* p = mat) s_draw_mesh_solid_basis(r, m, p, cr, cg, cb, ca);
        }

        private static void draw_wire_basis(ulong r, ulong m, float[] mat, byte cr, byte cg, byte cb, byte ca)
        {
            fixed (float* p = mat) s_draw_wire_basis(r, m, p, cr, cg, cb, ca);
        }

        private static bool world_to_screen(ulong r, float x, float y, float z, out float screenX, out float screenY)
        {
            float sx = 0, sy = 0;
            byte res = s_world_to_screen(r, x, y, z, &sx, &sy);
            screenX = sx; screenY = sy;
            return res != 0;
        }

        private static void get_window_size(ulong r, out int w, out int h)
        {
            int lw = 0, lh = 0;
            s_get_window_size(r, &lw, &lh);
            w = lw; h = lh;
        }

        private static ulong mesh_custom(ulong r, float[] verts, int vertexCount)
        {
            fixed (float* p = verts) return s_mesh_custom(r, p, vertexCount);
        }

        /// <summary>宿主创建的 r3d 上下文登记成借用句柄（所有权仍归宿主）。</summary>
        public static Jr3d R3d(IntPtr p)
        {
            if (p == IntPtr.Zero) return null;
            ulong h = borrow_r3d(p);
            return h == 0 ? null : new Jr3d(h);
        }

        public static Jr3d R3dCreate(Jwindow win, string shaderDir)
        {
            if (win == null) throw new ArgumentNullException(nameof(win));
            ulong h = r3d_create(win.Handle, shaderDir);
            return h == 0 ? null : new Jr3d(h);
        }

        public static void DrawMeshSolidBasis(Jr3d r, RenderMesh m, float[] mat, byte cr, byte cg, byte cb, byte ca)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            if (m == null) throw new ArgumentNullException(nameof(m));
            if (mat == null || mat.Length < 16) throw new ArgumentException("mat needs 16 floats", nameof(mat));
            draw_mesh_solid_basis(r.Handle, m.Handle, mat, cr, cg, cb, ca);
        }

        public static void DrawWireBasis(Jr3d r, RenderMesh m, float[] mat, byte cr, byte cg, byte cb, byte ca)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            if (m == null) throw new ArgumentNullException(nameof(m));
            if (mat == null || mat.Length < 16) throw new ArgumentException("mat needs 16 floats", nameof(mat));
            draw_wire_basis(r.Handle, m.Handle, mat, cr, cg, cb, ca);
        }

        public static bool WorldToScreen(Jr3d r, float x, float y, float z, out float screenX, out float screenY)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            return world_to_screen(r.Handle, x, y, z, out screenX, out screenY);
        }

        public static void GetWindowSize(Jr3d r, out int w, out int h)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            get_window_size(r.Handle, out w, out h);
        }

        public static RenderMesh MeshCustom(Jr3d r, float[] verts, int vertexCount)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            ulong h = mesh_custom(r.Handle, verts, vertexCount);
            if (h == 0) throw new InvalidOperationException("joy_r3d_mesh_custom failed");
            return new RenderMesh(h);
        }

        public static float[] MakeTrs(float sx, float sy, float sz, float tx, float ty, float tz)
        {
            return new float[16] {
                sx, 0, 0, 0,
                0, sy, 0, 0,
                0, 0, sz, 0,
                tx, ty, tz, 1
            };
        }

        public static bool SelfTest(out string detail)
        {
            try {
                if (borrow_r3d(IntPtr.Zero) != 0) { detail = "borrow(null)!=0"; return false; }
                float[] mat = MakeTrs(1f, 1f, 1f, 0f, 0f, 0f);
                draw_mesh_solid_basis(0UL, 0UL, mat, 255, 0, 0, 255);
                draw_wire_basis(0UL, 0UL, mat, 0, 255, 0, 255);
                float sx, sy;
                if (world_to_screen(0UL, 0f, 0f, 0f, out sx, out sy)) { detail = "world_to_screen(null)!=false"; return false; }
                int w, h;
                get_window_size(0UL, out w, out h);
                if (w != 0 || h != 0) { detail = "get_window_size(null) wrote " + w + "x" + h; return false; }
                float[] verts = new float[15] { 0,0,0,0,0, 1,0,0,1,0, 0,1,0,0,1 };
                if (mesh_custom(0UL, verts, 3) != 0UL) { detail = "mesh_custom(null)!=0"; return false; }
                if (r3d_create(0UL, "shaders") != 0UL) { detail = "r3d_create(badwin)!=0"; return false; }
                detail = "7 internal calls OK (null-handle safe)";
                return true;
            } catch (Exception ex) {
                detail = ex.GetType().Name + ": " + ex.Message;
                return false;
            }
        }
    }
}
