#include <SDL3/SDL.h>
#include <string.h>
#include "gameplay_controls_layer.h"
#include "../asset_manager.h"

struct gameplay_controls_layer {
	scene_node_p scene_node;
	/*button_p upButton;
	button_p downButton;
	button_p leftButton;
	button_p rightButton;
	button_p attackButton;*/

	bool prevUpPressed;
	bool prevDownPressed;
	bool prevLeftPressed;
	bool prevRightPressed;
};


static void send_gameplay_control_event(int event_code)
{
	SDL_Event event;
	SDL_zero(event);
	event.type = GAMEPLAY_CONTROL_EVENT;
	event.user.code = event_code;
	SDL_PushEvent(&event);
}

static void on_event(scene_node_p n, const void* e)
{
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)scene_node_get_userdata(n);
	SDL_Event* event = (SDL_Event*)e;
}

static void on_update(scene_node_p n, float dt)
{
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)scene_node_get_userdata(n);

	// 检测方向键松开事件
	//if (self->prevUpPressed && !self->upButton->is_pressed) {
	//	send_gameplay_control_event(GAMEPLAY_CONTROL_RELEASE_UP);
	//}
	//if (self->prevDownPressed && !self->downButton->is_pressed) {
	//	send_gameplay_control_event(GAMEPLAY_CONTROL_RELEASE_DOWN);
	//}
	//if (self->prevLeftPressed && !self->leftButton->is_pressed) {
	//	send_gameplay_control_event(GAMEPLAY_CONTROL_RELEASE_LEFT);
	//}
	//if (self->prevRightPressed && !self->rightButton->is_pressed) {
	//	send_gameplay_control_event(GAMEPLAY_CONTROL_RELEASE_RIGHT);
	//}

	//// 发送按下事件
	//if (self->upButton->is_pressed) {
	//	send_gameplay_control_event(GAMEPLAY_CONTROL_UP);
	//}

	//if (self->downButton->is_pressed) {
	//	send_gameplay_control_event(GAMEPLAY_CONTROL_DOWN);
	//}

	//if (self->leftButton->is_pressed) {
	//	send_gameplay_control_event(GAMEPLAY_CONTROL_LEFT);
	//}

	//if (self->rightButton->is_pressed) {
	//	send_gameplay_control_event(GAMEPLAY_CONTROL_RIGHT);
	//}

	//if (self->attackButton->is_pressed) {
	//	send_gameplay_control_event(GAMEPLAY_CONTROL_ATTACK);
	//}

	//self->prevUpPressed = self->upButton->is_pressed;
	//self->prevDownPressed = self->downButton->is_pressed;
	//self->prevLeftPressed = self->leftButton->is_pressed;
	//self->prevRightPressed = self->rightButton->is_pressed;
}

static void on_render(scene_node_p n, const void* arg)
{
	SDL_Renderer* renderer = (SDL_Renderer*)arg;
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)scene_node_get_userdata(n);
	//button_draw(self->upButton);
	//button_draw(self->downButton);
	//button_draw(self->leftButton);
	//button_draw(self->rightButton);
	//button_draw(self->attackButton);
}

static void on_destroy(scene_node_p n)
{
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)scene_node_get_userdata(n);
	/*button_destroy(self->upButton);
	button_destroy(self->downButton);
	button_destroy(self->leftButton);
	button_destroy(self->rightButton);
	button_destroy(self->attackButton);*/
}

gameplay_controls_layer_p create_gameplay_controls_layer(context* ctx)
{
	gameplay_controls_layer_p self;
	self = (gameplay_controls_layer_p)SDL_malloc(sizeof(gameplay_controls_layer_t));
	SDL_assert(self);
	self->scene_node = scene_node_create();
	SDL_assert(self->scene_node);
	scene_node_set_userdata(self->scene_node, self);
	scene_node_set_event_callback(self->scene_node, on_event);
	scene_node_set_update_callback(self->scene_node, on_update);
	scene_node_set_render_callback(self->scene_node, on_render);
	scene_node_set_destroy_callback(self->scene_node, on_destroy);
	scene_node_set_position(self->scene_node, 10, 150);
	scene_node_set_zorder(self->scene_node, 1);
	scene_node_set_scale(self->scene_node, 1, 1);

	//SDL_Rect logical_rect;
 //       SDL_GetRenderLogicalPresentation(ctx->renderer, &logical_rect.w, &logical_rect.h, NULL);
 //       int logical_w = logical_rect.w;
 //       int logical_h = logical_rect.h;

 //       // 边距（基于逻辑分辨率）
 //       const int left_margin = 10;
 //       const int bottom_margin = 60;
 //       const int right_margin = 10;

 //       const int dir_group_w = 100;
 //       const int dir_group_h = 100;
 //       const int attack_w = 60;
 //       const int attack_h = 60;

 //       // 方向键组左下角（逻辑坐标）
 //       float dir_x = left_margin;
 //       float dir_y = logical_h - dir_group_h - bottom_margin;  // 480-100-60=320

 //       // 攻击按钮右下角（逻辑坐标）
 //       float attack_x = logical_w - attack_w - right_margin;   // 640-60-10=570
 //       float attack_y = logical_h - attack_h - bottom_margin;  // 480-60-60=360

 //       // 创建按钮（使用逻辑坐标）
	//self->upButton = button_create(ctx->renderer, { dir_x + 50, dir_y + 0,   50, 50 });
	//self->downButton = button_create(ctx->renderer, { dir_x + 50, dir_y + 100, 50, 50 });
	//self->leftButton = button_create(ctx->renderer, { dir_x + 0,  dir_y + 50,  50, 50 });
	//self->rightButton = button_create(ctx->renderer, { dir_x + 100, dir_y + 50, 50, 50 });
	//self->attackButton = button_create(ctx->renderer, { attack_x, attack_y, 60, 60 });

 //       // 设置按钮文字
	font_p simhei_font = AssetManager::GetInstance()->GetSimheiFont24();
 //       button_set_textx(self->upButton, simhei_font, "U", (int)strlen("U"), { 255, 255, 255, 255 });
 //       button_set_textx(self->downButton, simhei_font, "D", (int)strlen("D"), { 255, 255, 255, 255 });
 //       button_set_textx(self->leftButton, simhei_font, "L", (int)strlen("L"), { 255, 255, 255, 255 });
 //       button_set_textx(self->rightButton, simhei_font, "R", (int)strlen("R"), { 255, 255, 255, 255 });
 //       button_set_textx(self->attackButton, simhei_font, "A", (int)strlen("A"), { 255, 255, 255, 255 });
 //       
 //       // 初始化按钮状态追踪
	//self->prevUpPressed = false;
	//self->prevDownPressed = false;
	//self->prevLeftPressed = false;
	//self->prevRightPressed = false;


	// children

	// up
	button_p up_button = button_create(ctx->renderer, 50, 50);
	button_set_textx(up_button, simhei_font, "U", (int)strlen("U"), { 255, 255, 255, 255 });
	scene_node_p up_button_node = scene_node_create();
	scene_node_set_position(up_button_node, 0, 0);
	scene_node_set_userdata(up_button_node, up_button);
	scene_node_set_zorder(up_button_node, 100);
	scene_node_set_update_callback(up_button_node, [](scene_node_p n, float dt) {
		button_p self = (button_p)scene_node_get_userdata(n);
		scene_node_get_world_position(n, &self->position.x, &self->position.y);
		if (self->is_pressed) {
			send_gameplay_control_event(GAMEPLAY_CONTROL_RELEASE_UP);
		}
	});

	scene_node_set_event_callback(up_button_node, [](scene_node_p n, const void* e) {
		button_p self = (button_p)scene_node_get_userdata(n);
		SDL_Event* event = (SDL_Event*)e;
		button_handle_event(self, event);
	});

	scene_node_set_render_callback(up_button_node, [](scene_node_p n, const void* arg) {
		button_p self = (button_p)scene_node_get_userdata(n);
		SDL_Renderer* renderer = (SDL_Renderer*)arg;
		button_draw(self);
	});

	scene_node_add_child(self->scene_node, up_button_node);

	// down
	button_p down_button = button_create(ctx->renderer, 50, 50);
	button_set_textx(down_button, simhei_font, "D", (int)strlen("D"), { 255, 255, 255, 255 });
	scene_node_p down_button_node = scene_node_create();
	scene_node_set_position(down_button_node, 0, 50);
	scene_node_set_userdata(down_button_node, down_button);
	scene_node_set_zorder(down_button_node, 100);
	scene_node_set_update_callback(down_button_node, [](scene_node_p n, float dt) {
		button_p self = (button_p)scene_node_get_userdata(n);
		scene_node_get_world_position(n, &self->position.x, &self->position.y);
		if (self->is_pressed) {
			send_gameplay_control_event(GAMEPLAY_CONTROL_RELEASE_UP);
		}
		});

	scene_node_set_event_callback(down_button_node, [](scene_node_p n, const void* e) {
		button_p self = (button_p)scene_node_get_userdata(n);
		SDL_Event* event = (SDL_Event*)e;
		button_handle_event(self, event);
		});

	scene_node_set_render_callback(down_button_node, [](scene_node_p n, const void* arg) {
		button_p self = (button_p)scene_node_get_userdata(n);
		SDL_Renderer* renderer = (SDL_Renderer*)arg;
		button_draw(self);
		});

	scene_node_add_child(self->scene_node, down_button_node);


	return self;
}

scene_node_p gameplay_controls_layer_get_node(gameplay_controls_layer_p gameplay_controls_layer)
{
	return gameplay_controls_layer->scene_node;
}
