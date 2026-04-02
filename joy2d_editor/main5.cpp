//// main.cpp
//#include <SDL3/SDL.h>
//#include "glad.h"   // GLAD 提供的 OpenGL ES 2.0 头文件
//#include <cmath>
//#include <cstdio>
//
//// 全局变量
//SDL_Window* g_window = nullptr;
//SDL_GLContext g_glContext = nullptr;
//GLuint        g_program = 0;
//GLuint        g_vbo = 0;
//GLuint        g_ibo = 0;
//GLint         g_uRotationLoc = 0;
//GLint         g_uColorLoc = 0;
//float         g_angle = 0.0f;
//Uint64        g_lastTime = 0;
//bool          g_running = true;
//
//// 顶点数据 (两个三角形组成矩形)
//static const float vertices[] = {
//    -0.8f, -0.8f,   // 左下
//     0.8f, -0.8f,   // 右下
//    -0.8f,  0.8f,   // 左上
//     0.8f,  0.8f    // 右上
//};
//static const GLushort indices[] = {
//    0, 1, 2,   // 三角形1
//    1, 3, 2    // 三角形2
//};
//
//// 顶点着色器 (OpenGL ES 2.0)
//static const char* vertexShaderSource =
//"uniform float u_rotation; \n"
//"attribute vec2 a_position; \n"
//"void main() { \n"
//"    float c = cos(u_rotation); \n"
//"    float s = sin(u_rotation); \n"
//"    vec2 rotatedPos = vec2( \n"
//"        a_position.x * c - a_position.y * s, \n"
//"        a_position.x * s + a_position.y * c  \n"
//"    ); \n"
//"    gl_Position = vec4(rotatedPos, 0.0, 1.0); \n"
//"} \n";
//
//// 片段着色器 (OpenGL ES 2.0)
//static const char* fragmentShaderSource =
//"uniform vec4 u_color; \n"
//"void main() { \n"
//"    gl_FragColor = u_color; \n"
//"} \n";
//
//// 编译着色器
//GLuint compileShader(GLenum type, const char* source) {
//        GLuint shader = glCreateShader(type);
//        glShaderSource(shader, 1, &source, nullptr);
//        glCompileShader(shader);
//
//        GLint compiled;
//        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
//        if (!compiled) {
//                GLint infoLen = 0;
//                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
//                if (infoLen > 1) {
//                        char* infoLog = new char[infoLen];
//                        glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
//                        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Shader compile error: %s", infoLog);
//                        delete[] infoLog;
//                }
//                glDeleteShader(shader);
//                return 0;
//        }
//        return shader;
//}
//
//// 链接程序
//GLuint createProgram() {
//        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
//        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
//        if (!vertexShader || !fragmentShader) return 0;
//
//        GLuint program = glCreateProgram();
//        glAttachShader(program, vertexShader);
//        glAttachShader(program, fragmentShader);
//        glLinkProgram(program);
//
//        GLint linked;
//        glGetProgramiv(program, GL_LINK_STATUS, &linked);
//        if (!linked) {
//                GLint infoLen = 0;
//                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
//                if (infoLen > 1) {
//                        char* infoLog = new char[infoLen];
//                        glGetProgramInfoLog(program, infoLen, nullptr, infoLog);
//                        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Program link error: %s", infoLog);
//                        delete[] infoLog;
//                }
//                glDeleteProgram(program);
//                return 0;
//        }
//
//        glDeleteShader(vertexShader);
//        glDeleteShader(fragmentShader);
//        return program;
//}
//
//// 初始化 OpenGL 资源
//bool initGL() {
//        g_program = createProgram();
//        if (!g_program) return false;
//
//        glUseProgram(g_program);
//        g_uRotationLoc = glGetUniformLocation(g_program, "u_rotation");
//        g_uColorLoc = glGetUniformLocation(g_program, "u_color");
//
//        // 创建 VBO
//        glGenBuffers(1, &g_vbo);
//        glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//        // 获取顶点属性位置并启用
//        GLint posLoc = glGetAttribLocation(g_program, "a_position");
//        glEnableVertexAttribArray(posLoc);
//        glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
//
//        // 创建 IBO
//        glGenBuffers(1, &g_ibo);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
//        return true;
//}
//
//// 清理资源
//void cleanupGL() {
//        if (g_program) glDeleteProgram(g_program);
//        if (g_vbo) glDeleteBuffers(1, &g_vbo);
//        if (g_ibo) glDeleteBuffers(1, &g_ibo);
//}
//
//// 绘制一帧
//void renderFrame() {
//        glClear(GL_COLOR_BUFFER_BIT);
//        glUseProgram(g_program);
//
//        // 更新旋转角度
//        glUniform1f(g_uRotationLoc, g_angle);
//
//        // 动态颜色 (彩虹色)
//        float r = (sin(g_angle) + 1.0f) / 2.0f;
//        float g = (cos(g_angle * 0.7f) + 1.0f) / 2.0f;
//        float b = (sin(g_angle * 1.3f + 2.0f) + 1.0f) / 2.0f;
//        glUniform4f(g_uColorLoc, r, g, b, 1.0f);
//
//        // 绘制矩形 (两个三角形)
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
//
//        SDL_GL_SwapWindow(g_window);
//}
//
//// 主循环
//void mainLoop() {
//        g_lastTime = SDL_GetTicks();
//        while (g_running) {
//                SDL_Event event;
//                while (SDL_PollEvent(&event)) {
//                        if (event.type == SDL_EVENT_QUIT) {
//                                g_running = false;
//                        }
//                        else if (event.type == SDL_EVENT_KEY_DOWN) {
//                                if (event.key.key == SDLK_ESCAPE) g_running = false;
//                        }
//                }
//
//                Uint64 now = SDL_GetTicks();
//                float delta = (now - g_lastTime) / 1000.0f;
//                g_lastTime = now;
//                g_angle += delta * 1.5f;   // 旋转速度 1.5 弧度/秒
//
//                renderFrame();
//                SDL_Delay(16);  // 约 60 FPS
//        }
//}
//
//int main(int argc, char* argv[]) {
//        if (!SDL_Init(SDL_INIT_VIDEO)) {
//                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL_GetError());
//                return -1;
//        }
//
//        // 请求 OpenGL ES 2.0 上下文
//        g_window = SDL_CreateWindow("Rotating Rectangle - SDL3", 800, 600, SDL_WINDOW_OPENGL);
//        if (!g_window) {
//                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window creation failed: %s", SDL_GetError());
//                SDL_Quit();
//                return -1;
//        }
//
//        g_glContext = SDL_GL_CreateContext(g_window);
//        if (!g_glContext) {
//                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GL context creation failed: %s", SDL_GetError());
//                SDL_DestroyWindow(g_window);
//                SDL_Quit();
//                return -1;
//        }
//
//        // 关键: 使用 GLAD 加载 OpenGL ES 2.0 函数
//        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
//                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load OpenGL ES functions");
//                return -1;
//        }
//
//        const char* version = (const char*)glGetString(GL_VERSION);
//        SDL_Log("OpenGL ES version: %s", version ? version : "Unknown");
//
//        if (!initGL()) {
//                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "initGL failed");
//                return -1;
//        }
//
//        mainLoop();
//
//        cleanupGL();
//        SDL_GL_DestroyContext(g_glContext);
//        SDL_DestroyWindow(g_window);
//        SDL_Quit();
//        return 0;
//}