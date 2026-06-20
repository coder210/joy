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

image_p AssetManager::tex_cache_get(jr2d_p r2d, const char* path)
{
        std::string k(path);
        auto it = tex_cache_.find(k);
        if (it != tex_cache_.end()) return it->second;
        image_p t = image_create(r2d, path);
        tex_cache_[k] = t;
        return t;
}

void AssetManager::tex_cache_clear()
{
        for (auto& kv : tex_cache_)
                if (kv.second) image_destroy(kv.second);
        tex_cache_.clear();
}