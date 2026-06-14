#ifndef JOY_H
#define JOY_H

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
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
} jcolor_t;

#ifdef __cplusplus
}
#endif

#endif // !JOY_H
