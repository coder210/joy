#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <joy2d/jui.h>
#include <SDL3/SDL.h>

class AssetManager
{
public:
	// 获取单例实例（使用前必须先调用 Init）
	static AssetManager* GetInstance();

	// 初始化单例，传入 SDL_Renderer，必须在使用前调用一次
	static void Init(SDL_Renderer* renderer);

	// 禁止拷贝和赋值
	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;

	~AssetManager();

	font_p GetSimheiFont24();

private:
	// 私有构造，只能通过 Init 内部调用
	explicit AssetManager(SDL_Renderer* renderer);

	static AssetManager* instance_;
	font_p simheiFont24_;
};

#endif // ASSET_MANAGER_H