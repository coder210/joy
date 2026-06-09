#include "luaclib.h"
#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../jscene.h"
#include "../jcore.h"
#include <stdlib.h>
#include <string.h>

#define SCENE_MT    "scene_t*"
#define NODE_MT     "scene_node_t*"
#define MANAGER_MT  "scene_manager_t*"

/* ===== 回调存储 ===== */
typedef struct {
    lua_State* L;
    int load_ref, update_ref, render_ref, destroy_ref, event_ref;
} scene_cbs_t;

typedef struct {
    lua_State* L;
    int update_ref, render_ref, destroy_ref, event_ref;
} node_cbs_t;

static lua_State* G = NULL;
static void init_g(lua_State* L) { if (!G) G = L; }

/* ===== 类型检查 ===== */
static scene_p* check_s(lua_State* L, int i) { return (scene_p*)luaL_checkudata(L, i, SCENE_MT); }
static scene_node_p* check_n(lua_State* L, int i) { return (scene_node_p*)luaL_checkudata(L, i, NODE_MT); }
static scene_manager_p* check_m(lua_State* L, int i) { return (scene_manager_p*)luaL_checkudata(L, i, MANAGER_MT); }

#define PUSH(T, MT, v) do { T* _p = (T*)lua_newuserdata(L, sizeof(T)); *_p = (T)(v); luaL_getmetatable(L, MT); lua_setmetatable(L, -2); } while(0)

/* ===== C 回调包装（通过全局 G 调 Lua） ===== */

/* Node */
static void node_upd_cb(scene_node_p n, float dt) {
    node_cbs_t* c = (node_cbs_t*)scene_node_get_userdata(n);
    if (!c || !c->update_ref) return;
    lua_rawgeti(G, LUA_REGISTRYINDEX, c->update_ref);
    if (lua_pcall(G, 0, 0, 0))
        log_error("node update: %s", lua_tostring(G, -1)), lua_pop(G, 1);
}
static void node_rend_cb(scene_node_p n, const void* arg) {
    node_cbs_t* c = (node_cbs_t*)scene_node_get_userdata(n);
    if (!c || !c->render_ref) return;
    lua_rawgeti(G, LUA_REGISTRYINDEX, c->render_ref);
    lua_pushlightuserdata(G, (void*)arg);
    if (lua_pcall(G, 1, 0, 0))
        log_error("node render: %s", lua_tostring(G, -1)), lua_pop(G, 1);
}
static void node_des_cb(scene_node_p n) {
    node_cbs_t* c = (node_cbs_t*)scene_node_get_userdata(n);
    if (!c) return;
    if (c->update_ref) luaL_unref(G, LUA_REGISTRYINDEX, c->update_ref);
    if (c->render_ref) luaL_unref(G, LUA_REGISTRYINDEX, c->render_ref);
    if (c->destroy_ref) luaL_unref(G, LUA_REGISTRYINDEX, c->destroy_ref);
    if (c->event_ref) luaL_unref(G, LUA_REGISTRYINDEX, c->event_ref);
    free(c);
}

/* Scene */
static void scene_upd_cb(scene_p s, float dt) {
    scene_cbs_t* c = (scene_cbs_t*)scene_get_userdata(s);
    if (!c || !c->update_ref) return;
    lua_rawgeti(G, LUA_REGISTRYINDEX, c->update_ref);
    lua_pushnumber(G, dt);
    if (lua_pcall(G, 1, 0, 0))
        log_error("scene update: %s", lua_tostring(G, -1)), lua_pop(G, 1);
}
static void scene_des_cb(scene_p s) {
    scene_cbs_t* c = (scene_cbs_t*)scene_get_userdata(s);
    if (!c) return;
    int refs[] = {c->load_ref, c->update_ref, c->render_ref, c->destroy_ref, c->event_ref};
    for (int i = 0; i < 5; i++) if (refs[i]) luaL_unref(G, LUA_REGISTRYINDEX, refs[i]);
    free(c);
}

/* ===== 取回调存储辅助 ===== */
static scene_cbs_t* get_scene_cbs(lua_State* L, int idx) {
    scene_p* sp = check_s(L, idx);
    if (!sp || !*sp) return NULL;
    return (scene_cbs_t*)scene_get_userdata(*sp);
}
static node_cbs_t* get_node_cbs(lua_State* L, int idx) {
    scene_node_p* np = check_n(L, idx);
    if (!np || !*np) return NULL;
    return (node_cbs_t*)scene_node_get_userdata(*np);
}

#undef SET_REF
/* ===== 设置回调 ref ===== */
static void set_ref(lua_State* L, int* ref, int src_idx) {
    if (*ref) luaL_unref(L, LUA_REGISTRYINDEX, *ref);
    lua_pushvalue(L, src_idx);
    *ref = luaL_ref(L, LUA_REGISTRYINDEX);
}

/* ===== Scene API ===== */
static int l_scene_create(lua_State* L) {
    init_g(L);
    scene_p s = scene_create(luaL_optstring(L, 1, ""));
    scene_cbs_t* c = (scene_cbs_t*)calloc(1, sizeof(scene_cbs_t)); c->L = G;
    scene_set_userdata(s, c);
    scene_set_update_callback(s, scene_upd_cb);
    scene_set_destroy_callback(s, scene_des_cb);
    PUSH(scene_p, SCENE_MT, s);
    return 1;
}
static int l_scene_destroy(lua_State* L) { scene_p* s = check_s(L,1); if(s&&*s) scene_destroy(*s); return 0; }
static int l_scene_set_update(lua_State* L) { scene_cbs_t* c = get_scene_cbs(L,1); if(c&&lua_isfunction(L,2)) set_ref(L,&c->update_ref,2); return 0; }
static int l_scene_add_root(lua_State* L) { scene_add_root_node(*check_s(L,1), *check_n(L,2)); return 0; }
static int l_scene_remove_root(lua_State* L) { scene_remove_root_node(*check_s(L,1), *check_n(L,2)); return 0; }
static int l_scene_update(lua_State* L) { scene_update(*check_s(L,1), (float)luaL_optnumber(L,2,0)); return 0; }
static int l_scene_get_name(lua_State* L) { lua_pushstring(L, scene_get_name(*check_s(L,1))); return 1; }
static int l_scene_root_count(lua_State* L) { lua_pushinteger(L, scene_get_root_count(*check_s(L,1))); return 1; }
static int l_scene_root_at(lua_State* L) {
    scene_node_p n = scene_get_root_at(*check_s(L,1), (int)luaL_checkinteger(L,2));
    if (n) PUSH(scene_node_p, NODE_MT, n); else lua_pushnil(L);
    return 1;
}
static int l_scene_get_root_count(lua_State* L) { lua_pushinteger(L, scene_get_root_count(*check_s(L,1))); return 1; }
static int l_scene_get_root_at(lua_State* L) { scene_node_p n = scene_get_root_at(*check_s(L,1), (int)luaL_checkinteger(L,2)); if(n) PUSH(scene_node_p,NODE_MT,n); else lua_pushnil(L); return 1; }

/* ===== Node API ===== */
static int l_node_create(lua_State* L) {
    init_g(L);
    scene_node_p n = scene_node_create();
    node_cbs_t* c = (node_cbs_t*)calloc(1, sizeof(node_cbs_t)); c->L = G;
    scene_node_set_userdata(n, c);
    scene_node_set_update_callback(n, node_upd_cb);
    scene_node_set_render_callback(n, node_rend_cb);
    scene_node_set_destroy_callback(n, node_des_cb);
    PUSH(scene_node_p, NODE_MT, n);
    return 1;
}
static int l_node_destroy(lua_State* L) { scene_node_p* n = check_n(L,1); if(n&&*n) scene_node_destroy(*n); return 0; }
static int l_node_add_child(lua_State* L) { scene_node_add_child(*check_n(L,1), *check_n(L,2)); return 0; }
static int l_node_remove_child(lua_State* L) { scene_node_remove_child(*check_n(L,1), *check_n(L,2)); return 0; }
static int l_node_set_position(lua_State* L) { scene_node_set_position(*check_n(L,1), (float)luaL_checknumber(L,2), (float)luaL_checknumber(L,3)); return 0; }
static int l_node_get_position(lua_State* L) { float x,y; scene_node_get_position(*check_n(L,1),&x,&y); lua_pushnumber(L,x); lua_pushnumber(L,y); return 2; }
static int l_node_get_world(lua_State* L) { float x,y; scene_node_get_world_position(*check_n(L,1),&x,&y); lua_pushnumber(L,x); lua_pushnumber(L,y); return 2; }
static int l_node_set_rotation(lua_State* L) { scene_node_set_rotation(*check_n(L,1), (float)luaL_checknumber(L,2)); return 0; }
static int l_node_set_scale(lua_State* L) { scene_node_set_scale(*check_n(L,1), (float)luaL_checknumber(L,2), (float)luaL_checknumber(L,3)); return 0; }
static int l_node_set_zorder(lua_State* L) { scene_node_set_zorder(*check_n(L,1), (int)luaL_checkinteger(L,2)); return 0; }
static int l_node_get_parent(lua_State* L) { scene_node_p p = scene_node_get_parent(*check_n(L,1)); if(p) PUSH(scene_node_p,NODE_MT,p); else lua_pushnil(L); return 1; }
static int l_node_get_child(lua_State* L) { scene_node_p c = scene_node_get_first_child(*check_n(L,1)); if(c) PUSH(scene_node_p,NODE_MT,c); else lua_pushnil(L); return 1; }
static int l_node_get_sibling(lua_State* L) { scene_node_p s = scene_node_get_next_sibling(*check_n(L,1)); if(s) PUSH(scene_node_p,NODE_MT,s); else lua_pushnil(L); return 1; }
static int l_node_count(lua_State* L) { lua_pushinteger(L, scene_node_get_child_count(*check_n(L,1))); return 1; }
static int l_node_set_render(lua_State* L) { node_cbs_t* c = get_node_cbs(L,1); if(c&&lua_isfunction(L,2)) set_ref(L,&c->render_ref,2); return 0; }
static int l_node_set_update(lua_State* L) { node_cbs_t* c = get_node_cbs(L,1); if(c&&lua_isfunction(L,2)) set_ref(L,&c->update_ref,2); return 0; }

/* ===== Node name / tag ===== */
static int l_node_set_name(lua_State* L) {
    scene_node_set_name(*check_n(L,1), luaL_checkstring(L,2)); return 0;
}
static int l_node_get_name(lua_State* L) {
    lua_pushstring(L, scene_node_get_name(*check_n(L,1))); return 1;
}
static int l_node_set_tag(lua_State* L) {
    scene_node_set_tag(*check_n(L,1), luaL_checkstring(L,2)); return 0;
}
static int l_node_get_tag(lua_State* L) {
    lua_pushstring(L, scene_node_get_tag(*check_n(L,1))); return 1;
}

/* ===== 递归查找 ===== */
static int l_find_node_by_name(lua_State* L) {
    init_g(L);
    scene_node_p n = scene_find_node_by_name_deep(*check_s(L,1), luaL_checkstring(L,2));
    if (n) PUSH(scene_node_p, NODE_MT, n); else lua_pushnil(L);
    return 1;
}
static int l_find_nodes_by_tag(lua_State* L) {
    scene_p s = *check_s(L,1);
    const char* tag = luaL_checkstring(L,2);
    /* 先数数量 */
    scene_node_p buf[128];
    int n = scene_find_nodes_by_tag_deep(s, tag, buf, 128);
    lua_createtable(L, n, 0);
    for (int i = 0; i < n; i++) {
        PUSH(scene_node_p, NODE_MT, buf[i]);
        lua_seti(L, -2, i + 1);
    }
    return 1;
}
/* manager 级快捷查找：在当前场景中查找 */
static int l_mgr_find_node(lua_State* L) {
    scene_p s = scene_manager_get_current(*check_m(L,1));
    if (!s) { lua_pushnil(L); return 1; }
    scene_node_p n = scene_find_node_by_name_deep(s, luaL_checkstring(L,2));
    if (n) {
        /* 返回节点信息的 table，避免创建独立 userdata */
        lua_createtable(L, 0, 4);
        lua_pushstring(L, scene_node_get_name(n));  lua_setfield(L, -2, "name");
        lua_pushstring(L, scene_node_get_tag(n));   lua_setfield(L, -2, "tag");
        float x, y; scene_node_get_position(n, &x, &y);
        lua_pushnumber(L, x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, y); lua_setfield(L, -2, "y");
        lua_pushinteger(L, scene_node_get_child_count(n)); lua_setfield(L, -2, "children");
    } else {
        lua_pushnil(L);
    }
    return 1;
}
static int l_mgr_find_nodes(lua_State* L) {
    scene_p s = scene_manager_get_current(*check_m(L,1));
    if (!s) { lua_createtable(L,0,0); return 1; }
    const char* tag = luaL_checkstring(L,2);
    scene_node_p buf[128];
    int n = scene_find_nodes_by_tag_deep(s, tag, buf, 128);
    lua_createtable(L, n, 0);
    for (int i = 0; i < n; i++) {
        PUSH(scene_node_p, NODE_MT, buf[i]);
        lua_seti(L, -2, i + 1);
    }
    return 1;
}

/* ===== Manager API ===== */
static int l_mgr_create(lua_State* L) { PUSH(scene_manager_p, MANAGER_MT, scene_manager_create()); return 1; }
static int l_mgr_destroy(lua_State* L) { scene_manager_destroy(*check_m(L,1)); return 0; }
static int l_mgr_push(lua_State* L) { scene_manager_push(*check_m(L,1), *check_s(L,2)); return 0; }
static int l_mgr_pop(lua_State* L) { scene_manager_pop(*check_m(L,1)); return 0; }
static int l_mgr_replace(lua_State* L) { scene_manager_replace(*check_m(L,1), *check_s(L,2)); return 0; }
static int l_mgr_update(lua_State* L) { scene_manager_update(*check_m(L,1), (float)luaL_optnumber(L,2,0)); return 0; }
static int l_mgr_render(lua_State* L) { scene_manager_render(*check_m(L,1), lua_touserdata(L,2)); return 0; }
static int l_mgr_current(lua_State* L) { scene_p s = scene_manager_get_current(*check_m(L,1)); if(s) PUSH(scene_p,SCENE_MT,s); else lua_pushnil(L); return 1; }

/* ===== __gc ===== */
static int l_scene_gc(lua_State* L) { scene_p* s = check_s(L,1); if(s&&*s) scene_destroy(*s); return 0; }
static int l_node_gc(lua_State* L) { scene_node_p* n = check_n(L,1); if(n&&*n) scene_node_destroy(*n); return 0; }
static int l_mgr_gc(lua_State* L) { scene_manager_destroy(*check_m(L,1)); return 0; }

/* ===== 注册 ===== */
int luaopen_scene(lua_State* L) {
    luaL_checkversion(L);

    /* 场景元表 */
    luaL_newmetatable(L, SCENE_MT);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
    /* 不注册 __gc — manager_replace/pop 负责销毁，避免双重释放 */
    lua_pop(L, 1);

    luaL_newmetatable(L, NODE_MT);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
    /* 不注册 __gc — scene_destroy 会递归清理整棵树，避免双重释放 */
    lua_pop(L, 1);

    luaL_newmetatable(L, MANAGER_MT);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_mgr_gc); lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);

    luaL_Reg lib[] = {
        {"create",       l_scene_create},
        {"destroy",      l_scene_destroy},
        {"set_update",   l_scene_set_update},
        {"add_root",     l_scene_add_root},
        {"remove_root",  l_scene_remove_root},
        {"update",       l_scene_update},
        {"get_name",     l_scene_get_name},
        {"root_count",   l_scene_root_count},
        {"root_at",      l_scene_root_at},
        {"get_root_count", l_scene_get_root_count},
        {"get_root_at",    l_scene_get_root_at},
        {"node_create",       l_node_create},
        {"node_destroy",      l_node_destroy},
        {"node_add_child",    l_node_add_child},
        {"node_remove_child", l_node_remove_child},
        {"node_set_position",    l_node_set_position},
        {"node_get_position",    l_node_get_position},
        {"node_get_world",       l_node_get_world},
        {"node_set_rotation",    l_node_set_rotation},
        {"node_set_scale",       l_node_set_scale},
        {"node_set_zorder",      l_node_set_zorder},
        {"node_get_parent",     l_node_get_parent},
        {"node_get_child",      l_node_get_child},
        {"node_get_sibling",    l_node_get_sibling},
        {"node_get_count",      l_node_count},
        {"node_set_render",     l_node_set_render},
        {"node_set_update",     l_node_set_update},
        {"node_set_name",       l_node_set_name},
        {"node_get_name",       l_node_get_name},
        {"node_set_tag",        l_node_set_tag},
        {"node_get_tag",        l_node_get_tag},
        {"find_node_by_name",   l_find_node_by_name},
        {"find_nodes_by_tag",   l_find_nodes_by_tag},
        {"mgr_create",   l_mgr_create},
        {"mgr_destroy",  l_mgr_destroy},
        {"mgr_push",     l_mgr_push},
        {"mgr_pop",      l_mgr_pop},
        {"mgr_replace",  l_mgr_replace},
        {"mgr_update",   l_mgr_update},
        {"mgr_render",   l_mgr_render},
        {"mgr_current",  l_mgr_current},
        {"mgr_find_node",  l_mgr_find_node},
        {"mgr_find_nodes", l_mgr_find_nodes},
        {NULL, NULL}
    };
    luaL_newlib(L, lib);
    return 1;
}
