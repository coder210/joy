#include "joy.h"
#include <SDL3/SDL.h>

/* ================================================================
 * 内部结构 — 完整 SDL3 GPU 类型
 * ================================================================ */

#define MAX_COLOR_CACHE 64

struct render_mesh {
	SDL_GPUBuffer* buffer;
	int            vertex_count;
};

struct jr3d {
	SDL_GPUDevice*     dev;
	SDL_Window*        win;
	SDL_GPUShader*     vs;
	SDL_GPUShader*     fs;
	SDL_GPUGraphicsPipeline* pipeline;
	SDL_GPUSampler*    sampler;
	SDL_GPUTexture*    depth;
	SDL_GPUCommandBuffer* cmd;
	SDL_GPURenderPass* rp;
	int                ww, wh;

	/* 相机/投影 */
	mat44_t            viewProj;
	float              fovy;
	float              aspect;
	float              near_;
	float              far_;

	/* 颜色纹理缓存 */
	struct { Uint8 r, g, b; SDL_GPUTexture* tex; } color_cache[MAX_COLOR_CACHE];
	int                color_count;
};

/* ================================================================
 * 底层工具 — 使用 void* 隐藏 SDL3 句柄，内部强转
 * ================================================================ */

void* r3d_load_shader(void* device, const char* filename,
	int stage, int sampler_num, int uniform_buffer_num)
{
	SDL_GPUDevice* dev = (SDL_GPUDevice*)device;
	size_t file_size;
	Uint8* data = (Uint8*)SDL_LoadFile(filename, &file_size);
	if (!data) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "r3d_load_shader: %s failed: %s", filename, SDL_GetError());
		return NULL;
	}
	SDL_GPUShaderCreateInfo ci = { 0 };
	ci.code = data;
	ci.code_size = file_size;
	ci.entrypoint = "main";
	ci.format = SDL_GPU_SHADERFORMAT_SPIRV;
	ci.num_samplers = sampler_num;
	ci.num_uniform_buffers = uniform_buffer_num;
	ci.num_storage_buffers = 0;
	ci.num_storage_textures = 0;
	ci.stage = (SDL_GPUShaderStage)stage;
	SDL_GPUShader* shader = SDL_CreateGPUShader(dev, &ci);
	SDL_free(data);
	if (!shader)
		SDL_LogError(SDL_LOG_CATEGORY_GPU, "r3d_load_shader: create %s failed: %s", filename, SDL_GetError());
	return (void*)shader;
}

void* r3d_create_pipeline(void* device,
	void* vertex_shader, void* fragment_shader,
	int swapchain_format, int depth_format)
{
	SDL_GPUDevice* dev = (SDL_GPUDevice*)device;
	SDL_GPUShader* vs = (SDL_GPUShader*)vertex_shader;
	SDL_GPUShader* fs = (SDL_GPUShader*)fragment_shader;

	SDL_GPUGraphicsPipelineCreateInfo ci = { 0 };
	SDL_GPUVertexAttribute attributes[2];
	attributes[0].location = 0;
	attributes[0].buffer_slot = 0;
	attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
	attributes[0].offset = 0;
	attributes[1].location = 1;
	attributes[1].buffer_slot = 0;
	attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
	attributes[1].offset = sizeof(float) * 3;

	ci.vertex_input_state.vertex_attributes = attributes;
	ci.vertex_input_state.num_vertex_attributes = 2;

	SDL_GPUVertexBufferDescription buffer_desc = { 0 };
	buffer_desc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	buffer_desc.instance_step_rate = 0;
	buffer_desc.slot = 0;
	buffer_desc.pitch = sizeof(float) * 5;

	ci.vertex_input_state.num_vertex_buffers = 1;
	ci.vertex_input_state.vertex_buffer_descriptions = &buffer_desc;
	ci.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
	ci.vertex_shader = vs;
	ci.fragment_shader = fs;
	ci.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
	ci.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
	ci.multisample_state.enable_mask = false;
	ci.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
	ci.target_info.num_color_targets = 1;
	ci.target_info.has_depth_stencil_target = true;
	ci.target_info.depth_stencil_format = (SDL_GPUTextureFormat)depth_format;

	SDL_GPUDepthStencilState ds = { 0 };
	ds.back_stencil_state.compare_op = SDL_GPU_COMPAREOP_NEVER;
	ds.back_stencil_state.pass_op = SDL_GPU_STENCILOP_ZERO;
	ds.back_stencil_state.fail_op = SDL_GPU_STENCILOP_ZERO;
	ds.back_stencil_state.depth_fail_op = SDL_GPU_STENCILOP_ZERO;
	ds.compare_op = SDL_GPU_COMPAREOP_LESS;
	ds.enable_depth_test = true;
	ds.enable_depth_write = true;
	ds.enable_stencil_test = false;
	ds.compare_mask = 0xFF;
	ds.write_mask = 0xFF;
	ci.depth_stencil_state = ds;

	SDL_GPUColorTargetDescription desc = { 0 };
	desc.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
	desc.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
	desc.blend_state.color_write_mask = SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A;
	desc.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
	desc.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
	desc.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
	desc.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
	desc.blend_state.enable_blend = true;
	desc.blend_state.enable_color_write_mask = false;
	desc.format = (SDL_GPUTextureFormat)swapchain_format;
	ci.target_info.color_target_descriptions = &desc;

	return (void*)SDL_CreateGPUGraphicsPipeline(dev, &ci);
}

void* r3d_upload_buffer(void* device, const void* data, size_t size)
{
	SDL_GPUDevice* dev = (SDL_GPUDevice*)device;
	SDL_GPUTransferBufferCreateInfo tb_ci = { 0 };
	tb_ci.size = size;
	tb_ci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(dev, &tb_ci);
	if (!tb) { SDL_LogError(SDL_LOG_CATEGORY_GPU, "r3d_upload_buffer: create tb failed"); return NULL; }
	void* ptr = SDL_MapGPUTransferBuffer(dev, tb, false);
	memcpy(ptr, data, size);
	SDL_UnmapGPUTransferBuffer(dev, tb);

	SDL_GPUBufferCreateInfo buf_ci = { 0 };
	buf_ci.size = size;
	buf_ci.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
	SDL_GPUBuffer* buf = SDL_CreateGPUBuffer(dev, &buf_ci);
	if (!buf) { SDL_ReleaseGPUTransferBuffer(dev, tb); return NULL; }

	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(dev);
	SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd);
	SDL_GPUTransferBufferLocation loc = { 0 };
	loc.offset = 0; loc.transfer_buffer = tb;
	SDL_GPUBufferRegion region = { 0 };
	region.buffer = buf; region.offset = 0; region.size = size;
	SDL_UploadToGPUBuffer(copy_pass, &loc, &region, false);
	SDL_EndGPUCopyPass(copy_pass);
	SDL_SubmitGPUCommandBuffer(cmd);
	SDL_ReleaseGPUTransferBuffer(dev, tb);
	return (void*)buf;
}

static SDL_GPUTexture* upload_pixels_to_texture(SDL_GPUDevice* device,
	const unsigned char* pixels, int w, int h, SDL_GPUTextureFormat format,
	SDL_GPUTextureUsageFlags usage)
{
	size_t image_size = (size_t)w * h * 4;
	SDL_GPUTransferBufferCreateInfo tb_ci = { 0 };
	tb_ci.size = image_size; tb_ci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(device, &tb_ci);
	if (!tb) return NULL;
	void* ptr = SDL_MapGPUTransferBuffer(device, tb, false);
	memcpy(ptr, pixels, image_size);
	SDL_UnmapGPUTransferBuffer(device, tb);

	SDL_GPUTextureCreateInfo tex_ci = { 0 };
	tex_ci.format = format; tex_ci.height = h; tex_ci.width = w;
	tex_ci.layer_count_or_depth = 1; tex_ci.num_levels = 1;
	tex_ci.sample_count = SDL_GPU_SAMPLECOUNT_1; tex_ci.type = SDL_GPU_TEXTURETYPE_2D;
	tex_ci.usage = usage;
	SDL_GPUTexture* tex = SDL_CreateGPUTexture(device, &tex_ci);
	if (!tex) { SDL_ReleaseGPUTransferBuffer(device, tb); return NULL; }

	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);
	SDL_GPUTextureTransferInfo ti = { 0 };
	ti.offset = 0; ti.pixels_per_row = w; ti.rows_per_layer = h; ti.transfer_buffer = tb;
	SDL_GPUTextureRegion region = { 0 };
	region.w = w; region.h = h; region.x = 0; region.y = 0;
	region.layer = 0; region.mip_level = 0; region.z = 0; region.d = 1;
	region.texture = tex;
	SDL_UploadToGPUTexture(copy, &ti, &region, false);
	SDL_EndGPUCopyPass(copy);
	SDL_SubmitGPUCommandBuffer(cmd);
	SDL_ReleaseGPUTransferBuffer(device, tb);
	return tex;
}

void* r3d_create_solid_texture(void* device,
	unsigned char r, unsigned char g, unsigned char b)
{
	SDL_GPUDevice* dev = (SDL_GPUDevice*)device;
	const int tw = 16, th = 16;
	size_t image_size = (size_t)tw * th * 4;
	unsigned char* pixels = (unsigned char*)SDL_malloc(image_size);
	if (!pixels) return NULL;
	for (int i = 0; i < tw * th; i++) {
		pixels[i * 4 + 0] = r; pixels[i * 4 + 1] = g;
		pixels[i * 4 + 2] = b; pixels[i * 4 + 3] = 255;
	}
	SDL_GPUTexture* tex = upload_pixels_to_texture(dev, pixels, tw, th,
		SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, SDL_GPU_TEXTUREUSAGE_SAMPLER);
	SDL_free(pixels);
	return (void*)tex;
}

void* r3d_create_depth_texture(void* device, int width, int height)
{
	SDL_GPUDevice* dev = (SDL_GPUDevice*)device;
	SDL_GPUTextureCreateInfo tex_ci = { 0 };
	tex_ci.format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
	tex_ci.height = height; tex_ci.width = width;
	tex_ci.layer_count_or_depth = 1; tex_ci.num_levels = 1;
	tex_ci.sample_count = SDL_GPU_SAMPLECOUNT_1; tex_ci.type = SDL_GPU_TEXTURETYPE_2D;
	tex_ci.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
	return (void*)SDL_CreateGPUTexture(dev, &tex_ci);
}

void* r3d_create_sampler(void* device)
{
	SDL_GPUDevice* dev = (SDL_GPUDevice*)device;
	SDL_GPUSamplerCreateInfo ci = { 0 };
	ci.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	ci.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	ci.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	ci.enable_anisotropy = true; ci.max_anisotropy = 4.0f;
	ci.compare_op = SDL_GPU_COMPAREOP_ALWAYS; ci.enable_compare = false;
	ci.mag_filter = SDL_GPU_FILTER_LINEAR; ci.min_filter = SDL_GPU_FILTER_LINEAR;
	ci.max_lod = 10.0f; ci.min_lod = 0.0f; ci.mip_lod_bias = 0.0f;
	ci.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
	return (void*)SDL_CreateGPUSampler(dev, &ci);
}

void r3d_mat44_to_float16(const mat44_t* m, float out[16])
{
	out[0] = m->m0;  out[1] = m->m1;  out[2] = m->m2;  out[3] = m->m3;
	out[4] = m->m4;  out[5] = m->m5;  out[6] = m->m6;  out[7] = m->m7;
	out[8] = m->m8;  out[9] = m->m9;  out[10] = m->m10; out[11] = m->m11;
	out[12] = m->m12; out[13] = m->m13; out[14] = m->m14; out[15] = m->m15;
}

/* ================================================================
 * 高级 API (v3.0) — 可重入
 * ================================================================ */

/* ---- 内部：查/创建颜色纹理 ---- */
static SDL_GPUTexture* get_color_texture(jr3d_p r, Uint8 cr, Uint8 cg, Uint8 cb)
{
	for (int i = 0; i < r->color_count; i++) {
		if (r->color_cache[i].r == cr && r->color_cache[i].g == cg && r->color_cache[i].b == cb)
			return r->color_cache[i].tex;
	}
	if (r->color_count >= MAX_COLOR_CACHE) {
		SDL_LogError(SDL_LOG_CATEGORY_GPU, "r3d: color cache full");
		return NULL;
	}
	SDL_GPUTexture* tex = (SDL_GPUTexture*)r3d_create_solid_texture(r->dev, cr, cg, cb);
	if (tex) {
		r->color_cache[r->color_count].r = cr;
		r->color_cache[r->color_count].g = cg;
		r->color_cache[r->color_count].b = cb;
		r->color_cache[r->color_count].tex = tex;
		r->color_count++;
	}
	return tex;
}

/* ---- 生命周期 ---- */

jr3d_p r3d_create(jwindow_p win, const char* shader_dir)
{
	SDL_Window* window = (SDL_Window*)get_raw_window(win);
	jr3d_p r = (jr3d_p)SDL_calloc(1, sizeof(jr3d_t));
	if (!r) return NULL;

	r->win = window;
	r->dev = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
	if (!r->dev) { SDL_LogError(SDL_LOG_CATEGORY_GPU, "r3d_create: CreateGPUDevice failed"); SDL_free(r); return NULL; }
	if (!SDL_ClaimWindowForGPUDevice(r->dev, r->win)) {
		SDL_LogError(SDL_LOG_CATEGORY_GPU, "r3d_create: ClaimWindow failed");
		SDL_DestroyGPUDevice(r->dev); SDL_free(r); return NULL;
	}

	/* 加载着色器 */
	size_t dirlen = strlen(shader_dir);
	char* vert_path = (char*)SDL_malloc(dirlen + 10);
	char* frag_path = (char*)SDL_malloc(dirlen + 10);
	if (!vert_path || !frag_path) { SDL_free(vert_path); SDL_free(frag_path); r3d_destroy(r); return NULL; }
	snprintf(vert_path, dirlen + 10, "%svert.spv", shader_dir);
	snprintf(frag_path, dirlen + 10, "%sfrag.spv", shader_dir);
	r->vs = (SDL_GPUShader*)r3d_load_shader(r->dev, vert_path, SDL_GPU_SHADERSTAGE_VERTEX, 0, 1);
	r->fs = (SDL_GPUShader*)r3d_load_shader(r->dev, frag_path, SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0);
	SDL_free(vert_path); SDL_free(frag_path);
	if (!r->vs || !r->fs) { r3d_destroy(r); return NULL; }

	SDL_GPUTextureFormat fmt = SDL_GetGPUSwapchainTextureFormat(r->dev, r->win);
	r->pipeline = (SDL_GPUGraphicsPipeline*)r3d_create_pipeline(r->dev,
		r->vs, r->fs, (int)fmt, (int)SDL_GPU_TEXTUREFORMAT_D16_UNORM);
	if (!r->pipeline) { r3d_destroy(r); return NULL; }

	r->sampler = (SDL_GPUSampler*)r3d_create_sampler(r->dev);
	if (!r->sampler) { r3d_destroy(r); return NULL; }

	SDL_GetWindowSize(r->win, &r->ww, &r->wh);
	r->depth = (SDL_GPUTexture*)r3d_create_depth_texture(r->dev, r->ww, r->wh);
	if (!r->depth) { r3d_destroy(r); return NULL; }

	r->aspect = (float)r->ww / (float)r->wh;
	r->fovy   = 45.0f;
	r->near_  = 0.1f;
	r->far_   = 100.0f;
	return r;
}

void r3d_destroy(jr3d_p r)
{
	if (!r) return;
	if (r->dev) SDL_WaitForGPUIdle(r->dev);

	for (int i = 0; i < r->color_count; i++)
		if (r->color_cache[i].tex) SDL_ReleaseGPUTexture(r->dev, r->color_cache[i].tex);
	r->color_count = 0;

	if (r->depth)    SDL_ReleaseGPUTexture(r->dev, r->depth);
	if (r->sampler)  SDL_ReleaseGPUSampler(r->dev, r->sampler);
	if (r->pipeline) SDL_ReleaseGPUGraphicsPipeline(r->dev, r->pipeline);
	if (r->vs)       SDL_ReleaseGPUShader(r->dev, r->vs);
	if (r->fs)       SDL_ReleaseGPUShader(r->dev, r->fs);
	if (r->dev) {
		SDL_ReleaseWindowFromGPUDevice(r->dev, r->win);
		SDL_DestroyGPUDevice(r->dev);
	}
	SDL_free(r);
}

/* ---- 帧管理 ---- */

bool r3d_begin(jr3d_p r)
{
	if (!r) return false;
	r->cmd = SDL_AcquireGPUCommandBuffer(r->dev);
	if (!r->cmd) return false;

	SDL_GetWindowSize(r->win, &r->ww, &r->wh);
	if (r->ww < 1 || r->wh < 1) { SDL_SubmitGPUCommandBuffer(r->cmd); r->cmd = NULL; return false; }

	SDL_GPUTexture* swapchain = NULL;
	Uint32 sw, sh;
	if (!SDL_WaitAndAcquireGPUSwapchainTexture(r->cmd, r->win, &swapchain, &sw, &sh) || !swapchain) {
		SDL_SubmitGPUCommandBuffer(r->cmd); r->cmd = NULL; return false;
	}

	if ((int)sw != r->ww || (int)sh != r->wh) {
		r->ww = (int)sw; r->wh = (int)sh;
		r->aspect = (float)r->ww / (float)r->wh;
		if (r->depth) SDL_ReleaseGPUTexture(r->dev, r->depth);
		r->depth = (SDL_GPUTexture*)r3d_create_depth_texture(r->dev, r->ww, r->wh);
	}

	SDL_GPUColorTargetInfo color = { 0 };
	color.clear_color.r = 0.05f; color.clear_color.g = 0.06f;
	color.clear_color.b = 0.10f; color.clear_color.a = 1;
	color.load_op = SDL_GPU_LOADOP_CLEAR; color.mip_level = 0;
	color.store_op = SDL_GPU_STOREOP_STORE; color.texture = swapchain;
	color.cycle = true; color.layer_or_depth_plane = 0;

	SDL_GPUDepthStencilTargetInfo depth = { 0 };
	depth.clear_depth = 1; depth.cycle = false;
	depth.load_op = SDL_GPU_LOADOP_CLEAR; depth.store_op = SDL_GPU_STOREOP_DONT_CARE;
	depth.texture = r->depth;

	r->rp = SDL_BeginGPURenderPass(r->cmd, &color, 1, &depth);
	if (!r->rp) { SDL_SubmitGPUCommandBuffer(r->cmd); r->cmd = NULL; return false; }

	SDL_BindGPUGraphicsPipeline(r->rp, r->pipeline);

	SDL_GPUViewport vp = { 0 };
	vp.w = (float)r->ww; vp.h = (float)r->wh; vp.max_depth = 1;
	SDL_SetGPUViewport(r->rp, &vp);

	return true;
}

void r3d_end(jr3d_p r)
{
	if (!r) return;
	if (r->rp)  { SDL_EndGPURenderPass(r->rp); r->rp = NULL; }
	if (r->cmd) { SDL_SubmitGPUCommandBuffer(r->cmd); r->cmd = NULL; }
}

/* ---- 相机 ---- */

void r3d_camera_lookat(jr3d_p r, vec3_t eye, vec3_t target, vec3_t up, float fovy)
{
	mat44_t view;
	mat44_t proj;
	if (!r) return;
	r->fovy = fovy;
	view = mat44_lookAt(eye, target, up);
	proj = mat44_perspective(ft_radians(r->fovy), r->aspect, r->near_, r->far_);
	r->viewProj = mat44_mul(view, proj);
}

void r3d_camera_fps(jr3d_p r, vec3_t pos, float yaw, float pitch, float fovy)
{
	float yr, pr;
	vec3_t camOffset;
	vec3_t target;
	vec3_t eye;
	vec3_t upv;
	if (!r) return;
	yr = ft_radians(yaw); pr = ft_radians(pitch);
	camOffset.x = -sinf(yr) * cosf(pr) * 6.0f;
	camOffset.y =  sinf(pr) * 6.0f;
	camOffset.z = -cosf(yr) * cosf(pr) * 6.0f;
	upv.x = 0; upv.y = 1; upv.z = 0;
	target.x = pos.x; target.y = pos.y + 1.0f; target.z = pos.z;
	eye = vec3_add(pos, camOffset);
	r3d_camera_lookat(r, eye, target, upv, fovy);
}

/* ---- 网格 ---- */

render_mesh_p r3d_mesh_cube(jr3d_p r, float w, float h, float d)
{
	float hw, hh, hd;
	float* verts;
	int vi;
	hw = w * 0.5f; hh = h * 0.5f; hd = d * 0.5f;
	verts = (float*)SDL_malloc(36 * 5 * sizeof(float));
	vi = 0;
	verts[vi++]=-hw; verts[vi++]=-hh; verts[vi++]= hd; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=-hh; verts[vi++]= hd; verts[vi++]=1; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]= hh; verts[vi++]= hd; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]= hw; verts[vi++]= hh; verts[vi++]= hd; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]= hh; verts[vi++]= hd; verts[vi++]=0; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]=-hh; verts[vi++]= hd; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=-hh; verts[vi++]=-hd; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]=-hw; verts[vi++]=-hh; verts[vi++]=-hd; verts[vi++]=1; verts[vi++]=0;
	verts[vi++]=-hw; verts[vi++]= hh; verts[vi++]=-hd; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]= hh; verts[vi++]=-hd; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]= hw; verts[vi++]= hh; verts[vi++]=-hd; verts[vi++]=0; verts[vi++]=1;
	verts[vi++]= hw; verts[vi++]=-hh; verts[vi++]=-hd; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]=-hw; verts[vi++]=-hh; verts[vi++]=-hd; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]=-hw; verts[vi++]=-hh; verts[vi++]= hd; verts[vi++]=1; verts[vi++]=0;
	verts[vi++]=-hw; verts[vi++]= hh; verts[vi++]= hd; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]= hh; verts[vi++]= hd; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]= hh; verts[vi++]=-hd; verts[vi++]=0; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]=-hh; verts[vi++]=-hd; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=-hh; verts[vi++]= hd; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=-hh; verts[vi++]=-hd; verts[vi++]=1; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]= hh; verts[vi++]=-hd; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]= hw; verts[vi++]= hh; verts[vi++]=-hd; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]= hw; verts[vi++]= hh; verts[vi++]= hd; verts[vi++]=0; verts[vi++]=1;
	verts[vi++]= hw; verts[vi++]=-hh; verts[vi++]= hd; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]=-hw; verts[vi++]= hh; verts[vi++]= hd; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]= hh; verts[vi++]= hd; verts[vi++]=1; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]= hh; verts[vi++]=-hd; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]= hw; verts[vi++]= hh; verts[vi++]=-hd; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]= hh; verts[vi++]=-hd; verts[vi++]=0; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]= hh; verts[vi++]= hd; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]=-hw; verts[vi++]=-hh; verts[vi++]=-hd; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=-hh; verts[vi++]=-hd; verts[vi++]=1; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=-hh; verts[vi++]= hd; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]= hw; verts[vi++]=-hh; verts[vi++]= hd; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]=-hh; verts[vi++]= hd; verts[vi++]=0; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]=-hh; verts[vi++]=-hd; verts[vi++]=0; verts[vi++]=0;
	return r3d_mesh_custom(r, verts, 36);
}

render_mesh_p r3d_mesh_sphere(jr3d_p r, float radius, int slices, int stacks)
{
	float* verts;
	int count, j, i, o;
	render_mesh_p m;
	verts = (float*)SDL_malloc((32*24*6) * 5 * sizeof(float));
	if (!verts) return NULL;
	count = 0;
	for (j = 0; j < stacks; j++) {
		float phi1 = (float)j / stacks * 3.14159265f;
		float phi2 = (float)(j + 1) / stacks * 3.14159265f;
		for (i = 0; i < slices; i++) {
			float theta1 = (float)i / slices * 2.0f * 3.14159265f;
			float theta2 = (float)(i + 1) / slices * 2.0f * 3.14159265f;
			float x1 = radius * sinf(phi1) * cosf(theta1), y1 = radius * cosf(phi1), z1 = radius * sinf(phi1) * sinf(theta1);
			float x2 = radius * sinf(phi1) * cosf(theta2), y2 = radius * cosf(phi1), z2 = radius * sinf(phi1) * sinf(theta2);
			float x3 = radius * sinf(phi2) * cosf(theta2), y3 = radius * cosf(phi2), z3 = radius * sinf(phi2) * sinf(theta2);
			float x4 = radius * sinf(phi2) * cosf(theta1), y4 = radius * cosf(phi2), z4 = radius * sinf(phi2) * sinf(theta1);
			float u1 = (float)i / slices, u2 = (float)(i + 1) / slices;
			float v1 = (float)j / stacks, v2 = (float)(j + 1) / stacks;
			if (count + 6 > 32*24*6) break;
			o = count * 5; verts[o]=x1; verts[o+1]=y1; verts[o+2]=z1; verts[o+3]=u1; verts[o+4]=v1; count++;
			o = count * 5; verts[o]=x2; verts[o+1]=y2; verts[o+2]=z2; verts[o+3]=u2; verts[o+4]=v1; count++;
			o = count * 5; verts[o]=x3; verts[o+1]=y3; verts[o+2]=z3; verts[o+3]=u2; verts[o+4]=v2; count++;
			o = count * 5; verts[o]=x1; verts[o+1]=y1; verts[o+2]=z1; verts[o+3]=u1; verts[o+4]=v1; count++;
			o = count * 5; verts[o]=x3; verts[o+1]=y3; verts[o+2]=z3; verts[o+3]=u2; verts[o+4]=v2; count++;
			o = count * 5; verts[o]=x4; verts[o+1]=y4; verts[o+2]=z4; verts[o+3]=u1; verts[o+4]=v2; count++;
		}
	}
	m = r3d_mesh_custom(r, verts, count);
	SDL_free(verts);
	return m;
}

render_mesh_p r3d_mesh_plane(jr3d_p r, float w, float d, int segs)
{
	float hw, hd, step_x, step_z, x0, z0, x1, z1, u0, v0, u1, v1;
	int total, iz, ix, idx, o;
	float* verts;
	render_mesh_p m;
	hw = w * 0.5f; hd = d * 0.5f;
	step_x = w / segs; step_z = d / segs;
	total = segs * segs * 6;
	verts = (float*)SDL_malloc(total * 5 * sizeof(float));
	if (!verts) return NULL;
	idx = 0;
	for (iz = 0; iz < segs; iz++) {
		for (ix = 0; ix < segs; ix++) {
			x0 = -hw + ix * step_x; z0 = -hd + iz * step_z;
			x1 = x0 + step_x; z1 = z0 + step_z;
			u0 = (float)ix / segs; v0 = (float)iz / segs;
			u1 = (float)(ix + 1) / segs; v1 = (float)(iz + 1) / segs;
			o = idx * 5;
			verts[o]=x0; verts[o+1]=0; verts[o+2]=z0; verts[o+3]=u0; verts[o+4]=v0; idx++;
			verts[o]=x1; verts[o+1]=0; verts[o+2]=z0; verts[o+3]=u1; verts[o+4]=v0; idx++;
			verts[o]=x1; verts[o+1]=0; verts[o+2]=z1; verts[o+3]=u1; verts[o+4]=v1; idx++;
			verts[o]=x0; verts[o+1]=0; verts[o+2]=z0; verts[o+3]=u0; verts[o+4]=v0; idx++;
			verts[o]=x1; verts[o+1]=0; verts[o+2]=z1; verts[o+3]=u1; verts[o+4]=v1; idx++;
			verts[o]=x0; verts[o+1]=0; verts[o+2]=z1; verts[o+3]=u0; verts[o+4]=v1; idx++;
		}
	}
	m = r3d_mesh_custom(r, verts, total);
	SDL_free(verts);
	return m;
}

render_mesh_p r3d_mesh_ramp(jr3d_p r, float w, float l, float h)
{
	float hw, hl;
	float* verts;
	render_mesh_p m;
	int vi;
	hw = w * 0.5f; hl = l * 0.5f;
	verts = (float*)SDL_malloc(24 * 5 * sizeof(float));
	vi = 0;
	/* 坡面 */
	verts[vi++]=-hw; verts[vi++]=0; verts[vi++]=-hl; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=0; verts[vi++]=-hl; verts[vi++]=1; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=h; verts[vi++]= hl; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]=0; verts[vi++]=-hl; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=h; verts[vi++]= hl; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]=h; verts[vi++]= hl; verts[vi++]=0; verts[vi++]=1;
	/* 左侧 */
	verts[vi++]=-hw; verts[vi++]=0; verts[vi++]=-hl; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]=-hw; verts[vi++]=h; verts[vi++]= hl; verts[vi++]=0; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]=0; verts[vi++]= hl; verts[vi++]=1; verts[vi++]=0;
	/* 右侧 */
	verts[vi++]= hw; verts[vi++]=0; verts[vi++]=-hl; verts[vi++]=1; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=0; verts[vi++]= hl; verts[vi++]=0; verts[vi++]=1;
	verts[vi++]= hw; verts[vi++]=h; verts[vi++]= hl; verts[vi++]=1; verts[vi++]=1;
	/* 背面 */
	verts[vi++]=-hw; verts[vi++]=h; verts[vi++]= hl; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=h; verts[vi++]= hl; verts[vi++]=1; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=0; verts[vi++]= hl; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]=h; verts[vi++]= hl; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=0; verts[vi++]= hl; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]=0; verts[vi++]= hl; verts[vi++]=0; verts[vi++]=1;
	/* 底面 */
	verts[vi++]=-hw; verts[vi++]=0; verts[vi++]=-hl; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=0; verts[vi++]=-hl; verts[vi++]=1; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=0; verts[vi++]= hl; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]=0; verts[vi++]=-hl; verts[vi++]=0; verts[vi++]=0;
	verts[vi++]= hw; verts[vi++]=0; verts[vi++]= hl; verts[vi++]=1; verts[vi++]=1;
	verts[vi++]=-hw; verts[vi++]=0; verts[vi++]= hl; verts[vi++]=0; verts[vi++]=1;

	m = r3d_mesh_custom(r, verts, 24);
	SDL_free(verts);
	return m;
}

render_mesh_p r3d_mesh_custom(jr3d_p r, const void* verts, int vertex_count)
{
	render_mesh_p m;
	(void)r;
	m = (render_mesh_p)SDL_malloc(sizeof(render_mesh_t));
	if (!m) return NULL;
	m->buffer = (SDL_GPUBuffer*)r3d_upload_buffer(r->dev, verts, (size_t)vertex_count * 5 * sizeof(float));
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = vertex_count;
	return m;
}

void r3d_mesh_destroy(jr3d_p r, render_mesh_p m)
{
	if (!r || !m) return;
	if (m->buffer) SDL_ReleaseGPUBuffer(r->dev, m->buffer);
	SDL_free(m);
}

/* ---- 内部：用 model 矩阵和纹理绘制一个网格 ---- */

static void draw_mesh_internal(jr3d_p r, render_mesh_p m, mat44_t* model, SDL_GPUTexture* tex)
{
	render_mvp_t mvp;
	SDL_GPUTextureSamplerBinding samp;
	SDL_GPUBufferBinding bind;
	if (!r->rp || !m || !m->buffer) return;

	memset(&mvp, 0, sizeof(mvp));
	r3d_mat44_to_float16(&r->viewProj, mvp.viewProj);
	r3d_mat44_to_float16(model, mvp.model);
	SDL_PushGPUVertexUniformData(r->cmd, 0, &mvp, sizeof(mvp));

	if (tex) {
		memset(&samp, 0, sizeof(samp));
		samp.texture = (SDL_GPUTexture*)tex; samp.sampler = r->sampler;
		SDL_BindGPUFragmentSamplers(r->rp, 0, &samp, 1);
	}

	memset(&bind, 0, sizeof(bind));
	bind.buffer = m->buffer; bind.offset = 0;
	SDL_BindGPUVertexBuffers(r->rp, 0, &bind, 1);
	SDL_DrawGPUPrimitives(r->rp, m->vertex_count, 1, 0, 0);
}

/* ---- 公开绘制 API ---- */

void r3d_draw_mesh(jr3d_p r, render_mesh_p m, vec3_t pos, vec3_t scale, jcolor_t color)
{
	mat44_t model;
	SDL_GPUTexture* tex;
	memset(&model, 0, sizeof(model));
	model.m0 = scale.x;  model.m5 = scale.y;  model.m10 = scale.z;
	model.m12 = pos.x;   model.m13 = pos.y;   model.m14 = pos.z;
	model.m15 = 1;
	tex = get_color_texture(r, color.r, color.g, color.b);
	draw_mesh_internal(r, m, &model, tex);
}

void r3d_draw_mesh_tex(jr3d_p r, render_mesh_p m, vec3_t pos, vec3_t scale, void* tex)
{
	mat44_t model = { 0 };
	model.m0 = scale.x;  model.m5 = scale.y;  model.m10 = scale.z;
	model.m12 = pos.x;   model.m13 = pos.y;   model.m14 = pos.z;
	model.m15 = 1;
	draw_mesh_internal(r, m, &model, (SDL_GPUTexture*)tex);
}

/* ---- 棋盘格纹理 ---- */

void* r3d_gen_checkerboard(jr3d_p r, int tile_size, int tile_count)
{
	int w = tile_size * tile_count;
	int h = w;
	int y, x, idx, tile_x, tile_y, light, gx, gy;
	size_t image_size = (size_t)w * h * 4;
	unsigned char* pixels = (unsigned char*)SDL_malloc(image_size);
	SDL_GPUTexture* tex;
	if (!pixels) return NULL;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			idx = (y * w + x) * 4;
			tile_x = x / tile_size;
			tile_y = y / tile_size;
			light = ((tile_x + tile_y) % 2) == 0;
			if (light) {
				pixels[idx+0] = 60;  pixels[idx+1] = 68;  pixels[idx+2] = 80;
			} else {
				pixels[idx+0] = 28;  pixels[idx+1] = 32;  pixels[idx+2] = 40;
			}
			gx = x % tile_size;
			gy = y % tile_size;
			if (gx < 1 || gy < 1 || gx > tile_size-2 || gy > tile_size-2) {
				pixels[idx+0] = 30;  pixels[idx+1] = 180; pixels[idx+2] = 255;
			}
			pixels[idx+3] = 255;
		}
	}

	tex = upload_pixels_to_texture(r->dev, pixels, w, h,
		SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, SDL_GPU_TEXTUREUSAGE_SAMPLER);
	SDL_free(pixels);
	return (void*)tex;
}

void r3d_draw_grid(jr3d_p r, int slices, float spacing, float line_width)
{
	float half, hw, p;
	int i;
	SDL_GPUTexture* gray_tex;
	SDL_GPUBuffer* bx;
	SDL_GPUBuffer* bz;
	render_mesh_t mx;
	render_mesh_t mz;
	mat44_t id;
	half = slices * spacing * 0.5f;
	gray_tex = get_color_texture(r, 100, 100, 120);
	hw = line_width;

	for (i = 0; i <= slices; i++) {
		p = -half + i * spacing;

		{
			float verts_x[6 * 5];
			int vi = 0;
			verts_x[vi++]=p; verts_x[vi++]=0; verts_x[vi++]=-half; verts_x[vi++]=0; verts_x[vi++]=0;
			verts_x[vi++]=p+hw; verts_x[vi++]=0; verts_x[vi++]=-half; verts_x[vi++]=1; verts_x[vi++]=0;
			verts_x[vi++]=p+hw; verts_x[vi++]=0; verts_x[vi++]=half; verts_x[vi++]=1; verts_x[vi++]=1;
			verts_x[vi++]=p; verts_x[vi++]=0; verts_x[vi++]=-half; verts_x[vi++]=0; verts_x[vi++]=0;
			verts_x[vi++]=p+hw; verts_x[vi++]=0; verts_x[vi++]=half; verts_x[vi++]=1; verts_x[vi++]=1;
			verts_x[vi++]=p; verts_x[vi++]=0; verts_x[vi++]=half; verts_x[vi++]=0; verts_x[vi++]=1;
			bx = (SDL_GPUBuffer*)r3d_upload_buffer(r->dev, verts_x, sizeof(verts_x));
		}
		if (bx) {
			memset(&id, 0, sizeof(id)); id.m15 = 1;
			mx.buffer = bx; mx.vertex_count = 6;
			draw_mesh_internal(r, &mx, &id, gray_tex);
			SDL_ReleaseGPUBuffer(r->dev, bx);
		}

		{
			float verts_z[6 * 5];
			int vi = 0;
			verts_z[vi++]=-half; verts_z[vi++]=0; verts_z[vi++]=p; verts_z[vi++]=0; verts_z[vi++]=0;
			verts_z[vi++]= half; verts_z[vi++]=0; verts_z[vi++]=p; verts_z[vi++]=1; verts_z[vi++]=0;
			verts_z[vi++]= half; verts_z[vi++]=0; verts_z[vi++]=p+hw; verts_z[vi++]=1; verts_z[vi++]=1;
			verts_z[vi++]=-half; verts_z[vi++]=0; verts_z[vi++]=p; verts_z[vi++]=0; verts_z[vi++]=0;
			verts_z[vi++]= half; verts_z[vi++]=0; verts_z[vi++]=p+hw; verts_z[vi++]=1; verts_z[vi++]=1;
			verts_z[vi++]=-half; verts_z[vi++]=0; verts_z[vi++]=p+hw; verts_z[vi++]=0; verts_z[vi++]=1;
			bz = (SDL_GPUBuffer*)r3d_upload_buffer(r->dev, verts_z, sizeof(verts_z));
		}
		if (bz) {
			memset(&id, 0, sizeof(id)); id.m15 = 1;
			mz.buffer = bz; mz.vertex_count = 6;
			draw_mesh_internal(r, &mz, &id, gray_tex);
			SDL_ReleaseGPUBuffer(r->dev, bz);
		}
	}
}
