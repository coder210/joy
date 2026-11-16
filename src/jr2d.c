/******************************************************************************
 * src/jr2d.c —— r2d 域唯一实现文件（一个域一个 .c，配 jr2d.h）
 * 职责：2D 渲染 / 字体 / 粒子 / spine / 2D 控件与即时模式 UI
 * 合并自：jr2d.c / jfont.c / jparticle.c / jspine.c / jui.c / jui_im.c
 * 边界：原 ui 域已按渲染上下文并入 r2d（依赖 joy_r2d_p 的控件归 2D 渲染器）；
 *       r3d 的 3D HUD 控件同理归 3D 渲染器，两边都不再有独立的 UI 文件。
 * 说明：本域公开 API 声明在配套头 src/jr2d.h，由 include/joy/japi.h 聚合包含；
 *       本文件是该域唯一的编译单元。
 *       内部符号（static / 私有宏 / typedef）保持各分片独立，未跨分片改名。
 ******************************************************************************/

#include "jr2d.h"
#include <float.h>
#include <stdlib.h>
#include <string.h>

/* ==================== 合并自 src/jr2d.c ==================== */
#include "external/stb_image.h"
#include "external/stb_image_write.h"
#include "external/stb_truetype.h"
#include <SDL3/SDL.h>
#include <spine/spine.h>
#include <spine/extension.h>
#include "external/klib/kvec.h"
#include "joy.h"

/* 裁剪栈最大深度：嵌套面板（窗口 > 面板 > 滚动区 > 列表项）足够用 */
#define R2D_CLIP_STACK_MAX 32

struct joy_r2d {
        SDL_Renderer* sdl;

        /* 每个 renderer 独立的帧时钟，由各自的 joy_r2d_begin() 递增。
         * 文字缓存条目按 owner 归属到对应时钟，多实例不会互相推进 LRU。 */
        Uint64 frame;

        /* FOV 渐变纹理按 renderer 持有: SDL_Texture 绑定创建它的 renderer,
         * 不能用函数级 static 缓存，否则多实例会跨 renderer 复用并悬空。 */
        SDL_Texture* fov_gradient_tex;

        /* joy_r2d_wrap 借用外部 renderer, 销毁时不能连带销毁它 */
        bool owns_sdl;

        /* 裁剪状态。clip_enabled 为 false 时表示无裁剪。
         * 栈保存 push 之前的 (enabled, rect)，pop 时原样恢复。 */
        bool     clip_enabled;
        joy_rect_t  clip_rect;
        int      clip_top;
        bool     stack_enabled[R2D_CLIP_STACK_MAX];
        joy_rect_t  stack_rect[R2D_CLIP_STACK_MAX];
};

/* 实现在下方文字缓存区（此刻 jtext_entry 尚未定义）：只清掉属于该
 * renderer 的缓存条目，供 joy_r2d_destroy 使用。 */
static void jtext_cache_clear_for(joy_r2d_p r2d);

struct joy_image {
        SDL_Texture* texture;
        char filepath[JOY_MAX_PATH];
};

struct joy_text {
        SDL_Texture* texture;
        int real_width;
        int real_height;
};


typedef struct joy_animation_clip {
        joy_image_p image;
        float duration;
        joy_rect_t src_rect;
}joy_animation_clip_t, * joy_animation_clip_p;


struct joy_animation {
        joy_r2d_p r2d;
        kvec_t(joy_animation_clip_t) clips;
        bool is_loop;
        int idx_frame;
        float total_time;
        float last_time;
        joy_vec2_t position;
        float rotation;
        joy_vec2_t scale;
        joy_vec2_t origin;
};


joy_r2d_p joy_r2d_create(joy_window_p win)
{
        int w, h;
        /* joy_core_get_raw_window 会直接解引用 win, 必须先判空 */
        if (!win) { SDL_LogError(SDL_LOG_CATEGORY_RENDER, "joy_r2d_create: null window"); return NULL; }
        SDL_Window* raw = (SDL_Window*)joy_core_get_raw_window(win);
        if (!raw) { SDL_LogError(SDL_LOG_CATEGORY_RENDER, "joy_r2d_create: null window"); return NULL; }
        SDL_GetWindowSize(raw, &w, &h);
        joy_r2d_p r2d = (joy_r2d_p)SDL_calloc(1, sizeof(joy_r2d_t));
        if (!r2d) return NULL;
        r2d->sdl = SDL_CreateRenderer(raw, NULL);
        if (!r2d->sdl) {
                SDL_LogError(SDL_LOG_CATEGORY_RENDER, "joy_r2d_create: SDL_CreateRenderer failed: %s", SDL_GetError());
                SDL_free(r2d);
                return NULL;
        }
        r2d->owns_sdl = true;
        joy_r2d_set_logical_presentation(r2d, w, h);
        return r2d;
}

joy_r2d_p joy_r2d_wrap(void* sdl_renderer)
{
        joy_r2d_p r2d = (joy_r2d_p)SDL_calloc(1, sizeof(joy_r2d_t));
        if (!r2d) return NULL;
        r2d->sdl = (SDL_Renderer*)sdl_renderer;
        /* 借用外部 renderer, 销毁时不能替宿主把它销毁掉 */
        r2d->owns_sdl = false;
        return r2d;
}

void* joy_r2d_unwrap(joy_r2d_p r2d)
{
        return r2d ? (void*)r2d->sdl : NULL;
}


joy_vec2_t joy_r2d_coordinates_from_window(joy_r2d_p r2d, float x, float y)
{
    joy_vec2_t logic;
    SDL_RenderCoordinatesFromWindow(r2d->sdl, x, y, &logic.x, &logic.y);
    return logic;
}

void joy_r2d_destroy(joy_r2d_p r2d)
{
        if (!r2d) return;
        /* 只释放属于这个 renderer 的缓存纹理。SDL 纹理不可跨 renderer 使用，
         * 若沿用全局 clear 会连带销毁其它存活 renderer 的纹理，造成悬空。 */
        jtext_cache_clear_for(r2d);
        /* FOV 渐变纹理是本 renderer 创建的, 必须在销毁 renderer 前释放 */
        if (r2d->fov_gradient_tex) SDL_DestroyTexture(r2d->fov_gradient_tex);
        if (r2d->owns_sdl) SDL_DestroyRenderer(r2d->sdl);
        SDL_free(r2d);
}

bool joy_r2d_get_color(joy_r2d_p r2d, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a)
{
        return SDL_GetRenderDrawColor(r2d->sdl, r, g, b, a);
}

bool joy_r2d_set_color(joy_r2d_p r2d, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
        return SDL_SetRenderDrawColor(r2d->sdl, r, g, b, a);
}

/* 递增本实例帧时钟：文字缓存以帧为单位判断条目是否闲置 */
void joy_r2d_begin(joy_r2d_p r2d) { r2d->frame++; SDL_RenderClear(r2d->sdl); }
void joy_r2d_end(joy_r2d_p r2d) { SDL_RenderPresent(r2d->sdl); }

bool joy_r2d_set_logical_presentation(joy_r2d_p r2d, int w, int h)
{
        return joy_r2d_set_logical_presentation_mode(r2d, w, h, SDL_LOGICAL_PRESENTATION_STRETCH);
}

bool joy_r2d_set_logical_presentation_mode(joy_r2d_p r2d, int w, int h, int mode)
{
        return SDL_SetRenderLogicalPresentation(r2d->sdl, w, h, (SDL_RendererLogicalPresentation)mode);
}

bool joy_r2d_get_logical_presentation(joy_r2d_p r2d, int* w, int* h, int* mode)
{
        SDL_RendererLogicalPresentation m;
        bool r = SDL_GetRenderLogicalPresentation(r2d->sdl, w, h, &m);
        if (mode) *mode = (int)m;
        return r;
}

void joy_r2d_get_size(joy_r2d_p r2d, int* w, int* h)
{
        SDL_GetRenderOutputSize(r2d->sdl, w, h);
}

bool joy_r2d_set_vsync(joy_r2d_p r2d, int vsync)
{
        return SDL_SetRenderVSync(r2d->sdl, vsync);
}

bool joy_r2d_set_scale(joy_r2d_p r2d, float sx, float sy)
{
        return SDL_SetRenderScale(r2d->sdl, sx, sy);
}

/* ===================== 比例坐标（0~1）基础转换 =====================
 *
 * 与 jui 里按钮 / 摇杆等控件的百分比语义一致：
 *   x / w → 逻辑分辨率的宽；y / h → 逻辑分辨率的高。
 * 未启用逻辑分辨率时回退到渲染输出尺寸，保证任何配置下都有效。
 */
void joy_r2d_fpx(joy_r2d_p r2d, float fx, float fy, float fw, float fh,
             float* x, float* y, float* w, float* h)
{
        int bw = 0, bh = 0;
        if (!r2d) { bw = bh = 0; }
        else {
                joy_r2d_get_logical_presentation(r2d, &bw, &bh, NULL);
                if (bw <= 0 || bh <= 0) joy_r2d_get_size(r2d, &bw, &bh);   /* 回退：输出尺寸 */
        }
        const float W = (float)bw, H = (float)bh;
        if (x) *x = fx * W;
        if (y) *y = fy * H;
        if (w) *w = fw * W;
        if (h) *h = fh * H;
}

joy_rect_t joy_r2d_frect(joy_r2d_p r2d, float fx, float fy, float fw, float fh)
{
        joy_rect_t r = { 0.0f, 0.0f, 0.0f, 0.0f };
        joy_r2d_fpx(r2d, fx, fy, fw, fh, &r.x, &r.y, &r.w, &r.h);
        return r;
}

void joy_r2d_set_colorf(joy_r2d_p r2d, float r, float g, float b, float a)
{
        SDL_SetRenderDrawColorFloat(r2d->sdl, r, g, b, a);
}

void joy_r2d_get_colorf(joy_r2d_p r2d, float* r, float* g, float* b, float* a)
{
        SDL_GetRenderDrawColorFloat(r2d->sdl, r, g, b, a);
}

bool joy_r2d_draw_point(joy_r2d_p r2d, float x, float y)
{
        return SDL_RenderPoint(r2d->sdl, x, y);
}

bool joy_r2d_draw_points(joy_r2d_p r2d, const joy_vec2_t* points, int count)
{
    if (count >= 512) {
        SDL_FPoint *raw_points = SDL_malloc(sizeof(SDL_FPoint) * count);
        for (int i = 0; i < count; i++) {
            raw_points[i].x = points[i].x;
            raw_points[i].y = points[i].y;
        }
        bool result = SDL_RenderPoints(r2d->sdl, raw_points, count);
        SDL_free(raw_points);
        return result;
    }
    else {
        SDL_FPoint raw_points[512];
        for (int i = 0; i < count; i++) {
            raw_points[i].x = points[i].x;
            raw_points[i].y = points[i].y;
        }
        return SDL_RenderPoints(r2d->sdl, raw_points, count);
    }
}

bool joy_r2d_draw_lines(joy_r2d_p r2d, const joy_vec2_t* points, int count)
{
        if (count >= 512) {
            SDL_FPoint* raw_points = SDL_malloc(sizeof(SDL_FPoint) * count);
            for (int i = 0; i < count; i++) {
                raw_points[i].x = points[i].x;
                raw_points[i].y = points[i].y;
            }
            bool result = SDL_RenderLines(r2d->sdl, raw_points, count);
            SDL_free(raw_points);
            return result;
        }
        else {
            SDL_FPoint raw_points[512];
            for (int i = 0; i < count; i++) {
                raw_points[i].x = points[i].x;
                raw_points[i].y = points[i].y;
            }
            return SDL_RenderLines(r2d->sdl, raw_points, count);
        }
}

bool joy_r2d_shape_draw_line(joy_r2d_p r2d, float x1, float y1, float x2, float y2)
{
    bool result;
    result = SDL_RenderLine(r2d->sdl, x1, y1, x2, y2);
    return result;
}

bool joy_r2d_shape_draw_rectangle(joy_r2d_p r2d, const char* mode, joy_rect_t rect)
{
        SDL_FRect raw_rect = { .x = rect.x, .y = rect.y, .w = rect.w, .h = rect.h};
        bool result;
        if (SDL_strcmp(mode, "line") == 0) {
                result = SDL_RenderRect(r2d->sdl, &raw_rect);
        }
        else if (SDL_strcmp(mode, "fill") == 0) {
                result = SDL_RenderFillRect(r2d->sdl, &raw_rect);
        }
        else {
                result = false;
        }
        return result;
}

static bool
draw_polygon_outline(joy_r2d_p r2d, joy_vec2_t* points, int count)
{
        if (count >= 512) {
            SDL_FPoint* raw_points = SDL_malloc(sizeof(SDL_FPoint) * (count + 1));
            for (int i = 0; i < count; i++) {
                raw_points[i].x = points[i].x;
                raw_points[i].y = points[i].y;
            }
            raw_points[count].x = raw_points[0].x;
            raw_points[count].y = raw_points[0].y;
            bool result = SDL_RenderLines(r2d->sdl, raw_points, count + 1);
            SDL_free(raw_points);
            return result;
        }
        else {
            SDL_FPoint raw_points[513] = { 0 };
            for (int i = 0; i < count; i++) {
                raw_points[i].x = points[i].x;
                raw_points[i].y = points[i].y;
            }
            raw_points[count].x = raw_points[0].x;
            raw_points[count].y = raw_points[0].y;
            return SDL_RenderLines(r2d->sdl, raw_points, count + 1);
        }
}

static bool
draw_polygon_filled(joy_r2d_p r2d,
        joy_vec2_t* points, int point_count, joy_color_t color)
{
        bool result;
        SDL_Vertex* vertices;
        int vertex_count;
        int* indices = NULL;
        int indices_count = 0;

        vertex_count = point_count;
        vertices = SDL_malloc(sizeof(SDL_Vertex) * vertex_count);

        if (vertex_count >= 3) {
                indices_count = (vertex_count - 2) * 3;
                indices = SDL_malloc(sizeof(int) * indices_count);
                if (indices) {
                        for (int i = 0; i < vertex_count - 2; i++) {
                                indices[i * 3] = 0;
                                indices[i * 3 + 1] = i + 1;
                                indices[i * 3 + 2] = i + 2;
                        }
                }
        }

        for (int i = 0; i < vertex_count; i++) {
                vertices[i].position.x = points[i].x;
                vertices[i].position.y = points[i].y;
                vertices[i].color.r = color.r;
                vertices[i].color.g = color.g;
                vertices[i].color.b = color.b;
                vertices[i].color.a = color.a;
                vertices[i].tex_coord.x = 0.0f;
                vertices[i].tex_coord.y = 0.0f;
        }

        if (indices) {
                result = SDL_RenderGeometry(r2d->sdl, NULL, vertices, vertex_count, indices, indices_count);
                SDL_free(indices);
        }
        else {
                result = false;
        }

        SDL_free(vertices);

        return result;
}

bool
joy_r2d_shape_draw_polygon(joy_r2d_p r2d, const char* mode,
        joy_vec2_t* points, int point_count, joy_color_t color)
{
        bool result;
        if (SDL_strcmp(mode, "line") == 0) {
                result = draw_polygon_outline(r2d, points, point_count);
        }
        else if (SDL_strcmp(mode, "fill") == 0) {
                result = draw_polygon_filled(r2d, points, point_count, color);
        }
        else {
                result = false;
        }
        return result;
}

static bool
draw_circle_outline(joy_r2d_p r2d, joy_vec2_t center, float radius, int segments)
{
        bool result;
        float angle;
        SDL_FPoint* points;
        points = (SDL_FPoint*)SDL_malloc(sizeof(SDL_FPoint) * (segments + 1));
        for (int i = 0; i < segments; i++) {
                angle = (float)i / (float)segments * 2.0f * 3.1415926535f;
                points[i].x = center.x + cosf(angle) * radius;
                points[i].y = center.y + sinf(angle) * radius;
        }
        points[segments].x = points[0].x;
        points[segments].y = points[0].y;
        result = SDL_RenderLines(r2d->sdl, points, segments + 1);
        SDL_free(points);
        return result;
}

static bool
draw_circle_filled(joy_r2d_p r2d, joy_vec2_t center, float radius, int segments, joy_color_t color)
{
        bool result;
        SDL_Vertex* vertices;
        int vertex_count;
        int* indices;

        vertex_count = segments + 1;
        vertices = SDL_malloc(sizeof(SDL_Vertex) * vertex_count);
        indices = SDL_malloc(sizeof(int) * segments * 3);

        if (!vertices || !indices) {
                if (vertices) SDL_free(vertices);
                if (indices) SDL_free(indices);
                return false;
        }

        vertices[0].position.x = center.x;
        vertices[0].position.y = center.y;
        vertices[0].color.r = color.r;
        vertices[0].color.g = color.g;
        vertices[0].color.b = color.b;
        vertices[0].color.a = color.a;
        vertices[0].tex_coord.x = 0.0f;
        vertices[0].tex_coord.y = 0.0f;

        for (int i = 0; i < segments; i++) {
                float angle = (float)i / (float)segments * 2.0f * 3.1415926535f;
                vertices[i + 1].position.x = center.x + cosf(angle) * radius;
                vertices[i + 1].position.y = center.y + sinf(angle) * radius;
                vertices[i + 1].color = vertices[0].color;
                vertices[i + 1].tex_coord.x = 0.0f;
                vertices[i + 1].tex_coord.y = 0.0f;

                indices[i * 3] = 0;
                indices[i * 3 + 1] = i + 1;
                indices[i * 3 + 2] = (i + 1) % segments + 1;
        }

        result = SDL_RenderGeometry(r2d->sdl, NULL, vertices, vertex_count, indices, segments * 3);

        SDL_free(vertices);
        SDL_free(indices);

        return result;
}

bool
joy_r2d_shape_draw_circle(joy_r2d_p r2d, const char* mode, joy_vec2_t center, float radius, int segments)
{
        bool result;
        joy_color_t color;
        if (segments < 8) segments = 8;
        if (segments > 360) segments = 360;
        if (SDL_strcmp(mode, "line") == 0) {
                result = draw_circle_outline(r2d, center, radius, segments);
        }
        else if (SDL_strcmp(mode, "fill") == 0) {
                SDL_GetRenderDrawColor(r2d->sdl, &color.r, &color.g, &color.b, &color.a);
                result = draw_circle_filled(r2d, center, radius, segments, color);
        }
        else {
                result = false;
        }
        return result;
}

bool
joy_r2d_shape_draw_grid(joy_r2d_p r2d,
        joy_vec2_t start, joy_vec2_t end,
        float grid_size)
{
        float x1, x2, y1, y2;
        for (float x = start.x; x <= end.x; x += grid_size) {
                x1 = x;
                x2 = start.y;
                y1 = x;
                y2 = end.y;
                SDL_RenderLine(r2d->sdl, x1, x2, y1, y2);
        }
        for (float y = start.y; y <= end.y; y += grid_size) {
                x1 = start.x;
                x2 = y;
                y1 = end.x;
                y2 = y;
                SDL_RenderLine(r2d->sdl, x1, x2, y1, y2);
        }
        return true;
}

bool
joy_r2d_shape_draw_gridx(joy_r2d_p r2d,
        joy_vec2_t position,
        int rows, int cols, float grid_size)
{
        float x1, x2, y1, y2;
        for (int i = 0; i <= rows; i++) {
                x1 = position.x;
                y1 = i * grid_size + position.y;
                x2 = cols * grid_size + position.x;
                y2 = y1;
                SDL_RenderLine(r2d->sdl, x1, y1, x2, y2);
        }
        for (int i = 0; i <= cols; i++) {
                x1 = i * grid_size + position.x;
                y1 = position.y;
                x2 = x1;
                y2 = rows * grid_size + position.y;
                SDL_RenderLine(r2d->sdl, x1, y1, x2, y2);
        }
        return true;
}



static joy_image_p load_bmp(joy_r2d_p r2d, const char* filename)
{
	char filepath[JOY_MAX_PATH] = { 0 };
	joy_image_p image;
	SDL_strlcat(filepath, filename, JOY_MAX_PATH);
	SDL_Surface* surface = SDL_LoadBMP(filepath);
	if (!surface) {
		return NULL;
	}
	image = (joy_image_p)SDL_malloc(sizeof(joy_image_t));
	SDL_assert(image);
	SDL_strlcpy(image->filepath, filepath, JOY_MAX_PATH);
	image->texture = SDL_CreateTextureFromSurface(r2d->sdl, surface);
	SDL_DestroySurface(surface);
	return image;
}

static joy_image_p load_png(joy_r2d_p r2d, const char* filename)
{
	stbi_uc* filedata;
	size_t sz;
	char filepath[JOY_MAX_PATH] = { 0 };
	joy_image_p image;
	int w, h, channels;
	unsigned char* pixels;
	SDL_Surface* surface;
	filedata = (stbi_uc*)SDL_LoadFile(filename, &sz);
	if (!filedata) {
		return NULL;
	}

	SDL_strlcat(filepath, filename, JOY_MAX_PATH);
	pixels = stbi_load_from_memory(filedata, sz, &w, &h, &channels, STBI_rgb_alpha);
	if (!pixels) {
		SDL_free(filedata);
		SDL_Log("Failed to decode PNG: %s", filename);
		return NULL;
	}

	surface = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, pixels, w * 4);
	if (!surface) {
		stbi_image_free(pixels);
		SDL_free(filedata);
		SDL_Log("Failed to create surface from PNG: %s", filename);
		return NULL;
	}

	image = (joy_image_p)SDL_malloc(sizeof(joy_image_t));
	if (!image) { stbi_image_free(pixels); SDL_DestroySurface(surface); SDL_free(filedata); return NULL; }

	image->texture = SDL_CreateTextureFromSurface(r2d->sdl, surface);
	SDL_DestroySurface(surface);
	stbi_image_free(pixels);
	SDL_free(filedata);
	SDL_strlcpy(image->filepath, filepath, JOY_MAX_PATH);

	if (!image->texture) {
		SDL_Log("SDL error: %s", SDL_GetError());
		SDL_Log("Failed to create texture from PNG: %s", filename);
		SDL_free(image);
		return NULL;
	}
	return image;
}

joy_image_p joy_r2d_image_create(joy_r2d_p r2d, const char* filename)
{
	joy_image_p image = NULL;
	if (SDL_strstr(filename, ".bmp")) {
		image = load_bmp(r2d, filename);
	}
	else if (SDL_strstr(filename, ".png")) {
		image = load_png(r2d, filename);
	}
	return image;
}

static joy_image_p image_create_from_surface(joy_r2d_p r2d,
        SDL_Surface* surface, 
        const char* label)
{
	joy_image_p image;
	if (!surface) return NULL;
	image = (joy_image_p)SDL_malloc(sizeof(joy_image_t));
	if (!image) return NULL;
	image->texture = SDL_CreateTextureFromSurface(r2d->sdl, surface);
	SDL_strlcpy(image->filepath, label ? label : "(surface)", JOY_MAX_PATH);
	if (!image->texture) {
		SDL_free(image);
		return NULL;
	}
	return image;
}

void joy_r2d_image_destroy(joy_image_p image)
{
	SDL_assert(image);
	if (image->texture) {
		SDL_DestroyTexture(image->texture);
	}
	SDL_free(image);
}

void joy_r2d_image_draw(joy_image_p image, joy_vec2_t position)
{
	joy_rect_t srcrect = { 0, 0, image->texture->w, image->texture->h };
	joy_vec2_t scale = { 1.0f, 1.0f };
	joy_vec2_t origin = { 0.0f, 0.0f };
	joy_r2d_image_draw_ex(image, &srcrect, position, 0.0f, scale, origin);
}

void joy_r2d_image_draw1(joy_image_p image,
	const joy_rect_t* srcrect, const joy_rect_t* destrect)
{
        SDL_FRect raw_src, raw_dest;
	SDL_Renderer *renderer;
	SDL_FRect* src_ptr = NULL;
	SDL_FRect* dst_ptr = NULL;
	renderer = SDL_GetRendererFromTexture(image->texture);
	if (srcrect) {
		raw_src.x = srcrect->x; raw_src.y = srcrect->y;
		raw_src.w = srcrect->w; raw_src.h = srcrect->h;
		src_ptr = &raw_src;
	}
	if (destrect) {
		raw_dest.x = destrect->x; raw_dest.y = destrect->y;
		raw_dest.w = destrect->w; raw_dest.h = destrect->h;
		dst_ptr = &raw_dest;
	}
	SDL_RenderTexture(renderer, image->texture, src_ptr, dst_ptr);
}

void joy_r2d_image_set_scale_mode(joy_image_p image, int nearest)
{
	SDL_SetTextureScaleMode(image->texture,
		nearest ? SDL_SCALEMODE_NEAREST : SDL_SCALEMODE_LINEAR);
}

void joy_r2d_image_draw_ex(joy_image_p image, const joy_rect_t* srcrect, joy_vec2_t position,
	float rotation, joy_vec2_t scale, joy_vec2_t origin)
{
	SDL_Renderer *renderer;
	SDL_FRect raw_src, raw_dest;
	SDL_FPoint center;
	renderer = SDL_GetRendererFromTexture(image->texture);

	// 负 scale → 转换为正宽度 + 翻转标志
	SDL_FlipMode flip = SDL_FLIP_NONE;
	float sx = scale.x, sy = scale.y;
	if (sx < 0) { flip = (SDL_FlipMode)(flip | SDL_FLIP_HORIZONTAL); sx = -sx; }
	if (sy < 0) { flip = (SDL_FlipMode)(flip | SDL_FLIP_VERTICAL);   sy = -sy; }

    raw_src.x = srcrect->x;
    raw_src.y = srcrect->y;
    raw_src.w = srcrect->w;
    raw_src.h = srcrect->h;

    raw_dest.x = (position.x - origin.x * sx);
    raw_dest.y = (position.y - origin.y * sy);
    raw_dest.w = srcrect->w * sx;
    raw_dest.h = srcrect->h * sy;
	center.x = origin.x * sx;
	center.y = origin.y * sy;

	SDL_RenderTextureRotated(renderer, image->texture,
        &raw_src, &raw_dest,
		rotation * 180.0f / 3.1415926f,
		&center, flip);
	//SDL_SetTextureAlphaMod(image->texture, 255);
}

joy_image_p joy_r2d_image_create_empty(joy_r2d_p r2d, int w, int h)
{
    SDL_Texture* tex = SDL_CreateTexture(r2d->sdl, SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING, w, h);
    if (!tex) return NULL;
    joy_image_p img = (joy_image_p)SDL_malloc(sizeof(joy_image_t));
    if (!img) { SDL_DestroyTexture(tex); return NULL; }
    img->texture = tex;
    SDL_strlcpy(img->filepath, "(empty)", JOY_MAX_PATH);
    return img;
}

joy_image_p joy_r2d_image_create_from_data(joy_r2d_p r2d, const void* rgba_pixels, int w, int h)
{
    SDL_Surface* surf = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32,
        (void*)rgba_pixels, w * 4);
    joy_image_p img = image_create_from_surface(r2d, surf, "(data)");
    SDL_DestroySurface(surf);
    return img;
}

void joy_r2d_image_update(joy_image_p image, const joy_rect_t* rect, const void* pixels, int pitch)
{
    SDL_Rect r = { (int)rect->x, (int)rect->y, (int)rect->w, (int)rect->h };
    SDL_UpdateTexture(image->texture, &r, pixels, pitch);
}

void joy_r2d_image_set_alpha(joy_image_p image, uint8_t alpha)
{
    SDL_SetTextureAlphaMod(image->texture, alpha);
}

void joy_r2d_image_set_blend_mode(joy_image_p image, int mode)
{
    SDL_BlendMode m;
    switch (mode) {
    case 0: m = SDL_BLENDMODE_NONE; break;
    case 1: m = SDL_BLENDMODE_BLEND; break;
    case 2: m = SDL_BLENDMODE_ADD; break;
    case 3: m = SDL_BLENDMODE_MOD; break;
    default: m = SDL_BLENDMODE_BLEND; break;
    }
    SDL_SetTextureBlendMode(image->texture, m);
}

bool joy_r2d_image_get_size(joy_image_p image, float* w, float* h)
{
    if (!image || !image->texture) return false;
    float fw, fh;
    SDL_GetTextureSize(image->texture, &fw, &fh);
    if (w) *w = fw; if (h) *h = fh;
    return true;
}

joy_text_p joy_r2d_text_createx(joy_r2d_p r2d, joy_font_p font,
        const char* str, int len, joy_color_t color) {
        joy_text_p texture;
        int capacity_codepoints = len * 4;
        int* codepoints = (int*)SDL_malloc(sizeof(int) * capacity_codepoints);
        int num_codepoints = joy_core_utf8_decode_all(str, len, codepoints, capacity_codepoints);
        texture = joy_r2d_text_create(r2d, font, codepoints, num_codepoints, color);
        SDL_free(codepoints);
        return texture;
}

joy_text_p joy_r2d_text_create(joy_r2d_p r2d, joy_font_p font,
        const int* codepoints, int num_codepoints,
        joy_color_t color)
{
        int x, y, font_size, font_ascent;
        float scale;
        int bitmap_index, pixel_index;
        unsigned char* bitmap;
        int advance_width, left_side_bearing;
        int c_x1, c_y1, c_x2, c_y2;
        SDL_Surface* surface;
        unsigned char* surface_pixels;
        Uint8 pixel;
        joy_text_p text;
        int kern;

        x = 0;
        advance_width = left_side_bearing = 0;
        scale = joy_r2d_font_get_scale(font);
        font_size = joy_r2d_font_get_size(font);
        font_ascent = joy_r2d_font_get_ascent(font);

        text = (joy_text_p)SDL_malloc(sizeof(joy_text_t));
        if (!text) return NULL;

        /* 第一遍：测量真实绘制范围。
           原实现直接按 font_size*num_codepoints 估算位图，负的左边距（byte_offset<0）、
           超出字身的下行部（c_y2-c_y1 > font_size）以及最后一个字形越过 real_width
           的列，都会让 stbtt 的 blit 写穿 bitmap —— 即堆越界写。 */
        int min_x = 0, max_x = 0, min_y = 0, max_y = 0;
        int mx = 0;
        for (int i = 0; i < num_codepoints; ++i) {
                stbtt_GetCodepointHMetrics((stbtt_fontinfo*)font, codepoints[i], &advance_width, &left_side_bearing);
                stbtt_GetCodepointBitmapBox((stbtt_fontinfo*)font, codepoints[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
                int px = mx + (int)roundf(left_side_bearing * scale);
                int py = font_ascent + c_y1;
                if (px < min_x) min_x = px;
                if (px + (c_x2 - c_x1) > max_x) max_x = px + (c_x2 - c_x1);
                if (py < min_y) min_y = py;
                if (py + (c_y2 - c_y1) > max_y) max_y = py + (c_y2 - c_y1);
                mx += (int)roundf(advance_width * scale);
                if (i + 1 < num_codepoints) {
                        kern = stbtt_GetCodepointKernAdvance((stbtt_fontinfo*)font, codepoints[i], codepoints[i + 1]);
                        mx += (int)roundf(kern * scale);
                }
        }

        /* 平移原点，保证所有 blit 偏移非负；位图按真实范围分配
           （多出来的行/列不进纹理，可见输出与原来一致）。 */
        int ox = (min_x < 0) ? -min_x : 0;
        int oy = (min_y < 0) ? -min_y : 0;

        int stride = font_size * num_codepoints;   /* 旧估算，仅作下界 */
        if (stride < max_x + ox)     stride = max_x + ox;
        if (stride < mx)             stride = mx;
        if (stride < 1)              stride = 1;
        int bitmap_h = font_size;
        if (bitmap_h < max_y + oy)   bitmap_h = max_y + oy;
        if (bitmap_h < 1)            bitmap_h = 1;

        bitmap = SDL_calloc((size_t)stride * (size_t)bitmap_h + 1, sizeof(unsigned char));
        if (!bitmap) { SDL_free(text); return NULL; }

        /* 第二遍：绘制。所有写入都落在 [0, stride*bitmap_h) 内。 */
        x = 0;
        for (int i = 0; i < num_codepoints; ++i) {
                stbtt_GetCodepointHMetrics((stbtt_fontinfo*)font, codepoints[i], &advance_width, &left_side_bearing);
                stbtt_GetCodepointBitmapBox((stbtt_fontinfo*)font, codepoints[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
                int px = x + (int)roundf(left_side_bearing * scale);
                int py = font_ascent + c_y1;
                int gw = c_x2 - c_x1, gh = c_y2 - c_y1;
                int byte_offset = ox + px + (oy + py) * stride;
                if (gw > 0 && gh > 0
                    && byte_offset >= 0
                    && byte_offset + (gh - 1) * stride + gw <= stride * bitmap_h) {
                        stbtt_MakeCodepointBitmap((stbtt_fontinfo*)font, bitmap + byte_offset,
                                                  gw, gh, stride, scale, scale, codepoints[i]);
                }
                x += (int)roundf(advance_width * scale);
                if (i + 1 < num_codepoints) {
                        kern = stbtt_GetCodepointKernAdvance((stbtt_fontinfo*)font, codepoints[i], codepoints[i + 1]);
                        x += (int)roundf(kern * scale);
                }
        }

        text->real_height = font_size;
        int bitmap_width = stride;
        text->real_width = x > 0 ? x : 1;
        surface = SDL_CreateSurface(text->real_width, text->real_height, SDL_PIXELFORMAT_RGBA32);
        if (!surface) { SDL_free(bitmap); return NULL; }

        surface_pixels = (Uint8*)surface->pixels;
        int pitch = surface->pitch / sizeof(Uint8);

        for (y = 0; y < text->real_height; ++y) {
                for (x = 0; x < text->real_width; ++x) {
                        bitmap_index = y * bitmap_width + x;
                        pixel_index = y * (pitch / 4) + x;
                        pixel = bitmap[bitmap_index];
                        surface_pixels[pixel_index * 4 + 0] = color.r;
                        surface_pixels[pixel_index * 4 + 1] = color.g;
                        surface_pixels[pixel_index * 4 + 2] = color.b;
                        surface_pixels[pixel_index * 4 + 3] = pixel & color.a;
                }
        }
        text->texture = SDL_CreateTextureFromSurface(r2d->sdl, surface);
        SDL_free(bitmap);
        SDL_DestroySurface(surface);
        return text;
}

void joy_r2d_text_updatex(joy_text_p text, joy_font_p font,
        const char* str, int len, joy_color_t color) {
        joy_text_p texture;
        int capacity_codepoints = len * 4;
        int* codepoints = (int*)SDL_malloc(sizeof(int) * capacity_codepoints);
        int num_codepoints = joy_core_utf8_decode_all(str, len, codepoints, capacity_codepoints);
        joy_r2d_text_update(text, font, codepoints, num_codepoints, color);
        SDL_free(codepoints);
}

void joy_r2d_text_update(joy_text_p text, joy_font_p font,
        const int* codepoints, int num_codepoints,
        joy_color_t color)
{
        int x, y, font_size, font_ascent;
        float font_scale;
        float w, h;
        int bitmap_index, pixel_index;
        unsigned char* bitmap;
        int advance_width, left_side_bearing;
        int c_x1, c_y1, c_x2, c_y2;
        Uint8 pixel;
        int kern;
        unsigned char* pixels;
        SDL_Renderer *renderer;

        x = 0;
        advance_width = left_side_bearing = 0;
        font_scale = joy_r2d_font_get_scale(font);
        font_size = joy_r2d_font_get_size(font);
        font_ascent = joy_r2d_font_get_ascent(font);

        /* 第一遍：测量真实绘制范围（与 joy_r2d_text_create 同一套修复：
           避免负左边距 / 下行部 / 末字形越列导致 blit 写穿 bitmap）。 */
        int min_x = 0, max_x = 0, min_y = 0, max_y = 0;
        int mx = 0;
        for (int i = 0; i < num_codepoints; ++i) {
                stbtt_GetCodepointHMetrics((stbtt_fontinfo*)font, codepoints[i], &advance_width, &left_side_bearing);
                stbtt_GetCodepointBitmapBox((stbtt_fontinfo*)font, codepoints[i], font_scale, font_scale, &c_x1, &c_y1, &c_x2, &c_y2);
                int px = mx + (int)roundf(left_side_bearing * font_scale);
                int py = font_ascent + c_y1;
                if (px < min_x) min_x = px;
                if (px + (c_x2 - c_x1) > max_x) max_x = px + (c_x2 - c_x1);
                if (py < min_y) min_y = py;
                if (py + (c_y2 - c_y1) > max_y) max_y = py + (c_y2 - c_y1);
                mx += (int)roundf(advance_width * font_scale);
                if (i + 1 < num_codepoints) {
                        kern = stbtt_GetCodepointKernAdvance((stbtt_fontinfo*)font, codepoints[i], codepoints[i + 1]);
                        mx += (int)roundf(kern * font_scale);
                }
        }

        int ox = (min_x < 0) ? -min_x : 0;
        int oy = (min_y < 0) ? -min_y : 0;

        int stride = font_size * num_codepoints;   /* 旧估算，仅作下界 */
        if (stride < max_x + ox)     stride = max_x + ox;
        if (stride < mx)             stride = mx;
        if (stride < 1)              stride = 1;
        int bitmap_h = font_size;
        if (bitmap_h < max_y + oy)   bitmap_h = max_y + oy;
        if (bitmap_h < 1)            bitmap_h = 1;

        bitmap = (unsigned char*)SDL_calloc((size_t)stride * (size_t)bitmap_h + 1, sizeof(unsigned char));
        if (!bitmap) return;

        /* 第二遍：绘制。所有写入都落在 [0, stride*bitmap_h) 内。 */
        x = 0;
        for (int i = 0; i < num_codepoints; ++i) {
                stbtt_GetCodepointHMetrics((stbtt_fontinfo*)font, codepoints[i], &advance_width, &left_side_bearing);
                stbtt_GetCodepointBitmapBox((stbtt_fontinfo*)font, codepoints[i], font_scale, font_scale, &c_x1, &c_y1, &c_x2, &c_y2);
                int px = x + (int)roundf(left_side_bearing * font_scale);
                int py = font_ascent + c_y1;
                int gw = c_x2 - c_x1, gh = c_y2 - c_y1;
                int byteOffset = ox + px + (oy + py) * stride;
                if (gw > 0 && gh > 0
                    && byteOffset >= 0
                    && byteOffset + (gh - 1) * stride + gw <= stride * bitmap_h) {
                        stbtt_MakeCodepointBitmap((stbtt_fontinfo*)font, bitmap + byteOffset,
                                                  gw, gh, stride, font_scale, font_scale, codepoints[i]);
                }
                x += (int)roundf(advance_width * font_scale);
                if (i + 1 < num_codepoints) {
                        kern = stbtt_GetCodepointKernAdvance((stbtt_fontinfo*)font, codepoints[i], codepoints[i + 1]);
                        x += (int)roundf(kern * font_scale);
                }
        }

        text->real_height = font_size;
        int old_bitmap_width = stride;
        text->real_width = x > 0 ? x : 1;

        SDL_Surface* surface = SDL_CreateSurface(text->real_width, text->real_height, SDL_PIXELFORMAT_RGBA32);
        if (!surface) return;

        Uint8* surface_pixels = (Uint8*)surface->pixels;
        int pitch = surface->pitch / sizeof(Uint8);

        for (y = 0; y < text->real_height; ++y) {
                for (x = 0; x < text->real_width; ++x) {
                        bitmap_index = y * old_bitmap_width + x;
                        pixel_index = y * (pitch / 4) + x;
                        pixel = bitmap[bitmap_index];
                        surface_pixels[pixel_index * 4 + 0] = color.r;
                        surface_pixels[pixel_index * 4 + 1] = color.g;
                        surface_pixels[pixel_index * 4 + 2] = color.b;
                        surface_pixels[pixel_index * 4 + 3] = pixel & color.a;
                }
        }

        SDL_GetTextureSize(text->texture, &w, &h);

        if ((text->real_width > w || text->real_height > h) && false) {
                renderer = SDL_GetRendererFromTexture(text->texture);
                SDL_DestroyTexture(text->texture);
                text->texture = SDL_CreateTextureFromSurface(renderer, surface);
        }
        else {
                SDL_Rect rect;
                rect.x = 0;
                rect.y = 0;
                rect.w = text->real_width;
                rect.h = text->real_height;
                SDL_UpdateTexture(text->texture, &rect, surface_pixels, pitch);
        }

        SDL_free(bitmap);
        SDL_DestroySurface(surface);
}

void joy_r2d_text_destroy(joy_text_p text)
{
        if (!text) return;
        SDL_DestroyTexture(text->texture);
        SDL_free(text);
}

void joy_r2d_text_print(joy_r2d_p r2d,
        joy_text_p text,
        float x, float y)
{
        SDL_FRect raw_dest = { x, y, (float)text->real_width, (float)text->real_height };
        SDL_RenderTexture(r2d->sdl, text->texture, NULL, &raw_dest);
}

/* ===================== 比例坐标（0~1）绘制封装 =====================
 *
 * 全部建立在 joy_r2d_fpx / joy_r2d_frect 之上，语义与控件层的百分比参数一致。
 * 逻辑分辨率改变时，用这些接口绘制的内容自动等比适配，无需改任何常量。
 */
bool joy_r2d_draw_point_f(joy_r2d_p r2d, float fx, float fy)
{
        float x = 0.0f, y = 0.0f;
        joy_r2d_fpx(r2d, fx, fy, 0.0f, 0.0f, &x, &y, NULL, NULL);
        return joy_r2d_draw_point(r2d, x, y);
}

bool joy_r2d_shape_draw_rectangle_f(joy_r2d_p r2d, const char* mode, float fx, float fy, float fw, float fh)
{
        return joy_r2d_shape_draw_rectangle(r2d, mode, joy_r2d_frect(r2d, fx, fy, fw, fh));
}

bool joy_r2d_shape_draw_line_f(joy_r2d_p r2d, float fx1, float fy1, float fx2, float fy2)
{
        float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
        joy_r2d_fpx(r2d, fx1, fy1, 0.0f, 0.0f, &x1, &y1, NULL, NULL);
        joy_r2d_fpx(r2d, fx2, fy2, 0.0f, 0.0f, &x2, &y2, NULL, NULL);
        return joy_r2d_shape_draw_line(r2d, x1, y1, x2, y2);
}

void joy_r2d_image_draw1_f(joy_r2d_p r2d, joy_image_p image, const joy_rect_t* srcrect,
        float fx, float fy, float fw, float fh)
{
        joy_rect_t dest = joy_r2d_frect(r2d, fx, fy, fw, fh);
        joy_r2d_image_draw1(image, srcrect, &dest);
}

/* 生成链路专用薄包装：srcrect=NULL 表示整张图缩放到目标矩形。
 * 坐标是【像素】，不经过 joy_r2d_frect（那是归一化比例 → 乘窗口宽高），
 * 与 ShapeDrawRectangle / ShapeDrawLine 的像素语义一致。
 * 参数全是标量 + 强句柄，joy.idl 判为 L1，thunk / vtable / cs 可自动生成。 */
void joy_r2d_image_draw_rect(joy_r2d_p r2d, joy_image_p image, float x, float y, float w, float h)
{
        (void)r2d;
        joy_rect_t dest = { x, y, w, h };
        joy_r2d_image_draw1(image, NULL, &dest);
}

void joy_r2d_text_printf(joy_r2d_p r2d, joy_font_p font, const char* str, int len, joy_color_t color,
        float fx, float fy)
{
        float x = 0.0f, y = 0.0f;
        joy_r2d_fpx(r2d, fx, fy, 0.0f, 0.0f, &x, &y, NULL, NULL);
        joy_r2d_text_printx(r2d, font, str, len, color, x, y);
}

/* 位置为比例，且绘制尺寸 = 字号 × scale（用于需要按屏幕比例缩放字号的场合）。
 * 不走 joy_r2d_text_printx 的纹理缓存（缓存条目尺寸固定），这里按次创建再释放。 */
void joy_r2d_text_printf_scaled(joy_r2d_p r2d, joy_font_p font, const char* str, int len, joy_color_t color,
        float fx, float fy, float scale)
{
        joy_text_p tex = joy_r2d_text_createx(r2d, font, str, len, color);
        if (!tex) return;
        float x = 0.0f, y = 0.0f;
        joy_r2d_fpx(r2d, fx, fy, 0.0f, 0.0f, &x, &y, NULL, NULL);
        SDL_FRect dest = { x, y, (float)tex->real_width * scale, (float)tex->real_height * scale };
        SDL_RenderTexture(r2d->sdl, tex->texture, NULL, &dest);
        joy_r2d_text_destroy(tex);
}

/* ===================== 文字纹理缓存 =====================
 *
 * 旧实现里 joy_r2d_text_printx() 是 joy_r2d_text_createx() -> joy_r2d_text_print() -> joy_r2d_text_destroy()，
 * 每次调用都新建并销毁一张 SDL 纹理。inputbox / datagrid / popup / tooltip /
 * combobox_input 都走这条路径；其中输入框绘制还会为测量子串宽度额外
 * 创建两张纹理，一个带选区的 inputbox 每帧要建 3~4 张。
 *
 * 这里按 (font, color, 内容字节) 缓存纹理并 LRU 复用。
 * 注意只有 joy_r2d_text_printx 走缓存；joy_r2d_text_createx 仍由调用方自行持有与释放，
 * 因此 label / button 这类自己缓存 jtext 的控件不受影响。
 */

#define JTEXT_CACHE_BUCKETS       1024
#define JTEXT_CACHE_DEFAULT_LIMIT 900

typedef struct joy_text_entry {
        struct joy_text_entry* hash_next;
        struct joy_text_entry* lru_prev;
        struct joy_text_entry* lru_next;
        joy_r2d_p owner;        /* 纹理隶属于哪个 renderer；缓存键的一部分 */
        joy_font_p    font;
        joy_color_t  color;
        int       len;
        char*     str;          /* 内容拷贝，len 字节，额外补一个 '\0' 便于调试 */
        joy_text_p   tex;
        Uint64    last_frame;   /* 以 owner->frame 为时间基准 */
} joy_text_entry_t;

static joy_text_entry_t* g_buckets[JTEXT_CACHE_BUCKETS];
static joy_text_entry_t* g_lru_head;    /* 最近使用 */
static joy_text_entry_t* g_lru_tail;    /* 最久未用 */
static int            g_cache_count = 0;
static int            g_cache_limit = JTEXT_CACHE_DEFAULT_LIMIT;

static Uint64 jtext_hash(joy_r2d_p owner, joy_font_p font,
        joy_color_t c, const char* s, int len)
{
        Uint64 h = 1469598103934665603ULL;          /* FNV-1a 64 */
        const unsigned char* p;
        int i;
#define JTEXT_HASH_BYTES(pv, n) do {                        \
                p = (const unsigned char*)(pv);             \
                for (i = 0; i < (int)(n); i++) {            \
                        h ^= (Uint64)p[i];                  \
                        h *= 1099511628211ULL;              \
                }                                           \
        } while (0)
        /* owner 必须进键：SDL 纹理不可跨 renderer 使用，两个 renderer
         * 打印同一段文字也不能命中同一张纹理。 */
        JTEXT_HASH_BYTES(&owner, sizeof(owner));
        JTEXT_HASH_BYTES(&font, sizeof(font));
        JTEXT_HASH_BYTES(&c, sizeof(c));
        if (s && len > 0) JTEXT_HASH_BYTES(s, len);
#undef JTEXT_HASH_BYTES
        return h;
}

static void jtext_lru_unlink(joy_text_entry_t* e)
{
        if (e->lru_prev) e->lru_prev->lru_next = e->lru_next;
        else             g_lru_head = e->lru_next;
        if (e->lru_next) e->lru_next->lru_prev = e->lru_prev;
        else             g_lru_tail = e->lru_prev;
        e->lru_prev = NULL;
        e->lru_next = NULL;
}

static void jtext_lru_push_front(joy_text_entry_t* e)
{
        e->lru_prev = NULL;
        e->lru_next = g_lru_head;
        if (g_lru_head) g_lru_head->lru_prev = e;
        g_lru_head = e;
        if (!g_lru_tail) g_lru_tail = e;
}

static void jtext_entry_free(joy_text_entry_t* e)
{
        if (!e) return;
        if (e->tex) joy_r2d_text_destroy(e->tex);
        if (e->str) SDL_free(e->str);
        SDL_free(e);
}

/* 摘除并释放指定条目（可从哈希桶与 LRU 链的任意位置移除） */
static void jtext_entry_remove(joy_text_entry_t* e)
{
        joy_text_entry_t** pp;
        Uint64 bucket;

        if (!e) return;
        bucket = jtext_hash(e->owner, e->font, e->color, e->str, e->len)
                 % JTEXT_CACHE_BUCKETS;
        pp = &g_buckets[bucket];
        while (*pp) {
                if (*pp == e) { *pp = e->hash_next; break; }
                pp = &(*pp)->hash_next;
        }
        jtext_lru_unlink(e);
        jtext_entry_free(e);
        g_cache_count--;
}

/* 淘汰最久未使用的条目 */
static void jtext_evict_one(void)
{
        jtext_entry_remove(g_lru_tail);
}

/* 只清除属于指定 renderer 的条目。destroy 后 SDL_Renderer 已失效，
 * 这些纹理必须随之释放；其它 renderer 的条目保持不动。 */
static void jtext_cache_clear_for(joy_r2d_p r2d)
{
        joy_text_entry_t* e = g_lru_head;
        while (e) {
                joy_text_entry_t* next = e->lru_next;
                if (e->owner == r2d) jtext_entry_remove(e);
                e = next;
        }
}

/* 内部: 只清掉用该字体烘焙的条目, 供 jfont.c 销毁字体时调用。
 * 全局 clear 会连带销毁其它存活 renderer 的文字纹理。 */
void joy_r2d_text_cache_clear_for_font(joy_font_p font)
{
        joy_text_entry_t* e = g_lru_head;
        while (e) {
                joy_text_entry_t* next = e->lru_next;
                if (e->font == font) jtext_entry_remove(e);
                e = next;
        }
}

static joy_text_p jtext_cache_get(joy_r2d_p r2d, joy_font_p font,
        const char* str, int len, joy_color_t color)
{
        joy_text_entry_t* e;
        joy_text_p tex;
        Uint64 bucket;

        if (!r2d || !font || !str || len <= 0) return NULL;

        bucket = jtext_hash(r2d, font, color, str, len) % JTEXT_CACHE_BUCKETS;

        for (e = g_buckets[bucket]; e; e = e->hash_next) {
                if (e->owner == r2d && e->font == font && e->len == len
                        && e->color.r == color.r && e->color.g == color.g
                        && e->color.b == color.b && e->color.a == color.a
                        && SDL_memcmp(e->str, str, (size_t)len) == 0) {
                        e->last_frame = r2d->frame;
                        jtext_lru_unlink(e);
                        jtext_lru_push_front(e);
                        return e->tex;
                }
        }

        tex = joy_r2d_text_createx(r2d, font, str, len, color);
        if (!tex) return NULL;

        while (g_cache_count >= g_cache_limit) jtext_evict_one();

        e = (joy_text_entry_t*)SDL_calloc(1, sizeof(joy_text_entry_t));
        if (!e) { joy_r2d_text_destroy(tex); return NULL; }
        e->str = (char*)SDL_malloc((size_t)len + 1);
        if (!e->str) { SDL_free(e); joy_r2d_text_destroy(tex); return NULL; }
        SDL_memcpy(e->str, str, (size_t)len);
        e->str[len] = '\0';

        e->owner      = r2d;
        e->font       = font;
        e->color      = color;
        e->len        = len;
        e->tex        = tex;
        e->last_frame = r2d->frame;

        e->hash_next = g_buckets[bucket];
        g_buckets[bucket] = e;
        jtext_lru_push_front(e);
        g_cache_count++;
        return e->tex;
}

void joy_r2d_text_cache_clear(void)
{
        while (g_lru_head) jtext_evict_one();
}

void joy_r2d_text_cache_set_limit(int max_entries)
{
        if (max_entries < 16) max_entries = 16;
        g_cache_limit = max_entries;
        while (g_cache_count > g_cache_limit) jtext_evict_one();
}

int joy_r2d_text_cache_get_count(void)
{
        return g_cache_count;
}

/* 回收空闲超过 max_idle_frames 帧的条目（单位：帧）。
 * 每个条目按自己 owner 的帧时钟判断，多实例互不干扰。 */
void joy_r2d_text_cache_sweep(int max_idle_frames)
{
        if (max_idle_frames < 0) return;
        while (g_lru_tail
               && (g_lru_tail->owner->frame - g_lru_tail->last_frame)
                  > (Uint64)max_idle_frames) {
                jtext_evict_one();
        }
}

void joy_r2d_text_printx(joy_r2d_p r2d, joy_font_p font,
        const char* str, int len, joy_color_t color, float x, float y)
{
        joy_text_p t = jtext_cache_get(r2d, font, str, len, color);
        if (t) {
                joy_r2d_text_print(r2d, t, x, y);
        }
}

bool joy_r2d_text_measurex(joy_r2d_p r2d, joy_font_p font, const char* str, int len,
        joy_color_t color, int* out_w, int* out_h)
{
        joy_text_p t;
        if (out_w) *out_w = 0;
        if (out_h) *out_h = 0;
        if (!r2d || !font || !str || len <= 0) return false;

        /* 走同一套缓存：命中时不产生任何纹理创建开销 */
        t = jtext_cache_get(r2d, font, str, len, color);
        if (!t) return false;

        if (out_w) *out_w = joy_r2d_text_get_width(t);
        if (out_h) *out_h = joy_r2d_text_get_height(t);
        return true;
}

int joy_r2d_text_get_width(joy_text_p jtext_tex)
{
        return jtext_tex->real_width;
}

int joy_r2d_text_get_height(joy_text_p jtext_tex)
{
        return jtext_tex->real_height;
}


joy_animation_p joy_r2d_animation_create(joy_r2d_p r2d)
{
        joy_animation_p animation;
        animation = (joy_animation_p)SDL_malloc(sizeof(joy_animation_t));
        animation->r2d = r2d;
        animation->is_loop = true;
        kv_init(animation->clips);
        animation->position = (joy_vec2_t){ 0, 0 };
        animation->rotation = 0;
        animation->scale = (joy_vec2_t){ 1.0f, 1.0f };
        animation->origin = (joy_vec2_t){ 0.0f, 0.0f };
        animation->total_time = animation->last_time = animation->idx_frame = 0;
        return animation;
}

void joy_r2d_animation_destroy(joy_animation_p animation)
{
        kv_destroy(animation->clips);
        SDL_free(animation);
}

void joy_r2d_animation_reset(joy_animation_p animation)
{
        uint64_t ticks;
        animation->idx_frame = 0;
        animation->total_time = 0;
        if (joy_core_utils_get_current_time(&ticks)) {
                animation->last_time = ticks;
        }
        else {
                animation->last_time = 0;
        }
}

void joy_r2d_animation_add_clip(joy_animation_p animation,
        const char* image_path, float duration,
        joy_rect_t src_rect)
{
        joy_animation_clip_t animation_clip;
        animation_clip.image = joy_r2d_image_create(animation->r2d, image_path);
        animation_clip.duration = duration;
        animation_clip.src_rect = src_rect;
        kv_push(joy_animation_clip_t, animation->clips, animation_clip);
}

bool joy_r2d_animation_is_finished(joy_animation_p animation)
{
        if (animation->is_loop) {
                return false;
        }
        else {
                return (animation->idx_frame == kv_size(animation->clips));
        }
}

void joy_r2d_animation_set_position(joy_animation_p animation, float x, float y)
{
        animation->position.x = x;
        animation->position.y = y;
}

void joy_r2d_animation_set_scale(joy_animation_p animation, float x, float y)
{
        animation->scale.x = x;
        animation->scale.y = y;
}

void joy_r2d_animation_set_rotation(joy_animation_p animation, float rotation)
{
        animation->rotation = rotation;
}

void joy_r2d_animation_update(joy_animation_p animation, float dt)
{
        joy_animation_clip_p animation_clip;
        if (kv_size(animation->clips) <= 0) {
                return;
        }

        animation_clip = &kv_A(animation->clips, animation->idx_frame);
        animation->total_time += dt;
        if (animation->total_time >= animation_clip->duration) {
                if (animation->idx_frame >= kv_size(animation->clips) - 1) {
                        if (animation->is_loop)
                                animation->idx_frame = 0;
                        else
                                animation->idx_frame = kv_size(animation->clips) - 1;
                }
                else {
                        animation->idx_frame += 1;
                }
                animation->total_time -= animation_clip->duration;
        }
}

void joy_r2d_animation_draw(joy_animation_p animation, joy_rect_t* camera)
{
        joy_animation_clip_p animation_clip;
        float r_x, r_y;
        joy_rect_t dest_rect;
        joy_vec2_t center = { 0 };

        if (kv_size(animation->clips) <= 0) {
                return;
        }

        animation_clip = &kv_A(animation->clips, animation->idx_frame);
        joy_r2d_image_draw_ex(animation_clip->image, &animation_clip->src_rect, animation->position, animation->rotation, animation->scale, animation->origin);
}

/* ===== 裁剪 ===== */

/* 把当前裁剪矩形下发给 SDL。注意 joy_rect_t 是浮点、SDL_Rect 是整数，
 * 这里向下取整左上角、向上取整尺寸，避免裁剪区比预期小一圈。 */
static void r2d_apply_clip(joy_r2d_p r2d)
{
        SDL_Rect cr;
        if (!r2d) return;
        if (!r2d->clip_enabled) {
                SDL_SetRenderClipRect(r2d->sdl, NULL);
                return;
        }
        cr.x = (int)floorf(r2d->clip_rect.x);
        cr.y = (int)floorf(r2d->clip_rect.y);
        cr.w = (int)ceilf(r2d->clip_rect.w);
        cr.h = (int)ceilf(r2d->clip_rect.h);
        if (cr.w <= 0 || cr.h <= 0) {
                /* 退化成空矩形：仍然设裁剪，效果是全部不绘制 */
                cr.w = 0; cr.h = 0;
        }
        SDL_SetRenderClipRect(r2d->sdl, &cr);
}

bool joy_r2d_set_clip_rect(joy_r2d_p r2d, joy_rect_t rect)
{
        if (!r2d) return false;
        r2d->clip_enabled = true;
        r2d->clip_rect = rect;
        if (rect.w < 0) r2d->clip_rect.w = 0;
        if (rect.h < 0) r2d->clip_rect.h = 0;
        r2d_apply_clip(r2d);
        return true;
}

bool joy_r2d_get_clip_rect(joy_r2d_p r2d, joy_rect_t* out_rect)
{
        if (!r2d) return false;
        if (out_rect && r2d->clip_enabled) *out_rect = r2d->clip_rect;
        return r2d->clip_enabled;
}

void joy_r2d_clear_clip(joy_r2d_p r2d)
{
        if (!r2d) return;
        r2d->clip_enabled = false;
        r2d->clip_rect = (joy_rect_t){ 0, 0, 0, 0 };
        r2d_apply_clip(r2d);
}

bool joy_r2d_push_clip_rect(joy_r2d_p r2d, joy_rect_t rect)
{
        joy_rect_t out;
        if (!r2d) return false;
        if (r2d->clip_top >= R2D_CLIP_STACK_MAX) {
                SDL_LogError(SDL_LOG_CATEGORY_RENDER,
                        "joy_r2d_push_clip_rect: stack overflow (max %d)", R2D_CLIP_STACK_MAX);
                return false;
        }

        /* 保存上一层状态 */
        r2d->stack_enabled[r2d->clip_top] = r2d->clip_enabled;
        r2d->stack_rect[r2d->clip_top]    = r2d->clip_rect;
        r2d->clip_top++;

        out = rect;
        if (out.w < 0) out.w = 0;
        if (out.h < 0) out.h = 0;

        /* 与父级求交，保证嵌套时不会越出外层 */
        if (r2d->clip_enabled) {
                float x0 = rect.x;
                float y0 = rect.y;
                float x1 = rect.x + out.w;
                float y1 = rect.y + out.h;
                float px0 = r2d->clip_rect.x;
                float py0 = r2d->clip_rect.y;
                float px1 = r2d->clip_rect.x + r2d->clip_rect.w;
                float py1 = r2d->clip_rect.y + r2d->clip_rect.h;

                if (x0 < px0) x0 = px0;
                if (y0 < py0) y0 = py0;
                if (x1 > px1) x1 = px1;
                if (y1 > py1) y1 = py1;

                out.x = x0;
                out.y = y0;
                out.w = x1 - x0;
                out.h = y1 - y0;
                if (out.w < 0) out.w = 0;
                if (out.h < 0) out.h = 0;
        }

        r2d->clip_enabled = true;
        r2d->clip_rect = out;
        r2d_apply_clip(r2d);
        return true;
}

void joy_r2d_pop_clip_rect(joy_r2d_p r2d)
{
        if (!r2d || r2d->clip_top <= 0) return;
        r2d->clip_top--;
        r2d->clip_enabled = r2d->stack_enabled[r2d->clip_top];
        r2d->clip_rect    = r2d->stack_rect[r2d->clip_top];
        r2d_apply_clip(r2d);
}

void joy_r2d_reset_clip_stack(joy_r2d_p r2d)
{
        if (!r2d) return;
        r2d->clip_top = 0;
        r2d->clip_enabled = false;
        r2d->clip_rect = (joy_rect_t){ 0, 0, 0, 0 };
        r2d_apply_clip(r2d);
}

bool joy_r2d_point_in_clip(joy_r2d_p r2d, float x, float y)
{
        if (!r2d) return true;              /* 无渲染器时不拦截 */
        if (!r2d->clip_enabled) return true;
        return x >= r2d->clip_rect.x
            && x <  r2d->clip_rect.x + r2d->clip_rect.w
            && y >= r2d->clip_rect.y
            && y <  r2d->clip_rect.y + r2d->clip_rect.h;
}

/* ===== Blend mode ===== */
bool joy_r2d_set_blend_mode(joy_r2d_p r2d, int mode)
{
        SDL_BlendMode sdl_mode = SDL_BLENDMODE_NONE;
        switch (mode) {
        case JR2D_BLEND_BLEND: sdl_mode = SDL_BLENDMODE_BLEND; break;
        case JR2D_BLEND_ADD:   sdl_mode = SDL_BLENDMODE_ADD;   break;
        case JR2D_BLEND_MOD:   sdl_mode = SDL_BLENDMODE_MOD;   break;
        default: break;
        }
        return SDL_SetRenderDrawBlendMode(r2d->sdl, sdl_mode);
}

/* ===== 低级纹理操作 ===== */
void* joy_r2d_create_texture_from_surface(joy_r2d_p r2d, void* surface)
{
        return (void*)SDL_CreateTextureFromSurface(r2d->sdl, (SDL_Surface*)surface);
}

void joy_r2d_destroy_texture(void* texture)
{
        if (texture) SDL_DestroyTexture((SDL_Texture*)texture);
}

void joy_r2d_set_texture_alpha(void* texture, uint8_t alpha)
{
        SDL_SetTextureAlphaMod((SDL_Texture*)texture, alpha);
}

void joy_r2d_set_texture_blend_mode(void* texture, int mode)
{
        SDL_BlendMode sdl_mode = SDL_BLENDMODE_NONE;
        switch (mode) {
        case JR2D_BLEND_BLEND: sdl_mode = SDL_BLENDMODE_BLEND; break;
        case JR2D_BLEND_ADD:   sdl_mode = SDL_BLENDMODE_ADD;   break;
        case JR2D_BLEND_MOD:   sdl_mode = SDL_BLENDMODE_MOD;   break;
        default: break;
        }
        SDL_SetTextureBlendMode((SDL_Texture*)texture, sdl_mode);
}

bool joy_r2d_get_texture_size(void* texture, float* w, float* h)
{
        return SDL_GetTextureSize((SDL_Texture*)texture, w, h);
}

bool joy_r2d_render_texture(joy_r2d_p r2d, void* texture, const joy_rect_t* src, const joy_rect_t* dst)
{
        SDL_Texture* tex = (SDL_Texture*)texture;
        const SDL_FRect* s = src ? (const SDL_FRect*)src : NULL;
        const SDL_FRect* d = dst ? (const SDL_FRect*)dst : NULL;
        return SDL_RenderTexture(r2d->sdl, tex, s, d);
}

bool joy_r2d_render_texture_rotated(joy_r2d_p r2d, void* texture, const joy_rect_t* src, const joy_rect_t* dst, double angle, int flip)
{
        SDL_FlipMode f = SDL_FLIP_NONE;
        if (flip == JR2D_FLIP_H)  f = SDL_FLIP_HORIZONTAL;
        if (flip == JR2D_FLIP_V)  f = SDL_FLIP_VERTICAL;
        if (flip == JR2D_FLIP_HV) f = (SDL_FlipMode)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
        return SDL_RenderTextureRotated(r2d->sdl, (SDL_Texture*)texture,
                (const SDL_FRect*)src, (const SDL_FRect*)dst, angle, NULL, f);
}

void* joy_r2d_create_texture(joy_r2d_p r2d, int w, int h)
{
        return (void*)SDL_CreateTexture(r2d->sdl, SDL_PIXELFORMAT_RGBA8888,
                SDL_TEXTUREACCESS_STREAMING, w, h);
}

void joy_r2d_update_texture(void* texture, const joy_rect_t* rect, const void* pixels, int pitch)
{
        SDL_UpdateTexture((SDL_Texture*)texture,
                (const SDL_Rect*)rect, pixels, pitch);
}

bool joy_r2d_render_geometry(joy_r2d_p r2d, void* texture,
                const float* vert_data, int vertex_count,
                const int* indices, int index_count)
{
        /* vert_data: 每顶点 8 个 float (pos.x, pos.y, color.r, color.g, color.b, color.a, tex.x, tex.y) */
        if (vertex_count <= 0 || index_count <= 0) return false;
        SDL_Vertex* verts = (SDL_Vertex*)SDL_malloc(sizeof(SDL_Vertex) * vertex_count);
        if (!verts) return false;
        for (int i = 0; i < vertex_count; i++) {
                const float* v = vert_data + i * 8;
                verts[i].position.x = v[0];
                verts[i].position.y = v[1];
                verts[i].color.r = v[2];
                verts[i].color.g = v[3];
                verts[i].color.b = v[4];
                verts[i].color.a = v[5];
                verts[i].tex_coord.x = v[6];
                verts[i].tex_coord.y = v[7];
        }
        bool ok = SDL_RenderGeometry(r2d->sdl, (SDL_Texture*)texture,
                verts, vertex_count, indices, index_count);
        SDL_free(verts);
        return ok;
}

/* =====================================================================
 * 2D FOV 视野(战争迷雾): 复用 3D 的逐角射线遮挡 + 渐变纹理羽化软边。
 * 在 2D 像素平面上构造 A/B/C 三环径向多边形, 由渐变纹理驱动 alpha，
 * 使可见边界在 feather 宽度内由透明渐变到全浓，消除硬边/锯齿。
 * ===================================================================== */

#define JOY_FOV_2D_DEG2RAD 0.0174532925f

/* 一次性渐变纹理: v=0 透明 → v=1 全浓(墨雾 alpha 205)。
 * 按 renderer 实例惰性创建并缓存，由 joy_r2d_destroy 释放。 */
static SDL_Texture* joy_r2d_fov_gradient_tex_2d(joy_r2d_p r)
{
    if (!r) return NULL;
    if (r->fov_gradient_tex) return r->fov_gradient_tex;
    const int H = 256;
    SDL_Surface* surf = SDL_CreateSurface(1, H, SDL_PIXELFORMAT_RGBA32);
    if (!surf) return NULL;
    unsigned char* px = (unsigned char*)surf->pixels;
    for (int y = 0; y < H; y++) {
        /* t=0 为视野边界(透明), t=1 为描边外沿。用 smoothstep 3t²-2t³ 让细墨线
           两缘柔和；alpha 峰值刻意压低调淡，只做范围标记、不遮挡场景。 */
        float t = (float)y / (float)(H - 1);
        float s = t * t * (3.0f - 2.0f * t);
        uint8_t a = (uint8_t)(100.0f * s + 0.5f);       // 淡墨标记线
        px[(size_t)y * surf->pitch + 0] = 30;           // 淡墨色
        px[(size_t)y * surf->pitch + 1] = 32;
        px[(size_t)y * surf->pitch + 2] = 38;
        px[(size_t)y * surf->pitch + 3] = a;
    }
    r->fov_gradient_tex = SDL_CreateTextureFromSurface(r->sdl, surf);
    SDL_DestroySurface(surf);
    if (r->fov_gradient_tex) SDL_SetTextureBlendMode(r->fov_gradient_tex, SDL_BLENDMODE_BLEND);
    return r->fov_gradient_tex;
}

/* px/py: 观察点在输出像素坐标; radii[seg] 每方向可见半径(像素);
   feather/outer_radius 亦为像素。需在 joy_r2d_begin/joy_r2d_end 与地图绘制的同一
   坐标系下调用(相机相对像素)。 */
void joy_r2d_fov_draw_2d(joy_r2d_p r, float px, float py,
                     const float* radii, int seg,
                     float feather, float outer_radius)
{
    if (!r || !radii || seg <= 0) return;
    (void)outer_radius;   /* 现在只画视野范围描边，不再向远处做遮蔽填充 */

    const float STEP = 360.0f / seg;
    const int tri_count = seg * 2;                     /* 每段 2 三角（只画羽化描边带） */
    const int vcount = seg * 4;                        /* 每段 4 顶点 */
    float* vert = (float*)SDL_malloc((size_t)vcount * 8 * sizeof(float));
    int*   idx  = (int*)SDL_malloc((size_t)tri_count * 3 * sizeof(int));
    if (!vert || !idx) { SDL_free(vert); SDL_free(idx); return; }

    int vi = 0, ii = 0;
    for (int i = 0; i < seg; i++) {
        int ni = (i + 1) % seg;
        float a0 = STEP * i  * JOY_FOV_2D_DEG2RAD;
        float a1 = STEP * ni * JOY_FOV_2D_DEG2RAD;
        float s0x = sinf(a0), c0y = cosf(a0);
        float s1x = sinf(a1), c1y = cosf(a1);
        float rA0 = radii[i],             rB0 = radii[i] + feather;
        float rA1 = radii[ni],            rB1 = radii[ni] + feather;
        /* A: 视野可见边界(uv.y=0 透明), B: 边界外羽化带外沿(uv.y=1 全浓)。
           B→外不再填充——视野只是一个"范围/方向描边"标记，不遮挡场景。 */
        float a0x = px + s0x*rA0, a0y = py + c0y*rA0;
        float b0x = px + s0x*rB0, b0y = py + c0y*rB0;
        float a1x = px + s1x*rA1, a1y = py + c1y*rA1;
        float b1x = px + s1x*rB1, b1y = py + c1y*rB1;

        /* 顶点色全白(opaque)，alpha 由纹理渐变决定 */
        int base = vi;
        float* v = vert + (size_t)vi * 8;
#define PUT_VERT(xx, yy, uu) do { \
            v[0] = (xx); v[1] = (yy); v[2] = 1.f; v[3] = 1.f; v[4] = 1.f; v[5] = 1.f; \
            v[6] = (uu); v[7] = 1.f; v += 8; \
        } while (0)
        PUT_VERT(a1x, a1y, 0.0f);   /* A1 (vi+0) */
        PUT_VERT(a0x, a0y, 0.0f);   /* A0 (vi+1) */
        PUT_VERT(b1x, b1y, 1.0f);   /* B1 (vi+2) */
        PUT_VERT(b0x, b0y, 1.0f);   /* B0 (vi+3) */
#undef PUT_VERT

        /* A→B 羽化描边带：视野边界一圈柔和的"范围/方向轮廓" */
        idx[ii++] = base + 0; idx[ii++] = base + 1; idx[ii++] = base + 2;
        idx[ii++] = base + 1; idx[ii++] = base + 3; idx[ii++] = base + 2;

        vi += 4;
    }

    SDL_Texture* tex = joy_r2d_fov_gradient_tex_2d(r);
    if (tex)
        joy_r2d_render_geometry(r, tex, vert, vcount, idx, tri_count * 3);
    else
        joy_r2d_render_geometry(r, NULL, vert, vcount, idx, tri_count * 3);

    SDL_free(vert);
    SDL_free(idx);
}

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef R2D_CLIP_STACK_MAX
#undef JTEXT_CACHE_BUCKETS
#undef JTEXT_CACHE_DEFAULT_LIMIT
#undef JTEXT_HASH_BYTES
#undef JOY_FOV_2D_DEG2RAD
#undef PUT_VERT

/* ==================== 结束 src/jr2d.c ==================== */

/* ==================== 合并自 src/jfont.c ==================== */

struct joy_font {
	stbtt_fontinfo fontinfo;
	unsigned char* fontdata;
	int fontsize;
	float scale;
	int ascent;
	int descent;
	int line_gap;
};

joy_font_p joy_r2d_font_create(const char* filename, int fontsize)
{
	size_t data_size;
	unsigned char* fontdata;
	joy_font_p font;

	fontdata = SDL_LoadFile(filename, &data_size);
	if (!fontdata) {
		joy_core_log_info("Failed to load font file: %s", filename);
		return NULL;
	}

	font = (joy_font_p)SDL_calloc(1, sizeof(joy_font_t));
	if (!font) {
		SDL_free(fontdata);
		return NULL;
	}

	if (!stbtt_InitFont(&font->fontinfo, fontdata, 0)) {
		joy_core_log_info("Failed to initialize font");
		SDL_free(fontdata);
		SDL_free(font);
		return NULL;
	}

	font->fontdata = fontdata;
	font->fontsize = fontsize;

	stbtt_GetFontVMetrics(&font->fontinfo, &font->ascent, &font->descent, &font->line_gap);
	font->scale = stbtt_ScaleForPixelHeight(&font->fontinfo, fontsize);
	font->ascent = roundf(font->ascent * font->scale);
	font->descent = roundf(font->descent * font->scale);

	return font;
}

void joy_r2d_font_destroy(joy_font_p font)
{
	if (!font) return;

	/* 文字缓存以 font 指针作为键的一部分。字体释放后该指针失效，
	 * 若新字体恰好分配到同一地址，缓存会命中用旧字体烘焙的纹理，
	 * 渲染出错误的字形。只清掉该字体的条目即可：整体 clear 会连带
	 * 销毁其它仍存活 renderer 的文字纹理。 */
	joy_r2d_text_cache_clear_for_font(font);

	if (font->fontdata) SDL_free(font->fontdata);
	SDL_free(font);
}

float joy_r2d_font_get_scale(joy_font_p font)
{
	return font->scale;
}

int joy_r2d_font_get_size(joy_font_p font)
{
	return font->fontsize;
}

int joy_r2d_font_get_ascent(joy_font_p font)
{
	return font->ascent;
}

/* ==================== 结束 src/jfont.c ==================== */

/* ==================== 合并自 src/jparticle.c ==================== */
/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jparticle.c
Description: 2D 粒子系统实现
Author: ydlc
Version: 1.0
Date: 2024.7.15
History:
*************************************************/

/* ======================== 内部粒子结构 ======================== */
typedef struct {
    float x, y;
    float vx, vy;
    float life;       /* 剩余生命 */
    float max_life;   /* 初始生命(用于插值) */
    float size;
    float r, g, b, a;
} joy_particle_inner_t;

/* 发射器 */
struct joy_particle {
        joy_particle_cfg_t cfg;
        float emit_acc;         /* 持续发射累积时间 */
        kvec_t(joy_particle_inner_t) particles;
};

/* ======================== 工具函数 ======================== */
static float randf(float min, float max) {
    return min + (float)rand() / (float)RAND_MAX * (max - min);
}

static float deg2rad(float deg) {
    return deg * (float)JOY_PI / 180.0f;
}

/* ======================== 默认配置 ======================== */
joy_particle_cfg_t joy_r2d_particle_cfg_default(void) {
    joy_particle_cfg_t cfg;
    cfg.x = 0; cfg.y = 0;
    cfg.spawn_radius = 0;
    cfg.direction = -90;   /* 默认向上 */
    cfg.spread = 30;
    cfg.speed_min = 50;  cfg.speed_max = 100;
    cfg.lifetime_min = 0.5f; cfg.lifetime_max = 1.5f;
    cfg.size_start = 4; cfg.size_end = 1;
    cfg.r = 255; cfg.g = 255; cfg.b = 255; cfg.a = 255;
    cfg.fadeout = 0.8f;
    cfg.burst = 20;
    cfg.rate = 0;
    cfg.gravity = false;
    cfg.gravity_y = 50.0f;
    return cfg;
}

/* ======================== 创建/销毁 ======================== */
joy_particle_p joy_r2d_particle_create(joy_particle_cfg_t cfg) {
    joy_particle_p p = (joy_particle_p)malloc(sizeof(joy_particle_t));
    if (!p) return NULL;
    p->cfg = cfg;
    p->emit_acc = 0;
    kv_init(p->particles);
    return p;
}

void joy_r2d_particle_destroy(joy_particle_p p) {
    if (!p) return;
    kv_destroy(p->particles);
    free(p);
}

/* ======================== 控制 ======================== */
void joy_r2d_particle_set_position(joy_particle_p p, float x, float y) {
    if (!p) return;
    p->cfg.x = x;
    p->cfg.y = y;
}

void joy_r2d_particle_clear(joy_particle_p p) {
    if (!p) return;
    p->particles.n = 0;
    p->emit_acc = 0;
}

/* ======================== 发射粒子 ======================== */
void joy_r2d_particle_emit(joy_particle_p p) {
    if (!p) return;
    joy_particle_cfg_t* c = &p->cfg;
    int n = c->burst > 0 ? c->burst : 1;
    for (int i = 0; i < n; i++) {
        joy_particle_inner_t pt = {0};
        /* 位置 */
        float angle = randf(0, (float)JOY_PI * 2);
        float r = randf(0, c->spawn_radius);
        pt.x = c->x + cosf(angle) * r;
        pt.y = c->y + sinf(angle) * r;
        /* 速度方向 */
        float dir = deg2rad(c->direction + randf(-c->spread, c->spread));
        float speed = randf(c->speed_min, c->speed_max);
        pt.vx = cosf(dir) * speed;
        pt.vy = sinf(dir) * speed;
        /* 生命周期 */
        pt.max_life = randf(c->lifetime_min, c->lifetime_max);
        pt.life = pt.max_life;
        /* 大小 */
        pt.size = randf(c->size_start, c->size_end > c->size_start
                        ? c->size_end : c->size_start);
        /* 颜色 */
        pt.r = c->r;
        pt.g = c->g;
        pt.b = c->b;
        pt.a = c->a;
        kv_push(joy_particle_inner_t, p->particles, pt);
    }
}

/* ======================== 更新 ======================== */
void joy_r2d_particle_update(joy_particle_p p, float dt) {
    if (!p) return;
    joy_particle_cfg_t* c = &p->cfg;

    /* 持续发射 */
    if (c->rate > 0) {
        p->emit_acc += dt;
        float interval = 1.0f / c->rate;
        while (p->emit_acc >= interval) {
            p->emit_acc -= interval;
            joy_r2d_particle_emit(p);
        }
    }

    /* 更新每个粒子 */
    size_t w = 0;
    for (size_t i = 0; i < p->particles.n; i++) {
        joy_particle_inner_t* pt = &kv_A(p->particles, i);
        pt->life -= dt;
        if (pt->life <= 0) continue; /* 死亡 */

        /* 移动 */
        pt->x += pt->vx * dt;
        pt->y += pt->vy * dt;

        /* 重力 */
        if (c->gravity) pt->vy += c->gravity_y * dt;

        /* 淡出 alpha */
        float life_ratio = pt->life / pt->max_life;
        if (life_ratio < c->fadeout) {
            pt->a = c->a * (life_ratio / c->fadeout);
        }

        /* 缩小 */
        float t = 1.0f - life_ratio;
        pt->size = c->size_start + (c->size_end - c->size_start) * t;

        /* 保留 */
        if (w != i) kv_A(p->particles, w) = kv_A(p->particles, i);
        w++;
    }
    p->particles.n = w;
}

/* ======================== 渲染 ======================== */
void joy_r2d_particle_render(joy_particle_p p, joy_r2d_p r2d) {
    if (!p || !r2d) return;
    for (size_t i = 0; i < p->particles.n; i++) {
        joy_particle_inner_t* pt = &kv_A(p->particles, i);
        float half = pt->size * 0.5f;
        joy_rect_t r = { pt->x - half, pt->y - half, pt->size, pt->size };
        joy_r2d_set_color(r2d, (uint8_t)pt->r, (uint8_t)pt->g, (uint8_t)pt->b, (uint8_t)pt->a);
        joy_r2d_shape_draw_rectangle(r2d, "fill", r);
    }
}

/* ======================== 查询 ======================== */
int joy_r2d_particle_count(joy_particle_p p) {
    return p ? (int)p->particles.n : 0;
}

bool joy_r2d_particle_is_active(joy_particle_p p) {
    return p && p->particles.n > 0;
}

/* ==================== 结束 src/jparticle.c ==================== */

/* ==================== 合并自 src/jspine.c ==================== */
/******************************************************************************
 * jspine —— joy 的 Spine 骨骼动画模块（无条件编译，spine-c 由 joy::all 链接）
 *
 * 渲染实现基于 Spine 官方 spine-sdl 封装（spine-runtimes 4.1，spine-sdl/src/spine-sdl-c.c），
 * 移植到 SDL3（SDL_Vertex.color 在 SDL3 是 SDL_FColor）并接入 joy 的 jr2d。
 *
 * 面向 ECS 的两层结构：
 *   joy_spine_asset_t    共享资源（atlas + skeletonData + animationStateData），多实体复用
 *   joy_spine_instance_t 组件：每实体一份，纯数据，可直接 emplace 进 ECS（entt 等）
 *****************************************************************************/



/* ==================== 内部类型 ==================== */

struct joy_spine_asset {
        spAtlas *atlas;
        spSkeletonData *data;
        spAnimationStateData *state_data; /* 共享的 mix 配置 */
        float scale;
};

/* 顶点缓存（组件里以 void* 持有，避免 joy.h 暴露 SDL 类型） */
typedef struct {
        SDL_Vertex *items;
        int size;
        int capacity;
} joy_spine_vertex_array_t;

typedef struct joy_spine_draw_cache {
        joy_spine_vertex_array_t vertices;
        spIntArray *indices;
        spFloatArray *world_vertices;
} joy_spine_draw_cache_t;

/* ==================== spine-c 要求的宿主回调 ==================== */

void _spAtlasPage_createTexture(spAtlasPage *self, const char *path) {
        int width = 0, height = 0, components = 0;
        stbi_uc *image_data;
        SDL_Texture *texture;
        size_t file_len = 0;
        stbi_uc *filedata;

        /* 不能用 stbi_load(path)：它内部走 stdio fopen，Android 的资源在
         * APK 内读不到。先经 SDL_LoadFile（AAssetManager）取到内存再解码。 */
        filedata = (stbi_uc *)SDL_LoadFile(path, &file_len);
        if (!filedata) {
                printf("[jspine] atlas page read failed: '%s' (%s)\n", path, SDL_GetError());
                return;
        }
        image_data = stbi_load_from_memory(filedata, (int)file_len, &width, &height, &components, 4);
        SDL_free(filedata);
        if (!image_data) {
                printf("[jspine] atlas page decode failed: '%s' (%s)\n",
                       path, stbi_failure_reason() ? stbi_failure_reason() : "unknown");
                return;
        }
        texture = SDL_CreateTexture((SDL_Renderer *) self->atlas->rendererObject,
                                    SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, width, height);
        if (!texture) {
                printf("[jspine] SDL_CreateTexture failed: %s\n", SDL_GetError());
                stbi_image_free(image_data);
                return;
        }
        if (!SDL_UpdateTexture(texture, NULL, image_data, width * 4)) {
                printf("[jspine] SDL_UpdateTexture failed: %s\n", SDL_GetError());
                SDL_DestroyTexture(texture);
                stbi_image_free(image_data);
                return;
        }
        stbi_image_free(image_data);
        self->rendererObject = texture;
}

void _spAtlasPage_disposeTexture(spAtlasPage *self) {
        if (self && self->rendererObject) SDL_DestroyTexture((SDL_Texture *) self->rendererObject);
}

char *_spUtil_readFile(const char *path, int *length) {
        size_t size = 0;
        char *filedata;
        char *data;

        if (length) *length = 0;
        /* spine 的默认 _spReadFile() 直接 fopen，Android 的 APK assets 读不到；
         * .atlas / .json / .skel 全部改走 SDL_LoadFile（AAssetManager）。
         * 返回值由 spine 用 FREE()/_spFree() 释放，所以必须换成 spine 自己的
         * MALLOC() 分配一份，不能把 SDL 的缓冲直接交出去（分配器不能混用）。 */
        filedata = (char *)SDL_LoadFile(path, &size);
        if (!filedata) return NULL;
        data = MALLOC(char, (int)size + 1);
        if (!data) {
                SDL_free(filedata);
                return NULL;
        }
        memcpy(data, filedata, size);
        data[size] = '\0';
        SDL_free(filedata);
        if (length) *length = (int)size;
        return data;
}

/* ==================== 内部工具 ==================== */

static void vertex_array_clear(joy_spine_vertex_array_t *arr) {
        if (arr) arr->size = 0;
}

static void vertex_array_add(joy_spine_vertex_array_t *arr, SDL_Vertex v) {
        int i;
        if (!arr) return;
        if (arr->size == arr->capacity) {
                int new_cap = arr->capacity > 0 ? arr->capacity * 2 : 32;
                SDL_Vertex *new_items = (SDL_Vertex *) calloc((size_t) new_cap, sizeof(SDL_Vertex));
                if (!new_items) return;
                for (i = 0; i < arr->size; i++) new_items[i] = arr->items[i];
                free(arr->items);
                arr->items = new_items;
                arr->capacity = new_cap;
        }
        arr->items[arr->size++] = v;
}

static int ends_with(const char *path, const char *suffix) {
        size_t plen = strlen(path);
        size_t slen = strlen(suffix);
        if (slen > plen) return 0;
#ifdef _MSC_VER
        return _stricmp(path + plen - slen, suffix) == 0;
#else
        return strcasecmp(path + plen - slen, suffix) == 0;
#endif
}

/* 收集一个 slot 的世界顶点/UV/索引/纹理，返回值：0=无可绘制内容，1=ok */
static int collect_attachment(spSlot *slot, spAttachment *attachment,
                              spFloatArray *world_vertices,
                              float **out_uvs, unsigned short **out_indices,
                              int *out_vertices_count, int *out_indices_count,
                              SDL_Texture **out_texture) {
        if (attachment->type == SP_ATTACHMENT_REGION) {
                static unsigned short quad_indices[] = {0, 1, 2, 2, 3, 0};
                spRegionAttachment *region = (spRegionAttachment *) attachment;
                spFloatArray_setSize(world_vertices, 8);
                spRegionAttachment_computeWorldVertices(region, slot, world_vertices->items, 0, 2);
                *out_uvs = region->uvs;
                *out_indices = quad_indices;
                *out_vertices_count = 4;
                *out_indices_count = 6;
                *out_texture = (SDL_Texture *) ((spAtlasRegion *) region->rendererObject)->page->rendererObject;
                return 1;
        } else if (attachment->type == SP_ATTACHMENT_MESH) {
                spMeshAttachment *mesh = (spMeshAttachment *) attachment;
                spFloatArray_setSize(world_vertices, mesh->super.worldVerticesLength);
                spVertexAttachment_computeWorldVertices(SUPER(mesh), slot, 0,
                                                        mesh->super.worldVerticesLength,
                                                        world_vertices->items, 0, 2);
                *out_uvs = mesh->uvs;
                *out_indices = mesh->triangles;
                *out_vertices_count = mesh->super.worldVerticesLength >> 1;
                *out_indices_count = mesh->trianglesCount;
                *out_texture = (SDL_Texture *) ((spAtlasRegion *) mesh->rendererObject)->page->rendererObject;
                return 1;
        }
        return 0;
}

/* ==================== 共享资源 ==================== */

joy_spine_asset_p joy_r2d_spine_asset_load(joy_r2d_p r2d, const char *atlas_path, const char *skeleton_path,
                                 float scale) {
        joy_spine_asset_p asset;
        spSkeletonJson *json = NULL;
        spSkeletonBinary *binary = NULL;

        if (!r2d || !atlas_path || !skeleton_path) return NULL;

        /* 全局：让 spine 使用 y 向下的坐标系，与屏幕坐标一致 */
        spBone_setYDown(-1);

        asset = (joy_spine_asset_p) calloc(1, sizeof(joy_spine_asset_t));
        if (!asset) return NULL;
        asset->scale = scale > 0 ? scale : 1.0f;

        /* renderer 作为 atlas 的 rendererObject，回调里用它建纹理 */
        asset->atlas = spAtlas_createFromFile(atlas_path, joy_r2d_unwrap(r2d));
        if (!asset->atlas) {
                printf("[jspine] atlas read failed: %s\n", atlas_path);
                free(asset);
                return NULL;
        }

        if (ends_with(skeleton_path, ".skel")) {
                binary = spSkeletonBinary_create(asset->atlas);
                binary->scale = asset->scale;
                asset->data = spSkeletonBinary_readSkeletonDataFile(binary, skeleton_path);
                if (!asset->data) {
                        printf("[jspine] skeleton binary failed: %s\n",
                               binary->error ? binary->error : "(unknown)");
                }
                spSkeletonBinary_dispose(binary);
        } else {
                json = spSkeletonJson_create(asset->atlas);
                json->scale = asset->scale;
                asset->data = spSkeletonJson_readSkeletonDataFile(json, skeleton_path);
                if (!asset->data) {
                        printf("[jspine] skeleton json failed: %s\n",
                               json->error ? json->error : "(unknown)");
                }
                spSkeletonJson_dispose(json);
        }
        if (!asset->data) {
                spAtlas_dispose(asset->atlas);
                free(asset);
                return NULL;
        }

        asset->state_data = spAnimationStateData_create(asset->data);
        asset->state_data->defaultMix = 0.2f;
        return asset;
}

void joy_r2d_spine_asset_free(joy_spine_asset_p asset) {
        if (!asset) return;
        if (asset->state_data) spAnimationStateData_dispose(asset->state_data);
        if (asset->data) spSkeletonData_dispose(asset->data);
        if (asset->atlas) spAtlas_dispose(asset->atlas);
        free(asset);
}

spSkeletonData *joy_r2d_spine_asset_data(joy_spine_asset_p asset) {
        return asset ? asset->data : NULL;
}

int joy_r2d_spine_asset_animation_count(joy_spine_asset_p asset) {
        return (asset && asset->data) ? asset->data->animationsCount : 0;
}

const char *joy_r2d_spine_asset_animation_name(joy_spine_asset_p asset, int index) {
        if (!asset || !asset->data || index < 0 || index >= asset->data->animationsCount) return NULL;
        return asset->data->animations[index]->name;
}

float joy_r2d_spine_asset_animation_duration(joy_spine_asset_p asset, int index) {
        if (!asset || !asset->data || index < 0 || index >= asset->data->animationsCount) return 0;
        return asset->data->animations[index]->duration;
}

/* ==================== 组件（实例） ==================== */

bool joy_r2d_spine_instance_init(joy_spine_instance_p inst, joy_spine_asset_p asset) {
        joy_spine_draw_cache_t *cache;

        if (!inst || !asset) return false;
        memset(inst, 0, sizeof(*inst));

        inst->asset = asset;
        inst->skeleton = spSkeleton_create(asset->data);
        inst->state = spAnimationState_create(asset->state_data);
        inst->clipper = spSkeletonClipping_create();

        cache = (joy_spine_draw_cache_t *) calloc(1, sizeof(joy_spine_draw_cache_t));
        if (!cache) {
                spSkeleton_dispose(inst->skeleton);
                spAnimationState_dispose(inst->state);
                spSkeletonClipping_dispose(inst->clipper);
                memset(inst, 0, sizeof(*inst));
                return false;
        }
        cache->indices = spIntArray_create(12);
        cache->world_vertices = spFloatArray_create(64);
        inst->draw_cache = cache;

        /* 默认状态 */
        inst->scale_x = 1.0f;
        inst->scale_y = 1.0f;
        inst->time_scale = 1.0f;
        inst->alpha = 1.0f;
        inst->tint.r = 255; inst->tint.g = 255; inst->tint.b = 255; inst->tint.a = 255;
        inst->visible = true;
        inst->zorder = 0;

        spSkeleton_setToSetupPose(inst->skeleton);
        spSkeleton_updateWorldTransform(inst->skeleton);
        return true;
}

void joy_r2d_spine_instance_free(joy_spine_instance_p inst) {
        joy_spine_draw_cache_t *cache;
        if (!inst) return;
        cache = (joy_spine_draw_cache_t *) inst->draw_cache;
        if (cache) {
                if (cache->indices) spIntArray_dispose(cache->indices);
                if (cache->world_vertices) spFloatArray_dispose(cache->world_vertices);
                free(cache->vertices.items);
                free(cache);
        }
        if (inst->clipper) spSkeletonClipping_dispose(inst->clipper);
        if (inst->state) spAnimationState_dispose(inst->state);
        if (inst->skeleton) spSkeleton_dispose(inst->skeleton);
        memset(inst, 0, sizeof(*inst)); /* asset 不属于实例，不释放 */
}

void joy_r2d_spine_instance_reset(joy_spine_instance_p inst) {
        if (!inst || !inst->skeleton) return;
        spSkeleton_setToSetupPose(inst->skeleton);
        spSkeleton_updateWorldTransform(inst->skeleton);
}

bool joy_r2d_spine_instance_set_animation(joy_spine_instance_p inst, int track, const char *name, bool loop) {
        spAnimation *animation;
        if (!inst || !inst->state || !inst->asset || !inst->asset->data || !name) return false;
        animation = spSkeletonData_findAnimation(inst->asset->data, name);
        if (!animation) return false;
        spAnimationState_setAnimation(inst->state, track, animation, loop ? 1 : 0);
        strncpy(inst->animation, name, JSPINE_NAME_MAX - 1);
        inst->animation[JSPINE_NAME_MAX - 1] = '\0';
        return true;
}

bool joy_r2d_spine_instance_add_animation(joy_spine_instance_p inst, int track, const char *name, bool loop,
                                   float delay) {
        spAnimation *animation;
        if (!inst || !inst->state || !inst->asset || !inst->asset->data || !name) return false;
        animation = spSkeletonData_findAnimation(inst->asset->data, name);
        if (!animation) return false;
        spAnimationState_addAnimation(inst->state, track, animation, loop ? 1 : 0, delay);
        return true;
}

/* 组件字段 -> spine 骨架（不推进动画、不算世界变换） */
static void jspine_sync_fields(joy_spine_instance_p inst) {
        spSkeleton *skeleton = inst->skeleton;
        skeleton->x = inst->x;
        skeleton->y = inst->y;
        skeleton->scaleX = inst->scale_x * (inst->flip_x ? -1.0f : 1.0f);
        skeleton->scaleY = inst->scale_y;
        if (skeleton->root) skeleton->root->rotation = inst->rotation;
        skeleton->color.r = inst->tint.r / 255.0f;
        skeleton->color.g = inst->tint.g / 255.0f;
        skeleton->color.b = inst->tint.b / 255.0f;
        skeleton->color.a = inst->alpha * (inst->tint.a / 255.0f);
}

/* 只同步变换（不推进动画）：暂停、或改完字段想立刻生效时调用 */
void joy_r2d_spine_instance_sync(joy_spine_instance_p inst) {
        if (!inst || !inst->skeleton) return;
        jspine_sync_fields(inst);
        spSkeleton_updateWorldTransform(inst->skeleton);
}

void joy_r2d_spine_instance_update(joy_spine_instance_p inst, float dt) {
        spSkeleton *skeleton;
        if (!inst || !inst->skeleton) return;
        skeleton = inst->skeleton;

        jspine_sync_fields(inst);

        if (inst->state) {
                spAnimationState_update(inst->state, dt * inst->time_scale);
                spAnimationState_apply(inst->state, skeleton);
        }
        spSkeleton_updateWorldTransform(skeleton);
}

void joy_r2d_spine_instance_draw(joy_r2d_p r2d, joy_spine_instance_p inst) {
        SDL_Renderer *renderer;
        spSkeleton *skeleton;
        spSkeletonClipping *clipper;
        joy_spine_draw_cache_t *cache;
        int i;

        if (!r2d || !inst || !inst->skeleton || !inst->visible) return;
        cache = (joy_spine_draw_cache_t *) inst->draw_cache;
        if (!cache) return;

        renderer = (SDL_Renderer *) joy_r2d_unwrap(r2d);
        skeleton = inst->skeleton;
        clipper = inst->clipper;

        for (i = 0; i < skeleton->slotsCount; ++i) {
                spSlot *slot = skeleton->drawOrder[i];
                spAttachment *attachment = slot->attachment;
                spFloatArray *vertices = cache->world_vertices;
                spColor *attachment_color = NULL;
                float *uvs = NULL;
                unsigned short *indices = NULL;
                int vertices_count = 0;
                int indices_count = 0;
                SDL_Texture *texture = NULL;
                SDL_Vertex sdl_vertex;
                int ii;

                if (!attachment) {
                        if (clipper) spSkeletonClipping_clipEnd(clipper, slot);
                        continue;
                }
                if (slot->color.a == 0 || !slot->bone->active) {
                        if (clipper) spSkeletonClipping_clipEnd(clipper, slot);
                        continue;
                }

                if (!collect_attachment(slot, attachment, vertices, &uvs, &indices,
                                        &vertices_count, &indices_count, &texture)) {
                        if (attachment->type == SP_ATTACHMENT_CLIPPING) {
                                spSkeletonClipping_clipStart(clipper, slot,
                                                             (spClippingAttachment *) attachment);
                        }
                        continue;
                }
                attachment_color = (attachment->type == SP_ATTACHMENT_REGION)
                                   ? &((spRegionAttachment *) attachment)->color
                                   : &((spMeshAttachment *) attachment)->color;
                if (attachment_color->a == 0 || !texture) {
                        if (clipper) spSkeletonClipping_clipEnd(clipper, slot);
                        continue;
                }

                /* SDL3: SDL_Vertex.color 是 SDL_FColor（0.0~1.0） */
                sdl_vertex.color.r = skeleton->color.r * slot->color.r * attachment_color->r;
                sdl_vertex.color.g = skeleton->color.g * slot->color.g * attachment_color->g;
                sdl_vertex.color.b = skeleton->color.b * slot->color.b * attachment_color->b;
                sdl_vertex.color.a = skeleton->color.a * slot->color.a * attachment_color->a;

                if (clipper && spSkeletonClipping_isClipping(clipper)) {
                        spSkeletonClipping_clipTriangles(clipper, vertices->items, vertices_count << 1,
                                                         indices, indices_count, uvs, 2);
                        vertices = clipper->clippedVertices;
                        vertices_count = clipper->clippedVertices->size >> 1;
                        uvs = clipper->clippedUVs->items;
                        indices = (unsigned short *) clipper->clippedTriangles->items;
                        indices_count = clipper->clippedTriangles->size;
                }

                vertex_array_clear(&cache->vertices);
                for (ii = 0; ii < vertices_count << 1; ii += 2) {
                        sdl_vertex.position.x = vertices->items[ii];
                        sdl_vertex.position.y = vertices->items[ii + 1];
                        sdl_vertex.tex_coord.x = uvs[ii];
                        sdl_vertex.tex_coord.y = uvs[ii + 1];
                        vertex_array_add(&cache->vertices, sdl_vertex);
                }
                spIntArray_clear(cache->indices);
                for (ii = 0; ii < indices_count; ii++)
                        spIntArray_add(cache->indices, indices[ii]);

                switch (slot->data->blendMode) {
                        case SP_BLEND_MODE_MULTIPLY:
                                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_MOD);
                                break;
                        case SP_BLEND_MODE_ADDITIVE:
                                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
                                break;
                        case SP_BLEND_MODE_SCREEN:
                        case SP_BLEND_MODE_NORMAL:
                        default:
                                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
                                break;
                }

                SDL_RenderGeometry(renderer, texture, cache->vertices.items, cache->vertices.size,
                                   cache->indices->items, indices_count);
                if (clipper) spSkeletonClipping_clipEnd(clipper, slot);
        }
        if (clipper) spSkeletonClipping_clipEnd2(clipper);
}

/* ==================== 变换/状态便捷接口 ==================== */

void joy_r2d_spine_instance_set_position(joy_spine_instance_p inst, float x, float y) {
        if (inst) { inst->x = x; inst->y = y; }
}

void joy_r2d_spine_instance_set_scale(joy_spine_instance_p inst, float sx, float sy) {
        if (inst) { inst->scale_x = sx; inst->scale_y = sy; }
}

void joy_r2d_spine_instance_set_rotation(joy_spine_instance_p inst, float degrees) {
        if (inst) inst->rotation = degrees;
}

void joy_r2d_spine_instance_set_flip_x(joy_spine_instance_p inst, bool flip) {
        if (inst) inst->flip_x = flip;
}

void joy_r2d_spine_instance_set_alpha(joy_spine_instance_p inst, float alpha) {
        if (inst) inst->alpha = alpha;
}

void joy_r2d_spine_instance_set_tint(joy_spine_instance_p inst, joy_color_t tint) {
        if (inst) inst->tint = tint;
}

const char *joy_r2d_spine_instance_animation(joy_spine_instance_p inst) {
        return (inst && inst->animation[0]) ? inst->animation : NULL;
}

bool joy_r2d_spine_instance_bounds(joy_spine_instance_p inst, float *out_x, float *out_y, float *out_w,
                            float *out_h) {
        spSkeleton *skeleton;
        joy_spine_draw_cache_t *cache;
        float min_x = FLT_MAX, min_y = FLT_MAX, max_x = -FLT_MAX, max_y = -FLT_MAX;
        int i, found = 0;

        if (!inst || !inst->skeleton) return false;
        skeleton = inst->skeleton;
        cache = (joy_spine_draw_cache_t *) inst->draw_cache;
        if (!cache) return false;

        for (i = 0; i < skeleton->slotsCount; i++) {
                spSlot *slot = skeleton->drawOrder[i];
                spAttachment *attachment = slot->attachment;
                float *uvs = NULL;
                unsigned short *indices = NULL;
                int vertices_count = 0, indices_count = 0;
                SDL_Texture *texture = NULL;
                int v;

                if (!attachment) continue;
                if (!collect_attachment(slot, attachment, cache->world_vertices, &uvs, &indices,
                                        &vertices_count, &indices_count, &texture)) continue;
                for (v = 0; v < vertices_count; v++) {
                        float x = cache->world_vertices->items[v * 2];
                        float y = cache->world_vertices->items[v * 2 + 1];
                        if (x < min_x) min_x = x;
                        if (y < min_y) min_y = y;
                        if (x > max_x) max_x = x;
                        if (y > max_y) max_y = y;
                }
                found = 1;
        }
        if (!found) return false;
        if (out_x) *out_x = min_x;
        if (out_y) *out_y = min_y;
        if (out_w) *out_w = max_x - min_x;
        if (out_h) *out_h = max_y - min_y;
        return true;
}

/* ==================== 结束 src/jspine.c ==================== */

/* ==================== 合并自 src/jui.c / jui_im.c（ui 域并入 r2d 域） ==================== */
/* 原 ui 域唯一的 .c/.h 已删除；控件实现按渲染上下文并入 r2d。
   内部符号（static / 私有宏 / typedef）保持各分片独立，见文件末尾 #undef。 */
/* ==================== 合并自 src/jui.c ==================== */

#define JOYSTICK_BG_COLOR 0x555555FF
#define JOYSTICK_HANDLE_COLOR 0x888888FF
#define JOYSTICK_HANDLE_ACTIVE_COLOR 0x4CAF50FF

static joy_rect_t jui_frect(joy_r2d_p r2d, float fx, float fy, float fw, float fh)
{
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        joy_rect_t r = { fx * (float)lw, fy * (float)lh, fw * (float)lw, fh * (float)lh };
        return r;
}
static void jui_fpx(joy_r2d_p r2d, float fx, float fy, float fw, float fh, float* x, float* y, float* w, float* h)
{
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        *x = fx * (float)lw; *y = fy * (float)lh;
        *w = fw * (float)lw; *h = fh * (float)lh;
}

typedef kvec_t(char*) joy_dg_row_t;

struct joy_label {
        joy_r2d_p r2d;
        joy_vec2_t position;
        joy_text_p text;
        joy_color_t color;
};


struct joy_button {
        joy_r2d_p r2d;   // 渲染器
        joy_vec2_t position;      // 位置
        float width;              // 宽度
        float height;             // 高度
        joy_color_t normal_color;   // 正常状态颜色
        joy_color_t hover_color;    // 悬停状态颜色
        joy_color_t pressed_color;  // 按下状态颜色
        joy_color_t display_color;  // 实际渲染颜色（平滑过渡用）
        joy_vec2_t text_pos;      // 文本相对位置
        joy_image_p image;            // 按钮正常纹理（可选）
        joy_image_p pressed_image;    // 按钮按下纹理（可选）
        joy_text_p text;      // 按钮文件纹理
        bool is_hovered;          // 内部状态：由 is_hovered() / is_pressed() 设置
        bool is_pressed;
};

struct joy_combobox_item {
        joy_rect_t rect;
        joy_text_p text;
        bool is_selected;
};

struct joy_combobox {
        joy_rect_t main_rect;
        joy_rect_t dropdown_rect;
        joy_combobox_item_t items[20];
        int item_count;
        int item_height;
        float ui_scale;             /* 相对 480x320 设计稿的缩放，供绘制换算固定像素 */
        int selected_index;
        bool is_open;
        joy_text_p text;
        joy_color_t bg_color;
        joy_color_t text_color;
        joy_color_t hover_color;
        joy_r2d_p r2d;
};

struct joy_datagrid {
        joy_r2d_p r2d;
        int x, y;              // 位置
        int width, height;     // 尺寸
        int row_height;        // 行高
        int col_count;         // 列数
        int row_count;         // 行数（包括表头）
        int visible_rows;      // 可见行数
        int scroll_offset;     // 滚动偏移
        int selected_row;      // 选中的行
        joy_text_p headers[512];        // 表头
        kvec_t(joy_dg_row_t) data;              // 表格数据（klib 动态数组 of joy_dg_row_t）
        int* col_widths;       // 列宽
        bool has_header;   // 是否有表头
        joy_font_p font;        // 字体
        joy_color_t grid_color;
        joy_color_t bg_color;
        joy_color_t text_color;
        joy_color_t header_bg_color;
        joy_color_t selected_color;
};

struct joy_radio {
        joy_r2d_p r2d;                // 渲染器（布局换算用）
        int x, y;                  // 位置
        int size;                  // 大小
        /* 创建时传入的比例（fx=较宽, fy=较高, size_frac=较宽）。保存下来，
         * 渲染/命中前按"当前"逻辑分辨率重算（同 checkbox），否则分辨率改变后
         * 单选框会停在旧像素位置且相对屏幕偏小。 */
        float fx, fy, size_frac;
        bool checked;              // 是否选中
        char* label;               // 标签文本
        int group_id;              // 所属组ID
        joy_rect_t hit_area;         // 点击区域
        bool hover;                // 鼠标悬停状态
        joy_color_t r1_color;
        joy_color_t r2_color;
        joy_color_t r3_color;
};

/* 布局重算（实现见下）：把创建时保存的比例按"当前"逻辑分辨率换算成像素，
 * 使逻辑分辨率改变后控件仍保持原有的相对位置与大小。 */
static void checkbox_update_layout(joy_checkbox_p cb);
static void radio_update_layout(joy_radio_p rb);

struct joy_checkbox {
        joy_r2d_p r2d;                 // 渲染器（坐标转换用）
        int x, y;                   // 位置
        int size;                   // 大小
        /* 创建时传入的比例（fx=较宽, fy=较高, size_frac=较宽）。
         * 保存下来，在绘制/命中前用"当前"逻辑分辨率重算 x/y/size —— 否则逻辑分辨率
         * 改变（如 480x320 → 960x640）后复选框会停在旧像素位置且相对屏幕偏小。 */
        float fx, fy, size_frac;
        joy_checkbox_state_t state;     // 当前状态
        char* label;                // 标签文本
        joy_color_t box_color;         // 框的颜色
        joy_color_t check_color;       // 勾选标记的颜色
        joy_color_t text_color;        // 文本颜色
        joy_color_t hover_color;       // 悬停颜色
        bool hover;                 // 悬停状态
        bool enabled;               // 是否启用
        joy_rect_t hit_area;           // 点击区域
};

struct joy_inputbox {
        joy_r2d_p r2d;
        joy_rect_t rect;               // 位置和大小
        joy_color_t bg_color;           // 背景色
        joy_color_t border_color;       // 边框色
        joy_color_t text_color;         // 文字色
        joy_color_t cursor_color;       // 光标色
        joy_color_t sel_color;          // 选中背景色
        joy_color_t focus_border_color; // 聚焦时边框色
        joy_font_p font;                  // 字体
        char* buffer;                 // 文本缓冲区
        int buf_len;                  // 缓冲区大小
        int text_len;                 // 当前文本长度
        int cursor_pos;               // 光标位置（字节偏移）
        int sel_start;                // 选中起始（-1 表示无选中）
        int sel_end;                  // 选中结束
        int scroll_offset;            // 文本滚动偏移（像素）
        bool focused;                 // 是否聚焦
        bool hovered;                 // 鼠标悬停
        uint64_t cursor_timer;          // 光标闪烁计时
        bool cursor_visible;          // 光标是否可见
        void (*on_submit)(struct joy_inputbox* ti, void* userdata);
        void (*on_change)(struct joy_inputbox* ti, void* userdata);
        void* cb_userdata;
};

struct joy_slider {
        joy_r2d_p r2d;
        joy_rect_t track;             // 滑轨区域
        float min, max;              // 取值范围
        float value;                 // 当前值
        float thumb_width;           // 滑块宽度（高度 = track.h）
        bool is_dragging;            // 是否在拖动
        joy_color_t track_color;       // 滑轨背景色
        joy_color_t fill_color;        // 已填充部分颜色
        joy_color_t thumb_color;       // 滑块颜色
        joy_color_t thumb_hover_color; // 滑块悬停颜色
        bool hovered;                // 滑块是否悬停
};



struct joy_popup {
        joy_r2d_p r2d;
        float x, y, w, h;
        float title_height;         // title bar height (set in draw)
        float padding;
        float close_size;
        joy_color_t bg_color;
        joy_color_t border_color;
        joy_color_t title_bg_color;
        joy_color_t content_color;
        joy_color_t close_color;
        joy_color_t close_hover_color;
        joy_color_t title_text_color;
        bool visible;
        bool closeable;
        char title[JOY_POPUP_MAX_TITLE];
        char content[JOY_POPUP_MAX_CONTENT];
        bool close_hovered;
        bool is_dragging;
        float drag_off_x, drag_off_y;
};


struct joy_tooltip {
        joy_r2d_p r2d;
        float x, y;
        char text[JOY_TOOLTIP_MAX_TEXT];
        int text_len;
        joy_color_t bg_color;
        joy_color_t border_color;
        joy_color_t text_color;
        float padding;
        bool visible;
};


struct joy_joystick {
        joy_r2d_p r2d;
        joy_rect_t background;
        joy_rect_t handle;
        joy_vec2_t center;
        float radius;
        float handle_radius;
        joy_vec2_t direction;
        float magnitude;
        float deadzone;
        bool is_dragging;
        bool is_touch_active;
        int touch_id;
        joy_image_p base_image;     // 当前显示的底圈纹理（指向 default 或某 mode 的纹理，不单独拥有）
        joy_image_p thumb_image;    // 当前显示的摇杆头纹理
        joy_image_p base_default;   // 默认底圈（joy_r2d_joystick_set_textures 设置）
        joy_image_p thumb_default;  // 默认摇杆头
        joy_image_p mode_base[5];   // 各方向模式专属底圈（NULL=回退默认）
        joy_image_p mode_thumb[5];  // 各方向模式专属摇杆头
        joy_joystick_dir_mode dir_mode;  // 方向约束模式（默认全方向）
};

static float distance_squared(float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        return dx * dx + dy * dy;
}

joy_label_p
joy_r2d_label_create(joy_r2d_p r2d, joy_font_p fontinfo, const int* codepoints, int num_codepoints)
{
        joy_label_p label = (joy_label_p)malloc(sizeof(joy_label_t));
        label->r2d = r2d;
        label->position = (joy_vec2_t){ 0, 0 };
        label->color = (joy_color_t){ 255, 255,255, 255 };
        label->text = joy_r2d_text_create(r2d, fontinfo, codepoints, num_codepoints, label->color);
        return label;
}

void
joy_r2d_label_destroy(joy_label_p label)
{
        if (!label) return;
        if (label->text) joy_r2d_text_destroy(label->text);
        free(label);
}

void
joy_r2d_label_set_text(joy_label_p label, joy_font_p font,
        const int* codepoints, int num_codepoints,
        joy_color_t color)
{
        if (label->text) {
                joy_r2d_text_update(label->text, font, codepoints, num_codepoints, color);
        }
        else {
                label->text = joy_r2d_text_create(label->r2d, font, codepoints, num_codepoints, color);
        }
}

void joy_r2d_label_draw(joy_label_p label)
{
        //joy_rect_t dest_rect;
        if (!label) return;

        //r2d = label->r2d;
        //dest_rect.x = label->position.x;
        //dest_rect.y = label->position.y;
        //dest_rect.w = label->text->w;
        //dest_rect.h = label->text->h;
        //SDL_RenderTexture(r2d, label->text, NULL, &dest_rect);
        joy_r2d_text_print(label->r2d, label->text, label->position.x, label->position.y);
}

void joy_r2d_label_handle_event(joy_label_p label, void* event)
{
        (void)label;
        (void)event;
}

joy_button_p
joy_r2d_button_create(joy_r2d_p r2d, float cx, float cy, float wf, float hf)
{
        joy_button_p btn = (joy_button_p)malloc(sizeof(joy_button_t));
        if (!btn) return NULL;
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        float w = wf * (float)lw, h = hf * (float)lh;
        float x = (cx - wf * 0.5f) * (float)lw, y = (cy - hf * 0.5f) * (float)lh;
        btn->r2d = r2d;
        btn->position = (joy_vec2_t){ x, y };
        btn->width = w;
        btn->height = h;
        btn->normal_color = (joy_color_t){ 100, 100, 200, 255 };
        btn->hover_color = (joy_color_t){ 150, 150, 250, 255 };
        btn->pressed_color = (joy_color_t){ 50, 50, 150, 255 };
        btn->text_pos = (joy_vec2_t){ 0, 0 };
        btn->image = NULL;
        btn->pressed_image = NULL;
        btn->text = NULL;
        btn->is_hovered = btn->is_pressed = false;
        btn->display_color = btn->normal_color;
        return btn;
}

void joy_r2d_button_set_normal_color(joy_button_p btn, joy_color_t color)
{
        if (!btn) return;
        btn->normal_color = color;
}

void joy_r2d_button_set_hover_color(joy_button_p btn, joy_color_t color)
{
        if (!btn) return;
        btn->hover_color = color;
}

void joy_r2d_button_set_pressed_color(joy_button_p btn, joy_color_t color)
{
        if (!btn) return;
        btn->pressed_color = color;
}

void joy_r2d_button_set_image(joy_button_p btn, joy_image_p image)
{
        if (!btn) return;
        btn->image = image;
}

void joy_r2d_button_set_pressed_image(joy_button_p btn, joy_image_p image)
{
        if (!btn) return;
        btn->pressed_image = image;
}



void joy_r2d_button_set_textx(joy_button_p btn, joy_font_p font,
        const char* str, int len, joy_color_t color)
{
        if (!btn) return;
        if (btn->text) {
                joy_r2d_text_updatex(btn->text, font, str, len, color);
        }
        else {
                btn->text = joy_r2d_text_createx(btn->r2d, font, str, len, color);
        }
}

void
joy_r2d_button_set_text(joy_button_p btn, joy_font_p font,
        const int* codepoints, int num_codepoints, joy_color_t color)
{
        if (!btn) return;
        if (btn->text) {
                joy_r2d_text_update(btn->text, font, codepoints, num_codepoints, color);
        }
        else {
                btn->text = joy_r2d_text_create(btn->r2d, font, codepoints, num_codepoints, color);
        }
}

void joy_r2d_button_set_position(joy_button_p btn, float x, float y)
{
        if (!btn) return;
        btn->position.x = x;
        btn->position.y = y;
}

joy_vec2_t joy_r2d_button_get_position(joy_button_p btn)
{
        return btn->position;
}

void joy_r2d_button_set_size(joy_button_p btn, float width, float height)
{
        if (!btn) return;
        btn->width = width;
        btn->height = height;
}

static bool button_hit_test(joy_button_p btn, float x, float y)
{
        joy_vec2_t logic = joy_r2d_coordinates_from_window(btn->r2d, x, y);
        return logic.x >= btn->position.x && logic.x <= btn->position.x + btn->width
                && logic.y >= btn->position.y && logic.y <= btn->position.y + btn->height;
}

bool joy_r2d_button_is_hovered(joy_button_p btn, float x, float y)
{
        if (!btn) return false;
        btn->is_hovered = button_hit_test(btn, x, y);
        return btn->is_hovered;
}

bool joy_r2d_button_is_pressed(joy_button_p btn, float x, float y)
{
        if (!btn) return false;
        btn->is_pressed = button_hit_test(btn, x, y);
        return btn->is_pressed;
}

void joy_r2d_button_set_hovered(joy_button_p btn, bool v)
{
        if (btn) btn->is_hovered = v;
}

void joy_r2d_button_set_text_offset(joy_button_p btn, float x, float y)
{
        if (!btn) return;
        btn->text_pos.x = x;
        btn->text_pos.y = y;
}

void joy_r2d_button_set_pressed(joy_button_p btn, bool v)
{
        if (btn) btn->is_pressed = v;
}

bool joy_r2d_button_get_pressed(joy_button_p btn)
{
        return btn ? btn->is_pressed : false;
}

void joy_r2d_button_destroy(joy_button_p btn)
{
        if (!btn) return;
        if (btn->text) joy_r2d_text_destroy(btn->text);
        if (btn->image) joy_r2d_image_destroy(btn->image);
        if (btn->pressed_image) joy_r2d_image_destroy(btn->pressed_image);
        free(btn);
}


void joy_r2d_button_draw(joy_button_p btn)
{
        joy_r2d_p r2d;
        joy_rect_t dest_rect, rect, inner;
        joy_color_t color;
        if (!btn) return;

        color = btn->normal_color;
        if (btn->is_pressed) {
                color = btn->pressed_color;
        }
        else if (btn->is_hovered) {
                color = btn->hover_color;
        }

        // 平滑过渡：display_color 每帧向目标颜色靠拢 15%
        #define LERP_CHANNEL(a, b, t) ((uint8_t)((float)(a) + ((float)(b) - (float)(a)) * (t)))
        float t = 0.15f;
        btn->display_color.r = LERP_CHANNEL(btn->display_color.r, color.r, t);
        btn->display_color.g = LERP_CHANNEL(btn->display_color.g, color.g, t);
        btn->display_color.b = LERP_CHANNEL(btn->display_color.b, color.b, t);
        btn->display_color.a = LERP_CHANNEL(btn->display_color.a, color.a, t);
        #undef LERP_CHANNEL

        rect.x = btn->position.x;
        rect.y = btn->position.y;
        rect.w = btn->width;
        rect.h = btn->height;

        r2d = btn->r2d;
        joy_r2d_set_color(r2d, btn->display_color.r, btn->display_color.g, btn->display_color.b, btn->display_color.a);
        joy_r2d_shape_draw_rectangle(r2d, "fill", rect);
        joy_r2d_set_color(r2d, 0, 0, 0, 255);
        joy_r2d_shape_draw_rectangle(r2d, "line", rect);
        /* 加粗边框：内缩 1 逻辑像素再描一圈，形成约 2px 黑框（窗口缩小时仍可见） */
        inner = rect;
        inner.x += 1;
        inner.y += 1;
        inner.w -= 2;
        inner.h -= 2;
        if (inner.w > 0 && inner.h > 0) {
                joy_r2d_shape_draw_rectangle(r2d, "line", inner);
        }

        if (btn->is_pressed && btn->pressed_image) {
                joy_r2d_image_draw1(btn->pressed_image, NULL, &rect);
        }
        else if (btn->image) {
                joy_r2d_image_draw1(btn->image, NULL, &rect);
        }

        if (btn->text) {
                float widht = joy_r2d_text_get_width(btn->text);
                float height = joy_r2d_text_get_height(btn->text);
                dest_rect.x = rect.x + (rect.w - widht) / 2.0f + btn->text_pos.x;
                dest_rect.y = rect.y + (rect.h - height) / 2.0f + btn->text_pos.y;
                joy_r2d_text_print(r2d, btn->text, dest_rect.x, dest_rect.y);
        }
}

/* 事件处理已移至 Lua 层，见 joy.lua 中的 joy.mousepressed/mousereleased/mousemoved */

joy_combobox_p joy_r2d_combobox_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh)
{
        joy_rect_t rect = jui_frect(r2d, fx, fy, fw, fh);
        joy_combobox_p combobox;
        combobox = (joy_combobox_p)malloc(sizeof(joy_combobox_t));
        combobox->r2d = r2d;
        combobox->text = NULL;
        combobox->main_rect = rect;
        combobox->dropdown_rect.x = rect.x;
        combobox->dropdown_rect.y = rect.y + rect.h;
        combobox->dropdown_rect.w = rect.w;
        combobox->dropdown_rect.h = 0;
        // item_height 原为 480x320 设计稿下的固定 30px，不随逻辑分辨率换算 ——
        // 高分辨率下会出现"文字比列表项还高、溢出到列表背景外"，这里按分辨率缩放。
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        const float s = (lw > 0) ? ((float)lw / 480.0f) : 1.0f;
        combobox->ui_scale = s;
        combobox->item_count = 0;
        combobox->item_height = (int)(30.0f * s);
        combobox->selected_index = -1;
        combobox->is_open = false;
        combobox->bg_color = (joy_color_t){ 240, 240, 240, 255 };
        combobox->text_color = (joy_color_t){ 0, 0, 0, 255 };
        combobox->hover_color = (joy_color_t){ 150, 150, 250, 255 };
        return combobox;
}

void joy_r2d_combobox_destroy(joy_combobox_p combobox)
{
        if (combobox) {
                for (int i = 0; i < combobox->item_count; i++)
                        if (combobox->items[i].text) joy_r2d_text_destroy(combobox->items[i].text);
                free(combobox);
        }
}

void
joy_r2d_combobox_set_text(joy_combobox_p combobox, joy_font_p font,
        const int* codepoints, int len, joy_color_t color)
{
        if (combobox->text) {
                joy_r2d_text_destroy(combobox->text);
        }
        combobox->text = joy_r2d_text_create(combobox->r2d, font, codepoints, len, color);
}

void joy_r2d_combobox_add_item(joy_combobox_p combobox, joy_font_p font, const char* str, int len, joy_color_t color)
{
        int capacity_codepoints = len * 4;
        int* codepoints = (int*)SDL_malloc(sizeof(int) * capacity_codepoints);
        int num_codepoints = joy_core_utf8_decode_all(str, len, codepoints, capacity_codepoints);
        joy_r2d_combobox_add_itemx(combobox, font, codepoints, num_codepoints, color);
        SDL_free(codepoints);
}

void joy_r2d_combobox_add_itemx(joy_combobox_p combobox, joy_font_p font, const int* codepoints, int len, joy_color_t color)
{
        joy_combobox_item_p item;
        joy_r2d_p r2d;
        if (combobox->item_count > 20) {
                return;
        }
        r2d = combobox->r2d;
        item = combobox->items + combobox->item_count;
        item->text = joy_r2d_text_create(r2d, font, codepoints, len, color);
        item->is_selected = false;
        combobox->item_count++;
        combobox->dropdown_rect.h = combobox->item_count * combobox->item_height;
}

bool joy_r2d_combobox_is_pressed(joy_combobox_p combobox, float x, float y)
{
        if (!combobox) return false;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(combobox->r2d, x, y);
        joy_rectangle_t rect;
        rect.x = combobox->main_rect.x;
        rect.y = combobox->main_rect.y;
        rect.width = combobox->main_rect.w;
        rect.height = combobox->main_rect.h;
        return joy_collision_2d_check_point_rectangle(logic, rect);
}

bool joy_r2d_combobox_is_open(joy_combobox_p combobox)
{
        return combobox ? combobox->is_open : false;
}

void joy_r2d_combobox_set_open(joy_combobox_p combobox, bool v)
{
        if (combobox) combobox->is_open = v;
}

int joy_r2d_combobox_hit_item(joy_combobox_p combobox, float x, float y)
{
        if (!combobox || !combobox->is_open) return -1;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(combobox->r2d, x, y);
        for (int i = 0; i < combobox->item_count; i++) {
                joy_rectangle_t item_rect = {
                    combobox->dropdown_rect.x,
                    combobox->dropdown_rect.y + i * combobox->item_height,
                    combobox->dropdown_rect.w,
                    combobox->item_height
                };
                if (joy_collision_2d_check_point_rectangle(logic, item_rect)) return i;
        }
        return -1;
}

void joy_r2d_combobox_set_selected(joy_combobox_p combobox, int index)
{
        if (combobox) combobox->selected_index = index;
}

int joy_r2d_combobox_get_selected(joy_combobox_p combobox)
{
        return combobox ? combobox->selected_index : -1;
}

void joy_r2d_combobox_draw(joy_combobox_p combobox)
{
        if (!combobox) return;
        joy_r2d_p r2d;
        r2d = combobox->r2d;
        joy_r2d_set_color(r2d, combobox->bg_color.r, combobox->bg_color.g, combobox->bg_color.b, combobox->bg_color.a);
        joy_r2d_shape_draw_rectangle(r2d, "fill", combobox->main_rect);


        joy_r2d_set_color(r2d, 0, 0, 0, 255);
        joy_r2d_shape_draw_rectangle(r2d, "line", combobox->main_rect);

        const float s = (combobox->ui_scale > 0.0f) ? combobox->ui_scale : 1.0f;

        // 渲染当前选中的文本（左边距按分辨率换算，垂直居中）
        if (combobox->selected_index >= 0 && combobox->items[combobox->selected_index].text) {
                // 注：jtext 的内部结构（real_height）在 jui.c 不可见，无法精确垂直居中，
                // 这里用按分辨率换算的顶部偏移（设计稿 2px）。
                joy_text_p t = combobox->items[combobox->selected_index].text;
                joy_r2d_text_print(r2d, t, combobox->main_rect.x + 5.0f * s,
                            combobox->main_rect.y + 2.0f * s);
        }


        // 如果下拉框是打开的, 渲染选择列表
        if (combobox->is_open) {
                // 渲染下拉箭头
                joy_color_t color = { 0, 0, 0, 255 };
                joy_r2d_set_color(r2d, 0, 0, 0, 255);
                // 箭头各偏移是设计稿像素（25/10/5/15/20），按分辨率换算并垂直居中
                float ax = combobox->main_rect.x + combobox->main_rect.w;
                float ay = combobox->main_rect.y;
                float ah = 10.0f * s;
                float ao = (combobox->main_rect.h - ah) * 0.5f;
                if (ao < 0.0f) ao = 0.0f;
                joy_vec2_t arrow_points[3] = {
                    {ax - 25.0f * s, ay + ao},
                    {ax - 5.0f * s,  ay + ao},
                    {ax - 15.0f * s, ay + ao + ah}
                };
                joy_r2d_shape_draw_polygon(r2d, "line", arrow_points, 3, color);


                joy_r2d_set_color(r2d, combobox->bg_color.r, combobox->bg_color.g, combobox->bg_color.b, combobox->bg_color.a);
                joy_r2d_shape_draw_rectangle(r2d, "fill", combobox->dropdown_rect);

                joy_r2d_set_color(r2d, 0, 0, 0, 255);
                joy_r2d_shape_draw_rectangle(r2d, "line", combobox->dropdown_rect);

                joy_combobox_item_p item;

                for (int i = 0; i < combobox->item_count; i++) {
                        joy_rect_t item_rect = {
                            combobox->dropdown_rect.x,
                            combobox->dropdown_rect.y + i * combobox->item_height,
                            combobox->dropdown_rect.w,
                            combobox->item_height
                        };

                        // 如果是当前选中的项, 使用高亮颜色
                        if (i == combobox->selected_index) {
                                joy_r2d_set_color(r2d, combobox->hover_color.r, combobox->hover_color.g, combobox->hover_color.b, combobox->hover_color.a);
                                joy_r2d_shape_draw_rectangle(r2d, "line", item_rect);
                        }

                        joy_r2d_set_color(r2d, 0, 0, 0, 255);
                        joy_r2d_shape_draw_rectangle(r2d, "line", item_rect);

                        item = combobox->items + i;
                        if (item->text) {
                                // 顶部偏移按分辨率换算（设计稿 2px）：
                                // item_height 已同步放大，文字不会溢出到下一项
                                joy_r2d_text_print(r2d, item->text, item_rect.x + 5.0f * s,
                                            item_rect.y + 2.0f * s);
                        }
                }
        }
        else {
                // 渲染下拉箭头
                joy_color_t color = { 0, 0, 0, 255 };
                float ax = combobox->main_rect.x + combobox->main_rect.w;
                float ay = combobox->main_rect.y;
                float ah = 10.0f * s;
                float ao = (combobox->main_rect.h - ah) * 0.5f;
                if (ao < 0.0f) ao = 0.0f;
                joy_vec2_t arrow_points[3] = {
                    {ax - 15.0f * s, ay + ao},
                    {ax - 25.0f * s, ay + ao + ah},
                    {ax - 5.0f * s,  ay + ao + ah}
                };
                joy_r2d_shape_draw_polygon(r2d, "line", arrow_points, 3, color);
        }
}


/* ===== 可输入下拉列表框 (combobox_input) ===== */

struct joy_combobox_input {
        joy_r2d_p r2d;
        joy_rect_t rect;               // 整体控件区域
        joy_color_t bg_color;          // 背景色
        joy_color_t border_color;      // 边框色
        joy_color_t text_color;        // 文字色
        joy_color_t cursor_color;      // 光标色
        joy_color_t sel_color;         // 选中背景色
        joy_color_t focus_border_color; // 聚焦时边框色
        joy_font_p font;

        // 文本输入
        char* buffer;
        int buf_len;
        int text_len;
        int cursor_pos;
        int sel_start, sel_end;
        int scroll_offset;
        bool focused;
        uint64_t cursor_timer;
        bool cursor_visible;

        // 下拉列表
        char items[20][JOY_MAX_IP];
        int item_count;
        int selected_index;
        bool is_open;
        int item_height;

        // 箭头按钮区域（右侧）
        joy_rect_t arrow_rect;
        bool arrow_hovered;

        // 回调
        void (*on_change)(struct joy_combobox_input* ci, void* userdata);
        void* cb_userdata;

        // 弹层节点（展开时自动挂载，关闭时自动卸载）
        joy_scene_node_p popup_node;
};

joy_combobox_input_p joy_r2d_combobox_input_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, joy_font_p font, int buf_len)
{
        joy_rect_t rect = jui_frect(r2d, fx, fy, fw, fh);
        joy_combobox_input_p ci = (joy_combobox_input_p)calloc(1, sizeof(joy_combobox_input_t));
        if (!ci) return NULL;
        ci->r2d = r2d;
        ci->rect = rect;
        ci->font = font;
        ci->buf_len = buf_len > 0 ? buf_len : 256;
        ci->buffer = (char*)malloc(ci->buf_len);
        ci->buffer[0] = '\0';
        ci->text_len = 0;
        ci->cursor_pos = 0;
        ci->sel_start = -1;
        ci->sel_end = -1;
        ci->scroll_offset = 0;
        ci->focused = false;
        ci->cursor_timer = 0;
        ci->cursor_visible = false;
        ci->item_count = 0;
        ci->selected_index = -1;
        ci->is_open = false;
        /* item_height / 箭头宽度原本是 480x320 设计稿下的固定像素（24 / 20），
         * 逻辑分辨率提高后不跟着放大 —— 表现为"字号比列表项还高"、文字溢出到
         * 列表背景外面（与下方控件重叠看不清）。这里按逻辑分辨率换算。 */
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        const float s = (lw > 0) ? ((float)lw / 480.0f) : 1.0f;

        ci->item_height = (int)(24.0f * s);
        ci->arrow_hovered = false;
        // 箭头按钮在右侧（设计稿 20px）
        ci->arrow_rect.w = 20.0f * s;
        ci->arrow_rect.x = rect.x + rect.w - ci->arrow_rect.w;
        ci->arrow_rect.y = rect.y;
        ci->arrow_rect.h = rect.h;
        // 配色
        ci->bg_color = (joy_color_t){ 255, 255, 255, 255 };
        ci->border_color = (joy_color_t){ 80, 80, 80, 255 };
        ci->text_color = (joy_color_t){ 0, 0, 0, 255 };
        ci->cursor_color = (joy_color_t){ 0, 0, 0, 255 };
        ci->sel_color = (joy_color_t){ 100, 150, 255, 100 };
        ci->focus_border_color = (joy_color_t){ 50, 100, 200, 255 };
        return ci;
}

void joy_r2d_combobox_input_destroy(joy_combobox_input_p ci)
{
        if (ci) {
                free(ci->buffer);
                free(ci);
        }
}

void joy_r2d_combobox_input_add_item(joy_combobox_input_p ci, const char* str, int len)
{
        if (!ci || ci->item_count >= 20) return;
        if (len <= 0) len = (int)strlen(str);
        if (len >= JOY_MAX_IP) len = JOY_MAX_IP - 1;
        memcpy(ci->items[ci->item_count], str, len);
        ci->items[ci->item_count][len] = '\0';
        ci->item_count++;
}

void joy_r2d_combobox_input_clear_items(joy_combobox_input_p ci)
{
        if (ci) ci->item_count = 0;
}

const char* joy_r2d_combobox_input_get_text(joy_combobox_input_p ci)
{
        return ci ? ci->buffer : "";
}

void joy_r2d_combobox_input_set_text(joy_combobox_input_p ci, const char* str)
{
        if (!ci) return;
        int len = (int)strlen(str);
        if (len >= ci->buf_len) len = ci->buf_len - 1;
        memcpy(ci->buffer, str, len);
        ci->buffer[len] = '\0';
        ci->text_len = len;
        ci->cursor_pos = len;
        ci->scroll_offset = 0;
}

static void ci_inputbox_focus(joy_combobox_input_p ci)
{
        if (!ci || ci->focused) return;
        ci->focused = true;
        ci->cursor_timer = joy_core_get_ticks();
        ci->cursor_visible = true;
        SDL_Window* win = SDL_GetRenderWindow((SDL_Renderer*)joy_r2d_unwrap(ci->r2d));
        if (win) SDL_StartTextInput(win);
}

static void ci_inputbox_defocus(joy_combobox_input_p ci)
{
        if (!ci || !ci->focused) return;
        ci->focused = false;
        ci->sel_start = -1;
        ci->sel_end = -1;
        SDL_Window* win = SDL_GetRenderWindow((SDL_Renderer*)joy_r2d_unwrap(ci->r2d));
        if (win) SDL_StopTextInput(win);
}

static int ci_cursor_x(joy_combobox_input_p ci)
{
        if (ci->cursor_pos <= 0 || !ci->font) return 0;
        joy_color_t c = { 0,0,0,255 };
        joy_text_p tex = joy_r2d_text_createx(ci->r2d, ci->font, ci->buffer, ci->cursor_pos, c);
        if (!tex) return 0;
        int w = joy_r2d_text_get_width(tex);
        joy_r2d_text_destroy(tex);
        return w;
}

static int ci_cursor_x_at(joy_combobox_input_p ci, int pos)
{
        if (pos <= 0 || !ci->font) return 0;
        joy_color_t c = { 0,0,0,255 };
        joy_text_p tex = joy_r2d_text_createx(ci->r2d, ci->font, ci->buffer, pos, c);
        if (!tex) return 0;
        int w = joy_r2d_text_get_width(tex);
        joy_r2d_text_destroy(tex);
        return w;
}

static int ci_pos_from_x(joy_combobox_input_p ci, float local_x)
{
        if (!ci || ci->text_len <= 0 || local_x <= 0) return 0;
        int total_w = ci_cursor_x_at(ci, ci->text_len);
        if (local_x >= total_w) return ci->text_len;
        for (int i = 0; i < ci->text_len; ) {
                int next = i + 1;
                while (next < ci->text_len && ((unsigned char)ci->buffer[next] & 0xC0) == 0x80)
                        next++;
                int w = ci_cursor_x_at(ci, next);
                if (w >= local_x) {
                        int prev_w = ci_cursor_x_at(ci, i);
                        return (local_x - prev_w < w - local_x) ? i : next;
                }
                i = next;
        }
        return ci->text_len;
}

static void ci_scroll_to_cursor(joy_combobox_input_p ci)
{
        if (!ci) return;
        int cx = ci_cursor_x(ci);
        // 文字区宽度 = 总宽 - 箭头按钮区（箭头宽度已按分辨率缩放，不再写死 24）
        float text_area_w = ci->rect.w - ci->arrow_rect.w - 4.0f;
        if (cx - ci->scroll_offset < 0)
                ci->scroll_offset = cx;
        else if (cx - ci->scroll_offset > text_area_w)
                ci->scroll_offset = cx - (int)text_area_w;
}

static void ci_insert(joy_combobox_input_p ci, const char* text, int len)
{
        if (!ci || !text || len <= 0) return;
        if (ci->text_len + len >= ci->buf_len) return;
        memmove(ci->buffer + ci->cursor_pos + len, ci->buffer + ci->cursor_pos,
                ci->text_len - ci->cursor_pos);
        memcpy(ci->buffer + ci->cursor_pos, text, len);
        ci->cursor_pos += len;
        ci->text_len += len;
        ci->buffer[ci->text_len] = '\0';
        ci_scroll_to_cursor(ci);
        if (ci->on_change) ci->on_change(ci, ci->cb_userdata);
}

static void ci_backspace(joy_combobox_input_p ci)
{
        if (ci->cursor_pos <= 0) return;
        int pos = ci->cursor_pos - 1;
        while (pos > 0 && ((unsigned char)ci->buffer[pos] & 0xC0) == 0x80)
                pos--;
        int ch_len = ci->cursor_pos - pos;
        memmove(ci->buffer + pos, ci->buffer + ci->cursor_pos,
                ci->text_len - ci->cursor_pos);
        ci->text_len -= ch_len;
        ci->cursor_pos = pos;
        ci->buffer[ci->text_len] = '\0';
        ci_scroll_to_cursor(ci);
        if (ci->on_change) ci->on_change(ci, ci->cb_userdata);
}

void joy_r2d_combobox_input_draw(joy_combobox_input_p ci)
{
        if (!ci) return;
        joy_r2d_p r = ci->r2d;
        joy_rect_t rect = ci->rect;

        // 设计稿像素（文字左边距 4、光标边界、分隔线等）按逻辑分辨率换算，
        // 理由同 item_height：不换算在 960x640 下会偏小/偏移。
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r, &lw, &lh, NULL);
        const float s = (lw > 0) ? ((float)lw / 480.0f) : 1.0f;

        // 背景
        joy_r2d_set_color(r, ci->bg_color.r, ci->bg_color.g, ci->bg_color.b, ci->bg_color.a);
        joy_r2d_shape_draw_rectangle(r, "fill", rect);

        // 边框（聚焦时高亮）
        joy_color_t border = ci->focused ? ci->focus_border_color : ci->border_color;
        joy_r2d_set_color(r, border.r, border.g, border.b, border.a);
        joy_r2d_shape_draw_rectangle(r, "line", rect);

        // 裁剪区域（文字区域，排除箭头按钮）
        SDL_Renderer* sdl_r = (SDL_Renderer*)joy_r2d_unwrap(r);
        // 裁剪区排除箭头按钮（宽度随分辨率换算，原来写死 24）
        SDL_Rect clip_rect = { (int)rect.x + 2, (int)rect.y,
                (int)(rect.w - ci->arrow_rect.w - 4.0f), (int)rect.h };
        bool clip_was_enabled = SDL_RenderClipEnabled(sdl_r);
        SDL_Rect old_clip;
        if (clip_was_enabled) SDL_GetRenderClipRect(sdl_r, &old_clip);
        SDL_SetRenderClipRect(sdl_r, &clip_rect);

        // 文字
        if (ci->text_len > 0 && ci->font) {
                float text_x = rect.x + 4.0f * s;
                float text_y = rect.y + (rect.h - (float)joy_r2d_font_get_size(ci->font)) / 2.0f;
                float ox = text_x - (float)ci->scroll_offset;
                joy_r2d_text_printx(r, ci->font, ci->buffer, ci->text_len, ci->text_color, ox, text_y);
        }

        // 恢复裁剪
        if (clip_was_enabled)
                SDL_SetRenderClipRect(sdl_r, &old_clip);
        else
                SDL_SetRenderClipRect(sdl_r, NULL);

        // 光标
        if (ci->focused) {
                uint64_t now = joy_core_get_ticks();
                if (now - ci->cursor_timer >= 530) {
                        ci->cursor_visible = !ci->cursor_visible;
                        ci->cursor_timer = now;
                }
                if (ci->cursor_visible) {
                        int cx = ci_cursor_x(ci) - ci->scroll_offset + (int)rect.x + (int)(4.0f * s);
                        // 光标右边界 = 文字区右缘（总宽 - 箭头按钮区），原来写死 22
                        if (cx >= rect.x + 2 && cx <= rect.x + rect.w - (int)(ci->arrow_rect.w + 4.0f)) {
                                float cy = rect.y + 3.0f * s;
                                float ch = rect.h - 6.0f * s;
                                joy_r2d_set_color(r, ci->cursor_color.r, ci->cursor_color.g, ci->cursor_color.b, ci->cursor_color.a);
                                joy_r2d_shape_draw_line(r, (float)cx + 0.5f, cy, (float)cx + 0.5f, cy + ch);
                        }
                }
        }

        /* 箭头按钮：展开时朝上、收起时朝下。
         * 三角原本是设计稿下的固定像素（半宽 5、半高 3），逻辑分辨率提高后不换算
         * —— 960x640 下只有 10x6 px，小到几乎看不见（像是"没有展开/收起标识"）。
         * 这里按逻辑分辨率缩放（s 已在函数开头算好），并跟随箭头区大小。 */
        float arrow_cx = ci->arrow_rect.x + ci->arrow_rect.w / 2.0f;
        float arrow_cy = ci->arrow_rect.y + ci->arrow_rect.h / 2.0f;
        float aw = 5.0f * s, ah = 3.0f * s;          // 半宽 / 半高（设计稿 5 / 3）
        // 用箭头区实际高度再兜一层，避免控件本身很高时三角仍偏小
        float min_h = ci->arrow_rect.h * 0.18f;
        if (ah < min_h) { ah = min_h; aw = ah * (5.0f / 3.0f); }

        if (ci->is_open) {
                // 上箭头
                joy_vec2_t pts[3] = {
                    {arrow_cx - aw, arrow_cy + ah},
                    {arrow_cx + aw, arrow_cy + ah},
                    {arrow_cx,      arrow_cy - ah}
                };
                joy_r2d_shape_draw_polygon(r, "fill", pts, 3, ci->text_color);
        } else {
                // 下箭头
                joy_vec2_t pts[3] = {
                    {arrow_cx - aw, arrow_cy - ah},
                    {arrow_cx + aw, arrow_cy - ah},
                    {arrow_cx,      arrow_cy + ah}
                };
                joy_r2d_shape_draw_polygon(r, "fill", pts, 3, ci->text_color);
        }

        // 竖线分隔箭头区域
        joy_r2d_set_color(r, ci->border_color.r, ci->border_color.g, ci->border_color.b, ci->border_color.a);
        joy_r2d_shape_draw_line(r, ci->arrow_rect.x, rect.y + 2.0f * s, ci->arrow_rect.x, rect.y + rect.h - 2.0f * s);
}

void joy_r2d_combobox_input_draw_dropdown(joy_combobox_input_p ci)
{
        if (!ci || !ci->is_open || ci->item_count <= 0) return;
        joy_r2d_p r = ci->r2d;
        joy_rect_t rect = ci->rect;
        /* 设计稿像素（文字左边距等）按逻辑分辨率换算，理由同 create 中的 item_height */
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r, &lw, &lh, NULL);
        const float s = (lw > 0) ? ((float)lw / 480.0f) : 1.0f;
        joy_rect_t drop_rect = {
            rect.x, rect.y + rect.h,
            rect.w, (float)(ci->item_count * ci->item_height)
        };
        // 背景
        joy_color_t drop_bg = { 255, 255, 255, 255 };
        joy_r2d_set_color(r, drop_bg.r, drop_bg.g, drop_bg.b, drop_bg.a);
        joy_r2d_shape_draw_rectangle(r, "fill", drop_rect);
        // 边框
        joy_r2d_set_color(r, 0, 0, 0, 255);
        joy_r2d_shape_draw_rectangle(r, "line", drop_rect);

        for (int i = 0; i < ci->item_count; i++) {
                joy_rect_t item_rect = {
                    drop_rect.x, drop_rect.y + i * ci->item_height,
                    drop_rect.w, (float)ci->item_height
                };
                // 选中项高亮
                if (i == ci->selected_index) {
                        joy_color_t hl = { 200, 220, 255, 255 };
                        joy_r2d_set_color(r, hl.r, hl.g, hl.b, hl.a);
                        joy_r2d_shape_draw_rectangle(r, "fill", item_rect);
                }
                // 分隔线
                joy_r2d_set_color(r, 220, 220, 220, 255);
                joy_r2d_shape_draw_line(r, item_rect.x, item_rect.y + item_rect.h,
                        item_rect.x + item_rect.w, item_rect.y + item_rect.h);
                // 文字：垂直居中 + 左边距按分辨率换算
                //（原来是固定 +4 / +2，字号变大后会顶出列表项、溢出到列表外）
                float th = (float)joy_r2d_font_get_size(ci->font);
                float ty = item_rect.y + (item_rect.h - th) * 0.5f;
                if (ty < item_rect.y) ty = item_rect.y;
                joy_r2d_text_printx(r, ci->font, ci->items[i], (int)strlen(ci->items[i]),
                        ci->text_color, item_rect.x + 4.0f * s, ty);
        }
}

bool joy_r2d_combobox_input_is_open(joy_combobox_input_p ci)
{
        return ci ? ci->is_open : false;
}

/* 下面两个内部函数在后面才定义，这里提前声明以便 joy_r2d_combobox_input_set_open 使用 */
static void ci_open_dropdown(joy_combobox_input_p ci);
static void ci_close_dropdown(joy_combobox_input_p ci);

/* 主动展开/收起下拉列表：调用方用 hit_item 选中某项后需要收起列表，
 * 原来没有这个接口（内部 ci_open_dropdown / ci_close_dropdown 是 static）。 */
void joy_r2d_combobox_input_set_open(joy_combobox_input_p ci, bool v)
{
        if (!ci) return;
        if (v) ci_open_dropdown(ci);
        else   ci_close_dropdown(ci);
}

void joy_r2d_combobox_input_set_popup_node(joy_combobox_input_p ci, joy_scene_node_p node)
{
        if (ci) ci->popup_node = node;
}

/* 弹层节点渲染回调：绘制下拉列表 */
static void ci_popup_render(joy_scene_node_p node, const void* arg)
{
        (void)arg;
        joy_combobox_input_p ci = (joy_combobox_input_p)joy_scene_node_get_userdata(node);
        if (ci) joy_r2d_combobox_input_draw_dropdown(ci);
}

/* 展开下拉列表时挂载到弹层节点 */
static void ci_open_dropdown(joy_combobox_input_p ci)
{
        if (!ci || ci->is_open) return;
        ci->is_open = true;
        if (ci->popup_node) {
                joy_scene_node_set_userdata(ci->popup_node, ci);
                joy_scene_node_set_render_callback(ci->popup_node, ci_popup_render);
        }
}

/* 关闭下拉列表时从弹层节点卸载 */
static void ci_close_dropdown(joy_combobox_input_p ci)
{
        if (!ci || !ci->is_open) return;
        ci->is_open = false;
        if (ci->popup_node) {
                joy_scene_node_set_render_callback(ci->popup_node, NULL);
                joy_scene_node_set_userdata(ci->popup_node, NULL);
        }
}

void joy_r2d_combobox_input_process_event(joy_combobox_input_p ci, const void* ev)
{
        if (!ci || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_MOTION) {
                joy_vec2_t logic = joy_r2d_coordinates_from_window(ci->r2d, e->motion.x, e->motion.y);
                ci->arrow_hovered = (logic.x >= ci->arrow_rect.x && logic.x <= ci->arrow_rect.x + ci->arrow_rect.w
                        && logic.y >= ci->arrow_rect.y && logic.y <= ci->arrow_rect.y + ci->arrow_rect.h);
        }
        else if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                joy_vec2_t logic = joy_r2d_coordinates_from_window(ci->r2d, e->button.x, e->button.y);
                bool in_rect = (logic.x >= ci->rect.x && logic.x <= ci->rect.x + ci->rect.w
                        && logic.y >= ci->rect.y && logic.y <= ci->rect.y + ci->rect.h);
                bool in_arrow = (logic.x >= ci->arrow_rect.x && logic.x <= ci->arrow_rect.x + ci->arrow_rect.w
                        && logic.y >= ci->arrow_rect.y && logic.y <= ci->arrow_rect.y + ci->arrow_rect.h);

                if (in_arrow) {
                        // 点击箭头：切换下拉列表
                        if (ci->is_open)
                                ci_close_dropdown(ci);
                        else
                                ci_open_dropdown(ci);
                        ci_inputbox_focus(ci);
                }
                else if (in_rect) {
                        // 点击文字区域：聚焦并定位光标
                        ci_inputbox_focus(ci);
                        float local_x = logic.x - ci->rect.x - 2 + ci->scroll_offset;
                        ci->cursor_pos = ci_pos_from_x(ci, local_x);
                        ci_scroll_to_cursor(ci);
                        ci_close_dropdown(ci);
                }
                else {
                        // 点击外部：关闭列表并失焦
                        ci_close_dropdown(ci);
                        ci_inputbox_defocus(ci);
                }
        }
        else if (e->type == SDL_EVENT_TEXT_INPUT) {
                if (ci->focused) {
                        ci_insert(ci, e->text.text, (int)strlen(e->text.text));
                }
        }
        else if (e->type == SDL_EVENT_KEY_DOWN) {
                if (!ci->focused) return;
                switch (e->key.scancode) {
                case SDL_SCANCODE_BACKSPACE: ci_backspace(ci); break;
                case SDL_SCANCODE_LEFT:
                        if (ci->cursor_pos > 0) ci->cursor_pos--;
                        ci_scroll_to_cursor(ci);
                        break;
                case SDL_SCANCODE_RIGHT:
                        if (ci->cursor_pos < ci->text_len) ci->cursor_pos++;
                        ci_scroll_to_cursor(ci);
                        break;
                case SDL_SCANCODE_DOWN:
                        if (ci->is_open && ci->selected_index < ci->item_count - 1)
                                ci->selected_index++;
                        break;
                case SDL_SCANCODE_UP:
                        if (ci->is_open && ci->selected_index > 0)
                                ci->selected_index--;
                        break;
                case SDL_SCANCODE_RETURN:
                case SDL_SCANCODE_KP_ENTER:
                        if (ci->is_open && ci->selected_index >= 0 && ci->selected_index < ci->item_count) {
                                joy_r2d_combobox_input_set_text(ci, ci->items[ci->selected_index]);
                                ci_close_dropdown(ci);
                        }
                        break;
                default: break;
                }
        }
}

int joy_r2d_combobox_input_hit_item(joy_combobox_input_p ci, float x, float y)
{
        if (!ci || !ci->is_open) return -1;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(ci->r2d, x, y);
        for (int i = 0; i < ci->item_count; i++) {
                joy_rect_t item_rect = {
                    ci->rect.x, ci->rect.y + ci->rect.h + i * ci->item_height,
                    ci->rect.w, (float)ci->item_height
                };
                if (logic.x >= item_rect.x && logic.x <= item_rect.x + item_rect.w
                        && logic.y >= item_rect.y && logic.y <= item_rect.y + item_rect.h)
                        return i;
        }
        return -1;
}


joy_datagrid_p
joy_r2d_datagrid_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, int col_count, bool has_header, void* font)
{
        joy_rect_t rect = jui_frect(r2d, fx, fy, fw, fh);
        joy_datagrid_p grid;
        grid = (joy_datagrid_p)malloc(sizeof(joy_datagrid_t));
        if (!grid) return NULL;
        memset(grid, 0, sizeof(joy_datagrid_t));

        grid->r2d = r2d;
        grid->x = rect.x;
        grid->y = rect.y;
        grid->width = rect.w;
        grid->height = rect.h;
        grid->row_height = 30;
        grid->col_count = col_count;
        grid->row_count = 0;
        grid->visible_rows = (rect.h - grid->row_height) / grid->row_height;
        grid->scroll_offset = 0;
        grid->selected_row = -1;
        grid->has_header = has_header;
        grid->font = (joy_font_p)font;
        kv_init(grid->data);
        grid->col_widths = (int*)malloc(col_count * sizeof(int));
        grid->bg_color = (joy_color_t){ 0xFF, 0xFF, 0xFF, 0xFF };
        grid->grid_color = (joy_color_t){ 0xCC, 0xCC, 0xCC, 0xFF };
        grid->text_color = (joy_color_t){ 0x00, 0x00, 0x00, 0xFF };
        grid->header_bg_color = (joy_color_t){ 0xEE, 0xEE, 0xEE, 0xFF };
        grid->selected_color = (joy_color_t){ 0x33, 0x99, 0xFF, 0xFF };
        memset(grid->headers, 0, 512 * sizeof(char));
        int default_width = grid->width / col_count;
        for (int i = 0; i < col_count; i++) {
                grid->col_widths[i] = default_width;
        }
        return grid;
}


void joy_r2d_datagrid_setheaders(joy_datagrid_p grid, const char* headers)
{
        joy_font_p fontinfo;
        joy_color_t color;
        color = (joy_color_t){ 0x0, 0x0, 0x0, 0xFF };
        if (!grid->has_header) return;
        fontinfo = grid->font;
        for (int i = 0; i < grid->col_count; i++) {
                //grid->headers[i] = joy_r2d_text_createx(grid->r2d, fontinfo, headers[i], SDL_strlen(headers[i]), color);
        }
}

void joy_r2d_datagrid_setheader(joy_datagrid_p grid, int index, const char* header)
{
        joy_font_p fontinfo;
        joy_color_t color;
        if (!grid->has_header) return;
        if (index < grid->col_count) return;
        color = (joy_color_t){ 0x0, 0x0, 0x0, 0xFF };
        fontinfo = grid->font;
        if (grid->headers[index]) {
                /* 由外部管理 text 更新 */
        }
        else {
                /* 由外部设置 header text */
        }
}

void joy_r2d_datagrid_addrow(joy_datagrid_p grid, const char** row_data)
{
        if (!grid) return;
        joy_dg_row_t row;
        kv_init(row);
        for (int i = 0; i < grid->col_count; i++) {
                kv_push(char*, row, row_data && row_data[i] ? joy_core_utils_strdup(row_data[i], NULL) : NULL);
        }
        kv_push(joy_dg_row_t, grid->data, row);
        grid->row_count = (int)kv_size(grid->data);
}

void joy_r2d_datagrid_setcell(joy_datagrid_p grid, int row_index, int col_index, const char* data)
{
        if (!grid) return;
        if (row_index < (int)kv_size(grid->data) && col_index < grid->col_count) {
                joy_dg_row_t* row = &kv_A(grid->data, row_index);
                if (kv_A(*row, col_index)) free(kv_A(*row, col_index));
                kv_A(*row, col_index) = data ? joy_core_utils_strdup(data, NULL) : NULL;
        }
}

static void datagrid_cleardata(joy_datagrid_p grid)
{
        for (size_t i = 0; i < kv_size(grid->data); i++) {
                joy_dg_row_t* row = &kv_A(grid->data, i);
                for (size_t j = 0; j < kv_size(*row); j++)
                        if (kv_A(*row, j)) { free(kv_A(*row, j)); kv_A(*row, j) = NULL; }
                kv_destroy(*row);
        }
        kv_destroy(grid->data);
        grid->row_count = grid->has_header ? 1 : 0;
}


void joy_r2d_datagrid_destroy(joy_datagrid_p grid)
{
        if (!grid) return;
        if (grid->has_header) {
                for (int i = 0; i < grid->col_count; i++)
                        if (grid->headers[i]) joy_r2d_text_destroy(grid->headers[i]);
        }
        datagrid_cleardata(grid);
        free(grid->col_widths);
        free(grid);
}

void joy_r2d_datagrid_draw(joy_datagrid_p grid, joy_r2d_p r2d)
{
        // 计算可见行范围
        int start_row = grid->scroll_offset;
        int end_row = start_row + grid->visible_rows;
        if (end_row > grid->row_count) end_row = grid->row_count;

        // 绘制背景
        joy_color_t bg_color = grid->bg_color;
        joy_r2d_set_color(r2d, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        joy_rect_t bg_rect = { grid->x, grid->y, grid->width, grid->height };
        joy_r2d_shape_draw_rectangle(r2d, "fill", bg_rect);

        // 获取网格线和文字颜色
        joy_color_t grid_color = grid->grid_color;
        joy_color_t text_color = grid->text_color;
        joy_color_t header_bg_color = grid->header_bg_color;
        joy_color_t selected_color = grid->selected_color;

        // 绘制表头
        joy_r2d_set_color(r2d, header_bg_color.r, header_bg_color.g, header_bg_color.b, header_bg_color.a);
        joy_rect_t header_rect = { grid->x, grid->y, grid->width, grid->row_height };
        joy_r2d_shape_draw_rectangle(r2d, "fill", header_rect);


        {
                int x = grid->x;
                int y = grid->y;
                for (int col = 0; col < grid->col_count; col++) {
                        joy_text_p tex = grid->headers[col];
                        joy_r2d_text_print(r2d, tex, x, y);

                        // 绘制垂直分隔线
                        joy_r2d_set_color(r2d, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
                        joy_r2d_shape_draw_line(r2d, x + grid->col_widths[col], y, x + grid->col_widths[col], y + grid->row_height);

                        x += grid->col_widths[col];
                }
        }

        // 绘制数据行
        for (int row = start_row; row < end_row; row++) {
                int y = grid->y + (row - start_row) * grid->row_height + grid->row_height;

                // 绘制选中行背景
                if (row == grid->selected_row) {
                        joy_r2d_set_color(r2d, selected_color.r, selected_color.g, selected_color.b, selected_color.a);
                        joy_rect_t selected_rect = { grid->x, y, grid->width, grid->row_height };
                        joy_r2d_shape_draw_rectangle(r2d, "fill", selected_rect);
                }
                else {
                        /* 非选中行默认无背景 */
                }

                // 绘制单元格内容和分隔线
                int x = grid->x;
                for (int col = 0; col < grid->col_count; col++) {
                        if ((size_t)row < kv_size(grid->data) && (size_t)col < kv_size(kv_A(grid->data, row))) {
                                char* text = kv_A(kv_A(grid->data, row), col);
                                if (text) {
                                        joy_color_t c = { 0,0,0,255 };
                                        joy_r2d_text_printx(r2d, grid->font, text, (int)strlen(text), c, (float)x + 2, (float)y);
                                }
                        }

                        // 绘制垂直分隔线
                        joy_r2d_set_color(r2d, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
                        joy_r2d_shape_draw_line(r2d, x + grid->col_widths[col], y, x + grid->col_widths[col], y + grid->row_height);

                        x += grid->col_widths[col];
                }

                // 绘制水平分隔线
                joy_r2d_set_color(r2d, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
                joy_r2d_shape_draw_line(r2d, grid->x, y + grid->row_height, grid->x + grid->width, y + grid->row_height);
        }

        // 绘制外边框
        joy_r2d_set_color(r2d, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
        joy_r2d_shape_draw_rectangle(r2d, "line", bg_rect);
}

int joy_r2d_datagrid_hit_row(joy_datagrid_p grid, float x, float y)
{
        if (!grid) return -1;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(grid->r2d, x, y);
        joy_rectangle_t rect = { (float)grid->x, (float)(grid->y + grid->row_height), (float)grid->width, (float)grid->height };
        if (!joy_collision_2d_check_point_rectangle(logic, rect))
                return -1;
        int clicked_row = grid->scroll_offset + ((int)(logic.y - rect.y)) / grid->row_height;
        if (clicked_row < grid->row_count) return clicked_row;
        return -1;
}

void joy_r2d_datagrid_scroll(joy_datagrid_p grid, int lines)
{
        if (!grid) return;
        grid->scroll_offset -= lines;
        int max_scroll = grid->row_count - grid->visible_rows;
        if (max_scroll < 0) max_scroll = 0;
        if (grid->scroll_offset < 0) grid->scroll_offset = 0;
        if (grid->scroll_offset > max_scroll) grid->scroll_offset = max_scroll;
}

void joy_r2d_datagrid_set_selected_row(joy_datagrid_p grid, int row)
{
        if (grid) grid->selected_row = row;
}

joy_radio_p joy_r2d_radio_create(float fx, float fy, float size_frac, const char* label, int group_id)
{
        int lw = 480, lh = 320;
        int x = (int)(fx * (float)lw), y = (int)(fy * (float)lh), size = (int)(size_frac * (float)lw);
        joy_radio_p radio;
        radio = (joy_radio_p)malloc(sizeof(joy_radio_t));
        if (!radio) return NULL;

        radio->x = x;
        radio->y = y;
        radio->size = size;
        // 记住比例：拿到 r2d 后按真实逻辑分辨率重算（见 radio_update_layout）
        radio->fx = fx;
        radio->fy = fy;
        radio->size_frac = size_frac;
        radio->r2d = NULL;
        radio->checked = false;
        radio->group_id = group_id;
        radio->hover = false;

        // 复制标签文本
        if (label) {
                radio->label = joy_core_utils_strdup(label, NULL);
        }
        else {
                radio->label = NULL;
        }

        // 设置点击区域(包含标签宽度)
        int label_width = label ? strlen(label) * 10 : 0; // 估算标签宽度
        radio->hit_area.x = x;
        radio->hit_area.y = y;
        radio->hit_area.w = size + 5 + label_width;
        radio->hit_area.h = size;

        return radio;
}

void joy_r2d_radio_destroy(joy_radio_p radio)
{
        if (!radio) return;

        if (radio->label) {
                free(radio->label);
        }

        free(radio);
}


/* 按"当前"逻辑分辨率把比例换算成像素（位置/尺寸/命中区），同 checkbox_update_layout。
 * joy_r2d_radio_create 拿不到 r2d（只能拿到比例），真正的换算推迟到这里。 */
static void radio_update_layout(joy_radio_p rb)
{
        if (!rb || !rb->r2d) return;

        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(rb->r2d, &lw, &lh, NULL);
        if (lw <= 0 || lh <= 0) return;

        const float scale = (float)lw / 480.0f;      // 相对设计稿 480x320 的缩放
        rb->x    = (int)(rb->fx * (float)lw);
        rb->y    = (int)(rb->fy * (float)lh);
        rb->size = (int)(rb->size_frac * (float)lw);

        int label_width = rb->label ? (int)(strlen(rb->label) * 10 * scale) : 0;
        rb->hit_area.x = rb->x;
        rb->hit_area.y = rb->y;
        rb->hit_area.w = rb->size + (int)(5.0f * scale) + label_width;
        rb->hit_area.h = rb->size;
}

void joy_r2d_radio_render(joy_radio_p radio, joy_r2d_p r2d)
{
        if (!radio) return;

        // 可能刚拿到 r2d：先按当前逻辑分辨率重算布局
        if (r2d) radio->r2d = r2d;
        radio_update_layout(radio);

        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        const float scale = (lw > 0) ? ((float)lw / 480.0f) : 1.0f;

        float center_x = radio->x + radio->size / 2;
        float center_y = radio->y + radio->size / 2;
        float r1 = radio->size / 2;
        float r2 = r1 - 2.0f * scale;    // 描边/内圈厚度是设计稿像素，按分辨率换算
        float r3 = r2 - 4.0f * scale;

        // 使用足够大的分段数绘制平滑圆形
        const int segments = 99; // 增加分段数使圆形平滑
        radio->r1_color.r = 0;
        radio->r1_color.g = 0;
        radio->r1_color.b = 255;
        radio->r1_color.a = 255;

        radio->r2_color.r = 255;
        radio->r2_color.g = 255;
        radio->r2_color.b = 255;
        radio->r2_color.a = 255;

        radio->r3_color.r = 0;
        radio->r3_color.g = 0;
        radio->r3_color.b = 255;
        radio->r3_color.a = 255;


        /* 外圈 */
        joy_r2d_set_color(r2d, radio->r1_color.r, radio->r1_color.g, radio->r1_color.b, radio->r1_color.a);
        joy_r2d_shape_draw_circle(r2d, "fill", (joy_vec2_t) { center_x, center_y }, r1, 32);

        /* 内圈 */
        joy_r2d_set_color(r2d, radio->r2_color.r, radio->r2_color.g, radio->r2_color.b, radio->r2_color.a);
        joy_r2d_shape_draw_circle(r2d, "fill", (joy_vec2_t) { center_x, center_y }, r2, 32);

        /* 选中标记 */
        if (radio->checked) {
                joy_r2d_set_color(r2d, radio->r3_color.r, radio->r3_color.g, radio->r3_color.b, radio->r3_color.a);
                joy_r2d_shape_draw_circle(r2d, "fill", (joy_vec2_t) { center_x, center_y }, r3, 32);
        }

        // 渲染标签文本
        if (radio->label) {
                /* 标签文本 — 由外部传入 font 后使用 joy_r2d_text_printx 渲染 */
        }
}

bool joy_r2d_radio_is_pressed(joy_radio_p radio, float x, float y)
{
        if (!radio)
                return false;
        joy_rectangle_t hit_area;
        joy_vec2_t pt = { x, y };

        hit_area.x = radio->hit_area.x;
        hit_area.y = radio->hit_area.y;
        hit_area.width = radio->hit_area.w;
        hit_area.height = radio->hit_area.h;

        if (joy_collision_2d_check_point_rectangle(pt, hit_area)) {
                radio->checked = !radio->checked;
                return true;
        }
        return false;
}

void joy_r2d_radio_set_hovered(joy_radio_p radio, bool v)
{
        if (radio) radio->hover = v;
}


joy_checkbox_p joy_r2d_checkbox_create(float fx, float fy, float size_frac, const char* label)
{
        int lw = 480, lh = 320;
        int x = (int)(fx * (float)lw), y = (int)(fy * (float)lh), size = (int)(size_frac * (float)lw);
        joy_checkbox_p cb;
        cb = malloc(sizeof(joy_checkbox_t));
        if (!cb) return NULL;

        cb->x = x;
        cb->y = y;
        cb->size = size;
        // 记住比例：拿到 r2d 后会按真实逻辑分辨率重算（见 checkbox_update_layout）
        cb->fx = fx;
        cb->fy = fy;
        cb->size_frac = size_frac;
        cb->r2d = NULL;
        cb->state = JOY_CHECKBOX_UNCHECKED;
        cb->label = label ? joy_core_utils_strdup(label, NULL) : NULL;
        cb->hover = false;
        cb->enabled = true;

        // 设置默认颜色
        cb->box_color = (joy_color_t){ 0, 0, 0, 255 };        // 黑色边框
        cb->check_color = (joy_color_t){ 0, 120, 0, 255 };    // 绿色勾选
        cb->text_color = (joy_color_t){ 0, 0, 0, 255 };       // 黑色文本
        cb->hover_color = (joy_color_t){ 200, 200, 200, 255 };// 浅灰色悬停

        // 设置点击区域(包含标签)
        int label_width = 0;
        if (label) {
                // 估算标签宽度(实际渲染时需要精确计算)
                label_width = strlen(label) * 8;
        }
        cb->hit_area.x = x;
        cb->hit_area.y = y;
        cb->hit_area.w = size + 5 + label_width;
        cb->hit_area.h = size;
        return cb;
}


void joy_r2d_checkbox_destroy(joy_checkbox_p checkbox)
{
        if (!checkbox) return;

        if (checkbox->label) {
                free(checkbox->label);
        }

        free(checkbox);
}

void joy_r2d_checkbox_set_state(joy_checkbox_p checkbox, joy_checkbox_state_t state)
{
        if (!checkbox) return;
        checkbox->state = state;
}

joy_checkbox_state_t joy_r2d_checkbox_get_state(joy_checkbox_p checkbox)
{
        return checkbox ? checkbox->state : JOY_CHECKBOX_UNCHECKED;
}

void joy_r2d_checkbox_set_r2d(joy_checkbox_p checkbox, joy_r2d_p r2d)
{
        if (!checkbox) return;
        checkbox->r2d = r2d;
}

void joy_r2d_checkbox_set_position(joy_checkbox_p checkbox, int x, int y)
{
        if (!checkbox) return;
        int dx = x - checkbox->x;
        int dy = y - checkbox->y;
        checkbox->x = x;
        checkbox->y = y;
        checkbox->hit_area.x += dx;
        checkbox->hit_area.y += dy;
}

void joy_r2d_checkbox_set_text_color(joy_checkbox_p checkbox, joy_color_t color)
{
        if (!checkbox) return;
        checkbox->text_color = color;
}

void joy_r2d_checkbox_toggle(joy_checkbox_p checkbox)
{
        if (!checkbox || !checkbox->enabled) return;

        if (checkbox->state == JOY_CHECKBOX_UNCHECKED) {
                checkbox->state = JOY_CHECKBOX_CHECKED;
        }
        else {
                checkbox->state = JOY_CHECKBOX_UNCHECKED;
        }
}


// 处理复选框点击事件
bool joy_r2d_checkbox_is_pressed(joy_checkbox_p cb, float x, float y)
{
        if (!cb || !cb->enabled) return false;
        // 命中前先按当前逻辑分辨率重算命中区，否则分辨率改变后点击位置对不上
        checkbox_update_layout(cb);
        joy_vec2_t pt = { x, y };
        // 与 button/slider/inputbox 一致：窗口坐标转逻辑坐标，保证全屏下命中正确
        if (cb->r2d) pt = joy_r2d_coordinates_from_window(cb->r2d, x, y);
        joy_rectangle_t hit_area;
        hit_area.x = cb->hit_area.x;
        hit_area.y = cb->hit_area.y;
        hit_area.width = cb->hit_area.w;
        hit_area.height = cb->hit_area.h;
        if (joy_collision_2d_check_point_rectangle(pt, hit_area)) {
                joy_r2d_checkbox_toggle(cb);
                return true;
        }
        return false;
}

void joy_r2d_checkbox_set_hovered(joy_checkbox_p cb, bool v)
{
        if (cb) cb->hover = v;
}

void joy_r2d_checkbox_set_colors(joy_checkbox_p checkbox, joy_color_t box, joy_color_t check, joy_color_t hover)
{
        if (!checkbox) return;
        checkbox->box_color = box;
        checkbox->check_color = check;
        checkbox->hover_color = hover;
}

/* 按"当前"逻辑分辨率把比例换算成像素（位置/尺寸/命中区）。
 * 创建时只能拿到比例（没有 r2d），所以真正的换算推迟到这里。
 * 逻辑分辨率改变后，复选框仍保持原有的相对位置与大小。 */
static void checkbox_update_layout(joy_checkbox_p cb)
{
        if (!cb || !cb->r2d) return;

        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(cb->r2d, &lw, &lh, NULL);
        if (lw <= 0 || lh <= 0) return;

        const float scale = (float)lw / 480.0f;      // 相对设计稿 480x320 的缩放
        cb->x    = (int)(cb->fx * (float)lw);
        cb->y    = (int)(cb->fy * (float)lh);
        cb->size = (int)(cb->size_frac * (float)lw);

        int label_width = 0;
        if (cb->label) label_width = (int)(strlen(cb->label) * 8 * scale);
        cb->hit_area.x = cb->x;
        cb->hit_area.y = cb->y;
        cb->hit_area.w = cb->size + (int)(5.0f * scale) + label_width;
        cb->hit_area.h = cb->size;
}

void joy_r2d_checkbox_draw(joy_checkbox_p checkbox, joy_r2d_p r2d, void* font)
{
        if (!checkbox) return;

        // 可能刚 set_r2d：先按当前逻辑分辨率重算布局
        if (r2d) checkbox->r2d = r2d;
        checkbox_update_layout(checkbox);

        float cx = (float)checkbox->x;
        float cy = (float)checkbox->y;
        float sz = (float)checkbox->size;

        uint8_t alpha = checkbox->enabled ? 255 : 100;

        // === 框体背景 ===
        joy_r2d_set_color(r2d, checkbox->box_color.r, checkbox->box_color.g, checkbox->box_color.b, alpha);
        joy_rect_t fill_rect = { cx, cy, sz, sz };
        joy_r2d_shape_draw_rectangle(r2d, "fill", fill_rect);

        // === 悬停时变色 ===
        if (checkbox->hover && checkbox->enabled) {
                joy_r2d_set_color(r2d, checkbox->hover_color.r, checkbox->hover_color.g, checkbox->hover_color.b, 255);
                joy_r2d_shape_draw_rectangle(r2d, "fill", fill_rect);
        }

        // === 边框 ===
        joy_r2d_set_color(r2d, 255, 255, 255, alpha);
        joy_r2d_shape_draw_rectangle(r2d, "line", fill_rect);

        // === 选中状态 ===
        if (checkbox->state == JOY_CHECKBOX_CHECKED && checkbox->enabled) {
                joy_r2d_set_color(r2d, checkbox->check_color.r, checkbox->check_color.g, checkbox->check_color.b, 255);
                float p = sz * 0.18f;
                float m = sz * 0.5f;
                float e = sz - p;
                float h1 = sz * 0.38f;
                float h2 = sz - p;
                joy_vec2_t check[3] = {
                    {cx + p,      cy + m},
                    {cx + h1,     cy + h2},
                    {cx + e,      cy + p}
                };
                // 加粗：画 4 条偏移线（-1.5, -0.5, 0.5, 1.5）
                for (float off = -1.5f; off <= 1.5f; off += 1.0f) {
                        joy_vec2_t thick[3] = {
                            {check[0].x + off * 0.4f, check[0].y + off * 0.4f},
                            {check[1].x + off * 0.4f, check[1].y + off * 0.4f},
                            {check[2].x + off * 0.4f, check[2].y + off * 0.4f}
                        };
                        joy_r2d_draw_lines(r2d, thick, 3);
                }
                joy_r2d_draw_lines(r2d, check, 3);
        }
        else if (checkbox->state == JOY_CHECKBOX_INDETERMINATE && checkbox->enabled) {
                // 不确定状态：白色短横线
                joy_r2d_set_color(r2d, 255, 255, 255, 255);
                float p = sz * 0.25f;
                joy_vec2_t dash[2] = {
                    {cx + p, cy + sz * 0.5f},
                    {cx + sz - p, cy + sz * 0.5f}
                };
                joy_r2d_shape_draw_line(r2d, dash[0].x, dash[0].y, dash[1].x, dash[1].y);
        }

        // === 禁用遮罩 ===
        if (!checkbox->enabled) {
                joy_r2d_set_color(r2d, 200, 200, 200, 120);
                joy_r2d_shape_draw_rectangle(r2d, "fill", fill_rect);
        }

        // === 标签文本 ===
        if (checkbox->label && font) {
                joy_font_p f = (joy_font_p)font;
                joy_color_t tc = checkbox->enabled ? checkbox->text_color : (joy_color_t){ 160, 160, 160, 255 };
                // 标签与框的间距 8 是设计稿像素，按分辨率换算
                int lw = 0, lh = 0;
                joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
                const float scale = (lw > 0) ? ((float)lw / 480.0f) : 1.0f;
                joy_r2d_text_printx(r2d, f, checkbox->label, (int)strlen(checkbox->label), tc,
                        cx + sz + 8.0f * scale,
                        cy + (sz - (float)joy_r2d_font_get_size(f)) / 2);
        }
}




// 初始化摇杆
joy_joystick_p joy_r2d_joystick_create(joy_r2d_p r2d, float cx, float cy, float radius_frac)
{
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        float x = cx * (float)lw, y = cy * (float)lh, radius = radius_frac * (float)lw;
        joy_joystick_p joystick;
        joystick = (joy_joystick_p)malloc(sizeof(joy_joystick_t));

        joystick->r2d = r2d;
        joystick->center.x = x;
        joystick->center.y = y;
        joystick->radius = radius;
        joystick->handle_radius = radius * 0.4f;

        joystick->background.x = x - radius;
        joystick->background.y = y - radius;
        joystick->background.w = radius * 2;
        joystick->background.h = radius * 2;

        joystick->handle.x = x - joystick->handle_radius;
        joystick->handle.y = y - joystick->handle_radius;
        joystick->handle.w = joystick->handle_radius * 2;
        joystick->handle.h = joystick->handle_radius * 2;

        joystick->direction.x = 0.0f;
        joystick->direction.y = 0.0f;
        joystick->magnitude = 0.0f;
        joystick->deadzone = 0.1f;
        joystick->is_dragging = false;
        joystick->is_touch_active = false;
        joystick->touch_id = -1;
        joystick->base_image = NULL;
        joystick->thumb_image = NULL;
        joystick->base_default = NULL;
        joystick->thumb_default = NULL;
        for (int i = 0; i < 5; i++) { joystick->mode_base[i] = NULL; joystick->mode_thumb[i] = NULL; }
        joystick->dir_mode = JOYSTICK_DIR_OMNI;
        return joystick;
}

void joy_r2d_joystick_destroy(joy_joystick_p joystick)
{
        if (!joystick) return;
        // base_image/thumb_image 只是指向 default 或某 mode 纹理的指针，不单独释放，避免重复释放
        if (joystick->base_default) joy_r2d_image_destroy(joystick->base_default);
        if (joystick->thumb_default) joy_r2d_image_destroy(joystick->thumb_default);
        for (int i = 0; i < 5; i++) {
                if (joystick->mode_base[i]) joy_r2d_image_destroy(joystick->mode_base[i]);
                if (joystick->mode_thumb[i]) joy_r2d_image_destroy(joystick->mode_thumb[i]);
        }
        free(joystick);
}

// 设置摇杆默认纹理：底圈(base) 与摇杆头(thumb)。传空串 / NULL 则清除（退回纯绘制）。
// 设为默认集合，并立即作为当前显示（除非后续有模式专属纹理）。
void joy_r2d_joystick_set_textures(joy_joystick_p joystick, const char* base_path, const char* thumb_path)
{
        if (!joystick) return;
        if (joystick->base_default) { joy_r2d_image_destroy(joystick->base_default); joystick->base_default = NULL; }
        if (joystick->thumb_default) { joy_r2d_image_destroy(joystick->thumb_default); joystick->thumb_default = NULL; }
        if (base_path && base_path[0])
                joystick->base_default = joy_r2d_image_create(joystick->r2d, base_path);
        if (thumb_path && thumb_path[0])
                joystick->thumb_default = joy_r2d_image_create(joystick->r2d, thumb_path);
        // 立即作为当前显示（若当前模式无专属纹理，则显示默认）
        joystick->base_image = joystick->base_default;
        joystick->thumb_image = joystick->thumb_default;
}

// 为某个方向模式注册专属纹理；切换该模式时自动换上，未注册则回退默认纹理。
void joy_r2d_joystick_set_textures_for_mode(joy_joystick_p joystick, joy_joystick_dir_mode mode, const char* base_path, const char* thumb_path)
{
        if (!joystick) return;
        if (mode < 0 || mode > JOYSTICK_DIR_HORIZONTAL) return;
        if (joystick->mode_base[(int)mode]) { joy_r2d_image_destroy(joystick->mode_base[(int)mode]); joystick->mode_base[(int)mode] = NULL; }
        if (joystick->mode_thumb[(int)mode]) { joy_r2d_image_destroy(joystick->mode_thumb[(int)mode]); joystick->mode_thumb[(int)mode] = NULL; }
        if (base_path && base_path[0])
                joystick->mode_base[(int)mode] = joy_r2d_image_create(joystick->r2d, base_path);
        if (thumb_path && thumb_path[0])
                joystick->mode_thumb[(int)mode] = joy_r2d_image_create(joystick->r2d, thumb_path);
        // 若当前正好是该模式，立即切换显示
        if (joystick->dir_mode == mode) {
                joystick->base_image = joystick->mode_base[(int)mode] ? joystick->mode_base[(int)mode] : joystick->base_default;
                joystick->thumb_image = joystick->mode_thumb[(int)mode] ? joystick->mode_thumb[(int)mode] : joystick->thumb_default;
        }
}

// 设置摇杆位置
void joy_r2d_joystick_set_position(joy_joystick_p joystick, float x, float y)
{
        joystick->center.x = x;
        joystick->center.y = y;

        joystick->background.x = x - joystick->radius;
        joystick->background.y = y - joystick->radius;

        if (!joystick->is_dragging) {
                joystick->handle.x = x - joystick->handle_radius;
                joystick->handle.y = y - joystick->handle_radius;
        }
}

// 设置摇杆半径（底圈与手柄同步缩放）。用于逻辑分辨率变化时保持设计尺寸。
void joy_r2d_joystick_set_radius(joy_joystick_p joystick, float radius)
{
        if (!joystick || radius <= 0.0f) return;
        joystick->radius = radius;
        joystick->handle_radius = radius * 0.4f;

        joystick->background.x = joystick->center.x - radius;
        joystick->background.y = joystick->center.y - radius;
        joystick->background.w = radius * 2.0f;
        joystick->background.h = radius * 2.0f;

        if (!joystick->is_dragging) {
                joystick->handle.x = joystick->center.x - joystick->handle_radius;
                joystick->handle.y = joystick->center.y - joystick->handle_radius;
                joystick->handle.w = joystick->handle_radius * 2.0f;
                joystick->handle.h = joystick->handle_radius * 2.0f;
        }
}

// 设置方向约束模式（全方向/四向/八向/仅上/下/左/右）
void joy_r2d_joystick_set_dir_mode(joy_joystick_p joystick, joy_joystick_dir_mode mode)
{
        if (!joystick) return;
        joystick->dir_mode = mode;
        // 切换显示纹理：优先该模式专属，否则默认
        joystick->base_image = joystick->mode_base[(int)mode] ? joystick->mode_base[(int)mode] : joystick->base_default;
        joystick->thumb_image = joystick->mode_thumb[(int)mode] ? joystick->mode_thumb[(int)mode] : joystick->thumb_default;
}

// 前向声明（定义见下方）；供 joy_r2d_joystick_set_vector 在定义前调用，避免 static 函数“定义前使用”被当成隐式声明
static void joystick_apply_dir_mode(joy_joystick_dir_mode mode, joy_vec2_t* d);

// 以方向向量驱动摇杆（无需触摸），用于键盘/外部输入同步显示。
// (dx,dy) 为方向（屏幕坐标，y 向下）；长度>0 即视为按下（满量程），为 0 则回中。
// 同样受 dir_mode 约束（小圆只沿允许方向滑动），视觉与触摸一致。
void joy_r2d_joystick_set_vector(joy_joystick_p j, float dx, float dy)
{
        if (!j) return;
        if (dx == 0.0f && dy == 0.0f) { joy_r2d_joystick_reset(j); return; }
        j->is_dragging = true;
        j->is_touch_active = true;
        float len = sqrtf(dx * dx + dy * dy);
        joy_vec2_t d = { dx / len, dy / len };
        joystick_apply_dir_mode(j->dir_mode, &d);   // 同样受方向约束
        j->direction.x = d.x;
        j->direction.y = d.y;
        j->magnitude = 1.0f;
        j->handle.x = j->center.x + d.x * j->radius - j->handle_radius;
        j->handle.y = j->center.y + d.y * j->radius - j->handle_radius;
}

// 按方向模式吸附/约束方向向量 d（屏幕坐标，y 向下；保留其长度=强度）
static void joystick_apply_dir_mode(joy_joystick_dir_mode mode, joy_vec2_t* d)
{
        float len = sqrtf(d->x * d->x + d->y * d->y);
        if (len < 1e-6f) return;
        if (mode == JOYSTICK_DIR_OMNI) return;
        float nx = d->x / len, ny = d->y / len;          // 单位方向
        if (mode == JOYSTICK_DIR_VERTICAL) {
                nx = 0.0f; ny = (ny < 0.0f) ? -1.0f : 1.0f;
        } else if (mode == JOYSTICK_DIR_HORIZONTAL) {
                ny = 0.0f; nx = (nx < 0.0f) ? -1.0f : 1.0f;
        } else {
                int steps = (mode == JOYSTICK_DIR_4WAY) ? 4 : 8;
                float step = 2.0f * 3.1415926f / (float)steps;
                float ang = atan2f(ny, nx);
                float snap = roundf(ang / step) * step;   // 对齐到最近离散方向
                nx = cosf(snap); ny = sinf(snap);
        }
        d->x = nx * len;                                  // 保留原强度
        d->y = ny * len;
}

// 更新摇杆拖拽逻辑
static void joystick_update_drag(joy_joystick_p joystick, float touch_x, float touch_y)
{
        // 计算手指到中心的偏移量
        float dx = touch_x - joystick->center.x;
        float dy = touch_y - joystick->center.y;

        // 计算距离
        float distance = sqrtf(dx * dx + dy * dy);

        // 控制距离不超过摇杆半径
        if (distance > joystick->radius) {
                dx = (dx / distance) * joystick->radius;
                dy = (dy / distance) * joystick->radius;
                distance = joystick->radius;
        }

        // 计算方向和强度
        joystick->magnitude = distance / joystick->radius;

        // 应用死区
        if (joystick->magnitude < joystick->deadzone) {
                joystick->direction.x = 0.0f;
                joystick->direction.y = 0.0f;
                joystick->magnitude = 0.0f;

                // 手柄回到中心
                joystick->handle.x = joystick->center.x - joystick->handle_radius;
                joystick->handle.y = joystick->center.y - joystick->handle_radius;
        }
        else {
                // 非死区时更新方向（并应用方向约束模式）
                joystick->direction.x = dx / joystick->radius;
                joystick->direction.y = dy / joystick->radius;
                joystick_apply_dir_mode(joystick->dir_mode, &joystick->direction);

                // 更新手柄位置（同样受方向约束：由受限后的方向反推，小圆只沿允许方向滑动）
                joystick->handle.x = joystick->center.x + joystick->direction.x * joystick->radius - joystick->handle_radius;
                joystick->handle.y = joystick->center.y + joystick->direction.y * joystick->radius - joystick->handle_radius;
        }
}

// 处理事件和响应逻辑(分辨率)
bool joy_r2d_joystick_is_dragging(joy_joystick_p j)
{
        return j ? j->is_dragging : false;
}

void joy_r2d_joystick_touch_down(joy_joystick_p j, float x, float y)
{
        if (!j) return;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(j->r2d, x, y);
        float dist_sq = distance_squared(logic.x, logic.y, j->center.x, j->center.y);
        if (dist_sq <= (j->radius * j->radius)) {
                j->is_dragging = true;
                j->is_touch_active = true;
                j->touch_id = -1;
                joystick_update_drag(j, logic.x, logic.y);
        }
}

void joy_r2d_joystick_touch_move(joy_joystick_p j, float x, float y)
{
        if (!j || !j->is_dragging) return;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(j->r2d, x, y);
        joystick_update_drag(j, logic.x, logic.y);
}

void joy_r2d_joystick_touch_up(joy_joystick_p j)
{
        if (!j) return;
        joy_r2d_joystick_reset(j);
}

// 重置摇杆
void joy_r2d_joystick_reset(joy_joystick_p joystick)
{
        joystick->direction.x = 0.0f;
        joystick->direction.y = 0.0f;
        joystick->magnitude = 0.0f;
        joystick->is_dragging = false;
        joystick->is_touch_active = false;
        joystick->touch_id = -1;

        // 手柄回到中心
        joystick->handle.x = joystick->center.x - joystick->handle_radius;
        joystick->handle.y = joystick->center.y - joystick->handle_radius;
}

// 绘制摇杆：优先使用纹理（底圈 base_image + 摇杆头 thumb_image），
// 未设置纹理时退回原纯绘制（圆形背景 + 十字 + 圆形手柄），保持向后兼容。
void joy_r2d_joystick_draw(joy_joystick_p joystick)
{
        if (!joystick) return;
        joy_r2d_p r2d = joystick->r2d;

        // ---- 底圈 ----
        if (joystick->base_image) {
                joy_r2d_image_draw1(joystick->base_image, NULL, &joystick->background);
        } else {
                joy_r2d_set_color(r2d,
                        (JOYSTICK_BG_COLOR >> 24) & 0xFF,
                        (JOYSTICK_BG_COLOR >> 16) & 0xFF,
                        (JOYSTICK_BG_COLOR >> 8) & 0xFF,
                        JOYSTICK_BG_COLOR & 0xFF);
                // 绘制圆形背景, 使用多段模拟圆形
                joy_r2d_shape_draw_circle(r2d, "line", joystick->center, joystick->radius, 32);

                // 绘制十字线
                joy_r2d_set_color(r2d, 0x66, 0x66, 0x66, 0xFF);
                joy_r2d_shape_draw_line(r2d,
                        joystick->center.x - joystick->radius, joystick->center.y,
                        joystick->center.x + joystick->radius, joystick->center.y);
                joy_r2d_shape_draw_line(r2d,
                        joystick->center.x, joystick->center.y - joystick->radius,
                        joystick->center.x, joystick->center.y + joystick->radius);
        }

        // ---- 摇杆头 ----
        if (joystick->thumb_image) {
                joy_r2d_image_draw1(joystick->thumb_image, NULL, &joystick->handle);
        } else {
                joy_color_t handle_color = joystick->is_dragging ?
                        (joy_color_t) { 0x4C, 0xAF, 0x50, 0xFF } :
                        (joy_color_t) { 0x88, 0x88, 0x88, 0xFF };
                joy_r2d_set_color(r2d, handle_color.r, handle_color.g, handle_color.b, handle_color.a);

                // 绘制圆形手柄
                joy_vec2_t handle_center = {
                        joystick->handle.x + joystick->handle_radius,
                        joystick->handle.y + joystick->handle_radius
                };
                joy_r2d_shape_draw_circle(r2d, "fill", handle_center, joystick->radius * 0.5f, 24);

                // 绘制手柄内框
                joy_r2d_set_color(r2d, handle_color.r, handle_color.g, handle_color.b, handle_color.a - 0x40);
                joy_rect_t handle_fill = {
                        joystick->handle.x + 2, joystick->handle.y + 2,
                        joystick->handle.w - 4, joystick->handle.h - 4
                };
                joy_r2d_shape_draw_rectangle(r2d, "fill", handle_fill);
        }

        // ---- 方向指示线（仅纯绘制时；纹理已用摇杆头位移表达方向）----
        if (!joystick->thumb_image && joy_r2d_joystick_get_magnitude(joystick) > 0) {
                joy_vec2_t dir = joy_r2d_joystick_get_direction(joystick);
                joy_r2d_set_color(r2d, 0, 0, 0, 100);
                joy_r2d_shape_draw_line(r2d,
                        joystick->center.x,
                        joystick->center.y,
                        joystick->center.x + dir.x * joystick->radius,
                        joystick->center.y + dir.y * joystick->radius);
        }
}

// 获取摇杆方向
joy_vec2_t joy_r2d_joystick_get_direction(joy_joystick_p joystick)
{
        return joystick->direction;
}

// 获取摇杆强度
float joy_r2d_joystick_get_magnitude(joy_joystick_p joystick)
{
        return joystick->magnitude;
}

/* =================================================================
 * InputBox
 * ================================================================= */

#define INPUTBOX_BUF_INIT 256

joy_inputbox_p joy_r2d_inputbox_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, joy_font_p font)
{
        joy_rect_t rect = jui_frect(r2d, fx, fy, fw, fh);
        joy_inputbox_p ti = (joy_inputbox_p)calloc(1, sizeof(joy_inputbox_t));
        if (!ti) return NULL;
        ti->r2d = r2d;
        ti->rect = rect;
        ti->font = font;
        ti->bg_color = (joy_color_t){ 255, 255, 255, 255 };
        ti->border_color = (joy_color_t){ 180, 180, 180, 255 };
        ti->focus_border_color = (joy_color_t){ 66, 133, 244, 255 };
        ti->text_color = (joy_color_t){ 0, 0, 0, 255 };
        ti->cursor_color = (joy_color_t){ 0, 0, 0, 255 };
        ti->sel_color = (joy_color_t){ 100, 150, 255, 255 };
        ti->buf_len = INPUTBOX_BUF_INIT;
        ti->buffer = (char*)calloc(1, ti->buf_len);
        ti->text_len = 0;
        ti->cursor_pos = 0;
        ti->sel_start = -1;
        ti->sel_end = -1;
        ti->focused = false;
        ti->cursor_timer = joy_core_get_ticks();
        ti->cursor_visible = true;
        return ti;
}

void joy_r2d_inputbox_destroy(joy_inputbox_p ti)
{
        if (!ti) return;
        free(ti->buffer);
        free(ti);
}


/* ---- 选中支持 ---- */

static void inputbox_clear_sel(joy_inputbox_p ti)
{
        ti->sel_start = ti->sel_end = -1;
}

static bool inputbox_has_sel(joy_inputbox_p ti)
{
        return ti->sel_start >= 0 && ti->sel_end >= 0 && ti->sel_start != ti->sel_end;
}

/* 删除选中文本，将光标置于选中起始 */
static void inputbox_delete_sel(joy_inputbox_p ti)
{
        if (!inputbox_has_sel(ti)) return;
        int a = ti->sel_start, b = ti->sel_end;
        if (a > b) { int t = a; a = b; b = t; }
        memmove(ti->buffer + a, ti->buffer + b, ti->text_len - b);
        ti->text_len -= (b - a);
        ti->cursor_pos = a;
        inputbox_clear_sel(ti);
        ti->buffer[ti->text_len] = '\0';
}


void joy_r2d_inputbox_set_text(joy_inputbox_p ti, const char* text)
{
        if (!ti || !text) return;
        int len = (int)strlen(text);
        if (len >= ti->buf_len) {
                ti->buf_len = len + 64;
                ti->buffer = (char*)realloc(ti->buffer, ti->buf_len);
        }
        memcpy(ti->buffer, text, len);
        ti->buffer[len] = '\0';
        ti->text_len = len;
        ti->cursor_pos = len;
        inputbox_clear_sel(ti);
        ti->scroll_offset = 0;
        if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
}

const char* joy_r2d_inputbox_get_text(joy_inputbox_p ti)
{
        return ti ? ti->buffer : "";
}

void joy_r2d_inputbox_set_callback(joy_inputbox_p ti,
        void (*submit)(joy_inputbox_p, void*),
        void (*change)(joy_inputbox_p, void*),
        void* userdata)
{
        if (!ti) return;
        ti->on_submit = submit;
        ti->on_change = change;
        ti->cb_userdata = userdata;
}

static int inputbox_cursor_x(joy_inputbox_p ti);
static int inputbox_cursor_x_at(joy_inputbox_p ti, int pos);

/* 滚动到光标可见，并尽量填充右侧空白 */
static void inputbox_scroll_to_cursor(joy_inputbox_p ti)
{
        int cw = inputbox_cursor_x(ti);
        int visible_w = (int)ti->rect.w - 8;

        /* 1. 保证光标可见 */
        if (cw - ti->scroll_offset > visible_w)
                ti->scroll_offset = cw - visible_w;
        else if (cw - ti->scroll_offset < 0)
                ti->scroll_offset = cw;

        /* 2. 如果文字总宽度小于可见区域，直接左对齐 */
        int total_w = inputbox_cursor_x_at(ti, ti->text_len);
        if (total_w < visible_w)
                ti->scroll_offset = 0;

        /* 3. 如果右侧出现空白（删除文字后），把隐藏的左侧文字拉出来 */
        int rightmost_visible = ti->scroll_offset + visible_w;
        if (rightmost_visible > total_w && total_w > visible_w)
                ti->scroll_offset = total_w - visible_w;

        if (ti->scroll_offset < 0)
                ti->scroll_offset = 0;
}

/* 在 buffer 的 cursor_pos 处插入 len 字节 */
void joy_r2d_inputbox_insert(joy_inputbox_p ti, const char* s, int len)
{
        /* 有选中时先删除选中文本 */
        if (inputbox_has_sel(ti)) inputbox_delete_sel(ti);

        if (ti->text_len + len >= ti->buf_len) {
                while (ti->text_len + len >= ti->buf_len)
                        ti->buf_len *= 2;
                ti->buffer = (char*)realloc(ti->buffer, ti->buf_len);
        }
        memmove(ti->buffer + ti->cursor_pos + len,
                ti->buffer + ti->cursor_pos,
                ti->text_len - ti->cursor_pos);
        memcpy(ti->buffer + ti->cursor_pos, s, len);
        ti->text_len += len;
        ti->cursor_pos += len;
        ti->buffer[ti->text_len] = '\0';
        if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
        inputbox_scroll_to_cursor(ti);
}

/* 移动光标并清除选中 */
static void inputbox_move_cursor(joy_inputbox_p ti, int pos)
{
        ti->cursor_pos = pos;
        inputbox_clear_sel(ti);
        inputbox_scroll_to_cursor(ti);
}

/* 扩展选中到光标位置 */
static void inputbox_update_sel(joy_inputbox_p ti)
{
        if (ti->sel_start < 0) ti->sel_start = ti->cursor_pos;
        ti->sel_end = ti->cursor_pos;
}

/* ---- end 选中支持 ---- */

void joy_r2d_inputbox_cursor_left(joy_inputbox_p ti)
{
        if (!ti || ti->cursor_pos <= 0) return;
        inputbox_move_cursor(ti, ti->cursor_pos - 1);
}

void joy_r2d_inputbox_cursor_right(joy_inputbox_p ti)
{
        if (!ti || ti->cursor_pos >= ti->text_len) return;
        inputbox_move_cursor(ti, ti->cursor_pos + 1);
}

void joy_r2d_inputbox_cursor_home(joy_inputbox_p ti)
{
        if (!ti) return;
        inputbox_move_cursor(ti, 0);
        ti->scroll_offset = 0;
}

void joy_r2d_inputbox_cursor_end(joy_inputbox_p ti)
{
        if (!ti) return;
        inputbox_move_cursor(ti, ti->text_len);
        inputbox_scroll_to_cursor(ti);
}

void joy_r2d_inputbox_select_left(joy_inputbox_p ti)
{
        if (!ti || ti->cursor_pos <= 0) return;
        ti->cursor_pos--;
        inputbox_update_sel(ti);
        inputbox_scroll_to_cursor(ti);
}

void joy_r2d_inputbox_select_right(joy_inputbox_p ti)
{
        if (!ti || ti->cursor_pos >= ti->text_len) return;
        ti->cursor_pos++;
        inputbox_update_sel(ti);
        inputbox_scroll_to_cursor(ti);
}

void joy_r2d_inputbox_select_home(joy_inputbox_p ti)
{
        if (!ti) return;
        inputbox_update_sel(ti);
        ti->cursor_pos = 0;
        ti->scroll_offset = 0;
        ti->sel_end = 0;
}

void joy_r2d_inputbox_select_end(joy_inputbox_p ti)
{
        if (!ti) return;
        inputbox_update_sel(ti);
        ti->cursor_pos = ti->text_len;
        ti->sel_end = ti->text_len;
        inputbox_scroll_to_cursor(ti);
}

void joy_r2d_inputbox_select_all(joy_inputbox_p ti)
{
        if (!ti) return;
        ti->sel_start = 0;
        ti->sel_end = ti->text_len;
        ti->cursor_pos = ti->text_len;
        inputbox_scroll_to_cursor(ti);
}

/* 删除光标前一个 UTF-8 字符（退格） */
void joy_r2d_inputbox_backspace(joy_inputbox_p ti)
{
        if (inputbox_has_sel(ti)) {
                inputbox_delete_sel(ti);
                inputbox_scroll_to_cursor(ti);
                if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
                return;
        }
        if (ti->cursor_pos <= 0) return;
        int pos = ti->cursor_pos - 1;
        while (pos > 0 && ((unsigned char)ti->buffer[pos] & 0xC0) == 0x80)
                pos--;
        int ch_len = ti->cursor_pos - pos;
        memmove(ti->buffer + pos, ti->buffer + ti->cursor_pos,
                ti->text_len - ti->cursor_pos);
        ti->text_len -= ch_len;
        ti->cursor_pos = pos;
        ti->buffer[ti->text_len] = '\0';
        if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
        inputbox_scroll_to_cursor(ti);
}

/* 删除光标后一个 UTF-8 字符（Delete） */
void joy_r2d_inputbox_delete(joy_inputbox_p ti)
{
        if (inputbox_has_sel(ti)) {
                inputbox_delete_sel(ti);
                inputbox_scroll_to_cursor(ti);
                if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
                return;
        }
        if (ti->cursor_pos >= ti->text_len) return;
        int pos = ti->cursor_pos;
        int ch_len = 1;
        while (pos + ch_len < ti->text_len && ((unsigned char)ti->buffer[pos + ch_len] & 0xC0) == 0x80)
                ch_len++;
        memmove(ti->buffer + pos, ti->buffer + pos + ch_len,
                ti->text_len - pos - ch_len);
        ti->text_len -= ch_len;
        ti->buffer[ti->text_len] = '\0';
        if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
        inputbox_scroll_to_cursor(ti);
}

void joy_r2d_inputbox_paste(joy_inputbox_p ti)
{
        if (!ti) return;
        const char* clip = joy_core_get_clipboard_text();
        if (clip) joy_r2d_inputbox_insert(ti, clip, (int)strlen(clip));
}

void joy_r2d_inputbox_copy(joy_inputbox_p ti)
{
        if (!ti || ti->text_len <= 0) return;
        if (inputbox_has_sel(ti)) {
                int a = ti->sel_start, b = ti->sel_end;
                if (a > b) { int t = a; a = b; b = t; }
                int len = b - a;
                char* buf = (char*)malloc(len + 1);
                memcpy(buf, ti->buffer + a, len);
                buf[len] = '\0';
                joy_core_set_clipboard_text(buf);
                free(buf);
        }
        else {
                joy_core_set_clipboard_text(ti->buffer);
        }
}

void joy_r2d_inputbox_cut(joy_inputbox_p ti)
{
        if (!ti || ti->text_len <= 0) return;
        if (inputbox_has_sel(ti)) {
                int a = ti->sel_start, b = ti->sel_end;
                if (a > b) { int t = a; a = b; b = t; }
                int len = b - a;
                char* buf = (char*)malloc(len + 1);
                memcpy(buf, ti->buffer + a, len);
                buf[len] = '\0';
                joy_core_set_clipboard_text(buf);
                free(buf);
                inputbox_delete_sel(ti);
                ti->buffer[ti->text_len] = '\0';
                if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
                inputbox_scroll_to_cursor(ti);
        }
        else {
                joy_core_set_clipboard_text(ti->buffer);
                ti->text_len = 0;
                ti->cursor_pos = 0;
                ti->scroll_offset = 0;
                ti->buffer[0] = '\0';
                if (ti->on_change) ti->on_change(ti, ti->cb_userdata);
        }
}

/* 获取光标之前的文本宽度（像素） */
static int inputbox_cursor_x(joy_inputbox_p ti)
{
        if (ti->cursor_pos <= 0 || !ti->font) return 0;
        /* 用 joy_r2d_text_createx 生成光标前的子串，量宽度 */
        joy_color_t c = { 0,0,0,255 };
        joy_text_p tex = joy_r2d_text_createx(ti->r2d, ti->font, ti->buffer, ti->cursor_pos, c);
        if (!tex) return 0;
        int w = joy_r2d_text_get_width(tex);
        joy_r2d_text_destroy(tex);
        return w;
}

/* 获取 buffer 中从开头到 pos 字节位置的文本宽度（像素） */
static int inputbox_cursor_x_at(joy_inputbox_p ti, int pos)
{
        if (pos <= 0 || !ti->font) return 0;
        joy_color_t c = { 0,0,0,255 };
        joy_text_p tex = joy_r2d_text_createx(ti->r2d, ti->font, ti->buffer, pos, c);
        if (!tex) return 0;
        int w = joy_r2d_text_get_width(tex);
        joy_r2d_text_destroy(tex);
        return w;
}

/* 根据点击的局部 x 坐标计算光标位置 */
static int inputbox_pos_from_x(joy_inputbox_p ti, float local_x)
{
        if (!ti || ti->text_len <= 0 || local_x <= 0) return 0;
        int total_w = inputbox_cursor_x_at(ti, ti->text_len);
        if (local_x >= total_w) return ti->text_len;
        for (int i = 0; i < ti->text_len; ) {
                int next = i + 1;
                while (next < ti->text_len && ((unsigned char)ti->buffer[next] & 0xC0) == 0x80)
                        next++;
                int w = inputbox_cursor_x_at(ti, next);
                if (w >= local_x) {
                        int prev_w = inputbox_cursor_x_at(ti, i);
                        return (local_x - prev_w < w - local_x) ? i : next;
                }
                i = next;
        }
        return ti->text_len;
}

void joy_r2d_inputbox_focus(joy_inputbox_p ti)
{
        if (!ti) return;
        ti->focused = true;
        ti->cursor_timer = joy_core_get_ticks();
        ti->cursor_visible = true;
        /* 从 r2d 获取 window 并启用文本输入。
         * 注意：即使已经处于聚焦态也要调用 SDL_StartTextInput——用户可能用系统返回键
         * 手动收起了输入法（此时 SDL 会把 text_input_active / screen_keyboard_shown
         * 复位为 false，但 inputbox 自身的 focused 仍是 true）。再次点击该框时需要重新
         * 拉起键盘，否则表现为"点一下没反应、要再点一下"。
         * 键盘本来就开着时，SDL_StartTextInput 内部是 no-op（不会重复 show/start）。 */
        SDL_Window* win = SDL_GetRenderWindow((SDL_Renderer*)joy_r2d_unwrap(ti->r2d));
        if (win) SDL_StartTextInput(win);
}

void joy_r2d_inputbox_defocus(joy_inputbox_p ti)
{
        if (!ti || !ti->focused) return;
        ti->focused = false;
        inputbox_clear_sel(ti);
        SDL_Window* win = SDL_GetRenderWindow((SDL_Renderer*)joy_r2d_unwrap(ti->r2d));
        if (win) SDL_StopTextInput(win);
}

/* 仅清除焦点，不调用 SDL_StopTextInput。
 * 用于在两个输入框之间切换焦点：保持软键盘/IME 不收起。
 * 为什么不能直接走 joy_r2d_inputbox_defocus：
 *   切换焦点时若先 SDL_StopTextInput(隐藏键盘)、同一帧内紧接着另一框
 *   SDL_StartTextInput(显示键盘)，Android 的 IME hide/show 是异步投递到 UI 线程的，
 *   show 会与尚未完成的 hide 竞态被吞掉，表现为"从 A 直接点 B 唤不起输入法，
 *   必须先点输入框外让 A 失焦、隔一帧再点 B 才行"。
 * 因此：框间切换用本函数只转移焦点（键盘保持），点到所有框之外才用
 * joy_r2d_inputbox_defocus 真正收起键盘。 */
void joy_r2d_inputbox_blur(joy_inputbox_p ti)
{
        if (!ti || !ti->focused) return;
        ti->focused = false;
        inputbox_clear_sel(ti);
}

void joy_r2d_inputbox_draw(joy_inputbox_p ti)
{
        if (!ti) return;
        joy_r2d_p r = ti->r2d;
        joy_rect_t rect = ti->rect;

        /* 背景 */
        joy_r2d_set_color(r, ti->bg_color.r, ti->bg_color.g, ti->bg_color.b, ti->bg_color.a);
        joy_r2d_shape_draw_rectangle(r, "fill", rect);

        /* 边框（聚焦时高亮） */
        joy_color_t border = ti->focused ? ti->focus_border_color : ti->border_color;
        joy_r2d_set_color(r, border.r, border.g, border.b, border.a);
        joy_r2d_shape_draw_rectangle(r, "line", rect);

        /* 设置裁剪区域，防止文字绘制到 inputbox 矩形之外 */
        SDL_Renderer* sdl_r = (SDL_Renderer*)joy_r2d_unwrap(r);
        SDL_Rect clip_rect = { (int)rect.x + 2, (int)rect.y, (int)rect.w - 4, (int)rect.h };
        bool clip_was_enabled = SDL_RenderClipEnabled(sdl_r);
        SDL_Rect old_clip;
        if (clip_was_enabled)
                SDL_GetRenderClipRect(sdl_r, &old_clip);
        SDL_SetRenderClipRect(sdl_r, &clip_rect);

        /* 选中高亮（绘制在文字下方） */
        if (inputbox_has_sel(ti) && ti->font) {
                int a = ti->sel_start, b = ti->sel_end;
                if (a > b) { int t = a; a = b; b = t; }
                joy_color_t c = { 0,0,0,255 };
                joy_text_p ta = joy_r2d_text_createx(r, ti->font, ti->buffer, a, c);
                joy_text_p tb = joy_r2d_text_createx(r, ti->font, ti->buffer + a, b - a, c);
                if (ta && tb) {
                        float sx = rect.x + 4 + (float)(joy_r2d_text_get_width(ta) - ti->scroll_offset);
                        float sw = (float)joy_r2d_text_get_width(tb);
                        joy_rect_t sel_rect = { sx, rect.y + 1, sw, rect.h - 2 };
                        joy_r2d_set_color(r, ti->sel_color.r, ti->sel_color.g, ti->sel_color.b, ti->sel_color.a);
                        joy_r2d_shape_draw_rectangle(r, "fill", sel_rect);
                }
                if (ta) joy_r2d_text_destroy(ta);
                if (tb) joy_r2d_text_destroy(tb);
        }

        /* 文字 */
        if (ti->text_len > 0 && ti->font) {
                float text_x = rect.x + 4;
                float text_y = rect.y + (rect.h - (float)joy_r2d_font_get_size(ti->font)) / 2.0f;
                float ox = text_x - (float)ti->scroll_offset;

                if (inputbox_has_sel(ti)) {
                        int a = ti->sel_start, b = ti->sel_end;
                        if (a > b) { int t = a; a = b; b = t; }
                        joy_color_t white = { 255, 255, 255, 255 };
                        /* 选中前部分 */
                        if (a > 0)
                                joy_r2d_text_printx(r, ti->font, ti->buffer, a, ti->text_color, ox, text_y);
                        /* 选中部分（白色） */
                        if (b > a) {
                                float offset_before = (float)joy_r2d_text_get_width(joy_r2d_text_createx(r, ti->font, ti->buffer, a, ti->text_color));
                                joy_r2d_text_printx(r, ti->font, ti->buffer + a, b - a, white, ox + offset_before, text_y);
                        }
                        /* 选中后部分 */
                        if (b < ti->text_len) {
                                float offset_sel = (float)joy_r2d_text_get_width(joy_r2d_text_createx(r, ti->font, ti->buffer, b, ti->text_color));
                                joy_r2d_text_printx(r, ti->font, ti->buffer + b, ti->text_len - b, ti->text_color, ox + offset_sel, text_y);
                        }
                }
                else {
                        joy_r2d_text_printx(r, ti->font, ti->buffer, ti->text_len, ti->text_color, ox, text_y);
                }
        }

        /* 恢复裁剪区域 */
        if (clip_was_enabled)
                SDL_SetRenderClipRect(sdl_r, &old_clip);
        else
                SDL_SetRenderClipRect(sdl_r, NULL);

        /* 光标（聚焦时绘制） */
        if (ti->focused) {
                uint64_t now = joy_core_get_ticks();
                if (now - ti->cursor_timer >= 530) {
                        ti->cursor_visible = !ti->cursor_visible;
                        ti->cursor_timer = now;
                }
                if (ti->cursor_visible) {
                        int cx = inputbox_cursor_x(ti) - ti->scroll_offset + (int)rect.x + 4;
                        if (cx >= rect.x + 2 && cx <= rect.x + rect.w - 2) {
                                float cy = rect.y + 3;
                                float ch = rect.h - 6;
                                joy_r2d_set_color(r, ti->cursor_color.r, ti->cursor_color.g, ti->cursor_color.b, ti->cursor_color.a);
                                joy_r2d_shape_draw_line(r, (float)cx + 0.5f, cy, (float)cx + 0.5f, cy + ch);
                        }
                }
        }
}

bool joy_r2d_inputbox_is_pressed(joy_inputbox_p ti, float x, float y)
{
        if (!ti) return false;
        joy_vec2_t logic;
        joy_rectangle_t rect;
        rect.x = ti->rect.x;
        rect.y = ti->rect.y;
        rect.width = ti->rect.w;
        rect.height = ti->rect.h;
        logic = joy_r2d_coordinates_from_window(ti->r2d, x, y);
        if (joy_collision_2d_check_point_rectangle(logic, rect)) {
                joy_r2d_inputbox_focus(ti);
                inputbox_clear_sel(ti);
                float local_x = logic.x - ti->rect.x - 2 + ti->scroll_offset;
                ti->cursor_pos = inputbox_pos_from_x(ti, local_x);
                inputbox_scroll_to_cursor(ti);
                return true;
        }
        joy_r2d_inputbox_defocus(ti);
        return false;
}

bool joy_r2d_inputbox_is_hovered(joy_inputbox_p ti, float x, float y)
{
        if (!ti) return false;
        joy_vec2_t logic;
        joy_rectangle_t rect;
        rect.x = ti->rect.x;
        rect.y = ti->rect.y;
        rect.width = ti->rect.w;
        rect.height = ti->rect.h;
        logic = joy_r2d_coordinates_from_window(ti->r2d, x, y);
        ti->hovered = joy_collision_2d_check_point_rectangle(logic, rect);
        return ti->hovered;
}

bool joy_r2d_inputbox_is_focused(joy_inputbox_p ti)
{
        return ti ? ti->focused : false;
}

/* =================================================================
 * Slider
 * ================================================================= */

static float slider_thumb_x(joy_slider_p s)
{
        float range = s->max - s->min;
        if (range <= 0) return s->track.x;
        float ratio = (s->value - s->min) / range;
        return s->track.x + ratio * s->track.w;
}

static joy_rect_t slider_thumb_rect(joy_slider_p s)
{
        float tx = slider_thumb_x(s);
        float half = s->thumb_width * 0.5f;
        // 限制滑块方块不超出滑轨边界
        if (tx - half < s->track.x) tx = s->track.x + half;
        if (tx + half > s->track.x + s->track.w) tx = s->track.x + s->track.w - half;
        joy_rect_t r;
        r.x = tx - half;
        r.y = s->track.y;
        r.w = s->thumb_width;
        r.h = s->track.h;
        return r;
}

static void slider_update_value_from_x(joy_slider_p s, float logic_x)
{
        float ratio = (logic_x - s->track.x) / s->track.w;
        float v = s->min + ratio * (s->max - s->min);
        if (v < s->min) v = s->min;
        if (v > s->max) v = s->max;
        s->value = v;
}

static bool slider_thumb_hit(joy_slider_p s, float logic_x, float logic_y)
{
        joy_rect_t tr = slider_thumb_rect(s);
        return logic_x >= tr.x && logic_x <= tr.x + tr.w
                && logic_y >= tr.y && logic_y <= tr.y + tr.h;
}

joy_slider_p joy_r2d_slider_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, float min, float max, float initial)
{
        float x, y, w, h;
        jui_fpx(r2d, fx, fy, fw, fh, &x, &y, &w, &h);
        joy_slider_p s = (joy_slider_p)calloc(1, sizeof(joy_slider_t));
        if (!s) return NULL;
        s->r2d = r2d;
        s->track.x = x;
        s->track.y = y;
        s->track.w = w;
        s->track.h = h;
        s->min = min;
        s->max = max;
        s->value = initial;
        if (s->value < min) s->value = min;
        if (s->value > max) s->value = max;
        s->thumb_width = h;
        s->is_dragging = false;
        s->hovered = false;
        s->track_color = (joy_color_t){ 80,  80,  80,  255 };
        s->fill_color = (joy_color_t){ 66,  133, 244, 255 };
        s->thumb_color = (joy_color_t){ 200, 200, 200, 255 };
        s->thumb_hover_color = (joy_color_t){ 240, 240, 240, 255 };
        return s;
}

void joy_r2d_slider_destroy(joy_slider_p s)
{
        free(s);
}

void joy_r2d_slider_set_value(joy_slider_p s, float v)
{
        if (!s) return;
        if (v < s->min) v = s->min;
        if (v > s->max) v = s->max;
        s->value = v;
}

float joy_r2d_slider_get_value(joy_slider_p s)
{
        return s ? s->value : 0;
}

void joy_r2d_slider_set_range(joy_slider_p s, float min, float max)
{
        if (!s) return;
        s->min = min;
        s->max = max;
        if (s->value < min) s->value = min;
        if (s->value > max) s->value = max;
}

void joy_r2d_slider_set_colors(joy_slider_p s, joy_color_t track, joy_color_t fill, joy_color_t thumb, joy_color_t thumb_hover)
{
        if (!s) return;
        s->track_color = track;
        s->fill_color = fill;
        s->thumb_color = thumb;
        s->thumb_hover_color = thumb_hover;
}

void joy_r2d_slider_set_rect(joy_slider_p s, float x, float y, float w, float h)
{
        if (!s) return;
        s->track.x = x;
        s->track.y = y;
        s->track.w = w;
        s->track.h = h;
        s->thumb_width = h;
}

void joy_r2d_slider_draw(joy_slider_p s)
{
        if (!s) return;
        joy_r2d_p r = s->r2d;

        /* 滑轨背景 */
        joy_r2d_set_color(r, s->track_color.r, s->track_color.g, s->track_color.b, s->track_color.a);
        joy_r2d_shape_draw_rectangle(r, "fill", s->track);

        /* 已填充部分（从 track.x 到 thumb 中心） */
        float tx = slider_thumb_x(s);
        float fill_w = tx - s->track.x;
        if (fill_w > 0) {
                joy_r2d_set_color(r, s->fill_color.r, s->fill_color.g, s->fill_color.b, s->fill_color.a);
                joy_rect_t fill_rect = { s->track.x, s->track.y, fill_w, s->track.h };
                joy_r2d_shape_draw_rectangle(r, "fill", fill_rect);
        }

        /* 滑块 */
        joy_rect_t tr = slider_thumb_rect(s);
        joy_color_t tc = s->hovered ? s->thumb_hover_color : s->thumb_color;
        joy_r2d_set_color(r, tc.r, tc.g, tc.b, tc.a);
        joy_r2d_shape_draw_rectangle(r, "fill", tr);
        joy_r2d_set_color(r, 0, 0, 0, 255);
        joy_r2d_shape_draw_rectangle(r, "line", tr);
}

bool joy_r2d_slider_is_pressed(joy_slider_p s, float x, float y)
{
        if (!s) return false;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(s->r2d, x, y);
        if (slider_thumb_hit(s, logic.x, logic.y)) {
                s->is_dragging = true;
                return true;
        }
        /* 点击滑轨时跳转到该位置 */
        if (logic.x >= s->track.x && logic.x <= s->track.x + s->track.w
                && logic.y >= s->track.y && logic.y <= s->track.y + s->track.h) {
                slider_update_value_from_x(s, logic.x);
                s->is_dragging = true;
                return true;
        }
        return false;
}

void joy_r2d_slider_set_pressed(joy_slider_p s, bool v)
{
        if (s) s->is_dragging = v;
}

void joy_r2d_slider_drag_move(joy_slider_p s, float x, float y)
{
        if (!s || !s->is_dragging) return;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(s->r2d, x, y);
        slider_update_value_from_x(s, logic.x);
}

bool joy_r2d_slider_is_hovered(joy_slider_p s, float x, float y)
{
        if (!s) return false;
        joy_vec2_t logic = joy_r2d_coordinates_from_window(s->r2d, x, y);
        s->hovered = slider_thumb_hit(s, logic.x, logic.y);
        return s->hovered;
}

void joy_r2d_slider_set_hovered(joy_slider_p s, bool v)
{
        if (s) s->hovered = v;
}

/* ===== Popup ===== */
#define POPUP_CLOSE_SIZE 24
#define POPUP_PADDING 12
#define POPUP_TITLE_HEIGHT 32

joy_popup_p joy_r2d_popup_create(joy_r2d_p r2d, float fw, float fh)
{
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        float w = fw * (float)lw, h = fh * (float)lh;
        joy_popup_p p = (joy_popup_p)calloc(1, sizeof(joy_popup_t));
        if (!p) return NULL;
        p->r2d = r2d;
        p->w = w; p->h = h;
        p->x = 0; p->y = 0;
        p->title_height = POPUP_TITLE_HEIGHT;
        p->padding = POPUP_PADDING;
        p->close_size = POPUP_CLOSE_SIZE;
        p->visible = true;
        p->closeable = true;
        /* Default colors */
        p->bg_color = (joy_color_t){ 45, 45, 55, 240 };
        p->border_color = (joy_color_t){ 80, 80, 100, 255 };
        p->title_bg_color = (joy_color_t){ 55, 55, 70, 240 };
        p->content_color = (joy_color_t){ 220, 220, 230, 255 };
        p->close_color = (joy_color_t){ 200, 60, 60, 255 };
        p->close_hover_color = (joy_color_t){ 255, 80, 80, 255 };
        p->title_text_color = (joy_color_t){ 255, 255, 255, 255 };
        p->title[0] = '\0';
        p->content[0] = '\0';
        return p;
}

void joy_r2d_popup_destroy(joy_popup_p p)
{
        free(p);
}

void joy_r2d_popup_set_title(joy_popup_p p, const char* text, int len)
{
        if (!p || !text) return;
        if (len < 0) len = (int)strlen(text);
        if (len >= JOY_POPUP_MAX_TITLE) len = JOY_POPUP_MAX_TITLE - 1;
        memcpy(p->title, text, len);
        p->title[len] = '\0';
}

void joy_r2d_popup_set_content(joy_popup_p p, const char* text, int len)
{
        if (!p || !text) return;
        if (len < 0) len = (int)strlen(text);
        if (len >= JOY_POPUP_MAX_CONTENT) len = JOY_POPUP_MAX_CONTENT - 1;
        memcpy(p->content, text, len);
        p->content[len] = '\0';
}

void joy_r2d_popup_set_position(joy_popup_p p, float x, float y)
{
        if (p) { p->x = x; p->y = y; }
}

void joy_r2d_popup_set_visible(joy_popup_p p, bool v)
{
        if (p) p->visible = v;
}

void joy_r2d_popup_set_closeable(joy_popup_p p, bool v)
{
        if (p) p->closeable = v;
}

static void popup_draw_text_lines(joy_r2d_p r2d, joy_font_p font, const char* text, float x, float y, float max_w, joy_color_t color)
{
        if (!text || text[0] == '\0') return;
        /* 按行分割渲染 */
        const char* start = text;
        while (*start) {
                const char* end = strchr(start, '\n');
                if (!end) end = start + strlen(start);
                int line_len = (int)(end - start);
                if (line_len > 0) {
                        joy_r2d_text_printx(r2d, font, start, line_len, color, x, y);
                        y += (float)joy_r2d_font_get_size(font) + 4;
                }
                if (*end == '\n') end++;
                start = end;
        }
}

void joy_r2d_popup_draw(joy_popup_p p, joy_font_p title_font, joy_font_p content_font)
{
        if (!p || !p->visible) return;
        joy_r2d_p r = p->r2d;
        float x = p->x, y = p->y, w = p->w, h = p->h;

        /* 半透明背景遮罩（使用 blend 模式） */
        joy_r2d_set_blend_mode(r, JR2D_BLEND_BLEND);
        joy_r2d_set_color(r, 0, 0, 0, 120);
        joy_rect_t full = { 0, 0, 1920, 1080 };
        joy_r2d_shape_draw_rectangle(r, "fill", full);
        joy_r2d_set_blend_mode(r, JR2D_BLEND_NONE);

        /* 边框 */
        joy_r2d_set_color(r, p->border_color.r, p->border_color.g, p->border_color.b, p->border_color.a);
        joy_r2d_shape_draw_rectangle(r, "line", (joy_rect_t) { x - 1, y - 1, w + 2, h + 2 });

        /* 背景 */
        joy_r2d_set_blend_mode(r, JR2D_BLEND_BLEND);
        joy_r2d_set_color(r, p->bg_color.r, p->bg_color.g, p->bg_color.b, p->bg_color.a);
        joy_r2d_shape_draw_rectangle(r, "fill", (joy_rect_t) { x, y, w, h });

        /* 标题栏背景 */
        joy_r2d_set_color(r, p->title_bg_color.r, p->title_bg_color.g, p->title_bg_color.b, p->title_bg_color.a);
        joy_r2d_shape_draw_rectangle(r, "fill", (joy_rect_t) { x, y, w, p->title_height });

        /* 标题栏分隔线 */
        joy_r2d_set_color(r, p->border_color.r, p->border_color.g, p->border_color.b, p->border_color.a);
        joy_r2d_shape_draw_line(r, x, y + p->title_height, x + w, y + p->title_height);

        /* 标题文字 */
        if (p->title[0] && title_font) {
                int title_len = (int)strlen(p->title);
                joy_r2d_text_printx(r, title_font, p->title, title_len, p->title_text_color,
                        x + p->padding, y + (p->title_height - (float)joy_r2d_font_get_size(title_font)) / 2);
        }

        /* 关闭按钮 */
        if (p->closeable) {
                float cx = x + w - p->close_size - 4;
                float cy = y + (p->title_height - p->close_size) / 2;
                if (p->close_hovered)
                        joy_r2d_set_color(r, p->close_hover_color.r, p->close_hover_color.g, p->close_hover_color.b, p->close_hover_color.a);
                else
                        joy_r2d_set_color(r, p->close_color.r, p->close_color.g, p->close_color.b, p->close_color.a);
                joy_r2d_shape_draw_rectangle(r, "fill", (joy_rect_t) { cx, cy, p->close_size, p->close_size });
                /* X 标记 */
                joy_r2d_set_color(r, 255, 255, 255, 255);
                joy_r2d_shape_draw_line(r, cx + 6, cy + 6, cx + p->close_size - 6, cy + p->close_size - 6);
                joy_r2d_shape_draw_line(r, cx + p->close_size - 6, cy + 6, cx + 6, cy + p->close_size - 6);
        }

        /* 内容文字 */
        if (p->content[0] && content_font) {
                joy_r2d_set_blend_mode(r, JR2D_BLEND_BLEND);
                popup_draw_text_lines(r, content_font, p->content,
                        x + p->padding, y + p->title_height + p->padding,
                        w - p->padding * 2, p->content_color);
        }
        joy_r2d_set_blend_mode(r, JR2D_BLEND_NONE);
}

bool joy_r2d_popup_is_close_clicked(joy_popup_p p, float mx, float my)
{
        if (!p || !p->visible || !p->closeable) return false;
        float cx = p->x + p->w - p->close_size - 4;
        float cy = p->y + (p->title_height - p->close_size) / 2;
        return mx >= cx && mx <= cx + p->close_size &&
                my >= cy && my <= cy + p->close_size;
}

bool joy_r2d_popup_is_title_clicked(joy_popup_p p, float mx, float my)
{
        if (!p || !p->visible) return false;
        return mx >= p->x && mx <= p->x + p->w &&
                my >= p->y && my <= p->y + p->title_height;
        /* 关闭按钮区域单独处理，但 title 区域也包含它 */
}

void joy_r2d_popup_drag_move(joy_popup_p p, float mx, float my)
{
        if (!p || !p->is_dragging) return;
        p->x = mx - p->drag_off_x;
        p->y = my - p->drag_off_y;
}

bool joy_r2d_popup_is_inside(joy_popup_p p, float mx, float my)
{
        if (!p || !p->visible) return false;
        return mx >= p->x && mx <= p->x + p->w &&
                my >= p->y && my <= p->y + p->h;
}

/* ===== Tooltip ===== */
joy_tooltip_p joy_r2d_tooltip_create(joy_r2d_p r2d, const char* text, int len)
{
        joy_tooltip_p t = (joy_tooltip_p)calloc(1, sizeof(joy_tooltip_t));
        if (!t) return NULL;
        t->r2d = r2d;
        t->x = 0; t->y = 0;
        t->padding = 8;
        t->visible = true;
        t->bg_color = (joy_color_t){ 40, 40, 55, 220 };
        t->border_color = (joy_color_t){ 100, 100, 130, 255 };
        t->text_color = (joy_color_t){ 255, 255, 255, 255 };
        joy_r2d_tooltip_set_text(t, text, len);
        return t;
}

void joy_r2d_tooltip_destroy(joy_tooltip_p t)
{
        free(t);
}

void joy_r2d_tooltip_set_text(joy_tooltip_p t, const char* text, int len)
{
        if (!t || !text) return;
        if (len < 0) len = (int)strlen(text);
        if (len >= JOY_TOOLTIP_MAX_TEXT) len = JOY_TOOLTIP_MAX_TEXT - 1;
        memcpy(t->text, text, len);
        t->text[len] = '\0';
        t->text_len = len;
}

void joy_r2d_tooltip_set_position(joy_tooltip_p t, float x, float y)
{
        if (t) { t->x = x; t->y = y; }
}

void joy_r2d_tooltip_set_visible(joy_tooltip_p t, bool v)
{
        if (t) t->visible = v;
}

void joy_r2d_tooltip_draw(joy_tooltip_p t, joy_font_p font)
{
        if (!t || !t->visible || !t->text_len) return;
        joy_r2d_p r = t->r2d;

        /* 估算宽高：按字符数 * fontsize/2 粗略估算 */
        float fh = (float)joy_r2d_font_get_size(font);
        float tw = t->text_len * fh * 0.55f + t->padding * 2;
        float th = fh + t->padding;

        /* 背景 */
        joy_r2d_set_blend_mode(r, JR2D_BLEND_BLEND);
        joy_r2d_set_color(r, t->bg_color.r, t->bg_color.g, t->bg_color.b, t->bg_color.a);
        joy_r2d_shape_draw_rectangle(r, "fill", (joy_rect_t) { t->x, t->y, tw, th });

        /* 边框 */
        joy_r2d_set_blend_mode(r, JR2D_BLEND_NONE);
        joy_r2d_set_color(r, t->border_color.r, t->border_color.g, t->border_color.b, t->border_color.a);
        joy_r2d_shape_draw_rectangle(r, "line", (joy_rect_t) { t->x, t->y, tw, th });

        /* 文字 */
        joy_r2d_text_printx(r, font, t->text, t->text_len, t->text_color,
                t->x + t->padding, t->y + t->padding / 2);
}

/* =================================================================
 * 通用事件处理函数
 * 将 joy_r2d_label_handle_event 重命名为 joy_r2d_label_process_event，风格统一
 * ================================================================= */

void joy_r2d_label_process_event(joy_label_p label, const void* ev)
{
        (void)label;
        (void)ev;
}

void joy_r2d_button_process_event(joy_button_p btn, const void* ev)
{
        if (!btn || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_MOTION) {
                joy_r2d_button_is_hovered(btn, e->motion.x, e->motion.y);
        }
        else if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                joy_r2d_button_is_pressed(btn, e->button.x, e->button.y);
        }
        else if (e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
                btn->is_pressed = false;
        }
}

/* ===================== 滚动层 scrollview =====================
 * 见 joy.h 中的用法说明。要点：
 *  - 事件坐标是"窗口坐标"，拖拽增量必须经 joy_r2d_coordinates_from_window 转逻辑坐标，
 *    否则窗口与逻辑分辨率不同（letterbox 缩放）时滚动速度和手指不一致；
 *  - 点击/滑动用"本次按下累计位移"区分，位移小=点击、大=滑动；
 *  - 裁剪用 joy_r2d_set_clip_rect（替换语义，非入栈），故 begin 时保存旧裁剪、end 时还原。
 */
struct joy_scrollview {
        joy_r2d_p r2d;
        joy_rect_t view;               // 视口（逻辑像素）
        float content_w, content_h; // 内容尺寸（逻辑像素）
        float off_x, off_y;         // 滚动偏移（≤0）
        bool  dragging;             // 正在拖拽
        float last_x, last_y;       // 上一帧拖拽点（逻辑坐标）
        float moved;                // 本次按下累计位移（点击/滑动判定）
        bool  show_bar;             // 是否绘制滚动条
        joy_color_t bar_bg, bar_fg;
        bool  saved_had_clip;       // begin/end 保存&还原裁剪
        joy_rect_t saved_clip;
};

#define SCROLLVIEW_BAR_T      3.0f    // 滚动条厚度（像素）
#define SCROLLVIEW_BAR_MIN    24.0f   // 滑块最小长度（像素）
#define SCROLLVIEW_DRAG_EPS   6.0f    // 拖拽判定阈值（逻辑像素）
#define SCROLLVIEW_WHEEL_STEP 48.0f   // 滚轮每格滚动距离（像素）

joy_scrollview_p joy_r2d_scrollview_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh)
{
        if (!r2d) return NULL;
        joy_scrollview_p v = (joy_scrollview_p)malloc(sizeof(joy_scrollview_t));
        if (!v) return NULL;
        memset(v, 0, sizeof(joy_scrollview_t));
        v->r2d      = r2d;
        v->view     = jui_frect(r2d, fx, fy, fw, fh);
        v->show_bar = true;
        v->bar_bg   = (joy_color_t){ 205, 205, 212, 255 };
        v->bar_fg   = (joy_color_t){ 90, 90, 98, 255 };
        return v;
}

void joy_r2d_scrollview_destroy(joy_scrollview_p v)
{
        if (v) free(v);
}

void joy_r2d_scrollview_set_view_px(joy_scrollview_p v, float x, float y, float w, float h)
{
        if (!v) return;
        v->view = (joy_rect_t){ x, y, w, h };
        joy_r2d_scrollview_clamp(v);
}

void joy_r2d_scrollview_set_content(joy_scrollview_p v, float content_w, float content_h)
{
        if (!v) return;
        v->content_w = content_w;
        v->content_h = content_h;
        joy_r2d_scrollview_clamp(v);
}

void joy_r2d_scrollview_clamp(joy_scrollview_p v)
{
        if (!v) return;
        const float minx = (v->content_w > v->view.w) ? -(v->content_w - v->view.w) : 0.0f;
        const float miny = (v->content_h > v->view.h) ? -(v->content_h - v->view.h) : 0.0f;
        if (v->off_x > 0.0f) v->off_x = 0.0f;
        if (v->off_x < minx) v->off_x = minx;
        if (v->off_y > 0.0f) v->off_y = 0.0f;
        if (v->off_y < miny) v->off_y = miny;
}

void joy_r2d_scrollview_set_offset(joy_scrollview_p v, float ox, float oy)
{
        if (!v) return;
        v->off_x = ox;
        v->off_y = oy;
        joy_r2d_scrollview_clamp(v);
}

float joy_r2d_scrollview_get_offset_x(joy_scrollview_p v) { return v ? v->off_x : 0.0f; }
float joy_r2d_scrollview_get_offset_y(joy_scrollview_p v) { return v ? v->off_y : 0.0f; }

void joy_r2d_scrollview_set_show_bar(joy_scrollview_p v, bool show) { if (v) v->show_bar = show; }
bool joy_r2d_scrollview_dragging(joy_scrollview_p v) { return v ? v->dragging : false; }

bool joy_r2d_scrollview_hit(joy_scrollview_p v, float win_x, float win_y)
{
        if (!v) return false;
        joy_vec2_t p = joy_r2d_coordinates_from_window(v->r2d, win_x, win_y);
        return p.x >= v->view.x && p.x < v->view.x + v->view.w
            && p.y >= v->view.y && p.y < v->view.y + v->view.h;
}

void joy_r2d_scrollview_begin(joy_scrollview_p v)
{
        if (!v) return;
        v->saved_had_clip = joy_r2d_get_clip_rect(v->r2d, &v->saved_clip);
        joy_r2d_set_clip_rect(v->r2d, v->view);
}

void joy_r2d_scrollview_end(joy_scrollview_p v)
{
        if (!v) return;
        if (v->saved_had_clip) joy_r2d_set_clip_rect(v->r2d, v->saved_clip);
        else                    joy_r2d_reset_clip_stack(v->r2d);
        if (!v->show_bar) return;

        const bool hbar = (v->content_w > v->view.w);
        const bool vbar = (v->content_h > v->view.h);
        // 横条贴视口底边、竖条贴右边；两者同存时各让出对方厚度，避免角上重叠
        if (hbar) {
                const float x0 = v->view.x;
                const float w0 = v->view.w - (vbar ? (SCROLLVIEW_BAR_T + 1.0f) : 0.0f);
                const float y0 = v->view.y + v->view.h - SCROLLVIEW_BAR_T - 1.0f;
                joy_r2d_set_color(v->r2d, v->bar_bg.r, v->bar_bg.g, v->bar_bg.b, v->bar_bg.a);
                joy_r2d_shape_draw_rectangle(v->r2d, "fill", (joy_rect_t){ x0, y0, w0, SCROLLVIEW_BAR_T });
                float bw = w0 * (v->view.w / v->content_w);
                if (bw < SCROLLVIEW_BAR_MIN) bw = SCROLLVIEW_BAR_MIN;
                if (bw > w0) bw = w0;
                const float k = -v->off_x / (v->content_w - v->view.w);   // 0..1
                joy_r2d_set_color(v->r2d, v->bar_fg.r, v->bar_fg.g, v->bar_fg.b, v->bar_fg.a);
                joy_r2d_shape_draw_rectangle(v->r2d, "fill",
                        (joy_rect_t){ x0 + k * (w0 - bw), y0, bw, SCROLLVIEW_BAR_T });
        }
        if (vbar) {
                const float y0 = v->view.y;
                const float h0 = v->view.h - (hbar ? (SCROLLVIEW_BAR_T + 1.0f) : 0.0f);
                const float x0 = v->view.x + v->view.w - SCROLLVIEW_BAR_T - 1.0f;
                joy_r2d_set_color(v->r2d, v->bar_bg.r, v->bar_bg.g, v->bar_bg.b, v->bar_bg.a);
                joy_r2d_shape_draw_rectangle(v->r2d, "fill", (joy_rect_t){ x0, y0, SCROLLVIEW_BAR_T, h0 });
                float bh = h0 * (v->view.h / v->content_h);
                if (bh < SCROLLVIEW_BAR_MIN) bh = SCROLLVIEW_BAR_MIN;
                if (bh > h0) bh = h0;
                const float k = -v->off_y / (v->content_h - v->view.h);   // 0..1
                joy_r2d_set_color(v->r2d, v->bar_fg.r, v->bar_fg.g, v->bar_fg.b, v->bar_fg.a);
                joy_r2d_shape_draw_rectangle(v->r2d, "fill",
                        (joy_rect_t){ x0, y0 + k * (h0 - bh), SCROLLVIEW_BAR_T, bh });
        }
}

bool joy_r2d_scrollview_process_event(joy_scrollview_p v, const void* ev)
{
        if (!v || !ev) return false;
        const SDL_Event* e = (const SDL_Event*)ev;

        if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (!joy_r2d_scrollview_hit(v, e->button.x, e->button.y)) {
                        v->dragging = false;              // 视口外按下：不接管
                        return false;
                }
                joy_vec2_t p = joy_r2d_coordinates_from_window(v->r2d, e->button.x, e->button.y);
                v->dragging = true;
                v->last_x  = p.x;
                v->last_y  = p.y;
                v->moved   = 0.0f;
                return false;                             // 不消费：子控件仍可拿到按下反馈
        }
        if (e->type == SDL_EVENT_MOUSE_MOTION) {
                if (!v->dragging) return false;
                joy_vec2_t p = joy_r2d_coordinates_from_window(v->r2d, e->motion.x, e->motion.y);
                const float dx = p.x - v->last_x;
                const float dy = p.y - v->last_y;
                v->last_x = p.x;
                v->last_y = p.y;
                v->off_x += dx;
                v->off_y += dy;
                joy_r2d_scrollview_clamp(v);
                v->moved += (dx < 0.0f ? -dx : dx) + (dy < 0.0f ? -dy : dy);
                return v->moved > SCROLLVIEW_DRAG_EPS;     // 已进入滚动 → 调用方忽略点击
        }
        if (e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
                const bool was_drag = v->dragging && (v->moved > SCROLLVIEW_DRAG_EPS);
                v->dragging = false;
                return was_drag;                          // 本次是滑动 → 调用方不要当点击
        }
        if (e->type == SDL_EVENT_MOUSE_WHEEL) {
                if (!joy_r2d_scrollview_hit(v, e->wheel.mouse_x, e->wheel.mouse_y)) return false;
                if (v->content_h > v->view.h)
                        v->off_y -= e->wheel.y * SCROLLVIEW_WHEEL_STEP;   // 纵向优先
                else if (v->content_w > v->view.w)
                        v->off_x -= e->wheel.x * SCROLLVIEW_WHEEL_STEP;
                else
                        return false;                     // 没有可滚方向：不消费
                joy_r2d_scrollview_clamp(v);
                return true;
        }
        return false;
}

void joy_r2d_inputbox_process_event(joy_inputbox_p ti, const void* ev)
{
        if (!ti || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_MOTION) {
                joy_r2d_inputbox_is_hovered(ti, e->motion.x, e->motion.y);
        }
        else if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                joy_r2d_inputbox_is_pressed(ti, e->button.x, e->button.y);
        }
        else if (e->type == SDL_EVENT_TEXT_INPUT) {
                if (ti->focused) {
                        joy_r2d_inputbox_insert(ti, e->text.text, (int)strlen(e->text.text));
                }
        }
        else if (e->type == SDL_EVENT_KEY_DOWN) {
                if (!ti->focused) return;
                switch (e->key.scancode) {
                case SDL_SCANCODE_BACKSPACE: joy_r2d_inputbox_backspace(ti); break;
                case SDL_SCANCODE_DELETE:    joy_r2d_inputbox_delete(ti); break;
                case SDL_SCANCODE_LEFT:
                        if (e->key.mod & SDL_KMOD_SHIFT) joy_r2d_inputbox_select_left(ti);
                        else joy_r2d_inputbox_cursor_left(ti);
                        break;
                case SDL_SCANCODE_RIGHT:
                        if (e->key.mod & SDL_KMOD_SHIFT) joy_r2d_inputbox_select_right(ti);
                        else joy_r2d_inputbox_cursor_right(ti);
                        break;
                case SDL_SCANCODE_HOME:
                        if (e->key.mod & SDL_KMOD_SHIFT) joy_r2d_inputbox_select_home(ti);
                        else joy_r2d_inputbox_cursor_home(ti);
                        break;
                case SDL_SCANCODE_END:
                        if (e->key.mod & SDL_KMOD_SHIFT) joy_r2d_inputbox_select_end(ti);
                        else joy_r2d_inputbox_cursor_end(ti);
                        break;
                case SDL_SCANCODE_A:
                        if (e->key.mod & SDL_KMOD_CTRL) joy_r2d_inputbox_select_all(ti);
                        break;
                case SDL_SCANCODE_V:
                        if (e->key.mod & SDL_KMOD_CTRL) joy_r2d_inputbox_paste(ti);
                        break;
                case SDL_SCANCODE_C:
                        if (e->key.mod & SDL_KMOD_CTRL) joy_r2d_inputbox_copy(ti);
                        break;
                case SDL_SCANCODE_X:
                        if (e->key.mod & SDL_KMOD_CTRL) joy_r2d_inputbox_cut(ti);
                        break;
                default: break;
                }
        }
}

void joy_r2d_slider_process_event(joy_slider_p s, const void* ev)
{
        if (!s || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_MOTION) {
                joy_r2d_slider_is_hovered(s, e->motion.x, e->motion.y);
                if (s->is_dragging) joy_r2d_slider_drag_move(s, e->motion.x, e->motion.y);
        }
        else if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                joy_r2d_slider_is_pressed(s, e->button.x, e->button.y);
        }
        else if (e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
                s->is_dragging = false;
        }
}

void joy_r2d_tooltip_process_event(joy_tooltip_p t, const void* ev)
{
        if (!t || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_MOTION) {
                joy_r2d_tooltip_set_position(t, e->motion.x + 12, e->motion.y - 10);
        }
}

void joy_r2d_popup_process_event(joy_popup_p p, const void* ev)
{
        if (!p || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mx = e->button.x, my = e->button.y;
                if (joy_r2d_popup_is_close_clicked(p, mx, my)) {
                        joy_r2d_popup_set_visible(p, false);
                } else if (joy_r2d_popup_is_title_clicked(p, mx, my)) {
                        p->is_dragging = true;
                }
        }
        else if (e->type == SDL_EVENT_MOUSE_MOTION) {
                if (p->is_dragging) {
                        joy_r2d_popup_drag_move(p, e->motion.x, e->motion.y);
                }
        }
        else if (e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
                p->is_dragging = false;
        }
}

void joy_r2d_combobox_process_event(joy_combobox_p cb, const void* ev)
{
        if (!cb || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mx = e->button.x, my = e->button.y;
                if (joy_r2d_combobox_is_pressed(cb, mx, my)) {
                        // 点击主框 → 切换展开
                        joy_r2d_combobox_set_open(cb, !joy_r2d_combobox_is_open(cb));
                } else if (joy_r2d_combobox_is_open(cb)) {
                        // 下拉已展开：检查是否点击了某个 item
                        int hit = joy_r2d_combobox_hit_item(cb, mx, my);
                        if (hit >= 0) {
                                joy_r2d_combobox_set_selected(cb, hit);
                                joy_r2d_combobox_set_open(cb, false);
                        } else {
                                // 点击下拉外部 → 收起
                                joy_r2d_combobox_set_open(cb, false);
                        }
                }
        }
}

void joy_r2d_checkbox_process_event(joy_checkbox_p cb, const void* ev)
{
        if (!cb || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                joy_r2d_checkbox_is_pressed(cb, e->button.x, e->button.y);
        }
}

void joy_r2d_radio_process_event(joy_radio_p rb, const void* ev)
{
        if (!rb || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                joy_r2d_radio_is_pressed(rb, e->button.x, e->button.y);
        }
}

void joy_r2d_datagrid_process_event(joy_datagrid_p dg, const void* ev)
{
        (void)dg;
        (void)ev;
        /* datagrid 需要配合外部状态管理 */
}

void joy_r2d_joystick_process_event(joy_joystick_p js, const void* ev)
{
        if (!js || !ev) return;
        const SDL_Event* e = (const SDL_Event*)ev;
        if (e->type == SDL_EVENT_FINGER_DOWN || e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float x = (e->type == SDL_EVENT_FINGER_DOWN) ? e->tfinger.x * 800 : e->button.x;
                float y = (e->type == SDL_EVENT_FINGER_DOWN) ? e->tfinger.y * 600 : e->button.y;
                joy_r2d_joystick_touch_down(js, x, y);
        }
        else if (e->type == SDL_EVENT_FINGER_MOTION || e->type == SDL_EVENT_MOUSE_MOTION) {
                if (joy_r2d_joystick_is_dragging(js)) {
                        float x = (e->type == SDL_EVENT_FINGER_MOTION) ? e->tfinger.x * 800 : e->motion.x;
                        float y = (e->type == SDL_EVENT_FINGER_MOTION) ? e->tfinger.y * 600 : e->motion.y;
                        joy_r2d_joystick_touch_move(js, x, y);
                }
        }
        else if (e->type == SDL_EVENT_FINGER_UP || e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
                joy_r2d_joystick_touch_up(js);
        }
}

/* ===== 通用函数：将 SDL 事件路由到控件数组 ===== */
void joy_r2d_process_widget_event(void** widgets, int count, const void* event)
{
        if (!widgets || !event) return;
        const SDL_Event* e = (const SDL_Event*)event;
        for (int i = 0; i < count; i++) {
                if (!widgets[i]) continue;
                /* 通过指针的指针判断控件类型（取前 4 字节作为类型标记） */
                /* 或者由调用方传入类型数组，更安全的方式是由脚本层直接调用各自 process_event */
                /* 此函数保持通用，实际分发给脚本层 */
                (void)e;
        }
}

/* =================================================================
 * 栅格布局 jlayout
 * 纯几何计算 + 可选标签绘制，不持有任何控件；矩形以“逻辑像素”表示
 * （场景用 s = lh/320 把设计像素换算成逻辑像素后传入）。
 * ================================================================= */
struct joy_layout_grid {
        float ox, oy, width, row_h, pad;
        int   ncols;
        float* col_fracs;
        int   row;
};

joy_layout_grid_t* joy_r2d_layout_grid_create(float ox, float oy, float width, float row_h,
                                    int ncols, const float* col_fracs, float pad)
{
        if (ncols <= 0 || !col_fracs) return NULL;
        joy_layout_grid_t* g = (joy_layout_grid_t*)malloc(sizeof(joy_layout_grid_t));
        if (!g) return NULL;
        g->col_fracs = (float*)malloc(sizeof(float) * (size_t)ncols);
        if (!g->col_fracs) { free(g); return NULL; }
        for (int i = 0; i < ncols; i++) g->col_fracs[i] = col_fracs[i];
        g->ox = ox; g->oy = oy; g->width = width; g->row_h = row_h; g->pad = pad;
        g->ncols = ncols; g->row = 0;
        return g;
}

void joy_r2d_layout_grid_destroy(joy_layout_grid_t* g)
{
        if (!g) return;
        if (g->col_fracs) free(g->col_fracs);
        free(g);
}

void joy_r2d_layout_grid_next_row(joy_layout_grid_t* g)
{
        if (g) g->row++;
}

void joy_r2d_layout_grid_set_row(joy_layout_grid_t* g, int row)
{
        if (g) g->row = row;
}

void joy_r2d_layout_grid_cell(joy_layout_grid_t* g, int col, float* x, float* y, float* w, float* h)
{
        if (!g || col < 0 || col >= g->ncols) {
                if (x) *x = 0.0f; if (y) *y = 0.0f; if (w) *w = 0.0f; if (h) *h = 0.0f;
                return;
        }
        float colx = g->ox;
        for (int i = 0; i < col; i++) colx += g->col_fracs[i] * g->width;
        float colw = g->col_fracs[col] * g->width;
        if (x) *x = colx + g->pad;
        if (y) *y = g->oy + (float)g->row * g->row_h + g->pad;
        if (w) *w = colw - 2.0f * g->pad;
        if (h) *h = g->row_h - 2.0f * g->pad;
        if (w && *w < 0.0f) *w = 0.0f;
        if (h && *h < 0.0f) *h = 0.0f;
}

void joy_r2d_layout_grid_label(joy_layout_grid_t* g, joy_r2d_p r2d, joy_font_p font,
                        const char* text, int n, joy_color_t c, int col,
                        float* cx, float* cy, float* cw, float* ch)
{
        float lx, ly, lw, lh;
        joy_r2d_layout_grid_cell(g, col, &lx, &ly, &lw, &lh);
        int tw = 0, th = 0;
        joy_r2d_text_measurex(r2d, font, text, n, c, &tw, &th);
        int wl = 0, hl = 0;
        joy_r2d_get_logical_presentation(r2d, &wl, &hl, NULL);
        float text_x = lx;                                   // 左对齐
        float text_y = ly + ((lh - (float)th) * 0.5f);       // 垂直居中
        joy_r2d_text_printf(r2d, font, text, n, c,
                     (wl > 0 ? text_x / (float)wl : 0.0f),
                     (hl > 0 ? text_y / (float)hl : 0.0f));
        if (cx) *cx = lx; if (cy) *cy = ly; if (cw) *cw = lw; if (ch) *ch = lh;
}

/* 在当前行创建 n 个等宽按钮（横向按钮条/菜单），各占一列。
 * 按钮尺寸取单元格的 0.86×0.7 居中，避免贴边；指针写入 out[]（调用方分配 n 个）。
 * 列数受栅格 ncols 限制（多出的 label 被忽略）。创建后自动换到下一行。 */
void joy_r2d_layout_grid_button_row(joy_layout_grid_t* g, joy_r2d_p r2d, joy_font_p font,
                             const char** labels, int n, joy_button_p* out)
{
        if (!g || !r2d || n <= 0 || !out) return;
        int lw = 0, lh = 0;
        joy_r2d_get_logical_presentation(r2d, &lw, &lh, NULL);
        const float LW = (lw > 0) ? (float)lw : 960.0f;
        const float LH = (lh > 0) ? (float)lh : 640.0f;
        const int cnt = (n < g->ncols) ? n : g->ncols;
        for (int i = 0; i < cnt; i++) {
                float cx, cy, cw, ch;
                joy_r2d_layout_grid_cell(g, i, &cx, &cy, &cw, &ch);
                float bw = cw * 0.86f, bh = ch * 0.7f;
                float bx = cx + (cw - bw) * 0.5f, by = cy + (ch - bh) * 0.5f;
                joy_button_p b = joy_r2d_button_create(r2d, bx / LW, by / LH, bw / LW, bh / LH);
                if (labels && labels[i])
                        joy_r2d_button_set_textx(b, font, labels[i], (int)strlen(labels[i]), (joy_color_t){ 0, 0, 0, 255 });
                out[i] = b;
        }
        joy_r2d_layout_grid_next_row(g);
}

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef JOYSTICK_BG_COLOR
#undef JOYSTICK_HANDLE_COLOR
#undef JOYSTICK_HANDLE_ACTIVE_COLOR
#undef LERP_CHANNEL
#undef INPUTBOX_BUF_INIT
#undef POPUP_CLOSE_SIZE
#undef POPUP_PADDING
#undef POPUP_TITLE_HEIGHT
#undef SCROLLVIEW_BAR_T
#undef SCROLLVIEW_BAR_MIN
#undef SCROLLVIEW_DRAG_EPS
#undef SCROLLVIEW_WHEEL_STEP

/* ==================== 结束 src/jui.c ==================== */

/* ==================== 合并自 src/jui_im.c ==================== */
/* ============================================================================
 *  joy 即时模式 UI 层（jui_im.c）
 *
 *  jui 原有控件是 retained 模式，适合固定布局的游戏 HUD；但层级树、控制台、
 *  资源列表这类行数动态变化的界面，retained 需要手工管理每一行的生命周期。
 *
 *  本层提供即时模式写法：调用方每帧只描述“这里有个按钮”，控件实例由 ctx
 *  按 id 池化复用——既保留了 button 的颜色过渡动画、slider 的拖拽状态，
 *  又不必由调用方管理 create/destroy。
 *
 *  关键设计：
 *   - 复用 jui 的 button / checkbox / slider（retained 实例），按 id 池化。
 *   - 裁剪基于 joy_r2d_push_clip_rect / joy_r2d_pop_clip_rect，滚动面板的基础。
 *   - 文字统一走 joy_r2d_text_printx 缓存，避免每帧建销纹理。
 * ============================================================================ */



/* ------------------------------ 结构定义 ------------------------------ */

typedef struct joy_ui_slot {
        int       id;         /* -1 = 空闲 */
        joy_button_p  btn;
        joy_checkbox_p cb;
        joy_slider_p  sl;
} joy_ui_slot_t;

struct joy_ui_ctx {
        joy_r2d_p r2d;
        joy_font_p font;
        joy_ui_theme_t theme;

        /* 输入（窗口坐标，内部转逻辑坐标） */
        float  mx, my;             /* 逻辑坐标 */
        float  winx, winy;
        bool   ldown, lpressed, lreleased;
        float  wheel;
        bool   wheel_used;
        bool   shift, ctrl;

        /* 即时模式状态 */
        int    hot, active;
        int    widget_seq;
        int    id_base;

        /* 池化控件：复用 retained 实例 */
        joy_ui_slot_t* slots;
        int         slot_count;
        int         slot_cap;

        /* 裁剪栈镜像（用于命中测试） */
        int     clip_count;
        joy_rect_t clip_stack[32];
        bool    has_clip;

        /* 滚动区 */
        struct { int id; float off; } scrolls[32];
        int scroll_count;

        /* 每帧一次性输入 */
        bool  consumed;            /* begin 已消费 */
        float logx, logy;
};

/* ------------------------------ 主题 ------------------------------ */

static joy_ui_theme_t jui_default_theme(void)
{
        joy_ui_theme_t t;
        t.text         = (joy_color_t){ 212, 212, 212, 255 };
        t.btn_normal   = (joy_color_t){ 51,  51,  55,  255 };
        t.btn_hover    = (joy_color_t){ 70,  70,  75,  255 };
        t.btn_pressed  = (joy_color_t){ 23,  124, 189, 255 };
        t.btn_text     = (joy_color_t){ 212, 212, 212, 255 };
        t.cb_box       = (joy_color_t){ 51,  51,  55,  255 };
        t.cb_check     = (joy_color_t){ 23,  124, 189, 255 };
        t.cb_hover     = (joy_color_t){ 70,  70,  75,  255 };
        t.cb_text      = (joy_color_t){ 212, 212, 212, 255 };
        t.sl_track     = (joy_color_t){ 51,  51,  55,  255 };
        t.sl_fill      = (joy_color_t){ 23,  124, 189, 255 };
        t.sl_thumb     = (joy_color_t){ 150, 150, 156, 255 };
        t.sl_thumb_hover = (joy_color_t){ 200, 200, 210, 255 };
        t.panel        = (joy_color_t){ 37,  37,  38,  255 };
        t.panel_title  = (joy_color_t){ 45,  45,  48,  255 };
        t.border       = (joy_color_t){ 62,  62,  66,  255 };
        return t;
}

/* ------------------------------ 工具 ------------------------------ */

static joy_rect_t JUI_RECT(float x, float y, float w, float h)
{
        joy_rect_t r; r.x = x; r.y = y; r.w = w; r.h = h; return r;
}

static bool jui_pt_in(float x, float y, joy_rect_t r)
{
        return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

/* ------------------------------ 生命周期 ------------------------------ */

joy_ui_ctx_p joy_r2d_ctx_create(joy_r2d_p r2d, joy_font_p font)
{
        joy_ui_ctx_p ctx = (joy_ui_ctx_p)calloc(1, sizeof(joy_ui_ctx_t));
        if (!ctx) return NULL;
        ctx->r2d  = r2d;
        ctx->font = font;
        ctx->theme = jui_default_theme();
        return ctx;
}

void joy_r2d_ctx_destroy(joy_ui_ctx_p ctx)
{
        if (!ctx) return;
        for (int i = 0; i < ctx->slot_count; i++) {
                joy_ui_slot_t* s = &ctx->slots[i];
                if (s->btn) joy_r2d_button_destroy(s->btn);
                if (s->cb)  joy_r2d_checkbox_destroy(s->cb);
                if (s->sl)  joy_r2d_slider_destroy(s->sl);
        }
        free(ctx->slots);
        free(ctx);
}

void joy_r2d_set_theme(joy_ui_ctx_p ctx, const joy_ui_theme_t* theme)
{
        if (ctx && theme) ctx->theme = *theme;
}

/* ------------------------------ 槽位池 ------------------------------ */

static joy_ui_slot_t* jui_get_slot(joy_ui_ctx_p ctx, int id)
{
        if (ctx->slot_count < 32) {
                if (ctx->slot_count >= ctx->slot_cap) {
                        int nc = ctx->slot_cap ? ctx->slot_cap * 2 : 64;
                        joy_ui_slot_t* ns = (joy_ui_slot_t*)realloc(ctx->slots, (size_t)nc * sizeof(joy_ui_slot_t));
                        if (!ns) return NULL;
                        ctx->slots = ns;
                        ctx->slot_cap = nc;
                }
                joy_ui_slot_t* s = &ctx->slots[ctx->slot_count++];
                memset(s, 0, sizeof(*s));
                s->id = id;
                return s;
        }
        /* 已有足够槽位：查找或回收空闲 */
        joy_ui_slot_t* free_slot = NULL;
        for (int i = 0; i < ctx->slot_count; i++) {
                joy_ui_slot_t* s = &ctx->slots[i];
                if (s->id == id) return s;
                if (s->id < 0 && !free_slot) free_slot = s;
        }
        if (free_slot) { free_slot->id = id; return free_slot; }
        return NULL;
}

/* 每帧结束后清空槽位 id，下帧按需重新分配 */
static void jui_reset_slots(joy_ui_ctx_p ctx)
{
        for (int i = 0; i < ctx->slot_count; i++) ctx->slots[i].id = -1;
}

/* ------------------------------ 输入 ------------------------------ */

void joy_r2d_im_begin(joy_ui_ctx_p ctx)
{
        if (!ctx) return;
        ctx->consumed = true;
        /* 由调用方在每帧开始喂入的窗口坐标已存于 ctx->winx/winy，
           这里转为逻辑坐标，供 joy_r2d_hit / 控件使用 */
        joy_vec2_t p = joy_r2d_coordinates_from_window(ctx->r2d, ctx->winx, ctx->winy);
        ctx->mx = p.x;
        ctx->my = p.y;
        ctx->logx = p.x;
        ctx->logy = p.y;
        ctx->widget_seq = 0;
        ctx->hot = 0;
}

void joy_r2d_im_end(joy_ui_ctx_p ctx)
{
        if (!ctx) return;
        ctx->consumed = false;
        ctx->lpressed = false;
        ctx->lreleased = false;
        ctx->wheel_used = false;
        ctx->wheel = 0.0f;
        jui_reset_slots(ctx);
        ctx->widget_seq = 0;
}

void joy_r2d_input_motion(joy_ui_ctx_p ctx, float win_x, float win_y)
{
        if (!ctx) return;
        ctx->winx = win_x;
        ctx->winy = win_y;
}

void joy_r2d_input_down(joy_ui_ctx_p ctx, int button)
{
        if (!ctx) return;
        if (button == 0) { ctx->ldown = true; ctx->lpressed = true; }
}

void joy_r2d_input_up(joy_ui_ctx_p ctx, int button)
{
        if (!ctx) return;
        if (button == 0) { ctx->ldown = false; ctx->lreleased = true; }
}

void joy_r2d_input_wheel(joy_ui_ctx_p ctx, float dy)
{
        if (!ctx) return;
        ctx->wheel += dy;
}

void joy_r2d_input_key(joy_ui_ctx_p ctx, bool shift, bool ctrl)
{
        if (!ctx) return;
        ctx->shift = shift;
        ctx->ctrl  = ctrl;
}

void joy_r2d_get_mouse(joy_ui_ctx_p ctx, float* out_x, float* out_y)
{
        if (out_x) *out_x = ctx ? ctx->mx : 0.0f;
        if (out_y) *out_y = ctx ? ctx->my : 0.0f;
}

bool joy_r2d_is_down(joy_ui_ctx_p ctx)    { return ctx && ctx->ldown; }
bool joy_r2d_is_pressed(joy_ui_ctx_p ctx) { return ctx && ctx->lpressed; }
bool joy_r2d_is_released(joy_ui_ctx_p ctx){ return ctx && ctx->lreleased; }
float joy_r2d_take_wheel(joy_ui_ctx_p ctx)
{
        if (!ctx || ctx->wheel_used) return 0.0f;
        ctx->wheel_used = true;
        return ctx->wheel;
}
bool joy_r2d_is_shift(joy_ui_ctx_p ctx)   { return ctx && ctx->shift; }
bool joy_r2d_is_ctrl(joy_ui_ctx_p ctx)    { return ctx && ctx->ctrl; }

/* ------------------------------ 裁剪 ------------------------------ */

void joy_r2d_push_clip(joy_ui_ctx_p ctx, joy_rect_t rect)
{
        if (!ctx) return;
        if (ctx->clip_count >= 32) return;
        joy_r2d_push_clip_rect(ctx->r2d, rect);
        ctx->clip_stack[ctx->clip_count++] = rect;
        ctx->has_clip = true;
}

void joy_r2d_pop_clip(joy_ui_ctx_p ctx)
{
        if (!ctx || ctx->clip_count <= 0) return;
        ctx->clip_count--;
        joy_r2d_pop_clip_rect(ctx->r2d);
        ctx->has_clip = ctx->clip_count > 0;
}

void joy_r2d_reset_clip(joy_ui_ctx_p ctx)
{
        if (!ctx) return;
        ctx->clip_count = 0;
        ctx->has_clip = false;
        joy_r2d_reset_clip_stack(ctx->r2d);
}

bool joy_r2d_hit(joy_ui_ctx_p ctx, joy_rect_t rect)
{
        if (!ctx) return false;
        if (!jui_pt_in(ctx->mx, ctx->my, rect)) return false;
        if (ctx->has_clip) {
                /* 只需检查最内层裁剪 */
                joy_rect_t c = ctx->clip_stack[ctx->clip_count - 1];
                if (!jui_pt_in(ctx->mx, ctx->my, c)) return false;
        }
        return true;
}

/* ------------------------------ 绘制辅助 ------------------------------ */

void joy_r2d_fill_rect(joy_ui_ctx_p ctx, joy_rect_t r, joy_color_t c)
{
        if (!ctx) return;
        joy_r2d_set_color(ctx->r2d, c.r, c.g, c.b, c.a);
        joy_r2d_shape_draw_rectangle(ctx->r2d, "fill", r);
}

void joy_r2d_stroke_rect(joy_ui_ctx_p ctx, joy_rect_t r, joy_color_t c)
{
        if (!ctx) return;
        joy_r2d_set_color(ctx->r2d, c.r, c.g, c.b, c.a);
        joy_r2d_shape_draw_rectangle(ctx->r2d, "line", r);
}

void joy_r2d_line(joy_ui_ctx_p ctx, float x1, float y1, float x2, float y2, joy_color_t c)
{
        if (!ctx) return;
        joy_r2d_set_color(ctx->r2d, c.r, c.g, c.b, c.a);
        joy_r2d_shape_draw_line(ctx->r2d, x1, y1, x2, y2);
}

int joy_r2d_text(joy_ui_ctx_p ctx, float x, float y, const char* s, joy_color_t c)
{
        if (!ctx || !ctx->font || !s) return 0;
        int w, h;
        if (!joy_r2d_text_measurex(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c, &w, &h)) return 0;
        joy_r2d_text_printx(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c, x, y);
        return w;
}

bool joy_r2d_text_measure(joy_ui_ctx_p ctx, const char* s, joy_color_t c, int* out_w, int* out_h)
{
        if (!ctx || !ctx->font || !s) return false;
        return joy_r2d_text_measurex(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c, out_w, out_h);
}

int joy_r2d_text_centered(joy_ui_ctx_p ctx, joy_rect_t r, const char* s, joy_color_t c)
{
        if (!ctx || !ctx->font || !s) return 0;
        int w = 0, h = 0;
        if (!joy_r2d_text_measurex(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c, &w, &h)) return 0;
        joy_r2d_text_printx(ctx->r2d, ctx->font, s, (int)SDL_strlen(s), c,
                     r.x + (r.w - w) * 0.5f, r.y + (r.h - h) * 0.5f);
        return w;
}

/* ------------------------------ 面板 ------------------------------ */

void joy_r2d_panel_begin(joy_ui_ctx_p ctx, joy_rect_t r, const char* title)
{
        if (!ctx) return;
        joy_r2d_fill_rect(ctx, r, ctx->theme.panel);
        joy_rect_t th = JUI_RECT(r.x, r.y, r.w, 22.0f);
        joy_r2d_fill_rect(ctx, th, ctx->theme.panel_title);
        if (title) joy_r2d_text(ctx, r.x + 8.0f, r.y + 5.0f, title, (joy_color_t){ 133, 133, 133, 255 });
        joy_r2d_line(ctx, r.x, r.y + 22.0f, r.x + r.w, r.y + 22.0f, ctx->theme.border);
        joy_r2d_push_clip(ctx, JUI_RECT(r.x + 1.0f, r.y + 23.0f, r.w - 2.0f, r.h - 24.0f));
}

void joy_r2d_panel_end(joy_ui_ctx_p ctx)
{
        if (ctx) joy_r2d_pop_clip(ctx);
}

void joy_r2d_panel_frame(joy_ui_ctx_p ctx, joy_rect_t r)
{
        if (ctx) joy_r2d_stroke_rect(ctx, r, ctx->theme.border);
}

/* ------------------------------ 滚动 ------------------------------ */

float joy_r2d_scroll_begin(joy_ui_ctx_p ctx, int id, joy_rect_t view, float content_h)
{
        if (!ctx) return 0.0f;
        float* off = NULL;
        for (int i = 0; i < ctx->scroll_count; i++) {
                if (ctx->scrolls[i].id == id) { off = &ctx->scrolls[i].off; break; }
        }
        if (!off && ctx->scroll_count < 32) {
                ctx->scrolls[ctx->scroll_count].id = id;
                ctx->scrolls[ctx->scroll_count].off = 0.0f;
                off = &ctx->scrolls[ctx->scroll_count].off;
                ctx->scroll_count++;
        }
        if (!off) return 0.0f;

        float max_off = content_h > view.h ? content_h - view.h : 0.0f;
        float dy = joy_r2d_take_wheel(ctx);
        if (dy != 0.0f && joy_r2d_hit(ctx, view)) {
                *off -= dy * 46.0f;
        }
        if (*off < 0) *off = 0;
        if (*off > max_off) *off = max_off;
        return *off;
}

void joy_r2d_scroll_end(joy_ui_ctx_p ctx)
{
        /* 预留：滚动区结束后若需要恢复裁剪，可在这里处理 */
}

void joy_r2d_scroll_set_offset(joy_ui_ctx_p ctx, int id, float off, float content_h, float view_h)
{
        float* target = NULL;
        int i;
        if (!ctx) return;
        for (i = 0; i < ctx->scroll_count; i++) {
                if (ctx->scrolls[i].id == id) { target = &ctx->scrolls[i].off; break; }
        }
        if (!target) {
                if (ctx->scroll_count >= 32) return;
                ctx->scrolls[ctx->scroll_count].id = id;
                ctx->scrolls[ctx->scroll_count].off = 0.0f;
                target = &ctx->scrolls[ctx->scroll_count].off;
                ctx->scroll_count++;
        }
        if (content_h > 0.0f && view_h > 0.0f) {
                float max_off = content_h > view_h ? content_h - view_h : 0.0f;
                if (off < 0) off = 0;
                if (off > max_off) off = max_off;
        }
        *target = off;
}

void joy_r2d_scrollbar(joy_ui_ctx_p ctx, joy_rect_t view, float content_h, float off)
{
        if (!ctx || content_h <= view.h) return;
        float thumb_h = view.h * (view.h / content_h);
        if (thumb_h < 24.0f) thumb_h = 24.0f;
        float t = content_h > view.h ? off / (content_h - view.h) : 0.0f;
        float ty = view.y + (view.h - thumb_h) * t;
        joy_r2d_fill_rect(ctx, JUI_RECT(view.x + view.w - 6.0f, view.y, 5.0f, view.h),
                      ctx->theme.panel_title);
        joy_r2d_fill_rect(ctx, JUI_RECT(view.x + view.w - 6.0f, ty, 5.0f, thumb_h),
                      (joy_color_t){ 110, 110, 116, 255 });
}

/* ------------------------------ 控件 ------------------------------ */

bool joy_r2d_button(joy_ui_ctx_p ctx, int id, joy_rect_t r, const char* text)
{
        if (!ctx || !ctx->font) return false;
        joy_ui_slot_t* s = jui_get_slot(ctx, id);
        if (!s) return false;

        if (!s->btn) {
                s->btn = joy_r2d_button_create(ctx->r2d, 0.0f, 0.0f, r.w / 480.0f, r.h / 320.0f);
                if (!s->btn) return false;
                joy_r2d_button_set_textx(s->btn, ctx->font, text, (int)SDL_strlen(text), ctx->theme.btn_text);
        }
        joy_button_p b = s->btn;
        joy_r2d_button_set_position(b, r.x, r.y);
        joy_r2d_button_set_size(b, r.w, r.h);
        joy_r2d_button_set_normal_color(b, ctx->theme.btn_normal);
        joy_r2d_button_set_hover_color(b, ctx->theme.btn_hover);
        joy_r2d_button_set_pressed_color(b, ctx->theme.btn_pressed);

        bool hovered = joy_r2d_hit(ctx, r);
        joy_r2d_button_set_hovered(b, hovered);
        bool clicked = false;
        if (hovered && ctx->lpressed) { ctx->active = id; ctx->hot = id; }
        if (ctx->active == id && ctx->lreleased) {
                if (hovered) clicked = true;
                ctx->active = 0;
        }
        joy_r2d_button_set_pressed(b, ctx->active == id);

        joy_r2d_button_draw(b);
        return clicked;
}

bool joy_r2d_checkbox(joy_ui_ctx_p ctx, int id, joy_rect_t r, bool* value, const char* label)
{
        if (!ctx || !ctx->font) return false;
        joy_ui_slot_t* s = jui_get_slot(ctx, id);
        if (!s) return false;

        /* 复用 checkbox：label 只在创建时定，之后位置由 set_position 移动 */
        if (!s->cb) {
                s->cb = joy_r2d_checkbox_create(r.x / 480.0f, r.y / 320.0f, 14.0f / 480.0f, label);
                if (!s->cb) return false;
        }
        joy_checkbox_p cb = s->cb;
        joy_r2d_checkbox_set_r2d(cb, ctx->r2d);
        joy_r2d_checkbox_set_position(cb, (int)r.x, (int)r.y);
        joy_r2d_checkbox_set_colors(cb, ctx->theme.cb_box, ctx->theme.cb_check, ctx->theme.cb_hover);
        joy_r2d_checkbox_set_text_color(cb, ctx->theme.cb_text);

        joy_checkbox_state_t cur = *value ? JOY_CHECKBOX_CHECKED : JOY_CHECKBOX_UNCHECKED;
        if (joy_r2d_checkbox_get_state(cb) != cur) joy_r2d_checkbox_set_state(cb, cur);

        bool hovered = joy_r2d_hit(ctx, r);
        joy_r2d_checkbox_set_hovered(cb, hovered);

        bool changed = false;
        if (hovered && ctx->lpressed) ctx->active = id;
        if (ctx->active == id && ctx->lreleased) {
                if (hovered) { *value = !*value; changed = true; }
                ctx->active = 0;
        }

        joy_r2d_checkbox_draw(cb, ctx->r2d, ctx->font);
        return changed;
}

bool joy_r2d_slider(joy_ui_ctx_p ctx, int id, joy_rect_t track, float* value, float minv, float maxv)
{
        if (!ctx) return false;
        joy_ui_slot_t* s = jui_get_slot(ctx, id);
        if (!s) return false;

        if (!s->sl) {
                s->sl = joy_r2d_slider_create(ctx->r2d, track.x / 480.0f, track.y / 320.0f, track.w / 480.0f, track.h / 320.0f,
                                      minv, maxv, *value);
                if (!s->sl) return false;
        }
        joy_slider_p sl = s->sl;
        joy_r2d_slider_set_rect(sl, track.x, track.y, track.w, track.h);
        joy_r2d_slider_set_range(sl, minv, maxv);
        joy_r2d_slider_set_colors(sl, ctx->theme.sl_track, ctx->theme.sl_fill,
                          ctx->theme.sl_thumb, ctx->theme.sl_thumb_hover);

        /* 反向同步：外部可能改了 value */
        if (joy_r2d_slider_get_value(sl) != *value) joy_r2d_slider_set_value(sl, *value);

        bool hovered = joy_r2d_hit(ctx, track);
        joy_r2d_slider_set_hovered(sl, hovered);

        bool changed = false;
        if (hovered && ctx->lpressed) { ctx->active = id; }
        if (ctx->active == id) {
                /* 拖拽：从滑块中心到鼠标横坐标 */
                float ratio = (ctx->mx - track.x) / track.w;
                if (ratio < 0) ratio = 0;
                if (ratio > 1) ratio = 1;
                float nv = minv + ratio * (maxv - minv);
                if (nv != *value) { *value = nv; changed = true; }
                if (ctx->lreleased) ctx->active = 0;
        }

        joy_r2d_slider_draw(sl);
        return changed;
}

/* ==================== 结束 src/jui_im.c ==================== */

/* 分片私有宏在此收口，避免污染后续追加的分片 */
#undef JOYSTICK_BG_COLOR
#undef JOYSTICK_HANDLE_COLOR
#undef JOYSTICK_HANDLE_ACTIVE_COLOR
#undef LERP_CHANNEL
#undef INPUTBOX_BUF_INIT
#undef POPUP_CLOSE_SIZE
#undef POPUP_PADDING
#undef POPUP_TITLE_HEIGHT
#undef SCROLLVIEW_BAR_T
#undef SCROLLVIEW_BAR_MIN
#undef SCROLLVIEW_DRAG_EPS
#undef SCROLLVIEW_WHEEL_STEP
