#include "jutils.h"
#include "jtext.h"

struct text_texture {
	SDL_Texture *texture;
	int real_width;
	int real_height;
};

text_texture_p text_createx(font_p font, 
	const char* str, int len, SDL_Color color) {
	text_texture_p texture;
	int capacity_codepoints = len * 4;
	int* codepoints = (int*)SDL_malloc(sizeof(int) * capacity_codepoints);
        int num_codepoints = utf8_decode_all(str, len, codepoints, capacity_codepoints);
        texture = text_create(font, codepoints, num_codepoints, color);
	SDL_free(codepoints);
	return texture;
}

text_texture_p text_create(font_p font, 
	const int* codepoints, int num_codepoints, 
	SDL_Color color)
{
	int x, y;
	float scale;
	int bitmap_index, pixel_index;
	unsigned char* bitmap;
	int advance_width, left_side_bearing;
	int c_x1, c_y1, c_x2, c_y2;
	SDL_Surface* surface;
	unsigned char* surface_pixels;
	Uint8 pixel;
	text_texture_p text;
	int kern;

	x = 0;
	advance_width = left_side_bearing = 0;
	scale = font->scale;

	text = (text_texture_p)SDL_malloc(sizeof(text_texture_t));
	text->real_width = font->fontsize * num_codepoints;
	text->real_height = font->fontsize;
	bitmap = SDL_calloc(text->real_width * text->real_height + 1, sizeof(unsigned char));

	for (int i = 0; i < num_codepoints; ++i) {
		stbtt_GetCodepointHMetrics(&font->fontinfo, codepoints[i], &advance_width, &left_side_bearing);
		stbtt_GetCodepointBitmapBox(&font->fontinfo, codepoints[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
		y = font->ascent + c_y1;

		int byte_offset = x + roundf(left_side_bearing * scale) + (y * text->real_width);
		stbtt_MakeCodepointBitmap(&font->fontinfo, bitmap + byte_offset, c_x2 - c_x1, c_y2 - c_y1, text->real_width, scale, scale, codepoints[i]);
		x += roundf(advance_width * scale);
		kern = stbtt_GetCodepointKernAdvance(&font->fontinfo, codepoints[i], codepoints[i + 1]);
		x += roundf(kern * scale);
	}

	int bitmap_width = text->real_width;
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
	text->texture = SDL_CreateTextureFromSurface(font->renderer, surface);
	SDL_free(bitmap);
	SDL_DestroySurface(surface);
	return text;
}

void text_updatex(text_texture_p text, font_p font,
	const char* str, int len, SDL_Color color) {
	text_texture_p texture;
	int capacity_codepoints = len * 4;
	int* codepoints = (int*)SDL_malloc(sizeof(int) * capacity_codepoints);
	int num_codepoints = utf8_decode_all(str, len, codepoints, capacity_codepoints);
	text_update(text, font, codepoints, num_codepoints, color);
	SDL_free(codepoints);
}

void text_update(text_texture_p text, font_p font,
	const int* codepoints, int num_codepoints,
        SDL_Color color)
{
        int x, y;
        float scale;
        float w, h;
        int bitmap_index, pixel_index;
        unsigned char* bitmap;
        int advance_width, left_side_bearing;
        int c_x1, c_y1, c_x2, c_y2;
        Uint8 pixel;
        int kern;
        unsigned char* pixels;
        SDL_Renderer* renderer;

        x = 0;
        advance_width = left_side_bearing = 0;
        scale = font->scale;

	text->real_width = font->fontsize * num_codepoints;
	text->real_height = font->fontsize;

        bitmap = (unsigned char*)SDL_calloc(text->real_width * text->real_height + 1, sizeof(unsigned char));

        for (int i = 0; i < num_codepoints; ++i) {
                stbtt_GetCodepointHMetrics(&font->fontinfo, codepoints[i], &advance_width, &left_side_bearing);
                stbtt_GetCodepointBitmapBox(&font->fontinfo, codepoints[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
                y = font->ascent + c_y1;

                int byteOffset = x + roundf(left_side_bearing * scale) + (y * text->real_width);
                stbtt_MakeCodepointBitmap(&font->fontinfo, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, text->real_width, scale, scale, codepoints[i]);
                x += roundf(advance_width * scale);
                kern = stbtt_GetCodepointKernAdvance(&font->fontinfo, codepoints[i], codepoints[i + 1]);
                x += roundf(kern * scale);
        }

        int old_bitmap_width = text->real_width;
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

void text_destroy(text_texture_p text)
{
	if (!text) return;
	SDL_DestroyTexture(text->texture);
	SDL_free(text);
}

void text_print(SDL_Renderer* renderer,
	text_texture_p text,
	float x, float y)
{
	SDL_FRect dst_rect = { x, y, (float)text->real_width, (float)text->real_height };
	SDL_RenderTexture(renderer, text->texture, NULL, &dst_rect);
}

int text_get_width(text_texture_p text_tex)
{
	return text_tex->real_width;
}

int text_get_height(text_texture_p text_tex)
{
	return text_tex->real_height;
}

SDL_Texture* text_get_texture(text_texture_p text_tex)
{
	return text_tex ? text_tex->texture : NULL;
}
