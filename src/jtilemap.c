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
#include "external/cJSON.h"
#include <stdlib.h>
#include <string.h>
/* 公共头不再间接提供 SDL 声明：本 TU 用到 SDL_LoadFile / SDL_free，
 * 必须自己包含 SDL 头，否则 Android clang 在 C99 下报隐式函数声明。 */
#include <SDL3/SDL.h>
#include "joy.h"






/* ---------- 动画帧 ---------- */
typedef struct {
        int tileid;         /* tileset 内局部瓦片 ID */
        int duration;       /* 持续时间（毫秒） */
} joy_tilemap_anim_frame_t;
typedef joy_tilemap_anim_frame_t* joy_tilemap_anim_frame_p;

/* ---------- 动画序列 ---------- */
typedef struct {
        kvec_t(joy_tilemap_anim_frame_t) frames;
        int total_duration;          /* 所有帧总时长（毫秒），缓存加速 */
} joy_tilemap_anim_t;
typedef joy_tilemap_anim_t* joy_tilemap_anim_p;

/* ---------- 独立瓦片定义（用于 columns=0 的 tileset，如 deco） ---------- */
typedef struct {
        int    id;            /* tileset 内局部 ID */
        char* image;         /* 独立图片路径 */
        int    width;
        int    height;
} joy_tilemap_tile_def_t;
typedef joy_tilemap_tile_def_t* joy_tilemap_tile_def_p;

/* ---------- 图块集 ---------- */
struct joy_tilemap_tileset {
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
        kvec_t(joy_tilemap_anim_t) anims;

        /* 独立瓦片定义（用于 columns=0 的 tileset） */
        kvec_t(joy_tilemap_tile_def_t) tile_defs;

        /* 运行时加速：局部瓦片 id -> anims[] 下标，-1 表示无动画 */
        int* tile_anim_map;   /* 数组长度 = tilecount，仅在加载后有效 */
        int  tile_anim_map_alloced;
};




/* ---------- 图层基类 ---------- */
typedef struct joy_tilemap_layer {
        int   type;          /* JTILEMAP_LAYER_* */
        char* name;
        bool  visible;
        float opacity;
        float offsetx, offsety;
        float parallaxx, parallaxy;
} joy_tilemap_layer_t, * joy_tilemap_layer_p;

/* ---------- 瓦片图层 ---------- */
struct joy_tilemap_tile_layer {
        joy_tilemap_layer_t base;
        int   width;         /* 瓦片列数 */
        int   height;        /* 瓦片行数 */
        int* data;          /* 展平 GID 数组，行优先：data[y * width + x] */
};

/* ---------- 对象图层 ---------- */
struct joy_tilemap_object_layer {
        joy_tilemap_layer_t base;
        kvec_t(joy_tilemap_object_t) objects;
};


/* ---------- 图片图层 ---------- */
typedef struct {
        joy_tilemap_layer_t base;
        char* image;
        bool  repeatx;
        bool  repeaty;
} joy_tilemap_image_layer_t;
typedef joy_tilemap_image_layer_t* joy_tilemap_image_layer_p;

/* ---------- 运行时动画状态 ---------- */
typedef struct {
        uint32_t start_time;          /* 此瓦片启动时间（ms） */
} joy_tilemap_anim_state_t;

/* ---------- 主地图 ---------- */
struct joy_tilemap {
        char* version;
        char* tiledversion;
        char* orientation;
        char* renderorder;

        int width;          /* 瓦片列数 */
        int height;         /* 瓦片行数 */
        int tilewidth;      /* 默认瓦片宽度 */
        int tileheight;     /* 默认瓦片高度 */

        kvec_t(joy_tilemap_tileset_t) tilesets;

        /* layers 数组中每一项是 joy_tilemap_layer_p（实际为 *tile_layer / *object_layer / *image_layer） */
        kvec_t(void*) layers;

        /* 运行时：每加载一个 tile layer 时收集所有有动画的 gid 及启动时间 */
        kvec_t(int)          anim_gids;       /* 有动画的全局 GID */
        kvec_t(joy_tilemap_anim_state_t) anim_states;
};

/* GID 解析结果类型已在 joy.h 中完整定义（joy_tilemap_gid_resolve_t），此处不再重复定义 */


KHASH_MAP_INIT_STR(tex_cache, joy_image_p)

struct joy_tilemap_render_t {
        joy_r2d_p                 r2d;
        char* asset_base;
        joy_tilemap_path_remap_t  remap;
        float                  view_w;
        float                  view_h;
        float                  tile_px;   /* 每格目标像素；<=0 时用 map.tilewidth（兼容默认） */
        khash_t(tex_cache)* tex_cache;
};


/* 提取干净 GID（去掉翻转标志） */
JOY_INLINE int jtilemap_clean_gid(int raw_gid) { return raw_gid & JTILEMAP_GID_MASK; }
/* 提取翻转标志 */
JOY_INLINE int jtilemap_gid_flags(int raw_gid) { return raw_gid & ~JTILEMAP_GID_MASK; }

/* ==================== 构建 tileset 动画加速表 ==================== */

static void build_tileset_anim_map(joy_tilemap_tileset_t* ts)
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
        joy_tilemap_anim_t* anim = &kv_A(ts->anims, a);
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

static void collect_anim_gids(joy_tilemap_p tm)
{
    kv_init(tm->anim_gids);
    kv_init(tm->anim_states);

    for (size_t t = 0; t < kv_size(tm->tilesets); t++) {
        joy_tilemap_tileset_t* ts = &kv_A(tm->tilesets, t);
        if (!ts->tile_anim_map) continue;

        for (int i = 0; i < ts->tilecount; i++) {
            if (ts->tile_anim_map[i] >= 0) {
                int gid = ts->firstgid + i;
                kv_push(int, tm->anim_gids, gid);
                joy_tilemap_anim_state_t state;
                state.start_time = 0;
                kv_push(joy_tilemap_anim_state_t, tm->anim_states, state);
            }
        }
    }
}

/* ==================== JSON 解析后端（cJSON，无需 Lua VM） ==================== */
/* 与上方 Lua 后端字段一一对应：Tiled 的 Lua 导出与 JSON 导出结构同构，
   此处用现成的 cJSON 解析，使地图加载摆脱对 Lua 虚拟机的依赖。 */

static int cj_get_integer(cJSON* obj, const char* key, int def)
{
    cJSON* it = cJSON_GetObjectItem(obj, key);
    if (it && it->type == cJSON_Number) return (int)it->valuedouble;
    return def;
}
static float cj_get_number(cJSON* obj, const char* key, float def)
{
    cJSON* it = cJSON_GetObjectItem(obj, key);
    if (it && it->type == cJSON_Number) return (float)it->valuedouble;
    return def;
}
static bool cj_get_boolean(cJSON* obj, const char* key, bool def)
{
    cJSON* it = cJSON_GetObjectItem(obj, key);
    if (!it) return def;
    if (it->type == cJSON_True)  return true;
    if (it->type == cJSON_False) return false;
    return def;
}
static char* cj_get_string(cJSON* obj, const char* key)
{
    cJSON* it = cJSON_GetObjectItem(obj, key);
    if (it && it->type == cJSON_String && it->valuestring)
        return joy_core_utils_strdup(it->valuestring, NULL);
    return NULL;
}
/* 当字段可能是字符串或数字时，统一返回字符串（renderorder 等） */
static char* cj_get_string_or_number(cJSON* obj, const char* key)
{
    cJSON* it = cJSON_GetObjectItem(obj, key);
    if (!it) return NULL;
    if (it->type == cJSON_String && it->valuestring) return joy_core_utils_strdup(it->valuestring, NULL);
    if (it->type == cJSON_Number) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%lld", (long long)(int)it->valuedouble);
        return joy_core_utils_strdup(buf, NULL);
    }
    return NULL;
}

static void parse_animation_cjson(cJSON* arr, joy_tilemap_anim_t* anim)
{
    kv_init(anim->frames);
    anim->total_duration = 0;
    if (!arr || arr->type != cJSON_Array) return;
    int n = cJSON_GetArraySize(arr);
    for (int i = 0; i < n; i++) {
        cJSON* f = cJSON_GetArrayItem(arr, i);
        if (!f || f->type != cJSON_Object) continue;
        joy_tilemap_anim_frame_t frame;
        frame.tileid   = cj_get_integer(f, "tileid", 0);
        frame.duration = cj_get_integer(f, "duration", 100);
        kv_push(joy_tilemap_anim_frame_t, anim->frames, frame);
        anim->total_duration += frame.duration;
    }
}

static bool parse_tileset_cjson(cJSON* j, joy_tilemap_tileset_t* ts)
{
    memset(ts, 0, sizeof(*ts));
    kv_init(ts->anims);
    kv_init(ts->tile_defs);
    ts->tile_anim_map = NULL;
    ts->tile_anim_map_alloced = 0;

    ts->name        = cj_get_string(j, "name");
    ts->firstgid    = cj_get_integer(j, "firstgid", 1);
    ts->tilewidth   = cj_get_integer(j, "tilewidth", 64);
    ts->tileheight  = cj_get_integer(j, "tileheight", 64);
    ts->spacing     = cj_get_integer(j, "spacing", 0);
    ts->margin      = cj_get_integer(j, "margin", 0);
    ts->columns     = cj_get_integer(j, "columns", 0);
    ts->tilecount   = cj_get_integer(j, "tilecount", 0);
    ts->image       = cj_get_string(j, "image");
    ts->imagewidth  = cj_get_integer(j, "imagewidth", 0);
    ts->imageheight = cj_get_integer(j, "imageheight", 0);

    cJSON* tiles = cJSON_GetObjectItem(j, "tiles");
    if (tiles && tiles->type == cJSON_Array) {
        int n = cJSON_GetArraySize(tiles);
        for (int i = 0; i < n; i++) {
            cJSON* t = cJSON_GetArrayItem(tiles, i);
            if (!t || t->type != cJSON_Object) continue;
            int tile_id = cj_get_integer(t, "id", -1);

            cJSON* anim = cJSON_GetObjectItem(t, "animation");
            if (anim && anim->type == cJSON_Array) {
                joy_tilemap_anim_t a;
                parse_animation_cjson(anim, &a);
                kv_push(joy_tilemap_anim_t, ts->anims, a);
            }

            cJSON* img = cJSON_GetObjectItem(t, "image");
            if (img && img->type == cJSON_String && img->valuestring) {
                joy_tilemap_tile_def_t def;
                def.id = tile_id;
                def.image = joy_core_utils_strdup(img->valuestring, NULL);
                def.width  = cj_get_integer(t, "width", 64);
                def.height = cj_get_integer(t, "height", 64);
                kv_push(joy_tilemap_tile_def_t, ts->tile_defs, def);
            }
        }
    }
    return true;
}

static joy_tilemap_tile_layer_p parse_tile_layer_cjson(cJSON* j)
{
    joy_tilemap_tile_layer_p layer = (joy_tilemap_tile_layer_p)calloc(1, sizeof(joy_tilemap_tile_layer_t));
    if (!layer) return NULL;

    layer->base.type = JTILEMAP_LAYER_TILE;
    layer->base.name      = cj_get_string(j, "name");
    layer->base.visible   = cj_get_boolean(j, "visible", true);
    layer->base.opacity   = cj_get_number(j, "opacity", 1.0f);
    layer->base.offsetx   = cj_get_number(j, "offsetx", 0.0f);
    layer->base.offsety   = cj_get_number(j, "offsety", 0.0f);
    layer->base.parallaxx = cj_get_number(j, "parallaxx", 1.0f);
    layer->base.parallaxy = cj_get_number(j, "parallaxy", 1.0f);

    layer->width  = cj_get_integer(j, "width", 0);
    layer->height = cj_get_integer(j, "height", 0);

    int count = layer->width * layer->height;
    layer->data = (int*)calloc((size_t)(count > 0 ? count : 0), sizeof(int));

    cJSON* data = cJSON_GetObjectItem(j, "data");
    if (data && data->type == cJSON_Array) {
        int n = cJSON_GetArraySize(data);
        int m = (count > 0 ? count : 0);
        for (int i = 0; i < n && i < m; i++) {
            cJSON* v = cJSON_GetArrayItem(data, i);
            if (!v || v->type != cJSON_Number) continue;
            /* Tiled 的 GID 是 32 位无符号：高 3 位为水平/垂直/对角翻转标志。
             * 直接 (int)double 转换时，带 0x80000000/0x40000000/0x20000000 的
             * GID（如 3221225483 = 0xC000000B）会因超过 INT_MAX 溢出成 INT_MIN，
             * 清位后变 0 → 该瓦片被当作空格跳过，表现为"部分墙的贴图不显示"。
             * 这里先按 int64 截取低 32 位再转 int，保证翻转位与 GID 都不丢。 */
            layer->data[i] = (int)(uint32_t)(int64_t)v->valuedouble;
        }
    }
    return layer;
}

static joy_tilemap_object_layer_p parse_object_layer_cjson(cJSON* j)
{
    joy_tilemap_object_layer_p layer = (joy_tilemap_object_layer_p)calloc(1, sizeof(joy_tilemap_object_layer_t));
    if (!layer) return NULL;

    layer->base.type = JTILEMAP_LAYER_OBJECT;
    layer->base.name      = cj_get_string(j, "name");
    layer->base.visible   = cj_get_boolean(j, "visible", true);
    layer->base.opacity   = cj_get_number(j, "opacity", 1.0f);
    layer->base.offsetx   = cj_get_number(j, "offsetx", 0.0f);
    layer->base.offsety   = cj_get_number(j, "offsety", 0.0f);
    layer->base.parallaxx = cj_get_number(j, "parallaxx", 1.0f);
    layer->base.parallaxy = cj_get_number(j, "parallaxy", 1.0f);

    kv_init(layer->objects);

    cJSON* objs = cJSON_GetObjectItem(j, "objects");
    if (objs && objs->type == cJSON_Array) {
        int n = cJSON_GetArraySize(objs);
        for (int i = 0; i < n; i++) {
            cJSON* o = cJSON_GetArrayItem(objs, i);
            if (!o || o->type != cJSON_Object) continue;
            joy_tilemap_object_t obj;
            memset(&obj, 0, sizeof(obj));

            obj.id       = cj_get_integer(o, "id", 0);
            obj.name     = cj_get_string(o, "name");
            obj.type     = cj_get_string(o, "type");
            obj.x        = cj_get_number(o, "x", 0.0f);
            obj.y        = cj_get_number(o, "y", 0.0f);
            obj.width    = cj_get_number(o, "width", 0.0f);
            obj.height   = cj_get_number(o, "height", 0.0f);
            obj.rotation = cj_get_number(o, "rotation", 0.0f);
            obj.visible  = cj_get_boolean(o, "visible", true);

            /* 形状：优先读 Tiled Lua 导出的 shape 字符串（与 Lua 后端一致），
               否则按 Tiled JSON 原生的 point/ellipse/polygon/polyline 标志判定。 */
            char* shape_str = cj_get_string(o, "shape");
            if (shape_str) {
                if      (strcmp(shape_str, "rectangle") == 0) obj.shape = JTILEMAP_SHAPE_RECT;
                else if (strcmp(shape_str, "ellipse") == 0)   obj.shape = JTILEMAP_SHAPE_ELLIPSE;
                else if (strcmp(shape_str, "polygon") == 0)   obj.shape = JTILEMAP_SHAPE_POLYGON;
                else if (strcmp(shape_str, "polyline") == 0)  obj.shape = JTILEMAP_SHAPE_POLYLINE;
                else if (strcmp(shape_str, "point") == 0)     obj.shape = JTILEMAP_SHAPE_POINT;
                free(shape_str);
            } else if (cj_get_boolean(o, "point", false))      obj.shape = JTILEMAP_SHAPE_POINT;
            else if (cj_get_boolean(o, "ellipse", false))     obj.shape = JTILEMAP_SHAPE_ELLIPSE;
            else if (cJSON_GetObjectItem(o, "polygon"))       obj.shape = JTILEMAP_SHAPE_POLYGON;
            else if (cJSON_GetObjectItem(o, "polyline"))      obj.shape = JTILEMAP_SHAPE_POLYLINE;
            else                                             obj.shape = JTILEMAP_SHAPE_RECT;

            kv_push(joy_tilemap_object_t, layer->objects, obj);
        }
    }
    return layer;
}

static joy_tilemap_image_layer_p parse_image_layer_cjson(cJSON* j)
{
    joy_tilemap_image_layer_p layer = (joy_tilemap_image_layer_p)calloc(1, sizeof(joy_tilemap_image_layer_t));
    if (!layer) return NULL;

    layer->base.type = JTILEMAP_LAYER_IMAGE;
    layer->base.name      = cj_get_string(j, "name");
    layer->base.visible   = cj_get_boolean(j, "visible", true);
    layer->base.opacity   = cj_get_number(j, "opacity", 1.0f);
    layer->base.offsetx   = cj_get_number(j, "offsetx", 0.0f);
    layer->base.offsety   = cj_get_number(j, "offsety", 0.0f);
    layer->base.parallaxx = cj_get_number(j, "parallaxx", 1.0f);
    layer->base.parallaxy = cj_get_number(j, "parallaxy", 1.0f);

    layer->image   = cj_get_string(j, "image");
    layer->repeatx = cj_get_boolean(j, "repeatx", false);
    layer->repeaty = cj_get_boolean(j, "repeaty", false);

    return layer;
}

joy_tilemap_p joy_scene_tilemap_load_cjson(cJSON* root)
{
    if (!root || root->type != cJSON_Object) {
        joy_core_log_error("joy_scene_tilemap_load_cjson: root is not an object");
        return NULL;
    }

    joy_tilemap_p tm = (joy_tilemap_p)calloc(1, sizeof(joy_tilemap_t));
    if (!tm) {
        joy_core_log_error("joy_scene_tilemap_load_cjson: out of memory");
        return NULL;
    }

    kv_init(tm->tilesets);
    kv_init(tm->layers);

    tm->version      = cj_get_string(root, "version");
    tm->tiledversion = cj_get_string(root, "tiledversion");
    tm->orientation  = cj_get_string(root, "orientation");
    tm->renderorder  = cj_get_string_or_number(root, "renderorder");
    tm->width        = cj_get_integer(root, "width", 0);
    tm->height       = cj_get_integer(root, "height", 0);
    tm->tilewidth    = cj_get_integer(root, "tilewidth", 64);
    tm->tileheight   = cj_get_integer(root, "tileheight", 64);

    joy_core_log_info("jtilemap: loading %s map %dx%d, tile %dx%d",
             tm->orientation ? tm->orientation : "unknown",
             tm->width, tm->height, tm->tilewidth, tm->tileheight);

    cJSON* ts_arr = cJSON_GetObjectItem(root, "tilesets");
    if (ts_arr && ts_arr->type == cJSON_Array) {
        int n = cJSON_GetArraySize(ts_arr);
        for (int i = 0; i < n; i++) {
            cJSON* tj = cJSON_GetArrayItem(ts_arr, i);
            if (!tj || tj->type != cJSON_Object) continue;
            joy_tilemap_tileset_t ts;
            if (parse_tileset_cjson(tj, &ts)) {
                build_tileset_anim_map(&ts);
                kv_push(joy_tilemap_tileset_t, tm->tilesets, ts);
            }
        }
    }
    joy_core_log_info("jtilemap: parsed %zu tilesets", kv_size(tm->tilesets));

    cJSON* ly_arr = cJSON_GetObjectItem(root, "layers");
    if (ly_arr && ly_arr->type == cJSON_Array) {
        int n = cJSON_GetArraySize(ly_arr);
        for (int i = 0; i < n; i++) {
            cJSON* lj = cJSON_GetArrayItem(ly_arr, i);
            if (!lj || lj->type != cJSON_Object) continue;
            char* layer_type_str = cj_get_string(lj, "type");
            void* layer_obj = NULL;

            if (layer_type_str) {
                if (strcmp(layer_type_str, "tilelayer") == 0) {
                    layer_obj = parse_tile_layer_cjson(lj);
                } else if (strcmp(layer_type_str, "objectgroup") == 0) {
                    layer_obj = parse_object_layer_cjson(lj);
                } else if (strcmp(layer_type_str, "imagelayer") == 0) {
                    layer_obj = parse_image_layer_cjson(lj);
                } else if (strcmp(layer_type_str, "group") == 0) {
                    joy_core_log_info("jtilemap: skipping group layer");
                }
                free(layer_type_str);
            }

            if (layer_obj)
                kv_push(void*, tm->layers, layer_obj);
        }
    }
    joy_core_log_info("jtilemap: parsed %zu layers", kv_size(tm->layers));

    collect_anim_gids(tm);
    return tm;
}

joy_tilemap_p joy_scene_tilemap_load_file_json(const char* filename)
{
    if (!filename) {
        joy_core_log_error("joy_scene_tilemap_load_file_json: filename is NULL");
        return NULL;
    }

    size_t sz = 0;
    void* raw = SDL_LoadFile(filename, &sz);
    if (!raw) {
        joy_core_log_error("joy_scene_tilemap_load_file_json: cannot read %s", filename);
        return NULL;
    }
    char* buf = (char*)malloc(sz + 1);
    if (!buf) { SDL_free(raw); return NULL; }
    memcpy(buf, raw, sz);
    buf[sz] = '\0';
    SDL_free(raw);

    cJSON* root = cJSON_Parse(buf);
    free(buf);
    if (!root) {
        const char* err = cJSON_GetErrorPtr();
        joy_core_log_error("joy_scene_tilemap_load_file_json: parse error: %s", err ? err : "?");
        return NULL;
    }

    joy_tilemap_p tm = joy_scene_tilemap_load_cjson(root);
    cJSON_Delete(root);
    return tm;
}

/* ==================== 获取 deco 独立瓦片图片路径 ==================== */

const char* joy_scene_tilemap_get_tile_image(const joy_tilemap_tileset_p ts, int local_id)
{
    if (!ts) return NULL;
    for (size_t i = 0; i < kv_size(ts->tile_defs); i++) {
        if (kv_A(ts->tile_defs, i).id == local_id)
            return kv_A(ts->tile_defs, i).image;
    }
    return NULL;
}

bool joy_scene_tilemap_get_tile_def_size(const joy_tilemap_tileset_p ts, int local_id,
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

static void destroy_anim(joy_tilemap_anim_t* anim)
{
    kv_destroy(anim->frames);
}

static void destroy_tileset(joy_tilemap_tileset_t* ts)
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

static void destroy_object(joy_tilemap_object_t* obj)
{
    free(obj->name);
    free(obj->type);
}

static void destroy_layer(void* layer)
{
    joy_tilemap_layer_p base = (joy_tilemap_layer_p)layer;
    free(base->name);

    switch (base->type) {
    case JTILEMAP_LAYER_TILE: {
        joy_tilemap_tile_layer_p tl = (joy_tilemap_tile_layer_p)layer;
        free(tl->data);
        break;
    }
    case JTILEMAP_LAYER_OBJECT: {
        joy_tilemap_object_layer_p ol = (joy_tilemap_object_layer_p)layer;
        for (size_t i = 0; i < kv_size(ol->objects); i++)
            destroy_object(&kv_A(ol->objects, i));
        kv_destroy(ol->objects);
        break;
    }
    case JTILEMAP_LAYER_IMAGE: {
        joy_tilemap_image_layer_p il = (joy_tilemap_image_layer_p)layer;
        free(il->image);
        break;
    }
    }

    free(layer);
}

int joy_scene_tilemap_get_width(const joy_tilemap_p tm) {
    return tm ? tm->width : 0;
}

int joy_scene_tilemap_get_height(const joy_tilemap_p tm) {
    return tm ? tm->height : 0;
}

int joy_scene_tilemap_get_tilewidth(const joy_tilemap_p tm) {
    return tm ? tm->tilewidth : 0;
}

int joy_scene_tilemap_get_tileheight(const joy_tilemap_p tm) {
    return tm ? tm->tileheight : 0;
}

void joy_scene_tilemap_destroy(joy_tilemap_p tm)
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

joy_tilemap_gid_resolve_t joy_scene_tilemap_resolve_gid(const joy_tilemap_p tm, int gid)
{
    joy_tilemap_gid_resolve_t result;
    memset(&result, 0, sizeof(result));

    if (!tm || gid <= 0) return result;

    /* 找到 firstgid ≤ gid 且最大的 tileset */
    joy_tilemap_tileset_p best_ts = NULL;
    int best_firstgid = 0;

    for (size_t i = 0; i < kv_size(tm->tilesets); i++) {
        joy_tilemap_tileset_p ts = &kv_A(tm->tilesets, i);
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

int joy_scene_tilemap_get_anim_tileid(const joy_tilemap_p tm, int gid, uint32_t time_ms)
{
    joy_tilemap_gid_resolve_t res = joy_scene_tilemap_resolve_gid(tm, gid);
    if (!res.has_anim) return res.local_id;

    joy_tilemap_anim_t* anim = &kv_A(res.tileset->anims, (size_t)res.anim_idx);
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

bool joy_scene_tilemap_get_tile_uv(const joy_tilemap_p tm, int gid,
                                   uint32_t time_ms, float out_rect[4])
{
    if (!tm || !out_rect) return false;

    memset(out_rect, 0, 4 * sizeof(float));
    if (gid <= 0) return false;

    joy_tilemap_gid_resolve_t res = joy_scene_tilemap_resolve_gid(tm, gid);
    if (!res.tileset) return false;

    joy_tilemap_tileset_p ts = res.tileset;

    /* 确定当前显示的局部瓦片 ID（考虑动画） */
    int local_id = res.local_id;
    if (res.has_anim) {
        local_id = joy_scene_tilemap_get_anim_tileid(tm, gid, time_ms);
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

joy_tilemap_tile_layer_p joy_scene_tilemap_find_tile_layer(const joy_tilemap_p tm,
                                                         const char* name)
{
    if (!tm || !name) return NULL;

    for (size_t i = 0; i < kv_size(tm->layers); i++) {
        joy_tilemap_layer_p base = (joy_tilemap_layer_p)kv_A(tm->layers, i);
        if (base->type == JTILEMAP_LAYER_TILE && base->name &&
            strcmp(base->name, name) == 0) {
            return (joy_tilemap_tile_layer_p)base;
        }
    }
    return NULL;
}

joy_tilemap_object_layer_p joy_scene_tilemap_find_object_layer(const joy_tilemap_p tm,
                                                            const char* name)
{
    if (!tm || !name) return NULL;

    for (size_t i = 0; i < kv_size(tm->layers); i++) {
        joy_tilemap_layer_p base = (joy_tilemap_layer_p)kv_A(tm->layers, i);
        if (base->type == JTILEMAP_LAYER_OBJECT && base->name &&
            strcmp(base->name, name) == 0) {
            return (joy_tilemap_object_layer_p)base;
        }
    }
    return NULL;
}

void joy_scene_tilemap_object_layer_foreach(joy_tilemap_object_layer_p ol, void (*callback)(joy_tilemap_object_p obj, void* arg), void*arg)
{
    if (ol) {
        for (size_t i = 0; i < kv_size(ol->objects); i++) {
            joy_tilemap_object_t* obj = &kv_A(ol->objects, i);
            if (obj->shape != JTILEMAP_SHAPE_RECT)
                continue;
            callback(obj, arg);
        }
        joy_core_log_info("[tilemap] loaded %zu collision walls", kv_size(ol->objects));
    }
}

/* 遍历对象层内全部对象（不过滤形状：point/ellipse/polyline 等也回调；用于出生点解析） */
void joy_scene_tilemap_object_layer_foreach_all(joy_tilemap_object_layer_p ol, void (*callback)(joy_tilemap_object_p obj, void* arg), void* arg)
{
    if (ol) {
        for (size_t i = 0; i < kv_size(ol->objects); i++) {
            joy_tilemap_object_t* obj = &kv_A(ol->objects, i);
            callback(obj, arg);
        }
    }
}

int joy_scene_tilemap_get_tile(const joy_tilemap_tile_layer_p layer, int tx, int ty)
{
    if (!layer || !layer->data) return 0;
    if (tx < 0 || tx >= layer->width || ty < 0 || ty >= layer->height)
        return 0;
    return layer->data[ty * layer->width + tx];
}




/* 返回输入串去掉前导 ./ 与分隔符后的偏移, 不复制。
 * 原来的 static char buf[1024] 被所有 render 实例共享, 且曾直接作为哈希表的
 * key 保存, destroy 时 free 静态数组会破坏堆。 */
static const char* default_remap(const char* path)
{
        if (!path || !*path) return path;
        while (*path == '.' || *path == '/' || *path == '\\') path++;
        return path;
}

/* 哈希表一律接管 key 的堆副本, destroy 才能无条件 free(key)。
 * 失败条目(value=NULL)也入表, 避免每帧重复尝试加载同一缺失资源。 */
static void tex_cache_store(joy_tilemap_render_p r, const char* path, joy_image_p img)
{
        char* key = joy_core_utils_strdup(path, NULL);
        if (!key) return;
        int ret;
        khiter_t ki = kh_put(tex_cache, r->tex_cache, key, &ret);
        if (ki == kh_end(r->tex_cache)) { free(key); return; }
        if (ret == 0) free(key);   /* 桶已存在, 沿用原 key, 不能替换 */
        kh_value(r->tex_cache, ki) = img;
}

static joy_image_p load_texture_internal(joy_tilemap_render_p r, const char* path)
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

        /* 走 SDL_LoadFile + stbi_load_from_memory，而不是 stbi_load(路径) ——
         * stbi_load 内部是 stdio 的 fopen，安卓上图集打包在 APK 内读不到，
         * 结果是每个 tileset 纹理都加载失败（渲染时退化成同一张/空白）。 */
        size_t file_sz = 0;
        stbi_uc* filedata = (stbi_uc*)SDL_LoadFile(full_path, &file_sz);
        if (!filedata) {
                tex_cache_store(r, path, NULL);
                return NULL;
        }

        int w = 0, h = 0, ch = 0;
        unsigned char* data = stbi_load_from_memory(filedata, (int)file_sz, &w, &h, &ch, 4);
        SDL_free(filedata);
        if (!data) {
                tex_cache_store(r, path, NULL);
                return NULL;
        }

        joy_image_p img = joy_r2d_image_create_from_data(r->r2d, data, w, h);
        stbi_image_free(data);
        if (!img) {
                tex_cache_store(r, path, NULL);
                return NULL;
        }

        tex_cache_store(r, path, img);
        return img;
}

static joy_image_p load_tileset_tex(joy_tilemap_render_p r, const joy_tilemap_tileset_t* ts)
{
        if (!ts || !ts->image) return NULL;
        const char* mapped = r->remap ? r->remap(ts->image) : ts->image;
        return load_texture_internal(r, mapped);
}

static void render_tile_layer_internal(joy_tilemap_render_p r, joy_tilemap_p tm,
        const joy_tilemap_tile_layer_p layer,
        float cam_x, float cam_y, uint32_t time_ms)
{
        if (!layer->base.visible) return;
        /* 每个格子的画布像素：tile_px 预设 >0 时以它为准（调用者可设成游戏 PPM），
         * 否则回退到 map.tilewidth（Tiled 默认网格），保证两种调用都正确。 */
        float tilew = (r->tile_px > 0.0f) ? r->tile_px : (float)tm->tilewidth;
        float tileh = (r->tile_px > 0.0f) ? r->tile_px : (float)tm->tileheight;
        cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
        cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;

        int sx = (int)floorf(cam_x / tilew); if (sx < 0) sx = 0;
        int sy = (int)floorf(cam_y / tileh); if (sy < 0) sy = 0;
        int ex = (int)ceilf((cam_x + r->view_w) / tilew) + 1; if (ex > layer->width)  ex = layer->width;
        int ey = (int)ceilf((cam_y + r->view_h) / tileh) + 1; if (ey > layer->height) ey = layer->height;

        uint8_t alpha = (uint8_t)(layer->base.opacity * 255.0f);

        for (int ty = sy; ty < ey; ty++) {
                for (int tx = sx; tx < ex; tx++) {
                        int raw_gid = joy_scene_tilemap_get_tile(layer, tx, ty);
                        if (raw_gid == 0) continue;
                        int gid = jtilemap_clean_gid(raw_gid);
                        int flags = jtilemap_gid_flags(raw_gid);
                        joy_tilemap_gid_resolve_t res = joy_scene_tilemap_resolve_gid(tm, gid);
                        if (!res.tileset) continue;

                        /* 瓦片网格步长取 map 的 tilewidth/tileheight（这是 Tiled 里
                         * 相邻格子的画布间距），而不是 tileset 的。Tiled 允许二者不同
                         * （map.tilewidth 决定网格，tileset.tilewidth 是瓦片原始像素），
                         * 此时瓦片按 map 网格排布、并缩放到 map.tile 尺寸显示。
                         */
                        float ts_tw = (float)res.tileset->tilewidth;   /* 瓦片原始像素 */
                        float ts_th = (float)res.tileset->tileheight;
                        float scale_x = tilew / ts_tw;                 /* map.grid / tile 像素，Tiled 会拉伸瓦片 */
                        float scale_y = tileh / ts_th;
                        float dst_x = tx * tilew - cam_x;
                        float dst_y = ty * tileh - cam_y;
                        float dst_w = ts_tw * scale_x;                 /* 缩放后目标尺寸 = map.tile 尺寸 */
                        float dst_h = ts_th * scale_y;
                        /* 抗缝隙：瓦片quad取整并各边+1px 重叠，避免非整数 ppm 下
                         * 相邻瓦片间露出底色形成白线/网格（近白地面透出）。 */
                        int dst_ix = (int)floorf(dst_x + 0.5f);
                        int dst_iy = (int)floorf(dst_y + 0.5f);
                        int dst_iw = (int)floorf(dst_w + 0.5f) + 1;
                        int dst_ih = (int)floorf(dst_h + 0.5f) + 1;

                        const char* deco_img = joy_scene_tilemap_get_tile_image(res.tileset, res.local_id);
                        if (deco_img) {
                                const char* mapped = r->remap ? r->remap(deco_img) : deco_img;
                                joy_image_p timg = load_texture_internal(r, mapped);
                                if (!timg) continue;
                                joy_r2d_image_set_alpha(timg, alpha);
                                int dw = dst_iw, dh = dst_ih;
                                joy_scene_tilemap_get_tile_def_size(res.tileset, res.local_id, &dw, &dh);
                                joy_r2d_image_draw1(timg, NULL, &(joy_rect_t){ (float)dst_ix, (float)dst_iy, (float)dw, (float)dh });
                                continue;
                        }

                        float uv[4];
                        if (!joy_scene_tilemap_get_tile_uv(tm, gid, time_ms, uv)) continue;

                        joy_image_p timg = load_tileset_tex(r, res.tileset);
                        if (!timg) {
                                joy_r2d_set_color(r->r2d, 255, 0, 255, 255);
                                joy_r2d_shape_draw_rectangle(r->r2d, "fill", (joy_rect_t) { (float)dst_ix, (float)dst_iy, (float)dst_iw, (float)dst_ih });
                                continue;
                        }

                        joy_r2d_image_set_alpha(timg, alpha);
                        joy_rect_t src = { uv[0], uv[1], uv[2], uv[3] };

                        /* Tiled 翻转标志（bit D/H/V）→ SDL 旋转角 + 水平/垂直翻转。
                         * 官方语义（正交地图）：先对角镜像（transpose，x/y 互换），
                         * 再水平翻转，再垂直翻转。用 (angle, flip_h, flip_v) 精确表达
                         * 这 8 种组合（暴力枚举见 _test_flip.py）：
                         *   D=0:  angle=0°，flip_h=H, flip_v=V
                         *   D=1:  angle=90°，flip_h=V, flip_v=!H
                         * joy_r2d_image_draw_ex 的 rotation 参数按弧度处理（内部 *180/π）。
                         */
                        bool tiled_h = (flags & JTILEMAP_FLIP_H) != 0;
                        bool tiled_v = (flags & JTILEMAP_FLIP_V) != 0;
                        bool tiled_d = (flags & JTILEMAP_FLIP_D) != 0;

                        bool sdl_flip_h, sdl_flip_v;
                        float angle_rad;
                        if (!tiled_d) {
                                angle_rad = 0.0f;
                                sdl_flip_h = tiled_h;
                                sdl_flip_v = tiled_v;
                        } else {
                                angle_rad = 90.0f * 3.1415926f / 180.0f;
                                sdl_flip_h = tiled_v;
                                sdl_flip_v = !tiled_h;
                        }

                        /* 旋转/翻转绕瓦片中心：position 传目标中心，origin 传源中心。 */
                        joy_vec2_t pos = { (float)dst_ix + dst_iw * 0.5f, (float)dst_iy + dst_ih * 0.5f };
                        joy_vec2_t org = { ts_tw * 0.5f, ts_th * 0.5f };
                        joy_vec2_t scl = {
                                (sdl_flip_h ? -1.0f : 1.0f) * ((float)dst_iw / ts_tw),
                                (sdl_flip_v ? -1.0f : 1.0f) * ((float)dst_ih / ts_th)
                        };
                        joy_r2d_image_draw_ex(timg, &src, pos, angle_rad, scl, org);
                }
        }
}

static void render_object_layer_internal(const joy_tilemap_object_layer_p layer,
        float cam_x, float cam_y, joy_r2d_p r2d)
{
        if (!layer->base.visible) return;
        cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
        cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;
        joy_r2d_set_color(r2d, 255, 85, 0, 255);
        for (size_t i = 0; i < kv_size(layer->objects); i++) {
                joy_tilemap_object_t* obj = &kv_A(layer->objects, i);
                if (!obj->visible || obj->shape != JTILEMAP_SHAPE_RECT) continue;
                if (obj->width <= 0.0f || obj->height <= 0.0f || obj->width > 10000.0f || obj->height > 10000.0f) continue;
                joy_r2d_shape_draw_rectangle(r2d, "line", (joy_rect_t) { obj->x - cam_x, obj->y - cam_y, obj->width, obj->height });
        }
}

static void render_image_layer_internal(joy_tilemap_render_p r,
        const joy_tilemap_image_layer_p layer,
        float cam_x, float cam_y)
{
        if (!layer->base.visible || !layer->image) return;
        cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
        cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;

        const char* mapped = r->remap ? r->remap(layer->image) : layer->image;
        joy_image_p timg = load_texture_internal(r, mapped);
        if (!timg) return;

        float tex_w = 0, tex_h = 0;
        joy_r2d_image_get_size(timg, &tex_w, &tex_h);

        float vp_w = r->view_w, vp_h = r->view_h;
        float start_x = (layer->repeatx ? fmodf(-cam_x, tex_w) : -cam_x);
        float start_y = (layer->repeaty ? fmodf(-cam_y, tex_h) : -cam_y);
        if (start_x > 0) start_x -= tex_w;
        if (start_y > 0) start_y -= tex_h;

        for (float py = start_y; py < vp_h; py += tex_h)
                for (float px = start_x; px < vp_w; px += tex_w)
                        joy_r2d_image_draw1(timg, NULL, &(joy_rect_t){ px, py, tex_w, tex_h });
}

/* ======================== 公共 API ======================== */

joy_tilemap_render_p joy_scene_tilemap_render_create(joy_r2d_p renderer,
        const char* asset_base_path,
        joy_tilemap_path_remap_t remap)
{
        joy_tilemap_render_p r = (joy_tilemap_render_p)calloc(1, sizeof(joy_tilemap_render_t));
        if (!r) return NULL;
        r->r2d = renderer;
        r->remap = remap ? remap : default_remap;
        r->asset_base = asset_base_path ? joy_core_utils_strdup(asset_base_path, NULL) : NULL;
        r->view_w = 1024.0f;
        r->view_h = 760.0f;
        r->tile_px = 0.0f;   /* 默认 0：用 map.tilewidth 做每格像素 */
        r->tex_cache = kh_init(tex_cache);
        return r;
}

void joy_scene_tilemap_render_set_tile_px(joy_tilemap_render_p r, float px)
{
        if (r) r->tile_px = px;
}

void joy_scene_tilemap_render_all(joy_tilemap_render_p r, joy_tilemap_p tm,
        float camera_x, float camera_y,
        uint32_t time_ms)
{
        if (!r || !tm || !r->r2d) return;

        {
                int vp_w = 0, vp_h = 0, mode;
                joy_r2d_get_logical_presentation(r->r2d, &vp_w, &vp_h, &mode);
                if (vp_w <= 0 || vp_h <= 0)
                        joy_r2d_get_size(r->r2d, &vp_w, &vp_h);
                r->view_w = (float)vp_w;
                r->view_h = (float)vp_h;
        }

        for (size_t i = 0; i < kv_size(tm->layers); i++) {
                joy_tilemap_layer_p base = (joy_tilemap_layer_p)kv_A(tm->layers, i);
                switch (base->type) {
                case JTILEMAP_LAYER_TILE:
                        render_tile_layer_internal(r, tm, (joy_tilemap_tile_layer_p)base, camera_x, camera_y, time_ms);
                        break;
                case JTILEMAP_LAYER_OBJECT:
                        render_object_layer_internal((joy_tilemap_object_layer_p)base, camera_x, camera_y, r->r2d);
                        break;
                case JTILEMAP_LAYER_IMAGE:
                        render_image_layer_internal(r, (joy_tilemap_image_layer_p)base, camera_x, camera_y);
                        break;
                }
        }
}

/* 只渲染瓦片层（Ground/Object 等），跳过对象层（Collision/Player 的调试线框）与图片层。
 * 运行时关卡渲染应使用本接口：对象的碰撞/出生线框属于编辑期辅助，不该出现在游戏画面里。 */
void joy_scene_tilemap_render_tiles(joy_tilemap_render_p r, joy_tilemap_p tm,
        float camera_x, float camera_y,
        uint32_t time_ms)
{
        if (!r || !tm || !r->r2d) return;

        {
                int vp_w = 0, vp_h = 0, mode;
                joy_r2d_get_logical_presentation(r->r2d, &vp_w, &vp_h, &mode);
                if (vp_w <= 0 || vp_h <= 0)
                        joy_r2d_get_size(r->r2d, &vp_w, &vp_h);
                r->view_w = (float)vp_w;
                r->view_h = (float)vp_h;
        }

        for (size_t i = 0; i < kv_size(tm->layers); i++) {
                joy_tilemap_layer_p base = (joy_tilemap_layer_p)kv_A(tm->layers, i);
                if (base->type == JTILEMAP_LAYER_TILE)
                        render_tile_layer_internal(r, tm, (joy_tilemap_tile_layer_p)base, camera_x, camera_y, time_ms);
        }
}

bool joy_scene_tilemap_render_layer(joy_tilemap_render_p r, joy_tilemap_p tm,
        const char* layer_name,
        float camera_x, float camera_y,
        uint32_t time_ms)
{
        if (!r || !tm || !r->r2d || !layer_name) return false;

        for (size_t i = 0; i < kv_size(tm->layers); i++) {
                joy_tilemap_layer_p base = (joy_tilemap_layer_p)kv_A(tm->layers, i);
                if (!base->name || strcmp(base->name, layer_name) != 0) continue;
                switch (base->type) {
                case JTILEMAP_LAYER_TILE:
                        render_tile_layer_internal(r, tm, (joy_tilemap_tile_layer_p)base, camera_x, camera_y, time_ms);
                        return true;
                case JTILEMAP_LAYER_OBJECT:
                        render_object_layer_internal((joy_tilemap_object_layer_p)base, camera_x, camera_y, r->r2d);
                        return true;
                case JTILEMAP_LAYER_IMAGE:
                        render_image_layer_internal(r, (joy_tilemap_image_layer_p)base, camera_x, camera_y);
                        return true;
                }
        }
        return false;
}

void joy_scene_tilemap_render_destroy(joy_tilemap_render_p r)
{
        if (!r) return;
        if (r->tex_cache) {
                const char* key;
                joy_image_p timg;
                kh_foreach(r->tex_cache, key, timg, {
                        if (timg) joy_r2d_image_destroy(timg);
                        free((void*)key);
                        });
                kh_destroy(tex_cache, r->tex_cache);
        }
        if (r->asset_base) free(r->asset_base);
        free(r);
}
