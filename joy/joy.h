#ifndef JOY_H
#define JOY_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct jrect
{
        float x;
        float y;
        float w;
        float h;
} jrect_t;

typedef struct jcolor
{
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
} jcolor_t;

#ifdef __cplusplus
}
#endif

#endif // !JOY_H
