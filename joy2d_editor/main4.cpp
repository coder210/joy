#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// ==============================================
// 核心：跨平台包含 GLES2 头文件
// Windows / Android / iOS / Linux 全部自动适配
// ==============================================
#include <SDL3/SDL_opengles2.h>

// 解决 __imp_glClear 链接错误（关键！）
#define GL_GLEXT_PROTOTYPES
#include <SDL3/SDL_opengles2_gl2ext.h>

// 全局资源，供回调函数使用
static SDL_Window* g_window = NULL;
static SDL_GLContext g_gl_ctx = NULL;

// 初始化回调
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
        (void)argc;
        (void)argv;
        *appstate = NULL; // 此示例不需要 appstate

        // 初始化 SDL 视频子系统
        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("SDL_Init failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        // ==============================================
        // 统一设置：OpenGL ES 2.0（双平台都用这个！）
        // ==============================================
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        // 创建窗口
        g_window = SDL_CreateWindow(
                "SDL3 GLES2 cross (callbacks)",
                800, 600,
                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        );
        if (!g_window) {
                SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        // 创建 GLES2 上下文
        g_gl_ctx = SDL_GL_CreateContext(g_window);
        if (!g_gl_ctx) {
                SDL_Log("SDL_GL_CreateContext failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        // 加载函数指针 → 彻底解决链接错误！
        SDL_GL_LoadLibrary(NULL);

        return SDL_APP_CONTINUE; // 成功初始化
}

// 事件处理回调
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
        (void)appstate;

        if (event->type == SDL_EVENT_QUIT) {
                return SDL_APP_SUCCESS; // 收到退出事件，正常结束
        }

        // 其他事件可在此处理，返回 SDL_APP_CONTINUE 继续运行
        return SDL_APP_CONTINUE;
}

// 主循环迭代回调
SDL_AppResult SDL_AppIterate(void* appstate)
{
        (void)appstate;

        // GLES2 绘图
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(g_window);

        return SDL_APP_CONTINUE; // 继续运行
}

// 退出清理回调
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        (void)appstate;
        (void)result;

        // 清理资源
        if (g_gl_ctx) {
                SDL_GL_DestroyContext(g_gl_ctx);
                g_gl_ctx = NULL;
        }
        if (g_window) {
                SDL_DestroyWindow(g_window);
                g_window = NULL;
        }
        SDL_Quit();
}