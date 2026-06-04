#include <SDL3/SDL.h>
#include "jsclib.h"
#include <stdlib.h>

/* 供 graphics.c 获取当前窗口 */
static SDL_Window* cur_win = NULL;

SDL_Window* jsclib_get_cur_window(void)
{
	return cur_win;
}

static void free_win(js_State* J, void* p)
{
	(void)J;
	if (p) SDL_DestroyWindow((SDL_Window*)p);
}

static void jwindow_create(js_State* J)
{
	const char* title = js_tostring(J, 1);
	int w = (int)js_tonumber(J, 2);
	int h = (int)js_tonumber(J, 3);
	int flags = (int)js_tonumber(J, 4);
	if (js_isundefined(J, 4)) flags = 0;

	SDL_Window* win = SDL_CreateWindow(title, w, h, flags);
	if (!win) {
		js_pushundefined(J);
		return;
	}
	cur_win = win;
	js_newuserdata(J, "SDL_Window*", win, free_win);
}

void jsopen_window(js_State* J)
{
	js_newobject(J);
	js_newcfunction(J, jwindow_create, "create", 4);
	js_setproperty(J, -2, "create");
	js_setglobal(J, "window");
}
