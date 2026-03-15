#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image.h"
#include "external/stb_image_write.h"
#include "textures.h"
#include "mathx.h"


struct image {
	SDL_Texture* texture;
	char filepath[JOY_MAX_PATH];
};

 struct sprite_batch {
	image_p image;
	SDL_Vertex* vertices;
	int* indices;
	int num_vertices;
	int num_indices;
	int index_vertices;
	int index_indices;
};

static image_p load_bmp(SDL_Renderer* renderer, const char* filename)
{
	char filepath[JOY_MAX_PATH] = { 0 };
	image_p image;
	//SDL_strlcat(filepath, SDL_GetBasePath(), JOY_MAX_PATH);
	SDL_strlcat(filepath, filename, JOY_MAX_PATH);
	SDL_Surface* surface = SDL_LoadBMP(filepath);
	if (!surface) {
		return NULL;
	}
	image = (image_p)SDL_malloc(sizeof(image_t));
	SDL_assert(image);
	SDL_strlcpy(image->filepath, filepath, JOY_MAX_PATH);
	image->texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);
	return image;
}

static image_p load_png(SDL_Renderer* renderer, const char* filename)
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

	image = (image_p)SDL_malloc(sizeof(image_t));
	SDL_assert(image);
	//SDL_strlcat(filepath, SDL_GetBasePath(), JOY_MAX_PATH);
	SDL_strlcat(filepath, filename, JOY_MAX_PATH);
	pixels = stbi_load_from_memory(filedata, sz, &w, &h, &channels, STBI_rgb_alpha);
	surface = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, pixels, w * 4);
	image->texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);
	SDL_free(filedata);
	SDL_strlcpy(image->filepath, filepath, JOY_MAX_PATH);
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

void image_draw1(image_p image,
	const SDL_FRect* srcrect, const SDL_FRect* destrect)
{
	SDL_Renderer* renderer;
	renderer = SDL_GetRendererFromTexture(image->texture);
	SDL_RenderTexture(renderer, image->texture, srcrect, destrect);
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

sprite_batch_p
sprite_batch_create(SDL_Renderer* renderer, const char* filename)
{
	const int capacity = 256;
	sprite_batch_p batch;
	batch = (sprite_batch_p)SDL_malloc(sizeof(sprite_batch_t));
	batch->image = image_create(renderer, filename);
	batch->num_indices = capacity * 6;
	batch->num_vertices = capacity * 4;
	batch->index_indices = 0;
	batch->index_vertices = 0;
	batch->vertices = (SDL_Vertex*)SDL_malloc(sizeof(SDL_Vertex) * batch->num_vertices);
	batch->indices = (int*)SDL_malloc(sizeof(int) * batch->num_indices);
	return batch;
}

void sprite_batch_destroy(sprite_batch_p batch)
{
	SDL_free(batch->vertices);
	SDL_free(batch->indices);
	image_destroy(batch->image);
	SDL_free(batch);
}

void sprite_batch_add(sprite_batch_p batch,
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

	/* Ĭ��ͼƬ���ĵ� */
	center.x = dest_rect.x + dest_rect.w / 2;
	center.y = dest_rect.y + dest_rect.h / 2;

	sprite_batch_add_ex(batch, src_rect, dest_rect, rotation, center);
}

float sprite_batch_get_width(sprite_batch_p batch)
{
	return batch->image->texture->w;
}

float sprite_batch_get_height(sprite_batch_p batch)
{
	return batch->image->texture->h;
}

void sprite_batch_add_ex(sprite_batch_p batch,
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

		// ��ת
		translated.x = vertex->position.x - origin.x;
		translated.y = vertex->position.y - origin.y;
		translated = vec2_rotate(translated, angle);

		// ƽ�ƻ�ԭλ��
		vertex->position.x = origin.x + translated.x;
		vertex->position.y = origin.y + translated.y;
	}
}

void sprite_batch_clear(sprite_batch_p batch)
{
	if (batch) {
		batch->index_indices = 0;
		batch->index_vertices = 0;
	}
}

void sprite_batch_set_image(sprite_batch_p batch, const char* filename)
{
	SDL_Renderer* renderer;
	if (batch) {
		renderer = SDL_GetRendererFromTexture(batch->image->texture);
		image_destroy(batch->image);
		batch->image = image_create(renderer, filename);
	}
}

void sprite_batch_draw(sprite_batch_p batch)
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
