#define SDL_MAIN_USE_CALLBACKS 1
#include "flecs.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <joy2d/network.h>
#include <joy2d/core.h>
#include <joy2d/sys.h>
#include <joy2d/mathx.h>
#include "proto.h"
#include <iostream>
#include <map>
#include <list>
#include <string>
#include <cstring>      // for memcpy

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static kcpclient_p kcpclient = NULL;
static flecs::world world;
static std::map<int, int> local_inputs;   // 可保留用于积累输入（但当前版本直接发送）
static std::map<int, int> server_inputs;   // 可保留用于积累输入（但当前版本直接发送）

static bool ready;
static int server_frameid;
static std::string loaded_world_data;


// 方向键状态
static bool upPressed = false;
static bool downPressed = false;
static bool leftPressed = false;
static bool rightPressed = false;
static const fp_t MOVE_SPEED = fp_from_float(5.0f);

// 用于记录上一次发送的输入值，避免重复发送相同状态
static int lastSentInput = 0;
static int globalSequence = 0;

// 位掩码定义（每个方向占用一个独立位）
const int INPUT_UP = 1 << 0;   // 1
const int INPUT_DOWN = 1 << 1;   // 2
const int INPUT_LEFT = 1 << 2;   // 4
const int INPUT_RIGHT = 1 << 3;   // 8

static Uint64 lastTime = 0;
static float accumulator = 0.0f;
static const float FIXED_TIMESTEP = 1.0f / 60.0f;   // 60Hz固定步长

// 组件定义
struct LogicPosition { fp_t x, y; };
struct LogicVelocity { fp_t x, y; };

struct Position { float x, y; };
struct Player {};  // 标记组件，用于标识玩家实体

static void handle_cmd_loading(s2c_p s2c)
{
        log_info("C2S_CMD_LOADING");
        ready = true;
        //loaded_world_data.append(s2c->loading.data, s2c->loading.data_len);
        /* 初始化ecs世界 */

        /* 创建角色 */
        char data[JOY_MAX_BUFFER];
        int len;
        c2s_t c2s;
        c2s.cmd = C2S_CMD_PLAYER_JOIN;
        c2s.player_join.position_x = fp_from_float(1.2f);
        c2s.player_join.position_y = fp_from_float(2.3f);
        c2s_serialize(&c2s, data, &len);
        kcpclient_send(kcpclient, data, len);
}

// 网络消息回调（接收服务器消息）
static void msg_callback(net_message_p msg, void* userdata)
{
        if (msg->type == NET_TYPE_CONNECTED) {
                log_info("connected=%d", msg->conv);
                /* 发送准备包 */
                c2s_t c2s;
                int len;
                char buf[JOY_MAX_BUFFER];
                c2s.cmd = C2S_CMD_READY;
                c2s_serialize(&c2s, buf, &len);
                kcpclient_send(kcpclient, buf, len);
        }
        else if (msg->type == NET_TYPE_DISCONNECTED) {
                log_info("disconnected=%d", msg->conv);
        }
        else if (msg->type == NET_TYPE_MESSAGE) {
                log_info("msg=%s", msg->data);
                // 这里可以解析服务器下发的实体状态更新
                s2c_t s2c;
                if (s2c_deserialize(&s2c, msg->data, msg->len)) {
                        if (s2c.cmd == S2C_CMD_LOADING) {
                                handle_cmd_loading(&s2c);
                        }
                        else if (s2c.cmd == S2C_CMD_COMMAND) {
                                // 处理服务器命令（如玩家加入/离开、输入等）
                                for (const auto& player_input : s2c.command.player_inputs) {
                                        //server_inputs.insert({ player_input.sequence, player_input.keycode });
                                }
                        }
                }

        }
        SDL_free(msg->data);  // 释放消息数据内存
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
        sys_init_netenv();
        SDL_SetAppMetadata("Adventure client", "1.0", "com.adventure.client");

        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        if (!SDL_CreateWindowAndRenderer("adventure/client", 640, 480, 0, &window, &renderer)) {
                SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        // 连接到服务器（请根据实际情况修改IP和端口）
        log_info("client");
        kcpclient = kcpclient_create("192.168.1.13", 10000);
        kcpclient_set_callback(kcpclient, msg_callback, kcpclient);

        // 注册组件
        world.component<LogicPosition>();
        world.component<LogicVelocity>();
        world.component<Position>();
        world.component<Player>();

        // 注意：如果服务器会同步玩家实体，则客户端不需要主动创建。
        // 如需本地测试，可取消下面注释创建一个玩家实体
       /* world.entity()
                .set<LogicPosition>({ fp_from_float(320.0f), fp_from_float(240.0f) })
                .set<LogicVelocity>({ fp_from_float(0.0f), fp_from_float(0.0f) })
                .set<Position>({ 320.0f, 240.0f })
                .add<Player>();*/

        world.system<LogicVelocity, Player>()
                .interval(0.05f)
                .each([](LogicVelocity& v, Player& player) {
                /* 服务器输入 */
                for (auto rit = server_inputs.rbegin(); rit != server_inputs.rend(); ++rit) {
                        int sequence = rit->first;
                        int input = rit->second;
                        if (INPUT_UP == input) {
                                v.y = fp_sub(v.y, MOVE_SPEED);
                        }
                        else if (INPUT_DOWN == input) {
                                v.y = fp_add(v.y, MOVE_SPEED);
                        }
                        else if (INPUT_LEFT == input) {
                                v.x = fp_sub(v.x, MOVE_SPEED);
                        }
                        else if (INPUT_RIGHT == input) {
                                v.x = fp_add(v.x, MOVE_SPEED);
                        }
                }
                server_inputs.clear();

                /* 应用本地输入 */
                for (auto rit = local_inputs.rbegin(); rit != local_inputs.rend(); ++rit) {
                        int sequence = rit->first;
                        int input = rit->second;
                        if (INPUT_UP == input) {
                                v.y = fp_sub(v.y, MOVE_SPEED);
                        }
                        else if (INPUT_DOWN == input) {
                                v.y = fp_add(v.y, MOVE_SPEED);
                        }
                        else if (INPUT_LEFT == input) {
                                v.x = fp_sub(v.x, MOVE_SPEED);
                        }
                        else if (INPUT_RIGHT == input) {
                                v.x = fp_add(v.x, MOVE_SPEED);
                        }
                        //log_info("%d", *rit);

                        /* 同步到服务器 */
                        int len;
                        char buf[JOY_MAX_BUFFER];
                        c2s_t c2s;
                        c2s.cmd = C2S_CMD_PLAYER_INPUT;
                        c2s.player_input.sequence = sequence;
                        c2s.player_input.keycode = input;
                        c2s_serialize(&c2s, buf, &len);
                        kcpclient_send(kcpclient, buf, len);
                }
                local_inputs.clear();
                        });

        // 移动系统：每帧将速度加到位置
        world.system<LogicPosition, LogicVelocity>()
                .each([=](LogicPosition& p, LogicVelocity& v) {
                p.x = fp_add(p.x, v.x);
                p.y = fp_add(p.y, v.y);
                v.x = 0;
                v.y = 0;
                        });

        world.system<LogicPosition, Position>()
                .interval(0.02f)
                .each([=](LogicPosition& logic_position, Position& p) {
                p.x = fp_to_float(logic_position.x);
                p.y = fp_to_float(logic_position.y);
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

                // 将当前方向状态编码为一个整数（位掩码）
                int currentInput = 0;
                if (upPressed)    currentInput |= INPUT_UP;
                if (downPressed)  currentInput |= INPUT_DOWN;
                if (leftPressed)  currentInput |= INPUT_LEFT;
                if (rightPressed) currentInput |= INPUT_RIGHT;

                // 仅在输入状态发生变化时发送给服务器（避免重复发送相同值）
                if (currentInput != lastSentInput) {
                        // 转换为网络字节序后发送
                        //uint32_t netInput = htonl(static_cast<uint32_t>(currentInput));
                        //kcpclient_send(kcpclient, reinterpret_cast<const char*>(&netInput), sizeof(netInput));
                        lastSentInput = currentInput;

                        // 如果仍需保留到 local_inputs 列表，可以取消下面注释
                        local_inputs.insert({ globalSequence++, currentInput });
                }
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

        // 固定时间步长更新 ECS
        accumulator += deltaTime;
        while (accumulator >= FIXED_TIMESTEP) {
                world.progress(FIXED_TIMESTEP);
                accumulator -= FIXED_TIMESTEP;
        }

        // 处理网络事件（接收消息和更新连接状态）
        kcpclient_update(kcpclient);

        // 渲染
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // 绘制所有带有 Player 标记的实体（白色方块）
        world.query<Player, Position>().each([=](Player& player, Position& p) {
                SDL_FRect rect = { p.x - 15.0f, p.y - 15.0f, 30.0f, 30.0f };
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &rect);
                });

        SDL_RenderPresent(renderer);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        // 清理资源
        kcpclient_destroy(kcpclient);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        sys_release_netenv();
        // flecs 世界会在析构时自动释放资源
}

