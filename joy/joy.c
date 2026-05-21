///*********************************************
//Copyright (C): 2020-2021, lanchong.xyz/Ltd.
//File name: joy.c
//Description:
//Author: ydlc
//Version: 1.0
//Date: 2021.12.11
//History:
//**********************************************/
//#include "./external/../external/lua/lua.h"
//#include "./external/../external/lua/lualib.h"
//#include "./external/../external/lua/lauxlib.h"
//#include "./external/klib/klist.h"
//#include "jconfig.h"
//#include "env.h"
//#include "module.h"
//#include "log.h"
//#include "jsys.h"
//#include "jutils.h"
//#include "proto.h"
//#include "./luax.h"
//#include "./jsx.h"
//#include "./csx.h"
//#include "joy.h"
//
//typedef struct context {
//        char modname[JOY_MIN_BUFFER];
//        void* inst;
//}context_t, * context_p;
//
//struct __kl1_kcontext;
//void _context_free(struct __kl1_kcontext* ctx);
//struct __kl1_kcontext {
//        context_p data; struct __kl1_kcontext* next;
//}; typedef struct __kl1_kcontext kl1_kcontext; typedef struct {
//        size_t cnt, n, max; kl1_kcontext** buf;
//} kmp_kcontext_t; static __inline kmp_kcontext_t* kmp_init_kcontext() {
//        return calloc(1, sizeof(kmp_kcontext_t));
//} static __inline void kmp_destroy_kcontext(kmp_kcontext_t* mp) {
//        size_t k; for (k = 0; k < mp->n; ++k) {
//                _context_free(mp->buf[k]); free(mp->buf[k]);
//        } free(mp->buf); free(mp);
//} static __inline kl1_kcontext* kmp_alloc_kcontext(kmp_kcontext_t* mp) {
//        ++mp->cnt; if (mp->n == 0) return calloc(1, sizeof(kl1_kcontext)); return mp->buf[--mp->n];
//} static __inline void kmp_free_kcontext(kmp_kcontext_t* mp, kl1_kcontext* p) {
//        --mp->cnt; if (mp->n == mp->max) {
//                mp->max = mp->max ? mp->max << 1 : 16; mp->buf = realloc(mp->buf, sizeof(void*) * mp->max);
//        } mp->buf[mp->n++] = p;
//} typedef struct {
//        kl1_kcontext* head, * tail; kmp_kcontext_t* mp; size_t size;
//} kl_kcontext_t; static __inline kl_kcontext_t* kl_init_kcontext() {
//        kl_kcontext_t* kl = calloc(1, sizeof(kl_kcontext_t)); kl->mp = kmp_init_kcontext(); kl->head = kl->tail = kmp_alloc_kcontext(kl->mp); kl->head->next = 0; return kl;
//} static __inline void kl_destroy_kcontext(kl_kcontext_t* kl) {
//        kl1_kcontext* p; for (p = kl->head; p != kl->tail; p = p->next) kmp_free_kcontext(kl->mp, p); kmp_free_kcontext(kl->mp, p); kmp_destroy_kcontext(kl->mp); free(kl);
//} static __inline context_p* kl_pushp_kcontext(kl_kcontext_t* kl) {
//        kl1_kcontext* q, * p = kmp_alloc_kcontext(kl->mp); q = kl->tail; p->next = 0; kl->tail->next = p; kl->tail = p; ++kl->size; return &q->data;
//} static __inline int kl_shift_kcontext(kl_kcontext_t* kl, context_p* d) {
//        kl1_kcontext* p; if (kl->head->next == 0) return -1; --kl->size; p = kl->head; kl->head = kl->head->next; if (d) *d = p->data; kmp_free_kcontext(kl->mp, p); return 0;
//}
//
//void _context_free(struct __kl1_kcontext* ctx)
//{
//        SDL_free(ctx->data);
//}
//
//struct app {
//        env_p env;
//        modules_p modules;
//        klist_t(kcontext)* servers;
//        MonoDomain* root_domain;
//        MonoDomain* app_domain;
//        bool running;
//        Uint32 last_frame_time;
//        void (*quit)(app_p);
//};

//static void _init_env(lua_State* L, env_p env)
//{
//        const char* value;
//        lua_pushnil(L);
//        while (lua_next(L, -2) != 0) {
//                int keyt = lua_type(L, -2);
//                if (keyt != LUA_TSTRING) {
//                        log_error("Invalid config table");
//                        return;
//                }
//                const char* key = lua_tostring(L, -2);
//                if (lua_type(L, -1) == LUA_TBOOLEAN) {
//                        value = lua_toboolean(L, -1) ? "true" : "false";
//                        env_set(env, key, value);
//                }
//                else {
//                        value = lua_tostring(L, -1);
//                        if (value == NULL) {
//                                log_error("Invalid config table key = %s", key);
//                                return;
//                        }
//                        env_set(env, key, value);
//                }
//                lua_pop(L, 1);
//        }
//        lua_pop(L, 1);
//}
//
//static context_p new_service(app_p app, const char* filename)
//{
//        context_p ctx;
//        module_p mod;
//        const char* modname;
//        if (!filename) {
//                return NULL;
//        }
//        if (SDL_strstr(filename, ".lua")) {
//                modname = "luax";
//        }
//        else if (SDL_strstr(filename, ".js")) {
//                modname = "jsx";
//        }
//        else if (SDL_strstr(filename, ".dll")) {
//                modname = "csx";
//        }
//        else {
//                modname = NULL;
//        }
//        if (modules_query(app->modules, modname, &mod)) {
//                ctx = (context_p)SDL_malloc(sizeof(context_t));
//                SDL_strlcpy(ctx->modname, modname, JOY_MIN_BUFFER);
//                ctx->inst = module_instance_create(mod);
//                module_instance_init(mod, ctx->inst, app, filename);
//                *kl_pushp(kcontext, app->servers) = ctx;
//        }
//        else {
//                ctx = NULL;
//        }
//        return ctx;
//}
//
//app_p joy_create(const char* config_file)
//{
//        app_p app;
//        const char *start, *cpath, *orientation;
//        char* file_data;
//        size_t data_size;
//        lua_State* L;
//
//        app = NULL;
//
//        log_debug(SDL_GetBasePath());
//        log_debug("config_file=%s", config_file);
//        file_data = SDL_LoadFile(config_file, &data_size);
//        if (!file_data) {
//                return app;
//        }
//
//        _print_title();
//
//        L = luaL_newstate();
//        //root_path = SDL_GetBasePath();
//        sys_init_netenv();
//        //livS_mono_set_assemblies_path(root_path);
//
//        app = (app_p)SDL_malloc(sizeof(app_t));
//        SDL_memset(app, 0, sizeof(app_t));
//        app->env = env_create();
//        app->root_domain = livS_mono_jit_init("Joy");
//        app->app_domain = livS_mono_create_appdomain("JoyScript", NULL);
//        app->servers = kl_init(kcontext);
//        app->quit = joy_quit;
//        app->last_frame_time = SDL_GetTicks();
//        if (luaL_dostring(L, file_data) == LUA_OK) {
//                app->running = true;
//        }
//        else {
//                app->running = false;
//        }
//
//        SDL_free(file_data);
//
//        livS_mono_domain_set(app->app_domain, true);
//        _init_env(L, app->env);
//        lua_close(L);
//
//        orientation = env_get(app->env, "orientation");
//        if (orientation) {
//                SDL_SetHint(SDL_HINT_ORIENTATIONS, orientation);
//        }
//
//        cpath = env_get(app->env, "cpath");
//        app->modules = modules_create(cpath);
//        modules_register(app->modules, "luax",
//                luax_create, luax_init, luax_event_handler,
//                luax_update, luax_release);
//        modules_register(app->modules, "jsx",
//                jsx_create, jsx_init, jsx_event_handler,
//                jsx_update, jsx_release);
//        modules_register(app->modules, "csx",
//                csx_create, csx_init, csx_event_handler,
//                csx_update, csx_release);
//
//        start = env_get(app->env, "start");
//        new_service(app, start);
//
//        return app;
//}
//
//void joy_destroy(app_p app)
//{
//        module_p mod;
//        context_p ctx;
//        kliter_t(kcontext)* p;
//        p = kl_begin(app->servers);
//        while (p != kl_end(app->servers)) {
//                ctx = kl_val(p);
//                if (modules_query(app->modules, ctx->modname, &mod)) {
//                        module_instance_release(mod, ctx->inst);
//                }
//                p = kl_next(p);
//        }
//        kl_destroy(kcontext, app->servers);
//        modules_destroy(app->modules);
//        env_destroy(app->env);
//        livS_mono_domain_set(app->root_domain, false);
//        livS_mono_domain_unload(app->app_domain);
//        livS_mono_jit_cleanup(app->root_domain);
//        SDL_free(app);
//        sys_release_netenv();
//}
//
//void joy_update(app_p app)
//{
//        module_p mod;
//        context_p ctx;
//        float delta_time;
//
//        Uint32 current_frame_time = SDL_GetTicks();
//        delta_time = (current_frame_time - app->last_frame_time) / 1000.0f;
//        app->last_frame_time = current_frame_time;
//
//        kliter_t(kcontext)* p;
//        p = kl_begin(app->servers);
//        while (p != kl_end(app->servers)) {
//                ctx = kl_val(p);
//                if (modules_query(app->modules, ctx->modname, &mod)) {
//                        module_instance_update(mod, ctx->inst, delta_time);
//                }
//                p = kl_next(p);
//        }
//}
//
//void joy_event(app_p app, void* event)
//{
//        module_p mod;
//        context_p ctx;
//        kliter_t(kcontext)* p;
//        p = kl_begin(app->servers);
//        while (p != kl_end(app->servers)) {
//                ctx = kl_val(p);
//                if (modules_query(app->modules, ctx->modname, &mod)) {
//                        module_instance_event(mod, ctx->inst, event);
//                }
//                p = kl_next(p);
//        }
//}
