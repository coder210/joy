#include "jsclib.h"
#include "SDL3/SDL.h"

static void jevent_quit(js_State *J)
{
        (void)J;
        SDL_Event quit;
        SDL_zero(quit);
        quit.type = SDL_EVENT_QUIT;
        SDL_PushEvent(&quit);
}

void jsopen_event(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jevent_quit, "quit", 0);
        js_setproperty(J, -2, "quit");
        js_setglobal(J, "event");
}
