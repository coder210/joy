#include "liv-mono.h"
//#include "external/stb_image.h"
//#include "external/stb_truetype.h"
#include "external/dr_mp3.h"
#include "external/dr_flac.h"
#include "external/dr_wav.h"
//#include "external/stb_vorbis.c"
#include "SDL3/SDL.h"
#include "csclib.h"


static int
set_window_icon(SDL_Window* window, const char* bmp)
{
	SDL_Surface* icon;
	Uint32 key;
	icon = SDL_LoadBMP(bmp);
	if (icon) {
		key = SDL_MapSurfaceRGB(icon, 255, 255, 255);
		SDL_SetSurfaceColorKey(icon, true, key);
		SDL_SetWindowIcon(window, icon);
		SDL_DestroySurface(icon);
		return 0;
	}
	else {
		return 1;
	}
}


//static stbtt_fontinfo* open_font(const char* filename)
//{
//	SDL_IOStream* font_file;
//	Sint64 data_size;
//	unsigned char* font_buffer;
//	stbtt_fontinfo* font_info;
//	int font_offset;
//
//	font_info = (stbtt_fontinfo*)SDL_malloc(sizeof(stbtt_fontinfo));
//	font_file = SDL_IOFromFile(filename, "rb");
//	SDL_SeekIO(font_file, 0, SDL_IO_SEEK_END);
//	data_size = SDL_TellIO(font_file);
//	SDL_SeekIO(font_file, 0, SDL_IO_SEEK_SET);
//	font_buffer = (unsigned char*)SDL_malloc(data_size);
//	SDL_ReadIO(font_file, font_buffer, data_size);
//	SDL_CloseIO(font_file);
//
//	font_offset = stbtt_GetFontOffsetForIndex(font_buffer, 0);
//	if (stbtt_InitFont(font_info, font_buffer, font_offset)) {
//		return font_info;
//	}
//	else {
//		return NULL;
//	}
//}
//
//static void close_font(stbtt_fontinfo* font_info)
//{
//	SDL_free(font_info->data);
//	SDL_free(font_info);
//}
//
//
//
//static int draw_text(SDL_Renderer* renderer, stbtt_fontinfo* font_info, int font_size, const char* text, SDL_Point pos)
//{
//	float scale;
//	int ascent, descent, lineGap;
//	int b_w, b_h, l_h;
//	size_t text_len;
//	unsigned char* bitmap;
//	SDL_Texture* texture;
//	unsigned char* pixels;
//	int pitch;
//	int ax, lsb, c_x1, c_y1, c_x2, c_y2;
//	int x, y, kern, word, next_word, offset;
//	SDL_FRect dest_rect;
//	SDL_Color color;
//
//	text_len = strlen(text);
//	SDL_GetRenderDrawColor(renderer, &color.r, &color.g, &color.b, &color.a);
//
//	b_w = font_size * text_len * 2;
//	b_h = font_size;
//	l_h = font_size;
//
//	bitmap = calloc(b_w * b_h, sizeof(unsigned char));
//	scale = stbtt_ScaleForPixelHeight(font_info, l_h * 1.0f);
//
//	stbtt_GetFontVMetrics(font_info, &ascent, &descent, &lineGap);
//
//	ascent = (int)roundf(ascent * scale);
//	descent = (int)roundf(descent * scale);
//
//	x = y = 0;
//	for (int i = 0; i < text_len; ++i) {
//		word = text[i];
//		next_word = text[i + 1];
//
//		stbtt_GetCodepointHMetrics(font_info, word, &ax, &lsb);
//		stbtt_GetCodepointBitmapBox(font_info, word, scale, scale,
//			&c_x1, &c_y1, &c_x2, &c_y2);
//
//		y = ascent + c_y1;
//
//		offset = x + (int)roundf(lsb * scale) + (y * b_w);
//		stbtt_MakeCodepointBitmap(font_info, bitmap + offset,
//			c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, word);
//
//		x += (int)roundf(ax * scale);
//		kern = stbtt_GetCodepointKernAdvance(font_info, word, next_word);
//		x += (int)roundf(kern * scale);
//	}
//
//	if (bitmap) {
//		texture = SDL_CreateTexture(
//			renderer,
//			SDL_PIXELFORMAT_RGBA32,
//			SDL_TEXTUREACCESS_STREAMING,
//			b_w, b_h
//		);
//
//		SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
//		for (int i = 0; i < b_w * b_h; i++) {
//			pixels[i * 4 + 0] = color.r;
//			pixels[i * 4 + 1] = color.g;
//			pixels[i * 4 + 2] = color.b;
//			pixels[i * 4 + 3] = bitmap[i]; // A（透明度）
//		}
//		SDL_UnlockTexture(texture);
//
//		dest_rect.x = pos.x * 1.0f;
//		dest_rect.y = (pos.y + ascent) * 1.0f;
//		dest_rect.w = b_w * 1.0f;
//		dest_rect.h = b_h * 1.0f;
//
//		SDL_RenderTexture(renderer, texture, NULL, &dest_rect);
//		SDL_DestroyTexture(texture);
//		stbtt_FreeBitmap(bitmap, NULL);
//	}
//
//	return 0;
//}


static SDL_AudioDeviceID open_default_audio_device()
{
	SDL_AudioDeviceID audio_device;
	audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
	return audio_device;
}


static int put_audio_stream_data(SDL_AudioStream* stream, Uint8* data, Uint32 data_len)
{
	if (SDL_GetAudioStreamQueued(stream) < data_len) {
		SDL_PutAudioStreamData(stream, data, data_len);
		return 0;
	}
	return 1;
}


static SDL_Texture*
create_texture_from_surface(SDL_Renderer* renderer, const char* file)
{
	SDL_Surface* surface;
	SDL_Texture* texture;
	surface = SDL_LoadBMP(file);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);
	return texture;
}

//static SDL_Texture*
//create_texturex(SDL_Renderer* renderer, const char* filename)
//{
//	char* file_data;
//	size_t filename_len, data_size;
//	SDL_Surface* surface;
//	SDL_Texture* texture;
//	int w, h, channels;
//	unsigned char* pixels;
//
//	filename_len = strlen(filename);
//	file_data = (char*)SDL_LoadFile(filename, &data_size);
//	if (file_data) {
//		pixels = stbi_load_from_memory(file_data, data_size,
//			&w, &h, &channels, STBI_rgb_alpha);
//		surface = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, pixels, w * 4);
//		texture = SDL_CreateTextureFromSurface(renderer, surface);
//		if (texture) {
//			/*   lua_pushlightuserdata(L, texture);
//			   lua_pushinteger(L, surface->w);
//			   lua_pushinteger(L, surface->h);*/
//		}
//		else {
//			/*lua_pushnil(L);
//			lua_pushnil(L);
//			lua_pushnil(L);*/
//		}
//		SDL_DestroySurface(surface);
//		STBI_FREE(pixels);
//		SDL_free(file_data);
//	}
//	else {
//		texture = NULL;
//	}
//	return texture;
//}

static SDL_GPUShader*
create_vertex_shader(SDL_GPUDevice* device, const char* filename, const char* entrypoint)
{
	SDL_GPUShader* shader;
	Uint8* file_data;
	Uint64 data_size;
	SDL_GPUShaderCreateInfo ci;
	file_data = (Uint8*)SDL_LoadFile(filename, &data_size);
	if (file_data) {
		ci.code = file_data;
		ci.code_size = data_size;
		ci.entrypoint = entrypoint;
		ci.format = SDL_GPU_SHADERFORMAT_SPIRV;
		ci.num_samplers = 0;
		ci.num_storage_buffers = 0;
		ci.num_storage_textures = 0;
		ci.num_uniform_buffers = 0;
		ci.stage = SDL_GPU_SHADERSTAGE_VERTEX;
		shader = SDL_CreateGPUShader(device, &ci);
	}
	else {
		shader = NULL;
	}

	return shader;

}

static SDL_GPUShader*
create_fragment_shader(SDL_GPUDevice* device, const char* filename, const char* entrypoint)
{
	SDL_GPUShader* shader;
	Uint8* file_data;
	Uint64 data_size;
	SDL_GPUShaderCreateInfo ci;
	file_data = (Uint8*)SDL_LoadFile(filename, &data_size);
	if (file_data) {
		ci.code = file_data;
		ci.code_size = data_size;
		ci.entrypoint = entrypoint;
		ci.format = SDL_GPU_SHADERFORMAT_SPIRV;
		ci.num_samplers = 1;
		ci.num_storage_buffers = 0;
		ci.num_storage_textures = 0;
		ci.num_uniform_buffers = 0;
		ci.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
		shader = SDL_CreateGPUShader(device, &ci);
	}
	else {
		shader = NULL;
	}

	return shader;
}



static SDL_GPUGraphicsPipeline*
create_graphics_pipeline(SDL_GPUDevice* device, SDL_Window* window, SDL_GPUShader* vertex, SDL_GPUShader* fragment)
{
	SDL_GPUGraphicsPipelineCreateInfo ci;
	SDL_GPUVertexAttribute attributes[3];
	SDL_GPUVertexBufferDescription vertex_buffer_desc;
	SDL_GPUColorTargetDescription desc;
	SDL_GPUGraphicsPipeline* pipeline;

	memset(&ci, 0, sizeof(SDL_GPUGraphicsPipelineCreateInfo));


	/*position attribute*/
	attributes[0].location = 0;
	attributes[0].buffer_slot = 0;
	attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
	attributes[0].offset = 0;

	/*color attribute*/
	attributes[1].location = 1;
	attributes[1].buffer_slot = 0;
	attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
	attributes[1].offset = sizeof(float) * 2;

	/*uv attribute*/
	attributes[2].location = 2;
	attributes[2].buffer_slot = 0;
	attributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
	attributes[2].offset = sizeof(float) * 5;

	ci.vertex_input_state.vertex_attributes = attributes;
	ci.vertex_input_state.num_vertex_attributes = 3;

	vertex_buffer_desc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	vertex_buffer_desc.instance_step_rate = 0;
	vertex_buffer_desc.slot = 0;
	vertex_buffer_desc.pitch = sizeof(float) * 7;

	ci.vertex_input_state.num_vertex_buffers = 1;
	ci.vertex_input_state.vertex_buffer_descriptions = &vertex_buffer_desc;

	ci.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
	ci.vertex_shader = vertex;
	ci.fragment_shader = fragment;

	ci.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
	ci.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;

	ci.multisample_state.enable_mask = false;
	ci.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;

	ci.target_info.num_color_targets = 1;
	ci.target_info.has_depth_stencil_target = false;

	desc.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
	desc.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
	desc.blend_state.color_write_mask =
		SDL_GPU_COLORCOMPONENT_A | SDL_GPU_COLORCOMPONENT_R |
		SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B;
	desc.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
	desc.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
	desc.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
	desc.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
	desc.blend_state.enable_blend = true;
	desc.blend_state.enable_color_write_mask = false;
	desc.format = SDL_GetGPUSwapchainTextureFormat(device, window);

	ci.target_info.color_target_descriptions = &desc;

	pipeline = SDL_CreateGPUGraphicsPipeline(device, &ci);

	return pipeline;
}


typedef struct vertex {
	float x, y;
	float r, g, b;
	float u, v;
}vertex_p;


static SDL_GPUBuffer* create_and_upload_vertex_data(SDL_GPUDevice* device, vertex_p* vertices, int vertices_len)
{
	SDL_GPUBuffer* vertex_buffer;
	SDL_GPUTransferBufferCreateInfo transfer_buffer_ci;
	SDL_GPUTransferBuffer* transfer_buffer;
	void* ptr;
	SDL_GPUBufferCreateInfo gpu_buffer_ci;
	SDL_GPUCommandBuffer* cmd;
	SDL_GPUCopyPass* copy_pass;
	SDL_GPUTransferBufferLocation location;
	SDL_GPUBufferRegion region;

	transfer_buffer_ci.size = sizeof(vertices);
	transfer_buffer_ci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

	transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_buffer_ci);
	ptr = SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
	memcpy(ptr, &vertices, sizeof(vertices));
	SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

	gpu_buffer_ci.size = sizeof(vertices);
	gpu_buffer_ci.usage = SDL_GPU_BUFFERUSAGE_VERTEX;

	vertex_buffer = SDL_CreateGPUBuffer(device, &gpu_buffer_ci);

	cmd = SDL_AcquireGPUCommandBuffer(device);
	copy_pass = SDL_BeginGPUCopyPass(cmd);
	location.offset = 0;
	location.transfer_buffer = transfer_buffer;
	region.buffer = vertex_buffer;
	region.offset = 0;
	region.size = sizeof(vertices);
	SDL_UploadToGPUBuffer(copy_pass, &location, &region, false);
	SDL_EndGPUCopyPass(copy_pass);
	SDL_SubmitGPUCommandBuffer(cmd);
	SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
	SDL_free(vertices);
	return vertex_buffer;
}


static SDL_GPUBuffer*
create_and_upload_indices_data(SDL_GPUDevice* device, uint32_t* indices, int indices_len)
{
	SDL_GPUTransferBufferCreateInfo transfer_buffer_ci;
	SDL_GPUTransferBuffer* transfer_buffer;
	void* ptr;
	SDL_GPUBufferCreateInfo gpu_buffer_ci;
	SDL_GPUBuffer* indices_buffer;
	SDL_GPUCommandBuffer* cmd;
	SDL_GPUCopyPass* copy_pass;
	SDL_GPUTransferBufferLocation location;
	SDL_GPUBufferRegion region;

	transfer_buffer_ci.size = sizeof(indices);
	transfer_buffer_ci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

	transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_buffer_ci);
	ptr = SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
	memcpy(ptr, &indices, sizeof(indices));
	SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

	gpu_buffer_ci.size = sizeof(indices);
	gpu_buffer_ci.usage = SDL_GPU_BUFFERUSAGE_INDEX;

	indices_buffer = SDL_CreateGPUBuffer(device, &gpu_buffer_ci);

	cmd = SDL_AcquireGPUCommandBuffer(device);
	copy_pass = SDL_BeginGPUCopyPass(cmd);
	location.offset = 0;
	location.transfer_buffer = transfer_buffer;
	region.buffer = indices_buffer;
	region.offset = 0;
	region.size = sizeof(indices);
	SDL_UploadToGPUBuffer(copy_pass, &location, &region, false);
	SDL_EndGPUCopyPass(copy_pass);

	SDL_SubmitGPUCommandBuffer(cmd);

	SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);

	SDL_free(indices);

	return indices_buffer;
}


//static SDL_GPUTexture*
//create_gpu_texture(SDL_GPUDevice* device, const char* filename)
//{
//	int w, h;
//	unsigned char* image_data;
//	SDL_GPUTransferBufferCreateInfo transfer_buffer_ci;
//	SDL_GPUTransferBuffer* transfer_buffer;
//	void* ptr;
//	SDL_GPUTextureCreateInfo texture_ci;
//	SDL_GPUTexture* texture;
//	SDL_GPUCommandBuffer* cmd;
//	SDL_GPUCopyPass* copy_pass;
//	SDL_GPUTextureTransferInfo transfer_info;
//	SDL_GPUTextureRegion region;
//
//	stbi_set_flip_vertically_on_load(true);
//
//	image_data = stbi_load(filename, &w, &h, NULL, STBI_rgb_alpha);
//
//	transfer_buffer_ci.size = 4 * w * h;
//	transfer_buffer_ci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
//
//	transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_buffer_ci);
//	ptr = SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
//	memcpy(ptr, image_data, transfer_buffer_ci.size);
//	SDL_UnmapGPUTransferBuffer(device, transfer_buffer);
//
//	texture_ci.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
//	texture_ci.height = h;
//	texture_ci.width = w;
//	texture_ci.layer_count_or_depth = 1;
//	texture_ci.num_levels = 1;
//	texture_ci.sample_count = SDL_GPU_SAMPLECOUNT_1;
//	texture_ci.type = SDL_GPU_TEXTURETYPE_2D;
//	texture_ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
//
//	texture = SDL_CreateGPUTexture(device, &texture_ci);
//
//	cmd = SDL_AcquireGPUCommandBuffer(device);
//	copy_pass = SDL_BeginGPUCopyPass(cmd);
//
//	transfer_info.offset = 0;
//	transfer_info.pixels_per_row = w;
//	transfer_info.rows_per_layer = h;
//	transfer_info.transfer_buffer = transfer_buffer;
//
//	region.w = w;
//	region.h = h;
//	region.x = 0;
//	region.y = 0;
//	region.layer = 0;
//	region.mip_level = 0;
//	region.z = 0;
//	region.d = 1;
//	region.texture = texture;
//
//	SDL_UploadToGPUTexture(copy_pass, &transfer_info, &region, false);
//	SDL_EndGPUCopyPass(copy_pass);
//	SDL_SubmitGPUCommandBuffer(cmd);
//	SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
//	stbi_image_free(image_data);
//
//	return texture;
//}



static SDL_GPUSampler*
create_gpu_sampler(SDL_GPUDevice* device)
{
	SDL_GPUSampler* sampler;
	SDL_GPUSamplerCreateInfo ci;

	ci.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	ci.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	ci.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	ci.enable_anisotropy = false;
	ci.compare_op = SDL_GPU_COMPAREOP_ALWAYS;
	ci.enable_compare = false;
	ci.mag_filter = SDL_GPU_FILTER_LINEAR;
	ci.min_filter = SDL_GPU_FILTER_LINEAR;
	ci.max_lod = 1.0;
	ci.min_lod = 1.0;
	ci.mip_lod_bias = 0.0;
	ci.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
	sampler = SDL_CreateGPUSampler(device, &ci);

	return sampler;
}


static int
gpu_update(SDL_GPUDevice* device, SDL_Window* window, SDL_GPUGraphicsPipeline* pipeline, SDL_GPUBuffer* vertex_buffer, SDL_GPUBuffer* indices_buffer, SDL_GPUTexture* texture, SDL_GPUSampler* sampler)
{
	SDL_GPUCommandBuffer* cmd;
	SDL_GPUTexture* swapchain_texture;
	Uint32 width, height;
	SDL_GPUColorTargetInfo color_ti;
	SDL_GPURenderPass* render_pass;
	SDL_GPUBufferBinding indices_binding;
	SDL_GPUBufferBinding vertex_binding;
	SDL_GPUTextureSamplerBinding sampler_binding;
	int window_width, window_height;
	SDL_GPUViewport viewport;
	bool is_minimized;


	is_minimized = SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED;
	if (is_minimized) {
		return 0;
	}

	cmd = SDL_AcquireGPUCommandBuffer(device);
	SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window,
		&swapchain_texture, &width, &height);
	if (!swapchain_texture)
		return 0;

	memset(&color_ti, 0, sizeof(color_ti));
	color_ti.clear_color.r = 0.1f;
	color_ti.clear_color.g = 0.1f;
	color_ti.clear_color.b = 0.1f;
	color_ti.clear_color.a = 1.0f;
	color_ti.load_op = SDL_GPU_LOADOP_CLEAR;
	color_ti.mip_level = 0;
	color_ti.store_op = SDL_GPU_STOREOP_STORE;
	color_ti.texture = swapchain_texture;
	color_ti.cycle = true;
	color_ti.layer_or_depth_plane = 0;
	color_ti.cycle_resolve_texture = false;
	render_pass = SDL_BeginGPURenderPass(cmd, &color_ti, 1, NULL);
	SDL_BindGPUGraphicsPipeline(render_pass, pipeline);

	vertex_binding.buffer = vertex_buffer;
	vertex_binding.offset = 0;
	SDL_BindGPUVertexBuffers(render_pass, 0, &vertex_binding, 1);

	indices_binding.buffer = indices_buffer;
	indices_binding.offset = 0;
	SDL_BindGPUIndexBuffer(render_pass, &indices_binding,
		SDL_GPU_INDEXELEMENTSIZE_32BIT);

	sampler_binding.texture = texture;
	sampler_binding.sampler = sampler;
	SDL_BindGPUFragmentSamplers(render_pass, 0, &sampler_binding, 1);

	SDL_GetWindowSize(window, &window_width, &window_height);

	viewport.x = 0;
	viewport.y = 0;
	viewport.w = window_width * 1.0f;
	viewport.h = window_height * 1.0f;
	viewport.min_depth = 0;
	viewport.max_depth = 1;
	SDL_SetGPUViewport(render_pass, &viewport);
	SDL_DrawGPUIndexedPrimitives(render_pass, 6, 1, 0, 0, 0);
	SDL_EndGPURenderPass(render_pass);
	SDL_SubmitGPUCommandBuffer(cmd);

	return 0;
}


static SDL_Environment* create_environment(bool populated)
{
	return SDL_CreateEnvironment(populated);
}

static MonoString*
get_environment_variable(SDL_Environment* env, MonoString* name)
{
	char* cstr_name;
	const char* cstr_value;
	cstr_name = livS_mono_string_to_utf8(name);
	cstr_value = SDL_GetEnvironmentVariable(env, cstr_name);
	return livS_mono_string_new(livS_mono_domain_get(), cstr_value);
}

static bool
set_environment_variable(SDL_Environment* env, MonoString* name, MonoString* value, bool overwrite)
{
	bool result;
	char* cstr_name, * cstr_value;
	cstr_name = livS_mono_string_to_utf8(name);
	cstr_value = livS_mono_string_to_utf8(value);
	result = SDL_SetEnvironmentVariable(env, cstr_name, cstr_value, overwrite);
	return result;
}


static uint32_t get_event_type(SDL_Event* event)
{
	return event->type;
}

static int get_event_scancode(SDL_Event* event)
{
	return event->key.scancode;
}

typedef union Persion
{
	//MonoString *name;
	char name[512];
	int age;
	float score;
}Persion;

static Persion get_person_name()
{
	Persion person = { 0 };
	SDL_strlcpy(person.name, "nczx", 511);
	//person.name = livS_mono_string_new(livS_mono_domain_get(), "nczx");
	return person;
}

static Persion get_person_age()
{
	Persion person = { 0 };
	person.age = 100;
	return person;
}

static Persion get_person_score()
{
	Persion person = { 0 };
	person.score = 72.5f;
	return person;
}

static bool
create_Window_and_renderer(MonoString* title, int width, int height, SDL_WindowFlags window_flags, SDL_Window** window, SDL_Renderer** renderer)
{
	bool ok;
	char* cstr_title;
	cstr_title = livS_mono_string_to_utf8(title);
	ok = SDL_CreateWindowAndRenderer(cstr_title, width, height, window_flags, window, renderer);
	return ok;
}

static int thread_main(void* arg)
{
	MonoObject* delegate;
	MonoObject* result;
	MonoObject* exc;

	//uint32_t gchandle = (uint32_t)(uintptr_t)arg;


	//delegate = livS_mono_gchandle_get_target(gchandle);

	//result = livS_mono_runtime_delegate_invoke(delegate, NULL, &exc);
	//// 6. 检查异常
	//if (exc) {
	//	// 处理异常 - 打印堆栈跟踪等
	//	livS_mono_print_unhandled_exception(exc);
	//}
	//// 7. 释放GC句柄
	//livS_mono_gchandle_free(gchandle);

	delegate = (MonoObject*)arg;
	//MonoDomain *domain = livS_mono_object_get_domain(delegate);
	//livS_mono_runtime_delegate_invoke(delegate, NULL, &exc);
	return 0;
}

static SDL_Thread*
create_thread(MonoObject* delegate, MonoString* name)
{
	//uint32_t gchandle = livS_mono_gchandle_new(delegate, 0); // 0 = 普通引用

	SDL_Thread* thread;
	const char* cstr_name;
	cstr_name = livS_mono_string_to_utf8(name);

	MonoDomain* domain = livS_mono_domain_get();
	//MonoThread* thread = livS_mono_thread_attach(domain);
	//livS_mono_runtime_delegate_invoke(delegate, NULL, NULL);
	thread = SDL_CreateThread(thread_main, cstr_name, delegate);
	//livS_mono_free(name);
	return thread;
}


void csopen_sdl()
{
	livS_mono_add_internal_call("Livlib.Native.SDL::SetAppMetadata", SDL_SetAppMetadata);
	livS_mono_add_internal_call("Livlib.Native.SDL::Init", SDL_Init);
	livS_mono_add_internal_call("Livlib.Native.SDL::Quit", SDL_Quit);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetVersion", SDL_GetVersion);
	livS_mono_add_internal_call("Livlib.Native.SDL::Log", SDL_Log);
	livS_mono_add_internal_call("Livlib.Native.SDL::LogError", SDL_LogError);
	livS_mono_add_internal_call("Livlib.Native.SDL::Delay", SDL_Delay);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetCurrentDisplayMode", SDL_GetCurrentDisplayMode);
	livS_mono_add_internal_call("Livlib.Native.SDL::CreateWindow", SDL_CreateWindow);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetWindowSize", SDL_GetWindowSize);
	livS_mono_add_internal_call("Livlib.Native.SDL::DestroyWindow", SDL_DestroyWindow);
	livS_mono_add_internal_call("Livlib.Native.SDL::CreateWindowAndRenderer", create_Window_and_renderer);
	livS_mono_add_internal_call("Livlib.Native.SDL::SetWindowIcon", set_window_icon);
	livS_mono_add_internal_call("Livlib.Native.SDL::SetWindowFullscreen", SDL_SetWindowFullscreen);
	livS_mono_add_internal_call("Livlib.Native.SDL::CreateRenderer", SDL_CreateRenderer);
	livS_mono_add_internal_call("Livlib.Native.SDL::DestroyRenderer", SDL_DestroyRenderer);
	livS_mono_add_internal_call("Livlib.Native.SDL::SetRenderVSync", SDL_SetRenderVSync);
	livS_mono_add_internal_call("Livlib.Native.SDL::RenderClear", SDL_RenderClear);
	livS_mono_add_internal_call("Livlib.Native.SDL::RenderPresent", SDL_RenderPresent);
	livS_mono_add_internal_call("Livlib.Native.SDL::SetRenderScale", SDL_SetRenderScale);
	livS_mono_add_internal_call("Livlib.Native.SDL::RenderDebugText", SDL_RenderDebugText);
	livS_mono_add_internal_call("Livlib.Native.SDL::SetRenderDrawColor", SDL_SetRenderDrawColor);
	livS_mono_add_internal_call("Livlib.Native.SDL::RenderPoint", SDL_RenderPoint);
	livS_mono_add_internal_call("Livlib.Native.SDL::RenderLine", SDL_RenderLine);
	livS_mono_add_internal_call("Livlib.Native.SDL::RenderRect", SDL_RenderRect);
	livS_mono_add_internal_call("Livlib.Native.SDL::RenderFillRect", SDL_RenderFillRect);
	//livS_mono_add_internal_call("Livlib.Native.SDL::OpenFont", open_font);
	//livS_mono_add_internal_call("Livlib.Native.SDL::CloseFont", close_font);
	livS_mono_add_internal_call("Livlib.Native.SDL::LoadWAV", SDL_LoadWAV);
	livS_mono_add_internal_call("Livlib.Native.SDL::CreateAudioStream", SDL_CreateAudioStream);
	livS_mono_add_internal_call("Livlib.Native.SDL::OpenDefaultAudioDevice", open_default_audio_device);
	livS_mono_add_internal_call("Livlib.Native.SDL::PauseAudioDevice", SDL_PauseAudioDevice);
	livS_mono_add_internal_call("Livlib.Native.SDL::ResumeAudioDevice", SDL_ResumeAudioDevice);
	livS_mono_add_internal_call("Livlib.Native.SDL::BindAudioStream", SDL_BindAudioStream);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetAudioDeviceGain", SDL_GetAudioDeviceGain);
	livS_mono_add_internal_call("Livlib.Native.SDL::SetAudioDeviceGain", SDL_SetAudioDeviceGain);
	livS_mono_add_internal_call("Livlib.Native.SDL::CloseAudioDevice", SDL_CloseAudioDevice);
	livS_mono_add_internal_call("Livlib.Native.SDL::DestroyAudioStream", SDL_DestroyAudioStream);
	livS_mono_add_internal_call("Livlib.Native.SDL::PutAudioStreamData", put_audio_stream_data);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetTicks", SDL_GetTicks);
	livS_mono_add_internal_call("Livlib.Native.SDL::CreateTextureFromSurface", create_texture_from_surface);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetTextureSize", SDL_GetTextureSize);
	livS_mono_add_internal_call("Livlib.Native.SDL::RenderTextureRotated", SDL_RenderTextureRotated);
	livS_mono_add_internal_call("Livlib.Native.SDL::DestroyTexture", SDL_DestroyTexture);
	livS_mono_add_internal_call("Livlib.Native.SDL::LoadFile", SDL_LoadFile);
	livS_mono_add_internal_call("Livlib.Native.SDL::PollEvent", SDL_PollEvent);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetNumGPUDrivers", SDL_GetNumGPUDrivers);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetGPUDriver", SDL_GetGPUDriver);
	livS_mono_add_internal_call("Livlib.Native.SDL::CreateGPUDevice", SDL_CreateGPUDevice);
	livS_mono_add_internal_call("Livlib.Native.SDL::ClaimWindowForGPUDevice", SDL_ClaimWindowForGPUDevice);
	livS_mono_add_internal_call("Livlib.Native.SDL::CreateVertexShader", create_vertex_shader);
	livS_mono_add_internal_call("Livlib.Native.SDL::CreateFragmentShader", create_fragment_shader);
	livS_mono_add_internal_call("Livlib.Native.SDL::CreateGraphicsPipeline", create_graphics_pipeline);
	livS_mono_add_internal_call("Livlib.Native.SDL::CreateAndUploadVertexData", create_and_upload_vertex_data);
	livS_mono_add_internal_call("Livlib.Native.SDL::CreateAndUploadIndicesData", create_and_upload_indices_data);
	//livS_mono_add_internal_call("Livlib.Native.SDL::CreateGPUTexture", create_gpu_texture);
	livS_mono_add_internal_call("Livlib.Native.SDL::CreateGPUSampler", create_gpu_sampler);
	livS_mono_add_internal_call("Livlib.Native.SDL::GPUUpdate", gpu_update);
	livS_mono_add_internal_call("Livlib.Native.SDL::ReleaseGPUSampler", SDL_ReleaseGPUSampler);
	livS_mono_add_internal_call("Livlib.Native.SDL::ReleaseGPUTexture", SDL_ReleaseGPUTexture);
	livS_mono_add_internal_call("Livlib.Native.SDL::ReleaseGPUBuffer", SDL_ReleaseGPUBuffer);
	livS_mono_add_internal_call("Livlib.Native.SDL::ReleaseGPUGraphicsPipeline", SDL_ReleaseGPUGraphicsPipeline);
	livS_mono_add_internal_call("Livlib.Native.SDL::ReleaseGPUShader", SDL_ReleaseGPUShader);
	livS_mono_add_internal_call("Livlib.Native.SDL::ReleaseWindowFromGPUDevice", SDL_ReleaseWindowFromGPUDevice);
	livS_mono_add_internal_call("Livlib.Native.SDL::DestroyGPUDevice", SDL_DestroyGPUDevice);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetMouseState", SDL_GetMouseState);
	livS_mono_add_internal_call("Livlib.Native.SDL::CreateEnvironment", create_environment);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetEnvironmentVariable", get_environment_variable);
	livS_mono_add_internal_call("Livlib.Native.SDL::SetEnvironmentVariable", set_environment_variable);
	livS_mono_add_internal_call("Livlib.Native.SDL::DestroyEnvironment", SDL_DestroyEnvironment);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetEventType", get_event_type);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetEventScancode", get_event_scancode);
	livS_mono_add_internal_call("Livlib.Native.SDL::CreateThread", create_thread);


	livS_mono_add_internal_call("Livlib.Native.SDL::GetPersonName", get_person_name);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetPersonAge", get_person_age);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetPersonScore", get_person_score);

	livS_mono_add_internal_call("Livlib.Native.SDL::SetRenderLogicalPresentation", SDL_SetRenderLogicalPresentation);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetRenderLogicalPresentation", SDL_GetRenderLogicalPresentation);
	livS_mono_add_internal_call("Livlib.Native.SDL::GetRenderLogicalPresentationRect", SDL_GetRenderLogicalPresentationRect);
}
