#ifndef COMPONENT_H
#define COMPONENT_H
#include <stdint.h>
#include <joy2d/jmath.h>

struct ConnectionComponent
{
        int health;
        int frameid;
        int conv;
};


struct IdComponent
{
        int id;
        int64_t hp;
};

struct LogicRectComponent
{
        fp_t width;
        fp_t height;
};

struct LogicPositionComponent
{
        fp_t x;
        fp_t y;
};

struct LogicVelocityComponent
{
        fp_t x;
        fp_t y;
};

struct TransformComponent
{
        float position_x;
        float position_y;
        float rotation_x;
        float rotation_y;
        float scale_x;
        float scale_y;
};

struct PlayerComponent
{
        int conv;
};

struct AttackRayEffectComponent {
        float x, y, w, h;
        float lifetime;
} ;

#endif