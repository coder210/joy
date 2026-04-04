#include <SDL3/SDL.h>
#include <string.h>
#include "MobileInputLayer.h"

MobileInputLayer::MobileInputLayer(Resources* resources, SDL_Renderer* renderer)
{
        this->resources = resources;
        this->upButton = button_create(renderer, { 100, 200, 100, 100 });
        this->downButton = button_create(renderer, { 100, 400, 100, 100 });
        this->leftButton = button_create(renderer, { 0, 300, 100, 100 });
        this->rightButton = button_create(renderer, { 200, 300, 100, 100 });
        const char text[] = "up上up";
        int len = strlen(text);
        button_set_textx(this->upButton, resources->GetSimheiFont24(), text,
                len, { 255, 255, 255, 255 });
     /*   button_set_textx(this->downButton, resources->GetSimheiFont24(), "下",
                (int)strlen("下"), { 255, 255, 255, 255 });
        button_set_textx(this->leftButton, resources->GetSimheiFont24(), "左",
                (int)strlen("左"), { 255, 255, 255, 255 });
        button_set_textx(this->rightButton, resources->GetSimheiFont24(), "右",
                (int)strlen("右"), { 255, 255, 255, 255 });*/
}

MobileInputLayer::~MobileInputLayer()
{
        button_destroy(this->upButton);
        button_destroy(this->downButton);
        button_destroy(this->leftButton);
        button_destroy(this->rightButton);
}



void MobileInputLayer::Update()
{

}

void MobileInputLayer::Draw()
{
        button_draw(this->upButton);
        button_draw(this->downButton);
        button_draw(this->leftButton);
        button_draw(this->rightButton);
}
