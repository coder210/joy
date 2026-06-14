#include "jsdl.h"
#include <stdarg.h>

/* ===== 初始化/销毁 ===== */
bool jsdl_init(uint32_t flags) { return SDL_Init((SDL_InitFlags)flags); }
void jsdl_quit(void) { SDL_Quit(); }
bool jsdl_create_window_and_renderer(const char* t, int w, int h, uint32_t f, jsdl_window_t** win, jsdl_renderer_t** ren) {
    return SDL_CreateWindowAndRenderer(t, w, h, f, win, ren);
}
void jsdl_destroy_renderer(jsdl_renderer_t* ren) { SDL_DestroyRenderer(ren); }
void jsdl_destroy_window(jsdl_window_t* win) { SDL_DestroyWindow(win); }

/* ===== 渲染 ===== */
void jsdl_set_render_draw_color(jsdl_renderer_t* ren, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(ren, r, g, b, a);
}
void jsdl_render_clear(jsdl_renderer_t* ren) { SDL_RenderClear(ren); }
void jsdl_render_present(jsdl_renderer_t* ren) { SDL_RenderPresent(ren); }
void jsdl_render_fill_rect(jsdl_renderer_t* ren, const jsdl_rect_t* rect) {
    SDL_RenderFillRect(ren, rect);
}
void jsdl_render_texture(jsdl_renderer_t* ren, jsdl_texture_t* tex, const jsdl_rect_t* src, const jsdl_rect_t* dst) {
    SDL_RenderTexture(ren, tex, src, dst);
}
void jsdl_render_texture_rotated(jsdl_renderer_t* ren, jsdl_texture_t* tex, const jsdl_rect_t* src, const jsdl_rect_t* dst, double angle, const void* center, int flip) {
    /* center 实际应为 SDL_FPoint*（仅 xy），调用方传 nullptr 时无需关心 */
    SDL_RenderTextureRotated(ren, tex, src, dst, angle, (const SDL_FPoint*)center, (SDL_FlipMode)flip);
}

/* ===== 逻辑呈现 ===== */
void jsdl_set_logical_presentation(jsdl_renderer_t* ren, int w, int h, int mode) {
    SDL_SetRenderLogicalPresentation(ren, w, h, (SDL_RendererLogicalPresentation)mode);
}
void jsdl_get_logical_presentation(jsdl_renderer_t* ren, int* w, int* h, int* mode) {
    SDL_RendererLogicalPresentation m;
    SDL_GetRenderLogicalPresentation(ren, w, h, &m);
    if (mode) *mode = (int)m;
}

/* ===== 纹理/表面 ===== */
jsdl_surface_t* jsdl_create_surface_from(int w, int h, uint32_t format, void* pixels, int pitch) {
    return SDL_CreateSurfaceFrom(w, h, (SDL_PixelFormat)format, pixels, pitch);
}
jsdl_texture_t* jsdl_create_texture_from_surface(jsdl_renderer_t* ren, jsdl_surface_t* surf) {
    return SDL_CreateTextureFromSurface(ren, surf);
}
void jsdl_destroy_surface(jsdl_surface_t* surf) { SDL_DestroySurface(surf); }
void jsdl_destroy_texture(jsdl_texture_t* tex) { SDL_DestroyTexture(tex); }

/* ===== 时间 ===== */
uint64_t jsdl_get_ticks(void) { return SDL_GetTicks(); }

/* ===== 事件 ===== */
bool jsdl_push_quit_event(void) {
    SDL_Event e; SDL_zero(e); e.type = SDL_EVENT_QUIT; return SDL_PushEvent(&e);
}

/* ===== 日志 ===== */
void jsdl_log(const char* fmt, ...) {
    va_list ap; va_start(ap, fmt); char buf[4096]; SDL_vsnprintf(buf, sizeof(buf), fmt, ap); va_end(ap);
    SDL_Log("%s", buf);
}
const char* jsdl_get_error(void) { return SDL_GetError(); }
