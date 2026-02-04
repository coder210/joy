//#include "liv-joystick.h"
//#include <stdio.h>
//
//#define BACKGROUND_COLOR 0x333333FF
//
//int main(int argc, char* argv[]) {
//        // 初始化SDL
//        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//                SDL_Log("SDL初始化失败: %s", SDL_GetError());
//                return 1;
//        }
//
//        // 创建窗口
//        SDL_Window* window = SDL_CreateWindow("SDL3 Joystick (Adaptive)",
//                800, 600, SDL_WINDOW_RESIZABLE);
//
//        if (!window) {
//                SDL_Log("窗口创建失败: %s", SDL_GetError());
//                SDL_Quit();
//                return 1;
//        }
//
//        // 创建渲染器（设置逻辑分辨率）
//        SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
//        if (!renderer) {
//                SDL_Log("渲染器创建失败: %s", SDL_GetError());
//                SDL_DestroyWindow(window);
//                SDL_Quit();
//                return 1;
//        }
//
//        // 设置逻辑分辨率（自适应缩放的核心）
//        SDL_SetRenderLogicalPresentation(renderer, 800, 600, SDL_LOGICAL_PRESENTATION_LETTERBOX);
//
//        // 初始化左摇杆（在逻辑空间的固定位置）
//        joystick_p left_joystick;
//        left_joystick = joystick_create(renderer, 200.0f, 400.0f, 80.0f);
//
//        // 初始化右摇杆
//        joystick_p right_joystick;
//        right_joystick = joystick_create(renderer, 600.0f, 400.0f, 80.0f);
//
//        // 主循环
//        bool quit = false;
//        while (!quit) {
//                SDL_Event event;
//
//                // 处理事件
//                while (SDL_PollEvent(&event)) {
//                        if (event.type == SDL_EVENT_QUIT) {
//                                quit = true;
//                        }
//                        else if (event.type == SDL_EVENT_KEY_DOWN) {
//                                if (event.key.key == SDLK_ESCAPE) {
//                                        quit = true;
//                                }
//                                else if (event.key.key == SDLK_SPACE) {
//                                        // 打印摇杆状态
//                                        SDL_FPoint left_dir = joystick_get_direction(left_joystick);
//                                        SDL_FPoint right_dir = joystick_get_direction(&right_joystick);
//
//                                        printf("左摇杆: X=%.2f, Y=%.2f, 强度=%.2f\n",
//                                                left_dir.x, left_dir.y, joystick_get_magnitude(left_joystick));
//                                        printf("右摇杆: X=%.2f, Y=%.2f, 强度=%.2f\n",
//                                                right_dir.x, right_dir.y, joystick_get_magnitude(&right_joystick));
//                                }
//                        }
//
//                        // 将事件传递给两个摇杆
//                        joystick_handle_event(left_joystick, &event);
//                        joystick_handle_event(right_joystick, &event);
//                }
//
//                // 清除屏幕
//                SDL_SetRenderDrawColor(renderer,
//                        (BACKGROUND_COLOR >> 24) & 0xFF,
//                        (BACKGROUND_COLOR >> 16) & 0xFF,
//                        (BACKGROUND_COLOR >> 8) & 0xFF,
//                        BACKGROUND_COLOR & 0xFF);
//                SDL_RenderClear(renderer);
//
//                // 绘制摇杆
//                joystick_render(left_joystick);
//                joystick_render(right_joystick);
//
//                // 绘制方向指示线
//                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
//
//               /* if (joystick_get_magnitude(left_joystick) > 0) {
//                        SDL_FPoint dir = joystick_get_direction(left_joystick);
//                        SDL_RenderLine(renderer,
//                                left_joystick->center.x,
//                                left_joystick->center.y,
//                                left_joystick->center.x + dir.x * left_joystick->radius,
//                                left_joystick->center.y + dir.y * left_joystick->radius);
//                }
//
//                if (joystick_get_magnitude(&right_joystick) > 0) {
//                        SDL_FPoint dir = joystick_get_direction(&right_joystick);
//                        SDL_RenderLine(renderer,
//                                right_joystick.center.x,
//                                right_joystick.center.y,
//                                right_joystick.center.x + dir.x * right_joystick.radius,
//                                right_joystick.center.y + dir.y * right_joystick.radius);
//                }*/
//
//                // 绘制说明文字背景
//                SDL_FRect text_bg = { 10, 10, 350, 140 };
//                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x80);
//                SDL_RenderFillRect(renderer, &text_bg);
//
//                // 更新渲染
//                SDL_RenderPresent(renderer);
//
//                // 控制帧率
//                SDL_Delay(16);
//        }
//
//        // 清理资源
//        joystick_destroy(left_joystick);
//        joystick_destroy(right_joystick);
//        SDL_DestroyRenderer(renderer);
//        SDL_DestroyWindow(window);
//        SDL_Quit();
//
//        return 0;
//}