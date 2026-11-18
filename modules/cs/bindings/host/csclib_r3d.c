/* C# -> joy glue: hand-written InternalCalls for r3d L3 APIs.
 *
 * These APIs are L3 in the IDL (struct pointers / out params / void* buffers)
 * and the generator does not emit thunks for them. We register Mono
 * InternalCalls directly instead:
 *   - handles are uint64_t joy_obj handles; C# passes Jr3d/RenderMesh .Handle;
 *   - mat44 / vertex arrays are MonoArray<float>, read element by element;
 *   - out params are float-ptr / int-ptr (Mono passes the C# out argument address).
 *
 * Handle tags must match the generated chain ("joy_r3d_p" /
 * "joy_render_mesh_p") or joy_obj_deref type-check returns NULL.
 *
 * NOTE: keep this file ASCII-only. MSVC reads UTF-8-without-BOM as the local
 * codepage; non-ASCII comments break preprocessing.
 */
#include <stdio.h>   /* indirectly provides intptr_t / uintptr_t to mono detail headers */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <mono/metadata/object.h>
#include <mono/utils/mono-publib.h>  /* mono_free decl not carried by object.h */
#include "joy_csharp.h"
#include <joy/joy.h>
#include <joy/api/runtime/joy_obj.h>

/* Equivalent to generated dtor_joy_r3d_mesh_destroy_one: uses the public
 * single-arg destroy (deferred free queue); self-contained, no dependency on
 * static symbols in generated files. */
static void r3d_mesh_dtor(void* p)
{
        if (p) joy_r3d_mesh_destroy_one((joy_render_mesh_p)p);
}

/* handle -> raw pointer; NULL on failure (including wrong tag). */
static void* deref(uint64_t h, const char* tag)
{
        if (!h) return NULL;
        return joy_obj_deref(h, tag, NULL);
}

static joy_r3d_p deref_r3d(uint64_t h)          { return (joy_r3d_p)deref(h, "joy_r3d_p"); }
static joy_render_mesh_p deref_mesh(uint64_t h) { return (joy_render_mesh_p)deref(h, "joy_render_mesh_p"); }

/* Borrow a host-created joy_r3d_p into a script-side handle (owner stays host). */
static uint64_t w_borrow_r3d(intptr_t p)
{
        if (!p) return 0;
        return joy_obj_register((void*)p, "joy_r3d_p", NULL, "host.borrow");
}

/* mat44: C# passes float[16] in joy_mat44_t field order:
 * m0,m4,m8,m12, m1,m5,m9,m13, m2,m6,m10,m14, m3,m7,m11,m15. */
static void read_mat44(MonoArray* a, joy_mat44_t* out)
{
        float* p = (float*)(void*)mono_array_addr_with_size(a, sizeof(float), 0);
        out->m0 = p[0];  out->m4 = p[1];  out->m8  = p[2];  out->m12 = p[3];
        out->m1 = p[4];  out->m5 = p[5];  out->m9  = p[6];  out->m13 = p[7];
        out->m2 = p[8];  out->m6 = p[9];  out->m10 = p[10]; out->m14 = p[11];
        out->m3 = p[12]; out->m7 = p[13]; out->m11 = p[14]; out->m15 = p[15];
}

static void w_draw_mesh_solid_basis(uint64_t r, uint64_t m, MonoArray* mat,
                                    uint8_t cr, uint8_t cg, uint8_t cb, uint8_t ca)
{
        joy_r3d_p rp = deref_r3d(r);
        joy_render_mesh_p mp = deref_mesh(m);
        if (!rp || !mp || !mat) return;
        joy_mat44_t model;
        read_mat44(mat, &model);
        joy_color_t color = { cr, cg, cb, ca };
        joy_r3d_draw_mesh_solid_basis(rp, mp, &model, color);
}

static void w_draw_wire_basis(uint64_t r, uint64_t m, MonoArray* mat,
                              uint8_t cr, uint8_t cg, uint8_t cb, uint8_t ca)
{
        joy_r3d_p rp = deref_r3d(r);
        joy_render_mesh_p mp = deref_mesh(m);
        if (!rp || !mp || !mat) return;
        joy_mat44_t model;
        read_mat44(mat, &model);
        joy_color_t color = { cr, cg, cb, ca };
        joy_r3d_draw_wire_basis(rp, mp, &model, color);
}

static bool w_world_to_screen(uint64_t r, float x, float y, float z,
                              float* screen_x, float* screen_y)
{
        joy_r3d_p rp = deref_r3d(r);
        if (!rp) return false;
        joy_vec3_t world = { x, y, z };
        return joy_r3d_world_to_screen(rp, world, screen_x, screen_y);
}

static void w_get_window_size(uint64_t r, int* out_w, int* out_h)
{
        joy_r3d_p rp = deref_r3d(r);
        if (!rp) { if (out_w) *out_w = 0; if (out_h) *out_h = 0; return; }
        joy_r3d_get_window_size(rp, out_w, out_h);
}

/* mesh_custom: C# passes a float[] vertex buffer (5 floats per vertex: x,y,z,u,v). */
static uint64_t w_mesh_custom(uint64_t r, MonoArray* verts, int vertex_count)
{
        joy_r3d_p rp = deref_r3d(r);
        if (!rp || !verts || vertex_count <= 0) return 0;
        int need = mono_array_length(verts);
        if (need <= 0) return 0;
        float* buf = (float*)(void*)mono_array_addr_with_size(verts, sizeof(float), 0);
        joy_render_mesh_p mesh = joy_r3d_mesh_custom(rp, buf, vertex_count);
        if (!mesh) return 0;
        return joy_obj_register((void*)mesh, "joy_render_mesh_p", r3d_mesh_dtor, "joy_r3d_mesh_custom");
}

void csopen_r3d(void)
{
        csx_register_internal_call("Joy.Api.R3dExt::borrow_r3d", (const void*)w_borrow_r3d);
        csx_register_internal_call("Joy.Api.R3dExt::draw_mesh_solid_basis", (const void*)w_draw_mesh_solid_basis);
        csx_register_internal_call("Joy.Api.R3dExt::draw_wire_basis", (const void*)w_draw_wire_basis);
        csx_register_internal_call("Joy.Api.R3dExt::world_to_screen", (const void*)w_world_to_screen);
        csx_register_internal_call("Joy.Api.R3dExt::get_window_size", (const void*)w_get_window_size);
        csx_register_internal_call("Joy.Api.R3dExt::mesh_custom", (const void*)w_mesh_custom);
}
