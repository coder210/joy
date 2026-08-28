// 独立程序化地图单元测试：确定性验证 pmap_generate / pm_rasterize / pm_bfs
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

// 布局 → 栅格（供校验）
static void layout_to_walkable(bool walkable[PF_CELLS], const PmLayout& L)
{
    pm_rasterize(walkable, L.obs, L.count);
}

// 校验布局有效性：连通性、覆盖率、安全区、边界
static bool layout_valid(const PmLayout& L, int* reach, int* total)
{
    bool walkable[PF_CELLS];
    layout_to_walkable(walkable, L);
    *total = 0;
    for (int i = 0; i < PF_CELLS; i++) if (walkable[i]) (*total)++;
    bool conn = pm_bfs(walkable, PM_PLAYER_GX, PM_PLAYER_GZ, PM_ENEMY_GX, PM_ENEMY_GZ, reach);
    if (!conn) return false;
    if (*total <= 0 || *reach < *total * 3 / 10) return false;
    for (int k = 0; k < L.count; k++) {
        const PmObstacle& o = L.obs[k];
        // 出生点安全区
        if (o.x10 * o.x10 + o.z10 * o.z10 < PM_CLEAR * PM_CLEAR) return false;
        int dxe = o.x10 - 100, dze = o.z10 - 100;
        if (dxe * dxe + dze * dze < PM_CLEAR * PM_CLEAR) return false;
        // 边界
        if (o.x10 < -PM_REGION || o.x10 > PM_REGION || o.z10 < -PM_REGION || o.z10 > PM_REGION) return false;
        // 起终点格可走
        bool w2[PF_CELLS];
        layout_to_walkable(w2, L);
        if (!w2[PM_PLAYER_GZ * PF_GRID_N + PM_PLAYER_GX]) return false;
        if (!w2[PM_ENEMY_GZ * PF_GRID_N + PM_ENEMY_GX]) return false;
    }
    return true;
}

int main()
{
    // 用例 1：确定性——相同 seed 生成完全一致的布局
    {
        PmLayout a, b;
        for (int s = 1; s <= 50; s++) {
            random_num_p r1 = utils_random_create((uint32_t)s);
            random_num_p r2 = utils_random_create((uint32_t)s);
            pmap_generate(r1, &a);
            pmap_generate(r2, &b);
            utils_random_destroy(r1);
            utils_random_destroy(r2);
            if (a.count != b.count || a.seed != b.seed) {
                CHECK(false, "determinism: same seed -> same layout (count)");
                return 1;
            }
            bool same = true;
            for (int k = 0; k < a.count; k++) {
                const PmObstacle& x = a.obs[k], & y = b.obs[k];
                if (x.x10 != y.x10 || x.z10 != y.z10 || x.hx10 != y.hx10 ||
                    x.hz10 != y.hz10 || x.sy10 != y.sy10 || x.sphere != y.sphere) { same = false; break; }
            }
            if (!same) { CHECK(false, "determinism: same seed -> same layout (obs)"); return 1; }
        }
        CHECK(true, "determinism: same seed -> same layout (50 seeds)");
    }

    // 用例 2：不同 seed 生成不同布局（抽样验证非全空/非固定）
    {
        bool varied = false;
        PmLayout first, cur;
        random_num_p r = utils_random_create(1u);
        pmap_generate(r, &first);
        for (int s = 2; s <= 30; s++) {
            random_num_p rs = utils_random_create((uint32_t)s);
            pmap_generate(rs, &cur);
            utils_random_destroy(rs);
            if (cur.count != first.count) { varied = true; break; }
            bool same = true;
            for (int k = 0; k < cur.count; k++) {
                if (cur.obs[k].x10 != first.obs[k].x10 || cur.obs[k].z10 != first.obs[k].z10) { same = false; break; }
            }
            if (!same) { varied = true; break; }
        }
        utils_random_destroy(r);
        CHECK(varied, "variation: different seeds give different layouts");
    }

    // 用例 3：大量 seed 全部通过连通性 + 覆盖率 + 安全区校验
    {
        int fail_seed = -1;
        for (int s = 1; s <= 500; s++) {
            random_num_p r = utils_random_create((uint32_t)s);
            PmLayout L;
            pmap_generate(r, &L);
            utils_random_destroy(r);
            int reach = 0, total = 0;
            if (!layout_valid(L, &reach, &total)) { fail_seed = s; break; }
        }
        if (fail_seed < 0) CHECK(true, "connectivity: 500 seeds all valid");
        else {
            char buf[96];
            snprintf(buf, sizeof(buf), "connectivity: 500 seeds all valid (FAIL at seed %d)", fail_seed);
            CHECK(false, buf);
        }
    }

    // 用例 4：生成布局非空（正常区域应总能产出障碍）
    {
        int nonempty = 0;
        for (int s = 1; s <= 100; s++) {
            random_num_p r = utils_random_create((uint32_t)s);
            PmLayout L;
            pmap_generate(r, &L);
            utils_random_destroy(r);
            if (L.count > 0) nonempty++;
        }
        char buf[96];
        snprintf(buf, sizeof(buf), "coverage: nonempty layout rate %d/100", nonempty);
        CHECK(nonempty >= 90, buf);
    }

    // 用例 5：障碍数量在合理区间 [1, PM_MAX_OBS]
    {
        bool ok = true;
        for (int s = 1; s <= 200; s++) {
            random_num_p r = utils_random_create((uint32_t)s);
            PmLayout L;
            pmap_generate(r, &L);
            utils_random_destroy(r);
            if (L.count < 1 || L.count > PM_MAX_OBS) { ok = false; break; }
        }
        CHECK(ok, "count: obstacle count within [1, PM_MAX_OBS] (200 seeds)");
    }

    printf("\n%d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
