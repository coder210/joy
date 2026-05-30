#include "luaclib.h"
#include <SDL3/SDL.h>
#include "../external/lua/lauxlib.h"
#include "../jtextures.h"
#include "../jtext.h"
#include "../jshapes.h"

#define RENDERER_MT  "SDL_Renderer*"
#define IMAGE_MT     "image_t*"
#define FONT_MT      "font_t*"
#define TEXT_MT      "text_texture_t*"
#define BATCH_MT     "sprite_batch_t*"
#define CUR_REN_KEY  "joy.graphics.cur_ren"

/* ---- helper: get current renderer from registry ---- */
static SDL_Renderer* cur_ren(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, CUR_REN_KEY);
    SDL_Renderer** ren = (SDL_Renderer**)lua_touserdata(L, -1);
    if (!ren || !*ren) {
        lua_pop(L, 1);
        luaL_error(L, "no current renderer, call graphics.create() first");
        return NULL;
    }
    lua_pop(L, 1);
    return *ren;
}

/* =================================================================
 * Renderer
 * ================================================================= */
static int lr_gc(lua_State* L) {
    SDL_Renderer** ren = (SDL_Renderer**)lua_touserdata(L, 1);
    if (ren && *ren) { SDL_DestroyRenderer(*ren); *ren = NULL; }
    return 0;
}

static int lr_create(lua_State* L) {
    SDL_Window* win;
    if (lua_isuserdata(L, 1)) {
        SDL_Window** wp = (SDL_Window**)luaL_checkudata(L, 1, "SDL_Window*");
        win = wp ? *wp : NULL;
    } else {
        // 无参数时从 registry 取当前窗口
        lua_getfield(L, LUA_REGISTRYINDEX, "joy.window.cur_win");
        SDL_Window** wp = (SDL_Window**)lua_touserdata(L, -1);
        win = wp ? *wp : NULL;
        lua_pop(L, 1);
    }
    if (!win) { lua_pushnil(L); return 1; }
    SDL_Renderer** ren = (SDL_Renderer**)lua_newuserdata(L, sizeof(SDL_Renderer*));
    *ren = SDL_CreateRenderer(win, NULL);
    luaL_getmetatable(L, RENDERER_MT);
    lua_setmetatable(L, -2);
    lua_pushvalue(L, -1);
    lua_setfield(L, LUA_REGISTRYINDEX, CUR_REN_KEY);
    return 1;
}

static int l_select(lua_State* L) {
    SDL_Renderer** ren = (SDL_Renderer**)luaL_checkudata(L, 1, RENDERER_MT);
    lua_pushvalue(L, 1);
    lua_setfield(L, LUA_REGISTRYINDEX, CUR_REN_KEY);
    return 0;
}

static int lr_destroy(lua_State* L) {
    SDL_Renderer** ren = (SDL_Renderer**)luaL_checkudata(L, 1, RENDERER_MT);
    if (ren && *ren) { SDL_DestroyRenderer(*ren); *ren = NULL; }
    return 0;
}

static int lr_clear(lua_State* L) {
    SDL_Renderer* ren = cur_ren(L);
    SDL_SetRenderDrawColor(ren,
        (Uint8)luaL_optinteger(L, 1, 0),
        (Uint8)luaL_optinteger(L, 2, 0),
        (Uint8)luaL_optinteger(L, 3, 0),
        (Uint8)luaL_optinteger(L, 4, 255));
    SDL_RenderClear(ren);
    return 0;
}

static int lr_present(lua_State* L) {
    SDL_RenderPresent(cur_ren(L));
    return 0;
}

static int lr_get_size(lua_State* L) {
    SDL_Renderer* ren = cur_ren(L);
    int w, h;
    SDL_GetRenderOutputSize(ren, &w, &h);
    lua_pushinteger(L, w); lua_pushinteger(L, h);
    return 2;
}

static int lr_set_vsync(lua_State* L) {
    lua_pushboolean(L, SDL_SetRenderVSync(cur_ren(L), (int)luaL_checkinteger(L, 1)));
    return 1;
}

static int lr_set_scale(lua_State* L) {
    lua_pushboolean(L, SDL_SetRenderScale(cur_ren(L),
        (float)luaL_checknumber(L, 1),
        (float)luaL_checknumber(L, 2)));
    return 1;
}

static int lr_set_colorf(lua_State* L) {
    lua_pushboolean(L, SDL_SetRenderDrawColorFloat(cur_ren(L),
        (float)luaL_checknumber(L, 1),
        (float)luaL_checknumber(L, 2),
        (float)luaL_checknumber(L, 3),
        (float)luaL_optnumber(L, 4, 1)));
    return 1;
}

static int lr_set_color(lua_State* L) {
    lua_pushboolean(L, SDL_SetRenderDrawColor(cur_ren(L),
        (Uint8)luaL_checkinteger(L, 1),
        (Uint8)luaL_checkinteger(L, 2),
        (Uint8)luaL_checkinteger(L, 3),
        (Uint8)luaL_optinteger(L, 4, 255)));
    return 1;
}

static int lr_set_logical_presentation(lua_State* L) {
    SDL_SetRenderLogicalPresentation(cur_ren(L),
        (int)luaL_checkinteger(L, 1),
        (int)luaL_checkinteger(L, 2),
        (SDL_RendererLogicalPresentation)luaL_checkinteger(L, 3));
    return 0;
}

static int lr_get_logical_presentation(lua_State* L) {
    SDL_Renderer* ren = cur_ren(L);
    int w, h;
    SDL_RendererLogicalPresentation mode;
    lua_pushboolean(L, SDL_GetRenderLogicalPresentation(ren, &w, &h, &mode));
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    lua_pushinteger(L, (int)mode);
    return 4;
}

static int lr_get_logical_presentation_rect(lua_State* L) {
    SDL_Renderer* ren = cur_ren(L);
    SDL_FRect rect;
    lua_pushboolean(L, SDL_GetRenderLogicalPresentationRect(ren, &rect));
    lua_pushnumber(L, rect.x);
    lua_pushnumber(L, rect.y);
    lua_pushnumber(L, rect.w);
    lua_pushnumber(L, rect.h);
    return 5;
}

static int lr_debug_text(lua_State* L) {
    lua_pushboolean(L, SDL_RenderDebugText(cur_ren(L),
        (float)luaL_checknumber(L, 1),
        (float)luaL_checknumber(L, 2),
        luaL_checkstring(L, 3)));
    return 1;
}

/* ---- draw ---- */
static int lr_draw_point(lua_State* L) {
    lua_pushboolean(L, SDL_RenderPoint(cur_ren(L),
        (float)luaL_checknumber(L, 1),
        (float)luaL_checknumber(L, 2)));
    return 1;
}

static int lr_draw_points(lua_State* L) {
    SDL_Renderer* ren = cur_ren(L);
    int n = (int)luaL_len(L, 1);
    SDL_FPoint* pts = (SDL_FPoint*)SDL_malloc(n * sizeof(SDL_FPoint));
    for (int i = 1; i <= n; i++) {
        lua_rawgeti(L, 1, i);
        lua_getfield(L, -1, "x"); pts[i-1].x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, -1, "y"); pts[i-1].y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pop(L, 1);
    }
    lua_pushboolean(L, SDL_RenderPoints(ren, pts, n));
    SDL_free(pts);
    return 1;
}

static int lr_draw_line(lua_State* L) {
    lua_pushboolean(L, shape_draw_line(cur_ren(L),
        (float)luaL_checknumber(L, 1),
        (float)luaL_checknumber(L, 2),
        (float)luaL_checknumber(L, 3),
        (float)luaL_checknumber(L, 4)));
    return 1;
}

static int lr_draw_rectangle(lua_State* L) {
    SDL_FRect rect;
    rect.x = (float)luaL_checknumber(L, 2);
    rect.y = (float)luaL_checknumber(L, 3);
    rect.w = (float)luaL_checknumber(L, 4);
    rect.h = (float)luaL_checknumber(L, 5);
    lua_pushboolean(L, shape_draw_rectangle(cur_ren(L), luaL_checkstring(L, 1), rect));
    return 1;
}

static int lr_draw_polygon(lua_State* L) {
    SDL_Renderer* ren = cur_ren(L);
    const char* mode = luaL_checkstring(L, 1);
    int n = (lua_gettop(L) - 1) / 2;
    if (n < 3) return luaL_error(L, "polygon needs >= 3 vertices");
    SDL_FPoint* pts = (SDL_FPoint*)SDL_malloc(n * sizeof(SDL_FPoint));
    for (int i = 0; i < n; i++) {
        pts[i].x = (float)luaL_checknumber(L, 2 + i * 2);
        pts[i].y = (float)luaL_checknumber(L, 3 + i * 2);
    }
    SDL_FColor c;
    SDL_GetRenderDrawColorFloat(ren, &c.r, &c.g, &c.b, &c.a);
    lua_pushboolean(L, shape_draw_polygon(ren, mode, pts, n, c));
    SDL_free(pts);
    return 1;
}

static int lr_draw_circle(lua_State* L) {
    SDL_Renderer* ren = cur_ren(L);
    SDL_FPoint center = {
        (float)luaL_checknumber(L, 2),
        (float)luaL_checknumber(L, 3)
    };
    lua_pushboolean(L, shape_draw_circle(ren,
        luaL_checkstring(L, 1), center,
        (float)luaL_checknumber(L, 4),
        (int)luaL_optinteger(L, 5, 32)));
    return 1;
}

static int lr_draw_grid(lua_State* L) {
    SDL_Renderer* ren = cur_ren(L);
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TTABLE);
    SDL_FPoint start, end;
    lua_getfield(L, 1, "x"); start.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, 1, "y"); start.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, 2, "x"); end.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, 2, "y"); end.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    shape_draw_grid(ren, start, end, (float)luaL_checknumber(L, 3));
    return 0;
}

static int lr_draw_gridx(lua_State* L) {
    SDL_Renderer* ren = cur_ren(L);
    luaL_checktype(L, 1, LUA_TTABLE);
    SDL_FPoint pos;
    lua_getfield(L, 1, "x"); pos.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, 1, "y"); pos.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    shape_draw_gridx(ren, pos,
        (int)luaL_checkinteger(L, 2),
        (int)luaL_checkinteger(L, 3),
        (float)luaL_checknumber(L, 4));
    return 0;
}

/* =================================================================
 * Image
 * ================================================================= */
static int li_gc(lua_State* L) {
    image_p* img = (image_p*)lua_touserdata(L, 1);
    if (img && *img) { image_destroy(*img); *img = NULL; }
    return 0;
}

static int li_new(lua_State* L) {
    image_p* img = (image_p*)lua_newuserdata(L, sizeof(image_p));
    *img = image_create(cur_ren(L), luaL_checkstring(L, 1));
    if (*img) {
        luaL_getmetatable(L, IMAGE_MT);
        lua_setmetatable(L, -2);
    }
    return 1;
}

static int li_free(lua_State* L) {
    image_p* img = (image_p*)luaL_checkudata(L, 1, IMAGE_MT);
    if (img && *img) { image_destroy(*img); *img = NULL; }
    return 0;
}

static int li_draw(lua_State* L) {
    image_p* img = (image_p*)luaL_checkudata(L, 1, IMAGE_MT);
    image_draw(*img, (SDL_FPoint){
        (float)luaL_checknumber(L, 2),
        (float)luaL_checknumber(L, 3) });
    return 0;
}

static int li_draw_ex(lua_State* L) {
    image_p* img = (image_p*)luaL_checkudata(L, 1, IMAGE_MT);
    luaL_checktype(L, 2, LUA_TTABLE);
    SDL_FRect src;
    lua_getfield(L, 2, "x"); src.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, 2, "y"); src.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, 2, "w"); src.w = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, 2, "h"); src.h = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    image_draw_ex(*img, &src,
        (SDL_FPoint){(float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4)},
        (float)luaL_optnumber(L, 5, 0),
        (SDL_FPoint){(float)luaL_optnumber(L, 6, 1), (float)luaL_optnumber(L, 7, 1)},
        (SDL_FPoint){(float)luaL_optnumber(L, 8, 0), (float)luaL_optnumber(L, 9, 0)});
    return 0;
}

/* =================================================================
 * Font
 * ================================================================= */
static int lf_gc(lua_State* L) {
    font_p* f = (font_p*)lua_touserdata(L, 1);
    if (f && *f) { font_destroy(*f); *f = NULL; }
    return 0;
}

static int lf_create(lua_State* L) {
    font_p* f = (font_p*)lua_newuserdata(L, sizeof(font_p));
    *f = font_create(cur_ren(L), luaL_checkstring(L, 1), (int)luaL_checkinteger(L, 2));
    luaL_getmetatable(L, FONT_MT);
    lua_setmetatable(L, -2);
    return 1;
}

static int lf_destroy(lua_State* L) {
    font_p* f = (font_p*)luaL_checkudata(L, 1, FONT_MT);
    if (f && *f) { font_destroy(*f); *f = NULL; }
    return 0;
}

/* =================================================================
 * TextTexture
 * ================================================================= */
static int lt_gc(lua_State* L) {
    text_texture_p* t = (text_texture_p*)lua_touserdata(L, 1);
    if (t && *t) { text_destroy(*t); *t = NULL; }
    return 0;
}

static int lt_create(lua_State* L) {
    font_p* f = (font_p*)luaL_checkudata(L, 1, FONT_MT);
    int n = (int)luaL_len(L, 2);
    int* cps = (int*)SDL_malloc(n * sizeof(int));
    for (int i = 1; i <= n; i++) {
        lua_geti(L, 2, i); cps[i-1] = (int)luaL_checkinteger(L, -1); lua_pop(L, 1);
    }
    SDL_Color color = {
        (Uint8)luaL_optinteger(L, 3, 255),
        (Uint8)luaL_optinteger(L, 4, 255),
        (Uint8)luaL_optinteger(L, 5, 255),
        (Uint8)luaL_optinteger(L, 6, 255)
    };
    text_texture_p* t = (text_texture_p*)lua_newuserdata(L, sizeof(text_texture_p));
    *t = text_create(*f, cps, n, color);
    SDL_free(cps);
    luaL_getmetatable(L, TEXT_MT);
    lua_setmetatable(L, -2);
    return 1;
}

static int lt_update(lua_State* L) {
    text_texture_p* t = (text_texture_p*)luaL_checkudata(L, 1, TEXT_MT);
    font_p* f = (font_p*)luaL_checkudata(L, 2, FONT_MT);
    int n = (int)luaL_len(L, 3);
    int* cps = (int*)SDL_malloc(n * sizeof(int));
    for (int i = 1; i <= n; i++) {
        lua_geti(L, 3, i); cps[i-1] = (int)luaL_checkinteger(L, -1); lua_pop(L, 1);
    }
    SDL_Color color = {
        (Uint8)luaL_optinteger(L, 4, 255),
        (Uint8)luaL_optinteger(L, 5, 255),
        (Uint8)luaL_optinteger(L, 6, 255),
        (Uint8)luaL_optinteger(L, 7, 255)
    };
    text_update(*t, *f, cps, n, color);
    SDL_free(cps);
    return 0;
}

static int lt_destroy(lua_State* L) {
    text_texture_p* t = (text_texture_p*)luaL_checkudata(L, 1, TEXT_MT);
    if (t && *t) { text_destroy(*t); *t = NULL; }
    return 0;
}

static int lt_print(lua_State* L) {
    text_texture_p* t = (text_texture_p*)luaL_checkudata(L, 1, TEXT_MT);
    text_print(cur_ren(L), *t,
        (float)luaL_checknumber(L, 2),
        (float)luaL_checknumber(L, 3));
    return 0;
}

/* print(font, text, x, y [, r, g, b, a]) — 一站式 */
static int l_print(lua_State* L) {
    font_p* f = (font_p*)luaL_checkudata(L, 1, FONT_MT);
    const char* str = luaL_checkstring(L, 2);
    SDL_Color c = {
        (Uint8)luaL_optinteger(L, 5, 255),
        (Uint8)luaL_optinteger(L, 6, 255),
        (Uint8)luaL_optinteger(L, 7, 255),
        (Uint8)luaL_optinteger(L, 8, 255)
    };
    text_printx(cur_ren(L), *f, str, (int)SDL_strlen(str), c,
        (float)luaL_checknumber(L, 3),
        (float)luaL_checknumber(L, 4));
    return 0;
}

/* =================================================================
 * SpriteBatch
 * ================================================================= */
static int lb_gc(lua_State* L) {
    sprite_batch_p* b = (sprite_batch_p*)lua_touserdata(L, 1);
    if (b && *b) { sprite_batch_destroy(*b); *b = NULL; }
    return 0;
}

static int lb_new(lua_State* L) {
    sprite_batch_p* b = (sprite_batch_p*)lua_newuserdata(L, sizeof(sprite_batch_p));
    *b = sprite_batch_create(cur_ren(L), luaL_checkstring(L, 1));
    if (*b) {
        luaL_getmetatable(L, BATCH_MT);
        lua_setmetatable(L, -2);
    }
    return 1;
}

static int lb_free(lua_State* L) {
    sprite_batch_p* b = (sprite_batch_p*)luaL_checkudata(L, 1, BATCH_MT);
    if (b && *b) { sprite_batch_destroy(*b); *b = NULL; }
    return 0;
}

static int lb_get_width(lua_State* L) {
    sprite_batch_p* b = (sprite_batch_p*)luaL_checkudata(L, 1, BATCH_MT);
    lua_pushnumber(L, sprite_batch_get_width(*b));
    return 1;
}

static int lb_get_height(lua_State* L) {
    sprite_batch_p* b = (sprite_batch_p*)luaL_checkudata(L, 1, BATCH_MT);
    lua_pushnumber(L, sprite_batch_get_height(*b));
    return 1;
}

static int lb_add(lua_State* L) {
    sprite_batch_p* b = (sprite_batch_p*)luaL_checkudata(L, 1, BATCH_MT);
    luaL_checktype(L, 2, LUA_TTABLE);
    luaL_checktype(L, 3, LUA_TTABLE);
    SDL_FRect src, dst;
    lua_getfield(L, 2, "x"); src.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, 2, "y"); src.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, 2, "w"); src.w = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, 2, "h"); src.h = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, 3, "x"); dst.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, 3, "y"); dst.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, 3, "w"); dst.w = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, 3, "h"); dst.h = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    sprite_batch_add_ex(*b, src, dst,
        (float)luaL_optnumber(L, 4, 0),
        (SDL_FPoint){(float)luaL_optnumber(L, 5, 0), (float)luaL_optnumber(L, 6, 0)});
    return 0;
}

static int lb_draw(lua_State* L) {
    sprite_batch_p* b = (sprite_batch_p*)luaL_checkudata(L, 1, BATCH_MT);
    sprite_batch_draw(*b);
    return 0;
}

static int lb_set_image(lua_State* L) {
    sprite_batch_p* b = (sprite_batch_p*)luaL_checkudata(L, 1, BATCH_MT);
    sprite_batch_set_image(*b, luaL_checkstring(L, 2));
    return 0;
}

/* =================================================================
 * Module registration
 * ================================================================= */
int luaopen_graphics(lua_State* L) {
#define REG_MT(name, gcfunc) do { \
    luaL_newmetatable(L, name); \
    lua_pushcfunction(L, gcfunc); \
    lua_setfield(L, -2, "__gc"); \
    lua_pop(L, 1); \
} while(0)

    REG_MT(RENDERER_MT, lr_gc);
    REG_MT(IMAGE_MT, li_gc);
    REG_MT(FONT_MT, lf_gc);
    REG_MT(TEXT_MT, lt_gc);
    REG_MT(BATCH_MT, lb_gc);

    luaL_checkversion(L);
    luaL_Reg l[] = {
        {"create",    lr_create},
        {"select",    l_select},
        {"destroy",   lr_destroy},
        {"clear",     lr_clear},
        {"present",   lr_present},
        {"get_size",  lr_get_size},
        {"set_vsync", lr_set_vsync},
        {"set_scale", lr_set_scale},
        {"set_colorf", lr_set_colorf},
        {"set_color", lr_set_color},
        {"set_logical_presentation", lr_set_logical_presentation},
        {"get_logical_presentation", lr_get_logical_presentation},
        {"get_logical_presentation_rect", lr_get_logical_presentation_rect},
        {"debug_text", lr_debug_text},
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
        {"new_spritebatch", lb_new},
        {"free_spritebatch", lb_free},
        {"get_spritebatch_width", lb_get_width},
        {"get_spritebatch_height", lb_get_height},
        {"add_sprite",  lb_add},
        {"draw_spritebatch", lb_draw},
        {"set_spritebatch_image", lb_set_image},
        {NULL, NULL}
    };
    luaL_newlib(L, l);
    return 1;
}
