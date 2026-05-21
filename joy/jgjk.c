#include "jgjk.h"
#include "external/klib/kvec.h"
#include <string.h>
#include <float.h>
#include <stddef.h>

/* ============ 内部类型 ============ */

/* 单纯形 (GJK用) */
typedef struct simplex3d {
	vec3f_t points[4];
	int points_num;
} simplex3d_t;

/* EPA面法线 */
typedef struct {
	vec3f_t normal;
	fp_t distance;
} epa_face_t;

/* EPA边 */
typedef struct {
	size_t a, b;
} epa_edge_t;

/* ============ 单纯形操作 ============ */

static void simplex_init1(simplex3d_t *s, vec3f_t p0) {
	s->points[0] = p0;
	s->points_num = 1;
}

static void simplex_init2(simplex3d_t *s, vec3f_t p0, vec3f_t p1) {
	s->points[0] = p0;
	s->points[1] = p1;
	s->points_num = 2;
}

static void simplex_init3(simplex3d_t *s, vec3f_t p0, vec3f_t p1, vec3f_t p2) {
	s->points[0] = p0;
	s->points[1] = p1;
	s->points[2] = p2;
	s->points_num = 3;
}

static void simplex_push(simplex3d_t *s, vec3f_t p) {
	s->points[3] = s->points[2];
	s->points[2] = s->points[1];
	s->points[1] = s->points[0];
	s->points[0] = p;
	if (s->points_num < 4) s->points_num++;
}

/* ============ 各碰撞体的最远点计算 ============ */

static vec3f_t find_furthest_point(gjk3d_collider_p c, vec3f_t dir) {
	vec3f_t result = { fp_zero(), fp_zero(), fp_zero() };

	switch (c->type) {
	case GJK3D_CTYPE_SPHERE: {
		vec3f_t ndir = vec3f_normalize(dir);
		vec3f_t offset = vec3f_scale(ndir, c->radius);
		result = vec3f_add(c->position, offset);
		break;
	}
	case GJK3D_CTYPE_BOX: {
		/* AABB: 根据方向符号取半边长 */
		fp_t sx = dir.x > fp_zero() ? c->half_size.x : -c->half_size.x;
		fp_t sy = dir.y > fp_zero() ? c->half_size.y : -c->half_size.y;
		fp_t sz = dir.z > fp_zero() ? c->half_size.z : -c->half_size.z;
		result.x = fp_add(c->position.x, sx);
		result.y = fp_add(c->position.y, sy);
		result.z = fp_add(c->position.z, sz);
		break;
	}
	case GJK3D_CTYPE_CAPSULE: {
		/* 胶囊: 线段(沿axis方向 ± height/2) + 半径 */
		fp_t len = vec3f_length(dir);
		if (len == fp_zero()) { result = c->position; break; }
		vec3f_t ndir = vec3f_scale(dir, fp_div(fp_one(), len));
		fp_t dot = vec3f_dot(ndir, c->axis);
		fp_t half_factor = dot > fp_zero() ? fp_half() : -fp_half();
		vec3f_t end_offset = vec3f_scale(c->axis, fp_mul(half_factor, c->height));
		vec3f_t center = vec3f_add(c->position, end_offset);
		result = vec3f_add(center, vec3f_scale(ndir, c->radius));
		break;
	}
	case GJK3D_CTYPE_CYLINDER: {
		/* 圆柱: 底部圆盘 + 顶部圆盘 */
		fp_t len = vec3f_length(dir);
		if (len == fp_zero()) { result = c->position; break; }
		vec3f_t ndir = vec3f_scale(dir, fp_div(fp_one(), len));
		fp_t hy = ndir.y > fp_zero() ? c->height : fp_zero();
		/* 投影到水平面求圆盘边界 */
		fp_t r_scale = fp_sqrt(fp_sub(fp_one(), fp_mul(ndir.y, ndir.y)));
		result.x = fp_add(c->position.x, fp_mul(ndir.x, fp_mul(r_scale, c->radius)));
		result.y = fp_add(c->position.y, hy);
		result.z = fp_add(c->position.z, fp_mul(ndir.z, fp_mul(r_scale, c->radius)));
		break;
	}
	case GJK3D_CTYPE_CONE: {
		/* 圆锥: 锥顶在 center + (0, height/2, 0)，底在 center - (0, height/2, 0) */
		fp_t len = vec3f_length(dir);
		if (len == fp_zero()) { result = c->position; break; }
		vec3f_t ndir = vec3f_scale(dir, fp_div(fp_one(), len));
		fp_t sin_theta = c->radius / fp_sqrt(fp_add(fp_pow2(c->radius), fp_pow2(c->height)));
		if (ndir.y > sin_theta) {
			/* 方向朝上足够多 → 锥顶 */
			result = c->position;
			result.y = fp_add(result.y, c->height);
		} else {
			/* 底边圆盘 */
			fp_t dist = fp_sqrt(fp_add(fp_pow2(ndir.x), fp_pow2(ndir.z)));
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
	case GJK3D_CTYPE_MESH: {
		fp_t max_dot = fp_min_value();
		for (int i = 0; i < c->vertices_num; i++) {
			fp_t d = vec3f_dot(c->vertices[i], dir);
			if (d > max_dot) { max_dot = d; result = c->vertices[i]; }
		}
		break;
	}
	}
	return result;
}

/* ============ Minkowski差支持函数 ============ */

static vec3f_t get_support_point(gjk3d_collider_p ca, gjk3d_collider_p cb, vec3f_t dir) {
	vec3f_t a = find_furthest_point(ca, dir);
	vec3f_t b = find_furthest_point(cb, vec3f_negate(dir));
	return vec3f_sub(a, b);
}

/* ============ GJK核心 ============ */

static bool same_direction(vec3f_t dir, vec3f_t ao) {
	return vec3f_dot(dir, ao) > fp_zero();
}

static bool gjk_line(simplex3d_t *s, vec3f_t *dir) {
	vec3f_t a = s->points[0], b = s->points[1];
	vec3f_t ab = vec3f_normalize(vec3f_sub(b, a));
	vec3f_t ao = vec3f_normalize(vec3f_negate(a));
	if (same_direction(ab, ao)) {
		*dir = vec3f_normalize(vec3f_cross(vec3f_cross(ab, ao), ab));
	} else {
		simplex_init1(s, a);
		*dir = ao;
	}
	return false;
}

static bool gjk_triangle(simplex3d_t *s, vec3f_t *dir) {
	vec3f_t a = s->points[0], b = s->points[1], c = s->points[2];
	vec3f_t ab = vec3f_normalize(vec3f_sub(b, a));
	vec3f_t ac = vec3f_normalize(vec3f_sub(c, a));
	vec3f_t ao = vec3f_normalize(vec3f_negate(a));
	vec3f_t abc = vec3f_normalize(vec3f_cross(ab, ac));

	if (same_direction(vec3f_cross(abc, ac), ao)) {
		if (same_direction(ac, ao)) {
			simplex_init2(s, a, c);
			*dir = vec3f_normalize(vec3f_cross(vec3f_cross(ac, ao), ac));
		} else {
			simplex_init2(s, a, b);
			return gjk_line(s, dir);
		}
	} else {
		if (same_direction(vec3f_cross(ab, abc), ao)) {
			simplex_init2(s, a, b);
			return gjk_line(s, dir);
		} else {
			if (same_direction(abc, ao)) {
				*dir = abc;
			} else {
				simplex_init3(s, a, c, b);
				*dir = vec3f_negate(abc);
			}
		}
	}
	return false;
}

static bool gjk_tetrahedron(simplex3d_t *s, vec3f_t *dir) {
	vec3f_t a = s->points[0], b = s->points[1], c = s->points[2], d = s->points[3];
	vec3f_t ab = vec3f_normalize(vec3f_sub(b, a));
	vec3f_t ac = vec3f_normalize(vec3f_sub(c, a));
	vec3f_t ad = vec3f_normalize(vec3f_sub(d, a));
	vec3f_t ao = vec3f_normalize(vec3f_negate(a));
	vec3f_t abc = vec3f_normalize(vec3f_cross(ab, ac));
	vec3f_t acd = vec3f_normalize(vec3f_cross(ac, ad));
	vec3f_t adb = vec3f_normalize(vec3f_cross(ad, ab));

	if (same_direction(abc, ao)) { simplex_init3(s, a, b, c); return gjk_triangle(s, dir); }
	if (same_direction(acd, ao)) { simplex_init3(s, a, c, d); return gjk_triangle(s, dir); }
	if (same_direction(adb, ao)) { simplex_init3(s, a, d, b); return gjk_triangle(s, dir); }
	return true;
}

static bool next_simplex(simplex3d_t *s, vec3f_t *dir) {
	switch (s->points_num) {
	case 2: return gjk_line(s, dir);
	case 3: return gjk_triangle(s, dir);
	case 4: return gjk_tetrahedron(s, dir);
	}
	return false;
}

/* ============ EPA (扩展多面体算法) ============ */

/* 计算所有面法线和最近面索引 */
static void get_face_normals(kvec_t(vec3f_t) *polytope, kvec_t(size_t) *faces,
	kvec_t(epa_face_t) *normals, size_t *min_triangle) {
	*min_triangle = 0;
	fp_t min_distance = fp_max_value();

	for (size_t i = 0; i < faces->n; i += 3) {
		vec3f_t a = kv_A(*polytope, kv_A(*faces, i));
		vec3f_t b = kv_A(*polytope, kv_A(*faces, i + 1));
		vec3f_t c = kv_A(*polytope, kv_A(*faces, i + 2));

		vec3f_t ba = vec3f_normalize(vec3f_sub(b, a));
		vec3f_t ca = vec3f_normalize(vec3f_sub(c, a));
		vec3f_t n = vec3f_normalize(vec3f_cross(ba, ca));

		fp_t dist = vec3f_dot(n, a);
		if (dist < fp_zero()) { n = vec3f_negate(n); dist = -dist; }

		epa_face_t face;
		face.normal = n;
		face.distance = dist;
		kv_push(epa_face_t, *normals, face);

		if (dist < min_distance) { min_distance = dist; *min_triangle = i / 3; }
	}
}

/* 添加不重复边(EPA用) */
static void add_unique_edge(kvec_t(epa_edge_t) *edges, kvec_t(size_t) *faces, size_t a, size_t b) {
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
	epa_edge_t edge = { fa, fb };
	kv_push(epa_edge_t, *edges, edge);
}

/* EPA主函数: 从单纯形出发构建多面体,计算穿透法线和深度 */
static bool epa(simplex3d_t *simplex, gjk3d_collider_p ca, gjk3d_collider_p cb, gjk3d_contact_p contact) {
	kvec_t(vec3f_t) polytope;
	kvec_t(size_t) faces;
	kvec_t(epa_face_t) normals;

	kv_init(polytope);
	kv_init(faces);
	kv_init(normals);

	/* 将单纯形点复制到多面体 */
	for (int i = 0; i < simplex->points_num; i++) {
		kv_push(vec3f_t, polytope, simplex->points[i]);
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
	vec3f_t bake_min_normal = kv_A(normals, min_face).normal;
	fp_t bake_min_distance = kv_A(normals, min_face).distance;

	vec3f_t min_normal = { fp_zero(), fp_zero(), fp_zero() };
	fp_t min_distance = fp_max_value();

	int epa_iter = 0;
	while (min_distance == fp_max_value() && epa_iter < 64) {
		epa_iter++;
		min_normal = kv_A(normals, min_face).normal;
		min_distance = kv_A(normals, min_face).distance;

		vec3f_t support = get_support_point(ca, cb, min_normal);
		fp_t s_dist = vec3f_dot(min_normal, support);

		if (fp_abs(fp_sub(s_dist, min_distance)) > fp_from_float(0.001f)) {
			min_distance = fp_max_value();

			/* 找到需要移除的面,收集边界边 */
			kvec_t(epa_edge_t) unique_edges;
			kv_init(unique_edges);

			for (size_t i = 0; i < normals.n; i++) {
				vec3f_t n = kv_A(normals, i).normal;
				size_t f = i * 3;
				if (vec3f_dot(n, support) > vec3f_dot(n, kv_A(polytope, kv_A(faces, f)))) {
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
			kvec_t(size_t) new_faces;
			kv_init(new_faces);
			size_t new_vertex_index = polytope.n;
			for (size_t i = 0; i < unique_edges.n; i++) {
				kv_push(size_t, new_faces, kv_A(unique_edges, i).a);
				kv_push(size_t, new_faces, kv_A(unique_edges, i).b);
				kv_push(size_t, new_faces, new_vertex_index);
			}
			kv_push(vec3f_t, polytope, support);

			/* 计算新面的法线 */
			kvec_t(epa_face_t) new_normals;
			kv_init(new_normals);
			size_t new_min_face = 0;
			get_face_normals(&polytope, &new_faces, &new_normals, &new_min_face);

			/* 找到全局最小距离的面 */
			fp_t old_min_dist = fp_max_value();
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
				kv_push(epa_face_t, normals, kv_A(new_normals, i));
			}

			kv_destroy(new_faces);
			kv_destroy(new_normals);
			kv_destroy(unique_edges);
		}
	}

	contact->normal = min_normal;
	contact->depth = fp_add(min_distance, fp_from_float(0.001f));

	kv_destroy(polytope);
	kv_destroy(faces);
	kv_destroy(normals);

	return true;
}

/* ============ 公开API实现 ============ */

bool gjk3d_collide(gjk3d_collider_p c1, gjk3d_collider_p c2,
	vec3f_t init_dir, gjk3d_contact_p contact)
{
	simplex3d_t simplex;
	memset(&simplex, 0, sizeof(simplex));

	vec3f_t support = get_support_point(c1, c2, init_dir);
	simplex_push(&simplex, support);
	vec3f_t dir = vec3f_normalize(vec3f_negate(support));

	int gjk_iter = 0;
	while (gjk_iter < 128) {
		gjk_iter++;
		support = get_support_point(c1, c2, dir);
		if (vec3f_dot(support, dir) < fp_zero()) {
			/* 无碰撞 */
			contact->normal.x = fp_zero();
			contact->normal.y = fp_zero();
			contact->normal.z = fp_zero();
			contact->depth = fp_zero();
			return false;
		}
		simplex_push(&simplex, support);
		if (next_simplex(&simplex, &dir)) {
			return epa(&simplex, c1, c2, contact);
		}
	}

	contact->normal.x = fp_zero();
	contact->normal.y = fp_zero();
	contact->normal.z = fp_zero();
	contact->depth = fp_zero();
	return false;
}

/* ============ 碰撞体初始化函数 ============ */

void gjk3d_init_sphere(gjk3d_collider_t *c, vec3f_t pos, fp_t radius) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_SPHERE;
	c->position = pos;
	c->radius = radius;
}

void gjk3d_init_box(gjk3d_collider_t *c, vec3f_t pos, vec3f_t half_size) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_BOX;
	c->position = pos;
	c->half_size = half_size;
}

void gjk3d_init_capsule(gjk3d_collider_t *c, vec3f_t pos, fp_t radius, fp_t height, vec3f_t axis) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_CAPSULE;
	c->position = pos;
	c->radius = radius;
	c->height = height;
	c->axis = vec3f_normalize(axis);
}

void gjk3d_init_cylinder(gjk3d_collider_t *c, vec3f_t pos, fp_t radius, fp_t height) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_CYLINDER;
	c->position = pos;
	c->radius = radius;
	c->height = height;
}

void gjk3d_init_cone(gjk3d_collider_t *c, vec3f_t pos, fp_t radius, fp_t height) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_CONE;
	c->position = pos;
	c->radius = radius;
	c->height = height;
}

void gjk3d_init_mesh(gjk3d_collider_t *c, vec3f_t *vertices, int num) {
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
	vec2f_t points[3];
	int points_num;
} simplex2d_t;

static void simplex2d_push(simplex2d_t *s, vec2f_t p) {
	s->points[2] = s->points[1];
	s->points[1] = s->points[0];
	s->points[0] = p;
	if (s->points_num < 3) s->points_num++;
}

static vec2f_t simplex2d_a(const simplex2d_t *s) { return s->points[0]; }
static vec2f_t simplex2d_b(const simplex2d_t *s) { return s->points[1]; }
static vec2f_t simplex2d_c(const simplex2d_t *s) { return s->points[2]; }

/* 2D向量三重积: b*(a·c) - a*(b·c) */
static vec2f_t vec2f_triple(vec2f_t a, vec2f_t b, vec2f_t c) {
	fp_t ac = vec2f_dot(a, c);
	fp_t bc = vec2f_dot(b, c);
	return vec2f_sub(vec2f_scale(b, ac), vec2f_scale(a, bc));
}

static vec2f_t perp_cw(vec2f_t v) {
	vec2f_t r;
	r.x = v.y;
	r.y = -v.x;
	return r;
}

/* ---------- 各碰撞体的最远点计算 ---------- */

static vec2f_t find_furthest_point_2d(gjk2d_collider_p c, vec2f_t dir) {
	vec2f_t result = { fp_zero(), fp_zero() };

	switch (c->type) {
	case GJK2D_CTYPE_CIRCLE: {
		fp_t len = vec2f_length(dir);
		if (len < fp_from_float(1e-8f)) { result = c->position; break; }
		vec2f_t ndir = vec2f_scale(dir, fp_div(fp_one(), len));
		result = vec2f_add(c->position, vec2f_scale(ndir, c->radius));
		break;
	}
	case GJK2D_CTYPE_BOX: {
		fp_t sx = dir.x > fp_zero() ? c->half_size.x : -c->half_size.x;
		fp_t sy = dir.y > fp_zero() ? c->half_size.y : -c->half_size.y;
		result.x = fp_add(c->position.x, sx);
		result.y = fp_add(c->position.y, sy);
		break;
	}
	case GJK2D_CTYPE_CAPSULE: {
		fp_t len = vec2f_length(dir);
		if (len == fp_zero()) { result = c->position; break; }
		vec2f_t ndir = vec2f_scale(dir, fp_div(fp_one(), len));
		fp_t dot = vec2f_dot(ndir, c->axis);
		fp_t half_h = dot > fp_zero() ? fp_mul(fp_half(), c->height) : fp_mul(-fp_half(), c->height);
		vec2f_t center = vec2f_add(c->position, vec2f_scale(c->axis, half_h));
		result = vec2f_add(center, vec2f_scale(ndir, c->radius));
		break;
	}
	case GJK2D_CTYPE_POLYGON: {
		fp_t max_dot = fp_min_value();
		for (int i = 0; i < c->vertices_num; i++) {
			fp_t d = vec2f_dot(c->vertices[i], dir);
			if (d > max_dot) { max_dot = d; result = c->vertices[i]; }
		}
		break;
	}
	case GJK2D_CTYPE_PIE: {
		/* 扇形: 朝向 +X, 半角 sweep/2 */
		fp_t h = fp_mul(c->sweep, fp_half());
		fp_t ch = fp_cos(h), sh = fp_sin(h);
		fp_t zero = fp_zero();
		vec2f_t best = c->position;
		fp_t max_dot = vec2f_dot(best, dir);

		/* 检查弧上点 */
		fp_t len = vec2f_length(dir);
		if (len > zero) {
			vec2f_t ndir = vec2f_scale(dir, fp_div(fp_one(), len));
			/* 方向在锥内: dir.x>0 且 |dir.y| <= dir.x * tan(h) = dir.x * sh/ch */
			fp_t dy_abs = fp_abs(ndir.y);
			fp_t dx_tan = fp_mul(ndir.x, fp_div(sh, ch));
			if (ndir.x > zero && dy_abs <= dx_tan) {
				vec2f_t arcPt = vec2f_add(c->position, vec2f_scale(ndir, c->radius));
				fp_t d = vec2f_dot(arcPt, dir);
				if (d > max_dot) { max_dot = d; best = arcPt; }
			}
		}

		/* 检查两个角点 */
		vec2f_t corners[2] = {
			{ fp_add(c->position.x, fp_mul(c->radius, ch)), fp_add(c->position.y, fp_mul(c->radius, sh)) },
			{ fp_add(c->position.x, fp_mul(c->radius, ch)), fp_sub(c->position.y, fp_mul(c->radius, sh)) }
		};
		for (int i = 0; i < 2; i++) {
			fp_t d = vec2f_dot(corners[i], dir);
			if (d > max_dot) { max_dot = d; best = corners[i]; }
		}
		result = best;
		break;
	}
	}
	return result;
}

/* ---------- Minkowski差支持函数 ---------- */

static vec2f_t get_support_point_2d(gjk2d_collider_p ca, gjk2d_collider_p cb, vec2f_t dir) {
	vec2f_t a = find_furthest_point_2d(ca, dir);
	vec2f_t b = find_furthest_point_2d(cb, vec2f_negate(dir));
	return vec2f_sub(a, b);
}

/* ---------- 2D单纯形检测 ---------- */

static bool simplex2d_check(simplex2d_t *s, vec2f_t *dir) {
	vec2f_t a = simplex2d_a(s);
	vec2f_t ao = vec2f_negate(a);    /* 从a指向原点 */
	vec2f_t ab = vec2f_sub(simplex2d_b(s), a);

	if (s->points_num == 2) {
		*dir = vec2f_triple(ab, ao, ab);
		fp_t m2 = vec2f_dot(*dir, *dir);
		if (m2 < fp_from_float(1.2e-7f)) {  /* C# MathX.EPSILON≈1.19e-7 */
			*dir = perp_cw(ab);
		}
		*dir = vec2f_normalize(*dir);
	} else if (s->points_num == 3) {
		vec2f_t ac = vec2f_sub(simplex2d_c(s), a);

		/* v = triple(ab, ac, ac): 垂直于ac的向量,指向ab侧 */
		vec2f_t v = vec2f_triple(ab, ac, ac);
		if (vec2f_dot(v, ao) >= fp_zero()) {
			/* 原点在ac外侧, 丢弃b, 保留 {a, c} */
			s->points[1] = s->points[2];
			*dir = vec2f_normalize(v);
		} else {
			/* u = triple(ac, ab, ab): 垂直于ab的向量,指向ac侧 */
			vec2f_t u = vec2f_triple(ac, ab, ab);
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
	vec2f_t p1, p2;
	vec2f_t normal;
	fp_t distance;
} epa2d_edge_t;

/* 确定缠绕方向 */
static int get_winding_2d(const simplex2d_t *s) {
	fp_t total = fp_zero();
	for (int i = 0; i < s->points_num; i++) {
		vec2f_t p1 = s->points[i];
		vec2f_t p2 = s->points[(i + 1) % s->points_num];
		total = fp_add(total, vec2f_cross(p1, p2));
	}
	return total > fp_zero() ? 1 : -1;
}

static void build_edges_2d(epa2d_edge_t *edges, int *n, const simplex2d_t *s, int winding) {
	for (int i = 0; i < s->points_num; i++) {
		vec2f_t p1 = s->points[i];
		vec2f_t p2 = s->points[(i + 1) % s->points_num];
		/* 外向法线 */
		vec2f_t d = vec2f_sub(p2, p1);
		vec2f_t nml;
		if (winding > 0) {
			/* 顺时针: CW90 (x,y)->(y,-x) */
			nml.x = d.y;
			nml.y = -d.x;
		} else {
			/* 逆时针: CCW90 (x,y)->(-y,x) */
			nml.x = -d.y;
			nml.y = d.x;
		}
		fp_t nl = vec2f_length(nml);
		if (nl > fp_zero()) {
			nml = vec2f_scale(nml, fp_div(fp_one(), nl));
		}
		fp_t dist = fp_abs(vec2f_dot(p1, nml));
		edges[*n].p1 = p1;
		edges[*n].p2 = p2;
		edges[*n].normal = nml;
		edges[*n].distance = dist;
		(*n)++;
	}
}

/* 按距离排序 */
static void sort_edges_2d(epa2d_edge_t *edges, int n) {
	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < n - 1 - i; j++) {
			if (edges[j].distance > edges[j + 1].distance) {
				epa2d_edge_t tmp = edges[j];
				edges[j] = edges[j + 1];
				edges[j + 1] = tmp;
			}
		}
	}
}

static bool epa2d(simplex2d_t *simplex, gjk2d_collider_p ca, gjk2d_collider_p cb, gjk2d_contact_p contact) {
	if (simplex->points_num < 3) return false;

	int winding = get_winding_2d(simplex);

	epa2d_edge_t edges[3 * 64]; /* 最多64次迭代,每次最多增加2条边 */
	int edge_count = 0;
	build_edges_2d(edges, &edge_count, simplex, winding);
	sort_edges_2d(edges, edge_count);

	vec2f_t best_normal = edges[0].normal;
	fp_t best_dist = edges[0].distance;
	fp_t eps = fp_from_float(0.001f);

	for (int iter = 0; iter < 64; iter++) {
		vec2f_t support = get_support_point_2d(ca, cb, edges[0].normal);
		fp_t s_dist = vec2f_dot(support, edges[0].normal);

		if (fp_abs(fp_sub(s_dist, edges[0].distance)) <= eps) {
			best_normal = edges[0].normal;
			best_dist = s_dist;
			break;
		}

		/* 用新点扩展: 将最近的边替换为两条新边 */
		vec2f_t p1 = edges[0].p1;
		vec2f_t p2 = edges[0].p2;
		edges[0] = edges[edge_count - 1];
		edge_count--;

		/* 添加边 p1→support */
		{
			vec2f_t d = vec2f_sub(support, p1);
			vec2f_t nml;
			if (winding > 0) {
				nml.x = d.y; nml.y = -d.x;
			} else {
				nml.x = -d.y; nml.y = d.x;
			}
			fp_t nl = vec2f_length(nml);
			if (nl > fp_zero()) nml = vec2f_scale(nml, fp_div(fp_one(), nl));
			edges[edge_count].p1 = p1;
			edges[edge_count].p2 = support;
			edges[edge_count].normal = nml;
			edges[edge_count].distance = fp_abs(vec2f_dot(p1, nml));
			edge_count++;
		}

		/* 添加边 support→p2 */
		{
			vec2f_t d = vec2f_sub(p2, support);
			vec2f_t nml;
			if (winding > 0) {
				nml.x = d.y; nml.y = -d.x;
			} else {
				nml.x = -d.y; nml.y = d.x;
			}
			fp_t nl = vec2f_length(nml);
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

bool gjk2d_collide(gjk2d_collider_p c1, gjk2d_collider_p c2,
	vec2f_t init_dir, gjk2d_contact_p contact)
{
	simplex2d_t simplex;
	memset(&simplex, 0, sizeof(simplex));

	vec2f_t dir = init_dir;
	if (vec2f_length_squared(dir) < fp_from_float(1.2e-7f)) {
		dir.x = fp_one();
	}

	vec2f_t support = get_support_point_2d(c1, c2, dir);
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
		fp_t dlen = vec2f_length(dir);
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

void gjk2d_init_circle(gjk2d_collider_t *c, vec2f_t pos, fp_t radius) {
	memset(c, 0, sizeof(*c));
	c->type = GJK2D_CTYPE_CIRCLE;
	c->position = pos;
	c->radius = radius;
}

void gjk2d_init_box(gjk2d_collider_t *c, vec2f_t pos, vec2f_t half_size) {
	memset(c, 0, sizeof(*c));
	c->type = GJK2D_CTYPE_BOX;
	c->position = pos;
	c->half_size = half_size;
}

void gjk2d_init_capsule(gjk2d_collider_t *c, vec2f_t pos, vec2f_t axis, fp_t height, fp_t radius) {
	memset(c, 0, sizeof(*c));
	c->type = GJK2D_CTYPE_CAPSULE;
	c->position = pos;
	c->axis = vec2f_normalize(axis);
	c->height = height;
	c->radius = radius;
}

void gjk2d_init_polygon(gjk2d_collider_t *c, vec2f_t *vertices, int num) {
	memset(c, 0, sizeof(*c));
	c->type = GJK2D_CTYPE_POLYGON;
	c->vertices = vertices;
	c->vertices_num = num;
}

void gjk2d_init_pie(gjk2d_collider_t *c, vec2f_t pos, fp_t radius, fp_t sweep) {
	memset(c, 0, sizeof(*c));
	c->type = GJK2D_CTYPE_PIE;
	c->position = pos;
	c->radius = radius;
	c->sweep = sweep;
}
