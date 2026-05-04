#ifndef DEBUG_LAYER_H
#define DEBUG_LAYER_H
#include <joy2d/jtext.h>
#include "Resources.h"

class DebugLayer
{
public:
	DebugLayer(Resources* resources);
	~DebugLayer();
	void Update(int frameId, int fps);
        void Draw(SDL_Renderer* renderer);
private:
	int frameId;
	int fps;
        Resources* resources;
	text_texture_p fpsTexture;
	text_texture_p frameTexture;
};


#endif