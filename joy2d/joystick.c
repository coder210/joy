#include <math.h>
#include "joystick.h"
#include "graphics.h"

// ��ɫ���
#define JOYSTICK_BG_COLOR 0x555555FF
#define JOYSTICK_HANDLE_COLOR 0x888888FF
#define JOYSTICK_HANDLE_ACTIVE_COLOR 0x4CAF50FF

// ҡ�˽ṹ��
struct joystick {
	SDL_Renderer* renderer;      // ��Ⱦ��
	SDL_FRect background;        // ��������
	SDL_FRect handle;           // �ֱ�����
	SDL_FPoint center;          // ҡ�����ĵ�
	float radius;               // �����뾶
	float handle_radius;        // �ֱ��뾶
	SDL_FPoint direction;       // �������� (x: -1��1, y: -1��1)
	float magnitude;            // ǿ�� (0��1)
	float deadzone;             // ������ֵ
	bool is_dragging;           // �Ƿ������϶�
	bool is_touch_active;       // �Ƿ�������
	int touch_id;               // ����ID
};

// ������������
static float distance_squared(float x1, float y1, float x2, float y2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	return dx * dx + dy * dy;
}

// ��ʼ��ҡ��
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

// ����ҡ��λ��
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

// ����ҡ����ק����
static void joystick_update_drag(joystick_p joystick, float touch_x, float touch_y)
{
	// ������������ĵ������
	float dx = touch_x - joystick->center.x;
	float dy = touch_y - joystick->center.y;

	// �������
	float distance = sqrtf(dx * dx + dy * dy);

	// ���ƾ��벻���������뾶
	if (distance > joystick->radius) {
		dx = (dx / distance) * joystick->radius;
		dy = (dy / distance) * joystick->radius;
		distance = joystick->radius;
	}

	// ���㷽���ǿ��
	joystick->magnitude = distance / joystick->radius;

	// Ӧ������
	if (joystick->magnitude < joystick->deadzone) {
		joystick->direction.x = 0.0f;
		joystick->direction.y = 0.0f;
		joystick->magnitude = 0.0f;

		// �ֱ��ص�����
		joystick->handle.x = joystick->center.x - joystick->handle_radius;
		joystick->handle.y = joystick->center.y - joystick->handle_radius;
	}
	else {
		// ��һ����������
		joystick->direction.x = dx / joystick->radius;
		joystick->direction.y = dy / joystick->radius;

		// �����ֱ�λ��
		joystick->handle.x = joystick->center.x + dx - joystick->handle_radius;
		joystick->handle.y = joystick->center.y + dy - joystick->handle_radius;
	}
}

// �����¼�������Ӧ�߼��ֱ��ʣ�
void joystick_handle_event(joystick_p joystick, SDL_Event* event)
{
	SDL_FPoint logic_pos;
	int window_width, window_height;

	// ��ȡ��Ⱦ������ߴ�
	SDL_GetRenderOutputSize(joystick->renderer, &window_width, &window_height);

	switch (event->type) {
	case SDL_EVENT_FINGER_DOWN: {
		// ת�����굽�߼��ռ�
		SDL_RenderCoordinatesFromWindow(joystick->renderer,
			event->tfinger.x * window_width,
			event->tfinger.y * window_height,
			&logic_pos.x, &logic_pos.y);

		// ����Ƿ���ҡ��������
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
			// ת�����굽�߼��ռ�
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
			return; // ���Դ���ģ�������¼�
		}

		if (event->button.button == SDL_BUTTON_LEFT) {
			// ת�����굽�߼��ռ�
			SDL_RenderCoordinatesFromWindow(joystick->renderer,
				event->button.x, event->button.y,
				&logic_pos.x, &logic_pos.y);

			// ����Ƿ���ҡ��������
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
			return; // ���Դ���ģ�������¼�
		}

		if (joystick->is_dragging && !joystick->is_touch_active) {
			// ת�����굽�߼��ռ�
			SDL_RenderCoordinatesFromWindow(joystick->renderer,
				event->motion.x, event->motion.y,
				&logic_pos.x, &logic_pos.y);

			joystick_update_drag(joystick, logic_pos.x, logic_pos.y);
		}
		break;
	}

	case SDL_EVENT_MOUSE_BUTTON_UP: {
		if (event->button.which == SDL_TOUCH_MOUSEID) {
			return; // ���Դ���ģ�������¼�
		}

		if (event->button.button == SDL_BUTTON_LEFT && joystick->is_dragging && !joystick->is_touch_active) {
			joystick_reset(joystick);
		}
		break;
	}

	case SDL_EVENT_WINDOW_RESIZED: {
		// ����������µ���ҡ��λ�ã������Ҫ�������λ�ã�
		break;
	}
	}
}

// ����ҡ��
void joystick_reset(joystick_p joystick)
{
	joystick->direction.x = 0.0f;
	joystick->direction.y = 0.0f;
	joystick->magnitude = 0.0f;
	joystick->is_dragging = false;
	joystick->is_touch_active = false;
	joystick->touch_id = -1;

	// �ֱ��ص�����
	joystick->handle.x = joystick->center.x - joystick->handle_radius;
	joystick->handle.y = joystick->center.y - joystick->handle_radius;
}

// ����ҡ��
void joystick_draw(joystick_p joystick)
{
	SDL_Renderer* renderer = joystick->renderer;

	// ���Ʊ���Բ��
	SDL_SetRenderDrawColor(renderer,
		(JOYSTICK_BG_COLOR >> 24) & 0xFF,
		(JOYSTICK_BG_COLOR >> 16) & 0xFF,
		(JOYSTICK_BG_COLOR >> 8) & 0xFF,
		JOYSTICK_BG_COLOR & 0xFF);
	// ����Բ�α�����ʹ�ö����ģ��Բ�Σ�
	shape_draw_circle(renderer, "line", joystick->center, joystick->radius, 32);

	// ����ʮ����
	SDL_SetRenderDrawColor(renderer, 0x66, 0x66, 0x66, 0xFF);
	SDL_RenderLine(renderer,
		joystick->center.x - joystick->radius, joystick->center.y,
		joystick->center.x + joystick->radius, joystick->center.y);
	SDL_RenderLine(renderer,
		joystick->center.x, joystick->center.y - joystick->radius,
		joystick->center.x, joystick->center.y + joystick->radius);

	// �����ֱ�
	SDL_Color handle_color = joystick->is_dragging ?
		(SDL_Color) {
		0x4C, 0xAF, 0x50, 0xFF
	} :
		(SDL_Color) {
		0x88, 0x88, 0x88, 0xFF
	};

		SDL_SetRenderDrawColor(renderer, handle_color.r, handle_color.g, handle_color.b, handle_color.a);

		// ����Բ���ֱ�

		SDL_FPoint handle_center = {
		joystick->handle.x + joystick->handle_radius,
		joystick->handle.y + joystick->handle_radius
		};
		shape_draw_circle(renderer, "fill", handle_center, joystick->radius * 0.5f, 24);

		// ����ֱ�
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

// ��ȡҡ�˷���
SDL_FPoint joystick_get_direction(joystick_p joystick)
{
	return joystick->direction;
}

// ��ȡҡ��ǿ��
float joystick_get_magnitude(joystick_p joystick)
{
	return joystick->magnitude;
}