#include "liv-joystick.h"
#include "liv-graphics.h"

// 颜色定义
#define JOYSTICK_BG_COLOR 0x555555FF
#define JOYSTICK_HANDLE_COLOR 0x888888FF
#define JOYSTICK_HANDLE_ACTIVE_COLOR 0x4CAF50FF

// 摇杆结构体
typedef struct joystick {
	SDL_Renderer* renderer;      // 渲染器
	SDL_FRect background;        // 背景区域
	SDL_FRect handle;           // 手柄区域
	SDL_FPoint center;          // 摇杆中心点
	float radius;               // 背景半径
	float handle_radius;        // 手柄半径
	SDL_FPoint direction;       // 方向向量 (x: -1到1, y: -1到1)
	float magnitude;            // 强度 (0到1)
	float deadzone;             // 死区阈值
	bool is_dragging;           // 是否正在拖动
	bool is_touch_active;       // 是否触摸激活
	int touch_id;               // 触摸ID
} joystick_t, * joystick_p;

// 计算两点间距离
static float distance_squared(float x1, float y1, float x2, float y2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	return dx * dx + dy * dy;
}

// 初始化摇杆
joystick_p joystick_create(SDL_Renderer* renderer, float x, float y, float radius)
{
	joystick_p joystick;
	joystick = (joystick_p)SDL_malloc(sizeof(joystick_t));

	joystick->renderer = renderer;
	joystick->center.x = x;
	joystick->center.y = y;
	joystick->radius = radius;
	joystick->handle_radius = radius * 0.4f;

	joystick->background.x = x - radius;
	joystick->background.y = y - radius;
	joystick->background.w = radius * 2;
	joystick->background.h = radius * 2;

	joystick->handle.x = x - joystick->handle_radius;
	joystick->handle.y = y - joystick->handle_radius;
	joystick->handle.w = joystick->handle_radius * 2;
	joystick->handle.h = joystick->handle_radius * 2;

	joystick->direction.x = 0.0f;
	joystick->direction.y = 0.0f;
	joystick->magnitude = 0.0f;
	joystick->deadzone = 0.1f;
	joystick->is_dragging = false;
	joystick->is_touch_active = false;
	joystick->touch_id = -1;
	return joystick;
}

void joystick_destroy(joystick_p joystick)
{
	SDL_free(joystick);
}

// 设置摇杆位置
void joystick_set_position(joystick_p joystick, float x, float y)
{
	joystick->center.x = x;
	joystick->center.y = y;

	joystick->background.x = x - joystick->radius;
	joystick->background.y = y - joystick->radius;

	if (!joystick->is_dragging) {
		joystick->handle.x = x - joystick->handle_radius;
		joystick->handle.y = y - joystick->handle_radius;
	}
}

// 处理摇杆拖拽更新
static void joystick_update_drag(joystick_p joystick, float touch_x, float touch_y)
{
	// 计算相对于中心点的向量
	float dx = touch_x - joystick->center.x;
	float dy = touch_y - joystick->center.y;

	// 计算距离
	float distance = sqrtf(dx * dx + dy * dy);

	// 限制距离不超过背景半径
	if (distance > joystick->radius) {
		dx = (dx / distance) * joystick->radius;
		dy = (dy / distance) * joystick->radius;
		distance = joystick->radius;
	}

	// 计算方向和强度
	joystick->magnitude = distance / joystick->radius;

	// 应用死区
	if (joystick->magnitude < joystick->deadzone) {
		joystick->direction.x = 0.0f;
		joystick->direction.y = 0.0f;
		joystick->magnitude = 0.0f;

		// 手柄回到中心
		joystick->handle.x = joystick->center.x - joystick->handle_radius;
		joystick->handle.y = joystick->center.y - joystick->handle_radius;
	}
	else {
		// 归一化方向向量
		joystick->direction.x = dx / joystick->radius;
		joystick->direction.y = dy / joystick->radius;

		// 更新手柄位置
		joystick->handle.x = joystick->center.x + dx - joystick->handle_radius;
		joystick->handle.y = joystick->center.y + dy - joystick->handle_radius;
	}
}

// 处理事件（自适应逻辑分辨率）
void joystick_handle_event(joystick_p joystick, SDL_Event* event)
{
	SDL_FPoint logic_pos;
	int window_width, window_height;

	// 获取渲染器输出尺寸
	SDL_GetRenderOutputSize(joystick->renderer, &window_width, &window_height);

	switch (event->type) {
	case SDL_EVENT_FINGER_DOWN: {
		// 转换坐标到逻辑空间
		SDL_RenderCoordinatesFromWindow(joystick->renderer,
			event->tfinger.x * window_width,
			event->tfinger.y * window_height,
			&logic_pos.x, &logic_pos.y);

		// 检查是否在摇杆区域内
		float dist_sq = distance_squared(logic_pos.x, logic_pos.y,
			joystick->center.x, joystick->center.y);
		if (dist_sq <= (joystick->radius * joystick->radius)) {
			joystick->is_dragging = true;
			joystick->is_touch_active = true;
			joystick->touch_id = (int)event->tfinger.fingerID;
			joystick_update_drag(joystick, logic_pos.x, logic_pos.y);
		}
		break;
	}

	case SDL_EVENT_FINGER_MOTION: {
		if (joystick->is_touch_active && joystick->touch_id == (int)event->tfinger.fingerID) {
			// 转换坐标到逻辑空间
			SDL_RenderCoordinatesFromWindow(joystick->renderer,
				event->tfinger.x * window_width,
				event->tfinger.y * window_height,
				&logic_pos.x, &logic_pos.y);

			joystick_update_drag(joystick, logic_pos.x, logic_pos.y);
		}
		break;
	}

	case SDL_EVENT_FINGER_UP: {
		if (joystick->is_touch_active && joystick->touch_id == (int)event->tfinger.fingerID) {
			joystick_reset(joystick);
		}
		break;
	}

	case SDL_EVENT_MOUSE_BUTTON_DOWN: {
		if (event->button.which == SDL_TOUCH_MOUSEID) {
			return; // 忽略触摸模拟的鼠标事件
		}

		if (event->button.button == SDL_BUTTON_LEFT) {
			// 转换坐标到逻辑空间
			SDL_RenderCoordinatesFromWindow(joystick->renderer,
				event->button.x, event->button.y,
				&logic_pos.x, &logic_pos.y);

			// 检查是否在摇杆区域内
			float dist_sq = distance_squared(logic_pos.x, logic_pos.y,
				joystick->center.x, joystick->center.y);
			if (dist_sq <= (joystick->radius * joystick->radius)) {
				joystick->is_dragging = true;
				joystick_update_drag(joystick, logic_pos.x, logic_pos.y);
			}
		}
		break;
	}

	case SDL_EVENT_MOUSE_MOTION: {
		if (event->motion.which == SDL_TOUCH_MOUSEID) {
			return; // 忽略触摸模拟的鼠标事件
		}

		if (joystick->is_dragging && !joystick->is_touch_active) {
			// 转换坐标到逻辑空间
			SDL_RenderCoordinatesFromWindow(joystick->renderer,
				event->motion.x, event->motion.y,
				&logic_pos.x, &logic_pos.y);

			joystick_update_drag(joystick, logic_pos.x, logic_pos.y);
		}
		break;
	}

	case SDL_EVENT_MOUSE_BUTTON_UP: {
		if (event->button.which == SDL_TOUCH_MOUSEID) {
			return; // 忽略触摸模拟的鼠标事件
		}

		if (event->button.button == SDL_BUTTON_LEFT && joystick->is_dragging && !joystick->is_touch_active) {
			joystick_reset(joystick);
		}
		break;
	}

	case SDL_EVENT_WINDOW_RESIZED: {
		// 这里可以重新调整摇杆位置（如果需要保持相对位置）
		break;
	}
	}
}

// 重置摇杆
void joystick_reset(joystick_p joystick)
{
	joystick->direction.x = 0.0f;
	joystick->direction.y = 0.0f;
	joystick->magnitude = 0.0f;
	joystick->is_dragging = false;
	joystick->is_touch_active = false;
	joystick->touch_id = -1;

	// 手柄回到中心
	joystick->handle.x = joystick->center.x - joystick->handle_radius;
	joystick->handle.y = joystick->center.y - joystick->handle_radius;
}

// 绘制摇杆
void joystick_draw(joystick_p joystick)
{
	SDL_Renderer* renderer = joystick->renderer;

	// 绘制背景圆形
	SDL_SetRenderDrawColor(renderer,
		(JOYSTICK_BG_COLOR >> 24) & 0xFF,
		(JOYSTICK_BG_COLOR >> 16) & 0xFF,
		(JOYSTICK_BG_COLOR >> 8) & 0xFF,
		JOYSTICK_BG_COLOR & 0xFF);
	// 绘制圆形背景（使用多边形模拟圆形）
	shape_draw_circle(renderer, "line", joystick->center, joystick->radius, 32);

	// 绘制十字线
	SDL_SetRenderDrawColor(renderer, 0x66, 0x66, 0x66, 0xFF);
	SDL_RenderLine(renderer,
		joystick->center.x - joystick->radius, joystick->center.y,
		joystick->center.x + joystick->radius, joystick->center.y);
	SDL_RenderLine(renderer,
		joystick->center.x, joystick->center.y - joystick->radius,
		joystick->center.x, joystick->center.y + joystick->radius);

	// 绘制手柄
	SDL_Color handle_color = joystick->is_dragging ?
		(SDL_Color) {
		0x4C, 0xAF, 0x50, 0xFF
	} :
		(SDL_Color) {
		0x88, 0x88, 0x88, 0xFF
	};

		SDL_SetRenderDrawColor(renderer, handle_color.r, handle_color.g, handle_color.b, handle_color.a);

		// 绘制圆形手柄

		SDL_FPoint handle_center = {
		joystick->handle.x + joystick->handle_radius,
		joystick->handle.y + joystick->handle_radius
		};
		shape_draw_circle(renderer, "fill", handle_center, joystick->radius * 0.5f, 24);

		// 填充手柄
		SDL_SetRenderDrawColor(renderer, handle_color.r, handle_color.g, handle_color.b, handle_color.a - 0x40);

		SDL_FRect handle_fill = {
		    joystick->handle.x + 2,
		    joystick->handle.y + 2,
		    joystick->handle.w - 4,
		    joystick->handle.h - 4
		};
		//shape_draw_rectangle(renderer, "fill", handle_fill);



		SDL_RenderFillRect(renderer, &handle_fill);


	/*	if (joystick_get_magnitude(joystick) > 0) {
			SDL_FPoint dir = joystick_get_direction(joystick);
			SDL_RenderLine(renderer,
				joystick->center.x,
				joystick->center.y,
				joystick->center.x + dir.x * joystick->radius,
				joystick->center.y + dir.y * joystick->radius);
		}*/
}

// 获取摇杆方向
SDL_FPoint joystick_get_direction(joystick_p joystick)
{
	return joystick->direction;
}

// 获取摇杆强度
float joystick_get_magnitude(joystick_p joystick)
{
	return joystick->magnitude;
}