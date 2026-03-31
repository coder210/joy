#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

int main(int argc, char* argv[])
{
        // 1. 初始化SDL
        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("SDL初始化失败: %s", SDL_GetError());
                return -1;
        }

        // 2. 配置OpenGL
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        // 3. 创建窗口
        SDL_Window* window = SDL_CreateWindow(
                "SDL3 + OpenGL 纯净版",
                800, 600,
                SDL_WINDOW_OPENGL
        );
        if (!window) {
                SDL_Log("窗口创建失败: %s", SDL_GetError());
                SDL_Quit();
                return -1;
        }

        // 4. 创建GL上下文
        SDL_GLContext ctx = SDL_GL_CreateContext(window);
        if (!ctx) {
                SDL_Log("GL上下文创建失败");
                SDL_DestroyWindow(window);
                SDL_Quit();
                return -1;
        }

        // 5. 开启垂直同步
        SDL_GL_SetSwapInterval(1);

        // 6. 主循环
        SDL_Event e;
        int running = 1;
        while (running) {
                while (SDL_PollEvent(&e)) {
                        if (e.type == SDL_EVENT_QUIT)
                                running = 0;
                }

                // 清屏
                glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                // 交换缓冲
                SDL_GL_SwapWindow(window);
        }

        // 7. 清理
        SDL_GL_DestroyContext(ctx);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
}