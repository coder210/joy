#include "jbox2d.h"
#include "external/klib/khash.h"
#include "external/klib/klist.h"
#include "external/klib/kvec.h"


// --- klib container types ---

#define kl_no_free(x)
#define fp_negate(x)    (-(x))
#define fp_equal(a,b)   ((a)==(b))
#define fp_less(a,b)    ((a)<(b))
#define fp_greater(a,b) ((a)>(b))
#define fp_lequal(a,b)  ((a)<=(b))
#define fp_gequal(a,b)  ((a)>=(b))
KHASH_INIT(bodies_map, int, box2d_body_t*, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(arbiters_map, int64_t, box2d_arbiter_t*, 1, kh_int64_hash_func, kh_int64_hash_equal)
KLIST_INIT(joints_list, box2d_joint_t*, kl_no_free)


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


typedef enum box2d_axis_e {
	FACE_A_X,
	FACE_A_Y,
	FACE_B_X,
	FACE_B_Y
} box2d_axis_e;

typedef enum box2d_edge_e {
	NO_EDGE = 0,
	EDGE1,
	EDGE2,
	EDGE3,
	EDGE4
} box2d_edge_e;


typedef union box2d_feature_t {
	struct box2d_edges_t {
		char in_edge1;
		char out_edge1;
		char in_edge2;
		char out_edge2;
	} e;
	int value;
} box2d_feature_t;


typedef struct box2d_contact_t {
	vec2f_t position;
	vec2f_t normal;
	fp_t separation;
	box2d_feature_t feature;
} box2d_contact_t;


typedef struct box2d_clip_vertex_t {
	vec2f_t v;
	box2d_feature_t fp;
} box2d_clip_vertex_t;


static inline void
_flip(box2d_feature_t* fp) {
	char tmp = fp->e.in_edge1;
	fp->e.in_edge1 = fp->e.in_edge2;
	fp->e.in_edge2 = tmp;
	tmp = fp->e.out_edge1;
	fp->e.out_edge1 = fp->e.out_edge2;
	fp->e.out_edge2 = tmp;
}

static int
_clip_segment_to_line(box2d_clip_vertex_t vout[2], box2d_clip_vertex_t vin[2],
	const vec2f_t* normal, fp_t offset, char clip_edge) {
	int num_out = 0;
	vec2f_t tmp;

	fp_t distance0 = fp_sub(vec2f_dot(*normal, vin[0].v), offset);
	fp_t distance1 = fp_sub(vec2f_dot(*normal, vin[1].v), offset);

	if (fp_lequal(distance0, fp_zero())) vout[num_out++] = vin[0];
	if (fp_lequal(distance1, fp_zero())) vout[num_out++] = vin[1];

	if (fp_less(fp_mul(distance0, distance1), fp_zero())) {
		fp_t interp = fp_div(distance0, fp_sub(distance0, distance1));
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
_compute_incident_edge(box2d_clip_vertex_t c[2], const vec2f_t* h, const vec2f_t* pos,
	const mat22f_t* rot, const vec2f_t* normal) {
	vec2f_t tmp;
	mat22f_t rotT = mat22f_transpose(*rot);
	tmp = mat22f_mul_vec2f(rotT, *normal);
	vec2f_t n = vec2f_negate(tmp);
	vec2f_t abs_n = vec2f_abs(n);

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
_collide(box2d_contact_t* contacts, box2d_body_t* a_body, box2d_body_t* b_body) {
	vec2f_t tmp;

	fp_t half = fp_half();
	vec2f_t a_h = vec2f_scale(a_body->bounds, half);
	vec2f_t b_h = vec2f_scale(b_body->bounds, half);

	vec2f_t a_pos = a_body->position;
	vec2f_t b_pos = b_body->position;

	mat22f_t a_rot = mat22f_rotate(a_body->rotation);
	mat22f_t b_rot = mat22f_rotate(b_body->rotation);

	mat22f_t a_rotT = mat22f_transpose(a_rot);
	mat22f_t b_rotT = mat22f_transpose(b_rot);

	vec2f_t dp = vec2f_sub(b_pos, a_pos);

	vec2f_t a_d = mat22f_mul_vec2f(a_rotT, dp);
	vec2f_t b_d = mat22f_mul_vec2f(b_rotT, dp);

	mat22f_t c = mat22f_mul(a_rotT, b_rot);
	mat22f_t abs_c = mat22f_abs(c);
	mat22f_t abs_ct = mat22f_transpose(abs_c);

	// Box A faces
	vec2f_t a_abs_d = vec2f_abs(a_d);
	tmp = vec2f_sub(a_abs_d, a_h);
	vec2f_t tmp2 = mat22f_mul_vec2f(abs_c, b_h);
	vec2f_t a_face = vec2f_sub(tmp, tmp2);
	if (fp_greater(a_face.x, fp_zero()) || fp_greater(a_face.y, fp_zero()))
		return 0;

	// Box B faces
	vec2f_t abs_db = vec2f_abs(b_d);
	tmp = mat22f_mul_vec2f(abs_ct, a_h);
	tmp = vec2f_sub(abs_db, tmp);
	vec2f_t b_face = vec2f_sub(tmp, b_h);
	if (fp_greater(b_face.x, fp_zero()) || fp_greater(b_face.y, fp_zero()))
		return 0;

	// Find best axis
	box2d_axis_e axis = FACE_A_X;
	fp_t separation = a_face.x;

	vec2f_t normal = fp_greater(a_d.x, fp_zero()) ? a_rot.col1 : vec2f_negate(a_rot.col1);

	const fp_t relative_tol = fp_from_float(0.95f);
	const fp_t absolute_tol = fp_from_float(0.01f);

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
	vec2f_t front_normal, side_normal;
	box2d_clip_vertex_t incident_edge[2] = { { 0 } };
	fp_t front, neg_side, pos_side;
	char neg_edge, pos_edge;

	switch (axis) {
	case FACE_A_X: {
		front_normal = normal;
		front = fp_add(vec2f_dot(a_pos, front_normal), a_h.x);
		side_normal = a_rot.col2;
		fp_t side = vec2f_dot(a_pos, side_normal);
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
		fp_t side = vec2f_dot(a_pos, side_normal);
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
		fp_t side = vec2f_dot(b_pos, side_normal);
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
		fp_t side = vec2f_dot(b_pos, side_normal);
		neg_side = fp_add(fp_negate(side), b_h.x);
		pos_side = fp_add(side, b_h.x);
		neg_edge = EDGE2;
		pos_edge = EDGE4;
		_compute_incident_edge(incident_edge, &a_h, &a_pos, &a_rot, &front_normal);
	}
				 break;
	}

	box2d_clip_vertex_t clip_points1[2] = { { 0 } };
	box2d_clip_vertex_t clip_points2[2] = { { 0 } };
	int np;

	vec2f_t neg_side_normal = vec2f_negate(side_normal);
	np = _clip_segment_to_line(clip_points1, incident_edge, &neg_side_normal, neg_side, neg_edge);

	if (np < 2) return 0;

	np = _clip_segment_to_line(clip_points2, clip_points1, &side_normal, pos_side, pos_edge);

	if (np < 2) return 0;

	int num_contacts = 0;
	for (int i = 0; i < 2; ++i) {
		fp_t sep = fp_sub(vec2f_dot(front_normal, clip_points2[i].v), front);
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
static inline vec2f_t
v3(fp_t s, vec2f_t a) {
	return vec2f_cross3(a, s);
}

static void
_prestep(box2d_joint_t* joint, box2d_body_t* body1, box2d_body_t* body2, fp_t inv_dt) {
	mat22f_t Rot1 = mat22f_rotate(body1->rotation);
	mat22f_t Rot2 = mat22f_rotate(body2->rotation);

	joint->r1 = mat22f_mul_vec2f(Rot1, joint->local_anchor1);
	joint->r2 = mat22f_mul_vec2f(Rot2, joint->local_anchor2);

	mat22f_t K1;
	K1.col1.x = fp_add(body1->inv_mass, body2->inv_mass);
	K1.col2.x = fp_zero();
	K1.col1.y = fp_zero();
	K1.col2.y = K1.col1.x;

	mat22f_t K2;
	fp_t neg_invI1 = fp_negate(body1->invI);
	K2.col1.x = fp_mul(body1->invI, fp_pow2(joint->r1.y));
	K2.col2.x = fp_mul(neg_invI1, fp_mul(joint->r1.x, joint->r1.y));
	K2.col1.y = fp_mul(neg_invI1, fp_mul(joint->r1.x, joint->r1.y));
	K2.col2.y = fp_mul(body1->invI, fp_pow2(joint->r1.x));

	mat22f_t K3;
	fp_t neg_invI2 = fp_negate(body2->invI);
	K3.col1.x = fp_mul(fp_mul(body2->invI, joint->r2.y), joint->r2.y);
	K3.col2.x = fp_mul(fp_mul(neg_invI2, joint->r2.x), joint->r2.y);
	K3.col1.y = fp_mul(fp_mul(neg_invI2, joint->r2.x), joint->r2.y);
	K3.col2.y = fp_mul(fp_mul(body2->invI, joint->r2.x), joint->r2.x);

	mat22f_t K = mat22f_add(K1, K2);
	K = mat22f_add(K, K3);
	K.col1.x = fp_add(K.col1.x, joint->softness);
	K.col2.y = fp_add(K.col2.y, joint->softness);

	joint->M = mat22f_invert(K);

	vec2f_t p1 = vec2f_add(body1->position, joint->r1);
	vec2f_t p2 = vec2f_add(body2->position, joint->r2);
	vec2f_t dp = vec2f_sub(p2, p1);

	if (POSITION_CORRECTION) {
		fp_t bias_factor = fp_mul(fp_negate(joint->biasfactor), inv_dt);
		joint->bias = vec2f_scale(dp, bias_factor);
	}
	else {
		joint->bias.x = fp_zero();
		joint->bias.y = fp_zero();
	}

	if (WARM_STARTING) {
		vec2f_t tmp2;
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
_joint_apply_impulse(box2d_joint_t* joint, box2d_body_t* body1, box2d_body_t* body2) {
	vec2f_t tmp2;
	tmp2 = v3(body2->angular_velocity, joint->r2);
	tmp2 = vec2f_add(body2->velocity, tmp2);
	tmp2 = vec2f_sub(tmp2, body1->velocity);
	vec2f_t r1a = v3(body1->angular_velocity, joint->r1);
	vec2f_t dv = vec2f_sub(tmp2, r1a);
	tmp2 = vec2f_sub(joint->bias, dv);
	vec2f_t ps = vec2f_scale(joint->p, joint->softness);
	vec2f_t force = vec2f_sub(tmp2, ps);
	vec2f_t impulse = mat22f_mul_vec2f(joint->M, force);

	tmp2 = vec2f_scale(impulse, body1->inv_mass);
	body1->velocity = vec2f_sub(body1->velocity, tmp2);
	body1->angular_velocity = fp_sub(body1->angular_velocity, fp_mul(body1->invI, vec2f_cross(joint->r1, impulse)));

	tmp2 = vec2f_scale(impulse, body2->inv_mass);
	body2->velocity = vec2f_add(body2->velocity, tmp2);
	body2->angular_velocity = fp_add(body2->angular_velocity, fp_mul(body2->invI, vec2f_cross(joint->r2, impulse)));

	joint->p = vec2f_add(joint->p, impulse);
}

static box2d_arbiter_t*
_arbiter_create(box2d_arbiter_key_t key, fp_t friction, box2d_manifold_t* manifolds, int num) {
	box2d_arbiter_t* arbiter = (box2d_arbiter_t*)malloc(sizeof(box2d_arbiter_t));
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
_arbiter_destroy(box2d_arbiter_t* arbiter) {
	free(arbiter);
}

static void
_arbiter_update(box2d_arbiter_t* arbiter, box2d_manifold_t* manifolds, int num) {
	for (int i = 0; i < num; ++i) {
		box2d_manifold_t* c_new = manifolds + i;
		int k = -1;
		for (int j = 0; j < arbiter->num_manifolds; ++j) {
			box2d_manifold_t* c_old = arbiter->manifolds + j;
			if (c_new->feature == c_old->feature) {
				k = j;
				break;
			}
		}

		if (k > -1) {
			box2d_manifold_t* c = manifolds + i;
			box2d_manifold_t* c_old = arbiter->manifolds + k;
			if (WARM_STARTING) {
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
_arbiter_prestep(box2d_arbiter_t* arbiter, box2d_body_t* b1, box2d_body_t* b2, fp_t inv_dt) {
	const fp_t k_allowed_penetration = fp_from_float(0.01f);
	fp_t k_biasfactor = POSITION_CORRECTION ? fp_from_float(0.2f) : fp_zero();
	vec2f_t tmp;

	for (int i = 0; i < arbiter->num_manifolds; ++i) {
		box2d_manifold_t* m = arbiter->manifolds + i;

		vec2f_t r1 = vec2f_sub(m->position, b1->position);
		vec2f_t r2 = vec2f_sub(m->position, b2->position);

		fp_t rn1 = vec2f_dot(r1, m->normal);
		fp_t rn2 = vec2f_dot(r2, m->normal);
		fp_t knormal = fp_add(b1->inv_mass, b2->inv_mass);
		knormal = fp_add(knormal, fp_add(fp_mul(b1->invI, fp_sub(vec2f_dot(r1, r1), fp_pow2(rn1))), fp_mul(b2->invI, fp_sub(vec2f_dot(r2, r2), fp_pow2(rn2)))));
		m->mass_normal = fp_div(fp_one(), knormal);

		vec2f_t tangent = vec2f_cross2(m->normal, fp_one());
		fp_t rt1 = vec2f_dot(r1, tangent);
		fp_t rt2 = vec2f_dot(r2, tangent);
		fp_t k_tangent = fp_add(b1->inv_mass, b2->inv_mass);
		k_tangent = fp_add(k_tangent, fp_mul(b1->invI, fp_sub(vec2f_dot(r1, r1), fp_pow2(rt1))));
		k_tangent = fp_add(k_tangent, fp_mul(b2->invI, fp_sub(vec2f_dot(r2, r2), fp_pow2(rt2))));
		m->mass_tangent = fp_div(fp_one(), k_tangent);

		fp_t corr_sep = fp_min(fp_zero(), fp_add(m->separation, k_allowed_penetration));
		m->bias = fp_mul(fp_mul(fp_negate(k_biasfactor), inv_dt), corr_sep);

		if (ACCUMULATE_IMPULSES) {
			vec2f_t n_pn = vec2f_scale(m->normal, m->pn);
			vec2f_t t_pt = vec2f_scale(tangent, m->pt);

			vec2f_t P = vec2f_add(n_pn, t_pt);

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
_arbiter_apply_impulse(box2d_arbiter_t* arbiter, box2d_body_t* b1, box2d_body_t* b2) {
	vec2f_t tmp;
	for (int i = 0; i < arbiter->num_manifolds; ++i) {
		box2d_manifold_t* m = arbiter->manifolds + i;
		m->r1 = vec2f_sub(m->position, b1->position);
		m->r2 = vec2f_sub(m->position, b2->position);

		vec2f_t r1a = v3(b1->angular_velocity, m->r1);
		vec2f_t r2a = v3(b2->angular_velocity, m->r2);
		tmp = vec2f_add(b2->velocity, r2a);
		tmp = vec2f_sub(tmp, b1->velocity);
		vec2f_t dv = vec2f_sub(tmp, r1a);

		fp_t vn = fp_negate(vec2f_dot(dv, m->normal));
		fp_t dpn = fp_mul(m->mass_normal, fp_add(vn, m->bias));

		if (ACCUMULATE_IMPULSES) {
			fp_t pn0 = m->pn;
			m->pn = fp_max(fp_add(pn0, dpn), fp_zero());
			dpn = fp_sub(m->pn, pn0);
		}
		else {
			dpn = fp_max(dpn, fp_zero());
		}

		vec2f_t Pn = vec2f_scale(m->normal, dpn);

		vec2f_t b1Pn = vec2f_scale(Pn, b1->inv_mass);
		b1->velocity = vec2f_sub(b1->velocity, b1Pn);
		b1->angular_velocity = fp_sub(b1->angular_velocity, fp_mul(b1->invI, vec2f_cross(m->r1, Pn)));

		vec2f_t b2Pn = vec2f_scale(Pn, b2->inv_mass);
		b2->velocity = vec2f_add(b2->velocity, b2Pn);
		b2->angular_velocity = fp_add(b2->angular_velocity, fp_mul(b2->invI, vec2f_cross(m->r2, Pn)));

		r1a = v3(b1->angular_velocity, m->r1);
		r2a = v3(b2->angular_velocity, m->r2);

		tmp = vec2f_add(b2->velocity, r2a);
		tmp = vec2f_sub(tmp, b1->velocity);
		dv = vec2f_sub(tmp, r1a);

		vec2f_t tangent = vec2f_cross2(m->normal, fp_one());
		fp_t vt = fp_negate(vec2f_dot(dv, tangent));
		fp_t dpt = fp_mul(m->mass_tangent, vt);

		if (ACCUMULATE_IMPULSES) {
			fp_t max_pt = fp_mul(arbiter->friction, m->pn);
			fp_t old_ti = m->pt;
			m->pt = fp_clamp(fp_add(old_ti, dpt), fp_negate(max_pt), max_pt);
			dpt = fp_sub(m->pt, old_ti);
		}
		else {
			fp_t max_pt = fp_mul(arbiter->friction, dpn);
			dpt = fp_clamp(dpt, fp_negate(max_pt), max_pt);
		}

		vec2f_t pt = vec2f_scale(tangent, dpt);

		vec2f_t b1_pt = vec2f_scale(pt, b1->inv_mass);
		b1->velocity = vec2f_sub(b1->velocity, b1_pt);
		b1->angular_velocity = fp_sub(b1->angular_velocity, fp_mul(b1->invI, vec2f_cross(m->r1, pt)));

		vec2f_t b2_pt = vec2f_scale(pt, b2->inv_mass);
		b2->velocity = vec2f_add(b2->velocity, b2_pt);
		b2->angular_velocity = fp_add(b2->angular_velocity, fp_mul(b2->invI, vec2f_cross(m->r2, pt)));
	}
}


typedef struct box2d_world_t {
	vec2f_t gravity;
	int iterations;
	int id;
	khash_t(bodies_map)* bodies;
	klist_t(joints_list)* joints;
	khash_t(arbiters_map)* arbiters;
} box2d_world_t;


static void
_integrate_forces(box2d_world_t* world, fp_t dt) {
	vec2f_t acceleration;
	khint_t k;
	for (k = kh_begin(world->bodies); k != kh_end(world->bodies); ++k) {
		if (!kh_exist(world->bodies, k)) continue;
		box2d_body_t* body = kh_val(world->bodies, k);
		if (body && fp_greater(body->inv_mass, fp_zero())) {
			acceleration = vec2f_scale(body->force, body->inv_mass);
			acceleration = vec2f_add(world->gravity, acceleration);
			vec2f_t dv = vec2f_scale(acceleration, dt);
			body->velocity = vec2f_add(body->velocity, dv);
			body->angular_velocity = fp_add(body->angular_velocity, fp_mul(fp_mul(dt, body->invI), body->torque));
		}
	}
}

static void
_integrate_velocities(box2d_world_t* world, fp_t dt) {
	vec2f_t position;
	khint_t k;
	for (k = kh_begin(world->bodies); k != kh_end(world->bodies); ++k) {
		if (!kh_exist(world->bodies, k)) continue;
		box2d_body_t* body = kh_val(world->bodies, k);
		if (body && fp_gequal(body->inv_mass, fp_zero())) {
			position = vec2f_scale(body->velocity, dt);
			body->position = vec2f_add(body->position, position);
			body->rotation = fp_add(body->rotation, fp_mul(body->angular_velocity, dt));

			body->force = (vec2f_t){ fp_zero(), fp_zero() };
			body->torque = fp_zero();
		}
	}
}

// O(n^2) broad-phase
static void
_broad_phase(box2d_world_t* world) {
	(void)world;
}

static void
_narrow_phase(box2d_world_t* world) {
	box2d_contact_t contacts[2];
	box2d_manifold_t ms[2];

	// Collect body pointers into a vector for ordered iteration
	kvec_t(box2d_body_t*) body_vec;
	kv_init(body_vec);
	{
		khint_t k;
		for (k = kh_begin(world->bodies); k != kh_end(world->bodies); ++k) {
			if (kh_exist(world->bodies, k))
				kv_push(box2d_body_t*, body_vec, kh_val(world->bodies, k));
		}
	}

	int nb = kv_size(body_vec);
	for (int i = 0; i < nb; ++i) {
		box2d_body_t* bi = kv_A(body_vec, i);
		for (int j = i + 1; j < nb; ++j) {
			box2d_body_t* bj = kv_A(body_vec, j);
			if (fp_equal(bi->inv_mass, fp_zero()) && fp_equal(bj->inv_mass, fp_zero())) {
				continue;
			}
			int num_contacts = _collide(contacts, bi, bj);
			box2d_arbiter_key_t key;
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
					ms[ci].r1 = (vec2f_t){ fp_zero(), fp_zero() };
					ms[ci].r2 = (vec2f_t){ fp_zero(), fp_zero() };
					ms[ci].mass_normal = ms[ci].mass_tangent = fp_zero();
					ms[ci].bias = fp_zero();
				}

				khint_t kk = kh_get(arbiters_map, world->arbiters, key.value);
				box2d_arbiter_t* arbiter = (kk != kh_end(world->arbiters)) ? kh_val(world->arbiters, kk) : NULL;
				if (arbiter == NULL) {
					fp_t friction = fp_sqrt(fp_mul(bi->friction, bj->friction));
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
					box2d_arbiter_t* arbiter = kh_val(world->arbiters, kk);
					kh_del(arbiters_map, world->arbiters, kk);
					_arbiter_destroy(arbiter);
				}
			}
		}
	}
	kv_destroy(body_vec);
}

static void
_presolve(box2d_world_t* world, fp_t inv_dt) {
	khint_t k;
	for (k = kh_begin(world->arbiters); k != kh_end(world->arbiters); ++k) {
		if (!kh_exist(world->arbiters, k)) continue;
		box2d_arbiter_t* arbiter = kh_val(world->arbiters, k);
		box2d_body_t* b1 = NULL, * b2 = NULL;
		khint_t kk;

		kk = kh_get(bodies_map, world->bodies, arbiter->id1);
		if (kk != kh_end(world->bodies)) b1 = kh_val(world->bodies, kk);
		kk = kh_get(bodies_map, world->bodies, arbiter->id2);
		if (kk != kh_end(world->bodies)) b2 = kh_val(world->bodies, kk);

		_arbiter_prestep(arbiter, b1, b2, inv_dt);
	}

	kliter_t(joints_list)* joint_iter = kl_begin(world->joints);
	while (joint_iter != kl_end(world->joints)) {
		box2d_joint_t* joint = kl_val(joint_iter);
		box2d_body_t* b1 = NULL, * b2 = NULL;
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
_solve(box2d_world_t* world) {
	for (int i = 0; i < world->iterations; ++i) {
		khint_t k;
		for (k = kh_begin(world->arbiters); k != kh_end(world->arbiters); ++k) {
			if (!kh_exist(world->arbiters, k)) continue;
			box2d_arbiter_t* arbiter = kh_val(world->arbiters, k);
			box2d_body_t* b1 = NULL, * b2 = NULL;
			khint_t kk;

			kk = kh_get(bodies_map, world->bodies, arbiter->id1);
			if (kk != kh_end(world->bodies)) b1 = kh_val(world->bodies, kk);
			kk = kh_get(bodies_map, world->bodies, arbiter->id2);
			if (kk != kh_end(world->bodies)) b2 = kh_val(world->bodies, kk);

			_arbiter_apply_impulse(arbiter, b1, b2);
		}

		kliter_t(joints_list)* joint_iter = kl_begin(world->joints);
		while (joint_iter != kl_end(world->joints)) {
			box2d_joint_t* joint = kl_val(joint_iter);
			box2d_body_t* b1 = NULL, * b2 = NULL;
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


box2d_world_t*
box2d_create_world(vec2f_t gravity, int iterations) {
	box2d_world_t* world = (box2d_world_t*)malloc(sizeof(box2d_world_t));
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
box2d_clear_world(box2d_world_t* world) {
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
box2d_destroy_world(box2d_world_t* world) {
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
box2d_step(box2d_world_t* world, fp_t dt) {
	fp_t inv_dt = fp_greater(dt, fp_zero()) ? fp_div(fp_one(), dt) : fp_zero();

	_broad_phase(world);
	_narrow_phase(world);
	_integrate_forces(world, dt);
	_presolve(world, inv_dt);
	_solve(world);
	_integrate_velocities(world, dt);
}

box2d_body_t*
box2d_create_body(box2d_world_t* world, fp_t mass, fp_t bx, fp_t by) {
	box2d_body_t* body = (box2d_body_t*)malloc(sizeof(box2d_body_t));
	assert(body != NULL);

	body->id = ++world->id;
	body->position = (vec2f_t){ fp_zero(), fp_zero() };
	body->rotation = fp_zero();
	body->velocity = (vec2f_t){ fp_zero(), fp_zero() };
	body->angular_velocity = fp_zero();
	body->force = (vec2f_t){ fp_zero(), fp_zero() };
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
box2d_destroy_body(box2d_world_t* world, int id) {
	khint_t k = kh_get(bodies_map, world->bodies, id);
	if (k != kh_end(world->bodies)) {
		free(kh_val(world->bodies, k));
		kh_del(bodies_map, world->bodies, k);
	}
}

box2d_joint_t*
box2d_create_joint(box2d_world_t* world, int id1, int id2, vec2f_t anchor) {
	khint_t kk;

	kk = kh_get(bodies_map, world->bodies, id1);
	box2d_body_t* b1 = (kk != kh_end(world->bodies)) ? kh_val(world->bodies, kk) : NULL;
	kk = kh_get(bodies_map, world->bodies, id2);
	box2d_body_t* b2 = (kk != kh_end(world->bodies)) ? kh_val(world->bodies, kk) : NULL;

	if (b1 == NULL || b2 == NULL) {
		return NULL;
	}

	box2d_joint_t* joint = (box2d_joint_t*)malloc(sizeof(box2d_joint_t));
	assert(joint != NULL);

	joint->id1 = b1->id;
	joint->id2 = b2->id;

	mat22f_t Rot1 = mat22f_rotate(b1->rotation);
	mat22f_t Rot2 = mat22f_rotate(b2->rotation);
	mat22f_t Rot1T = mat22f_transpose(Rot1);
	mat22f_t Rot2T = mat22f_transpose(Rot2);

	vec2f_t distance1 = vec2f_sub(anchor, b1->position);
	vec2f_t distance2 = vec2f_sub(anchor, b2->position);

	joint->local_anchor1 = mat22f_mul_vec2f(Rot1T, distance1);
	joint->local_anchor2 = mat22f_mul_vec2f(Rot2T, distance2);

	joint->p = (vec2f_t){ fp_zero(), fp_zero() };

	joint->softness = fp_zero();
	joint->biasfactor = fp_from_float(0.2f);

	*kl_pushp(joints_list, world->joints) = joint;
	return joint;
}

void
box2d_destroy_joint(box2d_world_t* world, box2d_joint_t* joint) {
	klist_t(joints_list)* new_list = kl_init(joints_list);
	kliter_t(joints_list)* iter = kl_begin(world->joints);
	while (iter != kl_end(world->joints)) {
		box2d_joint_t* j = kl_val(iter);
		if (j != joint)
			*kl_pushp(joints_list, new_list) = j;
		iter = kl_next(iter);
	}
	kl_destroy(joints_list, world->joints);
	world->joints = new_list;
	free(joint);
}

void
box2d_foreach_bodies(box2d_world_t* world, void (*iterator)(box2d_body_t*)) {
	khint_t k;
	for (k = kh_begin(world->bodies); k != kh_end(world->bodies); ++k) {
		if (!kh_exist(world->bodies, k)) continue;
		iterator(kh_val(world->bodies, k));
	}
}

void
box2d_foreach_joints(box2d_world_t* world, void (*iterator)(box2d_joint_t*, box2d_body_t*, box2d_body_t*)) {
	kliter_t(joints_list)* joint_iter = kl_begin(world->joints);
	while (joint_iter != kl_end(world->joints)) {
		box2d_joint_t* joint = kl_val(joint_iter);
		box2d_body_t* b1 = NULL, * b2 = NULL;
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
box2d_foreach_arbiters(box2d_world_t* world, void (*iterator)(box2d_arbiter_t*)) {
	khint_t k;
	for (k = kh_begin(world->arbiters); k != kh_end(world->arbiters); ++k) {
		if (!kh_exist(world->arbiters, k)) continue;
		iterator(kh_val(world->arbiters, k));
	}
}
