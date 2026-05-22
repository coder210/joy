/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jtilemap_render.h
Description: Tilemap SDL 渲染封装（纹理管理/层渲染/翻转/透明度/视差）
Author: ydlc
Version: 1.0
Date: 2026.5.22
History:
*************************************************/
#ifndef JTILEMAP_RENDER_H
#define JTILEMAP_RENDER_H

#include <stdint.h>
#include "jconfig.h"
#include "jtilemap.h"
#include "SDL3/SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- 路径映射回调 ---------- */
/* 参数: original_path — tileset 的 image 字段原始路径
 * 返回: 映射后的完整文件路径（内部会 strdup，调用者返回临时字符串即可）
 * 默认实现: 直接返回原路径
 */
typedef const char* (*jtilemap_path_remap_t)(const char* original_path);

/* ---------- API ---------- */

/* 初始化渲染模块，asset_base_path 会拼在所有图片路径前 */
/* renderer: SDL 渲染器, remap: 路径映射回调（NULL 用默认） */
JOY_API void jtilemap_render_init(SDL_Renderer* renderer,
                                   const char* asset_base_path,
                                   jtilemap_path_remap_t remap);

/* 渲染地图所有层（按 layers[] 顺序） */
JOY_API void jtilemap_render_all(jtilemap_p tm,
                                  float camera_x, float camera_y,
                                  uint32_t time_ms);

/* 渲染指定名称的层，返回 true 成功，false 层未找到 */
JOY_API bool jtilemap_render_layer(jtilemap_p tm,
                                    const char* layer_name,
                                    float camera_x, float camera_y,
                                    uint32_t time_ms);

/* 释放所有缓存纹理 */
JOY_API void jtilemap_render_destroy(void);

#ifdef __cplusplus
}
#endif

#endif /* !JTILEMAP_RENDER_H */
