using System;
using System.Runtime.CompilerServices;

namespace Joy.Api
{
    /// <summary>
    /// GJK 3D 碰撞桥：委托 joy_collision_gjk3df_*（与 C++ 帧同步碰撞同一实现）。
    /// 这些 C API 在 IDL 里是 X 级、生成器不出绑定，故手写 InternalCall。
    /// 参数/返回值均为 fp 原始 long（Q32），不做浮点转换，保证确定性。
    /// </summary>
    public static class CollisionExt
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool gjk_box_box(
            long ax, long ay, long az, long ahx, long ahy, long ahz,
            long bx, long by, long bz, long bhx, long bhy, long bhz,
            long dirx, long diry, long dirz,
            out long nx, out long ny, out long nz, out long depth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool gjk_mesh_box(
            long[] verts, int vertCount,
            long bx, long by, long bz, long bhx, long bhy, long bhz,
            long dirx, long diry, long dirz,
            out long nx, out long ny, out long nz, out long depth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool ray_obb(
            long ox, long oy, long oz, long dx, long dy, long dz,
            long bx, long by, long bz, long bhx, long bhy, long bhz,
            long rx, long ry, long rz,
            out long dist);

        /// <summary>
        /// box(a) vs box(b) 碰撞检测。命中时输出 normal（a→b）与 depth（fp 原始值）。
        /// </summary>
        public static bool BoxBox(
            long ax, long ay, long az, long ahx, long ahy, long ahz,
            long bx, long by, long bz, long bhx, long bhy, long bhz,
            long dirx, long diry, long dirz,
            out long nx, out long ny, out long nz, out long depth)
        {
            return gjk_box_box(ax, ay, az, ahx, ahy, ahz,
                               bx, by, bz, bhx, bhy, bhz,
                               dirx, diry, dirz,
                               out nx, out ny, out nz, out depth);
        }

        /// <summary>
        /// mesh（顶点数组，每 3 个 fp = xyz）vs box 的 GJK 检测。命中时输出 normal（mesh→box）与 depth。
        /// </summary>
        public static bool MeshBox(
            long[] verts, int vertCount,
            long bx, long by, long bz, long bhx, long bhy, long bhz,
            long dirx, long diry, long dirz,
            out long nx, out long ny, out long nz, out long depth)
        {
            return gjk_mesh_box(verts, vertCount,
                                bx, by, bz, bhx, bhy, bhz,
                                dirx, diry, dirz,
                                out nx, out ny, out nz, out depth);
        }

        /// <summary>射线 vs OBB（rotation 为欧拉弧度）。命中输出距离（fp）。</summary>
        public static bool RayObb(
            long ox, long oy, long oz, long dx, long dy, long dz,
            long bx, long by, long bz, long bhx, long bhy, long bhz,
            long rx, long ry, long rz,
            out long dist)
        {
            return ray_obb(ox, oy, oz, dx, dy, dz,
                           bx, by, bz, bhx, bhy, bhz,
                           rx, ry, rz, out dist);
        }
    }
}
