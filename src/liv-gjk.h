/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-gjk.h
Description: gjkµÄ2d,3dµÄÅö×²¼ì²â
Author: ydlc
Version: 1.0
Date: 2024.12.30
History:
*************************************************/
#ifndef LIV_GJK_H
#define LIV_GJK_H
#include "liv-math.h"

typedef struct gjk2d_collider {
	int type;
	fp_t radius;
	vec3f_t position;
	vec3f_t *vertices;
	int vertices_num;
}gjk2d_collider_t, *gjk2d_collider_p;

typedef struct gjk2d_contact {
	vec2f_t normal;
	fp_t depth;
}gjk2d_contact_t, *gjk2d_contact_p;


typedef enum gjk3d_ctype {
	GJK3D_CTYPE_SHPERE,
	GJK3D_CTYPE_BOX
}gjk3d_ctype_k;

typedef struct gjk3d_collider {
	gjk3d_ctype_k type;
	fp_t radius;
	vec3f_t position;
	vec3f_t *vertices;
	int vertices_num;
}gjk3d_collider_t, *gjk3d_collider_p;

typedef struct gjk3d_contact {
	vec3f_t normal;
	fp_t depth;
}gjk3d_contact_t, *gjk3d_contact_p;

bool gjk2d_collide(gjk3d_collider_p c1, gjk3d_collider_p c2,
	vec3f_t init_dir, gjk2d_contact_p contact);

bool gjk3d_collide(gjk3d_collider_p c1, gjk3d_collider_p c2,
	vec3f_t init_dir, gjk3d_contact_p contact);

#endif // !LIV_GJK_H
