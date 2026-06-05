#include "jsclib.h"
#include "../jshapes.h"
#include "../jtextures.h"
#include "../jfont.h"
#include "../jtext.h"
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <string.h>

#define TAG_RENDERER "SDL_Renderer*"
#define TAG_IMAGE    "image_t*"
#define TAG_FONT     "font_t*"
#define TAG_TEXT     "text_texture_t*"
#define TAG_BATCH    "sprite_batch_t*"

static SDL_Renderer* check_ren(duk_context* ctx, int idx)
{
	void* p = duk_get_pointer(ctx, idx);
	if (!p) duk_error(ctx, DUK_ERR_ERROR, "graphics: invalid renderer");
	return (SDL_Renderer*)p;
}

static SDL_Color read_color(duk_context* ctx, int idx)
{
	SDL_Color c = { 255, 255, 255, 255 };
	if (duk_is_object(ctx, idx)) {
		duk_get_prop_string(ctx, idx, "r"); c.r = (Uint8)duk_to_number(ctx, -1); duk_pop(ctx);
		duk_get_prop_string(ctx, idx, "g"); c.g = (Uint8)duk_to_number(ctx, -1); duk_pop(ctx);
		duk_get_prop_string(ctx, idx, "b"); c.b = (Uint8)duk_to_number(ctx, -1); duk_pop(ctx);
		duk_get_prop_string(ctx, idx, "a"); c.a = (Uint8)duk_to_number(ctx, -1); duk_pop(ctx);
	}
	return c;
}

/* ---- renderer ---- */
static duk_ret_t jg_create(duk_context* ctx)
{
	SDL_Window* win = (SDL_Window*)duk_get_pointer(ctx, 1);
	if (!win) { duk_error(ctx, DUK_ERR_ERROR, "graphics.create() expects a window pointer"); return 0; }
	SDL_Renderer* ren = SDL_CreateRenderer(win, NULL);
	if (!ren) { duk_push_undefined(ctx); return 1; }
	duk_push_pointer(ctx, ren);
	return 1;
}

static duk_ret_t jg_select(duk_context* ctx)
{ check_ren(ctx, 1); duk_push_undefined(ctx); return 1; }

static duk_ret_t jg_destroy(duk_context* ctx)
{
	SDL_Renderer* ren = (SDL_Renderer*)duk_get_pointer(ctx, 1);
	if (ren) SDL_DestroyRenderer(ren);
	return 0;
}

#define JG_REN() check_ren(ctx, 1)

static duk_ret_t jg_clear(duk_context* ctx) {
	SDL_Renderer* ren = JG_REN();
	SDL_SetRenderDrawColor(ren, (Uint8)duk_to_number(ctx,2), (Uint8)duk_to_number(ctx,3), (Uint8)duk_to_number(ctx,4), (Uint8)(duk_is_undefined(ctx,5)?255:duk_to_number(ctx,5)));
	SDL_RenderClear(ren);
	return 0;
}

static duk_ret_t jg_present(duk_context* ctx) { SDL_RenderPresent(JG_REN()); return 0; }

static duk_ret_t jg_get_size(duk_context* ctx) { int w,h; SDL_GetRenderOutputSize(JG_REN(),&w,&h); duk_push_number(ctx,(duk_double_t)w); duk_push_number(ctx,(duk_double_t)h); return 2; }

static duk_ret_t jg_set_vsync(duk_context* ctx) { duk_push_boolean(ctx,SDL_SetRenderVSync(JG_REN(),(int)duk_to_number(ctx,2))); return 1; }

static duk_ret_t jg_set_scale(duk_context* ctx) { duk_push_boolean(ctx,SDL_SetRenderScale(JG_REN(),(float)duk_to_number(ctx,2),(float)duk_to_number(ctx,3))); return 1; }

static duk_ret_t jg_set_colorf(duk_context* ctx) {
	SDL_Renderer* ren = JG_REN();
	duk_push_boolean(ctx,SDL_SetRenderDrawColorFloat(ren,(float)duk_to_number(ctx,2),(float)duk_to_number(ctx,3),(float)duk_to_number(ctx,4),(float)(duk_is_undefined(ctx,5)?1.0f:duk_to_number(ctx,5))));
	return 1;
}

static duk_ret_t jg_set_color(duk_context* ctx) {
	SDL_Renderer* ren = JG_REN();
	duk_push_boolean(ctx,SDL_SetRenderDrawColor(ren,(Uint8)duk_to_number(ctx,2),(Uint8)duk_to_number(ctx,3),(Uint8)duk_to_number(ctx,4),(Uint8)(duk_is_undefined(ctx,5)?255:duk_to_number(ctx,5))));
	return 1;
}

static duk_ret_t jg_point(duk_context* ctx) { duk_push_boolean(ctx,SDL_RenderPoint(JG_REN(),(float)duk_to_number(ctx,2),(float)duk_to_number(ctx,3))); return 1; }

static duk_ret_t jg_line(duk_context* ctx) { duk_push_boolean(ctx,shape_draw_line(JG_REN(),(float)duk_to_number(ctx,2),(float)duk_to_number(ctx,3),(float)duk_to_number(ctx,4),(float)duk_to_number(ctx,5))); return 1; }

static duk_ret_t jg_rectangle(duk_context* ctx) {
	SDL_FRect r; r.x=(float)duk_to_number(ctx,3); r.y=(float)duk_to_number(ctx,4); r.w=(float)duk_to_number(ctx,5); r.h=(float)duk_to_number(ctx,6);
	duk_push_boolean(ctx,shape_draw_rectangle(JG_REN(),duk_to_string(ctx,2),r)); return 1;
}

static duk_ret_t jg_circle(duk_context* ctx) {
	SDL_Renderer* ren = JG_REN();
	SDL_FPoint c = { (float)duk_to_number(ctx,3), (float)duk_to_number(ctx,4) };
	duk_push_boolean(ctx,shape_draw_circle(ren,duk_to_string(ctx,2),c,(float)duk_to_number(ctx,5),(int)(duk_is_undefined(ctx,6)?32:duk_to_number(ctx,6))));
	return 1;
}

static duk_ret_t jg_get_renderer(duk_context* ctx) { duk_push_pointer(ctx, JG_REN()); return 1; }

static duk_ret_t jg_create_font(duk_context* ctx) {
	font_p f = font_create(JG_REN(), duk_to_string(ctx,2), (int)duk_to_number(ctx,3));
	duk_push_pointer(ctx, f);
	return 1;
}

static duk_ret_t jg_print(duk_context* ctx) {
	font_p f = (font_p)duk_get_pointer(ctx, 2);
	const char* str = duk_to_string(ctx, 3);
	SDL_Color c = { 255,255,255,255 };
	if (duk_is_object(ctx, 6)) c = read_color(ctx, 6);
	text_printx(JG_REN(), f, str, (int)SDL_strlen(str), c, (float)duk_to_number(ctx,4), (float)duk_to_number(ctx,5));
	return 0;
}

void jsopen_graphics(duk_context* ctx)
{
	duk_push_object(ctx);
	duk_push_c_function(ctx, jg_create, 1); duk_put_prop_string(ctx, -2, "create");
	duk_push_c_function(ctx, jg_select, 1); duk_put_prop_string(ctx, -2, "select");
	duk_push_c_function(ctx, jg_destroy, 1); duk_put_prop_string(ctx, -2, "destroy");
	duk_push_c_function(ctx, jg_clear, 5); duk_put_prop_string(ctx, -2, "clear");
	duk_push_c_function(ctx, jg_present, 1); duk_put_prop_string(ctx, -2, "present");
	duk_push_c_function(ctx, jg_get_size, 1); duk_put_prop_string(ctx, -2, "get_size");
	duk_push_c_function(ctx, jg_set_vsync, 2); duk_put_prop_string(ctx, -2, "set_vsync");
	duk_push_c_function(ctx, jg_set_scale, 3); duk_put_prop_string(ctx, -2, "set_scale");
	duk_push_c_function(ctx, jg_set_colorf, 5); duk_put_prop_string(ctx, -2, "set_colorf");
	duk_push_c_function(ctx, jg_set_color, 5); duk_put_prop_string(ctx, -2, "set_color");
	duk_push_c_function(ctx, jg_point, 3); duk_put_prop_string(ctx, -2, "point");
	duk_push_c_function(ctx, jg_line, 5); duk_put_prop_string(ctx, -2, "line");
	duk_push_c_function(ctx, jg_rectangle, 6); duk_put_prop_string(ctx, -2, "rectangle");
	duk_push_c_function(ctx, jg_circle, 6); duk_put_prop_string(ctx, -2, "circle");
	duk_push_c_function(ctx, jg_create_font, 3); duk_put_prop_string(ctx, -2, "create_font");
	duk_push_c_function(ctx, jg_print, 6); duk_put_prop_string(ctx, -2, "print");
	duk_push_c_function(ctx, jg_get_renderer, 1); duk_put_prop_string(ctx, -2, "get_renderer");
	duk_put_global_string(ctx, "graphics");
}