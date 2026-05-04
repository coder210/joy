#include "debug_layer.h"

DebugLayer::DebugLayer(Resources* resources)
{
        this->resources = resources;
        this->frameId = this->fps = 0;
        const char* fpsStr = "fps:";
        this->fpsTexture = text_createx(resources->GetSimheiFont24(), fpsStr, SDL_strlen(fpsStr), { 255,255,255,255 });
        const char* frameIdStr = "frameId:";
        this->frameTexture = text_createx(resources->GetSimheiFont24(), frameIdStr, SDL_strlen(frameIdStr), { 255,255,255,255 });
}

DebugLayer::~DebugLayer()
{
        text_destroy(this->fpsTexture);
        text_destroy(this->frameTexture);
}

void DebugLayer::Update(int frameId, int fps)
{
        this->frameId = frameId;
        this->fps = fps;
}

void DebugLayer::Draw(SDL_Renderer* renderer)
{
        char content[JOY_MAX_PATH] = { 0 };
        sprintf(content, "fps:%d", this->fps);
        text_updatex(this->fpsTexture, resources->GetSimheiFont24(), content, SDL_strlen(content), { 255,255,255,255 });
        text_print(renderer, this->fpsTexture, 10, 10);

        sprintf(content, "中frameId:%d", this->frameId);
        text_updatex(this->frameTexture, resources->GetSimheiFont24(), content, SDL_strlen(content), { 255,255,255,255 });
        text_print(renderer, this->frameTexture, 100, 10);
}
