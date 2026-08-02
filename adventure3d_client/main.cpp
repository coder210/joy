#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <joy/joy.h>
#include "context.h"
#include "scenes/game_scene.h"


static scene_manager_p g_mgr;
static Context* ctx;

SDL_AppResult SDL_AppInit(void**, int, char**)
{
        sys_init_netenv();

        if (!init_env()) {
                log_info("SDL init failed: %s", get_error());
                return SDL_APP_FAILURE;
        }

	ctx = new Context();

        game_timer_init(&ctx->game_timer);
        game_timer_set_target_fps(&ctx->game_timer, 60);

        ctx->window = create_window("client", 240, 160);
        ctx->r3d = r3d_create(ctx->window, "adventure3d_client/shaders");

        ctx->running = true;

        g_mgr = scene_manager_create();
        scene_manager_push(g_mgr, game_scene_get_scene(game_scene_create(ctx)));

        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void*, SDL_Event* e)
{
	scene_manager_handle_event(g_mgr, e);

	if (should_close(e)) return SDL_APP_SUCCESS;
	if (e->type == SDL_EVENT_KEY_DOWN) {
		switch (e->key.scancode) {
		case SDL_SCANCODE_ESCAPE:
		case SDL_SCANCODE_R:
			scene_manager_replace(g_mgr, game_scene_get_scene(game_scene_create(ctx)));
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
        // pure 3D rendering: render3d_system manages r3d_begin/r3d_end internally
        scene_manager_render(g_mgr, ctx->r3d);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void*, SDL_AppResult)
{
        scene_manager_destroy(g_mgr);
        if (ctx->r3d) r3d_destroy(ctx->r3d);
        destroy_window(ctx->window);
	delete ctx;
        sys_release_netenv();
}
