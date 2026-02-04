/*********************************************
Copyright (C): 2020-2021, lanchong.xyz/Ltd.
File name: liv-server.c
Description:
Author: ydlc
Version: 1.0
Date: 2021.12.11
History:
**********************************************/
#include "./external/lua/lua.h"
#include "./external/lua/lualib.h"
#include "./external/lua/lauxlib.h"
#include "./external/klib/klist.h"
#include "liv.h"
#include "liv-config.h"
#include "liv-env.h"
#include "liv-module.h"
#include "liv-log.h"
#include "liv-sys.h"
#include "liv-utils.h"
#include "liv-proto.h"
#include "liv-luax.h"
#include "liv-jsx.h"
#include "liv-csx.h"

typedef struct context {
        char modname[LIVNET_MIN_BUFFER];
        void* inst;
}context_t, * context_p;

struct __kl1_kcontext;
void _context_free(struct __kl1_kcontext* ctx);
struct __kl1_kcontext {
        context_p data; struct __kl1_kcontext* next;
}; typedef struct __kl1_kcontext kl1_kcontext; typedef struct {
        size_t cnt, n, max; kl1_kcontext** buf;
} kmp_kcontext_t; static __inline kmp_kcontext_t* kmp_init_kcontext() {
        return calloc(1, sizeof(kmp_kcontext_t));
} static __inline void kmp_destroy_kcontext(kmp_kcontext_t* mp) {
        size_t k; for (k = 0; k < mp->n; ++k) {
                _context_free(mp->buf[k]); free(mp->buf[k]);
        } free(mp->buf); free(mp);
} static __inline kl1_kcontext* kmp_alloc_kcontext(kmp_kcontext_t* mp) {
        ++mp->cnt; if (mp->n == 0) return calloc(1, sizeof(kl1_kcontext)); return mp->buf[--mp->n];
} static __inline void kmp_free_kcontext(kmp_kcontext_t* mp, kl1_kcontext* p) {
        --mp->cnt; if (mp->n == mp->max) {
                mp->max = mp->max ? mp->max << 1 : 16; mp->buf = realloc(mp->buf, sizeof(void*) * mp->max);
        } mp->buf[mp->n++] = p;
} typedef struct {
        kl1_kcontext* head, * tail; kmp_kcontext_t* mp; size_t size;
} kl_kcontext_t; static __inline kl_kcontext_t* kl_init_kcontext() {
        kl_kcontext_t* kl = calloc(1, sizeof(kl_kcontext_t)); kl->mp = kmp_init_kcontext(); kl->head = kl->tail = kmp_alloc_kcontext(kl->mp); kl->head->next = 0; return kl;
} static __inline void kl_destroy_kcontext(kl_kcontext_t* kl) {
        kl1_kcontext* p; for (p = kl->head; p != kl->tail; p = p->next) kmp_free_kcontext(kl->mp, p); kmp_free_kcontext(kl->mp, p); kmp_destroy_kcontext(kl->mp); free(kl);
} static __inline context_p* kl_pushp_kcontext(kl_kcontext_t* kl) {
        kl1_kcontext* q, * p = kmp_alloc_kcontext(kl->mp); q = kl->tail; p->next = 0; kl->tail->next = p; kl->tail = p; ++kl->size; return &q->data;
} static __inline int kl_shift_kcontext(kl_kcontext_t* kl, context_p* d) {
        kl1_kcontext* p; if (kl->head->next == 0) return -1; --kl->size; p = kl->head; kl->head = kl->head->next; if (d) *d = p->data; kmp_free_kcontext(kl->mp, p); return 0;
}

void _context_free(struct __kl1_kcontext* ctx)
{
        SDL_free(ctx->data);
}

typedef struct app {
        env_p env;
        modules_p modules;
        klist_t(kcontext)* servers;
        MonoDomain* root_domain;
        MonoDomain* app_domain;
        bool running;
        Uint32 last_frame_time;
        void (*quit)(app_p);
}app_t, * app_p;


static void _init_env(lua_State* L, env_p env)
{
        const char* value;
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
                int keyt = lua_type(L, -2);
                if (keyt != LUA_TSTRING) {
                        liv_log_error("Invalid config table");
                        return;
                }
                const char* key = lua_tostring(L, -2);
                if (lua_type(L, -1) == LUA_TBOOLEAN) {
                        value = lua_toboolean(L, -1) ? "true" : "false";
                        liv_env_set(env, key, value);
                }
                else {
                        value = lua_tostring(L, -1);
                        if (value == NULL) {
                                liv_log_error("Invalid config table key = %s", key);
                                return;
                        }
                        liv_env_set(env, key, value);
                }
                lua_pop(L, 1);
        }
        lua_pop(L, 1);
}

static context_p liv_new_service(app_p app, const char* filename)
{
        context_p ctx;
        module_p mod;
        const char* modname;
        if (!filename) {
                return NULL;
        }
        if (SDL_strstr(filename, ".lua")) {
                modname = "luax";
        }
        else if (SDL_strstr(filename, ".js")) {
                modname = "jsx";
        }
        else if (SDL_strstr(filename, ".dll")) {
                modname = "csx";
        }
        else {
                modname = NULL;
        }
        if (liv_modules_query(app->modules, modname, &mod)) {
                ctx = (context_p)SDL_malloc(sizeof(context_t));
                SDL_strlcpy(ctx->modname, modname, LIVNET_MIN_BUFFER);
                ctx->inst = liv_module_instance_create(mod);
                liv_module_instance_init(mod, ctx->inst, app, filename);
                *kl_pushp(kcontext, app->servers) = ctx;
        }
        else {
                ctx = NULL;
        }
        return ctx;
}

app_p liv_create(const char* file_data)
{
        app_p app;
        const char *start, *cpath, *orientation;
        lua_State* L;

        L = luaL_newstate();
        //root_path = SDL_GetBasePath();
        sys_init_netenv();
        //livS_mono_set_assemblies_path(root_path);

        app = (app_p)SDL_malloc(sizeof(app_t));
        SDL_memset(app, 0, sizeof(app_t));
        app->env = liv_env_create();
        app->root_domain = livS_mono_jit_init("Liv");
        app->app_domain = livS_mono_create_appdomain("LivScript", NULL);
        app->servers = kl_init(kcontext);
        app->quit = liv_quit;
        app->last_frame_time = SDL_GetTicks();
        if (luaL_dostring(L, file_data) == LUA_OK) {
                app->running = true;
        }
        else {
                app->running = false;
        }

        livS_mono_domain_set(app->app_domain, true);
        _init_env(L, app->env);
        lua_close(L);

        orientation = liv_env_get(app->env, "orientation");
        if (orientation) {
                SDL_SetHint(SDL_HINT_ORIENTATIONS, orientation);
        }

        cpath = liv_env_get(app->env, "cpath");
        app->modules = liv_modules_create(cpath);
        liv_modules_register(app->modules, "luax",
                liv_luax_create, liv_luax_init, liv_luax_event_handler,
                liv_luax_update, liv_luax_release);
        liv_modules_register(app->modules, "jsx",
                liv_jsx_create, liv_jsx_init, liv_jsx_event_handler,
                liv_jsx_update, liv_jsx_release);
        liv_modules_register(app->modules, "csx",
                liv_csx_create, liv_csx_init, liv_csx_event_handler,
                liv_csx_update, liv_csx_release);

        start = liv_env_get(app->env, "start");
        liv_new_service(app, start);

        return app;
}

void liv_destroy(app_p app)
{
        module_p mod;
        context_p ctx;
        kliter_t(kcontext)* p;
        p = kl_begin(app->servers);
        while (p != kl_end(app->servers)) {
                ctx = kl_val(p);
                if (liv_modules_query(app->modules, ctx->modname, &mod)) {
                        liv_module_instance_release(mod, ctx->inst);
                }
                p = kl_next(p);
        }
        kl_destroy(kcontext, app->servers);
        liv_modules_destroy(app->modules);
        liv_env_destroy(app->env);
        livS_mono_domain_set(app->root_domain, false);
        livS_mono_domain_unload(app->app_domain);
        livS_mono_jit_cleanup(app->root_domain);
        SDL_free(app);
        sys_release_netenv();
}

void liv_update(app_p app)
{
        module_p mod;
        context_p ctx;
        float delta_time;

        Uint32 current_frame_time = SDL_GetTicks();
        delta_time = (current_frame_time - app->last_frame_time) / 1000.0f;
        app->last_frame_time = current_frame_time;

        kliter_t(kcontext)* p;
        p = kl_begin(app->servers);
        while (p != kl_end(app->servers)) {
                ctx = kl_val(p);
                if (liv_modules_query(app->modules, ctx->modname, &mod)) {
                        liv_module_instance_update(mod, ctx->inst, delta_time);
                }
                p = kl_next(p);
        }
}

void liv_event(app_p app, void* event)
{
        module_p mod;
        context_p ctx;
        kliter_t(kcontext)* p;
        p = kl_begin(app->servers);
        while (p != kl_end(app->servers)) {
                ctx = kl_val(p);
                if (liv_modules_query(app->modules, ctx->modname, &mod)) {
                        liv_module_instance_event(mod, ctx->inst, event);
                }
                p = kl_next(p);
        }
}

bool liv_running(app_p app)
{
        return app->running;
}

void liv_quit(app_p app)
{
        app->running = false;
}

void liv_set(app_p app, const char* key, const char* value)
{
        liv_env_set(app->env, key, value);
}

const char* liv_get(app_p app, const char* key)
{
        return liv_env_get(app->env, key);
}

MonoDomain* liv_get_app_domain(app_p app)
{
        return app->app_domain;
}

void liv_info(app_p app, const char *str)
{
        liv_log_info("%s", str);
}
