#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string.h>
#include <stdlib.h>
#include <joy/jconfig.h>
#include <joy/jscene.h>
#include <joy/jcore.h>
#include <joy/jsys.h>
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
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
                SDL_Log("SDL init failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        ctx = (context*)calloc(1, sizeof(context));

        game_timer_init(&ctx->game_timer);
        game_timer_set_target_fps(&ctx->game_timer, 60);  // 锁定 60 FPS

        SDL_CreateWindowAndRenderer("client", 640, 480, 0, &ctx->window, &ctx->renderer);
        SDL_SetRenderLogicalPresentation(ctx->renderer, 640, 480, SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_STRETCH);
        AssetManager::Init(ctx->renderer);

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

        if (e->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
        if (e->type == SDL_EVENT_KEY_DOWN || e->type == SDL_EVENT_KEY_UP) {
                bool d = (e->type == SDL_EVENT_KEY_DOWN);
                if (!d) return SDL_APP_CONTINUE; // 只处理按下事件
                switch (e->key.scancode) {
                case SDL_SCANCODE_SPACE:
                case SDL_SCANCODE_RETURN:
                        // 仅在加载/菜单场景下才启动游戏
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
        // 菜单层按钮事件
        else if (e->type == MENU_EVENT) {
                switch (e->user.code) {
                case MENU_EVENT_START:
                        scene_manager_replace(g_mgr, game_scene_get_scene(game_scene_create(ctx)));
                        break;
                case MENU_EVENT_SETTINGS:
                        SDL_Log("Settings button is clickable (work in progress)");
                        break;
                case MENU_EVENT_EXIT: {
                        SDL_Event quit;
                        SDL_zero(quit);
                        quit.type = SDL_EVENT_QUIT;
                        SDL_PushEvent(&quit);
                        break;
                }
                }
        }

        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void*)
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

void SDL_AppQuit(void*, SDL_AppResult)
{
        scene_manager_destroy(g_mgr);
        SDL_DestroyRenderer(ctx->renderer);
        SDL_DestroyWindow(ctx->window);
        free(ctx);
        sys_release_netenv();
}