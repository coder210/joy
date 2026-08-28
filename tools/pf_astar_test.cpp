// 独立 A* 寻路单元测试：确定性验证 pathfind_astar / pf_build_walkable
// 编译（MSVC）：
//   cl /std:c++17 /EHsc /I D:\github\joy /I D:\github\joy\SDL\include ^
//       /I D:\github\joy\project_proxy3d /I D:\github\joy\project_proxy3d\shared ^
//       pf_astar_test.cpp
// 运行：pf_astar_test.exe   → 输出每个用例 PASS/FAIL

#include <cstdio>
#include <cstdlib>
#include <joy/joy.h>
#include "../project_proxy3d/entt.hpp"
#include "../project_proxy3d/shared/systems/pathfinding_system.h"

// 用真实 obstacle 数据源（spawn_def 暂不引入，直接在本文件构造障碍）
// 注意：以下直接使用 ECS 组件构造，pf_build_walkable 依赖
// StaticMeshComponent + ColliderComponent + LogicPositionComponent 三件套。

static int g_pass = 0, g_fail = 0;

#define CHECK(cond, name) do { \
    if (cond) { g_pass++; printf("PASS  %s\n", name); } \
    else      { g_fail++; printf("FAIL  %s\n", name); } \
} while (0)

static void add_box(entt::registry& r, float x, float z, float sx, float sz)
{
    entt::entity e = r.create();
    r.emplace<StaticMeshComponent>(e);
    ColliderComponent col;
    col.type = ColliderComponent::BOX;
    col.half_size = { fp_from_float(sx * 0.5f), fp_from_float(0.5f), fp_from_float(sz * 0.5f) };
    r.emplace<ColliderComponent>(e, col);
    r.emplace<LogicPositionComponent>(e, fp_from_float(x), fp_from_float(0.5f), fp_from_float(z));
}

// 校验路径：所有格可行 + 相邻性
static bool path_valid(const GlobalState::AstarWork& work, const int* path, int len)
{
    for (int i = 0; i < len; i++) {
        int c = path[i];
        if (c < 0 || c >= PF_CELLS) return false;
        if (!work.walkable[c]) return false;
        if (i > 0) {
            int pc = path[i - 1];
            int dx = (c % PF_GRID_N) - (pc % PF_GRID_N);
            int dz = (c / PF_GRID_N) - (pc / PF_GRID_N);
            if (dx * dx + dz * dz != 1) return false;
        }
    }
    return true;
}

int main()
{
    {
        // 用例 1：空地直线。无障碍，(0,0)→(19,19)，4 方向最短 = 38 步
        entt::registry r;
        r.ctx().emplace<GlobalState>();
        auto& work = r.ctx().get<GlobalState>().astar_work;
        int path[512];
        int len = pathfind_astar(work, r, 0, 0, 19, 19, path, 512);
        CHECK(len == 38, "open field path length == 38");
        CHECK(path_valid(work, path, len), "open field path all walkable+adjacent");
        CHECK(path[len - 1] == 19 * PF_GRID_N + 19, "open field ends at goal");
    }

    {
        // 用例 2：绕障。竖墙 box 位于 (5,0) 尺寸 1.2x3：
        //   覆盖格列 13..16、行 7..12 → 阻挡 (0,10)→(19,10) 直线，必须从下方行 13 绕行
        entt::registry r;
        r.ctx().emplace<GlobalState>();
        auto& work = r.ctx().get<GlobalState>().astar_work;
        add_box(r, 5.0f, 0.0f, 1.2f, 3.0f);   // 墙段
        int path[512];
        int len = pathfind_astar(work, r, 0, 10, 19, 10, path, 512);
        CHECK(len > 0, "wall: path found");
        if (len > 0) {
            CHECK(path_valid(work, path, len), "wall: path all walkable+adjacent");
            // 最优绕行 = 3(下) + 19(横跨) + 3(上) = 25
            CHECK(len == 25, "wall: detour length == 25");
            // 路径不得进入墙的阻挡矩形（列 13..16 且 行 7..12）
            bool hits_wall = false;
            for (int i = 0; i < len; i++) {
                int gx = path[i] % PF_GRID_N, gz = path[i] / PF_GRID_N;
                if (gx >= 13 && gx <= 16 && gz >= 7 && gz <= 12) hits_wall = true;
            }
            CHECK(!hits_wall, "wall: path avoids blocked region");
        }
    }

    {
        // 用例 3：起点越界钳制。start=(20,20)（世界 (10,10)，即敌人出生点）→ 钳到 (19,19)
        entt::registry r;
        r.ctx().emplace<GlobalState>();
        auto& work = r.ctx().get<GlobalState>().astar_work;
        int path[512];
        int len = pathfind_astar(work, r, 20, 20, 0, 0, path, 512);
        CHECK(len > 0, "clamp: out-of-range start still finds path");
        if (len > 0) CHECK(path_valid(work, path, len), "clamp: path valid");
    }

    {
        // 用例 4：终点不可通行回退。玩家站在箱子上（贴障碍），终点格不可行 → 回退最近可行格
        entt::registry r;
        r.ctx().emplace<GlobalState>();
        auto& work = r.ctx().get<GlobalState>().astar_work;
        add_box(r, 3.0f, 3.0f, 1.0f, 1.0f);   // 覆盖格 (12..14, 12..14)
        int path[512];
        int len = pathfind_astar(work, r, 0, 0, 13, 12, path, 512);
        CHECK(len > 0, "unwalkable goal: path found via nearest walkable");
        if (len > 0) {
            CHECK(path_valid(work, path, len), "unwalkable goal: path valid");
            // 终点应落在箱子外扩区之外的可行格
            int last = path[len - 1];
            int lx = last % PF_GRID_N, lz = last / PF_GRID_N;
            CHECK(lx != 13 || lz != 12, "unwalkable goal: substituted goal differs from original");
        }
    }

    {
        // 用例 5：完全封闭（横贯全图的墙把地图切成两半）→ 返回 0
        entt::registry r;
        r.ctx().emplace<GlobalState>();
        auto& work = r.ctx().get<GlobalState>().astar_work;
        add_box(r, 5.0f, 0.0f, 1.2f, 20.0f);   // 全高墙：阻挡所有行的列 13..16
        int path[512];
        int len = pathfind_astar(work, r, 0, 10, 19, 10, path, 512);
        CHECK(len == 0, "enclosed map: returns 0 (no path)");
    }

    printf("\n%d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
