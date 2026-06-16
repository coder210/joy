#include "jsclib.h"
#include "../jr2d.h"
#include "../jfont.h"
#include <stdlib.h>
#include <string.h>

static jr2d_p check_ren(duk_context* ctx, int idx)
{
        void* p = duk_get_pointer(ctx, idx);
        if (!p)
                duk_error(ctx, DUK_ERR_ERROR, "graphics: invalid renderer");
        return (jr2d_p)p;
}

static jcolor_t read_color(duk_context* ctx, int idx)
{
        jcolor_t c = { 255,255,255,255 };
        if (duk_is_object(ctx, idx)) {
                duk_get_prop_string(ctx, idx, "r");
                c.r = (Uint8)duk_to_number(ctx, -1);
                duk_pop(ctx);
                duk_get_prop_string(ctx, idx, "g");
                c.g = (Uint8)duk_to_number(ctx, -1);
                duk_pop(ctx); duk_get_prop_string(ctx, idx, "b");
                c.b = (Uint8)duk_to_number(ctx, -1);
                duk_pop(ctx); duk_get_prop_string(ctx, idx, "a");
                c.a = (Uint8)duk_to_number(ctx, -1); duk_pop(ctx);
        }
        return c;
}

static duk_ret_t jg_create(duk_context* ctx)
{
        jwindow_p win = (jwindow_p)duk_get_pointer(ctx, 0);
        if (!win) {
                duk_error(ctx, DUK_ERR_ERROR, "graphics.create() expects a window pointer");
                return 0;
        }
        jr2d_p r2d = r2d_create(win);
        if (!r2d) {
                duk_push_undefined(ctx); return 1;
        }
        duk_push_pointer(ctx, r2d);
        return 1;
}

static duk_ret_t jg_select(duk_context* ctx)
{
        check_ren(ctx, 0);
        duk_push_undefined(ctx);
        return 1;
}

static duk_ret_t jg_destroy(duk_context* ctx)
{
        jr2d_p r2d = (jr2d_p)duk_get_pointer(ctx, 0);
        if (r2d)
                r2d_destroy(r2d);
        return 0;
}

#define JG_REN() check_ren(ctx, 0)



static duk_ret_t jg_begin(duk_context* ctx) 
{ 
        jr2d_p r2d = JG_REN(); 
        r2d_set_color(r2d, (Uint8)duk_to_number(ctx, 1), (Uint8)duk_to_number(ctx, 2), (Uint8)duk_to_number(ctx, 3), (Uint8)(duk_is_undefined(ctx, 4) ? 255 : duk_to_number(ctx, 4)));
        r2d_begin(r2d);
        return 0;
}

static duk_ret_t jg_end_frame(duk_context* ctx) 
{ 
        r2d_end(JG_REN());
        return 0;
}

/* get_size: 返回 {w:n, h:n} */
static duk_ret_t jg_get_size(duk_context* ctx) 
{ 
        int w, h; 
        r2d_get_size(JG_REN(), &w, &h);
        duk_push_object(ctx); 
        duk_push_number(ctx, (duk_double_t)w);
        duk_put_prop_string(ctx, -2, "w");
        duk_push_number(ctx, (duk_double_t)h);
        duk_put_prop_string(ctx, -2, "h"); 
        return 1;
}

static duk_ret_t jg_set_vsync(duk_context* ctx) 
{ 
        duk_push_boolean(ctx, r2d_set_vsync(JG_REN(), (int)duk_to_number(ctx, 1)));
        return 1;
}

static duk_ret_t jg_set_scale(duk_context* ctx)
{ 
        duk_push_boolean(ctx, r2d_set_scale(JG_REN(), (float)duk_to_number(ctx, 1), (float)duk_to_number(ctx, 2))); return 1;
}

static duk_ret_t jg_set_colorf(duk_context* ctx) { jr2d_p r2d = JG_REN(); r2d_set_colorf(r2d, (float)duk_to_number(ctx, 1), (float)duk_to_number(ctx, 2), (float)duk_to_number(ctx, 3), (float)(duk_is_undefined(ctx, 4) ? 1.0f : duk_to_number(ctx, 4))); duk_push_boolean(ctx, true); return 1; }
static duk_ret_t jg_set_color(duk_context* ctx) { jr2d_p r2d = JG_REN(); r2d_set_color(r2d, (Uint8)duk_to_number(ctx, 1), (Uint8)duk_to_number(ctx, 2), (Uint8)duk_to_number(ctx, 3), (Uint8)(duk_is_undefined(ctx, 4) ? 255 : duk_to_number(ctx, 4))); duk_push_boolean(ctx, true); return 1; }
static duk_ret_t jg_point(duk_context* ctx) { duk_push_boolean(ctx, r2d_draw_point(JG_REN(), (float)duk_to_number(ctx, 1), (float)duk_to_number(ctx, 2))); return 1; }
static duk_ret_t jg_line(duk_context* ctx) { duk_push_boolean(ctx, shape_draw_line(JG_REN(), (float)duk_to_number(ctx, 1), (float)duk_to_number(ctx, 2), (float)duk_to_number(ctx, 3), (float)duk_to_number(ctx, 4))); return 1; }
static duk_ret_t jg_rectangle(duk_context* ctx) { jrect_t r; r.x = (float)duk_to_number(ctx, 2); r.y = (float)duk_to_number(ctx, 3); r.w = (float)duk_to_number(ctx, 4); r.h = (float)duk_to_number(ctx, 5); duk_push_boolean(ctx, shape_draw_rectangle(JG_REN(), duk_to_string(ctx, 1), r)); return 1; }
static duk_ret_t jg_circle(duk_context* ctx) { jr2d_p r2d = JG_REN(); vec2_t c = { (float)duk_to_number(ctx,2),(float)duk_to_number(ctx,3) }; duk_push_boolean(ctx, shape_draw_circle(r2d, duk_to_string(ctx, 1), c, (float)duk_to_number(ctx, 4), (int)(duk_is_undefined(ctx, 5) ? 32 : duk_to_number(ctx, 5)))); return 1; }
static duk_ret_t jg_get_renderer(duk_context* ctx) { duk_push_pointer(ctx, JG_REN()); return 1; }

static duk_ret_t jg_create_font(duk_context* ctx)
{ 
        duk_push_pointer(ctx, font_create(duk_to_string(ctx, 1), (int)duk_to_number(ctx, 2)));
        return 1;
}

static duk_ret_t jg_print(duk_context* ctx) { font_p f = (font_p)duk_get_pointer(ctx, 1); const char* str = duk_to_string(ctx, 2); jcolor_t c = { 255,255,255,255 }; if (duk_is_object(ctx, 5)) c = read_color(ctx, 5); jtext_printx(JG_REN(), f, str, (int)SDL_strlen(str), c, (float)duk_to_number(ctx, 3), (float)duk_to_number(ctx, 4)); return 0; }

/* polygon: (ren, mode, x1,y1, x2,y2, ...) */
static duk_ret_t jg_polygon(duk_context* ctx) {
        jr2d_p r2d = JG_REN();
        const char* mode = duk_to_string(ctx, 1);
        int n = (duk_get_top(ctx) - 2) / 2;
        if (n < 3) { duk_error(ctx, DUK_ERR_ERROR, "polygon needs >= 3 vertices"); return 0; }
        vec2_t* pts = (vec2_t*)SDL_malloc((n + 1) * sizeof(vec2_t));
        for (int i = 0; i < n; i++) {
                pts[i].x = (float)duk_to_number(ctx, 2 + i * 2);
                pts[i].y = (float)duk_to_number(ctx, 3 + i * 2);
        }
        jcolor_t c;
        r2d_get_color(r2d, &c.r, &c.g, &c.b, &c.a);
        duk_push_boolean(ctx, shape_draw_polygon(r2d, mode, pts, n, c));
        SDL_free(pts);
        return 1;
}

void jsopen_render2d(duk_context* ctx)
{
        duk_push_object(ctx);
        duk_push_c_function(ctx, jg_create, 1); duk_put_prop_string(ctx, -2, "create");
        duk_push_c_function(ctx, jg_select, 1); duk_put_prop_string(ctx, -2, "select");
        duk_push_c_function(ctx, jg_destroy, 1); duk_put_prop_string(ctx, -2, "destroy");
        duk_push_c_function(ctx, jg_begin, 5); duk_put_prop_string(ctx, -2, "begin");
        duk_push_c_function(ctx, jg_end_frame, 1); duk_put_prop_string(ctx, -2, "end_frame");
        duk_push_c_function(ctx, jg_get_size, 1); duk_put_prop_string(ctx, -2, "get_size");
        duk_push_c_function(ctx, jg_set_vsync, 2); duk_put_prop_string(ctx, -2, "set_vsync");
        duk_push_c_function(ctx, jg_set_scale, 3); duk_put_prop_string(ctx, -2, "set_scale");
        duk_push_c_function(ctx, jg_set_colorf, 5); duk_put_prop_string(ctx, -2, "set_colorf");
        duk_push_c_function(ctx, jg_set_color, 5); duk_put_prop_string(ctx, -2, "set_color");
        duk_push_c_function(ctx, jg_point, 3); duk_put_prop_string(ctx, -2, "point");
        duk_push_c_function(ctx, jg_line, 5); duk_put_prop_string(ctx, -2, "line");
        duk_push_c_function(ctx, jg_rectangle, 6); duk_put_prop_string(ctx, -2, "rectangle");
        duk_push_c_function(ctx, jg_circle, 6); duk_put_prop_string(ctx, -2, "circle");
        duk_push_c_function(ctx, jg_polygon, DUK_VARARGS); duk_put_prop_string(ctx, -2, "polygon");
        duk_push_c_function(ctx, jg_create_font, 3); duk_put_prop_string(ctx, -2, "create_font");
        duk_push_c_function(ctx, jg_print, 6); duk_put_prop_string(ctx, -2, "print");
        duk_push_c_function(ctx, jg_get_renderer, 1); duk_put_prop_string(ctx, -2, "get_renderer");
        duk_put_global_string(ctx, "r2d");
}
