#include "Resources.h"

Resources::Resources(SDL_Renderer* renderer)
{
	this->simheiFont24 = font_create(renderer, "adventure_server_fonts/simhei.ttf", 24);
}

Resources::~Resources()
{
	font_destroy(this->simheiFont24);
}

font_p Resources::GetSimheiFont24()
{
	return this->simheiFont24;
}
