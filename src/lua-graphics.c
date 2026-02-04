#include "luaclib.h"
#include "liv-graphics.h"
#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"


static int l_render_create(lua_State* L)
{
        SDL_Window* window;
        const char* name;
        SDL_Renderer* renderer;
        SDL_assert(lua_islightuserdata(L, 1));
        window = (SDL_Window*)lua_touserdata(L, 1);
        name = luaL_checkstring(L, 2);
        renderer = SDL_CreateRenderer(window, name);
        lua_pushlightuserdata(L, renderer);
        return 1;
}

static int l_render_destroy(lua_State* L)
{
        SDL_Renderer* renderer;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        SDL_DestroyRenderer(renderer);
        return 0;
}

static int l_set_render_vsync(lua_State* L)
{
        SDL_Renderer* renderer;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        bool r = SDL_SetRenderVSync(renderer, 1);
        lua_pushboolean(L, r);
        return 1;
}


static int l_set_render_scale(lua_State* L)
{
        SDL_Renderer* renderer;
        float x, y;
        bool ok;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        x = luaL_checknumber(L, 2);
        y = luaL_checknumber(L, 3);
        ok = SDL_SetRenderScale(renderer, x, y);
        lua_pushboolean(L, ok);
        return 1;
}

static int l_render_debug_text(lua_State* L)
{
        SDL_Renderer* renderer;
        float x, y;
        const char* str;
        bool ok;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        x = luaL_checknumber(L, 2);
        y = luaL_checknumber(L, 3);
        str = luaL_checkstring(L, 4);
        ok = SDL_RenderDebugText(renderer, x, y, str);
        lua_pushboolean(L, ok);
        return 1;
}


static int l_set_colorf(lua_State* L)
{
        float r, g, b, a;
        bool ret;
        SDL_Renderer* renderer;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        r = (float)luaL_checknumber(L, 2);
        g = (float)luaL_checknumber(L, 3);
        b = (float)luaL_checknumber(L, 4);
        a = (float)luaL_checknumber(L, 5);
        ret = SDL_SetRenderDrawColorFloat(renderer, r, g, b, a);
        lua_pushboolean(L, ret);
        return 1;
}


static int l_set_color(lua_State* L)
{
        int r, g, b, a;
        bool ret;
        SDL_Renderer* renderer;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        r = luaL_checkinteger(L, 2);
        g = luaL_checkinteger(L, 3);
        b = luaL_checkinteger(L, 4);
        a = luaL_checkinteger(L, 5);
        ret = SDL_SetRenderDrawColor(renderer, r, g, b, a);
        lua_pushboolean(L, ret);
        return 1;
}


static int l_render_clear(lua_State* L)
{
        SDL_Renderer* renderer;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        bool r = SDL_RenderClear(renderer);
        lua_pushboolean(L, r);
        return 1;
}

static int l_render_present(lua_State* L)
{
        SDL_Renderer* renderer;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        bool r = SDL_RenderPresent(renderer);
        lua_pushboolean(L, r);
        return 1;
}

static int l_set_render_draw_color(lua_State* L)
{
        Uint8 r, g, b, a;
        bool ret;
        SDL_Renderer* renderer;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        r = (Uint8)luaL_checkinteger(L, 2);
        g = (Uint8)luaL_checkinteger(L, 3);
        b = (Uint8)luaL_checkinteger(L, 4);
        a = (Uint8)luaL_checkinteger(L, 5);
        ret = SDL_SetRenderDrawColor(renderer, r, g, b, a);
        lua_pushboolean(L, ret);
        return 1;
}


static int l_get_render_logical_presentation(lua_State* L)
{
        SDL_Renderer* renderer;
        int w, h;
        bool ok;
        SDL_RendererLogicalPresentation mode;
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        ok = SDL_GetRenderLogicalPresentation(renderer, &w, &h, &mode);
        lua_pushboolean(L, ok);
        lua_pushinteger(L, w);
        lua_pushinteger(L, h);
        lua_pushinteger(L, mode);
        return 4;
}

static int l_get_render_logical_presentation_rect(lua_State* L)
{
        SDL_Renderer* renderer;
        SDL_FRect rect;
        bool ok;
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        ok = SDL_GetRenderLogicalPresentationRect(renderer, &rect);
        lua_pushboolean(L, ok);
        lua_pushinteger(L, rect.x);
        lua_pushinteger(L, rect.y);
        lua_pushinteger(L, rect.h);
        lua_pushinteger(L, rect.w);
        return 5;
}

static int l_set_render_logical_presentation(lua_State* L)
{
        SDL_Renderer* renderer;
        int w, h;
        SDL_RendererLogicalPresentation mode;
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        w = luaL_checkinteger(L, 2);
        h = luaL_checkinteger(L, 3);
        mode = (SDL_RendererLogicalPresentation)luaL_checkinteger(L, 4);
        SDL_SetRenderLogicalPresentation(renderer, w, h, mode);
        return 0;
}



static int l_draw_point(lua_State* L)
{
        SDL_Renderer* renderer;
        float x, y;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        x = (float)luaL_checknumber(L, 2);
        y = (float)luaL_checknumber(L, 3);
        bool r = SDL_RenderPoint(renderer, x, y);
        lua_pushboolean(L, r);
        return 1;
}

static int l_draw_points(lua_State* L)
{
        SDL_Renderer* renderer;
        SDL_FPoint* points;
        SDL_FPoint point = { 0, 0 };
        int array_length;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        array_length = luaL_len(L, 2);
        points = (SDL_FPoint*)SDL_malloc(sizeof(SDL_FPoint) * array_length);

        for (int i = 1; i <= array_length; i++) {
                lua_rawgeti(L, 2, i);
                if (lua_istable(L, -1)) {
                        lua_getfield(L, -1, "x");
                        if (lua_isnumber(L, -1)) {
                                point.x = lua_tonumber(L, -1);
                        }
                        lua_pop(L, 1);
                        lua_getfield(L, -1, "y");
                        if (lua_isnumber(L, -1)) {
                                point.y = lua_tonumber(L, -1);
                        }
                        lua_pop(L, 1);
                        points[i - 1] = point;
                }
                lua_pop(L, 1); 
        }

        bool r = SDL_RenderPoints(renderer, points, array_length);
        lua_pushboolean(L, r);
        return 1;
}

static int l_draw_line(lua_State* L)
{
        SDL_Renderer* renderer;
        float x1, y1, x2, y2;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        x1 = (float)luaL_checknumber(L, 2);
        y1 = (float)luaL_checknumber(L, 3);
        x2 = (float)luaL_checknumber(L, 4);
        y2 = (float)luaL_checknumber(L, 5);
        bool r = shape_draw_line(renderer, x1, y1, x2, y2);
        lua_pushboolean(L, r);
        return 1;
}

static int l_draw_rectangle(lua_State* L)
{
        SDL_Renderer* renderer;
        SDL_FRect rect;
        const char* mode;
        bool result;

        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        mode = luaL_checkstring(L, 2);
        rect.x = (float)luaL_checknumber(L, 3);
        rect.y = (float)luaL_checknumber(L, 4);
        rect.w = (float)luaL_checknumber(L, 5);
        rect.h = (float)luaL_checknumber(L, 6);
        result = shape_draw_rectangle(renderer, mode, rect);
        lua_pushboolean(L, result);
        return 1;
}

static int l_draw_polygon(lua_State* L)
{
        bool result;
        SDL_Renderer* renderer;
        SDL_FPoint* points;
        int argc, vertex_count;
        const char* mode;
        SDL_FColor color;
        SDL_assert(lua_islightuserdata(L, 1));
        argc = lua_gettop(L);
        vertex_count = (argc - 2) / 2;
        if (vertex_count < 3) {
                luaL_error(L, "polygon needs at least 3 vertices (got %d)", vertex_count);
                lua_pushboolean(L, false);
                return 1;
        }
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        mode = luaL_checkstring(L, 2);
        SDL_GetRenderDrawColorFloat(renderer, &color.r, &color.g, &color.b, &color.a);

        points = SDL_malloc(sizeof(SDL_FPoint) * (vertex_count + 1));
        for (int i = 0; i < vertex_count; i++) {
                points[i].x = (float)luaL_checknumber(L, 3 + i * 2);
                points[i].y = (float)luaL_checknumber(L, 4 + i * 2);
        }
        result = shape_draw_polygon(renderer, mode, points, vertex_count + 1, color);
        SDL_free(points);
        lua_pushboolean(L, result);
        return 1;
}

static int l_draw_circle(lua_State* L)
{
        SDL_Renderer* renderer;
        bool result = false;
        int argc;
        const char* mode;
        SDL_FPoint center;
        float radius;
        int segments = 32;
        SDL_assert(lua_islightuserdata(L, 1));

        argc = lua_gettop(L);
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        mode = luaL_checkstring(L, 2);
        center.x = luaL_checknumber(L, 3);
        center.y = luaL_checknumber(L, 4);
        radius = luaL_checknumber(L, 5);

        if (argc >= 6) {
                segments = luaL_optinteger(L, 6, 32);
        }

        result = shape_draw_circle(renderer, mode, center, radius, segments);

        lua_pushboolean(L, result);
        return 1;
}

static int l_draw_grid(lua_State* L)
{
        SDL_Renderer* renderer;
        SDL_FPoint start, end;
        float grid_size;

        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        luaL_checktype(L, 2, LUA_TTABLE);
        luaL_checktype(L, 3, LUA_TTABLE);

        lua_getfield(L, 2, "x");
        start.x = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "y");
        start.y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        
        lua_getfield(L, 3, "x");
        end.x = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "y");
        end.y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);

        grid_size = (float)luaL_checknumber(L, 4);
        
        shape_draw_grid(renderer, start, end, grid_size);
        return 0;
}

static int l_draw_gridx(lua_State* L)
{
        SDL_Renderer* renderer;
        int rows, cols;
        SDL_FPoint position;
        float grid_size;

        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 2, "x");
        position.x = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "y");
        position.y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        rows = luaL_checkinteger(L, 3);
        cols = luaL_checkinteger(L, 4);
        grid_size = (float)luaL_checknumber(L, 5);
        shape_draw_gridx(renderer, position, rows, cols, grid_size);
        return 0;
}



static int l_image_new(lua_State* L)
{
        image_p image;
        const char* filename;
        SDL_Renderer* renderer;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        filename = luaL_checkstring(L, 2);
        image = image_create(renderer, filename);
        if (image) {
                lua_pushlightuserdata(L, image);
        }
        else {
                lua_pushnil(L);
        }
        return 1;
}

static int l_image_free(lua_State* L)
{
        image_p image;
        SDL_assert(lua_islightuserdata(L, 1));
        image = (image_p)lua_touserdata(L, 1);
        image_destroy(image);
        return 0;
}

static int l_image_draw(lua_State* L)
{
        image_p image;
        SDL_FPoint position;
        SDL_assert(lua_islightuserdata(L, 1));
        image = (image_p)lua_touserdata(L, 1);
        position.x = (float)luaL_checknumber(L, 2);
        position.y = (float)luaL_checknumber(L, 3);
        image_draw(image, position);
        return 0;
}


static int l_image_draw_ex(lua_State* L)
{
        image_p image;
        SDL_FPoint position;
        SDL_assert(lua_islightuserdata(L, 1));
        image = (image_p)lua_touserdata(L, 1);
        position.x = (float)luaL_checknumber(L, 2);
        position.y = (float)luaL_checknumber(L, 3);
        image_draw(image, position);
        return 0;
}

static int l_font_create(lua_State* L)
{
        const char* filename;
        int fontsize;
        font_p fontinfo;
        SDL_Renderer* renderer;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        filename = luaL_checkstring(L, 2);
        fontsize = luaL_checkinteger(L, 3);
        fontinfo = font_create(renderer, filename, fontsize);
        lua_pushlightuserdata(L, fontinfo);
        return 1;
}

static int l_font_destroy(lua_State* L)
{
        font_p fontinfo;
        fontinfo = (font_p)lua_touserdata(L, 1);
        font_destroy(fontinfo);
        return 0;
}

static int l_text_create(lua_State* L)
{
        int* codepoints;
        int num_codepoints;
        text_texture_p text_texture;
        font_p fontinfo;
        SDL_Color color = (SDL_Color){255,255,255,255};
        fontinfo = (font_p)lua_touserdata(L, 1);
        num_codepoints = (int)luaL_len(L, 2);
        codepoints = (int*)SDL_malloc(sizeof(int) * num_codepoints);
        for (int i = 1; i <= num_codepoints; i++) {
                lua_geti(L, 2, i);
                codepoints[i - 1] = luaL_checkinteger(L, -1);
                lua_pop(L, 1);
        }
        color.r = luaL_checkinteger(L, 3);
        color.g = luaL_checkinteger(L, 4);
        color.b = luaL_checkinteger(L, 5);
        color.a = luaL_checkinteger(L, 6);
        text_texture = text_create(fontinfo, codepoints, num_codepoints, color);
        lua_pushlightuserdata(L, text_texture);
        SDL_free(codepoints);
        return 1;
}

static int l_text_update(lua_State* L)
{
        int* codepoints;
        int num_codepoints;
        text_texture_p text_texture;
        font_p fontinfo;
        SDL_Color color = (SDL_Color){ 255,255,255,255 };
        text_texture = (text_texture_p)lua_touserdata(L, 1);
        fontinfo = (font_p)lua_touserdata(L, 2);
        num_codepoints = (int)luaL_len(L, 3);
        codepoints = (int*)SDL_malloc(sizeof(int) * num_codepoints);
        for (int i = 1; i <= num_codepoints; i++) {
                lua_geti(L, 3, i);
                codepoints[i - 1] = luaL_checkinteger(L, -1);
                lua_pop(L, 1);
        }
        color.r = luaL_checkinteger(L, 4);
        color.g = luaL_checkinteger(L, 5);
        color.b = luaL_checkinteger(L, 6);
        color.a = luaL_checkinteger(L, 7);
        text_update(text_texture, fontinfo, codepoints, num_codepoints, color);
        SDL_free(codepoints);
        return 1;
}

static int l_text_print(lua_State* L)
{
        SDL_Renderer* renderer;
        text_texture_p text_texture;
        float x, y;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        text_texture = (text_texture_p)lua_touserdata(L, 2);
        x = (float)luaL_checknumber(L, 3);
        y = (float)luaL_checknumber(L, 4);
        text_print(renderer, text_texture, x, y);
        return 0;
}

static int l_text_destroy(lua_State* L)
{
        text_texture_p text_texture;
        SDL_assert(lua_islightuserdata(L, 1));
        text_texture = (text_texture_p)lua_touserdata(L, 1);
        text_destroy(text_texture);
        return 0;
}

static int l_spritebatch_new(lua_State* L)
{
        spritebatch_p batch;
        const char* filename;
        SDL_Renderer* renderer;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        filename = luaL_checkstring(L, 2);
        batch = spritebatch_create(renderer, filename);
        if (batch) {
                lua_pushlightuserdata(L, batch);
        }
        else {
                lua_pushnil(L);
        }
        return 1;
}

static int l_spritebatch_destroy(lua_State* L)
{
        spritebatch_p batch;
        batch = (spritebatch_p)lua_touserdata(L, 1);
        spritebatch_destroy(batch);
        return 0;
}

static int l_spritebatch_get_width(lua_State* L)
{
        spritebatch_p batch;
        batch = (spritebatch_p)lua_touserdata(L, 1);
        lua_pushnumber(L, spritebatch_get_width(batch));
        return 1;
}

static int l_spritebatch_get_height(lua_State* L)
{
        spritebatch_p batch;
        batch = (spritebatch_p)lua_touserdata(L, 1);
        lua_pushnumber(L, spritebatch_get_height(batch));
        return 1;
}

static int l_spritebatch_add(lua_State* L)
{
        spritebatch_p batch;
        SDL_FRect src_rect;
        SDL_FPoint position, origin;
        float rotation, scale_x, scale_y;

        batch = (spritebatch_p)lua_touserdata(L, 1);
        luaL_checktype(L, 2, LUA_TTABLE);
        luaL_checktype(L, 3, LUA_TTABLE);

        lua_getfield(L, 2, "x");
        src_rect.x = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "y");
        src_rect.y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "w");
        src_rect.w = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "h");
        src_rect.h = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);


        lua_getfield(L, 3, "x");
        position.x = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "y");
        position.y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);

        rotation = (float)luaL_checknumber(L, 4);
        scale_x = (float)luaL_checknumber(L, 5);
        scale_y = (float)luaL_checknumber(L, 6);

        lua_getfield(L, 7, "x");
        origin.x = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 7, "y");
        origin.y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);

        spritebatch_add(batch, position, rotation, 
                scale_x, scale_y, origin, src_rect);
        return 0;
}

static int l_spritebatch_add_ex(lua_State* L)
{
        spritebatch_p batch;
        SDL_FRect src_rect, dest_rect;
        float rotation;
        SDL_FPoint origin;

        batch = (spritebatch_p)lua_touserdata(L, 1);
        luaL_checktype(L, 2, LUA_TTABLE);
        luaL_checktype(L, 3, LUA_TTABLE);

        lua_getfield(L, 2, "x");
        src_rect.x = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "y");
        src_rect.y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "w");
        src_rect.w = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "h");
        src_rect.h = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 3, "x");
        dest_rect.x = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "y");
        dest_rect.y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "w");
        dest_rect.w = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "h");
        dest_rect.h = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);

        rotation = (float)luaL_checknumber(L, 4);

        lua_getfield(L, 5, "x");
        origin.x = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 5, "y");
        origin.y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);

        spritebatch_add_ex(batch, src_rect, dest_rect, rotation, origin);
        return 0;
}

static int l_spritebatch_draw(lua_State* L)
{
        spritebatch_p batch;
        batch = (spritebatch_p)lua_touserdata(L, 1);
        spritebatch_draw(batch);
        return 0;
}

int luaopen_graphics(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
                {"create", l_render_create},
                {"destroy", l_render_destroy},
                {"set_vsync", l_set_render_vsync},
                {"set_scale", l_set_render_scale},
                {"debugText", l_render_debug_text},
                {"set_colorf", l_set_colorf},
                {"set_color", l_set_color},
                {"clear", l_render_clear},
                {"present", l_render_present},
                {"set_logical_presentation", l_set_render_logical_presentation},
                {"get_logical_presentation", l_get_render_logical_presentation},
                {"get_logical_presentation_rect", l_get_render_logical_presentation_rect},

                {"point", l_draw_point},
                {"points", l_draw_points},
                {"line", l_draw_line},
                {"rectangle", l_draw_rectangle},
                {"polygon", l_draw_polygon},
                {"circle", l_draw_circle},
                {"grid", l_draw_grid},
                {"gridx", l_draw_gridx},

                {"new_image", l_image_new},
                {"free_image", l_image_free},
                {"draw_image", l_image_draw},
                {"draw_image_ex", l_image_draw_ex},

                {"create_font", l_font_create},
                {"destroy_font", l_font_destroy},
                {"create_text", l_text_create},
                {"update_text", l_text_update},
                {"destroy_text", l_text_destroy},
                {"print_text", l_text_print},

                {"new_spritebatch", l_spritebatch_new},
                {"free_spritebatch", l_spritebatch_destroy},
                {"get_spritebatch_width", l_spritebatch_get_width},
                {"get_spritebatch_height", l_spritebatch_get_height},
                {"add_sprite", l_spritebatch_add},
                {"add_sprite_ex", l_spritebatch_add_ex},
                {"draw_spritebatch", l_spritebatch_draw},
                {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
