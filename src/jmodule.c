/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: module.h
Description: 插件系统
Author: ydlc
Version: 1.0
Date: 2021.3.3
History:

插件系统为可选模块：由 JOY_ENABLE_PLUGINS 控制（见 cmake/JoySources.cmake），
CMake 转发为 JOY_USE_PLUGINS。未启用时本文件为空编译单元，不影响任何项目。
公开声明在 japi.h 的 JOY_USE_PLUGINS 条件段内，两者必须同步。
*************************************************/

#ifdef JOY_USE_PLUGINS

#include <SDL3/SDL.h>
#include "external/klib/khash.h"
#include "joy.h"


KHASH_INIT(kmod, const char*, joy_module_p, 1, kh_str_hash_func, kh_str_hash_equal)


struct joy_modules {
	char path[1024];
	khash_t(kmod)* mods;
};

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
			joy_core_log_error("Invalid C service path");
			break;
		}
		dl = SDL_LoadObject(tmp);
		path = l;
	} while (dl == NULL);

	SDL_free(tmp);

	if (dl == NULL) {
		char msg[512] = { 0 };
		joy_core_log_error("try open %s failed : %s", name, msg);
	}

	return dl;
}

static int _open_sym(joy_module_p mod)
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
	mod->create = (joy_module_create_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_strlcpy(tmp + name_size, "_init", total_size);
	mod->init = (joy_module_init_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_strlcpy(tmp + name_size, "_event", total_size);
	mod->event_handler = (joy_module_event_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_strlcpy(tmp + name_size, "_update", total_size);
	mod->update = (joy_module_update_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_strlcpy(tmp + name_size, "_release", total_size);
	mod->release = (joy_module_release_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_free(tmp);
	return mod->init != NULL;
}

joy_modules_p joy_script_module_create(const char* path)
{
	joy_modules_p mods;
	mods = SDL_malloc(sizeof(joy_modules_t));
	SDL_strlcpy(mods->path, path, 1024);
	mods->mods = kh_init(kmod);
	return mods;
}

void joy_script_module_destroy(joy_modules_p modules)
{
	const char* name;
	joy_module_p mod;
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

bool joy_script_module_query(joy_modules_p modules, const char* name, joy_module_p *mod)
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

void joy_script_module_register(joy_modules_p modules, const char* name,
	joy_module_create_t create,
	joy_module_init_t init,
	joy_module_event_t event_handler,
	joy_module_update_t update,
	joy_module_release_t release)
{
	joy_module_p mod;
	int ret;
	khint_t k;

	k = kh_get(kmod, modules->mods, name);
	if (k == kh_end(modules->mods)) {
		mod = (joy_module_p)SDL_malloc(sizeof(joy_module_t));
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

void joy_script_module_require(joy_modules_p modules, const char* name)
{
	int ret;
	void *handle;
	joy_module_p mod;
	khint_t k;

	k = kh_get(kmod, modules->mods, name);
	if (k != kh_end(modules->mods)) {
		return;
	}
	handle = _try_open(modules->path, name);
	if (handle) {
		mod = (joy_module_p)SDL_malloc(sizeof(joy_module_t));
		SDL_strlcpy(mod->name, name, 512);
		mod->handle = handle;   /* _open_sym 与 destroy 都要用到 DLL 句柄 */
		if (_open_sym(mod)) {
			k = kh_put(kmod, modules->mods, mod->name, &ret);
			kh_val(modules->mods, k) = mod;
		}
		else {
			SDL_UnloadObject(handle);
			joy_core_log_error("%s register failed.", mod->name);
			SDL_free(mod);
		}
	}
	else {
		joy_core_log_error("%s register failed.", name);
	}
}

bool joy_script_modules_retire(joy_modules_p modules, const char* name, joy_module_p *mod)
{
	khint_t k;
	k = kh_get(kmod, modules->mods, name);
	if (k != kh_end(modules->mods)) {
		*mod = kh_val(modules->mods, k);
		kh_del(kmod, modules->mods, k);
		return true;
	}
	else {
		joy_core_log_error("%s not found.", name);
		return false;
	}
}

void* joy_script_module_instance_create(joy_module_p mod)
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

void joy_script_module_instance_init(joy_module_p mod, void* inst, void* ctx, const char* param)
{
	if (mod->init) {
		mod->init(inst, ctx, param);
	}
}

void joy_script_module_instance_event(joy_module_p mod, void* inst, void *event)
{
	if (mod->event_handler) {
		mod->event_handler(inst, event);
	}
}

void joy_script_module_instance_update(joy_module_p mod, void* inst, float delta_time)
{
	if (mod->update) {
		mod->update(inst, delta_time);
	}
}

void joy_script_module_instance_release(joy_module_p mod, void* inst)
{
	if (mod->release) {
		mod->release(inst);
	}
}

#endif /* JOY_USE_PLUGINS */
