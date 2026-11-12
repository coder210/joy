#include "external/stb_truetype.h"
#include <SDL3/SDL.h>
#include "joy.h"

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
	joy_r2d_jtext_cache_clear_for_font(font);

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
