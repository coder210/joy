// 导出 pmap 生成的地图网格（用于可视化）：
//   行 = Z, 列 = X。字符含义（先障碍格再可通行格，分两组打印）：
//   布局占用格:  #=障碍实体格  .=空地
//   可通行格(带0.6m外扩):  .=可通行(敌人可走)  M=方块+外扩 占
// 用法: dump_map <seed>
// 说明: 手动用 cl 编译本文件时加 /utf-8（源码含中文注释，避免 MSVC 按系统代码页误读）：
//   cl /utf-8 /std:c++17 /EHsc /I e:\c++\joy /I e:\c++\joy\SDL\include ^
//       /I e:\c++\joy\project_proxy3d /I e:\c++\joy\project_proxy3d\shared ^
//       dump_map.cpp /link /LIBPATH:e:\c++\joy\build\Debug joy.lib
#include <cstdio>
#include <cstdlib>
#include <joy/joy.h>
#include "../project_proxy3d/shared/systems/procedural_map.h"

int main(int argc, char** argv)
{
    uint32_t seed = argc > 1 ? (uint32_t)atoi(argv[1]) : 1u;
    PmLayout L;
    pmap_generate_seed(seed, &L);

    printf("seed=%d count=%d\n", L.seed, L.count);

    // 布局占用格（障碍实体中心格）
    char occ[PF_GRID_N][PF_GRID_N];
    for (int z = 0; z < PF_GRID_N; z++) for (int x = 0; x < PF_GRID_N; x++) occ[z][x] = '.';
    for (int k = 0; k < L.count; k++) {
        int x = L.obs[k].gx0, z = L.obs[k].gz0;
        occ[z][x] = '#';
    }
    printf("---layout-occupied---\n");
    for (int z = 0; z < PF_GRID_N; z++) { for (int x = 0; x < PF_GRID_N; x++) putchar(occ[z][x]); putchar('\n'); }

    // 可通行格（敌人寻路实际使用）
    printf("---walkable---\n");
    for (int z = 0; z < PF_GRID_N; z++) { for (int x = 0; x < PF_GRID_N; x++)
        putchar(L.walkable[z * PF_GRID_N + x] ? '.' : '#'); putchar('\n'); }
    return 0;
}