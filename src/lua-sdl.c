#include "luaclib.h"
#include "SDL3/SDL.h"
#include "external/lua/lapi.h"
#include "external/lua/lauxlib.h"


static int lset_app_metadata(lua_State *L)
{
        const char* name, * version, * identifier;
        bool r;
        name = luaL_checkstring(L, 1);
        version = luaL_checkstring(L, 2);
        identifier = luaL_checkstring(L, 3);
        r = SDL_SetAppMetadata(name, version, identifier);
        lua_pushboolean(L, r);
        return 1;
}

static int lget_version(lua_State *L)
{
        int version;
        version = SDL_GetVersion();
        lua_pushinteger(L, version);
        return 3;
}

static int llog(lua_State *L)
{
        const char* str;
        str = luaL_checkstring(L, 1);
        SDL_Log("%s", str);
        return 0;
}

static int lerror(lua_State *L)
{
        const char* str;
        str = luaL_checkstring(L, 1);
        SDL_LogError(SDL_LOG_PRIORITY_ERROR, "%s", str);
        return 0;
}

static int ldelay(lua_State *L)
{
        Uint32 ms;
        ms = (Uint32)luaL_checkinteger(L, 1);
        SDL_Delay(ms);
        return 0;
}

static int lget_current_display_mode(lua_State *L)
{
        int index;
        const SDL_DisplayMode* mode;
        index = (int)luaL_checkinteger(L, 1);
        mode = SDL_GetCurrentDisplayMode(index);
        lua_pushnumber(L, mode->w);
        lua_pushnumber(L, mode->h);
        lua_pushnumber(L, mode->format);
        lua_pushnumber(L, mode->refresh_rate);
        return 4;
}

static int lget_ticks(lua_State *L)
{
        uint64_t ms;
        ms = SDL_GetTicks();
        lua_pushinteger(L, ms);
        return 1;
}

static int lcreate_texture_from_surface(lua_State *L)
{
        SDL_Renderer *renderer;
        const char* file;
        size_t len;
        SDL_Surface* surface;
        SDL_Texture* texture;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = lua_touserdata(L, 1);
        file = luaL_checklstring(L, 2, &len);
        surface = SDL_LoadBMP(file);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        lua_pushlightuserdata(L, texture);
        lua_pushinteger(L, surface->w);
        lua_pushinteger(L, surface->h);
        SDL_DestroySurface(surface);
        return 3;
}

static int lget_texture_size(lua_State *L)
{
        float w, h;
        SDL_Texture* texture;
        SDL_assert(lua_islightuserdata(L, 1));
        texture = lua_touserdata(L, 1);
        bool r = SDL_GetTextureSize(texture, &w, &h);
        lua_pushboolean(L, r);
        lua_pushnumber(L, w);
        lua_pushnumber(L, h);
        return 3;
}

static int lrender_texture_rotated(lua_State *L)
{
        SDL_Renderer *renderer;
        SDL_Texture* texture;
        SDL_FRect s_rect, d_rect;
        lua_Number angle;
        SDL_FPoint center;
        lua_Integer flip;
        bool r;
        SDL_assert(lua_islightuserdata(L, 1));
        SDL_assert(lua_islightuserdata(L, 2));
        luaL_checktype(L, 3, LUA_TTABLE);
        luaL_checktype(L, 4, LUA_TTABLE);
        luaL_checktype(L, 6, LUA_TTABLE);

        renderer = lua_touserdata(L, 1);
        texture = lua_touserdata(L, 2);

        lua_getfield(L, 3, "x");
        s_rect.x = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "y");
        s_rect.y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "h");
        s_rect.h = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 3, "w");
        s_rect.w = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 4, "x");
        d_rect.x = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 4, "y");
        d_rect.y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 4, "h");
        d_rect.h = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 4, "w");
        d_rect.w = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);

        angle = luaL_checknumber(L, 5);

        lua_getfield(L, 6, "x");
        center.x = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 6, "y");
        center.y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);

        flip = luaL_checkinteger(L, 7);
        r = SDL_RenderTextureRotated(renderer, texture,
                &s_rect, &d_rect, angle, &center, flip);
        lua_pushboolean(L, r);
        return 1;
}

static int ldestroy_texture(lua_State *L)
{
        SDL_Texture* texture;
        SDL_assert(lua_islightuserdata(L, 1));
        texture = lua_touserdata(L, 1);
        SDL_DestroyTexture(texture);
        return 0;
}

static int lrender_test(lua_State *L)
{
        SDL_Renderer *renderer;
        SDL_assert(lua_islightuserdata(L, 1));
        renderer = (SDL_Renderer*)lua_touserdata(L, 1);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 128, 0, SDL_ALPHA_OPAQUE);
        SDL_FRect rect;
        rect.x = rect.y = 0;
        rect.w = rect.h = 20;
        SDL_RenderFillRect(renderer, &rect);
        bool r = SDL_RenderPresent(renderer);
        lua_pushboolean(L, r);
        return 1;
}

static int lload_file(lua_State *L)
{
        const char* file;
        char* data;
        size_t data_size;

        file = luaL_checkstring(L, 1);
        data = (char*)SDL_LoadFile(file, &data_size);
        if (data_size)
        {
                lua_pushboolean(L, true);
                lua_pushlstring(L, data, data_size);
                SDL_free(data);
        }
        else
        {
                lua_pushboolean(L, false);
                lua_pushnil(L);
        }

        return 2;
}

static int lget_gpu_devices(lua_State *L)
{
        int num_gpu_drivers;
        const char* driver;
        num_gpu_drivers = SDL_GetNumGPUDrivers();
        lua_newtable(L);
        for (int i = 0; i < num_gpu_drivers; i++) {
                driver = SDL_GetGPUDriver(i);
                lua_pushstring(L, driver);
                lua_seti(L, -2, i + 1);
        }
        return 1;
}

static int lcreate_gpu_device(lua_State *L)
{
        bool debug_mode;
        /*const char* name;*/
        SDL_GPUDevice* device;
        debug_mode = lua_toboolean(L, 1);
        /* name = luaL_checkstring(L, 2); */
        device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV
                | SDL_GPU_SHADERFORMAT_DXIL
                | SDL_GPU_SHADERFORMAT_MSL,
                debug_mode, NULL);
        lua_pushlightuserdata(L, device);
        return 1;
}

static int lclaim_window_for_gpu_device(lua_State *L)
{
        bool ok;
        SDL_GPUDevice* device;
        SDL_Window* window;
        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
        window = (SDL_Window*)lua_touserdata(L, 2);
        ok = SDL_ClaimWindowForGPUDevice(device, window);
        lua_pushboolean(L, ok);
        return 1;
}

static int lcreate_vertex_shader(lua_State *L)
{
        SDL_GPUDevice* device;
        const char* filename;
        SDL_GPUShader* shader;
        Uint8* file_data;
        Uint64 data_size;
        SDL_GPUShaderCreateInfo ci;

        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
        filename = luaL_checkstring(L, 2);

        file_data = (Uint8*)SDL_LoadFile(filename, &data_size);
        if (file_data) {
                ci.code = file_data;
                ci.code_size = data_size;
                ci.entrypoint = "main";
                ci.format = SDL_GPU_SHADERFORMAT_SPIRV;
                ci.num_samplers = 0;
                ci.num_storage_buffers = 0;
                ci.num_storage_textures = 0;
                ci.num_uniform_buffers = 0;
                ci.stage = SDL_GPU_SHADERSTAGE_VERTEX;

                shader = SDL_CreateGPUShader(device, &ci);

                lua_pushlightuserdata(L, shader);
        }
        else {
                lua_pushnil(L);
        }

        return 1;
}

static int lcreate_fragment_shader(lua_State *L)
{
        SDL_GPUDevice* device;
        const char* filename;
        SDL_GPUShader* shader;
        Uint8* file_data;
        Uint64 data_size;
        SDL_GPUShaderCreateInfo ci;

        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
        filename = luaL_checkstring(L, 2);

        file_data = (Uint8*)SDL_LoadFile(filename, &data_size);
        if (file_data) {
                ci.code = file_data;
                ci.code_size = data_size;
                ci.entrypoint = "main";
                ci.format = SDL_GPU_SHADERFORMAT_SPIRV;
                ci.num_samplers = 1;
                ci.num_storage_buffers = 0;
                ci.num_storage_textures = 0;
                ci.num_uniform_buffers = 0;
                ci.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;

                shader = SDL_CreateGPUShader(device, &ci);

                lua_pushlightuserdata(L, shader);
        }
        else {
                lua_pushnil(L);
        }

        return 1;
}

static int lcreate_graphics_pipeline(lua_State *L)
{
        SDL_GPUDevice* device;
        SDL_Window* window;
        SDL_GPUShader* vertex, * fragment;
        SDL_GPUGraphicsPipelineCreateInfo ci;
        SDL_GPUVertexAttribute attributes[3];
        SDL_GPUVertexBufferDescription vertex_buffer_desc;
        SDL_GPUColorTargetDescription desc;
        SDL_GPUGraphicsPipeline* pipeline;

        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
        window = (SDL_Window*)lua_touserdata(L, 2);
        vertex = (SDL_GPUShader*)lua_touserdata(L, 3);
        fragment = (SDL_GPUShader*)lua_touserdata(L, 4);


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

        lua_pushlightuserdata(L, pipeline);

        return 1;
}


typedef struct vertex {
        float x, y;
        float r, g, b;
        float u, v;
}vertex_p;


static int lcreate_and_upload_vertex_data(lua_State *L)
{
        SDL_GPUDevice* device;
        SDL_GPUBuffer* vertex_buffer;
        SDL_GPUTransferBufferCreateInfo transfer_buffer_ci;
        SDL_GPUTransferBuffer* transfer_buffer;
        void* ptr;
        SDL_GPUBufferCreateInfo gpu_buffer_ci;
        SDL_GPUCommandBuffer* cmd;
        SDL_GPUCopyPass* copy_pass;
        SDL_GPUTransferBufferLocation location;
        SDL_GPUBufferRegion region;
        vertex_p* vertices;
        int vertices_len;

        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
        vertices_len = (int)lua_rawlen(L, 2);
        vertices = (vertex_p*)SDL_malloc(sizeof(vertex_p) * vertices_len);
        SDL_assert(vertices);
        for (int i = 0; i < vertices_len; i++) {
                lua_rawgeti(L, 2, i + 1);
                if (lua_istable(L, -1)) {
                        lua_rawgeti(L, -1, 1);
                        vertices[i].x = (float)lua_tonumber(L, -1);
                        lua_pop(L, 1);
                        lua_rawgeti(L, -1, 2);
                        vertices[i].y = (float)lua_tonumber(L, -1);
                        lua_pop(L, 1);
                        lua_rawgeti(L, -1, 3);
                        vertices[i].r = (float)lua_tonumber(L, -1);
                        lua_pop(L, 1);
                        lua_rawgeti(L, -1, 4);
                        vertices[i].g = (float)lua_tonumber(L, -1);
                        lua_pop(L, 1);
                        lua_rawgeti(L, -1, 5);
                        vertices[i].b = (float)lua_tonumber(L, -1);
                        lua_pop(L, 1);
                        lua_rawgeti(L, -1, 6);
                        vertices[i].u = (float)lua_tonumber(L, -1);
                        lua_pop(L, 1);
                        lua_rawgeti(L, -1, 7);
                        vertices[i].v = (float)lua_tonumber(L, -1);
                        lua_pop(L, 1);
                }
                lua_pop(L, 1);
        }

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
        lua_pushlightuserdata(L, vertex_buffer);
        return 1;
}

static int lcreate_and_upload_indices_data(lua_State *L)
{
        SDL_GPUDevice* device;
        SDL_GPUTransferBufferCreateInfo transfer_buffer_ci;
        SDL_GPUTransferBuffer* transfer_buffer;
        void* ptr;
        SDL_GPUBufferCreateInfo gpu_buffer_ci;
        SDL_GPUBuffer* indices_buffer;
        SDL_GPUCommandBuffer* cmd;
        SDL_GPUCopyPass* copy_pass;
        SDL_GPUTransferBufferLocation location;
        SDL_GPUBufferRegion region;
        uint32_t* indices;
        int indices_len;

        SDL_assert(lua_islightuserdata(L, 1));
        SDL_assert(lua_istable(L, 2));

        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
        indices_len = (int)luaL_len(L, 2);
        indices = (uint32_t*)SDL_malloc(sizeof(uint32_t) * indices_len);
        SDL_assert(indices);
        for (int i = 0; i < indices_len; i++) {
                lua_geti(L, 2, i + 1);
                indices[i] = (uint32_t)lua_tointeger(L, -1);
                lua_pop(L, 1);
        }

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

        lua_pushlightuserdata(L, indices_buffer);
        return 1;
}

//static int lcreate_gpu_texture(lua_State *L)
//{
//        int w, h;
//        unsigned char* image_data;
//        SDL_GPUTransferBufferCreateInfo transfer_buffer_ci;
//        SDL_GPUTransferBuffer* transfer_buffer;
//        void* ptr;
//        SDL_GPUTextureCreateInfo texture_ci;
//        SDL_GPUTexture* texture;
//        SDL_GPUCommandBuffer* cmd;
//        SDL_GPUCopyPass* copy_pass;
//        SDL_GPUTextureTransferInfo transfer_info;
//        SDL_GPUTextureRegion region;
//        SDL_GPUDevice* device;
//        const char* filename;
//
//        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
//        filename = luaL_checkstring(L, 2);
//
//        stbi_set_flip_vertically_on_load(true);
//
//        image_data = stbi_load(filename, &w, &h, NULL, STBI_rgb_alpha);
//
//        transfer_buffer_ci.size = 4 * w * h;
//        transfer_buffer_ci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
//
//        transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_buffer_ci);
//        ptr = SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
//        memcpy(ptr, image_data, transfer_buffer_ci.size);
//        SDL_UnmapGPUTransferBuffer(device, transfer_buffer);
//
//        texture_ci.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
//        texture_ci.height = h;
//        texture_ci.width = w;
//        texture_ci.layer_count_or_depth = 1;
//        texture_ci.num_levels = 1;
//        texture_ci.sample_count = SDL_GPU_SAMPLECOUNT_1;
//        texture_ci.type = SDL_GPU_TEXTURETYPE_2D;
//        texture_ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
//
//        texture = SDL_CreateGPUTexture(device, &texture_ci);
//
//        cmd = SDL_AcquireGPUCommandBuffer(device);
//        copy_pass = SDL_BeginGPUCopyPass(cmd);
//
//        transfer_info.offset = 0;
//        transfer_info.pixels_per_row = w;
//        transfer_info.rows_per_layer = h;
//        transfer_info.transfer_buffer = transfer_buffer;
//
//        region.w = w;
//        region.h = h;
//        region.x = 0;
//        region.y = 0;
//        region.layer = 0;
//        region.mip_level = 0;
//        region.z = 0;
//        region.d = 1;
//        region.texture = texture;
//
//        SDL_UploadToGPUTexture(copy_pass, &transfer_info, &region, false);
//        SDL_EndGPUCopyPass(copy_pass);
//        SDL_SubmitGPUCommandBuffer(cmd);
//        SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
//        stbi_image_free(image_data);
//
//        lua_pushlightuserdata(L, texture);
//
//        return 1;
//}

static int lcreate_gpu_sampler(lua_State *L)
{
        SDL_GPUSampler* sampler;
        SDL_GPUSamplerCreateInfo ci;
        SDL_GPUDevice* device;

        device = (SDL_GPUDevice*)lua_touserdata(L, 1);

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

        lua_pushlightuserdata(L, sampler);

        return 1;
}

static int lgpu_update(lua_State *L)
{
        SDL_GPUDevice* device;
        SDL_Window* window;
        SDL_GPUGraphicsPipeline* pipeline;
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
        SDL_GPUBuffer* vertex_buffer, * indices_buffer;
        SDL_GPUTexture* texture;
        SDL_GPUSampler* sampler;
        bool is_minimized;

        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
        window = (SDL_Window*)lua_touserdata(L, 2);
        pipeline = (SDL_GPUGraphicsPipeline*)lua_touserdata(L, 3);
        vertex_buffer = (SDL_GPUBuffer*)lua_touserdata(L, 4);
        indices_buffer = (SDL_GPUBuffer*)lua_touserdata(L, 5);
        texture = (SDL_GPUTexture*)lua_touserdata(L, 6);
        sampler = (SDL_GPUSampler*)lua_touserdata(L, 7);

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


static int lrelease_gpu_sampler(lua_State *L)
{
        SDL_GPUDevice* device;
        SDL_GPUSampler* sampler;
        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
        sampler = (SDL_GPUSampler*)lua_touserdata(L, 2);
        SDL_ReleaseGPUSampler(device, sampler);
        return 0;
}

static int lrelease_gpu_texture(lua_State *L)
{
        SDL_GPUDevice* device;
        SDL_GPUTexture* texture;
        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
        texture = (SDL_GPUTexture*)lua_touserdata(L, 2);
        SDL_ReleaseGPUTexture(device, texture);
        return 0;
}

static int lrelease_gpu_buffer(lua_State *L)
{
        SDL_GPUDevice* device;
        SDL_GPUBuffer* buffer;
        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
        buffer = (SDL_GPUBuffer*)lua_touserdata(L, 2);
        SDL_ReleaseGPUBuffer(device, buffer);
        return 0;
}

static int lrelease_gpu_graphics_pipeline(lua_State *L)
{
        SDL_GPUDevice* device;
        SDL_GPUGraphicsPipeline* graphics_pipeline;
        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
        graphics_pipeline = (SDL_GPUGraphicsPipeline*)lua_touserdata(L, 2);
        SDL_ReleaseGPUGraphicsPipeline(device, graphics_pipeline);
        return 0;
}

static int lrelease_gpu_shader(lua_State *L)
{
        SDL_GPUDevice* device;
        SDL_GPUShader* shader;
        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
        shader = (SDL_GPUShader*)lua_touserdata(L, 2);
        SDL_ReleaseGPUShader(device, shader);
        return 0;
}

static int lrelease_window_from_gpu_device(lua_State *L)
{
        SDL_GPUDevice* device;
        SDL_Window* window;
        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
        window = (SDL_Window*)lua_touserdata(L, 2);
        SDL_ReleaseWindowFromGPUDevice(device, window);
        return 0;
}

static int ldestroy_gpu_device(lua_State *L)
{
        SDL_GPUDevice* device;
        device = (SDL_GPUDevice*)lua_touserdata(L, 1);
        SDL_DestroyGPUDevice(device);
        return 0;
}

static int lget_mouse_state(lua_State *L)
{
        float x, y;
        SDL_GetMouseState(&x, &y);
        lua_pushnumber(L, x);
        lua_pushnumber(L, y);
        return 2;
}

int luaopen_sdl(lua_State *L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
            {"set_app_metadata", lset_app_metadata},
            {"get_version", lget_version},
            {"log", llog},
            {"error", lerror},
            {"delay", ldelay},
            {"get_current_display_mode", lget_current_display_mode},
            {"get_ticks", lget_ticks},
            {"create_texture_from_surface", lcreate_texture_from_surface},
            {"get_texture_size", lget_texture_size},
            {"render_texture_rotated", lrender_texture_rotated},
            {"destroy_texture", ldestroy_texture},
            {"render_test", lrender_test},
            {"load_file", lload_file},
            {"get_gpu_devices", lget_gpu_devices},
            {"create_gpu_device", lcreate_gpu_device},
            {"claim_window_for_gpu_device", lclaim_window_for_gpu_device},
            {"create_vertex_shader", lcreate_vertex_shader},
            {"create_fragment_shader", lcreate_fragment_shader},
            {"create_graphics_pipeline", lcreate_graphics_pipeline},
            {"create_and_upload_vertex_data", lcreate_and_upload_vertex_data},
            {"create_and_upload_indices_data", lcreate_and_upload_indices_data},
            {"create_gpu_sampler", lcreate_gpu_sampler},
            {"gpu_update", lgpu_update},
            {"release_gpu_sampler", lrelease_gpu_sampler},
            {"release_gpu_texture", lrelease_gpu_texture},
            {"release_gpu_buffer", lrelease_gpu_buffer},
            {"release_gpu_graphics_pipeline", lrelease_gpu_graphics_pipeline},
            {"release_gpu_shader", lrelease_gpu_shader},
            {"release_window_from_gpu_device", lrelease_window_from_gpu_device},
            {"destroy_gpu_device", ldestroy_gpu_device},
            {"get_mouse_state", lget_mouse_state},
            {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
