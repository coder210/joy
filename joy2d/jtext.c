#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"
#include "jutils.h"
#include "jtext.h"


struct font_t {
	stbtt_fontinfo fontinfo;
	unsigned char* fontdata;
	SDL_Renderer* renderer;
	int fontsize;
	float scale;
	int ascent;
	int descent;
	int line_gap;
};

struct text_texture {
	SDL_Texture *texture;
	int real_width;
	int real_height;
};


/**
 * �����������
 */
font_p font_create(SDL_Renderer* renderer, const char* filename, int fontsize)
{
	size_t data_size;
	unsigned char* fontdata;
	font_p font;

	// ���������ļ�
	fontdata = SDL_LoadFile(filename, &data_size);
	if (!fontdata) {
		SDL_Log("Failed to load font file: %s", filename);
		return NULL;
	}

	// ��������ṹ��
	font = (font_p)SDL_calloc(1, sizeof(font_t));
	if (!font) {
		SDL_free(fontdata);
		return NULL;
	}

	// ��ʼ��stb_truetype������Ϣ
	if (!stbtt_InitFont(&font->fontinfo, fontdata, 0)) {
		SDL_Log("Failed to initialize font");
		SDL_free(fontdata);
		SDL_free(font);
		return NULL;
	}

	// ��������
	font->fontdata = fontdata;
	font->renderer = renderer;
	font->fontsize = fontsize;

	/**
	 * ��ȡ��ֱ�����ϵĶ���
	 * ascent������ӻ��ߵ������ĸ߶ȣ�
	 * descent�����ߵ��ײ��ĸ߶ȣ�ͨ��Ϊ��ֵ��
	 * line_gap����������֮��ļ�ࣻ
	 * �м��Ϊ��ascent - descent + line_gap��
	*/
	stbtt_GetFontVMetrics(&font->fontinfo,
		&font->ascent,
		&font->descent,
		&font->line_gap);

	/* �������ŵ����ָ� */
	font->scale = stbtt_ScaleForPixelHeight(&font->fontinfo, fontsize);
	font->ascent = roundf(font->ascent * font->scale);
	font->descent = roundf(font->descent * font->scale);

	return font;
}

/**
 * �ͷ��������
 */
void font_destroy(font_p font)
{
	if (!font) return;

	if (font->fontdata) {
		SDL_free(font->fontdata);
	}
	SDL_free(font);
}

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

/**
 * �����ı�����
 */
text_texture_p text_create(font_p font, 
	const int* codepoints, int num_codepoints, 
	SDL_Color color)
{
	size_t sizedata;
	int pitch;
	int x, y;/* λͼ��x,����λͼ��y (��ͬ�ַ��ĸ߶Ȳ�ͬ�� */
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

	/* ����λͼ */
	text = (text_texture_p)SDL_malloc(sizeof(text_texture_t));
	text->real_width = font->fontsize * num_codepoints; /* λͼ�Ŀ� */
	text->real_height = font->fontsize; /* λͼ�ĸ� */
	bitmap = SDL_calloc(text->real_width * text->real_height + 1, sizeof(unsigned char));

	/* ѭ������codepoints��ÿ���ַ� */
	for (int i = 0; i < num_codepoints; ++i) {
		/**
		  * ��ȡˮƽ�����ϵĶ���
		  * advanceWidth���ֿ���
		  * leftSideBearing�����λ�ã�
		*/
		stbtt_GetCodepointHMetrics(&font->fontinfo, codepoints[i], &advance_width, &left_side_bearing);

		/* ��ȡ�ַ��ı߿򣨱߽磩 */
		stbtt_GetCodepointBitmapBox(&font->fontinfo, codepoints[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

		/* ����λͼ��y (��ͬ�ַ��ĸ߶Ȳ�ͬ�� */
		y = font->ascent + c_y1;

		/* ��Ⱦ�ַ� */
		int byte_offset = x + roundf(left_side_bearing * scale) + (y * text->real_width);
		stbtt_MakeCodepointBitmap(&font->fontinfo, bitmap + byte_offset, c_x2 - c_x1, c_y2 - c_y1, text->real_width, scale, scale, codepoints[i]);
		/* ����x */
		x += roundf(advance_width * scale);

		/* �����־� */
		kern = stbtt_GetCodepointKernAdvance(&font->fontinfo, codepoints[i], codepoints[i + 1]);
		x += roundf(kern * scale);
	}
	/* 用实际渲染宽度代替估算值 */
	int bitmap_width = text->real_width;
	text->real_width = x > 0 ? x : 1;
	surface = SDL_CreateSurface(text->real_width, text->real_height, SDL_PIXELFORMAT_RGBA32);
	if (!surface) {
		SDL_free(bitmap);
		return NULL;
	}

	surface_pixels = (Uint8*)surface->pixels;
	pitch = surface->pitch / sizeof(Uint8);
	/* �Ҷ�תRGBA����͸���ȣ� */
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
        int pitch;
        int x, y;/* λͼ��x,����λͼ��y (��ͬ�ַ��ĸ߶Ȳ�ͬ�� */
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

        /* ����λͼ */
	text->real_width = font->fontsize * num_codepoints; /* λͼ�Ŀ� */
	text->real_height = font->fontsize; /* λͼ�ĸ� */

        bitmap = (unsigned char*)SDL_calloc(text->real_width * text->real_height + 1, sizeof(unsigned char));

        /* ѭ������codepoints��ÿ���ַ� */
        for (int i = 0; i < num_codepoints; ++i) {
                /**
                  * ��ȡˮƽ�����ϵĶ���
                  * advanceWidth���ֿ���
                  * leftSideBearing�����λ�ã�
                */
                stbtt_GetCodepointHMetrics(&font->fontinfo, codepoints[i], &advance_width, &left_side_bearing);

                /* ��ȡ�ַ��ı߿򣨱߽磩 */
                stbtt_GetCodepointBitmapBox(&font->fontinfo, codepoints[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

                /* ����λͼ��y (��ͬ�ַ��ĸ߶Ȳ�ͬ�� */
                y = font->ascent + c_y1;

                /* ��Ⱦ�ַ� */
                int byteOffset = x + roundf(left_side_bearing * scale) + (y * text->real_width);
                stbtt_MakeCodepointBitmap(&font->fontinfo, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, text->real_width, scale, scale, codepoints[i]);

                /* ����x */
                x += roundf(advance_width * scale);

                /* �����־� */
                kern = stbtt_GetCodepointKernAdvance(&font->fontinfo, codepoints[i], codepoints[i + 1]);
                x += roundf(kern * scale);
        }

        /* 用实际渲染宽度代替估算值 */
        int old_bitmap_width = text->real_width;
        text->real_width = x > 0 ? x : 1;

        SDL_Surface* surface = SDL_CreateSurface(text->real_width, text->real_height, SDL_PIXELFORMAT_RGBA32);
        if (!surface) return;

        Uint8* surface_pixels = (Uint8*)surface->pixels;
        pitch = surface->pitch / sizeof(Uint8);

        /* �Ҷ�תRGBA����͸���ȣ� */
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
		/* TODO:����û�иı�text��ֵ */
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
 * �ͷ��ı�����
 */
void text_destroy(text_texture_p text)
{
	if (!text) return;
	SDL_DestroyTexture(text->texture);
	SDL_free(text);
}

/**
 * �����ı�����
 */
void text_print(SDL_Renderer* renderer,
	text_texture_p text,
	float x, float y)
{
	SDL_FRect dst_rect = { x, y, text->texture->w, text->texture->h };
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
