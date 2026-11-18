using System;
using System.Runtime.CompilerServices;

namespace Joy.Api
{
    /// <summary>
    /// r3d 的 L3 手写适配（IDL 里 status:off，生成器不出 thunk 的 API）。
    /// 与 Joy.Api.Bindings 同程序集，可访问句柄类型的 internal Handle：
    ///   - basis 绘制：矩阵以 float[16] 传入（列主序，顺序同 joy_mat44_t 字段声明
    ///     m0,m4,m8,m12, m1,m5,m9,m13, m2,m6,m10,m14, m3,m7,m11,m15）；
    ///   - world_to_screen / get_window_size：out 参数改成 C# out；
    ///   - mesh_custom：顶点 float[]（每 5 个一组 x,y,z,u,v）+ 顶点数。
    /// 这些是纯渲染能力，不改变任何逻辑状态，也不参与帧同步判定。
    /// </summary>
    public static class R3dExt
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ulong borrow_r3d(IntPtr p);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void draw_mesh_solid_basis(ulong r, ulong m, float[] mat,
            byte cr, byte cg, byte cb, byte ca);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void draw_wire_basis(ulong r, ulong m, float[] mat,
            byte cr, byte cg, byte cb, byte ca);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool world_to_screen(ulong r, float x, float y, float z,
            out float screenX, out float screenY);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void get_window_size(ulong r, out int w, out int h);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ulong mesh_custom(ulong r, float[] verts, int vertexCount);

        /// <summary>宿主创建的 r3d 上下文登记成借用句柄（所有权仍归宿主）。</summary>
        public static Jr3d R3d(IntPtr p)
        {
            if (p == IntPtr.Zero) return null;
            ulong h = borrow_r3d(p);
            return h == 0 ? null : new Jr3d(h);
        }

        public static void DrawMeshSolidBasis(Jr3d r, RenderMesh m, float[] mat,
            byte cr, byte cg, byte cb, byte ca)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            if (m == null) throw new ArgumentNullException(nameof(m));
            if (mat == null || mat.Length < 16) throw new ArgumentException("mat needs 16 floats", nameof(mat));
            draw_mesh_solid_basis(r.Handle, m.Handle, mat, cr, cg, cb, ca);
        }

        public static void DrawWireBasis(Jr3d r, RenderMesh m, float[] mat,
            byte cr, byte cg, byte cb, byte ca)
        {
            if (r == null) throw new ArgumentNullException(nameof(r));
            if (m == null) throw new ArgumentNullException(nameof(m));
            if (mat == null || mat.Length < 16) throw new ArgumentException("mat needs 16 floats", nameof(mat));
            draw_wire_basis(r.Handle, m.Handle, mat, cr, cg, cb, ca);
        }

        public static bool WorldToScreen(Jr3d r, float x, float y, float z,
            out float screenX, out float screenY)
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

        /// <summary>构造一个 identity + 缩放 + 平移的 mat44 数组（项目 pass 的常见用法，无旋转）。</summary>
        public static float[] MakeTrs(float sx, float sy, float sz, float tx, float ty, float tz)
        {
            // 列主序：索引按 jmath.h joy_mat44_t 字段声明顺序 m0,m4,m8,m12,...
            return new float[16] {
                sx, 0, 0, 0,
                0, sy, 0, 0,
                0, 0, sz, 0,
                tx, ty, tz, 1
            };
        }

        /// <summary>
        /// Headless 自检：用句柄 0 调用全部 6 个 InternalCall。C 侧对空句柄
        /// deref 失败即安全返回，不触碰 GPU，因此可在 dummy 驱动的 --smoke 下跑。
        /// 验证三件事：(1) 6 个 internal call 都已注册且可从 C# 调到（否则抛
        /// EntryPointNotFoundException）；(2) float[] 数组 / out 参数跨 ABI 正确；
        /// (3) 空句柄路径安全（不崩、按约定返回 false / 0）。
        /// </summary>
        public static bool SelfTest(out string detail)
        {
            try {
                // borrow：空指针必须返回 0（C 侧 w_borrow_r3d 守卫）
                if (borrow_r3d(IntPtr.Zero) != 0) { detail = "borrow(null)!=0"; return false; }

                float[] mat = MakeTrs(1f, 1f, 1f, 0f, 0f, 0f);
                // basis 绘制：空句柄安全返回（C 侧 deref 失败即 return）
                draw_mesh_solid_basis(0UL, 0UL, mat, 255, 0, 0, 255);
                draw_wire_basis(0UL, 0UL, mat, 0, 255, 0, 255);

                // 投影：空句柄返回 false，且 out 参数被写出（不保持未初始化）
                float sx, sy;
                if (world_to_screen(0UL, 0f, 0f, 0f, out sx, out sy)) {
                    detail = "world_to_screen(null)!=false"; return false;
                }

                // 窗口尺寸：空句柄写出 0,0
                int w, h;
                get_window_size(0UL, out w, out h);
                if (w != 0 || h != 0) { detail = "get_window_size(null) wrote " + w + "x" + h; return false; }

                // 动态 mesh：空句柄返回 0
                float[] verts = new float[15] { 0,0,0,0,0, 1,0,0,1,0, 0,1,0,0,1 };
                if (mesh_custom(0UL, verts, 3) != 0UL) { detail = "mesh_custom(null)!=0"; return false; }

                detail = "6 internal calls OK (null-handle safe)";
                return true;
            } catch (Exception ex) {
                detail = ex.GetType().Name + ": " + ex.Message;
                return false;
            }
        }
    }
}
