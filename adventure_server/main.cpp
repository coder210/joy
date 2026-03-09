#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#define UTF8_IMPLEMENTATION
extern "C" {
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <joy2d/graphics.h>
#include <joy2d/calculator.h>
#include <joy2d/network.h>
#include <joy2d/sys.h>
#include <joy2d/profiler.h>
#include <joy2d/ecs.h>
#include "utf8.h"
}

#include <map>
#include <string>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static kcpserver_p kcpserver = NULL;
static simple_fps_counter_p  fps_counter = NULL;
static font_p simhei_font = NULL;
static int grad_size = 50;
static text_texture_p up_text = NULL;
static text_texture_p fps_text = NULL;
static ecs_world_p ecs_world = NULL;
static utf8_int32_t chinese_codepoints[132];
static int chinese_codepoints_num;
static utf8_int32_t chinese_codepoints2[132];
static int chinese_codepoints2_num;


static int get_codepoints(const char* text, utf8_int32_t *codepoints)
{
        utf8_int32_t cp;
        int count = 0;
        const char* pos = text;
        while ((pos = utf8codepoint(pos, &cp)) && cp != 0) {
                codepoints[count] = cp;
                ++count;
                //printf("字符%d:U+%04x,%d\n", ++count, cp, cp);
                //if (utf8islower(cp)) printf(" (小写)");
                //if (utf8isupper(cp)) printf(" (大写)");
                //printf(",字节数:%zu\n", utf8codepointsize(cp));
        }
        return count;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
        const int width = 640;
        const int height = 480;
        std::string ip = "192.168.2.11";
        const int port = 30000;

        sys_init_netenv();

        SDL_SetAppMetadata("adventure server", "1.0", "com.joy2d.adventure_server");
        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        if (!SDL_CreateWindowAndRenderer("adventure/server", width, height, 0, &window, &renderer)) {
                SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        SDL_SetRenderLogicalPresentation(renderer, width, height, SDL_LOGICAL_PRESENTATION_STRETCH);

        std::string k = SDL_GetBasePath();
        k.append("fonts/simhei.ttf");
       
        chinese_codepoints_num = get_codepoints("中国", chinese_codepoints);
        chinese_codepoints2_num = get_codepoints("chinese", chinese_codepoints2);

        simhei_font = font_create(renderer, k.c_str(), grad_size * 0.4f);
        up_text = text_create(simhei_font, chinese_codepoints, chinese_codepoints_num, {255, 255, 255, 255});
        fps_text = text_create(simhei_font, chinese_codepoints2, chinese_codepoints2_num, {255, 255, 255, 255});
        fps_counter = simple_fps_create();

        ecs_world = ecs_create();
        kcpserver = kcpserver_create(ip.c_str(), port);

        
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
        if (event->type == SDL_EVENT_QUIT) {
                return SDL_APP_SUCCESS;
        }
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
        simple_fps_update(fps_counter);

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        //text_update(fps_text, simhei_font, chinese_codepoints, chinese_codepoints_num, { 255, 255, 255, 255 });
        text_print(renderer, fps_text, 100, 0);
        text_print(renderer, up_text, 200, 0);
       
        SDL_RenderPresent(renderer);
        return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        sys_release_netenv();
        kcpserver_destroy(kcpserver);
        simple_fps_destory(fps_counter);

        ecs_destroy(ecs_world);

}

