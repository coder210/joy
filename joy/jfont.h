#ifndef JFONT_H
#define JFONT_H
#include "SDL3/SDL.h"
#include "jconfig.h"
#include "external/stb_truetype.h"

typedef struct font_t {
	stbtt_fontinfo fontinfo;
	unsigned char* fontdata;
	SDL_Renderer* renderer;
	int fontsize;
	float scale;
	int ascent;
	int descent;
	int line_gap;
} font_t, *font_p;

#ifdef __cplusplus
extern "C" {
#endif
	JOY_API font_p font_create(SDL_Renderer* renderer, const char* filename, int fontsize);
	JOY_API void font_destroy(font_p font);
#ifdef __cplusplus
}
#endif

#endif // !JFONT_H
