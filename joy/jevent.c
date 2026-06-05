#include "jevent.h"
#include <SDL3/SDL.h>
#include "jcore.h"


static const char* key_name(SDL_Keycode k)
{
	static char buf[2];
	switch (k) {
	case SDLK_RETURN:    return "return";
	case SDLK_ESCAPE:    return "escape";
	case SDLK_BACKSPACE: return "backspace";
	case SDLK_TAB:       return "tab";
	case SDLK_SPACE:     return "space";
	case SDLK_DELETE:    return "delete";
	case SDLK_UP:        return "up";
	case SDLK_DOWN:      return "down";
	case SDLK_LEFT:      return "left";
	case SDLK_RIGHT:     return "right";
	case SDLK_HOME:      return "home";
	case SDLK_END:       return "end";
	case SDLK_PAGEUP:    return "pageup";
	case SDLK_PAGEDOWN:  return "pagedown";
	case SDLK_INSERT:    return "insert";
	case SDLK_LSHIFT:    return "lshift";
	case SDLK_RSHIFT:    return "rshift";
	case SDLK_LCTRL:     return "lctrl";
	case SDLK_RCTRL:     return "rctrl";
	case SDLK_LALT:      return "lalt";
	case SDLK_RALT:      return "ralt";
	default:
		if (k >= SDLK_A && k <= SDLK_Z) { buf[0] = (char)('a' + (k - SDLK_A)); buf[1] = 0; return buf; }
		if (k >= SDLK_0 && k <= SDLK_9) { buf[0] = (char)('0' + (k - SDLK_0)); buf[1] = 0; return buf; }
		return NULL;
	}
}


void dispatch_event(event_callbacks_t cbs, const SDL_Event* event, void* arg)
{
	switch (event->type) {
	case SDL_EVENT_MOUSE_BUTTON_DOWN: {
		float x = (float)event->button.x;
		float y = (float)event->button.y;
		int b = event->button.button;
		if (cbs.on_mousepressed)
			cbs.on_mousepressed(x, y, b, arg);
		else
			log_error("unhandled SDL_EVENT_MOUSE_BUTTON_DOWN");
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_UP: {
		float x = (float)event->button.x;
		float y = (float)event->button.y;
		int b = event->button.button;
		if (cbs.on_mousereleased)
			cbs.on_mousereleased(x, y, b, arg);
		else
			log_error("unhandled SDL_EVENT_MOUSE_BUTTON_UP");
		break;
	}
	case SDL_EVENT_MOUSE_MOTION: {
		float x = (float)event->motion.x;
		float y = (float)event->motion.y;
		float dx = event->motion.xrel;
		float dy = event->motion.yrel;
		if (cbs.on_mousemoved)
			cbs.on_mousemoved(x, y, dx, dy, arg);
		else
			log_error("unhandled SDL_EVENT_MOUSE_MOTION");
		break;
	}
	case SDL_EVENT_KEY_DOWN: {
		const char* name = key_name(event->key.key);
		if (!name) break;
		int sc = event->key.scancode;
		bool rep = event->key.repeat;
		bool ctrl = event->key.mod & SDL_KMOD_CTRL;
		bool shift = event->key.mod & SDL_KMOD_SHIFT;
		bool alt = event->key.mod & SDL_KMOD_ALT;
		if (cbs.on_keypressed)
			cbs.on_keypressed(name, sc, rep, ctrl, shift, alt, arg);
		else
			log_error("unhandled SDL_EVENT_KEY_DOWN");
		break;
	}
	case SDL_EVENT_KEY_UP: {
		const char* name = key_name(event->key.key);
		if (!name) break;
		int sc = event->key.scancode;
		if (cbs.on_keyreleased)
			cbs.on_keyreleased(name, sc, arg);
		else
			log_error("unhandled SDL_EVENT_KEY_UP");
		break;
	}
	case SDL_EVENT_TEXT_INPUT: {
		const char* text = event->text.text;
		if (cbs.on_textinput)
			cbs.on_textinput(text, arg);
		else
			log_error("unhandled SDL_EVENT_TEXT_INPUT");
		break;
	}
	case SDL_EVENT_WINDOW_RESIZED: {
		int w = event->window.data1;
		int h = event->window.data2;
		if (cbs.on_resize)
			cbs.on_resize(w, h, arg);
		else
			log_error("unhandled SDL_EVENT_WINDOW_RESIZED");
		break;
	}
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
		SDL_Event quit;
		SDL_zero(quit);
		quit.type = SDL_EVENT_QUIT;
		SDL_PushEvent(&quit);
		break;
	}
	default:
		log_debug("unhandled SDL event type: 0x%x", event->type);
		break;
	}
}
