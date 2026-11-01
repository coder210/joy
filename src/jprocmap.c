/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jprocmap.c
Description: 程序化地图生成（BSP 房间 + 门洞版）实现
Author: ydlc / joy
Date: 2026.8.29
History:
        2026.8.29  从 project_proxy3d/shared/systems/procedural_map.h 迁移并参数化。
        所有坐标用"十分之一米"整数；生成结果仅由 seed 决定 → 完全确定性，可用作帧同步依据。
        参数（网格尺寸/出生安全区/障碍密度与高度/簇大小范围）全部集中在 jpm_config_t 中。
*************************************************/

#include "joy.h"

/* ============ 内部辅助 ============ */

static int jpm_rand_mod(random_num_p rng, int mod)
{
        if (mod <= 0) return 0;
        return (int)(utils_random_next(rng) % (uint32_t)mod);
}

/* 格 → 世界坐标（十分之一米）：世界原点落在网格中心附近，与默认 config 完全一致 */
static int jpm_grid_center10(const jpm_config_t* cfg, int g)
{
        return (g - cfg->grid_n / 2) * 10 + 5;
}

/* 该格中心是否落入任意出生安全区 */
static bool jpm_in_spawn_zone(const jpm_config_t* cfg, int gx, int gz)
{
        int x10 = jpm_grid_center10(cfg, gx);
        int z10 = jpm_grid_center10(cfg, gz);
        for (int s = 0; s < cfg->spawn_count; s++) {
                const jpm_spawn_t* sp = &cfg->spawns[s];
                int dx = x10 - sp->cx10, dz = z10 - sp->cz10;
                if (dx * dx + dz * dz < sp->radius10 * sp->radius10) return true;
        }
        return false;
}

/* 整数栅格化（含 pad 外扩）：把我方障碍实体重建为可通行栅格。内部使用 jpm_rasterize 的统一口径 */
static void jpm_rasterize_impl(const jpm_config_t* cfg, bool walkable[JPM_MAX_CELLS],
                               const jpm_obstacle_t* obs, int n)
{
        int grid_n = cfg->grid_n;
        for (int i = 0; i < grid_n * grid_n; i++) walkable[i] = true;

        const int offset10 = cfg->offset10;
        const int pad10    = cfg->pad10;
        for (int k = 0; k < n; k++) {
                const jpm_obstacle_t* o = &obs[k];
                int gx0 = (o->x10 - o->hx10 - pad10 + offset10) / 10;
                int gx1 = (o->x10 + o->hx10 + pad10 + offset10) / 10;
                int gz0 = (o->z10 - o->hz10 - pad10 + offset10) / 10;
                int gz1 = (o->z10 + o->hz10 + pad10 + offset10) / 10;
                if (gx0 < 0) gx0 = 0; if (gx1 > grid_n - 1) gx1 = grid_n - 1;
                if (gz0 < 0) gz0 = 0; if (gz1 > grid_n - 1) gz1 = grid_n - 1;
                for (int gz = gz0; gz <= gz1; gz++)
                        for (int gx = gx0; gx <= gx1; gx++)
                                walkable[gz * grid_n + gx] = false;
        }
}

/* 4 方向 BFS：返回从起点可达的开放格总数（用于连通性校验） */
static int jpm_bfs_impl(const bool walkable[JPM_MAX_CELLS], int grid_n, int sx, int sz)
{
        int queue[JPM_MAX_CELLS];
        int head = 0, tail = 0;
        bool visited[JPM_MAX_CELLS];
        for (int i = 0; i < grid_n * grid_n; i++) visited[i] = false;

        int start = sz * grid_n + sx;
        if (start < 0 || start >= grid_n * grid_n || !walkable[start]) return 0;
        queue[tail++] = start;
        visited[start] = true;
        const int dx4[4] = { 1, -1, 0, 0 };
        const int dz4[4] = { 0, 0, 1, -1 };
        while (head < tail) {
                int cur = queue[head++];
                int cx = cur % grid_n, cz = cur / grid_n;
                for (int d = 0; d < 4; d++) {
                        int nx = cx + dx4[d], nz = cz + dz4[d];
                        if (nx < 0 || nx >= grid_n || nz < 0 || nz >= grid_n) continue;
                        int ni = nz * grid_n + nx;
                        if (!walkable[ni] || visited[ni]) continue;
                        visited[ni] = true;
                        queue[tail++] = ni;
                }
        }
        return tail;
}

/* 连接每一格与所属障碍（重叠格取最小编号，确定性）；开放格为 -1 */
static void jpm_build_cell_mask(const jpm_config_t* cfg, const jpm_obstacle_t* obs, int n,
                                int cellobs[JPM_MAX_CELLS])
{
        int grid_n = cfg->grid_n;
        const int offset10 = cfg->offset10, pad10 = cfg->pad10;
        for (int i = 0; i < grid_n * grid_n; i++) cellobs[i] = -1;
        for (int k = 0; k < n; k++) {
                const jpm_obstacle_t* o = &obs[k];
                int x0 = (o->x10 - o->hx10 - pad10 + offset10) / 10;
                int x1 = (o->x10 + o->hx10 + pad10 + offset10) / 10;
                int z0 = (o->z10 - o->hz10 - pad10 + offset10) / 10;
                int z1 = (o->z10 + o->hz10 + pad10 + offset10) / 10;
                if (x0 < 0) x0 = 0; if (x1 > grid_n - 1) x1 = grid_n - 1;
                if (z0 < 0) z0 = 0; if (z1 > grid_n - 1) z1 = grid_n - 1;
                for (int zz = z0; zz <= z1; zz++)
                        for (int xx = x0; xx <= x1; xx++) {
                                int i = zz * grid_n + xx;
                                if (cellobs[i] == -1) cellobs[i] = k;
                        }
        }
}

/* 连通性修复：从玩家主区穿越障碍做多源 BFS，找到连接主区与"封闭口袋"的最小
 * 卷积障碍链，把链上的障碍整块移除，直到所有开放格均与玩家可达。
 * 必须整块移除障碍实体（非仅凿开格子图）——游戏运行时可通行网格是从障碍碰撞体
 * 重新栅格化的（不读本布局 walkable），只有真删障碍实体，游戏网格与修复结果才一致。
 * 完全确定性：只由布局决定，无随机；每轮至少移除 1 块，单调收敛、必然终止。 */
static void jpm_repair_connectivity(const jpm_config_t* cfg, jpm_layout_t* out)
{
        int grid_n = cfg->grid_n;
        const int dx4[4] = { 1, -1, 0, 0 };
        const int dz4[4] = { 0, 0, 1, -1 };
        const int sph = grid_n / 2;   /* 玩家主格（网格中心） */

        for (;;) {
                jpm_rasterize_impl(cfg, out->walkable, out->obs, out->count);
                bool* w = out->walkable;
                int cells = grid_n * grid_n;

                /* 主连通区（开放格，从玩家 BFS） */
                bool inmain[JPM_MAX_CELLS];
                for (int i = 0; i < cells; i++) inmain[i] = false;
                {
                        int mq[JPM_MAX_CELLS], mh = 0, mt = 0;
                        int s = sph * grid_n + sph;
                        if (w[s]) { mq[mt++] = s; inmain[s] = true; }
                        while (mh < mt) {
                                int c = mq[mh++], cx = c % grid_n, cz = c / grid_n;
                                for (int d = 0; d < 4; d++) {
                                        int nx = cx + dx4[d], nz = cz + dz4[d];
                                        if (nx < 0 || nx >= grid_n || nz < 0 || nz >= grid_n) continue;
                                        int ni = nz * grid_n + nx;
                                        if (!w[ni] || inmain[ni]) continue;
                                        inmain[ni] = true; mq[mt++] = ni;
                                }
                        }
                }

                /* 找一个未连通的开放格（某个封闭口袋的入口） */
                int target = -1;
                for (int i = 0; i < cells; i++) if (w[i] && !inmain[i]) { target = i; break; }
                if (target < 0) break;   /* 全部连通 */

                /* 每格属于哪块障碍 */
                int cellobs[JPM_MAX_CELLS];
                jpm_build_cell_mask(cfg, out->obs, out->count, cellobs);

                /* 多源 BFS：从主区开放格出发穿越所有格，parent 记录来向 */
                int parent[JPM_MAX_CELLS];
                for (int i = 0; i < cells; i++) parent[i] = inmain[i] ? -1 : -2;
                {
                        int q[JPM_MAX_CELLS], h = 0, t = 0;
                        for (int i = 0; i < cells; i++) if (inmain[i]) q[t++] = i;
                        while (h < t) {
                                int c = q[h++], cx = c % grid_n, cz = c / grid_n;
                                for (int d = 0; d < 4; d++) {
                                        int nx = cx + dx4[d], nz = cz + dz4[d];
                                        if (nx < 0 || nx >= grid_n || nz < 0 || nz >= grid_n) continue;
                                        int ni = nz * grid_n + nx;
                                        if (parent[ni] != -2) continue;
                                        parent[ni] = c; q[t++] = ni;
                                }
                        }
                }

                /* 沿 parent 从 target 回凿：链上穿过的障碍整块标记移除 */
                bool rm[JPM_MAX_OBS];
                for (int k = 0; k < out->count; k++) rm[k] = false;
                for (int c = target; c != -1 && parent[c] != -1; c = parent[c])
                        if (cellobs[c] >= 0) rm[cellobs[c]] = true;

                int n = 0;
                for (int k = 0; k < out->count; k++) if (!rm[k]) out->obs[n++] = out->obs[k];
                if (n == out->count) {
                        /* 防御：本轮未移除任何障碍（异常情况），强制移除 target 所在障碍避免死循环 */
                        int fo = cellobs[target];
                        if (fo >= 0) {
                                int n2 = 0;
                                for (int k = 0; k < out->count; k++) if (k != fo) out->obs[n2++] = out->obs[k];
                                n = n2;
                        } else break;
                }
                out->count = n;
        }
}

/* ============ BSP 房间切分 ============ */

#define BSP_MAX_WALL 64   /* 最多隔墙数（水平/竖直），grid_n<=64 足够 */

/* 每道隔墙定义为：一条固定列/行的 1 格厚墙，沿另一轴在 [a,b] 内延伸。
 * 切分时该列/行不再分给两个子区域，因此祖先墙与后代墙永不交叉，
 * 门洞可安全地凿在任意位置而不被其它墙挡住。 */
static int  bsp_wall_axis[BSP_MAX_WALL]; /* 0=竖直墙(固定列)，1=水平墙(固定行) */
static int  bsp_wall_fix[BSP_MAX_WALL];  /* 固定列或行的下标 */
static int  bsp_wall_a[BSP_MAX_WALL];    /* 沿另一轴的起点 */
static int  bsp_wall_b[BSP_MAX_WALL];    /* 沿另一轴的终点 */
static int  bsp_wall_count;

/* 递归切分矩形 [x0..x1]×[y0..y1]。min_size 为房间最小边长，深度受 max_depth 限制。
 * 每刀优先切更长的一侧，并在两子区域间留一条隔墙。完全确定性（仅依赖 rng）。 */
static void bsp_split_rec(random_num_p rng, int min_size, int depth, int max_depth,
                          int x0, int y0, int x1, int y1)
{
        int w = x1 - x0 + 1, h = y1 - y0 + 1;
        bool can_split_col = w >= 2 * min_size;   /* 可竖切（产生竖直隔墙） */
        bool can_split_row = h >= 2 * min_size;   /* 可横切（产生水平隔墙） */
        if (!can_split_col && !can_split_row) return;   /* 叶子：这就是一个房间 */
        if (depth >= max_depth) return;

        /* 取向：优先切更长的一侧，使房间更方正 */
        int axis;
        if (can_split_col && can_split_row) axis = (w >= h) ? 0 : 1;
        else axis = can_split_col ? 0 : 1;

        if (axis == 0) {
                int lo = x0 + min_size, hi = x1 - min_size;
                int bc = lo + jpm_rand_mod(rng, hi - lo + 1);
                if (bsp_wall_count < BSP_MAX_WALL) {
                        bsp_wall_axis[bsp_wall_count] = 0;
                        bsp_wall_fix[bsp_wall_count] = bc;
                        bsp_wall_a[bsp_wall_count] = y0;
                        bsp_wall_b[bsp_wall_count] = y1;
                        bsp_wall_count++;
                }
                bsp_split_rec(rng, min_size, depth + 1, max_depth, x0, y0, bc - 1, y1);
                bsp_split_rec(rng, min_size, depth + 1, max_depth, bc + 1, y0, x1, y1);
        } else {
                int lo = y0 + min_size, hi = y1 - min_size;
                int br = lo + jpm_rand_mod(rng, hi - lo + 1);
                if (bsp_wall_count < BSP_MAX_WALL) {
                        bsp_wall_axis[bsp_wall_count] = 1;
                        bsp_wall_fix[bsp_wall_count] = br;
                        bsp_wall_a[bsp_wall_count] = x0;
                        bsp_wall_b[bsp_wall_count] = x1;
                        bsp_wall_count++;
                }
                bsp_split_rec(rng, min_size, depth + 1, max_depth, x0, y0, x1, br - 1);
                bsp_split_rec(rng, min_size, depth + 1, max_depth, x0, br + 1, x1, y1);
        }
}

/* ============ 公开接口 ============ */

void jpm_config_default(jpm_config_t* cfg)
{
        cfg->grid_n       = 20;
        cfg->offset10     = 100;   /* 10m */
        cfg->pad10        = 6;     /* 0.6m，与寻路外扩一致 */
        cfg->density      = 25;
        cfg->max_obs      = JPM_MAX_OBS;
        cfg->height_min10 = 5;     /* 0.5m */
        cfg->height_max10 = 18;    /* 1.8m */
        cfg->cluster_min  = 5;
        cfg->cluster_max  = 10;
        cfg->spawn_count  = 0;
        /* 默认追加两组与游戏一致的安全区：玩家 (0,0) + 敌人 (10m,10m)，半径 2.2m */
        jpm_config_add_spawn(cfg, 0, 0, 22);
        jpm_config_add_spawn(cfg, 100, 100, 22);
}

void jpm_config_add_spawn(jpm_config_p cfg, int cx10, int cz10, int radius10)
{
        if (cfg->spawn_count >= JPM_MAX_SPAWN) return;
        cfg->spawns[cfg->spawn_count].cx10 = cx10;
        cfg->spawns[cfg->spawn_count].cz10 = cz10;
        cfg->spawns[cfg->spawn_count].radius10 = radius10;
        cfg->spawn_count++;
}

uint32_t jpm_wave_seed(uint32_t base_seed, int wave)
{
        return base_seed + (uint32_t)wave * 1000003u;
}

void jpm_rasterize(const jpm_config_t* cfg, bool walkable[JPM_MAX_CELLS],
                   const jpm_obstacle_t* obs, int n)
{
        jpm_rasterize_impl(cfg, walkable, obs, n);
}

int jpm_bfs(const bool walkable[JPM_MAX_CELLS], int grid_n, int sx, int sz)
{
        return jpm_bfs_impl(walkable, grid_n, sx, sz);
}

void jpm_generate_seed(const jpm_config_t* cfg, uint32_t seed, jpm_layout_t* out)
{
        int grid_n = cfg->grid_n;
        const int min_size = 5;   /* 房间最小边长（格） */
        random_num_p rng = utils_random_create(seed);
        out->seed = (int)seed;
        out->count = 0;

        if (grid_n >= 2 * min_size) {
                int max_depth = grid_n <= 24 ? 4 : 5;
                bsp_wall_count = 0;

                /* 1. BSP 递归切分：每刀在两子区域之间留一道 1 格厚隔墙 */
                bsp_split_rec(rng, min_size, 0, max_depth, 0, 0, grid_n - 1, grid_n - 1);

                /* 2. 把隔墙切成障碍实体：每道墙凿一个门洞（连通相邻房间），
                 *    并跳过落在出生安全区的格（安全区天然形成开口）。 */
                int n = 0;
                const int door_w = 3;   /* 门洞宽（格）≥3，抵消两侧障碍 0.6m 外扩后仍有净通道 */
                for (int w = 0; w < bsp_wall_count && n < cfg->max_obs; w++) {
                        int a = bsp_wall_a[w], b = bsp_wall_b[w];
                        int span = b - a + 1;
                        int door = span >= door_w ? a + jpm_rand_mod(rng, span - door_w + 1) : -1;

                        if (bsp_wall_axis[w] == 0) {
                                /* 竖直隔墙：固定列 fx，沿行延伸 */
                                int fx = bsp_wall_fix[w];
                                int r = a;
                                while (r <= b && n < cfg->max_obs) {
                                        if ((door >= 0 && r >= door && r < door + door_w)
                                            || jpm_in_spawn_zone(cfg, fx, r)) { r++; continue; }
                                        int r0 = r;
                                        while (r <= b && !((door >= 0 && r >= door && r < door + door_w))
                                               && !jpm_in_spawn_zone(cfg, fx, r)) r++;
                                        int r1 = r - 1;
                                        int zmn = (r0 - grid_n / 2) * 10;
                                        int zmx = (r1 - grid_n / 2 + 1) * 10;
                                        jpm_obstacle_t* o = &out->obs[n++];
                                        o->gx0 = fx; o->gz0 = r0; o->gw = 1; o->gh = r1 - r0 + 1;
                                        o->x10  = jpm_grid_center10(cfg, fx);
                                        o->z10  = (zmn + zmx) / 2;
                                        o->hx10 = 5;
                                        o->hz10 = (zmx - zmn) / 2;
                                        o->sy10 = cfg->height_min10 + jpm_rand_mod(rng,
                                                cfg->height_max10 - cfg->height_min10 + 1);
                                }
                        } else {
                                /* 水平隔墙：固定行 fr，沿列延伸 */
                                int fr = bsp_wall_fix[w];
                                int c = a;
                                while (c <= b && n < cfg->max_obs) {
                                        if ((door >= 0 && c >= door && c < door + door_w)
                                            || jpm_in_spawn_zone(cfg, c, fr)) { c++; continue; }
                                        int c0 = c;
                                        while (c <= b && !((door >= 0 && c >= door && c < door + door_w))
                                               && !jpm_in_spawn_zone(cfg, c, fr)) c++;
                                        int c1 = c - 1;
                                        int xmn = (c0 - grid_n / 2) * 10;
                                        int xmx = (c1 - grid_n / 2 + 1) * 10;
                                        jpm_obstacle_t* o = &out->obs[n++];
                                        o->gx0 = c0; o->gz0 = fr; o->gw = c1 - c0 + 1; o->gh = 1;
                                        o->x10  = (xmn + xmx) / 2;
                                        o->z10  = jpm_grid_center10(cfg, fr);
                                        o->hx10 = (xmx - xmn) / 2;
                                        o->hz10 = 5;
                                        o->sy10 = cfg->height_min10 + jpm_rand_mod(rng,
                                                cfg->height_max10 - cfg->height_min10 + 1);
                                }
                        }
                }
                out->count = n;
        }

        /* 3. 连通性兜底：BSP 门洞已保证全连通，此步仅作安全网 */
        jpm_repair_connectivity(cfg, out);

        utils_random_destroy(rng);
}

void jpm_generate(const jpm_config_t* cfg, random_num_p rng, jpm_layout_t* out)
{
        uint32_t seed = utils_random_next(rng);
        jpm_generate_seed(cfg, seed, out);
}

int jpm_sample_open_tiles(random_num_p rng, const jpm_config_t* cfg,
                          const jpm_layout_t* L, int count, int min_dist10,
                          int* out_tiles)
{
        int grid_n = cfg->grid_n;
        /* 以玩家安全区（spawns[0]）圆心作为距离基准，缺省回退到网格中心 */
        int basex = grid_n / 2 * 10;
        int basez = grid_n / 2 * 10;
        if (cfg->spawn_count > 0) { basex = cfg->spawns[0].cx10; basez = cfg->spawns[0].cz10; }

        int open[JPM_MAX_CELLS], n = 0;
        for (int gz = 0; gz < grid_n; gz++) {
                for (int gx = 0; gx < grid_n; gx++) {
                        int i = gz * grid_n + gx;
                        if (!L->walkable[i]) continue;
                        int x10 = jpm_grid_center10(cfg, gx);
                        int z10 = jpm_grid_center10(cfg, gz);
                        int dx = x10 - basex, dz = z10 - basez;
                        if (dx * dx + dz * dz < min_dist10 * min_dist10) continue;   /* 远离玩家 */
                        open[n++] = i;
                }
        }
        /* Fisher-Yates 洗牌，取前 count 个 */
        for (int i = n - 1; i > 0 && count > 0; i--) {
                int j = jpm_rand_mod(rng, i + 1);
                int t = open[i]; open[i] = open[j]; open[j] = t;
        }
        int take = n < count ? n : count;
        for (int i = 0; i < take; i++) out_tiles[i] = open[i];
        return take;
}