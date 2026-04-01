#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "glad.h"
#include <math.h>
#include <cstdio>

static SDL_Window* g_window = nullptr;
static SDL_GLContext g_gl_ctx = nullptr;
static GLuint g_prog = 0;
static GLuint g_vao = 0;
static GLuint g_vbo = 0;
static GLuint g_ebo = 0;

static GLint uModelLoc, uViewLoc, uProjLoc;

// 矩阵工具函数
static void MatIdentity(float m[16]) {
        for (int i = 0; i < 16; i++) m[i] = 0;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
}

static void MatRotateX(float out[16], float rad) {
        MatIdentity(out);
        float c = cosf(rad), s = sinf(rad);
        out[5] = c; out[6] = s; out[9] = -s; out[10] = c;
}

static void MatRotateY(float out[16], float rad) {
        MatIdentity(out);
        float c = cosf(rad), s = sinf(rad);
        out[0] = c; out[2] = s; out[8] = -s; out[10] = c;
}

static void MatMul(float out[16], const float a[16], const float b[16]) {
        float res[16] = { 0 };
        for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                        res[i * 4 + j] = a[i * 4 + 0] * b[0 * 4 + j] +
                        a[i * 4 + 1] * b[1 * 4 + j] +
                        a[i * 4 + 2] * b[2 * 4 + j] +
                        a[i * 4 + 3] * b[3 * 4 + j];
        for (int i = 0; i < 16; i++) out[i] = res[i];
}

static void MatLookAt(float view[16], float eyeX, float eyeY, float eyeZ,
        float cenX, float cenY, float cenZ,
        float upX, float upY, float upZ) {
        float fx = cenX - eyeX, fy = cenY - eyeY, fz = cenZ - eyeZ;
        float len = sqrtf(fx * fx + fy * fy + fz * fz);
        fx /= len; fy /= len; fz /= len;
        float rx = fy * upZ - fz * upY, ry = fz * upX - fx * upZ, rz = fx * upY - fy * upX;
        len = sqrtf(rx * rx + ry * ry + rz * rz);
        rx /= len; ry /= len; rz /= len;
        float ux = ry * fz - rz * fy, uy = rz * fx - rx * fz, uz = rx * fy - ry * fx;
        MatIdentity(view);
        view[0] = rx; view[1] = ux; view[2] = -fx;
        view[4] = ry; view[5] = uy; view[6] = -fy;
        view[8] = rz; view[9] = uz; view[10] = -fz;
        view[12] = -(rx * eyeX + ry * eyeY + rz * eyeZ);
        view[13] = -(ux * eyeX + uy * eyeY + uz * eyeZ);
        view[14] = -(-fx * eyeX - fy * eyeY - fz * eyeZ);
}

// 标准透视矩阵，近平面设为 0.1，提高深度精度
static void MatPerspective(float proj[16], float fovRad, float aspect, float nearP, float farP) {
        MatIdentity(proj);
        float tanHalfFov = tanf(fovRad * 0.5f);
        proj[0] = 1.0f / (aspect * tanHalfFov);
        proj[5] = 1.0f / tanHalfFov;
        proj[10] = -(farP + nearP) / (farP - nearP);
        proj[11] = -1.0f;
        proj[14] = -(2.0f * farP * nearP) / (farP - nearP);
        proj[15] = 0.0f;
}

// 立方体顶点 (位置 + 颜色)
static const float cubeVerts[] = {
    -1,-1,-1, 1,0,0,
     1,-1,-1, 0,1,0,
     1, 1,-1, 1,1,0,
    -1, 1,-1, 0,0,1,
    -1,-1, 1, 1,0,1,
     1,-1, 1, 0,1,1,
     1, 1, 1, 1,1,1,
    -1, 1, 1, 0,0,0,
};
static const GLuint cubeIdx[] = {
    0,1,2, 0,2,3,  // 前面
    4,5,6, 4,6,7,  // 后面
    0,1,5, 0,5,4,  // 底面
    3,2,6, 3,6,7,  // 顶面
    1,2,6, 1,6,5,  // 右面
    0,3,7, 0,7,4   // 左面
};

static const char* vShaderSrc = R"(
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aColor;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
out vec3 vCol;
void main(){
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
    vCol = aColor;
}
)";

static const char* fShaderSrc = R"(
#version 330 core
in vec3 vCol;
out vec4 FragColor;
void main(){
    FragColor = vec4(vCol, 1.0);
}
)";

static GLuint CompileShader(GLenum type, const char* src) {
        GLuint sh = glCreateShader(type);
        glShaderSource(sh, 1, &src, NULL);
        glCompileShader(sh);
        GLint ok;
        glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
        if (!ok) {
                char buf[512];
                glGetShaderInfoLog(sh, 512, NULL, buf);
                SDL_Log("Shader compilation failed: %s", buf);
        }
        return sh;
}

static void InitShader() {
        GLuint vs = CompileShader(GL_VERTEX_SHADER, vShaderSrc);
        GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fShaderSrc);
        g_prog = glCreateProgram();
        glAttachShader(g_prog, vs);
        glAttachShader(g_prog, fs);
        glLinkProgram(g_prog);
        GLint ok;
        glGetProgramiv(g_prog, GL_LINK_STATUS, &ok);
        if (!ok) {
                char buf[512];
                glGetProgramInfoLog(g_prog, 512, NULL, buf);
                SDL_Log("Program link failed: %s", buf);
        }
        glDeleteShader(vs);
        glDeleteShader(fs);

        uModelLoc = glGetUniformLocation(g_prog, "uModel");
        uViewLoc = glGetUniformLocation(g_prog, "uView");
        uProjLoc = glGetUniformLocation(g_prog, "uProj");
}

static void InitBuffer() {
        glGenVertexArrays(1, &g_vao);
        glGenBuffers(1, &g_vbo);
        glGenBuffers(1, &g_ebo);

        glBindVertexArray(g_vao);

        glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIdx), cubeIdx, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
}

SDL_AppResult SDL_AppInit(void** app, int argc, char** argv) {
        // 初始化 SDL
        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("SDL_Init failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        // 设置 OpenGL 属性
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        // 创建窗口和 OpenGL 上下文
        g_window = SDL_CreateWindow("Rotating Cube - Fixed Depth", 800, 600,
                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if (!g_window) {
                SDL_Log("CreateWindow failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        g_gl_ctx = SDL_GL_CreateContext(g_window);
        if (!g_gl_ctx) {
                SDL_Log("CreateContext failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        // 加载 OpenGL 函数指针
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
                SDL_Log("gladLoadGLLoader failed");
                return SDL_APP_FAILURE;
        }

        InitShader();
        InitBuffer();

        // 启用深度测试，并确保深度函数为 LESS（默认即为 LESS，但显式声明）
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        // 启用背面剔除（可选），正面为逆时针
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

        SDL_Log("OpenGL version: %s", glGetString(GL_VERSION));
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* app, SDL_Event* event) {
        if (event->type == SDL_EVENT_QUIT)
                return SDL_APP_SUCCESS;
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* app) {
        static float angle = 0.0f;
        angle += 0.012f;  // 旋转速度适中

        // 清空颜色和深度缓冲区
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int w, h;
        SDL_GetWindowSize(g_window, &w, &h);
        glViewport(0, 0, w, h);

        glUseProgram(g_prog);

        // 计算模型矩阵：绕 X 轴和 Y 轴旋转
        float model[16], rx[16], ry[16];
        MatRotateX(rx, angle * 0.7f);
        MatRotateY(ry, angle * 0.5f);
        MatMul(model, rx, ry);

        // 视图矩阵：相机位置 (0, 0, 5)，观察原点，上方向 (0,1,0)
        float view[16];
        MatLookAt(view, 0, 0, 5.0f, 0, 0, 0, 0, 1, 0);

        // 投影矩阵：FOV = 60° (≈1.047rad)，近平面 0.1，远平面 100
        float proj[16];
        float aspect = (float)w / (float)h;
        MatPerspective(proj, 1.047f, aspect, 0.1f, 100.0f);

        glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, model);
        glUniformMatrix4fv(uViewLoc, 1, GL_FALSE, view);
        glUniformMatrix4fv(uProjLoc, 1, GL_FALSE, proj);

        glBindVertexArray(g_vao);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(g_window);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* app, SDL_AppResult result) {
        glDeleteVertexArrays(1, &g_vao);
        glDeleteBuffers(1, &g_vbo);
        glDeleteBuffers(1, &g_ebo);
        glDeleteProgram(g_prog);
        SDL_GL_DestroyContext(g_gl_ctx);
        SDL_DestroyWindow(g_window);
        SDL_Quit();
}