#include "asset_manager.h"

// 静态成员初始化
AssetManager* AssetManager::instance_ = nullptr;

AssetManager::AssetManager(SDL_Renderer* renderer)
{
        simheiFont24_ = font_create(renderer, "adventure_server_fonts/simhei.ttf", 24);
}

AssetManager::~AssetManager()
{
        if (simheiFont24_)
                font_destroy(simheiFont24_);
}

void AssetManager::Init(SDL_Renderer* renderer)
{
        if (instance_ == nullptr) {
                instance_ = new AssetManager(renderer);
        }
        // 若已经初始化，可根据需要添加日志或断言
}

AssetManager* AssetManager::GetInstance()
{
        // 调用前必须确保已调用 Init，否则返回 nullptr
        return instance_;
}

font_p AssetManager::GetSimheiFont24()
{
        return simheiFont24_;
}