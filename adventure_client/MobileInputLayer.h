#ifndef MOBILE_INPUT_LAYER_H
#define MOBILE_INPUT_LAYER_H
#include <joy2d/jui.h>
#include "Resources.h"

class MobileInputLayer
{
public:
	MobileInputLayer(Resources* resources, SDL_Renderer* renderer);
	~MobileInputLayer();
        void Update();
        void Draw();
private:
	Resources* resources;
        button_p upButton;
        button_p downButton;
        button_p leftButton;
        button_p rightButton;
};


#endif