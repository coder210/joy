#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <joy2d/jcore.h>
#include <joy2d/jsys.h>
#include <joy2d/jscene.h>
#include "scenes/game_scene.h"
#include "app_context.h"

static scene_manager_p g_mgr;
static Context* ctx;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	sys_init_netenv();
	SDL_SetAppMetadata("adventure server", "1.0", "com.example.adventure-server");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		log_info("SDL init failed: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	ctx = (Context*)calloc(1, sizeof(Context));
        ctx->FIXED_TIMESTEP = 1 / 50.0f;

	game_timer_init(&ctx->game_timer);
	game_timer_set_target_fps(&ctx->game_timer, 60);  // Ëø¶¨ 60 FPS

	SDL_CreateWindowAndRenderer("server", 640, 480, 0, &ctx->window, &ctx->renderer);
	SDL_SetRenderLogicalPresentation(ctx->renderer, 640, 480, SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_STRETCH);

	g_mgr = scene_manager_create();
	scene_manager_push(g_mgr, game_scene_get_scene(game_scene_create(ctx)));

	log_info("server start");
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
        scene_manager_handle_event(g_mgr, e);

        if (e->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
        if (e->type == SDL_EVENT_KEY_DOWN || e->type == SDL_EVENT_KEY_UP) {
                bool d = (e->type == SDL_EVENT_KEY_DOWN);
                switch (e->key.scancode) {
                case SDL_SCANCODE_SPACE:
                        //if (d) { scene_manager_replace(g_mgr, game_scene_get_scene(game_scene_create(ctx))); }
                        break;
                case SDL_SCANCODE_ESCAPE:
                        if (d) {
                                //scene_manager_replace(g_mgr, loading_scene_get_scene(loading_scene_create(ctx)));
                        }
                        break;
                case SDL_SCANCODE_R:
                        break;
                }
        }

        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
        game_timer_update(&ctx->game_timer);
        float dt = game_timer_get_delta_time(&ctx->game_timer);
        scene_manager_update(g_mgr, dt);
        SDL_SetRenderDrawColor(ctx->renderer, 30, 30, 40, 255);
        SDL_RenderClear(ctx->renderer);
        scene_manager_render(g_mgr, ctx->renderer);
        SDL_RenderPresent(ctx->renderer);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        scene_manager_destroy(g_mgr);
        SDL_DestroyRenderer(ctx->renderer);
        SDL_DestroyWindow(ctx->window);
        free(ctx);
        sys_release_netenv();
}
