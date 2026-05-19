#include "jrender.h"
#include <string.h>

/* ============ 着色器加载 ============ */
SDL_GPUShader* render_load_shader(SDL_GPUDevice* device, const char* filename,
	SDL_GPUShaderStage stage, uint32_t sampler_num, uint32_t uniform_buffer_num)
{
	size_t file_size;
	Uint8* data = (Uint8*)SDL_LoadFile(filename, &file_size);
	if (!data) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "render_load_shader: load file %s failed: %s", filename, SDL_GetError());
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
	ci.stage = stage;
	SDL_GPUShader* shader = SDL_CreateGPUShader(device, &ci);
	SDL_free(data);
	if (!shader)
		SDL_LogError(SDL_LOG_CATEGORY_GPU, "render_load_shader: create shader from %s failed: %s", filename, SDL_GetError());
	return shader;
}

/* ============ 图形管线创建 ============ */
SDL_GPUGraphicsPipeline* render_create_pipeline(SDL_GPUDevice* device,
	SDL_GPUShader* vertex_shader, SDL_GPUShader* fragment_shader,
	SDL_GPUTextureFormat swapchain_format, SDL_GPUTextureFormat depth_format)
{
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
	buffer_desc.pitch = sizeof(float) * 5; /* 3 pos + 2 uv */

	ci.vertex_input_state.num_vertex_buffers = 1;
	ci.vertex_input_state.vertex_buffer_descriptions = &buffer_desc;
	ci.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
	ci.vertex_shader = vertex_shader;
	ci.fragment_shader = fragment_shader;
	ci.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
	ci.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
	ci.multisample_state.enable_mask = false;
	ci.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
	ci.target_info.num_color_targets = 1;
	ci.target_info.has_depth_stencil_target = true;
	ci.target_info.depth_stencil_format = depth_format;

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
	desc.blend_state.color_write_mask = SDL_GPU_COLORCOMPONENT_A | SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B;
	desc.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
	desc.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
	desc.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
	desc.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
	desc.blend_state.enable_blend = true;
	desc.blend_state.enable_color_write_mask = false;
	desc.format = swapchain_format;
	ci.target_info.color_target_descriptions = &desc;

	return SDL_CreateGPUGraphicsPipeline(device, &ci);
}

/* ============ 顶点缓冲区创建与上传 ============ */
SDL_GPUBuffer* render_upload_buffer(SDL_GPUDevice* device, const void* data, size_t size)
{
	SDL_GPUTransferBufferCreateInfo tb_ci = { 0 };
	tb_ci.size = size;
	tb_ci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(device, &tb_ci);
	if (!tb) {
		SDL_LogError(SDL_LOG_CATEGORY_GPU, "render_upload_buffer: create transfer buffer failed");
		return NULL;
	}
	void* ptr = SDL_MapGPUTransferBuffer(device, tb, false);
	memcpy(ptr, data, size);
	SDL_UnmapGPUTransferBuffer(device, tb);

	SDL_GPUBufferCreateInfo buf_ci = { 0 };
	buf_ci.size = size;
	buf_ci.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
	SDL_GPUBuffer* buf = SDL_CreateGPUBuffer(device, &buf_ci);
	if (!buf) {
		SDL_LogError(SDL_LOG_CATEGORY_GPU, "render_upload_buffer: create buffer failed");
		SDL_ReleaseGPUTransferBuffer(device, tb);
		return NULL;
	}

	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd);
	SDL_GPUTransferBufferLocation loc = { 0 };
	loc.offset = 0;
	loc.transfer_buffer = tb;
	SDL_GPUBufferRegion region = { 0 };
	region.buffer = buf;
	region.offset = 0;
	region.size = size;
	SDL_UploadToGPUBuffer(copy_pass, &loc, &region, false);
	SDL_EndGPUCopyPass(copy_pass);
	SDL_SubmitGPUCommandBuffer(cmd);
	SDL_ReleaseGPUTransferBuffer(device, tb);
	return buf;
}

/* ============ 上传像素数据到纹理 ============ */
static SDL_GPUTexture* upload_pixels_to_texture(SDL_GPUDevice* device,
	const unsigned char* pixels, int w, int h, SDL_GPUTextureFormat format,
	SDL_GPUTextureUsageFlags usage)
{
	size_t image_size = (size_t)w * h * 4;

	SDL_GPUTransferBufferCreateInfo tb_ci = { 0 };
	tb_ci.size = image_size;
	tb_ci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(device, &tb_ci);
	if (!tb) return NULL;
	void* ptr = SDL_MapGPUTransferBuffer(device, tb, false);
	memcpy(ptr, pixels, image_size);
	SDL_UnmapGPUTransferBuffer(device, tb);

	SDL_GPUTextureCreateInfo tex_ci = { 0 };
	tex_ci.format = format;
	tex_ci.height = h;
	tex_ci.width = w;
	tex_ci.layer_count_or_depth = 1;
	tex_ci.num_levels = 1;
	tex_ci.sample_count = SDL_GPU_SAMPLECOUNT_1;
	tex_ci.type = SDL_GPU_TEXTURETYPE_2D;
	tex_ci.usage = usage;
	SDL_GPUTexture* tex = SDL_CreateGPUTexture(device, &tex_ci);
	if (!tex) { SDL_ReleaseGPUTransferBuffer(device, tb); return NULL; }

	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);
	SDL_GPUTextureTransferInfo ti = { 0 };
	ti.offset = 0;
	ti.pixels_per_row = w;
	ti.rows_per_layer = h;
	ti.transfer_buffer = tb;
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

/* ============ 纯色纹理 ============ */
SDL_GPUTexture* render_create_solid_texture(SDL_GPUDevice* device,
	unsigned char r, unsigned char g, unsigned char b)
{
	const int tw = 16, th = 16;
	size_t image_size = (size_t)tw * th * 4;
	unsigned char* pixels = (unsigned char*)SDL_malloc(image_size);
	if (!pixels) return NULL;
	for (int i = 0; i < tw * th; i++) {
		pixels[i * 4 + 0] = r;
		pixels[i * 4 + 1] = g;
		pixels[i * 4 + 2] = b;
		pixels[i * 4 + 3] = 255;
	}
	SDL_GPUTexture* tex = upload_pixels_to_texture(device, pixels, tw, th,
		SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, SDL_GPU_TEXTUREUSAGE_SAMPLER);
	SDL_free(pixels);
	return tex;
}

/* ============ 深度纹理 ============ */
SDL_GPUTexture* render_create_depth_texture(SDL_GPUDevice* device, int width, int height)
{
	SDL_GPUTextureCreateInfo tex_ci = { 0 };
	tex_ci.format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
	tex_ci.height = height;
	tex_ci.width = width;
	tex_ci.layer_count_or_depth = 1;
	tex_ci.num_levels = 1;
	tex_ci.sample_count = SDL_GPU_SAMPLECOUNT_1;
	tex_ci.type = SDL_GPU_TEXTURETYPE_2D;
	tex_ci.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
	return SDL_CreateGPUTexture(device, &tex_ci);
}

/* ============ 采样器 ============ */
SDL_GPUSampler* render_create_sampler(SDL_GPUDevice* device)
{
	SDL_GPUSamplerCreateInfo ci = { 0 };
	ci.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	ci.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	ci.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	ci.enable_anisotropy = true;
	ci.max_anisotropy = 4.0f;
	ci.compare_op = SDL_GPU_COMPAREOP_ALWAYS;
	ci.enable_compare = false;
	ci.mag_filter = SDL_GPU_FILTER_LINEAR;
	ci.min_filter = SDL_GPU_FILTER_LINEAR;
	ci.max_lod = 10.0f;
	ci.min_lod = 0.0f;
	ci.mip_lod_bias = 0.0f;
	ci.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
	return SDL_CreateGPUSampler(device, &ci);
}

/* ============ 矩阵转换: mat44_t → GPU 列主序 float[16] ============ */
void render_mat44_to_float16(const mat44_t* m, float out[16])
{
	out[0]  = m->m0;  out[1]  = m->m1;  out[2]  = m->m2;  out[3]  = m->m3;
	out[4]  = m->m4;  out[5]  = m->m5;  out[6]  = m->m6;  out[7]  = m->m7;
	out[8]  = m->m8;  out[9]  = m->m9;  out[10] = m->m10; out[11] = m->m11;
	out[12] = m->m12; out[13] = m->m13; out[14] = m->m14; out[15] = m->m15;
}
