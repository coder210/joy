#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image.h"
#include "external/stb_image_write.h"
#include "jmath.h"
#include "jr2d.h"

union jr2d {
        SDL_Renderer* sdl;
};

struct image {
        SDL_Texture* texture;
        char filepath[JOY_MAX_PATH];
};

jr2d_p r2d_create(jwindow_p win)
{
        jr2d_p r2d = (jr2d_p)SDL_malloc(sizeof(jr2d_t));
        r2d->sdl = SDL_CreateRenderer(get_raw_window(win), "r2d");
        return r2d;
}

void r2d_destroy(jr2d_p r2d)
{
        SDL_DestroyRenderer(r2d->sdl);
}



void r2d_set_color(jr2d_p r2d, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        SDL_SetRenderDrawColor(r2d->sdl, r, g, b, a);
}
void r2d_begin(jr2d_p r2d) { SDL_RenderClear(r2d->sdl); }
void r2d_end(jr2d_p r2d) { SDL_RenderPresent(r2d->sdl); }

void r2d_set_logical_presentation(jr2d_p r2d, int w, int h, int mode) {
        SDL_SetRenderLogicalPresentation(r2d->sdl, w, h, (SDL_RendererLogicalPresentation)mode);
}
void r2d_get_logical_presentation(jr2d_p r2d, int* w, int* h, int* mode) {
        SDL_RendererLogicalPresentation m;
        SDL_GetRenderLogicalPresentation(r2d->sdl, w, h, &m);
        if (mode) *mode = (int)m;
}

bool shape_draw_line(jr2d_p r2d, float x1, float y1, float x2, float y2)
{
        bool result;
        result = SDL_RenderLine(r2d->sdl, x1, y1, x2, y2);
        return result;
}

bool shape_draw_rectangle(jr2d_p r2d, const char* mode, jrect_t rect)
{
        bool result;
        if (SDL_strcmp(mode, "line") == 0) {
                result = SDL_RenderRect(r2d->sdl, &rect);
        }
        else if (SDL_strcmp(mode, "fill") == 0) {
                result = SDL_RenderFillRect(r2d->sdl, &rect);
        }
        else {
                result = false;
        }
        return result;
}

static bool
draw_polygon_outline(jr2d_p r2d, vec2_t* points, int point_count)
{
        bool result;
        points[point_count].x = points[0].x;
        points[point_count].y = points[0].y;
        result = SDL_RenderLines(r2d->sdl, points, point_count + 1);
        return result;
}

static bool
draw_polygon_filled(jr2d_p r2d,
        vec2_t* points, int point_count, SDL_FColor color)
{
        bool result;
        SDL_Vertex* vertices;
        int vertex_count;
        int* indices = NULL;
        int indices_count = 0;

        vertex_count = point_count - 1;
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
                vertices[i].color = color;
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
shape_draw_polygon(jr2d_p r2d, const char* mode,
        vec2_t* points, int point_count, SDL_FColor color)
{
        bool result;
        if (SDL_strcmp(mode, "line") == 0) {
                result = draw_polygon_outline(r2d->sdl, points, point_count);
        }
        else if (SDL_strcmp(mode, "fill") == 0) {
                result = draw_polygon_filled(r2d->sdl, points, point_count, color);
        }
        else {
                result = false;
        }
        return result;
}

static bool
draw_circle_outline(jr2d_p r2d, vec2_t center, float radius, int segments)
{
        bool result;
        float angle;
        vec2_t* points;
        points = SDL_malloc(sizeof(vec2_t) * (segments + 1));
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
draw_circle_filled(jr2d_p r2d, vec2_t center, float radius, int segments, SDL_FColor color)
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
        vertices[0].color = color;
        vertices[0].tex_coord.x = 0.0f;
        vertices[0].tex_coord.y = 0.0f;

        for (int i = 0; i < segments; i++) {
                float angle = (float)i / (float)segments * 2.0f * 3.1415926535f;
                vertices[i + 1].position.x = center.x + cosf(angle) * radius;
                vertices[i + 1].position.y = center.y + sinf(angle) * radius;
                vertices[i + 1].color = color;
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
shape_draw_circle(jr2d_p r2d, const char* mode, vec2_t center, float radius, int segments)
{
        bool result;
        SDL_FColor color;
        if (segments < 8) segments = 8;
        if (segments > 360) segments = 360;
        if (SDL_strcmp(mode, "line") == 0) {
                result = draw_circle_outline(r2d->sdl, center, radius, segments);
        }
        else if (SDL_strcmp(mode, "fill") == 0) {
                SDL_GetRenderDrawColorFloat(r2d->sdl, &color.r, &color.g, &color.b, &color.a);
                result = draw_circle_filled(r2d->sdl, center, radius, segments, color);
        }
        else {
                result = false;
        }
        return result;
}

bool
shape_draw_grid(jr2d_p r2d,
        vec2_t start, vec2_t end,
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
shape_draw_gridx(jr2d_p r2d,
        vec2_t position,
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



static image_p load_bmp(jr2d_p r2d, const char* filename)
{
	char filepath[JOY_MAX_PATH] = { 0 };
	image_p image;
	SDL_strlcat(filepath, filename, JOY_MAX_PATH);
	SDL_Surface* surface = SDL_LoadBMP(filepath);
	if (!surface) {
		return NULL;
	}
	image = (image_p)SDL_malloc(sizeof(image_t));
	SDL_assert(image);
	SDL_strlcpy(image->filepath, filepath, JOY_MAX_PATH);
	image->texture = SDL_CreateTextureFromSurface(r2d->sdl, surface);
	SDL_DestroySurface(surface);
	return image;
}

static image_p load_png(jr2d_p r2d, const char* filename)
{
	stbi_uc* filedata;
	size_t sz;
	char filepath[JOY_MAX_PATH] = { 0 };
	image_p image;
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

	image = (image_p)SDL_malloc(sizeof(image_t));
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

image_p image_create(jr2d_p r2d, const char* filename)
{
	image_p image = NULL;
	if (SDL_strstr(filename, ".bmp")) {
		image = load_bmp(r2d->sdl, filename);
	}
	else if (SDL_strstr(filename, ".png")) {
		image = load_png(r2d->sdl, filename);
	}
	return image;
}

static image_p image_create_from_surface(jr2d_p r2d, 
        SDL_Surface* surface, 
        const char* label)
{
	image_p image;
	if (!surface) return NULL;
	image = (image_p)SDL_malloc(sizeof(image_t));
	if (!image) return NULL;
	image->texture = SDL_CreateTextureFromSurface(r2d->sdl, surface);
	SDL_strlcpy(image->filepath, label ? label : "(surface)", JOY_MAX_PATH);
	if (!image->texture) {
		SDL_free(image);
		return NULL;
	}
	return image;
}

void image_destroy(image_p image)
{
	SDL_assert(image);
	if (image->texture) {
		SDL_DestroyTexture(image->texture);
	}
	SDL_free(image);
}

void image_draw(image_p image, vec2_t position)
{
	jrect_t srcrect = { 0, 0, image->texture->w, image->texture->h };
	vec2_t scale = { 1.0f, 1.0f };
	vec2_t origin = { 0.0f, 0.0f };
	image_draw_ex(image, &srcrect, position, 0.0f, scale, origin);
}

void image_draw1(image_p image,
	const jrect_t* srcrect, const jrect_t* destrect)
{
	SDL_Renderer* renderer;
	renderer = SDL_GetRendererFromTexture(image->texture);
	SDL_RenderTexture(renderer, image->texture, srcrect, destrect);
}

void image_draw_ex(image_p image, const jrect_t* srcrect, vec2_t position,
	float rotation, vec2_t scale, vec2_t origin)
{
	SDL_Renderer* renderer;
	float w, h;
	jrect_t dest_rect;
	vec2_t center;
	w = srcrect->w;
	h = srcrect->h;
	renderer = SDL_GetRendererFromTexture(image->texture);

	// 负 scale → 转换为正宽度 + 翻转标志
	SDL_FlipMode flip = SDL_FLIP_NONE;
	float sx = scale.x, sy = scale.y;
	if (sx < 0) { flip = (SDL_FlipMode)(flip | SDL_FLIP_HORIZONTAL); sx = -sx; }
	if (sy < 0) { flip = (SDL_FlipMode)(flip | SDL_FLIP_VERTICAL);   sy = -sy; }

	dest_rect.x = (position.x - origin.x * sx);
	dest_rect.y = (position.y - origin.y * sy);
	dest_rect.w = w * sx;
	dest_rect.h = h * sy;
	center.x = origin.x * sx;
	center.y = origin.y * sy;

	SDL_RenderTextureRotated(renderer, image->texture,
		srcrect, &dest_rect,
		rotation * 180.0f / 3.1415926f,
		&center, flip);
	//SDL_SetTextureAlphaMod(image->texture, 255);
}
