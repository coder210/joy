using System;
using System.Runtime.InteropServices;

namespace Joy.Api
{
    /// <summary>
    /// GJK 3D 碰撞桥：委托 joy_collision_gjk3df_*（与 C++ 帧同步碰撞同一实现）。
    /// CoreCLR 函数指针表绑定。参数/返回值均为 fp 原始 long（Q32），保证确定性。
    /// C 侧 bool = 1 字节（delegate* 用 byte 表达）；out 参数用指针。
    /// </summary>
    public static unsafe class CollisionExt
    {
        // bool w_gjk_box_box(15×int64, int64*×4)
        private static delegate* unmanaged[Cdecl]<
            long, long, long, long, long, long,
            long, long, long, long, long, long,
            long, long, long,
            long*, long*, long*, long*, byte> s_gjk_box_box;

        // bool w_gjk_mesh_box(int64* verts, int, 9×int64, int64*×4)
        private static delegate* unmanaged[Cdecl]<
            long*, int,
            long, long, long, long, long, long,
            long, long, long,
            long*, long*, long*, long*, byte> s_gjk_mesh_box;

        // bool w_ray_obb(15×int64, int64* out_dist)
        private static delegate* unmanaged[Cdecl]<
            long, long, long, long, long, long,
            long, long, long, long, long, long,
            long, long, long,
            long*, byte> s_ray_obb;

        internal static void Init()
        {
            s_gjk_box_box = (delegate* unmanaged[Cdecl]<
                long, long, long, long, long, long,
                long, long, long, long, long, long,
                long, long, long,
                long*, long*, long*, long*, byte>)NativeFuncs.Lookup("Joy.Api.CollisionExt::gjk_box_box");
            s_gjk_mesh_box = (delegate* unmanaged[Cdecl]<
                long*, int,
                long, long, long, long, long, long,
                long, long, long,
                long*, long*, long*, long*, byte>)NativeFuncs.Lookup("Joy.Api.CollisionExt::gjk_mesh_box");
            s_ray_obb = (delegate* unmanaged[Cdecl]<
                long, long, long, long, long, long,
                long, long, long, long, long, long,
                long, long, long,
                long*, byte>)NativeFuncs.Lookup("Joy.Api.CollisionExt::ray_obb");
        }

        public static bool BoxBox(
            long ax, long ay, long az, long ahx, long ahy, long ahz,
            long bx, long by, long bz, long bhx, long bhy, long bhz,
            long dirx, long diry, long dirz,
            out long nx, out long ny, out long nz, out long depth)
        {
            long lx = 0, ly = 0, lz = 0, ld = 0;
            byte r = s_gjk_box_box(ax, ay, az, ahx, ahy, ahz, bx, by, bz, bhx, bhy, bhz,
                                   dirx, diry, dirz, &lx, &ly, &lz, &ld);
            nx = lx; ny = ly; nz = lz; depth = ld;
            return r != 0;
        }

        public static bool MeshBox(
            long[] verts, int vertCount,
            long bx, long by, long bz, long bhx, long bhy, long bhz,
            long dirx, long diry, long dirz,
            out long nx, out long ny, out long nz, out long depth)
        {
            long lx = 0, ly = 0, lz = 0, ld = 0;
            byte r;
            fixed (long* pv = verts)
            {
                r = s_gjk_mesh_box(pv, vertCount, bx, by, bz, bhx, bhy, bhz,
                                   dirx, diry, dirz, &lx, &ly, &lz, &ld);
            }
            nx = lx; ny = ly; nz = lz; depth = ld;
            return r != 0;
        }

        public static bool RayObb(
            long ox, long oy, long oz, long dx, long dy, long dz,
            long bx, long by, long bz, long bhx, long bhy, long bhz,
            long rx, long ry, long rz,
            out long dist)
        {
            long ld = 0;
            byte r = s_ray_obb(ox, oy, oz, dx, dy, dz, bx, by, bz, bhx, bhy, bhz,
                               rx, ry, rz, &ld);
            dist = ld;
            return r != 0;
        }
    }
}
