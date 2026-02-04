/*********************************************
Copyright (C): 2020-2021, lanchong.xyz/Ltd.
File name: liv-module.c
Description: 动态加载so(linux), dll(windows)扩展库,扩展库必须定时4个函数,
init, create, release, signal
Author: ydlc
Version: 1.0
Date: 2021.12.11
History:
**********************************************/
#include "liv-module.h"
#include <SDL3/SDL.h>
#include "./external/klib/khash.h"
#include "liv-log.h"


KHASH_INIT(kmod, const char*, module_p, 1, kh_str_hash_func, kh_str_hash_equal)


typedef struct modules {
	char path[1024];
	khash_t(kmod)* mods;
}modules_t, * modules_p;



static void* _try_open(const char* path, const char* name)
{
	const char* l;
	size_t path_size = SDL_strlen(path);
	size_t name_size = SDL_strlen(name);
	size_t sz = path_size + name_size;
	void* dl = NULL;
	char* tmp = (char*)SDL_malloc(sizeof(char) * sz);
	SDL_assert(tmp != NULL);
	do {
		SDL_memset(tmp, 0, sz);
		while (*path == ';')
			path++;
		if (*path == '\0')
			break;
		l = strchr(path, ';');
		if (l == NULL) l = path + SDL_strlen(path);
		int len = l - path;
		int i;
		for (i = 0; path[i] != '?' && i < len; i++) {
			tmp[i] = path[i];
		}
		SDL_memcpy(tmp + i, name, name_size);
		if (path[i] == '?') {
			SDL_strlcpy(tmp + i + name_size, path + i + 1, len - i - 1);
		}
		else {
			liv_log_error("Invalid C service path");
			break;
		}
		dl = SDL_LoadObject(tmp);
		path = l;
	} while (dl == NULL);

	SDL_free(tmp);

	if (dl == NULL) {
		char msg[512] = { 0 };
		liv_log_error("try open %s failed : %s", name, msg);
	}

	return dl;
}

static int _open_sym(module_p mod)
{
	size_t name_size, total_size;
	char* tmp;
	name_size = SDL_strlen(mod->name);
	total_size = name_size + 9;
	tmp = (char*)SDL_malloc(sizeof(char) * (total_size));
	if (!tmp) {
		return 0;
	}
	SDL_memcpy(tmp, mod->name, name_size);
	SDL_strlcpy(tmp + name_size, "_create", total_size);
	mod->create = (module_create_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_strlcpy(tmp + name_size, "_init", total_size);
	mod->init = (module_init_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_strlcpy(tmp + name_size, "_event", total_size);
	mod->event_handler = (module_event_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_strlcpy(tmp + name_size, "_update", total_size);
	mod->update = (module_update_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_strlcpy(tmp + name_size, "_release", total_size);
	mod->release = (module_release_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_free(tmp);
	return mod->init != NULL;
}

modules_p liv_modules_create(const char* path)
{
	modules_p mods;
	mods = SDL_malloc(sizeof(modules_t));
	SDL_strlcpy(mods->path, path, 1024);
	mods->mods = kh_init(kmod);
	return mods;
}

void liv_modules_destroy(modules_p modules)
{
	const char* name;
	module_p mod;
	khint_t p;
	for (p = kh_begin(modules->mods); p != kh_end(modules->mods); p++) {
		if (kh_exist(modules->mods, p)) {
			name = kh_key(modules->mods, p);
			mod = kh_val(modules->mods, p);
			if (mod->handle) {
				SDL_UnloadObject(mod->handle);
			}
			SDL_free(mod);
		}
	}
	kh_destroy(kmod, modules->mods);
	SDL_free(modules);
}

bool liv_modules_query(modules_p modules, const char* name, module_p *mod)
{
	khint_t k;
	k = kh_get(kmod, modules->mods, name);
	if (k != kh_end(modules->mods)) {
		*mod = kh_val(modules->mods, k);
		return true;
	}
	else {
		return false;
	}
}

void liv_modules_register(modules_p modules, const char* name,
	module_create_t create,
	module_init_t init,
	module_event_t event_handler,
	module_update_t update,
	module_release_t release)
{
	module_p mod;
	int ret;
	khint_t k;

	k = kh_get(kmod, modules->mods, name);
	if (k == kh_end(modules->mods)) {
		mod = (module_p)SDL_malloc(sizeof(module_t));
		SDL_strlcpy(mod->name, name, 512);
		mod->handle = NULL;
		mod->create = create;
		mod->init = init;
		mod->event_handler = event_handler;
		mod->update = update;
		mod->release = release;
		k = kh_put(kmod, modules->mods, mod->name, &ret);
		kh_val(modules->mods, k) = mod;
	}
}

void liv_modules_require(modules_p modules, const char* name)
{
	int ret;
	void *handle;
	module_p mod;
	khint_t k;

	k = kh_get(kmod, modules->mods, name);
	if (k != kh_end(modules->mods)) {
		return;
	}
	handle = _try_open(modules->path, name);
	if (handle) {
		mod = (module_p)SDL_malloc(sizeof(module_t));
		SDL_strlcpy(mod->name, name, 512);
		if (_open_sym(mod)) {
			k = kh_put(kmod, modules->mods, mod->name, &ret);
			kh_val(modules->mods, k) = mod;
		}
		else {
			SDL_UnloadObject(mod->handle);
			liv_log_error("%s register failed.", mod->name);
			SDL_free(mod);
		}
	}
	else {
		liv_log_error("%s register failed.", name);
	}
}

bool liv_moudles_retire(modules_p modules, const char* name, module_p *mod)
{
	khint_t k;
	k = kh_get(kmod, modules->mods, name);
	if (k == kh_end(modules->mods)) {
		*mod = kh_val(modules->mods, k);
		kh_del(kmod, modules->mods, k);
		return true;
	}
	else {
		liv_log_error("%s register failed.", name);
		return false;
	}
}

void* liv_module_instance_create(module_p mod)
{
	void* inst;
	if (mod->create) {
		inst = mod->create();
	}
	else {
		inst = NULL;
	}
	return inst;
}

void liv_module_instance_init(module_p mod, void* inst, void* ctx, const char* param)
{
	if (mod->init) {
		mod->init(inst, ctx, param);
	}
}

void liv_module_instance_event(module_p mod, void* inst, void *event)
{
	if (mod->event_handler) {
		mod->event_handler(inst, event);
	}
}

void liv_module_instance_update(module_p mod, void* inst, float delta_time)
{
	if (mod->update) {
		mod->update(inst, delta_time);
	}
}

void liv_module_instance_release(module_p mod, void* inst)
{
	if (mod->release) {
		mod->release(inst);
	}
}
