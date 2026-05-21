#ifndef BOX2D_H
#define BOX2D_H

#include <stdint.h>
#include <stdbool.h>
#include "jmath.h"


#define ACCUMULATE_IMPULSES 1
#define WARM_STARTING 1
#define POSITION_CORRECTION 1
#define MAX_POINTS 2


typedef struct box2d_body_t {
	int id;
	vec2f_t position;
	fp_t rotation;
	vec2f_t velocity;
	fp_t angular_velocity;
	vec2f_t force;
	fp_t torque;
	vec2f_t bounds;
	fp_t friction;
	fp_t mass, inv_mass;
	fp_t I, invI;
} box2d_body_t;


typedef struct box2d_joint_t {
	mat22f_t M;
	vec2f_t local_anchor1, local_anchor2;
	vec2f_t r1, r2;
	vec2f_t bias;
	vec2f_t p;		// accumulated impulse
	int id1, id2;
	fp_t biasfactor;
	fp_t softness;
} box2d_joint_t;


typedef struct box2d_manifold_t {
	vec2f_t position;
	vec2f_t normal;
	fp_t separation;
	int feature;
	fp_t pn;	// accumulated normal impulse
	fp_t pt;	// accumulated tangent impulse
	vec2f_t r1, r2;
	fp_t mass_normal, mass_tangent;
	fp_t bias;
} box2d_manifold_t;


typedef union box2d_arbiter_key_t {
	struct id_pair {
		int id1, id2;
	} id_pair;
	int64_t value;
} box2d_arbiter_key_t;


typedef struct box2d_arbiter_t {
	box2d_manifold_t manifolds[MAX_POINTS];
	int num_manifolds, id1, id2;
	fp_t friction; // combined friction
} box2d_arbiter_t;


typedef struct box2d_world_t box2d_world_t;


box2d_world_t* box2d_create_world(vec2f_t gravity, int iterations);
void box2d_clear_world(box2d_world_t* world);
void box2d_destroy_world(box2d_world_t* world);
void box2d_step(box2d_world_t* world, fp_t dt);
box2d_body_t* box2d_create_body(box2d_world_t* world, fp_t mass, fp_t bx, fp_t by);
void box2d_destroy_body(box2d_world_t* world, int id);
box2d_joint_t* box2d_create_joint(box2d_world_t* world, int id1, int id2, vec2f_t anchor);
void box2d_destroy_joint(box2d_world_t* world, box2d_joint_t* joint);
void box2d_foreach_bodies(box2d_world_t* world, void (*iterator)(box2d_body_t*));
void box2d_foreach_joints(box2d_world_t* world, void (*iterator)(box2d_joint_t*, box2d_body_t*, box2d_body_t*));
void box2d_foreach_arbiters(box2d_world_t* world, void (*iterator)(box2d_arbiter_t*));


#endif
