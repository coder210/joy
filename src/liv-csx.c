/************************************************
Copyright: 2021-2022, lanchong.xyz/Ltd.
File name: mod-csx.c
Description:
Author: ydlc
Version: 1.0
Date: 2021.12.14
History:
*************************************************/
#include <SDL3/SDL.h>
#include "liv-mono.h"
#include "liv-csx.h"
#include "csclib.h"
#include "liv-log.h"
#include "liv-utils.h"

typedef struct csx {
        app_p app;
	MonoImage* image;
        MonoClass* klass;
        MonoObject* instance;
}csx_t, *csx_p;


static void
_load_csharp_script(csx_p csx, const char* root_path, const char* csharp_start)
{
        char filename[LIVNET_MAX_PATH];
        MonoImage* image;
        MonoDomain* app_domain;
        app_domain = liv_get_app_domain(csx->app);
        SDL_memset(filename, 0, LIVNET_MAX_PATH);
        SDL_strlcat(filename, root_path, 1024);
        SDL_strlcat(filename, csharp_start, 1024);
        image = livS_mono_image_open(filename, SDL_LoadFile);
        if (image) {
                csx->klass = livS_mono_class_from_name(image, "", "Program");
                csx->instance = livS_mono_object_new(app_domain, csx->klass);
                livS_mono_runtime_object_init(csx->instance);
        }
}

static bool ccore_running(csx_p csx)
{
        return liv_running(csx->app);
}

static void ccore_quit(csx_p csx)
{
        liv_quit(csx->app);
}

static MonoString* ccore_get_env(csx_p csx, MonoString* name)
{
        const char* cstr_value;
        char* cstr_name;
        cstr_name = livS_mono_string_to_utf8(name);
        cstr_value = liv_get(csx->app, cstr_name);
        livS_mono_free(cstr_name);
        return livS_mono_string_new(livS_mono_domain_get(), cstr_value);
}

static void ccore_set_env(csx_p csx, MonoString* name, MonoString* value)
{
        const char* cstr_value, * cstr_name;
        cstr_name = livS_mono_string_to_utf8(name);
        cstr_value = livS_mono_string_to_utf8(value);
        liv_set(csx->app, cstr_name, cstr_value);
}

static void ccore_log(csx_p csx, MonoString* str)
{
        const char* cstr;
        cstr = livS_mono_string_to_utf8(str);
        liv_info(csx->app, cstr);
        
}



static void csopen_core()
{
        livS_mono_add_internal_call("Livlib.Native.Core::Running", ccore_running);
        livS_mono_add_internal_call("Livlib.Native.Core::Quit", ccore_quit);
        livS_mono_add_internal_call("Livlib.Native.Core::Log", ccore_log);
        livS_mono_add_internal_call("Livlib.Native.Core::GetEnv", ccore_get_env);
        livS_mono_add_internal_call("Livlib.Native.Core::SetEnv", ccore_set_env);
        livS_mono_add_internal_call("Livlib.Native.Core::Log", ccore_log);
}

void* liv_csx_create(void)
{
        const char *root_path;
	csx_p csx;
	csx = (csx_p)SDL_malloc(sizeof(csx_t));
	SDL_assert(csx);
	return csx;
}

int liv_csx_init(void* inst, void *app_ptr, const char* param)
{
        csx_p csx;
        MonoImageOpenStatus status;
        MonoString* exc_str;
        MonoObject* ex;
        MonoMethod* start_method;
        void* args[1];
        const char *root_path;

        csx = (csx_p)inst;
        csx->app = (app_p)app_ptr;
        root_path = liv_get(csx->app, "root_path");

        csopen_sdl();
        csopen_kcp();
        csopen_math();
        csopen_collision();
        csopen_sys();
        csopen_utils();
        csopen_shape_batch();
        csopen_sprite_batch();
        csopen_core();
      
        _load_csharp_script(inst, root_path, param);

        args[0] = &csx;
        start_method = livS_mono_class_get_method(csx->klass, "Start", 1);
        livS_mono_runtime_invoke(start_method, csx->instance, args, &ex);
        if (ex) {
                exc_str = livS_mono_object_to_string(ex, NULL);
                liv_log_info("Exception: %s", livS_mono_string_to_utf8(exc_str));
        }
        return 0;
}

int liv_csx_event_handler(void *inst, void* event)
{
        csx_p csx;
        MonoObject* ex;
        MonoString* exc_str;
        MonoMethod* event_method;
        void* args[1];

        csx = (csx_p)inst;
        args[0] = event;
        event_method = livS_mono_class_get_method(csx->klass, "Event", 1);
        livS_mono_runtime_invoke(event_method, csx->instance, args, &ex);
        if (ex) {
                exc_str = livS_mono_object_to_string(ex, NULL);
                //SDL_Log(livS_mono_string_to_utf8(exc_str));
        }
        return 0;
}

int liv_csx_update(void* inst, float delta_time)
{
        csx_p csx;
        MonoObject* exception;
        MonoString* exc_str;
        MonoMethod* update_method;
        csx = (csx_p)inst;
        void* args[1];

        args[0] = &delta_time;
        update_method = livS_mono_class_get_method(csx->klass, "Update", 1);
        livS_mono_runtime_invoke(update_method, csx->instance, args, &exception);
        if (exception) {
                exc_str = livS_mono_object_to_string(exception, NULL);
                //SDL_Log(livS_mono_string_to_utf8(exc_str));
        }
        return 0;
}

void liv_csx_release(void* inst)
{
        csx_p csx;
        MonoImage* image;
        MonoObject* exception;
        MonoString* exc_str;
        MonoMethod* destroy_method;
        csx = (csx_p)inst;

        destroy_method = livS_mono_class_get_method(csx->klass, "Destroy", 0);
        livS_mono_runtime_invoke(destroy_method, csx->instance, NULL, &exception);
        if (exception) {
                exc_str = livS_mono_object_to_string(exception, NULL);
                //SDL_Log(livS_mono_string_to_utf8(exc_str));
        }

        image = livS_mono_class_get_image(csx->klass);
        livS_mono_image_close(image);
}

