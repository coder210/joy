#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image.h"
#include "external/stb_image_write.h"
#include "external/stb_truetype.h"
#include "external/klib/khash.h"
#include "external/klib/kvec.h"
#include "liv-math.h"
#include "liv-hash.h"
#include "liv-utils.h"
#include "liv-graphics.h"

typedef struct image {
	SDL_Texture* texture;
	char filepath[LIVNET_MAX_PATH];
}image_t, * image_p;

typedef struct font_t {
	stbtt_fontinfo fontinfo;
	unsigned char* fontdata;
	SDL_Renderer* renderer;
	int fontsize;
	float scale;
	int ascent;
	int descent;
	int line_gap;
} font_t, * font_p;

typedef struct text_texture {
	SDL_Texture *texture;
	int real_width;
	int real_height;
}text_texture_t, * text_texture_p;

typedef struct spritebatch {
	image_p image;
	SDL_Vertex* vertices;
	int* indices;
	int num_vertices;
	int num_indices;
	int index_vertices;
	int index_indices;
}spritebatch_t, * spritebatch_p;

typedef struct animation_clip {
	image_p image;
	float duration;
	SDL_FRect src_rect;
}animation_clip_t, * animation_clip_p;


typedef struct animation {
	SDL_Renderer* renderer;
	kvec_t(animation_clip_t) clips;
	bool is_loop;
	int idx_frame;
	float total_time;
	float last_time;
	SDL_FPoint position;
	float rotation;
	SDL_FPoint scale;
	SDL_FPoint origin;
}animation_t, * animation_p;

/////////////////////////////////////////////////////////////////////////
//////////////////////////////shape//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

bool shape_draw_line(SDL_Renderer* renderer, float x1, float y1, float x2, float y2)
{
	bool result;
	result = SDL_RenderLine(renderer, x1, y1, x2, y2);
	return result;
}

bool shape_draw_rectangle(SDL_Renderer* renderer, const char* mode, SDL_FRect rect)
{
	bool result;
	if (SDL_strcmp(mode, "line") == 0) {
		result = SDL_RenderRect(renderer, &rect);
	}
	else if (SDL_strcmp(mode, "fill") == 0) {
		result = SDL_RenderFillRect(renderer, &rect);
	}
	else {
		result = false;
	}
	return result;
}

static bool
draw_polygon_outline(SDL_Renderer* renderer, SDL_FPoint* points, int point_count)
{
	bool result;
	int vertex_count = point_count - 1;
	points[vertex_count].x = points[0].x;
	points[vertex_count].y = points[0].y;
	result = SDL_RenderLines(renderer, points, point_count);
	return result;
}

static bool
draw_polygon_filled(SDL_Renderer* renderer,
	SDL_FPoint* points, int point_count, SDL_FColor color)
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
		result = SDL_RenderGeometry(renderer, NULL, vertices, vertex_count, indices, indices_count);
		SDL_free(indices);
	}
	else {
		result = false;
	}

	SDL_free(vertices);

	return result;
}

bool
shape_draw_polygon(SDL_Renderer* renderer, const char* mode,
	SDL_FPoint* points, int point_count, SDL_FColor color)
{
	bool result;
	if (SDL_strcmp(mode, "line") == 0) {
		result = draw_polygon_outline(renderer, points, point_count);
	}
	else if (SDL_strcmp(mode, "fill") == 0) {
		result = draw_polygon_filled(renderer, points, point_count, color);
	}
	else {
		result = false;
	}
	return result;
}

static bool
draw_circle_outline(SDL_Renderer* renderer, SDL_FPoint center, float radius, int segments)
{
	bool result;
	float angle;
	SDL_FPoint* points;
	points = SDL_malloc(sizeof(SDL_FPoint) * (segments + 1));
	for (int i = 0; i < segments; i++) {
		angle = (float)i / (float)segments * 2.0f * 3.1415926535f;
		points[i].x = center.x + cosf(angle) * radius;
		points[i].y = center.y + sinf(angle) * radius;
	}
	points[segments].x = points[0].x;
	points[segments].y = points[0].y;
	result = SDL_RenderLines(renderer, points, segments + 1);
	SDL_free(points);
	return result;
}

static bool
draw_circle_filled(SDL_Renderer* renderer, SDL_FPoint center, float radius, int segments, SDL_FColor color)
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

	result = SDL_RenderGeometry(renderer, NULL, vertices, vertex_count, indices, segments * 3);

	SDL_free(vertices);
	SDL_free(indices);

	return result;
}

bool
shape_draw_circle(SDL_Renderer* renderer, const char* mode, SDL_FPoint center, float radius, int segments)
{
	bool result;
	SDL_FColor color;
	if (segments < 8) segments = 8;
	if (segments > 360) segments = 360;
	if (SDL_strcmp(mode, "line") == 0) {
		result = draw_circle_outline(renderer, center, radius, segments);
	}
	else if (SDL_strcmp(mode, "fill") == 0) {
		SDL_GetRenderDrawColorFloat(renderer, &color.r, &color.g, &color.b, &color.a);
		result = draw_circle_filled(renderer, center, radius, segments, color);
	}
	else {
		result = false;
	}
	return result;
}

bool
shape_draw_grid(SDL_Renderer* renderer,
	SDL_FPoint start, SDL_FPoint end,
	float grid_size)
{
	float x1, x2, y1, y2;
	for (float x = start.x; x <= end.x; x += grid_size) {
		x1 = x;
		x2 = start.y;
		y1 = x;
		y2 = end.y;
		SDL_RenderLine(renderer, x1, x2, y1, y2);
	}
	for (float y = start.y; y <= end.y; y += grid_size) {
		x1 = start.x;
		x2 = y;
		y1 = end.x;
		y2 = y;
		SDL_RenderLine(renderer, x1, x2, y1, y2);
	}
	return true;
}

bool
shape_draw_gridx(SDL_Renderer* renderer,
	SDL_FPoint position,
	int rows, int cols, float grid_size)
{
	float x1, x2, y1, y2;
	for (int i = 0; i <= rows; i++) {
		x1 = position.x;
		y1 = i * grid_size + position.y;
		x2 = cols * grid_size + position.x;
		y2 = y1;
		SDL_RenderLine(renderer, x1, y1, x2, y2);
	}
	for (int i = 0; i <= cols; i++) {
		x1 = i * grid_size + position.x;
		y1 = position.y;
		x2 = x1;
		y2 = rows * grid_size + position.y;
		SDL_RenderLine(renderer, x1, y1, x2, y2);
	}
	return true;
}
////////////////////////////////////////////////////////////////////////
//////////////////////////////image/////////////////////////////////////
////////////////////////////////////////////////////////////////////////
static image_p load_bmp(SDL_Renderer* renderer, const char* filename)
{
	char filepath[LIVNET_MAX_PATH] = { 0 };
	image_p image;
	//SDL_strlcat(filepath, SDL_GetBasePath(), LIVNET_MAX_PATH);
	SDL_strlcat(filepath, filename, LIVNET_MAX_PATH);
	SDL_Surface* surface = SDL_LoadBMP(filepath);
	if (!surface) {
		return NULL;
	}
	image = (image_p)SDL_malloc(sizeof(image_t));
	SDL_assert(image);
	SDL_strlcpy(image->filepath, filepath, LIVNET_MAX_PATH);
	image->texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);
	return image;
}

static image_p load_png(SDL_Renderer* renderer, const char* filename)
{
	stbi_uc* filedata;
	size_t sz;
	char filepath[LIVNET_MAX_PATH] = { 0 };
	image_p image;
	int w, h, channels;
	unsigned char* pixels;
	SDL_Surface* surface;
	filedata = (stbi_uc*)SDL_LoadFile(filename, &sz);
	if (!filedata) {
		return NULL;
	}

	image = (image_p)SDL_malloc(sizeof(image_t));
	SDL_assert(image);
	//SDL_strlcat(filepath, SDL_GetBasePath(), LIVNET_MAX_PATH);
	SDL_strlcat(filepath, filename, LIVNET_MAX_PATH);
	pixels = stbi_load_from_memory(filedata, sz, &w, &h, &channels, STBI_rgb_alpha);
	surface = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, pixels, w * 4);
	image->texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);
	SDL_free(filedata);
	SDL_strlcpy(image->filepath, filepath, LIVNET_MAX_PATH);
	return image;
}

image_p image_create(SDL_Renderer* renderer, const char* filename)
{
	image_p image = NULL;
	if (SDL_strstr(filename, ".bmp")) {
		image = load_bmp(renderer, filename);
	}
	else if (SDL_strstr(filename, ".png")) {
		image = load_png(renderer, filename);
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

void image_draw(image_p image, SDL_FPoint position)
{
	SDL_FRect srcrect = { 0, 0, image->texture->w, image->texture->h };
	SDL_FPoint scale = { 1.0f, 1.0f };
	SDL_FPoint origin = { 0.0f, 0.0f };
	image_draw_ex(image, &srcrect, position, 0.0f, scale, origin);
}

void image_draw_ex(image_p image, const SDL_FRect* srcrect, SDL_FPoint position,
	float rotation, SDL_FPoint scale, SDL_FPoint origin)
{
	SDL_FColor color;
	SDL_Renderer* renderer;
	float w, h;
	SDL_FRect dest_rect;
	SDL_FPoint center;
	w = srcrect->w;
	h = srcrect->h;
	renderer = SDL_GetRendererFromTexture(image->texture);
	SDL_GetRenderDrawColorFloat(renderer, &color.r, &color.g, &color.b, &color.a);
	dest_rect.x = (position.x - origin.x * scale.x);
	dest_rect.y = (position.y - origin.y * scale.y);
	dest_rect.w = w * scale.x;
	dest_rect.h = h * scale.y;
	center.x = origin.x * scale.x;
	center.y = origin.y * scale.y;
	//SDL_SetTextureColorModFloat(image->texture, color.r, color.g, color.b);
	//SDL_SetTextureAlphaModFloat(image->texture, color.a);
	SDL_RenderTextureRotated(renderer, image->texture,
		srcrect, &dest_rect,
		rotation * 180.0f / 3.1415926f,
		&center, SDL_FLIP_NONE);
	//SDL_SetTextureColorMod(image->texture, 255, 255, 255);
	//SDL_SetTextureAlphaMod(image->texture, 255);
}

spritebatch_p
spritebatch_create(SDL_Renderer* renderer, const char* filename)
{
	const int capacity = 256;
	spritebatch_p batch;
	batch = (spritebatch_p)SDL_malloc(sizeof(spritebatch_t));
	batch->image = image_create(renderer, filename);
	batch->num_indices = capacity * 6;
	batch->num_vertices = capacity * 4;
	batch->index_indices = 0;
	batch->index_vertices = 0;
	batch->vertices = (SDL_Vertex*)SDL_malloc(sizeof(SDL_Vertex) * batch->num_vertices);
	batch->indices = (int*)SDL_malloc(sizeof(int) * batch->num_indices);
	return batch;
}

void spritebatch_destroy(spritebatch_p batch)
{
	SDL_free(batch->vertices);
	SDL_free(batch->indices);
	image_destroy(batch->image);
	SDL_free(batch);
}

void spritebatch_add(spritebatch_p batch,
	SDL_FPoint position,
	float rotation,
	float scale_x, float scale_y,
	SDL_FPoint origin,
	SDL_FRect src_rect)
{
	SDL_Renderer* renderer;
	float w, h;
	SDL_FRect dest_rect;
	SDL_FPoint center;

	renderer = SDL_GetRendererFromTexture(batch->image->texture);
	SDL_GetTextureSize(batch->image->texture, &w, &h);

	dest_rect.x = position.x;
	dest_rect.y = position.y;
	dest_rect.w = w * scale_x;
	dest_rect.h = h * scale_y;

	/* 默认图片中心点 */
	center.x = dest_rect.x + dest_rect.w / 2;
	center.y = dest_rect.y + dest_rect.h / 2;

	spritebatch_add_ex(batch, src_rect, dest_rect, rotation, center);
}

float spritebatch_get_width(spritebatch_p batch)
{
	return batch->image->texture->w;
}

float spritebatch_get_height(spritebatch_p batch)
{
	return batch->image->texture->h;
}

void spritebatch_add_ex(spritebatch_p batch,
	SDL_FRect src_rect, SDL_FRect dest_rect,
	float rotation, SDL_FPoint origin)
{
	SDL_Renderer* renderer;
	float w, h;
	SDL_FColor color;
	SDL_Vertex* vertex;
	vec2_t translated;

	if (batch->num_indices <= batch->index_indices) {
		batch->num_indices *= 2;
		batch->indices = (int*)SDL_realloc(batch->indices, sizeof(int) * batch->num_indices);
	}
	if (batch->num_vertices <= batch->index_vertices) {
		batch->num_vertices *= 2;
		batch->vertices = (SDL_Vertex*)SDL_realloc(batch->vertices, sizeof(SDL_Vertex) * batch->num_vertices);
	}

	renderer = SDL_GetRendererFromTexture(batch->image->texture);
	SDL_GetRenderDrawColorFloat(renderer, &color.r, &color.g, &color.b, &color.a);
	SDL_GetTextureSize(batch->image->texture, &w, &h);

	batch->indices[batch->index_indices++] = 0 + batch->index_vertices;
	batch->indices[batch->index_indices++] = 1 + batch->index_vertices;
	batch->indices[batch->index_indices++] = 2 + batch->index_vertices;
	batch->indices[batch->index_indices++] = 0 + batch->index_vertices;
	batch->indices[batch->index_indices++] = 2 + batch->index_vertices;
	batch->indices[batch->index_indices++] = 3 + batch->index_vertices;

	vertex = &batch->vertices[batch->index_vertices];
	vertex->position.x = dest_rect.x;
	vertex->position.y = dest_rect.y;
	vertex->color = color;
	vertex->tex_coord.x = src_rect.x / w;
	vertex->tex_coord.y = src_rect.y / h;
	batch->index_vertices++;

	vertex = &batch->vertices[batch->index_vertices];
	vertex->position.x = dest_rect.x + dest_rect.w;
	vertex->position.y = dest_rect.y;
	vertex->color = color;
	vertex->tex_coord.x = (src_rect.x + src_rect.w) / w;
	vertex->tex_coord.y = src_rect.y / h;
	batch->index_vertices++;

	vertex = &batch->vertices[batch->index_vertices];
	vertex->position.x = dest_rect.x + dest_rect.w;
	vertex->position.y = dest_rect.y + dest_rect.h;
	vertex->color = color;
	vertex->tex_coord.x = (src_rect.x + src_rect.w) / w;
	vertex->tex_coord.y = (src_rect.y + src_rect.h) / h;
	batch->index_vertices++;

	vertex = &batch->vertices[batch->index_vertices];
	vertex->position.x = dest_rect.x;
	vertex->position.y = dest_rect.y + dest_rect.h;
	vertex->color = color;
	vertex->tex_coord.x = src_rect.x / w;
	vertex->tex_coord.y = (src_rect.y + src_rect.h) / h;
	batch->index_vertices++;

	ft_t angle = rotation / 180.0f * 3.1415926353f;

	for (int i = batch->index_vertices - 4; i < batch->index_vertices; i++) {
		vertex = &batch->vertices[i];

		// 旋转
		translated.x = vertex->position.x - origin.x;
		translated.y = vertex->position.y - origin.y;
		translated = vec2_rotate(translated, angle);

		// 平移回原位置
		vertex->position.x = origin.x + translated.x;
		vertex->position.y = origin.y + translated.y;
	}
}

void spritebatch_clear(spritebatch_p batch)
{
	if (batch) {
		batch->index_indices = 0;
		batch->index_vertices = 0;
	}
}

void spritebatch_set_image(spritebatch_p batch, const char* filename)
{
	SDL_Renderer* renderer;
	if (batch) {
		renderer = SDL_GetRendererFromTexture(batch->image->texture);
		image_destroy(batch->image);
		batch->image = image_create(renderer, filename);
	}
}

void spritebatch_draw(spritebatch_p batch)
{
	SDL_Renderer* renderer;
	renderer = SDL_GetRendererFromTexture(batch->image->texture);
	SDL_RenderGeometry(renderer,
		batch->image->texture,
		batch->vertices,
		batch->index_vertices,
		batch->indices,
		batch->index_indices);
}


animation_p animation_create(SDL_Renderer* renderer)
{
	animation_p animation;
	animation = (animation_p)SDL_malloc(sizeof(animation_t));
	animation->renderer = renderer;
	animation->is_loop = true;
	kv_init(animation->clips);
	animation->position = (SDL_FPoint){0, 0};
	animation->rotation = 0;
	animation->scale = (SDL_FPoint){ 1.0f, 1.0f };
	animation->origin = (SDL_FPoint){ 0.0f, 0.0f };
	animation->total_time = animation->last_time = animation->idx_frame = 0;
	return animation;
}

void animation_destroy(animation_p animation)
{
	kv_destroy(animation->clips);
	SDL_free(animation);
}

void animation_reset(animation_p animation)
{
	uint64_t ticks;
	animation->idx_frame = 0;
	animation->total_time = 0;
	if (utils_get_current_time(&ticks)) {
		animation->last_time = ticks;
	}
	else {
		animation->last_time = 0;
	}
}

void animation_add_clip(animation_p animation,
	const char* image_path, float duration,
	SDL_FRect src_rect)
{
	animation_clip_t animation_clip;
	animation_clip.image = image_create(animation->renderer, image_path);
	animation_clip.duration = duration;
	animation_clip.src_rect = src_rect;
	kv_push(animation_clip_t, animation->clips, animation_clip);
}

bool animation_is_finished(animation_p animation)
{
	if (animation->is_loop) {
		return false;
	}
	else {
		return (animation->idx_frame == kv_size(animation->clips));
	}
}

void animation_set_position(animation_p animation, float x, float y)
{
	animation->position.x = x;
	animation->position.y = y;
}

void animation_set_scale(animation_p animation, float x, float y)
{
	animation->scale.x = x;
	animation->scale.y = y;
}

void animation_set_rotation(animation_p animation, float rotation)
{
	animation->rotation = rotation;
}

void animation_update(animation_p animation, float dt)
{
	animation_clip_p animation_clip;
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

void animation_draw(animation_p animation, SDL_FRect* camera)
{
	animation_clip_p animation_clip;
	float r_x, r_y;
	SDL_FRect dest_rect;
	SDL_FPoint center = { 0 };

	if (kv_size(animation->clips) <= 0) {
		return;
	}

	animation_clip = &kv_A(animation->clips, animation->idx_frame);
	image_draw_ex(animation_clip->image, &animation_clip->src_rect, animation->position, animation->rotation, animation->scale, animation->origin);
}

////////////////////////////////////////////////////////////////////////
//////////////////////////////font//////////////////////////////////////
////////////////////////////////////////////////////////////////////////


/**
 * 创建字体对象
 */
font_p font_create(SDL_Renderer* renderer, const char* filename, int fontsize)
{
	size_t data_size;
	unsigned char* fontdata;
	font_p font;

	// 加载字体文件
	fontdata = SDL_LoadFile(filename, &data_size);
	if (!fontdata) {
		SDL_Log("Failed to load font file: %s", filename);
		return NULL;
	}

	// 分配字体结构体
	font = (font_p)SDL_calloc(1, sizeof(font_t));
	if (!font) {
		SDL_free(fontdata);
		return NULL;
	}

	// 初始化stb_truetype字体信息
	if (!stbtt_InitFont(&font->fontinfo, fontdata, 0)) {
		SDL_Log("Failed to initialize font");
		SDL_free(fontdata);
		SDL_free(font);
		return NULL;
	}

	// 保存数据
	font->fontdata = fontdata;
	font->renderer = renderer;
	font->fontsize = fontsize;

	/**
	 * 获取垂直方向上的度量
	 * ascent：字体从基线到顶部的高度；
	 * descent：基线到底部的高度，通常为负值；
	 * line_gap：两个字体之间的间距；
	 * 行间距为：ascent - descent + line_gap。
	*/
	stbtt_GetFontVMetrics(&font->fontinfo,
		&font->ascent,
		&font->descent,
		&font->line_gap);

	/* 根据缩放调整字高 */
	font->scale = stbtt_ScaleForPixelHeight(&font->fontinfo, fontsize);
	font->ascent = roundf(font->ascent * font->scale);
	font->descent = roundf(font->descent * font->scale);

	return font;
}

/**
 * 释放字体对象
 */
void font_destroy(font_p font)
{
	if (!font) return;

	if (font->fontdata) {
		SDL_free(font->fontdata);
	}
	SDL_free(font);
}


/**
 * 创建文本纹理
 */
text_texture_p text_create(font_p font, 
	const int* codepoints, int num_codepoints, 
	SDL_Color color)
{
	size_t sizedata;
	int pitch;
	int x, y;/* 位图的x,计算位图的y (不同字符的高度不同） */
	float scale;
	int bitmap_index, pixel_index;
	unsigned char* bitmap;
	int advance_width, left_side_bearing;
	int c_x1, c_y1, c_x2, c_y2;
	SDL_Surface* surface;
	unsigned char* surface_pixels;
	Uint8 alpha;
	text_texture_p text;
	char pixel;
	int kern;

	x = 0;
	advance_width = left_side_bearing = 0;
	scale = font->scale;

	/* 创建位图 */
	text = (text_texture_p)SDL_malloc(sizeof(text_texture_t));
	text->real_width = font->fontsize * num_codepoints; /* 位图的宽 */
	text->real_height = font->fontsize; /* 位图的高 */
	bitmap = SDL_calloc(text->real_width * text->real_height + 1, sizeof(unsigned char));

	/* 循环加载codepoints中每个字符 */
	for (int i = 0; i < num_codepoints; ++i) {
		/**
		  * 获取水平方向上的度量
		  * advanceWidth：字宽；
		  * leftSideBearing：左侧位置；
		*/
		stbtt_GetCodepointHMetrics(&font->fontinfo, codepoints[i], &advance_width, &left_side_bearing);

		/* 获取字符的边框（边界） */
		stbtt_GetCodepointBitmapBox(&font->fontinfo, codepoints[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

		/* 计算位图的y (不同字符的高度不同） */
		y = font->ascent + c_y1;

		/* 渲染字符 */
		int byte_offset = x + roundf(left_side_bearing * scale) + (y * text->real_width);
		stbtt_MakeCodepointBitmap(&font->fontinfo, bitmap + byte_offset, c_x2 - c_x1, c_y2 - c_y1, text->real_width, scale, scale, codepoints[i]);
		/* 调整x */
		x += roundf(advance_width * scale);

		/* 调整字距 */
		kern = stbtt_GetCodepointKernAdvance(&font->fontinfo, codepoints[i], codepoints[i + 1]);
		x += roundf(kern * scale);
	}
	surface = SDL_CreateSurface(1024, text->real_height, SDL_PIXELFORMAT_RGBA32);
	if (!surface) {
		SDL_free(bitmap);
		return NULL;
	}

	surface_pixels = (Uint8*)surface->pixels;
	pitch = surface->pitch / sizeof(Uint8);
	/* 灰度转RGBA（带透明度） */
	for (y = 0; y < text->real_height; ++y) {
		for (x = 0; x < text->real_width; ++x) {
			bitmap_index = y * text->real_width + x;
			pixel_index = y * (pitch / 4) + x;
			pixel = bitmap[bitmap_index];
			surface_pixels[pixel_index * 4 + 0] = color.r;
			surface_pixels[pixel_index * 4 + 1] = color.g;
			surface_pixels[pixel_index * 4 + 2] = color.b;
			surface_pixels[pixel_index * 4 + 3] = pixel & color.a;
		}
	}
	text->texture = SDL_CreateTextureFromSurface(font->renderer, surface);
	SDL_free(bitmap);
	SDL_DestroySurface(surface);
	return text;
}

void text_update(text_texture_p text, font_p font,
	const int* codepoints, int num_codepoints,
        SDL_Color color)
{
        int pitch;
        int x, y;/* 位图的x,计算位图的y (不同字符的高度不同） */
        float scale;
        float w, h;
        int bitmap_index, pixel_index;
        unsigned char* bitmap;
        int advance_width, left_side_bearing;
        int c_x1, c_y1, c_x2, c_y2;
        Uint8 alpha;
        char pixel;
        int kern;
        unsigned char* pixels;
        SDL_Renderer* renderer;

        x = 0;
        advance_width = left_side_bearing = 0;

        /* scale = fontsize / (ascent - descent) */
        scale = font->scale;

        /* 创建位图 */
	text->real_width = font->fontsize * num_codepoints; /* 位图的宽 */
	text->real_height = font->fontsize; /* 位图的高 */

        bitmap = (unsigned char*)SDL_calloc(text->real_width * text->real_height + 1, sizeof(unsigned char));

        /* 循环加载codepoints中每个字符 */
        for (int i = 0; i < num_codepoints; ++i) {
                /**
                  * 获取水平方向上的度量
                  * advanceWidth：字宽；
                  * leftSideBearing：左侧位置；
                */
                stbtt_GetCodepointHMetrics(&font->fontinfo, codepoints[i], &advance_width, &left_side_bearing);

                /* 获取字符的边框（边界） */
                stbtt_GetCodepointBitmapBox(&font->fontinfo, codepoints[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

                /* 计算位图的y (不同字符的高度不同） */
                y = font->ascent + c_y1;

                /* 渲染字符 */
                int byteOffset = x + roundf(left_side_bearing * scale) + (y * text->real_width);
                stbtt_MakeCodepointBitmap(&font->fontinfo, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, text->real_width, scale, scale, codepoints[i]);

                /* 调整x */
                x += roundf(advance_width * scale);

                /* 调整字距 */
                kern = stbtt_GetCodepointKernAdvance(&font->fontinfo, codepoints[i], codepoints[i + 1]);
                x += roundf(kern * scale);
        }

        SDL_Surface* surface = SDL_CreateSurface(text->real_width, text->real_height, SDL_PIXELFORMAT_RGBA32);
        if (!surface) return;

        Uint8* surface_pixels = (Uint8*)surface->pixels;
        pitch = surface->pitch / sizeof(Uint8);

        /* 灰度转RGBA（带透明度） */
        for (y = 0; y < text->real_height; ++y) {
                for (x = 0; x < text->real_width; ++x) {
                        bitmap_index = y * text->real_width + x;
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
		/* TODO:其它没有改变text的值 */
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


/**
 * 释放文本纹理
 */
void text_destroy(text_texture_p text)
{
	if (!text) return;
	SDL_DestroyTexture(text->texture);
	SDL_free(text);
}

/**
 * 绘制文本纹理
 */
void text_print(SDL_Renderer* renderer,
	text_texture_p text,
	float x, float y)
{
	SDL_FRect dst_rect = { x, y, text->texture->w, text->texture->h };
	SDL_RenderTexture(renderer, text->texture, NULL, &dst_rect);
}


////////////////////////////////////////////////////////////////////////
//////////////////////////////ui////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

label_p
label_create(SDL_Renderer* renderer, font_p fontinfo, const int* codepoints, int num_codepoints)
{
	label_p label = (label_p)SDL_malloc(sizeof(label_t));
	label->renderer = renderer;
	label->position = (SDL_FPoint){ 0, 0 };
	label->color = (SDL_Color){ 255, 255,255, 255 };
	label->text = text_create(fontinfo, codepoints, num_codepoints, label->color);
	return label;
}

void
label_destroy(label_p label)
{
	if (!label) return;
	if (label->text) text_destroy(label->text);
	SDL_free(label);
}

void
label_set_text(label_p label, font_p fontinfo,
	const int* codepoints, int num_codepoints,
	SDL_Color color)
{
	if (label->text) {
		text_update(label->text, fontinfo, codepoints, num_codepoints, color);
	}
	else {
		label->text = text_create(fontinfo, codepoints, num_codepoints, color);
	}
}

void label_draw(label_p label)
{
	//SDL_FRect dest_rect;
	if (!label) return;

	//renderer = label->renderer;
	//dest_rect.x = label->position.x;
	//dest_rect.y = label->position.y;
	//dest_rect.w = label->text->w;
	//dest_rect.h = label->text->h;
	//SDL_RenderTexture(renderer, label->text, NULL, &dest_rect);
	text_print(label->renderer, label->text, label->position.x, label->position.y);
}

void label_handle_event(label_p label, SDL_Event* event)
{
	SDL_Renderer* renderer;
	SDL_FPoint logic_motion_pos;
	renderer = label->renderer;
}

button_p
button_create(SDL_Renderer* renderer, SDL_FRect rect)
{
	button_p btn = (button_p)SDL_malloc(sizeof(button_t));
	int codepoint_len;
	if (!btn) return NULL;
	btn->renderer = renderer;
	btn->rect = rect;
	btn->normal_color = (SDL_Color){ 100, 100, 200, 255 };
	btn->hover_color = (SDL_Color){ 150, 150, 250, 255 };
	btn->pressed_color = (SDL_Color){ 50, 50, 150, 255 };
	btn->image = NULL;
	btn->text = NULL;
	btn->is_hovered = btn->is_pressed = false;
	return btn;
}

void button_set_normal_color(button_p btn, SDL_Color color)
{
	btn->normal_color = color;
}

void button_set_hover_color(button_p btn, SDL_Color color)
{
	btn->hover_color = color;
}

void button_set_pressed_color(button_p btn, SDL_Color color)
{
	btn->pressed_color = color;
}

void
button_set_text(button_p btn, font_p fontinfo,
	const int* codepoints, int num_codepoints, SDL_Color color)
{
	if (btn->text) {
		text_update(btn->text, fontinfo, codepoints, num_codepoints, color);
	}
	else {
		btn->text = text_create(fontinfo, codepoints, num_codepoints, color);
	}
}

void button_destroy(button_p btn)
{
	if (!btn) return;
	if (btn->text) text_destroy(btn->text);
	if (btn->image) SDL_DestroyTexture(btn->image->texture);
	SDL_free(btn);
}


void button_draw(button_p btn)
{
	SDL_Renderer* renderer;
	SDL_FRect dest_rect;
	SDL_Color color;
	if (!btn) return;

	color = btn->normal_color;
	if (btn->is_pressed) {
		color = btn->pressed_color;
	}
	else if (btn->is_hovered) {
		color = btn->hover_color;
	}

	renderer = btn->renderer;
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &btn->rect);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderRect(renderer, &btn->rect);

	if (btn->image) {
		SDL_RenderTexture(renderer, btn->image->texture, NULL, &btn->rect);
	}

	if (btn->text) {
		dest_rect.x = btn->rect.x + (btn->rect.w - btn->text->real_width) / 2.0f;
		dest_rect.y = btn->rect.y + (btn->rect.h - btn->text->real_height) / 2.0f;
		//dest_rect.w = btn->text->texture->w;
		//dest_rect.h = btn->text->texture->h;
		//SDL_RenderTexture(renderer, btn->text, NULL, &dest_rect);
		text_print(renderer, btn->text, dest_rect.x, dest_rect.y);
	}
}

void button_handle_event(button_p btn, SDL_Event* event)
{
	SDL_Renderer* renderer;
	SDL_FPoint logic_pos;
	int window_width, window_height;
	renderer = btn->renderer;
	SDL_GetRenderOutputSize(renderer, &window_width, &window_height);

	if (event->type == SDL_EVENT_FINGER_MOTION) {
		SDL_RenderCoordinatesFromWindow(renderer, event->tfinger.x * window_width, event->tfinger.y * window_height, &logic_pos.x, &logic_pos.y);
		btn->is_hovered = SDL_PointInRectFloat(&logic_pos, &btn->rect);
	}
	else if (event->type == SDL_EVENT_FINGER_DOWN) {
		SDL_RenderCoordinatesFromWindow(renderer, event->tfinger.x * window_width, event->tfinger.y * window_height, &logic_pos.x, &logic_pos.y);
		if (SDL_PointInRectFloat(&logic_pos, &btn->rect)) {
			btn->is_pressed = true;
		}
	}
	else if (event->type == SDL_EVENT_FINGER_UP) {
		btn->is_pressed = false;
	}
	else if (event->type == SDL_EVENT_MOUSE_MOTION) {
		if (event->motion.which == SDL_TOUCH_MOUSEID) {
			return;
		}
		SDL_RenderCoordinatesFromWindow(renderer, event->motion.x, event->motion.y, &logic_pos.x, &logic_pos.y);
		btn->is_hovered = SDL_PointInRectFloat(&logic_pos, &btn->rect);
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (event->button.which == SDL_TOUCH_MOUSEID) {
			return;
		}
		if (event->button.button == SDL_BUTTON_LEFT) {
			SDL_RenderCoordinatesFromWindow(renderer, event->button.x, event->button.y, &logic_pos.x, &logic_pos.y);
			if (SDL_PointInRectFloat(&logic_pos, &btn->rect)) {
				btn->is_pressed = true;
			}
		}
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
		if (event->button.which == SDL_TOUCH_MOUSEID) {
			return;
		}
		if (event->button.button == SDL_BUTTON_LEFT) {
			btn->is_pressed = false;
		}
	}
}

combobox_p combobox_create(SDL_Renderer* renderer, SDL_FRect rect)
{
	combobox_p combobox;
	combobox = (combobox_p)SDL_malloc(sizeof(combobox_t));
	combobox->renderer = renderer;
	combobox->text = NULL;
	combobox->main_rect = rect;
	combobox->dropdown_rect.x = rect.x;
	combobox->dropdown_rect.y = rect.y + rect.h;
	combobox->dropdown_rect.w = rect.w;
	combobox->dropdown_rect.h = 0;
	combobox->item_count = 0;
	combobox->item_height = 30;
	combobox->selected_index = -1;
	combobox->is_open = false;
	combobox->bg_color = (SDL_Color){ 240, 240, 240, 255 };
	combobox->text_color = (SDL_Color){ 0, 0, 0, 255 };
	combobox->hover_color = (SDL_Color){ 150, 150, 250, 255 };
	return combobox;
}

void combobox_destroy(combobox_p combobox)
{
	if (combobox) {
		SDL_free(combobox);
	}
}

void
combobox_set_text(combobox_p combobox, const char* filename, int fontsize,
	const int* codepoints, int len, SDL_Color color)
{
	if (combobox->text) {
		SDL_DestroyTexture(combobox->text);
	}
	//combobox->text = create_text_texture(combobox->renderer, filename, fontsize, codepoints, len, color);
}

void combobox_add_item(combobox_p combobox, const char* filename, int fontsize, const int* codepoints, int len, SDL_Color color)
{
	combobox_item_p item;
	SDL_Renderer* renderer;
	if (combobox->item_count > 20) {
		return;
	}
	renderer = combobox->renderer;
	item = combobox->items + combobox->item_count;
	//item->text = create_text_texture(renderer, filename, fontsize, codepoints, len, color);
	item->is_selected = false;
	combobox->item_count++;
	combobox->dropdown_rect.h = combobox->item_count * combobox->item_height;
}

bool combobox_handle_event(combobox_p combobox, SDL_Event* event)
{
	SDL_Renderer* renderer;
	SDL_FPoint logic_button_pos;

	renderer = combobox->renderer;
	if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		SDL_RenderCoordinatesFromWindow(renderer, event->button.x, event->button.y, &logic_button_pos.x, &logic_button_pos.y);

		// 检查是否点击了主矩形区域
		if (SDL_PointInRectFloat(&logic_button_pos, &combobox->main_rect)) {
			combobox->is_open = !combobox->is_open;
			return true;
		}

		// 如果下拉框是打开的，检查是否点击了选项
		if (combobox->is_open) {
			for (int i = 0; i < combobox->item_count; i++) {
				SDL_FRect item_rect = {
				    combobox->dropdown_rect.x,
				    combobox->dropdown_rect.y + i * combobox->item_height,
				    combobox->dropdown_rect.w,
				    combobox->item_height
				};

				if (SDL_PointInRectFloat(&logic_button_pos, &item_rect)) {
					combobox->selected_index = i;
					combobox->is_open = false;
					return true;
				}
			}

			// 如果点击了下拉框外部，关闭下拉框
			if (!SDL_PointInRectFloat(&logic_button_pos, &combobox->dropdown_rect)) {
				combobox->is_open = false;
				return true;
			}
		}
	}

	return false;
}

void combobox_draw(combobox_p combobox)
{
	SDL_Renderer* renderer;
	renderer = combobox->renderer;
	SDL_SetRenderDrawColor(renderer, combobox->bg_color.r, combobox->bg_color.g, combobox->bg_color.b, combobox->bg_color.a);
	SDL_RenderFillRect(renderer, &combobox->main_rect);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderRect(renderer, &combobox->main_rect);

	// 渲染当前选中的文本
	if (combobox->selected_index >= 0) {
		SDL_Point point;
		//char* text;
		point.x = combobox->main_rect.x;
		point.y = combobox->main_rect.y;
		//text = combobox->items[combobox->selected_index].text;
		//render_text(combobox->font_info, 18, text, SDL_strlen(text), point, renderer);
	}


	// 如果下拉框是打开的，渲染选项列表
	if (combobox->is_open) {
		// 渲染下拉箭头
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_Vertex arrowPoints[3] = {
		    {combobox->main_rect.x + combobox->main_rect.w - 25, combobox->main_rect.y + 10},
		    {combobox->main_rect.x + combobox->main_rect.w - 5, combobox->main_rect.y + 10},
		    {combobox->main_rect.x + combobox->main_rect.w - 15, combobox->main_rect.y + 20}
		};
		SDL_RenderGeometry(renderer, NULL, arrowPoints, 3, NULL, 0);


		SDL_SetRenderDrawColor(renderer, combobox->bg_color.r, combobox->bg_color.g, combobox->bg_color.b, combobox->bg_color.a);
		SDL_RenderFillRect(renderer, &combobox->dropdown_rect);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderRect(renderer, &combobox->dropdown_rect);

		combobox_item_p item;

		for (int i = 0; i < combobox->item_count; i++) {
			SDL_FRect item_rect = {
			    combobox->dropdown_rect.x,
			    combobox->dropdown_rect.y + i * combobox->item_height,
			    combobox->dropdown_rect.w,
			    combobox->item_height
			};

			// 如果是当前选中的项，使用悬停颜色
			if (i == combobox->selected_index) {
				SDL_SetRenderDrawColor(renderer, combobox->hover_color.r, combobox->hover_color.g, combobox->hover_color.b, combobox->hover_color.a);
				SDL_RenderFillRect(renderer, &item_rect);
			}

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderRect(renderer, &item_rect);

			item = combobox->items + i;
			SDL_Point point;
			point.x = item_rect.x + 5;
			point.y = item_rect.y;

			//render_text(combobox->font_info, 18, item->text, SDL_strlen(item->text), point, renderer);
		}
	}
	else {
		// 渲染下拉箭头
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_Vertex arrowPoints[3] = {
		    {combobox->main_rect.x + combobox->main_rect.w - 15, combobox->main_rect.y + 10},
		    {combobox->main_rect.x + combobox->main_rect.w - 25, combobox->main_rect.y + 20},
		    {combobox->main_rect.x + combobox->main_rect.w - 5, combobox->main_rect.y + 20}
		};
		SDL_RenderGeometry(renderer, NULL, arrowPoints, 3, NULL, 0);
	}
}


datagrid_p
datagrid_create(SDL_Renderer* renderer, SDL_Rect rect, int col_count, bool has_header, void* font)
{
	datagrid_p grid;
	grid = (datagrid_p)SDL_malloc(sizeof(datagrid_t));
	if (!grid) return NULL;
	SDL_memset(grid, 0, sizeof(datagrid_t));

	grid->renderer = renderer;
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
	//grid->font = font_new(renderer, "resources/fonts/simhei.ttf", 20);
	grid->data = NULL;
	grid->col_widths = (int*)SDL_malloc(col_count * sizeof(int));
	grid->bg_color = (SDL_Color){ 0xFF, 0xFF, 0xFF, 0xFF };
	grid->grid_color = (SDL_Color){ 0xCC, 0xCC, 0xCC, 0xFF };
	grid->text_color = (SDL_Color){ 0x00, 0x00, 0x00, 0xFF };
	grid->header_bg_color = (SDL_Color){ 0xEE, 0xEE, 0xEE, 0xFF };
	grid->selected_color = (SDL_Color){ 0x33, 0x99, 0xFF, 0xFF };
	SDL_memset(grid->headers, 0, 512 * sizeof(codepoint_array_p));
	int default_width = grid->width / col_count;
	for (int i = 0; i < col_count; i++) {
		grid->col_widths[i] = default_width;
	}
	return grid;
}


void datagrid_setheaders(datagrid_p grid, const codepoint_array_p* headers)
{
	font_p fontinfo;
	SDL_Color color;
	color = (SDL_Color){ 0x0, 0x0, 0x0, 0xFF };
	if (!grid->has_header) return;
	fontinfo = grid->font;
	for (int i = 0; i < grid->col_count; i++) {
		//grid->headers[i] = create_text_texture(grid->renderer, fontinfo, headers[i]->array, headers[i]->length, color);
	}
}

void datagrid_setheader(datagrid_p grid, int index, const codepoint_array_p header)
{
	font_p fontinfo;
	SDL_Color color;
	if (!grid->has_header) return;
	if (index < grid->col_count) return;
	color = (SDL_Color){ 0x0, 0x0, 0x0, 0xFF };
	fontinfo = grid->font;
	if (grid->headers[index]) {
		//SDL_DestroyTexture(grid->headers[index]);
		//SDL_UpdateTexture(grid->headers[index], NULL, NULL, 0);
		//update_text_texture(grid->headers[index], fontinfo, header->array, header->length, color);
	}
	else {
		//grid->headers[index] = create_text_texture(grid->renderer, fontinfo, header->array, header->length, color);
	}
}

void datagrid_addrow(datagrid_p grid, const char** row_data)
{
	grid->row_count++;
	grid->data = SDL_realloc(grid->data, grid->row_count * sizeof(char**));
	grid->data[grid->row_count - 1] = SDL_malloc(grid->col_count * sizeof(char*));
	for (int i = 0; i < grid->col_count; i++) {
		grid->data[grid->row_count - 1][i] = row_data[i] ? SDL_strdup(row_data[i]) : NULL;
	}
}

void datagrid_setcell(datagrid_p grid, int row_index, int col_index, const char* data)
{
	if (!grid->has_header) return;
	if (row_index < grid->row_count && col_index < grid->col_count) {
		if (grid->data[row_index][col_index]) {
			SDL_free(grid->data[row_index][col_index]);
		}
		grid->data[row_index][col_index] = SDL_strdup(data);
	}
}

static void datagrid_cleardata(datagrid_p grid)
{
	for (int i = (grid->has_header ? 1 : 0); i < grid->row_count; i++) {
		for (int j = 0; j < grid->col_count; j++) {
			if (grid->data[i][j]) SDL_free(grid->data[i][j]);
		}
		SDL_free(grid->data[i]);
	}

	if (grid->row_count > (grid->has_header ? 1 : 0)) {
		SDL_free(grid->data);
		grid->data = NULL;
	}

	grid->row_count = grid->has_header ? 1 : 0;
}


void datagrid_destroy(datagrid_p grid)
{
	if (!grid) return;

	if (grid->has_header) {
		for (int i = 0; i < grid->col_count; i++) {
			if (grid->headers[i]) SDL_free(grid->headers[i]);
		}
		SDL_free(grid->headers);
	}

	// 释放数据
	datagrid_cleardata(grid);

	// 释放其他资源
	SDL_free(grid->col_widths);
	SDL_free(grid);
}

void datagrid_draw(datagrid_p grid, SDL_Renderer* renderer)
{
	// 计算可见行范围
	int start_row = grid->scroll_offset;
	int end_row = start_row + grid->visible_rows;
	if (end_row > grid->row_count) end_row = grid->row_count;

	// 绘制背景
	SDL_Color bg_color = grid->bg_color;
	SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	SDL_FRect bg_rect = { grid->x, grid->y, grid->width, grid->height };
	SDL_RenderFillRect(renderer, &bg_rect);

	// 绘制网格线和内容
	SDL_Color grid_color = grid->grid_color;
	SDL_Color text_color = grid->text_color;
	SDL_Color header_bg_color = grid->header_bg_color;
	SDL_Color selected_color = grid->selected_color;

	// 绘制标题
	SDL_SetRenderDrawColor(renderer, header_bg_color.r, header_bg_color.g, header_bg_color.b, header_bg_color.a);
	SDL_FRect header_rect = { grid->x, grid->y, grid->width, grid->row_height };
	SDL_RenderFillRect(renderer, &header_rect);

	{
		int x = grid->x;
		int y = grid->y;
		for (int col = 0; col < grid->col_count; col++) {
			SDL_FRect rect;
			rect.x = x;
			rect.y = y;
			rect.w = grid->headers[col]->w;
			rect.h = grid->headers[col]->h;
			SDL_RenderTexture(grid->renderer, grid->headers[col], NULL, &rect);
			//render_text(grid->font, 18, text, SDL_strlen(text), point, renderer);

			// 绘制垂直线
			SDL_SetRenderDrawColor(renderer, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
			SDL_RenderLine(renderer, x + grid->col_widths[col], y, x + grid->col_widths[col], y + grid->row_height);

			x += grid->col_widths[col];
		}
	}

	// 绘制数据内容
	for (int row = start_row; row < end_row; row++) {
		int y = grid->y + (row - start_row) * grid->row_height + grid->row_height;

		// 绘制行背景
		if (row == grid->selected_row) {
			SDL_SetRenderDrawColor(renderer, selected_color.r, selected_color.g, selected_color.b, selected_color.a);
			SDL_FRect selected_rect = { grid->x, y, grid->width, grid->row_height };
			SDL_RenderFillRect(renderer, &selected_rect);
		}
		else {
			/* SDL_SetRenderDrawColor(renderer, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
			 SDL_FRect header_rect = { grid->x, y, grid->width, grid->row_height };
			 SDL_RenderFillRect(renderer, &header_rect);*/
		}

		// 绘制单元格内容和网格线
		int x = grid->x;
		for (int col = 0; col < grid->col_count; col++) {
			//text = grid->data[row][col];
			//if (text) {
			//        SDL_Point point;
			//        point.x = x;
			//        point.y = y;
			//        // grid->col_widths[col], grid->row_height
			//        //render_text(grid->font, 18, text, SDL_strlen(text), point, renderer);
			//}

			// 绘制垂直线
			SDL_SetRenderDrawColor(renderer, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
			SDL_RenderLine(renderer, x + grid->col_widths[col], y, x + grid->col_widths[col], y + grid->row_height);

			x += grid->col_widths[col];
		}

		// 绘制水平线
		SDL_SetRenderDrawColor(renderer, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
		SDL_RenderLine(renderer, grid->x, y + grid->row_height, grid->x + grid->width, y + grid->row_height);
	}

	// 绘制外边框
	SDL_SetRenderDrawColor(renderer, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
	SDL_RenderRect(renderer, &bg_rect);
}

void datagrid_handle_event(datagrid_p grid, SDL_Event* event, SDL_Renderer* renderer)
{
	SDL_FPoint logic_button_pos;
	SDL_FRect rect;
	if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		//logic_button_pos.x = event->button.x;
		//logic_button_pos.y = event->button.y;

		SDL_RenderCoordinatesFromWindow(renderer, event->button.x, event->button.y, &logic_button_pos.x, &logic_button_pos.y);


		rect.x = grid->x;
		rect.y = grid->y + grid->row_height;
		rect.w = grid->width;
		rect.h = grid->height;

		// 检查点击是否在DataGrid范围内
		if (SDL_PointInRectFloat(&logic_button_pos, &rect)) {
			// 计算点击的行
			int relative_y = logic_button_pos.y - rect.y;
			int clicked_row = grid->scroll_offset + relative_y / grid->row_height;

			if (clicked_row < grid->row_count) {
				grid->selected_row = clicked_row;
			}
		}
	}
	else if (event->type == SDL_EVENT_MOUSE_WHEEL) {
		// 处理滚轮事件
		int scroll_amount = event->wheel.y * 3; // 调整滚动速度

		grid->scroll_offset -= scroll_amount;

		// 限制滚动范围
		int max_scroll = grid->row_count - grid->visible_rows;
		if (max_scroll < 0) max_scroll = 0;

		if (grid->scroll_offset < 0)
			grid->scroll_offset = 0;
		if (grid->scroll_offset > max_scroll)
			grid->scroll_offset = max_scroll;
	}
}

radio_button_p radio_button_create(int x, int y, int size, const char* label, int group_id)
{
	radio_button_p radio;
	radio = (radio_button_p)SDL_malloc(sizeof(radio_button_t));
	if (!radio) return NULL;

	radio->x = x;
	radio->y = y;
	radio->size = size;
	radio->checked = false;
	radio->group_id = group_id;
	radio->hover = false;

	// 复制标签文本
	if (label) {
		radio->label = SDL_strdup(label);
	}
	else {
		radio->label = NULL;
	}

	// 设置点击区域（包括标签区域）
	int label_width = label ? strlen(label) * 10 : 0; // 估算标签宽度
	radio->hit_area.x = x;
	radio->hit_area.y = y;
	radio->hit_area.w = size + 5 + label_width;
	radio->hit_area.h = size;

	return radio;
}

void radio_button_destroy(radio_button_p radio)
{
	if (!radio) return;

	if (radio->label) {
		SDL_free(radio->label);
	}

	SDL_free(radio);
}


radio_button_group_p radio_button_group_create()
{
	radio_button_group_p group;
	group = (radio_button_group_p)SDL_malloc(sizeof(radio_button_group_t));
	if (!group) return NULL;

	group->capacity = 16;
	group->buttons = SDL_malloc(group->capacity * sizeof(radio_button_p));
	group->count = 0;
	group->selected_index = -1;

	return group;
}

void radio_button_group_add_button(radio_button_group_p group, radio_button_p radio)
{
	if (!group || !radio) return;

	group->count++;
	if (group->capacity < group->count) {
		group->capacity *= 2;
		group->buttons = SDL_realloc(group->buttons, group->capacity * sizeof(radio_button_p));
	}
	group->buttons[group->count - 1] = radio;
}

void radio_button_group_set_selected(radio_button_group_p group, int index)
{
	if (!group || index < 0 || index >= group->count) return;

	// 取消之前选中的按钮
	if (group->selected_index >= 0 && group->selected_index < group->count) {
		group->buttons[group->selected_index]->checked = false;
	}

	// 设置新的选中按钮
	group->selected_index = index;
	group->buttons[index]->checked = true;
}

// 通过按钮指针设置选中
void radio_button_group_set_selected_by_button(radio_button_group_p group, radio_button_p selected)
{
	if (!group || !selected) return;
	for (int i = 0; i < group->count; i++) {
		if (group->buttons[i] == selected) {
			radio_button_group_set_selected(group, i);
			return;
		}
	}
}

int radio_button_group_get_selected_index(radio_button_group_p group)
{
	return group ? group->selected_index : -1;
}

radio_button_p radio_button_group_get_selected_button(radio_button_group_p group)
{
	if (!group || group->selected_index < 0 || group->selected_index >= group->count) {
		return NULL;
	}
	return group->buttons[group->selected_index];
}

// 释放单选按钮组内存
void radio_button_group_destroy(radio_button_group_p group)
{
	if (!group) return;

	// 注意：这里不释放单个按钮的内存，由创建者负责
	SDL_free(group->buttons);
	SDL_free(group);
}

void radio_button_render(radio_button_p radio, SDL_Renderer* renderer)
{
	if (!radio) return;

	float center_x = radio->x + radio->size / 2;
	float center_y = radio->y + radio->size / 2;
	float r1 = radio->size / 2;
	float r2 = r1 - 2;
	float r3 = r2 - 4;

	// 使用足够多的点来绘制平滑的圆
	const int segments = 99; // 增加分段数使圆更平滑
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


	SDL_Vertex triangle[3];
	for (int i = 0; i < segments; i++) {
		float angle1 = (float)i / (float)segments * 2.0f * 3.14159f;
		float angle2 = (float)(i + 1) / (float)segments * 2.0f * 3.14159f;
		triangle[0].position = (SDL_FPoint){ center_x, center_y };
		triangle[0].color = radio->r1_color;
		triangle[1].position = (SDL_FPoint){ center_x + cosf(angle1) * r1, center_y + sinf(angle1) * r1 };
		triangle[1].color = radio->r1_color;
		triangle[2].position = (SDL_FPoint){ center_x + cosf(angle2) * r1, center_y + sinf(angle2) * r1 };
		triangle[2].color = radio->r1_color;
		SDL_RenderGeometry(renderer, NULL, triangle, 3, NULL, 0);

		triangle[0].position = (SDL_FPoint){ center_x, center_y };
		triangle[0].color = radio->r2_color;
		triangle[1].position = (SDL_FPoint){ center_x + cosf(angle1) * r2, center_y + sinf(angle1) * r2 };
		triangle[1].color = radio->r2_color;
		triangle[2].position = (SDL_FPoint){ center_x + cosf(angle2) * r2, center_y + sinf(angle2) * r2 };
		triangle[2].color = radio->r2_color;
		SDL_RenderGeometry(renderer, NULL, triangle, 3, NULL, 0);

		if (radio->checked) {
			triangle[0].position = (SDL_FPoint){ center_x, center_y };
			triangle[0].color = radio->r3_color;
			triangle[1].position = (SDL_FPoint){ center_x + cosf(angle1) * r3, center_y + sinf(angle1) * r3 };
			triangle[1].color = radio->r3_color;
			triangle[2].position = (SDL_FPoint){ center_x + cosf(angle2) * r3, center_y + sinf(angle2) * r3 };
			triangle[2].color = radio->r3_color;
			SDL_RenderGeometry(renderer, NULL, triangle, 3, NULL, 0);
		}
	}

	// 绘制标签文本
	if (radio->label) {
		/* SDL_Color text_color = { 0, 0, 0, 255 };
		 SDL_Surface* text_surface = TTF_RenderText_Blended(font, radio->label, text_color);

		 if (text_surface) {
			 SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

			 if (text_texture) {
				 SDL_FRect text_rect = {
				     radio->x + radio->size + 5,
				     radio->y + (radio->size - text_surface->h) / 2,
				     text_surface->w,
				     text_surface->h
				 };

				 SDL_RenderTexture(renderer, text_texture, NULL, &text_rect);
				 SDL_DestroyTexture(text_texture);
			 }

			 SDL_DestroySurface(text_surface);
		 }*/
	}
}

void radio_button_group_draw(radio_button_group_p group, SDL_Renderer* renderer)
{
	if (!group) return;

	for (int i = 0; i < group->count; i++) {
		radio_button_render(group->buttons[i], renderer);
	}
}

// 处理单选框事件
bool radio_button_handle_event(radio_button_p radio, SDL_Event* event, radio_button_group_p group, SDL_Renderer* renderer)
{
	SDL_FPoint mouse_pos;

	if (!radio || !event) return false;

	if (event->type == SDL_EVENT_MOUSE_MOTION) {
		// 检查鼠标悬停
	       /* int mouse_x = event->motion.x;
		int mouse_y = event->motion.y;*/
		SDL_RenderCoordinatesFromWindow(renderer, event->motion.x, event->motion.y, &mouse_pos.x, &mouse_pos.y);


		bool was_hover = radio->hover;
		/*radio->hover = (mouse_x >= radio->hit_area.x &&
			mouse_x <= radio->hit_area.x + radio->hit_area.w &&
			mouse_y >= radio->hit_area.y &&
			mouse_y <= radio->hit_area.y + radio->hit_area.h);*/
		radio->hover = SDL_PointInRectFloat(&mouse_pos, &radio->hit_area);

		return (was_hover != radio->hover); // 返回悬停状态是否改变
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		// 检查鼠标点击
	       /* int mouse_x = event->button.x;
		int mouse_y = event->button.y;*/
		SDL_RenderCoordinatesFromWindow(renderer, event->button.x, event->button.y, &mouse_pos.x, &mouse_pos.y);


		if (SDL_PointInRectFloat(&mouse_pos, &radio->hit_area)) {
			// 如果是单选组，通过组设置选中状态
			if (group) {
				radio_button_group_set_selected_by_button(group, radio);
			}
			else {
				// 如果没有组，单独切换状态
				radio->checked = !radio->checked;
			}

			return true;
		}
	}

	return false;
}

// 处理单选按钮组事件
bool radio_button_group_handle_event(radio_button_group_p group, SDL_Event* event, SDL_Renderer* renderer)
{
	if (!group || !event) return false;

	bool handled = false;

	for (int i = 0; i < group->count; i++) {
		if (radio_button_handle_event(group->buttons[i], event, group, renderer)) {
			handled = true;
		}
	}

	return handled;
}


checkbox_p checkbox_create(int x, int y, int size, const char* label)
{
	checkbox_p cb;
	cb = SDL_malloc(sizeof(checkbox_t));
	if (!cb) return NULL;

	cb->x = x;
	cb->y = y;
	cb->size = size;
	cb->state = LIVNET_CHECKBOX_UNCHECKED;
	cb->label = label ? SDL_strdup(label) : NULL;
	cb->hover = false;
	cb->enabled = true;

	// 设置默认颜色
	cb->box_color = (SDL_Color){ 0, 0, 0, 255 };        // 黑色边框
	cb->check_color = (SDL_Color){ 0, 120, 0, 255 };    // 绿色勾选
	cb->text_color = (SDL_Color){ 0, 0, 0, 255 };       // 黑色文本
	cb->hover_color = (SDL_Color){ 200, 200, 200, 255 };// 浅灰色悬停

	// 设置点击区域（包括标签）
	int label_width = 0;
	if (label) {
		// 估算标签宽度（实际渲染时需要精确计算）
		label_width = SDL_strlen(label) * 8;
	}
	cb->hit_area.x = x;
	cb->hit_area.y = y;
	cb->hit_area.w = size + 5 + label_width;
	cb->hit_area.h = size;
	return cb;
}


void checkbox_destroy(checkbox_p checkbox)
{
	if (!checkbox) return;

	if (checkbox->label) {
		SDL_free(checkbox->label);
	}

	SDL_free(checkbox);
}

void checkbox_set_state(checkbox_p checkbox, checkbox_state_t state)
{
	if (!checkbox) return;
	checkbox->state = state;
}

void checkbox_toggle(checkbox_p checkbox)
{
	if (!checkbox || !checkbox->enabled) return;

	if (checkbox->state == LIVNET_CHECKBOX_UNCHECKED) {
		checkbox->state = LIVNET_CHECKBOX_CHECKED;
	}
	else {
		checkbox->state = LIVNET_CHECKBOX_UNCHECKED;
	}
}


// 处理复选框事件
bool checkbox_handle_event(checkbox_p checkbox, SDL_Event* event, SDL_Renderer* renderer)
{
	SDL_FPoint logic_mouse_pos;
	if (!checkbox || !checkbox->enabled) return false;

	if (event->type == SDL_EVENT_MOUSE_MOTION) {
		bool was_hover = checkbox->hover;
		SDL_RenderCoordinatesFromWindow(renderer, event->motion.x, event->motion.y, &logic_mouse_pos.x, &logic_mouse_pos.y);
		checkbox->hover = SDL_PointInRectFloat(&logic_mouse_pos, &checkbox->hit_area);
		return (was_hover != checkbox->hover);
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		SDL_RenderCoordinatesFromWindow(renderer, event->button.x, event->button.y, &logic_mouse_pos.x, &logic_mouse_pos.y);
		if (SDL_PointInRectFloat(&logic_mouse_pos, &checkbox->hit_area)) {
			checkbox_toggle(checkbox);
			return true;
		}
	}

	return false;
}

void checkbox_draw(checkbox_p checkbox, SDL_Renderer* renderer, void* font)
{
	if (!checkbox) return;

	// 绘制复选框背景（如果悬停）
	if (checkbox->hover && checkbox->enabled) {
		SDL_SetRenderDrawColor(renderer,
			checkbox->hover_color.r,
			checkbox->hover_color.g,
			checkbox->hover_color.b,
			checkbox->hover_color.a);
		SDL_FRect bgRect = {
		    checkbox->x - 2,
		    checkbox->y - 2,
		    checkbox->size + 4,
		    checkbox->size + 4
		};
		SDL_RenderFillRect(renderer, &bgRect);
	}

	// 绘制复选框外框
	SDL_SetRenderDrawColor(renderer,
		checkbox->box_color.r,
		checkbox->box_color.g,
		checkbox->box_color.b,
		checkbox->box_color.a);
	SDL_FRect boxRect = {
	    checkbox->x,
	    checkbox->y,
	    checkbox->size,
	    checkbox->size
	};
	SDL_RenderRect(renderer, &boxRect);

	// 如果被选中，绘制勾选标记
	if (checkbox->state != LIVNET_CHECKBOX_UNCHECKED && checkbox->enabled) {
		SDL_SetRenderDrawColor(renderer,
			checkbox->check_color.r,
			checkbox->check_color.g,
			checkbox->check_color.b,
			checkbox->check_color.a);

		if (checkbox->state == LIVNET_CHECKBOX_CHECKED) {
			// 绘制勾选标记（对号）
			const float padding = checkbox->size * 0.2f;
			SDL_FPoint points[3] = {
			    {checkbox->x + padding, checkbox->y + checkbox->size * 0.5f},
			    {checkbox->x + checkbox->size * 0.4f, checkbox->y + checkbox->size - padding},
			    {checkbox->x + checkbox->size - padding, checkbox->y + padding}
			};
			SDL_RenderLines(renderer, points, 3);
		}
		else {
			// 绘制不确定状态（横线）
			const float padding = checkbox->size * 0.3f;
			SDL_FPoint points[2] = {
			    {checkbox->x + padding, checkbox->y + checkbox->size * 0.5f},
			    {checkbox->x + checkbox->size - padding, checkbox->y + checkbox->size * 0.5f}
			};
			SDL_RenderLine(renderer, points[0].x, points[0].y, points[1].x, points[1].y);
		}
	}

	// 如果禁用，绘制半透明效果
	if (!checkbox->enabled) {
		SDL_SetRenderDrawColor(renderer, 200, 200, 200, 128);
		SDL_RenderFillRect(renderer, &boxRect);
	}

	// 绘制标签文本
	if (checkbox->label && font) {
		/*SDL_Color textColor = checkbox->enabled ? checkbox->textColor : (SDL_Color) { 128, 128, 128, 255 };
		SDL_Surface* textSurface = TTF_RenderText_Blended(font, checkbox->label, textColor);

		if (textSurface) {
			SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

			if (textTexture) {
				SDL_FRect textRect = {
				    checkbox->x + checkbox->size + 5,
				    checkbox->y + (checkbox->size - textSurface->h) / 2,
				    textSurface->w,
				    textSurface->h
				};

				SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
				SDL_DestroyTexture(textTexture);
			}

			SDL_DestroySurface(textSurface);
		}*/
	}
}


