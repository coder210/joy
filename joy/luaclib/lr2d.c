#include "luaclib.h"
#include <SDL3/SDL.h>
#include "../external/lua/lauxlib.h"
#include "../jr2d.h"

/* ---- helper: get renderer from stack ---- */
static jr2d_p get_ren(lua_State* L, int idx) {
        return (jr2d_p)lua_touserdata(L, idx);
}

/* =================================================================
 * Renderer
 * ================================================================= */
static int lr_create(lua_State* L) {
        jwindow_p win = (jwindow_p)lua_touserdata(L, 1);
        if (!win) { lua_pushnil(L); return 1; }
        jr2d_p r2d = r2d_create(win);
        lua_pushlightuserdata(L, r2d);
        return 1;
}

static int l_select(lua_State* L) {
        return 0;
}

static int lr_destroy(lua_State* L) {
        jr2d_p r2d = (jr2d_p)lua_touserdata(L, 1);
        if (r2d) r2d_destroy(r2d);
        return 0;
}

static int lr_begin(lua_State* L) {
        jr2d_p r2d = get_ren(L, 1);
        r2d_set_color(r2d,
                (Uint8)luaL_optinteger(L, 2, 0),
                (Uint8)luaL_optinteger(L, 3, 0),
                (Uint8)luaL_optinteger(L, 4, 0),
                (Uint8)luaL_optinteger(L, 5, 255));
        r2d_begin(r2d);
        return 0;
}

static int lr_end_frame(lua_State* L) {
        r2d_end(get_ren(L, 1));
        return 0;
}

static int lr_get_size(lua_State* L) {
        jr2d_p r2d = get_ren(L, 1);
        int w, h;
        r2d_get_size(r2d, &w, &h);
        lua_pushinteger(L, w);
        lua_pushinteger(L, h);
        return 2;
}

static int lr_set_vsync(lua_State* L) {
        lua_pushboolean(L, r2d_set_vsync(get_ren(L, 1), (int)luaL_checkinteger(L, 2)));
        return 1;
}

static int lr_set_scale(lua_State* L) {
        lua_pushboolean(L, r2d_set_scale(get_ren(L, 1),
                (float)luaL_checknumber(L, 2),
                (float)luaL_checknumber(L, 3)));
        return 1;
}

static int lr_set_color(lua_State* L) {
        lua_pushboolean(L, r2d_set_color(get_ren(L, 1),
                (Uint8)luaL_checkinteger(L, 2),
                (Uint8)luaL_checkinteger(L, 3),
                (Uint8)luaL_checkinteger(L, 4),
                (Uint8)luaL_optinteger(L, 5, 255)));
        return 1;
}

static int lr_set_logical_presentation(lua_State* L) {
        r2d_set_logical_presentation(get_ren(L, 1),
                (int)luaL_checkinteger(L, 2),
                (int)luaL_checkinteger(L, 3));
        return 0;
}

static int lr_get_logical_presentation(lua_State* L) {
        jr2d_p r2d = get_ren(L, 1);
        int w, h;
        int mode;
        lua_pushboolean(L, r2d_get_logical_presentation(r2d, &w, &h, &mode));
        lua_pushinteger(L, w);
        lua_pushinteger(L, h);
        lua_pushinteger(L, (int)mode);
        return 4;
}

//static int lr_get_logical_presentation_rect(lua_State* L) {
//    jr2d_p r2d = get_ren(L, 1);
//    jrect_t rect;
//    lua_pushboolean(L, SDL_GetRenderLogicalPresentationRect(r2d, &rect));
//    lua_pushnumber(L, rect.x);
//    lua_pushnumber(L, rect.y);
//    lua_pushnumber(L, rect.w);
//    lua_pushnumber(L, rect.h);
//    return 5;
//}


/* ---- draw ---- */
static int lr_draw_point(lua_State* L) {
        lua_pushboolean(L, r2d_draw_point(get_ren(L, 1),
                (float)luaL_checknumber(L, 2),
                (float)luaL_checknumber(L, 3)));
        return 1;
}

static int lr_draw_points(lua_State* L) {
        jr2d_p r2d = get_ren(L, 1);
        int n = (int)luaL_len(L, 2);
        vec2_t* pts = (vec2_t*)SDL_malloc(n * sizeof(vec2_t));
        for (int i = 1; i <= n; i++) {
                lua_rawgeti(L, 2, i);
                lua_getfield(L, -1, "x"); pts[i - 1].x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
                lua_getfield(L, -1, "y"); pts[i - 1].y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
                lua_pop(L, 1);
        }
        lua_pushboolean(L, r2d_draw_points(r2d, pts, n));
        SDL_free(pts);
        return 1;
}

static int lr_draw_line(lua_State* L) {
        lua_pushboolean(L, shape_draw_line(get_ren(L, 1),
                (float)luaL_checknumber(L, 2),
                (float)luaL_checknumber(L, 3),
                (float)luaL_checknumber(L, 4),
                (float)luaL_checknumber(L, 5)));
        return 1;
}

static int lr_draw_rectangle(lua_State* L) {
        jrect_t rect;
        rect.x = (float)luaL_checknumber(L, 3);
        rect.y = (float)luaL_checknumber(L, 4);
        rect.w = (float)luaL_checknumber(L, 5);
        rect.h = (float)luaL_checknumber(L, 6);
        lua_pushboolean(L, shape_draw_rectangle(get_ren(L, 1), luaL_checkstring(L, 2), rect));
        return 1;
}

static int lr_draw_polygon(lua_State* L) {
        jr2d_p r2d = get_ren(L, 1);
        const char* mode = luaL_checkstring(L, 2);
        int n = (lua_gettop(L) - 2) / 2;
        if (n < 3) return luaL_error(L, "polygon needs >= 3 vertices");
        vec2_t* pts = (vec2_t*)SDL_malloc((n + 1) * sizeof(vec2_t));
        for (int i = 0; i < n; i++) {
                pts[i].x = (float)luaL_checknumber(L, 3 + i * 2);
                pts[i].y = (float)luaL_checknumber(L, 4 + i * 2);
        }
        jcolor_t c;
        r2d_get_color(r2d, &c.r, &c.g, &c.b, &c.a);
        lua_pushboolean(L, shape_draw_polygon(r2d, mode, pts, n, c));
        SDL_free(pts);
        return 1;
}

static int lr_draw_circle(lua_State* L) {
        jr2d_p r2d = get_ren(L, 1);
        vec2_t center = {
            (float)luaL_checknumber(L, 3),
            (float)luaL_checknumber(L, 4)
        };
        lua_pushboolean(L, shape_draw_circle(r2d,
                luaL_checkstring(L, 2), center,
                (float)luaL_checknumber(L, 5),
                (int)luaL_optinteger(L, 6, 32)));
        return 1;
}

static int lr_draw_grid(lua_State* L) {
        jr2d_p r2d = get_ren(L, 1);
        luaL_checktype(L, 2, LUA_TTABLE);
        luaL_checktype(L, 3, LUA_TTABLE);
        vec2_t start, end;
        lua_getfield(L, 2, "x"); start.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); start.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 3, "x"); end.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 3, "y"); end.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        shape_draw_grid(r2d, start, end, (float)luaL_checknumber(L, 4));
        return 0;
}

static int lr_draw_gridx(lua_State* L) {
        jr2d_p r2d = get_ren(L, 1);
        luaL_checktype(L, 2, LUA_TTABLE);
        vec2_t pos;
        lua_getfield(L, 2, "x"); pos.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); pos.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        shape_draw_gridx(r2d, pos,
                (int)luaL_checkinteger(L, 3),
                (int)luaL_checkinteger(L, 4),
                (float)luaL_checknumber(L, 5));
        return 0;
}

/* =================================================================
 * Image
 * ================================================================= */
static int li_new(lua_State* L) {
        image_p img = image_create(get_ren(L, 1), luaL_checkstring(L, 2));
        lua_pushlightuserdata(L, img);
        return 1;
}

static int li_free(lua_State* L) {
        image_p img = (image_p)lua_touserdata(L, 1);
        if (img) image_destroy(img);
        return 0;
}

static int li_draw(lua_State* L) {
        image_p img = (image_p)lua_touserdata(L, 1);
        if (img) image_draw(img, (vec2_t) {
                (float)luaL_checknumber(L, 2),
                        (float)luaL_checknumber(L, 3)
        });
        return 0;
}

static int li_draw_ex(lua_State* L) {
        image_p img = (image_p)lua_touserdata(L, 1);
        if (!img) return 0;
        luaL_checktype(L, 2, LUA_TTABLE);
        jrect_t src, dst;
        lua_getfield(L, 2, "x"); src.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "y"); src.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "w"); src.w = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, 2, "h"); src.h = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        image_draw_ex(img, &src,
                (vec2_t) {
                (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4)
        },
                (float)luaL_optnumber(L, 5, 0),
                (vec2_t) {
                (float)luaL_optnumber(L, 6, 1), (float)luaL_optnumber(L, 7, 1)
        },
                (vec2_t) {
                (float)luaL_optnumber(L, 8, 0), (float)luaL_optnumber(L, 9, 0)
        });
        return 0;
}

/* =================================================================
 * Font
 * ================================================================= */
static int lf_create(lua_State* L) {
        const char* font_path = luaL_checkstring(L, 1);
        font_p f = font_create(font_path, (int)luaL_checkinteger(L, 2));
        lua_pushlightuserdata(L, f);
        return 1;
}

static int lf_destroy(lua_State* L) {
        font_p f = (font_p)lua_touserdata(L, 1);
        if (f) font_destroy(f);
        return 0;
}


static int lt_create(lua_State* L) {
        jr2d_p r2d = (jr2d_p)lua_touserdata(L, 1);
        font_p f = (font_p)lua_touserdata(L, 2);
        if (!f) return luaL_error(L, "invalid font");
        int n = (int)luaL_len(L, 2);
        int* cps = (int*)SDL_malloc(n * sizeof(int));
        for (int i = 1; i <= n; i++) {
                lua_geti(L, 2, i); cps[i - 1] = (int)luaL_checkinteger(L, -1); lua_pop(L, 1);
        }
        jcolor_t color = {
            (Uint8)luaL_optinteger(L, 3, 255),
            (Uint8)luaL_optinteger(L, 4, 255),
            (Uint8)luaL_optinteger(L, 5, 255),
            (Uint8)luaL_optinteger(L, 6, 255)
        };
        jtext_p t = text_create(r2d, f, cps, n, color);
        SDL_free(cps);
        lua_pushlightuserdata(L, t);
        return 1;
}

static int lt_update(lua_State* L) {
        jtext_p t = (jtext_p)lua_touserdata(L, 1);
        font_p f = (font_p)lua_touserdata(L, 2);
        if (!t || !f) return luaL_error(L, "invalid text or font");
        int n = (int)luaL_len(L, 3);
        int* cps = (int*)SDL_malloc(n * sizeof(int));
        for (int i = 1; i <= n; i++) {
                lua_geti(L, 3, i);
                cps[i - 1] = (int)luaL_checkinteger(L, -1);
                lua_pop(L, 1);
        }
        jcolor_t color = {
            (Uint8)luaL_optinteger(L, 4, 255),
            (Uint8)luaL_optinteger(L, 5, 255),
            (Uint8)luaL_optinteger(L, 6, 255),
            (Uint8)luaL_optinteger(L, 7, 255)
        };
        text_update(t, f, cps, n, color);
        SDL_free(cps);
        return 0;
}

static int lt_destroy(lua_State* L) {
        jtext_p t = (jtext_p)lua_touserdata(L, 1);
        if (t) text_destroy(t);
        return 0;
}

static int lt_print(lua_State* L) {
        jr2d_p r2d = get_ren(L, 1);
        jtext_p t = (jtext_p)lua_touserdata(L, 2);
        if (t) jtext_print(r2d, t,
                (float)luaL_checknumber(L, 3),
                (float)luaL_checknumber(L, 4));
        return 0;
}

/* print(ren, font, text, x, y [, r, g, b, a]) */
static int l_print(lua_State* L) {
        font_p f = (font_p)lua_touserdata(L, 2);
        if (!f) return luaL_error(L, "invalid font");
        const char* str = luaL_checkstring(L, 3);
        jcolor_t c = {
            (Uint8)luaL_optinteger(L, 6, 255),
            (Uint8)luaL_optinteger(L, 7, 255),
            (Uint8)luaL_optinteger(L, 8, 255),
            (Uint8)luaL_optinteger(L, 9, 255)
        };
        jtext_printx(get_ren(L, 1), f, str, (int)SDL_strlen(str), c,
                (float)luaL_checknumber(L, 4),
                (float)luaL_checknumber(L, 5));
        return 0;
}

static int l_get_renderer(lua_State* L) {
        jr2d_p r2d = get_ren(L, 1);
        lua_pushlightuserdata(L, r2d);
        return 1;
}

/* =================================================================
 * Module registration
 * ================================================================= */
int luaopen_render2d(lua_State* L) {
        luaL_checkversion(L);
        luaL_Reg l[] = {
            {"create",    lr_create},
            {"select",    l_select},
            {"destroy",   lr_destroy},
            {"begin",     lr_begin},
            {"end_frame", lr_end_frame},
            {"get_size",  lr_get_size},
            {"set_vsync", lr_set_vsync},
            {"set_scale", lr_set_scale},
            {"set_color", lr_set_color},
            {"set_logical_presentation", lr_set_logical_presentation},
            {"get_logical_presentation", lr_get_logical_presentation},
            {"point",      lr_draw_point},
            {"points",     lr_draw_points},
            {"line",       lr_draw_line},
            {"rectangle",  lr_draw_rectangle},
            {"polygon",    lr_draw_polygon},
            {"circle",     lr_draw_circle},
            {"grid",       lr_draw_grid},
            {"gridx",      lr_draw_gridx},
            {"new_image",  li_new},
            {"free_image", li_free},
            {"draw_image", li_draw},
            {"draw_image_ex", li_draw_ex},
            {"create_font", lf_create},
            {"destroy_font", lf_destroy},
            {"create_text", lt_create},
            {"update_text", lt_update},
            {"destroy_text", lt_destroy},
            {"print_text",  lt_print},
            {"print",       l_print},
            {"get_renderer", l_get_renderer},
            {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
