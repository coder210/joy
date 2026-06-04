#include <assert.h>
#include <stdlib.h>
#include "jsclib.h"
#include "../jui.h"

#define LABEL_TAG    "ui_label"
#define BUTTON_TAG   "ui_button"
#define COMBO_TAG    "ui_combobox"
#define CHECK_TAG    "ui_checkbox"
#define RADIO_TAG    "ui_radiobutton"
#define GRID_TAG     "ui_datagrid"
#define TEXTI_TAG    "ui_textinput"
#define SLIDER_TAG   "ui_slider"
#define RADIOG_TAG   "ui_radiogroup"

static void free_ud(js_State *J, void *p) { (void)J; if (p) SDL_free(p); }

static SDL_Color read_color(js_State *J, int idx)
{
        SDL_Color c;
        js_getproperty(J, idx, "r"); c.r = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "g"); c.g = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "b"); c.b = js_tonumber(J, -1); js_pop(J, 1);
        js_getproperty(J, idx, "a"); c.a = js_tonumber(J, -1); js_pop(J, 1);
        return c;
}

static void push_ud(js_State *J, const char *tag, void *p)
{
        js_newuserdata(J, tag, p, free_ud);
}

/* ===== Label ===== */
static void jui_label_create(js_State *J)
{
        SDL_Renderer *r = js_touserdata(J, 1, "SDL_Renderer*");
        void *f = js_touserdata(J, 2, NULL);
        font_p *fp = (font_p*)f;
        label_p lb = label_create(r, fp ? *fp : NULL, NULL, 0);
        push_ud(J, LABEL_TAG, lb);
}
static void jui_label_destroy(js_State *J) { label_destroy(js_touserdata(J, 1, LABEL_TAG)); }
static void jui_label_set_position(js_State *J) {
        label_p lb = js_touserdata(J, 1, LABEL_TAG);
        lb->position.x = js_tonumber(J, 2); lb->position.y = js_tonumber(J, 3);
}
static void jui_label_draw(js_State *J) { label_draw(js_touserdata(J, 1, LABEL_TAG)); }

/* ===== Button ===== */
static void jui_button_create(js_State *J) {
        SDL_Renderer *r = js_touserdata(J, 1, "SDL_Renderer*");
        button_p b = button_create(r, js_tonumber(J, 2), js_tonumber(J, 3));
        push_ud(J, BUTTON_TAG, b);
}
static void jui_button_destroy(js_State *J) { button_destroy(js_touserdata(J, 1, BUTTON_TAG)); }
static void jui_button_set_position(js_State *J) {
        button_set_position(js_touserdata(J, 1, BUTTON_TAG), js_tonumber(J, 2), js_tonumber(J, 3));
}
static void jui_button_set_text(js_State *J) {
        button_p b = js_touserdata(J, 1, BUTTON_TAG);
        void *f = js_touserdata(J, 2, NULL);
        font_p *fp = (font_p*)f;
        const char *s = js_tostring(J, 3);
        SDL_Color c = {255,255,255,255};
        if (js_isobject(J, 4)) c = read_color(J, 4);
        button_set_textx(b, fp ? *fp : NULL, s, strlen(s), c);
}
static void jui_button_is_hovered(js_State *J) {
        js_pushboolean(J, button_is_hovered(js_touserdata(J, 1, BUTTON_TAG), js_tonumber(J, 2), js_tonumber(J, 3)));
}
static void jui_button_is_pressed(js_State *J) {
        js_pushboolean(J, button_is_pressed(js_touserdata(J, 1, BUTTON_TAG), js_tonumber(J, 2), js_tonumber(J, 3)));
}
static void jui_button_set_hovered(js_State *J) { button_set_hovered(js_touserdata(J, 1, BUTTON_TAG), js_toboolean(J, 2)); }
static void jui_button_set_pressed(js_State *J) { button_set_pressed(js_touserdata(J, 1, BUTTON_TAG), js_toboolean(J, 2)); }
static void jui_button_draw(js_State *J) { button_draw(js_touserdata(J, 1, BUTTON_TAG)); }

/* ===== Combobox ===== */
static void jui_combobox_create(js_State *J) {
        SDL_Renderer *r = js_touserdata(J, 1, "SDL_Renderer*");
        SDL_FRect rect = {js_tonumber(J,2), js_tonumber(J,3), js_tonumber(J,4), js_tonumber(J,5)};
        push_ud(J, COMBO_TAG, combobox_create(r, rect));
}
static void jui_combobox_destroy(js_State *J) { combobox_destroy(js_touserdata(J, 1, COMBO_TAG)); }
static void jui_combobox_additem(js_State *J) {
        combobox_p c = js_touserdata(J, 1, COMBO_TAG);
        if (c->item_count >= 20) return;
        const char *text = js_tostring(J, 2);
        void *f = js_touserdata(J, 3, NULL);
        font_p *fp = (font_p*)f;
        SDL_Color col = {0,0,0,255};
        combobox_item_p item = &c->items[c->item_count];
        item->text = text_createx(fp ? *fp : NULL, text, strlen(text), col);
        item->is_selected = false;
        c->item_count++;
        c->dropdown_rect.h = c->item_count * c->item_height;
}
static void jui_combobox_draw(js_State *J) { combobox_draw(js_touserdata(J, 1, COMBO_TAG)); }
static void jui_combobox_is_pressed(js_State *J) { js_pushboolean(J, combobox_is_pressed(js_touserdata(J,1,COMBO_TAG), js_tonumber(J,2), js_tonumber(J,3))); }
static void jui_combobox_is_open(js_State *J) { js_pushboolean(J, combobox_is_open(js_touserdata(J,1,COMBO_TAG))); }
static void jui_combobox_set_open(js_State *J) { combobox_set_open(js_touserdata(J,1,COMBO_TAG), js_toboolean(J,2)); }
static void jui_combobox_hit_item(js_State *J) { js_pushnumber(J, combobox_hit_item(js_touserdata(J,1,COMBO_TAG), js_tonumber(J,2), js_tonumber(J,3))); }
static void jui_combobox_set_selected(js_State *J) { combobox_set_selected(js_touserdata(J,1,COMBO_TAG), js_tonumber(J,2)); }
static void jui_combobox_get_selected(js_State *J) { js_pushnumber(J, combobox_get_selected(js_touserdata(J,1,COMBO_TAG))); }

/* ===== Checkbox ===== */
static void jui_checkbox_create(js_State *J) {
        checkbox_p cb = checkbox_create(js_tonumber(J,1), js_tonumber(J,2), js_tonumber(J,3), js_tostring(J,4));
        push_ud(J, CHECK_TAG, cb);
}
static void jui_checkbox_destroy(js_State *J) { checkbox_destroy(js_touserdata(J, 1, CHECK_TAG)); }
static void jui_checkbox_draw(js_State *J) {
        checkbox_draw(js_touserdata(J,1,CHECK_TAG), js_touserdata(J,2,"SDL_Renderer*"), NULL);
}
static void jui_checkbox_is_pressed(js_State *J) { js_pushboolean(J, checkbox_is_pressed(js_touserdata(J,1,CHECK_TAG), js_tonumber(J,2), js_tonumber(J,3))); }
static void jui_checkbox_set_hovered(js_State *J) { checkbox_set_hovered(js_touserdata(J,1,CHECK_TAG), js_toboolean(J,2)); }

/* ===== Radio Button ===== */
static void jui_radiobutton_create(js_State *J) {
        radio_button_p r = radio_button_create(js_tonumber(J,1), js_tonumber(J,2), js_tonumber(J,3), js_tostring(J,4), js_tonumber(J,5));
        push_ud(J, RADIO_TAG, r);
}
static void jui_radiobutton_destroy(js_State *J) { radio_button_destroy(js_touserdata(J,1,RADIO_TAG)); }
static void jui_radiobutton_is_pressed(js_State *J) { js_pushboolean(J, radio_button_is_pressed(js_touserdata(J,1,RADIO_TAG), js_tonumber(J,2), js_tonumber(J,3))); }
static void jui_radiobutton_set_hovered(js_State *J) { radio_button_set_hovered(js_touserdata(J,1,RADIO_TAG), js_toboolean(J,2)); }

/* ===== Radio Group ===== */
static void jui_radiogroup_create(js_State *J) { push_ud(J, RADIOG_TAG, radio_button_group_create()); }
static void jui_radiogroup_destroy(js_State *J) { radio_button_group_destroy(js_touserdata(J,1,RADIOG_TAG)); }
static void jui_radiogroup_add_button(js_State *J) { radio_button_group_add_button(js_touserdata(J,1,RADIOG_TAG), js_touserdata(J,2,RADIO_TAG)); }
static void jui_radiogroup_set_selected(js_State *J) { radio_button_group_set_selected(js_touserdata(J,1,RADIOG_TAG), js_tonumber(J,2)); }
static void jui_radiogroup_draw(js_State *J) { radio_button_group_draw(js_touserdata(J,1,RADIOG_TAG), js_touserdata(J,2,"SDL_Renderer*")); }

/* ===== Datagrid ===== */
static void jui_datagrid_create(js_State *J) {
        SDL_Renderer *r = js_touserdata(J, 1, "SDL_Renderer*");
        SDL_Rect rect = {(int)js_tonumber(J,2),(int)js_tonumber(J,3),(int)js_tonumber(J,4),(int)js_tonumber(J,5)};
        int cols = js_tonumber(J, 6);
        int has_hdr = js_toboolean(J, 7);
        void *f = js_touserdata(J, 8, NULL);
        font_p *fp = (font_p*)f;
        push_ud(J, GRID_TAG, datagrid_create(r, rect, cols, has_hdr, fp ? *fp : NULL));
}
static void jui_datagrid_destroy(js_State *J) { datagrid_destroy(js_touserdata(J,1,GRID_TAG)); }
static void jui_datagrid_draw(js_State *J) { datagrid_draw(js_touserdata(J,1,GRID_TAG), js_touserdata(J,2,"SDL_Renderer*")); }
static void jui_datagrid_setheaders(js_State *J) {
        datagrid_p g = js_touserdata(J,1,GRID_TAG);
        void *f = js_touserdata(J,2,NULL);
        font_p *fp = (font_p*)f;
        SDL_Color col = {0,0,0,255};
        int n = js_getlength(J, 3);
        for (int i = 0; i < n && i < g->col_count; i++) {
                js_getindex(J, 3, i);
                const char *text = js_tostring(J, -1);
                if (g->headers[i]) text_destroy(g->headers[i]);
                g->headers[i] = text_createx(fp ? *fp : NULL, text, strlen(text), col);
                js_pop(J, 1);
        }
}
static void jui_datagrid_addrow(js_State *J) {
        datagrid_p g = js_touserdata(J,1,GRID_TAG);
        int n = js_getlength(J, 2);
        if (n > g->col_count) n = g->col_count;
        const char **row = malloc(n * sizeof(char*));
        for (int i = 0; i < n; i++) {
                js_getindex(J, 2, i);
                row[i] = js_tostring(J, -1);
                js_pop(J, 1);
        }
        datagrid_addrow(g, row);
        free(row);
}
static void jui_datagrid_hit_row(js_State *J) { js_pushnumber(J, datagrid_hit_row(js_touserdata(J,1,GRID_TAG), js_tonumber(J,2), js_tonumber(J,3))); }
static void jui_datagrid_scroll(js_State *J) { datagrid_scroll(js_touserdata(J,1,GRID_TAG), js_tonumber(J,2)); }
static void jui_datagrid_set_selected_row(js_State *J) { datagrid_set_selected_row(js_touserdata(J,1,GRID_TAG), js_tonumber(J,2)); }
static void jui_datagrid_get_row_count(js_State *J) { js_pushnumber(J, ((datagrid_p)js_touserdata(J,1,GRID_TAG))->row_count); }
static void jui_datagrid_get_cell(js_State *J) {
        datagrid_p g = js_touserdata(J,1,GRID_TAG);
        int r = js_tonumber(J,2), c = js_tonumber(J,3);
        if (r >= 0 && (size_t)r < kv_size(g->data) && c >= 0 && c < g->col_count) {
                char *v = kv_A(kv_A(g->data, r), c);
                if (v) { js_pushstring(J, v); return; }
        }
        js_pushundefined(J);
}

/* ===== TextInput ===== */
static void jui_textinput_create(js_State *J) {
        SDL_Renderer *r = js_touserdata(J,1,"SDL_Renderer*");
        void *f = js_touserdata(J,2,NULL);
        font_p *fp = (font_p*)f;
        SDL_FRect rect = {js_tonumber(J,3), js_tonumber(J,4), js_tonumber(J,5), js_tonumber(J,6)};
        push_ud(J, TEXTI_TAG, textinput_create(r, rect, fp ? *fp : NULL));
}
static void jui_textinput_destroy(js_State *J) { textinput_destroy(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_set_text(js_State *J) { textinput_set_text(js_touserdata(J,1,TEXTI_TAG), js_tostring(J,2)); }
static void jui_textinput_get_text(js_State *J) { js_pushstring(J, textinput_get_text(js_touserdata(J,1,TEXTI_TAG))); }
static void jui_textinput_draw(js_State *J) { textinput_draw(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_is_pressed(js_State *J) { js_pushboolean(J, textinput_is_pressed(js_touserdata(J,1,TEXTI_TAG), js_tonumber(J,2), js_tonumber(J,3))); }
static void jui_textinput_is_hovered(js_State *J) { js_pushboolean(J, textinput_is_hovered(js_touserdata(J,1,TEXTI_TAG), js_tonumber(J,2), js_tonumber(J,3))); }
static void jui_textinput_focus(js_State *J) { textinput_focus(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_defocus(js_State *J) { textinput_defocus(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_insert(js_State *J) { const char *t = js_tostring(J,2); textinput_insert(js_touserdata(J,1,TEXTI_TAG), t, strlen(t)); }
static void jui_textinput_backspace(js_State *J) { textinput_backspace(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_delete(js_State *J) { textinput_delete(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_cursor_left(js_State *J) { textinput_cursor_left(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_cursor_right(js_State *J) { textinput_cursor_right(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_cursor_home(js_State *J) { textinput_cursor_home(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_cursor_end(js_State *J) { textinput_cursor_end(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_select_left(js_State *J) { textinput_select_left(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_select_right(js_State *J) { textinput_select_right(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_select_all(js_State *J) { textinput_select_all(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_paste(js_State *J) { textinput_paste(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_copy(js_State *J) { textinput_copy(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_cut(js_State *J) { textinput_cut(js_touserdata(J,1,TEXTI_TAG)); }
static void jui_textinput_is_focused(js_State *J) { js_pushboolean(J, textinput_is_focused(js_touserdata(J,1,TEXTI_TAG))); }

/* ===== Slider ===== */
static void jui_slider_create(js_State *J) {
        SDL_Renderer *r = js_touserdata(J,1,"SDL_Renderer*");
        push_ud(J, SLIDER_TAG, slider_create(r, js_tonumber(J,2), js_tonumber(J,3), js_tonumber(J,4), js_tonumber(J,5), js_tonumber(J,6), js_tonumber(J,7), js_tonumber(J,8)));
}
static void jui_slider_destroy(js_State *J) { slider_destroy(js_touserdata(J,1,SLIDER_TAG)); }
static void jui_slider_set_value(js_State *J) { slider_set_value(js_touserdata(J,1,SLIDER_TAG), js_tonumber(J,2)); }
static void jui_slider_get_value(js_State *J) { js_pushnumber(J, slider_get_value(js_touserdata(J,1,SLIDER_TAG))); }
static void jui_slider_set_range(js_State *J) { slider_set_range(js_touserdata(J,1,SLIDER_TAG), js_tonumber(J,2), js_tonumber(J,3)); }
static void jui_slider_draw(js_State *J) { slider_draw(js_touserdata(J,1,SLIDER_TAG)); }
static void jui_slider_is_pressed(js_State *J) { js_pushboolean(J, slider_is_pressed(js_touserdata(J,1,SLIDER_TAG), js_tonumber(J,2), js_tonumber(J,3))); }
static void jui_slider_set_pressed(js_State *J) { slider_set_pressed(js_touserdata(J,1,SLIDER_TAG), js_toboolean(J,2)); }
static void jui_slider_drag_move(js_State *J) { slider_drag_move(js_touserdata(J,1,SLIDER_TAG), js_tonumber(J,2), js_tonumber(J,3)); }
static void jui_slider_is_hovered(js_State *J) { js_pushboolean(J, slider_is_hovered(js_touserdata(J,1,SLIDER_TAG), js_tonumber(J,2), js_tonumber(J,3))); }
static void jui_slider_set_hovered(js_State *J) { slider_set_hovered(js_touserdata(J,1,SLIDER_TAG), js_toboolean(J,2)); }

/* ===== Registration ===== */
static void reg_sub(js_State *J, const char *name, const js_CFunction *pairs)
{
        js_newobject(J);
        while (pairs[0]) {
                js_newcfunction(J, pairs[0], pairs[1] ? pairs[1] : "", 0);
                js_setproperty(J, -2, pairs[1]);
                pairs += 2;
        }
        js_setproperty(J, -2, name);
}

void jsopen_ui(js_State *J)
{
        js_newobject(J);

        static js_CFunction label_fns[] = {
                (js_CFunction)jui_label_create, "create",
                (js_CFunction)jui_label_destroy, "destroy",
                (js_CFunction)jui_label_set_position, "set_position",
                (js_CFunction)jui_label_draw, "draw",
                NULL, NULL
        }; reg_sub(J, "label", label_fns);

        static js_CFunction button_fns[] = {
                (js_CFunction)jui_button_create, "create",
                (js_CFunction)jui_button_destroy, "destroy",
                (js_CFunction)jui_button_set_position, "set_position",
                (js_CFunction)jui_button_set_text, "set_text",
                (js_CFunction)jui_button_is_hovered, "is_hovered",
                (js_CFunction)jui_button_is_pressed, "is_pressed",
                (js_CFunction)jui_button_set_hovered, "set_hovered",
                (js_CFunction)jui_button_set_pressed, "set_pressed",
                (js_CFunction)jui_button_draw, "draw",
                NULL, NULL
        }; reg_sub(J, "button", button_fns);

        static js_CFunction combobox_fns[] = {
                (js_CFunction)jui_combobox_create, "create",
                (js_CFunction)jui_combobox_destroy, "destroy",
                (js_CFunction)jui_combobox_additem, "additem",
                (js_CFunction)jui_combobox_draw, "draw",
                (js_CFunction)jui_combobox_is_pressed, "is_pressed",
                (js_CFunction)jui_combobox_is_open, "is_open",
                (js_CFunction)jui_combobox_set_open, "set_open",
                (js_CFunction)jui_combobox_hit_item, "hit_item",
                (js_CFunction)jui_combobox_set_selected, "set_selected",
                (js_CFunction)jui_combobox_get_selected, "get_selected",
                NULL, NULL
        }; reg_sub(J, "combobox", combobox_fns);

        static js_CFunction checkbox_fns[] = {
                (js_CFunction)jui_checkbox_create, "create",
                (js_CFunction)jui_checkbox_destroy, "destroy",
                (js_CFunction)jui_checkbox_draw, "draw",
                (js_CFunction)jui_checkbox_is_pressed, "is_pressed",
                (js_CFunction)jui_checkbox_set_hovered, "set_hovered",
                NULL, NULL
        }; reg_sub(J, "checkbox", checkbox_fns);

        static js_CFunction radio_fns[] = {
                (js_CFunction)jui_radiobutton_create, "create",
                (js_CFunction)jui_radiobutton_destroy, "destroy",
                (js_CFunction)jui_radiobutton_is_pressed, "is_pressed",
                (js_CFunction)jui_radiobutton_set_hovered, "set_hovered",
                NULL, NULL
        }; reg_sub(J, "radiobutton", radio_fns);

        static js_CFunction radiog_fns[] = {
                (js_CFunction)jui_radiogroup_create, "create",
                (js_CFunction)jui_radiogroup_destroy, "destroy",
                (js_CFunction)jui_radiogroup_add_button, "add_button",
                (js_CFunction)jui_radiogroup_set_selected, "set_selected",
                (js_CFunction)jui_radiogroup_draw, "draw",
                NULL, NULL
        }; reg_sub(J, "button_group", radiog_fns);

        static js_CFunction grid_fns[] = {
                (js_CFunction)jui_datagrid_create, "create",
                (js_CFunction)jui_datagrid_destroy, "destroy",
                (js_CFunction)jui_datagrid_draw, "draw",
                (js_CFunction)jui_datagrid_setheaders, "setheaders",
                (js_CFunction)jui_datagrid_addrow, "addrow",
                (js_CFunction)jui_datagrid_hit_row, "hit_row",
                (js_CFunction)jui_datagrid_scroll, "scroll",
                (js_CFunction)jui_datagrid_set_selected_row, "set_selected_row",
                (js_CFunction)jui_datagrid_get_row_count, "get_row_count",
                (js_CFunction)jui_datagrid_get_cell, "get_cell",
                NULL, NULL
        }; reg_sub(J, "datagrid", grid_fns);

        static js_CFunction texti_fns[] = {
                (js_CFunction)jui_textinput_create, "create",
                (js_CFunction)jui_textinput_destroy, "destroy",
                (js_CFunction)jui_textinput_set_text, "set_text",
                (js_CFunction)jui_textinput_get_text, "get_text",
                (js_CFunction)jui_textinput_draw, "draw",
                (js_CFunction)jui_textinput_is_pressed, "is_pressed",
                (js_CFunction)jui_textinput_is_hovered, "is_hovered",
                (js_CFunction)jui_textinput_focus, "focus",
                (js_CFunction)jui_textinput_defocus, "defocus",
                (js_CFunction)jui_textinput_insert, "insert",
                (js_CFunction)jui_textinput_backspace, "backspace",
                (js_CFunction)jui_textinput_delete, "delete",
                (js_CFunction)jui_textinput_cursor_left, "cursor_left",
                (js_CFunction)jui_textinput_cursor_right, "cursor_right",
                (js_CFunction)jui_textinput_cursor_home, "cursor_home",
                (js_CFunction)jui_textinput_cursor_end, "cursor_end",
                (js_CFunction)jui_textinput_select_left, "select_left",
                (js_CFunction)jui_textinput_select_right, "select_right",
                (js_CFunction)jui_textinput_select_all, "select_all",
                (js_CFunction)jui_textinput_paste, "paste",
                (js_CFunction)jui_textinput_copy, "copy",
                (js_CFunction)jui_textinput_cut, "cut",
                (js_CFunction)jui_textinput_is_focused, "is_focused",
                NULL, NULL
        }; reg_sub(J, "textinput", texti_fns);

        static js_CFunction slider_fns[] = {
                (js_CFunction)jui_slider_create, "create",
                (js_CFunction)jui_slider_destroy, "destroy",
                (js_CFunction)jui_slider_set_value, "set_value",
                (js_CFunction)jui_slider_get_value, "get_value",
                (js_CFunction)jui_slider_set_range, "set_range",
                (js_CFunction)jui_slider_draw, "draw",
                (js_CFunction)jui_slider_is_pressed, "is_pressed",
                (js_CFunction)jui_slider_set_pressed, "set_pressed",
                (js_CFunction)jui_slider_drag_move, "drag_move",
                (js_CFunction)jui_slider_is_hovered, "is_hovered",
                (js_CFunction)jui_slider_set_hovered, "set_hovered",
                NULL, NULL
        }; reg_sub(J, "slider", slider_fns);

        js_setglobal(J, "ui");
}
