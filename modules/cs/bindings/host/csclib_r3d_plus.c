/* C# -> joy glue: hand-written InternalCalls for r3d L3/L2 APIs (batch 2).
 *
 * Companion to csclib_r3d.c: texture load, logical-presentation conversion,
 * by-value vec3/color draws, and r3d UI object creation/query/color/state.
 * These are status:off in joy.idl (void* buffers, by-value structs, char*
 * returns) so the generator emits no thunks; we register Mono InternalCalls.
 *
 * Handles are uint64_t joy_obj handles. Create returns are registered with
 * the matching *_destroy dtor so the C# strong-type wrapper (whose finalizer
 * calls Native.Release) frees them. Tags must match the generated chain.
 *
 * NOTE: keep this file ASCII-only (MSVC reads UTF-8-without-BOM as local codepage).
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>   /* mono_get_root_domain */
#include <mono/utils/mono-publib.h>
#include "joy_csharp.h"
#include <joy/joy.h>
#include <joy/api/runtime/joy_obj.h>

static void* deref(uint64_t h, const char* tag)
{
        if (!h) return NULL;
        return joy_obj_deref(h, tag, NULL);
}
static joy_r3d_p deref_r3d(uint64_t h) { return (joy_r3d_p)deref(h, "joy_r3d_p"); }
static joy_render_mesh_p deref_mesh(uint64_t h) { return (joy_render_mesh_p)deref(h, "joy_render_mesh_p"); }

static char* dup_mono_str(MonoString* s)
{
        return s ? mono_string_to_utf8(s) : NULL;
}

/* ---- dtors for created UI objects ---- */
static void dtor_button(void* p)         { if (p) joy_r3d_button_destroy((joy_r3d_button_p)p); }
static void dtor_label(void* p)          { if (p) joy_r3d_label_destroy((joy_r3d_label_p)p); }
static void dtor_input(void* p)          { if (p) joy_r3d_input_destroy((joy_r3d_input_p)p); }
static void dtor_combobox(void* p)       { if (p) joy_r3d_combobox_input_destroy((joy_r3d_combobox_input_p)p); }
static void dtor_slider(void* p)         { if (p) joy_r3d_slider_destroy((joy_r3d_slider_p)p); }
static void dtor_checkbox(void* p)       { if (p) joy_r3d_checkbox_destroy((joy_r3d_checkbox_p)p); }

/* ---- texture ---- */
static intptr_t w_create_texture(uint64_t r_h, int w, int h, MonoArray* rgba, bool flip)
{
        joy_r3d_p r = deref_r3d(r_h);
        if (!r || !rgba) return 0;
        int need = mono_array_length(rgba);
        if (need < w * h * 4) return 0;
        unsigned char* buf = (unsigned char*)mono_array_addr_with_size(rgba, 1, 0);
        void* tex = joy_r3d_create_texture(r, w, h, buf, flip ? 1 : 0);
        return (intptr_t)tex;
}

static intptr_t w_load_texture_png(uint64_t r_h, MonoString* path, int* out_w, int* out_h)
{
        joy_r3d_p r = deref_r3d(r_h);
        if (!r) { if (out_w) *out_w = 0; if (out_h) *out_h = 0; return 0; }
        char* p = dup_mono_str(path);
        void* tex = joy_r3d_load_texture_png(r, p, out_w, out_h);
        if (p) mono_free(p);
        return (intptr_t)tex;
}

/* ---- logical presentation ---- */
static void w_get_logical_presentation(uint64_t r_h, int* w, int* h)
{
        joy_r3d_p r = deref_r3d(r_h);
        if (!r) { if (w) *w = 0; if (h) *h = 0; return; }
        joy_r3d_get_logical_presentation(r, w, h);
}
static void w_window_to_logical(uint64_t r_h, float wx, float wy, float* lx, float* ly)
{
        joy_r3d_p r = deref_r3d(r_h);
        if (!r) { if (lx) *lx = 0; if (ly) *ly = 0; return; }
        joy_r3d_window_to_logical(r, wx, wy, lx, ly);
}
static void w_logical_to_window(uint64_t r_h, float lx, float ly, float* wx, float* wy)
{
        joy_r3d_p r = deref_r3d(r_h);
        if (!r) { if (wx) *wx = 0; if (wy) *wy = 0; return; }
        joy_r3d_logical_to_window(r, lx, ly, wx, wy);
}

/* ---- by-value draws ---- */
static void w_draw_mesh(uint64_t r_h, uint64_t m_h,
                        float px, float py, float pz, float sx, float sy, float sz,
                        uint8_t cr, uint8_t cg, uint8_t cb, uint8_t ca)
{
        joy_r3d_p r = deref_r3d(r_h);
        joy_render_mesh_p m = deref_mesh(m_h);
        if (!r || !m) return;
        joy_vec3_t pos = { px, py, pz }, scale = { sx, sy, sz };
        joy_color_t color = { cr, cg, cb, ca };
        joy_r3d_draw_mesh(r, m, pos, scale, color);
}
static void w_draw_mesh_tex(uint64_t r_h, uint64_t m_h,
                            float px, float py, float pz, float sx, float sy, float sz,
                            intptr_t tex)
{
        joy_r3d_p r = deref_r3d(r_h);
        joy_render_mesh_p m = deref_mesh(m_h);
        if (!r || !m) return;
        joy_vec3_t pos = { px, py, pz }, scale = { sx, sy, sz };
        joy_r3d_draw_mesh_tex(r, m, pos, scale, (void*)tex);
}

/* ---- UI create ---- */
static uint64_t w_button_create(float x, float y, float w, float h, MonoString* text)
{
        char* t = dup_mono_str(text);
        joy_r3d_button_p b = joy_r3d_button_create(x, y, w, h, t ? t : "");
        if (t) mono_free(t);
        if (!b) return 0;
        return joy_obj_register((void*)b, "joy_r3d_button_p", dtor_button, "joy_r3d_button_create");
}
static uint64_t w_label_create(float x, float y, float fs,
                               uint8_t cr, uint8_t cg, uint8_t cb, MonoString* text)
{
        char* t = dup_mono_str(text);
        joy_r3d_label_p l = joy_r3d_label_create(x, y, fs, cr, cg, cb, t ? t : "");
        if (t) mono_free(t);
        if (!l) return 0;
        return joy_obj_register((void*)l, "joy_r3d_label_p", dtor_label, "joy_r3d_label_create");
}
static uint64_t w_input_create(float x, float y, float w, float h, float fs,
                               uint8_t cr, uint8_t cg, uint8_t cb, MonoString* text)
{
        char* t = dup_mono_str(text);
        joy_r3d_input_p inp = joy_r3d_input_create(x, y, w, h, fs, cr, cg, cb, t ? t : "");
        if (t) mono_free(t);
        if (!inp) return 0;
        return joy_obj_register((void*)inp, "joy_r3d_input_p", dtor_input, "joy_r3d_input_create");
}
static uint64_t w_combobox_input_create(float x, float y, float w, float h, float fs,
                                        uint8_t cr, uint8_t cg, uint8_t cb, MonoString* text)
{
        char* t = dup_mono_str(text);
        joy_r3d_combobox_input_p ci = joy_r3d_combobox_input_create(x, y, w, h, fs, cr, cg, cb, t ? t : "");
        if (t) mono_free(t);
        if (!ci) return 0;
        return joy_obj_register((void*)ci, "joy_r3d_combobox_input_p", dtor_combobox, "joy_r3d_combobox_input_create");
}
static uint64_t w_slider_create(float x, float y, float w, float h, float min, float max, float initial)
{
        joy_r3d_slider_p s = joy_r3d_slider_create(x, y, w, h, min, max, initial);
        if (!s) return 0;
        return joy_obj_register((void*)s, "joy_r3d_slider_p", dtor_slider, "joy_r3d_slider_create");
}
static uint64_t w_checkbox_create(float x, float y, float size, MonoString* label)
{
        char* t = dup_mono_str(label);
        joy_r3d_checkbox_p cb = joy_r3d_checkbox_create(x, y, size, t ? t : "");
        if (t) mono_free(t);
        if (!cb) return 0;
        return joy_obj_register((void*)cb, "joy_r3d_checkbox_p", dtor_checkbox, "joy_r3d_checkbox_create");
}

/* ---- get_text ---- */
static MonoString* w_input_get_text(uint64_t inp_h)
{
        joy_r3d_input_p inp = (joy_r3d_input_p)deref(inp_h, "joy_r3d_input_p");
        const char* txt = inp ? joy_r3d_input_get_text(inp) : NULL;
        return mono_string_new(mono_get_root_domain(), txt ? txt : "");
}
static MonoString* w_combobox_input_get_text(uint64_t ci_h)
{
        joy_r3d_combobox_input_p ci = (joy_r3d_combobox_input_p)deref(ci_h, "joy_r3d_combobox_input_p");
        const char* txt = ci ? joy_r3d_combobox_input_get_text(ci) : NULL;
        return mono_string_new(mono_get_root_domain(), txt ? txt : "");
}

/* ---- slider colors / checkbox state ---- */
static void w_slider_set_colors(uint64_t s_h,
                                uint8_t tr, uint8_t tg, uint8_t tb, uint8_t ta,
                                uint8_t fr, uint8_t fg, uint8_t fb, uint8_t fa,
                                uint8_t hr, uint8_t hg, uint8_t hb, uint8_t ha,
                                uint8_t mr, uint8_t mg, uint8_t mb, uint8_t ma)
{
        joy_r3d_slider_p s = (joy_r3d_slider_p)deref(s_h, "joy_r3d_slider_p");
        if (!s) return;
        joy_color_t track = { tr, tg, tb, ta }, fill = { fr, fg, fb, fa };
        joy_color_t thumb = { hr, hg, hb, ha }, thumb_hover = { mr, mg, mb, ma };
        joy_r3d_slider_set_colors(s, track, fill, thumb, thumb_hover);
}
static void w_checkbox_set_state(uint64_t cb_h, int state)
{
        joy_r3d_checkbox_p cb = (joy_r3d_checkbox_p)deref(cb_h, "joy_r3d_checkbox_p");
        if (cb) joy_r3d_checkbox_set_state(cb, (joy_r3d_checkbox_state_t)state);
}
static int w_checkbox_get_state(uint64_t cb_h)
{
        joy_r3d_checkbox_p cb = (joy_r3d_checkbox_p)deref(cb_h, "joy_r3d_checkbox_p");
        return cb ? (int)joy_r3d_checkbox_get_state(cb) : 0;
}

void csopen_r3d_plus(void)
{
        csx_register_internal_call("Joy.Api.R3dExtPlus::create_texture", (const void*)w_create_texture);
        csx_register_internal_call("Joy.Api.R3dExtPlus::load_texture_png", (const void*)w_load_texture_png);
        csx_register_internal_call("Joy.Api.R3dExtPlus::get_logical_presentation", (const void*)w_get_logical_presentation);
        csx_register_internal_call("Joy.Api.R3dExtPlus::window_to_logical", (const void*)w_window_to_logical);
        csx_register_internal_call("Joy.Api.R3dExtPlus::logical_to_window", (const void*)w_logical_to_window);
        csx_register_internal_call("Joy.Api.R3dExtPlus::draw_mesh", (const void*)w_draw_mesh);
        csx_register_internal_call("Joy.Api.R3dExtPlus::draw_mesh_tex", (const void*)w_draw_mesh_tex);
        csx_register_internal_call("Joy.Api.R3dExtPlus::button_create", (const void*)w_button_create);
        csx_register_internal_call("Joy.Api.R3dExtPlus::label_create", (const void*)w_label_create);
        csx_register_internal_call("Joy.Api.R3dExtPlus::input_create", (const void*)w_input_create);
        csx_register_internal_call("Joy.Api.R3dExtPlus::combobox_input_create", (const void*)w_combobox_input_create);
        csx_register_internal_call("Joy.Api.R3dExtPlus::slider_create", (const void*)w_slider_create);
        csx_register_internal_call("Joy.Api.R3dExtPlus::checkbox_create", (const void*)w_checkbox_create);
        csx_register_internal_call("Joy.Api.R3dExtPlus::input_get_text", (const void*)w_input_get_text);
        csx_register_internal_call("Joy.Api.R3dExtPlus::combobox_input_get_text", (const void*)w_combobox_input_get_text);
        csx_register_internal_call("Joy.Api.R3dExtPlus::slider_set_colors", (const void*)w_slider_set_colors);
        csx_register_internal_call("Joy.Api.R3dExtPlus::checkbox_set_state", (const void*)w_checkbox_set_state);
        csx_register_internal_call("Joy.Api.R3dExtPlus::checkbox_get_state", (const void*)w_checkbox_get_state);
}
