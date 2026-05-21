#include "jcollision.h"

ray2d_collisionf_t 
collision2df_get_ray_circle(ray2df_t ray, circlef_t circle)
{
        vec2f_t seg;
        fp_t r_sq, d_sq, seg_len_sq, seg_proj, dx;
        ray2d_collisionf_t result;
        vec2f_t distance;

        result.hit = false;

        r_sq = fp_pow2(circle.radius);
        seg = vec2f_sub(circle.center, ray.origin);

        seg_len_sq = vec2f_length_squared(seg);
        ray.direction = vec2f_normalize(ray.direction);
        seg_proj = vec2f_dot(seg, ray.direction);

        /* ��Բ��, �����ֽǶ� */
        if (seg_len_sq == r_sq) {
                result.hit = true;
                result.point = ray.origin;
                return result;
        }

        /* ��Բ�� */
        if (seg_len_sq < r_sq) {
                fp_t t = fp_sqrt(fp_sub(r_sq, fp_sub(seg_len_sq, fp_pow2(seg_proj))));
                distance = vec2f_scale(ray.direction, fp_add(t, seg_proj));
                result.hit = true;
                result.point = vec2f_add(ray.origin, distance);
                return result;
        }

        /* ��Բ��, �����߼нǴ���90, û���ཻ */
        if (seg_proj < fp_zero()) {
                return result;
        }

        /* ��Բ��, �����߼н�С��90 */
        /* ��Բ�������ߵĴ��߳���(d) */
        d_sq = fp_sub(seg_len_sq, fp_pow2(seg_proj));
        if (d_sq > r_sq) {
                return result;
        }
        else if (d_sq == r_sq) {
                result.hit = true;
                result.point = vec2f_add(ray.origin,
                         vec2f_scale(ray.direction, seg_proj));
                return result;
        }
        else {
                dx = fp_sqrt(fp_sub(r_sq, d_sq));
                result.hit = true;
                result.point = vec2f_add(ray.origin,
                         vec2f_scale(ray.direction, fp_sub(seg_proj, dx)));
                return result;
        }
}

// 辅助函数：计算矩形碰撞点的法线（轴对齐边界专用）
static vec2f_t collision2df_calculate_rect_normal(vec2f_t point, vec2f_t min, vec2f_t max)
{
        vec2f_t normal = { fp_zero(), fp_zero() };
        const fp_t epsilon = fp_from_float(1); // 极小值，避免浮点误差

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

        /* ������ת����OBB�ľֲ�����ϵ(������ת)*/
        local_origin = vec2f_sub(ray.origin, center);
        local_origin = vec2f_rotate(local_origin, -rotation); // ����ת
        local_dir = vec2f_rotate(ray.direction, -rotation);

        t_min = fp_zero();
        t_max = fp_max_value();

        /* check y axis */
        if (fp_abs(local_dir.x) < EPSILON){
                if (local_origin.x < -half_extents.x 
                        || local_origin.x > half_extents.x)
                        return result;
        }
        else{
                inv_dir = fp_div(fp_one(), local_dir.x);
                t1 = fp_mul(fp_sub(-half_extents.x, local_origin.x), inv_dir);
                t2 = fp_mul(fp_sub(half_extents.x, local_origin.x), inv_dir);
                if (t1 > t2) {
                        tmp = t1;
                        t1 = t2;
                        t2 = tmp;
                }
                t_min = fp_max(t_min, t1);
                t_max = fp_min(t_max, t2);
                if (t_min > t_max)
                        return result;
        }

        /* check x axis */
        if (fp_abs(local_dir.y) < EPSILON){
                if (local_origin.y < -half_extents.y 
                        || local_origin.y > half_extents.y)
                        return result;
        }
        else{
                inv_dir = fp_div(fp_one(), local_dir.y);
                t1 = fp_mul(fp_sub(-half_extents.y, local_origin.y), inv_dir);
                t2 = fp_mul(fp_sub(half_extents.y, local_origin.y), inv_dir);
                if (t1 > t2) {
                        tmp = t1;
                        t1 = t2;
                        t2 = tmp;
                }
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
        result.normal = vec2f_normalize(vec2f_sub(result.point, ray.origin));
        return result;
}

ray2d_collisionf_t 
collision2df_get_ray_polygon(ray2df_t ray, polygonf_t polygon)
{
        ray2d_collisionf_t result;
        return result;
}

bool 
collision2df_check_point_rectangle(vec2f_t point, rectanglef_t rect)
{
        return false;
}

bool 
collision2df_check_point_circle(vec2f_t point, circlef_t circle)
{
        return false;
}

bool 
collision2df_check_point_triangle(vec2f_t point, vec2f_t p1, vec2f_t p2, vec2f_t p3)
{
        return false;
}

bool 
collision2df_check_lines(vec2f_t a1, vec2f_t b1, vec2f_t a2, vec2f_t b2, vec2f_t*cp)
{
        return false;
}

bool 
collision2df_check_point_line(vec2f_t point, vec2f_t p1, vec2f_t p2, fp_t threshold)
{
        return false;
}

bool 
collision2df_check_circles(circlef_t a, circlef_t b)
{
        bool collision;
        vec2f_t distance;
        fp_t distance_squared, radius_sum;

        distance = vec2f_sub(b.center, a.center);
        distance_squared = vec2f_length_squared(distance);
        radius_sum = fp_add(b.radius, a.radius);
        collision = distance_squared <= fp_pow2(radius_sum);

        return collision;
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

bool 
collision2df_check_polygons(polygonf_t a, polygonf_t b)
{
        return false;
}

bool 
collision2df_check_circle_rectangle(circlef_t a, rectanglef_t b)
{
        return false;
}

bool 
collision2df_check_circle_polygon(circlef_t a, polygonf_t b)
{
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


JOY_INLINE vec2f_t edge_at(polygonf_t poly, int index)
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
        ray3d_collisionf_t result;
        return result;
}

ray3d_collisionf_t
collision3df_get_ray_box(ray3df_t ray, bounding_boxf_t box)
{
        ray3d_collisionf_t result;
        return result;
}

bool 
collision3df_check_spheres(spheref_t a, spheref_t b)
{
        return false;
}

bool 
collision3df_check_boxes(bounding_boxf_t a, bounding_boxf_t b)
{
        return false;
}

bool 
collision3df_check_box_sphere(bounding_boxf_t a, spheref_t b)
{
        return false;
}

