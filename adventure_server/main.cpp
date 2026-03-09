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
static uint32_t* chinese_codepoints;
static size_t chinese_codepoints_num;
static uint32_t* chinese_codepoints2;
static size_t chinese_codepoints2_num;

static const char* utf8_decode(const char* s, int* code) {
        unsigned char c = *(unsigned char*)s;
        if (c < 0x80) {                /* 单字节: 0xxxxxxx */
                *code = c;
                return s + 1;
        }
        else if (c < 0xE0) {           /* 双字节: 110xxxxx 10xxxxxx */
                if ((s[1] & 0xC0) != 0x80) return NULL;
                *code = ((c & 0x1F) << 6) | (s[1] & 0x3F);
                if (*code < 0x80) return NULL;  /* 过度编码 */
                return s + 2;
        }
        else if (c < 0xF0) {           /* 三字节: 1110xxxx 10xxxxxx 10xxxxxx */
                if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80) return NULL;
                *code = ((c & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
                if (*code < 0x800) return NULL;  /* 过度编码 */
                return s + 3;
        }
        else if (c < 0xF8) {           /* 四字节: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
                if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80 || (s[3] & 0xC0) != 0x80) return NULL;
                *code = ((c & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
                if (*code < 0x10000) return NULL; /* 过度编码 */
                return s + 4;
        }
        else {
                return NULL;               /* 5字节及以上或非法首字节 */
        }
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
        chinese_codepoints = utf8_decode_all("中国", strlen("中国"), &chinese_codepoints_num);
        chinese_codepoints2 = utf8_decode_all("chinese", strlen("chinese"), &chinese_codepoints2_num);

        //chinese_codepoints[0] = 20013;
        //chinese_codepoints[1] = 22269;
        //chinese_codepoints_num = 2;
        


        simhei_font = font_create(renderer, k.c_str(), grad_size * 0.4f);
        up_text = text_create(simhei_font, (int*)chinese_codepoints, chinese_codepoints_num, {255, 255, 255, 255});
        fps_text = text_create(simhei_font, (int*)chinese_codepoints2, chinese_codepoints2_num, {255, 255, 255, 255});
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

