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
#include "jr2d.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- 路径映射回调 ---------- */
typedef const char* (*jtilemap_path_remap_t)(const char* original_path);

/* ---------- 渲染实例（不透明句柄，可重入） ---------- */
typedef struct jtilemap_render_t jtilemap_render_t, *jtilemap_render_p;

/* 创建渲染实例，asset_base_path 会拼在所有图片路径前 */
/* renderer: SDL 渲染器, remap: 路径映射回调（NULL 用默认） */
JOY_API jtilemap_render_p jtilemap_render_create(jr2d_p renderer,
                                                   const char* asset_base_path,
                                                   jtilemap_path_remap_t remap);

/* 渲染地图所有层（按 layers[] 顺序） */
JOY_API void jtilemap_render_all(jtilemap_render_p r,
                                  jtilemap_p tm,
                                  float camera_x, float camera_y,
                                  uint32_t time_ms);

/* 渲染指定名称的层，返回 true 成功，false 层未找到 */
JOY_API bool jtilemap_render_layer(jtilemap_render_p r,
                                    jtilemap_p tm,
                                    const char* layer_name,
                                    float camera_x, float camera_y,
                                    uint32_t time_ms);

/* 销毁渲染实例，释放所有缓存纹理 */
JOY_API void jtilemap_render_destroy(jtilemap_render_p r);

#ifdef __cplusplus
}
#endif

#endif /* !JTILEMAP_RENDER_H */
