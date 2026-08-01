#ifndef COLLIDER_COMPONENT_H
#define COLLIDER_COMPONENT_H
#include <joy/jmath.h>

struct ColliderComponent {
    enum ShapeType : int { BOX = 0, SPHERE = 1, CAPSULE = 2 };
    ShapeType type = BOX;
    vec3f_t half_size = { fp_from_float(0.3f), fp_from_float(0.6f), fp_from_float(0.3f) };
    fp_t radius = fp_from_float(0.3f);
    fp_t height = fp_from_float(1.2f);
};

#endif
