#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <joy/joy.h>
#include "context.h"
#include "asset_manager.h"
#include "scenes/loading_scene.h"
#include "scenes/game_scene.h"
#include "layers/menu_layer.h"


static scene_manager_p g_mgr;
static context* ctx;

SDL_AppResult SDL_AppInit(void**, int, char**)
{
        sys_init_netenv();

        if (!init_env()) {
                log_info("SDL init failed: %s", get_error());
                return SDL_APP_FAILURE;
        }

	ctx = new context();

        game_timer_init(&ctx->game_timer);
        game_timer_set_target_fps(&ctx->game_timer, 60);

        AssetManager::Init();

        ctx->window = create_window("client", 480, 320);
        ctx->r2d = r2d_create(ctx->window);
       

        ctx->FIXED_TIMESTEP = 1 / 50.0f;
        ctx->running = true;

        g_mgr = scene_manager_create();
        loading_scene_t* loading = loading_scene_create(ctx);
        scene_manager_push(g_mgr, loading_scene_get_scene(loading));

        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void*, SDL_Event* e)
{
	scene_manager_handle_event(g_mgr, e);

	if (should_close(e)) return SDL_APP_SUCCESS;
	if (e->type == SDL_EVENT_KEY_DOWN) {
		switch (e->key.scancode) {
		case SDL_SCANCODE_SPACE:
		case SDL_SCANCODE_RETURN:
			{ scene_p cur = scene_manager_get_current(g_mgr);
			if (cur && strcmp(scene_get_name(cur), "Loading") == 0) {
				scene_manager_replace(g_mgr, game_scene_get_scene(game_scene_create(ctx)));
			} }
			break;
		case SDL_SCANCODE_ESCAPE:
			scene_manager_replace(g_mgr, loading_scene_get_scene(loading_scene_create(ctx)));
			break;
		case SDL_SCANCODE_R:
			scene_manager_destroy(g_mgr);
			g_mgr = scene_manager_create();
			scene_manager_push(g_mgr, loading_scene_get_scene(loading_scene_create(ctx)));
			break;
		}
	}
	else if (is_user_event(e, MENU_EVENT)) {
		switch (get_event_code(e)) {
		case MENU_EVENT_START:
			scene_manager_replace(g_mgr, game_scene_get_scene(game_scene_create(ctx)));
			break;
		case MENU_EVENT_SETTINGS:
			log_info("Settings button is clickable (work in progress)");
			break;
		case MENU_EVENT_EXIT:
			push_quit_event();
			break;
		}
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void*)
{
        game_timer_update(&ctx->game_timer);
        float dt = game_timer_get_delta_time(&ctx->game_timer);
        scene_manager_update(g_mgr, dt);
        r2d_set_color(ctx->r2d, 50, 120, 50, 255);
        r2d_begin(ctx->r2d);
        scene_manager_render(g_mgr, ctx->r2d);
        r2d_end(ctx->r2d);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void*, SDL_AppResult)
{
        scene_manager_destroy(g_mgr);
        r2d_destroy(ctx->r2d);
        destroy_window(ctx->window);
	delete ctx;
        sys_release_netenv();
}
