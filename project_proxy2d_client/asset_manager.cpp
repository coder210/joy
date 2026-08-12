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
        tex_cache_clear();
}

void AssetManager::Init()
{
        if (instance_ == nullptr) {
                instance_ = new AssetManager();
        }
}

AssetManager* AssetManager::GetInstance()
{
        return instance_;
}

font_p AssetManager::GetSimheiFont24()
{
        return simheiFont24_;
}


void AssetManager::tex_cache_clear()
{
        for (auto& kv : tex_cache_)
                if (kv.second) image_destroy(kv.second);
        tex_cache_.clear();
}