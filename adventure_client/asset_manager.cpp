#include "asset_manager.h"

// 静态成员初始化
AssetManager* AssetManager::instance_ = nullptr;

AssetManager::AssetManager()
{
        simheiFont24_ = font_create("adventure_client/fonts/simhei.ttf", 24);
}

AssetManager::~AssetManager()
{
        if (simheiFont24_)
                font_destroy(simheiFont24_);
}

void AssetManager::Init()
{
        if (instance_ == nullptr) {
                instance_ = new AssetManager();
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