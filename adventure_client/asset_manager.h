#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H
#include <joy/joy.h>
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

	// 清空纹理缓存（释放所有纹理）
	void tex_cache_clear();

private:
	explicit AssetManager();

	static AssetManager* instance_;
	font_p simheiFont24_;
	std::unordered_map<std::string, image_p> tex_cache_;
};

#endif // ASSET_MANAGER_H