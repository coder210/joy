#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H
#include <joy/jfont.h>
#include <joy/jr2d.h>
#include <string>
#include <unordered_map>

class AssetManager
{
public:
	static AssetManager* GetInstance();
	static void Init();

	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;

	~AssetManager();

	font_p GetSimheiFont24();

	// 纹理缓存：获取/加载纹理
	image_p tex_cache_get(jr2d_p r2d, const char* path);
	// 清空纹理缓存（释放所有纹理）
	void tex_cache_clear();

private:
	explicit AssetManager();

	static AssetManager* instance_;
	font_p simheiFont24_;
	std::unordered_map<std::string, image_p> tex_cache_;
};

#endif // ASSET_MANAGER_H