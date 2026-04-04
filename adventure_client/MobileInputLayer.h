#ifndef MOBILE_INPUT_LAYER_H
#define MOBILE_INPUT_LAYER_H
#include <joy2d/jui.h>
#include "Resources.h"


#define MOBILE_INPUT_EVENT (SDL_EVENT_USER + 0x1)

enum MobileInput {
        MOBILE_INPUT_NONE = 1 << 0,
        MOBILE_INPUT_UP = 1 << 1,
        MOBILE_INPUT_DOWN = 1 << 2,
        MOBILE_INPUT_LEFT = 1 << 3,
        MOBILE_INPUT_RIGHT = 1 << 4,
        MOBILE_INPUT_ATTACK = 1 << 5,
};

class MobileInputLayer
{
public:
	MobileInputLayer(Resources* resources, SDL_Renderer* renderer);
	~MobileInputLayer();
        void ListenEvent(SDL_Event* e);
        void Update();
        void Draw();
private:
	Resources* resources;
        button_p upButton;
        button_p downButton;
        button_p leftButton;
        button_p rightButton;
        button_p attackButton;
};


#endif