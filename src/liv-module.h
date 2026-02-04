/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-module.h
Description: ²å¼þÏµÍ³
Author: ydlc
Version: 1.0
Date: 2021.3.3
History:
*************************************************/
#ifndef LIV_MODULE_H
#define LIV_MODULE_H
#include <stdbool.h>

typedef void* (*module_create_t)(void);
typedef int (*module_init_t)(void* inst, void* app, const char* param);
typedef int (*module_event_t)(void* inst, void* event);
typedef int (*module_update_t)(void* inst, float delta_time);
typedef void (*module_release_t)(void* inst);

typedef struct module {
	char name[512];
	void* handle;
	module_create_t create;
	module_init_t init;
	module_event_t event_handler;
	module_update_t update;
	module_release_t release;
}module_t, *module_p;

typedef struct modules modules_t, *modules_p;

modules_p liv_modules_create(const char* path);
void liv_modules_destroy(modules_p modules);
bool liv_modules_query(modules_p modules, const char* name, module_p* mod);
void liv_modules_register(modules_p modules, const char* name,
	module_create_t create,
	module_init_t init,
	module_event_t event_handler,
	module_update_t update,
	module_release_t release);
void liv_modules_require(modules_p modules, const char* name);
bool liv_moudles_retire(modules_p modules, const char* name, module_p* mod);

void* liv_module_instance_create(module_p mod);
void liv_module_instance_init(module_p mod, void* inst, void* ctx, const char* param);
void liv_module_instance_event(module_p mod, void* inst, void* event);
void liv_module_instance_update(module_p mod, void* inst, float delta_time);
void liv_module_instance_release(module_p mod, void* inst);

#endif // !LIV_MODULE_H
