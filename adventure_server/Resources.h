#ifndef RESOURCES_H
#define RESOURCES_H
#include <joy2d/jui.h>
#include <SDL3/SDL.h>
#include <string>

class Resources
{
public:
	Resources(SDL_Renderer* renderer);
	~Resources();

	font_p GetSimheiFont24();

private:
	font_p simheiFont24;
};


#endif