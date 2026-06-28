#include "../joy.h"
#include "jsclib.h"

#define PTR(n)  duk_get_pointer(ctx, n)
#define INT(n)  ((int)duk_to_number(ctx, n))
#define STR(n)  duk_to_string(ctx, n)

/* ---- helpers ---- */

static vec3f_t read_vec3f(duk_context* ctx, int idx)
{
	vec3f_t v = { 0, 0, 0 };
	duk_get_prop_string(ctx, idx, "x"); v.x = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, idx, "y"); v.y = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
	duk_get_prop_string(ctx, idx, "z"); v.z = (fp_t)duk_to_number(ctx, -1); duk_pop(ctx);
	return v;
}

/* ---- API ---- */

static duk_ret_t jgjk3d_new_collider(duk_context* ctx)
{
	gjk3d_collider_t* c = (gjk3d_collider_t*)malloc(sizeof(gjk3d_collider_t));
	if (!c) { duk_push_pointer(ctx, NULL); return 1; }
	memset(c, 0, sizeof(*c));
	duk_push_pointer(ctx, c);
	return 1;
}

static duk_ret_t jgjk3d_init_box(duk_context* ctx)
{
	gjk3d_collider_t* c = (gjk3d_collider_t*)PTR(0);
	c->position = read_vec3f(ctx, 1);
	c->half_size = read_vec3f(ctx, 2);
	c->type = GJK3D_CTYPE_BOX;
	return 0;
}

static duk_ret_t jgjk3d_init_sphere(duk_context* ctx)
{
	gjk3d_collider_t* c = (gjk3d_collider_t*)PTR(0);
	c->position = read_vec3f(ctx, 1);
	c->radius = (fp_t)duk_to_number(ctx, 2);
	c->type = GJK3D_CTYPE_SPHERE;
	return 0;
}

static duk_ret_t jgjk3d_collide(duk_context* ctx)
{
	gjk3d_collider_t* a = (gjk3d_collider_t*)PTR(0);
	gjk3d_collider_t* b = (gjk3d_collider_t*)PTR(1);
	vec3f_t dir = read_vec3f(ctx, 2);

	gjk3d_contact_t contact;
	int hit = gjk3d_collide(a, b, dir, &contact);

	duk_push_object(ctx);
	duk_push_boolean(ctx, hit != 0); duk_put_prop_string(ctx, -2, "hit");
	if (hit) {
		duk_push_number(ctx, (duk_double_t)contact.depth); duk_put_prop_string(ctx, -2, "depth");
		duk_push_object(ctx);
		duk_push_number(ctx, (duk_double_t)contact.normal.x); duk_put_prop_string(ctx, -2, "x");
		duk_push_number(ctx, (duk_double_t)contact.normal.y); duk_put_prop_string(ctx, -2, "y");
		duk_push_number(ctx, (duk_double_t)contact.normal.z); duk_put_prop_string(ctx, -2, "z");
		duk_put_prop_string(ctx, -2, "normal");
	}
	return 1;
}

static duk_ret_t jgjk3d_destroy(duk_context* ctx)
{
	gjk3d_collider_t* c = (gjk3d_collider_t*)PTR(0);
	if (c) free(c);
	return 0;
}

/* ---- 模块注册 ---- */

void jsopen_gjk3d(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jgjk3d_new_collider, 0); duk_put_prop_string(ctx, -2, "new_collider");
	duk_push_c_function(ctx, jgjk3d_init_box, 3);     duk_put_prop_string(ctx, -2, "init_box");
	duk_push_c_function(ctx, jgjk3d_init_sphere, 3);  duk_put_prop_string(ctx, -2, "init_sphere");
	duk_push_c_function(ctx, jgjk3d_collide, 3);      duk_put_prop_string(ctx, -2, "collide");
	duk_push_c_function(ctx, jgjk3d_destroy, 1);      duk_put_prop_string(ctx, -2, "destroy");
	duk_put_global_string(ctx, "gjk3d");
}
