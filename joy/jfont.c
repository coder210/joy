#define STB_TRUETYPE_IMPLEMENTATION
#include "jutils.h"
#include "jfont.h"

font_p font_create(SDL_Renderer* renderer, const char* filename, int fontsize)
{
	size_t data_size;
	unsigned char* fontdata;
	font_p font;

	fontdata = SDL_LoadFile(filename, &data_size);
	if (!fontdata) {
		SDL_Log("Failed to load font file: %s", filename);
		return NULL;
	}

	font = (font_p)SDL_calloc(1, sizeof(font_t));
	if (!font) {
		SDL_free(fontdata);
		return NULL;
	}

	if (!stbtt_InitFont(&font->fontinfo, fontdata, 0)) {
		SDL_Log("Failed to initialize font");
		SDL_free(fontdata);
		SDL_free(font);
		return NULL;
	}

	font->fontdata = fontdata;
	font->renderer = renderer;
	font->fontsize = fontsize;

	stbtt_GetFontVMetrics(&font->fontinfo, &font->ascent, &font->descent, &font->line_gap);
	font->scale = stbtt_ScaleForPixelHeight(&font->fontinfo, fontsize);
	font->ascent = roundf(font->ascent * font->scale);
	font->descent = roundf(font->descent * font->scale);

	return font;
}

void font_destroy(font_p font)
{
	if (!font) return;
	if (font->fontdata) SDL_free(font->fontdata);
	SDL_free(font);
}
