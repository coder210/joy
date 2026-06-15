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
#include "jconfig.h"
#include "jcore.h"
#include "jmath.h"
#include "jfont.h"
#include "joy.h"

#ifdef __cplusplus
extern "C" {
#endif
        typedef union jr2d jr2d_t, * jr2d_p;
        typedef struct image image_t, * image_p;
        typedef struct text_texture text_texture_t, * text_p;
        typedef struct animation animation_t, * animation_p;


        JOY_API jr2d_p r2d_create(jwindow_p win);
        JOY_API jr2d_p r2d_wrap(void* sdl_renderer);
        JOY_API void r2d_destroy(jr2d_p r2d);
        JOY_API void* r2d_unwrap(jr2d_p r2d);
        JOY_API vec2_t r2d_coordinates_from_window(jr2d_p r2d, float x, float y);

        JOY_API bool r2d_get_color(jr2d_p r2d, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        JOY_API bool r2d_set_color(jr2d_p r2d, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        JOY_API void r2d_begin(jr2d_p r2d);
        JOY_API void r2d_end(jr2d_p r2d);
        JOY_API bool r2d_set_logical_presentation(jr2d_p r2d, int w, int h, int mode);
        JOY_API bool r2d_get_logical_presentation(jr2d_p r2d, int* w, int* h, int* mode);
        JOY_API void r2d_get_size(jr2d_p r2d, int *w, int *h);
        JOY_API bool r2d_set_vsync(jr2d_p r2d, int vsync);
        JOY_API bool r2d_set_scale(jr2d_p r2d, float sx, float sy);
        JOY_API void r2d_set_colorf(jr2d_p r2d, float r, float g, float b, float a);
        JOY_API void r2d_get_colorf(jr2d_p r2d, float* r, float* g, float* b, float* a);
        JOY_API bool r2d_draw_point(jr2d_p r2d, float x, float y);
        JOY_API bool r2d_draw_points(jr2d_p r2d, const vec2_t* points, int count);

        JOY_API bool shape_draw_line(jr2d_p r2d,
                float x1, float y1, float x2, float y2);
        JOY_API bool r2d_draw_lines(jr2d_p r2d, const vec2_t* points, int count);
        JOY_API bool shape_draw_rectangle(jr2d_p r2d,
                const char* mode, jrect_t rect);
        JOY_API bool shape_draw_polygon(jr2d_p r2d,
                const char* mode, vec2_t* points, int point_count,
                jcolor_t color);
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
                const jrect_t* srcrect, const jrect_t* destrect);
        JOY_API void image_draw_ex(image_p image, 
                const jrect_t* srcrect, vec2_t position,
                float rotation, vec2_t scale, vec2_t origin);

        JOY_API text_p text_createx(font_p font, const char* str, int len, jcolor_t color);
        JOY_API text_p text_create(font_p font, const int* codepoints, int num_codepoints, jcolor_t color);
        JOY_API void text_updatex(text_p text, font_p font, const char* str, int len, jcolor_t color);
        JOY_API void text_update(text_p text, font_p font,
                const int* codepoints, int num_codepoints, jcolor_t color);
        JOY_API void text_destroy(text_p text_tex);
        JOY_API void text_print(jr2d_p r2d, text_p text_tex, float x, float y);
        JOY_API void text_printx(jr2d_p r2d, font_p font, const char* str, int len, jcolor_t color, float x, float y);
        JOY_API int text_get_width(text_p text_tex);
        JOY_API int text_get_height(text_p text_tex);

        JOY_API animation_p animation_create(jr2d_p r2d);
        JOY_API void animation_destroy(animation_p animation);
        JOY_API void animation_reset(animation_p animation);
        JOY_API void animation_add_clip(animation_p animation,
                const char* image_path, float duration, jrect_t src_rect);
        JOY_API bool animation_is_finished(animation_p animation);
        JOY_API void animation_set_position(animation_p animation, float x, float y);
        JOY_API void animation_set_scale(animation_p animation, float x, float y);
        JOY_API void animation_set_rotation(animation_p animation, float rotation);
        JOY_API void animation_update(animation_p animation, float dt);
        JOY_API void animation_draw(animation_p animation, jrect_t* camera);


#ifdef __cplusplus
}
#endif

#endif // !JR2D_H
