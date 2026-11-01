#include "external/stb_truetype.h"
#include <SDL3/SDL.h>
#include "joy.h"

struct jfont {
	stbtt_fontinfo fontinfo;
	unsigned char* fontdata;
	int fontsize;
	float scale;
	int ascent;
	int descent;
	int line_gap;
};

font_p font_create(const char* filename, int fontsize)
{
	size_t data_size;
	unsigned char* fontdata;
	font_p font;

	fontdata = SDL_LoadFile(filename, &data_size);
	if (!fontdata) {
		log_info("Failed to load font file: %s", filename);
		return NULL;
	}

	font = (font_p)SDL_calloc(1, sizeof(font_t));
	if (!font) {
		SDL_free(fontdata);
		return NULL;
	}

	if (!stbtt_InitFont(&font->fontinfo, fontdata, 0)) {
		log_info("Failed to initialize font");
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

void font_destroy(font_p font)
{
	if (!font) return;

	/* 文字缓存以 font 指针作为键的一部分。字体释放后该指针失效，
	 * 若新字体恰好分配到同一地址，缓存会命中用旧字体烘焙的纹理，
	 * 渲染出错误的字形。这里整体清空缓存以杜绝该隐患
	 * （字体销毁是低频操作，代价可接受）。 */
	jtext_cache_clear();

	if (font->fontdata) SDL_free(font->fontdata);
	SDL_free(font);
}

float font_get_scale(font_p font)
{
	return font->scale;
}

int font_get_size(font_p font)
{
	return font->fontsize;
}

int font_get_ascent(font_p font)
{
	return font->ascent;
}
