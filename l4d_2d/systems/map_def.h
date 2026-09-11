#ifndef MAP_DEF_H
#define MAP_DEF_H

#include <joy/joy.h>
#include <cstdio>
#include "level_sources.h"

// Tiled 工程约定：瓦片尺寸(px)/格，对象层名 / 地形图层名，GID=1 为墙
static constexpr int MAP_TILE_PX = 16;
static constexpr const char* MAP_TILE_LAYER = "terrain";
static constexpr const char* MAP_SPAWN_LAYER = "spawn";

// 诊断日志（定义见文件末尾）
static void map_log_load(const char* source, int level);

// ============================================================
// 地图布局定义 —— 与 3D 共用手绘关卡（project_proxy3d/shared/systems/level_maps.h）
// 每关 64×64 ASCII 网格，'#' 墙(碰撞)、'.' 地(可通行)、'P' 玩家出生、'E' 敌人生成。
// 由 map_load_level 在游戏场景 on_load 时装入运行时网格 g_map_grid；
// 服务端/客户端解析同一关卡 → 逐格一致，可作帧同步依据。
// ============================================================

inline constexpr int MAP_W = 64;
inline constexpr int MAP_H = 64;
inline constexpr int LEVEL_COUNT = LEVEL_SOURCES_COUNT;

// 地形类型枚举（渲染用；关卡网格仅含墙/地，树石水均为保留类型）
enum MapTileType {
    MAP_TILE_EMPTY = 0,
    MAP_TILE_WALL,    // '#' 或 GID=1
    MAP_TILE_TREE,    // 保留（不再由关卡生成）
    MAP_TILE_ROCK,
    MAP_TILE_WATER,
    MAP_TILE_GRASS,
    MAP_TILE_FLOWER,
    MAP_TILE_PATH,
};

// 运行时网格 + 出生信息（由 map_load_level 填充）
inline char g_map_grid[MAP_H][MAP_W];  // 保持兼容 ASCII，jtilemap 也写到这里

struct MapLevelInfo {
    int level = -1;
    int player_cell = -1;      // 'P' / spawn 点格子索引 (y*MAP_W + x)，-1=缺失
    int enemy_cells[256];      // 'E' / enemy 格子索引（行主序扫描，双端同序）
    int enemy_n = 0;
    jtilemap_p  tilemap = nullptr;      // jtilemap 句柄，由 jtilemap_load_file 持有
    jtilemap_render_p tilemap_render = nullptr; // 渲染句柄
};
inline MapLevelInfo g_map_info;

// 从 spawn 对象层读取出生/敌人生成点（对象像素坐标 → 格）
static void map_tm_spawn_cb(jtilemap_object_p obj, void* arg)
{
    (void)arg;
    if (!obj || !obj->type) return;
    float cx = obj->x + obj->width * 0.5f;
    float cy = obj->y + obj->height * 0.5f;
    int col = (int)(cx / (float)MAP_TILE_PX);
    int row = (int)(cy / (float)MAP_TILE_PX);
    if (col < 0) col = 0; if (col >= MAP_W) col = MAP_W - 1;
    if (row < 0) row = 0; if (row >= MAP_H) row = MAP_H - 1;
    int cell = row * MAP_W + col;
    if (strcmp(obj->type, "player") == 0) {
        g_map_info.player_cell = cell;
    } else if (strcmp(obj->type, "enemy") == 0) {
        if (g_map_info.enemy_n < 256)
            g_map_info.enemy_cells[g_map_info.enemy_n++] = cell;
    }
}

// 探测一个可读的关卡 .lua 路径
static bool map_tm_probe_path(char* out, size_t out_sz, int level, const char* base)
{
    snprintf(out, out_sz, "%slevel%d.lua", base, level);
    FILE* f = std::fopen(out, "rb");
    if (f) { std::fclose(f); return true; }
    return false;
}

// 用 jtilemap 从 Tiled .lua 装载关卡；成功返回 true，失败回退 ASCII。
static bool map_load_from_tilemap(int level)
{
    char path[256];
    const char* bases[] = {
        "levels/",
        "l4d_2d/levels/",
        "../l4d_2d/levels/",
    };
    const char* use = nullptr;
    for (const char* b : bases) {
        if (map_tm_probe_path(path, sizeof(path), level, b)) { use = b; break; }
    }
    if (!use) return false;

    jtilemap_p tm = jtilemap_load_file(path);
    if (!tm) return false;

    jtilemap_tile_layer_p tl = jtilemap_find_tile_layer(tm, MAP_TILE_LAYER);
    if (!tl) { jtilemap_destroy(tm); return false; }

    for (int r = 0; r < MAP_H; r++)
        for (int c = 0; c < MAP_W; c++)
            g_map_grid[r][c] = '.';   // 默认空（可通行）
    for (int y = 0; y < MAP_H; y++) {
        // 每行 width 格；直接用 get_tile 逐格读
        for (int x = 0; x < MAP_W; x++) {
            int gid = jtilemap_get_tile(tl, x, y);
            if (gid != 0) g_map_grid[y][x] = '#';
            else          g_map_grid[y][x] = '.';
        }
    }

    // spawn 对象层 → 出生/敌人点
    g_map_info.player_cell = -1;
    g_map_info.enemy_n = 0;
    jtilemap_object_layer_p ol = jtilemap_find_object_layer(tm, MAP_SPAWN_LAYER);
    if (ol) jtilemap_object_layer_foreach(ol, map_tm_spawn_cb, nullptr);
    if (g_map_info.player_cell < 0) { jtilemap_destroy(tm); return false; }

    g_map_info.tilemap = tm;   // 交给渲染层复用；切关时由调用方销毁
    return true;
}

static inline void map_load_level(int level)
{
    if (level < 1 || level > LEVEL_COUNT) level = 1;
    if (g_map_info.level == level) return;

    // 切关时释放上一关的 jtilemap
    if (g_map_info.tilemap) { jtilemap_destroy(g_map_info.tilemap); g_map_info.tilemap = nullptr; }

    g_map_info = {};                    // 清零（enemy_cells 归零）
    g_map_info.level = level;

    if (map_load_from_tilemap(level)) { map_log_load("tilemap", level); return; }  // 优先 Tiled .lua 数据源

    // 回退：编译期内嵌 ASCII
    const char* src = LEVEL_SRCS[level - 1];
    for (int r = 0; r < MAP_H; r++)
        for (int c = 0; c < MAP_W; c++)
            g_map_grid[r][c] = '.';

    int r = 0, c = 0;
    for (const char* p = src; *p && r < MAP_H; ++p) {
        char ch = *p;
        if (ch == '\n') { r++; c = 0; continue; }
        if (c >= MAP_W) { c++; continue; }   // 忽略行内多余字符（容错）
        g_map_grid[r][c] = (ch == '#') ? '#' : '.';
        if (ch == 'P') {
            g_map_info.player_cell = r * MAP_W + c;
        } else if (ch == 'E') {
            if (g_map_info.enemy_n < 256)
                g_map_info.enemy_cells[g_map_info.enemy_n++] = r * MAP_W + c;
        }
        c++;
    }
    map_log_load("ascii", level);
}

// 获取指定格子的地形字符（越界一律视为墙）
static inline char map_grid_at(int x, int y)
{
    if (x < 0 || y < 0 || x >= MAP_W || y >= MAP_H) return '#';
    return g_map_grid[y][x];
}
// 兼容旧名
static inline char map_tile_at(int x, int y) { return map_grid_at(x, y); }

static inline int map_tile_type(char c)
{
    switch (c) {
    case '#': return MAP_TILE_WALL;
    default:  return MAP_TILE_EMPTY;
    }
}

// 该地形是否参与碰撞（关卡中仅墙碰撞）
static inline bool map_is_collidable(char c)
{
    return c == '#';
}

// 日志：诊断地图加载结果写到 map_load_debug.log
static void map_log_load(const char* source, int level)
{
    FILE* f = std::fopen("map_load_debug.log", "a");
    if (!f) return;
    fprintf(f, "[%s] level=%d enemy_n=%d player_cell=%d\n",
        source, level, g_map_info.enemy_n, g_map_info.player_cell);
    if (g_map_info.player_cell >= 0) {
        int cx = g_map_info.player_cell % MAP_W;
        int cy = g_map_info.player_cell / MAP_W;
        int walls = 0;
        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++)
                walls += map_is_collidable(map_grid_at(cx + dx, cy + dy));
        fprintf(f, " => player @(%d,%d) 3x3 walls=%d\n", cx, cy, walls);
    }
    int n_walls = 0;
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++)
            n_walls += map_is_collidable(map_grid_at(x, y));
    fprintf(f, " => total walls: %d\n\n", n_walls);
    fclose(f);
}

// 玩家出生点：'P' 格中心（世界米坐标），缺失回退 (0.5,0.5)
static inline void map_player_spawn(int level, fp_t* fx, fp_t* fy)
{
    if (g_map_info.level != level) map_load_level(level);
    if (g_map_info.player_cell < 0) { *fx = fp_from_float(0.5f); *fy = fp_from_float(0.5f); return; }
    *fx = fp_from_float((float)(g_map_info.player_cell % MAP_W) + 0.5f);
    *fy = fp_from_float((float)(g_map_info.player_cell / MAP_W) + 0.5f);
}

#endif