#define SDL_MAIN_USE_CALLBACKS
#define STB_IMAGE_IMPLEMENTATION
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "stb_image.h"
#include <chrono>
#include <cmath>    // 用于 sinf, cosf, tanf, sqrtf
#include <cstring>  // 用于 memcpy
#include <iostream>
#include <vector>

// -------------------- 自定义数学库 --------------------
struct Vec3 {
        float x, y, z;

        Vec3() : x(0), y(0), z(0) {}

        Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

        Vec3 operator+(const Vec3& v) const {
                return Vec3(x + v.x, y + v.y, z + v.z);
        }

        Vec3 operator-(const Vec3& v) const {
                return Vec3(x - v.x, y - v.y, z - v.z);
        }

        Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }

        Vec3 operator/(float s) const { return Vec3(x / s, y / s, z / s); }

        Vec3& operator+=(const Vec3& v) {
                x += v.x;
                y += v.y;
                z += v.z;
                return *this;
        }

        Vec3& operator-=(const Vec3& v) {
                x -= v.x;
                y -= v.y;
                z -= v.z;
                return *this;
        }

        Vec3& operator*=(float s) {
                x *= s;
                y *= s;
                z *= s;
                return *this;
        }

        Vec3& operator/=(float s) {
                x /= s;
                y /= s;
                z /= s;
                return *this;
        }
};

inline float dot(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 cross(const Vec3& a, const Vec3& b) {
        return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x);
}

inline float length(const Vec3& v) {
        return sqrtf(dot(v, v));
}

inline Vec3 normalize(const Vec3& v) {
        float len = length(v);
        return (len > 0) ? v / len : Vec3(0, 0, 0);
}

struct Mat4 {
        float m[16];  // 列主序

        // 默认构造单位矩阵
        Mat4() {
                for (int i = 0; i < 16; ++i) m[i] = 0;
                m[0] = m[5] = m[10] = m[15] = 1.0f;
        }

        // 从数组构造
        Mat4(const float* data) { memcpy(m, data, sizeof(m)); }

        // 矩阵乘法
        Mat4 operator*(const Mat4& other) const {
                Mat4 res;
                for (int col = 0; col < 4; ++col) {
                        for (int row = 0; row < 4; ++row) {
                                float sum = 0;
                                for (int k = 0; k < 4; ++k) {
                                        sum += m[col * 4 + k] * other.m[k * 4 + row];
                                }
                                res.m[col * 4 + row] = sum;
                        }
                }
                return res;
        }

        // 访问元素 (col, row)
        float& operator()(int col, int row) { return m[col * 4 + row]; }

        const float& operator()(int col, int row) const { return m[col * 4 + row]; }

        // ---------- 静态构造方法 ----------
        static Mat4 identity() { return Mat4(); }

        static Mat4 translate(const Vec3& t) {
                Mat4 mat;
                mat.m[12] = t.x;
                mat.m[13] = t.y;
                mat.m[14] = t.z;
                return mat;
        }

        static Mat4 rotateX(float angle) {
                Mat4 mat;
                float c = cosf(angle), s = sinf(angle);
                mat.m[5] = c;
                mat.m[6] = s;
                mat.m[9] = -s;
                mat.m[10] = c;
                return mat;
        }

        static Mat4 rotateY(float angle) {
                Mat4 mat;
                float c = cosf(angle), s = sinf(angle);
                mat.m[0] = c;
                mat.m[2] = -s;
                mat.m[8] = s;
                mat.m[10] = c;
                return mat;
        }

        static Mat4 rotateZ(float angle) {
                Mat4 mat;
                float c = cosf(angle), s = sinf(angle);
                mat.m[0] = c;
                mat.m[1] = s;
                mat.m[4] = -s;
                mat.m[5] = c;
                return mat;
        }

        // 透视投影矩阵 (列主序)
        static Mat4 perspective(float fov, float aspect, float near, float far) {
                float tanHalf = tanf(fov / 2.0f);
                Mat4 mat;
                mat.m[0] = 1.0f / (aspect * tanHalf);
                mat.m[5] = 1.0f / tanHalf;
                mat.m[10] = far / (near - far);
                mat.m[11] = -1.0f;
                mat.m[14] = (near * far) / (near - far);
                mat.m[15] = 0.0f;
                return mat;
        }

        // 观察矩阵 (lookAt)
        static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
                Vec3 f = normalize(center - eye);
                Vec3 s = normalize(cross(f, up));
                Vec3 u = cross(s, f);

                Mat4 mat;
                // 第一列
                mat.m[0] = s.x;
                mat.m[1] = s.y;
                mat.m[2] = s.z;
                mat.m[3] = 0;
                // 第二列
                mat.m[4] = u.x;
                mat.m[5] = u.y;
                mat.m[6] = u.z;
                mat.m[7] = 0;
                // 第三列
                mat.m[8] = -f.x;
                mat.m[9] = -f.y;
                mat.m[10] = -f.z;
                mat.m[11] = 0;
                // 第四列
                mat.m[12] = -dot(s, eye);
                mat.m[13] = -dot(u, eye);
                mat.m[14] = dot(f, eye);
                mat.m[15] = 1.0f;
                return mat;
        }
};

// 角度转弧度
inline float radians(float degrees) {
        return degrees * (3.14159265358979323846f / 180.0f);
}

// -------------------- 原代码（glm 部分已替换） --------------------

struct GPUShaderBundle {
        SDL_GPUShader* vertex{};
        SDL_GPUShader* fragment{};

        operator bool() const { return vertex && fragment; }
};

SDL_Window* gWindow = nullptr;
bool gShouldExit = false;

// SDL GPU resources
SDL_GPUDevice* gDevice = nullptr;
GPUShaderBundle gShaders;
SDL_GPUGraphicsPipeline* gGraphicsPipeline;
SDL_GPUBuffer* gPlaneVertexBuffer;
SDL_GPUTexture* gTexture;
SDL_GPUTexture* gDepthTexture;
SDL_GPUSampler* gSampler;

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 720

// 第一人称摄像机结构（使用自定义 Vec3）
struct FPSCamera {
        Vec3 position = Vec3(0.0f, 0.0f, 5.0f);
        Vec3 front = Vec3(0.0f, 0.0f, -1.0f);
        Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
        Vec3 right = Vec3(1.0f, 0.0f, 0.0f);
        Vec3 worldUp = Vec3(0.0f, 1.0f, 0.0f);

        float yaw = -90.0f;
        float pitch = 0.0f;

        float movementSpeed = 2.5f;
        float mouseSensitivity = 0.1f;
        float zoom = 45.0f;

        void updateCameraVectors() {
                Vec3 newFront;
                newFront.x = cosf(radians(yaw)) * cosf(radians(pitch));
                newFront.y = sinf(radians(pitch));
                newFront.z = sinf(radians(yaw)) * cosf(radians(pitch));
                front = normalize(newFront);

                right = normalize(cross(front, worldUp));
                up = normalize(cross(right, front));
        }

        Mat4 getViewMatrix() {
                return Mat4::lookAt(position, position + front, up);
        }

        void processMouseMovement(float xoffset, float yoffset,
                bool constrainPitch = true) {
                xoffset *= mouseSensitivity;
                yoffset *= mouseSensitivity;

                yaw += xoffset;
                pitch += yoffset;

                if (constrainPitch) {
                        if (pitch > 89.0f) pitch = 89.0f;
                        if (pitch < -89.0f) pitch = -89.0f;
                }
                updateCameraVectors();
        }

        void processKeyboard(int direction, float deltaTime) {
                float velocity = movementSpeed * deltaTime;
                switch (direction) {
                case 0:
                        position += front * velocity;
                        break;
                case 1:
                        position -= front * velocity;
                        break;
                case 2:
                        position -= right * velocity;
                        break;
                case 3:
                        position += right * velocity;
                        break;
                case 4:
                        position += worldUp * velocity;
                        break;
                case 5:
                        position -= worldUp * velocity;
                        break;
                }
        }
};

FPSCamera gCamera;

// 时间管理
std::chrono::high_resolution_clock::time_point gLastFrameTime;
float gDeltaTime = 0.0f;

// 输入状态
bool gKeys[6] = { false };  // W, S, A, D, Space, LShift

struct MVP {
        Mat4 proj;
        Mat4 model;
} gMVP;

float gRotateY = 0;
float gRotateX = 0;

bool initSDL() {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
                SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL init failed");
                return false;
        }

        gDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV,
                true, nullptr);
        if (!gDevice) {
                SDL_LogError(SDL_LOG_CATEGORY_GPU, "SDL create gpu gDevice failed: %s",
                        SDL_GetError());
                return false;
        }

        gWindow =
                SDL_CreateWindow("FPS Camera Demo", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
        if (!gWindow) {
                SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "SDL create gWindow failed");
                return false;
        }

        if (!SDL_ClaimWindowForGPUDevice(gDevice, gWindow)) {
                SDL_LogError(SDL_LOG_CATEGORY_GPU, "Your system don't support SDL GPU");
                return false;
        }

        gLastFrameTime = std::chrono::high_resolution_clock::now();
        return true;
}

SDL_GPUShader* loadSDLGPUShader(const char* filename, SDL_GPUShaderStage stage,
        uint32_t sampler_num, uint32_t uniform_buffer_num) {
        size_t file_size;
        Uint8* data = (Uint8*)SDL_LoadFile(filename, &file_size);
        if (!data) {
                SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "load file %s failed!: %s",
                        filename, SDL_GetError());
                return nullptr;
        }

        SDL_GPUShaderCreateInfo ci{};
        ci.code = data;
        ci.code_size = file_size;
        ci.entrypoint = "main";
        ci.format = SDL_GPU_SHADERFORMAT_SPIRV;
        ci.num_samplers = sampler_num;
        ci.num_uniform_buffers = uniform_buffer_num;
        ci.num_storage_buffers = 0;
        ci.num_storage_textures = 0;
        ci.stage = stage;

        SDL_GPUShader* shader = SDL_CreateGPUShader(gDevice, &ci);
        SDL_free(data);  // 无论成功与否，释放临时数据

        if (!shader) {
                SDL_LogError(SDL_LOG_CATEGORY_GPU,
                        "create gpu shader from %s failed: %s", filename,
                        SDL_GetError());
        }
        return shader;
}

GPUShaderBundle createSDLGPUShaderBundle() {
   /*     const char* dir = "shaders";
        SDL_Storage* storage = SDL_OpenFileStorage(dir);
        if (!storage) {
                SDL_LogError(
                        SDL_LOG_CATEGORY_SYSTEM,
                        "Open storage %s failed! You must run this program at root dir!",
                        dir);
                return {};
        }

        while (!SDL_StorageReady(storage)) {
                SDL_Delay(1);
        }*/

        GPUShaderBundle bundle;
        bundle.vertex =
                loadSDLGPUShader("shaders/vert.spv",SDL_GPU_SHADERSTAGE_VERTEX, 0, 1);
        bundle.fragment = loadSDLGPUShader("shaders/frag.spv", 
                SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0);
        //SDL_CloseStorage(storage);
        return bundle;
}

SDL_GPUGraphicsPipeline* createGraphicsPipeline() {
        SDL_GPUGraphicsPipelineCreateInfo ci{};

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
        ci.vertex_input_state.num_vertex_attributes = sizeof(attributes) / sizeof(attributes[0]);;

        SDL_GPUVertexBufferDescription buffer_desc;
        buffer_desc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
        buffer_desc.instance_step_rate = 0;
        buffer_desc.slot = 0;
        buffer_desc.pitch = sizeof(float) * 5;

        ci.vertex_input_state.num_vertex_buffers = 1;
        ci.vertex_input_state.vertex_buffer_descriptions = &buffer_desc;

        ci.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
        ci.vertex_shader = gShaders.vertex;
        ci.fragment_shader = gShaders.fragment;

        ci.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
        ci.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;

        ci.multisample_state.enable_mask = false;
        ci.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;

        ci.target_info.num_color_targets = 1;
        ci.target_info.has_depth_stencil_target = true;
        ci.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;

        SDL_GPUDepthStencilState state{};
        state.back_stencil_state.compare_op = SDL_GPU_COMPAREOP_NEVER;
        state.back_stencil_state.pass_op = SDL_GPU_STENCILOP_ZERO;
        state.back_stencil_state.fail_op = SDL_GPU_STENCILOP_ZERO;
        state.back_stencil_state.depth_fail_op = SDL_GPU_STENCILOP_ZERO;
        state.compare_op = SDL_GPU_COMPAREOP_LESS;
        state.enable_depth_test = true;
        state.enable_depth_write = true;
        state.enable_stencil_test = false;
        state.compare_mask = 0xFF;
        state.write_mask = 0xFF;
        ci.depth_stencil_state = state;

        SDL_GPUColorTargetDescription desc;
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
        desc.format = SDL_GetGPUSwapchainTextureFormat(gDevice, gWindow);

        ci.target_info.color_target_descriptions = &desc;

        return SDL_CreateGPUGraphicsPipeline(gDevice, &ci);
}

struct Vertex {
        float x, y, z;
        float u, v;
};

void createAndUploadVertexData() {
        // clang-format off
        Vertex vertices[] = {
            Vertex{-0.5f, -0.5f, -0.5f,  0.0f, 0.0f},
            Vertex{ 0.5f, -0.5f, -0.5f,  1.0f, 0.0f},
            Vertex{ 0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
            Vertex{ 0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
            Vertex{-0.5f,  0.5f, -0.5f,  0.0f, 1.0f},
            Vertex{-0.5f, -0.5f, -0.5f,  0.0f, 0.0f},

            Vertex{-0.5f, -0.5f,  0.5f,  0.0f, 0.0f},
            Vertex{ 0.5f, -0.5f,  0.5f,  1.0f, 0.0f},
            Vertex{ 0.5f,  0.5f,  0.5f,  1.0f, 1.0f},
            Vertex{ 0.5f,  0.5f,  0.5f,  1.0f, 1.0f},
            Vertex{-0.5f,  0.5f,  0.5f,  0.0f, 1.0f},
            Vertex{-0.5f, -0.5f,  0.5f,  0.0f, 0.0f},

            Vertex{-0.5f,  0.5f,  0.5f,  1.0f, 0.0f},
            Vertex{-0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
            Vertex{-0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
            Vertex{-0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
            Vertex{-0.5f, -0.5f,  0.5f,  0.0f, 0.0f},
            Vertex{-0.5f,  0.5f,  0.5f,  1.0f, 0.0f},

            Vertex{ 0.5f,  0.5f,  0.5f,  1.0f, 0.0f},
            Vertex{ 0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
            Vertex{ 0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
            Vertex{ 0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
            Vertex{ 0.5f, -0.5f,  0.5f,  0.0f, 0.0f},
            Vertex{ 0.5f,  0.5f,  0.5f,  1.0f, 0.0f},

            Vertex{-0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
            Vertex{ 0.5f, -0.5f, -0.5f,  1.0f, 1.0f},
            Vertex{ 0.5f, -0.5f,  0.5f,  1.0f, 0.0f},
            Vertex{ 0.5f, -0.5f,  0.5f,  1.0f, 0.0f},
            Vertex{-0.5f, -0.5f,  0.5f,  0.0f, 0.0f},
            Vertex{-0.5f, -0.5f, -0.5f,  0.0f, 1.0f},

            Vertex{-0.5f,  0.5f, -0.5f,  0.0f, 1.0f},
            Vertex{ 0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
            Vertex{ 0.5f,  0.5f,  0.5f,  1.0f, 0.0f},
            Vertex{ 0.5f,  0.5f,  0.5f,  1.0f, 0.0f},
            Vertex{-0.5f,  0.5f,  0.5f,  0.0f, 0.0f},
            Vertex{-0.5f,  0.5f, -0.5f,  0.0f, 1.0f}
        };
        //clang-format on

        SDL_GPUTransferBufferCreateInfo transfer_buffer_ci;
        transfer_buffer_ci.size = sizeof(vertices);
        transfer_buffer_ci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

        SDL_GPUTransferBuffer* transfer_buffer =
                SDL_CreateGPUTransferBuffer(gDevice, &transfer_buffer_ci);
        void* ptr = SDL_MapGPUTransferBuffer(gDevice, transfer_buffer, false);
        memcpy(ptr, &vertices, sizeof(vertices));
        SDL_UnmapGPUTransferBuffer(gDevice, transfer_buffer);

        SDL_GPUBufferCreateInfo gpu_buffer_ci;
        gpu_buffer_ci.size = sizeof(vertices);
        gpu_buffer_ci.usage = SDL_GPU_BUFFERUSAGE_VERTEX;

        gPlaneVertexBuffer = SDL_CreateGPUBuffer(gDevice, &gpu_buffer_ci);

        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(gDevice);
        SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd);
        SDL_GPUTransferBufferLocation location;
        location.offset = 0;
        location.transfer_buffer = transfer_buffer;
        SDL_GPUBufferRegion region;
        region.buffer = gPlaneVertexBuffer;
        region.offset = 0;
        region.size = sizeof(vertices);
        SDL_UploadToGPUBuffer(copy_pass, &location, &region, false);
        SDL_EndGPUCopyPass(copy_pass);

        SDL_SubmitGPUCommandBuffer(cmd);
        SDL_ReleaseGPUTransferBuffer(gDevice, transfer_buffer);
}

void createImageTexture() {
        int w, h;
        stbi_set_flip_vertically_on_load(true);

        // 使用 SDL_LoadFile 读取文件内容
        size_t file_size = 0;
        void* file_data = SDL_LoadFile("textures/logo.png", &file_size);
        if (!file_data) {
                SDL_Log("Failed to load file: textures/logo.png");
                return;
        }

        // 从内存解析图像
        unsigned char* data = stbi_load_from_memory(
                static_cast<const unsigned char*>(file_data),
                static_cast<int>(file_size),
                &w, &h, nullptr, STBI_rgb_alpha
        );
        SDL_free(file_data); // 释放文件数据

        if (!data) {
                SDL_Log("Failed to parse image from memory");
                return;
        }

        size_t image_size = 4 * w * h;

        SDL_GPUTransferBufferCreateInfo transfer_buffer_ci;
        transfer_buffer_ci.size = image_size;
        transfer_buffer_ci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

        SDL_GPUTransferBuffer* transfer_buffer =
                SDL_CreateGPUTransferBuffer(gDevice, &transfer_buffer_ci);
        void* ptr = SDL_MapGPUTransferBuffer(gDevice, transfer_buffer, false);
        memcpy(ptr, data, image_size);
        SDL_UnmapGPUTransferBuffer(gDevice, transfer_buffer);

        SDL_GPUTextureCreateInfo texture_ci;
        texture_ci.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        texture_ci.height = h;
        texture_ci.width = w;
        texture_ci.layer_count_or_depth = 1;
        texture_ci.num_levels = 1;
        texture_ci.sample_count = SDL_GPU_SAMPLECOUNT_1;
        texture_ci.type = SDL_GPU_TEXTURETYPE_2D;
        texture_ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;

        gTexture = SDL_CreateGPUTexture(gDevice, &texture_ci);

        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(gDevice);
        SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd);

        SDL_GPUTextureTransferInfo transfer_info;
        transfer_info.offset = 0;
        transfer_info.pixels_per_row = w;
        transfer_info.rows_per_layer = h;
        transfer_info.transfer_buffer = transfer_buffer;

        SDL_GPUTextureRegion region;
        region.w = w;
        region.h = h;
        region.x = 0;
        region.y = 0;
        region.layer = 0;
        region.mip_level = 0;
        region.z = 0;
        region.d = 1;
        region.texture = gTexture;

        SDL_UploadToGPUTexture(copy_pass, &transfer_info, &region, false);
        SDL_EndGPUCopyPass(copy_pass);

        SDL_SubmitGPUCommandBuffer(cmd);
        SDL_ReleaseGPUTransferBuffer(gDevice, transfer_buffer);
        stbi_image_free(data);
}

void createDepthTexture(int w, int h) {
        SDL_GPUTextureCreateInfo texture_ci;
        texture_ci.format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
        texture_ci.height = h;
        texture_ci.width = w;
        texture_ci.layer_count_or_depth = 1;
        texture_ci.num_levels = 1;
        texture_ci.sample_count = SDL_GPU_SAMPLECOUNT_1;
        texture_ci.type = SDL_GPU_TEXTURETYPE_2D;
        texture_ci.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;

        gDepthTexture = SDL_CreateGPUTexture(gDevice, &texture_ci);
}

void createSampler() {
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

        gSampler = SDL_CreateGPUSampler(gDevice, &ci);
}

void updateMVPData() {
        Vec3 cubePositions[] = {
            Vec3(0.0f, 0.0f, 0.0f),
            Vec3(2.0f, 0.0f, 0.0f),
            Vec3(-2.0f, 0.0f, 0.0f),
            Vec3(0.0f, 0.0f, 2.0f),
            Vec3(0.0f, 0.0f, -2.0f),
            Vec3(0.0f, 2.0f, 0.0f),
            Vec3(0.0f, -2.0f, 0.0f)
        };

        // 使用第一个立方体的变换
        gMVP.model = Mat4::identity();
        // 组合旋转：先绕Y再绕X（保持与原来意图一致）
       /* gMVP.model = Mat4::rotateY(radians(gRotateY)) *
                     Mat4::rotateX(radians(gRotateX)) *
                     gMVP.model;*/
        gMVP.model = Mat4::translate(cubePositions[0]);  // 平移

        // 乘以视图矩阵
        Mat4 view = gCamera.getViewMatrix();
        gMVP.model = view * gMVP.model;

        // 投影矩阵
        gMVP.proj = Mat4::perspective(radians(gCamera.zoom),
                (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
                0.1f, 100.0f);
}

void processInput() {
        const bool* keyboardState = SDL_GetKeyboardState(NULL);

        gKeys[0] = keyboardState[SDL_SCANCODE_W];
        gKeys[1] = keyboardState[SDL_SCANCODE_S];
        gKeys[2] = keyboardState[SDL_SCANCODE_A];
        gKeys[3] = keyboardState[SDL_SCANCODE_D];
        gKeys[4] = keyboardState[SDL_SCANCODE_SPACE];
        gKeys[5] = keyboardState[SDL_SCANCODE_LSHIFT];

        if (gKeys[0]) gCamera.processKeyboard(0, gDeltaTime);
        if (gKeys[1]) gCamera.processKeyboard(1, gDeltaTime);
        if (gKeys[2]) gCamera.processKeyboard(2, gDeltaTime);
        if (gKeys[3]) gCamera.processKeyboard(3, gDeltaTime);
        if (gKeys[4]) gCamera.processKeyboard(4, gDeltaTime);
        if (gKeys[5]) gCamera.processKeyboard(5, gDeltaTime);
}

// SDL main loop
SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
        if (!initSDL()) {
                return SDL_APP_FAILURE;
        }

        gShaders = createSDLGPUShaderBundle();
        if (!gShaders) {
                SDL_LogError(SDL_LOG_CATEGORY_GPU, "Shader load failed! Program exit!");
                return SDL_APP_FAILURE;
        }

        gGraphicsPipeline = createGraphicsPipeline();
        if (!gGraphicsPipeline) {
                SDL_LogError(SDL_LOG_CATEGORY_GPU, "Graphics pipeline load failed! %s",
                        SDL_GetError());
                return SDL_APP_FAILURE;
        }

        createAndUploadVertexData();
        createImageTexture();
        createDepthTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
        createSampler();

        gCamera.updateCameraVectors();
        updateMVPData();

        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        gDeltaTime = std::chrono::duration<float>(currentTime - gLastFrameTime).count();
        gLastFrameTime = currentTime;

        gRotateX += 20.0f * gDeltaTime;
        gRotateY += 30.0f * gDeltaTime;

        processInput();
        updateMVPData();

        bool is_minimized = SDL_GetWindowFlags(gWindow) & SDL_WINDOW_MINIMIZED;
        if (is_minimized) {
                return SDL_APP_CONTINUE;
        }

        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(gDevice);
        SDL_GPUTexture* swapchain_texture = nullptr;
        Uint32 width, height;

        if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, gWindow, &swapchain_texture,
                &width, &height)) {
                SDL_LogError(SDL_LOG_CATEGORY_GPU,
                        "SDL swapchain texture acquire failed! %s",
                        SDL_GetError());
        }

        if (!swapchain_texture) {
                return SDL_APP_CONTINUE;
        }

        SDL_GPUColorTargetInfo color_target_info{};
        color_target_info.clear_color.r = 0.1;
        color_target_info.clear_color.g = 0.1;
        color_target_info.clear_color.b = 0.1;
        color_target_info.clear_color.a = 1;
        color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        color_target_info.mip_level = 0;
        color_target_info.store_op = SDL_GPU_STOREOP_STORE;
        color_target_info.texture = swapchain_texture;
        color_target_info.cycle = true;
        color_target_info.layer_or_depth_plane = 0;
        color_target_info.cycle_resolve_texture = false;

        SDL_GPUDepthStencilTargetInfo depth_target_info{};
        depth_target_info.clear_depth = 1;
        depth_target_info.cycle = false;
        depth_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        depth_target_info.store_op = SDL_GPU_STOREOP_DONT_CARE;
        depth_target_info.texture = gDepthTexture;

        SDL_GPURenderPass* render_pass =
                SDL_BeginGPURenderPass(cmd, &color_target_info, 1, &depth_target_info);
        SDL_BindGPUGraphicsPipeline(render_pass, gGraphicsPipeline);

        SDL_GPUBufferBinding binding;
        binding.buffer = gPlaneVertexBuffer;
        binding.offset = 0;
        SDL_BindGPUVertexBuffers(render_pass, 0, &binding, 1);

        SDL_GPUTextureSamplerBinding sampler_binding;
        sampler_binding.texture = gTexture;
        sampler_binding.sampler = gSampler;
        SDL_BindGPUFragmentSamplers(render_pass, 0, &sampler_binding, 1);

        // 传递 uniform 数据（gMVP 包含 proj 和 model，顺序与着色器一致）
        SDL_PushGPUVertexUniformData(cmd, 0, &gMVP, sizeof(gMVP));

        int window_width, window_height;
        SDL_GetWindowSize(gWindow, &window_width, &window_height);

        SDL_GPUViewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.w = window_width;
        viewport.h = window_height;
        viewport.min_depth = 0;
        viewport.max_depth = 1;
        SDL_SetGPUViewport(render_pass, &viewport);
        SDL_DrawGPUPrimitives(render_pass, 36, 1, 0, 0);

        SDL_EndGPURenderPass(render_pass);

        if (!SDL_SubmitGPUCommandBuffer(cmd)) {
                SDL_LogError(SDL_LOG_CATEGORY_GPU,
                        "SDL submit command buffer failed! %s", SDL_GetError());
        }

        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
        if (event->type == SDL_EVENT_QUIT) {
                return SDL_APP_SUCCESS;
        }

        if (event->type == SDL_EVENT_MOUSE_MOTION) {
                static int lastX = WINDOW_WIDTH / 2;
                static int lastY = WINDOW_HEIGHT / 2;

                int x = event->motion.x;
                int y = event->motion.y;

                float xoffset = x - lastX;
                float yoffset = lastY - y; // Y轴反转

                lastX = x;
                lastY = y;

                gCamera.processMouseMovement(xoffset, yoffset);
        }

        if (event->type == SDL_EVENT_MOUSE_WHEEL) {
                gCamera.zoom -= event->wheel.y * 5.0f;
                if (gCamera.zoom < 1.0f) gCamera.zoom = 1.0f;
                if (gCamera.zoom > 90.0f) gCamera.zoom = 90.0f;
        }

        if (event->type == SDL_EVENT_KEY_DOWN) {
                if (event->key.key == SDLK_ESCAPE) {
                        return SDL_APP_SUCCESS;
                }
        }

        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
        SDL_WaitForGPUIdle(gDevice);

        SDL_ReleaseGPUSampler(gDevice, gSampler);
        SDL_ReleaseGPUTexture(gDevice, gTexture);
        SDL_ReleaseGPUTexture(gDevice, gDepthTexture);
        SDL_ReleaseGPUBuffer(gDevice, gPlaneVertexBuffer);
        SDL_ReleaseGPUGraphicsPipeline(gDevice, gGraphicsPipeline);
        SDL_ReleaseGPUShader(gDevice, gShaders.vertex);
        SDL_ReleaseGPUShader(gDevice, gShaders.fragment);
        SDL_ReleaseWindowFromGPUDevice(gDevice, gWindow);
        SDL_DestroyGPUDevice(gDevice);
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
}