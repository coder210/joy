#include "jmath.h"
#include "jshapes.h"


bool shape_draw_line(SDL_Renderer* renderer, float x1, float y1, float x2, float y2)
{
	bool result;
	result = SDL_RenderLine(renderer, x1, y1, x2, y2);
	return result;
}

bool shape_draw_rectangle(SDL_Renderer* renderer, const char* mode, SDL_FRect rect)
{
	bool result;
	if (SDL_strcmp(mode, "line") == 0) {
		result = SDL_RenderRect(renderer, &rect);
	}
	else if (SDL_strcmp(mode, "fill") == 0) {
		result = SDL_RenderFillRect(renderer, &rect);
	}
	else {
		result = false;
	}
	return result;
}

static bool
draw_polygon_outline(SDL_Renderer* renderer, SDL_FPoint* points, int point_count)
{
	bool result;
	int vertex_count = point_count - 1;
	points[vertex_count].x = points[0].x;
	points[vertex_count].y = points[0].y;
	result = SDL_RenderLines(renderer, points, point_count);
	return result;
}

static bool
draw_polygon_filled(SDL_Renderer* renderer,
	SDL_FPoint* points, int point_count, SDL_FColor color)
{
	bool result;
	SDL_Vertex* vertices;
	int vertex_count;
	int* indices = NULL;
	int indices_count = 0;

	vertex_count = point_count - 1;
	vertices = SDL_malloc(sizeof(SDL_Vertex) * vertex_count);

	if (vertex_count >= 3) {
		indices_count = (vertex_count - 2) * 3;
		indices = SDL_malloc(sizeof(int) * indices_count);
		if (indices) {
			for (int i = 0; i < vertex_count - 2; i++) {
				indices[i * 3] = 0;
				indices[i * 3 + 1] = i + 1;
				indices[i * 3 + 2] = i + 2;
			}
		}
	}

	for (int i = 0; i < vertex_count; i++) {
		vertices[i].position.x = points[i].x;
		vertices[i].position.y = points[i].y;
		vertices[i].color = color;
		vertices[i].tex_coord.x = 0.0f;
		vertices[i].tex_coord.y = 0.0f;
	}

	if (indices) {
		result = SDL_RenderGeometry(renderer, NULL, vertices, vertex_count, indices, indices_count);
		SDL_free(indices);
	}
	else {
		result = false;
	}

	SDL_free(vertices);

	return result;
}

bool
shape_draw_polygon(SDL_Renderer* renderer, const char* mode,
	SDL_FPoint* points, int point_count, SDL_FColor color)
{
	bool result;
	if (SDL_strcmp(mode, "line") == 0) {
		result = draw_polygon_outline(renderer, points, point_count);
	}
	else if (SDL_strcmp(mode, "fill") == 0) {
		result = draw_polygon_filled(renderer, points, point_count, color);
	}
	else {
		result = false;
	}
	return result;
}

static bool
draw_circle_outline(SDL_Renderer* renderer, SDL_FPoint center, float radius, int segments)
{
	bool result;
	float angle;
	SDL_FPoint* points;
	points = SDL_malloc(sizeof(SDL_FPoint) * (segments + 1));
	for (int i = 0; i < segments; i++) {
		angle = (float)i / (float)segments * 2.0f * 3.1415926535f;
		points[i].x = center.x + cosf(angle) * radius;
		points[i].y = center.y + sinf(angle) * radius;
	}
	points[segments].x = points[0].x;
	points[segments].y = points[0].y;
	result = SDL_RenderLines(renderer, points, segments + 1);
	SDL_free(points);
	return result;
}

static bool
draw_circle_filled(SDL_Renderer* renderer, SDL_FPoint center, float radius, int segments, SDL_FColor color)
{
	bool result;
	SDL_Vertex* vertices;
	int vertex_count;
	int* indices;

	vertex_count = segments + 1;
	vertices = SDL_malloc(sizeof(SDL_Vertex) * vertex_count);
	indices = SDL_malloc(sizeof(int) * segments * 3);

	if (!vertices || !indices) {
		if (vertices) SDL_free(vertices);
		if (indices) SDL_free(indices);
		return false;
	}

	vertices[0].position.x = center.x;
	vertices[0].position.y = center.y;
	vertices[0].color = color;
	vertices[0].tex_coord.x = 0.0f;
	vertices[0].tex_coord.y = 0.0f;

	for (int i = 0; i < segments; i++) {
		float angle = (float)i / (float)segments * 2.0f * 3.1415926535f;
		vertices[i + 1].position.x = center.x + cosf(angle) * radius;
		vertices[i + 1].position.y = center.y + sinf(angle) * radius;
		vertices[i + 1].color = color;
		vertices[i + 1].tex_coord.x = 0.0f;
		vertices[i + 1].tex_coord.y = 0.0f;

		indices[i * 3] = 0;
		indices[i * 3 + 1] = i + 1;
		indices[i * 3 + 2] = (i + 1) % segments + 1;
	}

	result = SDL_RenderGeometry(renderer, NULL, vertices, vertex_count, indices, segments * 3);

	SDL_free(vertices);
	SDL_free(indices);

	return result;
}

bool
shape_draw_circle(SDL_Renderer* renderer, const char* mode, SDL_FPoint center, float radius, int segments)
{
	bool result;
	SDL_FColor color;
	if (segments < 8) segments = 8;
	if (segments > 360) segments = 360;
	if (SDL_strcmp(mode, "line") == 0) {
		result = draw_circle_outline(renderer, center, radius, segments);
	}
	else if (SDL_strcmp(mode, "fill") == 0) {
		SDL_GetRenderDrawColorFloat(renderer, &color.r, &color.g, &color.b, &color.a);
		result = draw_circle_filled(renderer, center, radius, segments, color);
	}
	else {
		result = false;
	}
	return result;
}

bool
shape_draw_grid(SDL_Renderer* renderer,
	SDL_FPoint start, SDL_FPoint end,
	float grid_size)
{
	float x1, x2, y1, y2;
	for (float x = start.x; x <= end.x; x += grid_size) {
		x1 = x;
		x2 = start.y;
		y1 = x;
		y2 = end.y;
		SDL_RenderLine(renderer, x1, x2, y1, y2);
	}
	for (float y = start.y; y <= end.y; y += grid_size) {
		x1 = start.x;
		x2 = y;
		y1 = end.x;
		y2 = y;
		SDL_RenderLine(renderer, x1, x2, y1, y2);
	}
	return true;
}

bool
shape_draw_gridx(SDL_Renderer* renderer,
	SDL_FPoint position,
	int rows, int cols, float grid_size)
{
	float x1, x2, y1, y2;
	for (int i = 0; i <= rows; i++) {
		x1 = position.x;
		y1 = i * grid_size + position.y;
		x2 = cols * grid_size + position.x;
		y2 = y1;
		SDL_RenderLine(renderer, x1, y1, x2, y2);
	}
	for (int i = 0; i <= cols; i++) {
		x1 = i * grid_size + position.x;
		y1 = position.y;
		x2 = x1;
		y2 = rows * grid_size + position.y;
		SDL_RenderLine(renderer, x1, y1, x2, y2);
	}
	return true;
}
