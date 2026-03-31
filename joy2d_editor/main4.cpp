#include <SDL3/SDL.h>

// ==============================================
// 核心：跨平台包含 GLES2 头文件
// Windows / Android / iOS / Linux 全部自动适配
// ==============================================
#include <SDL3/SDL_opengles2.h>

// 解决 __imp_glClear 链接错误（关键！）
#define GL_GLEXT_PROTOTYPES
#include <SDL3/SDL_opengles2_gl2ext.h>

int main(int argc, char* argv[])
{
        SDL_Init(SDL_INIT_VIDEO);

        // ==============================================
        // 统一设置：OpenGL ES 2.0（双平台都用这个！）
        // ==============================================
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        // 创建窗口
        SDL_Window* window = SDL_CreateWindow(
                "SDL3 GLES2 cross",
                800, 600,
                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        );

        // 创建 GLES2 上下文
        SDL_GLContext gl_ctx = SDL_GL_CreateContext(window);

        // 加载函数指针 → 彻底解决链接错误！
        SDL_GL_LoadLibrary(NULL);

        // 渲染循环
        int running = 1;
        SDL_Event event;

        while (running) {
                while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_EVENT_QUIT)
                                running = 0;
                }

                // GLES2 绘图
                glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                SDL_GL_SwapWindow(window);
        }

        // 清理
        SDL_GL_DestroyContext(gl_ctx);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
}