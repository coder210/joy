#ifndef MAP_DEF_H
#define MAP_DEF_H

// ============================================================
// 地图布局定义（服务端和客户端共享，保证初始条件一致）
// 单位：米，1 格 = 1m，原点 (0,0) 在左上角，共 MAP_W x MAP_H 格
//
// 字符含义：
//   '#'  树篱墙（碰撞）
//   'T'  树    （碰撞）
//   'R'  石头  （碰撞）
//   '~'  水    （碰撞）
//   'g'  草丛  （装饰，不碰撞）
//   'f'  花    （装饰，不碰撞）
//   'p'  小路  （装饰，不碰撞）
//   '.'  地面  （可通行）
// ============================================================

inline constexpr int MAP_W = 30;
inline constexpr int MAP_H = 20;

inline const char* MAP_LAYOUT[MAP_H] = {
    "##############################",
    "#...pp...........~~~~........#",
    "#..ppp....TT.....~~~~...RR...#",
    "#..pp....TTT......~~......RR.#",
    "#........TTT......~~.........#",
    "#..gg......TT................#",
    "#..gg.......T.....~~~........#",
    "#...........T....~~~.........#",
    "#..RR..............~~~..gg...#",
    "#..RR.............~~~..gggg..#",
    "#..TT............~~~~....gg..#",
    "#..TT....RR.......~~~~.......#",
    "#.......RR..........RR....T..#",
    "#.....gg.............RR..TT..#",
    "#...gg......TT..TT...........#",
    "#..........TT..TT........RR..#",
    "#....pp..........T......RR...#",
    "#...ppp....~~~~..T...........#",
    "#...pp....~~~~...............#",
    "##############################",
};

// 地形类型枚举（渲染用）
enum MapTileType {
    MAP_TILE_EMPTY = 0,
    MAP_TILE_WALL,    // '#'
    MAP_TILE_TREE,    // 'T'
    MAP_TILE_ROCK,    // 'R'
    MAP_TILE_WATER,   // '~'
    MAP_TILE_GRASS,   // 'g'
    MAP_TILE_FLOWER,  // 'f'
    MAP_TILE_PATH,    // 'p'
};

// 字符 -> 地形类型
static inline int map_tile_type(char c)
{
    switch (c) {
    case '#': return MAP_TILE_WALL;
    case 'T': return MAP_TILE_TREE;
    case 'R': return MAP_TILE_ROCK;
    case '~': return MAP_TILE_WATER;
    case 'g': return MAP_TILE_GRASS;
    case 'f': return MAP_TILE_FLOWER;
    case 'p': return MAP_TILE_PATH;
    default:  return MAP_TILE_EMPTY;
    }
}

// 获取指定格子的地形字符（越界一律视为墙）
static inline char map_tile_at(int x, int y)
{
    if (x < 0 || y < 0 || x >= MAP_W || y >= MAP_H) return '#';
    return MAP_LAYOUT[y][x];
}

// 该地形是否参与碰撞
static inline bool map_is_collidable(char c)
{
    return c == '#' || c == 'T' || c == 'R' || c == '~';
}

#endif
