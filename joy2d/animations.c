#include "external/klib/kvec.h"
#include "textures.h"
#include "utils.h"
#include "animations.h"


typedef struct sprite_animation_clip {
	image_p image;
	float duration;
	SDL_FRect src_rect;
}sprite_animation_clip_t, * sprite_animation_clip_p;


struct sprite_animation {
	SDL_Renderer* renderer;
	kvec_t(sprite_animation_clip_t) clips;
	bool is_loop;
	int idx_frame;
	float total_time;
	float last_time;
	SDL_FPoint position;
	float rotation;
	SDL_FPoint scale;
	SDL_FPoint origin;
};




sprite_animation_p sprite_animation_create(SDL_Renderer* renderer)
{
	sprite_animation_p animation;
	animation = (sprite_animation_p)SDL_malloc(sizeof(sprite_animation_t));
	animation->renderer = renderer;
	animation->is_loop = true;
	kv_init(animation->clips);
	animation->position = (SDL_FPoint){ 0, 0 };
	animation->rotation = 0;
	animation->scale = (SDL_FPoint){ 1.0f, 1.0f };
	animation->origin = (SDL_FPoint){ 0.0f, 0.0f };
	animation->total_time = animation->last_time = animation->idx_frame = 0;
	return animation;
}

void sprite_animation_destroy(sprite_animation_p animation)
{
	kv_destroy(animation->clips);
	SDL_free(animation);
}

void sprite_animation_reset(sprite_animation_p animation)
{
	uint64_t ticks;
	animation->idx_frame = 0;
	animation->total_time = 0;
	if (utils_get_current_time(&ticks)) {
		animation->last_time = ticks;
	}
	else {
		animation->last_time = 0;
	}
}

void sprite_animation_add_clip(sprite_animation_p animation,
	const char* image_path, float duration,
	SDL_FRect src_rect)
{
	sprite_animation_clip_t sprite_animation_clip;
	sprite_animation_clip.image = image_create(animation->renderer, image_path);
	sprite_animation_clip.duration = duration;
	sprite_animation_clip.src_rect = src_rect;
	kv_push(sprite_animation_clip_t, animation->clips, sprite_animation_clip);
}

bool sprite_animation_is_finished(sprite_animation_p animation)
{
	if (animation->is_loop) {
		return false;
	}
	else {
		return (animation->idx_frame == kv_size(animation->clips));
	}
}

void sprite_animation_set_position(sprite_animation_p animation, float x, float y)
{
	animation->position.x = x;
	animation->position.y = y;
}

void sprite_animation_set_scale(sprite_animation_p animation, float x, float y)
{
	animation->scale.x = x;
	animation->scale.y = y;
}

void sprite_animation_set_rotation(sprite_animation_p animation, float rotation)
{
	animation->rotation = rotation;
}

void sprite_animation_update(sprite_animation_p animation, float dt)
{
	sprite_animation_clip_p sprite_animation_clip;
	if (kv_size(animation->clips) <= 0) {
		return;
	}

	sprite_animation_clip = &kv_A(animation->clips, animation->idx_frame);
	animation->total_time += dt;
	if (animation->total_time >= sprite_animation_clip->duration) {
		if (animation->idx_frame >= kv_size(animation->clips) - 1) {
			if (animation->is_loop)
				animation->idx_frame = 0;
			else
				animation->idx_frame = kv_size(animation->clips) - 1;
		}
		else {
			animation->idx_frame += 1;
		}
		animation->total_time -= sprite_animation_clip->duration;
	}
}

void sprite_animation_draw(sprite_animation_p animation, SDL_FRect* camera)
{
	sprite_animation_clip_p sprite_animation_clip;
	float r_x, r_y;
	SDL_FRect dest_rect;
	SDL_FPoint center = { 0 };

	if (kv_size(animation->clips) <= 0) {
		return;
	}

	sprite_animation_clip = &kv_A(animation->clips, animation->idx_frame);
	image_draw_ex(sprite_animation_clip->image, &sprite_animation_clip->src_rect, animation->position, animation->rotation, animation->scale, animation->origin);
}
