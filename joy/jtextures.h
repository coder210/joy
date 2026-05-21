/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: textures.h
Description: Õº∆¨¥¶¿Ì
Author: ydlc
Version: 1.0
Date: 2024.7.15
History:
*************************************************/
#ifndef TEXTURES_H
#define TEXTURES_H
#include "SDL3/SDL.h"
#include "jconfig.h"

typedef struct image image_t, *image_p;
typedef struct sprite_batch sprite_batch_t, * sprite_batch_p;

#ifdef __cplusplus
extern "C" {
#endif

	JOY_API image_p image_create(SDL_Renderer* renderer, const char* filename);
	JOY_API void image_destroy(image_p image);
	JOY_API void image_draw(image_p image, SDL_FPoint position);
	JOY_API void image_draw1(image_p image,
		const SDL_FRect* srcrect, const SDL_FRect* destrect);
	JOY_API void image_draw_ex(image_p image, 
		const SDL_FRect* srcrect, SDL_FPoint position, 
		float rotation, SDL_FPoint scale, SDL_FPoint origin);
	JOY_API sprite_batch_p sprite_batch_create(SDL_Renderer* renderer,
		const char* filename);
	JOY_API void sprite_batch_destroy(sprite_batch_p batch);
	JOY_API void sprite_batch_add(sprite_batch_p batch,
		SDL_FPoint position, float rotation, 
		float scale_x, float scale_y, SDL_FPoint origin, SDL_FRect src_rect);
	JOY_API float sprite_batch_get_width(sprite_batch_p batch);
	JOY_API float sprite_batch_get_height(sprite_batch_p batch);
	JOY_API void sprite_batch_add_ex(sprite_batch_p batch, 
		SDL_FRect src_rect, SDL_FRect dest_rect, 
		float rotation, SDL_FPoint origin);
	JOY_API void sprite_batch_clear(sprite_batch_p batch);
	JOY_API void sprite_batch_set_image(sprite_batch_p batch, 
		const char* filename);
	JOY_API void sprite_batch_draw(sprite_batch_p batch);

#ifdef __cplusplus
}
#endif

#endif // !TEXTURES_H
