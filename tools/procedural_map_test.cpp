// 独立程序化地图单元测试：确定性验证 pmap_generate_seed / pm_rasterize / pm_bfs / pmap_wave_seed
// 编译（MSVC，链接 joy.lib）：
//   cl /std:c++17 /EHsc /I D:\github\joy /I D:\github\joy\SDL\include ^
//       /I D:\github\joy\project_proxy3d /I D:\github\joy\project_proxy3d\shared ^
//       procedural_map_test.cpp /link /LIBPATH:D:\github\joy\build\Debug joy.lib
// 运行：procedural_map_test.exe → 输出每个用例 PASS/FAIL

#include <cstdio>
#include <joy/joy.h>
#include "../project_proxy3d/shared/systems/procedural_map.h"

static int g_pass = 0, g_fail = 0;

#define CHECK(cond, name) do { \
    if (cond) { g_pass++; printf("PASS  %s\n", name); } \
    else      { g_fail++; printf("FAIL  %s\n", name); } \
} while (0)

// 校验布局有效性（方案 A 不变量）：
// - 全可达：从玩家出生格 BFS 必须到达所有开放格（无死角）
// - 出生点安全区：障碍不得靠近玩家 (0,0) / 敌人出生区 (10,10) 2.2m
// - 单格 1×1、格子对齐、高度在 [PM_HEIGHT_MIN, PM_HEIGHT_MAX]
static bool layout_valid(const PmLayout& L, int* reach, int* total)
{
    *total = 0;
    for (int i = 0; i < PF_CELLS; i++) if (L.walkable[i]) (*total)++;
    if (*total <= 0) return false;
    *reach = pm_bfs(L.walkable, PM_PLAYER_GX, PM_PLAYER_GZ);
    if (*reach != *total) return false;   // 全可达
    if (!L.walkable[PM_PLAYER_GZ * PF_GRID_N + PM_PLAYER_GX]) return false;
    if (!L.walkable[PM_ENEMY_GZ * PF_GRID_N + PM_ENEMY_GX]) return false;
    for (int k = 0; k < L.count; k++) {
        const PmObstacle& o = L.obs[k];
        // 出生点安全区
        if (o.x10 * o.x10 + o.z10 * o.z10 < PM_CLEAR * PM_CLEAR) return false;
        int dxe = o.x10 - 100, dze = o.z10 - 100;
        if (dxe * dxe + dze * dze < PM_CLEAR * PM_CLEAR) return false;
        // 单格 1×1、格子对齐
        if (o.gw != 1 || o.gh != 1) return false;
        if (o.hx10 != 5 || o.hz10 != 5) return false;
        if (o.x10 != (o.gx0 - 10) * 10 + 5) return false;
        if (o.z10 != (o.gz0 - 10) * 10 + 5) return false;
        // 高度随机 0.5~1.8m
        if (o.sy10 < PM_HEIGHT_MIN || o.sy10 > PM_HEIGHT_MAX) return false;
    }
    return true;
}

int main()
{
    // 用例 1：确定性——相同 seed 生成完全一致的布局
    {
        for (int s = 1; s <= 50; s++) {
            PmLayout a, b;
            pmap_generate_seed((uint32_t)s, &a);
            pmap_generate_seed((uint32_t)s, &b);
            if (a.count != b.count || a.seed != b.seed) {
                CHECK(false, "determinism: same seed -> same layout (count)");
                return 1;
            }
            bool same = true;
            for (int k = 0; k < a.count; k++) {
                const PmObstacle& x = a.obs[k], & y = b.obs[k];
                if (x.x10 != y.x10 || x.z10 != y.z10 || x.hx10 != y.hx10 ||
                    x.hz10 != y.hz10 || x.sy10 != y.sy10 ||
                    x.gx0 != y.gx0 || x.gz0 != y.gz0 || x.gw != y.gw || x.gh != y.gh) { same = false; break; }
            }
            if (!same) { CHECK(false, "determinism: same seed -> same layout (obs)"); return 1; }
        }
        CHECK(true, "determinism: same seed -> same layout (50 seeds)");
    }

    // 用例 2：不同 seed 生成不同布局（抽样验证非全空/非固定）
    {
        bool varied = false;
        PmLayout first, cur;
        pmap_generate_seed(1u, &first);
        for (int s = 2; s <= 30; s++) {
            pmap_generate_seed((uint32_t)s, &cur);
            if (cur.count != first.count) { varied = true; break; }
            bool same = true;
            for (int k = 0; k < cur.count; k++) {
                if (cur.obs[k].x10 != first.obs[k].x10 || cur.obs[k].z10 != first.obs[k].z10) { same = false; break; }
            }
            if (!same) { varied = true; break; }
        }
        CHECK(varied, "variation: different seeds give different layouts");
    }

    // 用例 3：大量 seed 全部通过全可达 + 安全区 + 对齐 + 高度范围校验
    {
        int fail_seed = -1;
        for (int s = 1; s <= 500; s++) {
            PmLayout L;
            pmap_generate_seed((uint32_t)s, &L);
            int reach = 0, total = 0;
            if (!layout_valid(L, &reach, &total)) { fail_seed = s; break; }
        }
        if (fail_seed < 0) CHECK(true, "reachability: 500 seeds fully reachable & valid");
        else {
            char buf[96];
            snprintf(buf, sizeof(buf), "reachability: 500 seeds fully reachable & valid (FAIL at seed %d)", fail_seed);
            CHECK(false, buf);
        }
    }

    // 用例 4：障碍密度与数量——多数 seed 能产出障碍，且数量在 [0, PM_MAX_OBS]
    {
        int nonempty = 0;
        bool ok_count = true;
        for (int s = 1; s <= 100; s++) {
            PmLayout L;
            pmap_generate_seed((uint32_t)s, &L);
            if (L.count > 0) nonempty++;
            if (L.count < 0 || L.count > PM_MAX_OBS) { ok_count = false; break; }
        }
        char buf[96];
        snprintf(buf, sizeof(buf), "coverage: nonempty layout rate %d/100", nonempty);
        CHECK(nonempty >= 80, buf);
        CHECK(ok_count, "count: obstacle count within [0, PM_MAX_OBS] (100 seeds)");
    }

    // 用例 5：波次换图 seed 派生——不同波次 seed 不同，且派生出的布局不同
    {
        const uint32_t base = 12345u;
        bool seeds_distinct = true, layouts_distinct = false;
        PmLayout w1, w2;
        pmap_generate_seed(pmap_wave_seed(base, 1), &w1);
        for (int w = 2; w <= 10; w++) {
            if (pmap_wave_seed(base, w) == pmap_wave_seed(base, 1)) { seeds_distinct = false; break; }
            pmap_generate_seed(pmap_wave_seed(base, w), &w2);
            bool same = (w2.count == w1.count);
            if (same) {
                same = true;
                for (int k = 0; k < w2.count; k++) {
                    if (w2.obs[k].x10 != w1.obs[k].x10 || w2.obs[k].z10 != w1.obs[k].z10) { same = false; break; }
                }
            }
            if (!same) { layouts_distinct = true; break; }
        }
        CHECK(seeds_distinct, "wave seed: pmap_wave_seed differs across waves");
        CHECK(layouts_distinct, "wave seed: different waves produce different layouts");
    }

    printf("\n%d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
