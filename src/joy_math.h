/******************************************************************************
 * src/joy_math.h —— math 域声明头（joy_math.c 的配套头）
 * 职责：数学 / 碰撞 / GJK / box2d 域的公开 API 声明。
 * 方向：本头是该域声明的归属地，由 include/joy/japi.h 聚合包含；
 *       本头只依赖基础类型头，不再 include 聚合门面 joy.h，避免循环包含。
 * 生成链：本头是 gen_bindings.js 的 DECL_SOURCES 之一，改动声明后必须跑 --scan。
 ******************************************************************************/
#ifndef JOY_MATH_IMPL_H
#define JOY_MATH_IMPL_H

#include <joy/jtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

        ///////////////////////////////////////////////////////////////////////
       ////////////////////////////  collision ///////////////////////////////
       ///////////////////////////////////////////////////////////////////////


        JOY_API joy_ray2d_collision_t joy_math_collision2d_get_ray_circle(joy_ray2d_t ray, joy_circle_t circle);
        JOY_API joy_ray2d_collision_t joy_math_collision2d_get_ray_rectangle(joy_ray2d_t ray, joy_rectangle_t rect);
        JOY_API joy_ray2d_collision_t joy_math_collision2d_get_ray_rectanglex(joy_ray2d_t ray, joy_rectangle_t rect, joy_ft_t angle);
        JOY_API joy_ray2d_collision_t joy_math_collision2d_get_ray_polygon(joy_ray2d_t ray, joy_polygon_t polygon);
        JOY_API bool joy_math_collision2d_check_point_rectangle(joy_vec2_t point, joy_rectangle_t rect);
        JOY_API bool joy_math_collision2d_check_point_circle(joy_vec2_t point, joy_circle_t circle);
        JOY_API joy_ft_t joy_math_collision2d_get_squared_distance_point_circle(joy_vec2_t point, joy_circle_t circle);
        JOY_API bool joy_math_collision2d_check_point_triangle(joy_vec2_t point, joy_vec2_t p1, joy_vec2_t p2, joy_vec2_t p3);
        JOY_API bool joy_math_collision2d_check_lines(joy_vec2_t a1, joy_vec2_t b1, joy_vec2_t a2, joy_vec2_t b2, joy_vec2_t* cp);
        JOY_API bool joy_math_collision2d_check_point_line(joy_vec2_t point, joy_vec2_t p1, joy_vec2_t p2, joy_ft_t threshold);
        JOY_API bool joy_math_collision2d_check_circles(joy_circle_t a, joy_circle_t b);
        JOY_API bool joy_math_collision2d_check_rectangles(joy_rectangle_t a, joy_rectangle_t b);
        JOY_API bool joy_math_collision2d_check_polygons(joy_polygon_t a, joy_polygon_t b);
        JOY_API bool joy_math_collision2d_check_circle_rectangle(joy_circle_t a, joy_rectangle_t b);
        JOY_API bool joy_math_collision2d_check_circle_polygon(joy_circle_t a, joy_polygon_t b);
        JOY_API int joy_math_collision2d_get_circles(joy_circle_t a, joy_circle_t b, joy_contact2d_t* contact);
        JOY_API int joy_math_collision2d_get_rectangles(joy_rectangle_t a, joy_ft_t a_angle, joy_rectangle_t b, joy_ft_t b_angle, joy_contact2d_t* contact);
        JOY_API int joy_math_collision2d_get_polygons(joy_polygon_t a, joy_polygon_t b, joy_contact2d_t* contact);
        JOY_API joy_ray3d_collision_t joy_math_collision3d_get_ray_triangle(joy_ray3d_t ray, joy_vec3_t p1, joy_vec3_t p2, joy_vec3_t p3);
        JOY_API joy_ray3d_collision_t joy_math_collision3d_get_ray_quad(joy_ray3d_t ray, joy_vec3_t p1, joy_vec3_t p2, joy_vec3_t p3, joy_vec3_t p4);
        JOY_API joy_ray3d_collision_t joy_math_collision3d_get_ray_sphere(joy_ray3d_t ray, joy_sphere_t sphere);
        JOY_API joy_ray3d_collision_t joy_math_collision3d_get_ray_box(joy_ray3d_t ray, joy_bounding_box_t box);
        JOY_API bool joy_math_collision3d_check_spheres(joy_sphere_t a, joy_sphere_t b);
        JOY_API bool joy_math_collision3d_check_boxes(joy_bounding_box_t a, joy_bounding_box_t b);
        JOY_API bool joy_math_collision3d_check_box_sphere(joy_bounding_box_t a, joy_sphere_t b);

        JOY_API joy_ray2d_collisionf_t joy_math_collision2df_get_ray_circle(joy_ray2df_t ray,
                joy_circlef_t circle);
        JOY_API joy_ray2d_collisionf_t joy_math_collision2df_get_ray_rectangle(joy_ray2df_t ray,
                joy_rectanglef_t rect);
        JOY_API joy_ray2d_collisionf_t joy_math_collision2df_get_ray_rectanglex(joy_ray2df_t ray,
                joy_rectanglef_t rect, joy_fp_t angle /* 旋转角, 单位: 度 */);
        JOY_API joy_ray2d_collisionf_t joy_math_collision2df_get_ray_polygon(joy_ray2df_t ray,
                joy_polygonf_t polygon);
        JOY_API bool joy_math_collision2df_check_point_rectangle(joy_vec2f_t point,
                joy_rectanglef_t rect);
        JOY_API bool joy_math_collision2df_check_point_circle(joy_vec2f_t point, joy_circlef_t circle);
        JOY_API joy_fp_t joy_math_collision2df_get_squared_distance_point_circle(joy_vec2f_t point, joy_circlef_t circle);
        JOY_API bool joy_math_collision2df_check_point_triangle(joy_vec2f_t point,
                joy_vec2f_t p1, joy_vec2f_t p2, joy_vec2f_t p3);
        JOY_API bool joy_math_collision2df_check_lines(joy_vec2f_t a1, joy_vec2f_t b1,
                joy_vec2f_t a2, joy_vec2f_t b2, joy_vec2f_t* cp);
        JOY_API bool joy_math_collision2df_check_point_line(joy_vec2f_t point,
                joy_vec2f_t p1, joy_vec2f_t p2, joy_fp_t threshold);
        JOY_API bool joy_math_collision2df_check_circles(joy_circlef_t a, joy_circlef_t b);
        JOY_API bool joy_math_collision2df_check_rectangles(joy_rectanglef_t a, joy_rectanglef_t b);
        JOY_API bool joy_math_collision2df_check_polygons(joy_polygonf_t a, joy_polygonf_t b);
        JOY_API bool joy_math_collision2df_check_circle_rectangle(joy_circlef_t a, joy_rectanglef_t b);
        JOY_API bool joy_math_collision2df_check_circle_polygon(joy_circlef_t a, joy_polygonf_t b);
        JOY_API int joy_math_collision2df_get_circles(joy_circlef_t a, joy_circlef_t b,
                joy_contact2df_t* contact);
        JOY_API int joy_math_collision2df_get_rectangles(joy_rectanglef_t a, joy_fp_t a_angle /* 度 */,
                joy_rectanglef_t b, joy_fp_t b_angle /* 度 */,
                joy_contact2df_t* contact);
        JOY_API joy_fp_t joy_math_find_min_separation(joy_polygonf_t a, joy_polygonf_t b,
                joy_vec2f_t* axis, joy_vec2f_t* point);
        JOY_API int joy_math_collision2df_get_polygons(joy_polygonf_t a, joy_polygonf_t b,
                joy_contact2df_t* contact);

        /* OBB（旋转矩形）沿 (dx,dy) 平移的「扫掠体」-> 凸包顶点（世界坐标）。
         *
         * 注意输出一般是六边形而不是矩形：矩形斜向平移时，起点盒与终点盒的凸包
         * 有 6 个顶点；只有位移平行于矩形某条边时才退化为矩形（4 顶点）。
         * 故这里以多边形输出，调用方缓冲区 out 容量至少 8。
         *
         * center     : OBB 中心（起点位置；终点 = center + (dx,dy)）
         * axis       : 长轴单位向量（短轴取其逆时针 90°，无需三角函数）
         * half_long  : 沿长轴的半长
         * half_short : 沿短轴的半宽
         * dx, dy     : 本步位移
         * out        : 输出顶点（容量 >= 8）
         * out_count  : 输出顶点数（4~8）
         *
         * 绕序：统一输出为「y 向下坐标系下的顺时针」，此时 vec2f_normal 给出外法线，
         * 与 joy_math_find_min_separation 的约定一致 —— 绕序反了会让分离距离符号反转、
         * 判定结果完全相反。
         *
         * 典型用法：把 out/out_count 包成 joy_polygonf_t，再交给 joy_math_collision2df_check_polygons。 */
        JOY_API void joy_math_collision2df_swept_obb(joy_vec2f_t center, joy_vec2f_t axis,
                joy_fp_t half_long, joy_fp_t half_short, joy_fp_t dx, joy_fp_t dy,
                joy_vec2f_t* out, int* out_count);

        /* ---- 瓦片地图扫掠碰撞（定点数，帧同步确定）----
         * solid(col,row) 返回 true 表示该格不可通行；实现须对越界格也返回 true（视为墙）。
         * joy_math_tilemap_sweep_x: 沿 X 推进 dx（可负）；撞墙则把 *px 贴到该格边界并返回 true。
         * joy_math_tilemap_sweep_y: 沿 Y 推进 vy*tick（vy>0 下落、<0 上升）；撞到则把 *py 贴到格边界、
         *                      并把 *vy 清零，返回 true。 */
        typedef bool (*joy_tilemap_solid_fn)(int col, int row);
        JOY_API bool joy_math_tilemap_sweep_x(joy_fp_t* px, joy_fp_t* py, joy_fp_t dx, joy_fp_t w, joy_fp_t h,
                joy_tilemap_solid_fn solid);
        JOY_API bool joy_math_tilemap_sweep_y(joy_fp_t* px, joy_fp_t* py, joy_fp_t* vy, joy_fp_t tick, joy_fp_t w, joy_fp_t h,
                joy_tilemap_solid_fn solid);

        JOY_API joy_ray3d_collisionf_t joy_math_collision3df_get_ray_triangle(joy_ray3df_t ray,
                joy_vec3f_t p1, joy_vec3f_t p2, joy_vec3f_t p3);
        JOY_API joy_ray3d_collisionf_t joy_math_collision3df_get_ray_quad(joy_ray3df_t ray,
                joy_vec3f_t p1, joy_vec3f_t p2, joy_vec3f_t p3, joy_vec3f_t p4);
        JOY_API joy_ray3d_collisionf_t joy_math_collision3df_get_ray_sphere(joy_ray3df_t ray,
                joy_spheref_t sphere);
        JOY_API joy_ray3d_collisionf_t joy_math_collision3df_get_ray_box(joy_ray3df_t ray,
                joy_bounding_boxf_t box);
        /* OBB(旋转盒)射线检测: position=中心, half_size=半边长, rotation=欧拉角(弧度) */
        JOY_API joy_ray3d_collisionf_t joy_math_collision3df_get_ray_obb(joy_ray3df_t ray,
                joy_vec3f_t position, joy_vec3f_t half_size, joy_vec3f_t rotation);
        JOY_API bool joy_math_collision3df_check_spheres(joy_spheref_t a, joy_spheref_t b);
        JOY_API bool joy_math_collision3df_check_boxes(joy_bounding_boxf_t a,
                joy_bounding_boxf_t b);
        JOY_API bool joy_math_collision3df_check_box_sphere(joy_bounding_boxf_t a,
                joy_spheref_t b);


        /* 主碰撞检测: GJK + EPA, 返回是否碰撞, contact填充法线和深度 */
        JOY_API bool joy_math_gjk2d_collide(joy_gjk2d_collider_p c1, joy_gjk2d_collider_p c2,
                joy_vec2f_t init_dir, joy_gjk2d_contact_p contact);
        JOY_API void joy_math_gjk2d_init_circle(joy_gjk2d_collider_t* c, joy_vec2f_t pos, joy_fp_t radius);
        JOY_API void joy_math_gjk2d_init_box(joy_gjk2d_collider_t* c, joy_vec2f_t pos, joy_vec2f_t half_size);
        JOY_API void joy_math_gjk2d_init_capsule(joy_gjk2d_collider_t* c, joy_vec2f_t pos, joy_vec2f_t axis, joy_fp_t height, joy_fp_t radius);
        JOY_API void joy_math_gjk2d_init_polygon(joy_gjk2d_collider_t* c, joy_vec2f_t* vertices, int num);
        JOY_API void joy_math_gjk2d_init_pie(joy_gjk2d_collider_t* c, joy_vec2f_t pos, joy_fp_t radius, joy_fp_t sweep);
        JOY_API bool joy_math_gjk3df_collide(joy_gjk3df_collider_p c1, joy_gjk3df_collider_p c2,
                joy_vec3f_t init_dir, joy_gjk3df_contact_p contact);
        /* 快捷初始化函数(仅设置参数,无内存分配) */
        JOY_API void joy_math_gjk3df_init_sphere(joy_gjk3df_collider_t* c, joy_vec3f_t pos, joy_fp_t radius);
        JOY_API void joy_math_gjk3df_init_box(joy_gjk3df_collider_t* c, joy_vec3f_t pos, joy_vec3f_t half_size);
        JOY_API void joy_math_gjk3df_init_obb(joy_gjk3df_collider_t* c, joy_vec3f_t pos, joy_vec3f_t half_size, joy_vec3f_t rotation);
        JOY_API void joy_math_gjk3df_init_capsule(joy_gjk3df_collider_t* c, joy_vec3f_t pos, joy_fp_t radius, joy_fp_t height, joy_vec3f_t axis);
        JOY_API void joy_math_gjk3df_init_cylinder(joy_gjk3df_collider_t* c, joy_vec3f_t pos, joy_fp_t radius, joy_fp_t height);
        JOY_API void joy_math_gjk3df_init_cone(joy_gjk3df_collider_t* c, joy_vec3f_t pos, joy_fp_t radius, joy_fp_t height);
        JOY_API void joy_math_gjk3df_init_mesh(joy_gjk3df_collider_t* c, joy_vec3f_t* vertices, int num);

        /* ============ GJK 3D 浮点版 (joy_ft_t/joy_vec3_t) ============ */
        /* 主碰撞检测: GJK + EPA, 返回是否碰撞, contact填充法线和深度 */
        JOY_API bool joy_math_gjk3d_collide(joy_gjk3d_collider_p c1, joy_gjk3d_collider_p c2,
                joy_vec3_t init_dir, joy_gjk3d_contact_p contact);
        /* 快捷初始化函数(仅设置参数,无内存分配) */
        JOY_API void joy_math_gjk3d_init_sphere(joy_gjk3d_collider_t* c, joy_vec3_t pos, joy_ft_t radius);
        JOY_API void joy_math_gjk3d_init_box(joy_gjk3d_collider_t* c, joy_vec3_t pos, joy_vec3_t half_size);
        JOY_API void joy_math_gjk3d_init_capsule(joy_gjk3d_collider_t* c, joy_vec3_t pos, joy_ft_t radius, joy_ft_t height, joy_vec3_t axis);
        JOY_API void joy_math_gjk3d_init_cylinder(joy_gjk3d_collider_t* c, joy_vec3_t pos, joy_ft_t radius, joy_ft_t height);
        JOY_API void joy_math_gjk3d_init_cone(joy_gjk3d_collider_t* c, joy_vec3_t pos, joy_ft_t radius, joy_ft_t height);
        JOY_API void joy_math_gjk3d_init_mesh(joy_gjk3d_collider_t* c, joy_vec3_t* vertices, int num);

        /* ==================== box2d ==================== */
        JOY_API joy_box2d_world_t* joy_math_box2d_create_world(joy_vec2f_t gravity, int iterations);
        JOY_API void joy_math_box2d_clear_world(joy_box2d_world_t* world);
        JOY_API void joy_math_box2d_destroy_world(joy_box2d_world_t* world);
        JOY_API void joy_math_box2d_step(joy_box2d_world_t* world, joy_fp_t dt);
        JOY_API joy_box2d_body_t* joy_math_box2d_create_body(joy_box2d_world_t* world, joy_fp_t mass, joy_fp_t bx, joy_fp_t by);
        JOY_API void joy_math_box2d_destroy_body(joy_box2d_world_t* world, int id);
        JOY_API joy_box2d_joint_t* joy_math_box2d_create_joint(joy_box2d_world_t* world, int id1, int id2, joy_vec2f_t anchor);
        JOY_API void joy_math_box2d_destroy_joint(joy_box2d_world_t* world, joy_box2d_joint_t* joint);
        JOY_API void joy_math_box2d_foreach_bodies(joy_box2d_world_t* world, void (*iterator)(joy_box2d_body_t*));
        JOY_API void joy_math_box2d_foreach_joints(joy_box2d_world_t* world, void (*iterator)(joy_box2d_joint_t*, joy_box2d_body_t*, joy_box2d_body_t*));
        JOY_API void joy_math_box2d_foreach_arbiters(joy_box2d_world_t* world, void (*iterator)(joy_box2d_arbiter_t*));

#ifdef __cplusplus
}
#endif

#endif /* JOY_MATH_IMPL_H */
