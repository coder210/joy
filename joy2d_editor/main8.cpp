//#define SDL_MAIN_USE_CALLBACKS 1
//#include <SDL3/SDL.h>
//#include <SDL3/SDL_main.h>
//#include <string.h>
//#include <stdlib.h>
//#include <math.h>
//#include <joy2d/jconfig.h>
//#include <joy2d/jscene.h>
//#include <joy2d/jcore.h>
//
//static SDL_Window*   g_win = NULL;
//static SDL_Renderer* g_ren = NULL;
//static scene_manager_p g_mgr = NULL;
//static node_p g_player = NULL;
//static game_timer_t g_timer;  // 帧率控制器
//static const int W = 800, H = 600;
//
//SDL_AppResult SDL_AppInit(void** st, int argc, char* argv[]) {
//    SDL_SetAppMetadata("jscene test", "1.0", "c.jscene");
//    if (!SDL_Init(SDL_INIT_VIDEO)) return SDL_APP_FAILURE;
//    g_win = SDL_CreateWindow("jscene test", W, H, 0);
//    if (!g_win) return SDL_APP_FAILURE;
//    g_ren = SDL_CreateRenderer(g_win, NULL);
//    if (!g_ren) return SDL_APP_FAILURE;
//    g_mgr = scene_manager_create();
//    game_timer_init(&g_timer);
//    game_timer_set_target_fps(&g_timer, 60);  // 锁定 60 FPS
//    return SDL_APP_CONTINUE;
//}
//
//SDL_AppResult SDL_AppEvent(void* st, SDL_Event* e) {
//    if (e->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
//    if (e->type == SDL_EVENT_KEY_DOWN || e->type == SDL_EVENT_KEY_UP) {
//        bool d = (e->type == SDL_EVENT_KEY_DOWN);
//        switch (e->key.scancode) {
//        case SDL_SCANCODE_SPACE:
//            break;
//        case SDL_SCANCODE_ESCAPE:
//            break;
//        case SDL_SCANCODE_R:
//            break;
//        default:
//            break;
//        }
//    }
//    return SDL_APP_CONTINUE;
//}
//
//SDL_AppResult SDL_AppIterate(void* st) {
//    game_timer_update(&g_timer); 
//    float dt = game_timer_get_delta_time(&g_timer);
//    SDL_SetRenderDrawColor(g_ren, 30, 30, 40, 255);
//    SDL_RenderClear(g_ren);
//
//    SDL_SetRenderDrawColor(g_ren, 255, 255, 255, 255);
//    SDL_FRect rect = { 0, 0, 100, 100 };
//    SDL_RenderRect(g_ren, &rect);
//
//    SDL_SetRenderDrawColor(g_ren, 100, 100, 100, 255);
//    SDL_RenderPresent(g_ren);
//    return SDL_APP_CONTINUE;
//}
//
//void SDL_AppQuit(void* st, SDL_AppResult r) {
//    scene_manager_destroy(g_mgr);
//    SDL_DestroyRenderer(g_ren);
//    SDL_DestroyWindow(g_win);
//}
