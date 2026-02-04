#include "liv-profiler.h"


simple_fps_counter_p simple_fps_create()
{
	simple_fps_counter_p fps;
	fps = (simple_fps_counter_p)SDL_malloc(sizeof(simple_fps_counter_t));
	fps->fps = fps->frame_count = 0;
	fps->last_time = SDL_GetTicks();
	return fps;
}

void simple_fps_destory(simple_fps_counter_p fps)
{
	SDL_free(fps);
}

int simple_fps_update(simple_fps_counter_p fps)
{
	fps->frame_count++;
	fps->current_time = SDL_GetTicks();

	Uint64 elapsed = fps->current_time - fps->last_time;

	if (elapsed >= 1000) {
		fps->fps = fps->frame_count;
		fps->frame_count = 0;
		fps->last_time = fps->current_time;
	}

	return fps->fps;
}
