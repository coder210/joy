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

struct PositionComponent
{
        float x;
        float y;
};

struct PlayerComponent
{
};

#endif