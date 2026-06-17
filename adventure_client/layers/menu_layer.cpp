// SDL types via jsdl
#include <string.h>
#include "menu_layer.h"
#include "../asset_manager.h"

static void setup_button_texture(button_p btn, jr2d_p renderer) {
	btn->image = image_create(renderer, "adventure_client/ui/buttons/button_blue_9slides.png");
	btn->pressed_image = image_create(renderer, "adventure_client/ui/buttons/button_blue_9slides_pressed.png");
	// 有纹理后去掉纯色背景
	jcolor_t transparent = {0, 0, 0, 0};
	button_set_normal_color(btn, transparent);
	button_set_hover_color(btn, transparent);
	button_set_pressed_color(btn, transparent);
}

struct menu_layer {
        scene_node_p scene_node;
};

static void send_menu_event(int event_code)
{
    push_user_event(MENU_EVENT, event_code);
}

static void on_event(scene_node_p n, const void* e)
{
        menu_layer_p self = (menu_layer_p)scene_node_get_userdata(n);

}

static void on_update(scene_node_p n, float dt)
{
        menu_layer_p self = (menu_layer_p)scene_node_get_userdata(n);
}

static void on_render(scene_node_p n, const void* arg)
{
        jr2d_p renderer = (jr2d_p)arg;
        menu_layer_p self = (menu_layer_p)scene_node_get_userdata(n);
}

static void on_destroy(scene_node_p n)
{
        menu_layer_p self = (menu_layer_p)scene_node_get_userdata(n);
        free(self);
}

static void load_menu_buttons(menu_layer_p self, context* ctx)
{
        int logical_width, logical_height;
        r2d_get_logical_presentation(ctx->r2d, &logical_width, &logical_height, NULL);
        font_p simhei_font = AssetManager::GetInstance()->GetSimheiFont24();

        int btn_width = 120;
        int btn_height = 50;
        int btn_x = (logical_width - btn_width) / 2;
        int start_y = (logical_height - btn_height * 3 - 40) / 2;

        // 开始按钮
        button_p start_btn = button_create(ctx->r2d, (float)btn_width, (float)btn_height);
        setup_button_texture(start_btn, ctx->r2d);
        start_btn->text_pos.y = 3.0f;
        button_set_textx(start_btn, simhei_font, "Start", (int)strlen("Start"), { 220, 200, 160, 255 });
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
                int et = get_event_type(e);
                if (et == JOY_EVENT_MOUSE_BUTTON_DOWN) {
                    float x, y; get_mouse_pos(e, &x, &y);
                    if (button_is_pressed(self, x, y))
                        send_menu_event(MENU_EVENT_START);
                } else if (et == JOY_EVENT_MOUSE_BUTTON_UP) {
                    button_set_pressed(self, false);
                } else if (et == JOY_EVENT_MOUSE_MOTION) {
                    float x, y; get_mouse_pos(e, &x, &y);
                    button_is_hovered(self, x, y);
                }
        });
        scene_node_set_render_callback(start_node, [](scene_node_p n, const void* arg) {
                button_p self = (button_p)scene_node_get_userdata(n);
                jr2d_p r2d = (jr2d_p)arg;
                button_draw(self);
        });
        scene_node_add_child(self->scene_node, start_node);

        // 设置按钮
        int settings_y = start_y + btn_height + 20;
        button_p settings_btn = button_create(ctx->r2d, (float)btn_width, (float)btn_height);
        setup_button_texture(settings_btn, ctx->r2d);
        settings_btn->text_pos.y = 3.0f;
        button_set_textx(settings_btn, simhei_font, "Settings", (int)strlen("Settings"), { 220, 200, 160, 255 });
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
                int et = get_event_type(e);
                if (et == JOY_EVENT_MOUSE_BUTTON_DOWN) {
                    float x, y; get_mouse_pos(e, &x, &y);
                    if (button_is_pressed(self, x, y))
                        send_menu_event(MENU_EVENT_SETTINGS);
                } else if (et == JOY_EVENT_MOUSE_BUTTON_UP) {
                    button_set_pressed(self, false);
                } else if (et == JOY_EVENT_MOUSE_MOTION) {
                    float x, y; get_mouse_pos(e, &x, &y);
                    button_is_hovered(self, x, y);
                }
        });
        scene_node_set_render_callback(settings_node, [](scene_node_p n, const void* arg) {
                button_p self = (button_p)scene_node_get_userdata(n);
                jr2d_p renderer = (jr2d_p)arg;
                button_draw(self);
        });
        scene_node_add_child(self->scene_node, settings_node);

        // 退出按钮
        int exit_y = settings_y + btn_height + 20;
        button_p exit_btn = button_create(ctx->r2d, (float)btn_width, (float)btn_height);
        setup_button_texture(exit_btn, ctx->r2d);
        exit_btn->text_pos.y = 3.0f;
        button_set_textx(exit_btn, simhei_font, "Exit", (int)strlen("Exit"), { 220, 200, 160, 255 });
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
                int et = get_event_type(e);
                if (et == JOY_EVENT_MOUSE_BUTTON_DOWN) {
                    float x, y; get_mouse_pos(e, &x, &y);
                    if (button_is_pressed(self, x, y))
                        send_menu_event(MENU_EVENT_EXIT);
                } else if (et == JOY_EVENT_MOUSE_BUTTON_UP) {
                    button_set_pressed(self, false);
                } else if (et == JOY_EVENT_MOUSE_MOTION) {
                    float x, y; get_mouse_pos(e, &x, &y);
                    button_is_hovered(self, x, y);
                }
        });
        scene_node_set_render_callback(exit_node, [](scene_node_p n, const void* arg) {
                button_p self = (button_p)scene_node_get_userdata(n);
                jr2d_p renderer = (jr2d_p)arg;
                button_draw(self);
        });
        scene_node_add_child(self->scene_node, exit_node);
}

menu_layer_p create_menu_layer(context* ctx)
{
        menu_layer_p self = (menu_layer_p)malloc(sizeof(menu_layer_t));
        // jsdl_assert(self);
        self->scene_node = scene_node_create();
        // jsdl_assert(self->scene_node);
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
