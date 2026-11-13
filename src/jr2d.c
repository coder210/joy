#include "external/stb_image.h"
#include "external/stb_image_write.h"
#include "external/stb_truetype.h"
#include <SDL3/SDL.h>
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
