/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jtilemap.c
Description: Tiled 地图数据解析实现（Lua 导出格式）
Author: ydlc
Version: 1.0
Date: 2026.5.22
History:
*************************************************/
#include "external/stb_image.h"
#include "external/klib/kvec.h"
#include "external/klib/khash.h"
#include "external/lua/lua.h"
#include "external/lua/lauxlib.h"
#include "external/lua/lualib.h"
#include "joy.h"






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

/* ---------- 独立瓦片定义（用于 columns=0 的 tileset，如 deco） ---------- */
typedef struct {
        int    id;            /* tileset 内局部 ID */
        char* image;         /* 独立图片路径 */
        int    width;
        int    height;
} jtilemap_tile_def_t;
typedef jtilemap_tile_def_t* jtilemap_tile_def_p;

/* ---------- 图块集 ---------- */
typedef struct jtilemap_tileset {
        char* name;
        int    firstgid;
        int    tilewidth;
        int    tileheight;
        int    spacing;
        int    margin;
        int    columns;
        int    tilecount;
        char* image;
        int    imagewidth;
        int    imageheight;

        /* 动画：anims[n] 对应 tileset 内部局部瓦片 id = anims[n].frames[0].tileid（首个帧的 id） */
        kvec_t(jtilemap_anim_t) anims;

        /* 独立瓦片定义（用于 columns=0 的 tileset） */
        kvec_t(jtilemap_tile_def_t) tile_defs;

        /* 运行时加速：局部瓦片 id -> anims[] 下标，-1 表示无动画 */
        int* tile_anim_map;   /* 数组长度 = tilecount，仅在加载后有效 */
        int  tile_anim_map_alloced;
} jtilemap_tileset_t, * jtilemap_tileset_p;




/* ---------- 图层基类 ---------- */
typedef struct jtilemap_layer {
        int   type;          /* JTILEMAP_LAYER_* */
        char* name;
        bool  visible;
        float opacity;
        float offsetx, offsety;
        float parallaxx, parallaxy;
} jtilemap_layer_t, * jtilemap_layer_p;

/* ---------- 瓦片图层 ---------- */
struct jtilemap_tile_layer {
        jtilemap_layer_t base;
        int   width;         /* 瓦片列数 */
        int   height;        /* 瓦片行数 */
        int* data;          /* 展平 GID 数组，行优先：data[y * width + x] */
};

/* ---------- 对象图层 ---------- */
struct jtilemap_object_layer {
        jtilemap_layer_t base;
        kvec_t(jtilemap_object_t) objects;
};


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
struct jtilemap {
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
};

/* ---------- GID 解析结果 ---------- */
struct jtilemap_gid_resolve {
        jtilemap_tileset_p tileset;
        int local_id;               /* tileset 内局部 ID */
        bool has_anim;
        int  anim_idx;              /* tileset->anims[] 下标，仅 has_anim 时有效 */
};


KHASH_MAP_INIT_STR(tex_cache, image_p)

struct jtilemap_render_t {
        jr2d_p                 r2d;
        char* asset_base;
        jtilemap_path_remap_t  remap;
        float                  view_w;
        float                  view_h;
        khash_t(tex_cache)* tex_cache;
};


/* 提取干净 GID（去掉翻转标志） */
JOY_INLINE int jtilemap_clean_gid(int raw_gid) { return raw_gid & JTILEMAP_GID_MASK; }
/* 提取翻转标志 */
JOY_INLINE int jtilemap_gid_flags(int raw_gid) { return raw_gid & ~JTILEMAP_GID_MASK; }

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

jtilemap_p jtilemap_load_lua(struct lua_State* L, int idx)
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

jtilemap_p jtilemap_load_file(const char* filename)
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

const char* jtilemap_get_tile_image(const jtilemap_tileset_p ts, int local_id)
{
    if (!ts) return NULL;
    for (size_t i = 0; i < kv_size(ts->tile_defs); i++) {
        if (kv_A(ts->tile_defs, i).id == local_id)
            return kv_A(ts->tile_defs, i).image;
    }
    return NULL;
}

bool jtilemap_get_tile_def_size(const jtilemap_tileset_p ts, int local_id,
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

int jtilemap_get_width(const jtilemap_p tm) {
    return tm ? tm->width : 0;
}

int jtilemap_get_height(const jtilemap_p tm) {
    return tm ? tm->height : 0;
}

void jtilemap_destroy(jtilemap_p tm)
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

jtilemap_gid_resolve_t jtilemap_resolve_gid(const jtilemap_p tm, int gid)
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

int jtilemap_get_anim_tileid(const jtilemap_p tm, int gid, uint32_t time_ms)
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

bool jtilemap_get_tile_uv(const jtilemap_p tm, int gid,
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

jtilemap_tile_layer_p jtilemap_find_tile_layer(const jtilemap_p tm,
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

jtilemap_object_layer_p jtilemap_find_object_layer(const jtilemap_p tm,
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

void jtilemap_object_layer_foreach(jtilemap_object_layer_p ol, void (*callback)(jtilemap_object_p obj, void* arg), void*arg)
{
    if (ol) {
        for (size_t i = 0; i < kv_size(ol->objects); i++) {
            jtilemap_object_t* obj = &kv_A(ol->objects, i);
            if (obj->shape != JTILEMAP_SHAPE_RECT)
                continue;
            callback(obj, arg);
        }
        log_info("[tilemap] loaded %zu collision walls", kv_size(ol->objects));
    }
}

int jtilemap_get_tile(const jtilemap_tile_layer_p layer, int tx, int ty)
{
    if (!layer || !layer->data) return 0;
    if (tx < 0 || tx >= layer->width || ty < 0 || ty >= layer->height)
        return 0;
    return layer->data[ty * layer->width + tx];
}




static const char* default_remap(const char* path)
{
        if (!path || !*path) return path;
        const char* p = path;
        while (*p == '.' || *p == '/' || *p == '\\') p++;
        static char buf[1024];
        snprintf(buf, sizeof(buf), "%s", p);
        return buf;
}

static image_p load_texture_internal(jtilemap_render_p r, const char* path)
{
        if (!path || !*path) return NULL;
        khiter_t k = kh_get(tex_cache, r->tex_cache, path);
        if (k != kh_end(r->tex_cache))
                return kh_value(r->tex_cache, k);

        char full_path[1024];
        if (r->asset_base)
                snprintf(full_path, sizeof(full_path), "%s%s", r->asset_base, path);
        else
                snprintf(full_path, sizeof(full_path), "%s", path);

        int w = 0, h = 0, ch = 0;
        unsigned char* data = stbi_load(full_path, &w, &h, &ch, 4);
        if (!data) {
                int ret;
                khiter_t ki = kh_put(tex_cache, r->tex_cache, path, &ret);
                kh_value(r->tex_cache, ki) = NULL;
                return NULL;
        }

        image_p img = image_create_from_data(r->r2d, data, w, h);
        stbi_image_free(data);
        if (!img) return NULL;

        char* key = utils_strdup(path, NULL);
        int ret;
        khiter_t ki = kh_put(tex_cache, r->tex_cache, key, &ret);
        kh_value(r->tex_cache, ki) = img;
        return img;
}

static image_p load_tileset_tex(jtilemap_render_p r, const jtilemap_tileset_t* ts)
{
        if (!ts || !ts->image) return NULL;
        const char* mapped = r->remap ? r->remap(ts->image) : ts->image;
        return load_texture_internal(r, mapped);
}

static void render_tile_layer_internal(jtilemap_render_p r, jtilemap_p tm,
        const jtilemap_tile_layer_p layer,
        float cam_x, float cam_y, uint32_t time_ms)
{
        if (!layer->base.visible) return;
        float tilew = (float)tm->tilewidth, tileh = (float)tm->tileheight;
        cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
        cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;

        int sx = (int)floorf(cam_x / tilew); if (sx < 0) sx = 0;
        int sy = (int)floorf(cam_y / tileh); if (sy < 0) sy = 0;
        int ex = (int)ceilf((cam_x + r->view_w) / tilew) + 1; if (ex > layer->width)  ex = layer->width;
        int ey = (int)ceilf((cam_y + r->view_h) / tileh) + 1; if (ey > layer->height) ey = layer->height;

        uint8_t alpha = (uint8_t)(layer->base.opacity * 255.0f);

        for (int ty = sy; ty < ey; ty++) {
                for (int tx = sx; tx < ex; tx++) {
                        int raw_gid = jtilemap_get_tile(layer, tx, ty);
                        if (raw_gid == 0) continue;
                        int gid = jtilemap_clean_gid(raw_gid);
                        int flags = jtilemap_gid_flags(raw_gid);
                        jtilemap_gid_resolve_t res = jtilemap_resolve_gid(tm, gid);
                        if (!res.tileset) continue;

                        float dst_x = tx * tilew - cam_x, dst_y = ty * tileh - cam_y;
                        float dst_w = (float)res.tileset->tilewidth;
                        float dst_h = (float)res.tileset->tileheight;

                        const char* deco_img = jtilemap_get_tile_image(res.tileset, res.local_id);
                        if (deco_img) {
                                const char* mapped = r->remap ? r->remap(deco_img) : deco_img;
                                image_p timg = load_texture_internal(r, mapped);
                                if (!timg) continue;
                                image_set_alpha(timg, alpha);
                                int dw = (int)dst_w, dh = (int)dst_h;
                                jtilemap_get_tile_def_size(res.tileset, res.local_id, &dw, &dh);
                                image_draw1(timg, NULL, &(jrect_t){ dst_x, dst_y, (float)dw, (float)dh });
                                continue;
                        }

                        float uv[4];
                        if (!jtilemap_get_tile_uv(tm, gid, time_ms, uv)) continue;

                        image_p timg = load_tileset_tex(r, res.tileset);
                        if (!timg) {
                                r2d_set_color(r->r2d, 255, 0, 255, 255);
                                shape_draw_rectangle(r->r2d, "fill", (jrect_t) { dst_x, dst_y, dst_w, dst_h });
                                continue;
                        }

                        image_set_alpha(timg, alpha);
                        jrect_t src = { uv[0], uv[1], uv[2], uv[3] };

                        int flip = JR2D_FLIP_NONE;
                        if (flags & JTILEMAP_FLIP_H) flip |= JR2D_FLIP_H;
                        if (flags & JTILEMAP_FLIP_V) flip |= JR2D_FLIP_V;
                        image_draw_ex(timg, &src, (vec2_t) { dst_x, dst_y },
                                (flags & JTILEMAP_FLIP_D) ? 90.0f : 0.0f,
                                (vec2_t) {
                                flip& JR2D_FLIP_H ? -1 : 1, flip& JR2D_FLIP_V ? -1 : 1
                        },
                                (vec2_t) {
                                0, 0
                        });
                }
        }
}

static void render_object_layer_internal(const jtilemap_object_layer_p layer,
        float cam_x, float cam_y, jr2d_p r2d)
{
        if (!layer->base.visible) return;
        cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
        cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;
        r2d_set_color(r2d, 255, 85, 0, 255);
        for (size_t i = 0; i < kv_size(layer->objects); i++) {
                jtilemap_object_t* obj = &kv_A(layer->objects, i);
                if (!obj->visible || obj->shape != JTILEMAP_SHAPE_RECT) continue;
                if (obj->width <= 0.0f || obj->height <= 0.0f || obj->width > 10000.0f || obj->height > 10000.0f) continue;
                shape_draw_rectangle(r2d, "line", (jrect_t) { obj->x - cam_x, obj->y - cam_y, obj->width, obj->height });
        }
}

static void render_image_layer_internal(jtilemap_render_p r,
        const jtilemap_image_layer_p layer,
        float cam_x, float cam_y)
{
        if (!layer->base.visible || !layer->image) return;
        cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
        cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;

        const char* mapped = r->remap ? r->remap(layer->image) : layer->image;
        image_p timg = load_texture_internal(r, mapped);
        if (!timg) return;

        float tex_w = 0, tex_h = 0;
        image_get_size(timg, &tex_w, &tex_h);

        float vp_w = r->view_w, vp_h = r->view_h;
        float start_x = (layer->repeatx ? fmodf(-cam_x, tex_w) : -cam_x);
        float start_y = (layer->repeaty ? fmodf(-cam_y, tex_h) : -cam_y);
        if (start_x > 0) start_x -= tex_w;
        if (start_y > 0) start_y -= tex_h;

        for (float py = start_y; py < vp_h; py += tex_h)
                for (float px = start_x; px < vp_w; px += tex_w)
                        image_draw1(timg, NULL, &(jrect_t){ px, py, tex_w, tex_h });
}

/* ======================== 公共 API ======================== */

jtilemap_render_p jtilemap_render_create(jr2d_p renderer,
        const char* asset_base_path,
        jtilemap_path_remap_t remap)
{
        jtilemap_render_p r = (jtilemap_render_p)calloc(1, sizeof(jtilemap_render_t));
        if (!r) return NULL;
        r->r2d = renderer;
        r->remap = remap ? remap : default_remap;
        r->asset_base = asset_base_path ? utils_strdup(asset_base_path, NULL) : NULL;
        r->view_w = 1024.0f;
        r->view_h = 760.0f;
        r->tex_cache = kh_init(tex_cache);
        return r;
}

void jtilemap_render_all(jtilemap_render_p r, jtilemap_p tm,
        float camera_x, float camera_y,
        uint32_t time_ms)
{
        if (!r || !tm || !r->r2d) return;

        {
                int vp_w = 0, vp_h = 0, mode;
                r2d_get_logical_presentation(r->r2d, &vp_w, &vp_h, &mode);
                if (vp_w <= 0 || vp_h <= 0)
                        r2d_get_size(r->r2d, &vp_w, &vp_h);
                r->view_w = (float)vp_w;
                r->view_h = (float)vp_h;
        }

        for (size_t i = 0; i < kv_size(tm->layers); i++) {
                jtilemap_layer_p base = (jtilemap_layer_p)kv_A(tm->layers, i);
                switch (base->type) {
                case JTILEMAP_LAYER_TILE:
                        render_tile_layer_internal(r, tm, (jtilemap_tile_layer_p)base, camera_x, camera_y, time_ms);
                        break;
                case JTILEMAP_LAYER_OBJECT:
                        render_object_layer_internal((jtilemap_object_layer_p)base, camera_x, camera_y, r->r2d);
                        break;
                case JTILEMAP_LAYER_IMAGE:
                        render_image_layer_internal(r, (jtilemap_image_layer_p)base, camera_x, camera_y);
                        break;
                }
        }
}

bool jtilemap_render_layer(jtilemap_render_p r, jtilemap_p tm,
        const char* layer_name,
        float camera_x, float camera_y,
        uint32_t time_ms)
{
        if (!r || !tm || !r->r2d || !layer_name) return false;

        for (size_t i = 0; i < kv_size(tm->layers); i++) {
                jtilemap_layer_p base = (jtilemap_layer_p)kv_A(tm->layers, i);
                if (!base->name || strcmp(base->name, layer_name) != 0) continue;
                switch (base->type) {
                case JTILEMAP_LAYER_TILE:
                        render_tile_layer_internal(r, tm, (jtilemap_tile_layer_p)base, camera_x, camera_y, time_ms);
                        return true;
                case JTILEMAP_LAYER_OBJECT:
                        render_object_layer_internal((jtilemap_object_layer_p)base, camera_x, camera_y, r->r2d);
                        return true;
                case JTILEMAP_LAYER_IMAGE:
                        render_image_layer_internal(r, (jtilemap_image_layer_p)base, camera_x, camera_y);
                        return true;
                }
        }
        return false;
}

void jtilemap_render_destroy(jtilemap_render_p r)
{
        if (!r) return;
        if (r->tex_cache) {
                const char* key;
                image_p timg;
                kh_foreach(r->tex_cache, key, timg, {
                        if (timg) image_destroy(timg);
                        free((void*)key);
                        });
                kh_destroy(tex_cache, r->tex_cache);
        }
        if (r->asset_base) free(r->asset_base);
        free(r);
}
