#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"
#include "SDL3/SDL.h"
#include "liv-ecs.h"
#include "luaclib.h"


static int l_camera_count(lua_State* L)
{
	SDL_Camera* camera;
	SDL_CameraID* devices = NULL;
	int devcount = 0;
	devices = SDL_GetCameras(&devcount);
	if (devices == NULL) {
		SDL_Log("Couldn't enumerate camera devices: %s", SDL_GetError());
	}
	else if (devcount == 0) {
		SDL_Log("Couldn't find any camera devices! Please connect a camera and try again.");
	}
	camera = SDL_OpenCamera(devices[0], NULL);  // just take the first thing we see in any format it wants.
	SDL_free(devices);
	if (camera == NULL) {
		SDL_Log("Couldn't open camera: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	lua_pushlightuserdata(L, camera);
	return 0;
}

static int l_camera_open(lua_State* L)
{
	SDL_Camera* camera;
	SDL_CameraID* devices = NULL;
	int devcount = 0;
	devices = SDL_GetCameras(&devcount);
	if (devices == NULL) {
		SDL_Log("Couldn't enumerate camera devices: %s", SDL_GetError());
	}
	else if (devcount == 0) {
		SDL_Log("Couldn't find any camera devices! Please connect a camera and try again.");
	}
	camera = SDL_OpenCamera(devices[0], NULL);  // just take the first thing we see in any format it wants.
	SDL_free(devices);
	if (camera == NULL) {
		SDL_Log("Couldn't open camera: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	lua_pushlightuserdata(L, camera);
	return 0;
}

static int l_camera_acquire_frame(lua_State* L)
{
	SDL_Camera* camera;
	Uint64 timestampNS = 0;
	SDL_Surface* frame;
	camera = (SDL_Camera*)lua_touserdata(L, 1);
	frame = SDL_AcquireCameraFrame(camera, &timestampNS);
	if (frame != NULL) {
		/* Some platforms (like Emscripten) don't know _what_ the camera offers
		   until the user gives permission, so we build the texture and resize
		   the window when we get a first frame from the camera. */
		   //if (!texture) {
		   //	SDL_SetWindowSize(window, frame->w, frame->h);  /* Resize the window to match */
		   //	texture = SDL_CreateTexture(renderer, frame->format, SDL_TEXTUREACCESS_STREAMING, frame->w, frame->h);
		   //}

		   //if (texture) {
		   //	SDL_UpdateTexture(texture, NULL, frame->pixels, frame->pitch);
		   //}

		SDL_ReleaseCameraFrame(camera, frame);
	}

	return 0;
}

static int l_camera_close(lua_State* L)
{
	SDL_Camera* camera;
	camera = (SDL_Camera*)lua_touserdata(L, 1);
	SDL_CloseCamera(camera);
	return 0;
}

int luaopen_camera(lua_State* L)
{
	luaL_checkversion(L);
	luaL_Reg l[] = {
		{"open", l_camera_open},
		{"close", l_camera_close},
		{"acquire_frame", l_camera_acquire_frame},
		{NULL, NULL}
	};
	luaL_newlib(L, l);
	return 1;
}
