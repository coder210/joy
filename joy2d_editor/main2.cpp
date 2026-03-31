#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <math.h>

#define SCREEN_W 800
#define SCREEN_H 600

// 全局窗口渲染器
static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;

// 3D向量结构
typedef struct {
        float x, y, z;
} Vec3;

// 立方体原始顶点
static Vec3 cubeRaw[8] = {
    {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
    {-1,-1, 1}, {1,-1, 1}, {1,1, 1}, {-1,1, 1}
};

// 面顶点索引
static int faceList[6][4] = {
    {0,1,2,3},
    {4,5,6,7},
    {0,1,5,4},
    {3,2,6,7},
    {1,5,6,2},
    {0,4,7,3}
};

// 六个面颜色 RGBA
static float colors[6][4] = {
    {1,0,0,1},
    {0,1,0,1},
    {0,0,1,1},
    {1,1,0,1},
    {1,0,1,1},
    {0,1,1,1}
};

// 时间计时
static Uint64 startTick = 0;

// 3D旋转 + 透视投影
static Vec3 Project(Vec3 p, float time, float* outRawZ)
{
        float rx = time * 0.8f;
        float ry = time * 0.5f;

        // X轴旋转
        float y1 = p.y * cosf(rx) - p.z * sinf(rx);
        float z1 = p.y * sinf(rx) + p.z * cosf(rx);
        p.y = y1; p.z = z1;

        // Y轴旋转
        float x1 = p.x * cosf(ry) + p.z * sinf(ry);
        float zz = -p.x * sinf(ry) + p.z * cosf(ry);
        p.x = x1; p.z = zz;

        *outRawZ = p.z;

        // 透视投影
        float scale = 200.0f / (p.z + 4.0f);
        Vec3 res;
        res.x = p.x * scale + SCREEN_W / 2.0f;
        res.y = p.y * scale + SCREEN_H / 2.0f;
        res.z = 0;
        return res;
}

// SDL 初始化回调
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
        SDL_SetAppMetadata("SDL3 纯回调透视正方体", "1.0", "com.example.cube");

        if (!SDL_Init(SDL_INIT_VIDEO))
        {
                SDL_Log("SDL初始化失败: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        if (!SDL_CreateWindowAndRenderer("SDL3 透视旋转正方体",
                SCREEN_W, SCREEN_H, 0, &window, &renderer))
        {
                SDL_Log("窗口创建失败: %s", SDL_GetError());
                SDL_Quit();
                return SDL_APP_FAILURE;
        }

        startTick = SDL_GetTicks();
        return SDL_APP_CONTINUE;
}

// 事件回调
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
        if (event->type == SDL_EVENT_QUIT)
        {
                return SDL_APP_SUCCESS;
        }
        // ESC退出
        if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE)
        {
                return SDL_APP_SUCCESS;
        }
        return SDL_APP_CONTINUE;
}

// 每一帧迭代渲染回调
SDL_AppResult SDL_AppIterate(void* appstate)
{
        float time = (SDL_GetTicks() - startTick) / 1000.0f;

        // 清背景
        SDL_SetRenderDrawColor(renderer, 15, 15, 25, 255);
        SDL_RenderClear(renderer);

        // 遍历6个面渲染 + 背面剔除
        for (int f = 0; f < 6; f++)
        {
                int* idx4 = faceList[f];
                SDL_Vertex v[4];
                float avgZ = 0.0f;

                for (int i = 0; i < 4; i++)
                {
                        float rz;
                        Vec3 p = Project(cubeRaw[idx4[i]], time, &rz);
                        avgZ += rz;

                        v[i].position.x = p.x;
                        v[i].position.y = p.y;
                        v[i].color.r = colors[f][0];
                        v[i].color.g = colors[f][1];
                        v[i].color.b = colors[f][2];
                        v[i].color.a = colors[f][3];
                        v[i].tex_coord.x = 0;
                        v[i].tex_coord.y = 0;
                }
                avgZ /= 4.0f;

                // 背面剔除
                if (avgZ > 0.0f) continue;

                int triIdx[6] = { 0,1,2, 0,2,3 };
                SDL_RenderGeometry(renderer, nullptr, v, 4, triIdx, 6);
        }

        SDL_RenderPresent(renderer);
        return SDL_APP_CONTINUE;
}

// 退出清理
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
}