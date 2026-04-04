#ifndef COMPONENT_H
#define COMPONENT_H

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

#endif