#include "joy.h"
#include <SDL3/SDL.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ================================================================
 * 内部结构 — 完整 SDL3 GPU 类型
 * ================================================================ */

#define MAX_COLOR_CACHE 64

struct render_mesh {
	SDL_GPUBuffer* buffer;
	SDL_GPUBuffer* index_buffer;   /* 可选: 非空表示用索引绘制 */
	int            vertex_count;
	int            index_count;
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
	int                last_ww, last_wh;  /* 上次已处理的渲染尺寸（resize 检测基准） */

	/* 相机/投影 */
	mat44_t            viewProj;
	float              fovy;
	float              aspect;
	float              near_;
	float              far_;

	/* 颜色纹理缓存 */
	struct { Uint8 r, g, b; SDL_GPUTexture* tex; } color_cache[MAX_COLOR_CACHE];
	int                color_count;

	/* 光照渲染（可选, 由 r3d_create 探测 lighting.vert/frag.spv 自动启用） */
	SDL_GPUShader*     lvs;
	SDL_GPUShader*     lfs;
	SDL_GPUGraphicsPipeline* lighting_pipeline;
	jr3d_light_block_t light_block;
	bool               light_block_valid;
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
	ds.enable_depth_write = false;   // 非 lit 用于字体/图片 billboard, 关闭深度写入避免共面三角形 z-fighting 被剔除
	ds.enable_stencil_test = false;
	ds.compare_mask = 0xFF;
	ds.write_mask = 0xFF;
	ci.depth_stencil_state = ds;

	SDL_GPUColorTargetDescription desc = { 0 };
	desc.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
	desc.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
	desc.blend_state.color_write_mask = SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A;
	desc.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	desc.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	desc.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	desc.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	desc.blend_state.enable_blend = true;
	desc.blend_state.enable_color_write_mask = false;
	desc.format = (SDL_GPUTextureFormat)swapchain_format;
	ci.target_info.color_target_descriptions = &desc;

	return (void*)SDL_CreateGPUGraphicsPipeline(dev, &ci);
}

void* r3d_create_lighting_pipeline(void* device,
	void* vertex_shader, void* fragment_shader,
	int swapchain_format, int depth_format)
{
	SDL_GPUDevice* dev = (SDL_GPUDevice*)device;
	SDL_GPUShader* vs = (SDL_GPUShader*)vertex_shader;
	SDL_GPUShader* fs = (SDL_GPUShader*)fragment_shader;

	SDL_GPUGraphicsPipelineCreateInfo ci = { 0 };
	/* pos FLOAT3 + normal FLOAT3 + uv FLOAT2 */
	SDL_GPUVertexAttribute attributes[3];
	attributes[0].location = 0;
	attributes[0].buffer_slot = 0;
	attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
	attributes[0].offset = 0;
	attributes[1].location = 1;
	attributes[1].buffer_slot = 0;
	attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
	attributes[1].offset = sizeof(float) * 3;
	attributes[2].location = 2;
	attributes[2].buffer_slot = 0;
	attributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
	attributes[2].offset = sizeof(float) * 6;

	ci.vertex_input_state.vertex_attributes = attributes;
	ci.vertex_input_state.num_vertex_attributes = 3;

	SDL_GPUVertexBufferDescription buffer_desc = { 0 };
	buffer_desc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	buffer_desc.instance_step_rate = 0;
	buffer_desc.slot = 0;
	buffer_desc.pitch = sizeof(float) * 8;

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
	SDL_GPUTextureUsageFlags usage, int flip)
{
	const int bpp = 4; /* RGBA32 */
	const int src_row = w * bpp;
	/* GPU 上传要求每行字节数按 256 对齐(示例亦用连续 w, 但非对齐宽度会错位, 这里显式对齐) */
	int aligned_row = (src_row + 255) & ~255;
	int pixels_per_row = aligned_row / bpp;
	size_t buf_size = (size_t)aligned_row * h;

	SDL_GPUTransferBufferCreateInfo tb_ci = { 0 };
	tb_ci.size = buf_size; tb_ci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(device, &tb_ci);
	if (!tb) return NULL;
	unsigned char* ptr = (unsigned char*)SDL_MapGPUTransferBuffer(device, tb, false);
	for (int y = 0; y < h; y++) {
		int sy = flip ? (h - 1 - y) : y;   /* flip: 与 stbi_set_flip_vertically_on_load(true) 对应 */
		memcpy(ptr + (size_t)y * aligned_row, pixels + (size_t)sy * src_row, src_row);
	}
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
	ti.offset = 0; ti.pixels_per_row = pixels_per_row; ti.rows_per_layer = h; ti.transfer_buffer = tb;
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
		SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, SDL_GPU_TEXTUREUSAGE_SAMPLER, 0);
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
	ci.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	ci.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	ci.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
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

void* r3d_get_device(jr3d_p r)
{
	if (!r) return NULL;
	return (void*)r->dev;
}

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

	/* 加载着色器（兼容 shader_dir 带/不带尾部分隔符） */
	size_t dirlen = strlen(shader_dir);
	bool has_sep = (dirlen > 0) && (shader_dir[dirlen - 1] == '/' || shader_dir[dirlen - 1] == '\\');
	const char* sep = has_sep ? "" : "/";
	size_t plen = dirlen + 16;
	char* vert_path = (char*)SDL_malloc(plen);
	char* frag_path = (char*)SDL_malloc(plen);
	if (!vert_path || !frag_path) { SDL_free(vert_path); SDL_free(frag_path); r3d_destroy(r); return NULL; }
	snprintf(vert_path, plen, "%s%svert.spv", shader_dir, sep);
	snprintf(frag_path, plen, "%s%sfrag.spv", shader_dir, sep);
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
	r->last_ww = r->ww; r->last_wh = r->wh;
	r->depth = (SDL_GPUTexture*)r3d_create_depth_texture(r->dev, r->ww, r->wh);
	if (!r->depth) { r3d_destroy(r); return NULL; }

	r->aspect = (float)r->ww / (float)r->wh;
	r->fovy   = 45.0f;
	r->near_  = 0.1f;
	r->far_   = 100.0f;

	/* 光照管线(可选): 若存在 lighting.vert.spv / lighting.frag.spv 则自动启用 */
	{
		size_t lplen = dirlen + 24;
		char* lvert_path = (char*)SDL_malloc(lplen);
		char* lfrag_path = (char*)SDL_malloc(lplen);
		if (lvert_path && lfrag_path) {
			snprintf(lvert_path, lplen, "%s%slighting.vert.spv", shader_dir, sep);
			snprintf(lfrag_path, lplen, "%s%slighting.frag.spv", shader_dir, sep);
			r->lvs = (SDL_GPUShader*)r3d_load_shader(r->dev, lvert_path, SDL_GPU_SHADERSTAGE_VERTEX, 0, 1);
			r->lfs = (SDL_GPUShader*)r3d_load_shader(r->dev, lfrag_path, SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 1);
			if (r->lvs && r->lfs) {
				r->lighting_pipeline = (SDL_GPUGraphicsPipeline*)r3d_create_lighting_pipeline(r->dev,
					r->lvs, r->lfs, (int)fmt, (int)SDL_GPU_TEXTUREFORMAT_D16_UNORM);
			}
		}
		SDL_free(lvert_path); SDL_free(lfrag_path);
	}

	/* 默认光照: 环境光 + 一个方向光 */
	memset(&r->light_block, 0, sizeof(r->light_block));
	r->light_block.ambient[0] = 0.15f;
	r->light_block.ambient[1] = 0.15f;
	r->light_block.ambient[2] = 0.15f;
	r->light_block.ambient[3] = 1.0f;
	r->light_block.lights[0].enabled = 1;
	r->light_block.lights[0].type = JR3D_LIGHT_DIRECTIONAL;
	r->light_block.lights[0].position[0] = 30.0f;
	r->light_block.lights[0].position[1] = 25.0f;
	r->light_block.lights[0].position[2] = 30.0f;
	r->light_block.lights[0].target[0] = 0.0f;
	r->light_block.lights[0].target[1] = -1.0f;
	r->light_block.lights[0].target[2] = 0.0f;
	r->light_block.lights[0].color[0] = 1.0f;
	r->light_block.lights[0].color[1] = 0.95f;
	r->light_block.lights[0].color[2] = 0.8f;
	r->light_block.lights[0].color[3] = 1.0f;
	r->light_block_valid = true;

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
	if (r->lighting_pipeline) SDL_ReleaseGPUGraphicsPipeline(r->dev, r->lighting_pipeline);
	if (r->vs)       SDL_ReleaseGPUShader(r->dev, r->vs);
	if (r->fs)       SDL_ReleaseGPUShader(r->dev, r->fs);
	if (r->lvs)      SDL_ReleaseGPUShader(r->dev, r->lvs);
	if (r->lfs)      SDL_ReleaseGPUShader(r->dev, r->lfs);
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

	/* 以实际 swapchain 尺寸为基准检测 resize：
	 * 窗口 resize 后 swapchain 尺寸变化, 更新投影宽高比并重建深度纹理；
	 * 不能拿 SDL_GetWindowSize 的逻辑尺寸与 swapchain 比较,
	 * 否则 GetWindowSize 已先刷新 ww/wh, 条件恒不成立导致 aspect 永不更新 */
	if ((int)sw != r->last_ww || (int)sh != r->last_wh) {
		r->last_ww = (int)sw; r->last_wh = (int)sh;
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
	/* 用实际 swapchain 尺寸，避免 SDL_GetWindowSize 逻辑尺寸与物理像素不一致 */
	vp.w = (float)r->last_ww; vp.h = (float)r->last_wh; vp.max_depth = 1;
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
	// Column-major multiply: viewProj = proj * view (clip = viewProj * model * v = P * V * M * v)
	r->viewProj = mat44_mul(proj, view);
	if (r->light_block_valid) {
		r->light_block.viewPos[0] = eye.x;
		r->light_block.viewPos[1] = eye.y;
		r->light_block.viewPos[2] = eye.z;
		r->light_block.viewPos[3] = 1.0f;
	}
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
	int total, iz, ix, idx;
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
			verts[idx*5+0]=x0; verts[idx*5+1]=0; verts[idx*5+2]=z0; verts[idx*5+3]=u0; verts[idx*5+4]=v0; idx++;
			verts[idx*5+0]=x1; verts[idx*5+1]=0; verts[idx*5+2]=z0; verts[idx*5+3]=u1; verts[idx*5+4]=v0; idx++;
			verts[idx*5+0]=x1; verts[idx*5+1]=0; verts[idx*5+2]=z1; verts[idx*5+3]=u1; verts[idx*5+4]=v1; idx++;
			verts[idx*5+0]=x0; verts[idx*5+1]=0; verts[idx*5+2]=z0; verts[idx*5+3]=u0; verts[idx*5+4]=v0; idx++;
			verts[idx*5+0]=x1; verts[idx*5+1]=0; verts[idx*5+2]=z1; verts[idx*5+3]=u1; verts[idx*5+4]=v1; idx++;
			verts[idx*5+0]=x0; verts[idx*5+1]=0; verts[idx*5+2]=z1; verts[idx*5+3]=u0; verts[idx*5+4]=v1; idx++;
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

/* 通用 RGBA32 纹理创建: 自动处理 upload 行对齐, flip=1 时上下翻转(与 stbi_set_flip_vertically_on_load 对应) */
void* r3d_create_texture(jr3d_p r, int w, int h, const void* rgba32, int flip)
{
	if (!r || !rgba32 || w <= 0 || h <= 0) return NULL;
	return (void*)upload_pixels_to_texture(r->dev, (const unsigned char*)rgba32, w, h,
		SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, SDL_GPU_TEXTUREUSAGE_SAMPLER, flip);
}

/* 从 PNG 文件加载并创建 GPU 纹理: SDL_LoadPNG -> RGBA32 -> r3d_create_texture(flip=1)
   返回 SDL_GPUTexture* (void*)。失败返回 NULL 并通过 SDL_GetError 暴露原因。
   统一封装了图片文件加载, 供各 app 复用, 避免重复手写 SDL_LoadPNG 流程。 */
void* r3d_load_texture_png(jr3d_p r, const char* path, int* outW, int* outH)
{
	if (!r || !path) return NULL;
	SDL_Surface* surf = SDL_LoadPNG(path);
	if (!surf) {
		SDL_Log("r3d_load_texture_png: load %s failed: %s", path, SDL_GetError());
		return NULL;
	}
	SDL_Surface* rgba = SDL_ConvertSurface(surf, SDL_PIXELFORMAT_RGBA32);
	int w = surf->w, h = surf->h;
	SDL_DestroySurface(surf);
	if (!rgba) {
		SDL_Log("r3d_load_texture_png: convert %s failed: %s", path, SDL_GetError());
		return NULL;
	}
	void* tex = r3d_create_texture(r, rgba->w, rgba->h, rgba->pixels, 1);
	SDL_DestroySurface(rgba);
	if (outW) *outW = w;
	if (outH) *outH = h;
	SDL_Log("r3d_load_texture_png: OK %s %dx%d tex=%p", path, w, h, tex);
	return tex;
}

/* 单位平面 quad: 4 顶点(FLOAT3 pos + FLOAT2 uv) + 6 索引(0,1,2,0,3,2), 范围 x/y ∈ [-0.5,0.5]
   UV: (0,0)左上 (1,1)右下, 与 stbi flip=true 后的纹理朝向一致 */
render_mesh_p r3d_mesh_quad(jr3d_p r)
{
	static const float verts[4 * 5] = {
		/* x,    y,    z,   u,  v */
		-0.5f, -0.5f, 0,  0, 1,   /* 左下 */
		 0.5f, -0.5f, 0,  1, 1,   /* 右下 */
		 0.5f,  0.5f, 0,  1, 0,   /* 右上 */
		-0.5f,  0.5f, 0,  0, 0,   /* 左上 */
	};
	static const Uint32 indices[6] = { 0, 1, 2, 0, 2, 3 };

	render_mesh_p m = (render_mesh_p)SDL_malloc(sizeof(render_mesh_t));
	if (!m) return NULL;
	m->buffer = (SDL_GPUBuffer*)r3d_upload_buffer(r->dev, verts, sizeof(verts));
	m->index_buffer = (SDL_GPUBuffer*)r3d_upload_buffer(r->dev, indices, sizeof(indices));
	if (!m->buffer || !m->index_buffer) {
		if (m->buffer) SDL_ReleaseGPUBuffer(r->dev, m->buffer);
		if (m->index_buffer) SDL_ReleaseGPUBuffer(r->dev, m->index_buffer);
		SDL_free(m);
		return NULL;
	}
	m->vertex_count = 4;
	m->index_count = 6;
	return m;
}

/* ---- 光照网格（pos FLOAT3 + normal FLOAT3 + uv FLOAT2, 8 float/顶点, 无索引） ---- */

render_mesh_p r3d_mesh_cube_lit(jr3d_p r, float w, float h, float d)
{
	float hw = w * 0.5f, hh = h * 0.5f, hd = d * 0.5f;
	float verts[36 * 8];
	render_mesh_p m;
	int vi = 0;
#define LIT_V(PX,PY,PZ,NX,NY,NZ,U,V) do { \
		verts[vi++]=(PX); verts[vi++]=(PY); verts[vi++]=(PZ); \
		verts[vi++]=(NX); verts[vi++]=(NY); verts[vi++]=(NZ); \
		verts[vi++]=(U);  verts[vi++]=(V); \
	} while (0)
	/* +Z */ LIT_V(-hw,-hh, hd,0,0, 1,0,0); LIT_V( hw,-hh, hd,0,0, 1,1,0); LIT_V( hw, hh, hd,0,0, 1,1,1);
	         LIT_V( hw, hh, hd,0,0, 1,1,1); LIT_V(-hw, hh, hd,0,0, 1,0,1); LIT_V(-hw,-hh, hd,0,0, 1,0,0);
	/* -Z */ LIT_V( hw,-hh,-hd,0,0,-1,0,0); LIT_V(-hw,-hh,-hd,0,0,-1,1,0); LIT_V(-hw, hh,-hd,0,0,-1,1,1);
	         LIT_V(-hw, hh,-hd,0,0,-1,1,1); LIT_V( hw, hh,-hd,0,0,-1,0,1); LIT_V( hw,-hh,-hd,0,0,-1,0,0);
	/* +X */ LIT_V( hw,-hh, hd,1,0,0,0,0); LIT_V( hw,-hh,-hd,1,0,0,1,0); LIT_V( hw, hh,-hd,1,0,0,1,1);
	         LIT_V( hw, hh,-hd,1,0,0,1,1); LIT_V( hw, hh, hd,1,0,0,0,1); LIT_V( hw,-hh, hd,1,0,0,0,0);
	/* -X */ LIT_V(-hw,-hh,-hd,-1,0,0,0,0); LIT_V(-hw,-hh, hd,-1,0,0,1,0); LIT_V(-hw, hh, hd,-1,0,0,1,1);
	         LIT_V(-hw, hh, hd,-1,0,0,1,1); LIT_V(-hw, hh,-hd,-1,0,0,0,1); LIT_V(-hw,-hh,-hd,-1,0,0,0,0);
	/* +Y */ LIT_V(-hw, hh, hd,0,1,0,0,0); LIT_V( hw, hh, hd,0,1,0,1,0); LIT_V( hw, hh,-hd,0,1,0,1,1);
	         LIT_V( hw, hh,-hd,0,1,0,1,1); LIT_V(-hw, hh,-hd,0,1,0,0,1); LIT_V(-hw, hh, hd,0,1,0,0,0);
	/* -Y */ LIT_V(-hw,-hh,-hd,0,-1,0,0,0); LIT_V( hw,-hh,-hd,0,-1,0,1,0); LIT_V( hw,-hh, hd,0,-1,0,1,1);
	         LIT_V( hw,-hh, hd,0,-1,0,1,1); LIT_V(-hw,-hh, hd,0,-1,0,0,1); LIT_V(-hw,-hh,-hd,0,-1,0,0,0);
#undef LIT_V
	m = (render_mesh_p)SDL_malloc(sizeof(render_mesh_t));
	if (!m) return NULL;
	m->buffer = (SDL_GPUBuffer*)r3d_upload_buffer(r->dev, verts, sizeof(verts));
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = 36;
	return m;
}

void r3d_mesh_destroy(jr3d_p r, render_mesh_p m)
{
	if (!r || !m) return;
	if (m->buffer) SDL_ReleaseGPUBuffer(r->dev, m->buffer);
	if (m->index_buffer) SDL_ReleaseGPUBuffer(r->dev, m->index_buffer);
	SDL_free(m);
}

render_mesh_p r3d_mesh_sphere_lit(jr3d_p r, float radius, int slices, int stacks)
{
	int stack, slice, vcount, vi;
	float* verts;
	render_mesh_p m;
	if (slices < 3) slices = 3;
	if (stacks < 2) stacks = 2;
	vcount = slices * stacks * 6;
	verts = (float*)SDL_malloc((size_t)vcount * 8 * sizeof(float));
	if (!verts) return NULL;
	vi = 0;
	for (stack = 0; stack < stacks; stack++) {
		float phi0 = (float)M_PI * (float)stack / (float)stacks;
		float phi1 = (float)M_PI * (float)(stack + 1) / (float)stacks;
		for (slice = 0; slice < slices; slice++) {
			float th0 = 2.0f * (float)M_PI * (float)slice / (float)slices;
			float th1 = 2.0f * (float)M_PI * (float)(slice + 1) / (float)slices;
			/* 四角点: a0(phi0,th0) a1(phi0,th1) b0(phi1,th0) b1(phi1,th1) */
			float ca0[3] = { sinf(phi0)*cosf(th0), cosf(phi0), sinf(phi0)*sinf(th0) };
			float ca1[3] = { sinf(phi0)*cosf(th1), cosf(phi0), sinf(phi0)*sinf(th1) };
			float cb0[3] = { sinf(phi1)*cosf(th0), cosf(phi1), sinf(phi1)*sinf(th0) };
			float cb1[3] = { sinf(phi1)*cosf(th1), cosf(phi1), sinf(phi1)*sinf(th1) };
			int i;
			float* tri[6];
			float u0 = (float)slice / (float)slices, u1 = (float)(slice + 1) / (float)slices;
			float v0 = (float)stack / (float)stacks, v1 = (float)(stack + 1) / (float)stacks;
			tri[0] = ca0; tri[1] = cb0; tri[2] = ca1;
			tri[3] = ca1; tri[4] = cb0; tri[5] = cb1;
			for (i = 0; i < 6; i++) {
				float u = (i == 0 || i == 2 || i == 3) ? u0 : u1;
				float v = (i == 0 || i == 1) ? v0 : v1;
				verts[vi++] = tri[i][0] * radius;
				verts[vi++] = tri[i][1] * radius;
				verts[vi++] = tri[i][2] * radius;
				verts[vi++] = tri[i][0];
				verts[vi++] = tri[i][1];
				verts[vi++] = tri[i][2];
				verts[vi++] = u;
				verts[vi++] = v;
			}
		}
	}
	m = (render_mesh_p)SDL_malloc(sizeof(render_mesh_t));
	if (!m) { SDL_free(verts); return NULL; }
	m->buffer = (SDL_GPUBuffer*)r3d_upload_buffer(r->dev, verts, (size_t)vcount * 8 * sizeof(float));
	SDL_free(verts);
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = vcount;
	return m;
}

render_mesh_p r3d_mesh_plane_lit(jr3d_p r, float w, float d, int segs)
{
	int x, z, vcount, vi;
	float* verts;
	render_mesh_p m;
	if (segs < 1) segs = 1;
	vcount = segs * segs * 6;
	verts = (float*)SDL_malloc((size_t)vcount * 8 * sizeof(float));
	if (!verts) return NULL;
	vi = 0;
	for (z = 0; z < segs; z++) {
		for (x = 0; x < segs; x++) {
			float x0 = (float)x / (float)segs * w - w * 0.5f;
			float x1 = (float)(x + 1) / (float)segs * w - w * 0.5f;
			float z0 = (float)z / (float)segs * d - d * 0.5f;
			float z1 = (float)(z + 1) / (float)segs * d - d * 0.5f;
			float u0 = (float)x / (float)segs, u1 = (float)(x + 1) / (float)segs;
			float v0 = (float)z / (float)segs, v1 = (float)(z + 1) / (float)segs;
			/* 两个三角形 */
			verts[vi++]=x0; verts[vi++]=0; verts[vi++]=z0; verts[vi++]=0; verts[vi++]=1; verts[vi++]=0; verts[vi++]=u0; verts[vi++]=v0;
			verts[vi++]=x1; verts[vi++]=0; verts[vi++]=z0; verts[vi++]=0; verts[vi++]=1; verts[vi++]=0; verts[vi++]=u1; verts[vi++]=v0;
			verts[vi++]=x0; verts[vi++]=0; verts[vi++]=z1; verts[vi++]=0; verts[vi++]=1; verts[vi++]=0; verts[vi++]=u0; verts[vi++]=v1;
			verts[vi++]=x0; verts[vi++]=0; verts[vi++]=z1; verts[vi++]=0; verts[vi++]=1; verts[vi++]=0; verts[vi++]=u0; verts[vi++]=v1;
			verts[vi++]=x1; verts[vi++]=0; verts[vi++]=z0; verts[vi++]=0; verts[vi++]=1; verts[vi++]=0; verts[vi++]=u1; verts[vi++]=v0;
			verts[vi++]=x1; verts[vi++]=0; verts[vi++]=z1; verts[vi++]=0; verts[vi++]=1; verts[vi++]=0; verts[vi++]=u1; verts[vi++]=v1;
		}
	}
	m = (render_mesh_p)SDL_malloc(sizeof(render_mesh_t));
	if (!m) { SDL_free(verts); return NULL; }
	m->buffer = (SDL_GPUBuffer*)r3d_upload_buffer(r->dev, verts, (size_t)vcount * 8 * sizeof(float));
	SDL_free(verts);
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = vcount;
	return m;
}

render_mesh_p r3d_mesh_ramp_lit(jr3d_p r, float w, float l, float h)
{
	/* 三角棱柱斜坡: 底在 y=0, 沿 +Z 从 0 升到 h */
	float L = sqrtf(l * l + h * h);
	float sn = h / L, cn = l / L;   /* 坡面法线 (0, sn, -cn) */
	float verts[30 * 8];   /* 5 面 × 6 顶点 = 30 顶点, 每顶点 8 float */
	render_mesh_p m;
	int vi = 0;
	/* A(0,0,0) B(w,0,0) C(w,h,l) D(0,h,l) E(0,0,l) F(w,0,l) */
#define RV(PX,PY,PZ,NX,NY,NZ,U,V) do { \
		verts[vi++]=(PX); verts[vi++]=(PY); verts[vi++]=(PZ); \
		verts[vi++]=(NX); verts[vi++]=(NY); verts[vi++]=(NZ); \
		verts[vi++]=(U);  verts[vi++]=(V); \
	} while (0)
	/* 底面 (0,-1,0) */
	RV(0,0,0, 0,-1,0, 0,0); RV(w,0,0, 0,-1,0, 1,0); RV(w,0,l, 0,-1,0, 1,1);
	RV(w,0,l, 0,-1,0, 1,1); RV(0,0,l, 0,-1,0, 0,1); RV(0,0,0, 0,-1,0, 0,0);
	/* 坡面 (0, sn, -cn) */
	RV(0,0,0, 0, sn,-cn, 0,0); RV(w,0,0, 0, sn,-cn, 1,0); RV(w,h,l, 0, sn,-cn, 1,1);
	RV(w,h,l, 0, sn,-cn, 1,1); RV(0,h,l, 0, sn,-cn, 0,1); RV(0,0,0, 0, sn,-cn, 0,0);
	/* 背面 z=l (0,0,1) */
	RV(0,0,l, 0,0, 1, 0,0); RV(w,0,l, 0,0, 1, 1,0); RV(w,h,l, 0,0, 1, 1,1);
	RV(w,h,l, 0,0, 1, 1,1); RV(0,h,l, 0,0, 1, 0,1); RV(0,0,l, 0,0, 1, 0,0);
	/* 左面 x=0 (-1,0,0) */
	RV(0,0,0,-1,0,0, 0,0); RV(0,0,l,-1,0,0, 1,0); RV(0,h,l,-1,0,0, 1,1);
	RV(0,h,l,-1,0,0, 1,1); RV(0,0,l,-1,0,0, 0,1); RV(0,0,0,-1,0,0, 0,0);
	/* 右面 x=w (1,0,0) */
	RV(w,0,0, 1,0,0, 0,0); RV(w,h,l, 1,0,0, 1,1); RV(w,0,l, 1,0,0, 1,0);
	RV(w,h,l, 1,0,0, 1,1); RV(w,0,0, 1,0,0, 0,0); RV(w,0,l, 1,0,0, 0,0);
#undef RV
	m = (render_mesh_p)SDL_malloc(sizeof(render_mesh_t));
	if (!m) return NULL;
	m->buffer = (SDL_GPUBuffer*)r3d_upload_buffer(r->dev, verts, sizeof(verts));
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = 30;
	return m;
}

render_mesh_p r3d_mesh_custom_lit(jr3d_p r, const void* verts, int vertex_count)
{
	render_mesh_p m;
	m = (render_mesh_p)SDL_malloc(sizeof(render_mesh_t));
	if (!m) return NULL;
	m->buffer = (SDL_GPUBuffer*)r3d_upload_buffer(r->dev, verts, (size_t)vertex_count * 8 * sizeof(float));
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = vertex_count;
	return m;
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

	if (m->index_buffer && m->index_count > 0) {
		SDL_GPUBufferBinding ibind;
		memset(&ibind, 0, sizeof(ibind));
		ibind.buffer = m->index_buffer; ibind.offset = 0;
		SDL_BindGPUIndexBuffer(r->rp, &ibind, SDL_GPU_INDEXELEMENTSIZE_32BIT);
		SDL_DrawGPUIndexedPrimitives(r->rp, m->index_count, 1, 0, 0, 0);
	} else {
		SDL_DrawGPUPrimitives(r->rp, m->vertex_count, 1, 0, 0);
	}
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

/* billboard 绘制: 单位 quad 经 right/up 基向量旋转 + size 缩放 + center 平移 */
void r3d_draw_mesh_tex_basis(jr3d_p r, render_mesh_p m,
	vec3_t center, vec3_t right, vec3_t up, float size, void* tex)
{
	if (!r || !m) return;
	mat44_t model = { 0 };
	/* 列主序: 第0列 = right*size, 第1列 = up*size, 第2列 = right×up(normal), 第3列 = center */
	model.m0  = right.x * size;  model.m1  = right.y * size;  model.m2  = right.z * size;
	model.m4  = up.x * size;     model.m5  = up.y * size;     model.m6  = up.z * size;
	model.m8  = right.y * up.z - right.z * up.y;   /* n = right × up */
	model.m9  = right.z * up.x - right.x * up.z;
	model.m10 = right.x * up.y - right.y * up.x;
	model.m12 = center.x; model.m13 = center.y; model.m14 = center.z;
	model.m15 = 1;
	draw_mesh_internal(r, m, &model, (SDL_GPUTexture*)tex);
}

/* ---- 光照绘制（需在 r3d_begin/r3d_end 之间调用） ---- */

void r3d_set_light_block(jr3d_p r, const jr3d_light_block_t* block)
{
	if (!r || !block) return;
	r->light_block = *block;
	r->light_block_valid = true;
}

void r3d_draw_mesh_lit(jr3d_p r, render_mesh_p m, const mat44_t* model, void* tex)
{
	render_mvp_t mvp;
	SDL_GPUTextureSamplerBinding samp;
	SDL_GPUBufferBinding bind;
	if (!r || !r->rp || !r->lighting_pipeline || !r->light_block_valid || !m) return;
	/* 拦截明显非法的 buffer 指针（如 0xFFFFFFFFFFFFFFFF / 内核地址），避免崩溃 */
	if ((uintptr_t)m->buffer < (uintptr_t)0x1000 || (uintptr_t)m->buffer == (uintptr_t)-1) {
		log_error("r3d_draw_mesh_lit: 跳过非法 mesh buffer %p", (void*)m->buffer);
		return;
	}

	memset(&mvp, 0, sizeof(mvp));
	r3d_mat44_to_float16(&r->viewProj, mvp.viewProj);
	r3d_mat44_to_float16(model, mvp.model);
	SDL_PushGPUVertexUniformData(r->cmd, 0, &mvp, sizeof(mvp));
	SDL_PushGPUFragmentUniformData(r->cmd, 0, &r->light_block, sizeof(r->light_block));

	SDL_BindGPUGraphicsPipeline(r->rp, r->lighting_pipeline);

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

void r3d_draw_mesh_lit_color(jr3d_p r, render_mesh_p m, const mat44_t* model, jcolor_t color)
{
	SDL_GPUTexture* tex;
	if (!r) return;
	tex = get_color_texture(r, color.r, color.g, color.b);
	r3d_draw_mesh_lit(r, m, model, tex);
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
		SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, SDL_GPU_TEXTUREUSAGE_SAMPLER, 0);
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
