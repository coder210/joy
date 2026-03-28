/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: shapes.h
Description: ÐÎ×´
Author: ydlc
Version: 1.0
Date: 2024.7.15
History:
*************************************************/
#ifndef SHAPES_H
#define SHAPES_H
#include "SDL3/SDL.h"
#include "jconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

	JOY_API bool shape_draw_line(SDL_Renderer* renderer, 
		float x1, float y1, float x2, float y2);
	JOY_API bool shape_draw_rectangle(SDL_Renderer* renderer, 
		const char* mode, SDL_FRect rect);
	JOY_API bool shape_draw_polygon(SDL_Renderer* renderer,
		const char* mode, SDL_FPoint* points, int point_count, 
		SDL_FColor color);
	JOY_API bool shape_draw_circle(SDL_Renderer* renderer, 
		const char* mode, SDL_FPoint center, 
		float radius, int segments);
	JOY_API bool shape_draw_grid(SDL_Renderer* renderer, 
		SDL_FPoint start, SDL_FPoint end, float grid_size);
	JOY_API bool shape_draw_gridx(SDL_Renderer* renderer, 
		SDL_FPoint position, int rows, int cols, float grid_size);

#ifdef __cplusplus
}
#endif

#endif // !SHAPES_H
