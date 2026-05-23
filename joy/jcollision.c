#include "jcollision.h"

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
	ȡA B��������İ뾶,bounds�д洢���Ǳ߳�
	�����ȡ������һ��ĳ��ȣ�Ҳ������Ϊ����
	�������������ϵ�µĵ�һ���޵ĵ㣬��������ϵ�Ծ�������Ϊ(0,0)��
	��ôa_h��b_h�����Ͻǵ�һ���޵ĵ�
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
        ȡA B����������������꣬���屾������������ľֲ�����
	position�洢�ľ���������ʵ�����������λ�ã�����������������
	������Ϊ��ȫ����Ļ���꣬Ҳ������ʵ�ʿ���������
        */
        a_pos.x = fp_add(a.x, a_h.x);
        a_pos.y = fp_add(a.y, a_h.y);
        b_pos.x = fp_add(b.x, b_h.x);
        b_pos.y = fp_add(b.y, b_h.y);

        /*
        ��ȡA B����������ת�ǣ�Ȼ�������ת����
	��Ϊ�Ƕ�ά�ռ��е���ת�����Ծ�����2x2�ģ�
        */
        a_res = fp_div(fp_mul(a_angle, fp_pi()), fp_from_float(180));
        b_res = fp_div(fp_mul(b_angle, fp_pi()), fp_from_float(180));
	a_rot = mat22f_rotate(a_res);
	b_rot = mat22f_rotate(b_res);

        /*
        ��������ת����ת��
	�������a���Ծ���a_rot�����ǽ�a���Լ��ľֲ�����ϵA�任���������꣬
	�任���a�ڳ���b_rotT���ǽ�a�任���ֲ�����ϵB�¡����յ�Ч�����ǽ�
	����a��A����ϵ�任��B����ϵ�¡�
        */
	a_rotT = mat22f_transpose(a_rot);
	b_rotT = mat22f_transpose(b_rot);

        
	/* ������������ĵ�����õ��ľ������������������ߵ����� */
	dp = vec2f_sub(b_pos, a_pos);

        /*
        �������߷ֱ����������Ե�ת�þ���
	�ͻὫ��������dp�任�����Եľֲ�����ϵ��
        */
	a_d = mat22f_mul_vec2f(a_rotT, dp);
	b_d = mat22f_mul_vec2f(b_rotT, dp);

	c = mat22f_mul(a_rotT, b_rot);
	abs_c = mat22f_abs(c);
	abs_ct = mat22f_transpose(abs_c);

        /*
        Box A faces
	a_d�����������յ����ߵ�����������Abs�����a_d��ʾ��ֻ����ֵ���塣
	ʵ���ϣ�����Ĵ�����Abs(a_d) - (a_h + abs_c * b_h)
	a_h���Ǿ���A���Ͻǵĵ㣬abs_c * b_h�������B����A���εľֲ�����ϵ�µ��ĸ�������
	x,y�����ϵ����ֵ����Ϊa_d��a_h,b_h���Ǵ�(0,0)����������������������е���ֵ����
	��ʾ��ֵ����
	Abs(a_d) - (a_h + abs_c * b_h)�������ʽ�ӣ����ֻ��x��
	ʵ���Ͼ���a_d��x�����ʾ�������ε�����������x��ͶӰ����Ϊs��
	a_h�Ǿ���A��x�������������ߵ����ͶӰ����a,abs_c*b_h���Ǿ���B��x�������������ߵ����ͶӰ����b
	���s-a-b����0����˵��ͶӰ���ཻ��
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

	/* B��A���Ҳ���x�������Σ������x�Ḻ����*/
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

