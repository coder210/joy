#ifndef COMPONENT_H
#define COMPONENT_H

#include <joy2d/jmath.h>

struct IdComponent
{
        int id;
};

struct ConnectionComponent
{
        int health;
        int frameid;
        int conv;
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