/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: collision.h
Description: aabb, obb, gjk
Author: ydlc
Version: 1.0
Date: 2024.12.14
History:
*************************************************/
#ifndef COLLISION_H
#define COLLISION_H
#include <string.h>
#include "jconfig.h"
#include "jmath.h"

typedef struct ray2df {
        vec2f_t origin, direction;
}ray2df_t;

typedef struct ray2d_collisionf {
        bool hit;               // Did the ray hit something?
        fp_t distance;         // Distance to the nearest hit
        vec2f_t point;          // Point of the nearest hit
        vec2f_t normal;         // Surface normal of hit
}ray2d_collisionf_t;


typedef struct rectanglef {
        fp_t x; // rectangle top-left corner position x
        fp_t y; // rectangle top-left corner position y
        fp_t width;
        fp_t height;
}rectanglef_t;

typedef struct circlef {
        vec2f_t center;
        fp_t radius;
}circlef_t;

typedef struct polygonf {
        vec2f_t* vertices;
        int num_vertices;
}polygonf_t;


typedef struct contact2df {
        vec2f_t sp; // starting point
        vec2f_t ep; // ending point
        vec2f_t normal;
        fp_t depth;
}contact2df_t;


typedef struct ray3df {
        vec3f_t position, direction;
}ray3df_t;

typedef struct ray3d_collisionf {
        bool hit;               // Did the ray hit something?
        fp_t distance;         // Distance to the nearest hit
        vec3f_t point;          // Point of the nearest hit
        vec3f_t normal;         // Surface normal of hit
}ray3d_collisionf_t;

typedef struct spheref {
        vec3f_t position;
        fp_t radius;
}spheref_t;

typedef struct bounding_boxf {
        vec3f_t min;            // Minimum vertex box-corner
        vec3f_t max;            // Maximum vertex box-corner
} bounding_boxf_t;

#ifdef __cplusplus
extern "C" {
#endif

        JOY_API ray2d_collisionf_t collision2df_get_ray_circle(ray2df_t ray,
                circlef_t circle);
        JOY_API ray2d_collisionf_t collision2df_get_ray_rectangle(ray2df_t ray,
                rectanglef_t rect);
        JOY_API ray2d_collisionf_t collision2df_get_ray_rectanglex(ray2df_t ray,
                rectanglef_t rect, fp_t angle);
        JOY_API ray2d_collisionf_t collision2df_get_ray_polygon(ray2df_t ray,
                polygonf_t polygon);
        JOY_API bool collision2df_check_point_rectangle(vec2f_t point,
                rectanglef_t rect);
        JOY_API bool collision2df_check_point_circle(vec2f_t point, circlef_t circle);
        JOY_API bool collision2df_check_point_triangle(vec2f_t point,
                vec2f_t p1, vec2f_t p2, vec2f_t p3);
        JOY_API bool collision2df_check_lines(vec2f_t a1, vec2f_t b1,
                vec2f_t a2, vec2f_t b2, vec2f_t* cp);
        JOY_API bool collision2df_check_point_line(vec2f_t point,
                vec2f_t p1, vec2f_t p2, fp_t threshold);
        JOY_API bool collision2df_check_circles(circlef_t a, circlef_t b);
        JOY_API bool collision2df_check_rectangles(rectanglef_t a, rectanglef_t b);
        JOY_API bool collision2df_check_polygons(polygonf_t a, polygonf_t b);
        JOY_API bool collision2df_check_circle_rectangle(circlef_t a, rectanglef_t b);
        JOY_API bool collision2df_check_circle_polygon(circlef_t a, polygonf_t b);
        JOY_API int collision2df_get_circles(circlef_t a, circlef_t b,
                contact2df_t* contact);
        JOY_API int collision2df_get_rectangles(rectanglef_t a, fp_t a_angle,
                rectanglef_t b, fp_t b_angle,
                contact2df_t* contact);
        JOY_API fp_t find_min_separation(polygonf_t a, polygonf_t b,
                vec2f_t* axis, vec2f_t* point);
        JOY_API int collision2df_get_polygons(polygonf_t a, polygonf_t b,
                contact2df_t* contact);
        JOY_API ray3d_collisionf_t collision3df_get_ray_triangle(ray3df_t ray,
                vec3f_t p1, vec3f_t p2, vec3f_t p3);
        JOY_API ray3d_collisionf_t collision3df_get_ray_quad(ray3df_t ray,
                vec3f_t p1, vec3f_t p2, vec3f_t p3, vec3f_t p4);
        JOY_API ray3d_collisionf_t collision3df_get_ray_sphere(ray3df_t ray,
                spheref_t sphere);
        JOY_API ray3d_collisionf_t collision3df_get_ray_box(ray3df_t ray,
                bounding_boxf_t box);
        JOY_API bool collision3df_check_spheres(spheref_t a, spheref_t b);
        JOY_API bool collision3df_check_boxes(bounding_boxf_t a, 
                bounding_boxf_t b);
        JOY_API bool collision3df_check_box_sphere(bounding_boxf_t a, 
                spheref_t b);

#ifdef __cplusplus
}
#endif

#endif // !COLLISION_H