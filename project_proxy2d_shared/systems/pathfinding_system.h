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

/* ============================================================
 * 敌人 A* 寻路系统
 *  - 网格 = MAP_LAYOUT（30x20），障碍 = map_is_collidable 的格子
 *  - ai_system 追击时写 PathComponent(target_gx/gy, dirty=true)
 *  - 本系统节拍重算路径，并沿路径逐格移动（pos 与格子对齐）
 *  - 路径点 = 格子左上角（敌人碰撞盒 1x1，正好占满一格）
 * ============================================================ */

// 起点/终点格子坐标 → 路径格子索引序列（不含起点，含终点），返回路径点数
static int pathfind_astar(int sx, int sy, int tx, int ty, int* out, int max_out)
{
    const int N = MAP_W * MAP_H;
    int gscore[N], fscore[N], came[N];
    bool closed[N];
    int open[N];
    int open_len = 0;

    for (int i = 0; i < N; i++) {
        gscore[i] = 0x3fffffff;
        fscore[i] = 0x3fffffff;
        came[i] = -1;
        closed[i] = false;
    }

    // 起点/终点合法性
    if (sx < 0 || sy < 0 || sx >= MAP_W || sy >= MAP_H) return 0;
    if (map_is_collidable(map_tile_at(sx, sy))) return 0;
    if (tx < 0 || ty < 0 || tx >= MAP_W || ty >= MAP_H) return 0;
    if (map_is_collidable(map_tile_at(tx, ty))) return 0;

    int start = sy * MAP_W + sx;
    int goal  = ty * MAP_W + tx;
    gscore[start] = 0;
    fscore[start] = (sx > tx ? sx - tx : tx - sx) + (sy > ty ? sy - ty : ty - sy);
    open[open_len++] = start;

    const int dirs[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

    while (open_len > 0) {
        // 线性取 f 最小（600 格规模足够）
        int best_i = 0;
        for (int i = 1; i < open_len; i++) {
            if (fscore[open[i]] < fscore[open[best_i]]) best_i = i;
        }
        int cur = open[best_i];
        open[best_i] = open[--open_len];

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
            if (map_is_collidable(map_tile_at(nx, ny))) continue;
            int ni = ny * MAP_W + nx;
            if (closed[ni]) continue;

            int ng = gscore[cur] + 1;
            if (ng < gscore[ni]) {
                gscore[ni] = ng;
                fscore[ni] = ng + (nx > tx ? nx - tx : tx - nx) + (ny > ty ? ny - ty : ty - ny);
                came[ni] = cur;
                bool in_open = false;
                for (int i = 0; i < open_len; i++) {
                    if (open[i] == ni) { in_open = true; break; }
                }
                if (!in_open) open[open_len++] = ni;
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
                int sx = (int)std::floor(fp_to_float(pos.x));
                int sy = (int)std::floor(fp_to_float(pos.y));
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
        }
    }
}

#endif // PATHFINDING_SYSTEM_H
