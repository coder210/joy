#include <SDL3/SDL.h>
#include <string.h>
#include "MobileInputLayer.h"


MobileInputLayer::MobileInputLayer(Resources* resources, SDL_Renderer* renderer)
{
        this->resources = resources;

        SDL_Rect logical_rect;
        SDL_GetRenderLogicalPresentation(renderer, &logical_rect.w, &logical_rect.h, NULL);
        int logical_w = logical_rect.w;
        int logical_h = logical_rect.h;

        // 边距（基于逻辑分辨率）
        const int left_margin = 10;
        const int bottom_margin = 60;
        const int right_margin = 10;

        const int dir_group_w = 100;
        const int dir_group_h = 100;
        const int attack_w = 60;
        const int attack_h = 60;

        // 方向键组左下角（逻辑坐标）
        float dir_x = left_margin;
        float dir_y = logical_h - dir_group_h - bottom_margin;  // 480-100-60=320

        // 攻击按钮右下角（逻辑坐标）
        float attack_x = logical_w - attack_w - right_margin;   // 640-60-10=570
        float attack_y = logical_h - attack_h - bottom_margin;  // 480-60-60=360

        // 创建按钮（使用逻辑坐标）
        this->upButton = button_create(renderer, { dir_x + 50, dir_y + 0,   50, 50 });
        this->downButton = button_create(renderer, { dir_x + 50, dir_y + 100, 50, 50 });
        this->leftButton = button_create(renderer, { dir_x + 0,  dir_y + 50,  50, 50 });
        this->rightButton = button_create(renderer, { dir_x + 100, dir_y + 50, 50, 50 });
        this->attackButton = button_create(renderer, { attack_x, attack_y, 60, 60 });

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
