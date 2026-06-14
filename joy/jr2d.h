/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jr2d.h
Description:
Author: ydlc
Version: 1.0
Date: 2024.7.15
History:
*************************************************/
#ifndef JR2D_H
#define JR2D_H
#include "SDL3/SDL.h"
#include "jconfig.h"
#include "jcore.h"
#include "jmath.h"

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


        typedef union jr2d jr2d_t, * jr2d_p;
        typedef struct image image_t, * image_p;

        JOY_API jr2d_p r2d_create(jwindow_p win);
        JOY_API void r2d_destroy(jr2d_p r2d);

        JOY_API void r2d_set_color(jr2d_p r2d, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        JOY_API void r2d_begin(jr2d_p r2d);
        JOY_API void r2d_end(jr2d_p r2d);
        JOY_API void r2d_set_logical_presentation(jr2d_p r2d, int w, int h, int mode);
        JOY_API void r2d_get_logical_presentation(jr2d_p r2d, int* w, int* h, int* mode);


        JOY_API bool shape_draw_line(jr2d_p r2d,
                float x1, float y1, float x2, float y2);
        JOY_API bool shape_draw_rectangle(jr2d_p r2d,
                const char* mode, jrect_t rect);
        JOY_API bool shape_draw_polygon(jr2d_p r2d,
                const char* mode, vec2_t* points, int point_count,
                SDL_FColor color);
        JOY_API bool shape_draw_circle(jr2d_p r2d,
                const char* mode, vec2_t center,
                float radius, int segments);
        JOY_API bool shape_draw_grid(jr2d_p r2d,
                vec2_t start, vec2_t end, float grid_size);
        JOY_API bool shape_draw_gridx(jr2d_p r2d,
                vec2_t position, int rows, int cols, float grid_size);


        JOY_API image_p image_create(jr2d_p r2d, const char* filename);
        JOY_API void image_destroy(image_p image);
        JOY_API void image_draw(image_p image, vec2_t position);
        JOY_API void image_draw1(image_p image,
                const SDL_FRect* srcrect, const SDL_FRect* destrect);
        JOY_API void image_draw_ex(image_p image,
                const SDL_FRect* srcrect, vec2_t position,
                float rotation, vec2_t scale, vec2_t origin);
       

#ifdef __cplusplus
}
#endif

#endif // !JR2D_H
