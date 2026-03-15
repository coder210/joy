/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: animations.h
Description: ¶¯»­
Author: ydlc
Version: 1.0
Date: 2024.7.15
History:
*************************************************/
#ifndef ANIMATIONS_H
#define ANIMATIONS_H
#include "SDL3/SDL.h"
#include "config.h"

typedef struct sprite_animation sprite_animation_t, * sprite_animation_p;

#ifdef __cplusplus
extern "C" {
#endif

	JOY_API sprite_animation_p sprite_animation_create(SDL_Renderer* renderer);
	JOY_API void sprite_animation_destroy(sprite_animation_p sprite_animation);
	JOY_API void sprite_animation_reset(sprite_animation_p sprite_animation);
	JOY_API void sprite_animation_add_clip(sprite_animation_p sprite_animation,
		const char* image_path, float duration, SDL_FRect src_rect);
	JOY_API bool sprite_animation_is_finished(sprite_animation_p sprite_animation);
	JOY_API void sprite_animation_set_position(sprite_animation_p sprite_animation, float x, float y);
	JOY_API void sprite_animation_set_scale(sprite_animation_p sprite_animation, float x, float y);
	JOY_API void sprite_animation_set_rotation(sprite_animation_p sprite_animation, float rotation);
	JOY_API void sprite_animation_update(sprite_animation_p sprite_animation, float dt);
	JOY_API void sprite_animation_draw(sprite_animation_p sprite_animation, SDL_FRect* camera);

#ifdef __cplusplus
}
#endif

#endif // !ANIMATIONS_H
