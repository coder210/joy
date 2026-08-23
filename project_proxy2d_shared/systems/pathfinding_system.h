#ifndef PATHFINDING_SYSTEM_H
#define PATHFINDING_SYSTEM_H

#include "../components/pathfinding_component.h"
#include "../components/ai_component.h"
#include "../components/enemy_component.h"
#include "../components/logic_position_component.h"
#include "../components/action_state_component.h"
#include "map_def.h"
#include <joy/jmath.h>
#include <cmath>

struct Context;

// 定点数向下取整转 int（算术右移即 floor(x/2^32)，负数向负无穷），纯整数运算，帧同步确定性。
static inline int fp_floor_to_int(fp_t x)
{
    return (int)(x >> FP_FRACTIONAL_PLACES);
}

/* ============================================================
 * 敌人 A* 寻路系统
 *  - 网格 = MAP_LAYOUT（30x20），障碍 = map_is_collidable 的格子
 *  - ai_system 追击时写 PathComponent(target_gx/gy, dirty=true)
 *  - 本系统节拍重算路径，并沿路径逐格移动（pos 与格子对齐）
 *  - 路径点 = 格子左上角（敌人碰撞盒 1x1，正好占满一格）
 * ============================================================ */

// 地图可通行性预计算缓存（30x20 地图，避免 A* 每次访问格重复调用 map_tile_at/map_is_collidable）
static bool pf_walkable[MAP_W * MAP_H];
static bool pf_walkable_init = false;
static inline void pf_init_walkable()
{
    if (pf_walkable_init) return;
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++)
            pf_walkable[y * MAP_W + x] = !map_is_collidable(map_tile_at(x, y));
    pf_walkable_init = true;
}

// 起点/终点格子坐标 → 路径格子索引序列（不含起点，含终点），返回路径点数
// 性能优化：二叉堆取最小 f（O(log N)）+ in_open 布尔数组 O(1) 查重 + 可通行性查表 O(1)。
// 30 个敌人同时追击时，A* 从 O(N²) 降到 O(N log N)，避免 LogicFPS 因寻路开销暴跌。
static int pathfind_astar(int sx, int sy, int tx, int ty, int* out, int max_out)
{
    pf_init_walkable();
    const int N = MAP_W * MAP_H;

    // 起点/终点合法性
    if (sx < 0 || sy < 0 || sx >= MAP_W || sy >= MAP_H) return 0;
    if (tx < 0 || ty < 0 || tx >= MAP_W || ty >= MAP_H) return 0;
    int start = sy * MAP_W + sx;
    int goal  = ty * MAP_W + tx;
    if (!pf_walkable[start] || !pf_walkable[goal]) return 0;

    static int gscore[N], fscore[N], came[N];
    static bool closed[N], in_open[N];
    static int heap[N];          // 二叉堆：存节点 id，按 fscore 最小堆
    int heap_len = 0;

    // 惰性重置：只重置本次用到的节点（open/closed 集合内的），避免每次全量清零 N 个数组
    // 通过 generation 标记 + 接触列表实现，或直接全量重置（N=600 成本可接受）。
    for (int i = 0; i < N; i++) {
        gscore[i] = 0x3fffffff;
        fscore[i] = 0x3fffffff;
        came[i] = -1;
        closed[i] = false;
        in_open[i] = false;
    }

    gscore[start] = 0;
    fscore[start] = (sx > tx ? sx - tx : tx - sx) + (sy > ty ? sy - ty : ty - sy);

    // 二叉堆上浮
    auto heap_up = [&](int i) {
        while (i > 0) {
            int p = (i - 1) >> 1;
            if (fscore[heap[p]] <= fscore[heap[i]]) break;
            int t = heap[i]; heap[i] = heap[p]; heap[p] = t;
            i = p;
        }
    };
    // 二叉堆下沉
    auto heap_down = [&](int i) {
        for (;;) {
            int l = i * 2 + 1, r = i * 2 + 2, m = i;
            if (l < heap_len && fscore[heap[l]] < fscore[heap[m]]) m = l;
            if (r < heap_len && fscore[heap[r]] < fscore[heap[m]]) m = r;
            if (m == i) break;
            int t = heap[i]; heap[i] = heap[m]; heap[m] = t;
            i = m;
        }
    };

    heap[heap_len++] = start;
    in_open[start] = true;

    const int dirs[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

    while (heap_len > 0) {
        int cur = heap[0];
        in_open[cur] = false;
        heap[0] = heap[--heap_len];
        if (heap_len > 0) heap_down(0);

        if (cur == goal) {
            // 回溯路径（goal → start），再反转
            int len = 0;
            int node = cur;
            while (node != start && node >= 0) {
                if (len < max_out) out[len++] = node;
                node = came[node];
            }
            for (int i = 0, j = len - 1; i < j; i++, j--) {
                int t = out[i]; out[i] = out[j]; out[j] = t;
            }
            return len;
        }

        closed[cur] = true;
        int cx = cur % MAP_W, cy = cur / MAP_W;
        for (int d = 0; d < 4; d++) {
            int nx = cx + dirs[d][0], ny = cy + dirs[d][1];
            if (nx < 0 || ny < 0 || nx >= MAP_W || ny >= MAP_H) continue;
            int ni = ny * MAP_W + nx;
            if (!pf_walkable[ni]) continue;
            if (closed[ni]) continue;

            int ng = gscore[cur] + 1;
            if (ng < gscore[ni]) {
                gscore[ni] = ng;
                fscore[ni] = ng + (nx > tx ? nx - tx : tx - nx) + (ny > ty ? ny - ty : ty - ny);
                came[ni] = cur;
                if (!in_open[ni]) {
                    in_open[ni] = true;
                    heap[heap_len++] = ni;
                    heap_up(heap_len - 1);
                } else {
                    // 已在堆中：f 可能变小，需从当前位置上浮
                    for (int i = 0; i < heap_len; i++) {
                        if (heap[i] == ni) { heap_up(i); break; }
                    }
                }
            }
        }
    }
    return 0; // 无可行路径
}

// 每 30 tick 重算一次路径（目标格子变化时由 ai_system 置 dirty）
static const int PF_RECOMPUTE_INTERVAL = 30;

static void pathfinding_system(entt::registry& registry, Context* ctx, float /*dt*/)
{
    fp_t step = fp_mul(fp_from_float(2.5f), fp_from_float(ctx->FIXED_TIMESTEP));

    auto view = registry.view<EnemyComponent, AIComponent, PathComponent,
                              LogicPositionComponent, ActionStateComponent>();

    for (auto e : view) {
        auto& ai  = view.get<AIComponent>(e);
        auto& pc  = view.get<PathComponent>(e);
        auto& pos = view.get<LogicPositionComponent>(e);

        // 非追击状态：清空路径，移动由 ai_system（巡逻）负责
        if (ai.state != 1) {
            pc.path_len = 0;
            pc.next_idx = 0;
            pc.dirty = false;
            continue;
        }

        // 需要重算路径
        if (pc.dirty) {
            pc.dirty = false;
            if (pc.recompute_cooldown <= 0) {
                pc.recompute_cooldown = PF_RECOMPUTE_INTERVAL;
                int sx = fp_floor_to_int(pos.x);
                int sy = fp_floor_to_int(pos.y);
                pc.path_len = pathfind_astar(sx, sy, pc.target_gx, pc.target_gy,
                                             pc.path, (int)(sizeof(pc.path) / sizeof(pc.path[0])));
                pc.next_idx = 0;
            } else {
                pc.recompute_cooldown--;
            }
        }

        // 沿路径移动
        if (pc.path_len > 0 && pc.next_idx < pc.path_len) {
            int idx = pc.path[pc.next_idx];
            int gx = idx % MAP_W;
            int gy = idx / MAP_W;
            fp_t tx = fp_from_float((float)gx);
            fp_t ty = fp_from_float((float)gy);

            fp_t dx = fp_sub(tx, pos.x);
            fp_t dy = fp_sub(ty, pos.y);

            // 到达当前路径点：对齐并推进
            if (fp_abs(dx) <= step && fp_abs(dy) <= step) {
                pos.x = tx;
                pos.y = ty;
                pc.next_idx++;
            } else {
                // 轴对齐逼近（与 ai_system 巡逻一致）
                if (fp_abs(dx) > fp_abs(dy))
                    pos.x = dx > fp_zero() ? fp_add(pos.x, step) : fp_sub(pos.x, step);
                else
                    pos.y = dy > fp_zero() ? fp_add(pos.y, step) : fp_sub(pos.y, step);
            }
        } else {
            // 路径已走完（追到目标格 / 等待重算冷却）：静止待命。
            // 若不覆盖，ai_system 每 tick 保持 WALK → speed_ratio=1.0 → 静止时也最高速自旋。
            auto& act = view.get<ActionStateComponent>(e);
            act.state = ActionStateComponent::IDLE;
        }
    }
}

#endif // PATHFINDING_SYSTEM_H
