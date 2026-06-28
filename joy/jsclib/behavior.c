#include "../joy.h"
#include "jsclib.h"

/* ===== 全局 Duktape 上下文 + JS 回调存储 ===== */
static duk_context* G = NULL;
#define CB_ARR_KEY "__bt_cbs"

static void ensure_cb_arr(duk_context* ctx) {
    if (G) return;
    G = ctx;
    duk_push_global_object(ctx);
    duk_push_array(ctx);
    duk_put_prop_string(ctx, -2, CB_ARR_KEY);
    duk_pop(ctx);
}

/* ===== C 回调包装器（bt_action_fn / bt_condition_fn 签名） ===== */

/* Action 回调：从 userdata 取索引，调 JS 函数，解析返回值 */
static bt_status_t js_action_cb(float dt) {
    /* 此回调没有 node 参数，需要通过全局方式获取函数引用 */
    /* 实现复杂，暂用存储索引的方式 */
    return BT_SUCCESS;
}

/* 改用 bt_tick_fn 签名，通过 node->userdata 取索引 */
static bt_status_t js_action_tick(bt_node_t* node, float dt) {
    if (!G) return BT_SUCCESS;
    int idx = (int)(intptr_t)bt_action_get_userdata(node);
    if (idx <= 0) return BT_SUCCESS;

    duk_push_global_object(G);
    duk_get_prop_string(G, -1, CB_ARR_KEY);
    duk_get_prop_index(G, -1, (duk_uarridx_t)(idx - 1));
    duk_remove(G, -2); /* 移除全局对象 */
    duk_remove(G, -2); /* 移除数组 */

    duk_push_number(G, (duk_double_t)dt);
    if (duk_pcall(G, 1)) {
        log_error("bt.action error: %s", duk_to_string(G, -1));
        duk_pop(G);
        return BT_FAILURE;
    }
    const char* s = duk_to_string(G, -1);
    bt_status_t r = BT_SUCCESS;
    if (s) {
        if (strcmp(s, "failure") == 0) r = BT_FAILURE;
        else if (strcmp(s, "running") == 0) r = BT_RUNNING;
    }
    duk_pop(G);
    return r;
}

static bt_status_t js_cond_tick(bt_node_t* node, float dt) {
    (void)dt;
    if (!G) return BT_FAILURE;
    int idx = (int)(intptr_t)bt_action_get_userdata(node);
    if (idx <= 0) return BT_FAILURE;

    duk_push_global_object(G);
    duk_get_prop_string(G, -1, CB_ARR_KEY);
    duk_get_prop_index(G, -1, (duk_uarridx_t)(idx - 1));
    duk_remove(G, -2);
    duk_remove(G, -2);

    if (duk_pcall(G, 0)) {
        log_error("bt.condition error: %s", duk_to_string(G, -1));
        duk_pop(G);
        return BT_FAILURE;
    }
    bool ok = duk_get_boolean(G, -1);
    duk_pop(G);
    return ok ? BT_SUCCESS : BT_FAILURE;
}

/* ===== 工厂函数 ===== */

static duk_ret_t jbt_action(duk_context* ctx) {
    ensure_cb_arr(ctx);
    duk_push_global_object(ctx);
    duk_get_prop_string(ctx, -1, CB_ARR_KEY);
    int len = (int)duk_get_length(ctx, -1);
    duk_dup(ctx, 0); /* 复制 arg 0（JS 函数） */
    duk_put_prop_index(ctx, -2, (duk_uarridx_t)len);
    int idx = len + 1;
    duk_pop(ctx);
    duk_pop(ctx);

    bt_node_t* node = bt_action_create(NULL);
    node->tick = js_action_tick;
    bt_action_set_userdata(node, (void*)(intptr_t)idx);
    duk_push_pointer(ctx, node);
    return 1;
}

static duk_ret_t jbt_condition(duk_context* ctx) {
    ensure_cb_arr(ctx);
    duk_push_global_object(ctx);
    duk_get_prop_string(ctx, -1, CB_ARR_KEY);
    int len = (int)duk_get_length(ctx, -1);
    duk_dup(ctx, 0);
    duk_put_prop_index(ctx, -2, (duk_uarridx_t)len);
    int idx = len + 1;
    duk_pop(ctx);
    duk_pop(ctx);

    bt_node_t* node = bt_action_create(NULL);
    node->tick = js_cond_tick;
    bt_action_set_userdata(node, (void*)(intptr_t)idx);
    duk_push_pointer(ctx, node);
    return 1;
}

static duk_ret_t jbt_wait(duk_context* ctx) {
    duk_push_pointer(ctx, bt_wait_create((float)duk_to_number(ctx, 0)));
    return 1;
}

static duk_ret_t jbt_sequence(duk_context* ctx) {
    int n = (int)duk_get_length(ctx, 0);
    bt_node_t** ch = (bt_node_t**)malloc(sizeof(bt_node_t*) * n);
    for (int i = 0; i < n; i++) {
        duk_get_prop_index(ctx, 0, (duk_uarridx_t)i);
        ch[i] = (bt_node_t*)duk_get_pointer(ctx, -1);
        duk_pop(ctx);
    }
    duk_push_pointer(ctx, bt_sequence_create(ch, n));
    free(ch);
    return 1;
}
static duk_ret_t jbt_selector(duk_context* ctx) {
    int n = (int)duk_get_length(ctx, 0);
    bt_node_t** ch = (bt_node_t**)malloc(sizeof(bt_node_t*) * n);
    for (int i = 0; i < n; i++) {
        duk_get_prop_index(ctx, 0, (duk_uarridx_t)i);
        ch[i] = (bt_node_t*)duk_get_pointer(ctx, -1);
        duk_pop(ctx);
    }
    duk_push_pointer(ctx, bt_selector_create(ch, n));
    free(ch);
    return 1;
}
static duk_ret_t jbt_parallel(duk_context* ctx) {
    int n = (int)duk_get_length(ctx, 0);
    int sc = duk_is_undefined(ctx, 1) ? n : (int)duk_to_number(ctx, 1);
    bt_node_t** ch = (bt_node_t**)malloc(sizeof(bt_node_t*) * n);
    for (int i = 0; i < n; i++) {
        duk_get_prop_index(ctx, 0, (duk_uarridx_t)i);
        ch[i] = (bt_node_t*)duk_get_pointer(ctx, -1);
        duk_pop(ctx);
    }
    duk_push_pointer(ctx, bt_parallel_create(ch, n, sc));
    free(ch);
    return 1;
}
static duk_ret_t jbt_inverter(duk_context* ctx) {
    duk_push_pointer(ctx, bt_inverter_create((bt_node_t*)duk_get_pointer(ctx, 0)));
    return 1;
}
static duk_ret_t jbt_succeeder(duk_context* ctx) {
    duk_push_pointer(ctx, bt_succeeder_create((bt_node_t*)duk_get_pointer(ctx, 0)));
    return 1;
}
static duk_ret_t jbt_repeater(duk_context* ctx) {
    bt_node_t* child = (bt_node_t*)duk_get_pointer(ctx, 0);
    int times = duk_is_undefined(ctx, 1) ? -1 : (int)duk_to_number(ctx, 1);
    duk_push_pointer(ctx, bt_repeater_create(child, times));
    return 1;
}
static duk_ret_t jbt_run(duk_context* ctx) {
    bt_node_t* node = (bt_node_t*)duk_get_pointer(ctx, 0);
    float dt = (float)duk_to_number(ctx, 1);
    const char* names[] = { "success", "failure", "running" };
    duk_push_string(ctx, names[bt_node_run(node, dt)]);
    return 1;
}
static duk_ret_t jbt_reset(duk_context* ctx) {
    bt_node_reset((bt_node_t*)duk_get_pointer(ctx, 0));
    return 0;
}

void jsopen_behavior(duk_context* ctx) {
    duk_push_object(ctx);
    #define REG(n,a) duk_push_c_function(ctx,jbt_##n,a); duk_put_prop_string(ctx,-2,#n)
    REG(action,1); REG(condition,1); REG(wait,1);
    REG(sequence,1); REG(selector,1); REG(parallel,2);
    REG(inverter,1); REG(succeeder,1); REG(repeater,2);
    REG(run,2); REG(reset,1);
    #undef REG
    duk_put_global_string(ctx, "behavior");
}
