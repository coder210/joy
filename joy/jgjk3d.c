#include "external/klib/kvec.h"
#include "joy.h"

/* ======================================================================== */
/*              GJK 3D 浮点版 (gjk3d_* / ft_t / vec3_t)                     */
/* 算法与定点数版 jgjk.c 完全一致, 仅类型从 fp_t/vec3f_t 换成 ft_t/vec3_t   */
/* ======================================================================== */

/* ============ 内部类型 ============ */

/* 单纯形 (GJK用) */
typedef struct simplex3d {
	vec3_t points[4];
	int points_num;
} simplex3d_t;

/* EPA面法线 */
typedef struct {
	vec3_t normal;
	ft_t distance;
} epa_face_t;

/* EPA边 */
typedef struct {
	size_t a, b;
} epa_edge_t;

/* ============ 单纯形操作 ============ */

static void simplex_init1(simplex3d_t *s, vec3_t p0) {
	s->points[0] = p0;
	s->points_num = 1;
}

static void simplex_init2(simplex3d_t *s, vec3_t p0, vec3_t p1) {
	s->points[0] = p0;
	s->points[1] = p1;
	s->points_num = 2;
}

static void simplex_init3(simplex3d_t *s, vec3_t p0, vec3_t p1, vec3_t p2) {
	s->points[0] = p0;
	s->points[1] = p1;
	s->points[2] = p2;
	s->points_num = 3;
}

static void simplex_push(simplex3d_t *s, vec3_t p) {
	s->points[3] = s->points[2];
	s->points[2] = s->points[1];
	s->points[1] = s->points[0];
	s->points[0] = p;
	if (s->points_num < 4) s->points_num++;
}

/* ============ 各碰撞体的最远点计算 ============ */

static vec3_t find_furthest_point(gjk3d_collider_p c, vec3_t dir) {
	vec3_t result = { ft_zero(), ft_zero(), ft_zero() };

	switch (c->type) {
	case GJK3D_CTYPE_SPHERE: {
		vec3_t ndir = vec3_normalize(dir);
		vec3_t offset = vec3_scale(ndir, c->radius);
		result = vec3_add(c->position, offset);
		break;
	}
	case GJK3D_CTYPE_BOX: {
		/* AABB: 根据方向符号取半边长 */
		ft_t sx = dir.x > ft_zero() ? c->half_size.x : -c->half_size.x;
		ft_t sy = dir.y > ft_zero() ? c->half_size.y : -c->half_size.y;
		ft_t sz = dir.z > ft_zero() ? c->half_size.z : -c->half_size.z;
		result.x = ft_add(c->position.x, sx);
		result.y = ft_add(c->position.y, sy);
		result.z = ft_add(c->position.z, sz);
		break;
	}
	case GJK3D_CTYPE_CAPSULE: {
		/* 胶囊: 线段(沿axis方向 ± height/2) + 半径 */
		ft_t len = vec3_length(dir);
		if (len == ft_zero()) { result = c->position; break; }
		vec3_t ndir = vec3_scale(dir, ft_div(ft_one(), len));
		ft_t dot = vec3_dot(ndir, c->axis);
		ft_t half_factor = dot > ft_zero() ? ft_half() : -ft_half();
		vec3_t end_offset = vec3_scale(c->axis, ft_mul(half_factor, c->height));
		vec3_t center = vec3_add(c->position, end_offset);
		result = vec3_add(center, vec3_scale(ndir, c->radius));
		break;
	}
	case GJK3D_CTYPE_CYLINDER: {
		/* 圆柱: 底部圆盘 + 顶部圆盘 */
		ft_t len = vec3_length(dir);
		if (len == ft_zero()) { result = c->position; break; }
		vec3_t ndir = vec3_scale(dir, ft_div(ft_one(), len));
		ft_t hy = ndir.y > ft_zero() ? c->height : ft_zero();
		/* 投影到水平面求圆盘边界 */
		ft_t r_scale = ft_sqrt(ft_sub(ft_one(), ft_mul(ndir.y, ndir.y)));
		result.x = ft_add(c->position.x, ft_mul(ndir.x, ft_mul(r_scale, c->radius)));
		result.y = ft_add(c->position.y, hy);
		result.z = ft_add(c->position.z, ft_mul(ndir.z, ft_mul(r_scale, c->radius)));
		break;
	}
	case GJK3D_CTYPE_CONE: {
		/* 圆锥: 锥顶在 center + (0, height/2, 0)，底在 center - (0, height/2, 0) */
		ft_t len = vec3_length(dir);
		if (len == ft_zero()) { result = c->position; break; }
		vec3_t ndir = vec3_scale(dir, ft_div(ft_one(), len));
		ft_t sin_theta = c->radius / ft_sqrt(ft_add(ft_pow2(c->radius), ft_pow2(c->height)));
		if (ndir.y > sin_theta) {
			/* 方向朝上足够多 → 锥顶 */
			result = c->position;
			result.y = ft_add(result.y, c->height);
		} else {
			/* 底边圆盘 */
			ft_t dist = ft_sqrt(ft_add(ft_pow2(ndir.x), ft_pow2(ndir.z)));
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
		ft_t max_dot = ft_min_value();
		for (int i = 0; i < c->vertices_num; i++) {
			ft_t d = vec3_dot(c->vertices[i], dir);
			if (d > max_dot) { max_dot = d; result = c->vertices[i]; }
		}
		break;
	}
	}
	return result;
}

/* ============ Minkowski差支持函数 ============ */

static vec3_t get_support_point(gjk3d_collider_p ca, gjk3d_collider_p cb, vec3_t dir) {
	vec3_t a = find_furthest_point(ca, dir);
	vec3_t b = find_furthest_point(cb, vec3_negate(dir));
	return vec3_sub(a, b);
}

/* ============ GJK核心 ============ */

/* 相对方向判断: 点积显著为正(余弦 > 1e-4)才视为同向,
   避免对称退化位形下符号在噪声中翻转导致分支路径依赖 */
static bool same_direction(vec3_t dir, vec3_t ao) {
	ft_t dot = vec3_dot(dir, ao);
	if (dot <= ft_zero()) return false;
	/* (dot / (|dir||ao|))^2 > eps^2, 避免 sqrt */
	ft_t eps2 = ft_from_float(1e-8f);
	ft_t len2 = ft_mul(vec3_square_length(dir), vec3_square_length(ao));
	return ft_mul(dot, dot) > ft_mul(eps2, len2);
}

/* 安全叉积归一化: 三点近共线(叉积退化)时返回 false */
static bool safe_cross_normalize(vec3_t a, vec3_t b, vec3_t *out) {
	ft_t ab2 = ft_mul(vec3_square_length(a), vec3_square_length(b));
	if (ab2 == ft_zero()) return false;
	vec3_t n = vec3_cross(a, b);
	ft_t len2 = vec3_square_length(n);
	ft_t eps2 = ft_from_float(1e-8f);
	if (len2 <= ft_mul(ab2, eps2)) return false;
	*out = vec3_normalize(n);
	return true;
}

static bool gjk_line(simplex3d_t *s, vec3_t *dir) {
	vec3_t a = s->points[0], b = s->points[1];
	vec3_t ab = vec3_normalize(vec3_sub(b, a));
	vec3_t ao = vec3_normalize(vec3_negate(a));
	if (same_direction(ab, ao)) {
		*dir = vec3_normalize(vec3_cross(vec3_cross(ab, ao), ab));
	} else {
		simplex_init1(s, a);
		*dir = ao;
	}
	return false;
}

static bool gjk_triangle(simplex3d_t *s, vec3_t *dir) {
	vec3_t a = s->points[0], b = s->points[1], c = s->points[2];
	vec3_t ab = vec3_normalize(vec3_sub(b, a));
	vec3_t ac = vec3_normalize(vec3_sub(c, a));
	vec3_t ao = vec3_normalize(vec3_negate(a));
	vec3_t abc;
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

static bool gjk_tetrahedron(simplex3d_t *s, vec3_t *dir) {
	vec3_t a = s->points[0], b = s->points[1], c = s->points[2], d = s->points[3];
	vec3_t ab = vec3_normalize(vec3_sub(b, a));
	vec3_t ac = vec3_normalize(vec3_sub(c, a));
	vec3_t ad = vec3_normalize(vec3_sub(d, a));
	vec3_t ao = vec3_normalize(vec3_negate(a));
	vec3_t abc, acd, adb;
	/* 各面退化(近共线)时跳过该分支, 避免法线为噪声方向 */
	bool has_abc = safe_cross_normalize(ab, ac, &abc);
	bool has_acd = safe_cross_normalize(ac, ad, &acd);
	bool has_adb = safe_cross_normalize(ad, ab, &adb);

	if (has_abc && same_direction(abc, ao)) { simplex_init3(s, a, b, c); return gjk_triangle(s, dir); }
	if (has_acd && same_direction(acd, ao)) { simplex_init3(s, a, c, d); return gjk_triangle(s, dir); }
	if (has_adb && same_direction(adb, ao)) { simplex_init3(s, a, d, b); return gjk_triangle(s, dir); }
	return true;
}

static bool next_simplex(simplex3d_t *s, vec3_t *dir) {
	switch (s->points_num) {
	case 2: return gjk_line(s, dir);
	case 3: return gjk_triangle(s, dir);
	case 4: return gjk_tetrahedron(s, dir);
	}
	return false;
}

/* ============ EPA (扩展多面体算法) ============ */

/* 计算所有面法线和最近面索引 */
static void get_face_normals(kvec_t(vec3_t) *polytope, kvec_t(size_t) *faces,
	kvec_t(epa_face_t) *normals, size_t *min_triangle) {
	*min_triangle = 0;
	ft_t min_distance = ft_max_value();

	for (size_t i = 0; i < faces->n; i += 3) {
		vec3_t a = kv_A(*polytope, kv_A(*faces, i));
		vec3_t b = kv_A(*polytope, kv_A(*faces, i + 1));
		vec3_t c = kv_A(*polytope, kv_A(*faces, i + 2));

		vec3_t ba = vec3_normalize(vec3_sub(b, a));
		vec3_t ca = vec3_normalize(vec3_sub(c, a));
		vec3_t n;
		if (!safe_cross_normalize(ba, ca, &n)) {
			/* 退化面(三点近共线): 法线退化为噪声方向, 跳过该面,
			   将其距离置为最大避免被选为最近面 */
			epa_face_t face;
			face.normal = (vec3_t){ ft_one(), ft_zero(), ft_zero() };
			face.distance = ft_max_value();
			kv_push(epa_face_t, *normals, face);
			continue;
		}

		ft_t dist = vec3_dot(n, a);
		if (dist < ft_zero()) { n = vec3_negate(n); dist = -dist; }

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
	kvec_t(vec3_t) polytope;
	kvec_t(size_t) faces;
	kvec_t(epa_face_t) normals;

	kv_init(polytope);
	kv_init(faces);
	kv_init(normals);

	/* 将单纯形点复制到多面体 */
	for (int i = 0; i < simplex->points_num; i++) {
		kv_push(vec3_t, polytope, simplex->points[i]);
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
	vec3_t bake_min_normal = kv_A(normals, min_face).normal;
	ft_t bake_min_distance = kv_A(normals, min_face).distance;

	vec3_t min_normal = { ft_zero(), ft_zero(), ft_zero() };
	ft_t min_distance = ft_max_value();

	int epa_iter = 0;
	while (min_distance == ft_max_value() && epa_iter < 64) {
		epa_iter++;
		min_normal = kv_A(normals, min_face).normal;
		min_distance = kv_A(normals, min_face).distance;

		vec3_t support = get_support_point(ca, cb, min_normal);
		ft_t s_dist = vec3_dot(min_normal, support);

		if (ft_abs(ft_sub(s_dist, min_distance)) > 0.001f) {
			min_distance = ft_max_value();

			/* 找到需要移除的面,收集边界边 */
			kvec_t(epa_edge_t) unique_edges;
			kv_init(unique_edges);

			for (size_t i = 0; i < normals.n; i++) {
				vec3_t n = kv_A(normals, i).normal;
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
			kvec_t(size_t) new_faces;
			kv_init(new_faces);
			size_t new_vertex_index = polytope.n;
			for (size_t i = 0; i < unique_edges.n; i++) {
				kv_push(size_t, new_faces, kv_A(unique_edges, i).a);
				kv_push(size_t, new_faces, kv_A(unique_edges, i).b);
				kv_push(size_t, new_faces, new_vertex_index);
			}
			kv_push(vec3_t, polytope, support);

			/* 计算新面的法线 */
			kvec_t(epa_face_t) new_normals;
			kv_init(new_normals);
			size_t new_min_face = 0;
			get_face_normals(&polytope, &new_faces, &new_normals, &new_min_face);

			/* 找到全局最小距离的面 */
			ft_t old_min_dist = ft_max_value();
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

	/* EPA后处理: 用一组候选方向重新验证穿透深度, 取最小值,
	   纠正EPA因退化/局部最优收敛到错误支撑面的情况 */
	{
		vec3_t candidates[8];
		int n = 0;
		vec3_t axis[3] = {
			{ ft_one(), ft_zero(), ft_zero() },
			{ ft_zero(), ft_one(), ft_zero() },
			{ ft_zero(), ft_zero(), ft_one() }
		};
		for (int i = 0; i < 3; i++) {
			candidates[n++] = axis[i];
			candidates[n++] = vec3_negate(axis[i]);
		}
		/* 两物体中心连线方向 */
		vec3_t delta = vec3_sub(cb->position, ca->position);
		ft_t dl = vec3_length(delta);
		if (dl > ft_zero()) {
			candidates[n++] = vec3_scale(delta, ft_div(ft_one(), dl));
			candidates[n++] = vec3_scale(vec3_negate(delta), ft_div(ft_one(), dl));
		}

		for (int i = 0; i < n; i++) {
			vec3_t support = get_support_point(ca, cb, candidates[i]);
			ft_t d = vec3_dot(candidates[i], support);
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
static bool degenerate_fallback(gjk3d_collider_p ca, gjk3d_collider_p cb, gjk3d_contact_p contact) {
	vec3_t axes[6] = {
		{  ft_one(), ft_zero(), ft_zero() },
		{ -ft_one(), ft_zero(), ft_zero() },
		{  ft_zero(),  ft_one(), ft_zero() },
		{  ft_zero(), -ft_one(), ft_zero() },
		{  ft_zero(), ft_zero(),  ft_one() },
		{  ft_zero(), ft_zero(), -ft_one() }
	};
	ft_t min_d = ft_max_value();
	vec3_t min_n = { ft_zero(), ft_zero(), ft_zero() };
	for (int i = 0; i < 6; i++) {
		vec3_t s = get_support_point(ca, cb, axes[i]);
		ft_t d = vec3_dot(axes[i], s);
		if (d <= ft_zero()) return false; /* 该方向分离 */
		if (d < min_d) { min_d = d; min_n = axes[i]; }
	}
	contact->normal = min_n;
	contact->depth = ft_add(min_d, 0.001f);
	return true;
}

/* ============ 公开API实现 ============ */
bool gjk3d_collide(gjk3d_collider_p c1, gjk3d_collider_p c2,
	vec3_t init_dir, gjk3d_contact_p contact)
{
	simplex3d_t simplex;
	memset(&simplex, 0, sizeof(simplex));

	vec3_t support = get_support_point(c1, c2, init_dir);
	simplex_push(&simplex, support);
	vec3_t dir = vec3_normalize(vec3_negate(support));

	ft_t tiny2 = ft_from_float(1e-8f);
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

void gjk3d_init_sphere(gjk3d_collider_t *c, vec3_t pos, ft_t radius) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_SPHERE;
	c->position = pos;
	c->radius = radius;
}

void gjk3d_init_box(gjk3d_collider_t *c, vec3_t pos, vec3_t half_size) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_BOX;
	c->position = pos;
	c->half_size = half_size;
}

void gjk3d_init_capsule(gjk3d_collider_t *c, vec3_t pos, ft_t radius, ft_t height, vec3_t axis) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_CAPSULE;
	c->position = pos;
	c->radius = radius;
	c->height = height;
	c->axis = vec3_normalize(axis);
}

void gjk3d_init_cylinder(gjk3d_collider_t *c, vec3_t pos, ft_t radius, ft_t height) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_CYLINDER;
	c->position = pos;
	c->radius = radius;
	c->height = height;
}

void gjk3d_init_cone(gjk3d_collider_t *c, vec3_t pos, ft_t radius, ft_t height) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_CONE;
	c->position = pos;
	c->radius = radius;
	c->height = height;
}

void gjk3d_init_mesh(gjk3d_collider_t *c, vec3_t *vertices, int num) {
	memset(c, 0, sizeof(*c));
	c->type = GJK3D_CTYPE_MESH;
	c->vertices = vertices;
	c->vertices_num = num;
}
