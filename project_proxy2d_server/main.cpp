#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <joy/joy.h>
#include "scenes/game_scene.h"
#include "asset_manager.h"
#include "context.h"
#include "../project_proxy2d_shared/systems/draw_fps_pass.h"

static scene_manager_p g_mgr;
static Context* ctx;
static game_timer_t game_timer;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
        sys_init_netenv();

        if (!init_env()) {
                log_info("SDL init failed: %s", get_error());
                return SDL_APP_FAILURE;
        }

        ctx = new Context();

        game_timer_init(&game_timer);
        game_timer_set_target_fps(&game_timer, 60);

        AssetManager::Init();

        ctx->window = create_window("server", 480, 320);
        ctx->r2d = r2d_create(ctx->window);
        r2d_set_logical_presentation(ctx->r2d, 480, 320);

        g_mgr = scene_manager_create();
        scene_manager_push(g_mgr, game_scene_get_scene(game_scene_create(ctx)));

        log_info("server start");
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
        if (should_close(e)) {
                return SDL_APP_SUCCESS;
        }
        else {
                scene_manager_handle_event(g_mgr, e);
                return SDL_APP_CONTINUE;
        }
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
        game_timer_update(&game_timer);
        float dt = game_timer_get_delta_time(&game_timer);
        scene_manager_update(g_mgr, dt);
        r2d_set_color(ctx->r2d, 255, 255, 255, 255);
        r2d_begin(ctx->r2d);
        scene_manager_render(g_mgr, ctx->r2d);
        draw_fps_pass(ctx->r2d, AssetManager::GetInstance()->GetSimheiFont24(), dt);
        r2d_end(ctx->r2d);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        scene_manager_destroy(g_mgr);
        r2d_destroy(ctx->r2d);
        destroy_window(ctx->window);
        delete ctx;
        sys_release_netenv();
}
