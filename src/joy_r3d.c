/******************************************************************************
 * src/joy_r3d.c —— r3d 域唯一实现文件（一个域一个 .c，配 joy_r3d.h）
 * 职责：3D 渲染 / 光照 / glTF
 * 合并自：jr3d.c / jlight.c / jgltf.c
 * 说明：本域公开 API 声明在配套头 src/joy_r3d.h，由 include/joy/japi.h 聚合包含；
 *       本文件是该域唯一的编译单元。
 *       内部符号（static / 私有宏 / typedef）保持各分片独立，未跨分片改名。
 ******************************************************************************/

#include "joy_r3d.h"

/* ==================== 合并自 src/jr3d.c ==================== */
#include "joy.h"
#include <SDL3/SDL.h>

/* stb_truetype 实现已由 jimpl.c 编译一次(含 STB_TRUETYPE_IMPLEMENTATION),
   此处仅包含头文件以使用其声明, 避免多重定义 */
#include "external/stb_truetype.h"

/* ================================================================
 * 内部结构 — 完整 SDL3 GPU 类型
 * ================================================================ */

#define MAX_COLOR_CACHE 256

struct joy_render_mesh {
	joy_r3d_p         owner;  /* 创建它的 r3d 上下文，供单参数 dtor 使用 */
	SDL_GPUBuffer* buffer;
	SDL_GPUBuffer* index_buffer;   /* 可选: 非空表示用索引绘制 */
	int            vertex_count;
	int            index_count;
};

/* 统一分配网格：记录 owner，让 joy_r3d_mesh_destroy_one 能还原双参数销毁上下文。 */
static joy_render_mesh_p r3d_mesh_alloc(joy_r3d_p r)
{
	joy_render_mesh_p m = SDL_calloc(1, sizeof(joy_render_mesh_t));
	if (m) m->owner = r;
	return m;
}

struct joy_r3d {
	SDL_GPUDevice*     dev;
	SDL_Window*        win;
	SDL_GPUShader*     vs;
	SDL_GPUShader*     fs;
	SDL_GPUGraphicsPipeline* pipeline;
	SDL_GPUGraphicsPipeline* line_pipeline;  /* 线框渲染: LINELIST 基元 */
	SDL_GPUGraphicsPipeline* solid_pipeline; /* 实体填充: 纯色 + 深度写入, 用于"填充+描边"画风 */
	SDL_GPUSampler*    sampler;
	SDL_GPUTexture*    depth;
	SDL_GPUCommandBuffer* cmd;
	SDL_GPURenderPass* rp;
	int                ww, wh;
	int                last_ww, last_wh;  /* 上次已处理的渲染尺寸（resize 检测基准） */
	Uint8              clear_r, clear_g, clear_b;  /* 清屏色 */

	/* 2D UI 逻辑分辨率(仿 jr2d logical presentation, 窗口缩放自适应) */
	int                logical_w, logical_h;  /* 逻辑分辨率(0=关闭, 直接用窗口像素) */
	int                logical_mode;          /* 呈现模式: STRETCH / LETTERBOX(见 joy.h 常量) */
	float              ui_sx, ui_sy;          /* UI 缩放系数: 窗口像素/逻辑像素(LETTERBOX 下 x==y) */
	float              ui_offx, ui_offy;      /* UI letterbox 居中偏移(像素, 在 joy_r3d_begin 更新) */

	/* 相机/投影 */
	joy_mat44_t            viewProj;
	float              fovy;
	float              aspect;
	float              near_;
	float              far_;

	/* 颜色纹理缓存 */
	struct { Uint8 r, g, b, a; SDL_GPUTexture* tex; } color_cache[MAX_COLOR_CACHE];
	int                color_count;

	/* 光照渲染（可选, 由 joy_r3d_create 探测 lighting.vert/frag.spv 自动启用） */
	SDL_GPUShader*     lvs;
	SDL_GPUShader*     lfs;
	SDL_GPUGraphicsPipeline* lighting_pipeline;
	joy_r3d_light_block_t light_block;
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
	char               font_cache[256]; /* 已烘焙的字符串缓存(最近一次) */
	Uint8              font_color_r, font_color_g, font_color_b; /* 烘焙文字颜色(默认白) */
	Uint8              font_cache_r, font_cache_g, font_cache_b; /* 已烘焙颜色缓存(最近一次) */

	/* 多槽烘焙缓存: 同一 (字符串,颜色) 只光栅化一次, 之后直接复用纹理。
	   解决 splash/菜单/HUD 每帧交替绘制多句文字、单槽缓存反复失效导致每帧重烘焙+刷日志的问题。
	   采用 LRU 淘汰 + 可配置上限(font_cache_limit): 当不同 (字符串,颜色) 组合超过上限时,
	   优先淘汰最久未使用的条目, 避免固定文字被频繁变化的文本挤掉后每帧重建纹理(缓存颠簸)。 */
	int  font_slots_used;
	int  font_cache_limit;         /* 缓存上限(默认 32), 可经 joy_r3d_font_cache_set_limit 调整 */
	Uint64 font_cache_tick;        /* 每次烘焙操作的单调计数, 用作 LRU 时间戳 */
	Uint64 font_cache_frame;       /* 渲染帧计数(joy_r3d_begin 递增), 供 sweep 判断闲置帧数 */
	struct {
		char str[256]; int w, h; Uint8 r, g, b;
		SDL_GPUTexture* tex;
		Uint64 last_use;           /* 最近一次命中/创建的时刻(bake 计数) */
		Uint64 last_frame;         /* 最近一次命中/创建所在的渲染帧 */
	} font_slots[64];              /* 物理数组上限 64(可配置上限 <= 64) */

	/* 每帧 draw call 计数: joy_r3d_begin 清零, draw_mesh_internal_proj 自增, 供 render3d_pass 输出诊断 */
	int  draw_calls;

	/* 延迟释放队列: joy_r3d_mesh_destroy 不再立即释放 GPU buffer,
	   而是延迟到 joy_r3d_end 之后, 避免释放仍被 GPU 引用的 buffer 导致崩溃 */
	SDL_GPUBuffer**    pending_buffers;
	int                pending_count;
	int                pending_cap;

	/* 惰性创建的共享 GPU 资源: 必须按实例持有。函数级 static 会让第二个 r3d
	   复用绑定在首个 device 上的纹理/mesh, 且销毁设备时无处释放。 */
	joy_render_mesh_p  unit_disc;         /* 单位圆盘 mesh(圆形按钮等复用) */
	SDL_GPUTexture*    fov_gradient_tex;  /* FOV 渐变纹理 */
	joy_render_mesh_p  fov_prev_mesh;     /* FOV 上一帧 mesh, 延迟释放 */
};

/* 前向声明: 延迟释放队列的 buffer, 在 joy_r3d_end / joy_r3d_destroy 后统一释放 */
static void r3d_flush_pending(joy_r3d_p r);

/* ================================================================
 * 底层工具 — 使用 void* 隐藏 SDL3 句柄，内部强转
 * ================================================================ */

void* joy_r3d_load_shader(void* device, const char* filename,
	int stage, int sampler_num, int uniform_buffer_num)
{
	SDL_GPUDevice* dev = (SDL_GPUDevice*)device;
	size_t file_size;
	Uint8* data = (Uint8*)SDL_LoadFile(filename, &file_size);
	if (!data) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "joy_r3d_load_shader: %s failed: %s", filename, SDL_GetError());
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
		SDL_LogError(SDL_LOG_CATEGORY_GPU, "joy_r3d_load_shader: create %s failed: %s", filename, SDL_GetError());
	return (void*)shader;
}

void* joy_r3d_create_pipeline(void* device,
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
	ci.rasterizer_state.enable_depth_clip = true; /* 关闭 FEATURE_DEPTH_CLAMPING 后 SDL_GPU 要求 enable_depth_clip 必须为 true */
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

/* 线框 pipeline: 与非 lit 完全相同的 vertex format 和 shader, 仅 primitive type 改为 LINELIST */
void* joy_r3d_create_line_pipeline(void* device,
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
	ci.primitive_type = SDL_GPU_PRIMITIVETYPE_LINELIST;
	ci.vertex_shader = vs;
	ci.fragment_shader = fs;
	ci.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
	ci.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
	ci.rasterizer_state.enable_depth_clip = true; /* 关闭 FEATURE_DEPTH_CLAMPING 后 SDL_GPU 要求 enable_depth_clip 必须为 true */
	ci.multisample_state.enable_mask = false;
	ci.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
	ci.target_info.num_color_targets = 1;
	ci.target_info.has_depth_stencil_target = true;
	ci.target_info.depth_stencil_format = (SDL_GPUTextureFormat)depth_format;

	SDL_GPUDepthStencilState ds = { 0 };
	ds.compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL;  /* 描边与填充同深度时通过, 保证边线画在填充表面 */
	ds.enable_depth_test = true;
	ds.enable_depth_write = false;
	ds.enable_stencil_test = false;
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

/* 实体填充 pipeline: 与非 lit pipeline 相同的 vertex format/shader,
   仅开启深度写入, 用于"填充+描边"画风的填充 pass（提供遮挡与层次） */
void* joy_r3d_create_solid_pipeline(void* device,
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
	ci.rasterizer_state.enable_depth_clip = true; /* 关闭 FEATURE_DEPTH_CLAMPING 后 SDL_GPU 要求 enable_depth_clip 必须为 true */
	ci.multisample_state.enable_mask = false;
	ci.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
	ci.target_info.num_color_targets = 1;
	ci.target_info.has_depth_stencil_target = true;
	ci.target_info.depth_stencil_format = (SDL_GPUTextureFormat)depth_format;

	SDL_GPUDepthStencilState ds = { 0 };
	ds.compare_op = SDL_GPU_COMPAREOP_LESS;
	ds.enable_depth_test = true;
	ds.enable_depth_write = true;   /* 关键差异: 填充写深度, 供描边 pass 遮挡背面边 */
	ds.enable_stencil_test = false;
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

void* joy_r3d_create_lighting_pipeline(void* device,
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
	ci.rasterizer_state.enable_depth_clip = true; /* 关闭 FEATURE_DEPTH_CLAMPING 后 SDL_GPU 要求 enable_depth_clip 必须为 true */
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

void* joy_r3d_upload_buffer(void* device, const void* data, size_t size)
{
	SDL_GPUDevice* dev = (SDL_GPUDevice*)device;
	SDL_GPUTransferBufferCreateInfo tb_ci = { 0 };
	tb_ci.size = size;
	tb_ci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(dev, &tb_ci);
	if (!tb) { SDL_LogError(SDL_LOG_CATEGORY_GPU, "joy_r3d_upload_buffer: create tb failed"); return NULL; }
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

void* joy_r3d_create_solid_texture(void* device,
	unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	SDL_GPUDevice* dev = (SDL_GPUDevice*)device;
	const int tw = 16, th = 16;
	size_t image_size = (size_t)tw * th * 4;
	unsigned char* pixels = (unsigned char*)SDL_malloc(image_size);
	if (!pixels) return NULL;
	for (int i = 0; i < tw * th; i++) {
		pixels[i * 4 + 0] = r; pixels[i * 4 + 1] = g;
		pixels[i * 4 + 2] = b; pixels[i * 4 + 3] = a;
	}
	SDL_GPUTexture* tex = upload_pixels_to_texture(dev, pixels, tw, th,
		SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, SDL_GPU_TEXTUREUSAGE_SAMPLER, 0);
	SDL_free(pixels);
	return (void*)tex;
}

void* joy_r3d_create_depth_texture(void* device, int width, int height)
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

void* joy_r3d_create_sampler(void* device)
{
	SDL_GPUDevice* dev = (SDL_GPUDevice*)device;
	SDL_GPUSamplerCreateInfo ci = { 0 };
	ci.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	ci.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	ci.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	ci.enable_anisotropy = false; ci.max_anisotropy = 1.0f; /* 关闭 FEATURE_ANISOTROPY 后必须关闭各向异性（SDL_GPU 断言要求） */
	ci.compare_op = SDL_GPU_COMPAREOP_ALWAYS; ci.enable_compare = false;
	ci.mag_filter = SDL_GPU_FILTER_LINEAR; ci.min_filter = SDL_GPU_FILTER_LINEAR;
	ci.max_lod = 10.0f; ci.min_lod = 0.0f; ci.mip_lod_bias = 0.0f;
	ci.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
	return (void*)SDL_CreateGPUSampler(dev, &ci);
}

void joy_r3d_mat44_to_float16(const joy_mat44_t* m, float out[16])
{
	out[0] = m->m0;  out[1] = m->m1;  out[2] = m->m2;  out[3] = m->m3;
	out[4] = m->m4;  out[5] = m->m5;  out[6] = m->m6;  out[7] = m->m7;
	out[8] = m->m8;  out[9] = m->m9;  out[10] = m->m10; out[11] = m->m11;
	out[12] = m->m12; out[13] = m->m13; out[14] = m->m14; out[15] = m->m15;
}

/* ================================================================
 * 高级 API (v3.0) — 可重入
 * ================================================================ */

/* ---- 内部：查/创建颜色纹理（含 alpha，供半透明绘制） ---- */
static SDL_GPUTexture* get_color_texture(joy_r3d_p r, Uint8 cr, Uint8 cg, Uint8 cb, Uint8 ca)
{
	for (int i = 0; i < r->color_count; i++) {
		if (r->color_cache[i].r == cr && r->color_cache[i].g == cg &&
			r->color_cache[i].b == cb && r->color_cache[i].a == ca)
			return r->color_cache[i].tex;
	}
	if (r->color_count >= MAX_COLOR_CACHE) {
		/* LRU: 淘汰最旧槽(0), 前移, 新颜色放最后, 避免缓存满后返回 NULL 导致颜色错乱 */
		if (r->color_cache[0].tex) SDL_ReleaseGPUTexture(r->dev, r->color_cache[0].tex);
		for (int i = 0; i < MAX_COLOR_CACHE - 1; i++) r->color_cache[i] = r->color_cache[i + 1];
		r->color_count = MAX_COLOR_CACHE - 1;
	}
	SDL_GPUTexture* tex = (SDL_GPUTexture*)joy_r3d_create_solid_texture(r->dev, cr, cg, cb, ca);
	if (tex) {
		r->color_cache[r->color_count].r = cr;
		r->color_cache[r->color_count].g = cg;
		r->color_cache[r->color_count].b = cb;
		r->color_cache[r->color_count].a = ca;
		r->color_cache[r->color_count].tex = tex;
		r->color_count++;
	}
	return tex;
}

/* ---- 生命周期 ---- */

void* joy_r3d_get_device(joy_r3d_p r)
{
	if (!r) return NULL;
	return (void*)r->dev;
}

joy_r3d_p joy_r3d_create(joy_window_p win, const char* shader_dir)
{
	SDL_Window* window = (SDL_Window*)joy_core_get_raw_window(win);
	joy_r3d_p r = (joy_r3d_p)SDL_calloc(1, sizeof(joy_r3d_t));
	if (!r) return NULL;

	r->win = window;
	/* 创建 GPU 设备：用 properties 放宽可选特性要求，提升 Android / Mali 等移动 GPU 兼容性。
	   SDL_GPU 的 Vulkan 后端默认要求 shaderClipDistance、depthClamp、drawIndirectFirstInstance、
	   sampleRateShading 等特性，许多移动 GPU 不满足，PrepareDriver 会失败并报
	   "No supported SDL_GPU backend found!"。按 SDL 官方指引关闭这四项可选特性
	   （CLIP_DISTANCE / DEPTH_CLAMPING / INDIRECT_DRAW_FIRST_INSTANCE / ANISOTROPY），
	   即可在更多安卓设备上创建成功。 */
	SDL_PropertiesID dev_props = SDL_CreateProperties();
	if (dev_props) {
		SDL_SetBooleanProperty(dev_props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_SPIRV_BOOLEAN, true);
		SDL_SetBooleanProperty(dev_props, SDL_PROP_GPU_DEVICE_CREATE_DEBUGMODE_BOOLEAN, true);
		SDL_SetBooleanProperty(dev_props, SDL_PROP_GPU_DEVICE_CREATE_VERBOSE_BOOLEAN, true);
		SDL_SetBooleanProperty(dev_props, SDL_PROP_GPU_DEVICE_CREATE_FEATURE_CLIP_DISTANCE_BOOLEAN, false);
		SDL_SetBooleanProperty(dev_props, SDL_PROP_GPU_DEVICE_CREATE_FEATURE_DEPTH_CLAMPING_BOOLEAN, false);
		SDL_SetBooleanProperty(dev_props, SDL_PROP_GPU_DEVICE_CREATE_FEATURE_INDIRECT_DRAW_FIRST_INSTANCE_BOOLEAN, false);
		SDL_SetBooleanProperty(dev_props, SDL_PROP_GPU_DEVICE_CREATE_FEATURE_ANISOTROPY_BOOLEAN, false);
		r->dev = SDL_CreateGPUDeviceWithProperties(dev_props);
		SDL_DestroyProperties(dev_props);
	} else {
		r->dev = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
	}
	if (!r->dev) { SDL_LogError(SDL_LOG_CATEGORY_GPU, "joy_r3d_create: CreateGPUDevice failed: %s", SDL_GetError()); SDL_free(r); return NULL; }
	if (!SDL_ClaimWindowForGPUDevice(r->dev, r->win)) {
		SDL_LogError(SDL_LOG_CATEGORY_GPU, "joy_r3d_create: ClaimWindow failed");
		SDL_DestroyGPUDevice(r->dev); SDL_free(r); return NULL;
	}

	/* 默认清屏色 = 深蓝黑 {13,15,26}, 与 UI 场景背景一致; 游戏场景可调用 joy_r3d_set_clear_color 改为白底 */
	r->clear_r = 13; r->clear_g = 15; r->clear_b = 26;

	/* 2D UI 逻辑分辨率默认关闭(直接用窗口像素), 缩放系数初始为 1 */
	r->logical_w = 0; r->logical_h = 0;
	r->logical_mode = JOY_LOGICAL_PRESENTATION_STRETCH;
	r->ui_sx = 1.0f; r->ui_sy = 1.0f;
	r->ui_offx = 0.0f; r->ui_offy = 0.0f;

	/* 字体默认白色 */
	r->font_color_r = 255; r->font_color_g = 255; r->font_color_b = 255;

	/* 文字烘焙缓存默认上限 32 条, LRU 计时与帧计数从 0 开始 */
	r->font_cache_limit = 32;
	r->font_cache_tick = 0;
	r->font_cache_frame = 0;

	/* 加载着色器（兼容 shader_dir 带/不带尾部分隔符, nullptr 视为空串） */
	const char* sd = shader_dir ? shader_dir : "";
	size_t dirlen = strlen(sd);
	bool has_sep = (dirlen > 0) && (sd[dirlen - 1] == '/' || sd[dirlen - 1] == '\\');
	const char* sep = has_sep ? "" : "/";
	size_t plen = dirlen + 16;
	char* vert_path = (char*)SDL_malloc(plen);
	char* frag_path = (char*)SDL_malloc(plen);
	if (!vert_path || !frag_path) { SDL_free(vert_path); SDL_free(frag_path); joy_r3d_destroy(r); return NULL; }
	snprintf(vert_path, plen, "%s%svert.spv", sd, sep);
	snprintf(frag_path, plen, "%s%sfrag.spv", sd, sep);
	r->vs = (SDL_GPUShader*)joy_r3d_load_shader(r->dev, vert_path, SDL_GPU_SHADERSTAGE_VERTEX, 0, 1);
	r->fs = (SDL_GPUShader*)joy_r3d_load_shader(r->dev, frag_path, SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0);
	SDL_free(vert_path); SDL_free(frag_path);
	if (!r->vs || !r->fs) { joy_r3d_destroy(r); return NULL; }

	SDL_GPUTextureFormat fmt = SDL_GetGPUSwapchainTextureFormat(r->dev, r->win);
	r->pipeline = (SDL_GPUGraphicsPipeline*)joy_r3d_create_pipeline(r->dev,
		r->vs, r->fs, (int)fmt, (int)SDL_GPU_TEXTUREFORMAT_D16_UNORM);
	if (!r->pipeline) { joy_r3d_destroy(r); return NULL; }

	/* 线框 pipeline（与非 lit 共用 shader, 仅 primitive type 不同） */
	r->line_pipeline = (SDL_GPUGraphicsPipeline*)joy_r3d_create_line_pipeline(r->dev,
		r->vs, r->fs, (int)fmt, (int)SDL_GPU_TEXTUREFORMAT_D16_UNORM);

	/* 实体填充 pipeline（写深度, 用于"填充+描边"画风的填充 pass） */
	r->solid_pipeline = (SDL_GPUGraphicsPipeline*)joy_r3d_create_solid_pipeline(r->dev,
		r->vs, r->fs, (int)fmt, (int)SDL_GPU_TEXTUREFORMAT_D16_UNORM);

	r->sampler = (SDL_GPUSampler*)joy_r3d_create_sampler(r->dev);
	if (!r->sampler) { joy_r3d_destroy(r); return NULL; }

	SDL_GetWindowSize(r->win, &r->ww, &r->wh);
	r->last_ww = r->ww; r->last_wh = r->wh;
	r->depth = (SDL_GPUTexture*)joy_r3d_create_depth_texture(r->dev, r->ww, r->wh);
	if (!r->depth) { joy_r3d_destroy(r); return NULL; }

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
			snprintf(lvert_path, lplen, "%s%slighting.vert.spv", sd, sep);
			snprintf(lfrag_path, lplen, "%s%slighting.frag.spv", sd, sep);
			r->lvs = (SDL_GPUShader*)joy_r3d_load_shader(r->dev, lvert_path, SDL_GPU_SHADERSTAGE_VERTEX, 0, 1);
			r->lfs = (SDL_GPUShader*)joy_r3d_load_shader(r->dev, lfrag_path, SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 1);
			if (r->lvs && r->lfs) {
				r->lighting_pipeline = (SDL_GPUGraphicsPipeline*)joy_r3d_create_lighting_pipeline(r->dev,
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

void joy_r3d_destroy(joy_r3d_p r)
{
	if (!r) return;
	if (r->dev) SDL_WaitForGPUIdle(r->dev);

	for (int i = 0; i < r->color_count; i++)
		if (r->color_cache[i].tex) SDL_ReleaseGPUTexture(r->dev, r->color_cache[i].tex);
	r->color_count = 0;

	if (r->depth)    SDL_ReleaseGPUTexture(r->dev, r->depth);
	if (r->dev && r->fov_gradient_tex) SDL_ReleaseGPUTexture(r->dev, r->fov_gradient_tex);
	if (r->sampler)  SDL_ReleaseGPUSampler(r->dev, r->sampler);
	if (r->pipeline) SDL_ReleaseGPUGraphicsPipeline(r->dev, r->pipeline);
	if (r->line_pipeline) SDL_ReleaseGPUGraphicsPipeline(r->dev, r->line_pipeline);
	if (r->solid_pipeline) SDL_ReleaseGPUGraphicsPipeline(r->dev, r->solid_pipeline);
	if (r->lighting_pipeline) SDL_ReleaseGPUGraphicsPipeline(r->dev, r->lighting_pipeline);
	if (r->vs)       SDL_ReleaseGPUShader(r->dev, r->vs);
	if (r->fs)       SDL_ReleaseGPUShader(r->dev, r->fs);
	if (r->lvs)      SDL_ReleaseGPUShader(r->dev, r->lvs);
	if (r->lfs)      SDL_ReleaseGPUShader(r->dev, r->lfs);
	/* 释放文字烘焙缓存里所有 GPU 纹理。注意 font_tex 与某个 font_slots[i].tex
	   常常指向同一纹理(每次 bake 后新纹理被存入槽, 无额外 acquire), 因此
	   先释放所有槽, 再单独释放 font_tex——但跳过已在槽中出现过的指针, 避免双重释放。 */
	for (int i = 0; i < r->font_slots_used; i++) {
		if (r->font_slots[i].tex) SDL_ReleaseGPUTexture(r->dev, r->font_slots[i].tex);
	}
	if (r->font_tex) {
		int in_slot = 0;
		for (int i = 0; i < r->font_slots_used; i++) {
			if (r->font_slots[i].tex == r->font_tex) { in_slot = 1; break; }
		}
		if (!in_slot) SDL_ReleaseGPUTexture(r->dev, r->font_tex);
	}
	if (r->dev) {
		/* 惰性共享 mesh 的 GPU buffer 走延迟队列, 必须在 flush 之前入队 */
		if (r->unit_disc)     { joy_r3d_mesh_destroy(r, r->unit_disc); r->unit_disc = NULL; }
		if (r->fov_prev_mesh) { joy_r3d_mesh_destroy(r, r->fov_prev_mesh); r->fov_prev_mesh = NULL; }
		r3d_flush_pending(r); /* 释放所有延迟队列中的 buffer */
		if (r->pending_buffers) SDL_free(r->pending_buffers);
		SDL_ReleaseWindowFromGPUDevice(r->dev, r->win);
		SDL_DestroyGPUDevice(r->dev);
	}
	/* SDL_LoadFile 分配的缓冲必须走 SDL_free; 此时 r3d 即将销毁,
	   font_info 不会再被使用, 释放其偏移指针指向的缓冲是安全的。 */
	if (r->font_ttf_buf) SDL_free(r->font_ttf_buf);
	SDL_free(r);
}

/* ---- 帧管理 ---- */

bool joy_r3d_begin(joy_r3d_p r)
{
	if (!r) return false;
	r->draw_calls = 0;
	r->font_cache_frame++;   /* 渲染帧计数, 供文字缓存 sweep 判断闲置帧数 */
	r->cmd = SDL_AcquireGPUCommandBuffer(r->dev);
	if (!r->cmd) return false;

	SDL_GetWindowSize(r->win, &r->ww, &r->wh);
	if (r->ww < 1 || r->wh < 1) { SDL_SubmitGPUCommandBuffer(r->cmd); r->cmd = NULL; return false; }

	/* 2D UI 缩放系数(ui_sx/ui_sy/ui_offx/ui_offy)在下方 resize 检测之后统一计算, 使用最终 swapchain 尺寸 */

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
		r->depth = (SDL_GPUTexture*)joy_r3d_create_depth_texture(r->dev, r->ww, r->wh);
	}

	/* 2D UI 逻辑分辨率: 根据呈现模式计算等比缩放系数 + letterbox 居中偏移(使用最终 swapchain 尺寸) */
	if (r->logical_w > 0 && r->logical_h > 0) {
		if (r->logical_mode == JOY_LOGICAL_PRESENTATION_LETTERBOX) {
			float sx = (float)r->ww / (float)r->logical_w;
			float sy = (float)r->wh / (float)r->logical_h;
			float s  = (sx < sy) ? sx : sy;          /* 取较小者: 逻辑分辨率完整可见, 等比 letterbox */
			r->ui_sx = s;
			r->ui_sy = s;
			r->ui_offx = ((float)r->ww - (float)r->logical_w * s) * 0.5f;
			r->ui_offy = ((float)r->wh - (float)r->logical_h * s) * 0.5f;
		} else {
			/* STRETCH(默认, 保持旧行为): 分别按 x/y 拉伸铺满 */
			r->ui_sx = (float)r->ww / (float)r->logical_w;
			r->ui_sy = (float)r->wh / (float)r->logical_h;
			r->ui_offx = 0.0f;
			r->ui_offy = 0.0f;
		}
	} else {
		r->ui_sx = 1.0f; r->ui_sy = 1.0f;
		r->ui_offx = 0.0f; r->ui_offy = 0.0f;
	}

	SDL_GPUColorTargetInfo color = { 0 };
	color.clear_color.r = r->clear_r / 255.0f; color.clear_color.g = r->clear_g / 255.0f;
	color.clear_color.b = r->clear_b / 255.0f; color.clear_color.a = 1;
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

void joy_r3d_end(joy_r3d_p r)
{
	if (!r) return;
	if (r->rp)  { SDL_EndGPURenderPass(r->rp); r->rp = NULL; }
	if (r->cmd) { SDL_SubmitGPUCommandBuffer(r->cmd); r->cmd = NULL; }
	/* 帧提交后再释放上一帧遗留的旧 buffer, 此时 GPU 已完成渲染 */
	r3d_flush_pending(r);
}

int joy_r3d_get_draw_calls(joy_r3d_p r)
{
	return r ? r->draw_calls : 0;
}

/* ---- 相机 ---- */

void joy_r3d_camera_lookat(joy_r3d_p r, joy_vec3_t eye, joy_vec3_t target, joy_vec3_t up, float fovy)
{
	joy_mat44_t view;
	joy_mat44_t proj;
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

void joy_r3d_camera_fps(joy_r3d_p r, joy_vec3_t pos, float yaw, float pitch, float fovy)
{
	float yr, pr;
	joy_vec3_t camOffset;
	joy_vec3_t target;
	joy_vec3_t eye;
	joy_vec3_t upv;
	if (!r) return;
	yr = ft_radians(yaw); pr = ft_radians(pitch);
	camOffset.x = -sinf(yr) * cosf(pr) * 6.0f;
	camOffset.y =  sinf(pr) * 6.0f;
	camOffset.z = -cosf(yr) * cosf(pr) * 6.0f;
	upv.x = 0; upv.y = 1; upv.z = 0;
	target.x = pos.x; target.y = pos.y + 1.0f; target.z = pos.z;
	eye = vec3_add(pos, camOffset);
	joy_r3d_camera_lookat(r, eye, target, upv, fovy);
}

/* ---- 网格 ---- */

joy_render_mesh_p joy_r3d_mesh_cube(joy_r3d_p r, float w, float h, float d)
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
	return joy_r3d_mesh_custom(r, verts, 36);
}

joy_render_mesh_p joy_r3d_mesh_sphere(joy_r3d_p r, float radius, int slices, int stacks)
{
	float* verts;
	int count, j, i, o;
	joy_render_mesh_p m;
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
	m = joy_r3d_mesh_custom(r, verts, count);
	SDL_free(verts);
	return m;
}

joy_render_mesh_p joy_r3d_mesh_plane(joy_r3d_p r, float w, float d, int segs)
{
	float hw, hd, step_x, step_z, x0, z0, x1, z1, u0, v0, u1, v1;
	int total, iz, ix, idx;
	float* verts;
	joy_render_mesh_p m;
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
	m = joy_r3d_mesh_custom(r, verts, total);
	SDL_free(verts);
	return m;
}

joy_render_mesh_p joy_r3d_mesh_ramp(joy_r3d_p r, float w, float l, float h)
{
	float hw, hl;
	float* verts;
	joy_render_mesh_p m;
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

	m = joy_r3d_mesh_custom(r, verts, 24);
	SDL_free(verts);
	return m;
}

joy_render_mesh_p joy_r3d_mesh_custom(joy_r3d_p r, const void* verts, int vertex_count)
{
	joy_render_mesh_p m;
	(void)r;
	m = r3d_mesh_alloc(r);
	if (!m) return NULL;
	m->buffer = (SDL_GPUBuffer*)joy_r3d_upload_buffer(r->dev, verts, (size_t)vertex_count * 5 * sizeof(float));
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = vertex_count;
	return m;
}

/* 通用 RGBA32 纹理创建: 自动处理 upload 行对齐, flip=1 时上下翻转(与 stbi_set_flip_vertically_on_load 对应) */
void* joy_r3d_create_texture(joy_r3d_p r, int w, int h, const void* rgba32, int flip)
{
	if (!r || !rgba32 || w <= 0 || h <= 0) return NULL;
	return (void*)upload_pixels_to_texture(r->dev, (const unsigned char*)rgba32, w, h,
		SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, SDL_GPU_TEXTUREUSAGE_SAMPLER, flip);
}

/* 从 PNG 文件加载并创建 GPU 纹理: SDL_LoadPNG -> RGBA32 -> joy_r3d_create_texture(flip=1)
   返回 SDL_GPUTexture* (void*)。失败返回 NULL 并通过 SDL_GetError 暴露原因。
   统一封装了图片文件加载, 供各 app 复用, 避免重复手写 SDL_LoadPNG 流程。 */
/* 纹理缺失兜底：按路径哈希生成稳定的纯色（不同缺失资源得到不同但确定的颜色，便于定位缺哪些贴图） */
static void r3d_missing_tex_color(const char* path, unsigned char* out)
{
	unsigned long h = 2166136261u;
	for (const char* p = path; *p; p++) { h ^= (unsigned char)(*p); h *= 16777619u; }
	out[0] = (unsigned char)(48 + (h & 0xBF));
	out[1] = (unsigned char)(48 + ((h >> 8) & 0xBF));
	out[2] = (unsigned char)(48 + ((h >> 16) & 0xBF));
}

void* joy_r3d_load_texture_png(joy_r3d_p r, const char* path, int* outW, int* outH)
{
	if (!r || !path) return NULL;
	SDL_Surface* surf = SDL_LoadPNG(path);
	if (!surf) {
		/* 兜底：运行时 CWD 未必是资源根目录（直接双击 exe / 从其它目录启动），
		   尝试相对可执行文件所在目录解析；资源根就是 exe 同目录，path 一律
		   不带 assets/ 前缀（Android 由 AAssetManager 按 APK assets 根解析）。
		   注意: SDL_GetBasePath() 返回 SDL 内部缓存指针，由 SDL 退出时释放，调用方不能 SDL_free。 */
		const char* base = SDL_GetBasePath();
		if (base) {
			char alt[1024];
			SDL_snprintf(alt, sizeof(alt), "%s%s", base, path);
			surf = SDL_LoadPNG(alt);
		}
	}
	if (!surf) {
		/* 纹理文件缺失：返回纯色纹理(色块)，避免引用该纹理的网格消失/崩溃 */
		unsigned char c[3];
		r3d_missing_tex_color(path, c);
		SDL_Log("joy_r3d_load_texture_png: %s 缺失, 回退为纯色色块(%d,%d,%d)", path, c[0], c[1], c[2]);
		if (outW) *outW = 1;
		if (outH) *outH = 1;
		return joy_r3d_create_solid_texture(r, c[0], c[1], c[2], 255);
	}
	SDL_Surface* rgba = SDL_ConvertSurface(surf, SDL_PIXELFORMAT_RGBA32);
	int w = surf->w, h = surf->h;
	SDL_DestroySurface(surf);
	if (!rgba) {
		unsigned char c[3];
		r3d_missing_tex_color(path, c);
		SDL_Log("joy_r3d_load_texture_png: convert %s failed, 回退为纯色色块(%d,%d,%d)", path, c[0], c[1], c[2]);
		if (outW) *outW = 1;
		if (outH) *outH = 1;
		return joy_r3d_create_solid_texture(r, c[0], c[1], c[2], 255);
	}
	void* tex = joy_r3d_create_texture(r, rgba->w, rgba->h, rgba->pixels, 1);
	SDL_DestroySurface(rgba);
	if (outW) *outW = w;
	if (outH) *outH = h;
	SDL_Log("joy_r3d_load_texture_png: OK %s %dx%d tex=%p", path, w, h, tex);
	return tex;
}

/* 前向声明: 用自定义投影矩阵绘制(供字体 2D/3D 函数使用, 定义在下方) */
static void draw_mesh_internal_proj(joy_r3d_p r, joy_render_mesh_p m, joy_mat44_t* model, joy_mat44_t* proj, SDL_GPUTexture* tex);

/* ================================================================
 * 字体渲染 (stb_truetype)
 *   整段文字烤成一张 RGBA32 纹理(白字形 + alpha), UV (0,0)-(1,1),
 *   通过 joy_r3d_draw_mesh_tex_basis 以 billboard 方式贴到 3D 空间。
 *   算法沿用原 main_tex.cpp 中已验证可用的 yaw/pitch 基向量计算,
 *   保证文字始终直立、不斜切。
 * ================================================================ */

/* 初始化字体: 逐行照抄原版 font_init, 仅 gFont* -> r->font* */
bool joy_r3d_font_init(joy_r3d_p r, const char* ttf_path, int pixel_height)
{
	if (!r || !ttf_path || pixel_height <= 0) return false;

	/* 重新 init 时先释放旧字体缓冲与全部烘焙纹理。旧缓冲被 font_info 持有偏移
	   指针, 但下面马上 InitFont 重新绑定, 期间不调用任何 stbtt 函数, 安全。
	   不能用 memset 直接丢弃 GPU 纹理句柄, 那是泄漏。 */
	joy_r3d_font_cache_clear(r);
	if (r->font_ttf_buf) { SDL_free(r->font_ttf_buf); r->font_ttf_buf = NULL; }

	// 读取 ttf (工作目录为 build/<config>, 字体在 joy_editor/fonts/arial.ttf)
	// 走 SDL_LoadFile：安卓上资源在 APK 内，fopen 读不到。
	size_t ttf_sz = 0;
	r->font_ttf_buf = (unsigned char*)SDL_LoadFile(ttf_path, &ttf_sz);
	if (!r->font_ttf_buf) { SDL_Log("font: open %s failed", ttf_path); return false; }

	if (!stbtt_InitFont(&r->font_info, r->font_ttf_buf, 0)) {
		SDL_Log("font: InitFont failed");
		SDL_free(r->font_ttf_buf);   /* SDL_LoadFile 必须用 SDL_free 释放 */
		r->font_ttf_buf = NULL;
		return false;
	}
	r->font_scale = stbtt_ScaleForPixelHeight(&r->font_info, (float)pixel_height);

	int ascent = 0, descent = 0, lineGap = 0;
	stbtt_GetFontVMetrics(&r->font_info, &ascent, &descent, &lineGap);
	r->font_ascent = (int)(ascent * r->font_scale);
	r->font_pix_h  = pixel_height;
	r->font_cache[0] = '\0';
	SDL_Log("font: ready scale=%.3f ascent=%d", r->font_scale, r->font_ascent);
	return true;
}

/* 设置 3D 世界文字颜色(RGB 烘焙进下一段 bake 的纹理, 需在 draw_3d 前调用) */
void joy_r3d_font_set_color(joy_r3d_p r, Uint8 cr, Uint8 cg, Uint8 cb)
{
	if (!r) return;
	r->font_color_r = cr; r->font_color_g = cg; r->font_color_b = cb;
}

/* 把整段字符串烘焙成一张 RGBA 纹理(白字 + alpha), 逐行照抄原版 bake_text_texture,
   仅 gFont 系列全局变量 -> r->font 结构体字段, gTextTex -> r->font_tex
   (固定宽度 gFontPixH*n, 原版对齐/裁剪, flip=0) */
static bool r3d_font_bake(joy_r3d_p r, const char* str)
{
	if (str == NULL) return false;

	/* 多槽缓存命中: 已有同 (字符串,颜色) 的烘焙纹理则直接复用, 不再光栅化/上传。
	   命中时刷新 last_use 以支持 LRU 淘汰。 */
	for (int i = 0; i < r->font_slots_used; i++) {
		if (r->font_slots[i].tex &&
			r->font_slots[i].r == r->font_color_r &&
			r->font_slots[i].g == r->font_color_g &&
			r->font_slots[i].b == r->font_color_b &&
			SDL_strcmp(r->font_slots[i].str, str) == 0) {
			r->font_tex   = r->font_slots[i].tex;
			r->font_tex_w = r->font_slots[i].w;
			r->font_tex_h = r->font_slots[i].h;
			r->font_slots[i].last_use = ++r->font_cache_tick;
			r->font_slots[i].last_frame = r->font_cache_frame;
			SDL_strlcpy(r->font_cache, str, sizeof(r->font_cache));
			r->font_cache_r = r->font_color_r; r->font_cache_g = r->font_color_g; r->font_cache_b = r->font_color_b;
			return true;
		}
	}
	// 兼容旧单槽路径(槽缓存为空时)
	if (r->font_slots_used == 0 && r->font_tex && SDL_strcmp(str, r->font_cache) == 0 &&
		r->font_color_r == r->font_cache_r && r->font_color_g == r->font_cache_g && r->font_color_b == r->font_cache_b)
		return true;

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
		size_t consumed = joy_core_utf8_decode(str + pos, (size_t)n - pos, &code);
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

	// 转成 RGBA32(文字颜色 + alpha)。位图第 0 行=顶部, 但 quad UV 是 (0,0)=左上对应纹理顶行,
	// 而 GPU 纹理第 0 行在底部, 这里把行上下翻转写入, 使纹理内容正确为正向。
	unsigned char* rgba = (unsigned char*)SDL_malloc((size_t)final_w * real_height * 4);
	if (!rgba) { SDL_free(bitmap); return false; }
	for (int yy = 0; yy < real_height; yy++) {
		int src_y = real_height - 1 - yy;   // 上下翻转: 顶部行写到纹理底, 底行写到顶
		for (int xx = 0; xx < final_w; xx++) {
			unsigned char a = bitmap[src_y * real_width + xx];
			int idx = (yy * final_w + xx) * 4;
			rgba[idx + 0] = r->font_color_r;
			rgba[idx + 1] = r->font_color_g;
			rgba[idx + 2] = r->font_color_b;
			rgba[idx + 3] = a;
		}
	}
	SDL_free(bitmap);

	// 注意: 不能在此释放旧的 r->font_tex——它已被存入某个缓存槽并被引用,
	// 生命周期交由槽的 LRU 淘汰统一管理。这里仅创建新纹理。
	r->font_tex = (SDL_GPUTexture*)joy_r3d_create_texture(r, final_w, real_height, rgba, 0);
	SDL_free(rgba);
	r->font_tex_w = final_w;
	r->font_tex_h = real_height;
	SDL_strlcpy(r->font_cache, str, sizeof(r->font_cache));
	r->font_cache_r = r->font_color_r; r->font_cache_g = r->font_color_g; r->font_cache_b = r->font_color_b;

	/* 存入多槽缓存。槽满(达到上限)时淘汰 last_use 最小的条目(LRU), 并释放其 GPU 纹理。
	   用 LRU 而非 FIFO: 固定的按钮/HUD 文字频繁命中, 不会被偶尔变化的文本(FPS/HP)挤掉,
	   避免每帧重建固定文字的 GPU 纹理(缓存颠簸)。 */
	Uint64 tick = ++r->font_cache_tick;
	if (r->font_slots_used < r->font_cache_limit) {
		r->font_slots[r->font_slots_used].tex = r->font_tex;
		r->font_slots[r->font_slots_used].w = final_w;
		r->font_slots[r->font_slots_used].h = real_height;
		r->font_slots[r->font_slots_used].r = r->font_color_r;
		r->font_slots[r->font_slots_used].g = r->font_color_g;
		r->font_slots[r->font_slots_used].b = r->font_color_b;
		r->font_slots[r->font_slots_used].last_use = tick;
		r->font_slots[r->font_slots_used].last_frame = r->font_cache_frame;
		SDL_strlcpy(r->font_slots[r->font_slots_used].str, str, sizeof(r->font_slots[r->font_slots_used].str));
		r->font_slots_used++;
	} else {
		int victim = 0;
		for (int i = 1; i < r->font_slots_used; i++) {
			if (r->font_slots[i].last_use < r->font_slots[victim].last_use) victim = i;
		}
		if (r->font_slots[victim].tex) SDL_ReleaseGPUTexture(r->dev, r->font_slots[victim].tex);
		r->font_slots[victim].tex = r->font_tex;
		r->font_slots[victim].w = final_w;
		r->font_slots[victim].h = real_height;
		r->font_slots[victim].r = r->font_color_r;
		r->font_slots[victim].g = r->font_color_g;
		r->font_slots[victim].b = r->font_color_b;
		r->font_slots[victim].last_use = tick;
		r->font_slots[victim].last_frame = r->font_cache_frame;
		SDL_strlcpy(r->font_slots[victim].str, str, sizeof(r->font_slots[victim].str));
	}

	return true;
}

/* 在 3D 空间绘制文字 billboard。
   topLeft: 文字左上角锚点(世界坐标); yaw/pitch: 屏幕基向量方向(与相机一致);
   size: 文字在世界中的高度。 right/up 由 yaw/pitch 推得, 保证文字直立。 */
bool joy_r3d_font_draw_3d(joy_r3d_p r, joy_render_mesh_p m, const char* text,
					  joy_vec3_t topLeft, float yaw, float pitch, float size)
{
	if (!r || !m || !text || size <= 0) return false;
	if (!r->font_ttf_buf) { SDL_Log("joy_r3d_font_draw_3d: font not init"); return false; }

	if (SDL_strcmp(r->font_cache, text) != 0 ||
		r->font_color_r != r->font_cache_r ||
		r->font_color_g != r->font_cache_g ||
		r->font_color_b != r->font_cache_b) {
		if (!r3d_font_bake(r, text)) return false;
	}

	float cy = cosf(yaw), sy = sinf(yaw);
	float cp = cosf(pitch), sp = sinf(pitch);
	joy_vec3_t right = { -cy, 0.0f, sy };
	joy_vec3_t up    = { sp * sy, cp, sp * sy };
	float ulen = sqrtf(up.x * up.x + up.y * up.y + up.z * up.z);
	if (ulen > 1e-4f) { up.x /= ulen; up.y /= ulen; up.z /= ulen; }

	float aspect = (float)r->font_tex_w / (float)r->font_tex_h;
	float worldH = size;
	float worldW = size * aspect;
	joy_vec3_t rightScaled = { right.x * aspect, right.y * aspect, right.z * aspect };
	joy_vec3_t center = {
		topLeft.x + rightScaled.x * (worldW * 0.5f) + up.x * (worldH * 0.5f),
		topLeft.y + rightScaled.y * (worldW * 0.5f) + up.y * (worldH * 0.5f),
		topLeft.z + rightScaled.z * (worldW * 0.5f) + up.z * (worldH * 0.5f),
	};
	joy_r3d_draw_mesh_tex_basis(r, m, center, rightScaled, up, worldH, (void*)r->font_tex);
	return true;
}

/* 在 3D 空间绘制文字, 朝向固定(不随相机 yaw/pitch 变化): 始终水平直立、正对 -Z。
   topLeft: 文字左上角锚点(世界坐标); size: 文字世界高度。 */
bool joy_r3d_font_draw_fixed(joy_r3d_p r, joy_render_mesh_p m, const char* text,
						 joy_vec3_t topLeft, float size)
{
	if (!r || !m || !text || size <= 0) return false;
	if (!r->font_ttf_buf) { SDL_Log("joy_r3d_font_draw_fixed: font not init"); return false; }

	if (SDL_strcmp(r->font_cache, text) != 0 ||
		r->font_color_r != r->font_cache_r ||
		r->font_color_g != r->font_cache_g ||
		r->font_color_b != r->font_cache_b) {
		if (!r3d_font_bake(r, text)) return false;
	}

	/* 固定基向量: 让文字水平横卧于 XZ 地面平面、字形朝上(对俯视相机可读)。
	   摄像机视角下屏幕右≈世界 -X, 屏幕上方≈世界 +Z(字形顶), 故取 right=-X, up=-Z。
	   若取 up=+Y 会得到竖直 XY 平面, 在俯视角下字形横竖颠倒。 */
	joy_vec3_t right = { -1.0f, 0.0f, 0.0f };
	joy_vec3_t up    = {  0.0f, 0.0f, -1.0f };

	float aspect = (float)r->font_tex_w / (float)r->font_tex_h;
	float worldH = size;
	float worldW = size * aspect;
	joy_vec3_t rightScaled = { right.x * aspect, right.y * aspect, right.z * aspect };
	joy_vec3_t center = {
		topLeft.x + rightScaled.x * (worldW * 0.5f) + up.x * (worldH * 0.5f),
		topLeft.y + rightScaled.y * (worldW * 0.5f) + up.y * (worldH * 0.5f),
		topLeft.z + rightScaled.z * (worldW * 0.5f) + up.z * (worldH * 0.5f),
	};
	joy_r3d_draw_mesh_tex_basis(r, m, center, rightScaled, up, worldH, (void*)r->font_tex);
	return true;
}

/* 在 3D 空间绘制文字 billboard: 文字平面由 right/up 基向量张成(通常取相机基向量,
   使文字始终面向相机、不因俯视角压扁), 文字沿 right 横向、沿 up 纵向, 字形正向同 2D 文字。 */
bool joy_r3d_font_draw_billboard(joy_r3d_p r, joy_render_mesh_p m, const char* text,
							 joy_vec3_t topLeft, joy_vec3_t right, joy_vec3_t up, float size)
{
	if (!r || !m || !text || size <= 0) return false;
	if (!r->font_ttf_buf) { SDL_Log("joy_r3d_font_draw_billboard: font not init"); return false; }

	if (SDL_strcmp(r->font_cache, text) != 0 ||
		r->font_color_r != r->font_cache_r ||
		r->font_color_g != r->font_cache_g ||
		r->font_color_b != r->font_cache_b) {
		if (!r3d_font_bake(r, text)) return false;
	}

	float aspect = (float)r->font_tex_w / (float)r->font_tex_h;
	joy_vec3_t rightScaled = { right.x * aspect, right.y * aspect, right.z * aspect };
	joy_vec3_t center = {
		topLeft.x + rightScaled.x * (size * 0.5f) + up.x * (size * 0.5f),
		topLeft.y + rightScaled.y * (size * 0.5f) + up.y * (size * 0.5f),
		topLeft.z + rightScaled.z * (size * 0.5f) + up.z * (size * 0.5f),
	};
	joy_r3d_draw_mesh_tex_basis(r, m, center, rightScaled, up, size, (void*)r->font_tex);
	return true;
}

/* 在屏幕(窗口)坐标绘制文字, 像 2D HUD 一样固定在窗口左上角, 不随相机旋转/移动。
   pixelX/pixelY: 文字左上角在窗口中的像素坐标(原点左上, Y 向下);
   fontSize: 文字像素高度; winW/winH: 窗口像素尺寸(通常传 r->ww/r->wh)。 */
static bool r3d_font_draw_2d_impl(joy_r3d_p r, joy_render_mesh_p m, const char* text,
								  float pixelX, float pixelY, float fontSize,
								  float winW, float winH)
{
	if (!r || !m || !text || fontSize <= 0 || winW <= 0 || winH <= 0) return false;
	if (!r->font_ttf_buf) { SDL_Log("joy_r3d_font_draw_2d: font not init"); return false; }

	/* 使用当前 r->font_color_* 作为烘焙颜色(由 joy_r3d_font_draw_2d / _color 设置) */

	if (SDL_strcmp(r->font_cache, text) != 0 ||
		r->font_color_r != r->font_cache_r ||
		r->font_color_g != r->font_cache_g ||
		r->font_color_b != r->font_cache_b) {
		if (!r3d_font_bake(r, text)) return false;
	}

	float aspect = (float)r->font_tex_w / (float)r->font_tex_h;
	float worldW = fontSize * aspect;
	float worldH = fontSize;

	/* model: 把单位 quad([-0.5,0.5]) 缩放到像素尺寸并平移到屏幕像素位置(中心对齐) */
	joy_mat44_t model; memset(&model, 0, sizeof(model));
	model.m0  = worldW;
	model.m5  = worldH;
	model.m10 = 1.0f;
	model.m12 = pixelX + worldW * 0.5f;
	model.m13 = pixelY + worldH * 0.5f;
	model.m14 = 0.0f;   /* SDL_GPU 的 NDC 深度范围是 [0,1]（Vulkan/D3D 惯例），不是 OpenGL 的 [-1,1]。
	                       原先 -1 超出 [0,1]，引擎打开 enable_depth_clip 后整个文字四边形被深度裁剪丢弃→文字不可见。
	                       改为 0：NDC.z=0 落在范围内、且为最靠前（depth 0），不被 3D 场景遮挡，与 draw_rect_2d 一致。 */
	model.m15 = 1.0f;

	/* 屏幕像素正交投影: 原点左上, X 向右, Y 向下, 覆盖整个窗口。
	   这是纯屏幕坐标映射, 不修正文字朝向——文字朝向由纹理内容保证。 */
	joy_mat44_t proj; memset(&proj, 0, sizeof(proj));
	/* 2D 正交投影: 逻辑坐标 -> 窗口像素(含 letterbox 居中偏移), 由 joy_r3d_begin 的 ui_sx/ui_sy/ui_offx/ui_offy 决定 */
	if (r->logical_mode == JOY_LOGICAL_PRESENTATION_LETTERBOX) {
		proj.m0  =  2.0f * r->ui_sx / (float)r->ww;
		proj.m5  = -2.0f * r->ui_sy / (float)r->wh;
		proj.m10 =  1.0f;
		proj.m12 =  2.0f * r->ui_offx / (float)r->ww - 1.0f;
		proj.m13 =  1.0f - 2.0f * r->ui_offy / (float)r->wh;
		proj.m15 =  1.0f;
	} else {
		proj.m0  =  2.0f / winW;
		proj.m5  = -2.0f / winH;
		proj.m10 =  1.0f;
		proj.m12 = -1.0f;
		proj.m13 =  1.0f;
		proj.m15 =  1.0f;
	}

	draw_mesh_internal_proj(r, m, &model, &proj, (SDL_GPUTexture*)r->font_tex);
	return true;
}

/* 无颜色版本默认白色(避免残留上一次 joy_r3d_font_draw_2d_color 的颜色) */
bool joy_r3d_font_draw_2d(joy_r3d_p r, joy_render_mesh_p m, const char* text,
					  float pixelX, float pixelY, float fontSize,
					  float winW, float winH)
{
	if (!r) return false;
	r->font_color_r = 255; r->font_color_g = 255; r->font_color_b = 255;
	return r3d_font_draw_2d_impl(r, m, text, pixelX, pixelY, fontSize, winW, winH);
}

/* 带颜色的 2D 文字绘制: 设置烘焙颜色后走公共绘制逻辑(不能被 draw_2d 重置为白色) */
bool joy_r3d_font_draw_2d_color(joy_r3d_p r, joy_render_mesh_p m, const char* text,
						  float pixelX, float pixelY, float fontSize,
						  float winW, float winH,
						  Uint8 cr, Uint8 cg, Uint8 cb)
{
	if (!r) return false;
	r->font_color_r = cr; r->font_color_g = cg; r->font_color_b = cb;
	return r3d_font_draw_2d_impl(r, m, text, pixelX, pixelY, fontSize, winW, winH);
}

/* 带描边的 2D HUD 文字: 先烘焙描边色绘制 4 个方向偏移, 再烘焙主色绘制原位。
   描边宽度 outline 为像素。任何背景下均可读(如浅色地面上的黑色昵称加白描边)。 */
bool joy_r3d_font_draw_2d_outline(joy_r3d_p r, joy_render_mesh_p m, const char* text,
							  float pixelX, float pixelY, float fontSize,
							  float winW, float winH,
							  Uint8 cr, Uint8 cg, Uint8 cb,     /* 主色 */
							  Uint8 or_, Uint8 og, Uint8 ob,    /* 描边色 */
							  float outline)
{
	if (!r || !m || !text || fontSize <= 0 || outline <= 0) return false;

	/* 描边: 烘焙描边色, 4 个方向各偏移 outline 像素(同色不重复烘焙) */
	r->font_color_r = or_; r->font_color_g = og; r->font_color_b = ob;
	r3d_font_draw_2d_impl(r, m, text, pixelX - outline, pixelY, fontSize, winW, winH);
	r3d_font_draw_2d_impl(r, m, text, pixelX + outline, pixelY, fontSize, winW, winH);
	r3d_font_draw_2d_impl(r, m, text, pixelX, pixelY - outline, fontSize, winW, winH);
	r3d_font_draw_2d_impl(r, m, text, pixelX, pixelY + outline, fontSize, winW, winH);

	/* 主色原位 */
	r->font_color_r = cr; r->font_color_g = cg; r->font_color_b = cb;
	return r3d_font_draw_2d_impl(r, m, text, pixelX, pixelY, fontSize, winW, winH);
}

void joy_r3d_get_window_size(joy_r3d_p r, int* outW, int* outH)
{
	if (!r) return;
	if (outW) *outW = r->last_ww;
	if (outH) *outH = r->last_wh;
}

/* 设置文字烘焙缓存上限(最小 4, 最大 64, 超出物理数组容量会被截断) */
void joy_r3d_font_cache_set_limit(joy_r3d_p r, int max_entries)
{
	int i, victim;
	if (!r) return;
	if (max_entries < 4) max_entries = 4;
	if (max_entries > 64) max_entries = 64;
	/* 缩小上限: 释放超出部分的条目(淘汰 last_use 最小的) */
	while (r->font_slots_used > max_entries) {
		victim = 0;
		for (i = 1; i < r->font_slots_used; i++) {
			if (r->font_slots[i].last_use < r->font_slots[victim].last_use) victim = i;
		}
		if (r->font_slots[victim].tex) SDL_ReleaseGPUTexture(r->dev, r->font_slots[victim].tex);
		r->font_slots[victim] = r->font_slots[r->font_slots_used - 1];
		r->font_slots_used--;
	}
	r->font_cache_limit = max_entries;
}

int joy_r3d_font_cache_get_count(joy_r3d_p r)
{
	return r ? r->font_slots_used : 0;
}

/* 清空缓存并释放全部缓存纹理。font_tex 若指向被释放的纹理也一并置空失效,
   保证后续调用不会用到悬空纹理(r3d_font_draw_* 在 font_tex 为空时会走 bake 重建)。 */
void joy_r3d_font_cache_clear(joy_r3d_p r)
{
	if (!r) return;
	for (int i = 0; i < r->font_slots_used; i++) {
		if (r->font_slots[i].tex == r->font_tex) r->font_tex = NULL;
		if (r->font_slots[i].tex) SDL_ReleaseGPUTexture(r->dev, r->font_slots[i].tex);
	}
	r->font_slots_used = 0;
	memset(r->font_slots, 0, sizeof(r->font_slots));
	r->font_tex = NULL;
	/* 单槽缓存也失效 */
	r->font_cache[0] = '\0';
}

/* 回收闲置超过 max_idle_frames 帧的缓存条目并释放其纹理。
   单位是渲染帧(joy_r3d_begin 推进的 font_cache_frame)。
   对一次性文本(如滚动的聊天记录)很有用: 它们用完即闲置, 周期性 sweep 会释放,
   避免缓存长期被历史文本占满、把固定 UI 文字挤出去。 */
void joy_r3d_font_cache_sweep(joy_r3d_p r, int max_idle_frames)
{
	int i;
	if (!r || max_idle_frames < 0) return;

	i = 0;
	while (i < r->font_slots_used) {
		if ((r->font_cache_frame - r->font_slots[i].last_frame) > (Uint64)max_idle_frames) {
			/* 若释放的正是单槽 font_tex 引用的纹理, 一并失效, 避免悬空 */
			if (r->font_slots[i].tex == r->font_tex) {
				r->font_tex = NULL;
				r->font_cache[0] = '\0';
			}
			if (r->font_slots[i].tex) SDL_ReleaseGPUTexture(r->dev, r->font_slots[i].tex);
			/* 用末尾元素覆盖, 保持数组紧凑 */
			r->font_slots[i] = r->font_slots[r->font_slots_used - 1];
			r->font_slots_used--;
			/* 不递增 i: 换过来的末尾元素也需要检查 */
		} else {
			i++;
		}
	}
	/* 若全部清空, 单槽 font_tex 也一并失效, 避免悬空 */
	if (r->font_slots_used == 0 && r->font_tex) {
		r->font_tex = NULL;
		r->font_cache[0] = '\0';
	}
}

/* 世界坐标投影到屏幕像素坐标(原点左上, Y 向下)。
   用 viewProj 矩阵变换后做透视除法, 再映射到窗口像素。
   返回 false 表示点在相机后面(clip.w <= 0), 调用方应跳过绘制。 */
bool joy_r3d_world_to_screen(joy_r3d_p r, joy_vec3_t world, float* screenX, float* screenY)
{
	if (!r || !screenX || !screenY) return false;

	/* clip = viewProj * vec4(world, 1)  (列主序) */
	float cx = r->viewProj.m0 * world.x + r->viewProj.m4 * world.y + r->viewProj.m8  * world.z + r->viewProj.m12;
	float cy = r->viewProj.m1 * world.x + r->viewProj.m5 * world.y + r->viewProj.m9  * world.z + r->viewProj.m13;
	float cw = r->viewProj.m3 * world.x + r->viewProj.m7 * world.y + r->viewProj.m11 * world.z + r->viewProj.m15;

	if (cw <= 0.0f) return false;

	float ndcX = cx / cw;
	float ndcY = cy / cw;

	*screenX = (ndcX + 1.0f) * 0.5f * (float)r->last_ww;
	*screenY = (1.0f - ndcY) * 0.5f * (float)r->last_wh;
	return true;
}

/* ================================================================
 * 2D UI 逻辑分辨率(仿 jr2d logical presentation, 窗口缩放自适应)
 * ================================================================ */
void joy_r3d_set_logical_presentation(joy_r3d_p r, int w, int h)
{
	if (!r) return;
	r->logical_w = w;
	r->logical_h = h;
	r->logical_mode = JOY_LOGICAL_PRESENTATION_STRETCH;
}

/* 设置逻辑分辨率 + 呈现模式(对标 jr2d 的 joy_r2d_set_logical_presentation_mode)。
   STRETCH:   分别按 x/y 拉伸铺满(旧行为, 非 3:2 屏圆会变形);
   LETTERBOX: 等比缩放并居中, 圆/文字在任意屏宽比下都不拉伸(推荐移动端使用)。 */
void joy_r3d_set_logical_presentation_mode(joy_r3d_p r, int w, int h, int mode)
{
	if (!r) return;
	r->logical_w = w;
	r->logical_h = h;
	r->logical_mode = mode;
}

void joy_r3d_get_logical_presentation(joy_r3d_p r, int* w, int* h)
{
	if (!r) return;
	if (w) *w = r->logical_w;
	if (h) *h = r->logical_h;
}

void joy_r3d_window_to_logical(joy_r3d_p r, float wx, float wy, float* lx, float* ly)
{
	if (!r) return;
	if (lx) *lx = (r->ui_sx > 0) ? (wx - r->ui_offx) / r->ui_sx : wx;
	if (ly) *ly = (r->ui_sy > 0) ? (wy - r->ui_offy) / r->ui_sy : wy;
}

void joy_r3d_logical_to_window(joy_r3d_p r, float lx, float ly, float* wx, float* wy)
{
	if (!r) return;
	if (wx) *wx = lx * r->ui_sx + r->ui_offx;
	if (wy) *wy = ly * r->ui_sy + r->ui_offy;
}

/* 内部: 窗口像素坐标 -> 逻辑坐标(供各 2D UI 命中测试函数使用, 含 letterbox 居中偏移) */
static void r3d_ui_to_logical(joy_r3d_p r, float wx, float wy, float* lx, float* ly)
{
	if (lx) { if (r->ui_sx > 0) *lx = (wx - r->ui_offx) / r->ui_sx; else *lx = wx; }
	if (ly) { if (r->ui_sy > 0) *ly = (wy - r->ui_offy) / r->ui_sy; else *ly = wy; }
}

/* ================================================================
 * 2D UI: button (仿 jr2d, 用屏幕正交投影渲染, 不随相机变化)
 * ================================================================ */

/* 内部: 在屏幕像素坐标绘制一个实心矩形(左上角 x,y, 尺寸 w,h) */
static void draw_rect_2d(joy_r3d_p r, joy_render_mesh_p m,
						 float x, float y, float w, float h,
						 Uint8 cr, Uint8 cg, Uint8 cb,
						 float winW, float winH)
{
	if (!r || !m || w <= 0 || h <= 0 || winW <= 0 || winH <= 0) return;

	joy_mat44_t model; memset(&model, 0, sizeof(model));
	model.m0  = w;
	model.m5  = h;
	model.m10 = 1.0f;
	model.m12 = x + w * 0.5f;
	model.m13 = y + h * 0.5f;
	model.m15 = 1.0f;

	joy_mat44_t proj; memset(&proj, 0, sizeof(proj));
	/* 2D 正交投影: 逻辑坐标 -> 窗口像素(含 letterbox 居中偏移), 由 joy_r3d_begin 的 ui_sx/ui_sy/ui_offx/ui_offy 决定 */
	if (r->logical_mode == JOY_LOGICAL_PRESENTATION_LETTERBOX) {
		proj.m0  =  2.0f * r->ui_sx / (float)r->ww;
		proj.m5  = -2.0f * r->ui_sy / (float)r->wh;
		proj.m10 =  1.0f;
		proj.m12 =  2.0f * r->ui_offx / (float)r->ww - 1.0f;
		proj.m13 =  1.0f - 2.0f * r->ui_offy / (float)r->wh;
		proj.m15 =  1.0f;
	} else {
		proj.m0  =  2.0f / winW;
		proj.m5  = -2.0f / winH;
		proj.m10 =  1.0f;
		proj.m12 = -1.0f;
		proj.m13 =  1.0f;
		proj.m15 =  1.0f;
	}

	SDL_GPUTexture* tex = get_color_texture(r, cr, cg, cb, 255);
	draw_mesh_internal_proj(r, m, &model, &proj, tex);
}

/* 公开的 2D 实心矩形绘制(逻辑/窗口像素坐标, 由调用方保证与 winW/winH 一致) */
void joy_r3d_draw_rect_2d(joy_r3d_p r, joy_render_mesh_p m,
					  float x, float y, float w, float h,
					  Uint8 cr, Uint8 cg, Uint8 cb,
					  float winW, float winH)
{
	draw_rect_2d(r, m, x, y, w, h, cr, cg, cb, winW, winH);
}

/* 在屏幕(逻辑)坐标绘制自定义实心色 mesh(2D 正交, 原点左上, Y 向下)。
   cx/cy: mesh 中心逻辑坐标; sx/sy: 缩放(逻辑像素);
   用于环形/扇形/拱形等非矩形图元(如按钮倒计时环形圈)。
   与 joy_r3d_draw_rect_2d 使用同一套 proj 映射, 保证与按钮/文字坐标对齐。 */
void joy_r3d_draw_mesh_2d(joy_r3d_p r, joy_render_mesh_p m,
					  float cx, float cy, float sx, float sy,
					  Uint8 cr, Uint8 cg, Uint8 cb,
					  float winW, float winH)
{
	if (!r || !m || winW <= 0 || winH <= 0) return;

	joy_mat44_t model; memset(&model, 0, sizeof(model));
	model.m0  = sx;
	model.m5  = sy;
	model.m10 = 1.0f;
	model.m12 = cx;
	model.m13 = cy;
	model.m15 = 1.0f;

	joy_mat44_t proj; memset(&proj, 0, sizeof(proj));
	/* 2D 正交投影: 逻辑坐标 -> 窗口像素(含 letterbox 居中偏移), 由 joy_r3d_begin 的 ui_sx/ui_sy/ui_offx/ui_offy 决定 */
	if (r->logical_mode == JOY_LOGICAL_PRESENTATION_LETTERBOX) {
		proj.m0  =  2.0f * r->ui_sx / (float)r->ww;
		proj.m5  = -2.0f * r->ui_sy / (float)r->wh;
		proj.m10 =  1.0f;
		proj.m12 =  2.0f * r->ui_offx / (float)r->ww - 1.0f;
		proj.m13 =  1.0f - 2.0f * r->ui_offy / (float)r->wh;
		proj.m15 =  1.0f;
	} else {
		proj.m0  =  2.0f / winW;
		proj.m5  = -2.0f / winH;
		proj.m10 =  1.0f;
		proj.m12 = -1.0f;
		proj.m13 =  1.0f;
		proj.m15 =  1.0f;
	}

	SDL_GPUTexture* tex = get_color_texture(r, cr, cg, cb, 255);
	draw_mesh_internal_proj(r, m, &model, &proj, tex);
}

/* 在屏幕(逻辑)坐标绘制贴图 quad(2D 正交, 原点左上, Y 向下)。
   (x,y,w,h): 目标矩形(逻辑像素); tex: SDL_GPUTexture*(由 joy_r3d_load_texture_png 返回);
   单位 quad 自带 UV(0,0)-(1,1), 与 r3d_font_draw_2d_impl / joy_r3d_draw_mesh_2d 同一套 proj 映射,
   保证与按钮/文字/矩形坐标对齐。用于摇杆底纹/按钮图标等贴图 UI。 */
void joy_r3d_draw_tex_2d(joy_r3d_p r, joy_render_mesh_p m,
					 float x, float y, float w, float h,
					 void* tex, float winW, float winH)
{
	if (!r || !m || !tex || winW <= 0 || winH <= 0) return;

	joy_mat44_t model; memset(&model, 0, sizeof(model));
	model.m0  = w;
	model.m5  = h;
	model.m10 = 1.0f;
	model.m12 = x + w * 0.5f;
	model.m13 = y + h * 0.5f;
	model.m15 = 1.0f;

	joy_mat44_t proj; memset(&proj, 0, sizeof(proj));
	/* 2D 正交投影: 逻辑坐标 -> 窗口像素(含 letterbox 居中偏移), 由 joy_r3d_begin 的 ui_sx/ui_sy/ui_offx/ui_offy 决定 */
	if (r->logical_mode == JOY_LOGICAL_PRESENTATION_LETTERBOX) {
		proj.m0  =  2.0f * r->ui_sx / (float)r->ww;
		proj.m5  = -2.0f * r->ui_sy / (float)r->wh;
		proj.m10 =  1.0f;
		proj.m12 =  2.0f * r->ui_offx / (float)r->ww - 1.0f;
		proj.m13 =  1.0f - 2.0f * r->ui_offy / (float)r->wh;
		proj.m15 =  1.0f;
	} else {
		proj.m0  =  2.0f / winW;
		proj.m5  = -2.0f / winH;
		proj.m10 =  1.0f;
		proj.m12 = -1.0f;
		proj.m13 =  1.0f;
		proj.m15 =  1.0f;
	}

	draw_mesh_internal_proj(r, m, &model, &proj, (SDL_GPUTexture*)tex);
}

joy_r3d_button_p joy_r3d_button_create(float x, float y, float w, float h, const char* text)
{
	joy_r3d_button_p b = (joy_r3d_button_p)SDL_malloc(sizeof(joy_r3d_button_t));
	if (!b) return NULL;
	b->x = x; b->y = y; b->w = w; b->h = h;
	b->text[0] = '\0';
	if (text) SDL_strlcpy(b->text, text, sizeof(b->text));
	b->state = JR3D_BTN_NORMAL;
	b->pressedInside = false;
	b->onClick = NULL;
	return b;
}

void joy_r3d_button_set_onclick(joy_r3d_button_p btn, void (*cb)(joy_r3d_button_p))
{
	if (btn) btn->onClick = cb;
}

bool joy_r3d_button_is_hovered(joy_r3d_p r, const joy_r3d_button_p btn, float mx, float my)
{
	if (!btn) return false;
	float lx = mx, ly = my;
	r3d_ui_to_logical(r, mx, my, &lx, &ly);
	return (lx >= btn->x && lx <= btn->x + btn->w &&
			ly >= btn->y && ly <= btn->y + btn->h);
}

bool joy_r3d_button_is_pressed(joy_r3d_p r, joy_r3d_button_p btn, bool mouse_down, float mx, float my)
{
	if (!btn) return false;
	bool hover = joy_r3d_button_is_hovered(r, btn, mx, my);
	if (hover && mouse_down) {
		btn->state = JR3D_BTN_PRESSED;
		btn->pressedInside = true;
		return true;
	}
	btn->state = hover ? JR3D_BTN_HOVER : JR3D_BTN_NORMAL;
	return false;
}

void joy_r3d_button_release(joy_r3d_p r, joy_r3d_button_p btn, float mx, float my)
{
	if (!btn) return;
	bool hover = joy_r3d_button_is_hovered(r, btn, mx, my);
	/* 完整 click: 按下在按钮内 且 松开也在按钮内 -> 触发回调 */
	if (btn->pressedInside && hover && btn->onClick) {
		btn->onClick(btn);
	}
	btn->pressedInside = false;
	btn->state = hover ? JR3D_BTN_HOVER : JR3D_BTN_NORMAL;
}

void joy_r3d_button_draw(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_button_p btn, float winW, float winH)
{
	if (!r || !m || !btn || winW <= 0 || winH <= 0) return;

	/* 配色(仿 project_proxy2d 主菜单按钮): normal 白 / hover 浅灰 / pressed 灰, 黑边框 + 黑文字 */
	Uint8 fr = 255, fg = 255, fb = 255;
	if (btn->state == JR3D_BTN_HOVER)   { fr = 220; fg = 225; fb = 235; }
	if (btn->state == JR3D_BTN_PRESSED) { fr = 200; fg = 200; fb = 210; }

	/* 按钮坐标与 winW/winH 同为逻辑坐标(窗口缩放自适应由 joy_r3d_begin 的 ui_sx/ui_sy 决定,
	   绘制时直接以逻辑尺寸做正交投影, 无需再缩放矩形) */
	float x = btn->x, y = btn->y, w = btn->w, h = btn->h;

	/* 填充矩形 */
	draw_rect_2d(r, m, x, y, w, h, fr, fg, fb, winW, winH);
	/* 黑边框(4 条 2px 粗) */
	float bw = 2.0f;
	draw_rect_2d(r, m, x,                y,                w, bw,     0, 0, 0, winW, winH);
	draw_rect_2d(r, m, x,                y + h - bw,       w, bw,     0, 0, 0, winW, winH);
	draw_rect_2d(r, m, x,                y,                bw, h,     0, 0, 0, winW, winH);
	draw_rect_2d(r, m, x + w - bw,       y,                bw, h,     0, 0, 0, winW, winH);

	/* 按钮文字: 居中绘制 */
	if (r->font_ttf_buf && btn->text[0]) {
		float fontSize = h * 0.5f;                 // 字号约为按钮高度一半
		float aspect = (float)r->font_tex_w / (float)r->font_tex_h;
		/* 用最新烘焙的文字测宽(若缓存的不是本按钮文字, 先烘焙) */
		if (SDL_strcmp(r->font_cache, btn->text) != 0) {
			r3d_font_bake(r, btn->text);
		}
		float textW = fontSize * ((float)r->font_tex_w / (float)r->font_tex_h);
		float tx = x + (w - textW) * 0.5f;
		float ty = y + (h - fontSize) * 0.5f;
		joy_r3d_font_draw_2d_color(r, m, btn->text, tx, ty, fontSize, winW, winH, 0, 0, 0);
		(void)aspect;
	}
}

void joy_r3d_button_destroy(joy_r3d_button_p btn)
{
	if (btn) SDL_free(btn);
}

/* 单位圆盘 mesh(半径1), 供圆形按钮/圆环等 2D 绘制复用,
   按 r3d 实例惰性创建一次, 由 joy_r3d_destroy 释放 */
static joy_render_mesh_p r3d_unit_disc(joy_r3d_p r)
{
	if (!r) return NULL;
	if (r->unit_disc) return r->unit_disc;
	enum { SEGS = 40 };
	float rim[SEGS * 2];
	for (int i = 0; i < SEGS; i++) {
		float a = (float)i / SEGS * 6.2831853f;
		rim[i*2] = sinf(a); rim[i*2+1] = -cosf(a);   // 从正上方逆时针? 用 -cos 使顶点从上方起
	}
	float verts[SEGS * 3 * 5];
	int vi = 0;
	for (int i = 0; i < SEGS; i++) {
		int j = (i + 1) % SEGS;
		verts[vi++]=0; verts[vi++]=0; verts[vi++]=0; verts[vi++]=0; verts[vi++]=0;
		verts[vi++]=rim[i*2]; verts[vi++]=rim[i*2+1]; verts[vi++]=0; verts[vi++]=0; verts[vi++]=0;
		verts[vi++]=rim[j*2]; verts[vi++]=rim[j*2+1]; verts[vi++]=0; verts[vi++]=0; verts[vi++]=0;
	}
	r->unit_disc = joy_r3d_mesh_custom(r, verts, vi / 5);
	return r->unit_disc;
}

/* 圆形按钮: 深色描边圆 + 填充圆(圆形版 joy_r3d_button_draw), 文字处理同矩形按钮。
   击打区仍用 btn 矩形(64x64), 但视觉为直径=min(w,h) 的圆。 */
void joy_r3d_button_draw_circle(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_button_p btn, float winW, float winH)
{
	if (!r || !btn || winW <= 0 || winH <= 0) return;
	Uint8 fr = 255, fg = 255, fb = 255;
	if (btn->state == JR3D_BTN_HOVER)   { fr = 220; fg = 225; fb = 235; }
	if (btn->state == JR3D_BTN_PRESSED) { fr = 200; fg = 200; fb = 210; }
	joy_render_mesh_p disc = r3d_unit_disc(r);
	if (!disc) return;
	float cx = btn->x + btn->w * 0.5f, cy = btn->y + btn->h * 0.5f;
	float R = (btn->w < btn->h ? btn->w : btn->h) * 0.5f;
	joy_r3d_draw_mesh_2d(r, disc, cx, cy, R + 3.0f, R + 3.0f, 0, 0, 0, winW, winH); // 黑描边
	joy_r3d_draw_mesh_2d(r, disc, cx, cy, R, R, fr, fg, fb, winW, winH);            // 填充
	/* 文字居中(与 joy_r3d_button_draw 一致) */
	if (r->font_ttf_buf && btn->text[0]) {
		float fontSize = btn->h * 0.5f;
		if (SDL_strcmp(r->font_cache, btn->text) != 0) r3d_font_bake(r, btn->text);
		float textW = fontSize * ((float)r->font_tex_w / (float)r->font_tex_h);
		float tx = btn->x + (btn->w - textW) * 0.5f;
		float ty = btn->y + (btn->h - fontSize) * 0.5f;
		joy_r3d_font_draw_2d_color(r, m, btn->text, tx, ty, fontSize, winW, winH, 0, 0, 0);
	}
}

/* ================================================================
 * 2D UI: label (仿 jr2d, 纯文字 2D HUD, 固定屏幕位置, 不随相机变化)
 * ================================================================ */

/* label 类型已在 joy.h 中定义 */
joy_r3d_label_p joy_r3d_label_create(float x, float y, float fontSize, uint8_t cr, uint8_t cg, uint8_t cb, const char* text)
{
	joy_r3d_label_p l = (joy_r3d_label_p)SDL_malloc(sizeof(joy_r3d_label_t));
	if (!l) return NULL;
	l->x = x; l->y = y; l->fontSize = fontSize;
	l->r = cr; l->g = cg; l->b = cb;
	l->text[0] = '\0';
	if (text) SDL_strlcpy(l->text, text, sizeof(l->text));
	return l;
}

/* 更新文字内容(支持动态文本如 FPS) */
void joy_r3d_label_set_text(joy_r3d_label_p l, const char* text)
{
	if (l && text) SDL_strlcpy(l->text, text, sizeof(l->text));
}

void joy_r3d_label_draw(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_label_p l, float winW, float winH)
{
	if (!r || !m || !l || winW <= 0 || winH <= 0) return;
	if (!r->font_ttf_buf || !l->text[0]) return;
	/* 使用 label 创建时的颜色(白底 UI 需深色文字, 不能再固定白色) */
	joy_r3d_font_draw_2d_color(r, m, l->text, l->x, l->y, l->fontSize, winW, winH,
		l->r, l->g, l->b);
}

void joy_r3d_label_destroy(joy_r3d_label_p l)
{
	if (l) SDL_free(l);
}

/* ================================================================
 * 2D UI: input (仿 jr2d inputbox, 屏幕像素坐标, 不随相机变化)
 * ================================================================ */

/* 测量 str 前 byte_len 字节的像素宽度(UTF-8 安全; 用 stbtt 累加 advance, 不重建 GPU 纹理) */
static float r3d_font_measure(joy_r3d_p r, const char* str, int byte_len)
{
	if (!r->font_ttf_buf || !str || byte_len <= 0) return 0.0f;
	float w = 0.0f;
	int pos = 0;
	int32_t code;
	while (pos < byte_len) {
		size_t consumed = joy_core_utf8_decode(str + pos, (size_t)(byte_len - pos), &code);
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
	size_t consumed = joy_core_utf8_decode(s + p, (size_t)(len - p), &(int32_t){0});
	if (consumed == 0) return len;
	return p + (int)consumed;
}

joy_r3d_input_p joy_r3d_input_create(float x, float y, float w, float h, float fontSize,
                              uint8_t cr, uint8_t cg, uint8_t cb, const char* text)
{
	joy_r3d_input_p inp = (joy_r3d_input_p)SDL_malloc(sizeof(joy_r3d_input_t));
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

void joy_r3d_input_set_text(joy_r3d_input_p inp, const char* text)
{
	if (!inp) return;
	inp->text[0] = '\0';
	if (text) SDL_strlcpy(inp->text, text, sizeof(inp->text));
	inp->cursor_pos = (int)SDL_strlen(inp->text);
	inp->scroll = 0;
}

const char* joy_r3d_input_get_text(joy_r3d_input_p inp)
{
	return inp ? inp->text : "";
}

bool joy_r3d_input_is_hovered(joy_r3d_p r, const joy_r3d_input_p inp, float mx, float my)
{
	if (!inp) return false;
	float lx = mx, ly = my;
	r3d_ui_to_logical(r, mx, my, &lx, &ly);
	return (lx >= inp->x && lx <= inp->x + inp->w &&
			ly >= inp->y && ly <= inp->y + inp->h);
}

void joy_r3d_input_focus(joy_r3d_input_p inp)
{
	if (inp) { inp->focused = true; inp->cursor_visible = true; inp->cursor_timer = SDL_GetTicks(); }
}

void joy_r3d_input_start_text_input(joy_r3d_p r, joy_r3d_input_p inp)
{
	if (!r || !inp) return;
	if (inp->focused && r->win) SDL_StartTextInput(r->win);
}

void joy_r3d_input_stop_text_input(joy_r3d_p r, joy_r3d_input_p inp)
{
	if (!r || !inp) return;
	if (r->win) SDL_StopTextInput(r->win);
}

void joy_r3d_input_defocus(joy_r3d_input_p inp)
{
	if (inp) inp->focused = false;
}

bool joy_r3d_input_is_focused(const joy_r3d_input_p inp)
{
	return inp ? inp->focused : false;
}

void joy_r3d_input_insert(joy_r3d_input_p inp, const char* s)
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

void joy_r3d_input_backspace(joy_r3d_input_p inp)
{
	if (!inp || inp->cursor_pos <= 0) return;
	int start = r3d_input_prev_char(inp->text, inp->cursor_pos);
	int ch_len = inp->cursor_pos - start;
	size_t cur = SDL_strlen(inp->text);
	SDL_memmove(inp->text + start, inp->text + inp->cursor_pos, cur - inp->cursor_pos + 1);
	inp->cursor_pos = start;
}

void joy_r3d_input_delete(joy_r3d_input_p inp)
{
	if (!inp) return;
	size_t cur = SDL_strlen(inp->text);
	if (inp->cursor_pos >= (int)cur) return;
	int nxt = r3d_input_next_char(inp->text, inp->cursor_pos, (int)cur);
	SDL_memmove(inp->text + inp->cursor_pos, inp->text + nxt, cur - nxt + 1);
	/* cursor_pos 不变 */
}

void joy_r3d_input_cursor_left(joy_r3d_input_p inp)
{
	if (!inp || inp->cursor_pos <= 0) return;
	inp->cursor_pos = r3d_input_prev_char(inp->text, inp->cursor_pos);
}

void joy_r3d_input_cursor_right(joy_r3d_input_p inp)
{
	if (!inp) return;
	size_t cur = SDL_strlen(inp->text);
	if (inp->cursor_pos >= (int)cur) return;
	inp->cursor_pos = r3d_input_next_char(inp->text, inp->cursor_pos, (int)cur);
}

void joy_r3d_input_cursor_home(joy_r3d_input_p inp)
{
	if (inp) inp->cursor_pos = 0;
}

void joy_r3d_input_cursor_end(joy_r3d_input_p inp)
{
	if (inp) inp->cursor_pos = (int)SDL_strlen(inp->text);
}

void joy_r3d_input_click(joy_r3d_p r, joy_r3d_input_p inp, float mx, float my)
{
	if (!inp) return;
	float lx = mx, ly = my;
	r3d_ui_to_logical(r, mx, my, &lx, &ly);
	inp->focused = true;
	inp->cursor_timer = SDL_GetTicks();
	inp->cursor_visible = true;
	if (!joy_r3d_input_is_hovered(r, inp, mx, my)) return;

	float padX = 4.0f;
	/* 换算系数: 让 measure(bake 像素宽) 与 joy_r3d_font_draw_2d(整图按 fontSize 缩放) 的屏幕像素宽一致。
	   draw_2d 中 1 个 bake 像素 = fontSize / font_tex_h 个屏幕像素。 */
	float fontSize = inp->fontSize > 0 ? inp->fontSize : (inp->h * 0.6f);
	float k = r->font_tex_h > 0 ? (fontSize / (float)r->font_tex_h) : 1.0f;

	/* 点击位置对应文本屏幕像素(已含 scroll, 单位与 draw 一致) */
	float px = (lx - (inp->x + padX)) + (float)inp->scroll * k;

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

void joy_r3d_input_set_onsubmit(joy_r3d_input_p inp, void (*cb)(joy_r3d_input_p))
{
	if (inp) inp->on_submit = cb;
}

void joy_r3d_input_submit(joy_r3d_input_p inp)
{
	if (inp && inp->on_submit) inp->on_submit(inp);
}

/* ===================== 选中 / 编辑辅助 (对齐 jr2d inputbox) ===================== */

static void r3d_input_clear_sel(joy_r3d_input_p inp)
{
	if (!inp) return;
	inp->sel_start = inp->sel_end = -1;
}

static bool r3d_input_has_sel(const joy_r3d_input_p inp)
{
	return inp && inp->sel_start >= 0 && inp->sel_end >= 0 && inp->sel_start != inp->sel_end;
}

/* 删除选中文本, 光标置于选中起点 */
static void r3d_input_delete_sel(joy_r3d_input_p inp)
{
	if (!inp || !r3d_input_has_sel(inp)) return;
	int a = inp->sel_start, b = inp->sel_end;
	if (a > b) { int t = a; a = b; b = t; }
	int cur = (int)SDL_strlen(inp->text);
	SDL_memmove(inp->text + a, inp->text + b, cur - b + 1);
	inp->cursor_pos = a;
	r3d_input_clear_sel(inp);
}

/* 文本屏幕像素宽度(用与 joy_r3d_font_draw_2d 一致的缩放) */
static float r3d_input_text_w(joy_r3d_p r, joy_r3d_input_p inp, const char* s, int n)
{
	float fontSize = inp->fontSize > 0 ? inp->fontSize : (inp->h * 0.6f);
	float k = r->font_tex_h > 0 ? (fontSize / (float)r->font_tex_h) : 1.0f;
	return r3d_font_measure(r, s, n) * k;
}

/* 滚动到光标可见 (对齐 inputbox_scroll_to_cursor) */
static void r3d_input_scroll_to_cursor(joy_r3d_p r, joy_r3d_input_p inp)
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

/* 在 cursor_pos 处插入 (对齐 joy_ui_inputbox_insert, 自动删除选中) */
static void r3d_input_insert_at(joy_r3d_p r, joy_r3d_input_p inp, const char* s, int len)
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

static void r3d_input_move_cursor(joy_r3d_input_p inp, int pos)
{
	inp->cursor_pos = pos;
	r3d_input_clear_sel(inp);
}

static void r3d_input_update_sel(joy_r3d_input_p inp)
{
	if (inp->sel_start < 0) inp->sel_start = inp->cursor_pos;
	inp->sel_end = inp->cursor_pos;
}

void joy_r3d_input_select_left(joy_r3d_input_p inp)
{
	if (!inp || inp->cursor_pos <= 0) return;
	inp->cursor_pos = r3d_input_prev_char(inp->text, inp->cursor_pos);
	r3d_input_update_sel(inp);
}

void joy_r3d_input_select_right(joy_r3d_input_p inp)
{
	int cur = (int)SDL_strlen(inp->text);
	if (!inp || inp->cursor_pos >= cur) return;
	inp->cursor_pos = r3d_input_next_char(inp->text, inp->cursor_pos, cur);
	r3d_input_update_sel(inp);
}

void joy_r3d_input_select_home(joy_r3d_input_p inp)
{
	if (!inp) return;
	r3d_input_update_sel(inp);
	inp->cursor_pos = 0;
	inp->sel_end = 0;
}

void joy_r3d_input_select_end(joy_r3d_input_p inp)
{
	if (!inp) return;
	r3d_input_update_sel(inp);
	inp->cursor_pos = (int)SDL_strlen(inp->text);
	inp->sel_end = inp->cursor_pos;
}

void joy_r3d_input_select_all(joy_r3d_input_p inp)
{
	if (!inp) return;
	inp->sel_start = 0;
	inp->sel_end = (int)SDL_strlen(inp->text);
	inp->cursor_pos = inp->sel_end;
}

void joy_r3d_input_paste(joy_r3d_input_p inp)
{
	if (!inp) return;
	char* clip = SDL_GetClipboardText();
	if (clip) { r3d_input_insert_at(NULL, inp, clip, (int)SDL_strlen(clip)); SDL_free(clip); }
}

void joy_r3d_input_copy(joy_r3d_input_p inp)
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

void joy_r3d_input_cut(joy_r3d_input_p inp)
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

/* ===================== 统一事件处理 (对齐 jr2d joy_ui_inputbox_process_event) ===================== */
/* 事件参数在公共头里是 const void*；SDL_Event 只在平台适配代码内强转,
 * 这样 include/joy/ 不再需要 SDL_events.h。 */
bool joy_r3d_input_handle_event(joy_r3d_p r, joy_r3d_input_p inp, const void* ev)
{
	const SDL_Event* event = (const SDL_Event*)ev;
	if (!r || !inp || !event) return false;

	if (event->type == SDL_EVENT_MOUSE_MOTION) {
		/* 仅更新 hover 状态(选中拖拽扩展可选, 此处保持简单) */
		joy_r3d_input_is_hovered(r, inp, event->motion.x, event->motion.y);
		return false;
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (event->button.button != SDL_BUTTON_LEFT)
			return false;
		if (joy_r3d_input_is_hovered(r, inp, event->button.x, event->button.y)) {
			/* 聚焦 + 点击定位光标 + 启动文本输入(幂等, 重复调用无害) */
			inp->focused = true;
			inp->cursor_visible = true;
			inp->cursor_timer = SDL_GetTicks();
			joy_r3d_input_click(r, inp, event->button.x, event->button.y);
			if (r->win) SDL_StartTextInput(r->win);
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
		case SDL_SCANCODE_BACKSPACE: joy_r3d_input_backspace(inp); break;
		case SDL_SCANCODE_DELETE:    joy_r3d_input_delete(inp); break;
		case SDL_SCANCODE_LEFT:
			if (mod & SDL_KMOD_SHIFT) joy_r3d_input_select_left(inp);
			else joy_r3d_input_cursor_left(inp);
			break;
		case SDL_SCANCODE_RIGHT:
			if (mod & SDL_KMOD_SHIFT) joy_r3d_input_select_right(inp);
			else joy_r3d_input_cursor_right(inp);
			break;
		case SDL_SCANCODE_HOME:
			if (mod & SDL_KMOD_SHIFT) joy_r3d_input_select_home(inp);
			else joy_r3d_input_cursor_home(inp);
			break;
		case SDL_SCANCODE_END:
			if (mod & SDL_KMOD_SHIFT) joy_r3d_input_select_end(inp);
			else joy_r3d_input_cursor_end(inp);
			break;
		case SDL_SCANCODE_A:
			if (mod & SDL_KMOD_CTRL) joy_r3d_input_select_all(inp);
			break;
		case SDL_SCANCODE_V:
			if (mod & SDL_KMOD_CTRL) joy_r3d_input_paste(inp);
			break;
		case SDL_SCANCODE_C:
			if (mod & SDL_KMOD_CTRL) joy_r3d_input_copy(inp);
			break;
		case SDL_SCANCODE_X:
			if (mod & SDL_KMOD_CTRL) joy_r3d_input_cut(inp);
			break;
		case SDL_SCANCODE_RETURN:
		case SDL_SCANCODE_KP_ENTER:
			joy_r3d_input_submit(inp);
			break;
		default: break;
		}
		r3d_input_scroll_to_cursor(r, inp);
		return true;   /* 消费键盘事件, 不传给相机 */
	}
	return false;
}

void joy_r3d_input_draw(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_input_p inp, float winW, float winH)
{
	if (!r || !m || !inp || winW <= 0 || winH <= 0) return;

	/* 背景: 白底 UI, 输入框用近白; 聚焦时略偏蓝 */
	Uint8 br = 250, bg = 250, bb = 250;
	if (inp->focused) { br = 244; bg = 246; bb = 252; }
	draw_rect_2d(r, m, inp->x, inp->y, inp->w, inp->h, br, bg, bb, winW, winH);

	/* 边框: 2px; 聚焦时蓝, 否则深灰 */
	float bw = 2.0f;
	Uint8 bor_r = inp->focused ? 60 : 90;
	Uint8 bor_g = inp->focused ? 120 : 90;
	Uint8 bor_b = inp->focused ? 220 : 100;
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

	/* 换算系数: measure(bake 像素宽) -> 屏幕像素宽, 与 joy_r3d_font_draw_2d 的整图缩放一致。
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
		joy_r3d_font_draw_2d_color(r, m, inp->text + vis_start, draw_x, ty, fontSize, winW, winH,
			inp->r, inp->g, inp->b);
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
				draw_rect_2d(r, m, caret_x, ty, 2.0f, fontSize, 60, 60, 70, winW, winH);
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

void joy_r3d_input_destroy(joy_r3d_input_p inp)
{
	if (inp) SDL_free(inp);
}

/* ================================================================
 * 2D UI: combobox_input (可输入下拉列表框, 仿 jr2d combobox_input)
 * ================================================================ */

joy_r3d_combobox_input_p joy_r3d_combobox_input_create(float x, float y, float w, float h,
	float fontSize, uint8_t cr, uint8_t cg, uint8_t cb, const char* text)
{
	joy_r3d_combobox_input_p ci = (joy_r3d_combobox_input_p)SDL_calloc(1, sizeof(joy_r3d_combobox_input_t));
	if (!ci) return NULL;
	ci->x = x; ci->y = y; ci->w = w; ci->h = h;
	ci->fontSize = fontSize;
	ci->arrow_w = 20.0f;
	ci->item_height = 24;
	ci->selected_index = -1;
	ci->is_open = false;
	ci->item_count = 0;
	ci->on_change = NULL;
	/* 内部输入框: 宽度减去箭头区域 */
	ci->input = joy_r3d_input_create(x, y, w - ci->arrow_w, h, fontSize, cr, cg, cb, text);
	return ci;
}

void joy_r3d_combobox_input_destroy(joy_r3d_combobox_input_p ci)
{
	if (!ci) return;
	if (ci->input) joy_r3d_input_destroy(ci->input);
	SDL_free(ci);
}

void joy_r3d_combobox_input_add_item(joy_r3d_combobox_input_p ci, const char* str)
{
	if (!ci || !str || ci->item_count >= 20) return;
	SDL_strlcpy(ci->items[ci->item_count], str, JOY_MAX_IP);
	ci->item_count++;
}

void joy_r3d_combobox_input_clear_items(joy_r3d_combobox_input_p ci)
{
	if (ci) ci->item_count = 0;
}

const char* joy_r3d_combobox_input_get_text(joy_r3d_combobox_input_p ci)
{
	return (ci && ci->input) ? joy_r3d_input_get_text(ci->input) : "";
}

void joy_r3d_combobox_input_set_text(joy_r3d_combobox_input_p ci, const char* str)
{
	if (ci && ci->input) joy_r3d_input_set_text(ci->input, str);
}

int joy_r3d_combobox_input_hit_item(joy_r3d_combobox_input_p ci, float x, float y)
{
	if (!ci || !ci->is_open) return -1;
	for (int i = 0; i < ci->item_count; i++) {
		float iy = ci->y + ci->h + i * ci->item_height;
		if (x >= ci->x && x <= ci->x + ci->w &&
			y >= iy && y <= iy + ci->item_height)
			return i;
	}
	return -1;
}

bool joy_r3d_combobox_input_handle_event(joy_r3d_p r, joy_r3d_combobox_input_p ci, const void* ev)
{
	const SDL_Event* event = (const SDL_Event*)ev;
	if (!r || !ci || !event) return false;

	if (event->type == SDL_EVENT_MOUSE_MOTION) {
		float mx = event->motion.x, my = event->motion.y;
		float lx = mx, ly = my;
		r3d_ui_to_logical(r, mx, my, &lx, &ly);
		float ax = ci->x + ci->w - ci->arrow_w;
		ci->arrow_hovered = (lx >= ax && lx <= ax + ci->arrow_w &&
							 ly >= ci->y && ly <= ci->y + ci->h);
		return false;
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (event->button.button != SDL_BUTTON_LEFT) return false;
		float mx = event->button.x, my = event->button.y;
		float lx = mx, ly = my;
		r3d_ui_to_logical(r, mx, my, &lx, &ly);
		float ax = ci->x + ci->w - ci->arrow_w;
		bool in_arrow = (lx >= ax && lx <= ax + ci->arrow_w &&
						 ly >= ci->y && ly <= ci->y + ci->h);
		bool in_rect = (lx >= ci->x && lx <= ci->x + ci->w &&
						ly >= ci->y && ly <= ci->y + ci->h);

		if (in_arrow) {
			ci->is_open = !ci->is_open;
			if (ci->input) joy_r3d_input_focus(ci->input);
			return true;
		}
		else if (ci->is_open) {
			int hit = joy_r3d_combobox_input_hit_item(ci, lx, ly);
			if (hit >= 0) {
				joy_r3d_combobox_input_set_text(ci, ci->items[hit]);
				ci->selected_index = hit;
				ci->is_open = false;
				if (ci->on_change) ci->on_change(ci);
				return true;
			}
		}
		if (in_rect) {
			if (ci->input) joy_r3d_input_click(r, ci->input, mx, my);
			ci->is_open = false;
			return true;
		}
		else {
			ci->is_open = false;
			if (ci->input) joy_r3d_input_defocus(ci->input);
			return false;
		}
	}
	else if (event->type == SDL_EVENT_TEXT_INPUT) {
		if (ci->input && joy_r3d_input_is_focused(ci->input)) {
			bool consumed = joy_r3d_input_handle_event(r, ci->input, event);
			if (ci->on_change) ci->on_change(ci);
			return consumed;
		}
		return false;
	}
	else if (event->type == SDL_EVENT_KEY_DOWN) {
		if (!ci->input || !joy_r3d_input_is_focused(ci->input)) return false;
		if (event->key.scancode == SDL_SCANCODE_DOWN) {
			if (ci->is_open && ci->selected_index < ci->item_count - 1) {
				ci->selected_index++;
				return true;
			}
		}
		else if (event->key.scancode == SDL_SCANCODE_UP) {
			if (ci->is_open && ci->selected_index > 0) {
				ci->selected_index--;
				return true;
			}
		}
		else if (event->key.scancode == SDL_SCANCODE_RETURN ||
				 event->key.scancode == SDL_SCANCODE_KP_ENTER) {
			if (ci->is_open && ci->selected_index >= 0 && ci->selected_index < ci->item_count) {
				joy_r3d_combobox_input_set_text(ci, ci->items[ci->selected_index]);
				ci->is_open = false;
				if (ci->on_change) ci->on_change(ci);
				return true;
			}
		}
		return joy_r3d_input_handle_event(r, ci->input, event);
	}
	return false;
}

void joy_r3d_combobox_input_draw(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_combobox_input_p ci, float winW, float winH)
{
	if (!r || !m || !ci || winW <= 0 || winH <= 0) return;

	/* 内部输入框 */
	if (ci->input) joy_r3d_input_draw(r, m, ci->input, winW, winH);

	/* 箭头区域背景(白底 UI) */
	float ax = ci->x + ci->w - ci->arrow_w;
	Uint8 ar = 245, ag = 245, ab = 245;
	if (ci->arrow_hovered) { ar = 232; ag = 232; ab = 238; }
	draw_rect_2d(r, m, ax, ci->y, ci->arrow_w, ci->h, ar, ag, ab, winW, winH);

	/* 箭头分隔线 */
	draw_rect_2d(r, m, ax, ci->y + 2, 1.0f, ci->h - 4, 90, 90, 100, winW, winH);

	/* 三角箭头: 用矩形条带近似实心三角形(不依赖字体符号, 避免编码/字形问题) */
	{
		float acx = ax + ci->arrow_w * 0.5f;   /* 箭头水平中心 */
		float acy = ci->y + ci->h * 0.5f;      /* 箭头垂直中心 */
		if (ci->is_open) {
			/* 上箭头 ▲: 顶部宽、底部窄 */
			draw_rect_2d(r, m, acx - 5.0f, acy - 4.0f, 10.0f, 2.0f, 60, 60, 70, winW, winH);
			draw_rect_2d(r, m, acx - 3.0f, acy - 2.0f,  6.0f, 2.0f, 60, 60, 70, winW, winH);
			draw_rect_2d(r, m, acx - 1.0f, acy,         2.0f, 2.0f, 60, 60, 70, winW, winH);
		} else {
			/* 下箭头 ▼: 顶部窄、底部宽 */
			draw_rect_2d(r, m, acx - 1.0f, acy - 2.0f,  2.0f, 2.0f, 60, 60, 70, winW, winH);
			draw_rect_2d(r, m, acx - 3.0f, acy,         6.0f, 2.0f, 60, 60, 70, winW, winH);
			draw_rect_2d(r, m, acx - 5.0f, acy + 2.0f, 10.0f, 2.0f, 60, 60, 70, winW, winH);
		}
	}
}

/* 仅绘制展开的下拉列表。供场景在渲染最后调用, 保证列表绘制在最上层,
   不被位于其下方的其他控件(如后续的输入框/按钮)遮挡。 */
void joy_r3d_combobox_input_draw_dropdown(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_combobox_input_p ci, float winW, float winH)
{
	if (!r || !m || !ci || winW <= 0 || winH <= 0) return;
	if (!ci->is_open || ci->item_count <= 0) return;

	float drop_h = ci->item_count * ci->item_height;
	/* 背景(白底 UI) */
	draw_rect_2d(r, m, ci->x, ci->y + ci->h, ci->w, drop_h, 250, 250, 250, winW, winH);
	/* 边框 */
	float bw = 1.0f;
	draw_rect_2d(r, m, ci->x, ci->y + ci->h, ci->w, bw, 90, 90, 100, winW, winH);
	draw_rect_2d(r, m, ci->x, ci->y + ci->h + drop_h - bw, ci->w, bw, 90, 90, 100, winW, winH);
	draw_rect_2d(r, m, ci->x, ci->y + ci->h, bw, drop_h, 90, 90, 100, winW, winH);
	draw_rect_2d(r, m, ci->x + ci->w - bw, ci->y + ci->h, bw, drop_h, 90, 90, 100, winW, winH);

	for (int i = 0; i < ci->item_count; i++) {
		float iy = ci->y + ci->h + i * ci->item_height;
		if (i == ci->selected_index) {
			draw_rect_2d(r, m, ci->x + 1, iy, ci->w - 2, ci->item_height, 70, 110, 200, winW, winH);
		}
		if (r->font_ttf_buf && ci->items[i][0]) {
			joy_r3d_font_draw_2d_color(r, m, ci->items[i], ci->x + 4, iy + 3, ci->fontSize, winW, winH,
				50, 50, 60);
		}
	}
}

/* ================================================================
 * 2D UI: slider (仿 jr2d slider, 屏幕像素坐标, 不随相机变化)
 * ================================================================ */
static float r3d_slider_thumb_x(joy_r3d_slider_p s)
{
	float range = s->max - s->min;
	if (range <= 0) return s->x;
	float ratio = (s->value - s->min) / range;
	return s->x + ratio * s->w;
}

static void r3d_slider_update_value_from_x(joy_r3d_slider_p s, float mx)
{
	float ratio = (mx - s->x) / s->w;
	float v = s->min + ratio * (s->max - s->min);
	if (v < s->min) v = s->min;
	if (v > s->max) v = s->max;
	s->value = v;
}

static bool r3d_slider_thumb_hit(joy_r3d_slider_p s, float mx, float my)
{
	float tx = r3d_slider_thumb_x(s);
	float half = s->thumb_width * 0.5f;
	if (tx - half < s->x) tx = s->x + half;
	if (tx + half > s->x + s->w) tx = s->x + s->w - half;
	return mx >= tx - half && mx <= tx + half
		&& my >= s->y && my <= s->y + s->h;
}

joy_r3d_slider_p joy_r3d_slider_create(float x, float y, float w, float h, float min, float max, float initial)
{
	joy_r3d_slider_p s = (joy_r3d_slider_p)SDL_malloc(sizeof(joy_r3d_slider_t));
	if (!s) return NULL;
	s->x = x; s->y = y; s->w = w; s->h = h;
	s->min = min; s->max = max;
	s->value = initial;
	if (s->value < min) s->value = min;
	if (s->value > max) s->value = max;
	s->thumb_width = h;
	s->is_dragging = false;
	s->hovered = false;
	s->track_color = (joy_color_t){ 80, 80, 80, 255 };
	s->fill_color = (joy_color_t){ 66, 133, 244, 255 };
	s->thumb_color = (joy_color_t){ 200, 200, 200, 255 };
	s->thumb_hover_color = (joy_color_t){ 240, 240, 240, 255 };
	return s;
}

void joy_r3d_slider_destroy(joy_r3d_slider_p s)
{
	if (s) SDL_free(s);
}

void joy_r3d_slider_set_value(joy_r3d_slider_p s, float v)
{
	if (!s) return;
	if (v < s->min) v = s->min;
	if (v > s->max) v = s->max;
	s->value = v;
}

float joy_r3d_slider_get_value(joy_r3d_slider_p s)
{
	return s ? s->value : 0;
}

void joy_r3d_slider_set_colors(joy_r3d_slider_p s, joy_color_t track, joy_color_t fill, joy_color_t thumb, joy_color_t thumb_hover)
{
	if (!s) return;
	s->track_color = track;
	s->fill_color = fill;
	s->thumb_color = thumb;
	s->thumb_hover_color = thumb_hover;
}

void joy_r3d_slider_draw(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_slider_p s, float winW, float winH)
{
	if (!r || !m || !s || winW <= 0 || winH <= 0) return;

	/* 滑轨背景 */
	draw_rect_2d(r, m, s->x, s->y, s->w, s->h,
		s->track_color.r, s->track_color.g, s->track_color.b, winW, winH);

	/* 已填充部分(从 track.x 到滑块中心) */
	float tx = r3d_slider_thumb_x(s);
	float fill_w = tx - s->x;
	if (fill_w > 0) {
		draw_rect_2d(r, m, s->x, s->y, fill_w, s->h,
			s->fill_color.r, s->fill_color.g, s->fill_color.b, winW, winH);
	}

	/* 滑块(悬停时亮色) */
	float half = s->thumb_width * 0.5f;
	float sx = tx - half;
	if (sx < s->x) sx = s->x;
	if (sx + s->thumb_width > s->x + s->w) sx = s->x + s->w - s->thumb_width;
	joy_color_t tc = s->hovered ? s->thumb_hover_color : s->thumb_color;
	draw_rect_2d(r, m, sx, s->y, s->thumb_width, s->h,
		tc.r, tc.g, tc.b, winW, winH);
	/* 滑块黑边框 */
	float bw = 1.0f;
	draw_rect_2d(r, m, sx, s->y, s->thumb_width, bw, 0, 0, 0, winW, winH);
	draw_rect_2d(r, m, sx, s->y + s->h - bw, s->thumb_width, bw, 0, 0, 0, winW, winH);
	draw_rect_2d(r, m, sx, s->y, bw, s->h, 0, 0, 0, winW, winH);
	draw_rect_2d(r, m, sx + s->thumb_width - bw, s->y, bw, s->h, 0, 0, 0, winW, winH);
}

bool joy_r3d_slider_is_pressed(joy_r3d_p r, joy_r3d_slider_p s, float x, float y)
{
	if (!s) return false;
	float lx = x, ly = y;
	r3d_ui_to_logical(r, x, y, &lx, &ly);
	if (r3d_slider_thumb_hit(s, lx, ly)) {
		s->is_dragging = true;
		return true;
	}
	/* 点击滑轨时跳转到该位置 */
	if (lx >= s->x && lx <= s->x + s->w
		&& ly >= s->y && ly <= s->y + s->h) {
		r3d_slider_update_value_from_x(s, lx);
		s->is_dragging = true;
		return true;
	}
	return false;
}

void joy_r3d_slider_set_pressed(joy_r3d_slider_p s, bool v)
{
	if (s) s->is_dragging = v;
}

void joy_r3d_slider_drag_move(joy_r3d_p r, joy_r3d_slider_p s, float x, float y)
{
	(void)y;
	if (!s || !s->is_dragging) return;
	float lx = x;
	r3d_ui_to_logical(r, x, 0, &lx, NULL);
	r3d_slider_update_value_from_x(s, lx);
}

bool joy_r3d_slider_is_hovered(joy_r3d_p r, joy_r3d_slider_p s, float x, float y)
{
	if (!s) return false;
	float lx = x, ly = y;
	r3d_ui_to_logical(r, x, y, &lx, &ly);
	s->hovered = r3d_slider_thumb_hit(s, lx, ly);
	return s->hovered;
}

void joy_r3d_slider_set_hovered(joy_r3d_slider_p s, bool v)
{
	if (s) s->hovered = v;
}

/* ================================================================
 * 2D UI: checkbox (仿 jr2d checkbox, 屏幕像素坐标, 不随相机变化)
 * ================================================================ */
/* 用小方块点阵画粗线段(近似斜线, 兼容仅 axis-aligned 矩形的 2D 绘制) */
static void draw_line_dots(joy_r3d_p r, joy_render_mesh_p m, float x0, float y0, float x1, float y1,
	float thickness, Uint8 cr, Uint8 cg, Uint8 cb, float winW, float winH)
{
	float dx = x1 - x0, dy = y1 - y0;
	float len = sqrtf(dx * dx + dy * dy);
	if (len <= 0) return;
	int steps = (int)(len / thickness) + 1;
	for (int i = 0; i <= steps; i++) {
		float t = (float)i / steps;
		float px = x0 + dx * t - thickness * 0.5f;
		float py = y0 + dy * t - thickness * 0.5f;
		draw_rect_2d(r, m, px, py, thickness, thickness, cr, cg, cb, winW, winH);
	}
}

joy_r3d_checkbox_p joy_r3d_checkbox_create(float x, float y, float size, const char* label)
{
	joy_r3d_checkbox_p cb = (joy_r3d_checkbox_p)SDL_malloc(sizeof(joy_r3d_checkbox_t));
	if (!cb) return NULL;
	cb->x = x; cb->y = y; cb->size = size;
	cb->state = JOY_R3D_CHECKBOX_UNCHECKED;
	cb->hovered = false;
	cb->enabled = true;
	cb->label = label ? joy_core_utils_strdup(label, NULL) : NULL;
	cb->label_w = label ? (float)strlen(label) * 8.0f : 0.0f;
	cb->box_color = (joy_color_t){ 0, 0, 0, 255 };
	cb->check_color = (joy_color_t){ 0, 120, 0, 255 };
	cb->text_color = (joy_color_t){ 200, 200, 200, 255 };
	cb->hover_color = (joy_color_t){ 200, 200, 200, 255 };
	return cb;
}

void joy_r3d_checkbox_destroy(joy_r3d_checkbox_p cb)
{
	if (!cb) return;
	/* label 由 joy_core_utils_strdup(malloc) 分配, 必须用 free 释放(与 jui.c 一致), 避免跨堆(SDL_free 走 SDL3.dll 的 CRT 堆) */
	if (cb->label) free(cb->label);
	SDL_free(cb);
}

void joy_r3d_checkbox_set_state(joy_r3d_checkbox_p cb, joy_r3d_checkbox_state_t state)
{
	if (cb) cb->state = state;
}

joy_r3d_checkbox_state_t joy_r3d_checkbox_get_state(joy_r3d_checkbox_p cb)
{
	return cb ? cb->state : JOY_R3D_CHECKBOX_UNCHECKED;
}

void joy_r3d_checkbox_toggle(joy_r3d_checkbox_p cb)
{
	if (!cb || !cb->enabled) return;
	cb->state = (cb->state == JOY_R3D_CHECKBOX_UNCHECKED)
		? JOY_R3D_CHECKBOX_CHECKED : JOY_R3D_CHECKBOX_UNCHECKED;
}

bool joy_r3d_checkbox_is_pressed(joy_r3d_p r, joy_r3d_checkbox_p cb, float x, float y)
{
	if (!cb || !cb->enabled) return false;
	float lx = x, ly = y;
	r3d_ui_to_logical(r, x, y, &lx, &ly);
	float hit_w = cb->size + 5.0f + cb->label_w;
	if (lx >= cb->x && lx <= cb->x + hit_w
		&& ly >= cb->y && ly <= cb->y + cb->size) {
		joy_r3d_checkbox_toggle(cb);
		return true;
	}
	return false;
}

void joy_r3d_checkbox_set_hovered(joy_r3d_checkbox_p cb, bool v)
{
	if (cb) cb->hovered = v;
}

bool joy_r3d_checkbox_is_hovered(joy_r3d_p r, joy_r3d_checkbox_p cb, float x, float y)
{
	if (!cb) return false;
	float lx = x, ly = y;
	r3d_ui_to_logical(r, x, y, &lx, &ly);
	float hit_w = cb->size + 5.0f + cb->label_w;
	cb->hovered = (lx >= cb->x && lx <= cb->x + hit_w
		&& ly >= cb->y && ly <= cb->y + cb->size);
	return cb->hovered;
}

void joy_r3d_checkbox_set_colors(joy_r3d_checkbox_p cb, joy_color_t box, joy_color_t check, joy_color_t hover)
{
	if (!cb) return;
	cb->box_color = box;
	cb->check_color = check;
	cb->hover_color = hover;
}

void joy_r3d_checkbox_draw(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_checkbox_p cb, float winW, float winH)
{
	if (!r || !m || !cb || winW <= 0 || winH <= 0) return;

	float cx = cb->x, cy = cb->y, sz = cb->size;
	Uint8 alpha = cb->enabled ? 255 : 100;

	/* 框体背景 */
	draw_rect_2d(r, m, cx, cy, sz, sz,
		cb->box_color.r, cb->box_color.g, cb->box_color.b, winW, winH);

	/* 悬停变色 */
	if (cb->hovered && cb->enabled) {
		draw_rect_2d(r, m, cx, cy, sz, sz,
			cb->hover_color.r, cb->hover_color.g, cb->hover_color.b, winW, winH);
	}

	/* 深灰边框 */
	draw_rect_2d(r, m, cx, cy, sz, 1.0f, 60, 60, 70, winW, winH);
	draw_rect_2d(r, m, cx, cy + sz - 1.0f, sz, 1.0f, 60, 60, 70, winW, winH);
	draw_rect_2d(r, m, cx, cy, 1.0f, sz, 60, 60, 70, winW, winH);
	draw_rect_2d(r, m, cx + sz - 1.0f, cy, 1.0f, sz, 60, 60, 70, winW, winH);

	/* 勾选状态: 用点阵画勾(仿 jr2d 的 3 点折线) */
	if (cb->state == JOY_R3D_CHECKBOX_CHECKED && cb->enabled) {
		float p = sz * 0.18f;
		float mh = sz * 0.5f;
		float e = sz - p;
		float h1 = sz * 0.38f;
		float h2 = sz - p;
		float thick = sz * 0.16f;
		draw_line_dots(r, m, cx + p, cy + mh, cx + h1, cy + h2, thick,
			cb->check_color.r, cb->check_color.g, cb->check_color.b, winW, winH);
		draw_line_dots(r, m, cx + h1, cy + h2, cx + e, cy + p, thick,
			cb->check_color.r, cb->check_color.g, cb->check_color.b, winW, winH);
	}
	else if (cb->state == JOY_R3D_CHECKBOX_INDETERMINATE && cb->enabled) {
		float p = sz * 0.25f;
		draw_line_dots(r, m, cx + p, cy + sz * 0.5f, cx + sz - p, cy + sz * 0.5f, sz * 0.16f,
			60, 60, 70, winW, winH);
	}

	/* 标签文本 */
	if (cb->label && cb->label[0] && r->font_ttf_buf) {
		joy_r3d_font_draw_2d_color(r, m, cb->label, cx + sz + 5.0f, cy + 1.0f, sz * 0.8f, winW, winH,
			50, 50, 60);
	}
}

/* 单位平面 quad: 4 顶点(FLOAT3 pos + FLOAT2 uv) + 6 索引(0,1,2,0,3,2), 范围 x/y ∈ [-0.5,0.5]
   UV: (0,0)左上 (1,1)右下, 与 stbi flip=true 后的纹理朝向一致 */
joy_render_mesh_p joy_r3d_mesh_quad(joy_r3d_p r)
{
	static const float verts[4 * 5] = {
		/* x,    y,    z,   u,  v */
		-0.5f, -0.5f, 0,  0, 1,   /* 左下 */
		 0.5f, -0.5f, 0,  1, 1,   /* 右下 */
		 0.5f,  0.5f, 0,  1, 0,   /* 右上 */
		-0.5f,  0.5f, 0,  0, 0,   /* 左上 */
	};
	static const Uint32 indices[6] = { 0, 1, 2, 0, 2, 3 };

	joy_render_mesh_p m = r3d_mesh_alloc(r);
	if (!m) return NULL;
	m->buffer = (SDL_GPUBuffer*)joy_r3d_upload_buffer(r->dev, verts, sizeof(verts));
	m->index_buffer = (SDL_GPUBuffer*)joy_r3d_upload_buffer(r->dev, indices, sizeof(indices));
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

/* ---- 线框网格（pos FLOAT3 + uv FLOAT2, 5 float/顶点, LINELIST） ---- */

joy_render_mesh_p joy_r3d_mesh_cube_wire(joy_r3d_p r, float w, float h, float d)
{
	float hw = w * 0.5f, hh = h * 0.5f, hd = d * 0.5f;
	float verts[24 * 5];
	int vi = 0;
#define WIRE_V(PX,PY,PZ) do { \
		verts[vi++]=(PX); verts[vi++]=(PY); verts[vi++]=(PZ); \
		verts[vi++]=0;    verts[vi++]=0; \
	} while (0)
	/* 底面 4 条边 */
	WIRE_V(-hw,-hh,-hd); WIRE_V( hw,-hh,-hd);
	WIRE_V( hw,-hh,-hd); WIRE_V( hw,-hh, hd);
	WIRE_V( hw,-hh, hd); WIRE_V(-hw,-hh, hd);
	WIRE_V(-hw,-hh, hd); WIRE_V(-hw,-hh,-hd);
	/* 顶面 4 条边 */
	WIRE_V(-hw, hh,-hd); WIRE_V( hw, hh,-hd);
	WIRE_V( hw, hh,-hd); WIRE_V( hw, hh, hd);
	WIRE_V( hw, hh, hd); WIRE_V(-hw, hh, hd);
	WIRE_V(-hw, hh, hd); WIRE_V(-hw, hh,-hd);
	/* 4 条竖边 */
	WIRE_V(-hw,-hh,-hd); WIRE_V(-hw, hh,-hd);
	WIRE_V( hw,-hh,-hd); WIRE_V( hw, hh,-hd);
	WIRE_V( hw,-hh, hd); WIRE_V( hw, hh, hd);
	WIRE_V(-hw,-hh, hd); WIRE_V(-hw, hh, hd);
#undef WIRE_V

	joy_render_mesh_p m = r3d_mesh_alloc(r);
	if (!m) return NULL;
	m->buffer = (SDL_GPUBuffer*)joy_r3d_upload_buffer(r->dev, verts, sizeof(verts));
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = 24;
	return m;
}

joy_render_mesh_p joy_r3d_mesh_sphere_wire(joy_r3d_p r, float radius, int slices, int stacks)
{
	/* 纬度环 (stacks-1) * slices * 2 + 经度线 slices * stacks * 2 */
	int lat_verts = (stacks - 1) * slices * 2;
	int lon_verts = slices * stacks * 2;
	int total = lat_verts + lon_verts;
	float* verts = (float*)SDL_malloc((size_t)total * 5 * sizeof(float));
	if (!verts) return NULL;
	int vi = 0;

	/* 纬度环: 在每个 phi 高度画一圈线段 */
	for (int i = 1; i < stacks; i++) {
		float phi = -90.0f + (float)i * 180.0f / stacks;
		float rad = radius * cosf(phi * (float)JOY_PI / 180.0f);
		float y   = radius * sinf(phi * (float)JOY_PI / 180.0f);
		for (int j = 0; j < slices; j++) {
			float t1 = (float)j       * 360.0f / slices * (float)JOY_PI / 180.0f;
			float t2 = (float)(j + 1) * 360.0f / slices * (float)JOY_PI / 180.0f;
			verts[vi++] = rad * cosf(t1); verts[vi++] = y; verts[vi++] = rad * sinf(t1); verts[vi++] = 0; verts[vi++] = 0;
			verts[vi++] = rad * cosf(t2); verts[vi++] = y; verts[vi++] = rad * sinf(t2); verts[vi++] = 0; verts[vi++] = 0;
		}
	}
	/* 经度线: 从北极到南极 */
	for (int j = 0; j < slices; j++) {
		float theta = (float)j * 360.0f / slices * (float)JOY_PI / 180.0f;
		for (int i = 0; i < stacks; i++) {
			float p1 = -90.0f + (float)i       * 180.0f / stacks;
			float p2 = -90.0f + (float)(i + 1) * 180.0f / stacks;
			float r1 = radius * cosf(p1 * (float)JOY_PI / 180.0f);
			float r2 = radius * cosf(p2 * (float)JOY_PI / 180.0f);
			float y1 = radius * sinf(p1 * (float)JOY_PI / 180.0f);
			float y2 = radius * sinf(p2 * (float)JOY_PI / 180.0f);
			verts[vi++] = r1 * cosf(theta); verts[vi++] = y1; verts[vi++] = r1 * sinf(theta); verts[vi++] = 0; verts[vi++] = 0;
			verts[vi++] = r2 * cosf(theta); verts[vi++] = y2; verts[vi++] = r2 * sinf(theta); verts[vi++] = 0; verts[vi++] = 0;
		}
	}

	joy_render_mesh_p m = r3d_mesh_alloc(r);
	if (!m) { SDL_free(verts); return NULL; }
	m->buffer = (SDL_GPUBuffer*)joy_r3d_upload_buffer(r->dev, verts, (size_t)total * 5 * sizeof(float));
	SDL_free(verts);
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = total;
	return m;
}

joy_render_mesh_p joy_r3d_mesh_plane_wire(joy_r3d_p r, float w, float d, int segs)
{
	/* (segs+1) 条 X 方向线 + (segs+1) 条 Z 方向线, 每条 2 顶点 */
	int total = (segs + 1) * 2 * 2;
	float* verts = (float*)SDL_malloc((size_t)total * 5 * sizeof(float));
	if (!verts) return NULL;
	int vi = 0;
	float hw = w * 0.5f, hd = d * 0.5f;
	float step_x = w / segs, step_z = d / segs;

	for (int i = 0; i <= segs; i++) {
		float z = -hd + i * step_z;
		verts[vi++] = -hw; verts[vi++] = 0; verts[vi++] = z; verts[vi++] = 0; verts[vi++] = 0;
		verts[vi++] =  hw; verts[vi++] = 0; verts[vi++] = z; verts[vi++] = 0; verts[vi++] = 0;
	}
	for (int i = 0; i <= segs; i++) {
		float x = -hw + i * step_x;
		verts[vi++] = x; verts[vi++] = 0; verts[vi++] = -hd; verts[vi++] = 0; verts[vi++] = 0;
		verts[vi++] = x; verts[vi++] = 0; verts[vi++] =  hd; verts[vi++] = 0; verts[vi++] = 0;
	}

	joy_render_mesh_p m = r3d_mesh_alloc(r);
	if (!m) { SDL_free(verts); return NULL; }
	m->buffer = (SDL_GPUBuffer*)joy_r3d_upload_buffer(r->dev, verts, (size_t)total * 5 * sizeof(float));
	SDL_free(verts);
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = total;
	return m;
}

/* 斜坡线框: 与 joy_r3d_mesh_ramp(w,l,h) 同几何（三角棱柱, 坡面沿 Z 上升） */
joy_render_mesh_p joy_r3d_mesh_ramp_wire(joy_r3d_p r, float w, float l, float h)
{
	float hw = w * 0.5f, hl = l * 0.5f;
	float verts[9 * 2 * 5];
	int vi = 0;
#define RAMP_WIRE_V(PX,PY,PZ) do { \
		verts[vi++]=(PX); verts[vi++]=(PY); verts[vi++]=(PZ); \
		verts[vi++]=0;    verts[vi++]=0; \
	} while (0)
	/* 9 条棱: A(-hw,0,-hl) B(hw,0,-hl) C(-hw,h,hl) D(hw,h,hl) E(-hw,0,hl) F(hw,0,hl) */
	RAMP_WIRE_V(-hw,0,-hl); RAMP_WIRE_V( hw,0,-hl);  /* A-B 底前 */
	RAMP_WIRE_V( hw,0,-hl); RAMP_WIRE_V( hw,h, hl);  /* B-D 右坡 */
	RAMP_WIRE_V( hw,h, hl); RAMP_WIRE_V(-hw,h, hl);  /* D-C 顶后 */
	RAMP_WIRE_V(-hw,h, hl); RAMP_WIRE_V(-hw,0,-hl);  /* C-A 左坡 */
	RAMP_WIRE_V(-hw,0,-hl); RAMP_WIRE_V(-hw,0, hl);  /* A-E 底左 */
	RAMP_WIRE_V(-hw,0, hl); RAMP_WIRE_V( hw,0, hl);  /* E-F 底后 */
	RAMP_WIRE_V( hw,0, hl); RAMP_WIRE_V( hw,0,-hl);  /* F-B 底右 */
	RAMP_WIRE_V(-hw,0, hl); RAMP_WIRE_V(-hw,h, hl);  /* E-C 左竖 */
	RAMP_WIRE_V( hw,0, hl); RAMP_WIRE_V( hw,h, hl);  /* F-D 右竖 */
#undef RAMP_WIRE_V

	joy_render_mesh_p m = r3d_mesh_alloc(r);
	if (!m) return NULL;
	m->buffer = (SDL_GPUBuffer*)joy_r3d_upload_buffer(r->dev, verts, sizeof(verts));
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = 18;
	return m;
}

/* ---- 光照网格（pos FLOAT3 + normal FLOAT3 + uv FLOAT2, 8 float/顶点, 无索引） ---- */

joy_render_mesh_p joy_r3d_mesh_cube_lit(joy_r3d_p r, float w, float h, float d)
{
	float hw = w * 0.5f, hh = h * 0.5f, hd = d * 0.5f;
	float verts[36 * 8];
	joy_render_mesh_p m;
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
	m = r3d_mesh_alloc(r);
	if (!m) return NULL;
	m->buffer = (SDL_GPUBuffer*)joy_r3d_upload_buffer(r->dev, verts, sizeof(verts));
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = 36;
	return m;
}

static void r3d_flush_pending(joy_r3d_p r)
{
	if (!r || r->pending_count == 0) return;
	for (int i = 0; i < r->pending_count; i++)
		SDL_ReleaseGPUBuffer(r->dev, r->pending_buffers[i]);
	r->pending_count = 0;
}

void joy_r3d_mesh_destroy(joy_r3d_p r, joy_render_mesh_p m)
{
	if (!r || !m) return;
	/* 延迟释放 GPU buffer: 推入队列, 待 joy_r3d_end 后统一释放,
	   避免释放仍被 GPU 引用的 buffer 导致 VULKAN 崩溃 */
	if (m->buffer) {
		if (r->pending_count >= r->pending_cap) {
			int nc = r->pending_cap ? r->pending_cap * 2 : 16;
			SDL_GPUBuffer** nb = (SDL_GPUBuffer**)SDL_realloc(r->pending_buffers,
				(size_t)nc * sizeof(SDL_GPUBuffer*));
			if (nb) { r->pending_buffers = nb; r->pending_cap = nc; }
		}
		if (r->pending_count < r->pending_cap)
			r->pending_buffers[r->pending_count++] = m->buffer;
	}
	/* index_buffer 同样可能仍被 GPU 引用, 一并延迟释放 */
	if (m->index_buffer) {
		if (r->pending_count < r->pending_cap)
			r->pending_buffers[r->pending_count++] = m->index_buffer;
	}
	SDL_free(m);
}

/* 生成链路专用：joy_r3d_mesh_destroy 的单参数版本，供 IDL 的 owns 使用。
 * render_mesh 内部记了创建它的 r3d_p（owner），所以 dtor 只需一个参数。
 * 约束：mesh 句柄不得比创建它的 r3d 上下文活得久，否则 owner 悬空。 */
void joy_r3d_mesh_destroy_one(joy_render_mesh_p m)
{
	if (!m) return;
	joy_r3d_mesh_destroy(m->owner, m);
}

joy_render_mesh_p joy_r3d_mesh_sphere_lit(joy_r3d_p r, float radius, int slices, int stacks)
{
	int stack, slice, vcount, vi;
	float* verts;
	joy_render_mesh_p m;
	if (slices < 3) slices = 3;
	if (stacks < 2) stacks = 2;
	vcount = slices * stacks * 6;
	verts = (float*)SDL_malloc((size_t)vcount * 8 * sizeof(float));
	if (!verts) return NULL;
	vi = 0;
	for (stack = 0; stack < stacks; stack++) {
		float phi0 = (float)JOY_PI * (float)stack / (float)stacks;
		float phi1 = (float)JOY_PI * (float)(stack + 1) / (float)stacks;
		for (slice = 0; slice < slices; slice++) {
			float th0 = 2.0f * (float)JOY_PI * (float)slice / (float)slices;
			float th1 = 2.0f * (float)JOY_PI * (float)(slice + 1) / (float)slices;
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
	m = r3d_mesh_alloc(r);
	if (!m) { SDL_free(verts); return NULL; }
	m->buffer = (SDL_GPUBuffer*)joy_r3d_upload_buffer(r->dev, verts, (size_t)vcount * 8 * sizeof(float));
	SDL_free(verts);
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = vcount;
	return m;
}

joy_render_mesh_p joy_r3d_mesh_plane_lit(joy_r3d_p r, float w, float d, int segs)
{
	int x, z, vcount, vi;
	float* verts;
	joy_render_mesh_p m;
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
	m = r3d_mesh_alloc(r);
	if (!m) { SDL_free(verts); return NULL; }
	m->buffer = (SDL_GPUBuffer*)joy_r3d_upload_buffer(r->dev, verts, (size_t)vcount * 8 * sizeof(float));
	SDL_free(verts);
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = vcount;
	return m;
}

joy_render_mesh_p joy_r3d_mesh_ramp_lit(joy_r3d_p r, float w, float l, float h)
{
	/* 三角棱柱斜坡: 底在 y=0, 沿 +Z 从 0 升到 h */
	float L = sqrtf(l * l + h * h);
	float sn = h / L, cn = l / L;   /* 坡面法线 (0, sn, -cn) */
	float verts[30 * 8];   /* 5 面 × 6 顶点 = 30 顶点, 每顶点 8 float */
	joy_render_mesh_p m;
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
	m = r3d_mesh_alloc(r);
	if (!m) return NULL;
	m->buffer = (SDL_GPUBuffer*)joy_r3d_upload_buffer(r->dev, verts, sizeof(verts));
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = 30;
	return m;
}

joy_render_mesh_p joy_r3d_mesh_custom_lit(joy_r3d_p r, const void* verts, int vertex_count)
{
	joy_render_mesh_p m;
	m = r3d_mesh_alloc(r);
	if (!m) return NULL;
	m->buffer = (SDL_GPUBuffer*)joy_r3d_upload_buffer(r->dev, verts, (size_t)vertex_count * 8 * sizeof(float));
	if (!m->buffer) { SDL_free(m); return NULL; }
	m->vertex_count = vertex_count;
	return m;
}

/* ---- 内部：用 model 矩阵和纹理绘制一个网格 ---- */

static void draw_mesh_internal(joy_r3d_p r, joy_render_mesh_p m, joy_mat44_t* model, SDL_GPUTexture* tex)
{
	draw_mesh_internal_proj(r, m, model, &r->viewProj, tex);
}

/* 内部: 用自定义投影矩阵(而非相机 viewProj)绘制, 用于 2D/UI 正交绘制 */
static void draw_mesh_internal_proj(joy_r3d_p r, joy_render_mesh_p m, joy_mat44_t* model, joy_mat44_t* proj, SDL_GPUTexture* tex)
{
	joy_render_mvp_t mvp;
	SDL_GPUTextureSamplerBinding samp;
	SDL_GPUBufferBinding bind;
	if (!r->rp || !m || !m->buffer) return;

	memset(&mvp, 0, sizeof(mvp));
	joy_r3d_mat44_to_float16(proj, mvp.viewProj);
	joy_r3d_mat44_to_float16(model, mvp.model);
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
	r->draw_calls++;
}

/* ---- 公开绘制 API ---- */

void joy_r3d_draw_mesh(joy_r3d_p r, joy_render_mesh_p m, joy_vec3_t pos, joy_vec3_t scale, joy_color_t color)
{
	joy_mat44_t model;
	SDL_GPUTexture* tex;
	memset(&model, 0, sizeof(model));
	model.m0 = scale.x;  model.m5 = scale.y;  model.m10 = scale.z;
	model.m12 = pos.x;   model.m13 = pos.y;   model.m14 = pos.z;
	model.m15 = 1;
	tex = get_color_texture(r, color.r, color.g, color.b, 255);
	draw_mesh_internal(r, m, &model, tex);
}

void joy_r3d_draw_mesh_tex(joy_r3d_p r, joy_render_mesh_p m, joy_vec3_t pos, joy_vec3_t scale, void* tex)
{
	joy_mat44_t model = { 0 };
	model.m0 = scale.x;  model.m5 = scale.y;  model.m10 = scale.z;
	model.m12 = pos.x;   model.m13 = pos.y;   model.m14 = pos.z;
	model.m15 = 1;
	draw_mesh_internal(r, m, &model, (SDL_GPUTexture*)tex);
}

/* billboard 绘制: 单位 quad 经 right/up 基向量旋转 + size 缩放 + center 平移 */
void joy_r3d_draw_mesh_tex_basis(joy_r3d_p r, joy_render_mesh_p m,
	joy_vec3_t center, joy_vec3_t right, joy_vec3_t up, float size, void* tex)
{
	if (!r || !m) return;
	joy_mat44_t model = { 0 };
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

/* ---- 线框绘制（需在 joy_r3d_begin/joy_r3d_end 之间调用） ---- */

void joy_r3d_draw_wire(joy_r3d_p r, joy_render_mesh_p m, joy_vec3_t pos, joy_vec3_t scale, joy_color_t color)
{
	joy_mat44_t model;
	SDL_GPUTexture* tex;
	if (!r || !r->rp || !r->line_pipeline || !m) return;

	memset(&model, 0, sizeof(model));
	model.m0 = scale.x;  model.m5 = scale.y;  model.m10 = scale.z;
	model.m12 = pos.x;   model.m13 = pos.y;   model.m14 = pos.z;
	model.m15 = 1;
	tex = get_color_texture(r, color.r, color.g, color.b, 255);

	SDL_BindGPUGraphicsPipeline(r->rp, r->line_pipeline);
	draw_mesh_internal(r, m, &model, tex);
	SDL_BindGPUGraphicsPipeline(r->rp, r->pipeline);
}

void joy_r3d_draw_wire_basis(joy_r3d_p r, joy_render_mesh_p m, const joy_mat44_t* model, joy_color_t color)
{
	SDL_GPUTexture* tex;
	if (!r || !r->rp || !r->line_pipeline || !m) return;

	tex = get_color_texture(r, color.r, color.g, color.b, 255);

	SDL_BindGPUGraphicsPipeline(r->rp, r->line_pipeline);
	draw_mesh_internal(r, m, (joy_mat44_t*)model, tex);
	SDL_BindGPUGraphicsPipeline(r->rp, r->pipeline);
}

/* ---- 实体填充绘制（写深度, 供"填充+描边"画风使用; 需在 joy_r3d_begin/joy_r3d_end 之间调用） ---- */

void joy_r3d_set_clear_color(joy_r3d_p r, uint8_t cr, uint8_t cg, uint8_t cb)
{
	if (!r) return;
	r->clear_r = cr; r->clear_g = cg; r->clear_b = cb;
}

void joy_r3d_draw_mesh_solid(joy_r3d_p r, joy_render_mesh_p m, joy_vec3_t pos, joy_vec3_t scale, joy_color_t color)
{
	joy_mat44_t model;
	SDL_GPUTexture* tex;
	if (!r || !r->rp || !r->solid_pipeline || !m) return;

	memset(&model, 0, sizeof(model));
	model.m0 = scale.x;  model.m5 = scale.y;  model.m10 = scale.z;
	model.m12 = pos.x;   model.m13 = pos.y;   model.m14 = pos.z;
	model.m15 = 1;
	tex = get_color_texture(r, color.r, color.g, color.b, 255);

	SDL_BindGPUGraphicsPipeline(r->rp, r->solid_pipeline);
	draw_mesh_internal(r, m, &model, tex);
	SDL_BindGPUGraphicsPipeline(r->rp, r->pipeline);
}

void joy_r3d_draw_mesh_solid_basis(joy_r3d_p r, joy_render_mesh_p m, const joy_mat44_t* model, joy_color_t color)
{
	SDL_GPUTexture* tex;
	if (!r || !r->rp || !r->solid_pipeline || !m) return;

	tex = get_color_texture(r, color.r, color.g, color.b, color.a);

	SDL_BindGPUGraphicsPipeline(r->rp, r->solid_pipeline);
	draw_mesh_internal(r, m, (joy_mat44_t*)model, tex);
	SDL_BindGPUGraphicsPipeline(r->rp, r->pipeline);
}

/* ================================================================
 * FOV 视野(战争迷雾): 逐角射线遮挡检测 + 羽化软边网格绘制
 * 通用引擎能力, 不依赖具体游戏数据; 遮挡矩形与观察点由调用方提供
 * ================================================================ */

#define JOY_FOV_DEG2RAD 0.0174532925f

float joy_r3d_fov_ray_dist(const joy_fov_rect* blocks, int n,
                       float px, float pz, float dx, float dz)
{
	float best = 1e9f;
	if (blocks == NULL || n <= 0) return best;
	for (int i = 0; i < n; i++) {
		const joy_fov_rect* a = &blocks[i];
		float tn = 0.0f, tf = 1e9f;
		if (fabsf(dx) < 1e-9f) {
			if (px < a->x - a->hx || px > a->x + a->hx) continue;
		} else {
			float t1 = (a->x - a->hx - px) / dx, t2 = (a->x + a->hx - px) / dx;
			if (t1 > t2) { float tt = t1; t1 = t2; t2 = tt; }
			if (t1 > tn) tn = t1;
			if (t2 < tf) tf = t2;
		}
		if (fabsf(dz) < 1e-9f) {
			if (pz < a->z - a->hz || pz > a->z + a->hz) continue;
		} else {
			float t1 = (a->z - a->hz - pz) / dz, t2 = (a->z + a->hz - pz) / dz;
			if (t1 > t2) { float tt = t1; t1 = t2; t2 = tt; }
			if (t1 > tn) tn = t1;
			if (t2 < tf) tf = t2;
		}
		if (tf < tn) continue;
		float t = tn >= 0.0f ? tn : tf;
		if (t > 1e-4f && t < best) best = t;
	}
	return best;
}

bool joy_r3d_fov_compute_radii(const joy_fov_rect* blocks, int n,
                           float px, float pz, float vis_radius,
                           float angle_start, float angle_step, int seg,
                           float* out)
{
	if (out == NULL || seg <= 0) return false;
	for (int i = 0; i < seg; i++) {
		float a = (angle_start + angle_step * i) * JOY_FOV_DEG2RAD;
		float dx = sinf(a), dz = cosf(a);
		float rd = joy_r3d_fov_ray_dist(blocks, n, px, pz, dx, dz);
		out[i] = (rd < vis_radius) ? rd : vis_radius;
	}
	return true;
}

/* 渐变纹理: v=0 透明 → v=1 全浓, 每帧复用采样以实现软边。
   按 device 实例惰性创建, 由 joy_r3d_destroy 释放 */
static void* joy_r3d_fov_gradient_texture(joy_r3d_p r)
{
	if (!r) return NULL;
	if (r->fov_gradient_tex) return r->fov_gradient_tex;
	const int H = 256;
	unsigned char* px = (unsigned char*)SDL_malloc((size_t)H * 4);
	if (!px) return NULL;
	for (int y = 0; y < H; y++) {
		unsigned char a = (unsigned char)(205.0f * (float)y / (float)(H - 1) + 0.5f);
		px[(size_t)y * 4 + 0] = 54; px[(size_t)y * 4 + 1] = 56;
		px[(size_t)y * 4 + 2] = 62; px[(size_t)y * 4 + 3] = a;
	}
	r->fov_gradient_tex = (SDL_GPUTexture*)joy_r3d_create_texture(r, 1, H, px, 0);
	SDL_free(px);
	return r->fov_gradient_tex;
}

void joy_r3d_fov_draw(joy_r3d_p r, float px, float py, float pz,
                  const float* radii, int seg,
                  float feather, float outer_radius)
{
	if (!r || !r->rp || !radii || seg <= 0) return;

	const float STEP = 360.0f / seg;
	const int vcount = seg * 4 * 3;                 /* 每段 4 三角 × 3 顶点 */
	float* verts = (float*)SDL_malloc((size_t)vcount * 5 * sizeof(float));
	if (!verts) return;
	int vi = 0;
	for (int i = 0; i < seg; i++) {
		int ni = (i + 1) % seg;
		float a0 = STEP * i * JOY_FOV_DEG2RAD, a1 = STEP * (ni) * JOY_FOV_DEG2RAD;
		float s0x = sinf(a0), c0z = cosf(a0);
		float s1x = sinf(a1), c1z = cosf(a1);
		float rA0 = radii[i],       rB0 = radii[i] + feather, rC0 = outer_radius;
		float rA1 = radii[ni],      rB1 = radii[ni] + feather, rC1 = outer_radius;
		float a0x = px + s0x*rA0, a0z = pz + c0z*rA0;
		float b0x = px + s0x*rB0, b0z = pz + c0z*rB0;
		float c0x = px + s0x*rC0, w0z = pz + c0z*rC0;
		float a1x = px + s1x*rA1, a1z = pz + c1z*rA1;
		float b1x = px + s1x*rB1, b1z = pz + c1z*rB1;
		float c1x = px + s1x*rC1, w1z = pz + c1z*rC1;

		/* 羽化带 A→B: alpha 0→1 渐变 */
		verts[vi]=a0x; verts[vi+1]=py; verts[vi+2]=a0z; verts[vi+3]=0.5f; verts[vi+4]=0.0f; vi+=5;
		verts[vi]=b0x; verts[vi+1]=py; verts[vi+2]=b0z; verts[vi+3]=0.5f; verts[vi+4]=1.0f; vi+=5;
		verts[vi]=b1x; verts[vi+1]=py; verts[vi+2]=b1z; verts[vi+3]=0.5f; verts[vi+4]=1.0f; vi+=5;
		verts[vi]=a0x; verts[vi+1]=py; verts[vi+2]=a0z; verts[vi+3]=0.5f; verts[vi+4]=0.0f; vi+=5;
		verts[vi]=b1x; verts[vi+1]=py; verts[vi+2]=b1z; verts[vi+3]=0.5f; verts[vi+4]=1.0f; vi+=5;
		verts[vi]=a1x; verts[vi+1]=py; verts[vi+2]=a1z; verts[vi+3]=0.5f; verts[vi+4]=0.0f; vi+=5;
		/* 实心带 B→C: 全浓 */
		verts[vi]=b0x; verts[vi+1]=py; verts[vi+2]=b0z; verts[vi+3]=0.5f; verts[vi+4]=1.0f; vi+=5;
		verts[vi]=c0x; verts[vi+1]=py; verts[vi+2]=w0z; verts[vi+3]=0.5f; verts[vi+4]=1.0f; vi+=5;
		verts[vi]=c1x; verts[vi+1]=py; verts[vi+2]=w1z; verts[vi+3]=0.5f; verts[vi+4]=1.0f; vi+=5;
		verts[vi]=b0x; verts[vi+1]=py; verts[vi+2]=b0z; verts[vi+3]=0.5f; verts[vi+4]=1.0f; vi+=5;
		verts[vi]=c1x; verts[vi+1]=py; verts[vi+2]=w1z; verts[vi+3]=0.5f; verts[vi+4]=1.0f; vi+=5;
		verts[vi]=b1x; verts[vi+1]=py; verts[vi+2]=b1z; verts[vi+3]=0.5f; verts[vi+4]=1.0f; vi+=5;
	}

	joy_render_mesh_p mesh = joy_r3d_mesh_custom(r, verts, vi / 5);
	SDL_free(verts);
	if (!mesh) return;

	/* 上一帧 mesh 在本帧绘制后延迟释放, 避免 GPU buffer 在被引用时销毁悬空。
	   按实例保存, 否则多个 r3d 会互相销毁对方持有的 mesh */
	if (r->fov_prev_mesh) { joy_r3d_mesh_destroy(r, r->fov_prev_mesh); r->fov_prev_mesh = NULL; }
	r->fov_prev_mesh = mesh;

	void* tex = joy_r3d_fov_gradient_texture(r);
	if (!tex) return;
	/* 主动绑定 base pipeline(采样纹理 alpha 混合), 单位模型矩阵, 雾面顶点已是世界坐标 */
	joy_mat44_t idm = mat44_identity();
	SDL_BindGPUGraphicsPipeline(r->rp, r->pipeline);
	draw_mesh_internal(r, mesh, &idm, (SDL_GPUTexture*)tex);
}

/* ---- 光照绘制（需在 joy_r3d_begin/joy_r3d_end 之间调用） ---- */

void joy_r3d_set_light_block(joy_r3d_p r, const joy_r3d_light_block_t* block)
{
	if (!r || !block) return;
	r->light_block = *block;
	r->light_block_valid = true;
}

void joy_r3d_draw_mesh_lit(joy_r3d_p r, joy_render_mesh_p m, const joy_mat44_t* model, void* tex)
{
	joy_render_mvp_t mvp;
	SDL_GPUTextureSamplerBinding samp;
	SDL_GPUBufferBinding bind;
	if (!r || !r->rp || !r->lighting_pipeline || !r->light_block_valid || !m) return;
	/* 拦截明显非法的 buffer 指针（如 0xFFFFFFFFFFFFFFFF / 内核地址），避免崩溃 */
	if ((uintptr_t)m->buffer < (uintptr_t)0x1000 || (uintptr_t)m->buffer == (uintptr_t)-1) {
		joy_core_log_error("joy_r3d_draw_mesh_lit: 跳过非法 mesh buffer %p", (void*)m->buffer);
		return;
	}

	memset(&mvp, 0, sizeof(mvp));
	joy_r3d_mat44_to_float16(&r->viewProj, mvp.viewProj);
	joy_r3d_mat44_to_float16(model, mvp.model);
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

	/* 恢复默认 pipeline, 与 joy_r3d_draw_wire / r3d_draw_solid 保持一致:
	   否则后续用 draw_mesh_internal(如网格、2D 文字) 会误用 lighting_pipeline,
	   顶点布局(pos+normal+uv)不匹配 5-float 网格/quad, 导致文字变成白块 */
	SDL_BindGPUGraphicsPipeline(r->rp, r->pipeline);
}

void joy_r3d_draw_mesh_lit_color(joy_r3d_p r, joy_render_mesh_p m, const joy_mat44_t* model, joy_color_t color)
{
	SDL_GPUTexture* tex;
	if (!r) return;
	tex = get_color_texture(r, color.r, color.g, color.b, 255);
	joy_r3d_draw_mesh_lit(r, m, model, tex);
}

/* ---- 棋盘格纹理 ---- */

void* joy_r3d_gen_checkerboard(joy_r3d_p r, int tile_size, int tile_count)
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

void joy_r3d_draw_grid(joy_r3d_p r, int slices, float spacing, float line_width)
{
	float half, hw, p;
	int i;
	SDL_GPUTexture* gray_tex;
	SDL_GPUBuffer* bx;
	SDL_GPUBuffer* bz;
	joy_render_mesh_t mx;
	joy_render_mesh_t mz;
	joy_mat44_t id;
	half = slices * spacing * 0.5f;
	gray_tex = get_color_texture(r, 100, 100, 120, 255);
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
			bx = (SDL_GPUBuffer*)joy_r3d_upload_buffer(r->dev, verts_x, sizeof(verts_x));
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
			bz = (SDL_GPUBuffer*)joy_r3d_upload_buffer(r->dev, verts_z, sizeof(verts_z));
		}
		if (bz) {
			memset(&id, 0, sizeof(id)); id.m15 = 1;
			mz.buffer = bz; mz.vertex_count = 6;
			draw_mesh_internal(r, &mz, &id, gray_tex);
			SDL_ReleaseGPUBuffer(r->dev, bz);
		}
	}
}

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef MAX_COLOR_CACHE
#undef R3D_MEASURE
#undef WIRE_V
#undef RAMP_WIRE_V
#undef LIT_V
#undef RV
#undef JOY_FOV_DEG2RAD

/* ==================== 结束 src/jr3d.c ==================== */

/* ==================== 合并自 src/jlight.c ==================== */
#include "external/klib/kvec.h"
#include <SDL3/SDL.h>
#include "joy.h"

#define JLIGHT_MAX 128
#define JLIGHT_OCCLUDER_MAX 512



/* ---------- 遮挡线段(墙壁/柱子等) ---------- */
typedef struct joy_light_occluder {
        float x1, y1, x2, y2;
} joy_light_occluder_t;

/* ---------- 灯光系统 ---------- */
struct joy_light_sys {
        int width, height;
        joy_image_p lightmap;
        joy_image_p gradient;
        joy_light_t lights[JLIGHT_MAX];
        bool active[JLIGHT_MAX];
        int count;
        uint8_t ambient_r, ambient_g, ambient_b;
        joy_light_occluder_t occluders[JLIGHT_OCCLUDER_MAX];
        int occluder_count;
};

typedef struct { float x, y; } v2;
static v2 v2f(float x, float y) { v2 v={x,y}; return v; }

typedef struct joy_seg SEG;
typedef struct joy_ep  EP;
struct joy_ep  { v2 pos; int begin; int seg_id; float angle; };
struct joy_seg { int p1_id, p2_id; float d; };

#define EP_POS(vc,id)  ((vc)->eps.a[(id)].pos)
#define EP_ANG(vc,id)  ((vc)->eps.a[(id)].angle)
#define EP_BGN(vc,id)  ((vc)->eps.a[(id)].begin)
#define EP_SEG(vc,id)  ((vc)->eps.a[(id)].seg_id)
#define SEG_P1(vc,s)   ((vc)->eps.a[(s).p1_id])
#define SEG_P2(vc,s)   ((vc)->eps.a[(s).p2_id])
#define SEG_P1P(vc,s)  (SEG_P1(vc,s).pos)
#define SEG_P2P(vc,s)  (SEG_P2(vc,s).pos)

typedef kvec_t(EP)  ep_vec;
typedef kvec_t(SEG) seg_vec;
typedef kvec_t(v2)  v2_vec;
typedef kvec_t(int) int_vec;

typedef struct {
    v2      center;
    seg_vec segs;
    ep_vec  eps;
    int_vec open;
    float   begin_angle;
    v2_vec  out;
} VC;

static v2 line_x(v2 p1, v2 p2, v2 p3, v2 p4) {
    float s = ((p4.x-p3.x)*(p1.y-p3.y)-(p4.y-p3.y)*(p1.x-p3.x))
            / ((p4.y-p3.y)*(p2.x-p1.x)-(p4.x-p3.x)*(p2.y-p1.y));
    return v2f(p1.x+s*(p2.x-p1.x), p1.y+s*(p2.y-p1.y));
}

static bool left_of(VC* vc, int a1, int a2, v2 p) {
    v2 pa = EP_POS(vc,a1), pb = EP_POS(vc,a2);
    return (pb.x-pa.x)*(p.y-pa.y) - (pb.y-pa.y)*(p.x-pa.x) < 0;
}

static v2 lerp(v2 a, v2 b, float f) {
    return v2f(a.x*(1-f)+b.x*f, a.y*(1-f)+b.y*f);
}

static int ep_cmp(const void* a, const void* b) {
    const EP* ea=(const EP*)a,*eb=(const EP*)b;
    if (ea->angle > eb->angle) return 1;
    if (ea->angle < eb->angle) return -1;
    if (!ea->begin &&  eb->begin) return 1;
    if ( ea->begin && !eb->begin) return -1;
    return 0;
}

static VC* vc_new(void) {
    VC* vc=(VC*)SDL_calloc(1,sizeof(VC));
    if(!vc)return NULL;
    kv_init(vc->segs); kv_init(vc->eps);
    kv_init(vc->open); kv_init(vc->out);
    return vc;
}
static void vc_free(VC* vc) {
    if(!vc)return;
    kv_destroy(vc->segs);kv_destroy(vc->eps);
    kv_destroy(vc->open);kv_destroy(vc->out);
    SDL_free(vc);
}
static void vc_clear(VC* vc) {
    vc->segs.n=0; vc->eps.n=0;
    vc->open.n=0; vc->out.n=0;
}

static void add_seg(VC* vc, float x1, float y1, float x2, float y2) {
    int si=(int)kv_size(vc->segs), ei=(int)kv_size(vc->eps);
    SEG s; EP e1,e2;
    memset(&s,0,sizeof(s));memset(&e1,0,sizeof(e1));memset(&e2,0,sizeof(e2));
    e1.pos=v2f(x1,y1); e1.seg_id=si;
    e2.pos=v2f(x2,y2); e2.seg_id=si;
    s.p1_id=ei; s.p2_id=ei+1;
    kv_push(SEG,vc->segs,s);
    kv_push(EP, vc->eps, e1);
    kv_push(EP, vc->eps, e2);
}

static void vc_bound(VC* vc, float minx, float miny, float maxx, float maxy) {
    add_seg(vc, minx, miny, maxx, miny);
    add_seg(vc, maxx, miny, maxx, maxy);
    add_seg(vc, maxx, maxy, minx, maxy);
    add_seg(vc, minx, maxy, minx, miny);
}

static void vc_setup(VC* vc, float lx, float ly) {
    vc->center=v2f(lx,ly);
    for(size_t i=0;i<kv_size(vc->segs);i++){
        SEG* s=&vc->segs.a[i];
        v2 p1=EP_POS(vc,s->p1_id), p2=EP_POS(vc,s->p2_id);
        float dx=0.5f*(p1.x+p2.x)-lx, dy=0.5f*(p1.y+p2.y)-ly;
        s->d=dx*dx+dy*dy;
        EP_ANG(vc,s->p1_id)=atan2f(p1.y-ly,p1.x-lx);
        EP_ANG(vc,s->p2_id)=atan2f(p2.y-ly,p2.x-lx);
        float da=EP_ANG(vc,s->p2_id)-EP_ANG(vc,s->p1_id);
        if(da<=-(float)JOY_PI)da+=2.0f*(float)JOY_PI;
        if(da> (float)JOY_PI)da-=2.0f*(float)JOY_PI;
        EP_BGN(vc,s->p1_id)=(da>0);
        EP_BGN(vc,s->p2_id)=!EP_BGN(vc,s->p1_id);
    }
}

static bool seg_front(VC* vc, int aid, int bid, v2 rel) {
    SEG* a=&vc->segs.a[aid]; SEG* b=&vc->segs.a[bid];
    v2 a1=EP_POS(vc,a->p1_id), a2=EP_POS(vc,a->p2_id);
    v2 b1=EP_POS(vc,b->p1_id), b2=EP_POS(vc,b->p2_id);
    float f=0.01f;
    bool A1=left_of(vc,a->p1_id,a->p2_id,lerp(b1,b2,f));
    bool A2=left_of(vc,a->p1_id,a->p2_id,lerp(b2,b1,f));
    bool A3=left_of(vc,a->p1_id,a->p2_id,rel);
    bool B1=left_of(vc,b->p1_id,b->p2_id,lerp(a1,a2,f));
    bool B2=left_of(vc,b->p1_id,b->p2_id,lerp(a2,a1,f));
    bool B3=left_of(vc,b->p1_id,b->p2_id,rel);
    if(B1==B2&&B2!=B3)return true;
    if(A1==A2&&A2==A3)return true;
    if(A1==A2&&A2!=A3)return false;
    if(B1==B2&&B2==B3)return false;
    return a->d < b->d;
}

static int open_front(VC* vc){return kv_size(vc->open)>0?kv_A(vc->open,0):-1;}

static void open_add(VC* vc,int sid){
    size_t at=kv_size(vc->open);
    for(size_t i=0;i<kv_size(vc->open);i++)
        if(seg_front(vc,sid,kv_A(vc->open,i),vc->center))
            {at=i;break;}
    kv_push(int,vc->open,0);
    for(size_t i=kv_size(vc->open)-1;i>at;i--)
        kv_A(vc->open,i)=kv_A(vc->open,i-1);
    kv_A(vc->open,at)=sid;
}

static void open_rm(VC* vc,int sid){
    size_t f=kv_size(vc->open);
    for(size_t i=0;i<kv_size(vc->open);i++)
        if(kv_A(vc->open,i)==sid){f=i;break;}
    if(f<kv_size(vc->open)){
        for(size_t i=f;i<kv_size(vc->open)-1;i++)
            kv_A(vc->open,i)=kv_A(vc->open,i+1);
        vc->open.n--;
    }
}

static void add_tri(VC* vc, float a1, float a2, int old_sid) {
    v2 p1=vc->center;
    v2 p2=v2f(vc->center.x+(float)cos(a1),vc->center.y+(float)sin(a1));
    v2 p3,p4;
    if(old_sid>=0){
        SEG* s=&vc->segs.a[old_sid];
        p3=EP_POS(vc,s->p1_id); p4=EP_POS(vc,s->p2_id);
    }else{
        p3=v2f(vc->center.x+(float)cos(a1)*500,vc->center.y+(float)sin(a1)*500);
        p4=v2f(vc->center.x+(float)cos(a2)*500,vc->center.y+(float)sin(a2)*500);
    }
    v2 pb=line_x(p3,p4,p1,p2);
    p2=v2f(vc->center.x+(float)cos(a2),vc->center.y+(float)sin(a2));
    v2 pe=line_x(p3,p4,p1,p2);
    kv_push(v2,vc->out,pb); kv_push(v2,vc->out,pe);
}

static int vc_sweep(VC* vc) {
    qsort(vc->eps.a,kv_size(vc->eps),sizeof(EP),ep_cmp);
    vc->out.n=0; vc->open.n=0; vc->begin_angle=0;
    for(int pass=0;pass<2;pass++){
        size_t n=kv_size(vc->eps);
        for(size_t i=0;i<n;i++){
            EP* ep=&vc->eps.a[i];
            int old_f=open_front(vc);
            if(ep->begin) open_add(vc,ep->seg_id);
            else          open_rm(vc,ep->seg_id);
            int new_f=open_front(vc);
            if(old_f!=new_f){
                if(pass==1) add_tri(vc,vc->begin_angle,ep->angle,old_f);
                vc->begin_angle=ep->angle;
            }
        }
    }
    return (int)kv_size(vc->out);
}

#define GSZ 256
static joy_image_p make_grad(joy_r2d_p r2d){
    joy_image_p t = joy_r2d_image_create_empty(r2d, GSZ, GSZ);
    if(!t)return NULL;
    uint32_t* px=(uint32_t*)malloc(GSZ*GSZ*4);
    if(!px){joy_r2d_image_destroy(t);return NULL;}
    float cx=(GSZ-1)*0.5f,cy=cx,mr=cx;
    for(int y=0;y<GSZ;y++)for(int x=0;x<GSZ;x++){
        float d=sqrtf((x-cx)*(x-cx)+(y-cy)*(y-cy));
        float k=d/mr;if(k>1)k=1;
        uint8_t a=(uint8_t)((1-k*k)*255);
        px[y*GSZ+x]=((uint32_t)a<<24)|0x00FFFFFF;
    }
    joy_rect_t full = {0, 0, GSZ, GSZ};
    joy_r2d_image_update(t, &full, px, GSZ*4);
    free(px);
    joy_r2d_image_set_blend_mode(t, JR2D_BLEND_ADD);
    return t;
}

joy_light_sys_p joy_r3d_light_sys_create(int w,int h){
    joy_light_sys_p s=(joy_light_sys_p)calloc(1,sizeof(joy_light_sys_t));
    if(!s)return NULL;
    s->width=w;s->height=h;
    s->ambient_r=20;s->ambient_g=20;s->ambient_b=20;
    return s;
}
static void ensure_tex(joy_light_sys_p s,joy_r2d_p r2d){
    if(!s->gradient) s->gradient = make_grad(r2d);
}
void joy_r3d_light_sys_destroy(joy_light_sys_p s){
    if(!s)return;
    if(s->gradient) joy_r2d_image_destroy(s->gradient);
    if(s->lightmap) joy_r2d_image_destroy(s->lightmap);
    free(s);
}
void joy_r3d_light_sys_resize(joy_light_sys_p s,int w,int h){
    if(!s)return;
    if(s->lightmap){joy_r2d_image_destroy(s->lightmap);s->lightmap=NULL;}
    s->width=w;s->height=h;
}
int joy_r3d_light_add(joy_light_sys_p s,joy_light_t l){
    if(!s||s->count>=JLIGHT_MAX)return -1;
    int id=-1;
    for(int i=0;i<JLIGHT_MAX;i++)if(!s->active[i]){id=i;break;}
    if(id<0)return -1;
    s->lights[id]=l;s->active[id]=true;s->count++;
    return id;
}
void joy_r3d_light_remove(joy_light_sys_p s,int id){
    if(!s||id<0||id>=JLIGHT_MAX||!s->active[id])return;
    s->active[id]=false;s->count--;
}
void joy_r3d_light_set(joy_light_sys_p s,int id,joy_light_t l){
    if(!s||id<0||id>=JLIGHT_MAX||!s->active[id])return;
    s->lights[id]=l;
}
void joy_r3d_light_clear(joy_light_sys_p s){
    if(!s)return;memset(s->active,0,sizeof(s->active));s->count=0;
}
void joy_r3d_light_clear_occluders(joy_light_sys_p s){if(s)s->occluder_count=0;}
int joy_r3d_light_add_occluder(joy_light_sys_p s,float x1,float y1,float x2,float y2){
    if(!s||s->occluder_count>=JLIGHT_OCCLUDER_MAX)return -1;
    int id=s->occluder_count++;
    s->occluders[id].x1=x1;s->occluders[id].y1=y1;
    s->occluders[id].x2=x2;s->occluders[id].y2=y2;
    return id;
}
void joy_r3d_light_set_ambient(joy_light_sys_p s,uint8_t r,uint8_t g,uint8_t b){
    if(!s)return;s->ambient_r=r;s->ambient_g=g;s->ambient_b=b;
}

void joy_r3d_light_render(joy_light_sys_p s, joy_r2d_p r2d){
    if(!s||!r2d||s->count==0)return;
    ensure_tex(s,r2d);
    if(!s->gradient)return;
    VC* vc=vc_new();
    if(!vc)return;
    joy_image_p grad = s->gradient;
    for(int li=0;li<JLIGHT_MAX;li++){
        if(!s->active[li])continue;
        joy_light_t* l=&s->lights[li];
        if(l->intensity<=0||l->radius<=0)continue;
        float lx=l->x,ly=l->y,rr=l->radius;
        uint8_t cr=(uint8_t)(l->r*l->intensity);
        uint8_t cg=(uint8_t)(l->g*l->intensity);
        uint8_t cb=(uint8_t)(l->b*l->intensity);

        if(s->occluder_count==0){
            float d=rr*2;
            joy_rect_t dst={lx-rr,ly-rr,d,d};
            joy_r2d_image_set_blend_mode(grad, JR2D_BLEND_ADD);
            joy_r2d_image_set_alpha(grad, 255);
            joy_r2d_image_draw1(grad, NULL, &dst);
            continue;
        }

        vc_clear(vc);
        vc->center=v2f(lx,ly);
        vc_bound(vc, 0,0,(float)s->width,(float)s->height);
        for(int oi=0;oi<s->occluder_count;oi++)
            add_seg(vc,
                s->occluders[oi].x1,s->occluders[oi].y1,
                s->occluders[oi].x2,s->occluders[oi].y2);
        vc_setup(vc,lx,ly);
        int n_out=vc_sweep(vc);
        if(n_out<2)continue;

        /* 调试轮廓（绿色线框） */
        joy_r2d_set_blend_mode(r2d, JR2D_BLEND_NONE);
        joy_r2d_set_color(r2d, 0,255,0,255);
        for(int i=0;i<n_out-1;i++)
            joy_r2d_shape_draw_line(r2d, vc->out.a[i].x,vc->out.a[i].y,
                              vc->out.a[i+1].x,vc->out.a[i+1].y);
        joy_r2d_shape_draw_line(r2d, vc->out.a[n_out-1].x,vc->out.a[n_out-1].y,
                          vc->out.a[0].x,vc->out.a[0].y);

        /* 三角扇（渐变纹理） */
        int nv = 1 + n_out;
        int nv_size = nv * 8; // 每顶点 8 float
        float* verts = (float*)SDL_malloc(sizeof(float) * nv_size);
        if(!verts)continue;
        verts[0]=lx; verts[1]=ly;  // pos
        verts[2]=1; verts[3]=1; verts[4]=1; verts[5]=1; // color
        verts[6]=0.5f; verts[7]=0.5f; // tex
        for(int i=0;i<n_out;i++){
            int bi = (i+1)*8;
            verts[bi]=vc->out.a[i].x; verts[bi+1]=vc->out.a[i].y;
            verts[bi+2]=1; verts[bi+3]=1; verts[bi+4]=1; verts[bi+5]=1;
            verts[bi+6]=0.5f+(vc->out.a[i].x-lx)/(2.0f*rr);
            verts[bi+7]=0.5f+(vc->out.a[i].y-ly)/(2.0f*rr);
        }
        int nt = n_out;
        int* idx = (int*)SDL_malloc(nt*3*sizeof(int));
        int ni=0;
        if(idx){
            for(int i=0;i+1<n_out;i++){
                idx[ni++]=0;idx[ni++]=i+1;idx[ni++]=i+2;
            }
            idx[ni++]=0;idx[ni++]=n_out;idx[ni++]=1;
        }
        if(ni>0){
            joy_r2d_image_set_blend_mode(grad, JR2D_BLEND_ADD);
            joy_r2d_image_set_alpha(grad, 255);
            joy_r2d_render_geometry(r2d, grad, verts, nv, idx, ni);
        }
        SDL_free(idx);SDL_free(verts);
    }
    vc_free(vc);
}

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef JLIGHT_MAX
#undef JLIGHT_OCCLUDER_MAX
#undef EP_POS
#undef EP_ANG
#undef EP_BGN
#undef EP_SEG
#undef SEG_P1
#undef SEG_P2
#undef SEG_P1P
#undef SEG_P2P
#undef GSZ

/* ==================== 结束 src/jlight.c ==================== */

/* ==================== 合并自 src/jgltf.c ==================== */
/******************************************************************************
 * jgltf —— joy 的 glTF/glb 加载 + CPU 骨骼蒙皮动画 + 绘制模块（可选, 需定义 JOY_USE_GLTF）
 *
 * 渲染：降级路径——每帧用 joy_r3d_mesh_custom_lit 按新顶点重建 lit mesh、
 *      joy_r3d_draw_mesh_lit 绘制（r3d_mesh_skinned_* 动态蒙皮接口从未实现，
 *      属新功能缺口，见 NEXT_SESSION P0-b；当前无 GLTF 模型 / 真机可验运行）。
 * 加载：先把带 BOM / 头部杂散字节的"畸形 glb"在内存中归一化重建为 clean glb
 *      （算法照抄 build/norm_test/norm_test.c），再用 cgltf 解析 + 加载 buffer。
 *
 * 说明：
 *   - 走非索引三角形列表绘制，因此带 INDICES 的 primitive
 *     在加载时按索引展开成非索引进制顶点（skin 属性随顶点复制），交给 GPU 时
 *     不建索引缓冲。
 *   - 未启用 JOY_USE_GLTF 时本文件为空编译单元，不影响任何项目。
 *****************************************************************************/

#ifdef JOY_USE_GLTF

#define CGLTF_IMPLEMENTATION
#include "external/cgltf/cgltf.h"

#include "joy.h"
/* 本 TU 用 SDL_IOFromFile / SDL_ReadIO / SDL_WriteIO / SDL_CloseIO 与 Sint64，
 * 公共头不再间接提供 SDL 声明，启用 JOY_USE_GLTF 时必须自己包含。 */
#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* 贴图解码用 stb_image（实现在 joy/jimpl.c 的 STB_IMAGE_IMPLEMENTATION） */
#include "external/stb_image.h"

/* ==================== 内部类型 ==================== */

/* 每个 primitive 的 CPU 蒙皮源数据（共享, 只读） */
typedef struct {
        int   vertex_count;   /* 已按索引展开的顶点数（非索引进制列表） */
        int   skinned;
        int   joint_count;    /* 该 primitive 关联 skin 的关节数 */
        int  *joint_node;     /* 每个 joint 对应的 node 下标 */
        joy_mat44_t *inverse_bind;/* 每个 joint 的逆绑定矩阵 */
        float *bind_verts;    /* lit 布局: pos.xyz + normal.xyz + uv.xy, 每顶点 8 float */
        float *jnt;           /* 每顶点 4 个关节索引(float), 为 skin joints 数组下标 */
        float *wts;           /* 每顶点 4 个权重 */
        void  *tex;           /* baseColor 贴图的 SDL 纹理(joy_r3d_create_texture), 无则 NULL */
} joy_gltf_prim_src_t;

struct joy_gltf_asset {
        int   refs;
        joy_r3d_p r3d;
        cgltf_data *data;
        void *clean_glb;        /* 规范化后的完整 glb 内存：cgltf 的 buffers/bin 与动画采样
                                   读数都指向它，必须存活到 cgltf_free 之后（随 asset 一并释放，
                                   否则 use-after-free）*/
        void  *white_tex;           /* 纯白填充纹理, 供光照管线采样 */
        int    node_count;
        int    prim_count;
        joy_gltf_prim_src_t *prims;
        int    anim_count;
        float *anim_duration;
        joy_vec3_t bounds_center;   /* 所有 primitive 绑定姿态的包围球中心（mesh 局部空间） */
        float  bounds_radius;   /* 包围球半径（mesh 局部空间） */
};

/* 句柄别名必须定义在 struct joy_gltf_instance 之前：它的第一个字段就要用
 * joy_gltf_asset_p。这两个 struct 此前从未 typedef 成句柄类型，文件里却一直
 * 使用未加 joy_ 前缀的旧别名，是 JOY_USE_GLTF 一打开就编译不过的预存缺陷
 * （收口版即有，与本轮改名无关）。 */
typedef struct joy_gltf_asset joy_gltf_asset_t, *joy_gltf_asset_p;

struct joy_gltf_instance {
        joy_gltf_asset_p asset;
        int     anim_index;
        float   time;
        float   speed;
        bool    paused;
        int     node_count;
        cgltf_node **nodes;
        bool   *matrix_mode;
        joy_mat44_t *static_mat;         /* has_matrix 节点的静态矩阵 */
        joy_mat44_t *node_world;         /* 各节点世界矩阵（mesh 局部空间） */
        joy_vec3_t *lcl_t;
        joy_vec4_t *lcl_r;
        joy_vec3_t *lcl_s;
        int     prim_count;
        joy_render_mesh_p *gpu;          /* 每 primitive 一个动态蒙皮缓冲 */
        float **skinned;             /* 每 primitive CPU 蒙皮输出（重叠写 GPU） */
        int     moved_vertices;
        joy_render_mesh_p bones_mesh;    /* 待销毁的上一帧骨骼线 mesh */
};

typedef struct joy_gltf_instance joy_gltf_instance_t, *joy_gltf_instance_p;

/* ==================== 数学（最小实现, 只用 <math.h>） ==================== */

/* 四元数到列主序旋转矩阵（与 joy_mat44_t/mat44_mul 的列主序约定一致） */
static joy_mat44_t quat_to_mat44(joy_vec4_t q) {
        float l = sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
        if (l < 1e-12f) l = 1.0f;
        float x = q.x/l, y = q.y/l, z = q.z/l, w = q.w/l;
        float x2=x*x, y2=y*y, z2=z*z;
        joy_mat44_t m;
        m.m0 = 1-2*(y2+z2); m.m4 = 2*(x*y-w*z);  m.m8 = 2*(x*z+w*y);  m.m12 = 0;
        m.m1 = 2*(x*y+w*z);  m.m5 = 1-2*(x2+z2);  m.m9 = 2*(y*z-w*x);  m.m13 = 0;
        m.m2 = 2*(x*z-w*y);  m.m6 = 2*(y*z+w*x);  m.m10= 1-2*(x2+y2);  m.m14 = 0;
        m.m3 = 0;            m.m7 = 0;            m.m11= 0;            m.m15 = 1;
        return m;
}

static joy_vec4_t quat_slerp(joy_vec4_t a, joy_vec4_t b, float t) {
        float dot = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
        if (dot < 0.0f) { dot = -dot; b.x=-b.x; b.y=-b.y; b.z=-b.z; b.w=-b.w; }
        if (dot > 0.9995f) {
                joy_vec4_t r = { a.x+(b.x-a.x)*t, a.y+(b.y-a.y)*t,
                             a.z+(b.z-a.z)*t, a.w+(b.w-a.w)*t };
                float l = sqrtf(r.x*r.x+r.y*r.y+r.z*r.z+r.w*r.w);
                if (l) { r.x/=l; r.y/=l; r.z/=l; r.w/=l; }
                return r;
        }
        float theta = acosf(dot);
        float st = sinf(theta);
        float sa = sinf((1-t)*theta)/st;
        float sb = sinf(t*theta)/st;
        joy_vec4_t r = { a.x*sa + b.x*sb, a.y*sa + b.y*sb, a.z*sa + b.z*sb, a.w*sa + b.w*sb };
        return r;
}

static joy_mat44_t mat_from_float16(const float f[16]) {
        joy_mat44_t m;
        m.m0=f[0]; m.m1=f[1]; m.m2=f[2]; m.m3=f[3];
        m.m4=f[4]; m.m5=f[5]; m.m6=f[6]; m.m7=f[7];
        m.m8=f[8]; m.m9=f[9]; m.m10=f[10]; m.m11=f[11];
        m.m12=f[12]; m.m13=f[13]; m.m14=f[14]; m.m15=f[15];
        return m;
}

/* ==================== glb 归一化重建（照抄 norm_test.c） ==================== */

/* 走 SDL IO（不是 fopen）：安卓上资源打包在 APK 内，fopen 读不到。
 * 仍用 malloc 分配，保持调用方 free() 的原有契约。 */
static unsigned char* read_file(const char* path, long* out_len) {
        SDL_IOStream* io = SDL_IOFromFile(path, "rb");
        if (!io) return NULL;
        Sint64 n = SDL_GetIOSize(io);
        if (n < 0) { SDL_CloseIO(io); return NULL; }
        unsigned char* b = (unsigned char*)malloc(n ? (size_t)n : 1);
        if (!b) { SDL_CloseIO(io); return NULL; }
        if (n) {
                size_t got = SDL_ReadIO(io, b, (size_t)n);
                if (got != (size_t)n) { free(b); SDL_CloseIO(io); return NULL; }
        }
        SDL_CloseIO(io);
        *out_len = (long)n;
        return b;
}

static size_t align4(size_t x) { return (x + 3u) & ~(size_t)3u; }

static long find_bytes(const unsigned char* d, long len, long from,
                       const unsigned char* needle, int nlen) {
        for (long i = from; i + nlen <= len; i++)
                if (memcmp(d + i, needle, (size_t)nlen) == 0) return i;
        return -1;
}

static long find_root_close(const unsigned char* d, long len, long start) {
        long depth = 0;
        for (long i = start; i < len; i++) {
                if (d[i] == '"') {
                        i++;
                        while (i < len) {
                                if (d[i] == '\\' && i + 1 < len) { i += 2; continue; }
                                if (d[i] == '"') break;
                                i++;
                        }
                        continue;
                }
                if (d[i] == '{' || d[i] == '[') depth++;
                else if (d[i] == '}' || d[i] == ']') {
                        if (--depth == 0) return i;
                }
        }
        return -1;
}

/* 把(maybe 含 BOM + 杂散字节的) glb 重建为 clean glb，返回 malloc 缓冲, 出参 <out_len>。 */
static unsigned char* make_clean_glb(const unsigned char* raw, long fsize, long* out_len) {
        long p = 0;
        if (fsize >= 3 && raw[0]==0xEF && raw[1]==0xBB && raw[2]==0xBF) p = 3;
        const unsigned char jn[] = {'J','S','O','N'};
        const unsigned char bn[] = {'B','I','N',0x00};

        long json_type = find_bytes(raw, fsize, p, jn, 4);
        long json_start = json_type + 4;
        long json_end   = find_root_close(raw, fsize, json_start);
        if (json_type < 0 || json_end < 0) return NULL;
        long json_len   = json_end - json_start + 1;

        long bin_type  = find_bytes(raw, fsize, json_end + 1, bn, 4);
        long bin_start = (bin_type >= 0) ? bin_type + 4 : fsize;
        long bin_len   = fsize - bin_start;

        /* The exported GLB has UTF-8 replacement U+FFFD (bytes EF BF BD)
         * sequences inserted throughout the BIN chunk body. Strip them so
         * bufferView offsets (relative to the clean binary data) resolve. */
        size_t bom_count = 0;
        {
                long k = bin_start;
                while (k + 2 < fsize) {
                        if (raw[k]==0xEF && raw[k+1]==0xBF && raw[k+2]==0xBD) { bom_count++; k += 3; }
                        else k++;
                }
        }
        long clean_bin_len = bin_len - (long)(bom_count * 3);
        if (clean_bin_len < 0) clean_bin_len = 0;

        size_t json_pad = align4((size_t)json_len) - (size_t)json_len;
        size_t bin_pad  = align4((size_t)clean_bin_len)  - (size_t)clean_bin_len;
        size_t clean_len = 12 + (8 + align4(json_len)) + (8 + align4(clean_bin_len));
        unsigned char* clean = (unsigned char*)calloc(clean_len, 1);

        unsigned char* o = clean;
        o[0]='g';o[1]='l';o[2]='T';o[3]='F';
        uint32_t v = 2; memcpy(o+4, &v, 4);
        uint32_t tl = (uint32_t)clean_len; memcpy(o+8, &tl, 4);
        o += 12;
        uint32_t jl = (uint32_t)align4(json_len); memcpy(o, &jl, 4);
        memcpy(o+4, "JSON", 4); o += 8;
        memcpy(o, raw + json_start, (size_t)json_len);
        memset(o + json_len, ' ', json_pad);
        o += align4(json_len);
        uint32_t bl = (uint32_t)align4(clean_bin_len); memcpy(o, &bl, 4);
        o[4]='B'; o[5]='I'; o[6]='N'; o[7]=0x00;
        o += 8;
        {
                long k = bin_start;
                long w = 0;
                while (k < fsize) {
                        if (k + 2 < fsize && raw[k]==0xEF && raw[k+1]==0xBB && raw[k+2]==0xBF) { k += 3; }
                        else { o[w++] = raw[k++]; }
                }
        }
        memset(o + clean_bin_len, 0, bin_pad);

        *out_len = (long)clean_len;
        return clean;
}

/* ==================== accessor / 采样小工具 ==================== */

static cgltf_accessor* find_attr(cgltf_primitive* p, const char* name) {
        cgltf_size i;
        for (i = 0; i < p->attributes_count; i++)
                if (strcmp(p->attributes[i].name, name) == 0)
                        return p->attributes[i].data;
        return NULL;
}

static void accessor_comp(cgltf_accessor* a, cgltf_size e, cgltf_size comps, float out[8]) {
        memset(out, 0, comps * sizeof(float));
        if (!a) return;
        cgltf_accessor_read_float(a, e, out, comps);
}

/* 读出 sampler 输出中第 e 个关键帧的 comps 个分量（cubic spline 取中心值分量） */
static void output_val(cgltf_animation_sampler* s, cgltf_size e, cgltf_size comps, float out[8]) {
        memset(out, 0, comps * sizeof(float));
        if (!s->output) return;
        cgltf_size base = e;
        if (s->interpolation == cgltf_interpolation_type_cubic_spline) base = 3*e + 1;
        cgltf_accessor_read_float(s->output, base, out, comps);
}

/* 对动画 tracker 求值（TRS 插值），返回该时刻采样值（前 comps 个分量） */
static void eval_sampler(cgltf_animation_sampler* s, float t, cgltf_size comps, float out[8]) {
        cgltf_accessor* in = s->input;
        cgltf_size n = in ? in->count : 0;
        if (!n) { memset(out, 0, comps*sizeof(float)); return; }

        float in0, in1;
        cgltf_accessor_read_float(in, 0, &in0, 1);
        cgltf_accessor_read_float(in, n-1, &in1, 1);

        float ft = t;
        if (n > 1 && in1 > in0) {           /* 循环到 [in0, in1) */
                float span = in1 - in0;
                ft = fmodf(t - in0, span);
                if (ft < 0.0f) ft += span;
                ft += in0;
        }

        cgltf_size k = 0;
        while (k + 1 < n) {
                float tk1;
                cgltf_accessor_read_float(in, k+1, &tk1, 1);
                if (ft < tk1) break;
                k++;
        }
        if (k >= n - 1) k = n - 2;

        float tk, tk1, v0[8], v1[8];
        cgltf_accessor_read_float(in, k, &tk, 1);
        cgltf_accessor_read_float(in, k+1, &tk1, 1);
        output_val(s, k, comps, v0);
        output_val(s, k+1, comps, v1);

        if (s->interpolation == cgltf_interpolation_type_step) {
                memcpy(out, v0, comps*sizeof(float));
                return;
        }
        float u = (tk1 > tk) ? (ft - tk) / (tk1 - tk) : 0.0f;
        if (comps == 4) {                    /* rotation: slerp */
                joy_vec4_t q0 = { v0[0], v0[1], v0[2], v0[3] };
                joy_vec4_t q1 = { v1[0], v1[1], v1[2], v1[3] };
                joy_vec4_t q  = quat_slerp(q0, q1, u);
                out[0]=q.x; out[1]=q.y; out[2]=q.z; out[3]=q.w;
        } else {                             /* translation / scale: lerp */
                cgltf_size c;
                for (c = 0; c < comps; c++) out[c] = v0[c] + (v1[c]-v0[c])*u;
        }
}

/* ==================== asset 加载 ==================== */

static int build_prim(joy_gltf_prim_src_t* out, cgltf_primitive* pr, cgltf_skin* skin) {
        cgltf_accessor *aPos = find_attr(pr, "POSITION");
        cgltf_accessor *aNrm = find_attr(pr, "NORMAL");
        cgltf_accessor *aUv  = find_attr(pr, "TEXCOORD_0");
        cgltf_accessor *aJnt = NULL, *aWts = NULL;
        cgltf_size si, v;

        if (!aPos) return 0;
        if (skin) { aJnt = find_attr(pr, "JOINTS_0"); aWts = find_attr(pr, "WEIGHTS_0"); }

        /* 仅当本 prim 的关节/权重访问器都有已加载的数据源时才做蒙皮；
           有 skin 但个别 prim 缺 JOINTS_0/WEIGHTS_0 数据（如无骨骼权重的装饰子网格）
           的 glb 是合法的，应安全回退为非蒙皮网格，而非崩溃 */
        int skin_ok = (skin && aJnt && aWts
                && aJnt->buffer_view && aJnt->buffer_view->buffer && aJnt->buffer_view->buffer->data
                && aWts->buffer_view && aWts->buffer_view->buffer && aWts->buffer_view->buffer->data);
        if (skin && !skin_ok)
                fprintf(stderr, "[jgltf] warn: prim 缺 skin 权重数据，按非蒙皮处理 (pr=%p)\n", (void*)pr);

        cgltf_size src_count = aPos->count;
        cgltf_size vcount;
        int indexed = (pr->indices && pr->indices->count > 0);
        vcount = indexed ? pr->indices->count : src_count;

        memset(out, 0, sizeof(*out));
        out->vertex_count = (int)vcount;
        out->skinned   = skin_ok ? 1 : 0;
        out->joint_count = skin_ok ? (int)skin->joints_count : 0;
        out->bind_verts = (float*)calloc(vcount * 8, sizeof(float));
        out->jnt = (skin_ok ? (float*)calloc(vcount * 4, sizeof(float)) : NULL);
        out->wts = (skin_ok ? (float*)calloc(vcount * 4, sizeof(float)) : NULL);

        if (skin_ok) {
                cgltf_size j;
                out->joint_node  = (int*)calloc(skin->joints_count ? skin->joints_count : 1, sizeof(int));
                out->inverse_bind= (joy_mat44_t*)calloc(skin->joints_count ? skin->joints_count : 1, sizeof(joy_mat44_t));
                if (skin->inverse_bind_matrices) {
                        for (j = 0; j < skin->joints_count; j++) {
                                float f[16];
                                cgltf_accessor_read_float(skin->inverse_bind_matrices, j, f, 16);
                                out->inverse_bind[j] = mat_from_float16(f);
                        }
                } else {
                        for (j = 0; j < skin->joints_count; j++)
                                out->inverse_bind[j] = mat44_identity();
                }
        }

        for (si = 0; si < vcount; si++) {
                cgltf_size src = indexed ? cgltf_accessor_read_index(pr->indices, si) : si;
                float pv[8], nv[8], uv[8], jv[8], wv[8];
                accessor_comp(aPos, src, 3, pv);
                accessor_comp(aNrm, src, 3, nv);
                accessor_comp(aUv,  src, 2, uv);
                float* o = out->bind_verts + si*8;
                o[0]=pv[0]; o[1]=pv[1]; o[2]=pv[2];
                o[3]=nv[0]; o[4]=nv[1]; o[5]=nv[2];
                o[6]=uv[0]; o[7]=uv[1];
                if (skin_ok) {
                        accessor_comp(aJnt, src, 4, jv);
                        accessor_comp(aWts, src, 4, wv);
                        float sum = wv[0]+wv[1]+wv[2]+wv[3];
                        if (sum > 1e-6f) { wv[0]/=sum; wv[1]/=sum; wv[2]/=sum; wv[3]/=sum; }
                        float* jp = out->jnt + si*4;
                        float* wp = out->wts + si*4;
                        jp[0]=jv[0]; jp[1]=jv[1]; jp[2]=jv[2]; jp[3]=jv[3];
                        wp[0]=wv[0]; wp[1]=wv[1]; wp[2]=wv[2]; wp[3]=wv[3];
                }
        }
        return 1;
}

/* 从 primitive 的 baseColor 贴图(image 的 bufferView)解码出 GPU 纹理；无贴图/失败返回 NULL */
void* jgltf_prim_load_texture(joy_r3d_p r3d, cgltf_primitive* pr) {
        if (!pr || !pr->material) return NULL;
        cgltf_texture_view* tv = &pr->material->pbr_metallic_roughness.base_color_texture;
        if (!tv->texture || !tv->texture->image) return NULL;
        cgltf_image* img = tv->texture->image;
        if (img->buffer_view) {
                cgltf_buffer* buf = img->buffer_view->buffer;
                const unsigned char* p = (const unsigned char*)buf->data + img->buffer_view->offset;
                int w = 0, h = 0, n = 0;
                unsigned char* rgba = stbi_load_from_memory(p, (int)img->buffer_view->size, &w, &h, &n, 4);
                if (!rgba || w <= 0 || h <= 0) { fprintf(stderr, "[jgltf] tex decode fail (%.0fx%.0f)\n", (double)w, (double)h); return NULL; }
                void* tex = joy_r3d_create_texture(r3d, w, h, rgba, 1);
                stbi_image_free(rgba);
                printf("[jgltf] tex %dx%d -> %p\n", w, h, tex);
                return tex;
        }
        return NULL;
}

joy_gltf_asset_p jgltf_asset_load(joy_r3d_p r3d, const char* path) {
        long fsize; unsigned char* raw; unsigned char* clean; long clen = 0;
        cgltf_options opt; cgltf_data* data = NULL; cgltf_result res;
        joy_gltf_asset_p a; int mi, pi; cgltf_size i, pc;

        raw = read_file(path, &fsize);
        if (!raw) { printf("[jgltf] cannot read '%s'\n", path); return NULL; }
        clean = make_clean_glb(raw, fsize, &clen);
        free(raw);
        if (!clean) { printf("[jgltf] cannot normalize glb '%s'\n", path); return NULL; }
        {
                char dpath[256];
                SDL_IOStream* dio;
                snprintf(dpath, sizeof(dpath), "clean_%s.glb", path);
                dio = SDL_IOFromFile(dpath, "wb");
                if (dio) { SDL_WriteIO(dio, clean, (size_t)clen); SDL_CloseIO(dio); }
        }

        memset(&opt, 0, sizeof(opt));
        res = cgltf_parse(&opt, clean, (cgltf_size)clen, &data);
        if (res != cgltf_result_success) {
                printf("[jgltf] cgltf_parse failed res=%d for '%s'\n", (int)res, path);
                free(clean);
                return NULL;
        }
        if (cgltf_load_buffers(&opt, data, NULL) != cgltf_result_success) {
                printf("[jgltf] cgltf_load_buffers failed for '%s'\n", path);
                cgltf_free(data);
                free(clean);
                return NULL;
        }

        /* mesh -> skin 映射（取第一个带着该 mesh 且有 skin 的节点） */
        cgltf_size mesh_count = data->meshes_count;
        cgltf_skin** mesh_skin = (cgltf_skin**)calloc(mesh_count ? mesh_count : 1, sizeof(cgltf_skin*));
        for (i = 0; i < data->nodes_count; i++) {
                cgltf_node* nd = &data->nodes[i];
                if (nd->mesh && nd->skin && !mesh_skin[(size_t)(nd->mesh - data->meshes)])
                        mesh_skin[(size_t)(nd->mesh - data->meshes)] = nd->skin;
        }

        cgltf_size prim_count = 0;
        for (mi = 0; mi < (int)mesh_count; mi++) prim_count += data->meshes[mi].primitives_count;

        a = (joy_gltf_asset_p)calloc(1, sizeof(*a));
        a->refs = 1;
        a->r3d = r3d;
        a->data = data;
        a->clean_glb = clean;   /* cgltf 数据(buffer/访问器/动画)指向它，asset 存活期内必须保留 */
        a->node_count = (int)data->nodes_count;
        a->prim_count = (int)prim_count;
        a->anim_count = (int)data->animations_count;
        a->prims = (joy_gltf_prim_src_t*)calloc(prim_count ? prim_count : 1, sizeof(joy_gltf_prim_src_t));
        a->anim_duration = (float*)calloc(a->anim_count ? a->anim_count : 1, sizeof(float));

        pc = 0;
        for (mi = 0; mi < (int)mesh_count; mi++) {
                cgltf_skin* skin = mesh_skin[mi];
                for (pi = 0; pi < (int)data->meshes[mi].primitives_count; pi++) {
                        cgltf_primitive* pr = &data->meshes[mi].primitives[pi];
                        build_prim(&a->prims[pc], pr, skin);
                        a->prims[pc].tex = jgltf_prim_load_texture(r3d, pr);
                        if (skin) {
                                cgltf_size j;
                                for (j = 0; j < skin->joints_count; j++)
                                        a->prims[pc].joint_node[j] = (int)(skin->joints[j] - data->nodes);
                        }
                        pc++;
                }
        }

        for (i = 0; i < (cgltf_size)a->anim_count; i++) {
                cgltf_animation* an = &data->animations[i];
                float dur = 0.0f; cgltf_size s;
                for (s = 0; s < an->samplers_count; s++) {
                        cgltf_size n = an->samplers[s].input ? an->samplers[s].input->count : 0;
                        if (n) { float t; cgltf_accessor_read_float(an->samplers[s].input, n-1, &t, 1);
                                 if (t > dur) dur = t; }
                }
                a->anim_duration[i] = dur;
        }

        free(mesh_skin);

        /* 纯白纹理（8x8, 光照管线采样） */
        if (r3d) {
                uint8_t rgba[8*8*4]; memset(rgba, 255, sizeof(rgba));
                a->white_tex = joy_r3d_create_texture(r3d, 8, 8, rgba, 0);
        }

        /* 计算模型空间包围球（对所有 primitive 的绑定姿态）：供调用方按模型实际
         * 尺寸归一化缩放 / 自适配相机，避免大尺寸模型(如 Fox.glb 约 26×79×155)
         * 直接扑出视锥导致不可见。 */
        {
                bool first = true;
                joy_vec3_t mn = {0,0,0}, mx = {0,0,0};
                int mi2;
                for (mi2 = 0; mi2 < a->prim_count; mi2++) {
                        joy_gltf_prim_src_t* ps = &a->prims[mi2];
                        int kk;
                        for (kk = 0; kk < ps->vertex_count; kk++) {
                                float x=ps->bind_verts[kk*8+0], y=ps->bind_verts[kk*8+1], z=ps->bind_verts[kk*8+2];
                                if (first) { mn.x=mx.x=x; mn.y=mx.y=y; mn.z=mx.z=z; first=false; }
                                else {
                                        if (x<mn.x)mn.x=x; if (y<mn.y)mn.y=y; if (z<mn.z)mn.z=z;
                                        if (x>mx.x)mx.x=x; if (y>mx.y)mx.y=y; if (z>mx.z)mx.z=z;
                                }
                        }
                }
                a->bounds_center.x=(mn.x+mx.x)*0.5f; a->bounds_center.y=(mn.y+mx.y)*0.5f; a->bounds_center.z=(mn.z+mx.z)*0.5f;
                float dx=mx.x-mn.x, dy=mx.y-mn.y, dz=mx.z-mn.z;
                a->bounds_radius = sqrtf(dx*dx+dy*dy+dz*dz) * 0.5f;
                if (a->bounds_radius < 1e-6f) a->bounds_radius = 1.0f;
        }

        printf("[jgltf] loaded '%s': nodes=%d prims=%d anims=%d\n",
               path, a->node_count, a->prim_count, a->anim_count);
        return a;
}

/* ==================== asset 生命周期 ==================== */

void jgltf_asset_retain(joy_gltf_asset_p a) { if (a) a->refs++; }

void jgltf_asset_release(joy_gltf_asset_p a) {
        int i;
        if (!a) return;
        if (--a->refs > 0) return;
        if (a->white_tex) {
                /* r3d 无公开纹理销毁, 交由 joy_r3d_destroy 一并回收 */
        }
        for (i = 0; i < a->prim_count; i++) {
                free(a->prims[i].bind_verts);
                free(a->prims[i].jnt);
                free(a->prims[i].wts);
                free(a->prims[i].joint_node);
                free(a->prims[i].inverse_bind);
        }
        free(a->prims);
        free(a->anim_duration);
        cgltf_free(a->data);
        free(a->clean_glb);      /* 须在 cgltf_free 之后再释放 clean 内存（动画数据指向它） */
        free(a);
}

int jgltf_asset_animation_count(joy_gltf_asset_p a) { return a ? a->anim_count : 0; }
const char* joy_r3d_gltf_asset_animation_name(joy_gltf_asset_p a, int index) {
        if (!a || index < 0 || index >= a->anim_count) return NULL;
        return a->data->animations[index].name;
}
float joy_r3d_gltf_asset_animation_duration(joy_gltf_asset_p a, int index) {
        if (!a || index < 0 || index >= a->anim_count) return 0.0f;
        return a->anim_duration[index];
}
int jgltf_asset_node_count(joy_gltf_asset_p a) { return a ? a->node_count : 0; }
int jgltf_asset_prim_count(joy_gltf_asset_p a) { return a ? a->prim_count : 0; }
int joy_r3d_gltf_asset_has_skin(joy_gltf_asset_p a) {
        int i;
        if (!a) return 0;
        for (i = 0; i < a->prim_count; i++) if (a->prims[i].skinned) return 1;
        return 0;
}
JOY_API void joy_r3d_gltf_asset_bounds(joy_gltf_asset_p a, joy_vec3_t* out_center, float* out_radius) {
        if (a) {
                if (out_center) *out_center = a->bounds_center;
                if (out_radius) *out_radius = a->bounds_radius;
        } else {
                if (out_center) { out_center->x=out_center->y=out_center->z=0.0f; }
                if (out_radius) *out_radius = 1.0f;
        }
}

/* ==================== instance ==================== */

joy_gltf_instance_p jgltf_instance_create(joy_gltf_asset_p a) {
        joy_gltf_instance_p inst;
        cgltf_node* nodes;
        int i;
        if (!a || a->node_count < 0) return NULL;

        inst = (joy_gltf_instance_p)calloc(1, sizeof(*inst));
        inst->asset = a;
        jgltf_asset_retain(a);
        inst->anim_index = 0;
        inst->time = 0.0f;
        inst->speed = 1.0f;
        inst->paused = false;
        inst->node_count = a->node_count;
        inst->prim_count = a->prim_count;

        nodes = a->data->nodes;
        inst->nodes = (cgltf_node**)calloc((size_t)(a->node_count ? a->node_count : 1), sizeof(cgltf_node*));
        inst->matrix_mode = (bool*)calloc((size_t)(a->node_count ? a->node_count : 1), sizeof(bool));
        inst->static_mat = (joy_mat44_t*)calloc((size_t)(a->node_count ? a->node_count : 1), sizeof(joy_mat44_t));
        inst->node_world = (joy_mat44_t*)calloc((size_t)(a->node_count ? a->node_count : 1), sizeof(joy_mat44_t));
        inst->lcl_t = (joy_vec3_t*)calloc((size_t)(a->node_count ? a->node_count : 1), sizeof(joy_vec3_t));
        inst->lcl_r = (joy_vec4_t*)calloc((size_t)(a->node_count ? a->node_count : 1), sizeof(joy_vec4_t));
        inst->lcl_s = (joy_vec3_t*)calloc((size_t)(a->node_count ? a->node_count : 1), sizeof(joy_vec3_t));
        for (i = 0; i < a->node_count; i++) {
                inst->nodes[i] = &nodes[i];
                if (nodes[i].has_matrix) {
                        inst->matrix_mode[i] = true;
                        inst->static_mat[i] = mat_from_float16(nodes[i].matrix);
                }
        }

        inst->gpu = (joy_render_mesh_p*)calloc((size_t)(a->prim_count ? a->prim_count : 1), sizeof(joy_render_mesh_p));
        inst->skinned = (float**)calloc((size_t)(a->prim_count ? a->prim_count : 1), sizeof(float*));
        for (i = 0; i < a->prim_count; i++) {
                joy_gltf_prim_src_t* src = &a->prims[i];
                float* buf = (float*)malloc((size_t)src->vertex_count * 8 * sizeof(float));
                memcpy(buf, src->bind_verts, (size_t)src->vertex_count * 8 * sizeof(float));
                inst->skinned[i] = buf;
                inst->gpu[i] = joy_r3d_mesh_custom_lit(a->r3d, buf, src->vertex_count);
        }
        return inst;
}

void jgltf_instance_destroy(joy_gltf_instance_p inst) {
        int i;
        if (!inst) return;
        for (i = 0; i < inst->prim_count; i++) {
                if (inst->gpu[i]) joy_r3d_mesh_destroy(inst->asset->r3d, inst->gpu[i]);
                free(inst->skinned[i]);
        }
        if (inst->bones_mesh) joy_r3d_mesh_destroy(inst->asset->r3d, inst->bones_mesh);
        free(inst->gpu);
        free(inst->skinned);
        free(inst->nodes);
        free(inst->matrix_mode);
        free(inst->static_mat);
        free(inst->node_world);
        free(inst->lcl_t);
        free(inst->lcl_r);
        free(inst->lcl_s);
        jgltf_asset_release(inst->asset);
        free(inst);
}

void jgltf_instance_set_animation(joy_gltf_instance_p inst, int index) {
        if (!inst || index < 0) return;
        if (index >= inst->asset->anim_count) index = 0;
        inst->anim_index = index;
        inst->time = 0.0f;
}
int  jgltf_instance_animation(joy_gltf_instance_p inst) { return inst ? inst->anim_index : -1; }
void jgltf_instance_set_speed(joy_gltf_instance_p inst, float speed) {
        if (inst) inst->speed = speed;
}
float jgltf_instance_speed(joy_gltf_instance_p inst) { return inst ? inst->speed : 1.0f; }
void jgltf_instance_set_paused(joy_gltf_instance_p inst, bool paused) {
        if (inst) inst->paused = paused;
}

void jgltf_instance_update(joy_gltf_instance_p inst, float dt) {
        joy_gltf_asset_p a; int ni, v, k; cgltf_size c;
        cgltf_animation* an; float dur;

        if (!inst || inst->paused) return;
        a = inst->asset;
        dur = (inst->anim_index < a->anim_count) ? a->anim_duration[inst->anim_index] : 0.0f;
        inst->time += dt * inst->speed;
        if (dur > 0.0f) {
                if (inst->time >= dur) inst->time = fmodf(inst->time, dur);
                if (inst->time < 0.0f) inst->time += dur;
        }

        /* 1. 从节点静态 TRS 初始化局部变换 */
        for (ni = 0; ni < inst->node_count; ni++) {
                cgltf_node* nd = inst->nodes[ni];
                inst->lcl_t[ni] = (joy_vec3_t){0,0,0};
                inst->lcl_r[ni] = (joy_vec4_t){0,0,0,1};
                inst->lcl_s[ni] = (joy_vec3_t){1,1,1};
                if (!inst->matrix_mode[ni]) {
                        if (nd->has_translation) inst->lcl_t[ni] = (joy_vec3_t){nd->translation[0],nd->translation[1],nd->translation[2]};
                        if (nd->has_rotation)   inst->lcl_r[ni] = (joy_vec4_t){nd->rotation[0],nd->rotation[1],nd->rotation[2],nd->rotation[3]};
                        if (nd->has_scale)      inst->lcl_s[ni] = (joy_vec3_t){nd->scale[0],nd->scale[1],nd->scale[2]};
                }
        }

        /* 2. 应用当前动画各 channel 的 TRS 采样 */
        an = (inst->anim_index < a->anim_count) ? &a->data->animations[inst->anim_index] : NULL;
        if (an) {
                for (c = 0; c < an->channels_count; c++) {
                        cgltf_animation_channel* ch = &an->channels[c];
                        if (!ch->target_node) continue;
                        ni = (int)(ch->target_node - a->data->nodes);
                        if (ni < 0 || ni >= inst->node_count) continue;
                        if (inst->matrix_mode[ni]) continue;
                        float out[8];
                        eval_sampler(ch->sampler, inst->time, 4, out);
                        switch (ch->target_path) {
                        case cgltf_animation_path_type_translation:
                                inst->lcl_t[ni] = (joy_vec3_t){out[0],out[1],out[2]}; break;
                        case cgltf_animation_path_type_rotation:
                                inst->lcl_r[ni] = (joy_vec4_t){out[0],out[1],out[2],out[3]}; break;
                        case cgltf_animation_path_type_scale:
                                inst->lcl_s[ni] = (joy_vec3_t){out[0],out[1],out[2]}; break;
                        default: break;
                        }
                }
        }

        /* 3. 合成各节点本地/世界矩阵 */
        for (ni = 0; ni < inst->node_count; ni++) {
                joy_mat44_t local;
                if (inst->matrix_mode[ni]) {
                        local = inst->static_mat[ni];
                } else {
                        joy_mat44_t T = mat44_translate(inst->lcl_t[ni].x, inst->lcl_t[ni].y, inst->lcl_t[ni].z);
                        joy_mat44_t R = quat_to_mat44(inst->lcl_r[ni]);
                        joy_mat44_t S = mat44_scale(inst->lcl_s[ni].x, inst->lcl_s[ni].y, inst->lcl_s[ni].z);
                        local = mat44_mul(T, mat44_mul(R, S));
                }
                cgltf_node* nd = inst->nodes[ni];
                if (nd->parent) {
                        int pni = (int)(nd->parent - a->data->nodes);
                        inst->node_world[ni] = mat44_mul(inst->node_world[pni], local);
                } else {
                        inst->node_world[ni] = local;
                }
        }

        /* 4. CPU 蒙皮 + 上传 */
        inst->moved_vertices = 0;
        for (ni = 0; ni < inst->prim_count; ni++) {
                joy_gltf_prim_src_t* src = &a->prims[ni];
                int vc = src->vertex_count;
                float* out = inst->skinned[ni];
                if (!src->skinned) continue;                /* 静态 primitive 无需更新 */

                /* 预计算每个 joint 的 skinMatrix = inverseBind * world（存临时数组, 不污染共享 source） */
                {
                        float* combo = (float*)malloc((size_t)(src->joint_count ? src->joint_count : 1) * 16 * sizeof(float));
                        for (k = 0; k < src->joint_count; k++) {
                                joy_mat44_t comb = mat44_mul(src->inverse_bind[k],
                                                         inst->node_world[src->joint_node[k]]);
                                memcpy(combo + k*16, (const float*)&comb, 16 * sizeof(float));
                        }
                        for (v = 0; v < vc; v++) {
                                float M[16]; int c4;
                                float bx,by,bz,nx,ny,nz;
                                memset(M, 0, sizeof(M));
                                const float* jp = src->jnt + v*4;
                                const float* wp = src->wts + v*4;
                                for (c4 = 0; c4 < 4; c4++) {
                                        int ji = (int)jp[c4];
                                        if (ji < 0 || ji >= src->joint_count) continue;
                                        float w = wp[c4];
                                        if (w <= 0.0f) continue;
                                        const float* cm = combo + ji*16;
                                        int i2;
                                        for (i2 = 0; i2 < 16; i2++) M[i2] += w * cm[i2];
                                }
                                bx = src->bind_verts[v*8+0]; by = src->bind_verts[v*8+1]; bz = src->bind_verts[v*8+2];
                                nx = src->bind_verts[v*8+3]; ny = src->bind_verts[v*8+4]; nz = src->bind_verts[v*8+5];
                                out[v*8+0] = M[0]*bx + M[4]*by + M[8]*bz  + M[12];
                                out[v*8+1] = M[1]*bx + M[5]*by + M[9]*bz  + M[13];
                                out[v*8+2] = M[2]*bx + M[6]*by + M[10]*bz + M[14];
                                out[v*8+3] = M[0]*nx + M[4]*ny + M[8]*nz;
                                out[v*8+4] = M[1]*nx + M[5]*ny + M[9]*nz;
                                out[v*8+5] = M[2]*nx + M[6]*ny + M[10]*nz;
                                out[v*8+6] = src->bind_verts[v*8+6];
                                out[v*8+7] = src->bind_verts[v*8+7];
                                if (fabsf(out[v*8+0]-bx) > 1e-3f ||
                                    fabsf(out[v*8+1]-by) > 1e-3f ||
                                    fabsf(out[v*8+2]-bz) > 1e-3f) inst->moved_vertices++;
                        }
                        free(combo);
                }
                joy_r3d_mesh_destroy(a->r3d, inst->gpu[ni]);
                inst->gpu[ni] = joy_r3d_mesh_custom_lit(a->r3d, out, vc);
        }
}

void jgltf_instance_draw(joy_r3d_p r3d, joy_gltf_instance_p inst, const joy_mat44_t* model) {
        int i;
        static int dbg_printed = 0;
        if (!inst || !r3d) return;
        if (!dbg_printed && inst->prim_count > 0) {
                float* v = inst->skinned[0];
                float* bv = inst->asset->prims[0].bind_verts;
                float mnx=1e9f,mny=1e9f,mnz=1e9f,mxx=-1e9f,mxy=-1e9f,mxz=-1e9f;
                float bmnx=1e9f,bmny=1e9f,bmnz=1e9f,bmxx=-1e9f,bmxy=-1e9f,bmxz=-1e9f;
                int vc = inst->asset->prims[0].vertex_count;
                int kk;
                for (kk = 0; kk < vc; kk++) {
                        float x=v[kk*8+0], y=v[kk*8+1], z=v[kk*8+2];
                        if (x<mnx)mnx=x; if (y<mny)mny=y; if (z<mnz)mnz=z;
                        if (x>mxx)mxx=x; if (y>mxy)mxy=y; if (z>mxz)mxz=z;
                        float bx=bv[kk*8+0], by=bv[kk*8+1], bz=bv[kk*8+2];
                        if (bx<bmnx)bmnx=bx; if (by<bmny)bmny=by; if (bz<bmnz)bmnz=bz;
                        if (bx>bmxx)bmxx=bx; if (by>bmxy)bmxy=by; if (bz>bmxz)bmxz=bz;
                }
                printf("[dbg-draw] prim0 vc=%d skinned_bbox=(%.3f,%.3f,%.3f)-(%.3f,%.3f,%.3f)\n",
                       vc, mnx,mny,mnz, mxx,mxy,mxz);
                printf("[dbg-draw] prim0 bind_bbox=(%.3f,%.3f,%.3f)-(%.3f,%.3f,%.3f)\n",
                       bmnx,bmny,bmnz, bmxx,bmxy,bmxz);
                if (inst->asset->prims[0].joint_count > 0) {
                        joy_mat44_t* ib = inst->asset->prims[0].inverse_bind;
                        printf("[dbg-draw] invbind0 m0=%.4f m5=%.4f m10=%.4f m12=%.4f m13=%.4f m14=%.4f\n",
                               ib[0].m0, ib[0].m5, ib[0].m10, ib[0].m12, ib[0].m13, ib[0].m14);
                        int jn0 = inst->asset->prims[0].joint_node[0];
                        joy_mat44_t* w = &inst->node_world[jn0];
                        printf("[dbg-draw] world0 m0=%.4f m5=%.4f m10=%.4f m12=%.4f m13=%.4f m14=%.4f\n",
                               w->m0, w->m5, w->m10, w->m12, w->m13, w->m14);
                }
                if (model) printf("[dbg-draw] model m0=%.3f m5=%.3f m10=%.3f m12=%.3f m13=%.3f m14=%.3f\n",
                                  model->m0, model->m5, model->m10, model->m12, model->m13, model->m14);
                dbg_printed = 1;
        }
        for (i = 0; i < inst->prim_count; i++) {
                if (!inst->gpu[i]) continue;
                void* tex = inst->asset->prims[i].tex;
                if (!tex) tex = inst->asset->white_tex;
                joy_r3d_draw_mesh_lit(r3d, inst->gpu[i], model, tex);
        }
}

/* 沿 skin 关节链画线段：每段 = 子关节世界位置 → 父关节世界位置（已按 model 变换到世界） */
void joy_r3d_gltf_instance_draw_bones(joy_r3d_p r3d, joy_gltf_instance_p inst, const joy_mat44_t* model) {
        joy_gltf_asset_p a; int j, k, seg; const joy_mat44_t* m;
        float* verts; int vcount;
        joy_mat44_t identity;
        joy_render_mesh_p mesh;

        if (!inst || !r3d || inst->prim_count <= 0) return;
        a = inst->asset;
        m = model ? model : &identity;

        /* 取第一个蒙皮 primitive 的关节表 */
        {
                const joy_gltf_prim_src_t* src = NULL;
                for (k = 0; k < inst->prim_count; k++) {
                        if (a->prims[k].skinned) { src = &a->prims[k]; break; }
                }
                if (!src || src->joint_count <= 0) return;

                seg = src->joint_count;
                verts = (float*)malloc((size_t)seg * 2 * 5 * sizeof(float));
                vcount = 0;
                for (j = 0; j < src->joint_count; j++) {
                        int node_i = src->joint_node[j];
                        int parent_i = -1;
                        cgltf_node* cy = inst->nodes[node_i];
                        float px, py, pz, cx, cy2, cz;
                        /* 子关节世界位置 */
                        const joy_mat44_t* w = &inst->node_world[node_i];
                        px = (*m).m0*(*w).m12 + (*m).m4*(*w).m13 + (*m).m8*(*w).m14 + (*m).m12;
                        py = (*m).m1*(*w).m12 + (*m).m5*(*w).m13 + (*m).m9*(*w).m14 + (*m).m13;
                        pz = (*m).m2*(*w).m12 + (*m).m6*(*w).m13 + (*m).m10*(*w).m14 + (*m).m14;
                        /* 父关节世界位置（无父则起点取关节自身, 画 0 长线） */
                        if (cy->parent) parent_i = (int)(cy->parent - a->data->nodes);
                        if (parent_i >= 0) {
                                const joy_mat44_t* pw = &inst->node_world[parent_i];
                                cx = (*m).m0*(*pw).m12 + (*m).m4*(*pw).m13 + (*m).m8*(*pw).m14 + (*m).m12;
                                cy2= (*m).m1*(*pw).m12 + (*m).m5*(*pw).m13 + (*m).m9*(*pw).m14 + (*m).m13;
                                cz = (*m).m2*(*pw).m12 + (*m).m6*(*pw).m13 + (*m).m10*(*pw).m14 + (*m).m14;
                        } else { cx = px; cy2 = py; cz = pz; }
                        float* o = verts + (size_t)vcount*5;
                        o[0]=cx; o[1]=cy2; o[2]=cz; o[3]=0; o[4]=0;
                        o[5]=px; o[6]=py; o[7]=pz; o[8]=0; o[9]=0;
                        vcount += 2;
                }
        }
        if (vcount <= 0) { free(verts); return; }

        /* 用 base 线框网格（5 float/顶点, pos+uv）画线段；销毁上一帧的旧网格 */
        mesh = joy_r3d_mesh_custom(r3d, verts, vcount);
        free(verts);
        if (!mesh) return;
        identity = mat44_identity();
        joy_r3d_draw_wire_basis(r3d, mesh, &identity, (joy_color_t){255, 80, 80, 255});
        if (inst->bones_mesh) joy_r3d_mesh_destroy(r3d, inst->bones_mesh);
        inst->bones_mesh = mesh;
}

int jgltf_instance_moved_vertices(joy_gltf_instance_p inst) { return inst ? inst->moved_vertices : 0; }

#endif /* JOY_USE_GLTF */

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef CGLTF_IMPLEMENTATION

/* ==================== 结束 src/jgltf.c ==================== */
