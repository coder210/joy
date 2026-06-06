/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jrender.h
Description: SDL_GPU 3D 渲染 — raylib 风格可重入 API + 底层工具
Author: ydlc
Version: 2.0
*************************************************/
#ifndef JRENDER_H
#define JRENDER_H
#include "jconfig.h"
#include "jmath.h"
#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================
 * 底层工具 (无状态，已可重入)
 * ================================================================ */

typedef struct render_mvp {
	float viewProj[16];
	float model[16];
} render_mvp_t;

JOY_API SDL_GPUShader* render_load_shader(SDL_GPUDevice* device, const char* filename,
	SDL_GPUShaderStage stage, uint32_t sampler_num, uint32_t uniform_buffer_num);

JOY_API SDL_GPUGraphicsPipeline* render_create_pipeline(SDL_GPUDevice* device,
	SDL_GPUShader* vertex_shader, SDL_GPUShader* fragment_shader,
	SDL_GPUTextureFormat swapchain_format, SDL_GPUTextureFormat depth_format);

JOY_API SDL_GPUBuffer* render_upload_buffer(SDL_GPUDevice* device,
	const void* data, size_t size);

JOY_API SDL_GPUTexture* render_create_solid_texture(SDL_GPUDevice* device,
	unsigned char r, unsigned char g, unsigned char b);

JOY_API SDL_GPUTexture* render_create_depth_texture(SDL_GPUDevice* device,
	int width, int height);

JOY_API SDL_GPUSampler* render_create_sampler(SDL_GPUDevice* device);

JOY_API void render_mat44_to_float16(const mat44_t* m, float out[16]);

/* ================================================================
 * 高级 API (v2.0) — 可重入，无全局变量，适合脚本绑定
 * ================================================================ */

/* ---------- 不透明句柄 ---------- */
typedef struct render3d render3d_t, * render3d_p;

/* ---------- 网格 ---------- */
typedef struct render_mesh render_mesh_t, * render_mesh_p;

/* ---------- 生命周期 ---------- */
JOY_API render3d_p render3d_create(SDL_Window* window, const char* shader_dir);
JOY_API void       render3d_destroy(render3d_p r);

/* ---------- 帧管理 ---------- */
JOY_API bool render3d_begin(render3d_p r);
JOY_API void render3d_end(render3d_p r);

/* ---------- 相机 ---------- */
JOY_API void render3d_camera_lookat(render3d_p r, vec3_t eye, vec3_t target, vec3_t up, float fovy);
JOY_API void render3d_camera_fps(render3d_p r, vec3_t pos, float yaw, float pitch, float fovy);

/* ---------- 网格 ---------- */
JOY_API render_mesh_p render3d_mesh_cube(render3d_p r, float w, float h, float d);
JOY_API render_mesh_p render3d_mesh_sphere(render3d_p r, float radius, int slices, int stacks);
JOY_API render_mesh_p render3d_mesh_plane(render3d_p r, float w, float d, int segs);
JOY_API render_mesh_p render3d_mesh_custom(render3d_p r, const void* verts, int vertex_count);
JOY_API void         render3d_mesh_destroy(render3d_p r, render_mesh_p m);

/* ---------- 绘制 ---------- */
JOY_API void render3d_draw_mesh(render3d_p r, render_mesh_p m, vec3_t pos, vec3_t scale, SDL_Color color);
JOY_API void render3d_draw_grid(render3d_p r, int slices, float spacing);

#ifdef __cplusplus
}
#endif

#endif // !JRENDER_H
