#define SDL_MAIN_USE_CALLBACKS 1   // 启用回调模式
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>          // 回调函数所需的头文件
#include <SDL3/SDL_opengl.h>
#include <math.h>

// 保持原始透视投影函数不变
static void perspective(float fovy, float aspect, float zNear, float zFar) {
        float f = 1.0f / tanf(fovy * 3.14159f / 360.0f);
        float dz = zFar - zNear;
        float proj[16] = {
            f / aspect, 0, 0, 0,
            0, f, 0, 0,
            0, 0, -(zFar + zNear) / dz, -1,
            0, 0, -2 * zFar * zNear / dz, 0
        };
        glMultMatrixf(proj);
}

// 全局/静态变量，用于在回调函数间共享状态
static SDL_Window* window = NULL;
static SDL_GLContext glCtx = NULL;
static float angle = 0.0f;

// 初始化回调：创建窗口、OpenGL上下文，设置初始状态
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
        // 初始化 SDL 视频子系统
        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("SDL_Init failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        // 设置 OpenGL 属性
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        // 创建窗口和 OpenGL 上下文
        window = SDL_CreateWindow("SDL Cube OpenGL", 800, 600, SDL_WINDOW_OPENGL);
        if (!window) {
                SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }
        glCtx = SDL_GL_CreateContext(window);
        if (!glCtx) {
                SDL_Log("SDL_GL_CreateContext failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        // ========== 修复 1：正确开启深度测试 ==========
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // ========== 修复 2：开启背面剔除（关键！） ==========
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        // ========== 修复 3：修正深度清除 ==========
        glClearDepth(1.0f);

        // 设置视口
        glViewport(0, 0, 800, 600);

        // 开启垂直同步
        SDL_GL_SetSwapInterval(1);

        // 返回成功
        return SDL_APP_CONTINUE;
}

// 事件回调：处理退出等事件
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
        if (event->type == SDL_EVENT_QUIT) {
                return SDL_APP_SUCCESS;   // 请求退出程序
        }
        return SDL_APP_CONTINUE;       // 继续运行
}

// 迭代回调：每帧渲染
SDL_AppResult SDL_AppIterate(void* appstate) {
        // ========== 修复 4：必须同时清除颜色 + 深度缓冲 ==========
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 投影矩阵
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        perspective(90.0f, 800.0f / 600.0f, 0.1f, 100.0f);

        // 相机矩阵
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0, 0, -5.0f);
        glRotatef(angle * 0.7f, 1, 1, 0);

        // 绘制立方体（顶点顺序未变，保持逆时针）
        glBegin(GL_QUADS);
        // 前 红
        glColor3f(1, 0, 0);
        glVertex3f(-1, -1, 1); glVertex3f(1, -1, 1); glVertex3f(1, 1, 1); glVertex3f(-1, 1, 1);
        // 后 蓝
        glColor3f(0, 0, 1);
        glVertex3f(-1, -1, -1); glVertex3f(-1, 1, -1); glVertex3f(1, 1, -1); glVertex3f(1, -1, -1);
        // 左 绿
        glColor3f(0, 1, 0);
        glVertex3f(-1, -1, -1); glVertex3f(-1, -1, 1); glVertex3f(-1, 1, 1); glVertex3f(-1, 1, -1);
        // 右 黄
        glColor3f(1, 1, 0);
        glVertex3f(1, -1, 1); glVertex3f(1, -1, -1); glVertex3f(1, 1, -1); glVertex3f(1, 1, 1);
        // 上 青
        glColor3f(0, 1, 1);
        glVertex3f(-1, 1, 1); glVertex3f(1, 1, 1); glVertex3f(1, 1, -1); glVertex3f(-1, 1, -1);
        // 下 紫
        glColor3f(1, 0, 1);
        glVertex3f(-1, -1, 1); glVertex3f(-1, -1, -1); glVertex3f(1, -1, -1); glVertex3f(1, -1, 1);
        glEnd();

        // 更新旋转角度
        angle += 0.5f;

        // 交换前后缓冲
        SDL_GL_SwapWindow(window);

        return SDL_APP_CONTINUE;
}

// 退出回调：清理资源
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
        if (glCtx) {
                SDL_GL_DestroyContext(glCtx);
        }
        if (window) {
                SDL_DestroyWindow(window);
        }
        SDL_Quit();
}