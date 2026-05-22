/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jtilemap.c
Description: Tiled 地图数据解析实现（Lua 导出格式）
Author: ydlc
Version: 1.0
Date: 2026.5.22
History:
*************************************************/
#include "jtilemap.h"
#include "jcore.h"
#include "jutils.h"
#include "external/lua/lauxlib.h"
#include "external/lua/lualib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/* ==================== 辅助：读取 Lua table 字段 ==================== */

static int lua_get_integer(struct lua_State* L, int idx, const char* key, int def)
{
    int val = def;
    int t = lua_getfield(L, idx, key);
    if (t == LUA_TNUMBER)
        val = (int)lua_tointeger(L, -1);
    lua_pop(L, 1);
    return val;
}

static float lua_get_number(struct lua_State* L, int idx, const char* key, float def)
{
    float val = def;
    int t = lua_getfield(L, idx, key);
    if (t == LUA_TNUMBER)
        val = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);
    return val;
}

static bool lua_get_boolean(struct lua_State* L, int idx, const char* key, bool def)
{
    bool val = def;
    int t = lua_getfield(L, idx, key);
    if (t == LUA_TBOOLEAN)
        val = lua_toboolean(L, -1) != 0;
    lua_pop(L, 1);
    return val;
}

static char* lua_get_string(struct lua_State* L, int idx, const char* key)
{
    char* val = NULL;
    int t = lua_getfield(L, idx, key);
    if (t == LUA_TSTRING) {
        const char* s = lua_tostring(L, -1);
        if (s)
            val = utils_strdup(s, NULL);
    }
    lua_pop(L, 1);
    return val;
}

/* 当字段可能是字符串或数字时，统一返回字符串 */
static char* lua_get_string_or_number(struct lua_State* L, int idx, const char* key)
{
    char* val = NULL;
    int t = lua_getfield(L, idx, key);
    if (t == LUA_TSTRING) {
        const char* s = lua_tostring(L, -1);
        if (s) val = utils_strdup(s, NULL);
    } else if (t == LUA_TNUMBER) {
        /* 数字转为字符串 */
        char buf[64];
        snprintf(buf, sizeof(buf), "%lld", (long long)lua_tointeger(L, -1));
        val = utils_strdup(buf, NULL);
    }
    lua_pop(L, 1);
    return val;
}

/* ==================== 解析动画帧 ==================== */

static void parse_animation(struct lua_State* L, int tbl_idx, jtilemap_anim_t* anim)
{
    kv_init(anim->frames);
    anim->total_duration = 0;

    lua_pushnil(L);
    while (lua_next(L, tbl_idx) != 0) {
        /* key 在 -2，value（动画帧 table）在 -1 */
        if (lua_istable(L, -1)) {
            jtilemap_anim_frame_t frame;
            frame.tileid   = lua_get_integer(L, -1, "tileid", 0);
            frame.duration = lua_get_integer(L, -1, "duration", 100);
            kv_push(jtilemap_anim_frame_t, anim->frames, frame);
            anim->total_duration += frame.duration;
        }
        lua_pop(L, 1);
    }
}

/* ==================== 解析 tileset ==================== */

static bool parse_tileset(struct lua_State* L, int idx, jtilemap_tileset_t* ts)
{
    memset(ts, 0, sizeof(*ts));
    kv_init(ts->anims);
    kv_init(ts->tile_defs);
    ts->tile_anim_map = NULL;
    ts->tile_anim_map_alloced = 0;

    ts->name        = lua_get_string(L, idx, "name");
    ts->firstgid    = lua_get_integer(L, idx, "firstgid", 1);
    ts->tilewidth   = lua_get_integer(L, idx, "tilewidth", 64);
    ts->tileheight  = lua_get_integer(L, idx, "tileheight", 64);
    ts->spacing     = lua_get_integer(L, idx, "spacing", 0);
    ts->margin      = lua_get_integer(L, idx, "margin", 0);
    ts->columns     = lua_get_integer(L, idx, "columns", 0);
    ts->tilecount   = lua_get_integer(L, idx, "tilecount", 0);
    ts->image       = lua_get_string(L, idx, "image");
    ts->imagewidth  = lua_get_integer(L, idx, "imagewidth", 0);
    ts->imageheight = lua_get_integer(L, idx, "imageheight", 0);

    /* 解析 tiles 数组（单个瓦片属性，含动画 + 独立图片） */
    int tiles_type = lua_getfield(L, idx, "tiles");
    if (tiles_type == LUA_TTABLE) {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            /* value 在 -1，应当是每个瓦片属性的 table */
            if (lua_istable(L, -1)) {
                int tile_id = lua_get_integer(L, -1, "id", -1);

                /* 检查是否有动画 */
                int anim_type = lua_getfield(L, -1, "animation");
                if (anim_type == LUA_TTABLE) {
                    jtilemap_anim_t anim;
                    parse_animation(L, lua_gettop(L), &anim);
                    kv_push(jtilemap_anim_t, ts->anims, anim);
                }
                lua_pop(L, 1);

                /* 检查是否有独立图片（用于 columns=0 的 deco 类 tileset） */
                int img_type = lua_getfield(L, -1, "image");
                if (img_type == LUA_TSTRING) {
                    jtilemap_tile_def_t def;
                    def.id = tile_id;
                    def.image = utils_strdup(lua_tostring(L, -1), NULL);
                    def.width  = lua_get_integer(L, -2, "width", 64);
                    def.height = lua_get_integer(L, -2, "height", 64);
                    kv_push(jtilemap_tile_def_t, ts->tile_defs, def);
                }
                lua_pop(L, 1);
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    return true;
}

/* ==================== 解析 tile layer ==================== */

static jtilemap_tile_layer_p parse_tile_layer(struct lua_State* L, int idx)
{
    jtilemap_tile_layer_p layer = (jtilemap_tile_layer_p)calloc(1, sizeof(jtilemap_tile_layer_t));
    if (!layer) return NULL;

    layer->base.type = JTILEMAP_LAYER_TILE;
    layer->base.name = lua_get_string(L, idx, "name");
    layer->base.visible = lua_get_boolean(L, idx, "visible", true);
    layer->base.opacity = lua_get_number(L, idx, "opacity", 1.0f);
    layer->base.offsetx = lua_get_number(L, idx, "offsetx", 0.0f);
    layer->base.offsety = lua_get_number(L, idx, "offsety", 0.0f);
    layer->base.parallaxx = lua_get_number(L, idx, "parallaxx", 1.0f);
    layer->base.parallaxy = lua_get_number(L, idx, "parallaxy", 1.0f);

    layer->width  = lua_get_integer(L, idx, "width", 0);
    layer->height = lua_get_integer(L, idx, "height", 0);

    /* 解析 data 数组 */
    int count = layer->width * layer->height;
    layer->data = (int*)calloc((size_t)count, sizeof(int));

    int data_type = lua_getfield(L, idx, "data");
    if (data_type == LUA_TTABLE) {
        int n = 0;
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            if (lua_type(L, -1) == LUA_TNUMBER && n < count) {
                layer->data[n++] = (int)lua_tointeger(L, -1);
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    return layer;
}

/* ==================== 解析 object layer ==================== */

static jtilemap_object_layer_p parse_object_layer(struct lua_State* L, int idx)
{
    jtilemap_object_layer_p layer = (jtilemap_object_layer_p)calloc(1, sizeof(jtilemap_object_layer_t));
    if (!layer) return NULL;

    layer->base.type = JTILEMAP_LAYER_OBJECT;
    layer->base.name = lua_get_string(L, idx, "name");
    layer->base.visible = lua_get_boolean(L, idx, "visible", true);
    layer->base.opacity = lua_get_number(L, idx, "opacity", 1.0f);
    layer->base.offsetx = lua_get_number(L, idx, "offsetx", 0.0f);
    layer->base.offsety = lua_get_number(L, idx, "offsety", 0.0f);
    layer->base.parallaxx = lua_get_number(L, idx, "parallaxx", 1.0f);
    layer->base.parallaxy = lua_get_number(L, idx, "parallaxy", 1.0f);

    kv_init(layer->objects);

    /* 解析 objects 数组 */
    int obj_type = lua_getfield(L, idx, "objects");
    if (obj_type == LUA_TTABLE) {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            if (lua_istable(L, -1)) {
                jtilemap_object_t obj;
                memset(&obj, 0, sizeof(obj));

                obj.id      = lua_get_integer(L, -1, "id", 0);
                obj.name    = lua_get_string(L, -1, "name");
                obj.type    = lua_get_string(L, -1, "type");
                obj.x       = (float)lua_get_number(L, -1, "x", 0.0);
                obj.y       = (float)lua_get_number(L, -1, "y", 0.0);
                obj.width   = (float)lua_get_number(L, -1, "width", 0.0);
                obj.height  = (float)lua_get_number(L, -1, "height", 0.0);
                obj.rotation = (float)lua_get_number(L, -1, "rotation", 0.0);
                obj.visible = lua_get_boolean(L, -1, "visible", true);

                /* 形状 */
                char* shape_str = lua_get_string(L, -1, "shape");
                if (shape_str) {
                    if (strcmp(shape_str, "rectangle") == 0)
                        obj.shape = JTILEMAP_SHAPE_RECT;
                    else if (strcmp(shape_str, "ellipse") == 0)
                        obj.shape = JTILEMAP_SHAPE_ELLIPSE;
                    else if (strcmp(shape_str, "polygon") == 0)
                        obj.shape = JTILEMAP_SHAPE_POLYGON;
                    else if (strcmp(shape_str, "polyline") == 0)
                        obj.shape = JTILEMAP_SHAPE_POLYLINE;
                    else if (strcmp(shape_str, "point") == 0)
                        obj.shape = JTILEMAP_SHAPE_POINT;
                    free(shape_str);
                }

                kv_push(jtilemap_object_t, layer->objects, obj);
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    return layer;
}

/* ==================== 解析 image layer ==================== */

static jtilemap_image_layer_p parse_image_layer(struct lua_State* L, int idx)
{
    jtilemap_image_layer_p layer = (jtilemap_image_layer_p)calloc(1, sizeof(jtilemap_image_layer_t));
    if (!layer) return NULL;

    layer->base.type = JTILEMAP_LAYER_IMAGE;
    layer->base.name = lua_get_string(L, idx, "name");
    layer->base.visible = lua_get_boolean(L, idx, "visible", true);
    layer->base.opacity = lua_get_number(L, idx, "opacity", 1.0f);
    layer->base.offsetx = lua_get_number(L, idx, "offsetx", 0.0f);
    layer->base.offsety = lua_get_number(L, idx, "offsety", 0.0f);
    layer->base.parallaxx = lua_get_number(L, idx, "parallaxx", 1.0f);
    layer->base.parallaxy = lua_get_number(L, idx, "parallaxy", 1.0f);

    layer->image   = lua_get_string(L, idx, "image");
    layer->repeatx = lua_get_boolean(L, idx, "repeatx", false);
    layer->repeaty = lua_get_boolean(L, idx, "repeaty", false);

    return layer;
}

/* ==================== 构建 tileset 动画加速表 ==================== */

static void build_tileset_anim_map(jtilemap_tileset_t* ts)
{
    if (ts->tilecount <= 0) return;

    /* 分配加速表，初始化为 -1（无动画） */
    ts->tile_anim_map_alloced = ts->tilecount;
    ts->tile_anim_map = (int*)calloc((size_t)ts->tilecount, sizeof(int));
    if (!ts->tile_anim_map) return;

    for (int i = 0; i < ts->tilecount; i++)
        ts->tile_anim_map[i] = -1;

    /* 遍历 anims，标记局部瓦片 id → anim 索引 */
    for (size_t a = 0; a < kv_size(ts->anims); a++) {
        jtilemap_anim_t* anim = &kv_A(ts->anims, a);
        if (kv_size(anim->frames) > 0) {
            /* Tiled 中 animation id 表示瓦片的局部 id，对应 anim 结构体携带该瓦片的动画 */
            /* 这里 a 就是该动画在 anims 中的下标 */
            /* 注意：Tiled 导出的 tiles[n] 的 n 是 id 字段，但 anims 是按顺序添加的 */
            /* 我们需要从帧列表第一个 tileid 得知它属于哪个瓦片 */
            int tile_id = kv_A(anim->frames, 0).tileid;
            if (tile_id >= 0 && tile_id < ts->tilecount)
                ts->tile_anim_map[tile_id] = (int)a;
        }
    }
}

/* ==================== 收集所有有动画的 GID ==================== */

static void collect_anim_gids(jtilemap_p tm)
{
    kv_init(tm->anim_gids);
    kv_init(tm->anim_states);

    for (size_t t = 0; t < kv_size(tm->tilesets); t++) {
        jtilemap_tileset_t* ts = &kv_A(tm->tilesets, t);
        if (!ts->tile_anim_map) continue;

        for (int i = 0; i < ts->tilecount; i++) {
            if (ts->tile_anim_map[i] >= 0) {
                int gid = ts->firstgid + i;
                kv_push(int, tm->anim_gids, gid);
                jtilemap_anim_state_t state;
                state.start_time = 0;
                kv_push(jtilemap_anim_state_t, tm->anim_states, state);
            }
        }
    }
}

/* ==================== 主加载函数 ==================== */

JOY_API jtilemap_p jtilemap_load_lua(struct lua_State* L, int idx)
{
    if (!L) return NULL;

    /* 确保 idx 是 table */
    if (lua_type(L, idx) != LUA_TTABLE) {
        log_error("jtilemap_load_lua: argument is not a table");
        return NULL;
    }

    jtilemap_p tm = (jtilemap_p)calloc(1, sizeof(jtilemap_t));
    if (!tm) {
        log_error("jtilemap_load_lua: out of memory");
        return NULL;
    }

    kv_init(tm->tilesets);
    kv_init(tm->layers);

    /* 读取地图元信息 */
    tm->version       = lua_get_string(L, idx, "version");
    tm->tiledversion  = lua_get_string(L, idx, "tiledversion");
    tm->orientation   = lua_get_string(L, idx, "orientation");
    tm->renderorder   = lua_get_string_or_number(L, idx, "renderorder");
    tm->width         = lua_get_integer(L, idx, "width", 0);
    tm->height        = lua_get_integer(L, idx, "height", 0);
    tm->tilewidth     = lua_get_integer(L, idx, "tilewidth", 64);
    tm->tileheight    = lua_get_integer(L, idx, "tileheight", 64);

    log_info("jtilemap: loading %s map %dx%d, tile %dx%d",
             tm->orientation ? tm->orientation : "unknown",
             tm->width, tm->height, tm->tilewidth, tm->tileheight);

    /* ===== 解析 tilesets ===== */
    int ts_type = lua_getfield(L, idx, "tilesets");
    if (ts_type == LUA_TTABLE) {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            if (lua_istable(L, -1)) {
                jtilemap_tileset_t ts;
                if (parse_tileset(L, lua_gettop(L), &ts)) {
                    build_tileset_anim_map(&ts);
                    kv_push(jtilemap_tileset_t, tm->tilesets, ts);
                }
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    log_info("jtilemap: parsed %zu tilesets", kv_size(tm->tilesets));

    /* ===== 解析 layers ===== */
    int layer_type = lua_getfield(L, idx, "layers");
    if (layer_type == LUA_TTABLE) {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            if (lua_istable(L, -1)) {
                /* 读取层类型 */
                char* layer_type_str = lua_get_string(L, -1, "type");
                void* layer_obj = NULL;

                if (layer_type_str) {
                    if (strcmp(layer_type_str, "tilelayer") == 0) {
                        layer_obj = parse_tile_layer(L, lua_gettop(L));
                    } else if (strcmp(layer_type_str, "objectgroup") == 0) {
                        layer_obj = parse_object_layer(L, lua_gettop(L));
                    } else if (strcmp(layer_type_str, "imagelayer") == 0) {
                        layer_obj = parse_image_layer(L, lua_gettop(L));
                    } else if (strcmp(layer_type_str, "group") == 0) {
                        /* 暂不处理 group 嵌套层 */
                        log_info("jtilemap: skipping group layer");
                    }
                    free(layer_type_str);
                }

                if (layer_obj)
                    kv_push(void*, tm->layers, layer_obj);
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    log_info("jtilemap: parsed %zu layers", kv_size(tm->layers));

    /* ===== 构建运行时动画表 ===== */
    collect_anim_gids(tm);

    return tm;
}

/* ==================== 从文件直接加载（封装 Lua） ==================== */

JOY_API jtilemap_p jtilemap_load_file(const char* filename)
{
    if (!filename) {
        log_error("jtilemap_load_file: filename is NULL");
        return NULL;
    }

    lua_State* L = luaL_newstate();
    if (!L) {
        log_error("jtilemap_load_file: failed to create Lua state");
        return NULL;
    }
    luaL_openlibs(L);

    /* 加载 Lua 文件（map.lua 以 return { ... } 结尾） */
    if (luaL_loadfile(L, filename) != LUA_OK) {
        log_error("jtilemap_load_file: load error: %s",
                  lua_tostring(L, -1) ? lua_tostring(L, -1) : "?");
        lua_close(L);
        return NULL;
    }

    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        log_error("jtilemap_load_file: execute error: %s",
                  lua_tostring(L, -1) ? lua_tostring(L, -1) : "?");
        lua_close(L);
        return NULL;
    }

    if (!lua_istable(L, -1)) {
        log_error("jtilemap_load_file: file did not return a table");
        lua_close(L);
        return NULL;
    }

    /* 解析 Lua table */
    jtilemap_p tm = jtilemap_load_lua(L, -1);
    lua_pop(L, 1);
    lua_close(L);

    return tm;
}

/* ==================== 获取 deco 独立瓦片图片路径 ==================== */

JOY_API const char* jtilemap_get_tile_image(const jtilemap_tileset_p ts, int local_id)
{
    if (!ts) return NULL;
    for (size_t i = 0; i < kv_size(ts->tile_defs); i++) {
        if (kv_A(ts->tile_defs, i).id == local_id)
            return kv_A(ts->tile_defs, i).image;
    }
    return NULL;
}

JOY_API bool jtilemap_get_tile_def_size(const jtilemap_tileset_p ts, int local_id,
                                          int* out_w, int* out_h)
{
    if (!ts) return false;
    for (size_t i = 0; i < kv_size(ts->tile_defs); i++) {
        if (kv_A(ts->tile_defs, i).id == local_id) {
            if (out_w) *out_w = kv_A(ts->tile_defs, i).width;
            if (out_h) *out_h = kv_A(ts->tile_defs, i).height;
            return true;
        }
    }
    return false;
}

/* ==================== 释放 ==================== */

static void destroy_anim(jtilemap_anim_t* anim)
{
    kv_destroy(anim->frames);
}

static void destroy_tileset(jtilemap_tileset_t* ts)
{
    free(ts->name);
    free(ts->image);
    for (size_t i = 0; i < kv_size(ts->anims); i++)
        destroy_anim(&kv_A(ts->anims, i));
    kv_destroy(ts->anims);
    for (size_t i = 0; i < kv_size(ts->tile_defs); i++)
        free(kv_A(ts->tile_defs, i).image);
    kv_destroy(ts->tile_defs);
    free(ts->tile_anim_map);
}

static void destroy_object(jtilemap_object_t* obj)
{
    free(obj->name);
    free(obj->type);
}

static void destroy_layer(void* layer)
{
    jtilemap_layer_p base = (jtilemap_layer_p)layer;
    free(base->name);

    switch (base->type) {
    case JTILEMAP_LAYER_TILE: {
        jtilemap_tile_layer_p tl = (jtilemap_tile_layer_p)layer;
        free(tl->data);
        break;
    }
    case JTILEMAP_LAYER_OBJECT: {
        jtilemap_object_layer_p ol = (jtilemap_object_layer_p)layer;
        for (size_t i = 0; i < kv_size(ol->objects); i++)
            destroy_object(&kv_A(ol->objects, i));
        kv_destroy(ol->objects);
        break;
    }
    case JTILEMAP_LAYER_IMAGE: {
        jtilemap_image_layer_p il = (jtilemap_image_layer_p)layer;
        free(il->image);
        break;
    }
    }

    free(layer);
}

JOY_API void jtilemap_destroy(jtilemap_p tm)
{
    if (!tm) return;

    free(tm->version);
    free(tm->tiledversion);
    free(tm->orientation);
    free(tm->renderorder);

    for (size_t i = 0; i < kv_size(tm->tilesets); i++)
        destroy_tileset(&kv_A(tm->tilesets, i));
    kv_destroy(tm->tilesets);

    for (size_t i = 0; i < kv_size(tm->layers); i++)
        destroy_layer(kv_A(tm->layers, i));
    kv_destroy(tm->layers);

    kv_destroy(tm->anim_gids);
    kv_destroy(tm->anim_states);

    free(tm);
}

/* ==================== GID 解析 ==================== */

JOY_API jtilemap_gid_resolve_t jtilemap_resolve_gid(const jtilemap_p tm, int gid)
{
    jtilemap_gid_resolve_t result;
    memset(&result, 0, sizeof(result));

    if (!tm || gid <= 0) return result;

    /* 找到 firstgid ≤ gid 且最大的 tileset */
    jtilemap_tileset_p best_ts = NULL;
    int best_firstgid = 0;

    for (size_t i = 0; i < kv_size(tm->tilesets); i++) {
        jtilemap_tileset_p ts = &kv_A(tm->tilesets, i);
        if (ts->firstgid <= gid && ts->firstgid >= best_firstgid) {
            best_ts = ts;
            best_firstgid = ts->firstgid;
        }
    }

    if (!best_ts) return result;

    result.tileset = best_ts;
    result.local_id = gid - best_ts->firstgid;

    /* 查询动画 */
    if (best_ts->tile_anim_map &&
        result.local_id >= 0 && result.local_id < best_ts->tile_anim_map_alloced) {
        int anim_idx = best_ts->tile_anim_map[result.local_id];
        if (anim_idx >= 0) {
            result.has_anim = true;
            result.anim_idx = anim_idx;
        }
    }

    return result;
}

/* ==================== 动画帧计算 ==================== */

JOY_API int jtilemap_get_anim_tileid(const jtilemap_p tm, int gid, uint32_t time_ms)
{
    jtilemap_gid_resolve_t res = jtilemap_resolve_gid(tm, gid);
    if (!res.has_anim) return res.local_id;

    jtilemap_anim_t* anim = &kv_A(res.tileset->anims, (size_t)res.anim_idx);
    if (kv_size(anim->frames) == 0) return res.local_id;

    /* 计算当前帧 */
    int total = anim->total_duration;
    if (total <= 0) {
        /* 防御：没有总时长则取第一帧 */
        return kv_A(anim->frames, 0).tileid;
    }

    int elapsed = (int)(time_ms % (uint32_t)total);
    int accum = 0;
    for (size_t f = 0; f < kv_size(anim->frames); f++) {
        accum += kv_A(anim->frames, f).duration;
        if (elapsed < accum)
            return kv_A(anim->frames, f).tileid;
    }

    return kv_A(anim->frames, kv_size(anim->frames) - 1).tileid;
}

/* ==================== 纹理坐标计算 ==================== */

JOY_API bool jtilemap_get_tile_uv(const jtilemap_p tm, int gid,
                                   uint32_t time_ms, float out_rect[4])
{
    if (!tm || !out_rect) return false;

    memset(out_rect, 0, 4 * sizeof(float));
    if (gid <= 0) return false;

    jtilemap_gid_resolve_t res = jtilemap_resolve_gid(tm, gid);
    if (!res.tileset) return false;

    jtilemap_tileset_p ts = res.tileset;

    /* 确定当前显示的局部瓦片 ID（考虑动画） */
    int local_id = res.local_id;
    if (res.has_anim) {
        local_id = jtilemap_get_anim_tileid(tm, gid, time_ms);
    }

    /* 处理无列信息的 tileset（仅 deco 类型，每个瓦片独立图片） */
    if (ts->columns <= 0) {
        /* 这种 tileset 没有固定网格，无法用 columns 计算 UV，返回零矩形 */
        return false;
    }

    /* 计算行/列 */
    int col = local_id % ts->columns;
    int row = local_id / ts->columns;

    out_rect[0] = (float)(ts->margin + col * (ts->tilewidth  + ts->spacing));
    out_rect[1] = (float)(ts->margin + row * (ts->tileheight + ts->spacing));
    out_rect[2] = (float)ts->tilewidth;
    out_rect[3] = (float)ts->tileheight;

    return true;
}

/* ==================== 图层查询 ==================== */

JOY_API jtilemap_tile_layer_p jtilemap_find_tile_layer(const jtilemap_p tm,
                                                         const char* name)
{
    if (!tm || !name) return NULL;

    for (size_t i = 0; i < kv_size(tm->layers); i++) {
        jtilemap_layer_p base = (jtilemap_layer_p)kv_A(tm->layers, i);
        if (base->type == JTILEMAP_LAYER_TILE && base->name &&
            strcmp(base->name, name) == 0) {
            return (jtilemap_tile_layer_p)base;
        }
    }
    return NULL;
}

JOY_API jtilemap_object_layer_p jtilemap_find_object_layer(const jtilemap_p tm,
                                                            const char* name)
{
    if (!tm || !name) return NULL;

    for (size_t i = 0; i < kv_size(tm->layers); i++) {
        jtilemap_layer_p base = (jtilemap_layer_p)kv_A(tm->layers, i);
        if (base->type == JTILEMAP_LAYER_OBJECT && base->name &&
            strcmp(base->name, name) == 0) {
            return (jtilemap_object_layer_p)base;
        }
    }
    return NULL;
}

JOY_API int jtilemap_get_tile(const jtilemap_tile_layer_p layer, int tx, int ty)
{
    if (!layer || !layer->data) return 0;
    if (tx < 0 || tx >= layer->width || ty < 0 || ty >= layer->height)
        return 0;
    return layer->data[ty * layer->width + tx];
}
