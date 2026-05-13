#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <joy2d/jcore.h>
#include <joy2d/jsys.h>
#include <joy2d/jscene.h>
#include "scenes/game_server_scene.h"

static scene_manager_p g_mgr;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	sys_init_netenv();
	SDL_SetAppMetadata("Adventure server", "1.0", "com.example.adventure-server");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		log_info("SDL init failed: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	log_info("server start");
	g_mgr = scene_manager_create();
	scene_manager_push(g_mgr, game_server_scene_get_scene(game_server_scene_create()));

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	scene_manager_handle_event(g_mgr, event);
	if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	scene_manager_update(g_mgr, 0.0f);
	scene_manager_render(g_mgr, NULL);
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	scene_manager_destroy(g_mgr);
	sys_release_netenv();
}
