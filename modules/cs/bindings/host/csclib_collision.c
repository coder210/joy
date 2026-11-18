/* C# -> joy glue: hand-written InternalCall for GJK 3D box-vs-box.
 *
 * The GJK/EPA collider APIs are level:X in the IDL (struct pointers + out
 * params), so the generator cannot emit them. This bridge exposes a pure
 * scalar box-vs-box query: C# passes fp values (int64 Q32) for both boxes and
 * the initial direction, and receives the contact normal + depth.
 *
 * Reusing joy_collision_gjk3df_* guarantees C# and C++ compute identical
 * collision results (same implementation), which is required for frame-sync
 * determinism.
 *
 * ASCII-only comments: MSVC reads UTF-8-without-BOM as the local codepage.
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <mono/metadata/object.h>
#include <mono/utils/mono-publib.h>
#include "joy_csharp.h"
#include <joy/joy.h>

/* box (a) vs box (b); fills normal (a->b) and depth on hit. */
static bool w_gjk_box_box(
        int64_t ax, int64_t ay, int64_t az, int64_t ahx, int64_t ahy, int64_t ahz,
        int64_t bx, int64_t by, int64_t bz, int64_t bhx, int64_t bhy, int64_t bhz,
        int64_t dirx, int64_t diry, int64_t dirz,
        int64_t* out_nx, int64_t* out_ny, int64_t* out_nz, int64_t* out_depth)
{
        joy_gjk3df_collider_t A, B;
        joy_gjk3df_contact_t contact;
        joy_vec3f_t pa = { ax, ay, az }, ha = { ahx, ahy, ahz };
        joy_vec3f_t pb = { bx, by, bz }, hb = { bhx, bhy, bhz };
        joy_vec3f_t dir = { dirx, diry, dirz };

        joy_collision_gjk3df_init_box(&A, pa, ha);
        joy_collision_gjk3df_init_box(&B, pb, hb);

        bool hit = joy_collision_gjk3df_collide(&A, &B, dir, &contact);
        if (hit) {
                if (out_nx) *out_nx = contact.normal.x;
                if (out_ny) *out_ny = contact.normal.y;
                if (out_nz) *out_nz = contact.normal.z;
                if (out_depth) *out_depth = contact.depth;
        }
        return hit;
}

void csopen_collision(void)
{
        csx_register_internal_call("Joy.Api.CollisionExt::gjk_box_box", (const void*)w_gjk_box_box);
}
