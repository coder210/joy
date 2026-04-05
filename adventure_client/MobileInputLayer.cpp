#include <SDL3/SDL.h>
#include <string.h>
#include "MobileInputLayer.h"


MobileInputLayer::MobileInputLayer(Resources* resources, SDL_Renderer* renderer)
{
        this->resources = resources;

        int w, h;
        SDL_GetRenderOutputSize(renderer, &w, &h);

        // 单独设置各边距（单位：像素）
        const int left_margin = 10;    // 方向键组距离左边距离
        const int bottom_margin = 60;  // 方向键组和攻击按钮距离下边距离
        const int right_margin = 10;   // 攻击按钮距离右边距离

        // 方向键组整体宽高（4个50x50按钮组成的十字布局）
        const int dir_group_w = 100;
        const int dir_group_h = 100;
        // 攻击按钮尺寸
        const int attack_w = 60;
        const int attack_h = 60;

        // 方向键组左下角锚点（X使用左边距，Y使用下边距）
        float dir_x = left_margin;
        float dir_y = h - dir_group_h - bottom_margin;

        // 攻击按钮右下角锚点（X使用右边距，Y使用下边距）
        float attack_x = w - attack_w - right_margin;
        float attack_y = h - attack_h - bottom_margin;

        // 创建按钮（相对坐标基于锚点）
        this->upButton = button_create(renderer, { dir_x + 50, dir_y + 0,   50, 50 });
        this->downButton = button_create(renderer, { dir_x + 50, dir_y + 100, 50, 50 });
        this->leftButton = button_create(renderer, { dir_x + 0,  dir_y + 50,  50, 50 });
        this->rightButton = button_create(renderer, { dir_x + 100, dir_y + 50, 50, 50 });
        this->attackButton = button_create(renderer, { attack_x, attack_y, (float)attack_w, (float)attack_h });

        // 设置按钮文字
        button_set_textx(this->upButton, resources->GetSimheiFont24(), "U",
                (int)strlen("U"), { 255, 255, 255, 255 });
        button_set_textx(this->downButton, resources->GetSimheiFont24(), "D",
                (int)strlen("D"), { 255, 255, 255, 255 });
        button_set_textx(this->leftButton, resources->GetSimheiFont24(), "L",
                (int)strlen("L"), { 255, 255, 255, 255 });
        button_set_textx(this->rightButton, resources->GetSimheiFont24(), "R",
                (int)strlen("R"), { 255, 255, 255, 255 });
        button_set_textx(this->attackButton, resources->GetSimheiFont24(), "A",
                (int)strlen("A"), { 255, 255, 255, 255 });
}

MobileInputLayer::~MobileInputLayer()
{
        button_destroy(this->upButton);
        button_destroy(this->downButton);
        button_destroy(this->leftButton);
        button_destroy(this->rightButton);
        button_destroy(this->attackButton);
}



void MobileInputLayer::ListenEvent(SDL_Event* e)
{
        button_handle_event(this->upButton, e);
        button_handle_event(this->downButton, e);
        button_handle_event(this->leftButton, e);
        button_handle_event(this->rightButton, e);
        button_handle_event(this->attackButton, e);
}


void MobileInputLayer::Update()
{
        if (this->upButton->is_pressed) {
                SDL_Event event;
                SDL_zero(event);
                event.type = MOBILE_INPUT_EVENT;
                event.user.code = MOBILE_INPUT_UP;
                SDL_PushEvent(&event);
        }

        if (this->downButton->is_pressed) {
                SDL_Event event;
                SDL_zero(event);
                event.type = MOBILE_INPUT_EVENT;
                event.user.code = MOBILE_INPUT_DOWN;
                SDL_PushEvent(&event);
        }

        if (this->leftButton->is_pressed) {
                SDL_Event event;
                SDL_zero(event);
                event.type = MOBILE_INPUT_EVENT;
                event.user.code = MOBILE_INPUT_LEFT;
                SDL_PushEvent(&event);
        }

        if (this->rightButton->is_pressed) {
                SDL_Event event;
                SDL_zero(event);
                event.type = MOBILE_INPUT_EVENT;
                event.user.code = MOBILE_INPUT_RIGHT;
                SDL_PushEvent(&event);
        }

        if (this->attackButton->is_pressed) {
                SDL_Event event;
                SDL_zero(event);
                event.type = MOBILE_INPUT_EVENT;
                event.user.code = MOBILE_INPUT_ATTACK;
                SDL_PushEvent(&event);
                this->attackButton->is_pressed = false;
        }

        if (!this->attackButton->is_pressed && !this->upButton->is_pressed && !this->downButton->is_pressed
                && !this->leftButton->is_pressed && !this->rightButton->is_pressed) {
                SDL_Event event;
                SDL_zero(event);
                event.type = MOBILE_INPUT_EVENT;
                event.user.code = MOBILE_INPUT_NONE;
                SDL_PushEvent(&event);
        }
}

void MobileInputLayer::Draw()
{
        button_draw(this->upButton);
        button_draw(this->downButton);
        button_draw(this->leftButton);
        button_draw(this->rightButton);
        button_draw(this->attackButton);
}
