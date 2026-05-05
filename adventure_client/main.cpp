#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string.h>
#include <stdlib.h>
#include <joy2d/jconfig.h>
#include <joy2d/jscene.h>
#include <joy2d/jcore.h>
#include <joy2d/jsys.h>
#include "context.h"
#include "scenes/loading_scene.h"


static scene_manager_p g_mgr = NULL;
static Context2* ctx = NULL;

SDL_AppResult SDL_AppInit(void**, int, char**)
{
        sys_init_netenv();
        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("SDL init failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        ctx = new Context2();

        g_mgr = scene_manager_create();
        LoadingScene* loading = new LoadingScene();
        scene_manager_push(g_mgr, loading->get_scene());
        // 注意: loading 的释放由场景管理器 pop 时触发 OnDestroy 回调处理
        // 或在 SDL_AppQuit 中统一清理

        SDL_CreateWindowAndRenderer("client", 640, 480, 0, &ctx->window, &ctx->renderer);
        SDL_SetRenderLogicalPresentation(ctx->renderer, 640, 480, SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_STRETCH);

        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void*, SDL_Event* e)
{
       
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void*)
{
        scene_manager_update(g_mgr, 1.0f / 60.0f);
        SDL_SetRenderDrawColor(ctx->renderer, 30, 30, 40, 255);
        SDL_RenderClear(ctx->renderer);
        scene_manager_render(g_mgr);
        SDL_RenderPresent(ctx->renderer);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void*, SDL_AppResult)
{
        sys_release_netenv();
        scene_manager_destroy(g_mgr);
        SDL_DestroyRenderer(ctx->renderer);
        SDL_DestroyWindow(ctx->window);
}