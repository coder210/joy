/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jgjk.h
Description: GJK + EPA 3D碰撞检测 (纯C实现)
Author: ydlc
Version: 2.0
Date: 2025.10.13
History:
*************************************************/
#ifndef JGJK_H
#define JGJK_H
#include "jconfig.h"
#include "jmath.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- 碰撞体类型 ---------- */
typedef enum gjk3d_ctype {
	GJK3D_CTYPE_SPHERE,   /* 球体: position + radius */
	GJK3D_CTYPE_BOX,      /* AABB盒: position + half_size */
	GJK3D_CTYPE_CAPSULE,  /* 胶囊: position(中心) + radius + height + axis */
	GJK3D_CTYPE_CYLINDER, /* 圆柱: position + radius + height */
	GJK3D_CTYPE_CONE,     /* 圆锥: position + radius + height */
	GJK3D_CTYPE_MESH      /* 自定义网格: vertices数组 */
} gjk3d_ctype_k;

/* ---------- 3D碰撞体 ---------- */
typedef struct gjk3d_collider {
	gjk3d_ctype_k type;
	vec3f_t position;      /* 中心位置 */
	fp_t radius;           /* 球体/胶囊/圆柱/圆锥半径 */
	vec3f_t half_size;     /* BOX半边长 */
	fp_t height;           /* 胶囊/圆柱/圆锥高度 */
	vec3f_t axis;          /* 胶囊朝向(单位向量) */
	vec3f_t *vertices;     /* MESH顶点数组(借用的指针,不负责释放) */
	int vertices_num;
} gjk3d_collider_t, *gjk3d_collider_p;

/* ---------- 接触信息(含EPA穿透深度) ---------- */
typedef struct gjk3d_contact {
	vec3f_t normal;
	fp_t depth;
} gjk3d_contact_t, *gjk3d_contact_p;

/* ---------- API ---------- */

/* 主碰撞检测: GJK + EPA, 返回是否碰撞, contact填充法线和深度 */
JOY_API bool gjk3d_collide(gjk3d_collider_p c1, gjk3d_collider_p c2,
	vec3f_t init_dir, gjk3d_contact_p contact);

/* 快捷初始化函数(仅设置参数,无内存分配) */
JOY_API void gjk3d_init_sphere(gjk3d_collider_t *c, vec3f_t pos, fp_t radius);
JOY_API void gjk3d_init_box(gjk3d_collider_t *c, vec3f_t pos, vec3f_t half_size);
JOY_API void gjk3d_init_capsule(gjk3d_collider_t *c, vec3f_t pos, fp_t radius, fp_t height, vec3f_t axis);
JOY_API void gjk3d_init_cylinder(gjk3d_collider_t *c, vec3f_t pos, fp_t radius, fp_t height);
JOY_API void gjk3d_init_cone(gjk3d_collider_t *c, vec3f_t pos, fp_t radius, fp_t height);
JOY_API void gjk3d_init_mesh(gjk3d_collider_t *c, vec3f_t *vertices, int num);

#ifdef __cplusplus
}
#endif

#endif // !JGJK_H
