#include "asset_manager.h"

AssetManager* AssetManager::instance_ = nullptr;

AssetManager::AssetManager()
{
        simheiFont24_ = font_create("project_proxy2d_server/fonts/simhei.ttf", 24);
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
        // 服务端仅缓存不主动加载纹理（由客户端在 image_create 中预加载后通过 tex_cache_get 传入）
        return NULL;
}

void AssetManager::tex_cache_clear()
{
        for (auto& kv : tex_cache_)
                if (kv.second) image_destroy(kv.second);
        tex_cache_.clear();
}