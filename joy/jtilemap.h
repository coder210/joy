/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jtilemap.h
Description: Tiled 地图数据解析（Lua 导出格式）
Author: ydlc
Version: 1.0
Date: 2026.5.22
History:
*************************************************/
#ifndef JTILEMAP_H
#define JTILEMAP_H

#include <stdint.h>
#include <stdbool.h>
#include "jconfig.h"
#include "external/klib/kvec.h"
#include "external/klib/khash.h"
#include "external/lua/lua.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- 分层类型常量 ---------- */
#define JTILEMAP_LAYER_TILE     0
#define JTILEMAP_LAYER_OBJECT   1
#define JTILEMAP_LAYER_IMAGE    2

/* ---------- 形状常量 ---------- */
#define JTILEMAP_SHAPE_RECT     0
#define JTILEMAP_SHAPE_POLYGON  1
#define JTILEMAP_SHAPE_POLYLINE 2
#define JTILEMAP_SHAPE_ELLIPSE  3
#define JTILEMAP_SHAPE_POINT    4

/* ---------- 动画帧 ---------- */
typedef struct {
    int tileid;         /* tileset 内局部瓦片 ID */
    int duration;       /* 持续时间（毫秒） */
} jtilemap_anim_frame_t;
typedef jtilemap_anim_frame_t* jtilemap_anim_frame_p;

/* ---------- 动画序列 ---------- */
typedef struct {
    kvec_t(jtilemap_anim_frame_t) frames;
    int total_duration;          /* 所有帧总时长（毫秒），缓存加速 */
} jtilemap_anim_t;
typedef jtilemap_anim_t* jtilemap_anim_p;

/* ---------- 图块集 ---------- */
typedef struct {
    char*  name;
    int    firstgid;
    int    tilewidth;
    int    tileheight;
    int    spacing;
    int    margin;
    int    columns;
    int    tilecount;
    char*  image;
    int    imagewidth;
    int    imageheight;

    /* 动画：anims[n] 对应 tileset 内部局部瓦片 id = anims[n].frames[0].tileid（首个帧的 id） */
    kvec_t(jtilemap_anim_t) anims;

    /* 运行时加速：局部瓦片 id -> anims[] 下标，-1 表示无动画 */
    int* tile_anim_map;   /* 数组长度 = tilecount，仅在加载后有效 */
    int  tile_anim_map_alloced;
} jtilemap_tileset_t;
typedef jtilemap_tileset_t* jtilemap_tileset_p;

/* ---------- 对象（碰撞体/实体） ---------- */
typedef struct {
    int    id;
    char*  name;
    char*  type;         /* 用户自定义类型 */
    int    shape;        /* JTILEMAP_SHAPE_* */
    float  x, y;
    float  width, height; /* 矩形有效 */
    float  rotation;
    bool   visible;
} jtilemap_object_t;
typedef jtilemap_object_t* jtilemap_object_p;

/* ---------- 图层基类 ---------- */
typedef struct {
    int   type;          /* JTILEMAP_LAYER_* */
    char* name;
    bool  visible;
    float opacity;
    float offsetx, offsety;
    float parallaxx, parallaxy;
} jtilemap_layer_t;
typedef jtilemap_layer_t* jtilemap_layer_p;

/* ---------- 瓦片图层 ---------- */
typedef struct {
    jtilemap_layer_t base;
    int   width;         /* 瓦片列数 */
    int   height;        /* 瓦片行数 */
    int*  data;          /* 展平 GID 数组，行优先：data[y * width + x] */
} jtilemap_tile_layer_t;
typedef jtilemap_tile_layer_t* jtilemap_tile_layer_p;

/* ---------- 对象图层 ---------- */
typedef struct {
    jtilemap_layer_t base;
    kvec_t(jtilemap_object_t) objects;
} jtilemap_object_layer_t;
typedef jtilemap_object_layer_t* jtilemap_object_layer_p;

/* ---------- 图片图层 ---------- */
typedef struct {
    jtilemap_layer_t base;
    char* image;
    bool  repeatx;
    bool  repeaty;
} jtilemap_image_layer_t;
typedef jtilemap_image_layer_t* jtilemap_image_layer_p;

/* ---------- 运行时动画状态 ---------- */
typedef struct {
    uint32_t start_time;          /* 此瓦片启动时间（ms） */
} jtilemap_anim_state_t;

/* ---------- 主地图 ---------- */
typedef struct {
    char* version;
    char* tiledversion;
    char* orientation;
    char* renderorder;

    int width;          /* 瓦片列数 */
    int height;         /* 瓦片行数 */
    int tilewidth;      /* 默认瓦片宽度 */
    int tileheight;     /* 默认瓦片高度 */

    kvec_t(jtilemap_tileset_t) tilesets;

    /* layers 数组中每一项是 jtilemap_layer_p（实际为 *tile_layer / *object_layer / *image_layer） */
    kvec_t(void*) layers;

    /* 运行时：每加载一个 tile layer 时收集所有有动画的 gid 及启动时间 */
    kvec_t(int)          anim_gids;       /* 有动画的全局 GID */
    kvec_t(jtilemap_anim_state_t) anim_states;
} jtilemap_t;
typedef jtilemap_t* jtilemap_p;

/* ---------- GID 解析结果 ---------- */
typedef struct {
    jtilemap_tileset_p tileset;
    int local_id;               /* tileset 内局部 ID */
    bool has_anim;
    int  anim_idx;              /* tileset->anims[] 下标，仅 has_anim 时有效 */
} jtilemap_gid_resolve_t;

/* ==================== 高级接口（加载/销毁） ==================== */

/* 从 Lua table（Tiled Lua 导出格式）加载地图 */
/* L: Lua 状态, idx: Lua table 在栈上的索引（支持负索引） */
/* 返回新分配的 jtilemap_t，失败返回 NULL */
JOY_API jtilemap_p jtilemap_load_lua(lua_State* L, int idx);

/* 直接加载 Lua 格式的地图文件（内部创建 Lua 状态，外部无需关心 Lua） */
/* filename: map.lua 文件路径 */
/* 返回新分配的 jtilemap_t，失败返回 NULL */
JOY_API jtilemap_p jtilemap_load_file(const char* filename);

/* 释放地图全部资源 */
JOY_API void jtilemap_destroy(jtilemap_p tm);

/* ==================== GID 解析 ==================== */

/* 解析全局 GID → 对应 tileset + 局部 ID */
JOY_API jtilemap_gid_resolve_t jtilemap_resolve_gid(const jtilemap_p tm, int gid);

/* ==================== 纹理坐标计算 ==================== */

/* 
 * 计算指定 GID 在当前时刻的纹理坐标（瓦片在 tileset 纹理图中的像素矩形）
 * tm:       地图
 * gid:      全局瓦片 ID（0 表示空白，此时返回零矩形）
 * time_ms:  当前时间（毫秒），用于驱动动画
 * out_rect: 输出 {x, y, w, h} 像素坐标
 * 返回值：   true 表示有效瓦片，false 表示空白
 */
JOY_API bool jtilemap_get_tile_uv(const jtilemap_p tm, int gid,
                                   uint32_t time_ms, float out_rect[4]);

/*
 * 获取 GID 在当前时刻应显示的局部瓦片 ID（考虑动画）
 * 无动画时返回 local_id == tileset 内局部 ID
 */
JOY_API int jtilemap_get_anim_tileid(const jtilemap_p tm, int gid,
                                      uint32_t time_ms);

/* ==================== 图层查询 ==================== */

/* 获取指定名称的 tile layer，未找到返回 NULL */
JOY_API jtilemap_tile_layer_p jtilemap_find_tile_layer(const jtilemap_p tm,
                                                         const char* name);

/* 获取指定名称的 object layer，未找到返回 NULL */
JOY_API jtilemap_object_layer_p jtilemap_find_object_layer(const jtilemap_p tm,
                                                            const char* name);

/* 获取 tile layer 中 (tx, ty) 处的 GID，越界返回 0 */
JOY_API int jtilemap_get_tile(const jtilemap_tile_layer_p layer,
                               int tx, int ty);

#ifdef __cplusplus
}
#endif

#endif /* !JTILEMAP_H */
