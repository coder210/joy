/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: text.h
Description: 字体处理
Author: ydlc
Version: 1.0
Date: 2024.7.15
History:
*************************************************/
#ifndef TEXT_H
#define TEXT_H
#include "SDL3/SDL.h"
#include "jconfig.h"

typedef struct font_t  font_t, *font_p;
typedef struct text_texture text_texture_t, *text_texture_p;

#ifdef __cplusplus
extern "C" {
#endif
	JOY_API font_p font_create(SDL_Renderer* renderer, const char* filename, int fontsize);
	JOY_API void font_destroy(font_p font);
	JOY_API text_texture_p text_createx(font_p font, const char* str, int len, SDL_Color color);
	JOY_API text_texture_p text_create(font_p font, const int* codepoints, int num_codepoints, SDL_Color color);
	JOY_API void text_updatex(text_texture_p text, font_p font,const char* str, int len, SDL_Color color);
	JOY_API void text_update(text_texture_p text, font_p font, 
		const int* codepoints, int num_codepoints, SDL_Color color);
	JOY_API void text_destroy(text_texture_p text_tex);
	JOY_API void text_print(SDL_Renderer* renderer, text_texture_p text_tex, float x, float y);
	JOY_API int text_get_width(text_texture_p text_tex);
	JOY_API int text_get_height(text_texture_p text_tex);
	JOY_API SDL_Texture* text_get_texture(text_texture_p text_tex);
#ifdef __cplusplus
}
#endif

#endif // !TEXT_H
