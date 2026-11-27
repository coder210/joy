/* C# -> joy glue: hand-written native bindings for r3d L3 APIs.
 *
 * These APIs are L3 in the IDL (struct pointers / out params / void* buffers)
 * and the generator does not emit thunks for them. We register native bindings:
 *   - handles are uint64_t joy_obj handles; C# passes Jr3d/RenderMesh .Handle;
 *   - mat44 / vertex arrays are float* (C# fixed float[]), read directly;
 *   - out params are float-ptr / int-ptr (C# passes &out argument address).
 *
 * CoreCLR 函数指针表绑定：string -> const char* (UTF-8)，数组 -> float* + 长度，
 * 不再使用 MonoString / MonoArray / mono_* API。
 *
 * Handle tags must match the generated chain ("joy_r3d_p" /
 * "joy_render_mesh_p") or joy_obj_deref type-check returns NULL.
 *
 * NOTE: keep this file ASCII-only. MSVC reads UTF-8-without-BOM as the local
 * codepage; non-ASCII comments break preprocessing.
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
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

/* dtor for a script-created r3d context; joy_r3d_destroy is single-arg. */
static void r3d_destroy_dtor(void* p)
{
        if (p) joy_r3d_destroy((joy_r3d_p)p);
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

/* Script-side r3d creation: deref the generated-chain Jwindow handle, call
 * joy_r3d_create(win, shader_dir), register the result as an owned handle. */
static uint64_t w_r3d_create(uint64_t win_h, const char* shader_dir)
{
        /* generated-chain window uses tag "joy_window_p"; the legacy host
         * borrow bridge registers "jwindow_p". Accept either. */
        joy_window_p win = (joy_window_p)deref(win_h, "joy_window_p");
        if (!win) win = (joy_window_p)deref(win_h, "jwindow_p");
        if (!win) return 0;
        joy_r3d_p r = joy_r3d_create(win, shader_dir ? shader_dir : "");
        if (!r) return 0;
        return joy_obj_register((void*)r, "joy_r3d_p", r3d_destroy_dtor, "joy_r3d_create");
}

/* mat44: C# passes float[16] in joy_mat44_t field order:
 * m0,m4,m8,m12, m1,m5,m9,m13, m2,m6,m10,m14, m3,m7,m11,m15. */
static void read_mat44(const float* p, joy_mat44_t* out)
{
        out->m0 = p[0];  out->m4 = p[1];  out->m8  = p[2];  out->m12 = p[3];
        out->m1 = p[4];  out->m5 = p[5];  out->m9  = p[6];  out->m13 = p[7];
        out->m2 = p[8];  out->m6 = p[9];  out->m10 = p[10]; out->m14 = p[11];
        out->m3 = p[12]; out->m7 = p[13]; out->m11 = p[14]; out->m15 = p[15];
}

static void w_draw_mesh_solid_basis(uint64_t r, uint64_t m, const float* mat,
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

static void w_draw_wire_basis(uint64_t r, uint64_t m, const float* mat,
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

/* mesh_custom: C# passes a float* vertex buffer (5 floats per vertex: x,y,z,u,v),
 * length validated by the C# wrapper. */
static uint64_t w_mesh_custom(uint64_t r, const float* verts, int vertex_count)
{
        joy_r3d_p rp = deref_r3d(r);
        if (!rp || !verts || vertex_count <= 0) return 0;
        joy_render_mesh_p mesh = joy_r3d_mesh_custom(rp, verts, vertex_count);
        if (!mesh) return 0;
        return joy_obj_register((void*)mesh, "joy_render_mesh_p", r3d_mesh_dtor, "joy_r3d_mesh_custom");
}

/* Lit meshes: pos+normal+uv (8 floats/vertex), drawn with the lighting pipeline.
 * Same handle tag "joy_render_mesh_p" as plain meshes; vertex layout differs,
 * so callers must not mix lit meshes with solid/wire draw calls (and vice versa). */
static uint64_t w_mesh_lit_cube(uint64_t r, float w, float h, float d)
{
        joy_r3d_p rp = deref_r3d(r);
        if (!rp) return 0;
        joy_render_mesh_p mesh = joy_r3d_mesh_cube_lit(rp, w, h, d);
        if (!mesh) return 0;
        return joy_obj_register((void*)mesh, "joy_render_mesh_p", r3d_mesh_dtor, "joy_r3d_mesh_cube_lit");
}

static uint64_t w_mesh_lit_plane(uint64_t r, float w, float d, int segs)
{
        joy_r3d_p rp = deref_r3d(r);
        if (!rp) return 0;
        joy_render_mesh_p mesh = joy_r3d_mesh_plane_lit(rp, w, d, segs);
        if (!mesh) return 0;
        return joy_obj_register((void*)mesh, "joy_render_mesh_p", r3d_mesh_dtor, "joy_r3d_mesh_plane_lit");
}

static uint64_t w_mesh_lit_sphere(uint64_t r, float radius, int slices, int stacks)
{
        joy_r3d_p rp = deref_r3d(r);
        if (!rp) return 0;
        joy_render_mesh_p mesh = joy_r3d_mesh_sphere_lit(rp, radius, slices, stacks);
        if (!mesh) return 0;
        return joy_obj_register((void*)mesh, "joy_render_mesh_p", r3d_mesh_dtor, "joy_r3d_mesh_sphere_lit");
}

/* Draw a lit mesh with a solid base color. The engine resolves the color to a
 * 1x1 texture internally; lighting (ambient + directional) is applied by the
 * fragment shader using the mesh normals. Model matrix uses the same float[16]
 * field order as the solid/wire basis calls. */
static void w_draw_mesh_lit_color(uint64_t r, uint64_t m, const float* mat,
                                  uint8_t cr, uint8_t cg, uint8_t cb, uint8_t ca)
{
        joy_r3d_p rp = deref_r3d(r);
        joy_render_mesh_p mp = deref_mesh(m);
        if (!rp || !mp || !mat) return;
        joy_mat44_t model;
        read_mat44(mat, &model);
        joy_color_t color = { cr, cg, cb, ca };
        joy_r3d_draw_mesh_lit_color(rp, mp, &model, color);
}

void csopen_r3d(void)
{
        csx_register_internal_call("Joy.Api.R3dExt::r3d_create", (const void*)w_r3d_create);
        csx_register_internal_call("Joy.Api.R3dExt::borrow_r3d", (const void*)w_borrow_r3d);
        csx_register_internal_call("Joy.Api.R3dExt::draw_mesh_solid_basis", (const void*)w_draw_mesh_solid_basis);
        csx_register_internal_call("Joy.Api.R3dExt::draw_wire_basis", (const void*)w_draw_wire_basis);
        csx_register_internal_call("Joy.Api.R3dExt::world_to_screen", (const void*)w_world_to_screen);
        csx_register_internal_call("Joy.Api.R3dExt::get_window_size", (const void*)w_get_window_size);
        csx_register_internal_call("Joy.Api.R3dExt::mesh_custom", (const void*)w_mesh_custom);
        csx_register_internal_call("Joy.Api.R3dExt::mesh_lit_cube", (const void*)w_mesh_lit_cube);
        csx_register_internal_call("Joy.Api.R3dExt::mesh_lit_plane", (const void*)w_mesh_lit_plane);
        csx_register_internal_call("Joy.Api.R3dExt::mesh_lit_sphere", (const void*)w_mesh_lit_sphere);
        csx_register_internal_call("Joy.Api.R3dExt::draw_mesh_lit_color", (const void*)w_draw_mesh_lit_color);
}
