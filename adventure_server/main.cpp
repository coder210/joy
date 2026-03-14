#define SDL_MAIN_USE_CALLBACKS 1
#include "flecs.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <joy2d/calculator.h>
#include <joy2d/sys.h>
#include <joy2d/network.h>
#include <joy2d/log.h>
#include <iostream>
#include <map>
#include <list>
#include <string>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static kcpserver_p kcpserver = NULL;
static flecs::world world;


// 方向键状态
static bool upPressed = false;
static bool downPressed = false;
static bool leftPressed = false;
static bool rightPressed = false;
static const float MOVE_SPEED = 5.0f;

static Uint64 lastTime = 0;
static float accumulator = 0.0f;
static const float FIXED_TIMESTEP = 1.0f / 60.0f;   // 60Hz固定步长

// 组件定义
struct Position { float x, y; };
struct Velocity { float x, y; };
struct Player {};  // 标记组件，用于标识玩家实体


static void msg_callback(net_message_p msg, void* userdata)
{
        if (msg->type == NET_TYPE_CONNECTED) {
                log_info("connected=%d", msg->conv);
        }
        else if (msg->type == NET_TYPE_DISCONNECTED) {
                log_info("disconnected=%d", msg->conv);
        }
        else if (msg->type == NET_TYPE_MESSAGE) {
                log_info("msg=%s", msg->data);
                //std::string data(msg->data, msg->len);
                //std::cout << "Received message: " << data << std::endl;
        }
        SDL_free(msg->data);  // 记得释放消息数据的内存
}


SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
        sys_init_netenv();
        SDL_SetAppMetadata("Adventure server", "1.0", "com.example.adventure-server");

        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        if (!SDL_CreateWindowAndRenderer("adventure/server", 640, 480, 0, &window, &renderer)) {
                SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        log_info("server start");
        kcpserver = kcpserver_create("192.168.1.13", 10000);
        kcpserver_set_callback(kcpserver, msg_callback, kcpserver);

        // 注册组件
        world.component<Position>();
        world.component<Velocity>();
        world.component<Player>();

        // 创建玩家实体（小方块），添加 Player 标记
        world.entity()
                .set<Position>({ 320.0f, 240.0f })
                .set<Velocity>({ 0.0f, 0.0f })
                .add<Player>();

        // 移动系统：每帧将速度加到位置
        world.system<Position, Velocity>()
                .each([=](Position& p, Velocity& v) {
                p.x += v.x;
                p.y += v.y;
                        });

        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
        if (event->type == SDL_EVENT_QUIT) {
                return SDL_APP_SUCCESS;
        }

        if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
                bool isDown = (event->type == SDL_EVENT_KEY_DOWN);
                SDL_Keycode key = event->key.key;

                // 更新方向键状态
                switch (key) {
                case SDLK_W: upPressed = isDown; break;    // W -> 上
                case SDLK_S: downPressed = isDown; break;  // S -> 下
                case SDLK_A: leftPressed = isDown; break;  // A -> 左
                case SDLK_D: rightPressed = isDown; break; // D -> 右
                case SDLK_Q: if (isDown) return SDL_APP_SUCCESS; break; // Q退出
                default: break;
                }

                // 合成速度
                float vx = 0.0f, vy = 0.0f;
                if (rightPressed) vx += MOVE_SPEED;
                if (leftPressed)  vx -= MOVE_SPEED;
                if (downPressed)  vy += MOVE_SPEED;
                if (upPressed)    vy -= MOVE_SPEED;

                // 通过查询找到玩家实体并更新其速度
                world.query<Player, Velocity>().each([vx, vy](Player player, Velocity& vel) {
                        vel.x = vx;
                        vel.y = vy;
                        });
        }

        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
        // 计算时间差
        Uint64 currentTime = SDL_GetPerformanceCounter();
        if (lastTime == 0) {
                lastTime = currentTime;
        }
        float deltaTime = (float)((currentTime - lastTime) / (double)SDL_GetPerformanceFrequency());
        lastTime = currentTime;

        // 固定时间步长更新
        accumulator += deltaTime;
        while (accumulator >= FIXED_TIMESTEP) {
                world.progress(FIXED_TIMESTEP);   // 以固定步长驱动ECS系统
                accumulator -= FIXED_TIMESTEP;
        }

        // 更新网络服务器
        net_message_t msg;
        kcpserver_update(kcpserver);
  //      while (kcpserver_poll_message(kcpserver, &msg)) {
  //              //std::string data(msg.data, msg.len);
  //              if (msg.type == NET_TYPE_CONNECTED) {
  //                      log_info("connected=%d", msg.conv);
  //              }
  //              else if (msg.type == NET_TYPE_DISCONNECTED) {
  //                      log_info("disconnected=%d", msg.conv);
  //              }
  //              else if (msg.type == NET_TYPE_MESSAGE) {
  //                      //std::string data(msg.data, msg.len);
  //                      //std::cout << "Received message: " << data << std::endl;

  //              }
		//SDL_free(msg.data);  // 记得释放消息数据的内存
  //      }

        // 渲染部分保持不变
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        world.query<Player, Position>().each([=](Player t, Position& p) {
                SDL_FRect rect = { p.x - 15.0f, p.y - 15.0f, 30.0f, 30.0f };
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &rect);
                });

        SDL_RenderPresent(renderer);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        // flecs 世界会自动释放资源
        kcpserver_destroy(kcpserver);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        sys_release_netenv();
}