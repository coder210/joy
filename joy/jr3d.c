#include "joy.h"
#include <SDL3/SDL.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* stb_truetype 实现已由 jimpl.c 编译一次(含 STB_TRUETYPE_IMPLEMENTATION),
   此处仅包含头文件以使用其声明, 避免多重定义 */
#include "external/stb_truetype.h"

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

	/* 字体渲染（stb_truetype）: 整段文字烤成一张纹理, 由引擎统一管理生命周期 */
	stbtt_fontinfo     font_info;
	unsigned char*     font_ttf_buf;   /* stbtt 内部持有偏移指针, 绝不在程序结束前释放 */
	float              font_scale;
	int                font_ascent;
	int                font_pix_h;
	SDL_GPUTexture*    font_tex;       /* 当前字符串烘焙的整张纹理 */
	int                font_tex_w;
	int                font_tex_h;
	char               font_cache[256]; /* 已烘焙的字符串缓存 */
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
	if (r->font_tex) SDL_ReleaseGPUTexture(r->dev, r->font_tex);
	if (r->dev) {
		SDL_ReleaseWindowFromGPUDevice(r->dev, r->win);
		SDL_DestroyGPUDevice(r->dev);
	}
	/* stbtt 持有 font_ttf_buf 内部偏移指针, 仅程序退出时释放(可接受) */
	if (r->font_ttf_buf) free(r->font_ttf_buf);
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

/* 前向声明: 用自定义投影矩阵绘制(供字体 2D/3D 函数使用, 定义在下方) */
static void draw_mesh_internal_proj(jr3d_p r, render_mesh_p m, mat44_t* model, mat44_t* proj, SDL_GPUTexture* tex);

/* ================================================================
 * 字体渲染 (stb_truetype)
 *   整段文字烤成一张 RGBA32 纹理(白字形 + alpha), UV (0,0)-(1,1),
 *   通过 r3d_draw_mesh_tex_basis 以 billboard 方式贴到 3D 空间。
 *   算法沿用原 main_tex.cpp 中已验证可用的 yaw/pitch 基向量计算,
 *   保证文字始终直立、不斜切。
 * ================================================================ */

/* 初始化字体: 逐行照抄原版 font_init, 仅 gFont* -> r->font* */
bool r3d_font_init(jr3d_p r, const char* ttf_path, int pixel_height)
{
	if (!r || !ttf_path || pixel_height <= 0) return false;

	// 读取 ttf (工作目录为 build/<config>, 字体在 joy_editor/fonts/arial.ttf)
	FILE* f = fopen(ttf_path, "rb");
	if (!f) { SDL_Log("font: open %s failed", ttf_path); return false; }
	fseek(f, 0, SEEK_END);
	long sz = ftell(f);
	fseek(f, 0, SEEK_SET);
	r->font_ttf_buf = (unsigned char*)SDL_malloc(sz);
	if (!r->font_ttf_buf) { fclose(f); return false; }
	fread(r->font_ttf_buf, 1, sz, f);
	fclose(f);

	// 注意: stbtt_fontinfo 内部保存指向 font_ttf_buf 的偏移指针, 该缓冲必须存活到程序结束, 不能 free
	if (!stbtt_InitFont(&r->font_info, r->font_ttf_buf, 0)) { SDL_Log("font: InitFont failed"); return false; }
	r->font_scale = stbtt_ScaleForPixelHeight(&r->font_info, (float)pixel_height);

	int ascent = 0, descent = 0, lineGap = 0;
	stbtt_GetFontVMetrics(&r->font_info, &ascent, &descent, &lineGap);
	r->font_ascent = (int)(ascent * r->font_scale);
	r->font_pix_h  = pixel_height;
	r->font_tex    = NULL;
	r->font_cache[0] = '\0';
	SDL_Log("font: ready scale=%.3f ascent=%d", r->font_scale, r->font_ascent);
	return true;
}

/* 把整段字符串烘焙成一张 RGBA 纹理(白字 + alpha), 逐行照抄原版 bake_text_texture,
   仅 gFont 系列全局变量 -> r->font 结构体字段, gTextTex -> r->font_tex
   (固定宽度 gFontPixH*n, 原版对齐/裁剪, flip=0) */
static bool r3d_font_bake(jr3d_p r, const char* str)
{
	if (str == NULL) return false;
	// 字符串未变则不重建
	if (r->font_tex && SDL_strcmp(str, r->font_cache) == 0) return true;

	int n = (int)SDL_strlen(str);
	if (n <= 0) { r->font_tex_w = 1; r->font_tex_h = (int)r->font_pix_h; return false; }

	int real_width = r->font_pix_h * n;          // 预估宽度(按字节+余量, 码点不会超过此宽)
	int real_height = (int)r->font_pix_h;
	unsigned char* bitmap = (unsigned char*)SDL_calloc((size_t)real_width * real_height + 1, 1);
	if (!bitmap) return false;

	int x = 0;
	size_t pos = 0;
	int32_t code;
	while (pos < (size_t)n) {
		size_t consumed = utf8_decode(str + pos, (size_t)n - pos, &code);
		if (consumed == 0) { code = '?'; consumed = 1; }   // 非法字节: 用 '?' 占位
		int c = (int)code;
		int advance_width = 0, left_side_bearing = 0;
		int c_x1, c_y1, c_x2, c_y2;
		stbtt_GetCodepointHMetrics(&r->font_info, c, &advance_width, &left_side_bearing);
		stbtt_GetCodepointBitmapBox(&r->font_info, c, r->font_scale, r->font_scale, &c_x1, &c_y1, &c_x2, &c_y2);
		int bw = c_x2 - c_x1, bh = c_y2 - c_y1;
		int y = r->font_ascent + c_y1;          // 顶部对齐(ascent 基线), 同 jr2d
		if (bw > 0 && bh > 0) {
			// 先渲染到临时 buffer, 再按 y 偏移复制到整行位图(避免负偏移越界)
			unsigned char* tmp = (unsigned char*)SDL_malloc((size_t)bw * bh);
			if (tmp) {
				stbtt_MakeCodepointBitmap(&r->font_info, tmp, bw, bh, bw, r->font_scale, r->font_scale, c);
				int bx = x + (int)(left_side_bearing * r->font_scale);
				for (int yy = 0; yy < bh; yy++) {
					int dy = y + yy;
					if (dy < 0 || dy >= real_height) continue;
					for (int xx = 0; xx < bw; xx++) {
						int dx = bx + xx;
						if (dx < 0 || dx >= real_width) continue;
						bitmap[dy * real_width + dx] = tmp[yy * bw + xx];
					}
				}
				SDL_free(tmp);
			}
		}
		x += (int)(advance_width * r->font_scale);
		pos += consumed;
	}
	int final_w = x > 0 ? x : 1;

	// 转成 RGBA32(白字 + alpha)。位图第 0 行=顶部, 但 quad UV 是 (0,0)=左上对应纹理顶行,
	// 而 GPU 纹理第 0 行在底部, 这里把行上下翻转写入, 使纹理内容正确为正向。
	unsigned char* rgba = (unsigned char*)SDL_malloc((size_t)final_w * real_height * 4);
	if (!rgba) { SDL_free(bitmap); return false; }
	for (int yy = 0; yy < real_height; yy++) {
		int src_y = real_height - 1 - yy;   // 上下翻转: 顶部行写到纹理底, 底行写到顶
		for (int xx = 0; xx < final_w; xx++) {
			unsigned char a = bitmap[src_y * real_width + xx];
			int idx = (yy * final_w + xx) * 4;
			rgba[idx + 0] = 255;
			rgba[idx + 1] = 255;
			rgba[idx + 2] = 255;
			rgba[idx + 3] = a;
		}
	}
	SDL_free(bitmap);

	if (r->font_tex) { SDL_ReleaseGPUTexture(r->dev, r->font_tex); r->font_tex = NULL; }
	r->font_tex = (SDL_GPUTexture*)r3d_create_texture(r, final_w, real_height, rgba, 0);
	SDL_free(rgba);
	r->font_tex_w = final_w;
	r->font_tex_h = real_height;
	SDL_strlcpy(r->font_cache, str, sizeof(r->font_cache));
	SDL_Log("font: baked text '%s' %dx%d tex=%p", str, final_w, real_height, r->font_tex);
	return true;
}

/* 在 3D 空间绘制文字 billboard。
   topLeft: 文字左上角锚点(世界坐标); yaw/pitch: 屏幕基向量方向(与相机一致);
   size: 文字在世界中的高度。 right/up 由 yaw/pitch 推得, 保证文字直立。 */
bool r3d_font_draw_3d(jr3d_p r, render_mesh_p m, const char* text,
					  vec3_t topLeft, float yaw, float pitch, float size)
{
	if (!r || !m || !text || size <= 0) return false;
	if (!r->font_ttf_buf) { SDL_Log("r3d_font_draw_3d: font not init"); return false; }

	if (SDL_strcmp(r->font_cache, text) != 0) {
		if (!r3d_font_bake(r, text)) return false;
	}

	float cy = cosf(yaw), sy = sinf(yaw);
	float cp = cosf(pitch), sp = sinf(pitch);
	vec3_t right = { -cy, 0.0f, sy };
	vec3_t up    = { sp * sy, cp, sp * sy };
	float ulen = sqrtf(up.x * up.x + up.y * up.y + up.z * up.z);
	if (ulen > 1e-4f) { up.x /= ulen; up.y /= ulen; up.z /= ulen; }

	float aspect = (float)r->font_tex_w / (float)r->font_tex_h;
	float worldH = size;
	float worldW = size * aspect;
	vec3_t rightScaled = { right.x * aspect, right.y * aspect, right.z * aspect };
	vec3_t center = {
		topLeft.x + rightScaled.x * (worldW * 0.5f) + up.x * (worldH * 0.5f),
		topLeft.y + rightScaled.y * (worldW * 0.5f) + up.y * (worldH * 0.5f),
		topLeft.z + rightScaled.z * (worldW * 0.5f) + up.z * (worldH * 0.5f),
	};
	r3d_draw_mesh_tex_basis(r, m, center, rightScaled, up, worldH, (void*)r->font_tex);
	return true;
}

/* 在 3D 空间绘制文字, 朝向固定(不随相机 yaw/pitch 变化): 始终水平直立、正对 -Z。
   topLeft: 文字左上角锚点(世界坐标); size: 文字世界高度。 */
bool r3d_font_draw_fixed(jr3d_p r, render_mesh_p m, const char* text,
						 vec3_t topLeft, float size)
{
	if (!r || !m || !text || size <= 0) return false;
	if (!r->font_ttf_buf) { SDL_Log("r3d_font_draw_fixed: font not init"); return false; }

	if (SDL_strcmp(r->font_cache, text) != 0) {
		if (!r3d_font_bake(r, text)) return false;
	}

	/* 固定基向量: right 沿世界 -X, up 沿世界 +Y, 文字方向恒定 */
	vec3_t right = { -1.0f, 0.0f, 0.0f };
	vec3_t up    = {  0.0f, 1.0f, 0.0f };

	float aspect = (float)r->font_tex_w / (float)r->font_tex_h;
	float worldH = size;
	float worldW = size * aspect;
	vec3_t rightScaled = { right.x * aspect, right.y * aspect, right.z * aspect };
	vec3_t center = {
		topLeft.x + rightScaled.x * (worldW * 0.5f) + up.x * (worldH * 0.5f),
		topLeft.y + rightScaled.y * (worldW * 0.5f) + up.y * (worldH * 0.5f),
		topLeft.z + rightScaled.z * (worldW * 0.5f) + up.z * (worldH * 0.5f),
	};
	r3d_draw_mesh_tex_basis(r, m, center, rightScaled, up, worldH, (void*)r->font_tex);
	return true;
}

/* 在屏幕(窗口)坐标绘制文字, 像 2D HUD 一样固定在窗口左上角, 不随相机旋转/移动。
   pixelX/pixelY: 文字左上角在窗口中的像素坐标(原点左上, Y 向下);
   fontSize: 文字像素高度; winW/winH: 窗口像素尺寸(通常传 r->ww/r->wh)。 */
bool r3d_font_draw_2d(jr3d_p r, render_mesh_p m, const char* text,
					  float pixelX, float pixelY, float fontSize,
					  float winW, float winH)
{
	if (!r || !m || !text || fontSize <= 0 || winW <= 0 || winH <= 0) return false;
	if (!r->font_ttf_buf) { SDL_Log("r3d_font_draw_2d: font not init"); return false; }

	if (SDL_strcmp(r->font_cache, text) != 0) {
		if (!r3d_font_bake(r, text)) return false;
	}

	float aspect = (float)r->font_tex_w / (float)r->font_tex_h;
	float worldW = fontSize * aspect;
	float worldH = fontSize;

	/* model: 把单位 quad([-0.5,0.5]) 缩放到像素尺寸并平移到屏幕像素位置(中心对齐) */
	mat44_t model; memset(&model, 0, sizeof(model));
	model.m0  = worldW;
	model.m5  = worldH;
	model.m10 = 1.0f;
	model.m12 = pixelX + worldW * 0.5f;
	model.m13 = pixelY + worldH * 0.5f;
	model.m15 = 1.0f;

	/* 屏幕像素正交投影: 原点左上, X 向右, Y 向下, 覆盖整个窗口。
	   这是纯屏幕坐标映射, 不修正文字朝向——文字朝向由纹理内容保证。 */
	mat44_t proj; memset(&proj, 0, sizeof(proj));
	proj.m0  =  2.0f / winW;
	proj.m5  = -2.0f / winH;
	proj.m10 =  1.0f;
	proj.m12 = -1.0f;
	proj.m13 =  1.0f;
	proj.m15 =  1.0f;

	draw_mesh_internal_proj(r, m, &model, &proj, (SDL_GPUTexture*)r->font_tex);
	return true;
}

void r3d_get_window_size(jr3d_p r, int* outW, int* outH)
{
	if (!r) return;
	if (outW) *outW = r->last_ww;
	if (outH) *outH = r->last_wh;
}

/* ================================================================
 * 2D UI: button (仿 jr2d, 用屏幕正交投影渲染, 不随相机变化)
 * ================================================================ */

/* 内部: 在屏幕像素坐标绘制一个实心矩形(左上角 x,y, 尺寸 w,h) */
static void draw_rect_2d(jr3d_p r, render_mesh_p m,
						 float x, float y, float w, float h,
						 Uint8 cr, Uint8 cg, Uint8 cb,
						 float winW, float winH)
{
	if (!r || !m || w <= 0 || h <= 0 || winW <= 0 || winH <= 0) return;

	mat44_t model; memset(&model, 0, sizeof(model));
	model.m0  = w;
	model.m5  = h;
	model.m10 = 1.0f;
	model.m12 = x + w * 0.5f;
	model.m13 = y + h * 0.5f;
	model.m15 = 1.0f;

	mat44_t proj; memset(&proj, 0, sizeof(proj));
	proj.m0  =  2.0f / winW;
	proj.m5  = -2.0f / winH;
	proj.m10 =  1.0f;
	proj.m12 = -1.0f;
	proj.m13 =  1.0f;
	proj.m15 =  1.0f;

	SDL_GPUTexture* tex = get_color_texture(r, cr, cg, cb);
	draw_mesh_internal_proj(r, m, &model, &proj, tex);
}

jr3d_button_p r3d_button_create(float x, float y, float w, float h, const char* text)
{
	jr3d_button_p b = (jr3d_button_p)SDL_malloc(sizeof(jr3d_button_t));
	if (!b) return NULL;
	b->x = x; b->y = y; b->w = w; b->h = h;
	b->text[0] = '\0';
	if (text) SDL_strlcpy(b->text, text, sizeof(b->text));
	b->state = JR3D_BTN_NORMAL;
	b->pressedInside = false;
	b->onClick = NULL;
	return b;
}

void r3d_button_set_onclick(jr3d_button_p btn, void (*cb)(jr3d_button_p))
{
	if (btn) btn->onClick = cb;
}

bool r3d_button_is_hovered(const jr3d_button_p btn, float mx, float my)
{
	if (!btn) return false;
	return (mx >= btn->x && mx <= btn->x + btn->w &&
			my >= btn->y && my <= btn->y + btn->h);
}

bool r3d_button_is_pressed(jr3d_button_p btn, bool mouse_down, float mx, float my)
{
	if (!btn) return false;
	bool hover = r3d_button_is_hovered(btn, mx, my);
	if (hover && mouse_down) {
		btn->state = JR3D_BTN_PRESSED;
		btn->pressedInside = true;
		return true;
	}
	btn->state = hover ? JR3D_BTN_HOVER : JR3D_BTN_NORMAL;
	return false;
}

void r3d_button_release(jr3d_button_p btn, float mx, float my)
{
	if (!btn) return;
	bool hover = r3d_button_is_hovered(btn, mx, my);
	/* 完整 click: 按下在按钮内 且 松开也在按钮内 -> 触发回调 */
	if (btn->pressedInside && hover && btn->onClick) {
		btn->onClick(btn);
	}
	btn->pressedInside = false;
	btn->state = hover ? JR3D_BTN_HOVER : JR3D_BTN_NORMAL;
}

void r3d_button_draw(jr3d_p r, render_mesh_p m, jr3d_button_p btn, float winW, float winH)
{
	if (!r || !m || !btn || winW <= 0 || winH <= 0) return;

	/* 配色(仿 jr2d button): normal 灰 / hover 浅灰 / pressed 深灰, 黑边框 */
	Uint8 fr = 130, fg = 130, fb = 130;
	if (btn->state == JR3D_BTN_HOVER)   { fr = 160; fg = 160; fb = 160; }
	if (btn->state == JR3D_BTN_PRESSED) { fr = 90;  fg = 90;  fb = 90;  }

	/* 填充矩形 */
	draw_rect_2d(r, m, btn->x, btn->y, btn->w, btn->h, fr, fg, fb, winW, winH);
	/* 黑边框(4 条 2px 粗) */
	float bw = 2.0f;
	draw_rect_2d(r, m, btn->x,                btn->y,                btn->w, bw,     0, 0, 0, winW, winH);
	draw_rect_2d(r, m, btn->x,                btn->y + btn->h - bw,  btn->w, bw,     0, 0, 0, winW, winH);
	draw_rect_2d(r, m, btn->x,                btn->y,                bw,    btn->h,  0, 0, 0, winW, winH);
	draw_rect_2d(r, m, btn->x + btn->w - bw,  btn->y,                bw,    btn->h,  0, 0, 0, winW, winH);

	/* 按钮文字: 居中绘制 */
	if (r->font_ttf_buf && btn->text[0]) {
		float fontSize = btn->h * 0.5f;                 // 字号约为按钮高度一半
		float aspect = (float)r->font_tex_w / (float)r->font_tex_h;
		/* 用最新烘焙的文字测宽(若缓存的不是本按钮文字, 先烘焙) */
		if (SDL_strcmp(r->font_cache, btn->text) != 0) {
			r3d_font_bake(r, btn->text);
		}
		float textW = fontSize * ((float)r->font_tex_w / (float)r->font_tex_h);
		float tx = btn->x + (btn->w - textW) * 0.5f;
		float ty = btn->y + (btn->h - fontSize) * 0.5f;
		r3d_font_draw_2d(r, m, btn->text, tx, ty, fontSize, winW, winH);
		(void)aspect;
	}
}

void r3d_button_destroy(jr3d_button_p btn)
{
	if (btn) SDL_free(btn);
}

/* ================================================================
 * 2D UI: label (仿 jr2d, 纯文字 2D HUD, 固定屏幕位置, 不随相机变化)
 * ================================================================ */

/* label 类型已在 joy.h 中定义 */
jr3d_label_p r3d_label_create(float x, float y, float fontSize, uint8_t cr, uint8_t cg, uint8_t cb, const char* text)
{
	jr3d_label_p l = (jr3d_label_p)SDL_malloc(sizeof(jr3d_label_t));
	if (!l) return NULL;
	l->x = x; l->y = y; l->fontSize = fontSize;
	l->r = cr; l->g = cg; l->b = cb;
	l->text[0] = '\0';
	if (text) SDL_strlcpy(l->text, text, sizeof(l->text));
	return l;
}

/* 更新文字内容(支持动态文本如 FPS) */
void r3d_label_set_text(jr3d_label_p l, const char* text)
{
	if (l && text) SDL_strlcpy(l->text, text, sizeof(l->text));
}

void r3d_label_draw(jr3d_p r, render_mesh_p m, jr3d_label_p l, float winW, float winH)
{
	if (!r || !m || !l || winW <= 0 || winH <= 0) return;
	if (!r->font_ttf_buf || !l->text[0]) return;
	r3d_font_draw_2d(r, m, l->text, l->x, l->y, l->fontSize, winW, winH);
}

void r3d_label_destroy(jr3d_label_p l)
{
	if (l) SDL_free(l);
}

/* ================================================================
 * 2D UI: input (仿 jr2d inputbox, 屏幕像素坐标, 不随相机变化)
 * ================================================================ */

/* 测量 str 前 byte_len 字节的像素宽度(UTF-8 安全; 用 stbtt 累加 advance, 不重建 GPU 纹理) */
static float r3d_font_measure(jr3d_p r, const char* str, int byte_len)
{
	if (!r->font_ttf_buf || !str || byte_len <= 0) return 0.0f;
	float w = 0.0f;
	int pos = 0;
	int32_t code;
	while (pos < byte_len) {
		size_t consumed = utf8_decode(str + pos, (size_t)(byte_len - pos), &code);
		if (consumed == 0) break;
		int advance = 0, lsb = 0;
		stbtt_GetCodepointHMetrics(&r->font_info, (int)code, &advance, &lsb);
		w += advance * r->font_scale;
		pos += (int)consumed;
	}
	return w;
}

/* 回退到上一个 UTF-8 字符起点(字节偏移) */
static int r3d_input_prev_char(const char* s, int pos)
{
	int p = pos;
	if (p <= 0) return 0;
	p--;   /* 先退到前一个字节, 再跳过 UTF-8 续字节回到字符起点 */
	while (p > 0 && ((unsigned char)s[p] & 0xC0) == 0x80) p--;
	return p;
}

/* 前进到下一个 UTF-8 字符起点(字节偏移) */
static int r3d_input_next_char(const char* s, int pos, int len)
{
	int p = pos;
	if (p >= len) return len;
	size_t consumed = utf8_decode(s + p, (size_t)(len - p), &(int32_t){0});
	if (consumed == 0) return len;
	return p + (int)consumed;
}

jr3d_input_p r3d_input_create(float x, float y, float w, float h, float fontSize,
                              uint8_t cr, uint8_t cg, uint8_t cb, const char* text)
{
	jr3d_input_p inp = (jr3d_input_p)SDL_malloc(sizeof(jr3d_input_t));
	if (!inp) return NULL;
	inp->x = x; inp->y = y; inp->w = w; inp->h = h;
	inp->fontSize = fontSize;
	inp->r = cr; inp->g = cg; inp->b = cb;
	inp->text[0] = '\0';
	if (text) SDL_strlcpy(inp->text, text, sizeof(inp->text));
	inp->cursor_pos = (int)SDL_strlen(inp->text);
	inp->scroll = 0;
	inp->focused = false;
	inp->cursor_timer = SDL_GetTicks();
	inp->cursor_visible = true;
	inp->sel_start = -1;
	inp->sel_end = -1;
	inp->on_submit = NULL;
	return inp;
}

void r3d_input_set_text(jr3d_input_p inp, const char* text)
{
	if (!inp) return;
	inp->text[0] = '\0';
	if (text) SDL_strlcpy(inp->text, text, sizeof(inp->text));
	inp->cursor_pos = (int)SDL_strlen(inp->text);
	inp->scroll = 0;
}

const char* r3d_input_get_text(jr3d_input_p inp)
{
	return inp ? inp->text : "";
}

bool r3d_input_is_hovered(const jr3d_input_p inp, float mx, float my)
{
	if (!inp) return false;
	return (mx >= inp->x && mx <= inp->x + inp->w &&
			my >= inp->y && my <= inp->y + inp->h);
}

void r3d_input_focus(jr3d_input_p inp)
{
	if (inp) { inp->focused = true; inp->cursor_visible = true; inp->cursor_timer = SDL_GetTicks(); }
}

void r3d_input_defocus(jr3d_input_p inp)
{
	if (inp) inp->focused = false;
}

bool r3d_input_is_focused(const jr3d_input_p inp)
{
	return inp ? inp->focused : false;
}

void r3d_input_insert(jr3d_input_p inp, const char* s)
{
	if (!inp || !s) return;
	size_t cur = SDL_strlen(inp->text);
	size_t add = SDL_strlen(s);
	size_t cap = sizeof(inp->text) - 1;
	if (cur + add > cap) add = cap - cur;          // 截断到容量
	if (add == 0) return;
	/* 在 cursor_pos 处插入 */
	SDL_memmove(inp->text + inp->cursor_pos + add,
				inp->text + inp->cursor_pos,
				cur - inp->cursor_pos + 1);          // 含结尾 '\0'
	SDL_memcpy(inp->text + inp->cursor_pos, s, add);
	inp->cursor_pos += (int)add;
}

void r3d_input_backspace(jr3d_input_p inp)
{
	if (!inp || inp->cursor_pos <= 0) return;
	int start = r3d_input_prev_char(inp->text, inp->cursor_pos);
	int ch_len = inp->cursor_pos - start;
	size_t cur = SDL_strlen(inp->text);
	SDL_memmove(inp->text + start, inp->text + inp->cursor_pos, cur - inp->cursor_pos + 1);
	inp->cursor_pos = start;
}

void r3d_input_delete(jr3d_input_p inp)
{
	if (!inp) return;
	size_t cur = SDL_strlen(inp->text);
	if (inp->cursor_pos >= (int)cur) return;
	int nxt = r3d_input_next_char(inp->text, inp->cursor_pos, (int)cur);
	SDL_memmove(inp->text + inp->cursor_pos, inp->text + nxt, cur - nxt + 1);
	/* cursor_pos 不变 */
}

void r3d_input_cursor_left(jr3d_input_p inp)
{
	if (!inp || inp->cursor_pos <= 0) return;
	inp->cursor_pos = r3d_input_prev_char(inp->text, inp->cursor_pos);
}

void r3d_input_cursor_right(jr3d_input_p inp)
{
	if (!inp) return;
	size_t cur = SDL_strlen(inp->text);
	if (inp->cursor_pos >= (int)cur) return;
	inp->cursor_pos = r3d_input_next_char(inp->text, inp->cursor_pos, (int)cur);
}

void r3d_input_cursor_home(jr3d_input_p inp)
{
	if (inp) inp->cursor_pos = 0;
}

void r3d_input_cursor_end(jr3d_input_p inp)
{
	if (inp) inp->cursor_pos = (int)SDL_strlen(inp->text);
}

void r3d_input_click(jr3d_p r, jr3d_input_p inp, float mx, float my)
{
	if (!inp) return;
	inp->focused = true;
	inp->cursor_timer = SDL_GetTicks();
	inp->cursor_visible = true;
	if (!r3d_input_is_hovered(inp, mx, my)) return;

	float padX = 4.0f;
	/* 换算系数: 让 measure(bake 像素宽) 与 r3d_font_draw_2d(整图按 fontSize 缩放) 的屏幕像素宽一致。
	   draw_2d 中 1 个 bake 像素 = fontSize / font_tex_h 个屏幕像素。 */
	float fontSize = inp->fontSize > 0 ? inp->fontSize : (inp->h * 0.6f);
	float k = r->font_tex_h > 0 ? (fontSize / (float)r->font_tex_h) : 1.0f;

	/* 点击位置对应文本屏幕像素(已含 scroll, 单位与 draw 一致) */
	float px = (mx - (inp->x + padX)) + (float)inp->scroll * k;

	int len = (int)SDL_strlen(inp->text);
	int pos = 0;
	float acc = 0.0f;
	int best = 0;
	float best_dist = 1e30f;
	while (pos <= len) {
		float d = SDL_fabsf(acc - px);
		if (d < best_dist) { best_dist = d; best = pos; }
		if (pos >= len) break;
		int nxt = r3d_input_next_char(inp->text, pos, len);
		acc += r3d_font_measure(r, inp->text + pos, nxt - pos) * k;
		pos = nxt;
	}
	inp->cursor_pos = best;
}

void r3d_input_set_onsubmit(jr3d_input_p inp, void (*cb)(jr3d_input_p))
{
	if (inp) inp->on_submit = cb;
}

void r3d_input_submit(jr3d_input_p inp)
{
	if (inp && inp->on_submit) inp->on_submit(inp);
}

/* ===================== 选中 / 编辑辅助 (对齐 jr2d inputbox) ===================== */

static void r3d_input_clear_sel(jr3d_input_p inp)
{
	if (!inp) return;
	inp->sel_start = inp->sel_end = -1;
}

static bool r3d_input_has_sel(const jr3d_input_p inp)
{
	return inp && inp->sel_start >= 0 && inp->sel_end >= 0 && inp->sel_start != inp->sel_end;
}

/* 删除选中文本, 光标置于选中起点 */
static void r3d_input_delete_sel(jr3d_input_p inp)
{
	if (!inp || !r3d_input_has_sel(inp)) return;
	int a = inp->sel_start, b = inp->sel_end;
	if (a > b) { int t = a; a = b; b = t; }
	int cur = (int)SDL_strlen(inp->text);
	SDL_memmove(inp->text + a, inp->text + b, cur - b + 1);
	inp->cursor_pos = a;
	r3d_input_clear_sel(inp);
}

/* 文本屏幕像素宽度(用与 r3d_font_draw_2d 一致的缩放) */
static float r3d_input_text_w(jr3d_p r, jr3d_input_p inp, const char* s, int n)
{
	float fontSize = inp->fontSize > 0 ? inp->fontSize : (inp->h * 0.6f);
	float k = r->font_tex_h > 0 ? (fontSize / (float)r->font_tex_h) : 1.0f;
	return r3d_font_measure(r, s, n) * k;
}

/* 滚动到光标可见 (对齐 inputbox_scroll_to_cursor) */
static void r3d_input_scroll_to_cursor(jr3d_p r, jr3d_input_p inp)
{
	float visW = inp->w - 8.0f;
	float cw = r3d_input_text_w(r, inp, inp->text, inp->cursor_pos);
	if (cw - inp->scroll > visW)          inp->scroll = (int)(cw - visW);
	else if (cw - inp->scroll < 0)        inp->scroll = (int)cw;
	float total = r3d_input_text_w(r, inp, inp->text, (int)SDL_strlen(inp->text));
	if (total < visW)                     inp->scroll = 0;
	int rightmost = inp->scroll + (int)visW;
	if (rightmost > (int)total && (int)total > (int)visW)
		inp->scroll = (int)total - (int)visW;
	if (inp->scroll < 0) inp->scroll = 0;
}

/* 在 cursor_pos 处插入 (对齐 inputbox_insert, 自动删除选中) */
static void r3d_input_insert_at(jr3d_p r, jr3d_input_p inp, const char* s, int len)
{
	if (!inp || !s || len <= 0) return;
	if (r3d_input_has_sel(inp)) r3d_input_delete_sel(inp);
	int cur = (int)SDL_strlen(inp->text);
	int cap = (int)sizeof(inp->text) - 1;
	if (cur + len > cap) len = cap - cur;
	if (len <= 0) return;
	SDL_memmove(inp->text + inp->cursor_pos + len,
				inp->text + inp->cursor_pos, cur - inp->cursor_pos + 1);
	SDL_memcpy(inp->text + inp->cursor_pos, s, len);
	inp->cursor_pos += len;
	inp->text[cur + len] = '\0';
	r3d_input_clear_sel(inp);
	r3d_input_scroll_to_cursor(r, inp);
}

static void r3d_input_move_cursor(jr3d_input_p inp, int pos)
{
	inp->cursor_pos = pos;
	r3d_input_clear_sel(inp);
}

static void r3d_input_update_sel(jr3d_input_p inp)
{
	if (inp->sel_start < 0) inp->sel_start = inp->cursor_pos;
	inp->sel_end = inp->cursor_pos;
}

void r3d_input_select_left(jr3d_input_p inp)
{
	if (!inp || inp->cursor_pos <= 0) return;
	inp->cursor_pos = r3d_input_prev_char(inp->text, inp->cursor_pos);
	r3d_input_update_sel(inp);
}

void r3d_input_select_right(jr3d_input_p inp)
{
	int cur = (int)SDL_strlen(inp->text);
	if (!inp || inp->cursor_pos >= cur) return;
	inp->cursor_pos = r3d_input_next_char(inp->text, inp->cursor_pos, cur);
	r3d_input_update_sel(inp);
}

void r3d_input_select_home(jr3d_input_p inp)
{
	if (!inp) return;
	r3d_input_update_sel(inp);
	inp->cursor_pos = 0;
	inp->sel_end = 0;
}

void r3d_input_select_end(jr3d_input_p inp)
{
	if (!inp) return;
	r3d_input_update_sel(inp);
	inp->cursor_pos = (int)SDL_strlen(inp->text);
	inp->sel_end = inp->cursor_pos;
}

void r3d_input_select_all(jr3d_input_p inp)
{
	if (!inp) return;
	inp->sel_start = 0;
	inp->sel_end = (int)SDL_strlen(inp->text);
	inp->cursor_pos = inp->sel_end;
}

void r3d_input_paste(jr3d_input_p inp)
{
	if (!inp) return;
	char* clip = SDL_GetClipboardText();
	if (clip) { r3d_input_insert_at(NULL, inp, clip, (int)SDL_strlen(clip)); SDL_free(clip); }
}

void r3d_input_copy(jr3d_input_p inp)
{
	if (!inp || (int)SDL_strlen(inp->text) <= 0) return;
	if (r3d_input_has_sel(inp)) {
		int a = inp->sel_start, b = inp->sel_end;
		if (a > b) { int t = a; a = b; b = t; }
		int len = b - a;
		char* buf = (char*)SDL_malloc(len + 1);
		SDL_memcpy(buf, inp->text + a, len);
		buf[len] = '\0';
		SDL_SetClipboardText(buf);
		SDL_free(buf);
	} else {
		SDL_SetClipboardText(inp->text);
	}
}

void r3d_input_cut(jr3d_input_p inp)
{
	if (!inp || (int)SDL_strlen(inp->text) <= 0) return;
	if (r3d_input_has_sel(inp)) {
		int a = inp->sel_start, b = inp->sel_end;
		if (a > b) { int t = a; a = b; b = t; }
		int len = b - a;
		char* buf = (char*)SDL_malloc(len + 1);
		SDL_memcpy(buf, inp->text + a, len);
		buf[len] = '\0';
		SDL_SetClipboardText(buf);
		SDL_free(buf);
		r3d_input_delete_sel(inp);
	} else {
		SDL_SetClipboardText(inp->text);
		inp->text[0] = '\0';
		inp->cursor_pos = 0;
		inp->scroll = 0;
	}
}

/* ===================== 统一事件处理 (对齐 jr2d inputbox_process_event) ===================== */
bool r3d_input_handle_event(jr3d_p r, jr3d_input_p inp, SDL_Event* event)
{
	if (!r || !inp || !event) return false;

	if (event->type == SDL_EVENT_MOUSE_MOTION) {
		/* 仅更新 hover 状态(选中拖拽扩展可选, 此处保持简单) */
		r3d_input_is_hovered(inp, event->motion.x, event->motion.y);
		return false;
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (event->button.button != SDL_BUTTON_LEFT)
			return false;
		if (r3d_input_is_hovered(inp, event->button.x, event->button.y)) {
			/* 聚焦 + 点击定位光标 + 启动文本输入 */
			bool was_focused = inp->focused;
			inp->focused = true;
			inp->cursor_visible = true;
			inp->cursor_timer = SDL_GetTicks();
			r3d_input_click(r, inp, event->button.x, event->button.y);
			if (!was_focused && r->win) SDL_StartTextInput(r->win);
			return true;   /* 消费: 调用方不应再旋转相机 */
		} else {
			if (inp->focused) {
				inp->focused = false;
				r3d_input_clear_sel(inp);
				if (r->win) SDL_StopTextInput(r->win);
			}
			return false;  /* 点框外: 不消费, 调用方可继续(如相机拖拽) */
		}
	}
	else if (event->type == SDL_EVENT_TEXT_INPUT) {
		if (inp->focused) {
			r3d_input_insert_at(r, inp, event->text.text, (int)SDL_strlen(event->text.text));
		}
		return inp->focused;
	}
	else if (event->type == SDL_EVENT_KEY_DOWN) {
		if (!inp->focused) return false;
		SDL_Keymod mod = event->key.mod;
		switch (event->key.scancode) {
		case SDL_SCANCODE_BACKSPACE: r3d_input_backspace(inp); break;
		case SDL_SCANCODE_DELETE:    r3d_input_delete(inp); break;
		case SDL_SCANCODE_LEFT:
			if (mod & SDL_KMOD_SHIFT) r3d_input_select_left(inp);
			else r3d_input_cursor_left(inp);
			break;
		case SDL_SCANCODE_RIGHT:
			if (mod & SDL_KMOD_SHIFT) r3d_input_select_right(inp);
			else r3d_input_cursor_right(inp);
			break;
		case SDL_SCANCODE_HOME:
			if (mod & SDL_KMOD_SHIFT) r3d_input_select_home(inp);
			else r3d_input_cursor_home(inp);
			break;
		case SDL_SCANCODE_END:
			if (mod & SDL_KMOD_SHIFT) r3d_input_select_end(inp);
			else r3d_input_cursor_end(inp);
			break;
		case SDL_SCANCODE_A:
			if (mod & SDL_KMOD_CTRL) r3d_input_select_all(inp);
			break;
		case SDL_SCANCODE_V:
			if (mod & SDL_KMOD_CTRL) r3d_input_paste(inp);
			break;
		case SDL_SCANCODE_C:
			if (mod & SDL_KMOD_CTRL) r3d_input_copy(inp);
			break;
		case SDL_SCANCODE_X:
			if (mod & SDL_KMOD_CTRL) r3d_input_cut(inp);
			break;
		case SDL_SCANCODE_RETURN:
		case SDL_SCANCODE_KP_ENTER:
			r3d_input_submit(inp);
			break;
		default: break;
		}
		r3d_input_scroll_to_cursor(r, inp);
		return true;   /* 消费键盘事件, 不传给相机 */
	}
	return false;
}

void r3d_input_draw(jr3d_p r, render_mesh_p m, jr3d_input_p inp, float winW, float winH)
{
	if (!r || !m || !inp || winW <= 0 || winH <= 0) return;

	/* 背景: 深色(字体纹理为白字, 深色底才看得清); 聚焦时略亮偏蓝灰 */
	Uint8 br = 40, bg = 40, bb = 40;
	if (inp->focused) { br = 50; bg = 54; bb = 64; }
	draw_rect_2d(r, m, inp->x, inp->y, inp->w, inp->h, br, bg, bb, winW, winH);

	/* 边框: 2px; 聚焦时蓝, 否则灰 */
	float bw = 2.0f;
	Uint8 bor_r = inp->focused ? 70 : 120;
	Uint8 bor_g = inp->focused ? 130 : 120;
	Uint8 bor_b = inp->focused ? 230 : 120;
	draw_rect_2d(r, m, inp->x,            inp->y,                inp->w, bw,     bor_r, bor_g, bor_b, winW, winH);
	draw_rect_2d(r, m, inp->x,            inp->y + inp->h - bw,  inp->w, bw,     bor_r, bor_g, bor_b, winW, winH);
	draw_rect_2d(r, m, inp->x,            inp->y,                bw,    inp->h,  bor_r, bor_g, bor_b, winW, winH);
	draw_rect_2d(r, m, inp->x + inp->w - bw, inp->y,            bw,    inp->h,  bor_r, bor_g, bor_b, winW, winH);

	if (!r->font_ttf_buf) return;

	/* 裁剪内容到框内(对齐 jr2d inputbox 的 SDL_SetRenderClipRect),
	   避免长文本滚动时右端字符溢出到框外。SDL_GPU scissor 用物理像素,
	   需按 ww/winW 比例缩放(input 坐标 x/y/w/h 与 winW 同逻辑像素系)。 */
	float clipScaleX = (winW > 0) ? (float)r->ww / winW : 1.0f;
	float clipScaleY = (winH > 0) ? (float)r->wh / winH : 1.0f;
	if (r->rp) {
		SDL_Rect scissor;
		scissor.x = (int)((inp->x + bw) * clipScaleX);
		scissor.y = (int)( inp->y        * clipScaleY);
		scissor.w = (int)((inp->w - 2.0f * bw) * clipScaleX);
		scissor.h = (int)( inp->h        * clipScaleY);
		if (scissor.w < 0) scissor.w = 0;
		if (scissor.h < 0) scissor.h = 0;
		SDL_SetGPUScissor(r->rp, &scissor);
	}

	float padX   = 4.0f;

	/* 选中高亮(绘制在文字下方, 对齐 inputbox 蓝底选中) */
	if (r3d_input_has_sel(inp)) {
		int a = inp->sel_start, b = inp->sel_end;
		if (a > b) { int t = a; a = b; b = t; }
		float xa = inp->x + padX + (r3d_input_text_w(r, inp, inp->text, a) - (float)inp->scroll);
		float xb = inp->x + padX + (r3d_input_text_w(r, inp, inp->text, b) - (float)inp->scroll);
		float sx = SDL_max(xa, inp->x + bw);
		float ex = SDL_min(xb, inp->x + inp->w - bw);
		if (ex > sx) {
			draw_rect_2d(r, m, sx, inp->y + 2.0f, ex - sx, inp->h - 4.0f,
							100, 150, 255, winW, winH);
		}
	}
	float fontSize = inp->fontSize > 0 ? inp->fontSize : (inp->h * 0.6f);
	float ty     = inp->y + (inp->h - fontSize) * 0.5f;
	float visW   = inp->w - 2.0f * bw - 2.0f * padX;   /* 可见文本宽度 */
	int   len    = (int)SDL_strlen(inp->text);

	/* 换算系数: measure(bake 像素宽) -> 屏幕像素宽, 与 r3d_font_draw_2d 的整图缩放一致。
	   draw_2d 中 1 个 bake 像素 = fontSize / font_tex_h 个屏幕像素。 */
	float k = r->font_tex_h > 0 ? (fontSize / (float)r->font_tex_h) : 1.0f;
	#define R3D_MEASURE(r, s, n) (r3d_font_measure((r), (s), (n)) * (k))

	/* 维护滚动: 保证光标可见(参考 jr2d inputbox_scroll_to_cursor) */
	float total = R3D_MEASURE(r, inp->text, len);
	if (inp->focused) {
		float cw = R3D_MEASURE(r, inp->text, inp->cursor_pos);
		if (cw - inp->scroll > visW)       inp->scroll = (int)(cw - visW);
		else if (cw - inp->scroll < 0)     inp->scroll = (int)cw;
		if (total < visW)                  inp->scroll = 0;
		else if (inp->scroll + visW > total) inp->scroll = (int)(total - visW);
		if (inp->scroll < 0)               inp->scroll = 0;
	}

	/* 计算可见字符起点: 第一个使 (measure(pos) - scroll) >= 0 的字节 */
	int vis_start = 0;
	{
		int p = 0;
		while (p < len) {
			float w = R3D_MEASURE(r, inp->text, p);
			if (w - inp->scroll >= 0.0f) { vis_start = p; break; }
			int nxt = r3d_input_next_char(inp->text, p, len);
			if (nxt <= p) { vis_start = len; break; }
			p = nxt;
		}
	}
	/* 可见起点之前的累计宽度(用于绘制偏移) */
	float pre_w = R3D_MEASURE(r, inp->text, vis_start);

	/* 绘制可见文本子串(从 vis_start 开始)。
	   整串起点 = x+padX-scroll, 子串比整串右移前 vis_start 字符宽度 pre_w,
	   故子串起点 = x+padX-scroll+pre_w (注意: 先减 scroll 再加 pre_w, 不能写成 -(pre_w-scroll)) */
	float draw_x = inp->x + padX - (float)inp->scroll + pre_w;
	if (vis_start < len) {
		r3d_font_draw_2d(r, m, inp->text + vis_start, draw_x, ty, fontSize, winW, winH);
	}

	/* 光标: 聚焦时闪烁, 限制绘制在框内 */
	if (inp->focused) {
		uint64_t now = SDL_GetTicks();
		if (now - inp->cursor_timer >= 500) {
			inp->cursor_visible = !inp->cursor_visible;
			inp->cursor_timer = now;
		}
		if (inp->cursor_visible) {
			float cw = R3D_MEASURE(r, inp->text, inp->cursor_pos);
			float caret_x = inp->x + padX + (cw - (float)inp->scroll);
			/* 仅在框内绘制光标 */
			if (caret_x >= inp->x + bw && caret_x <= inp->x + inp->w - bw) {
				draw_rect_2d(r, m, caret_x, ty, 2.0f, fontSize, 255, 255, 255, winW, winH);
			}
		}
	} else {
		inp->cursor_visible = true;   /* 失焦时复位, 下次聚焦立即显示 */
	}

	/* 恢复全屏裁剪(避免影响后续 2D 绘制) */
	if (r->rp) {
		SDL_Rect full = { 0, 0, r->ww, r->wh };
		SDL_SetGPUScissor(r->rp, &full);
	}
	#undef R3D_MEASURE
}

void r3d_input_destroy(jr3d_input_p inp)
{
	if (inp) SDL_free(inp);
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
	draw_mesh_internal_proj(r, m, model, &r->viewProj, tex);
}

/* 内部: 用自定义投影矩阵(而非相机 viewProj)绘制, 用于 2D/UI 正交绘制 */
static void draw_mesh_internal_proj(jr3d_p r, render_mesh_p m, mat44_t* model, mat44_t* proj, SDL_GPUTexture* tex)
{
	render_mvp_t mvp;
	SDL_GPUTextureSamplerBinding samp;
	SDL_GPUBufferBinding bind;
	if (!r->rp || !m || !m->buffer) return;

	memset(&mvp, 0, sizeof(mvp));
	r3d_mat44_to_float16(proj, mvp.viewProj);
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
