// Procedural-map connectivity unit test (flood-fill cluster generator).
// Verifies deterministic output of pmap_generate_seed across many seeds:
//   A. layout non-empty (not fell back to empty field)
//   B. BFS from player reaches ALL open cells (player<->enemy connectivity)
//   C. cluster quality: isolated single-cell obstacles (4 neighbours all open)
//      should be rare / none, i.e. obstacles form contiguous clusters.
// ASCII grid dump of a few seeds included for visual inspection.
//
// Build (MSVC, links joy.lib; cl from a VS x64 dev environment).
// Sources/headers contain UTF-8 Chinese comments — pass /utf-8, otherwise MSVC
// reads the file under the system codepage and may misalign braces:
//   cl /nologo /utf-8 /std:c++17 /EHsc /I e:\c++\joy /I e:\c++\joy\SDL\include ^
//       /I e:\c++\joy\project_proxy3d /I e:\c++\joy\project_proxy3d\shared ^
//       /I e:\c++\joy\build\Debug map_conn_test.cpp ^
//       /Fo:map_conn_test.obj /Fe:map_conn_test.exe ^
//       /link /LIBPATH:e:\c++\joy\build\Debug joy.lib
// Run: place exe next to joy.dll (e.g. build\Debug) then run map_conn_test.exe

#include <cstdio>
#include <cstdlib>
#include <joy/joy.h>
#include "../project_proxy3d/shared/systems/procedural_map.h"

static int g_pass = 0, g_fail = 0;
#define CHECK(cond) do { if (cond) g_pass++; else g_fail++; } while (0)

// One hint used to avoid polluting the file with non-ASCII bytes.

// Generate one seed and check it. Returns 1 when fully connected and non-empty.
static int verify_seed(uint32_t seed, PmLayout* L, int* oIsolated)
{
    pmap_generate_seed(seed, L);

    int total_open = 0;
    for (int i = 0; i < PF_CELLS; i++) if (L->walkable[i]) total_open++;
    int reach = pm_bfs(L->walkable, PM_PLAYER_GX, PM_PLAYER_GZ);

    // isolated singleton obstacle: blocked cell whose 4 neighbours are all open
    int isolated = 0;
    const int dx4[4] = { 1, -1, 0, 0 }, dz4[4] = { 0, 0, 1, -1 };
    for (int gz = 0; gz < PF_GRID_N; gz++) {
        for (int gx = 0; gx < PF_GRID_N; gx++) {
            if (L->walkable[gz * PF_GRID_N + gx]) continue;
            bool iso = true;
            for (int d = 0; d < 4; d++) {
                int nx = gx + dx4[d], nz = gz + dz4[d];
                if (nx < 0 || nx >= PF_GRID_N || nz < 0 || nz >= PF_GRID_N) continue;
                if (!L->walkable[nz * PF_GRID_N + nx]) { iso = false; break; }
            }
            if (iso) isolated++;
        }
    }
    *oIsolated = isolated;
    return (reach == total_open) && (L->count > 0);
}

// ASCII dump: '#'=obstacle, '.'=open, 'P'=player, 'E'=enemy
static void dump_grid(const PmLayout& L, uint32_t seed)
{
    printf("---- seed %u  obstacles=%d ----\n", seed, L.count);
    for (int gz = 0; gz < PF_GRID_N; gz++) {
        for (int gx = 0; gx < PF_GRID_N; gx++) {
            if (gx == PM_PLAYER_GX && gz == PM_PLAYER_GZ) { putchar('P'); continue; }
            if (gx == PM_ENEMY_GX  && gz == PM_ENEMY_GZ)  { putchar('E'); continue; }
            putchar(L.walkable[gz * PF_GRID_N + gx] ? '.' : '#');
        }
        putchar('\n');
    }
}

int main()
{
    const int N = 512;                     // seeds used for statistics
    const unsigned dump_seeds[] = { 1u, 2u, 3u, 0x9E3779B9u };

    int failEmpty = 0, failNoConn = 0, maxIsolated = 0, totIsolated = 0;

    for (uint32_t s = 0; s < N; s++) {
        PmLayout L;
        int iso = 0;
        int ok = verify_seed(s, &L, &iso);

        if (L.count <= 0) failEmpty++;
        else if (!ok)     failNoConn++;
        CHECK(L.count > 0);
        CHECK(ok == 1);

        if (iso > maxIsolated) maxIsolated = iso;
        totIsolated += iso;
        if (iso > 0) {
            printf("seed %u: count=%d  isolated_singletons=%d\n", s, L.count, iso);
        }
    }

    printf("=== summary (%d seeds) ===\n", N);
    printf("empty/fallback failures: %d\n", failEmpty);
    printf("connectivity failures:   %d\n", failNoConn);
    printf("isolated singletons:     total=%d max_per_seed=%d avg_per_seed=%.2f\n",
           totIsolated, maxIsolated, (double)totIsolated / N);

    printf("\n=== sample maps ===\n");
    for (unsigned i = 0; i < sizeof(dump_seeds) / sizeof(dump_seeds[0]); i++) {
        PmLayout L;
        pmap_generate_seed(dump_seeds[i], &L);
        dump_grid(L, dump_seeds[i]);
    }

    printf("\n%d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}