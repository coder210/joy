#include "luaclib.h"
#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../jbehavior.h"
#include "../jcore.h"
#include <stdlib.h>

#define BT_MT    "behavior_tree*"
#define MAX_BT_CHILDREN 32

/* ===== userdata 扩展结构 ===== */
typedef struct {
    bt_node_t* node;
    int   child_count;
    int   child_refs[MAX_BT_CHILDREN];
} bt_ud_t;

/* ===== 全局 Lua 状态引用 ===== */
static lua_State* G = NULL;

static void init_globals(lua_State* L) { if (!G) G = L; }

/* ===== Action 回调包装器 ===== */
static bt_status_t action_cb(bt_node_t* node, float dt) {
    lua_State* L = G;
    int ref = (int)(intptr_t)bt_action_get_userdata(node);
    if (ref == 0) return BT_SUCCESS;
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    lua_pushnumber(L, dt);
    if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
        log_error("bt.action error: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
        return BT_FAILURE;
    }
    const char* s = lua_tostring(L, -1);
    bt_status_t r = BT_SUCCESS;
    if (s) {
        if (strcmp(s, "failure") == 0) r = BT_FAILURE;
        else if (strcmp(s, "running") == 0) r = BT_RUNNING;
    }
    lua_pop(L, 1);
    return r;
}

/* ===== Condition 回调包装器 ===== */
static bt_status_t condition_cb(bt_node_t* node, float dt) {
    (void)dt;
    lua_State* L = G;
    int ref = (int)(intptr_t)bt_action_get_userdata(node);
    if (ref == 0) return BT_FAILURE;
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        log_error("bt.condition error: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
        return BT_FAILURE;
    }
    bool ok = lua_toboolean(L, -1);
    lua_pop(L, 1);
    return ok ? BT_SUCCESS : BT_FAILURE;
}

/* ===== 节点操作方法 ===== */
static bt_node_t* check_node(lua_State* L, int idx) {
    bt_ud_t* ud = (bt_ud_t*)luaL_checkudata(L, idx, BT_MT);
    return ud ? ud->node : NULL;
}

static bt_ud_t* push_node(lua_State* L, bt_node_t* node) {
    bt_ud_t* ud = (bt_ud_t*)lua_newuserdata(L, sizeof(bt_ud_t));
    ud->node = node;
    ud->child_count = 0;
    luaL_getmetatable(L, BT_MT);
    lua_setmetatable(L, -2);
    return ud;
}

/* 注册子节点到 registry，防止被 GC */
static void reg_children(lua_State* L, bt_ud_t* ud, int tbl_idx) {
    ud->child_count = (int)luaL_len(L, tbl_idx);
    if (ud->child_count > MAX_BT_CHILDREN) ud->child_count = MAX_BT_CHILDREN;
    for (int i = 0; i < ud->child_count; i++) {
        lua_geti(L, tbl_idx, i + 1);
        ud->child_refs[i] = luaL_ref(L, LUA_REGISTRYINDEX);
    }
}

/* ===== __gc ===== */
static int l_node_gc(lua_State* L) {
    bt_ud_t* ud = (bt_ud_t*)luaL_checkudata(L, 1, BT_MT);
    if (ud) {
        for (int i = 0; i < ud->child_count; i++)
            luaL_unref(L, LUA_REGISTRYINDEX, ud->child_refs[i]);
        if (ud->node) bt_node_destroy(ud->node);
    }
    return 0;
}

/* ===== run / reset ===== */
static int l_node_run(lua_State* L) {
    bt_node_t* node = check_node(L, 1);
    float dt = (float)luaL_optnumber(L, 2, 0);
    bt_status_t s = bt_node_run(node, dt);
    const char* names[] = { "success", "failure", "running" };
    lua_pushstring(L, names[s]);
    return 1;
}

static int l_node_reset(lua_State* L) {
    bt_node_t* node = check_node(L, 1);
    bt_node_reset(node);
    return 0;
}

/* ===== 工厂函数 ===== */

static int l_action(lua_State* L) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    init_globals(L);
    lua_pushvalue(L, 1);
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    bt_node_t* node = bt_action_create(NULL);
    node->tick = action_cb;
    bt_action_set_userdata(node, (void*)(intptr_t)ref);
    push_node(L, node);
    return 1;
}

static int l_condition(lua_State* L) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    init_globals(L);
    lua_pushvalue(L, 1);
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    bt_node_t* node = bt_action_create(NULL);
    node->tick = condition_cb;
    bt_action_set_userdata(node, (void*)(intptr_t)ref);
    push_node(L, node);
    return 1;
}

static int l_wait(lua_State* L) {
    float sec = (float)luaL_checknumber(L, 1);
    push_node(L, bt_wait_create(sec));
    return 1;
}

static int read_children(lua_State* L, int idx, bt_node_t*** out) {
    int n = (int)luaL_len(L, idx);
    *out = (bt_node_t**)malloc(sizeof(bt_node_t*) * n);
    for (int i = 0; i < n; i++) {
        lua_geti(L, idx, i + 1);
        (*out)[i] = check_node(L, -1);
        if (!(*out)[i]) luaL_error(L, "bt: child %d is not a valid node", i);
        lua_pop(L, 1);
    }
    return n;
}

/* 创建组合节点：读取子节点 + 注册 ref 防止 GC */
#define COMBO_FN(name, cfunc) \
static int l_##name(lua_State* L) { \
    bt_node_t** ch; int n = read_children(L, 1, &ch); \
    bt_ud_t* ud = push_node(L, cfunc(ch, n)); free(ch); \
    reg_children(L, ud, 1); return 1; \
}
COMBO_FN(sequence, bt_sequence_create)
COMBO_FN(selector, bt_selector_create)

static int l_parallel(lua_State* L) {
    bt_node_t** ch; int n = read_children(L, 1, &ch);
    int sc = (int)luaL_optinteger(L, 2, n);
    bt_ud_t* ud = push_node(L, bt_parallel_create(ch, n, sc)); free(ch);
    reg_children(L, ud, 1); return 1;
}

/* 单子装饰节点 */
#define DECO_FN(name, cfunc) \
static int l_##name(lua_State* L) { \
    bt_node_t* child = check_node(L, 1); \
    bt_ud_t* ud = push_node(L, cfunc(child)); \
    lua_pushvalue(L, 1); ud->child_refs[0] = luaL_ref(L, LUA_REGISTRYINDEX); \
    ud->child_count = 1; return 1; \
}
DECO_FN(inverter,  bt_inverter_create)
DECO_FN(succeeder, bt_succeeder_create)

static int l_repeater(lua_State* L) {
    bt_node_t* child = check_node(L, 1);
    int times = (int)luaL_optinteger(L, 2, -1);
    bt_ud_t* ud = push_node(L, bt_repeater_create(child, times));
    lua_pushvalue(L, 1);
    ud->child_refs[0] = luaL_ref(L, LUA_REGISTRYINDEX);
    ud->child_count = 1;
    return 1;
}

static int l_statemachine(lua_State* L) {
    luaL_error(L, "statemachine Lua binding not yet implemented");
    return 0;
}

/* ===== 注册 ===== */

int luaopen_behavior(lua_State* L) {
    luaL_checkversion(L);

    luaL_Reg l[] = {
        {"action",       l_action},
        {"condition",    l_condition},
        {"wait",         l_wait},
        {"sequence",     l_sequence},
        {"selector",     l_selector},
        {"parallel",     l_parallel},
        {"inverter",     l_inverter},
        {"succeeder",    l_succeeder},
        {"repeater",     l_repeater},
        {"statemachine", l_statemachine},
        {NULL, NULL}
    };
    luaL_newlib(L, l);

    /* node 元表 */
    luaL_newmetatable(L, BT_MT);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");             /* metatable.__index = metatable */
    lua_pushcfunction(L, l_node_run);
    lua_setfield(L, -2, "run");
    lua_pushcfunction(L, l_node_reset);
    lua_setfield(L, -2, "reset");
    lua_pushcfunction(L, l_node_gc);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);

    return 1;
}
