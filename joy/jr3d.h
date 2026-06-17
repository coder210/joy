/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jr3d.h
Description: SDL_GPU 3D 渲染 — raylib 风格可重入 API + 底层工具
Author: ydlc
Version: 3.0
*************************************************/
#ifndef JR3D_H
#define JR3D_H
#include "jconfig.h"
#include "jmath.h"
#include "jcore.h"
#include "joy.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================
 * 不透明句柄
 * ================================================================ */
typedef struct jr3d   jr3d_t, * jr3d_p;          /* 3D 渲染上下文 */
typedef struct render_mesh render_mesh_t, * render_mesh_p;  /* 网格 */

/* ---------- MVP 结构（无 SDL 依赖） ---------- */
typedef struct render_mvp {
	float viewProj[16];
	float model[16];
} render_mvp_t;

/* ================================================================
 * 底层工具函数 — 使用 void* 隐藏 SDL3 GPU 句柄
 * ================================================================ */
JOY_API void* r3d_load_shader(void* device, const char* filename,
	int stage, int sampler_num, int uniform_buffer_num);
JOY_API void* r3d_create_pipeline(void* device,
	void* vertex_shader, void* fragment_shader,
	int swapchain_format, int depth_format);
JOY_API void* r3d_upload_buffer(void* device, const void* data, size_t size);
JOY_API void* r3d_create_solid_texture(void* device,
	unsigned char r, unsigned char g, unsigned char b);
JOY_API void* r3d_create_depth_texture(void* device, int width, int height);
JOY_API void* r3d_create_sampler(void* device);
JOY_API void  r3d_mat44_to_float16(const mat44_t* m, float out[16]);

/* ================================================================
 * 高级 API (v3.0) — 可重入，无 SDL3 类型暴露
 * ================================================================ */

/* ---------- 生命周期 ---------- */
JOY_API jr3d_p r3d_create(jwindow_p win, const char* shader_dir);
JOY_API void   r3d_destroy(jr3d_p r);

/* ---------- 帧管理 ---------- */
JOY_API bool r3d_begin(jr3d_p r);
JOY_API void r3d_end(jr3d_p r);

/* ---------- 相机 ---------- */
JOY_API void r3d_camera_lookat(jr3d_p r, vec3_t eye, vec3_t target, vec3_t up, float fovy);
JOY_API void r3d_camera_fps(jr3d_p r, vec3_t pos, float yaw, float pitch, float fovy);

/* ---------- 网格 ---------- */
JOY_API render_mesh_p r3d_mesh_cube(jr3d_p r, float w, float h, float d);
JOY_API render_mesh_p r3d_mesh_sphere(jr3d_p r, float radius, int slices, int stacks);
JOY_API render_mesh_p r3d_mesh_plane(jr3d_p r, float w, float d, int segs);
JOY_API render_mesh_p r3d_mesh_ramp(jr3d_p r, float w, float l, float h);
JOY_API render_mesh_p r3d_mesh_custom(jr3d_p r, const void* verts, int vertex_count);
JOY_API void         r3d_mesh_destroy(jr3d_p r, render_mesh_p m);

/* ---------- 纹理 ---------- */
JOY_API void* r3d_gen_checkerboard(jr3d_p r, int tile_size, int tile_count);

/* ---------- 绘制 ---------- */
JOY_API void r3d_draw_mesh(jr3d_p r, render_mesh_p m, vec3_t pos, vec3_t scale, jcolor_t color);
JOY_API void r3d_draw_mesh_tex(jr3d_p r, render_mesh_p m, vec3_t pos, vec3_t scale, void* tex);
JOY_API void r3d_draw_grid(jr3d_p r, int slices, float spacing, float line_width);

#ifdef __cplusplus
}
#endif

#endif // !JR3D_H
