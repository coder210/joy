#ifndef JSDL_H
#define JSDL_H

#include <SDL3/SDL.h>
#include "jconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ===== 类型别名（直接映射到 SDL3 类型，零转换） ===== */
typedef SDL_Window    jsdl_window_t;
typedef SDL_Renderer  jsdl_renderer_t;
typedef SDL_Surface   jsdl_surface_t;
typedef SDL_Texture   jsdl_texture_t;
typedef SDL_FRect     jsdl_rect_t;
typedef SDL_Color     jsdl_color_t;

/* ===== 初始化/销毁 ===== */
JOY_API bool     jsdl_init(uint32_t flags);
JOY_API void     jsdl_quit(void);
JOY_API bool     jsdl_create_window_and_renderer(const char* title, int w, int h, uint32_t flags,
                                                  jsdl_window_t** win, jsdl_renderer_t** ren);
JOY_API void     jsdl_destroy_renderer(jsdl_renderer_t* ren);
JOY_API void     jsdl_destroy_window(jsdl_window_t* win);

/* ===== 渲染 ===== */
JOY_API void     jsdl_set_render_draw_color(jsdl_renderer_t* ren, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
JOY_API void     jsdl_render_clear(jsdl_renderer_t* ren);
JOY_API void     jsdl_render_present(jsdl_renderer_t* ren);
JOY_API void     jsdl_render_fill_rect(jsdl_renderer_t* ren, const jsdl_rect_t* rect);
JOY_API void     jsdl_render_texture(jsdl_renderer_t* ren, jsdl_texture_t* tex,
                                      const jsdl_rect_t* src, const jsdl_rect_t* dst);
JOY_API void     jsdl_render_texture_rotated(jsdl_renderer_t* ren, jsdl_texture_t* tex,
                                              const jsdl_rect_t* src, const jsdl_rect_t* dst,
                                              double angle, const void* center, int flip);

/* ===== 逻辑呈现 ===== */
JOY_API void     jsdl_set_logical_presentation(jsdl_renderer_t* ren, int w, int h, int mode);
JOY_API void     jsdl_get_logical_presentation(jsdl_renderer_t* ren, int* w, int* h, int* mode);

/* ===== 纹理/表面 ===== */
JOY_API jsdl_surface_t* jsdl_create_surface_from(int w, int h, uint32_t format, void* pixels, int pitch);
JOY_API jsdl_texture_t* jsdl_create_texture_from_surface(jsdl_renderer_t* ren, jsdl_surface_t* surf);
JOY_API void            jsdl_destroy_surface(jsdl_surface_t* surf);
JOY_API void            jsdl_destroy_texture(jsdl_texture_t* tex);

/* ===== 时间 ===== */
JOY_API uint64_t jsdl_get_ticks(void);

/* ===== 事件 ===== */
JOY_API bool     jsdl_push_quit_event(void);

/* ===== 日志 ===== */
JOY_API void     jsdl_log(const char* fmt, ...);
JOY_API const char* jsdl_get_error(void);

/* ===== 常量 ===== */
#define JSDL_INIT_VIDEO SDL_INIT_VIDEO
#define JSDL_INIT_AUDIO SDL_INIT_AUDIO
#define JSDL_EVENT_USER SDL_EVENT_USER

#define JSDL_FLIP_NONE        SDL_FLIP_NONE
#define JSDL_FLIP_HORIZONTAL  SDL_FLIP_HORIZONTAL
#define JSDL_FLIP_VERTICAL    SDL_FLIP_VERTICAL

#define JSDL_PRESENTATION_STRETCH SDL_LOGICAL_PRESENTATION_STRETCH
#define JSDL_PIXELFORMAT_RGBA32   SDL_PIXELFORMAT_RGBA32

#ifdef __cplusplus
}
#endif

#endif /* JSDL_H */
