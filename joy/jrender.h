/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jrender.h
Description: SDL_GPU 3D 渲染基础设施 (shader/pipeline/buffer/texture/sampler/MVP)
Author: ydlc
Version: 1.0
Date: 2025.10.13
History:
*************************************************/
#ifndef JRENDER_H
#define JRENDER_H
#include "jconfig.h"
#include "jmath.h"
#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- MVP Uniform 数据 (列主序 float[16], 直接上传 GPU) ---------- */
typedef struct render_mvp {
	float viewProj[16];
	float model[16];
} render_mvp_t;

/* ---------- 着色器加载 ---------- */
JOY_API SDL_GPUShader* render_load_shader(SDL_GPUDevice* device, const char* filename,
	SDL_GPUShaderStage stage, uint32_t sampler_num, uint32_t uniform_buffer_num);

/* ---------- 图形管线创建 ---------- */
/* 内置顶点格式: location=0 float3 position, location=1 float2 uv, 每个顶点 stride=20 */
JOY_API SDL_GPUGraphicsPipeline* render_create_pipeline(SDL_GPUDevice* device,
	SDL_GPUShader* vertex_shader, SDL_GPUShader* fragment_shader,
	SDL_GPUTextureFormat swapchain_format, SDL_GPUTextureFormat depth_format);

/* ---------- 顶点缓冲区创建与上传 ---------- */
JOY_API SDL_GPUBuffer* render_upload_buffer(SDL_GPUDevice* device,
	const void* data, size_t size);

/* ---------- 纹理 ---------- */
JOY_API SDL_GPUTexture* render_create_solid_texture(SDL_GPUDevice* device,
	unsigned char r, unsigned char g, unsigned char b);

JOY_API SDL_GPUTexture* render_create_depth_texture(SDL_GPUDevice* device,
	int width, int height);

/* ---------- 采样器 ---------- */
JOY_API SDL_GPUSampler* render_create_sampler(SDL_GPUDevice* device);

/* ---------- 矩阵转换: mat44_t(行主序内存) → GPU 列主序 float[16] ---------- */
JOY_API void render_mat44_to_float16(const mat44_t* m, float out[16]);

#ifdef __cplusplus
}
#endif

#endif // !JRENDER_H
