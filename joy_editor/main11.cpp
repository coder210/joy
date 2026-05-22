// main11.cpp — Tilemap 加载与渲染测试（对应 Lua tilemap.lua 逻辑）
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <string>
#include <unordered_map>
//#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL.h"
//#include "SDL3/SDL_main.h"
#include <joy/jtilemap.h>
#include <joy/jcore.h>
#include <joy/external/stb_image.h>

// ======================== 常量 ========================
static const int WINDOW_W = 1024;
static const int WINDOW_H = 760;

// ======================== 全局状态 ========================
static SDL_Window*   gWindow   = nullptr;
static SDL_Renderer* gRenderer = nullptr;
static struct {
    float x, y;
    float w, h;
} gCamera = { -100.0f, -100.0f, (float)WINDOW_W, (float)WINDOW_H };

// 资产根目录（exe 所在目录），由 main() 初始化
static std::string gAssetBasePath;

// ======================== 路径映射 ========================
// Tiled 导出的 map.lua 中 tileset image 路径以 "terrain/", "textures/", "effects/" 开头，
// 实际资源文件位于 assets/joy2d_editor_terrain/, assets/joy2d_editor_textures/, assets/joy2d_editor_effects/
static std::string remap_image_path(const char* orig_path)
{
    if (!orig_path || !*orig_path) return "";

    // 去掉开头的 "./" 或 "../" 等
    const char* p = orig_path;
    while (*p == '.' || *p == '/' || *p == '\\') p++;

    // 按前缀映射到实际资产目录
    std::string rel;
    if (strncmp(p, "terrain/", 8) == 0)
        rel = std::string("joy2d_editor_terrain/") + (p + 8);
    else if (strncmp(p, "textures/", 9) == 0)
        rel = std::string("joy2d_editor_textures/") + (p + 9);
    else if (strncmp(p, "effects/", 8) == 0)
        rel = std::string("joy2d_editor_effects/") + (p + 8);
    else
        rel = std::string("joy2d_editor_terrain/") + p;

    return gAssetBasePath + rel;
}

// ======================== 纹理缓存 ========================
// tileset name → SDL_Texture*
static std::unordered_map<std::string, SDL_Texture*> gTextures;

static SDL_Texture* load_tileset_texture(const jtilemap_tileset_t* ts)
{
    if (!ts->image) return nullptr;

    // 检查缓存
    std::string tname = ts->name ? ts->name : "";
    auto it = gTextures.find(tname);
    if (it != gTextures.end())
        return it->second;

    // 路径映射
    std::string path = remap_image_path(ts->image);
    log_info("tilemap_test: loading texture: %s (from %s)", path.c_str(), ts->image);

    // 使用 stb_image 加载 PNG
    int img_w = 0, img_h = 0, img_ch = 0;
    unsigned char* img_data = stbi_load(path.c_str(), &img_w, &img_h, &img_ch, 4);
    if (!img_data) {
        log_error("tilemap_test: failed to load image: %s (reason: %s)",
                  path.c_str(), stbi_failure_reason());
        gTextures[tname] = nullptr;
        return nullptr;
    }

    SDL_Surface* surface = SDL_CreateSurfaceFrom(img_w, img_h,
                                                   SDL_PIXELFORMAT_RGBA32,
                                                   img_data, img_w * 4);
    if (!surface) {
        stbi_image_free(img_data);
        log_error("tilemap_test: failed to create surface: %s", SDL_GetError());
        gTextures[tname] = nullptr;
        return nullptr;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(gRenderer, surface);
    SDL_DestroySurface(surface);
    stbi_image_free(img_data);
    if (!tex) {
        log_error("tilemap_test: failed to create texture: %s", SDL_GetError());
        gTextures[tname] = nullptr;
        return nullptr;
    }

    gTextures[tname] = tex;
    log_info("tilemap_test: loaded texture '%s' (%s)", ts->name ? ts->name : "?", path.c_str());
    return tex;
}

static void destroy_all_textures()
{
    for (auto& kv : gTextures) {
        if (kv.second)
            SDL_DestroyTexture(kv.second);
    }
    gTextures.clear();
}

// ======================== 渲染函数 ========================

static void render_tile_layer(const jtilemap_p tm, const jtilemap_tile_layer_p layer,
                               uint32_t time_ms)
{
    if (!layer->base.visible) return;

    float tilew = (float)tm->tilewidth;
    float tileh = (float)tm->tileheight;

    // 视口裁剪：只渲染屏幕可见范围内的瓦片
    int start_x = (int)floorf(gCamera.x / tilew);
    int start_y = (int)floorf(gCamera.y / tileh);
    int end_x   = (int)ceilf((gCamera.x + gCamera.w) / tilew) + 1;
    int end_y   = (int)ceilf((gCamera.y + gCamera.h) / tileh) + 1;

    if (start_x < 0) start_x = 0;
    if (start_y < 0) start_y = 0;
    if (end_x > layer->width)  end_x = layer->width;
    if (end_y > layer->height) end_y = layer->height;

    for (int ty = start_y; ty < end_y; ty++) {
        for (int tx = start_x; tx < end_x; tx++) {
            int gid = jtilemap_get_tile(layer, tx, ty);
            if (gid == 0) continue;

            // 解析 GID → tileset
            jtilemap_gid_resolve_t res = jtilemap_resolve_gid(tm, gid);
            if (!res.tileset) continue;

            // 先检查能否计算 UV（columns=0 的 deco 类 tileset 直接跳过）
            float uv[4];
            if (!jtilemap_get_tile_uv(tm, gid, time_ms, uv))
                continue;

            // 获取纹理
            SDL_Texture* tex = load_tileset_texture(res.tileset);
            if (!tex) {
                // 用纯色矩形替代缺失纹理
                SDL_FRect dr = {
                    (float)(tx * tm->tilewidth)  - gCamera.x,
                    (float)(ty * tm->tileheight) - gCamera.y,
                    (float)res.tileset->tilewidth,
                    (float)res.tileset->tileheight
                };
                SDL_SetRenderDrawColor(gRenderer, 255, 0, 255, 255);
                SDL_RenderFillRect(gRenderer, &dr);
                continue;
            }

            SDL_FRect src = { uv[0], uv[1], uv[2], uv[3] };
            SDL_FRect dst = {
                (float)(tx * tm->tilewidth)  - gCamera.x,
                (float)(ty * tm->tileheight) - gCamera.y,
                (float)res.tileset->tilewidth,
                (float)res.tileset->tileheight
            };

            SDL_RenderTexture(gRenderer, tex, &src, &dst);
        }
    }
}

static void render_object_layer(const jtilemap_object_layer_p layer)
{
    if (!layer->base.visible) return;

    SDL_SetRenderDrawColor(gRenderer, 255, 85, 0, 255); // 橙色 tint

    for (size_t i = 0; i < kv_size(layer->objects); i++) {
        jtilemap_object_t* obj = &kv_A(layer->objects, i);
        if (!obj->visible) continue;

        if (obj->shape == JTILEMAP_SHAPE_RECT) {
            SDL_FRect r = {
                obj->x - gCamera.x,
                obj->y - gCamera.y,
                obj->width,
                obj->height
            };
            SDL_RenderRect(gRenderer, &r);
        }
    }
}

static void render_image_layer(const jtilemap_image_layer_p layer,
                               const jtilemap_p tm, uint32_t time_ms)
{
    if (!layer->base.visible || !layer->image) return;
    // 背景图渲染逻辑简化（需单独加载纹理），暂留空
    (void)tm; (void)time_ms;
}

// ======================== 主循环 ========================
static int run_main_loop(jtilemap_p tm)
{
    bool running = true;
    uint64_t last_time = SDL_GetTicks();
    uint64_t total_time = 0;
    int frame_count = 0;
    int fps = 0;

    SDL_Event event;
    while (running) {
        // ---- 事件处理 ----
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                SDL_Scancode sc = event.key.scancode;
                if (sc == SDL_SCANCODE_ESCAPE || sc == SDL_SCANCODE_Q) {
                    running = false;
                } else if (sc == SDL_SCANCODE_UP || sc == SDL_SCANCODE_W) {
                    gCamera.y -= 10.0f;
                } else if (sc == SDL_SCANCODE_RIGHT || sc == SDL_SCANCODE_D) {
                    gCamera.x += 10.0f;
                } else if (sc == SDL_SCANCODE_DOWN || sc == SDL_SCANCODE_S) {
                    gCamera.y += 10.0f;
                } else if (sc == SDL_SCANCODE_LEFT || sc == SDL_SCANCODE_A) {
                    gCamera.x -= 10.0f;
                }
            }
        }

        // ---- 时间/FPS ----
        uint64_t current_time = SDL_GetTicks();
        uint64_t dt = current_time - last_time;
        total_time += dt;
        if (total_time >= 1000) {
            fps = frame_count;
            frame_count = 0;
            total_time -= 1000;
        } else {
            frame_count++;
        }
        last_time = current_time;

        // ---- 渲染 ----
        SDL_SetRenderDrawColor(gRenderer, 71, 171, 169, 255);
        SDL_RenderClear(gRenderer);

        // 渲染所有层
        for (size_t i = 0; i < kv_size(tm->layers); i++) {
            jtilemap_layer_p base = (jtilemap_layer_p)kv_A(tm->layers, i);
            switch (base->type) {
            case JTILEMAP_LAYER_TILE: {
                jtilemap_tile_layer_p tl = (jtilemap_tile_layer_p)base;
                render_tile_layer(tm, tl, (uint32_t)current_time);
                break;
            }
            case JTILEMAP_LAYER_OBJECT: {
                jtilemap_object_layer_p ol = (jtilemap_object_layer_p)base;
                render_object_layer(ol);
                break;
            }
            case JTILEMAP_LAYER_IMAGE: {
                jtilemap_image_layer_p il = (jtilemap_image_layer_p)base;
                render_image_layer(il, tm, (uint32_t)current_time);
                break;
            }
            }
        }

        // FPS 显示：用窗口标题代替 Lua 的 render_debug_text
        {
            char title[128];
            snprintf(title, sizeof(title), "tilemap - FPS:%d", fps);
            SDL_SetWindowTitle(gWindow, title);
        }

        SDL_RenderPresent(gRenderer);

        // 帧率控制
        SDL_Delay(1);
    }

    return 0;
}

// ======================== 入口 ========================
int main(int argc, char* argv[])
{
    (void)argc; (void)argv;

    // ---- 初始化 SDL ----
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        log_error("SDL_Init failed: %s", SDL_GetError());
        return -1;
    }

    if (!SDL_CreateWindowAndRenderer("tilemap", WINDOW_W, WINDOW_H,
                                      SDL_WINDOW_RESIZABLE, &gWindow, &gRenderer)) {
        log_error("SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // ---- 用 exe 所在目录作为资产根目录，不依赖工作目录 ----
    {
        const char* base = SDL_GetBasePath();
        gAssetBasePath = base ? base : "";
        log_info("tilemap_test: asset base path: %s", gAssetBasePath.c_str());
    }

    // ---- 直接通过 jtilemap 加载 map.lua（内部封装 Lua，外部无需关心） ----
    std::string map_path = gAssetBasePath + "joy2d_editor_tilemap/map.lua";
    log_info("tilemap_test: loading %s", map_path.c_str());

    jtilemap_p tm = jtilemap_load_file(map_path.c_str());
    if (!tm) {
        log_error("tilemap_test: failed to load tilemap from %s", map_path);
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        return -1;
    }

    log_info("tilemap_test: map loaded. size=%dx%d tiles, tile=%dx%d",
             tm->width, tm->height, tm->tilewidth, tm->tileheight);
    log_info("tilemap_test: tilesets=%zu, layers=%zu",
             kv_size(tm->tilesets), kv_size(tm->layers));

    // ---- 运行主循环 ----
    int ret = run_main_loop(tm);

    // ---- 清理 ----
    destroy_all_textures();
    jtilemap_destroy(tm);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();

    return ret;
}
