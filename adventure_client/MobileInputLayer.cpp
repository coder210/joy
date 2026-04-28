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
        
        // 初始化按钮状态追踪
        this->prevUpPressed = false;
        this->prevDownPressed = false;
        this->prevLeftPressed = false;
        this->prevRightPressed = false;
}

MobileInputLayer::~MobileInputLayer()
{
        button_destroy(this->upButton);
        button_destroy(this->downButton);
        button_destroy(this->leftButton);
        button_destroy(this->rightButton);
        button_destroy(this->attackButton);
}

void MobileInputLayer::SendInputEvent(MobileInput input)
{
        SDL_Event event;
        SDL_zero(event);
        event.type = MOBILE_INPUT_EVENT;
        event.user.code = input;
        SDL_PushEvent(&event);
}

void MobileInputLayer::CheckAndSendReleaseEvents()
{
        // 检测方向键松开事件
        if (this->prevUpPressed && !this->upButton->is_pressed) {
                SendInputEvent(MOBILE_INPUT_RELEASE_UP);
        }
        if (this->prevDownPressed && !this->downButton->is_pressed) {
                SendInputEvent(MOBILE_INPUT_RELEASE_DOWN);
        }
        if (this->prevLeftPressed && !this->leftButton->is_pressed) {
                SendInputEvent(MOBILE_INPUT_RELEASE_LEFT);
        }
        if (this->prevRightPressed && !this->rightButton->is_pressed) {
                SendInputEvent(MOBILE_INPUT_RELEASE_RIGHT);
        }
        
        // 更新上一帧状态
        this->prevUpPressed = this->upButton->is_pressed;
        this->prevDownPressed = this->downButton->is_pressed;
        this->prevLeftPressed = this->leftButton->is_pressed;
        this->prevRightPressed = this->rightButton->is_pressed;
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
        // 先检测并发送松开事件
        CheckAndSendReleaseEvents();
        
        // 发送按下事件
        if (this->upButton->is_pressed) {
                SendInputEvent(MOBILE_INPUT_UP);
        }

        if (this->downButton->is_pressed) {
                SendInputEvent(MOBILE_INPUT_DOWN);
        }

        if (this->leftButton->is_pressed) {
                SendInputEvent(MOBILE_INPUT_LEFT);
        }

        if (this->rightButton->is_pressed) {
                SendInputEvent(MOBILE_INPUT_RIGHT);
        }

        if (this->attackButton->is_pressed) {
                SendInputEvent(MOBILE_INPUT_ATTACK);
                this->attackButton->is_pressed = false;
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
