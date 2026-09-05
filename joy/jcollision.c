#include "joy.h"

ray2d_collisionf_t 
collision2df_get_ray_circle(ray2df_t ray, circlef_t circle)
{
        vec2f_t seg;
        fp_t r_sq, d_sq, seg_len_sq, seg_proj, dx;
        ray2d_collisionf_t result = {0};
        vec2f_t distance;

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
static vec2f_t collision2df_calculate_rect_normal(vec2f_t point, vec2f_t min, vec2f_t max)
{
        vec2f_t normal = { fp_zero(), fp_zero() };
        const fp_t epsilon = fp_from_float(0.001f); // 极小值，避免浮点误差

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
#define SWAP(a, b)  do { fp_t tmp = (a); (a) = (b); (b) = tmp; } while(0)

// 射线-矩形碰撞检测（修复版）
ray2d_collisionf_t collision2df_get_ray_rectangle(ray2df_t ray, rectanglef_t rect)
{
        ray2d_collisionf_t result = { 0 };
        vec2f_t min, max;
        fp_t t_min, t_max, inv_dir;
        fp_t t1, t2, tmp;


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
        vec2f_t delta = {
            fp_mul(t_min, ray.direction.x),
            fp_mul(t_min, ray.direction.y)
        };
        result.point = vec2f_add(ray.origin, delta);

        // 【修复】正确计算矩形碰撞法线（垂直于碰撞边）
        result.normal = collision2df_calculate_rect_normal(result.point, min, max);

        return result;
}



ray2d_collisionf_t 
collision2df_get_ray_rectanglex(ray2df_t ray, rectanglef_t rect, fp_t angle)
{
        ray2d_collisionf_t result={0};
        vec2f_t half_extents, center, local_origin, local_dir;
        fp_t rotation, t_min, t_max;
        fp_t t1, t2, tmp, inv_dir, t;
        vec2f_t local_hit;

        const fp_t EPSILON = fp_from_float(0.0001f);

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
                vec2f_t local_normal = {0, 0};
                if (fp_abs(fp_sub(fp_abs(local_hit.x), half_extents.x)) < fp_mul(half_extents.x, fp_from_float(0.01f))) {
                        local_normal.x = local_hit.x > fp_zero() ? fp_one() : fp_from_float(-1);
                }
                if (fp_abs(fp_sub(fp_abs(local_hit.y), half_extents.y)) < fp_mul(half_extents.y, fp_from_float(0.01f))) {
                        local_normal.y = local_hit.y > fp_zero() ? fp_one() : fp_from_float(-1);
                }
                /* 若同时命中角点，只取一个轴向（离边缘更近的那个） */
                if (local_normal.x != fp_zero() && local_normal.y != fp_zero()) {
                        fp_t dx = fp_abs(fp_sub(fp_abs(local_hit.x), half_extents.x));
                        fp_t dy = fp_abs(fp_sub(fp_abs(local_hit.y), half_extents.y));
                        if (dx < dy) local_normal.y = 0;
                        else         local_normal.x = 0;
                }
                result.normal = vec2f_rotate(local_normal, rotation);
                result.normal = vec2f_normalize(result.normal);
        }
        return result;
}

ray2d_collisionf_t 
collision2df_get_ray_polygon(ray2df_t ray, polygonf_t polygon)
{
        ray2d_collisionf_t result = {0};
        vec2f_t edge_start, edge_end, edge_vec, diff;
        fp_t denom, t, u;
        fp_t closest_t = fp_max_value();
        vec2f_t hit_edge = {0, 0};

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
collision2df_check_point_rectangle(vec2f_t point, rectanglef_t rect)
{
        return (point.x >= rect.x && point.x <= fp_add(rect.x, rect.width)
             && point.y >= rect.y && point.y <= fp_add(rect.y, rect.height));
}

bool 
collision2df_check_point_circle(vec2f_t point, circlef_t circle)
{
        vec2f_t diff = vec2f_sub(point, circle.center);
        return vec2f_length_squared(diff) <= fp_pow2(circle.radius);
}

fp_t
collision2df_get_squared_distance_point_circle(vec2f_t point, circlef_t circle)
{
        vec2f_t diff = vec2f_sub(point, circle.center);
        return vec2f_length_squared(diff);
}

bool 
collision2df_check_point_triangle(vec2f_t point, vec2f_t p1, vec2f_t p2, vec2f_t p3)
{
        /* Barycentric coordinate method */
        vec2f_t v0 = vec2f_sub(p3, p1);
        vec2f_t v1 = vec2f_sub(p2, p1);
        vec2f_t v2 = vec2f_sub(point, p1);

        fp_t dot00 = vec2f_dot(v0, v0);
        fp_t dot01 = vec2f_dot(v0, v1);
        fp_t dot02 = vec2f_dot(v0, v2);
        fp_t dot11 = vec2f_dot(v1, v1);
        fp_t dot12 = vec2f_dot(v1, v2);

        fp_t denom = fp_sub(fp_mul(dot00, dot11), fp_mul(dot01, dot01));
        if (denom == fp_zero())
                return false;

        fp_t inv_denom = fp_div(fp_one(), denom);
        fp_t u = fp_mul(fp_sub(fp_mul(dot11, dot02), fp_mul(dot01, dot12)), inv_denom);
        fp_t v = fp_mul(fp_sub(fp_mul(dot00, dot12), fp_mul(dot01, dot02)), inv_denom);

        return (u >= fp_zero() && v >= fp_zero()
             && fp_add(u, v) <= fp_one());
}

bool 
collision2df_check_lines(vec2f_t a1, vec2f_t b1, vec2f_t a2, vec2f_t b2, vec2f_t*cp)
{
        vec2f_t r = vec2f_sub(b1, a1);
        vec2f_t s = vec2f_sub(b2, a2);
        fp_t denom = vec2f_cross(r, s);

        if (denom == fp_zero())
                return false; /* parallel */

        vec2f_t diff = vec2f_sub(a2, a1);
        fp_t t = fp_div(vec2f_cross(diff, s), denom);
        fp_t u = fp_div(vec2f_cross(diff, r), denom);

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
collision2df_check_point_line(vec2f_t point, vec2f_t p1, vec2f_t p2, fp_t threshold)
{
        vec2f_t ab = vec2f_sub(p2, p1);
        vec2f_t ap = vec2f_sub(point, p1);
        fp_t ab_len_sq = vec2f_length_squared(ab);

        if (ab_len_sq == fp_zero())
                return vec2f_length_squared(ap) <= fp_pow2(threshold);

        fp_t proj = fp_div(vec2f_dot(ap, ab), ab_len_sq);
        proj = fp_clamp(proj, fp_zero(), fp_one());

        vec2f_t closest = vec2f_add(p1, vec2f_scale(ab, proj));
        vec2f_t diff = vec2f_sub(point, closest);

        return vec2f_length_squared(diff) <= fp_pow2(threshold);
}

bool 
collision2df_check_circles(circlef_t a, circlef_t b)
{
        vec2f_t distance;
        fp_t distance_squared, radius_sum;

        distance = vec2f_sub(b.center, a.center);
        distance_squared = vec2f_length_squared(distance);
        radius_sum = fp_add(b.radius, a.radius);

        return distance_squared <= fp_pow2(radius_sum);
}

/* aabb */
bool 
collision2df_check_rectangles(rectanglef_t a, rectanglef_t b)
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

/* SAT-based polygon-polygon overlap test */
bool 
collision2df_check_polygons(polygonf_t a, polygonf_t b)
{
        vec2f_t axis, point;
        fp_t ab_sep = find_min_separation(a, b, &axis, &point);
        if (ab_sep >= fp_zero())
                return false;
        fp_t ba_sep = find_min_separation(b, a, &axis, &point);
        if (ba_sep >= fp_zero())
                return false;
        return true;
}

/* Circle vs AABB: find closest point on rect to circle center */
bool 
collision2df_check_circle_rectangle(circlef_t a, rectanglef_t b)
{
        vec2f_t closest;
        closest.x = fp_clamp(a.center.x, b.x, fp_add(b.x, b.width));
        closest.y = fp_clamp(a.center.y, b.y, fp_add(b.y, b.height));

        vec2f_t diff = vec2f_sub(a.center, closest);
        return vec2f_length_squared(diff) <= fp_pow2(a.radius);
}

/* Circle vs convex polygon */
bool 
collision2df_check_circle_polygon(circlef_t a, polygonf_t b)
{
        if (b.num_vertices < 3)
                return false;

        /* 1. Check distance from center to each edge */
        for (int i = 0; i < b.num_vertices; i++) {
                vec2f_t e1 = b.vertices[i];
                vec2f_t e2 = b.vertices[(i + 1) % b.num_vertices];
                vec2f_t edge = vec2f_sub(e2, e1);
                vec2f_t to_center = vec2f_sub(a.center, e1);

                fp_t edge_len_sq = vec2f_length_squared(edge);
                if (edge_len_sq == fp_zero())
                        continue;

                fp_t t = fp_div(vec2f_dot(to_center, edge), edge_len_sq);
                t = fp_clamp(t, fp_zero(), fp_one());

                vec2f_t closest = vec2f_add(e1, vec2f_scale(edge, t));
                vec2f_t diff = vec2f_sub(a.center, closest);

                if (vec2f_length_squared(diff) <= fp_pow2(a.radius))
                        return true;
        }

        /* 3. Point-in-polygon test for circle center */
        {
                bool inside = true;
                for (int i = 0; i < b.num_vertices; i++) {
                        vec2f_t e1 = b.vertices[i];
                        vec2f_t e2 = b.vertices[(i + 1) % b.num_vertices];
                        vec2f_t edge = vec2f_sub(e2, e1);
                        vec2f_t normal = vec2f_normal(edge); /* inward normal */
                        vec2f_t to_center = vec2f_sub(a.center, e1);
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
collision2df_get_circles(circlef_t a, circlef_t b, contact2df_t *contact)
{
        vec2f_t distance;
        fp_t distance_squared, radius_sum;

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
collision2df_get_rectangles(rectanglef_t a, fp_t a_angle, 
        rectanglef_t b, fp_t b_angle,
	contact2df_t *contact)
{
        /*
        Setup
	取 A B 的半径: bounds 中存储的是边长(宽高)
	再取边长的一半, 即半宽/半高(半径)
	把矩形放到以中心为(0,0)的局部坐标系第一象限的顶点
	那么 a_h / b_h 就是右上角第一象限的点
        */
        vec2f_t a_bounds, b_bounds;
        vec2f_t a_h, b_h, a_pos, b_pos;
        mat22f_t a_rot, b_rot, a_rotT, b_rotT;
        vec2f_t dp, a_d, b_d;
        mat22f_t c, abs_c, abs_ct;
        vec2f_t abs_a_d, abs_b_d;
        vec2f_t a_face, b_face;
	vec2f_t normal;
	fp_t separation, r_separation, a_res, b_res;

	const fp_t relative_tol = FPFF(0.95f);
	const fp_t absolute_tol = FPFF(0.01f);

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


static vec2f_t edge_at(polygonf_t poly, int index)
{
        vec2f_t next_vertex, curr_vertex;
        curr_vertex = poly.vertices[index];
        next_vertex = poly.vertices[(index + 1) % poly.num_vertices];
        return vec2f_sub(next_vertex, curr_vertex);
}

fp_t 
find_min_separation(polygonf_t a, polygonf_t b, vec2f_t *axis, vec2f_t *point)
{
        fp_t separation, min_sep, proj;
        vec2f_t va, vb, normal, min_vertex;
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
collision2df_get_polygons(polygonf_t a, polygonf_t b, contact2df_t *contact)
{
        vec2f_t a_axis, b_axis, a_point, b_point;
        vec2f_t distance;
        fp_t ab_sep, ba_sep;
        ab_sep = find_min_separation(a, b, &a_axis, &a_point);
        if (ab_sep >= 0)
                return 0;
        ba_sep = find_min_separation(b, a, &b_axis, &b_point);
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


ray3d_collisionf_t
collision3df_get_ray_triangle(ray3df_t ray, vec3f_t p1, vec3f_t p2, vec3f_t p3)
{
        const fp_t EPSILON = fp_from_float(0.000001f);
        ray3d_collisionf_t result = {0};
        vec3f_t edge1, edge2;
        vec3f_t p, q, tv;
        fp_t det, inv_det, u, v, t;

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

ray3d_collisionf_t 
collision3df_get_ray_quad(ray3df_t ray, vec3f_t p1, vec3f_t p2, vec3f_t p3, vec3f_t p4)
{
        ray3d_collisionf_t result;
        result = collision3df_get_ray_triangle(ray, p1, p2, p4);
        if (!result.hit)
                result = collision3df_get_ray_triangle(ray, p2, p3, p4);
        return result;
}
 
ray3d_collisionf_t 
collision3df_get_ray_sphere(ray3df_t ray, spheref_t sphere)
{
        ray3d_collisionf_t result = {0};
        vec3f_t ray_sphere_pos = vec3f_sub(ray.position, sphere.position);
        fp_t dist_sq = vec3f_square_length(ray_sphere_pos);
        fp_t a = vec3f_dot(ray_sphere_pos, ray.direction);
        fp_t b = fp_sub(dist_sq, fp_pow2(sphere.radius));
        fp_t c = fp_sub(fp_pow2(a), b);

        /* 判别式 < 0: 无交点 */
        if (c < fp_zero())
                return result;

        fp_t d = fp_sqrt(c);
        fp_t t = fp_sub(-a, d);
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

ray3d_collisionf_t
collision3df_get_ray_box(ray3df_t ray, bounding_boxf_t box)
{
        /* Slabs method (ray-AABB) */
        ray3d_collisionf_t result = {0};
        vec3f_t dirfrac;
        fp_t t1, t2, tmin, tmax;
        const fp_t EPSILON = fp_from_float(0.00001f);

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

        fp_t t = tmin >= fp_zero() ? tmin : tmax;
        if (t < EPSILON)
                return result;

        result.hit = true;
        result.distance = t;
        result.point = vec3f_add(ray.position,
                vec3f_scale(ray.direction, t));

        /* Compute normal from which slab was hit */
        vec3f_t center = vec3f_scale(vec3f_add(box.min, box.max), fp_half());
        vec3f_t hit_to_center = vec3f_sub(center, result.point);
        vec3f_t half = vec3f_scale(vec3f_sub(box.max, box.min), fp_half());
        vec3f_t abs_hit = vec3f_abs(hit_to_center);

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

/* 列主序 mat44f_t 对方向向量做旋转(忽略平移分量) —— 与 jgjk.c 内 OBB 辅助一致 */
static vec3f_t jcoll_obb_mat44f_rotate_dir(mat44f_t m, vec3f_t v) {
        vec3f_t r;
        r.x = fp_add(fp_add(fp_mul(m.m0, v.x), fp_mul(m.m4, v.y)), fp_mul(m.m8,  v.z));
        r.y = fp_add(fp_add(fp_mul(m.m1, v.x), fp_mul(m.m5, v.y)), fp_mul(m.m9,  v.z));
        r.z = fp_add(fp_add(fp_mul(m.m2, v.x), fp_mul(m.m6, v.y)), fp_mul(m.m10, v.z));
        return r;
}

/* 列主序 mat44f_t 的转置对方向向量做旋转(即逆旋转，旋转矩阵正交) */
static vec3f_t jcoll_obb_mat44f_rotate_dir_t(mat44f_t m, vec3f_t v) {
        vec3f_t r;
        r.x = fp_add(fp_add(fp_mul(m.m0, v.x), fp_mul(m.m1, v.y)), fp_mul(m.m2,  v.z));
        r.y = fp_add(fp_add(fp_mul(m.m4, v.x), fp_mul(m.m5, v.y)), fp_mul(m.m6,  v.z));
        r.z = fp_add(fp_add(fp_mul(m.m8, v.x), fp_mul(m.m9, v.y)), fp_mul(m.m10, v.z));
        return r;
}

/* 欧拉角(弧度) -> 组合旋转矩阵, 顺序: R = Rz * Ry * Rx（与 jgjk.c euler_to_rot 一致） */
static mat44f_t jcoll_obb_euler_to_rot(vec3f_t euler) {
        mat44f_t rx = mat44f_rotate_x(euler.x);
        mat44f_t ry = mat44f_rotate_y(euler.y);
        mat44f_t rz = mat44f_rotate_z(euler.z);
        mat44f_t tmp = mat44f_mul(ry, rx);
        return mat44f_mul(rz, tmp);
}

/* OBB(旋转盒)射线检测: 把射线变换到 OBB 局部坐标系(用旋转矩阵的转置做逆旋转),
 * 在局部系内做 slab(AABB) 求交, 再把命中点与法线旋回世界空间。
 * 全程定点(fp_t), 帧同步确定。 */
ray3d_collisionf_t
collision3df_get_ray_obb(ray3df_t ray, vec3f_t position, vec3f_t half_size, vec3f_t rotation)
{
        ray3d_collisionf_t result = {0};
        const fp_t EPSILON = fp_from_float(0.0001f);

        mat44f_t R = jcoll_obb_euler_to_rot(rotation);   /* 世界旋转矩阵 M = R */

        /* 射线原点与方向变换到 OBB 局部系: M^-1 = M^T (正交矩阵) */
        vec3f_t rel = vec3f_sub(ray.position, position);
        vec3f_t local_origin = jcoll_obb_mat44f_rotate_dir_t(R, rel);
        vec3f_t local_dir    = jcoll_obb_mat44f_rotate_dir_t(R, ray.direction);

        /* t_min 初值取最小值: 便于区分"起点在盒内"(t_min<0)和"起点在盒外"(t_min>0) */
        fp_t t_min = fp_min_value();
        fp_t t_max = fp_max_value();
        fp_t t1, t2, tmp, inv_dir;

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

        fp_t t = t_min >= fp_zero() ? t_min : t_max;
        if (t < EPSILON) return result;                 /* 起点恰好贴盒面之类: 容差避免自命中 */

        /* 局部命中点 */
        vec3f_t local_hit;
        local_hit.x = fp_add(local_origin.x, fp_mul(local_dir.x, t));
        local_hit.y = fp_add(local_origin.y, fp_mul(local_dir.y, t));
        local_hit.z = fp_add(local_origin.z, fp_mul(local_dir.z, t));

        /* 局部法线: 判定命中哪一面(slab法) */
        vec3f_t local_normal = { fp_zero(), fp_zero(), fp_zero() };
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
                fp_t dm = fp_max_value();
                int drop = -1;
                if (local_normal.x != fp_zero()) {
                        fp_t d = fp_abs(fp_sub(fp_abs(local_hit.x), half_size.x));
                        if (d < dm) { dm = d; drop = 0; }
                }
                if (local_normal.y != fp_zero()) {
                        fp_t d = fp_abs(fp_sub(fp_abs(local_hit.y), half_size.y));
                        if (d < dm) { dm = d; drop = 1; }
                }
                if (local_normal.z != fp_zero()) {
                        fp_t d = fp_abs(fp_sub(fp_abs(local_hit.z), half_size.z));
                        if (d < dm) { dm = d; drop = 2; }
                }
                if (drop == 0) local_normal.x = fp_zero();
                else if (drop == 1) local_normal.y = fp_zero();
                else if (drop == 2) local_normal.z = fp_zero();
                n_count--;
        }

        /* 旋回世界空间: hit point 需要旋转 + 平移, normal 只需旋转 */
        vec3f_t world_hit = jcoll_obb_mat44f_rotate_dir(R, local_hit);
        world_hit = vec3f_add(world_hit, position);

        result.hit = true;
        result.point = world_hit;
        result.normal = vec3f_normalize(jcoll_obb_mat44f_rotate_dir(R, local_normal));
        result.distance = vec3f_length(vec3f_sub(world_hit, ray.position));

        return result;
}

bool
collision3df_check_spheres(spheref_t a, spheref_t b)
{
        vec3f_t diff = vec3f_sub(b.position, a.position);
        fp_t dist_sq = vec3f_square_length(diff);
        fp_t radius_sum = fp_add(a.radius, b.radius);
        return dist_sq <= fp_pow2(radius_sum);
}

bool 
collision3df_check_boxes(bounding_boxf_t a, bounding_boxf_t b)
{
        return (a.min.x <= b.max.x && a.max.x >= b.min.x
             && a.min.y <= b.max.y && a.max.y >= b.min.y
             && a.min.z <= b.max.z && a.max.z >= b.min.z);
}

bool 
collision3df_check_box_sphere(bounding_boxf_t a, spheref_t b)
{
        vec3f_t closest;
        closest.x = fp_clamp(b.position.x, a.min.x, a.max.x);
        closest.y = fp_clamp(b.position.y, a.min.y, a.max.y);
        closest.z = fp_clamp(b.position.z, a.min.z, a.max.z);

        vec3f_t diff = vec3f_sub(b.position, closest);
        return vec3f_square_length(diff) <= fp_pow2(b.radius);
}

/* =================================================================
 * 浮点版本（ft_t = float）
 * ================================================================= */

ray2d_collision_t 
collision2d_get_ray_circle(ray2d_t ray, circle_t circle)
{
        vec2_t seg;
        ft_t r_sq, d_sq, seg_len_sq, seg_proj, dx;
        ray2d_collision_t result = {0};
        vec2_t distance;

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

static vec2_t collision2d_calculate_rect_normal(vec2_t point, vec2_t min, vec2_t max)
{
        vec2_t normal = { 0, 0 };
        const ft_t epsilon = 0.001f;
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

#define SWAP_FT(a, b)  do { ft_t tmp = (a); (a) = (b); (b) = tmp; } while(0)

ray2d_collision_t collision2d_get_ray_rectangle(ray2d_t ray, rectangle_t rect)
{
        ray2d_collision_t result = { 0 };
        vec2_t min, max;
        ft_t t_min, t_max, inv_dir, t1, t2;

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
        vec2_t delta = { t_min * ray.direction.x, t_min * ray.direction.y };
        result.point = vec2_add(ray.origin, delta);
        result.normal = collision2d_calculate_rect_normal(result.point, min, max);
        return result;
}

ray2d_collision_t 
collision2d_get_ray_rectanglex(ray2d_t ray, rectangle_t rect, ft_t angle)
{
        ray2d_collision_t result = {0};
        vec2_t half_extents, center, local_origin, local_dir;
        ft_t rotation, t_min, t_max, t1, t2, tmp, inv_dir, t;
        vec2_t local_hit;
        const ft_t EPSILON = 0.0001f;

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
                vec2_t ln = {0, 0};
                ft_t dx = fabsf(fabsf(local_hit.x) - half_extents.x);
                ft_t dy = fabsf(fabsf(local_hit.y) - half_extents.y);
                if (dx < half_extents.x * 0.01f) ln.x = local_hit.x > 0 ? 1.0f : -1.0f;
                if (dy < half_extents.y * 0.01f) ln.y = local_hit.y > 0 ? 1.0f : -1.0f;
                if (ln.x != 0 && ln.y != 0) { if (dx < dy) ln.y = 0; else ln.x = 0; }
                result.normal = vec2_normalize(vec2_rotate(ln, rotation));
        }
        return result;
}

ray2d_collision_t 
collision2d_get_ray_polygon(ray2d_t ray, polygon_t polygon)
{
        ray2d_collision_t result = {0};
        vec2_t edge_start, edge_end, edge_vec, diff;
        ft_t denom, t, u;
        ft_t closest_t = FLT_MAX;
        vec2_t hit_edge = {0, 0};

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

bool collision2d_check_point_rectangle(vec2_t point, rectangle_t rect)
{
        return point.x >= rect.x && point.x <= rect.x + rect.width
            && point.y >= rect.y && point.y <= rect.y + rect.height;
}

bool collision2d_check_point_circle(vec2_t point, circle_t circle)
{
        vec2_t diff = vec2_sub(point, circle.center);
        return vec2_length_squared(diff) <= circle.radius * circle.radius;
}

ft_t collision2d_get_squared_distance_point_circle(vec2_t point, circle_t circle)
{
        vec2_t diff = vec2_sub(point, circle.center);
        return vec2_length_squared(diff);
}

bool collision2d_check_point_triangle(vec2_t point, vec2_t p1, vec2_t p2, vec2_t p3)
{
        vec2_t v0 = vec2_sub(p3, p1);
        vec2_t v1 = vec2_sub(p2, p1);
        vec2_t v2 = vec2_sub(point, p1);
        ft_t dot00 = vec2_dot(v0, v0);
        ft_t dot01 = vec2_dot(v0, v1);
        ft_t dot02 = vec2_dot(v0, v2);
        ft_t dot11 = vec2_dot(v1, v1);
        ft_t dot12 = vec2_dot(v1, v2);
        ft_t denom = dot00 * dot11 - dot01 * dot01;
        if (denom == 0.0f) return false;
        ft_t inv_denom = 1.0f / denom;
        ft_t u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
        ft_t v = (dot00 * dot12 - dot01 * dot02) * inv_denom;
        return u >= 0 && v >= 0 && u + v <= 1.0f;
}

bool collision2d_check_lines(vec2_t a1, vec2_t b1, vec2_t a2, vec2_t b2, vec2_t* cp)
{
        vec2_t r = vec2_sub(b1, a1);
        vec2_t s = vec2_sub(b2, a2);
        ft_t denom = vec2_cross(r, s);
        if (denom == 0.0f) return false;
        vec2_t diff = vec2_sub(a2, a1);
        ft_t t = vec2_cross(diff, s) / denom;
        ft_t u = vec2_cross(diff, r) / denom;
        if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
                if (cp) { cp->x = a1.x + r.x * t; cp->y = a1.y + r.y * t; }
                return true;
        }
        return false;
}

bool collision2d_check_point_line(vec2_t point, vec2_t p1, vec2_t p2, ft_t threshold)
{
        vec2_t ab = vec2_sub(p2, p1);
        vec2_t ap = vec2_sub(point, p1);
        ft_t ab_len_sq = vec2_length_squared(ab);
        if (ab_len_sq == 0)
                return vec2_length_squared(ap) <= threshold * threshold;
        ft_t proj = vec2_dot(ap, ab) / ab_len_sq;
        proj = fminf(fmaxf(proj, 0), 1);
        vec2_t closest = vec2_add(p1, vec2_scale(ab, proj));
        vec2_t diff = vec2_sub(point, closest);
        return vec2_length_squared(diff) <= threshold * threshold;
}

bool collision2d_check_circles(circle_t a, circle_t b)
{
        vec2_t distance = vec2_sub(b.center, a.center);
        ft_t dist_sq = vec2_length_squared(distance);
        ft_t radius_sum = a.radius + b.radius;
        return dist_sq <= radius_sum * radius_sum;
}

bool collision2d_check_rectangles(rectangle_t a, rectangle_t b)
{
        if (a.x > b.x + b.width) return false;
        if (a.x + a.width < b.x) return false;
        if (a.y > b.y + b.height) return false;
        if (a.y + a.height < b.y) return false;
        return true;
}

bool collision2d_check_circle_rectangle(circle_t a, rectangle_t b)
{
        vec2_t closest;
        closest.x = fminf(fmaxf(a.center.x, b.x), b.x + b.width);
        closest.y = fminf(fmaxf(a.center.y, b.y), b.y + b.height);
        vec2_t diff = vec2_sub(a.center, closest);
        return vec2_length_squared(diff) <= a.radius * a.radius;
}

/* ---- 辅助：获取多边形某条边 ---- */
static vec2_t poly_edge(polygon_t poly, int index)
{
        vec2_t curr = poly.vertices[index];
        vec2_t next = poly.vertices[(index + 1) % poly.num_vertices];
        return vec2_sub(next, curr);
}

/* ---- 辅助：计算多边形间最小分离 ---- */
static ft_t poly_min_sep(polygon_t a, polygon_t b, vec2_t* axis, vec2_t* point)
{
        ft_t separation = -FLT_MAX;
        for (int i = 0; i < a.num_vertices; i++) {
                vec2_t va = a.vertices[i];
                vec2_t normal = vec2_normal(poly_edge(a, i));
                ft_t min_sep = FLT_MAX;
                vec2_t min_vertex = {0,0};
                for (int j = 0; j < b.num_vertices; j++) {
                        vec2_t vb = b.vertices[j];
                        ft_t proj = vec2_dot(vec2_sub(vb, va), normal);
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

bool collision2d_check_polygons(polygon_t a, polygon_t b)
{
        vec2_t axis, point;
        ft_t ab_sep = poly_min_sep(a, b, &axis, &point);
        if (ab_sep >= 0) return false;
        ft_t ba_sep = poly_min_sep(b, a, &axis, &point);
        if (ba_sep >= 0) return false;
        return true;
}

bool collision2d_check_circle_polygon(circle_t a, polygon_t b)
{
        if (b.num_vertices < 3) return false;
        for (int i = 0; i < b.num_vertices; i++) {
                vec2_t e1 = b.vertices[i];
                vec2_t e2 = b.vertices[(i + 1) % b.num_vertices];
                vec2_t edge = vec2_sub(e2, e1);
                vec2_t to_center = vec2_sub(a.center, e1);
                ft_t edge_len_sq = vec2_length_squared(edge);
                if (edge_len_sq == 0) continue;
                ft_t t = vec2_dot(to_center, edge) / edge_len_sq;
                t = fminf(fmaxf(t, 0), 1);
                vec2_t closest = vec2_add(e1, vec2_scale(edge, t));
                vec2_t diff = vec2_sub(a.center, closest);
                if (vec2_length_squared(diff) <= a.radius * a.radius) return true;
        }
        {
                bool inside = true;
                for (int i = 0; i < b.num_vertices; i++) {
                        vec2_t e1 = b.vertices[i];
                        vec2_t e2 = b.vertices[(i + 1) % b.num_vertices];
                        vec2_t edge = vec2_sub(e2, e1);
                        vec2_t normal = vec2_normal(edge);
                        vec2_t to_center = vec2_sub(a.center, e1);
                        if (vec2_dot(to_center, normal) < 0) { inside = false; break; }
                }
                if (inside) return true;
        }
        return false;
}

int collision2d_get_circles(circle_t a, circle_t b, contact2d_t* contact)
{
        vec2_t distance = vec2_sub(b.center, a.center);
        ft_t dist_sq = vec2_length_squared(distance);
        ft_t radius_sum = a.radius + b.radius;
        if (dist_sq > radius_sum * radius_sum) return 0;
        contact->normal = vec2_normalize(distance);
        contact->sp = vec2_sub(b.center, vec2_scale(contact->normal, b.radius));
        contact->ep = vec2_add(a.center, vec2_scale(contact->normal, a.radius));
        contact->depth = vec2_length(vec2_sub(contact->ep, contact->sp));
        return 1;
}

int collision2d_get_rectangles(rectangle_t a, ft_t a_angle,
        rectangle_t b, ft_t b_angle, contact2d_t* contact)
{
        vec2_t a_bounds, b_bounds, a_h, b_h, a_pos, b_pos;
        mat22_t a_rot, b_rot, a_rotT, b_rotT, c, abs_c, abs_ct;
        vec2_t dp, a_d, b_d, abs_a_d, abs_b_d, a_face, b_face;
        vec2_t normal;
        ft_t separation, r_separation;

        a_bounds.x = a.width; a_bounds.y = a.height;
        b_bounds.x = b.width; b_bounds.y = b.height;
        a_h = vec2_scale(a_bounds, 0.5f);
        b_h = vec2_scale(b_bounds, 0.5f);
        a_pos.x = a.x + a_h.x; a_pos.y = a.y + a_h.y;
        b_pos.x = b.x + b_h.x; b_pos.y = b.y + b_h.y;

        ft_t a_res = a_angle * 3.14159265f / 180.0f;
        ft_t b_res = b_angle * 3.14159265f / 180.0f;
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

int collision2d_get_polygons(polygon_t a, polygon_t b, contact2d_t* contact)
{
        vec2_t a_axis, b_axis, a_point, b_point, distance;
        ft_t ab_sep = poly_min_sep(a, b, &a_axis, &a_point);
        if (ab_sep >= 0) return 0;
        ft_t ba_sep = poly_min_sep(b, a, &b_axis, &b_point);
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

ray3d_collision_t
collision3d_get_ray_triangle(ray3d_t ray, vec3_t p1, vec3_t p2, vec3_t p3)
{
        const ft_t EPSILON = 0.000001f;
        ray3d_collision_t result = {0};
        vec3_t edge1, edge2, p, q, tv;
        ft_t det, inv_det, u, v, t;

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

ray3d_collision_t 
collision3d_get_ray_quad(ray3d_t ray, vec3_t p1, vec3_t p2, vec3_t p3, vec3_t p4)
{
        ray3d_collision_t result = collision3d_get_ray_triangle(ray, p1, p2, p4);
        if (!result.hit) result = collision3d_get_ray_triangle(ray, p2, p3, p4);
        return result;
}

ray3d_collision_t 
collision3d_get_ray_sphere(ray3d_t ray, sphere_t sphere)
{
        ray3d_collision_t result = {0};
        vec3_t rs = vec3_sub(ray.position, sphere.position);
        ft_t dist_sq = vec3_square_length(rs);
        ft_t a = vec3_dot(rs, ray.direction);
        ft_t b = dist_sq - sphere.radius * sphere.radius;
        ft_t c = a * a - b;
        if (c < 0) return result;
        ft_t d = sqrtf(c);
        ft_t t = -a - d;
        if (t < 0) t = -a + d;
        if (t >= 0) {
                result.hit = true;
                result.distance = t;
                result.point = vec3_add(ray.position, vec3_scale(ray.direction, t));
                result.normal = vec3_normalize(vec3_sub(result.point, sphere.position));
        }
        return result;
}

ray3d_collision_t
collision3d_get_ray_box(ray3d_t ray, bounding_box_t box)
{
        ray3d_collision_t result = {0};
        vec3_t dirfrac;
        ft_t t1, t2, tmin, tmax;
        const ft_t EPSILON = 0.00001f;

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

        ft_t t = tmin >= 0 ? tmin : tmax;
        if (t < EPSILON) return result;

        result.hit = true;
        result.distance = t;
        result.point = vec3_add(ray.position, vec3_scale(ray.direction, t));

        vec3_t center = vec3_scale(vec3_add(box.min, box.max), 0.5f);
        vec3_t htc = vec3_sub(center, result.point);
        vec3_t half = vec3_scale(vec3_sub(box.max, box.min), 0.5f);
        vec3_t abs_hit = vec3_abs(htc);

        result.normal.x = (fabsf(abs_hit.x - half.x) < EPSILON) ? (htc.x > 0 ? 1 : -1) : 0;
        result.normal.y = (fabsf(abs_hit.y - half.y) < EPSILON) ? (htc.y > 0 ? 1 : -1) : 0;
        result.normal.z = (fabsf(abs_hit.z - half.z) < EPSILON) ? (htc.z > 0 ? 1 : -1) : 0;
        result.normal = vec3_normalize(result.normal);
        return result;
}

bool collision3d_check_spheres(sphere_t a, sphere_t b)
{
        vec3_t diff = vec3_sub(b.position, a.position);
        ft_t dist_sq = vec3_square_length(diff);
        ft_t radius_sum = a.radius + b.radius;
        return dist_sq <= radius_sum * radius_sum;
}

bool collision3d_check_boxes(bounding_box_t a, bounding_box_t b)
{
        return a.min.x <= b.max.x && a.max.x >= b.min.x
            && a.min.y <= b.max.y && a.max.y >= b.min.y
            && a.min.z <= b.max.z && a.max.z >= b.min.z;
}

bool collision3d_check_box_sphere(bounding_box_t a, sphere_t b)
{
        vec3_t closest;
        closest.x = fminf(fmaxf(b.position.x, a.min.x), a.max.x);
        closest.y = fminf(fmaxf(b.position.y, a.min.y), a.max.y);
        closest.z = fminf(fmaxf(b.position.z, a.min.z), a.max.z);
        vec3_t diff = vec3_sub(b.position, closest);
        return vec3_square_length(diff) <= b.radius * b.radius;
}

