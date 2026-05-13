#ifndef DEBUG_LAYER_H
#define DEBUG_LAYER_H
#include <joy2d/jtext.h>
#include "../asset_manager.h"

class DebugLayer
{
public:
	DebugLayer(AssetManager* assetManager);
	~DebugLayer();
	void Update(int frameId, int fps);
        void Draw(SDL_Renderer* renderer);
private:
	int frameId;
	int fps;
	AssetManager* assetManager;
	text_texture_p fpsTexture;
	text_texture_p frameTexture;
};


#endif