/******************************************************************************
 * src/joy_math.c —— math 域唯一实现文件（一个域一个 .c，配 joy_math.h）
 * 职责：数学 / 碰撞 / GJK / box2d
 * 合并自：jcollision.c / jgjk.c / jbox2d.c
 * 查表数据（SIN_LUT / tan_lut）已拆到独立数据分片 src/joy_lut.c。
 * 说明：本域公开 API 声明在配套头 src/joy_math.h，由 include/joy/japi.h 聚合包含；
 *       本文件是该域唯一的编译单元。
 *       内部符号（static / 私有宏 / typedef）保持各分片独立，未跨分片改名。
 ******************************************************************************/

#include "joy_math.h"

/* ==================== 合并自 src/jcollision.c ==================== */
#include "joy.h"

joy_ray2d_collisionf_t
joy_math_collision2df_get_ray_circle(joy_ray2df_t ray, joy_circlef_t circle)
{
        joy_vec2f_t seg;
        joy_fp_t r_sq, d_sq, seg_len_sq, seg_proj, dx;
        joy_ray2d_collisionf_t result = {0};
        joy_vec2f_t distance;

        r_sq = fp_pow2(circle.radius);
        seg = vec2f_sub(circle.center, ray.origin);

        seg_len_sq = vec2f_length_squared(seg);
        ray.direction = vec2f_normalize(ray.direction);
        seg_proj = vec2f_dot(seg, ray.direction);

        /* 射线起点在圆上 */
        if (seg_len_sq == r_sq) {
                result.hit = true;
                result.distance = fp_zero();
                result.point = ray.origin;
                result.normal = vec2f_normalize(seg);
                return result;
        }

        /* 射线起点在圆内：找远端的出口交点 */
        if (seg_len_sq < r_sq) {
                dx = fp_sqrt(fp_sub(r_sq, fp_sub(seg_len_sq, fp_pow2(seg_proj))));
                distance = vec2f_scale(ray.direction, fp_add(seg_proj, dx));
                result.hit = true;
                result.distance = vec2f_length(distance);
                result.point = vec2f_add(ray.origin, distance);
                result.normal = vec2f_normalize(
                        vec2f_sub(result.point, circle.center));
                return result;
        }

        /* 射线起点在圆外，射线背向圆 */
        if (seg_proj < fp_zero())
                return result;

        /* 射线起点在圆外，射线朝向圆 */
        d_sq = fp_sub(seg_len_sq, fp_pow2(seg_proj));
        if (d_sq > r_sq)
                return result;

        if (d_sq == r_sq) {
                result.hit = true;
                result.distance = seg_proj;
                result.point = vec2f_add(ray.origin,
                         vec2f_scale(ray.direction, seg_proj));
                result.normal = vec2f_normalize(
                        vec2f_sub(result.point, circle.center));
                return result;
        }
        else {
                dx = fp_sqrt(fp_sub(r_sq, d_sq));
                result.hit = true;
                result.distance = fp_sub(seg_proj, dx);
                result.point = vec2f_add(ray.origin,
                         vec2f_scale(ray.direction, result.distance));
                result.normal = vec2f_normalize(
                        vec2f_sub(result.point, circle.center));
                return result;
        }
}

// 辅助函数：计算矩形碰撞点的法线（轴对齐边界专用）
static joy_vec2f_t collision2df_calculate_rect_normal(joy_vec2f_t point, joy_vec2f_t min, joy_vec2f_t max)
{
        joy_vec2f_t normal = { fp_zero(), fp_zero() };
        const joy_fp_t epsilon = fp_from_float(0.001f); // 极小值，避免浮点误差

        // 左边界
        if (fp_abs(fp_sub(point.x, min.x)) < epsilon) {
                normal.x = fp_from_float(-1);
        }
        // 右边界
        else if (fp_abs(fp_sub(point.x, max.x)) < epsilon) {
                normal.x = fp_from_float(1);
        }
        // 下边界
        else if (fp_abs(fp_sub(point.y, min.y)) < epsilon) {
                normal.y = fp_from_float(-1);
        }
        // 上边界
        else if (fp_abs(fp_sub(point.y, max.y)) < epsilon) {
                normal.y = fp_from_float(1);
        }

        return vec2f_normalize(normal);
}

// 放在函数上方 / 头文件中
#define SWAP(a, b)  do { joy_fp_t tmp = (a); (a) = (b); (b) = tmp; } while(0)

// 射线-矩形碰撞检测（修复版）
joy_ray2d_collisionf_t joy_math_collision2df_get_ray_rectangle(joy_ray2df_t ray, joy_rectanglef_t rect)
{
        joy_ray2d_collisionf_t result = { 0 };
        joy_vec2f_t min, max;
        joy_fp_t t_min, t_max, inv_dir;
        joy_fp_t t1, t2, tmp;


        ray.direction = vec2f_normalize(ray.direction);

        // 1. 计算矩形的最小/最大坐标
        min.x = rect.x;
        min.y = rect.y;
        max.x = fp_add(rect.x, rect.width);
        max.y = fp_add(rect.y, rect.height);

        // 2. 初始化射线参数：t_min=0（起点），t_max=极大值（无限射线）
        t_min = fp_zero();
        t_max = fp_max_value(); // 【修复】替换为你的定点数最大值（无穷大）

        // ===================== 【修复】检查 X 轴 =====================
        if (ray.direction.x == fp_zero()) {
                // 射线平行于Y轴，判断起点X是否在矩形内
                if (ray.origin.x < min.x || ray.origin.x > max.x)
                        return result;
        }
        else {
                inv_dir = fp_div(fp_one(), ray.direction.x);
                t1 = fp_mul(fp_sub(min.x, ray.origin.x), inv_dir);
                t2 = fp_mul(fp_sub(max.x, ray.origin.x), inv_dir);

                // 保证t1 < t2
                if (t1 > t2) { SWAP(t1, t2); }

                t_min = fp_max(t_min, t1);
                t_max = fp_min(t_max, t2);

                // 无交集
                if (t_min > t_max) return result;
        }

        // ===================== 【修复】检查 Y 轴 =====================
        if (ray.direction.y == fp_zero()) {
                // 射线平行于X轴，判断起点Y是否在矩形内
                if (ray.origin.y < min.y || ray.origin.y > max.y)
                        return result;
        }
        else {
                inv_dir = fp_div(fp_one(), ray.direction.y);
                t1 = fp_mul(fp_sub(min.y, ray.origin.y), inv_dir);
                t2 = fp_mul(fp_sub(max.y, ray.origin.y), inv_dir);

                if (t1 > t2) { SWAP(t1, t2); }

                t_min = fp_max(t_min, t1);
                t_max = fp_min(t_max, t2);

                if (t_min > t_max) return result;
        }

        // ===================== 计算碰撞结果 =====================
        result.hit = true;
        // 【修复】正确的碰撞距离 = t_min（射线参数）
        result.distance = t_min;

        // 碰撞点 = 起点 + t_min * 方向
        joy_vec2f_t delta = {
            fp_mul(t_min, ray.direction.x),
            fp_mul(t_min, ray.direction.y)
        };
        result.point = vec2f_add(ray.origin, delta);

        // 【修复】正确计算矩形碰撞法线（垂直于碰撞边）
        result.normal = collision2df_calculate_rect_normal(result.point, min, max);

        return result;
}



joy_ray2d_collisionf_t
joy_math_collision2df_get_ray_rectanglex(joy_ray2df_t ray, joy_rectanglef_t rect, joy_fp_t angle)
{
        joy_ray2d_collisionf_t result={0};
        joy_vec2f_t half_extents, center, local_origin, local_dir;
        joy_fp_t rotation, t_min, t_max;
        joy_fp_t t1, t2, tmp, inv_dir, t;
        joy_vec2f_t local_hit;

        const joy_fp_t EPSILON = fp_from_float(0.0001f);

        half_extents.x = fp_mul(rect.width, fp_half());
        half_extents.y = fp_mul(rect.height, fp_half());
        center.x = fp_add(rect.x, half_extents.x);
        center.y = fp_add(rect.y, half_extents.y);
        rotation = fp_div(fp_mul(angle, fp_pi()), fp_from_float(180));

        /* 将射线变换到 OBB 局部坐标系 */
        local_origin = vec2f_sub(ray.origin, center);
        local_origin = vec2f_rotate(local_origin, -rotation);
        local_dir = vec2f_rotate(ray.direction, -rotation);

        t_min = fp_zero();
        t_max = fp_max_value();

        /* 局部 X 轴 (local x 对应世界 Y? 注意: slabs 法中 local_dir.x 是水平轴) */
        if (fp_abs(local_dir.x) < EPSILON){
                if (local_origin.x < -half_extents.x
                        || local_origin.x > half_extents.x)
                        return result;
        }
        else{
                inv_dir = fp_div(fp_one(), local_dir.x);
                t1 = fp_mul(fp_sub(-half_extents.x, local_origin.x), inv_dir);
                t2 = fp_mul(fp_sub(half_extents.x, local_origin.x), inv_dir);
                if (t1 > t2) { tmp = t1; t1 = t2; t2 = tmp; }
                t_min = fp_max(t_min, t1);
                t_max = fp_min(t_max, t2);
                if (t_min > t_max)
                        return result;
        }

        /* 局部 Y 轴 */
        if (fp_abs(local_dir.y) < EPSILON){
                if (local_origin.y < -half_extents.y
                        || local_origin.y > half_extents.y)
                        return result;
        }
        else{
                inv_dir = fp_div(fp_one(), local_dir.y);
                t1 = fp_mul(fp_sub(-half_extents.y, local_origin.y), inv_dir);
                t2 = fp_mul(fp_sub(half_extents.y, local_origin.y), inv_dir);
                if (t1 > t2) { tmp = t1; t1 = t2; t2 = tmp; }
                t_min = fp_max(t_min, t1);
                t_max = fp_min(t_max, t2);
                if (t_min > t_max)
                        return result;
        }

        if (t_max < 0)
                return result;

        t = t_min >= 0 ? t_min : t_max;
        if (t < 0)
                return result;

        local_hit.x = fp_add(local_origin.x, fp_mul(local_dir.x, t));
        local_hit.y = fp_add(local_origin.y, fp_mul(local_dir.y, t));

        result.hit = true;
        result.point = vec2f_add(vec2f_rotate(local_hit, rotation), center);
        result.distance = vec2f_length(vec2f_sub(result.point, ray.origin));

        /* 正确计算法线：确定被撞击的 OBB 面，然后旋回世界空间 */
        {
                joy_vec2f_t local_normal = {0, 0};
                if (fp_abs(fp_sub(fp_abs(local_hit.x), half_extents.x)) < fp_mul(half_extents.x, fp_from_float(0.01f))) {
                        local_normal.x = local_hit.x > fp_zero() ? fp_one() : fp_from_float(-1);
                }
                if (fp_abs(fp_sub(fp_abs(local_hit.y), half_extents.y)) < fp_mul(half_extents.y, fp_from_float(0.01f))) {
                        local_normal.y = local_hit.y > fp_zero() ? fp_one() : fp_from_float(-1);
                }
                /* 若同时命中角点，只取一个轴向（离边缘更近的那个） */
                if (local_normal.x != fp_zero() && local_normal.y != fp_zero()) {
                        joy_fp_t dx = fp_abs(fp_sub(fp_abs(local_hit.x), half_extents.x));
                        joy_fp_t dy = fp_abs(fp_sub(fp_abs(local_hit.y), half_extents.y));
                        if (dx < dy) local_normal.y = 0;
                        else         local_normal.x = 0;
                }
                result.normal = vec2f_rotate(local_normal, rotation);
                result.normal = vec2f_normalize(result.normal);
        }
        return result;
}

joy_ray2d_collisionf_t
joy_math_collision2df_get_ray_polygon(joy_ray2df_t ray, joy_polygonf_t polygon)
{
        joy_ray2d_collisionf_t result = {0};
        joy_vec2f_t edge_start, edge_end, edge_vec, diff;
        joy_fp_t denom, t, u;
        joy_fp_t closest_t = fp_max_value();
        joy_vec2f_t hit_edge = {0, 0};

        if (polygon.num_vertices < 3)
                return result;

        /* 1. Normalize ray direction */
        ray.direction = vec2f_normalize(ray.direction);

        /* 2. Test each edge for intersection */
        for (int i = 0; i < polygon.num_vertices; i++) {
                edge_start = polygon.vertices[i];
                edge_end = polygon.vertices[(i + 1) % polygon.num_vertices];
                edge_vec = vec2f_sub(edge_end, edge_start);

                /* Cross product: if zero, ray is parallel to this edge */
                denom = vec2f_cross(ray.direction, edge_vec);
                if (denom == fp_zero())
                        continue;

                diff = vec2f_sub(edge_start, ray.origin);
                t = fp_div(vec2f_cross(diff, edge_vec), denom);
                u = fp_div(vec2f_cross(diff, ray.direction), denom);

                /* Valid intersection: ray forward (t>=0), on segment (0<=u<=1) */
                if (t >= fp_zero() && u >= fp_zero() && u <= fp_one() && t < closest_t) {
                        closest_t = t;
                        hit_edge = edge_vec;
                }
        }

        /* 3. Build result if hit found */
        if (closest_t < fp_max_value()) {
                result.hit = true;
                result.distance = closest_t;
                result.point = vec2f_add(ray.origin,
                        vec2f_scale(ray.direction, closest_t));
                /* Normal: perpendicular to edge, always faces against ray */
                result.normal = vec2f_normal(hit_edge);
                if (vec2f_dot(result.normal, ray.direction) > fp_zero()) {
                        result.normal = vec2f_negate(result.normal);
                }
        }

        return result;
}

bool
joy_math_collision2df_check_point_rectangle(joy_vec2f_t point, joy_rectanglef_t rect)
{
        return (point.x >= rect.x && point.x <= fp_add(rect.x, rect.width)
             && point.y >= rect.y && point.y <= fp_add(rect.y, rect.height));
}

bool
joy_math_collision2df_check_point_circle(joy_vec2f_t point, joy_circlef_t circle)
{
        joy_vec2f_t diff = vec2f_sub(point, circle.center);
        return vec2f_length_squared(diff) <= fp_pow2(circle.radius);
}

joy_fp_t
joy_math_collision2df_get_squared_distance_point_circle(joy_vec2f_t point, joy_circlef_t circle)
{
        joy_vec2f_t diff = vec2f_sub(point, circle.center);
        return vec2f_length_squared(diff);
}

bool
joy_math_collision2df_check_point_triangle(joy_vec2f_t point, joy_vec2f_t p1, joy_vec2f_t p2, joy_vec2f_t p3)
{
        /* Barycentric coordinate method */
        joy_vec2f_t v0 = vec2f_sub(p3, p1);
        joy_vec2f_t v1 = vec2f_sub(p2, p1);
        joy_vec2f_t v2 = vec2f_sub(point, p1);

        joy_fp_t dot00 = vec2f_dot(v0, v0);
        joy_fp_t dot01 = vec2f_dot(v0, v1);
        joy_fp_t dot02 = vec2f_dot(v0, v2);
        joy_fp_t dot11 = vec2f_dot(v1, v1);
        joy_fp_t dot12 = vec2f_dot(v1, v2);

        joy_fp_t denom = fp_sub(fp_mul(dot00, dot11), fp_mul(dot01, dot01));
        if (denom == fp_zero())
                return false;

        joy_fp_t inv_denom = fp_div(fp_one(), denom);
        joy_fp_t u = fp_mul(fp_sub(fp_mul(dot11, dot02), fp_mul(dot01, dot12)), inv_denom);
        joy_fp_t v = fp_mul(fp_sub(fp_mul(dot00, dot12), fp_mul(dot01, dot02)), inv_denom);

        return (u >= fp_zero() && v >= fp_zero()
             && fp_add(u, v) <= fp_one());
}

bool
joy_math_collision2df_check_lines(joy_vec2f_t a1, joy_vec2f_t b1, joy_vec2f_t a2, joy_vec2f_t b2, joy_vec2f_t*cp)
{
        joy_vec2f_t r = vec2f_sub(b1, a1);
        joy_vec2f_t s = vec2f_sub(b2, a2);
        joy_fp_t denom = vec2f_cross(r, s);

        if (denom == fp_zero())
                return false; /* parallel */

        joy_vec2f_t diff = vec2f_sub(a2, a1);
        joy_fp_t t = fp_div(vec2f_cross(diff, s), denom);
        joy_fp_t u = fp_div(vec2f_cross(diff, r), denom);

        if (t >= fp_zero() && t <= fp_one()
         && u >= fp_zero() && u <= fp_one()) {
                if (cp) {
                        cp->x = fp_add(a1.x, fp_mul(r.x, t));
                        cp->y = fp_add(a1.y, fp_mul(r.y, t));
                }
                return true;
        }
        return false;
}

bool
joy_math_collision2df_check_point_line(joy_vec2f_t point, joy_vec2f_t p1, joy_vec2f_t p2, joy_fp_t threshold)
{
        joy_vec2f_t ab = vec2f_sub(p2, p1);
        joy_vec2f_t ap = vec2f_sub(point, p1);
        joy_fp_t ab_len_sq = vec2f_length_squared(ab);

        if (ab_len_sq == fp_zero())
                return vec2f_length_squared(ap) <= fp_pow2(threshold);

        joy_fp_t proj = fp_div(vec2f_dot(ap, ab), ab_len_sq);
        proj = fp_clamp(proj, fp_zero(), fp_one());

        joy_vec2f_t closest = vec2f_add(p1, vec2f_scale(ab, proj));
        joy_vec2f_t diff = vec2f_sub(point, closest);

        return vec2f_length_squared(diff) <= fp_pow2(threshold);
}

bool
joy_math_collision2df_check_circles(joy_circlef_t a, joy_circlef_t b)
{
        joy_vec2f_t distance;
        joy_fp_t distance_squared, radius_sum;

        distance = vec2f_sub(b.center, a.center);
        distance_squared = vec2f_length_squared(distance);
        radius_sum = fp_add(b.radius, a.radius);

        return distance_squared <= fp_pow2(radius_sum);
}

/* aabb */
bool
joy_math_collision2df_check_rectangles(joy_rectanglef_t a, joy_rectanglef_t b)
{
        if (a.x > fp_add(b.x, b.width))
                return false;
        if (fp_add(a.x, a.width) < b.x)
                return false;
        if (a.y > fp_add(b.y, b.height))
                return false;
        if (fp_add(a.y, a.height) < b.y)
                return false;
        return true;
}

/* ---- 瓦片地图扫掠碰撞（定点数，帧同步确定）----
 * 与「先推进到底、再按最小穿透方向推出」的根本区别：
 * 扫掠先算出「沿运动方向会撞到哪一格」，只推进到接触点，全程不产生穿透，
 * 任意速度都不会穿墙/穿平台。格边界是整数且轴对齐，故退化为整数行/列扫描。 */

static int
joy_math_tilemap_floor_to_int(joy_fp_t x)
{
        return (int)(x >> JOY_FP_FRACTIONAL_PLACES);
}

static bool
joy_math_tilemap_row_blocked(int r, int c0, int c1, joy_tilemap_solid_fn solid)
{
        for (int c = c0; c <= c1; c++)
                if (solid(c, r)) return true;
        return false;
}

static bool
joy_math_tilemap_col_blocked(int c, int r0, int r1, joy_tilemap_solid_fn solid)
{
        for (int r = r0; r <= r1; r++)
                if (solid(c, r)) return true;
        return false;
}

bool
joy_math_tilemap_sweep_x(joy_fp_t* px, joy_fp_t* py, joy_fp_t dx, joy_fp_t w, joy_fp_t h, joy_tilemap_solid_fn solid)
{
        if (dx == fp_zero()) return false;
        const joy_fp_t EPS = fp_from_float(0.001f);

        /* X 扫掠期间行跨度保持不变 */
        const int ry0 = joy_math_tilemap_floor_to_int(*py);
        const int ry1 = joy_math_tilemap_floor_to_int(fp_sub(fp_add(*py, h), EPS));

        if (dx > fp_zero()) {                                   /* 向右：右缘向右跨列 */
                const joy_fp_t r_old = fp_add(*px, w);
                const int c0 = joy_math_tilemap_floor_to_int(fp_sub(r_old, EPS)) + 1;
                const int c1 = joy_math_tilemap_floor_to_int(fp_sub(fp_add(r_old, dx), EPS));
                for (int c = c0; c <= c1; c++) {                /* 由近及远，首个阻挡即最先接触 */
                        if (joy_math_tilemap_col_blocked(c, ry0, ry1, solid)) {
                                *px = fp_sub(fp_from_int(c), w); /* 贴到该列左边界 */
                                return true;
                        }
                }
        } else {                                                /* 向左：左缘向左跨列 */
                const int c0 = joy_math_tilemap_floor_to_int(*px) - 1;
                const int c1 = joy_math_tilemap_floor_to_int(fp_add(*px, dx));
                for (int c = c0; c >= c1; c--) {
                        if (joy_math_tilemap_col_blocked(c, ry0, ry1, solid)) {
                                *px = fp_from_int(c + 1);        /* 贴到该列右边界 */
                                return true;
                        }
                }
        }
        *px = fp_add(*px, dx);                                  /* 无阻挡，完整推进 */
        return false;
}

bool
joy_math_tilemap_sweep_y(joy_fp_t* px, joy_fp_t* py, joy_fp_t* vy, joy_fp_t tick, joy_fp_t w, joy_fp_t h, joy_tilemap_solid_fn solid)
{
        const joy_fp_t dy = fp_mul(*vy, tick);
        if (dy == fp_zero()) return false;
        const joy_fp_t EPS = fp_from_float(0.001f);

        /* Y 扫掠期间列跨度保持不变 */
        const int cx0 = joy_math_tilemap_floor_to_int(*px);
        const int cx1 = joy_math_tilemap_floor_to_int(fp_sub(fp_add(*px, w), EPS));

        if (dy > fp_zero()) {                                   /* 下落：底边向下跨行 */
                const joy_fp_t b_old = fp_add(*py, h);
                const int r0 = joy_math_tilemap_floor_to_int(fp_sub(b_old, EPS)) + 1;
                const int r1 = joy_math_tilemap_floor_to_int(fp_sub(fp_add(b_old, dy), EPS));
                for (int r = r0; r <= r1; r++) {                /* 自上而下，首个阻挡即最先接触 */
                        if (joy_math_tilemap_row_blocked(r, cx0, cx1, solid)) {
                                *py = fp_sub(fp_from_int(r), h); /* 贴到该行顶边（落地） */
                                *vy = fp_zero();
                                return true;
                        }
                }
        } else {                                                /* 上升：顶边向上跨行 */
                const int r0 = joy_math_tilemap_floor_to_int(*py) - 1;
                const int r1 = joy_math_tilemap_floor_to_int(fp_add(*py, dy));
                for (int r = r0; r >= r1; r--) {
                        if (joy_math_tilemap_row_blocked(r, cx0, cx1, solid)) {
                                *py = fp_from_int(r + 1);        /* 贴到该行底边（撞顶） */
                                *vy = fp_zero();
                                return true;
                        }
                }
        }
        *py = fp_add(*py, dy);                                  /* 无阻挡，完整推进 */
        return false;
}

/* SAT-based polygon-polygon overlap test */
bool
joy_math_collision2df_check_polygons(joy_polygonf_t a, joy_polygonf_t b)
{
        joy_vec2f_t axis, point;
        joy_fp_t ab_sep = joy_math_find_min_separation(a, b, &axis, &point);
        if (ab_sep >= fp_zero())
                return false;
        joy_fp_t ba_sep = joy_math_find_min_separation(b, a, &axis, &point);
        if (ba_sep >= fp_zero())
                return false;
        return true;
}

/* OBB 沿 (dx,dy) 平移的扫掠凸包（Andrew monotone chain，8 点）。
 * 语义/绕序约定见 joy.h 中 joy_math_collision2df_swept_obb 的注释。 */
void
joy_math_collision2df_swept_obb(joy_vec2f_t center, joy_vec2f_t axis,
        joy_fp_t half_long, joy_fp_t half_short, joy_fp_t dx, joy_fp_t dy,
        joy_vec2f_t* out, int* out_count)
{
        if (!out || !out_count) return;
        *out_count = 0;

        const joy_fp_t one = fp_one();
        const joy_fp_t neg = fp_sub(fp_zero(), one);

        /* 长轴分量 pl，短轴分量 ps（axis 逆时针 90° = (-ay, ax)） */
        joy_vec2f_t pl, ps;
        pl.x = fp_mul(axis.x, half_long);
        pl.y = fp_mul(axis.y, half_long);
        ps.x = fp_mul(fp_sub(fp_zero(), axis.y), half_short);
        ps.y = fp_mul(axis.x, half_short);

        /* 起点盒 4 顶点 + 终点盒 4 顶点（终点 = 起点 + 位移） */
        const joy_fp_t sx[4] = { one, neg, neg, one };
        const joy_fp_t sy[4] = { one, one, neg, neg };
        joy_vec2f_t p[8];
        for (int i = 0; i < 4; i++) {
                p[i].x = fp_add(fp_add(center.x, fp_mul(sx[i], pl.x)), fp_mul(sy[i], ps.x));
                p[i].y = fp_add(fp_add(center.y, fp_mul(sx[i], pl.y)), fp_mul(sy[i], ps.y));
                p[i + 4].x = fp_add(p[i].x, dx);
                p[i + 4].y = fp_add(p[i].y, dy);
        }

        /* 按 x、再按 y 排序（n=8，插入排序足够） */
        for (int i = 1; i < 8; i++) {
                joy_vec2f_t key = p[i];
                int j = i - 1;
                while (j >= 0 && (p[j].x > key.x
                        || (p[j].x == key.x && p[j].y > key.y))) {
                        p[j + 1] = p[j];
                        j--;
                }
                p[j + 1] = key;
        }

        joy_vec2f_t h[8];
        int n = 0;

        /* 下凸壳 */
        for (int i = 0; i < 8; i++) {
                while (n >= 2) {
                        joy_vec2f_t u = vec2f_sub(h[n - 1], h[n - 2]);
                        joy_vec2f_t v = vec2f_sub(p[i], h[n - 2]);
                        if (vec2f_cross(u, v) > fp_zero()) break;
                        n--;
                }
                h[n++] = p[i];
        }
        /* 上凸壳 */
        const int lower = n + 1;
        for (int i = 6; i >= 0; i--) {
                while (n >= lower) {
                        joy_vec2f_t u = vec2f_sub(h[n - 1], h[n - 2]);
                        joy_vec2f_t v = vec2f_sub(p[i], h[n - 2]);
                        if (vec2f_cross(u, v) > fp_zero()) break;
                        n--;
                }
                h[n++] = p[i];
        }
        if (n > 1) n--;                 /* 末点与起点重合，去掉 */
        if (n > 8) n = 8;

        /* 统一绕序为「y 向下坐标系的顺时针」（有向面积 > 0）：
         * vec2f_normal 对这种绕序给出外法线，joy_math_find_min_separation 依赖它。 */
        joy_fp_t area = fp_zero();
        for (int i = 0; i < n; i++) {
                joy_vec2f_t a = h[i];
                joy_vec2f_t b = h[(i + 1) % n];
                area = fp_add(area, fp_sub(fp_mul(a.x, b.y), fp_mul(b.x, a.y)));
        }
        if (area < fp_zero()) {
                for (int i = 0; i < n / 2; i++) {
                        joy_vec2f_t t = h[i];
                        h[i] = h[n - 1 - i];
                        h[n - 1 - i] = t;
                }
        }

        for (int i = 0; i < n; i++) out[i] = h[i];
        *out_count = n;
}

/* Circle vs AABB: find closest point on rect to circle center */
bool
joy_math_collision2df_check_circle_rectangle(joy_circlef_t a, joy_rectanglef_t b)
{
        joy_vec2f_t closest;
        closest.x = fp_clamp(a.center.x, b.x, fp_add(b.x, b.width));
        closest.y = fp_clamp(a.center.y, b.y, fp_add(b.y, b.height));

        joy_vec2f_t diff = vec2f_sub(a.center, closest);
        return vec2f_length_squared(diff) <= fp_pow2(a.radius);
}

/* Circle vs convex polygon */
bool
joy_math_collision2df_check_circle_polygon(joy_circlef_t a, joy_polygonf_t b)
{
        if (b.num_vertices < 3)
                return false;

        /* 1. Check distance from center to each edge */
        for (int i = 0; i < b.num_vertices; i++) {
                joy_vec2f_t e1 = b.vertices[i];
                joy_vec2f_t e2 = b.vertices[(i + 1) % b.num_vertices];
                joy_vec2f_t edge = vec2f_sub(e2, e1);
                joy_vec2f_t to_center = vec2f_sub(a.center, e1);

                joy_fp_t edge_len_sq = vec2f_length_squared(edge);
                if (edge_len_sq == fp_zero())
                        continue;

                joy_fp_t t = fp_div(vec2f_dot(to_center, edge), edge_len_sq);
                t = fp_clamp(t, fp_zero(), fp_one());

                joy_vec2f_t closest = vec2f_add(e1, vec2f_scale(edge, t));
                joy_vec2f_t diff = vec2f_sub(a.center, closest);

                if (vec2f_length_squared(diff) <= fp_pow2(a.radius))
                        return true;
        }

        /* 3. Point-in-polygon test for circle center */
        {
                bool inside = true;
                for (int i = 0; i < b.num_vertices; i++) {
                        joy_vec2f_t e1 = b.vertices[i];
                        joy_vec2f_t e2 = b.vertices[(i + 1) % b.num_vertices];
                        joy_vec2f_t edge = vec2f_sub(e2, e1);
                        joy_vec2f_t normal = vec2f_normal(edge); /* inward normal */
                        joy_vec2f_t to_center = vec2f_sub(a.center, e1);
                        if (vec2f_dot(to_center, normal) < fp_zero()) {
                                inside = false;
                                break;
                        }
                }
                if (inside)
                        return true;
        }

        return false;
}

int
joy_math_collision2df_get_circles(joy_circlef_t a, joy_circlef_t b, joy_contact2df_t *contact)
{
        joy_vec2f_t distance;
        joy_fp_t distance_squared, radius_sum;

        distance = vec2f_sub(b.center, a.center);
        distance_squared = vec2f_length_squared(distance);
        radius_sum = fp_add(b.radius, a.radius);
        if (distance_squared > fp_pow2(radius_sum))
                return 0;

        contact->normal = vec2f_normalize(distance);
        contact->sp = vec2f_sub(b.center, vec2f_scale(contact->normal, b.radius));
        contact->ep = vec2f_add(a.center, vec2f_scale(contact->normal, a.radius));
        contact->depth = vec2f_length(vec2f_sub(contact->ep, contact->sp));

        return 1;
}

int
joy_math_collision2df_get_rectangles(joy_rectanglef_t a, joy_fp_t a_angle,
        joy_rectanglef_t b, joy_fp_t b_angle,
	joy_contact2df_t *contact)
{
        /*
        Setup
	取 A B 的半径: bounds 中存储的是边长(宽高)
	再取边长的一半, 即半宽/半高(半径)
	把矩形放到以中心为(0,0)的局部坐标系第一象限的顶点
	那么 a_h / b_h 就是右上角第一象限的点
        */
        joy_vec2f_t a_bounds, b_bounds;
        joy_vec2f_t a_h, b_h, a_pos, b_pos;
        joy_mat22f_t a_rot, b_rot, a_rotT, b_rotT;
        joy_vec2f_t dp, a_d, b_d;
        joy_mat22f_t c, abs_c, abs_ct;
        joy_vec2f_t abs_a_d, abs_b_d;
        joy_vec2f_t a_face, b_face;
	joy_vec2f_t normal;
	joy_fp_t separation, r_separation, a_res, b_res;

	const joy_fp_t relative_tol = JOY_FPFF(0.95f);
	const joy_fp_t absolute_tol = JOY_FPFF(0.01f);

        a_bounds.x = a.width; a_bounds.y = a.height;
        b_bounds.x = b.width; b_bounds.y = b.height;

	a_h = vec2f_scale(a_bounds, fp_half());
	b_h = vec2f_scale(b_bounds, fp_half());

        /*
	把 A B 的左上角坐标换算成中心坐标, 这是全局坐标
	position 存储的是实际参与碰撞检测的实体位置
	既不是完整屏幕坐标, 而是实际的碰撞体中心
        */
        a_pos.x = fp_add(a.x, a_h.x);
        a_pos.y = fp_add(a.y, a_h.y);
        b_pos.x = fp_add(b.x, b_h.x);
        b_pos.y = fp_add(b.y, b_h.y);

        /*
	取 A B 各自的旋转角(度), 转成弧度并构造 2x2 旋转矩阵
	因为二维空间中的旋转矩阵是 2x2 的
        */
        a_res = fp_div(fp_mul(a_angle, fp_pi()), fp_from_float(180));
        b_res = fp_div(fp_mul(b_angle, fp_pi()), fp_from_float(180));
	a_rot = mat22f_rotate(a_res);
	b_rot = mat22f_rotate(b_res);

        /*
	再取旋转矩阵的转置(正交矩阵的逆)
	a_rot 将 A 从自身局部坐标系变换到世界坐标
	a_rotT 将世界坐标变换回 A 的局部系; 最终效果是把
	A 中的向量变换到 B 的坐标系中
        */
	a_rotT = mat22f_transpose(a_rot);
	b_rotT = mat22f_transpose(b_rot);


	/* 计算 A 到 B 的中心距离向量 dp, 即两盒中心之间的连线 */
	dp = vec2f_sub(b_pos, a_pos);

        /*
	把中心距离向量分别乘以各自的转置矩阵
	将 dp 变换到各自的局部坐标系中
        */
	a_d = mat22f_mul_vec2f(a_rotT, dp);
	b_d = mat22f_mul_vec2f(b_rotT, dp);

	c = mat22f_mul(a_rotT, b_rot);
	abs_c = mat22f_abs(c);
	abs_ct = mat22f_transpose(abs_c);

        /*
        Box A faces
	a_d 是 A 局部系中指向 B 的距离向量, abs_a_d 只取其绝对值
	分离判据: a_face = Abs(a_d) - (a_h + abs_c * b_h)
	a_h 是 A 的半宽, abs_c * b_h 是 B 在 A 局部系下各轴上的投影半径
	x/y 轴分别计算: a_d、a_h、b_h 均从(0,0)出发, 比较各轴的投影长度
	即投影长度
	Abs(a_d) - (a_h + abs_c * b_h) 这个式子(以 x 轴为例)
	就是 a_d 在 x 轴上的分量减去两盒在 x 轴上的投影长度, 记为 s
	a_h 是 A 在 x 轴上的半投影 a, abs_c*b_h 是 B 在 x 轴上的半投影 b
	若 s > 0 说明两盒在该轴上的投影不相交(分离)
        */
        abs_a_d = vec2f_abs(a_d);
	a_face = vec2f_sub(vec2f_sub(abs_a_d, a_h), mat22f_mul_vec2f(abs_c, b_h));
	if (a_face.x > fp_zero() || a_face.y > fp_zero())
		return 0;

	/*Box B faces*/
	abs_b_d = vec2f_abs(b_d);
	b_face = vec2f_sub(vec2f_sub(abs_b_d, mat22f_mul_vec2f(abs_ct, a_h)), b_h);
	if (b_face.x > fp_zero() || b_face.y > fp_zero())
		return 0;

	/*
        Find best axis
        Box A faces
        */
        separation = a_face.x;
        r_separation = fp_mul(relative_tol, separation);

	/* 若 B 在 A 右侧, 分离法线取 A 的 +X 轴; 否则取 -X 轴 */
        normal = a_d.x > fp_zero() ? a_rot.col1 : vec2f_negate(a_rot.col1);

	if (a_face.y > fp_add(r_separation, fp_mul(absolute_tol, a_h.y))){
		separation = a_face.y;
		normal = a_d.y > fp_zero() ? a_rot.col2 : vec2f_negate(a_rot.col2);
	}

	/*Box B faces*/
	if (b_face.x > fp_add(r_separation, fp_mul(absolute_tol, b_h.x))) {
		separation = b_face.x;
		normal = b_d.x > fp_zero() ? b_rot.col1 : vec2f_negate(b_rot.col1);
	}

	if (b_face.y > fp_add(r_separation, fp_mul(absolute_tol, b_h.y))) {
		separation = b_face.y;
		normal = b_d.y > fp_zero() ? b_rot.col2 : vec2f_negate(b_rot.col2);
	}

        contact->depth = separation;
        contact->normal = normal;

        return 1;
}


static joy_vec2f_t edge_at(joy_polygonf_t poly, int index)
{
        joy_vec2f_t next_vertex, curr_vertex;
        curr_vertex = poly.vertices[index];
        next_vertex = poly.vertices[(index + 1) % poly.num_vertices];
        return vec2f_sub(next_vertex, curr_vertex);
}

joy_fp_t
joy_math_find_min_separation(joy_polygonf_t a, joy_polygonf_t b, joy_vec2f_t *axis, joy_vec2f_t *point)
{
        joy_fp_t separation, min_sep, proj;
        joy_vec2f_t va, vb, normal, min_vertex;
        separation = fp_min_value();
        for (int i = 0; i < a.num_vertices; i++) {
                va = a.vertices[i];
                normal = vec2f_normal(edge_at(a, i));
                min_sep = fp_max_value();
                for (int j = 0; j < b.num_vertices; j++) {
                        vb = b.vertices[j];
                        proj = vec2f_dot(vec2f_sub(vb, va), normal);
                        if (proj < min_sep) {
                                min_sep = proj;
                                min_vertex = vb;
                        }
                }
                if (min_sep > separation) {
                        separation = min_sep;
                        *axis = edge_at(a, i);
                        *point = min_vertex;
                }
        }
        return separation;
}

int
joy_math_collision2df_get_polygons(joy_polygonf_t a, joy_polygonf_t b, joy_contact2df_t *contact)
{
        joy_vec2f_t a_axis, b_axis, a_point, b_point;
        joy_vec2f_t distance;
        joy_fp_t ab_sep, ba_sep;
        ab_sep = joy_math_find_min_separation(a, b, &a_axis, &a_point);
        if (ab_sep >= 0)
                return 0;
        ba_sep = joy_math_find_min_separation(b, a, &b_axis, &b_point);
        if (ba_sep >= 0)
                return 0;
        if (ab_sep > ba_sep) {
                contact->depth = -ab_sep;
                contact->normal = vec2f_normal(a_axis);
                contact->sp = a_point;
                distance = vec2f_scale(contact->normal, contact->depth);
                contact->ep = vec2f_add(a_point, distance);
        }
        else {
                contact->depth = -ba_sep;
                contact->normal = vec2f_negate(vec2f_normal(b_axis));
                distance = vec2f_scale(contact->normal, contact->depth);
                contact->sp = vec2f_sub(b_point, distance);
                contact->ep = b_point;
        }
        return 1;
}


joy_ray3d_collisionf_t
joy_math_collision3df_get_ray_triangle(joy_ray3df_t ray, joy_vec3f_t p1, joy_vec3f_t p2, joy_vec3f_t p3)
{
        const joy_fp_t EPSILON = fp_from_float(0.000001f);
        joy_ray3d_collisionf_t result = {0};
        joy_vec3f_t edge1, edge2;
        joy_vec3f_t p, q, tv;
        joy_fp_t det, inv_det, u, v, t;

        ray.direction = vec3f_normalize(ray.direction);

        /* Find vectors for two edges sharing V1 */
        edge1 = vec3f_sub(p2, p1);
        edge2 = vec3f_sub(p3, p1);

        /* Begin calculating determinant - also used to calculate u parameter */
        p = vec3f_cross(ray.direction, edge2);

        det = vec3f_dot(edge1, p);

        if ((det > -EPSILON) && (det < EPSILON))
                return result;

        inv_det = fp_div(fp_one(), det);

        /* Calculate distance from V1 to ray origin */
        tv = vec3f_sub(ray.position, p1);

        /* Calculate u parameter and test bound */
        u = fp_mul(vec3f_dot(tv, p), inv_det);

        /* The intersection lies outside the triangle */
        if ((u < fp_zero()) || (u > fp_one()))
                return result;

        /* Prepare to test v parameter */
        q = vec3f_cross(tv, edge1);

        /* Calculate V parameter and test bound */
        v = fp_mul(vec3f_dot(ray.direction, q), inv_det);

        /* The intersection lies outside the triangle */
        if ((v < fp_zero()) || (fp_add(u, v) > fp_one()))
                return result;

        t = fp_mul(vec3f_dot(edge2, q), inv_det);

        if (t > EPSILON)
        {
                /* Ray hit, get hit point and normal */
                result.hit = true;
                result.distance = t;
                result.normal = vec3f_normalize(vec3f_cross(edge1, edge2));
                result.point = vec3f_add(ray.position, vec3f_scale(ray.direction, t));
        }

        return result;
}

joy_ray3d_collisionf_t
joy_math_collision3df_get_ray_quad(joy_ray3df_t ray, joy_vec3f_t p1, joy_vec3f_t p2, joy_vec3f_t p3, joy_vec3f_t p4)
{
        joy_ray3d_collisionf_t result;
        result = joy_math_collision3df_get_ray_triangle(ray, p1, p2, p4);
        if (!result.hit)
                result = joy_math_collision3df_get_ray_triangle(ray, p2, p3, p4);
        return result;
}

joy_ray3d_collisionf_t
joy_math_collision3df_get_ray_sphere(joy_ray3df_t ray, joy_spheref_t sphere)
{
        joy_ray3d_collisionf_t result = {0};
        joy_vec3f_t ray_sphere_pos = vec3f_sub(ray.position, sphere.position);
        joy_fp_t dist_sq = vec3f_square_length(ray_sphere_pos);
        joy_fp_t a = vec3f_dot(ray_sphere_pos, ray.direction);
        joy_fp_t b = fp_sub(dist_sq, fp_pow2(sphere.radius));
        joy_fp_t c = fp_sub(fp_pow2(a), b);

        /* 判别式 < 0: 无交点 */
        if (c < fp_zero())
                return result;

        joy_fp_t d = fp_sqrt(c);
        joy_fp_t t = fp_sub(-a, d);
        /* 若第一个交点在射线后方, 取第二个 */
        if (t < fp_zero())
                t = fp_add(-a, d);
        if (t >= fp_zero()) {
                result.hit = true;
                result.distance = t;
                result.point = vec3f_add(ray.position,
                        vec3f_scale(ray.direction, t));
                result.normal = vec3f_normalize(
                        vec3f_sub(result.point, sphere.position));
        }

        return result;
}

joy_ray3d_collisionf_t
joy_math_collision3df_get_ray_box(joy_ray3df_t ray, joy_bounding_boxf_t box)
{
        /* Slabs method (ray-AABB) */
        joy_ray3d_collisionf_t result = {0};
        joy_vec3f_t dirfrac;
        joy_fp_t t1, t2, tmin, tmax;
        const joy_fp_t EPSILON = fp_from_float(0.00001f);

        ray.direction = vec3f_normalize(ray.direction);

        dirfrac.x = fp_div(fp_one(), ray.direction.x);
        dirfrac.y = fp_div(fp_one(), ray.direction.y);
        dirfrac.z = fp_div(fp_one(), ray.direction.z);

        /* X slab */
        t1 = fp_mul(fp_sub(box.min.x, ray.position.x), dirfrac.x);
        t2 = fp_mul(fp_sub(box.max.x, ray.position.x), dirfrac.x);
        if (t1 > t2) { SWAP(t1, t2); }
        tmin = t1; tmax = t2;

        /* Y slab */
        t1 = fp_mul(fp_sub(box.min.y, ray.position.y), dirfrac.y);
        t2 = fp_mul(fp_sub(box.max.y, ray.position.y), dirfrac.y);
        if (t1 > t2) { SWAP(t1, t2); }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return result;

        /* Z slab */
        t1 = fp_mul(fp_sub(box.min.z, ray.position.z), dirfrac.z);
        t2 = fp_mul(fp_sub(box.max.z, ray.position.z), dirfrac.z);
        if (t1 > t2) { SWAP(t1, t2); }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return result;

        if (tmax < fp_zero())
                return result;

        joy_fp_t t = tmin >= fp_zero() ? tmin : tmax;
        if (t < EPSILON)
                return result;

        result.hit = true;
        result.distance = t;
        result.point = vec3f_add(ray.position,
                vec3f_scale(ray.direction, t));

        /* Compute normal from which slab was hit */
        joy_vec3f_t center = vec3f_scale(vec3f_add(box.min, box.max), fp_half());
        joy_vec3f_t hit_to_center = vec3f_sub(center, result.point);
        joy_vec3f_t half = vec3f_scale(vec3f_sub(box.max, box.min), fp_half());
        joy_vec3f_t abs_hit = vec3f_abs(hit_to_center);

        result.normal.x = (fp_abs(fp_sub(abs_hit.x, half.x)) < EPSILON)
                ? (hit_to_center.x > fp_zero() ? fp_one() : fp_from_float(-1))
                : fp_zero();
        result.normal.y = (fp_abs(fp_sub(abs_hit.y, half.y)) < EPSILON)
                ? (hit_to_center.y > fp_zero() ? fp_one() : fp_from_float(-1))
                : fp_zero();
        result.normal.z = (fp_abs(fp_sub(abs_hit.z, half.z)) < EPSILON)
                ? (hit_to_center.z > fp_zero() ? fp_one() : fp_from_float(-1))
                : fp_zero();
        result.normal = vec3f_normalize(result.normal);

        return result;
}

/* 列主序 joy_mat44f_t 对方向向量做旋转(忽略平移分量) —— 与 jgjk.c 内 OBB 辅助一致 */
static joy_vec3f_t jcoll_obb_mat44f_rotate_dir(joy_mat44f_t m, joy_vec3f_t v) {
        joy_vec3f_t r;
        r.x = fp_add(fp_add(fp_mul(m.m0, v.x), fp_mul(m.m4, v.y)), fp_mul(m.m8,  v.z));
        r.y = fp_add(fp_add(fp_mul(m.m1, v.x), fp_mul(m.m5, v.y)), fp_mul(m.m9,  v.z));
        r.z = fp_add(fp_add(fp_mul(m.m2, v.x), fp_mul(m.m6, v.y)), fp_mul(m.m10, v.z));
        return r;
}

/* 列主序 joy_mat44f_t 的转置对方向向量做旋转(即逆旋转，旋转矩阵正交) */
static joy_vec3f_t jcoll_obb_mat44f_rotate_dir_t(joy_mat44f_t m, joy_vec3f_t v) {
        joy_vec3f_t r;
        r.x = fp_add(fp_add(fp_mul(m.m0, v.x), fp_mul(m.m1, v.y)), fp_mul(m.m2,  v.z));
        r.y = fp_add(fp_add(fp_mul(m.m4, v.x), fp_mul(m.m5, v.y)), fp_mul(m.m6,  v.z));
        r.z = fp_add(fp_add(fp_mul(m.m8, v.x), fp_mul(m.m9, v.y)), fp_mul(m.m10, v.z));
        return r;
}

/* 欧拉角(弧度) -> 组合旋转矩阵, 顺序: R = Rz * Ry * Rx（与 jgjk.c euler_to_rot 一致） */
static joy_mat44f_t jcoll_obb_euler_to_rot(joy_vec3f_t euler) {
        joy_mat44f_t rx = mat44f_rotate_x(euler.x);
        joy_mat44f_t ry = mat44f_rotate_y(euler.y);
        joy_mat44f_t rz = mat44f_rotate_z(euler.z);
        joy_mat44f_t tmp = mat44f_mul(ry, rx);
        return mat44f_mul(rz, tmp);
}

/* OBB(旋转盒)射线检测: 把射线变换到 OBB 局部坐标系(用旋转矩阵的转置做逆旋转),
 * 在局部系内做 slab(AABB) 求交, 再把命中点与法线旋回世界空间。
 * 全程定点(joy_fp_t), 帧同步确定。 */
joy_ray3d_collisionf_t
joy_math_collision3df_get_ray_obb(joy_ray3df_t ray, joy_vec3f_t position, joy_vec3f_t half_size, joy_vec3f_t rotation)
{
        joy_ray3d_collisionf_t result = {0};
        const joy_fp_t EPSILON = fp_from_float(0.0001f);

        joy_mat44f_t R = jcoll_obb_euler_to_rot(rotation);   /* 世界旋转矩阵 M = R */

        /* 射线原点与方向变换到 OBB 局部系: M^-1 = M^T (正交矩阵) */
        joy_vec3f_t rel = vec3f_sub(ray.position, position);
        joy_vec3f_t local_origin = jcoll_obb_mat44f_rotate_dir_t(R, rel);
        joy_vec3f_t local_dir    = jcoll_obb_mat44f_rotate_dir_t(R, ray.direction);

        /* t_min 初值取最小值: 便于区分"起点在盒内"(t_min<0)和"起点在盒外"(t_min>0) */
        joy_fp_t t_min = fp_min_value();
        joy_fp_t t_max = fp_max_value();
        joy_fp_t t1, t2, tmp, inv_dir;

        /* X slab */
        if (fp_abs(local_dir.x) < EPSILON) {
                if (local_origin.x < fp_sub(fp_zero(), half_size.x)
                        || local_origin.x > half_size.x)
                        return result;
        }
        else {
                inv_dir = fp_div(fp_one(), local_dir.x);
                t1 = fp_mul(fp_sub(fp_sub(fp_zero(), half_size.x), local_origin.x), inv_dir);
                t2 = fp_mul(fp_sub(half_size.x, local_origin.x), inv_dir);
                if (t1 > t2) { SWAP(t1, t2); }
                t_min = fp_max(t_min, t1);
                t_max = fp_min(t_max, t2);
                if (t_min > t_max) return result;
        }

        /* Y slab */
        if (fp_abs(local_dir.y) < EPSILON) {
                if (local_origin.y < fp_sub(fp_zero(), half_size.y)
                        || local_origin.y > half_size.y)
                        return result;
        }
        else {
                inv_dir = fp_div(fp_one(), local_dir.y);
                t1 = fp_mul(fp_sub(fp_sub(fp_zero(), half_size.y), local_origin.y), inv_dir);
                t2 = fp_mul(fp_sub(half_size.y, local_origin.y), inv_dir);
                if (t1 > t2) { SWAP(t1, t2); }
                t_min = fp_max(t_min, t1);
                t_max = fp_min(t_max, t2);
                if (t_min > t_max) return result;
        }

        /* Z slab */
        if (fp_abs(local_dir.z) < EPSILON) {
                if (local_origin.z < fp_sub(fp_zero(), half_size.z)
                        || local_origin.z > half_size.z)
                        return result;
        }
        else {
                inv_dir = fp_div(fp_one(), local_dir.z);
                t1 = fp_mul(fp_sub(fp_sub(fp_zero(), half_size.z), local_origin.z), inv_dir);
                t2 = fp_mul(fp_sub(half_size.z, local_origin.z), inv_dir);
                if (t1 > t2) { SWAP(t1, t2); }
                t_min = fp_max(t_min, t1);
                t_max = fp_min(t_max, t2);
                if (t_min > t_max) return result;
        }

        if (t_max < fp_zero()) return result;          /* 盒子在射线反方向 */

        joy_fp_t t = t_min >= fp_zero() ? t_min : t_max;
        if (t < EPSILON) return result;                 /* 起点恰好贴盒面之类: 容差避免自命中 */

        /* 局部命中点 */
        joy_vec3f_t local_hit;
        local_hit.x = fp_add(local_origin.x, fp_mul(local_dir.x, t));
        local_hit.y = fp_add(local_origin.y, fp_mul(local_dir.y, t));
        local_hit.z = fp_add(local_origin.z, fp_mul(local_dir.z, t));

        /* 局部法线: 判定命中哪一面(slab法) */
        joy_vec3f_t local_normal = { fp_zero(), fp_zero(), fp_zero() };
        if (fp_abs(fp_sub(fp_abs(local_hit.x), half_size.x)) < fp_mul(half_size.x, fp_from_float(0.01f)))
                local_normal.x = local_hit.x > fp_zero() ? fp_one() : fp_from_float(-1);
        if (fp_abs(fp_sub(fp_abs(local_hit.y), half_size.y)) < fp_mul(half_size.y, fp_from_float(0.01f)))
                local_normal.y = local_hit.y > fp_zero() ? fp_one() : fp_from_float(-1);
        if (fp_abs(fp_sub(fp_abs(local_hit.z), half_size.z)) < fp_mul(half_size.z, fp_from_float(0.01f)))
                local_normal.z = local_hit.z > fp_zero() ? fp_one() : fp_from_float(-1);
        /* 若命中两条以上(角/边), 取离面最近的那个轴向 */
        int n_count = (local_normal.x != fp_zero() ? 1 : 0)
                    + (local_normal.y != fp_zero() ? 1 : 0)
                    + (local_normal.z != fp_zero() ? 1 : 0);
        while (n_count > 1) {
                joy_fp_t dm = fp_max_value();
                int drop = -1;
                if (local_normal.x != fp_zero()) {
                        joy_fp_t d = fp_abs(fp_sub(fp_abs(local_hit.x), half_size.x));
                        if (d < dm) { dm = d; drop = 0; }
                }
                if (local_normal.y != fp_zero()) {
                        joy_fp_t d = fp_abs(fp_sub(fp_abs(local_hit.y), half_size.y));
                        if (d < dm) { dm = d; drop = 1; }
                }
                if (local_normal.z != fp_zero()) {
                        joy_fp_t d = fp_abs(fp_sub(fp_abs(local_hit.z), half_size.z));
                        if (d < dm) { dm = d; drop = 2; }
                }
                if (drop == 0) local_normal.x = fp_zero();
                else if (drop == 1) local_normal.y = fp_zero();
                else if (drop == 2) local_normal.z = fp_zero();
                n_count--;
        }

        /* 旋回世界空间: hit point 需要旋转 + 平移, normal 只需旋转 */
        joy_vec3f_t world_hit = jcoll_obb_mat44f_rotate_dir(R, local_hit);
        world_hit = vec3f_add(world_hit, position);

        result.hit = true;
        result.point = world_hit;
        result.normal = vec3f_normalize(jcoll_obb_mat44f_rotate_dir(R, local_normal));
        result.distance = vec3f_length(vec3f_sub(world_hit, ray.position));

        return result;
}

bool
joy_math_collision3df_check_spheres(joy_spheref_t a, joy_spheref_t b)
{
        joy_vec3f_t diff = vec3f_sub(b.position, a.position);
        joy_fp_t dist_sq = vec3f_square_length(diff);
        joy_fp_t radius_sum = fp_add(a.radius, b.radius);
        return dist_sq <= fp_pow2(radius_sum);
}

bool
joy_math_collision3df_check_boxes(joy_bounding_boxf_t a, joy_bounding_boxf_t b)
{
        return (a.min.x <= b.max.x && a.max.x >= b.min.x
             && a.min.y <= b.max.y && a.max.y >= b.min.y
             && a.min.z <= b.max.z && a.max.z >= b.min.z);
}

bool
joy_math_collision3df_check_box_sphere(joy_bounding_boxf_t a, joy_spheref_t b)
{
        joy_vec3f_t closest;
        closest.x = fp_clamp(b.position.x, a.min.x, a.max.x);
        closest.y = fp_clamp(b.position.y, a.min.y, a.max.y);
        closest.z = fp_clamp(b.position.z, a.min.z, a.max.z);

        joy_vec3f_t diff = vec3f_sub(b.position, closest);
        return vec3f_square_length(diff) <= fp_pow2(b.radius);
}

/* =================================================================
 * 浮点版本（joy_ft_t = float）
 * ================================================================= */

joy_ray2d_collision_t
joy_math_collision2d_get_ray_circle(joy_ray2d_t ray, joy_circle_t circle)
{
        joy_vec2_t seg;
        joy_ft_t r_sq, d_sq, seg_len_sq, seg_proj, dx;
        joy_ray2d_collision_t result = {0};
        joy_vec2_t distance;

        r_sq = circle.radius * circle.radius;
        seg = vec2_sub(circle.center, ray.origin);
        seg_len_sq = vec2_length_squared(seg);
        ray.direction = vec2_normalize(ray.direction);
        seg_proj = vec2_dot(seg, ray.direction);

        if (seg_len_sq == r_sq) {
                result.hit = true;
                result.distance = 0.0f;
                result.point = ray.origin;
                result.normal = vec2_normalize(seg);
                return result;
        }
        if (seg_len_sq < r_sq) {
                dx = sqrtf(r_sq - (seg_len_sq - seg_proj * seg_proj));
                distance = vec2_scale(ray.direction, seg_proj + dx);
                result.hit = true;
                result.distance = vec2_length(distance);
                result.point = vec2_add(ray.origin, distance);
                result.normal = vec2_normalize(vec2_sub(result.point, circle.center));
                return result;
        }
        if (seg_proj < 0.0f)
                return result;

        d_sq = seg_len_sq - seg_proj * seg_proj;
        if (d_sq > r_sq)
                return result;
        if (d_sq == r_sq) {
                result.hit = true;
                result.distance = seg_proj;
                result.point = vec2_add(ray.origin, vec2_scale(ray.direction, seg_proj));
                result.normal = vec2_normalize(vec2_sub(result.point, circle.center));
                return result;
        } else {
                dx = sqrtf(r_sq - d_sq);
                result.hit = true;
                result.distance = seg_proj - dx;
                result.point = vec2_add(ray.origin, vec2_scale(ray.direction, result.distance));
                result.normal = vec2_normalize(vec2_sub(result.point, circle.center));
                return result;
        }
}

static joy_vec2_t collision2d_calculate_rect_normal(joy_vec2_t point, joy_vec2_t min, joy_vec2_t max)
{
        joy_vec2_t normal = { 0, 0 };
        const joy_ft_t epsilon = 0.001f;
        if (fabsf(point.x - min.x) < epsilon)
                normal.x = -1.0f;
        else if (fabsf(point.x - max.x) < epsilon)
                normal.x = 1.0f;
        else if (fabsf(point.y - min.y) < epsilon)
                normal.y = -1.0f;
        else if (fabsf(point.y - max.y) < epsilon)
                normal.y = 1.0f;
        return vec2_normalize(normal);
}

#define SWAP_FT(a, b)  do { joy_ft_t tmp = (a); (a) = (b); (b) = tmp; } while(0)

joy_ray2d_collision_t joy_math_collision2d_get_ray_rectangle(joy_ray2d_t ray, joy_rectangle_t rect)
{
        joy_ray2d_collision_t result = { 0 };
        joy_vec2_t min, max;
        joy_ft_t t_min, t_max, inv_dir, t1, t2;

        ray.direction = vec2_normalize(ray.direction);
        min.x = rect.x; min.y = rect.y;
        max.x = rect.x + rect.width; max.y = rect.y + rect.height;
        t_min = 0.0f; t_max = FLT_MAX;

        if (ray.direction.x == 0.0f) {
                if (ray.origin.x < min.x || ray.origin.x > max.x) return result;
        } else {
                inv_dir = 1.0f / ray.direction.x;
                t1 = (min.x - ray.origin.x) * inv_dir;
                t2 = (max.x - ray.origin.x) * inv_dir;
                if (t1 > t2) SWAP_FT(t1, t2);
                t_min = fmaxf(t_min, t1);
                t_max = fminf(t_max, t2);
                if (t_min > t_max) return result;
        }
        if (ray.direction.y == 0.0f) {
                if (ray.origin.y < min.y || ray.origin.y > max.y) return result;
        } else {
                inv_dir = 1.0f / ray.direction.y;
                t1 = (min.y - ray.origin.y) * inv_dir;
                t2 = (max.y - ray.origin.y) * inv_dir;
                if (t1 > t2) SWAP_FT(t1, t2);
                t_min = fmaxf(t_min, t1);
                t_max = fminf(t_max, t2);
                if (t_min > t_max) return result;
        }
        result.hit = true;
        result.distance = t_min;
        joy_vec2_t delta = { t_min * ray.direction.x, t_min * ray.direction.y };
        result.point = vec2_add(ray.origin, delta);
        result.normal = collision2d_calculate_rect_normal(result.point, min, max);
        return result;
}

joy_ray2d_collision_t
joy_math_collision2d_get_ray_rectanglex(joy_ray2d_t ray, joy_rectangle_t rect, joy_ft_t angle)
{
        joy_ray2d_collision_t result = {0};
        joy_vec2_t half_extents, center, local_origin, local_dir;
        joy_ft_t rotation, t_min, t_max, t1, t2, tmp, inv_dir, t;
        joy_vec2_t local_hit;
        const joy_ft_t EPSILON = 0.0001f;

        half_extents.x = rect.width * 0.5f;
        half_extents.y = rect.height * 0.5f;
        center.x = rect.x + half_extents.x;
        center.y = rect.y + half_extents.y;
        rotation = angle * 3.14159265f / 180.0f;

        local_origin = vec2_sub(ray.origin, center);
        local_origin = vec2_rotate(local_origin, -rotation);
        local_dir = vec2_rotate(ray.direction, -rotation);
        t_min = 0.0f; t_max = FLT_MAX;

        if (fabsf(local_dir.x) < EPSILON) {
                if (local_origin.x < -half_extents.x || local_origin.x > half_extents.x) return result;
        } else {
                inv_dir = 1.0f / local_dir.x;
                t1 = (-half_extents.x - local_origin.x) * inv_dir;
                t2 = (half_extents.x - local_origin.x) * inv_dir;
                if (t1 > t2) { tmp = t1; t1 = t2; t2 = tmp; }
                t_min = fmaxf(t_min, t1);
                t_max = fminf(t_max, t2);
                if (t_min > t_max) return result;
        }
        if (fabsf(local_dir.y) < EPSILON) {
                if (local_origin.y < -half_extents.y || local_origin.y > half_extents.y) return result;
        } else {
                inv_dir = 1.0f / local_dir.y;
                t1 = (-half_extents.y - local_origin.y) * inv_dir;
                t2 = (half_extents.y - local_origin.y) * inv_dir;
                if (t1 > t2) { tmp = t1; t1 = t2; t2 = tmp; }
                t_min = fmaxf(t_min, t1);
                t_max = fminf(t_max, t2);
                if (t_min > t_max) return result;
        }
        if (t_max < 0) return result;
        t = t_min >= 0 ? t_min : t_max;
        if (t < 0) return result;

        local_hit.x = local_origin.x + local_dir.x * t;
        local_hit.y = local_origin.y + local_dir.y * t;
        result.hit = true;
        result.point = vec2_add(vec2_rotate(local_hit, rotation), center);
        result.distance = vec2_length(vec2_sub(result.point, ray.origin));
        {
                joy_vec2_t ln = {0, 0};
                joy_ft_t dx = fabsf(fabsf(local_hit.x) - half_extents.x);
                joy_ft_t dy = fabsf(fabsf(local_hit.y) - half_extents.y);
                if (dx < half_extents.x * 0.01f) ln.x = local_hit.x > 0 ? 1.0f : -1.0f;
                if (dy < half_extents.y * 0.01f) ln.y = local_hit.y > 0 ? 1.0f : -1.0f;
                if (ln.x != 0 && ln.y != 0) { if (dx < dy) ln.y = 0; else ln.x = 0; }
                result.normal = vec2_normalize(vec2_rotate(ln, rotation));
        }
        return result;
}

joy_ray2d_collision_t
joy_math_collision2d_get_ray_polygon(joy_ray2d_t ray, joy_polygon_t polygon)
{
        joy_ray2d_collision_t result = {0};
        joy_vec2_t edge_start, edge_end, edge_vec, diff;
        joy_ft_t denom, t, u;
        joy_ft_t closest_t = FLT_MAX;
        joy_vec2_t hit_edge = {0, 0};

        if (polygon.num_vertices < 3) return result;
        ray.direction = vec2_normalize(ray.direction);

        for (int i = 0; i < polygon.num_vertices; i++) {
                edge_start = polygon.vertices[i];
                edge_end = polygon.vertices[(i + 1) % polygon.num_vertices];
                edge_vec = vec2_sub(edge_end, edge_start);
                denom = vec2_cross(ray.direction, edge_vec);
                if (denom == 0.0f) continue;
                diff = vec2_sub(edge_start, ray.origin);
                t = vec2_cross(diff, edge_vec) / denom;
                u = vec2_cross(diff, ray.direction) / denom;
                if (t >= 0.0f && u >= 0.0f && u <= 1.0f && t < closest_t) {
                        closest_t = t;
                        hit_edge = edge_vec;
                }
        }
        if (closest_t < FLT_MAX) {
                result.hit = true;
                result.distance = closest_t;
                result.point = vec2_add(ray.origin, vec2_scale(ray.direction, closest_t));
                result.normal = vec2_normal(hit_edge);
                if (vec2_dot(result.normal, ray.direction) > 0.0f)
                        result.normal = vec2_negate(result.normal);
        }
        return result;
}

bool joy_math_collision2d_check_point_rectangle(joy_vec2_t point, joy_rectangle_t rect)
{
        return point.x >= rect.x && point.x <= rect.x + rect.width
            && point.y >= rect.y && point.y <= rect.y + rect.height;
}

bool joy_math_collision2d_check_point_circle(joy_vec2_t point, joy_circle_t circle)
{
        joy_vec2_t diff = vec2_sub(point, circle.center);
        return vec2_length_squared(diff) <= circle.radius * circle.radius;
}

joy_ft_t joy_math_collision2d_get_squared_distance_point_circle(joy_vec2_t point, joy_circle_t circle)
{
        joy_vec2_t diff = vec2_sub(point, circle.center);
        return vec2_length_squared(diff);
}

bool joy_math_collision2d_check_point_triangle(joy_vec2_t point, joy_vec2_t p1, joy_vec2_t p2, joy_vec2_t p3)
{
        joy_vec2_t v0 = vec2_sub(p3, p1);
        joy_vec2_t v1 = vec2_sub(p2, p1);
        joy_vec2_t v2 = vec2_sub(point, p1);
        joy_ft_t dot00 = vec2_dot(v0, v0);
        joy_ft_t dot01 = vec2_dot(v0, v1);
        joy_ft_t dot02 = vec2_dot(v0, v2);
        joy_ft_t dot11 = vec2_dot(v1, v1);
        joy_ft_t dot12 = vec2_dot(v1, v2);
        joy_ft_t denom = dot00 * dot11 - dot01 * dot01;
        if (denom == 0.0f) return false;
        joy_ft_t inv_denom = 1.0f / denom;
        joy_ft_t u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
        joy_ft_t v = (dot00 * dot12 - dot01 * dot02) * inv_denom;
        return u >= 0 && v >= 0 && u + v <= 1.0f;
}

bool joy_math_collision2d_check_lines(joy_vec2_t a1, joy_vec2_t b1, joy_vec2_t a2, joy_vec2_t b2, joy_vec2_t* cp)
{
        joy_vec2_t r = vec2_sub(b1, a1);
        joy_vec2_t s = vec2_sub(b2, a2);
        joy_ft_t denom = vec2_cross(r, s);
        if (denom == 0.0f) return false;
        joy_vec2_t diff = vec2_sub(a2, a1);
        joy_ft_t t = vec2_cross(diff, s) / denom;
        joy_ft_t u = vec2_cross(diff, r) / denom;
        if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
                if (cp) { cp->x = a1.x + r.x * t; cp->y = a1.y + r.y * t; }
                return true;
        }
        return false;
}

bool joy_math_collision2d_check_point_line(joy_vec2_t point, joy_vec2_t p1, joy_vec2_t p2, joy_ft_t threshold)
{
        joy_vec2_t ab = vec2_sub(p2, p1);
        joy_vec2_t ap = vec2_sub(point, p1);
        joy_ft_t ab_len_sq = vec2_length_squared(ab);
        if (ab_len_sq == 0)
                return vec2_length_squared(ap) <= threshold * threshold;
        joy_ft_t proj = vec2_dot(ap, ab) / ab_len_sq;
        proj = fminf(fmaxf(proj, 0), 1);
        joy_vec2_t closest = vec2_add(p1, vec2_scale(ab, proj));
        joy_vec2_t diff = vec2_sub(point, closest);
        return vec2_length_squared(diff) <= threshold * threshold;
}

bool joy_math_collision2d_check_circles(joy_circle_t a, joy_circle_t b)
{
        joy_vec2_t distance = vec2_sub(b.center, a.center);
        joy_ft_t dist_sq = vec2_length_squared(distance);
        joy_ft_t radius_sum = a.radius + b.radius;
        return dist_sq <= radius_sum * radius_sum;
}

bool joy_math_collision2d_check_rectangles(joy_rectangle_t a, joy_rectangle_t b)
{
        if (a.x > b.x + b.width) return false;
        if (a.x + a.width < b.x) return false;
        if (a.y > b.y + b.height) return false;
        if (a.y + a.height < b.y) return false;
        return true;
}

bool joy_math_collision2d_check_circle_rectangle(joy_circle_t a, joy_rectangle_t b)
{
        joy_vec2_t closest;
        closest.x = fminf(fmaxf(a.center.x, b.x), b.x + b.width);
        closest.y = fminf(fmaxf(a.center.y, b.y), b.y + b.height);
        joy_vec2_t diff = vec2_sub(a.center, closest);
        return vec2_length_squared(diff) <= a.radius * a.radius;
}

/* ---- 辅助：获取多边形某条边 ---- */
static joy_vec2_t poly_edge(joy_polygon_t poly, int index)
{
        joy_vec2_t curr = poly.vertices[index];
        joy_vec2_t next = poly.vertices[(index + 1) % poly.num_vertices];
        return vec2_sub(next, curr);
}

/* ---- 辅助：计算多边形间最小分离 ---- */
static joy_ft_t poly_min_sep(joy_polygon_t a, joy_polygon_t b, joy_vec2_t* axis, joy_vec2_t* point)
{
        joy_ft_t separation = -FLT_MAX;
        for (int i = 0; i < a.num_vertices; i++) {
                joy_vec2_t va = a.vertices[i];
                joy_vec2_t normal = vec2_normal(poly_edge(a, i));
                joy_ft_t min_sep = FLT_MAX;
                joy_vec2_t min_vertex = {0,0};
                for (int j = 0; j < b.num_vertices; j++) {
                        joy_vec2_t vb = b.vertices[j];
                        joy_ft_t proj = vec2_dot(vec2_sub(vb, va), normal);
                        if (proj < min_sep) { min_sep = proj; min_vertex = vb; }
                }
                if (min_sep > separation) {
                        separation = min_sep;
                        *axis = poly_edge(a, i);
                        *point = min_vertex;
                }
        }
        return separation;
}

bool joy_math_collision2d_check_polygons(joy_polygon_t a, joy_polygon_t b)
{
        joy_vec2_t axis, point;
        joy_ft_t ab_sep = poly_min_sep(a, b, &axis, &point);
        if (ab_sep >= 0) return false;
        joy_ft_t ba_sep = poly_min_sep(b, a, &axis, &point);
        if (ba_sep >= 0) return false;
        return true;
}

bool joy_math_collision2d_check_circle_polygon(joy_circle_t a, joy_polygon_t b)
{
        if (b.num_vertices < 3) return false;
        for (int i = 0; i < b.num_vertices; i++) {
                joy_vec2_t e1 = b.vertices[i];
                joy_vec2_t e2 = b.vertices[(i + 1) % b.num_vertices];
                joy_vec2_t edge = vec2_sub(e2, e1);
                joy_vec2_t to_center = vec2_sub(a.center, e1);
                joy_ft_t edge_len_sq = vec2_length_squared(edge);
                if (edge_len_sq == 0) continue;
                joy_ft_t t = vec2_dot(to_center, edge) / edge_len_sq;
                t = fminf(fmaxf(t, 0), 1);
                joy_vec2_t closest = vec2_add(e1, vec2_scale(edge, t));
                joy_vec2_t diff = vec2_sub(a.center, closest);
                if (vec2_length_squared(diff) <= a.radius * a.radius) return true;
        }
        {
                bool inside = true;
                for (int i = 0; i < b.num_vertices; i++) {
                        joy_vec2_t e1 = b.vertices[i];
                        joy_vec2_t e2 = b.vertices[(i + 1) % b.num_vertices];
                        joy_vec2_t edge = vec2_sub(e2, e1);
                        joy_vec2_t normal = vec2_normal(edge);
                        joy_vec2_t to_center = vec2_sub(a.center, e1);
                        if (vec2_dot(to_center, normal) < 0) { inside = false; break; }
                }
                if (inside) return true;
        }
        return false;
}

int joy_math_collision2d_get_circles(joy_circle_t a, joy_circle_t b, joy_contact2d_t* contact)
{
        joy_vec2_t distance = vec2_sub(b.center, a.center);
        joy_ft_t dist_sq = vec2_length_squared(distance);
        joy_ft_t radius_sum = a.radius + b.radius;
        if (dist_sq > radius_sum * radius_sum) return 0;
        contact->normal = vec2_normalize(distance);
        contact->sp = vec2_sub(b.center, vec2_scale(contact->normal, b.radius));
        contact->ep = vec2_add(a.center, vec2_scale(contact->normal, a.radius));
        contact->depth = vec2_length(vec2_sub(contact->ep, contact->sp));
        return 1;
}

int joy_math_collision2d_get_rectangles(joy_rectangle_t a, joy_ft_t a_angle,
        joy_rectangle_t b, joy_ft_t b_angle, joy_contact2d_t* contact)
{
        joy_vec2_t a_bounds, b_bounds, a_h, b_h, a_pos, b_pos;
        joy_mat22_t a_rot, b_rot, a_rotT, b_rotT, c, abs_c, abs_ct;
        joy_vec2_t dp, a_d, b_d, abs_a_d, abs_b_d, a_face, b_face;
        joy_vec2_t normal;
        joy_ft_t separation, r_separation;

        a_bounds.x = a.width; a_bounds.y = a.height;
        b_bounds.x = b.width; b_bounds.y = b.height;
        a_h = vec2_scale(a_bounds, 0.5f);
        b_h = vec2_scale(b_bounds, 0.5f);
        a_pos.x = a.x + a_h.x; a_pos.y = a.y + a_h.y;
        b_pos.x = b.x + b_h.x; b_pos.y = b.y + b_h.y;

        joy_ft_t a_res = a_angle * 3.14159265f / 180.0f;
        joy_ft_t b_res = b_angle * 3.14159265f / 180.0f;
        a_rot = mat22_rotate(a_res);
        b_rot = mat22_rotate(b_res);
        a_rotT = mat22_transpose(a_rot);
        b_rotT = mat22_transpose(b_rot);
        dp = vec2_sub(b_pos, a_pos);
        a_d = mat22_mul_vec2(a_rotT, dp);
        b_d = mat22_mul_vec2(b_rotT, dp);
        c = mat22_mul(a_rotT, b_rot);
        abs_c = mat22_abs(c);
        abs_ct = mat22_transpose(abs_c);
        abs_a_d = vec2_abs(a_d);
        a_face = vec2_sub(vec2_sub(abs_a_d, a_h), mat22_mul_vec2(abs_c, b_h));
        if (a_face.x > 0 || a_face.y > 0) return 0;
        abs_b_d = vec2_abs(b_d);
        b_face = vec2_sub(vec2_sub(abs_b_d, mat22_mul_vec2(abs_ct, a_h)), b_h);
        if (b_face.x > 0 || b_face.y > 0) return 0;

        separation = a_face.x;
        r_separation = 0.95f * separation;
        normal = a_d.x > 0 ? a_rot.col1 : vec2_negate(a_rot.col1);
        if (a_face.y > r_separation + 0.01f * a_h.y) {
                separation = a_face.y;
                normal = a_d.y > 0 ? a_rot.col2 : vec2_negate(a_rot.col2);
        }
        if (b_face.x > r_separation + 0.01f * b_h.x) {
                separation = b_face.x;
                normal = b_d.x > 0 ? b_rot.col1 : vec2_negate(b_rot.col1);
        }
        if (b_face.y > r_separation + 0.01f * b_h.y) {
                separation = b_face.y;
                normal = b_d.y > 0 ? b_rot.col2 : vec2_negate(b_rot.col2);
        }
        contact->depth = separation;
        contact->normal = normal;
        return 1;
}

int joy_math_collision2d_get_polygons(joy_polygon_t a, joy_polygon_t b, joy_contact2d_t* contact)
{
        joy_vec2_t a_axis, b_axis, a_point, b_point, distance;
        joy_ft_t ab_sep = poly_min_sep(a, b, &a_axis, &a_point);
        if (ab_sep >= 0) return 0;
        joy_ft_t ba_sep = poly_min_sep(b, a, &b_axis, &b_point);
        if (ba_sep >= 0) return 0;
        if (ab_sep > ba_sep) {
                contact->depth = -ab_sep;
                contact->normal = vec2_normal(a_axis);
                contact->sp = a_point;
                distance = vec2_scale(contact->normal, contact->depth);
                contact->ep = vec2_add(a_point, distance);
        } else {
                contact->depth = -ba_sep;
                contact->normal = vec2_negate(vec2_normal(b_axis));
                distance = vec2_scale(contact->normal, contact->depth);
                contact->sp = vec2_sub(b_point, distance);
                contact->ep = b_point;
        }
        return 1;
}

joy_ray3d_collision_t
joy_math_collision3d_get_ray_triangle(joy_ray3d_t ray, joy_vec3_t p1, joy_vec3_t p2, joy_vec3_t p3)
{
        const joy_ft_t EPSILON = 0.000001f;
        joy_ray3d_collision_t result = {0};
        joy_vec3_t edge1, edge2, p, q, tv;
        joy_ft_t det, inv_det, u, v, t;

        ray.direction = vec3_normalize(ray.direction);
        edge1 = vec3_sub(p2, p1);
        edge2 = vec3_sub(p3, p1);
        p = vec3_cross(ray.direction, edge2);
        det = vec3_dot(edge1, p);
        if (det > -EPSILON && det < EPSILON) return result;
        inv_det = 1.0f / det;
        tv = vec3_sub(ray.position, p1);
        u = vec3_dot(tv, p) * inv_det;
        if (u < 0 || u > 1) return result;
        q = vec3_cross(tv, edge1);
        v = vec3_dot(ray.direction, q) * inv_det;
        if (v < 0 || u + v > 1) return result;
        t = vec3_dot(edge2, q) * inv_det;
        if (t > EPSILON) {
                result.hit = true;
                result.distance = t;
                result.normal = vec3_normalize(vec3_cross(edge1, edge2));
                result.point = vec3_add(ray.position, vec3_scale(ray.direction, t));
        }
        return result;
}

joy_ray3d_collision_t
joy_math_collision3d_get_ray_quad(joy_ray3d_t ray, joy_vec3_t p1, joy_vec3_t p2, joy_vec3_t p3, joy_vec3_t p4)
{
        joy_ray3d_collision_t result = joy_math_collision3d_get_ray_triangle(ray, p1, p2, p4);
        if (!result.hit) result = joy_math_collision3d_get_ray_triangle(ray, p2, p3, p4);
        return result;
}

joy_ray3d_collision_t
joy_math_collision3d_get_ray_sphere(joy_ray3d_t ray, joy_sphere_t sphere)
{
        joy_ray3d_collision_t result = {0};
        joy_vec3_t rs = vec3_sub(ray.position, sphere.position);
        joy_ft_t dist_sq = vec3_square_length(rs);
        joy_ft_t a = vec3_dot(rs, ray.direction);
        joy_ft_t b = dist_sq - sphere.radius * sphere.radius;
        joy_ft_t c = a * a - b;
        if (c < 0) return result;
        joy_ft_t d = sqrtf(c);
        joy_ft_t t = -a - d;
        if (t < 0) t = -a + d;
        if (t >= 0) {
                result.hit = true;
                result.distance = t;
                result.point = vec3_add(ray.position, vec3_scale(ray.direction, t));
                result.normal = vec3_normalize(vec3_sub(result.point, sphere.position));
        }
        return result;
}

joy_ray3d_collision_t
joy_math_collision3d_get_ray_box(joy_ray3d_t ray, joy_bounding_box_t box)
{
        joy_ray3d_collision_t result = {0};
        joy_vec3_t dirfrac;
        joy_ft_t t1, t2, tmin, tmax;
        const joy_ft_t EPSILON = 0.00001f;

        ray.direction = vec3_normalize(ray.direction);
        dirfrac.x = 1.0f / ray.direction.x;
        dirfrac.y = 1.0f / ray.direction.y;
        dirfrac.z = 1.0f / ray.direction.z;

        t1 = (box.min.x - ray.position.x) * dirfrac.x;
        t2 = (box.max.x - ray.position.x) * dirfrac.x;
        if (t1 > t2) SWAP_FT(t1, t2);
        tmin = t1; tmax = t2;

        t1 = (box.min.y - ray.position.y) * dirfrac.y;
        t2 = (box.max.y - ray.position.y) * dirfrac.y;
        if (t1 > t2) SWAP_FT(t1, t2);
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return result;

        t1 = (box.min.z - ray.position.z) * dirfrac.z;
        t2 = (box.max.z - ray.position.z) * dirfrac.z;
        if (t1 > t2) SWAP_FT(t1, t2);
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return result;
        if (tmax < 0) return result;

        joy_ft_t t = tmin >= 0 ? tmin : tmax;
        if (t < EPSILON) return result;

        result.hit = true;
        result.distance = t;
        result.point = vec3_add(ray.position, vec3_scale(ray.direction, t));

        joy_vec3_t center = vec3_scale(vec3_add(box.min, box.max), 0.5f);
        joy_vec3_t htc = vec3_sub(center, result.point);
        joy_vec3_t half = vec3_scale(vec3_sub(box.max, box.min), 0.5f);
        joy_vec3_t abs_hit = vec3_abs(htc);

        result.normal.x = (fabsf(abs_hit.x - half.x) < EPSILON) ? (htc.x > 0 ? 1 : -1) : 0;
        result.normal.y = (fabsf(abs_hit.y - half.y) < EPSILON) ? (htc.y > 0 ? 1 : -1) : 0;
        result.normal.z = (fabsf(abs_hit.z - half.z) < EPSILON) ? (htc.z > 0 ? 1 : -1) : 0;
        result.normal = vec3_normalize(result.normal);
        return result;
}

bool joy_math_collision3d_check_spheres(joy_sphere_t a, joy_sphere_t b)
{
        joy_vec3_t diff = vec3_sub(b.position, a.position);
        joy_ft_t dist_sq = vec3_square_length(diff);
        joy_ft_t radius_sum = a.radius + b.radius;
        return dist_sq <= radius_sum * radius_sum;
}

bool joy_math_collision3d_check_boxes(joy_bounding_box_t a, joy_bounding_box_t b)
{
        return a.min.x <= b.max.x && a.max.x >= b.min.x
            && a.min.y <= b.max.y && a.max.y >= b.min.y
            && a.min.z <= b.max.z && a.max.z >= b.min.z;
}

bool joy_math_collision3d_check_box_sphere(joy_bounding_box_t a, joy_sphere_t b)
{
        joy_vec3_t closest;
        closest.x = fminf(fmaxf(b.position.x, a.min.x), a.max.x);
        closest.y = fminf(fmaxf(b.position.y, a.min.y), a.max.y);
        closest.z = fminf(fmaxf(b.position.z, a.min.z), a.max.z);
        joy_vec3_t diff = vec3_sub(b.position, closest);
        return vec3_square_length(diff) <= b.radius * b.radius;
}


/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef SWAP
#undef SWAP_FT

/* ==================== 结束 src/jcollision.c ==================== */

/* ==================== 合并自 src/jgjk.c ==================== */
#include "external/klib/kvec.h"
#include "joy.h"

/* ============ OBB 辅助: 欧拉角(绕X/Y/Z) -> 列主序旋转矩阵 ============ */

/* 列主序 joy_mat44f_t 对方向向量做旋转(忽略平移分量) */
static joy_vec3f_t mat44f_rotate_dir(joy_mat44f_t m, joy_vec3f_t v) {
        joy_vec3f_t r;
        r.x = fp_add(fp_add(fp_mul(m.m0, v.x), fp_mul(m.m4, v.y)), fp_mul(m.m8,  v.z));
        r.y = fp_add(fp_add(fp_mul(m.m1, v.x), fp_mul(m.m5, v.y)), fp_mul(m.m9,  v.z));
        r.z = fp_add(fp_add(fp_mul(m.m2, v.x), fp_mul(m.m6, v.y)), fp_mul(m.m10, v.z));
        return r;
}

/* 列主序 joy_mat44f_t 的转置对方向向量做旋转(即逆旋转，旋转矩阵正交) */
static joy_vec3f_t mat44f_rotate_dir_t(joy_mat44f_t m, joy_vec3f_t v) {
        joy_vec3f_t r;
        r.x = fp_add(fp_add(fp_mul(m.m0, v.x), fp_mul(m.m1, v.y)), fp_mul(m.m2,  v.z));
        r.y = fp_add(fp_add(fp_mul(m.m4, v.x), fp_mul(m.m5, v.y)), fp_mul(m.m6,  v.z));
        r.z = fp_add(fp_add(fp_mul(m.m8, v.x), fp_mul(m.m9, v.y)), fp_mul(m.m10, v.z));
        return r;
}

/* 欧拉角(弧度) -> 组合旋转矩阵, 顺序: R = Rz * Ry * Rx */
static joy_mat44f_t euler_to_rot(joy_vec3f_t euler) {
        joy_mat44f_t rx = mat44f_rotate_x(euler.x);
        joy_mat44f_t ry = mat44f_rotate_y(euler.y);
        joy_mat44f_t rz = mat44f_rotate_z(euler.z);
        /* R = Rz * Ry * Rx */
        joy_mat44f_t tmp = mat44f_mul(ry, rx);
        return mat44f_mul(rz, tmp);
}

/* ============ 内部类型 ============ */

/* 单纯形 (GJK用) */
typedef struct joy_simplex3d_f {
	joy_vec3f_t points[4];
	int points_num;
} joy_simplex3d_f_t;

/* EPA面法线 */
typedef struct {
	joy_vec3f_t normal;
	joy_fp_t distance;
} joy_epa_face_f_t;

/* EPA边 */
typedef struct {
	size_t a, b;
} joy_epa_edge_f_t;

/* kvec_t 展开为匿名 struct, 直接用匿名类型作为形参/传址会让 MSVC 判定为
   互不兼容的匿名类型(C4116/C4133)。起具名别名后同一类型身份, 告警消除。 */
typedef kvec_t(size_t)       joy_size_buf_t;
typedef kvec_t(joy_vec3f_t)      joy_vec3f_buf_t;
typedef kvec_t(joy_epa_face_f_t) joy_epa_face_f_buf_t;
typedef kvec_t(joy_epa_edge_f_t) joy_epa_edge_f_buf_t;

/* ============ 单纯形操作 ============ */

static void simplex_init1_f(joy_simplex3d_f_t *s, joy_vec3f_t p0) {
	s->points[0] = p0;
	s->points_num = 1;
}

static void simplex_init2_f(joy_simplex3d_f_t *s, joy_vec3f_t p0, joy_vec3f_t p1) {
	s->points[0] = p0;
	s->points[1] = p1;
	s->points_num = 2;
}

static void simplex_init3_f(joy_simplex3d_f_t *s, joy_vec3f_t p0, joy_vec3f_t p1, joy_vec3f_t p2) {
	s->points[0] = p0;
	s->points[1] = p1;
	s->points[2] = p2;
	s->points_num = 3;
}

static void simplex_push_f(joy_simplex3d_f_t *s, joy_vec3f_t p) {
	s->points[3] = s->points[2];
	s->points[2] = s->points[1];
	s->points[1] = s->points[0];
	s->points[0] = p;
	if (s->points_num < 4) s->points_num++;
}

/* ============ 各碰撞体的最远点计算 ============ */

static joy_vec3f_t find_furthest_point_f(joy_gjk3df_collider_p c, joy_vec3f_t dir) {
	joy_vec3f_t result = { fp_zero(), fp_zero(), fp_zero() };

	switch (c->type) {
	case GJK3DF_CTYPE_SPHERE: {
		joy_vec3f_t ndir = vec3f_normalize(dir);
		joy_vec3f_t offset = vec3f_scale(ndir, c->radius);
		result = vec3f_add(c->position, offset);
		break;
	}
	case GJK3DF_CTYPE_BOX: {
		/* AABB: 根据方向符号取半边长 */
		joy_fp_t sx = dir.x > fp_zero() ? c->half_size.x : -c->half_size.x;
		joy_fp_t sy = dir.y > fp_zero() ? c->half_size.y : -c->half_size.y;
		joy_fp_t sz = dir.z > fp_zero() ? c->half_size.z : -c->half_size.z;
		result.x = fp_add(c->position.x, sx);
		result.y = fp_add(c->position.y, sy);
		result.z = fp_add(c->position.z, sz);
		break;
	}
	case GJK3DF_CTYPE_OBB: {
		/* 旋转盒: 把 dir 逆旋转到本地空间取 AABB 最远点, 再正旋转回世界 */
		joy_mat44f_t R = euler_to_rot(c->rotation);
		joy_vec3f_t local_dir = mat44f_rotate_dir_t(R, dir);  /* 逆旋转(旋转矩阵正交, 用转置) */
		joy_fp_t sx = local_dir.x > fp_zero() ? c->half_size.x : -c->half_size.x;
		joy_fp_t sy = local_dir.y > fp_zero() ? c->half_size.y : -c->half_size.y;
		joy_fp_t sz = local_dir.z > fp_zero() ? c->half_size.z : -c->half_size.z;
		joy_vec3f_t local_pt = { sx, sy, sz };
		joy_vec3f_t world_pt = mat44f_rotate_dir(R, local_pt);
		result.x = fp_add(c->position.x, world_pt.x);
		result.y = fp_add(c->position.y, world_pt.y);
		result.z = fp_add(c->position.z, world_pt.z);
		break;
	}
	case GJK3DF_CTYPE_CAPSULE: {
		/* 胶囊: 线段(沿axis方向 ± height/2) + 半径 */
		joy_fp_t len = vec3f_length(dir);
		if (len == fp_zero()) { result = c->position; break; }
		joy_vec3f_t ndir = vec3f_scale(dir, fp_div(fp_one(), len));
		joy_fp_t dot = vec3f_dot(ndir, c->axis);
		joy_fp_t half_factor = dot > fp_zero() ? fp_half() : -fp_half();
		joy_vec3f_t end_offset = vec3f_scale(c->axis, fp_mul(half_factor, c->height));
		joy_vec3f_t center = vec3f_add(c->position, end_offset);
		result = vec3f_add(center, vec3f_scale(ndir, c->radius));
		break;
	}
	case GJK3DF_CTYPE_CYLINDER: {
		/* 圆柱: 底部圆盘 + 顶部圆盘 */
		joy_fp_t len = vec3f_length(dir);
		if (len == fp_zero()) { result = c->position; break; }
		joy_vec3f_t ndir = vec3f_scale(dir, fp_div(fp_one(), len));
		joy_fp_t hy = ndir.y > fp_zero() ? c->height : fp_zero();
		/* 投影到水平面求圆盘边界 */
		joy_fp_t r_scale = fp_sqrt(fp_sub(fp_one(), fp_mul(ndir.y, ndir.y)));
		result.x = fp_add(c->position.x, fp_mul(ndir.x, fp_mul(r_scale, c->radius)));
		result.y = fp_add(c->position.y, hy);
		result.z = fp_add(c->position.z, fp_mul(ndir.z, fp_mul(r_scale, c->radius)));
		break;
	}
	case GJK3DF_CTYPE_CONE: {
		/* 圆锥: 底在 center, 锥顶在 center + (0, height, 0) */
		joy_fp_t len = vec3f_length(dir);
		if (len == fp_zero()) { result = c->position; break; }
		joy_vec3f_t ndir = vec3f_scale(dir, fp_div(fp_one(), len));
		joy_fp_t sin_theta = c->radius / fp_sqrt(fp_add(fp_pow2(c->radius), fp_pow2(c->height)));
		if (ndir.y > sin_theta) {
			/* 方向朝上足够多 → 锥顶 */
			result = c->position;
			result.y = fp_add(result.y, c->height);
		} else {
			/* 底边圆盘 */
			joy_fp_t dist = fp_sqrt(fp_add(fp_pow2(ndir.x), fp_pow2(ndir.z)));
			if (dist > fp_zero()) {
				result.x = fp_add(c->position.x, fp_mul(fp_div(ndir.x, dist), c->radius));
				result.y = c->position.y;
				result.z = fp_add(c->position.z, fp_mul(fp_div(ndir.z, dist), c->radius));
			} else {
				result.x = fp_add(c->position.x, c->radius);
				result.y = c->position.y;
				result.z = c->position.z;
			}
		}
		break;
	}
	case GJK3DF_CTYPE_MESH: {
		joy_fp_t max_dot = fp_min_value();
		for (int i = 0; i < c->vertices_num; i++) {
			joy_fp_t d = vec3f_dot(c->vertices[i], dir);
			if (d > max_dot) { max_dot = d; result = c->vertices[i]; }
		}
		break;
	}
	}
	return result;
}

/* ============ Minkowski差支持函数 ============ */

static joy_vec3f_t get_support_point_f(joy_gjk3df_collider_p ca, joy_gjk3df_collider_p cb, joy_vec3f_t dir) {
	joy_vec3f_t a = find_furthest_point_f(ca, dir);
	joy_vec3f_t b = find_furthest_point_f(cb, vec3f_negate(dir));
	return vec3f_sub(a, b);
}

/* ============ GJK核心 ============ */

/* 相对方向判断: 点积显著为正(余弦 > 1e-4)才视为同向,
   避免对称退化位形下符号在噪声中翻转导致分支路径依赖 */
static bool same_direction_f(joy_vec3f_t dir, joy_vec3f_t ao) {
	joy_fp_t dot = vec3f_dot(dir, ao);
	if (dot <= fp_zero()) return false;
	/* (dot / (|dir||ao|))^2 > eps^2, 避免 sqrt */
	joy_fp_t eps2 = fp_from_float(1e-8f);
	joy_fp_t len2 = fp_mul(vec3f_square_length(dir), vec3f_square_length(ao));
	return fp_mul(dot, dot) > fp_mul(eps2, len2);
}

/* 安全叉积归一化: 三点近共线(叉积退化)时返回 false */
static bool safe_cross_normalize_f(joy_vec3f_t a, joy_vec3f_t b, joy_vec3f_t *out) {
	joy_fp_t ab2 = fp_mul(vec3f_square_length(a), vec3f_square_length(b));
	if (ab2 == fp_zero()) return false;
	joy_vec3f_t n = vec3f_cross(a, b);
	joy_fp_t len2 = vec3f_square_length(n);
	joy_fp_t eps2 = fp_from_float(1e-8f);
	if (len2 <= fp_mul(ab2, eps2)) return false;
	*out = vec3f_normalize(n);
	return true;
}

static bool gjk_line_f(joy_simplex3d_f_t *s, joy_vec3f_t *dir) {
	joy_vec3f_t a = s->points[0], b = s->points[1];
	joy_vec3f_t ab = vec3f_normalize(vec3f_sub(b, a));
	joy_vec3f_t ao = vec3f_normalize(vec3f_negate(a));
	if (same_direction_f(ab, ao)) {
		*dir = vec3f_normalize(vec3f_cross(vec3f_cross(ab, ao), ab));
	} else {
		simplex_init1_f(s, a);
		*dir = ao;
	}
	return false;
}

static bool gjk_triangle_f(joy_simplex3d_f_t *s, joy_vec3f_t *dir) {
	joy_vec3f_t a = s->points[0], b = s->points[1], c = s->points[2];
	joy_vec3f_t ab = vec3f_normalize(vec3f_sub(b, a));
	joy_vec3f_t ac = vec3f_normalize(vec3f_sub(c, a));
	joy_vec3f_t ao = vec3f_normalize(vec3f_negate(a));
	joy_vec3f_t abc;
	if (!safe_cross_normalize_f(ab, ac, &abc)) {
		/* 三点近共线(退化三角形): 退化为线段处理 */
		simplex_init2_f(s, a, b);
		return gjk_line_f(s, dir);
	}

	if (same_direction_f(vec3f_cross(abc, ac), ao)) {
		if (same_direction_f(ac, ao)) {
			simplex_init2_f(s, a, c);
			*dir = vec3f_normalize(vec3f_cross(vec3f_cross(ac, ao), ac));
		} else {
			simplex_init2_f(s, a, b);
			return gjk_line_f(s, dir);
		}
	} else {
		if (same_direction_f(vec3f_cross(ab, abc), ao)) {
			simplex_init2_f(s, a, b);
			return gjk_line_f(s, dir);
		} else {
			if (same_direction_f(abc, ao)) {
				*dir = abc;
			} else {
				simplex_init3_f(s, a, c, b);
				*dir = vec3f_negate(abc);
			}
		}
	}
	return false;
}

static bool gjk_tetrahedron_f(joy_simplex3d_f_t *s, joy_vec3f_t *dir) {
	joy_vec3f_t a = s->points[0], b = s->points[1], c = s->points[2], d = s->points[3];
	joy_vec3f_t ab = vec3f_normalize(vec3f_sub(b, a));
	joy_vec3f_t ac = vec3f_normalize(vec3f_sub(c, a));
	joy_vec3f_t ad = vec3f_normalize(vec3f_sub(d, a));
	joy_vec3f_t ao = vec3f_normalize(vec3f_negate(a));
	joy_vec3f_t abc, acd, adb;
	/* 各面退化(近共线)时跳过该分支, 避免法线为噪声方向 */
	bool has_abc = safe_cross_normalize_f(ab, ac, &abc);
	bool has_acd = safe_cross_normalize_f(ac, ad, &acd);
	bool has_adb = safe_cross_normalize_f(ad, ab, &adb);

	if (has_abc && same_direction_f(abc, ao)) { simplex_init3_f(s, a, b, c); return gjk_triangle_f(s, dir); }
	if (has_acd && same_direction_f(acd, ao)) { simplex_init3_f(s, a, c, d); return gjk_triangle_f(s, dir); }
	if (has_adb && same_direction_f(adb, ao)) { simplex_init3_f(s, a, d, b); return gjk_triangle_f(s, dir); }
	return true;
}

static bool next_simplex_f(joy_simplex3d_f_t *s, joy_vec3f_t *dir) {
	switch (s->points_num) {
	case 2: return gjk_line_f(s, dir);
	case 3: return gjk_triangle_f(s, dir);
	case 4: return gjk_tetrahedron_f(s, dir);
	}
	return false;
}

/* ============ EPA (扩展多面体算法) ============ */

/* 计算所有面法线和最近面索引 */
static void get_face_normals_f(joy_vec3f_buf_t *polytope, joy_size_buf_t *faces,
	joy_epa_face_f_buf_t *normals, size_t *min_triangle) {
	*min_triangle = 0;
	joy_fp_t min_distance = fp_max_value();

	for (size_t i = 0; i < faces->n; i += 3) {
		joy_vec3f_t a = kv_A(*polytope, kv_A(*faces, i));
		joy_vec3f_t b = kv_A(*polytope, kv_A(*faces, i + 1));
		joy_vec3f_t c = kv_A(*polytope, kv_A(*faces, i + 2));

		joy_vec3f_t ba = vec3f_normalize(vec3f_sub(b, a));
		joy_vec3f_t ca = vec3f_normalize(vec3f_sub(c, a));
		joy_vec3f_t n;
		if (!safe_cross_normalize_f(ba, ca, &n)) {
			/* 退化面(三点近共线): 法线退化为噪声方向, 跳过该面,
			   将其距离置为最大避免被选为最近面 */
			joy_epa_face_f_t face;
			face.normal = (joy_vec3f_t){ fp_one(), fp_zero(), fp_zero() };
			face.distance = fp_max_value();
			kv_push(joy_epa_face_f_t, *normals, face);
			continue;
		}

		joy_fp_t dist = vec3f_dot(n, a);
		if (dist < fp_zero()) { n = vec3f_negate(n); dist = -dist; }

		joy_epa_face_f_t face;
		face.normal = n;
		face.distance = dist;
		kv_push(joy_epa_face_f_t, *normals, face);

		if (dist < min_distance) { min_distance = dist; *min_triangle = i / 3; }
	}
}

/* 添加不重复边(EPA用) */
static void add_unique_edge_f(joy_epa_edge_f_buf_t *edges, joy_size_buf_t *faces, size_t a, size_t b) {
	size_t fa = kv_A(*faces, a);
	size_t fb = kv_A(*faces, b);
	/* 查找是否存在反向边 */
	for (size_t i = 0; i < edges->n; i++) {
		if (kv_A(*edges, i).a == fb && kv_A(*edges, i).b == fa) {
			/* 找到反向边,移除(该边为内部边) */
			kv_A(*edges, i) = kv_A(*edges, edges->n - 1);
			edges->n--;
			return;
		}
	}
	joy_epa_edge_f_t edge = { fa, fb };
	kv_push(joy_epa_edge_f_t, *edges, edge);
}

/* EPA主函数: 从单纯形出发构建多面体,计算穿透法线和深度 */
static bool epa_f(joy_simplex3d_f_t *simplex, joy_gjk3df_collider_p ca, joy_gjk3df_collider_p cb, joy_gjk3df_contact_p contact) {
	joy_vec3f_buf_t polytope;
	joy_size_buf_t faces;
	joy_epa_face_f_buf_t normals;

	kv_init(polytope);
	kv_init(faces);
	kv_init(normals);

	/* 将单纯形点复制到多面体 */
	for (int i = 0; i < simplex->points_num; i++) {
		kv_push(joy_vec3f_t, polytope, simplex->points[i]);
	}

	/* 四面体的4个面(12个索引) */
	{
		size_t init_faces[] = { 0, 1, 2, 0, 3, 1, 0, 2, 3, 1, 3, 2 };
		for (int i = 0; i < 12; i++) {
			kv_push(size_t, faces, init_faces[i]);
		}
	}

	size_t min_face = 0;
	get_face_normals_f(&polytope, &faces, &normals, &min_face);

	/* 备份初始最小面 */
	joy_vec3f_t bake_min_normal = kv_A(normals, min_face).normal;
	joy_fp_t bake_min_distance = kv_A(normals, min_face).distance;

	joy_vec3f_t min_normal = { fp_zero(), fp_zero(), fp_zero() };
	joy_fp_t min_distance = fp_max_value();

	int epa_iter = 0;
	while (min_distance == fp_max_value() && epa_iter < 64) {
		epa_iter++;
		min_normal = kv_A(normals, min_face).normal;
		min_distance = kv_A(normals, min_face).distance;

		joy_vec3f_t support = get_support_point_f(ca, cb, min_normal);
		joy_fp_t s_dist = vec3f_dot(min_normal, support);

		if (fp_abs(fp_sub(s_dist, min_distance)) > fp_from_float(0.001f)) {
			min_distance = fp_max_value();

			/* 找到需要移除的面,收集边界边 */
			joy_epa_edge_f_buf_t unique_edges;
			kv_init(unique_edges);

			for (size_t i = 0; i < normals.n; i++) {
				joy_vec3f_t n = kv_A(normals, i).normal;
				size_t f = i * 3;
				if (vec3f_dot(n, support) > vec3f_dot(n, kv_A(polytope, kv_A(faces, f)))) {
					add_unique_edge_f(&unique_edges, &faces, f, f + 1);
					add_unique_edge_f(&unique_edges, &faces, f + 1, f + 2);
					add_unique_edge_f(&unique_edges, &faces, f + 2, f);

					/* 移除该面(用最后一个面替换) */
					kv_A(faces, f + 2) = kv_A(faces, faces.n - 1); faces.n--;
					kv_A(faces, f + 1) = kv_A(faces, faces.n - 1); faces.n--;
					kv_A(faces, f)     = kv_A(faces, faces.n - 1); faces.n--;
					kv_A(normals, i)   = kv_A(normals, normals.n - 1); normals.n--;
					i--; /* 重新检查当前索引 */
				}
			}

			/* 用边界边创建新面 */
			joy_size_buf_t new_faces;
			kv_init(new_faces);
			size_t new_vertex_index = polytope.n;
			for (size_t i = 0; i < unique_edges.n; i++) {
				kv_push(size_t, new_faces, kv_A(unique_edges, i).a);
				kv_push(size_t, new_faces, kv_A(unique_edges, i).b);
				kv_push(size_t, new_faces, new_vertex_index);
			}
			kv_push(joy_vec3f_t, polytope, support);

			/* 计算新面的法线 */
			joy_epa_face_f_buf_t new_normals;
			kv_init(new_normals);
			size_t new_min_face = 0;
			get_face_normals_f(&polytope, &new_faces, &new_normals, &new_min_face);

			/* 找到全局最小距离的面 */
			joy_fp_t old_min_dist = fp_max_value();
			size_t global_min = 0;
			for (size_t i = 0; i < normals.n; i++) {
				if (kv_A(normals, i).distance < old_min_dist) {
					old_min_dist = kv_A(normals, i).distance;
					global_min = i;
				}
			}

			if (new_normals.n > 0) {
				if (kv_A(new_normals, new_min_face).distance < old_min_dist) {
					min_face = new_min_face + normals.n;
				}
			} else {
				min_normal = bake_min_normal;
				min_distance = bake_min_distance;
				break;
			}

			/* 合并新面到主列表 */
			for (size_t i = 0; i < new_faces.n; i++) {
				kv_push(size_t, faces, kv_A(new_faces, i));
			}
			for (size_t i = 0; i < new_normals.n; i++) {
				kv_push(joy_epa_face_f_t, normals, kv_A(new_normals, i));
			}

			kv_destroy(new_faces);
			kv_destroy(new_normals);
			kv_destroy(unique_edges);
		}
	}

	/* EPA后处理: 用一组候选方向重新验证穿透深度, 取最小值,
	   纠正EPA因退化/局部最优收敛到错误支撑面的情况 */
	{
		joy_vec3f_t candidates[8];
		int n = 0;
		joy_vec3f_t axis[3] = {
			{ fp_one(), fp_zero(), fp_zero() },
			{ fp_zero(), fp_one(), fp_zero() },
			{ fp_zero(), fp_zero(), fp_one() }
		};
		for (int i = 0; i < 3; i++) {
			candidates[n++] = axis[i];
			candidates[n++] = vec3f_negate(axis[i]);
		}
		/* 两物体中心连线方向 */
		joy_vec3f_t delta = vec3f_sub(cb->position, ca->position);
		joy_fp_t dl = vec3f_length(delta);
		if (dl > fp_zero()) {
			candidates[n++] = vec3f_scale(delta, fp_div(fp_one(), dl));
			candidates[n++] = vec3f_scale(vec3f_negate(delta), fp_div(fp_one(), dl));
		}

		for (int i = 0; i < n; i++) {
			joy_vec3f_t support = get_support_point_f(ca, cb, candidates[i]);
			joy_fp_t d = vec3f_dot(candidates[i], support);
			if (d > fp_zero() && d < min_distance) {
				min_distance = d;
				min_normal = candidates[i];
			}
		}
	}

	contact->normal = min_normal;
	contact->depth = fp_add(min_distance, fp_from_float(0.001f));

	kv_destroy(polytope);
	kv_destroy(faces);
	kv_destroy(normals);

	return true;
}

/* ============ 退化兜底 ============ */

/* 当 GJK 因对称/共线退化无法构建四面体(如同心球、共线单纯形)时,
   用 ±3轴方向采样穿透距离: 任一方向分离(支持点投影 <= 0)则判定无碰撞,
   否则取最小穿透距离作为深度(法线为该轴方向) */
static bool degenerate_fallback_f(joy_gjk3df_collider_p ca, joy_gjk3df_collider_p cb, joy_gjk3df_contact_p contact) {
	joy_vec3f_t axes[6] = {
		{  fp_one(), fp_zero(), fp_zero() },
		{ -fp_one(), fp_zero(), fp_zero() },
		{  fp_zero(),  fp_one(), fp_zero() },
		{  fp_zero(), -fp_one(), fp_zero() },
		{  fp_zero(), fp_zero(),  fp_one() },
		{  fp_zero(), fp_zero(), -fp_one() }
	};
	joy_fp_t min_d = fp_max_value();
	joy_vec3f_t min_n = { fp_zero(), fp_zero(), fp_zero() };
	for (int i = 0; i < 6; i++) {
		joy_vec3f_t s = get_support_point_f(ca, cb, axes[i]);
		joy_fp_t d = vec3f_dot(axes[i], s);
		if (d <= fp_zero()) return false; /* 该方向分离 */
		if (d < min_d) { min_d = d; min_n = axes[i]; }
	}
	contact->normal = min_n;
	contact->depth = fp_add(min_d, fp_from_float(0.001f));
	return true;
}

/* ============ 公开API实现 ============ */
bool joy_math_gjk3df_collide(joy_gjk3df_collider_p c1, joy_gjk3df_collider_p c2,
	joy_vec3f_t init_dir, joy_gjk3df_contact_p contact)
{
	joy_simplex3d_f_t simplex;
	memset(&simplex, 0, sizeof(simplex));

	joy_vec3f_t support = get_support_point_f(c1, c2, init_dir);
	simplex_push_f(&simplex, support);
	joy_vec3f_t dir = vec3f_normalize(vec3f_negate(support));

	joy_fp_t tiny2 = fp_from_float(1e-8f);
	int gjk_iter = 0;
	while (gjk_iter < 128) {
		gjk_iter++;
		/* 方向退化(零向量): 无法继续推进, 进入退化兜底 */
		if (vec3f_square_length(dir) <= tiny2) {
			return degenerate_fallback_f(c1, c2, contact);
		}
		support = get_support_point_f(c1, c2, dir);
		if (vec3f_dot(support, dir) < fp_zero()) {
			/* 无碰撞 */
			contact->normal.x = fp_zero();
			contact->normal.y = fp_zero();
			contact->normal.z = fp_zero();
			contact->depth = fp_zero();
			return false;
		}
		/* 支持点与已有点重合(不再推进): 对称退化, 兜底判定 */
		{
			bool dup = false;
			for (int i = 0; i < simplex.points_num; i++) {
				if (vec3f_square_length(vec3f_sub(support, simplex.points[i])) <= tiny2) { dup = true; break; }
			}
			if (dup) return degenerate_fallback_f(c1, c2, contact);
		}
		simplex_push_f(&simplex, support);
		if (next_simplex_f(&simplex, &dir)) {
			return epa_f(&simplex, c1, c2, contact);
		}
	}

	contact->normal.x = fp_zero();
	contact->normal.y = fp_zero();
	contact->normal.z = fp_zero();
	contact->depth = fp_zero();
	return false;
}

/* ============ 碰撞体初始化函数 ============ */

void joy_math_gjk3df_init_sphere(joy_gjk3df_collider_t *c, joy_vec3f_t pos, joy_fp_t radius) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3DF_CTYPE_SPHERE;
	c->position = pos;
	c->radius = radius;
}

void joy_math_gjk3df_init_box(joy_gjk3df_collider_t *c, joy_vec3f_t pos, joy_vec3f_t half_size) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3DF_CTYPE_BOX;
	c->position = pos;
	c->half_size = half_size;
}

void joy_math_gjk3df_init_obb(joy_gjk3df_collider_t *c, joy_vec3f_t pos, joy_vec3f_t half_size, joy_vec3f_t rotation) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3DF_CTYPE_OBB;
	c->position = pos;
	c->half_size = half_size;
	c->rotation = rotation;
}

void joy_math_gjk3df_init_capsule(joy_gjk3df_collider_t *c, joy_vec3f_t pos, joy_fp_t radius, joy_fp_t height, joy_vec3f_t axis) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3DF_CTYPE_CAPSULE;
	c->position = pos;
	c->radius = radius;
	c->height = height;
	c->axis = vec3f_normalize(axis);
}

void joy_math_gjk3df_init_cylinder(joy_gjk3df_collider_t *c, joy_vec3f_t pos, joy_fp_t radius, joy_fp_t height) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3DF_CTYPE_CYLINDER;
	c->position = pos;
	c->radius = radius;
	c->height = height;
}

void joy_math_gjk3df_init_cone(joy_gjk3df_collider_t *c, joy_vec3f_t pos, joy_fp_t radius, joy_fp_t height) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3DF_CTYPE_CONE;
	c->position = pos;
	c->radius = radius;
	c->height = height;
}

void joy_math_gjk3df_init_mesh(joy_gjk3df_collider_t *c, joy_vec3f_t *vertices, int num) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3DF_CTYPE_MESH;
	c->vertices = vertices;
	c->vertices_num = num;
}

/* ======================================================================== */
/*              GJK 3D 浮点版 (gjk3d_* / joy_ft_t / joy_vec3_t)                     */
/* 算法与上方定点数版 (gjk3df_*) 完全一致, 仅类型从 joy_fp_t/joy_vec3f_t 换成       */
/* joy_ft_t/joy_vec3_t; 命名约定: 定点内部符号带 _f, 浮点内部符号保持干净在同一 TU 共存 */
/* ======================================================================== */

/* ============ 内部类型 ============ */

/* 单纯形 (GJK用); 注意 struct tag 需与定点版 simplex3d 区分, 二者共用 tag 命名空间 */
typedef struct joy_simplex3d {
	joy_vec3_t points[4];
	int points_num;
} joy_simplex3d_t;

/* EPA面法线 */
typedef struct {
	joy_vec3_t normal;
	joy_ft_t distance;
} joy_epa_face_t;

/* EPA边 */
typedef struct {
	size_t a, b;
} joy_epa_edge_t;

/* EPA kvec 具名别名(joy_size_buf_t 已在定点段定义, 此处复用) */
typedef kvec_t(joy_vec3_t)     joy_vec3_buf_t;
typedef kvec_t(joy_epa_face_t) joy_epa_face_buf_t;
typedef kvec_t(joy_epa_edge_t) joy_epa_edge_buf_t;

/* ============ 单纯形操作 ============ */

static void simplex_init1(joy_simplex3d_t *s, joy_vec3_t p0) {
	s->points[0] = p0;
	s->points_num = 1;
}

static void simplex_init2(joy_simplex3d_t *s, joy_vec3_t p0, joy_vec3_t p1) {
	s->points[0] = p0;
	s->points[1] = p1;
	s->points_num = 2;
}

static void simplex_init3(joy_simplex3d_t *s, joy_vec3_t p0, joy_vec3_t p1, joy_vec3_t p2) {
	s->points[0] = p0;
	s->points[1] = p1;
	s->points[2] = p2;
	s->points_num = 3;
}

static void simplex_push(joy_simplex3d_t *s, joy_vec3_t p) {
	s->points[3] = s->points[2];
	s->points[2] = s->points[1];
	s->points[1] = s->points[0];
	s->points[0] = p;
	if (s->points_num < 4) s->points_num++;
}

/* ============ 各碰撞体的最远点计算 ============ */

static joy_vec3_t find_furthest_point(joy_gjk3d_collider_p c, joy_vec3_t dir) {
	joy_vec3_t result = { ft_zero(), ft_zero(), ft_zero() };

	switch (c->type) {
	case GJK3D_CTYPE_SPHERE: {
		joy_vec3_t ndir = vec3_normalize(dir);
		joy_vec3_t offset = vec3_scale(ndir, c->radius);
		result = vec3_add(c->position, offset);
		break;
	}
	case GJK3D_CTYPE_BOX: {
		/* AABB: 根据方向符号取半边长 */
		joy_ft_t sx = dir.x > ft_zero() ? c->half_size.x : -c->half_size.x;
		joy_ft_t sy = dir.y > ft_zero() ? c->half_size.y : -c->half_size.y;
		joy_ft_t sz = dir.z > ft_zero() ? c->half_size.z : -c->half_size.z;
		result.x = ft_add(c->position.x, sx);
		result.y = ft_add(c->position.y, sy);
		result.z = ft_add(c->position.z, sz);
		break;
	}
	case GJK3D_CTYPE_CAPSULE: {
		/* 胶囊: 线段(沿axis方向 ± height/2) + 半径 */
		joy_ft_t len = vec3_length(dir);
		if (len == ft_zero()) { result = c->position; break; }
		joy_vec3_t ndir = vec3_scale(dir, ft_div(ft_one(), len));
		joy_ft_t dot = vec3_dot(ndir, c->axis);
		joy_ft_t half_factor = dot > ft_zero() ? ft_half() : -ft_half();
		joy_vec3_t end_offset = vec3_scale(c->axis, ft_mul(half_factor, c->height));
		joy_vec3_t center = vec3_add(c->position, end_offset);
		result = vec3_add(center, vec3_scale(ndir, c->radius));
		break;
	}
	case GJK3D_CTYPE_CYLINDER: {
		/* 圆柱: 底部圆盘 + 顶部圆盘 */
		joy_ft_t len = vec3_length(dir);
		if (len == ft_zero()) { result = c->position; break; }
		joy_vec3_t ndir = vec3_scale(dir, ft_div(ft_one(), len));
		joy_ft_t hy = ndir.y > ft_zero() ? c->height : ft_zero();
		/* 投影到水平面求圆盘边界 */
		joy_ft_t r_scale = ft_sqrt(ft_sub(ft_one(), ft_mul(ndir.y, ndir.y)));
		result.x = ft_add(c->position.x, ft_mul(ndir.x, ft_mul(r_scale, c->radius)));
		result.y = ft_add(c->position.y, hy);
		result.z = ft_add(c->position.z, ft_mul(ndir.z, ft_mul(r_scale, c->radius)));
		break;
	}
	case GJK3D_CTYPE_CONE: {
		/* 圆锥 */
		joy_ft_t len = vec3_length(dir);
		if (len == ft_zero()) { result = c->position; break; }
		joy_vec3_t ndir = vec3_scale(dir, ft_div(ft_one(), len));
		joy_ft_t sin_theta = c->radius / ft_sqrt(ft_add(ft_pow2(c->radius), ft_pow2(c->height)));
		if (ndir.y > sin_theta) {
			/* 方向朝上足够多 → 锥顶 */
			result = c->position;
			result.y = ft_add(result.y, c->height);
		} else {
			/* 底边圆盘 */
			joy_ft_t dist = ft_sqrt(ft_add(ft_pow2(ndir.x), ft_pow2(ndir.z)));
			if (dist > ft_zero()) {
				result.x = ft_add(c->position.x, ft_mul(ft_div(ndir.x, dist), c->radius));
				result.y = c->position.y;
				result.z = ft_add(c->position.z, ft_mul(ft_div(ndir.z, dist), c->radius));
			} else {
				result.x = ft_add(c->position.x, c->radius);
				result.y = c->position.y;
				result.z = c->position.z;
			}
		}
		break;
	}
	case GJK3D_CTYPE_MESH: {
		joy_ft_t max_dot = ft_min_value();
		for (int i = 0; i < c->vertices_num; i++) {
			joy_ft_t d = vec3_dot(c->vertices[i], dir);
			if (d > max_dot) { max_dot = d; result = c->vertices[i]; }
		}
		break;
	}
	}
	return result;
}

/* ============ Minkowski差支持函数 ============ */

static joy_vec3_t get_support_point(joy_gjk3d_collider_p ca, joy_gjk3d_collider_p cb, joy_vec3_t dir) {
	joy_vec3_t a = find_furthest_point(ca, dir);
	joy_vec3_t b = find_furthest_point(cb, vec3_negate(dir));
	return vec3_sub(a, b);
}

/* ============ GJK核心 ============ */

/* 相对方向判断: 点积显著为正(余弦 > 1e-4)才视为同向,
   避免对称退化位形下符号在噪声中翻转导致分支路径依赖 */
static bool same_direction(joy_vec3_t dir, joy_vec3_t ao) {
	joy_ft_t dot = vec3_dot(dir, ao);
	if (dot <= ft_zero()) return false;
	/* (dot / (|dir||ao|))^2 > eps^2, 避免 sqrt */
	joy_ft_t eps2 = ft_from_float(1e-8f);
	joy_ft_t len2 = ft_mul(vec3_square_length(dir), vec3_square_length(ao));
	return ft_mul(dot, dot) > ft_mul(eps2, len2);
}

/* 安全叉积归一化: 三点近共线(叉积退化)时返回 false */
static bool safe_cross_normalize(joy_vec3_t a, joy_vec3_t b, joy_vec3_t *out) {
	joy_ft_t ab2 = ft_mul(vec3_square_length(a), vec3_square_length(b));
	if (ab2 == ft_zero()) return false;
	joy_vec3_t n = vec3_cross(a, b);
	joy_ft_t len2 = vec3_square_length(n);
	joy_ft_t eps2 = ft_from_float(1e-8f);
	if (len2 <= ft_mul(ab2, eps2)) return false;
	*out = vec3_normalize(n);
	return true;
}

static bool gjk_line(joy_simplex3d_t *s, joy_vec3_t *dir) {
	joy_vec3_t a = s->points[0], b = s->points[1];
	joy_vec3_t ab = vec3_normalize(vec3_sub(b, a));
	joy_vec3_t ao = vec3_normalize(vec3_negate(a));
	if (same_direction(ab, ao)) {
		*dir = vec3_normalize(vec3_cross(vec3_cross(ab, ao), ab));
	} else {
		simplex_init1(s, a);
		*dir = ao;
	}
	return false;
}

static bool gjk_triangle(joy_simplex3d_t *s, joy_vec3_t *dir) {
	joy_vec3_t a = s->points[0], b = s->points[1], c = s->points[2];
	joy_vec3_t ab = vec3_normalize(vec3_sub(b, a));
	joy_vec3_t ac = vec3_normalize(vec3_sub(c, a));
	joy_vec3_t ao = vec3_normalize(vec3_negate(a));
	joy_vec3_t abc;
	if (!safe_cross_normalize(ab, ac, &abc)) {
		/* 三点近共线(退化三角形): 退化为线段处理 */
		simplex_init2(s, a, b);
		return gjk_line(s, dir);
	}

	if (same_direction(vec3_cross(abc, ac), ao)) {
		if (same_direction(ac, ao)) {
			simplex_init2(s, a, c);
			*dir = vec3_normalize(vec3_cross(vec3_cross(ac, ao), ac));
		} else {
			simplex_init2(s, a, b);
			return gjk_line(s, dir);
		}
	} else {
		if (same_direction(vec3_cross(ab, abc), ao)) {
			simplex_init2(s, a, b);
			return gjk_line(s, dir);
		} else {
			if (same_direction(abc, ao)) {
				*dir = abc;
			} else {
				simplex_init3(s, a, c, b);
				*dir = vec3_negate(abc);
			}
		}
	}
	return false;
}

static bool gjk_tetrahedron(joy_simplex3d_t *s, joy_vec3_t *dir) {
	joy_vec3_t a = s->points[0], b = s->points[1], c = s->points[2], d = s->points[3];
	joy_vec3_t ab = vec3_normalize(vec3_sub(b, a));
	joy_vec3_t ac = vec3_normalize(vec3_sub(c, a));
	joy_vec3_t ad = vec3_normalize(vec3_sub(d, a));
	joy_vec3_t ao = vec3_normalize(vec3_negate(a));
	joy_vec3_t abc, acd, adb;
	/* 各面退化(近共线)时跳过该分支, 避免法线为噪声方向 */
	bool has_abc = safe_cross_normalize(ab, ac, &abc);
	bool has_acd = safe_cross_normalize(ac, ad, &acd);
	bool has_adb = safe_cross_normalize(ad, ab, &adb);

	if (has_abc && same_direction(abc, ao)) { simplex_init3(s, a, b, c); return gjk_triangle(s, dir); }
	if (has_acd && same_direction(acd, ao)) { simplex_init3(s, a, c, d); return gjk_triangle(s, dir); }
	if (has_adb && same_direction(adb, ao)) { simplex_init3(s, a, d, b); return gjk_triangle(s, dir); }
	return true;
}

static bool next_simplex(joy_simplex3d_t *s, joy_vec3_t *dir) {
	switch (s->points_num) {
	case 2: return gjk_line(s, dir);
	case 3: return gjk_triangle(s, dir);
	case 4: return gjk_tetrahedron(s, dir);
	}
	return false;
}

/* ============ EPA (扩展多面体算法) ============ */

/* 计算所有面法线和最近面索引 */
static void get_face_normals(joy_vec3_buf_t *polytope, joy_size_buf_t *faces,
	joy_epa_face_buf_t *normals, size_t *min_triangle) {
	*min_triangle = 0;
	joy_ft_t min_distance = ft_max_value();

	for (size_t i = 0; i < faces->n; i += 3) {
		joy_vec3_t a = kv_A(*polytope, kv_A(*faces, i));
		joy_vec3_t b = kv_A(*polytope, kv_A(*faces, i + 1));
		joy_vec3_t c = kv_A(*polytope, kv_A(*faces, i + 2));

		joy_vec3_t ba = vec3_normalize(vec3_sub(b, a));
		joy_vec3_t ca = vec3_normalize(vec3_sub(c, a));
		joy_vec3_t n;
		if (!safe_cross_normalize(ba, ca, &n)) {
			/* 退化面(三点近共线): 法线退化为噪声方向, 跳过该面,
			   将其距离置为最大避免被选为最近面 */
			joy_epa_face_t face;
			face.normal = (joy_vec3_t){ ft_one(), ft_zero(), ft_zero() };
			face.distance = ft_max_value();
			kv_push(joy_epa_face_t, *normals, face);
			continue;
		}

		joy_ft_t dist = vec3_dot(n, a);
		if (dist < ft_zero()) { n = vec3_negate(n); dist = -dist; }

		joy_epa_face_t face;
		face.normal = n;
		face.distance = dist;
		kv_push(joy_epa_face_t, *normals, face);

		if (dist < min_distance) { min_distance = dist; *min_triangle = i / 3; }
	}
}

/* 添加不重复边(EPA用) */
static void add_unique_edge(joy_epa_edge_buf_t *edges, joy_size_buf_t *faces, size_t a, size_t b) {
	size_t fa = kv_A(*faces, a);
	size_t fb = kv_A(*faces, b);
	/* 查找是否存在反向边 */
	for (size_t i = 0; i < edges->n; i++) {
		if (kv_A(*edges, i).a == fb && kv_A(*edges, i).b == fa) {
			/* 找到反向边,移除(该边为内部边) */
			kv_A(*edges, i) = kv_A(*edges, edges->n - 1);
			edges->n--;
			return;
		}
	}
	joy_epa_edge_t edge = { fa, fb };
	kv_push(joy_epa_edge_t, *edges, edge);
}

/* EPA主函数: 从单纯形出发构建多面体,计算穿透法线和深度 */
static bool epa(joy_simplex3d_t *simplex, joy_gjk3d_collider_p ca, joy_gjk3d_collider_p cb, joy_gjk3d_contact_p contact) {
	joy_vec3_buf_t polytope;
	joy_size_buf_t faces;
	joy_epa_face_buf_t normals;

	kv_init(polytope);
	kv_init(faces);
	kv_init(normals);

	/* 将单纯形点复制到多面体 */
	for (int i = 0; i < simplex->points_num; i++) {
		kv_push(joy_vec3_t, polytope, simplex->points[i]);
	}

	/* 四面体的4个面(12个索引) */
	{
		size_t init_faces[] = { 0, 1, 2, 0, 3, 1, 0, 2, 3, 1, 3, 2 };
		for (int i = 0; i < 12; i++) {
			kv_push(size_t, faces, init_faces[i]);
		}
	}

	size_t min_face = 0;
	get_face_normals(&polytope, &faces, &normals, &min_face);

	/* 备份初始最小面 */
	joy_vec3_t bake_min_normal = kv_A(normals, min_face).normal;
	joy_ft_t bake_min_distance = kv_A(normals, min_face).distance;

	joy_vec3_t min_normal = { ft_zero(), ft_zero(), ft_zero() };
	joy_ft_t min_distance = ft_max_value();

	int epa_iter = 0;
	while (min_distance == ft_max_value() && epa_iter < 64) {
		epa_iter++;
		min_normal = kv_A(normals, min_face).normal;
		min_distance = kv_A(normals, min_face).distance;

		joy_vec3_t support = get_support_point(ca, cb, min_normal);
		joy_ft_t s_dist = vec3_dot(min_normal, support);

		if (ft_abs(ft_sub(s_dist, min_distance)) > 0.001f) {
			min_distance = ft_max_value();

			/* 找到需要移除的面,收集边界边 */
			joy_epa_edge_buf_t unique_edges;
			kv_init(unique_edges);

			for (size_t i = 0; i < normals.n; i++) {
				joy_vec3_t n = kv_A(normals, i).normal;
				size_t f = i * 3;
				if (vec3_dot(n, support) > vec3_dot(n, kv_A(polytope, kv_A(faces, f)))) {
					add_unique_edge(&unique_edges, &faces, f, f + 1);
					add_unique_edge(&unique_edges, &faces, f + 1, f + 2);
					add_unique_edge(&unique_edges, &faces, f + 2, f);

					/* 移除该面(用最后一个面替换) */
					kv_A(faces, f + 2) = kv_A(faces, faces.n - 1); faces.n--;
					kv_A(faces, f + 1) = kv_A(faces, faces.n - 1); faces.n--;
					kv_A(faces, f)     = kv_A(faces, faces.n - 1); faces.n--;
					kv_A(normals, i)   = kv_A(normals, normals.n - 1); normals.n--;
					i--; /* 重新检查当前索引 */
				}
			}

			/* 用边界边创建新面 */
			joy_size_buf_t new_faces;
			kv_init(new_faces);
			size_t new_vertex_index = polytope.n;
			for (size_t i = 0; i < unique_edges.n; i++) {
				kv_push(size_t, new_faces, kv_A(unique_edges, i).a);
				kv_push(size_t, new_faces, kv_A(unique_edges, i).b);
				kv_push(size_t, new_faces, new_vertex_index);
			}
			kv_push(joy_vec3_t, polytope, support);

			/* 计算新面的法线 */
			joy_epa_face_buf_t new_normals;
			kv_init(new_normals);
			size_t new_min_face = 0;
			get_face_normals(&polytope, &new_faces, &new_normals, &new_min_face);

			/* 找到全局最小距离的面 */
			joy_ft_t old_min_dist = ft_max_value();
			size_t global_min = 0;
			for (size_t i = 0; i < normals.n; i++) {
				if (kv_A(normals, i).distance < old_min_dist) {
					old_min_dist = kv_A(normals, i).distance;
					global_min = i;
				}
			}

			if (new_normals.n > 0) {
				if (kv_A(new_normals, new_min_face).distance < old_min_dist) {
					min_face = new_min_face + normals.n;
				}
			} else {
				min_normal = bake_min_normal;
				min_distance = bake_min_distance;
				break;
			}

			/* 合并新面到主列表 */
			for (size_t i = 0; i < new_faces.n; i++) {
				kv_push(size_t, faces, kv_A(new_faces, i));
			}
			for (size_t i = 0; i < new_normals.n; i++) {
				kv_push(joy_epa_face_t, normals, kv_A(new_normals, i));
			}

			kv_destroy(new_faces);
			kv_destroy(new_normals);
			kv_destroy(unique_edges);
		}
	}

	/* EPA后处理: 用一组候选方向重新验证穿透深度, 取最小值,
	   纠正EPA因退化/局部最优收敛到错误支撑面的情况 */
	{
		joy_vec3_t candidates[8];
		int n = 0;
		joy_vec3_t axis[3] = {
			{ ft_one(), ft_zero(), ft_zero() },
			{ ft_zero(), ft_one(), ft_zero() },
			{ ft_zero(), ft_zero(), ft_one() }
		};
		for (int i = 0; i < 3; i++) {
			candidates[n++] = axis[i];
			candidates[n++] = vec3_negate(axis[i]);
		}
		/* 两物体中心连线方向 */
		joy_vec3_t delta = vec3_sub(cb->position, ca->position);
		joy_ft_t dl = vec3_length(delta);
		if (dl > ft_zero()) {
			candidates[n++] = vec3_scale(delta, ft_div(ft_one(), dl));
			candidates[n++] = vec3_scale(vec3_negate(delta), ft_div(ft_one(), dl));
		}

		for (int i = 0; i < n; i++) {
			joy_vec3_t support = get_support_point(ca, cb, candidates[i]);
			joy_ft_t d = vec3_dot(candidates[i], support);
			if (d > ft_zero() && d < min_distance) {
				min_distance = d;
				min_normal = candidates[i];
			}
		}
	}

	contact->normal = min_normal;
	contact->depth = ft_add(min_distance, 0.001f);

	kv_destroy(polytope);
	kv_destroy(faces);
	kv_destroy(normals);

	return true;
}

/* ============ 退化兜底 ============ */

/* 当 GJK 因对称/共线退化无法构建四面体(如同心球、共线单纯形)时,
   用 ±3轴方向采样穿透距离: 任一方向分离(支持点投影 <= 0)则判定无碰撞,
   否则取最小穿透距离作为深度(法线为该轴方向) */
static bool degenerate_fallback(joy_gjk3d_collider_p ca, joy_gjk3d_collider_p cb, joy_gjk3d_contact_p contact) {
	joy_vec3_t axes[6] = {
		{  ft_one(), ft_zero(), ft_zero() },
		{ -ft_one(), ft_zero(), ft_zero() },
		{  ft_zero(),  ft_one(), ft_zero() },
		{  ft_zero(), -ft_one(), ft_zero() },
		{  ft_zero(), ft_zero(),  ft_one() },
		{  ft_zero(), ft_zero(), -ft_one() }
	};
	joy_ft_t min_d = ft_max_value();
	joy_vec3_t min_n = { ft_zero(), ft_zero(), ft_zero() };
	for (int i = 0; i < 6; i++) {
		joy_vec3_t s = get_support_point(ca, cb, axes[i]);
		joy_ft_t d = vec3_dot(axes[i], s);
		if (d <= ft_zero()) return false; /* 该方向分离 */
		if (d < min_d) { min_d = d; min_n = axes[i]; }
	}
	contact->normal = min_n;
	contact->depth = ft_add(min_d, 0.001f);
	return true;
}

/* ============ 公开API实现 ============ */
bool joy_math_gjk3d_collide(joy_gjk3d_collider_p c1, joy_gjk3d_collider_p c2,
	joy_vec3_t init_dir, joy_gjk3d_contact_p contact)
{
	joy_simplex3d_t simplex;
	memset(&simplex, 0, sizeof(simplex));

	joy_vec3_t support = get_support_point(c1, c2, init_dir);
	simplex_push(&simplex, support);
	joy_vec3_t dir = vec3_normalize(vec3_negate(support));

	joy_ft_t tiny2 = ft_from_float(1e-8f);
	int gjk_iter = 0;
	while (gjk_iter < 128) {
		gjk_iter++;
		/* 方向退化(零向量): 无法继续推进, 进入退化兜底 */
		if (vec3_square_length(dir) <= tiny2) {
			return degenerate_fallback(c1, c2, contact);
		}
		support = get_support_point(c1, c2, dir);
		if (vec3_dot(support, dir) < ft_zero()) {
			/* 无碰撞 */
			contact->normal.x = ft_zero();
			contact->normal.y = ft_zero();
			contact->normal.z = ft_zero();
			contact->depth = ft_zero();
			return false;
		}
		/* 支持点与已有点重合(不再推进): 对称退化, 兜底判定 */
		{
			bool dup = false;
			for (int i = 0; i < simplex.points_num; i++) {
				if (vec3_square_length(vec3_sub(support, simplex.points[i])) <= tiny2) { dup = true; break; }
			}
			if (dup) return degenerate_fallback(c1, c2, contact);
		}
		simplex_push(&simplex, support);
		if (next_simplex(&simplex, &dir)) {
			return epa(&simplex, c1, c2, contact);
		}
	}

	contact->normal.x = ft_zero();
	contact->normal.y = ft_zero();
	contact->normal.z = ft_zero();
	contact->depth = ft_zero();
	return false;
}

/* ============ 碰撞体初始化函数 ============ */

void joy_math_gjk3d_init_sphere(joy_gjk3d_collider_t *c, joy_vec3_t pos, joy_ft_t radius) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_SPHERE;
	c->position = pos;
	c->radius = radius;
}

void joy_math_gjk3d_init_box(joy_gjk3d_collider_t *c, joy_vec3_t pos, joy_vec3_t half_size) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_BOX;
	c->position = pos;
	c->half_size = half_size;
}

void joy_math_gjk3d_init_capsule(joy_gjk3d_collider_t *c, joy_vec3_t pos, joy_ft_t radius, joy_ft_t height, joy_vec3_t axis) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_CAPSULE;
	c->position = pos;
	c->radius = radius;
	c->height = height;
	c->axis = vec3_normalize(axis);
}

void joy_math_gjk3d_init_cylinder(joy_gjk3d_collider_t *c, joy_vec3_t pos, joy_ft_t radius, joy_ft_t height) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_CYLINDER;
	c->position = pos;
	c->radius = radius;
	c->height = height;
}

void joy_math_gjk3d_init_cone(joy_gjk3d_collider_t *c, joy_vec3_t pos, joy_ft_t radius, joy_ft_t height) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_CONE;
	c->position = pos;
	c->radius = radius;
	c->height = height;
}

void joy_math_gjk3d_init_mesh(joy_gjk3d_collider_t *c, joy_vec3_t *vertices, int num) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_MESH;
	c->vertices = vertices;
	c->vertices_num = num;
}

/* ======================================================================== */
/*                             GJK 2D 实现                                   */
/* ======================================================================== */

/* ---------- 内部类型 ---------- */

typedef struct {
	joy_vec2f_t points[3];
	int points_num;
} joy_simplex2d_t;

static void simplex2d_push(joy_simplex2d_t *s, joy_vec2f_t p) {
	s->points[2] = s->points[1];
	s->points[1] = s->points[0];
	s->points[0] = p;
	if (s->points_num < 3) s->points_num++;
}

static joy_vec2f_t simplex2d_a(const joy_simplex2d_t *s) { return s->points[0]; }
static joy_vec2f_t simplex2d_b(const joy_simplex2d_t *s) { return s->points[1]; }
static joy_vec2f_t simplex2d_c(const joy_simplex2d_t *s) { return s->points[2]; }

/* 2D向量三重积: b*(a·c) - a*(b·c) */
static joy_vec2f_t vec2f_triple(joy_vec2f_t a, joy_vec2f_t b, joy_vec2f_t c) {
	joy_fp_t ac = vec2f_dot(a, c);
	joy_fp_t bc = vec2f_dot(b, c);
	return vec2f_sub(vec2f_scale(b, ac), vec2f_scale(a, bc));
}

static joy_vec2f_t perp_cw(joy_vec2f_t v) {
	joy_vec2f_t r;
	r.x = v.y;
	r.y = -v.x;
	return r;
}

/* ---------- 各碰撞体的最远点计算 ---------- */

static joy_vec2f_t find_furthest_point_2d(joy_gjk2d_collider_p c, joy_vec2f_t dir) {
	joy_vec2f_t result = { fp_zero(), fp_zero() };

	switch (c->type) {
	case GJK2D_CTYPE_CIRCLE: {
		joy_fp_t len = vec2f_length(dir);
		if (len < fp_from_float(1e-8f)) { result = c->position; break; }
		joy_vec2f_t ndir = vec2f_scale(dir, fp_div(fp_one(), len));
		result = vec2f_add(c->position, vec2f_scale(ndir, c->radius));
		break;
	}
	case GJK2D_CTYPE_BOX: {
		joy_fp_t sx = dir.x > fp_zero() ? c->half_size.x : -c->half_size.x;
		joy_fp_t sy = dir.y > fp_zero() ? c->half_size.y : -c->half_size.y;
		result.x = fp_add(c->position.x, sx);
		result.y = fp_add(c->position.y, sy);
		break;
	}
	case GJK2D_CTYPE_CAPSULE: {
		joy_fp_t len = vec2f_length(dir);
		if (len == fp_zero()) { result = c->position; break; }
		joy_vec2f_t ndir = vec2f_scale(dir, fp_div(fp_one(), len));
		joy_fp_t dot = vec2f_dot(ndir, c->axis);
		joy_fp_t half_h = dot > fp_zero() ? fp_mul(fp_half(), c->height) : fp_mul(-fp_half(), c->height);
		joy_vec2f_t center = vec2f_add(c->position, vec2f_scale(c->axis, half_h));
		result = vec2f_add(center, vec2f_scale(ndir, c->radius));
		break;
	}
	case GJK2D_CTYPE_POLYGON: {
		joy_fp_t max_dot = fp_min_value();
		for (int i = 0; i < c->vertices_num; i++) {
			joy_fp_t d = vec2f_dot(c->vertices[i], dir);
			if (d > max_dot) { max_dot = d; result = c->vertices[i]; }
		}
		break;
	}
	case GJK2D_CTYPE_PIE: {
		/* 扇形: 朝向 +X, 半角 sweep/2 */
		joy_fp_t h = fp_mul(c->sweep, fp_half());
		joy_fp_t ch = fp_cos(h), sh = fp_sin(h);
		joy_fp_t zero = fp_zero();
		joy_vec2f_t best = c->position;
		joy_fp_t max_dot = vec2f_dot(best, dir);

		/* 检查弧上点 */
		joy_fp_t len = vec2f_length(dir);
		if (len > zero) {
			joy_vec2f_t ndir = vec2f_scale(dir, fp_div(fp_one(), len));
			/* 方向在锥内: dir.x>0 且 |dir.y| <= dir.x * tan(h) = dir.x * sh/ch */
			joy_fp_t dy_abs = fp_abs(ndir.y);
			joy_fp_t dx_tan = fp_mul(ndir.x, fp_div(sh, ch));
			if (ndir.x > zero && dy_abs <= dx_tan) {
				joy_vec2f_t arcPt = vec2f_add(c->position, vec2f_scale(ndir, c->radius));
				joy_fp_t d = vec2f_dot(arcPt, dir);
				if (d > max_dot) { max_dot = d; best = arcPt; }
			}
		}

		/* 检查两个角点 */
		joy_vec2f_t corners[2] = {
			{ fp_add(c->position.x, fp_mul(c->radius, ch)), fp_add(c->position.y, fp_mul(c->radius, sh)) },
			{ fp_add(c->position.x, fp_mul(c->radius, ch)), fp_sub(c->position.y, fp_mul(c->radius, sh)) }
		};
		for (int i = 0; i < 2; i++) {
			joy_fp_t d = vec2f_dot(corners[i], dir);
			if (d > max_dot) { max_dot = d; best = corners[i]; }
		}
		result = best;
		break;
	}
	}
	return result;
}

/* ---------- Minkowski差支持函数 ---------- */

static joy_vec2f_t get_support_point_2d(joy_gjk2d_collider_p ca, joy_gjk2d_collider_p cb, joy_vec2f_t dir) {
	joy_vec2f_t a = find_furthest_point_2d(ca, dir);
	joy_vec2f_t b = find_furthest_point_2d(cb, vec2f_negate(dir));
	return vec2f_sub(a, b);
}

/* ---------- 2D单纯形检测 ---------- */

static bool simplex2d_check(joy_simplex2d_t *s, joy_vec2f_t *dir) {
	joy_vec2f_t a = simplex2d_a(s);
	joy_vec2f_t ao = vec2f_negate(a);    /* 从a指向原点 */
	joy_vec2f_t ab = vec2f_sub(simplex2d_b(s), a);

	if (s->points_num == 2) {
		*dir = vec2f_triple(ab, ao, ab);
		joy_fp_t m2 = vec2f_dot(*dir, *dir);
		if (m2 < fp_from_float(1.2e-7f)) {  /* C# MathX.EPSILON≈1.19e-7 */
			*dir = perp_cw(ab);
		}
		*dir = vec2f_normalize(*dir);
	} else if (s->points_num == 3) {
		joy_vec2f_t ac = vec2f_sub(simplex2d_c(s), a);

		/* v = triple(ab, ac, ac): 垂直于ac的向量,指向ab侧 */
		joy_vec2f_t v = vec2f_triple(ab, ac, ac);
		if (vec2f_dot(v, ao) >= fp_zero()) {
			/* 原点在ac外侧, 丢弃b, 保留 {a, c} */
			s->points[1] = s->points[2];
			*dir = vec2f_normalize(v);
		} else {
			/* u = triple(ac, ab, ab): 垂直于ab的向量,指向ac侧 */
			joy_vec2f_t u = vec2f_triple(ac, ab, ab);
			if (vec2f_dot(u, ao) < fp_zero()) {
				/* 原点在三角形内部 → 碰撞！ */
				return true;
			}
			/* 原点在ab外侧, 丢弃c, 保留 {a, b} */
			*dir = vec2f_normalize(u);
		}
		/* 单纯形: points[0]=a(最新), points[1]=b, points[2]=c(最旧) */
		/* pn-- 后, [0]和[1]即为所需保留的两个点 */
		s->points_num--;
	}
	return false;
}

/* ---------- EPA (2D版 - 边扩展) ---------- */

typedef struct {
	joy_vec2f_t p1, p2;
	joy_vec2f_t normal;
	joy_fp_t distance;
} joy_epa2d_edge_t;

/* 确定缠绕方向 */
static int get_winding_2d(const joy_simplex2d_t *s) {
	joy_fp_t total = fp_zero();
	for (int i = 0; i < s->points_num; i++) {
		joy_vec2f_t p1 = s->points[i];
		joy_vec2f_t p2 = s->points[(i + 1) % s->points_num];
		total = fp_add(total, vec2f_cross(p1, p2));
	}
	return total > fp_zero() ? 1 : -1;
}

static void build_edges_2d(joy_epa2d_edge_t *edges, int *n, const joy_simplex2d_t *s, int winding) {
	for (int i = 0; i < s->points_num; i++) {
		joy_vec2f_t p1 = s->points[i];
		joy_vec2f_t p2 = s->points[(i + 1) % s->points_num];
		/* 外向法线 */
		joy_vec2f_t d = vec2f_sub(p2, p1);
		joy_vec2f_t nml;
		if (winding > 0) {
			/* 顺时针: CW90 (x,y)->(y,-x) */
			nml.x = d.y;
			nml.y = -d.x;
		} else {
			/* 逆时针: CCW90 (x,y)->(-y,x) */
			nml.x = -d.y;
			nml.y = d.x;
		}
		joy_fp_t nl = vec2f_length(nml);
		if (nl > fp_zero()) {
			nml = vec2f_scale(nml, fp_div(fp_one(), nl));
		}
		joy_fp_t dist = fp_abs(vec2f_dot(p1, nml));
		edges[*n].p1 = p1;
		edges[*n].p2 = p2;
		edges[*n].normal = nml;
		edges[*n].distance = dist;
		(*n)++;
	}
}

/* 按距离排序 */
static void sort_edges_2d(joy_epa2d_edge_t *edges, int n) {
	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < n - 1 - i; j++) {
			if (edges[j].distance > edges[j + 1].distance) {
				joy_epa2d_edge_t tmp = edges[j];
				edges[j] = edges[j + 1];
				edges[j + 1] = tmp;
			}
		}
	}
}

static bool epa2d(joy_simplex2d_t *simplex, joy_gjk2d_collider_p ca, joy_gjk2d_collider_p cb, joy_gjk2d_contact_p contact) {
	if (simplex->points_num < 3) return false;

	int winding = get_winding_2d(simplex);

	joy_epa2d_edge_t edges[3 * 64]; /* 最多64次迭代,每次最多增加2条边 */
	int edge_count = 0;
	build_edges_2d(edges, &edge_count, simplex, winding);
	sort_edges_2d(edges, edge_count);

	joy_vec2f_t best_normal = edges[0].normal;
	joy_fp_t best_dist = edges[0].distance;
	joy_fp_t eps = fp_from_float(0.001f);

	for (int iter = 0; iter < 64; iter++) {
		joy_vec2f_t support = get_support_point_2d(ca, cb, edges[0].normal);
		joy_fp_t s_dist = vec2f_dot(support, edges[0].normal);

		if (fp_abs(fp_sub(s_dist, edges[0].distance)) <= eps) {
			best_normal = edges[0].normal;
			best_dist = s_dist;
			break;
		}

		/* 用新点扩展: 将最近的边替换为两条新边 */
		joy_vec2f_t p1 = edges[0].p1;
		joy_vec2f_t p2 = edges[0].p2;
		edges[0] = edges[edge_count - 1];
		edge_count--;

		/* 添加边 p1→support */
		{
			joy_vec2f_t d = vec2f_sub(support, p1);
			joy_vec2f_t nml;
			if (winding > 0) {
				nml.x = d.y; nml.y = -d.x;
			} else {
				nml.x = -d.y; nml.y = d.x;
			}
			joy_fp_t nl = vec2f_length(nml);
			if (nl > fp_zero()) nml = vec2f_scale(nml, fp_div(fp_one(), nl));
			edges[edge_count].p1 = p1;
			edges[edge_count].p2 = support;
			edges[edge_count].normal = nml;
			edges[edge_count].distance = fp_abs(vec2f_dot(p1, nml));
			edge_count++;
		}

		/* 添加边 support→p2 */
		{
			joy_vec2f_t d = vec2f_sub(p2, support);
			joy_vec2f_t nml;
			if (winding > 0) {
				nml.x = d.y; nml.y = -d.x;
			} else {
				nml.x = -d.y; nml.y = d.x;
			}
			joy_fp_t nl = vec2f_length(nml);
			if (nl > fp_zero()) nml = vec2f_scale(nml, fp_div(fp_one(), nl));
			edges[edge_count].p1 = support;
			edges[edge_count].p2 = p2;
			edges[edge_count].normal = nml;
			edges[edge_count].distance = fp_abs(vec2f_dot(support, nml));
			edge_count++;
		}

		sort_edges_2d(edges, edge_count);
	}

	contact->normal = best_normal;
	contact->depth = fp_add(best_dist, eps);
	return true;
}

/* ============ 公开API实现 ============ */

bool joy_math_gjk2d_collide(joy_gjk2d_collider_p c1, joy_gjk2d_collider_p c2,
	joy_vec2f_t init_dir, joy_gjk2d_contact_p contact)
{
	joy_simplex2d_t simplex;
	memset(&simplex, 0, sizeof(simplex));

	joy_vec2f_t dir = init_dir;
	if (vec2f_length_squared(dir) < fp_from_float(1.2e-7f)) {
		dir.x = fp_one();
	}

	joy_vec2f_t support = get_support_point_2d(c1, c2, dir);
	simplex2d_push(&simplex, support);

	if (vec2f_dot(support, dir) <= fp_zero()) {
		contact->normal.x = fp_zero();
		contact->normal.y = fp_zero();
		contact->depth = fp_zero();
		return false;
	}

	dir = vec2f_negate(dir);

	for (int i = 0; i < 128; i++) {
		/* 确保方向为单位向量,防止极小方向导致数值问题 */
		joy_fp_t dlen = vec2f_length(dir);
		if (dlen < fp_from_float(1e-8f)) { dir.x = fp_one(); dir.y = fp_zero(); }
		else dir = vec2f_scale(dir, fp_div(fp_one(), dlen));

		support = get_support_point_2d(c1, c2, dir);
		simplex2d_push(&simplex, support);

		if (vec2f_dot(support, dir) <= fp_zero()) {
			contact->normal.x = fp_zero();
			contact->normal.y = fp_zero();
			contact->depth = fp_zero();
			return false;
		}

		if (simplex2d_check(&simplex, &dir)) {
			return epa2d(&simplex, c1, c2, contact);
		}
	}

	contact->normal.x = fp_zero();
	contact->normal.y = fp_zero();
	contact->depth = fp_zero();
	return false;
}

void joy_math_gjk2d_init_circle(joy_gjk2d_collider_t *c, joy_vec2f_t pos, joy_fp_t radius) {
	memset(c, 0, sizeof(*c));
	c->type = GJK2D_CTYPE_CIRCLE;
	c->position = pos;
	c->radius = radius;
}

void joy_math_gjk2d_init_box(joy_gjk2d_collider_t *c, joy_vec2f_t pos, joy_vec2f_t half_size) {
	memset(c, 0, sizeof(*c));
	c->type = GJK2D_CTYPE_BOX;
	c->position = pos;
	c->half_size = half_size;
}

void joy_math_gjk2d_init_capsule(joy_gjk2d_collider_t *c, joy_vec2f_t pos, joy_vec2f_t axis, joy_fp_t height, joy_fp_t radius) {
	memset(c, 0, sizeof(*c));
	c->type = GJK2D_CTYPE_CAPSULE;
	c->position = pos;
	c->axis = vec2f_normalize(axis);
	c->height = height;
	c->radius = radius;
}

void joy_math_gjk2d_init_polygon(joy_gjk2d_collider_t *c, joy_vec2f_t *vertices, int num) {
	memset(c, 0, sizeof(*c));
	c->type = GJK2D_CTYPE_POLYGON;
	c->vertices = vertices;
	c->vertices_num = num;
}

void joy_math_gjk2d_init_pie(joy_gjk2d_collider_t *c, joy_vec2f_t pos, joy_fp_t radius, joy_fp_t sweep) {
	memset(c, 0, sizeof(*c));
	c->type = GJK2D_CTYPE_PIE;
	c->position = pos;
	c->radius = radius;
	c->sweep = sweep;
}

/* ==================== 结束 src/jgjk.c ==================== */

/* ==================== 合并自 src/jbox2d.c ==================== */
#include "external/klib/khash.h"
#include "external/klib/klist.h"
#include "external/klib/kvec.h"
#include "joy.h"

// --- klib container types ---

#define kl_no_free(x)
#define fp_negate(x)    (-(x))
#define fp_equal(a,b)   ((a)==(b))
#define fp_less(a,b)    ((a)<(b))
#define fp_greater(a,b) ((a)>(b))
#define fp_lequal(a,b)  ((a)<=(b))
#define fp_gequal(a,b)  ((a)>=(b))
KHASH_INIT(bodies_map, int, joy_box2d_body_t*, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(arbiters_map, int64_t, joy_box2d_arbiter_t*, 1, kh_int64_hash_func, kh_int64_hash_equal)
KLIST_INIT(joints_list, joy_box2d_joint_t*, kl_no_free)


// Box vertex and edge numbering:
//
//        ^ y
//        |
//        e1
//   v2 ------ v1
//    |        |
// e2 |        | e4  --> x
//    |        |
//   v3 ------ v4
//        e3


typedef enum joy_box2d_axis_e {
	FACE_A_X,
	FACE_A_Y,
	FACE_B_X,
	FACE_B_Y
} joy_box2d_axis_e;

typedef enum joy_box2d_edge_e {
	NO_EDGE = 0,
	EDGE1,
	EDGE2,
	EDGE3,
	EDGE4
} joy_box2d_edge_e;


typedef union joy_box2d_feature_t {
	struct joy_box2d_edges_t {
		char in_edge1;
		char out_edge1;
		char in_edge2;
		char out_edge2;
	} e;
	int value;
} joy_box2d_feature_t;


typedef struct joy_box2d_contact_t {
	joy_vec2f_t position;
	joy_vec2f_t normal;
	joy_fp_t separation;
	joy_box2d_feature_t feature;
} joy_box2d_contact_t;


typedef struct joy_box2d_clip_vertex_t {
	joy_vec2f_t v;
	joy_box2d_feature_t fp;
} joy_box2d_clip_vertex_t;


static inline void
_flip(joy_box2d_feature_t* fp) {
	char tmp = fp->e.in_edge1;
	fp->e.in_edge1 = fp->e.in_edge2;
	fp->e.in_edge2 = tmp;
	tmp = fp->e.out_edge1;
	fp->e.out_edge1 = fp->e.out_edge2;
	fp->e.out_edge2 = tmp;
}

static int
_clip_segment_to_line(joy_box2d_clip_vertex_t vout[2], joy_box2d_clip_vertex_t vin[2],
	const joy_vec2f_t* normal, joy_fp_t offset, char clip_edge) {
	int num_out = 0;
	joy_vec2f_t tmp;

	joy_fp_t distance0 = fp_sub(vec2f_dot(*normal, vin[0].v), offset);
	joy_fp_t distance1 = fp_sub(vec2f_dot(*normal, vin[1].v), offset);

	if (fp_lequal(distance0, fp_zero())) vout[num_out++] = vin[0];
	if (fp_lequal(distance1, fp_zero())) vout[num_out++] = vin[1];

	if (fp_less(fp_mul(distance0, distance1), fp_zero())) {
		joy_fp_t interp = fp_div(distance0, fp_sub(distance0, distance1));
		tmp = vec2f_sub(vin[1].v, vin[0].v);
		tmp = vec2f_scale(tmp, interp);
		vout[num_out].v = vec2f_add(vin[0].v, tmp);
		if (fp_greater(distance0, fp_zero())) {
			vout[num_out].fp = vin[0].fp;
			vout[num_out].fp.e.in_edge1 = clip_edge;
			vout[num_out].fp.e.in_edge2 = NO_EDGE;
		}
		else {
			vout[num_out].fp = vin[1].fp;
			vout[num_out].fp.e.out_edge1 = clip_edge;
			vout[num_out].fp.e.out_edge2 = NO_EDGE;
		}
		++num_out;
	}

	return num_out;
}

static void
_compute_incident_edge(joy_box2d_clip_vertex_t c[2], const joy_vec2f_t* h, const joy_vec2f_t* pos,
	const joy_mat22f_t* rot, const joy_vec2f_t* normal) {
	joy_vec2f_t tmp;
	joy_mat22f_t rotT = mat22f_transpose(*rot);
	tmp = mat22f_mul_vec2f(rotT, *normal);
	joy_vec2f_t n = vec2f_negate(tmp);
	joy_vec2f_t abs_n = vec2f_abs(n);

	if (fp_greater(abs_n.x, abs_n.y)) {
		if (fp_sign(n.x) > 0) {
			c[0].v.x = h->x;
			c[0].v.y = fp_negate(h->y);
			c[0].fp.e.in_edge2 = EDGE3;
			c[0].fp.e.out_edge2 = EDGE4;

			c[1].v = *h;
			c[1].fp.e.in_edge2 = EDGE4;
			c[1].fp.e.out_edge2 = EDGE1;
		}
		else {
			c[0].v.x = fp_negate(h->x);
			c[0].v.y = h->y;
			c[0].fp.e.in_edge2 = EDGE1;
			c[0].fp.e.out_edge2 = EDGE2;

			c[1].v = vec2f_negate(*h);
			c[1].fp.e.in_edge2 = EDGE2;
			c[1].fp.e.out_edge2 = EDGE3;
		}
	}
	else {
		if (fp_sign(n.y) > 0) {
			c[0].v = *h;
			c[0].fp.e.in_edge2 = EDGE4;
			c[0].fp.e.out_edge2 = EDGE1;

			c[1].v.x = fp_negate(h->x);
			c[1].v.y = h->y;
			c[1].fp.e.in_edge2 = EDGE1;
			c[1].fp.e.out_edge2 = EDGE2;
		}
		else {
			c[0].v = vec2f_negate(*h);
			c[0].fp.e.in_edge2 = EDGE2;
			c[0].fp.e.out_edge2 = EDGE3;

			c[1].v.x = h->x;
			c[1].v.y = fp_negate(h->y);
			c[1].fp.e.in_edge2 = EDGE3;
			c[1].fp.e.out_edge2 = EDGE4;
		}
	}

	tmp = mat22f_mul_vec2f(*rot, c[0].v);
	c[0].v = vec2f_add(*pos, tmp);
	tmp = mat22f_mul_vec2f(*rot, c[1].v);
	c[1].v = vec2f_add(*pos, tmp);
}


// The normal points from A to B
static int
_collide(joy_box2d_contact_t* contacts, joy_box2d_body_t* a_body, joy_box2d_body_t* b_body) {
	joy_vec2f_t tmp;

	joy_fp_t half = fp_half();
	joy_vec2f_t a_h = vec2f_scale(a_body->bounds, half);
	joy_vec2f_t b_h = vec2f_scale(b_body->bounds, half);

	joy_vec2f_t a_pos = a_body->position;
	joy_vec2f_t b_pos = b_body->position;

	joy_mat22f_t a_rot = mat22f_rotate(a_body->rotation);
	joy_mat22f_t b_rot = mat22f_rotate(b_body->rotation);

	joy_mat22f_t a_rotT = mat22f_transpose(a_rot);
	joy_mat22f_t b_rotT = mat22f_transpose(b_rot);

	joy_vec2f_t dp = vec2f_sub(b_pos, a_pos);

	joy_vec2f_t a_d = mat22f_mul_vec2f(a_rotT, dp);
	joy_vec2f_t b_d = mat22f_mul_vec2f(b_rotT, dp);

	joy_mat22f_t c = mat22f_mul(a_rotT, b_rot);
	joy_mat22f_t abs_c = mat22f_abs(c);
	joy_mat22f_t abs_ct = mat22f_transpose(abs_c);

	// Box A faces
	joy_vec2f_t a_abs_d = vec2f_abs(a_d);
	tmp = vec2f_sub(a_abs_d, a_h);
	joy_vec2f_t tmp2 = mat22f_mul_vec2f(abs_c, b_h);
	joy_vec2f_t a_face = vec2f_sub(tmp, tmp2);
	if (fp_greater(a_face.x, fp_zero()) || fp_greater(a_face.y, fp_zero()))
		return 0;

	// Box B faces
	joy_vec2f_t abs_db = vec2f_abs(b_d);
	tmp = mat22f_mul_vec2f(abs_ct, a_h);
	tmp = vec2f_sub(abs_db, tmp);
	joy_vec2f_t b_face = vec2f_sub(tmp, b_h);
	if (fp_greater(b_face.x, fp_zero()) || fp_greater(b_face.y, fp_zero()))
		return 0;

	// Find best axis
	joy_box2d_axis_e axis = FACE_A_X;
	joy_fp_t separation = a_face.x;

	joy_vec2f_t normal = fp_greater(a_d.x, fp_zero()) ? a_rot.col1 : vec2f_negate(a_rot.col1);

	const joy_fp_t relative_tol = fp_from_float(0.95f);
	const joy_fp_t absolute_tol = fp_from_float(0.01f);

	if (fp_greater(a_face.y, fp_add(fp_mul(relative_tol, separation), fp_mul(absolute_tol, a_h.y)))) {
		axis = FACE_A_Y;
		separation = a_face.y;
		normal = fp_greater(a_d.y, fp_zero()) ? a_rot.col2 : vec2f_negate(a_rot.col2);
	}

	if (fp_greater(b_face.x, fp_add(fp_mul(relative_tol, separation), fp_mul(absolute_tol, b_h.x)))) {
		axis = FACE_B_X;
		separation = b_face.x;
		normal = fp_greater(b_d.x, fp_zero()) ? b_rot.col1 : vec2f_negate(b_rot.col1);
	}

	if (fp_greater(b_face.y, fp_add(fp_mul(relative_tol, separation), fp_mul(absolute_tol, b_h.y)))) {
		axis = FACE_B_Y;
		separation = b_face.y;
		normal = fp_greater(b_d.y, fp_zero()) ? b_rot.col2 : vec2f_negate(b_rot.col2);
	}

	// Setup clipping plane data based on the separating axis
	joy_vec2f_t front_normal, side_normal;
	joy_box2d_clip_vertex_t incident_edge[2] = { { 0 } };
	joy_fp_t front, neg_side, pos_side;
	char neg_edge, pos_edge;

	switch (axis) {
	case FACE_A_X: {
		front_normal = normal;
		front = fp_add(vec2f_dot(a_pos, front_normal), a_h.x);
		side_normal = a_rot.col2;
		joy_fp_t side = vec2f_dot(a_pos, side_normal);
		neg_side = fp_add(fp_negate(side), a_h.y);
		pos_side = fp_add(side, a_h.y);
		neg_edge = EDGE3;
		pos_edge = EDGE1;
		_compute_incident_edge(incident_edge, &b_h, &b_pos, &b_rot, &front_normal);
	}
				 break;

	case FACE_A_Y: {
		front_normal = normal;
		front = fp_add(vec2f_dot(a_pos, front_normal), a_h.y);
		side_normal = a_rot.col1;
		joy_fp_t side = vec2f_dot(a_pos, side_normal);
		neg_side = fp_add(fp_negate(side), a_h.x);
		pos_side = fp_add(side, a_h.x);
		neg_edge = EDGE2;
		pos_edge = EDGE4;
		_compute_incident_edge(incident_edge, &b_h, &b_pos, &b_rot, &front_normal);
	}
				 break;

	case FACE_B_X: {
		front_normal = vec2f_negate(normal);
		front = fp_add(vec2f_dot(b_pos, front_normal), b_h.x);
		side_normal = b_rot.col2;
		joy_fp_t side = vec2f_dot(b_pos, side_normal);
		neg_side = fp_add(fp_negate(side), b_h.y);
		pos_side = fp_add(side, b_h.y);
		neg_edge = EDGE3;
		pos_edge = EDGE1;
		_compute_incident_edge(incident_edge, &a_h, &a_pos, &a_rot, &front_normal);
	}
				 break;

	case FACE_B_Y: {
		front_normal = vec2f_negate(normal);
		front = fp_add(vec2f_dot(b_pos, front_normal), b_h.y);
		side_normal = b_rot.col1;
		joy_fp_t side = vec2f_dot(b_pos, side_normal);
		neg_side = fp_add(fp_negate(side), b_h.x);
		pos_side = fp_add(side, b_h.x);
		neg_edge = EDGE2;
		pos_edge = EDGE4;
		_compute_incident_edge(incident_edge, &a_h, &a_pos, &a_rot, &front_normal);
	}
				 break;
	}

	joy_box2d_clip_vertex_t clip_points1[2] = { { 0 } };
	joy_box2d_clip_vertex_t clip_points2[2] = { { 0 } };
	int np;

	joy_vec2f_t neg_side_normal = vec2f_negate(side_normal);
	np = _clip_segment_to_line(clip_points1, incident_edge, &neg_side_normal, neg_side, neg_edge);

	if (np < 2) return 0;

	np = _clip_segment_to_line(clip_points2, clip_points1, &side_normal, pos_side, pos_edge);

	if (np < 2) return 0;

	int num_contacts = 0;
	for (int i = 0; i < 2; ++i) {
		joy_fp_t sep = fp_sub(vec2f_dot(front_normal, clip_points2[i].v), front);
		if (fp_lequal(sep, fp_zero())) {
			contacts[num_contacts].separation = sep;
			contacts[num_contacts].normal = normal;
			tmp = vec2f_scale(front_normal, sep);
			contacts[num_contacts].position = vec2f_sub(clip_points2[i].v, tmp);
			contacts[num_contacts].feature = clip_points2[i].fp;

			if (axis == FACE_B_X || axis == FACE_B_Y) {
				_flip(&contacts[num_contacts].feature);
			}
			++num_contacts;
		}
	}

	return num_contacts;
}


// Wrapper for vec2f_cross3 (jmath.h has (vec, scalar) order, code uses (scalar, vec))
static inline joy_vec2f_t
v3(joy_fp_t s, joy_vec2f_t a) {
	return vec2f_cross3(a, s);
}

static void
_prestep(joy_box2d_joint_t* joint, joy_box2d_body_t* body1, joy_box2d_body_t* body2, joy_fp_t inv_dt) {
	joy_mat22f_t Rot1 = mat22f_rotate(body1->rotation);
	joy_mat22f_t Rot2 = mat22f_rotate(body2->rotation);

	joint->r1 = mat22f_mul_vec2f(Rot1, joint->local_anchor1);
	joint->r2 = mat22f_mul_vec2f(Rot2, joint->local_anchor2);

	joy_mat22f_t K1;
	K1.col1.x = fp_add(body1->inv_mass, body2->inv_mass);
	K1.col2.x = fp_zero();
	K1.col1.y = fp_zero();
	K1.col2.y = K1.col1.x;

	joy_mat22f_t K2;
	joy_fp_t neg_invI1 = fp_negate(body1->invI);
	K2.col1.x = fp_mul(body1->invI, fp_pow2(joint->r1.y));
	K2.col2.x = fp_mul(neg_invI1, fp_mul(joint->r1.x, joint->r1.y));
	K2.col1.y = fp_mul(neg_invI1, fp_mul(joint->r1.x, joint->r1.y));
	K2.col2.y = fp_mul(body1->invI, fp_pow2(joint->r1.x));

	joy_mat22f_t K3;
	joy_fp_t neg_invI2 = fp_negate(body2->invI);
	K3.col1.x = fp_mul(fp_mul(body2->invI, joint->r2.y), joint->r2.y);
	K3.col2.x = fp_mul(fp_mul(neg_invI2, joint->r2.x), joint->r2.y);
	K3.col1.y = fp_mul(fp_mul(neg_invI2, joint->r2.x), joint->r2.y);
	K3.col2.y = fp_mul(fp_mul(body2->invI, joint->r2.x), joint->r2.x);

	joy_mat22f_t K = mat22f_add(K1, K2);
	K = mat22f_add(K, K3);
	K.col1.x = fp_add(K.col1.x, joint->softness);
	K.col2.y = fp_add(K.col2.y, joint->softness);

	joint->M = mat22f_invert(K);

	joy_vec2f_t p1 = vec2f_add(body1->position, joint->r1);
	joy_vec2f_t p2 = vec2f_add(body2->position, joint->r2);
	joy_vec2f_t dp = vec2f_sub(p2, p1);

	if (JOY_POSITION_CORRECTION) {
		joy_fp_t bias_factor = fp_mul(fp_negate(joint->biasfactor), inv_dt);
		joint->bias = vec2f_scale(dp, bias_factor);
	}
	else {
		joint->bias.x = fp_zero();
		joint->bias.y = fp_zero();
	}

	if (JOY_WARM_STARTING) {
		joy_vec2f_t tmp2;
		tmp2 = vec2f_scale(joint->p, body1->inv_mass);
		body1->velocity = vec2f_sub(body1->velocity, tmp2);
		body1->angular_velocity = fp_sub(body1->angular_velocity, fp_mul(body1->invI, vec2f_cross(joint->r1, joint->p)));

		tmp2 = vec2f_scale(joint->p, body2->inv_mass);
		body2->velocity = vec2f_add(body2->velocity, tmp2);
		body2->angular_velocity = fp_add(body2->angular_velocity, fp_mul(body2->invI, vec2f_cross(joint->r2, joint->p)));
	}
	else {
		joint->p.x = fp_zero();
		joint->p.y = fp_zero();
	}
}


static void
_joint_apply_impulse(joy_box2d_joint_t* joint, joy_box2d_body_t* body1, joy_box2d_body_t* body2) {
	joy_vec2f_t tmp2;
	tmp2 = v3(body2->angular_velocity, joint->r2);
	tmp2 = vec2f_add(body2->velocity, tmp2);
	tmp2 = vec2f_sub(tmp2, body1->velocity);
	joy_vec2f_t r1a = v3(body1->angular_velocity, joint->r1);
	joy_vec2f_t dv = vec2f_sub(tmp2, r1a);
	tmp2 = vec2f_sub(joint->bias, dv);
	joy_vec2f_t ps = vec2f_scale(joint->p, joint->softness);
	joy_vec2f_t force = vec2f_sub(tmp2, ps);
	joy_vec2f_t impulse = mat22f_mul_vec2f(joint->M, force);

	tmp2 = vec2f_scale(impulse, body1->inv_mass);
	body1->velocity = vec2f_sub(body1->velocity, tmp2);
	body1->angular_velocity = fp_sub(body1->angular_velocity, fp_mul(body1->invI, vec2f_cross(joint->r1, impulse)));

	tmp2 = vec2f_scale(impulse, body2->inv_mass);
	body2->velocity = vec2f_add(body2->velocity, tmp2);
	body2->angular_velocity = fp_add(body2->angular_velocity, fp_mul(body2->invI, vec2f_cross(joint->r2, impulse)));

	joint->p = vec2f_add(joint->p, impulse);
}

static joy_box2d_arbiter_t*
_arbiter_create(joy_box2d_arbiter_key_t key, joy_fp_t friction, joy_box2d_manifold_t* manifolds, int num) {
	joy_box2d_arbiter_t* arbiter = (joy_box2d_arbiter_t*)malloc(sizeof(joy_box2d_arbiter_t));
	assert(arbiter);
	arbiter->id1 = key.id_pair.id1;
	arbiter->id2 = key.id_pair.id2;
	for (int i = 0; i < num; i++) {
		arbiter->manifolds[i] = manifolds[i];
	}
	arbiter->friction = friction;
	arbiter->num_manifolds = num;
	return arbiter;
}

static void
_arbiter_destroy(joy_box2d_arbiter_t* arbiter) {
	free(arbiter);
}

static void
_arbiter_update(joy_box2d_arbiter_t* arbiter, joy_box2d_manifold_t* manifolds, int num) {
	for (int i = 0; i < num; ++i) {
		joy_box2d_manifold_t* c_new = manifolds + i;
		int k = -1;
		for (int j = 0; j < arbiter->num_manifolds; ++j) {
			joy_box2d_manifold_t* c_old = arbiter->manifolds + j;
			if (c_new->feature == c_old->feature) {
				k = j;
				break;
			}
		}

		if (k > -1) {
			joy_box2d_manifold_t* c = manifolds + i;
			joy_box2d_manifold_t* c_old = arbiter->manifolds + k;
			if (JOY_WARM_STARTING) {
				c->pn = c_old->pn;
				c->pt = c_old->pt;
			}
			else {
				c->pn = fp_zero();
				c->pt = fp_zero();
			}
		}
	}

	for (int i = 0; i < num; ++i) {
		arbiter->manifolds[i] = manifolds[i];
	}

	arbiter->num_manifolds = num;
}

static void
_arbiter_prestep(joy_box2d_arbiter_t* arbiter, joy_box2d_body_t* b1, joy_box2d_body_t* b2, joy_fp_t inv_dt) {
	const joy_fp_t k_allowed_penetration = fp_from_float(0.01f);
	joy_fp_t k_biasfactor = JOY_POSITION_CORRECTION ? fp_from_float(0.2f) : fp_zero();
	joy_vec2f_t tmp;

	for (int i = 0; i < arbiter->num_manifolds; ++i) {
		joy_box2d_manifold_t* m = arbiter->manifolds + i;

		joy_vec2f_t r1 = vec2f_sub(m->position, b1->position);
		joy_vec2f_t r2 = vec2f_sub(m->position, b2->position);

		joy_fp_t rn1 = vec2f_dot(r1, m->normal);
		joy_fp_t rn2 = vec2f_dot(r2, m->normal);
		joy_fp_t knormal = fp_add(b1->inv_mass, b2->inv_mass);
		knormal = fp_add(knormal, fp_add(fp_mul(b1->invI, fp_sub(vec2f_dot(r1, r1), fp_pow2(rn1))), fp_mul(b2->invI, fp_sub(vec2f_dot(r2, r2), fp_pow2(rn2)))));
		m->mass_normal = fp_div(fp_one(), knormal);

		joy_vec2f_t tangent = vec2f_cross2(m->normal, fp_one());
		joy_fp_t rt1 = vec2f_dot(r1, tangent);
		joy_fp_t rt2 = vec2f_dot(r2, tangent);
		joy_fp_t k_tangent = fp_add(b1->inv_mass, b2->inv_mass);
		k_tangent = fp_add(k_tangent, fp_mul(b1->invI, fp_sub(vec2f_dot(r1, r1), fp_pow2(rt1))));
		k_tangent = fp_add(k_tangent, fp_mul(b2->invI, fp_sub(vec2f_dot(r2, r2), fp_pow2(rt2))));
		m->mass_tangent = fp_div(fp_one(), k_tangent);

		joy_fp_t corr_sep = fp_min(fp_zero(), fp_add(m->separation, k_allowed_penetration));
		m->bias = fp_mul(fp_mul(fp_negate(k_biasfactor), inv_dt), corr_sep);

		if (JOY_ACCUMULATE_IMPULSES) {
			joy_vec2f_t n_pn = vec2f_scale(m->normal, m->pn);
			joy_vec2f_t t_pt = vec2f_scale(tangent, m->pt);

			joy_vec2f_t P = vec2f_add(n_pn, t_pt);

			tmp = vec2f_scale(P, b1->inv_mass);
		b1->velocity = vec2f_sub(b1->velocity, tmp);
		b1->angular_velocity = fp_sub(b1->angular_velocity, fp_mul(b1->invI, vec2f_cross(r1, P)));

		tmp = vec2f_scale(P, b2->inv_mass);
		b2->velocity = vec2f_add(b2->velocity, tmp);
			b2->angular_velocity = fp_add(b2->angular_velocity, fp_mul(b2->invI, vec2f_cross(r2, P)));
		}
	}
}

static void
_arbiter_apply_impulse(joy_box2d_arbiter_t* arbiter, joy_box2d_body_t* b1, joy_box2d_body_t* b2) {
	joy_vec2f_t tmp;
	for (int i = 0; i < arbiter->num_manifolds; ++i) {
		joy_box2d_manifold_t* m = arbiter->manifolds + i;
		m->r1 = vec2f_sub(m->position, b1->position);
		m->r2 = vec2f_sub(m->position, b2->position);

		joy_vec2f_t r1a = v3(b1->angular_velocity, m->r1);
		joy_vec2f_t r2a = v3(b2->angular_velocity, m->r2);
		tmp = vec2f_add(b2->velocity, r2a);
		tmp = vec2f_sub(tmp, b1->velocity);
		joy_vec2f_t dv = vec2f_sub(tmp, r1a);

		joy_fp_t vn = fp_negate(vec2f_dot(dv, m->normal));
		joy_fp_t dpn = fp_mul(m->mass_normal, fp_add(vn, m->bias));

		if (JOY_ACCUMULATE_IMPULSES) {
			joy_fp_t pn0 = m->pn;
			m->pn = fp_max(fp_add(pn0, dpn), fp_zero());
			dpn = fp_sub(m->pn, pn0);
		}
		else {
			dpn = fp_max(dpn, fp_zero());
		}

		joy_vec2f_t Pn = vec2f_scale(m->normal, dpn);

		joy_vec2f_t b1Pn = vec2f_scale(Pn, b1->inv_mass);
		b1->velocity = vec2f_sub(b1->velocity, b1Pn);
		b1->angular_velocity = fp_sub(b1->angular_velocity, fp_mul(b1->invI, vec2f_cross(m->r1, Pn)));

		joy_vec2f_t b2Pn = vec2f_scale(Pn, b2->inv_mass);
		b2->velocity = vec2f_add(b2->velocity, b2Pn);
		b2->angular_velocity = fp_add(b2->angular_velocity, fp_mul(b2->invI, vec2f_cross(m->r2, Pn)));

		r1a = v3(b1->angular_velocity, m->r1);
		r2a = v3(b2->angular_velocity, m->r2);

		tmp = vec2f_add(b2->velocity, r2a);
		tmp = vec2f_sub(tmp, b1->velocity);
		dv = vec2f_sub(tmp, r1a);

		joy_vec2f_t tangent = vec2f_cross2(m->normal, fp_one());
		joy_fp_t vt = fp_negate(vec2f_dot(dv, tangent));
		joy_fp_t dpt = fp_mul(m->mass_tangent, vt);

		if (JOY_ACCUMULATE_IMPULSES) {
			joy_fp_t max_pt = fp_mul(arbiter->friction, m->pn);
			joy_fp_t old_ti = m->pt;
			m->pt = fp_clamp(fp_add(old_ti, dpt), fp_negate(max_pt), max_pt);
			dpt = fp_sub(m->pt, old_ti);
		}
		else {
			joy_fp_t max_pt = fp_mul(arbiter->friction, dpn);
			dpt = fp_clamp(dpt, fp_negate(max_pt), max_pt);
		}

		joy_vec2f_t pt = vec2f_scale(tangent, dpt);

		joy_vec2f_t b1_pt = vec2f_scale(pt, b1->inv_mass);
		b1->velocity = vec2f_sub(b1->velocity, b1_pt);
		b1->angular_velocity = fp_sub(b1->angular_velocity, fp_mul(b1->invI, vec2f_cross(m->r1, pt)));

		joy_vec2f_t b2_pt = vec2f_scale(pt, b2->inv_mass);
		b2->velocity = vec2f_add(b2->velocity, b2_pt);
		b2->angular_velocity = fp_add(b2->angular_velocity, fp_mul(b2->invI, vec2f_cross(m->r2, pt)));
	}
}


typedef struct joy_box2d_world_t {
	joy_vec2f_t gravity;
	int iterations;
	int id;
	khash_t(bodies_map)* bodies;
	klist_t(joints_list)* joints;
	khash_t(arbiters_map)* arbiters;
} joy_box2d_world_t;


static void
_integrate_forces(joy_box2d_world_t* world, joy_fp_t dt) {
	joy_vec2f_t acceleration;
	khint_t k;
	for (k = kh_begin(world->bodies); k != kh_end(world->bodies); ++k) {
		if (!kh_exist(world->bodies, k)) continue;
		joy_box2d_body_t* body = kh_val(world->bodies, k);
		if (body && fp_greater(body->inv_mass, fp_zero())) {
			acceleration = vec2f_scale(body->force, body->inv_mass);
			acceleration = vec2f_add(world->gravity, acceleration);
			joy_vec2f_t dv = vec2f_scale(acceleration, dt);
			body->velocity = vec2f_add(body->velocity, dv);
			body->angular_velocity = fp_add(body->angular_velocity, fp_mul(fp_mul(dt, body->invI), body->torque));
		}
	}
}

static void
_integrate_velocities(joy_box2d_world_t* world, joy_fp_t dt) {
	joy_vec2f_t position;
	khint_t k;
	for (k = kh_begin(world->bodies); k != kh_end(world->bodies); ++k) {
		if (!kh_exist(world->bodies, k)) continue;
		joy_box2d_body_t* body = kh_val(world->bodies, k);
		if (body && fp_gequal(body->inv_mass, fp_zero())) {
			position = vec2f_scale(body->velocity, dt);
			body->position = vec2f_add(body->position, position);
			body->rotation = fp_add(body->rotation, fp_mul(body->angular_velocity, dt));

			body->force = (joy_vec2f_t){ fp_zero(), fp_zero() };
			body->torque = fp_zero();
		}
	}
}

// O(n^2) broad-phase
static void
_broad_phase(joy_box2d_world_t* world) {
	(void)world;
}

static void
_narrow_phase(joy_box2d_world_t* world) {
	joy_box2d_contact_t contacts[2];
	joy_box2d_manifold_t ms[2];

	// Collect body pointers into a vector for ordered iteration
	kvec_t(joy_box2d_body_t*) body_vec;
	kv_init(body_vec);
	{
		khint_t k;
		for (k = kh_begin(world->bodies); k != kh_end(world->bodies); ++k) {
			if (kh_exist(world->bodies, k))
				kv_push(joy_box2d_body_t*, body_vec, kh_val(world->bodies, k));
		}
	}

	int nb = kv_size(body_vec);
	for (int i = 0; i < nb; ++i) {
		joy_box2d_body_t* bi = kv_A(body_vec, i);
		for (int j = i + 1; j < nb; ++j) {
			joy_box2d_body_t* bj = kv_A(body_vec, j);
			if (fp_equal(bi->inv_mass, fp_zero()) && fp_equal(bj->inv_mass, fp_zero())) {
				continue;
			}
			int num_contacts = _collide(contacts, bi, bj);
			joy_box2d_arbiter_key_t key;
			if (bi->id < bj->id) {
				key.id_pair.id1 = bi->id;
				key.id_pair.id2 = bj->id;
			}
			else {
				key.id_pair.id1 = bj->id;
				key.id_pair.id2 = bi->id;
			}
			if (num_contacts > 0) {
				for (int ci = 0; ci < num_contacts; ci++) {
					ms[ci].position = contacts[ci].position;
					ms[ci].normal = contacts[ci].normal;
					ms[ci].separation = contacts[ci].separation;
					ms[ci].feature = contacts[ci].feature.value;
					ms[ci].pt = ms[ci].pn = fp_zero();
					ms[ci].r1 = (joy_vec2f_t){ fp_zero(), fp_zero() };
					ms[ci].r2 = (joy_vec2f_t){ fp_zero(), fp_zero() };
					ms[ci].mass_normal = ms[ci].mass_tangent = fp_zero();
					ms[ci].bias = fp_zero();
				}

				khint_t kk = kh_get(arbiters_map, world->arbiters, key.value);
				joy_box2d_arbiter_t* arbiter = (kk != kh_end(world->arbiters)) ? kh_val(world->arbiters, kk) : NULL;
				if (arbiter == NULL) {
					joy_fp_t friction = fp_sqrt(fp_mul(bi->friction, bj->friction));
					arbiter = _arbiter_create(key, friction, ms, num_contacts);
					int ret;
					kk = kh_put(arbiters_map, world->arbiters, key.value, &ret);
					kh_val(world->arbiters, kk) = arbiter;
				}
				else {
					_arbiter_update(arbiter, ms, num_contacts);
				}
			}
			else {
				khint_t kk = kh_get(arbiters_map, world->arbiters, key.value);
				if (kk != kh_end(world->arbiters)) {
					joy_box2d_arbiter_t* arbiter = kh_val(world->arbiters, kk);
					kh_del(arbiters_map, world->arbiters, kk);
					_arbiter_destroy(arbiter);
				}
			}
		}
	}
	kv_destroy(body_vec);
}

static void
_presolve(joy_box2d_world_t* world, joy_fp_t inv_dt) {
	khint_t k;
	for (k = kh_begin(world->arbiters); k != kh_end(world->arbiters); ++k) {
		if (!kh_exist(world->arbiters, k)) continue;
		joy_box2d_arbiter_t* arbiter = kh_val(world->arbiters, k);
		joy_box2d_body_t* b1 = NULL, * b2 = NULL;
		khint_t kk;

		kk = kh_get(bodies_map, world->bodies, arbiter->id1);
		if (kk != kh_end(world->bodies)) b1 = kh_val(world->bodies, kk);
		kk = kh_get(bodies_map, world->bodies, arbiter->id2);
		if (kk != kh_end(world->bodies)) b2 = kh_val(world->bodies, kk);

		_arbiter_prestep(arbiter, b1, b2, inv_dt);
	}

	kliter_t(joints_list)* joint_iter = kl_begin(world->joints);
	while (joint_iter != kl_end(world->joints)) {
		joy_box2d_joint_t* joint = kl_val(joint_iter);
		joy_box2d_body_t* b1 = NULL, * b2 = NULL;
		khint_t kk;

		kk = kh_get(bodies_map, world->bodies, joint->id1);
		if (kk != kh_end(world->bodies)) b1 = kh_val(world->bodies, kk);
		kk = kh_get(bodies_map, world->bodies, joint->id2);
		if (kk != kh_end(world->bodies)) b2 = kh_val(world->bodies, kk);

		_prestep(joint, b1, b2, inv_dt);
		joint_iter = kl_next(joint_iter);
	}
}

static void
_solve(joy_box2d_world_t* world) {
	for (int i = 0; i < world->iterations; ++i) {
		khint_t k;
		for (k = kh_begin(world->arbiters); k != kh_end(world->arbiters); ++k) {
			if (!kh_exist(world->arbiters, k)) continue;
			joy_box2d_arbiter_t* arbiter = kh_val(world->arbiters, k);
			joy_box2d_body_t* b1 = NULL, * b2 = NULL;
			khint_t kk;

			kk = kh_get(bodies_map, world->bodies, arbiter->id1);
			if (kk != kh_end(world->bodies)) b1 = kh_val(world->bodies, kk);
			kk = kh_get(bodies_map, world->bodies, arbiter->id2);
			if (kk != kh_end(world->bodies)) b2 = kh_val(world->bodies, kk);

			_arbiter_apply_impulse(arbiter, b1, b2);
		}

		kliter_t(joints_list)* joint_iter = kl_begin(world->joints);
		while (joint_iter != kl_end(world->joints)) {
			joy_box2d_joint_t* joint = kl_val(joint_iter);
			joy_box2d_body_t* b1 = NULL, * b2 = NULL;
			khint_t kk;

			kk = kh_get(bodies_map, world->bodies, joint->id1);
			if (kk != kh_end(world->bodies)) b1 = kh_val(world->bodies, kk);
			kk = kh_get(bodies_map, world->bodies, joint->id2);
			if (kk != kh_end(world->bodies)) b2 = kh_val(world->bodies, kk);

			_joint_apply_impulse(joint, b1, b2);
			joint_iter = kl_next(joint_iter);
		}
	}
}


joy_box2d_world_t*
joy_math_box2d_create_world(joy_vec2f_t gravity, int iterations) {
	joy_box2d_world_t* world = (joy_box2d_world_t*)malloc(sizeof(joy_box2d_world_t));
	assert(world);
	world->gravity = gravity;
	world->iterations = iterations;
	world->bodies = kh_init(bodies_map);
	world->joints = kl_init(joints_list);
	world->arbiters = kh_init(arbiters_map);
	world->id = 0;
	return world;
}

void
joy_math_box2d_clear_world(joy_box2d_world_t* world) {
	khint_t k;
	for (k = kh_begin(world->bodies); k != kh_end(world->bodies); ++k) {
		if (!kh_exist(world->bodies, k)) continue;
		free(kh_val(world->bodies, k));
	}
	kh_destroy(bodies_map, world->bodies);
	world->bodies = kh_init(bodies_map);

	{
		kliter_t(joints_list)* iter = kl_begin(world->joints);
		while (iter != kl_end(world->joints)) {
			free(kl_val(iter));
			iter = kl_next(iter);
		}
	}
	kl_destroy(joints_list, world->joints);
	world->joints = kl_init(joints_list);

	for (k = kh_begin(world->arbiters); k != kh_end(world->arbiters); ++k) {
		if (!kh_exist(world->arbiters, k)) continue;
		free(kh_val(world->arbiters, k));
	}
	kh_destroy(arbiters_map, world->arbiters);
	world->arbiters = kh_init(arbiters_map);

	world->id = 0;
}

void
joy_math_box2d_destroy_world(joy_box2d_world_t* world) {
	khint_t k;
	for (k = kh_begin(world->bodies); k != kh_end(world->bodies); ++k) {
		if (!kh_exist(world->bodies, k)) continue;
		free(kh_val(world->bodies, k));
	}
	kh_destroy(bodies_map, world->bodies);

	{
		kliter_t(joints_list)* iter = kl_begin(world->joints);
		while (iter != kl_end(world->joints)) {
			free(kl_val(iter));
			iter = kl_next(iter);
		}
	}
	kl_destroy(joints_list, world->joints);

	for (k = kh_begin(world->arbiters); k != kh_end(world->arbiters); ++k) {
		if (!kh_exist(world->arbiters, k)) continue;
		free(kh_val(world->arbiters, k));
	}
	kh_destroy(arbiters_map, world->arbiters);

	free(world);
}


void
joy_math_box2d_step(joy_box2d_world_t* world, joy_fp_t dt) {
	joy_fp_t inv_dt = fp_greater(dt, fp_zero()) ? fp_div(fp_one(), dt) : fp_zero();

	_broad_phase(world);
	_narrow_phase(world);
	_integrate_forces(world, dt);
	_presolve(world, inv_dt);
	_solve(world);
	_integrate_velocities(world, dt);
}

joy_box2d_body_t*
joy_math_box2d_create_body(joy_box2d_world_t* world, joy_fp_t mass, joy_fp_t bx, joy_fp_t by) {
	joy_box2d_body_t* body = (joy_box2d_body_t*)malloc(sizeof(joy_box2d_body_t));
	assert(body != NULL);

	body->id = ++world->id;
	body->position = (joy_vec2f_t){ fp_zero(), fp_zero() };
	body->rotation = fp_zero();
	body->velocity = (joy_vec2f_t){ fp_zero(), fp_zero() };
	body->angular_velocity = fp_zero();
	body->force = (joy_vec2f_t){ fp_zero(), fp_zero() };
	body->torque = fp_zero();
	body->friction = fp_from_float(0.2f);

	body->bounds.x = bx;
	body->bounds.y = by;
	body->mass = mass;

	if (fp_less(body->mass, fp_max_value())) {
		body->inv_mass = fp_div(fp_one(), body->mass);
		body->I = fp_div(fp_mul(body->mass, fp_add(fp_pow2(body->bounds.x), fp_pow2(body->bounds.y))), fp_from_float(12));
		body->invI = fp_div(fp_one(), body->I);
	}
	else {
		body->inv_mass = fp_zero();
		body->I = fp_max_value();
		body->invI = fp_zero();
	}

	int ret;
	khint_t k = kh_put(bodies_map, world->bodies, body->id, &ret);
	kh_val(world->bodies, k) = body;
	return body;
}

void
joy_math_box2d_destroy_body(joy_box2d_world_t* world, int id) {
	khint_t k = kh_get(bodies_map, world->bodies, id);
	if (k != kh_end(world->bodies)) {
		free(kh_val(world->bodies, k));
		kh_del(bodies_map, world->bodies, k);
	}
}

joy_box2d_joint_t*
joy_math_box2d_create_joint(joy_box2d_world_t* world, int id1, int id2, joy_vec2f_t anchor) {
	khint_t kk;

	kk = kh_get(bodies_map, world->bodies, id1);
	joy_box2d_body_t* b1 = (kk != kh_end(world->bodies)) ? kh_val(world->bodies, kk) : NULL;
	kk = kh_get(bodies_map, world->bodies, id2);
	joy_box2d_body_t* b2 = (kk != kh_end(world->bodies)) ? kh_val(world->bodies, kk) : NULL;

	if (b1 == NULL || b2 == NULL) {
		return NULL;
	}

	joy_box2d_joint_t* joint = (joy_box2d_joint_t*)malloc(sizeof(joy_box2d_joint_t));
	assert(joint != NULL);

	joint->id1 = b1->id;
	joint->id2 = b2->id;

	joy_mat22f_t Rot1 = mat22f_rotate(b1->rotation);
	joy_mat22f_t Rot2 = mat22f_rotate(b2->rotation);
	joy_mat22f_t Rot1T = mat22f_transpose(Rot1);
	joy_mat22f_t Rot2T = mat22f_transpose(Rot2);

	joy_vec2f_t distance1 = vec2f_sub(anchor, b1->position);
	joy_vec2f_t distance2 = vec2f_sub(anchor, b2->position);

	joint->local_anchor1 = mat22f_mul_vec2f(Rot1T, distance1);
	joint->local_anchor2 = mat22f_mul_vec2f(Rot2T, distance2);

	joint->p = (joy_vec2f_t){ fp_zero(), fp_zero() };

	joint->softness = fp_zero();
	joint->biasfactor = fp_from_float(0.2f);

	*kl_pushp(joints_list, world->joints) = joint;
	return joint;
}

void
joy_math_box2d_destroy_joint(joy_box2d_world_t* world, joy_box2d_joint_t* joint) {
	klist_t(joints_list)* new_list = kl_init(joints_list);
	kliter_t(joints_list)* iter = kl_begin(world->joints);
	while (iter != kl_end(world->joints)) {
		joy_box2d_joint_t* j = kl_val(iter);
		if (j != joint)
			*kl_pushp(joints_list, new_list) = j;
		iter = kl_next(iter);
	}
	kl_destroy(joints_list, world->joints);
	world->joints = new_list;
	free(joint);
}

void
joy_math_box2d_foreach_bodies(joy_box2d_world_t* world, void (*iterator)(joy_box2d_body_t*)) {
	khint_t k;
	for (k = kh_begin(world->bodies); k != kh_end(world->bodies); ++k) {
		if (!kh_exist(world->bodies, k)) continue;
		iterator(kh_val(world->bodies, k));
	}
}

void
joy_math_box2d_foreach_joints(joy_box2d_world_t* world, void (*iterator)(joy_box2d_joint_t*, joy_box2d_body_t*, joy_box2d_body_t*)) {
	kliter_t(joints_list)* joint_iter = kl_begin(world->joints);
	while (joint_iter != kl_end(world->joints)) {
		joy_box2d_joint_t* joint = kl_val(joint_iter);
		joy_box2d_body_t* b1 = NULL, * b2 = NULL;
		khint_t kk;

		kk = kh_get(bodies_map, world->bodies, joint->id1);
		if (kk != kh_end(world->bodies)) b1 = kh_val(world->bodies, kk);
		kk = kh_get(bodies_map, world->bodies, joint->id2);
		if (kk != kh_end(world->bodies)) b2 = kh_val(world->bodies, kk);

		iterator(joint, b1, b2);
		joint_iter = kl_next(joint_iter);
	}
}

void
joy_math_box2d_foreach_arbiters(joy_box2d_world_t* world, void (*iterator)(joy_box2d_arbiter_t*)) {
	khint_t k;
	for (k = kh_begin(world->arbiters); k != kh_end(world->arbiters); ++k) {
		if (!kh_exist(world->arbiters, k)) continue;
		iterator(kh_val(world->arbiters, k));
	}
}

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef kl_no_free
#undef fp_negate
#undef fp_equal
#undef fp_less
#undef fp_greater
#undef fp_lequal
#undef fp_gequal

/* ==================== 结束 src/jbox2d.c ==================== */
