#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <joy/jsdl.h>
#include <joy/jsys.h>
#include <joy/jscene.h>
#include "scenes/game_scene.h"
#include "asset_manager.h"
#include "context.h"

static scene_manager_p g_mgr;
static Context* ctx;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	sys_init_netenv();

	if (!jsdl_init(JSDL_INIT_VIDEO)) {
		log_info("SDL init failed: %s", jsdl_get_error());
		return SDL_APP_FAILURE;
	}

	ctx = (Context*)calloc(1, sizeof(Context));
        ctx->FIXED_TIMESTEP = 1 / 50.0f;

	game_timer_init(&ctx->game_timer);
	game_timer_set_target_fps(&ctx->game_timer, 60);

	jsdl_create_window_and_renderer("server", 640, 480, 0, &ctx->window, &ctx->renderer);
	jsdl_set_logical_presentation(ctx->renderer, 640, 480, JSDL_PRESENTATION_STRETCH);
        AssetManager::Init(ctx->renderer);

	g_mgr = scene_manager_create();
	scene_manager_push(g_mgr, game_scene_get_scene(game_scene_create(ctx)));

	log_info("server start");
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
        if (jsdl_should_close(e)) {
                return SDL_APP_SUCCESS;
        }
        else {
                scene_manager_handle_event(g_mgr, e);
                return SDL_APP_CONTINUE;
        }
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
        game_timer_update(&ctx->game_timer);
        float dt = game_timer_get_delta_time(&ctx->game_timer);
        scene_manager_update(g_mgr, dt);
        jsdl_set_render_draw_color(ctx->renderer, 30, 30, 40, 255);
        jsdl_render_clear(ctx->renderer);
        scene_manager_render(g_mgr, ctx->renderer);
        jsdl_render_present(ctx->renderer);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        scene_manager_destroy(g_mgr);
        jsdl_destroy_renderer(ctx->renderer);
        jsdl_destroy_window(ctx->window);
        free(ctx);
        sys_release_netenv();
}
