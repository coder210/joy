#include <SDL3/SDL.h>
#include <string.h>
#include "MobileInputLayer.h"


MobileInputLayer::MobileInputLayer(Resources* resources, SDL_Renderer* renderer)
{
        this->resources = resources;
        // 大小改为 50x50，保持中心点不变
        float offset_x = 0;
        float offset_y = 300;

        this->upButton = button_create(renderer, { 50 + offset_x, 0 + offset_y, 50, 50 });
        this->downButton = button_create(renderer, { 50 + offset_x, 100 + offset_y, 50, 50 });
        this->leftButton = button_create(renderer, { 0 + offset_x, 50 + offset_y, 50, 50 });
        this->rightButton = button_create(renderer, { 100 + offset_x, 50 + offset_y, 50, 50 });
        this->attackButton = button_create(renderer, { 500 + offset_x, 50 + offset_y, 60, 60 });

        const char* text = "U";
        button_set_textx(this->upButton, resources->GetSimheiFont24(), text,
                (int)strlen(text), { 255, 255, 255, 255 });
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
