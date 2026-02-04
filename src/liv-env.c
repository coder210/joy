/************************************************
Copyright: 2021-2022, lanchong.xyz/Ltd.
File name: liv-env.c
Description: 存储变量的值
Author: ydlc
Version: 1.0
Date: 2021.12.14
History:
*************************************************/
#include "liv-env.h"
#include <SDL3/SDL.h>

typedef struct env {
	SDL_Environment* sdl_env;
}env_t, *env_p;

env_p liv_env_create(void)
{
	env_p env;
	env = SDL_malloc(sizeof(env_t));
	SDL_assert(env);
	env->sdl_env = SDL_CreateEnvironment(true);
	SDL_assert(env->sdl_env);
	return env;
}

const char *liv_env_get(env_p env, const char *key)
{
	return SDL_GetEnvironmentVariable(env->sdl_env, key);
}

void liv_env_set(env_p env, const char *key, const char *value)
{
	SDL_SetEnvironmentVariable(env->sdl_env, key, value, true);
}

void liv_env_destroy(env_p env)
{
	SDL_assert(env);
	SDL_DestroyEnvironment(env->sdl_env);
	SDL_free(env);
}
