#include "jsclib.h"
#include "../jscene.h"
#include <stdlib.h>
#include <string.h>

/* JS 绑定简化版：暴露核心 API 但不支持 C 回调（Duktape 对函数引用的限制）
 * 用户直接在 joy.update / joy.draw 中调用 scene.update / scene.render
 * Node 的 update/render 回调暂不支持（待扩展）
 */

#define PTR(n)   duk_get_pointer(ctx, n)
#define NUM(n)   ((float)duk_to_number(ctx, n))
#define INT(n)   ((int)duk_to_number(ctx, n))
#define STR(n)   duk_to_string(ctx, n)

/* ===== Scene ===== */
static duk_ret_t jscene_create(duk_context* ctx) {
    duk_push_pointer(ctx, scene_create(STR(0)));
    return 1;
}
static duk_ret_t jscene_destroy(duk_context* ctx) {
    scene_p s = (scene_p)PTR(0);
    if (s) scene_destroy(s);
    return 0;
}
static duk_ret_t jscene_add_root(duk_context* ctx) {
    scene_add_root_node((scene_p)PTR(0), (scene_node_p)PTR(1));
    return 0;
}
static duk_ret_t jscene_remove_root(duk_context* ctx) {
    scene_remove_root_node((scene_p)PTR(0), (scene_node_p)PTR(1));
    return 0;
}
static duk_ret_t jscene_update(duk_context* ctx) {
    scene_update((scene_p)PTR(0), NUM(1));
    return 0;
}
static duk_ret_t jscene_get_name(duk_context* ctx) {
    duk_push_string(ctx, scene_get_name((scene_p)PTR(0)));
    return 1;
}

/* ===== Node ===== */
static duk_ret_t jnode_create(duk_context* ctx) {
    duk_push_pointer(ctx, scene_node_create());
    return 1;
}
static duk_ret_t jnode_destroy(duk_context* ctx) {
    scene_node_p n = (scene_node_p)PTR(0);
    if (n) scene_node_destroy(n);
    return 0;
}
static duk_ret_t jnode_add_child(duk_context* ctx) {
    scene_node_add_child((scene_node_p)PTR(0), (scene_node_p)PTR(1));
    return 0;
}
static duk_ret_t jnode_set_position(duk_context* ctx) {
    scene_node_set_position((scene_node_p)PTR(0), NUM(1), NUM(2));
    return 0;
}
static duk_ret_t jnode_get_position(duk_context* ctx) {
    float x, y;
    scene_node_get_position((scene_node_p)PTR(0), &x, &y);
    duk_push_number(ctx, (duk_double_t)x);
    duk_push_number(ctx, (duk_double_t)y);
    return 2;  // Duktape 允许 >1 ? 查文档，安全起见返回对象
}
/* Duktape 2.x 只允 return 0/1，改用对象 */
static duk_ret_t jnode_get_position_v2(duk_context* ctx) {
    float x, y;
    scene_node_get_position((scene_node_p)PTR(0), &x, &y);
    duk_push_object(ctx);
    duk_push_number(ctx, (duk_double_t)x); duk_put_prop_string(ctx, -2, "x");
    duk_push_number(ctx, (duk_double_t)y); duk_put_prop_string(ctx, -2, "y");
    return 1;
}
static duk_ret_t jnode_get_world(duk_context* ctx) {
    float x, y;
    scene_node_get_world_position((scene_node_p)PTR(0), &x, &y);
    duk_push_object(ctx);
    duk_push_number(ctx, (duk_double_t)x); duk_put_prop_string(ctx, -2, "x");
    duk_push_number(ctx, (duk_double_t)y); duk_put_prop_string(ctx, -2, "y");
    return 1;
}
static duk_ret_t jnode_set_rotation(duk_context* ctx) {
    scene_node_set_rotation((scene_node_p)PTR(0), NUM(1));
    return 0;
}
static duk_ret_t jnode_set_scale(duk_context* ctx) {
    scene_node_set_scale((scene_node_p)PTR(0), NUM(1), NUM(2));
    return 0;
}
static duk_ret_t jnode_set_zorder(duk_context* ctx) {
    scene_node_set_zorder((scene_node_p)PTR(0), INT(1));
    return 0;
}
static duk_ret_t jnode_get_count(duk_context* ctx) {
    duk_push_number(ctx, (duk_double_t)scene_node_get_child_count((scene_node_p)PTR(0)));
    return 1;
}

/* ===== Manager ===== */
static duk_ret_t jmgr_create(duk_context* ctx) {
    duk_push_pointer(ctx, scene_manager_create());
    return 1;
}
static duk_ret_t jmgr_destroy(duk_context* ctx) {
    scene_manager_destroy((scene_manager_p)PTR(0));
    return 0;
}
static duk_ret_t jmgr_push(duk_context* ctx) {
    scene_manager_push((scene_manager_p)PTR(0), (scene_p)PTR(1));
    return 0;
}
static duk_ret_t jmgr_pop(duk_context* ctx) {
    scene_manager_pop((scene_manager_p)PTR(0));
    return 0;
}
static duk_ret_t jmgr_replace(duk_context* ctx) {
    scene_manager_replace((scene_manager_p)PTR(0), (scene_p)PTR(1));
    return 0;
}
static duk_ret_t jmgr_update(duk_context* ctx) {
    scene_manager_update((scene_manager_p)PTR(0), NUM(1));
    return 0;
}
static duk_ret_t jmgr_current(duk_context* ctx) {
    scene_p s = scene_manager_get_current((scene_manager_p)PTR(0));
    if (s) duk_push_pointer(ctx, s); else duk_push_null(ctx);
    return 1;
}

void jsopen_scene(duk_context* ctx) {
    duk_push_object(ctx);
    #define REG(n,a) duk_push_c_function(ctx, j##n, a); duk_put_prop_string(ctx, -2, #n)
    /* Scene */
    REG(scene_create, 1);      REG(scene_destroy, 1);
    REG(scene_add_root, 2);    REG(scene_remove_root, 2);
    REG(scene_update, 2);      REG(scene_get_name, 1);
    /* Node */
    REG(node_create, 0);       REG(node_destroy, 1);
    REG(node_add_child, 2);    REG(node_set_position, 3);
    REG(node_get_position_v2, 1); REG(node_get_world, 1);
    REG(node_set_rotation, 2); REG(node_set_scale, 3);
    REG(node_set_zorder, 2);   REG(node_get_count, 1);
    /* Manager */
    REG(mgr_create, 0);        REG(mgr_destroy, 1);
    REG(mgr_push, 2);          REG(mgr_pop, 1);
    REG(mgr_replace, 2);       REG(mgr_update, 2);
    REG(mgr_current, 1);
    #undef REG
    duk_put_global_string(ctx, "scene");
}
