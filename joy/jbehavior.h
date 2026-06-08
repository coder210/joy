/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jbehavior.h
Description: 行为树 (Behavior Tree) 核心
Author: ydlc
Version: 1.0
Date: 2021.3.18
History:
*************************************************/
#ifndef JBEHAVIOR_H
#define JBEHAVIOR_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===== 行为树状态 ===== */
typedef enum {
    BT_SUCCESS,
    BT_FAILURE,
    BT_RUNNING,
} bt_status_t;

/* ===== 前向声明 ===== */
typedef struct bt_node bt_node_t;

/* ===== 用户回调函数类型 ===== */
typedef bt_status_t (*bt_action_fn)(float dt);
typedef bool        (*bt_condition_fn)(void);

/* ===== 节点生命周期回调 ===== */
typedef void     (*bt_enter_fn)(bt_node_t* self);
typedef bt_status_t (*bt_tick_fn)(bt_node_t* self, float dt);
typedef void     (*bt_exit_fn)(bt_node_t* self);
typedef void     (*bt_destroy_fn)(bt_node_t* self);

/* ===== 基础节点结构 ===== */
struct bt_node {
    bt_status_t   status;      /* 当前运行状态 */
    bt_enter_fn   enter;       /* 进入回调 */
    bt_tick_fn    tick;        /* 更新回调 */
    bt_exit_fn    exit;        /* 退出回调 */
    bt_destroy_fn destroy;     /* 销毁回调 */
};

/* ===== 运行与生命周期管理 ===== */
bt_status_t bt_node_run(bt_node_t* node, float dt);
void        bt_node_reset(bt_node_t* node);
void        bt_node_destroy(bt_node_t* node);

/* ===== 叶子节点 ===== */

/* Action: 执行动作，返回 SUCCESS / FAILURE / RUNNING */
bt_node_t* bt_action_create(bt_action_fn fn);
bt_node_t* bt_action_createv(bt_action_fn fn, void* userdata);
void       bt_action_set_userdata(bt_node_t* node, void* userdata);
void*      bt_action_get_userdata(bt_node_t* node);

/* Condition: 条件判断，true → SUCCESS, false → FAILURE */
bt_node_t* bt_condition_create(bt_condition_fn pred);

/* Wait: 等待指定秒数 */
bt_node_t* bt_wait_create(float seconds);

/* ===== 组合节点 ===== */

/* Sequence: 顺序执行全部子节点，全部成功才返回 SUCCESS */
bt_node_t* bt_sequence_create(bt_node_t** children, int count);

/* Selector: 顺序尝试子节点，任一成功即返回 SUCCESS */
bt_node_t* bt_selector_create(bt_node_t** children, int count);

/* Parallel: 并行执行，success_count 个成功即成功 */
bt_node_t* bt_parallel_create(bt_node_t** children, int count, int success_count);

/* ===== 装饰节点 ===== */

/* Inverter: 反转子节点结果 */
bt_node_t* bt_inverter_create(bt_node_t* child);

/* Succeeder: 总是返回 SUCCESS */
bt_node_t* bt_succeeder_create(bt_node_t* child);

/* Repeater: 重复子节点 times 次（-1 无限） */
bt_node_t* bt_repeater_create(bt_node_t* child, int times);

/* ===== 内嵌状态机节点 ===== */
typedef struct bt_state_slot {
    const char*  name;                    /* 状态名称（不可为 NULL） */
    bt_enter_fn  state_enter;             /* 状态 enter（可为 NULL） */
    const char* (*state_update)(bt_node_t* node, float dt); /* 返回下一状态名，NULL 表示不切换 */
    bt_exit_fn   state_exit;              /* 状态 exit（可为 NULL） */
} bt_state_slot_t;

bt_node_t* bt_statemachine_create(bt_state_slot_t* states, int count, const char* initial);
void       bt_statemachine_transition(bt_node_t* node, const char* state_name);

#ifdef __cplusplus
}
#endif

#endif /* JBEHAVIOR_H */
