#include <SDL3/SDL.h>
#include "menu_layer.h"
#include "../asset_manager.h"

struct menu_layer {
        scene_node_p scene_node;
};

static void send_menu_event(int event_code)
{
        SDL_Event event;
        SDL_zero(event);
        event.type = MENU_EVENT;
        event.user.code = event_code;
        SDL_PushEvent(&event);
}

static void on_event(scene_node_p n, const void* e)
{
        menu_layer_p self = (menu_layer_p)scene_node_get_userdata(n);
        SDL_Event* event = (SDL_Event*)e;
}

static void on_update(scene_node_p n, float dt)
{
        menu_layer_p self = (menu_layer_p)scene_node_get_userdata(n);
}

static void on_render(scene_node_p n, const void* arg)
{
        SDL_Renderer* renderer = (SDL_Renderer*)arg;
        menu_layer_p self = (menu_layer_p)scene_node_get_userdata(n);
}

static void on_destroy(scene_node_p n)
{
        menu_layer_p self = (menu_layer_p)scene_node_get_userdata(n);
        SDL_free(self);
}

static void load_menu_buttons(menu_layer_p self, context* ctx)
{
        int logical_width, logical_height;
        SDL_GetRenderLogicalPresentation(ctx->renderer, &logical_width, &logical_height, NULL);
        font_p simhei_font = AssetManager::GetInstance()->GetSimheiFont24();

        int btn_width = 120;
        int btn_height = 50;
        int btn_x = (logical_width - btn_width) / 2;
        int start_y = (logical_height - btn_height * 3 - 40) / 2;

        // 开始按钮
        button_p start_btn = button_create(ctx->renderer, (float)btn_width, (float)btn_height);
        button_set_textx(start_btn, simhei_font, "Start", (int)strlen("Start"), { 220, 200, 160, 255 });
        button_set_callback(start_btn,
                [](button_p, void*) { send_menu_event(MENU_EVENT_START); },
                nullptr, nullptr);
        scene_node_p start_node = scene_node_create();
        scene_node_set_position(start_node, (float)btn_x, (float)start_y);
        scene_node_set_userdata(start_node, start_btn);
        scene_node_set_zorder(start_node, 100);
        scene_node_set_update_callback(start_node, [](scene_node_p n, float dt) {
                button_p self = (button_p)scene_node_get_userdata(n);
                scene_node_get_world_position(n, &self->position.x, &self->position.y);
        });
        scene_node_set_event_callback(start_node, [](scene_node_p n, const void* e) {
                button_p self = (button_p)scene_node_get_userdata(n);
                SDL_Event* event = (SDL_Event*)e;
                button_handle_event(self, event);
        });
        scene_node_set_render_callback(start_node, [](scene_node_p n, const void* arg) {
                button_p self = (button_p)scene_node_get_userdata(n);
                SDL_Renderer* renderer = (SDL_Renderer*)arg;
                button_draw(self);
        });
        scene_node_add_child(self->scene_node, start_node);

        // 设置按钮
        int settings_y = start_y + btn_height + 20;
        button_p settings_btn = button_create(ctx->renderer, (float)btn_width, (float)btn_height);
        button_set_textx(settings_btn, simhei_font, "Settings", (int)strlen("Settings"), { 220, 200, 160, 255 });
        button_set_callback(settings_btn,
                [](button_p, void*) { send_menu_event(MENU_EVENT_SETTINGS); },
                nullptr, nullptr);
        scene_node_p settings_node = scene_node_create();
        scene_node_set_position(settings_node, (float)btn_x, (float)settings_y);
        scene_node_set_userdata(settings_node, settings_btn);
        scene_node_set_zorder(settings_node, 100);
        scene_node_set_update_callback(settings_node, [](scene_node_p n, float dt) {
                button_p self = (button_p)scene_node_get_userdata(n);
                scene_node_get_world_position(n, &self->position.x, &self->position.y);
        });
        scene_node_set_event_callback(settings_node, [](scene_node_p n, const void* e) {
                button_p self = (button_p)scene_node_get_userdata(n);
                SDL_Event* event = (SDL_Event*)e;
                button_handle_event(self, event);
        });
        scene_node_set_render_callback(settings_node, [](scene_node_p n, const void* arg) {
                button_p self = (button_p)scene_node_get_userdata(n);
                SDL_Renderer* renderer = (SDL_Renderer*)arg;
                button_draw(self);
        });
        scene_node_add_child(self->scene_node, settings_node);

        // 退出按钮
        int exit_y = settings_y + btn_height + 20;
        button_p exit_btn = button_create(ctx->renderer, (float)btn_width, (float)btn_height);
        button_set_textx(exit_btn, simhei_font, "Exit", (int)strlen("Exit"), { 220, 200, 160, 255 });
        button_set_callback(exit_btn,
                [](button_p, void*) { send_menu_event(MENU_EVENT_EXIT); },
                nullptr, nullptr);
        scene_node_p exit_node = scene_node_create();
        scene_node_set_position(exit_node, (float)btn_x, (float)exit_y);
        scene_node_set_userdata(exit_node, exit_btn);
        scene_node_set_zorder(exit_node, 100);
        scene_node_set_update_callback(exit_node, [](scene_node_p n, float dt) {
                button_p self = (button_p)scene_node_get_userdata(n);
                scene_node_get_world_position(n, &self->position.x, &self->position.y);
        });
        scene_node_set_event_callback(exit_node, [](scene_node_p n, const void* e) {
                button_p self = (button_p)scene_node_get_userdata(n);
                SDL_Event* event = (SDL_Event*)e;
                button_handle_event(self, event);
        });
        scene_node_set_render_callback(exit_node, [](scene_node_p n, const void* arg) {
                button_p self = (button_p)scene_node_get_userdata(n);
                SDL_Renderer* renderer = (SDL_Renderer*)arg;
                button_draw(self);
        });
        scene_node_add_child(self->scene_node, exit_node);
}

menu_layer_p create_menu_layer(context* ctx)
{
        menu_layer_p self = (menu_layer_p)SDL_malloc(sizeof(menu_layer_t));
        SDL_assert(self);
        self->scene_node = scene_node_create();
        SDL_assert(self->scene_node);
        scene_node_set_userdata(self->scene_node, self);
        scene_node_set_event_callback(self->scene_node, on_event);
        scene_node_set_update_callback(self->scene_node, on_update);
        scene_node_set_render_callback(self->scene_node, on_render);
        scene_node_set_destroy_callback(self->scene_node, on_destroy);
        scene_node_set_position(self->scene_node, 0, 0);
        scene_node_set_zorder(self->scene_node, 100);

        load_menu_buttons(self, ctx);

        return self;
}

scene_node_p menu_layer_get_node(menu_layer_p layer)
{
        return layer->scene_node;
}
