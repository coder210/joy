/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-mono.h
Description: mono
Author: ydlc
Version: 1.0
Date: 2025.7.3
History:
*************************************************/
#ifndef LIV_MONO_H
#define LIV_MONO_H

#include <SDL3/SDL.h>


#ifdef LIVNET_CLASSIC_MONO
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/class.h>
#include <mono/metadata/object.h>
#else
typedef void MonoDomain;
typedef void MonoImage;
typedef void MonoClass;
typedef void MonoMethod;
typedef void MonoObject;
typedef void MonoString;
typedef void MonoArray;
typedef int MonoImageOpenStatus;


#endif // LIVNET_CLASSIC_MONO


static inline void livS_mono_set_assemblies_path(const char* path)
{
#ifdef LIVNET_CLASSIC_MONO
        mono_set_assemblies_path(path);
#endif
}

static inline MonoDomain* livS_mono_jit_init(const char* file)
{
        MonoDomain* domain;
#ifdef LIVNET_CLASSIC_MONO
        domain = mono_jit_init(file);
#else
        domain = NULL;
#endif
        return domain;
}

static inline MonoDomain* 
livS_mono_create_appdomain(char* friendly_name, char* config_file)
{
        MonoDomain* domain;
#ifdef LIVNET_CLASSIC_MONO
        domain = mono_domain_create_appdomain(friendly_name, config_file);
#else
        domain = NULL;
#endif
        return domain;
}

static inline void 
livS_mono_domain_set(MonoDomain* domain, bool force)
{
#ifdef LIVNET_CLASSIC_MONO
        mono_domain_set(domain, force);
#else
#endif
}


static inline MonoImage*
livS_mono_image_open(const char* filename, void* (*read_file)(const char*, size_t*))
{
        MonoImage* image;
        MonoImageOpenStatus status;
        char* data;
        size_t data_len;
        data = (char*)read_file(filename, &data_len);
#ifdef LIVNET_CLASSIC_MONO
        image = mono_image_open_from_data(data, data_len, 1, &status);
        if (status == MONO_IMAGE_OK && image) {
                mono_assembly_load_from(image, filename, &status);
        }
#else
        image = NULL;
#endif
        SDL_free(data);
        return image;
}

static inline MonoClass*
livS_mono_class_from_name(MonoImage* image, const char* name_space, const char* name)
{
        MonoClass* klass;
#ifdef LIVNET_CLASSIC_MONO
        klass = mono_class_from_name(image, name_space, name);
#else
        klass = NULL;
#endif
        return klass;
}

static inline MonoMethod*
livS_mono_class_get_method(MonoClass* klass, const char* name, int param_count)
{
        MonoMethod* method;
#ifdef LIVNET_CLASSIC_MONO
        method = mono_class_get_method_from_name(klass, name, param_count);
#else
        method = NULL;
#endif
        return method;
}

static inline MonoObject*
livS_mono_runtime_invoke(MonoMethod* method,
        void* obj, void** params, MonoObject** exc)
{
        MonoObject* object;
#ifdef LIVNET_CLASSIC_MONO
        object = mono_runtime_invoke(method, obj, params, exc);
#else
        object = NULL;
#endif
        return object;
}


static inline MonoString* 
livS_mono_object_to_string(MonoObject* obj, MonoObject** exc)
{
        MonoString* result;
#ifdef LIVNET_CLASSIC_MONO
        result = mono_object_to_string(obj, exc);
#else
        result = NULL;
#endif
        return result;
}

static inline MonoImage* livS_mono_class_get_image(MonoClass* klass)
{
        MonoImage* image;
#ifdef LIVNET_CLASSIC_MONO
        image = mono_class_get_image(klass);
#else
        image = NULL;
#endif
        return image;
}

static inline void livS_mono_image_close(MonoImage* image)
{
#ifdef LIVNET_CLASSIC_MONO
        mono_image_close(image);
#else
#endif
}

static inline void livS_mono_domain_unload(MonoDomain* domain)
{
#ifdef LIVNET_CLASSIC_MONO
        mono_domain_unload(domain);
#else
#endif
}

static inline void livS_mono_jit_cleanup(MonoDomain* domain)
{
#ifdef LIVNET_CLASSIC_MONO
        mono_jit_cleanup(domain);
#else
#endif
}

static inline char* livS_mono_string_to_utf8(MonoString* mono_str)
{
        char* str;
#ifdef LIVNET_CLASSIC_MONO
        str = mono_string_to_utf8(mono_str);
#else
        str = NULL;
#endif
        return str;
}

static inline int livS_mono_string_length(MonoString* s)
{
        int len;
#ifdef LIVNET_CLASSIC_MONO
        len = mono_string_length(s);
#else
        len = 0;
#endif
        return len;
}


static inline void livS_mono_add_internal_call(const char* name, const void* method)
{
#ifdef LIVNET_CLASSIC_MONO
        mono_add_internal_call(name, method);
#else
#endif
}

static inline char *livS_mono_array_addr(MonoArray* bytes, int size, uintptr_t idx)
{
        char* array_addr;
#ifdef LIVNET_CLASSIC_MONO
        array_addr = mono_array_addr_with_size((bytes), size, idx);
#else
        array_addr = NULL;
#endif
        return array_addr;
}

static inline int livS_mono_array_length(MonoArray* array)
{
        int len;
#ifdef LIVNET_CLASSIC_MONO
        len = mono_array_length(array);
#else
        len = 0;
#endif
        return len;
}

static inline MonoArray*
livS_mono_array_new(MonoDomain* domain, MonoClass* eclass, uintptr_t n)
{
        MonoArray* array;
#ifdef LIVNET_CLASSIC_MONO
        array = mono_array_new(domain, eclass, n);
#else
        array = NULL;
#endif
        return array;
}

static inline MonoDomain*
livS_mono_domain_get()
{
        MonoDomain* domain;
#ifdef LIVNET_CLASSIC_MONO
        domain = mono_domain_get();
#else
        domain = NULL;
#endif
        return domain;
}

static inline MonoClass*
livS_mono_get_byte_class() 
{
        MonoClass* mclass;
#ifdef LIVNET_CLASSIC_MONO
        mclass = mono_get_byte_class();
#else
        mclass = NULL;
#endif
        return mclass;
}

static inline MonoString*
livS_mono_string_new(MonoDomain *domain, const char *text)
{
        MonoString* str;
#ifdef LIVNET_CLASSIC_MONO
        str = mono_string_new(domain, text);
#else
        str = NULL;
#endif
        return str;
}

static inline MonoString*
livS_mono_string_new_len(MonoDomain* domain, const char* text, unsigned int len)
{
        MonoString* str;
#ifdef LIVNET_CLASSIC_MONO
        str = mono_string_new_len(domain, text, len);
#else
        str = NULL;
#endif
        return str;
}

static inline void
livS_mono_free(void* pointer)
{
#ifdef LIVNET_CLASSIC_MONO
        mono_free(pointer);
#else
#endif
}

static inline MonoString*
livS_mono_string_empty(MonoDomain *domain)
{
        MonoString* str;
#ifdef LIVNET_CLASSIC_MONO
        str = mono_string_empty(domain);
#else
        str = NULL;
#endif
        return str;
}

static inline MonoObject*
livS_mono_object_new(MonoDomain* domain, MonoClass* klass)
{
        MonoObject* obj;
#ifdef LIVNET_CLASSIC_MONO
        obj = mono_object_new(domain, klass);
#else
        obj = NULL;
#endif
        return obj;
}


static inline void
livS_mono_runtime_object_init(MonoObject *this_class)
{
#ifdef LIVNET_CLASSIC_MONO
        mono_runtime_object_init(this_class);
#else
#endif
}


#endif // !LIV_MONO_H

