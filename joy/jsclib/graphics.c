#include "jsclib.h"
#include "../jshapes.h"
#include "../jtextures.h"
#include "../jfont.h"
#include "../jtext.h"
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <string.h>

/* ---- tags for userdata ---- */
#define TAG_RENDERER "SDL_Renderer*"
#define TAG_IMAGE    "image_t*"
#define TAG_FONT     "font_t*"
#define TAG_TEXT     "text_texture_t*"
#define TAG_BATCH    "sprite_batch_t*"

/* ---- current renderer global ---- */
static SDL_Renderer* cur_ren = NULL;

/* ---- helpers ---- */
static SDL_Renderer* get_ren(js_State* J)
{
	if (!cur_ren) js_error(J, "graphics: no current renderer, call create() first");
	return cur_ren;
}

static void push_ud(js_State* J, const char* tag, void* p)
{
	js_newuserdata(J, tag, p, NULL);
}

static void reg_sub(js_State* J, const char* name, const js_CFunction* pairs)
{
	js_newobject(J);
	while (pairs[0]) {
		js_newcfunction(J, pairs[0], pairs[1] ? pairs[1] : "", 0);
		js_setproperty(J, -2, pairs[1]);
		pairs += 2;
	}
	js_setproperty(J, -2, name);
}

static SDL_Color read_color(js_State* J, int idx)
{
	SDL_Color c = { 255, 255, 255, 255 };
	if (js_isobject(J, idx)) {
		js_getproperty(J, idx, "r"); c.r = (Uint8)js_tonumber(J, -1); js_pop(J, 1);
		js_getproperty(J, idx, "g"); c.g = (Uint8)js_tonumber(J, -1); js_pop(J, 1);
		js_getproperty(J, idx, "b"); c.b = (Uint8)js_tonumber(J, -1); js_pop(J, 1);
		js_getproperty(J, idx, "a"); c.a = (Uint8)js_tonumber(J, -1); js_pop(J, 1);
	}
	return c;
}

/* ---- finalizers ---- */
static void ren_finalize(js_State* J, void* p)
{
	(void)J;
	if (p) {
		if (p == cur_ren) cur_ren = NULL;
		SDL_DestroyRenderer((SDL_Renderer*)p);
	}
}
static void image_finalize(js_State* J, void* p) { (void)J; if (p) image_destroy((image_p)p); }
static void font_finalize(js_State* J, void* p) { (void)J; if (p) font_destroy((font_p)p); }
static void text_finalize(js_State* J, void* p) { (void)J; if (p) text_destroy((text_texture_p)p); }
static void batch_finalize(js_State* J, void* p) { (void)J; if (p) sprite_batch_destroy((sprite_batch_p)p); }

/* =================================================================
 * Renderer
 * ================================================================= */
static void jg_create(js_State* J)
{
	SDL_Window* win;
	if (js_isuserdata(J, 1, "SDL_Window*")) {
		win = (SDL_Window*)js_touserdata(J, 1, "SDL_Window*");
	}
	else {
		js_error(J, "graphics.create() expects an SDL_Window* userdata");
		return;
	}
	SDL_Renderer* ren = SDL_CreateRenderer(win, NULL);
	if (!ren) {
		js_pushundefined(J);
		return;
	}
	cur_ren = ren;
	js_newuserdata(J, TAG_RENDERER, ren, ren_finalize);
}

static void jg_select(js_State* J)
{
	cur_ren = (SDL_Renderer*)js_touserdata(J, 1, TAG_RENDERER);
	js_pushundefined(J);
}

static void jg_destroy(js_State* J)
{
	SDL_Renderer* ren = (SDL_Renderer*)js_touserdata(J, 1, TAG_RENDERER);
	if (ren) {
		if (ren == cur_ren) cur_ren = NULL;
		SDL_DestroyRenderer(ren);
	}
	js_pushundefined(J);
}

static void jg_clear(js_State* J)
{
	SDL_Renderer* ren = get_ren(J);
	Uint8 r = (Uint8)js_tonumber(J, 1);
	Uint8 g = (Uint8)js_tonumber(J, 2);
	Uint8 b = (Uint8)js_tonumber(J, 3);
	Uint8 a = (Uint8)js_tonumber(J, 4);
	if (js_isundefined(J, 4)) a = 255;
	SDL_SetRenderDrawColor(ren, r, g, b, a);
	SDL_RenderClear(ren);
	js_pushundefined(J);
}

static void jg_present(js_State* J)
{
	SDL_RenderPresent(get_ren(J));
	js_pushundefined(J);
}

static void jg_get_size(js_State* J)
{
	int w, h;
	SDL_GetRenderOutputSize(get_ren(J), &w, &h);
	js_pushnumber(J, w);
	js_pushnumber(J, h);
}

static void jg_set_vsync(js_State* J)
{
	js_pushboolean(J, SDL_SetRenderVSync(get_ren(J), (int)js_tonumber(J, 1)));
}

static void jg_set_scale(js_State* J)
{
	js_pushboolean(J, SDL_SetRenderScale(get_ren(J),
		(float)js_tonumber(J, 1), (float)js_tonumber(J, 2)));
}

static void jg_set_colorf(js_State* J)
{
	SDL_Renderer* ren = get_ren(J);
	float r = (float)js_tonumber(J, 1);
	float g = (float)js_tonumber(J, 2);
	float b = (float)js_tonumber(J, 3);
	float a = (float)js_tonumber(J, 4);
	if (js_isundefined(J, 4)) a = 1.0f;
	js_pushboolean(J, SDL_SetRenderDrawColorFloat(ren, r, g, b, a));
}

static void jg_set_color(js_State* J)
{
	SDL_Renderer* ren = get_ren(J);
	Uint8 r = (Uint8)js_tonumber(J, 1);
	Uint8 g = (Uint8)js_tonumber(J, 2);
	Uint8 b = (Uint8)js_tonumber(J, 3);
	Uint8 a = (Uint8)js_tonumber(J, 4);
	if (js_isundefined(J, 4)) a = 255;
	js_pushboolean(J, SDL_SetRenderDrawColor(ren, r, g, b, a));
}

static void jg_set_logical_presentation(js_State* J)
{
	SDL_SetRenderLogicalPresentation(get_ren(J),
		(int)js_tonumber(J, 1),
		(int)js_tonumber(J, 2),
		(SDL_RendererLogicalPresentation)(int)js_tonumber(J, 3));
	js_pushundefined(J);
}

static void jg_get_logical_presentation(js_State* J)
{
	SDL_Renderer* ren = get_ren(J);
	int w, h;
	SDL_RendererLogicalPresentation mode;
	js_pushboolean(J, SDL_GetRenderLogicalPresentation(ren, &w, &h, &mode));
	js_pushnumber(J, w);
	js_pushnumber(J, h);
	js_pushnumber(J, (int)mode);
}

static void jg_get_logical_presentation_rect(js_State* J)
{
	SDL_Renderer* ren = get_ren(J);
	SDL_FRect rect;
	js_pushboolean(J, SDL_GetRenderLogicalPresentationRect(ren, &rect));
	js_pushnumber(J, rect.x);
	js_pushnumber(J, rect.y);
	js_pushnumber(J, rect.w);
	js_pushnumber(J, rect.h);
}

static void jg_debug_text(js_State* J)
{
	js_pushboolean(J, SDL_RenderDebugText(get_ren(J),
		(float)js_tonumber(J, 1),
		(float)js_tonumber(J, 2),
		js_tostring(J, 3)));
}

/* =================================================================
 * Drawing
 * ================================================================= */
static void jg_point(js_State* J)
{
	js_pushboolean(J, SDL_RenderPoint(get_ren(J),
		(float)js_tonumber(J, 1), (float)js_tonumber(J, 2)));
}

static void jg_points(js_State* J)
{
	SDL_Renderer* ren = get_ren(J);
	int n = js_getlength(J, 1);
	SDL_FPoint* pts = (SDL_FPoint*)SDL_malloc(n * sizeof(SDL_FPoint));
	for (int i = 0; i < n; i++) {
		js_getindex(J, 1, i);
		js_getproperty(J, -1, "x"); pts[i].x = (float)js_tonumber(J, -1); js_pop(J, 1);
		js_getproperty(J, -1, "y"); pts[i].y = (float)js_tonumber(J, -1); js_pop(J, 1);
		js_pop(J, 1);
	}
	js_pushboolean(J, SDL_RenderPoints(ren, pts, n));
	SDL_free(pts);
}

static void jg_line(js_State* J)
{
	js_pushboolean(J, shape_draw_line(get_ren(J),
		(float)js_tonumber(J, 1), (float)js_tonumber(J, 2),
		(float)js_tonumber(J, 3), (float)js_tonumber(J, 4)));
}

static void jg_rectangle(js_State* J)
{
	SDL_FRect rect;
	rect.x = (float)js_tonumber(J, 2);
	rect.y = (float)js_tonumber(J, 3);
	rect.w = (float)js_tonumber(J, 4);
	rect.h = (float)js_tonumber(J, 5);
	js_pushboolean(J, shape_draw_rectangle(get_ren(J), js_tostring(J, 1), rect));
}

static void jg_polygon(js_State* J)
{
	SDL_Renderer* ren = get_ren(J);
	const char* mode = js_tostring(J, 1);
	int top = js_gettop(J);
	int n = (top - 1) / 2;
	if (n < 3) { js_error(J, "polygon needs >= 3 vertices"); return; }
	SDL_FPoint* pts = (SDL_FPoint*)SDL_malloc((n + 1) * sizeof(SDL_FPoint));
	for (int i = 0; i < n; i++) {
		pts[i].x = (float)js_tonumber(J, 2 + i * 2);
		pts[i].y = (float)js_tonumber(J, 3 + i * 2);
	}
	SDL_FColor c;
	SDL_GetRenderDrawColorFloat(ren, &c.r, &c.g, &c.b, &c.a);
	js_pushboolean(J, shape_draw_polygon(ren, mode, pts, n, c));
	SDL_free(pts);
}

static void jg_circle(js_State* J)
{
	SDL_Renderer* ren = get_ren(J);
	SDL_FPoint center = {
		(float)js_tonumber(J, 2),
		(float)js_tonumber(J, 3)
	};
	int segs = 32;
	if (!js_isundefined(J, 5)) segs = (int)js_tonumber(J, 5);
	js_pushboolean(J, shape_draw_circle(ren, js_tostring(J, 1), center,
		(float)js_tonumber(J, 4), segs));
}

static void jg_grid(js_State* J)
{
	SDL_Renderer* ren = get_ren(J);
	js_getproperty(J, 1, "x"); float sx = (float)js_tonumber(J, -1); js_pop(J, 1);
	js_getproperty(J, 1, "y"); float sy = (float)js_tonumber(J, -1); js_pop(J, 1);
	js_getproperty(J, 2, "x"); float ex = (float)js_tonumber(J, -1); js_pop(J, 1);
	js_getproperty(J, 2, "y"); float ey = (float)js_tonumber(J, -1); js_pop(J, 1);
	SDL_FPoint start = { sx, sy }, end = { ex, ey };
	shape_draw_grid(ren, start, end, (float)js_tonumber(J, 3));
	js_pushundefined(J);
}

static void jg_gridx(js_State* J)
{
	SDL_Renderer* ren = get_ren(J);
	js_getproperty(J, 1, "x"); float px = (float)js_tonumber(J, -1); js_pop(J, 1);
	js_getproperty(J, 1, "y"); float py = (float)js_tonumber(J, -1); js_pop(J, 1);
	SDL_FPoint pos = { px, py };
	shape_draw_gridx(ren, pos, (int)js_tonumber(J, 2), (int)js_tonumber(J, 3), (float)js_tonumber(J, 4));
	js_pushundefined(J);
}

/* =================================================================
 * Image
 * ================================================================= */
static void jg_new_image(js_State* J)
{
	image_p img = image_create(get_ren(J), js_tostring(J, 1));
	if (img)
		js_newuserdata(J, TAG_IMAGE, img, image_finalize);
	else
		js_pushundefined(J);
}

static void jg_free_image(js_State* J)
{
	image_p img = (image_p)js_touserdata(J, 1, TAG_IMAGE);
	if (img) image_destroy(img);
	js_pushundefined(J);
}

static void jg_draw_image(js_State* J)
{
	image_p img = (image_p)js_touserdata(J, 1, TAG_IMAGE);
	image_draw(img, (SDL_FPoint){ (float)js_tonumber(J, 2), (float)js_tonumber(J, 3) });
	js_pushundefined(J);
}

static void jg_draw_image_ex(js_State* J)
{
	image_p img = (image_p)js_touserdata(J, 1, TAG_IMAGE);
	SDL_FRect src;
	js_getproperty(J, 2, "x"); src.x = (float)js_tonumber(J, -1); js_pop(J, 1);
	js_getproperty(J, 2, "y"); src.y = (float)js_tonumber(J, -1); js_pop(J, 1);
	js_getproperty(J, 2, "w"); src.w = (float)js_tonumber(J, -1); js_pop(J, 1);
	js_getproperty(J, 2, "h"); src.h = (float)js_tonumber(J, -1); js_pop(J, 1);
	SDL_FPoint pos = { (float)js_tonumber(J, 3), (float)js_tonumber(J, 4) };
	float rot = js_isundefined(J, 5) ? 0 : (float)js_tonumber(J, 5);
	SDL_FPoint scale = { 1, 1 };
	if (!js_isundefined(J, 6)) scale.x = (float)js_tonumber(J, 6);
	if (!js_isundefined(J, 7)) scale.y = (float)js_tonumber(J, 7);
	SDL_FPoint origin = { 0, 0 };
	if (!js_isundefined(J, 8)) origin.x = (float)js_tonumber(J, 8);
	if (!js_isundefined(J, 9)) origin.y = (float)js_tonumber(J, 9);
	image_draw_ex(img, &src, pos, rot, scale, origin);
	js_pushundefined(J);
}

/* =================================================================
 * Font
 * ================================================================= */
static void jg_create_font(js_State* J)
{
	font_p f = font_create(get_ren(J), js_tostring(J, 1), (int)js_tonumber(J, 2));
	if (f)
		js_newuserdata(J, TAG_FONT, f, font_finalize);
	else
		js_pushundefined(J);
}

static void jg_destroy_font(js_State* J)
{
	font_p f = (font_p)js_touserdata(J, 1, TAG_FONT);
	if (f) font_destroy(f);
	js_pushundefined(J);
}

/* =================================================================
 * Text
 * ================================================================= */
static void jg_create_text(js_State* J)
{
	font_p f = (font_p)js_touserdata(J, 1, TAG_FONT);
	int n = js_getlength(J, 2);
	int* cps = (int*)SDL_malloc(n * sizeof(int));
	for (int i = 0; i < n; i++) {
		js_getindex(J, 2, i);
		cps[i] = (int)js_tonumber(J, -1);
		js_pop(J, 1);
	}
	SDL_Color color = { 255, 255, 255, 255 };
	if (js_isobject(J, 3)) color = read_color(J, 3);
	text_texture_p t = text_create(f, cps, n, color);
	SDL_free(cps);
	if (t)
		js_newuserdata(J, TAG_TEXT, t, text_finalize);
	else
		js_pushundefined(J);
}

static void jg_update_text(js_State* J)
{
	text_texture_p t = (text_texture_p)js_touserdata(J, 1, TAG_TEXT);
	font_p f = (font_p)js_touserdata(J, 2, TAG_FONT);
	int n = js_getlength(J, 3);
	int* cps = (int*)SDL_malloc(n * sizeof(int));
	for (int i = 0; i < n; i++) {
		js_getindex(J, 3, i);
		cps[i] = (int)js_tonumber(J, -1);
		js_pop(J, 1);
	}
	SDL_Color color = { 255, 255, 255, 255 };
	if (js_isobject(J, 4)) color = read_color(J, 4);
	text_update(t, f, cps, n, color);
	SDL_free(cps);
	js_pushundefined(J);
}

static void jg_destroy_text(js_State* J)
{
	text_texture_p t = (text_texture_p)js_touserdata(J, 1, TAG_TEXT);
	if (t) text_destroy(t);
	js_pushundefined(J);
}

static void jg_print_text(js_State* J)
{
	text_texture_p t = (text_texture_p)js_touserdata(J, 1, TAG_TEXT);
	text_print(get_ren(J), t, (float)js_tonumber(J, 2), (float)js_tonumber(J, 3));
	js_pushundefined(J);
}

static void jg_print(js_State* J)
{
	font_p f = (font_p)js_touserdata(J, 1, TAG_FONT);
	const char* str = js_tostring(J, 2);
	float x = (float)js_tonumber(J, 3);
	float y = (float)js_tonumber(J, 4);
	SDL_Color c = { 255, 255, 255, 255 };
	if (js_isobject(J, 5)) c = read_color(J, 5);
	text_printx(get_ren(J), f, str, (int)SDL_strlen(str), c, x, y);
	js_pushundefined(J);
}

/* =================================================================
 * SpriteBatch
 * ================================================================= */
static void jg_new_spritebatch(js_State* J)
{
	sprite_batch_p b = sprite_batch_create(get_ren(J), js_tostring(J, 1));
	if (b)
		js_newuserdata(J, TAG_BATCH, b, batch_finalize);
	else
		js_pushundefined(J);
}

static void jg_free_spritebatch(js_State* J)
{
	sprite_batch_p b = (sprite_batch_p)js_touserdata(J, 1, TAG_BATCH);
	if (b) sprite_batch_destroy(b);
	js_pushundefined(J);
}

static void jg_sb_get_width(js_State* J)
{
	sprite_batch_p b = (sprite_batch_p)js_touserdata(J, 1, TAG_BATCH);
	js_pushnumber(J, sprite_batch_get_width(b));
}

static void jg_sb_get_height(js_State* J)
{
	sprite_batch_p b = (sprite_batch_p)js_touserdata(J, 1, TAG_BATCH);
	js_pushnumber(J, sprite_batch_get_height(b));
}

static void jg_add_sprite(js_State* J)
{
	sprite_batch_p b = (sprite_batch_p)js_touserdata(J, 1, TAG_BATCH);
	SDL_FRect src, dst;
	js_getproperty(J, 2, "x"); src.x = (float)js_tonumber(J, -1); js_pop(J, 1);
	js_getproperty(J, 2, "y"); src.y = (float)js_tonumber(J, -1); js_pop(J, 1);
	js_getproperty(J, 2, "w"); src.w = (float)js_tonumber(J, -1); js_pop(J, 1);
	js_getproperty(J, 2, "h"); src.h = (float)js_tonumber(J, -1); js_pop(J, 1);
	js_getproperty(J, 3, "x"); dst.x = (float)js_tonumber(J, -1); js_pop(J, 1);
	js_getproperty(J, 3, "y"); dst.y = (float)js_tonumber(J, -1); js_pop(J, 1);
	js_getproperty(J, 3, "w"); dst.w = (float)js_tonumber(J, -1); js_pop(J, 1);
	js_getproperty(J, 3, "h"); dst.h = (float)js_tonumber(J, -1); js_pop(J, 1);
	float rot = js_isundefined(J, 4) ? 0 : (float)js_tonumber(J, 4);
	SDL_FPoint origin = { 0, 0 };
	if (!js_isundefined(J, 5)) origin.x = (float)js_tonumber(J, 5);
	if (!js_isundefined(J, 6)) origin.y = (float)js_tonumber(J, 6);
	sprite_batch_add_ex(b, src, dst, rot, origin);
	js_pushundefined(J);
}

static void jg_draw_spritebatch(js_State* J)
{
	sprite_batch_p b = (sprite_batch_p)js_touserdata(J, 1, TAG_BATCH);
	sprite_batch_draw(b);
	js_pushundefined(J);
}

static void jg_set_sb_image(js_State* J)
{
	sprite_batch_p b = (sprite_batch_p)js_touserdata(J, 1, TAG_BATCH);
	sprite_batch_set_image(b, js_tostring(J, 2));
	js_pushundefined(J);
}

static void jg_get_renderer(js_State* J)
{
	SDL_Renderer* ren = get_ren(J);
	js_newuserdata(J, TAG_RENDERER, ren, NULL);
}

/* =================================================================
 * Module registration
 * ================================================================= */
void jsopen_graphics(js_State* J)
{
	js_newobject(J);

	static const js_CFunction renderer_fns[] = {
		(js_CFunction)jg_create, "create",
		(js_CFunction)jg_select, "select",
		(js_CFunction)jg_destroy, "destroy",
		(js_CFunction)jg_clear, "clear",
		(js_CFunction)jg_present, "present",
		(js_CFunction)jg_get_size, "get_size",
		(js_CFunction)jg_set_vsync, "set_vsync",
		(js_CFunction)jg_set_scale, "set_scale",
		(js_CFunction)jg_set_colorf, "set_colorf",
		(js_CFunction)jg_set_color, "set_color",
		(js_CFunction)jg_set_logical_presentation, "set_logical_presentation",
		(js_CFunction)jg_get_logical_presentation, "get_logical_presentation",
		(js_CFunction)jg_get_logical_presentation_rect, "get_logical_presentation_rect",
		(js_CFunction)jg_debug_text, "debug_text",
		NULL, NULL
	}; reg_sub(J, "renderer", renderer_fns);

	/* ---- drawing functions directly on graphics object ---- */
	js_newcfunction(J, jg_point, "point", 2); js_setproperty(J, -2, "point");
	js_newcfunction(J, jg_points, "points", 1); js_setproperty(J, -2, "points");
	js_newcfunction(J, jg_line, "line", 4); js_setproperty(J, -2, "line");
	js_newcfunction(J, jg_rectangle, "rectangle", 5); js_setproperty(J, -2, "rectangle");
	js_newcfunction(J, jg_polygon, "polygon", 0); js_setproperty(J, -2, "polygon");
	js_newcfunction(J, jg_circle, "circle", 5); js_setproperty(J, -2, "circle");
	js_newcfunction(J, jg_grid, "grid", 3); js_setproperty(J, -2, "grid");
	js_newcfunction(J, jg_gridx, "gridx", 4); js_setproperty(J, -2, "gridx");

	/* image */
	js_newcfunction(J, jg_new_image, "new_image", 1); js_setproperty(J, -2, "new_image");
	js_newcfunction(J, jg_free_image, "free_image", 1); js_setproperty(J, -2, "free_image");
	js_newcfunction(J, jg_draw_image, "draw_image", 3); js_setproperty(J, -2, "draw_image");
	js_newcfunction(J, jg_draw_image_ex, "draw_image_ex", 9); js_setproperty(J, -2, "draw_image_ex");

	/* font */
	js_newcfunction(J, jg_create_font, "create_font", 2); js_setproperty(J, -2, "create_font");
	js_newcfunction(J, jg_destroy_font, "destroy_font", 1); js_setproperty(J, -2, "destroy_font");

	/* text */
	js_newcfunction(J, jg_create_text, "create_text", 3); js_setproperty(J, -2, "create_text");
	js_newcfunction(J, jg_update_text, "update_text", 4); js_setproperty(J, -2, "update_text");
	js_newcfunction(J, jg_destroy_text, "destroy_text", 1); js_setproperty(J, -2, "destroy_text");
	js_newcfunction(J, jg_print_text, "print_text", 3); js_setproperty(J, -2, "print_text");
	js_newcfunction(J, jg_print, "print", 5); js_setproperty(J, -2, "print");

	/* sprite batch */
	js_newcfunction(J, jg_new_spritebatch, "new_spritebatch", 1); js_setproperty(J, -2, "new_spritebatch");
	js_newcfunction(J, jg_free_spritebatch, "free_spritebatch", 1); js_setproperty(J, -2, "free_spritebatch");
	js_newcfunction(J, jg_sb_get_width, "get_spritebatch_width", 1); js_setproperty(J, -2, "get_spritebatch_width");
	js_newcfunction(J, jg_sb_get_height, "get_spritebatch_height", 1); js_setproperty(J, -2, "get_spritebatch_height");
	js_newcfunction(J, jg_add_sprite, "add_sprite", 6); js_setproperty(J, -2, "add_sprite");
	js_newcfunction(J, jg_draw_spritebatch, "draw_spritebatch", 1); js_setproperty(J, -2, "draw_spritebatch");
	js_newcfunction(J, jg_set_sb_image, "set_spritebatch_image", 2); js_setproperty(J, -2, "set_spritebatch_image");

	/* get_renderer */
	js_newcfunction(J, jg_get_renderer, "get_renderer", 0); js_setproperty(J, -2, "get_renderer");

	js_setglobal(J, "graphics");
}
