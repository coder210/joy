/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jtilemap_render.c
Description: Tilemap SDL 渲染封装实现
Author: ydlc
Version: 1.0
Date: 2026.5.22
History:
*************************************************/
#include "jtilemap_render.h"
#include "jcore.h"
#include "jutils.h"
#include "external/stb_image.h"
#include "external/klib/khash.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ======================== 静态状态 ========================
static SDL_Renderer*          gRenderer   = NULL;
static char*                  gAssetBase  = NULL;
static jtilemap_path_remap_t  gRemapFunc  = NULL;
static float                  gViewW      = 1024.0f;
static float                  gViewH      = 760.0f;

// 纹理缓存: key = 路径字符串, value = SDL_Texture*
KHASH_MAP_INIT_STR(tex_cache, SDL_Texture*)
static khash_t(tex_cache)* gTexCache = NULL;

// ======================== 默认路径映射 ========================
// 该映射将 Tiled 导出的相对路径转换为实际资产目录路径
// 例如: "terrain/ground/tilemap_flat.png" → "joy2d_editor_terrain/ground/tilemap_flat.png"
static const char* default_remap(const char* path)
{
    if (!path || !*path) return path;

    const char* p = path;
    while (*p == '.' || *p == '/' || *p == '\\') p++;

    // 分配足够大的缓冲区
    static char buf[1024];
    if (strncmp(p, "terrain/", 8) == 0)
        snprintf(buf, sizeof(buf), "joy2d_editor_terrain/%s", p + 8);
    else if (strncmp(p, "textures/", 9) == 0)
        snprintf(buf, sizeof(buf), "joy2d_editor_textures/%s", p + 9);
    else if (strncmp(p, "effects/", 8) == 0)
        snprintf(buf, sizeof(buf), "joy2d_editor_effects/%s", p + 8);
    else
        snprintf(buf, sizeof(buf), "joy2d_editor_terrain/%s", p);
    return buf;
}

// ======================== 纹理加载 ========================
static SDL_Texture* load_texture_internal(const char* path)
{
    if (!path || !*path) return NULL;

    // 查缓存
    khiter_t k = kh_get(tex_cache, gTexCache, path);
    if (k != kh_end(gTexCache))
        return kh_value(gTexCache, k);

    // 拼 base path
    char full_path[1024];
    if (gAssetBase)
        snprintf(full_path, sizeof(full_path), "%s%s", gAssetBase, path);
    else
        snprintf(full_path, sizeof(full_path), "%s", path);

    // 加载图片
    int w = 0, h = 0, ch = 0;
    unsigned char* data = stbi_load(full_path, &w, &h, &ch, 4);
    if (!data) {
        log_error("jtilemap_render: failed to load %s (reason: %s)", full_path, stbi_failure_reason());
        // 缓存 nullptr 标记失败
        int ret;
        khiter_t ki = kh_put(tex_cache, gTexCache, path, &ret);
        kh_value(gTexCache, ki) = NULL;
        return NULL;
    }

    SDL_Surface* surf = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, data, w * 4);
    if (!surf) {
        stbi_image_free(data);
        log_error("jtilemap_render: surface %s", SDL_GetError());
        return NULL;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(gRenderer, surf);
    SDL_DestroySurface(surf);
    stbi_image_free(data);

    if (!tex) {
        log_error("jtilemap_render: texture %s", SDL_GetError());
        return NULL;
    }

    // 存入缓存
    char* key = utils_strdup(path, NULL);
    int ret;
    khiter_t ki = kh_put(tex_cache, gTexCache, key, &ret);
    kh_value(gTexCache, ki) = tex;
    return tex;
}

static SDL_Texture* load_tileset_tex(const jtilemap_tileset_t* ts)
{
    if (!ts || !ts->image) return NULL;
    const char* mapped = gRemapFunc ? gRemapFunc(ts->image) : ts->image;
    return load_texture_internal(mapped);
}

// ======================== 渲染瓦片层 ========================
static void render_tile_layer_internal(jtilemap_p tm, const jtilemap_tile_layer_p layer,
                                        float cam_x, float cam_y, uint32_t time_ms)
{
    if (!layer->base.visible) return;

    float tilew = (float)tm->tilewidth, tileh = (float)tm->tileheight;
    cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
    cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;

    int sx = (int)floorf(cam_x / tilew); if (sx < 0) sx = 0;
    int sy = (int)floorf(cam_y / tileh); if (sy < 0) sy = 0;
    int ex = (int)ceilf((cam_x + gViewW) / tilew) + 1; if (ex > layer->width)  ex = layer->width;
    int ey = (int)ceilf((cam_y + gViewH) / tileh) + 1; if (ey > layer->height) ey = layer->height;

    uint8_t alpha = (uint8_t)(layer->base.opacity * 255.0f);

    for (int ty = sy; ty < ey; ty++) {
        for (int tx = sx; tx < ex; tx++) {
            int raw_gid = jtilemap_get_tile(layer, tx, ty);
            if (raw_gid == 0) continue;

            int gid   = jtilemap_clean_gid(raw_gid);
            int flags = jtilemap_gid_flags(raw_gid);
            jtilemap_gid_resolve_t res = jtilemap_resolve_gid(tm, gid);
            if (!res.tileset) continue;

            float dst_x = tx * tilew - cam_x, dst_y = ty * tileh - cam_y;
            float dst_w = (float)res.tileset->tilewidth;
            float dst_h = (float)res.tileset->tileheight;

            // --- Deco 独立瓦片 ---
            const char* deco_img = jtilemap_get_tile_image(res.tileset, res.local_id);
            if (deco_img) {
                const char* mapped = gRemapFunc ? gRemapFunc(deco_img) : deco_img;
                SDL_Texture* tex = load_texture_internal(mapped);
                if (!tex) continue;
                SDL_SetTextureAlphaMod(tex, alpha);
                SDL_FRect dst = { dst_x, dst_y, dst_w, dst_h };
                SDL_RenderTexture(gRenderer, tex, NULL, &dst);
                continue;
            }

            // --- 普通瓦片 ---
            float uv[4];
            if (!jtilemap_get_tile_uv(tm, gid, time_ms, uv)) continue;

            SDL_Texture* tex = load_tileset_tex(res.tileset);
            if (!tex) {
                SDL_FRect dr = { dst_x, dst_y, dst_w, dst_h };
                SDL_SetRenderDrawColor(gRenderer, 255, 0, 255, 255);
                SDL_RenderFillRect(gRenderer, &dr);
                continue;
            }

            SDL_SetTextureAlphaMod(tex, alpha);
            SDL_FRect src = { uv[0], uv[1], uv[2], uv[3] };
            SDL_FRect dst = { dst_x, dst_y, dst_w, dst_h };

            // GID 翻转
            SDL_FlipMode flip = SDL_FLIP_NONE;
            if (flags & JTILEMAP_FLIP_H) flip = (SDL_FlipMode)(flip | SDL_FLIP_HORIZONTAL);
            if (flags & JTILEMAP_FLIP_V) flip = (SDL_FlipMode)(flip | SDL_FLIP_VERTICAL);

            if (flags & JTILEMAP_FLIP_D) {
                flip = (SDL_FlipMode)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
                SDL_RenderTextureRotated(gRenderer, tex, &src, &dst, 90.0, NULL, flip);
            } else if (flip != SDL_FLIP_NONE) {
                SDL_RenderTextureRotated(gRenderer, tex, &src, &dst, 0.0, NULL, flip);
            } else {
                SDL_RenderTexture(gRenderer, tex, &src, &dst);
            }
        }
    }
}

// ======================== 渲染对象层 ========================
static void render_object_layer_internal(const jtilemap_object_layer_p layer,
                                          float cam_x, float cam_y)
{
    if (!layer->base.visible) return;
    cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
    cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;
    SDL_SetRenderDrawColor(gRenderer, 255, 85, 0, 255);
    for (size_t i = 0; i < kv_size(layer->objects); i++) {
        jtilemap_object_t* obj = &kv_A(layer->objects, i);
        if (!obj->visible || obj->shape != JTILEMAP_SHAPE_RECT) continue;
        SDL_FRect r = { obj->x - cam_x, obj->y - cam_y, obj->width, obj->height };
        SDL_RenderRect(gRenderer, &r);
    }
}

// ======================== 渲染图片层 ========================
static void render_image_layer_internal(const jtilemap_image_layer_p layer,
                                         const jtilemap_p tm,
                                         float cam_x, float cam_y)
{
    if (!layer->base.visible || !layer->image) return;

    cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
    cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;

    const char* mapped = gRemapFunc ? gRemapFunc(layer->image) : layer->image;
    SDL_Texture* tex = load_texture_internal(mapped);
    if (!tex) return;

    float tex_w = 0, tex_h = 0;
    SDL_GetTextureSize(tex, &tex_w, &tex_h);

    // 获取视口尺寸
    float vp_w = gViewW, vp_h = gViewH;

    float start_x = (layer->repeatx ? fmodf(-cam_x, tex_w) : -cam_x);
    float start_y = (layer->repeaty ? fmodf(-cam_y, tex_h) : -cam_y);
    if (start_x > 0) start_x -= tex_w;
    if (start_y > 0) start_y -= tex_h;

    for (float py = start_y; py < vp_h; py += tex_h)
        for (float px = start_x; px < vp_w; px += tex_w) {
            SDL_FRect dst = { px, py, tex_w, tex_h };
            SDL_RenderTexture(gRenderer, tex, NULL, &dst);
        }
}

// ======================== 公共 API ========================

JOY_API void jtilemap_render_init(SDL_Renderer* renderer,
                                   const char* asset_base_path,
                                   jtilemap_path_remap_t remap)
{
    gRenderer  = renderer;
    gRemapFunc = remap ? remap : default_remap;

    if (gAssetBase) free(gAssetBase);
    gAssetBase = asset_base_path ? utils_strdup(asset_base_path, NULL) : NULL;

    // 初始化纹理缓存哈希表
    if (!gTexCache)
        gTexCache = kh_init(tex_cache);
}

JOY_API void jtilemap_render_all(jtilemap_p tm,
                                  float camera_x, float camera_y,
                                  uint32_t time_ms)
{
    if (!tm || !gRenderer) return;

    // 缓存视口尺寸（用于裁剪）
    {
        int vp_w = 0, vp_h = 0;
        SDL_GetCurrentRenderOutputSize(gRenderer, &vp_w, &vp_h);
        gViewW = (float)vp_w;
        gViewH = (float)vp_h;
    }

    for (size_t i = 0; i < kv_size(tm->layers); i++) {
        jtilemap_layer_p base = (jtilemap_layer_p)kv_A(tm->layers, i);
        switch (base->type) {
        case JTILEMAP_LAYER_TILE:
            render_tile_layer_internal(tm, (jtilemap_tile_layer_p)base, camera_x, camera_y, time_ms);
            break;
        case JTILEMAP_LAYER_OBJECT:
            render_object_layer_internal((jtilemap_object_layer_p)base, camera_x, camera_y);
            break;
        case JTILEMAP_LAYER_IMAGE:
            render_image_layer_internal((jtilemap_image_layer_p)base, tm, camera_x, camera_y);
            break;
        }
    }
}

JOY_API bool jtilemap_render_layer(jtilemap_p tm,
                                    const char* layer_name,
                                    float camera_x, float camera_y,
                                    uint32_t time_ms)
{
    if (!tm || !gRenderer || !layer_name) return false;

    for (size_t i = 0; i < kv_size(tm->layers); i++) {
        jtilemap_layer_p base = (jtilemap_layer_p)kv_A(tm->layers, i);
        if (!base->name || strcmp(base->name, layer_name) != 0) continue;

        switch (base->type) {
        case JTILEMAP_LAYER_TILE:
            render_tile_layer_internal(tm, (jtilemap_tile_layer_p)base, camera_x, camera_y, time_ms);
            return true;
        case JTILEMAP_LAYER_OBJECT:
            render_object_layer_internal((jtilemap_object_layer_p)base, camera_x, camera_y);
            return true;
        case JTILEMAP_LAYER_IMAGE:
            render_image_layer_internal((jtilemap_image_layer_p)base, tm, camera_x, camera_y);
            return true;
        }
    }
    return false;
}

JOY_API void jtilemap_render_destroy(void)
{
    if (gTexCache) {
        const char* key;
        SDL_Texture* tex;
        kh_foreach(gTexCache, key, tex, {
            if (tex) SDL_DestroyTexture(tex);
            free((void*)key);
        });
        kh_destroy(tex_cache, gTexCache);
        gTexCache = NULL;
    }

    if (gAssetBase) { free(gAssetBase); gAssetBase = NULL; }
    gRenderer  = NULL;
    gRemapFunc = NULL;
}
