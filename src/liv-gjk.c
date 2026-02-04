#include "liv-gjk.h"
#include <string.h>
#include <float.h>


typedef struct simplex3d {
        vec3f_t points[4];
        int points_num;
}simplex3d_t, * simplex3d_p;



bool gjk2d_collide(gjk3d_collider_p c1, gjk3d_collider_p c2,
        vec3f_t init_dir, gjk2d_contact_p contact)
{
        return false;
}



LIVNET_INLINE void
simplex_init1(simplex3d_p simplex, vec3f_t point0)
{
        simplex->points[0] = point0;
        simplex->points_num = 1;
}

LIVNET_INLINE void
simplex_init2(simplex3d_p simplex, vec3f_t point0, vec3f_t point1)
{
        simplex->points[0] = point0;
        simplex->points[1] = point1;
        simplex->points_num = 2;
}

LIVNET_INLINE void
simplex_init3(simplex3d_p simplex, vec3f_t point0, vec3f_t point1, vec3f_t point2)
{
        simplex->points[0] = point0;
        simplex->points[1] = point1;
        simplex->points[2] = point2;
        simplex->points_num = 3;
}

LIVNET_INLINE void 
simplex3d_push_point(simplex3d_p simplexs, vec3f_t point)
{
        simplexs->points[3] = simplexs->points[2];
        simplexs->points[2] = simplexs->points[1];
        simplexs->points[1] = simplexs->points[0];
        simplexs->points[0] = point;
        if (simplexs->points_num >= 4)
                simplexs->points_num = 4;
        else
                simplexs->points_num++;
}

LIVNET_INLINE vec3f_t
find_shpere_furthest_point(gjk3d_collider_p collider, vec3f_t dir)
{
        dir = vec3f_normalize(dir);
        vec3f_t distance = vec3f_scale(dir, collider->radius);
        vec3f_t max_point = vec3f_add(collider->position, distance);
        return max_point;
}

LIVNET_INLINE vec3f_t
find_mesh_furthest_point(gjk3d_collider_p collider, vec3f_t dir)
{
        vec3f_t max_point = { fp_zero(), fp_zero(), fp_zero() };
        fp_t max_distance = fp_from_float(-FLT_MAX);
        for (int i = 0; i < collider->vertices_num; i++) {
                vec3f_t vertex = collider->vertices[i];
                fp_t distance = vec3f_dot(vertex, dir);
                if (distance > max_distance) {
                        max_distance = distance;
                        max_point = vertex;
                }
        }
        return max_point;
}

LIVNET_INLINE vec3f_t
find_furthest_point(gjk3d_collider_p collider, vec3f_t dir)
{
        vec3f_t max_point = { fp_zero(), fp_zero(), fp_zero() };
        if (collider->type == GJK3D_CTYPE_SHPERE) {
                max_point = find_shpere_furthest_point(collider, dir);
        }
        else if (collider->type == GJK3D_CTYPE_BOX) {
                max_point = find_mesh_furthest_point(collider, dir);
        }
        return max_point;
}

LIVNET_INLINE vec3f_t
get_support_point(gjk3d_collider_p ca, gjk3d_collider_p cb, vec3f_t dir)
{
        vec3f_t a_furthest_point = find_furthest_point(ca, dir);
        vec3f_t b_furthest_point = find_furthest_point(cb, vec3f_negate(dir));
        return vec3f_sub(a_furthest_point, b_furthest_point);
}

LIVNET_INLINE bool 
same_direction(vec3f_t dir, vec3f_t ao)
{
        return vec3f_dot(dir, ao) > fp_zero();
}


LIVNET_INLINE bool
line(simplex3d_p simplex, vec3f_t *dir)
{
        vec3f_t a, b, ab, ao;
        a = simplex->points[0];
        b = simplex->points[1];
        ab = vec3f_normalize(vec3f_sub(b, a));
        ao = vec3f_normalize(vec3f_negate(a));
        if (same_direction(ab, ao)) {
                vec3f_t normal = vec3f_cross(vec3f_cross(ab, ao), ab);
                *dir = vec3f_normalize(normal);
        }
        else {
                simplex_init1(simplex, a);
                *dir = ao;
        }

        return false;
}

LIVNET_INLINE bool
triangle(simplex3d_p simplex, vec3f_t *dir)
{
        vec3f_t a, b, c, ab, ac, ao, abc;
        a = simplex->points[0];
        b = simplex->points[1];
        c = simplex->points[2];
        ab = vec3f_normalize(vec3f_sub(b, a));
        ac = vec3f_normalize(vec3f_sub(c, a));
        ao = vec3f_normalize(vec3f_negate(a));
        abc = vec3f_normalize(vec3f_cross(ab, ac));
        if (same_direction(vec3f_cross(abc, ac), ao)) {
                if (same_direction(ac, ao)) {
                        simplex_init2(simplex, a, c);
                        vec3f_t normal = vec3f_cross(vec3f_cross(ac, ao), ac);
                        *dir = vec3f_normalize(normal);
                }
                else {
                        simplex_init2(simplex, a, b);
                        return line(simplex, dir);
                }
        }
        else {
                if (same_direction(vec3f_cross(ab, abc), ao)) {
                        simplex_init2(simplex, a, b);
                        return line(simplex, dir);
                }
                else {
                        if (same_direction(abc, ao)) {
                                *dir = abc;
                        }
                        else {
                                simplex_init3(simplex, a, c, b);
                                *dir = vec3f_negate(abc);
                        }
                }
        }
        return false;
}

LIVNET_INLINE bool
tetrahedron(simplex3d_p simplex, vec3f_t *dir)
{
        vec3f_t a, b, c, d;
        vec3f_t ab, ac, ad, ao;
        vec3f_t abc, acd, adb;
        a = simplex->points[0];
        b = simplex->points[1];
        c = simplex->points[2];
        d = simplex->points[3];

        ab = vec3f_normalize(vec3f_sub(b, a));
        ac = vec3f_normalize(vec3f_sub(c, a));
        ad = vec3f_normalize(vec3f_sub(d, a));
        ao = vec3f_normalize(vec3f_negate(a));
        
        abc = vec3f_normalize(vec3f_cross(ab, ac));
        acd = vec3f_normalize(vec3f_cross(ac, ad));
        adb = vec3f_normalize(vec3f_cross(ad, ab));
        if (same_direction(abc, ao)) {
                simplex_init3(simplex, a, b, c);
                return triangle(simplex, dir);
        }
        if (same_direction(acd, ao)) {
                simplex_init3(simplex, a, c, d);
                return triangle(simplex, dir);
        }
        if (same_direction(adb, ao)) {
                simplex_init3(simplex, a, d, b);
                return triangle(simplex, dir);
        }
        return true;
}

LIVNET_INLINE bool 
next_simplex(simplex3d_p simplex, vec3f_t *dir)
{
        switch (simplex->points_num) {
        case 2: return line(simplex, dir);
        case 3: return triangle(simplex, dir);
        case 4: return tetrahedron(simplex, dir);
        }
        // never should be here
        return false;
}


bool 
gjk3d_collide(gjk3d_collider_p c1, gjk3d_collider_p c2,
        vec3f_t init_dir, gjk3d_contact_p contact)
{
        simplex3d_t simplex;
        vec3f_t support, dir;

        memset(&simplex, 0, sizeof(simplex));

        // Get initial support point in any dir
        support = get_support_point(c1, c2, init_dir);

        // simplex is an array of points, max count is 4
        simplex3d_push_point(&simplex, support);

        // New dir is towards the origin
        dir = vec3f_normalize(vec3f_negate(support));
        while (true) {
                support = get_support_point(c1, c2, dir);
                if (vec3f_dot(support, dir) < fp_zero()) {
                        return false; // no collision
                }
                simplex3d_push_point(&simplex, support);
                if (next_simplex(&simplex, &dir)) {
                        //*contact = Epa(&simplex, c1, c2);
                        contact->normal.x = fp_zero();
                        contact->normal.y = fp_zero();
                        contact->normal.z = fp_zero();
                        contact->depth = fp_zero();
                        return true;
                }
        }
        return false;
}


